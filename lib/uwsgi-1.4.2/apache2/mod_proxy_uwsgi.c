#define APR_WANT_MEMFUNC
#define APR_WANT_STRFUNC
#include "apr_strings.h"
#include "apr_hooks.h"
#include "apr_optional_hooks.h"
#include "apr_buckets.h"

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"
#include "util_script.h"

#include "mod_proxy.h"


#define UWSGI_SCHEME "uwsgi"
#define UWSGI_DEFAULT_PORT 3031

module AP_MODULE_DECLARE_DATA proxy_uwsgi_module;


static int uwsgi_canon(request_rec *r, char *url)
{
    char *host, sport[sizeof(":65535")];
    const char *err, *path;
    apr_port_t port = UWSGI_DEFAULT_PORT;

    if (strncasecmp(url, UWSGI_SCHEME "://", sizeof(UWSGI_SCHEME) + 2)) {
        return DECLINED;
    }
    url += sizeof(UWSGI_SCHEME); /* Keep slashes */

    err = ap_proxy_canon_netloc(r->pool, &url, NULL, NULL, &host, &port);
    if (err) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                      "error parsing URL %s: %s", url, err);
        return HTTP_BAD_REQUEST;
    }

    apr_snprintf(sport, sizeof(sport), ":%u", port);

    if (ap_strchr(host, ':')) { /* if literal IPv6 address */
        host = apr_pstrcat(r->pool, "[", host, "]", NULL);
    }

    path = ap_proxy_canonenc(r->pool, url, strlen(url), enc_path, 0,
                             r->proxyreq);
    if (!path) {
        return HTTP_BAD_REQUEST;
    }

    r->filename = apr_pstrcat(r->pool, "proxy:" UWSGI_SCHEME "://", host, sport, "/",
                              path, NULL);

    return OK;
}


static int uwsgi_send(proxy_conn_rec *conn, const char *buf, apr_size_t length,
                   request_rec *r)
{
    apr_status_t rv;
    apr_size_t written;

    while (length > 0) {
        written = length;
        if ((rv = apr_socket_send(conn->sock, buf, &written)) != APR_SUCCESS) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r,
                          "sending data to %s:%u failed",
                          conn->hostname, conn->port);
            return HTTP_SERVICE_UNAVAILABLE;
        }

        /* count for stats */
        conn->worker->s->transferred += written;
        buf += written;
        length -= written;
    }

    return OK;
}


/*
 * Send uwsgi header block
 */
static int uwsgi_send_headers(request_rec *r, proxy_conn_rec *conn)
{
    char *buf, *ptr;

    const apr_array_header_t *env_table;
    const apr_table_entry_t *env;

    int j;

    apr_size_t headerlen = 4;
    uint16_t pktsize, keylen, vallen;

    ap_add_common_vars(r);
    ap_add_cgi_vars(r);

    const char *script_name = apr_table_get(r->subprocess_env, "SCRIPT_NAME");
    const char *path_info = apr_table_get(r->subprocess_env, "PATH_INFO");
    if (script_name && path_info) {
        apr_table_set(r->subprocess_env, "SCRIPT_NAME", apr_pstrndup(r->pool, script_name, strlen(script_name)-strlen(path_info)));
    }

    env_table = apr_table_elts(r->subprocess_env);
    env = (apr_table_entry_t *)env_table->elts;

    for (j = 0; j < env_table->nelts; ++j) {
        headerlen += 2 + strlen(env[j].key) + 2 + strlen(env[j].val) ;
    }

    ptr = buf = apr_palloc(r->pool, headerlen);

    ptr+=4;

    for (j = 0; j < env_table->nelts; ++j) {
	keylen = strlen(env[j].key);
	*ptr++= (uint8_t) (keylen & 0xff);
	*ptr++= (uint8_t) ((keylen >> 8)  & 0xff);
	memcpy(ptr, env[j].key, keylen) ; ptr+=keylen;

	vallen = strlen(env[j].val);
	*ptr++= (uint8_t) (vallen & 0xff);
	*ptr++= (uint8_t) ((vallen >> 8)  & 0xff);
	memcpy(ptr, env[j].val, vallen) ; ptr+=vallen;
    }

    pktsize = headerlen-4;

    buf[0] = 0;
    buf[1] = (uint8_t) (pktsize & 0xff);
    buf[2] = (uint8_t) ((pktsize >> 8) & 0xff);
    buf[0] = 0;

    return uwsgi_send(conn, buf, headerlen, r);
}


static int uwsgi_send_body(request_rec *r, proxy_conn_rec *conn)
{
    if (ap_should_client_block(r)) {
        char *buf = apr_palloc(r->pool, AP_IOBUFSIZE);
        int status;
        apr_size_t readlen;

        readlen = ap_get_client_block(r, buf, AP_IOBUFSIZE);
        while (readlen > 0) {
            status = uwsgi_send(conn, buf, readlen, r);
            if (status != OK) {
                return HTTP_SERVICE_UNAVAILABLE;
            }
            readlen = ap_get_client_block(r, buf, AP_IOBUFSIZE);
        }
        if (readlen == -1) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                          "receiving request body failed");
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    return OK;
}

static
apr_status_t ap_proxygetline(apr_bucket_brigade *bb, char *s, int n, request_rec *r,
                             int fold, int *writen)
{
    char *tmp_s = s;
    apr_status_t rv;
    apr_size_t len;

    rv = ap_rgetline(&tmp_s, n, &len, r, fold, bb);
    apr_brigade_cleanup(bb);

    if (rv == APR_SUCCESS) {
        *writen = (int) len;
    } else if (rv == APR_ENOSPC) {
        *writen = n;
    } else {
        *writen = -1;
    }

    return rv;
}

static int uwsgi_response(request_rec *r, proxy_conn_rec *backend, proxy_server_conf *conf)
{

	char buffer[HUGE_STRING_LEN];
	const char *buf;
	char *value, *end;
	int len;
	apr_status_t rc;
	conn_rec *c = r->connection;
	apr_off_t readbytes;
	apr_status_t rv;
	apr_bucket *e;
	apr_read_type_e mode = APR_NONBLOCK_READ;

	request_rec *rp = ap_proxy_make_fake_req(backend->connection, r);
	rp->proxyreq = PROXYREQ_RESPONSE;

	apr_bucket_brigade *bb = apr_brigade_create(r->pool, c->bucket_alloc);


	rc = ap_proxygetline(bb, buffer, sizeof(buffer), rp, 0, &len);
	if (len == 0) {
		rc = ap_proxygetline(bb, buffer, sizeof(buffer), rp, 0, &len);
	}

	if (len <= 0) {
		// oops
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	backend->worker->s->read += len;

	if (!apr_date_checkmask(buffer, "HTTP/#.# ###*")) {
		// oops
		return HTTP_INTERNAL_SERVER_ERROR;
	}

        int major, minor;

        major = buffer[5] - '0';
        minor = buffer[7] - '0';

        /* If not an HTTP/1 message or
        * if the status line was > 8192 bytes
         */
        if ((major != 1) || (len >= sizeof(buffer)-1)) {
        	return ap_proxyerror(r, HTTP_BAD_GATEWAY,
                apr_pstrcat(r->pool, "Corrupt status line returned by remote "
                            "server: ", buffer, NULL));
        }

        char keepchar = buffer[12];
        buffer[12] = '\0';
        r->status = atoi(&buffer[9]);

        if (keepchar != '\0') {
                buffer[12] = keepchar;
        } else {
                /* 2616 requires the space in Status-Line; the origin
                 * server may have sent one but ap_rgetline_core will
                 * have stripped it. */
                buffer[12] = ' ';
                buffer[13] = '\0';
            }
        r->status_line = apr_pstrdup(r->pool, &buffer[9]);

		// start parsing headers;
		while ((len = ap_getline(buffer, sizeof(buffer), rp, 1)) > 0) {
			value = strchr(buffer, ':');
			// invalid header skip
			if (!value) continue;
			*value = '\0';
        		++value;
        		while (apr_isspace(*value)) ++value; 
        		for (end = &value[strlen(value)-1]; end > value && apr_isspace(*end); --end) *end = '\0';
			apr_table_add(r->headers_out, buffer, value);
		}


	if ((buf = apr_table_get(r->headers_out, "Content-Type"))) {
                ap_set_content_type(r, apr_pstrdup(r->pool, buf));
            }
	
	for(;;) {
		rv = ap_get_brigade(rp->input_filters, bb,
                                        AP_MODE_READBYTES, mode,
                                        conf->io_buffer_size);
		if (APR_STATUS_IS_EAGAIN(rv)
                        || (rv == APR_SUCCESS && APR_BRIGADE_EMPTY(bb))) {
			e = apr_bucket_flush_create(c->bucket_alloc);
			APR_BRIGADE_INSERT_TAIL(bb, e);
			if (ap_pass_brigade(r->output_filters, bb) || c->aborted) {
				break;
			}
			apr_brigade_cleanup(bb);
			mode = APR_BLOCK_READ;
			continue;
		}
		else if (rv == APR_EOF) {
			break;
		}
		else if (rv != APR_SUCCESS) {
			ap_proxy_backend_broke(r, bb);
			ap_pass_brigade(r->output_filters, bb);
		}

		mode = APR_NONBLOCK_READ;
		apr_brigade_length(bb, 0, &readbytes);
		backend->worker->s->read += readbytes;
		ap_pass_brigade(r->output_filters, bb);
		apr_brigade_cleanup(bb);
	}
	e = apr_bucket_eos_create(c->bucket_alloc);
	APR_BRIGADE_INSERT_TAIL(bb, e);
        ap_pass_brigade(r->output_filters, bb);

	apr_brigade_cleanup(bb);

    return OK;
}

static int uwsgi_handler(request_rec *r, proxy_worker *worker,
                        proxy_server_conf *conf, char *url,
                        const char *proxyname, apr_port_t proxyport)
{
    int status;
    proxy_conn_rec *backend = NULL;
    apr_pool_t *p = r->pool;
    apr_uri_t *uri = apr_palloc(r->pool, sizeof(*uri));
    char server_portstr[32];

    if (strncasecmp(url, UWSGI_SCHEME "://", sizeof(UWSGI_SCHEME) + 2)) {
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                      "declining URL %s", url);
        return DECLINED;
    }

    // ADD PATH_INFO
    apr_table_add(r->subprocess_env, "PATH_INFO", url+strlen(worker->name));


    /* Create space for state information */
    status = ap_proxy_acquire_connection(UWSGI_SCHEME, &backend, worker,
                                         r->server);
    if (status != OK) {
        goto cleanup;
    }
    backend->is_ssl = 0;

    /* Step One: Determine Who To Connect To */
    status = ap_proxy_determine_connection(p, r, conf, worker, backend,
                                           uri, &url, proxyname, proxyport,
                                           server_portstr, sizeof(server_portstr));
    if (status != OK) {
        goto cleanup;
    }


    /* Step Two: Make the Connection */
    if (ap_proxy_connect_backend(UWSGI_SCHEME, backend, worker, r->server)) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                      "failed to make connection to backend: %s:%u",
                      backend->hostname, backend->port);
        status = HTTP_SERVICE_UNAVAILABLE;
        goto cleanup;
    }

    /* Step Three: Create conn_rec */
    if (!backend->connection) {
	if ((status = ap_proxy_connection_create(UWSGI_SCHEME, backend,
						r->connection, r->server)) != OK)
		goto cleanup;
    }

    /* Step Four: Process the Request */
    if (   ((status = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)) != OK)
        || ((status = uwsgi_send_headers(r, backend)) != OK)
        || ((status = uwsgi_send_body(r, backend)) != OK)
        || ((status = uwsgi_response(r, backend, conf)) != OK)) {
        goto cleanup;
    }

cleanup:
    if (backend) {
        backend->close = 1; /* always close the socket */
        ap_proxy_release_connection(UWSGI_SCHEME, backend, r->server);
    }
    return status;
}


static void register_hooks(apr_pool_t *p)
{
    proxy_hook_scheme_handler(uwsgi_handler, NULL, NULL, APR_HOOK_FIRST);
    proxy_hook_canon_handler(uwsgi_canon, NULL, NULL, APR_HOOK_FIRST);
}


module AP_MODULE_DECLARE_DATA proxy_uwsgi_module = {
    STANDARD20_MODULE_STUFF,
    NULL,		/* create per-directory config structure */
    NULL,		/* merge per-directory config structures */
    NULL,		/* create per-server config structure */
    NULL,		/* merge per-server config structures */
    NULL,		/* command table */
    register_hooks	/* register hooks */
};

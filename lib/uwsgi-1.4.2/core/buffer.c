#include "uwsgi.h"

extern struct uwsgi_server uwsgi;

struct uwsgi_buffer *uwsgi_buffer_new(size_t len) {
	struct uwsgi_buffer *ub = uwsgi_calloc(sizeof(struct uwsgi_buffer));

	if (len) {
		ub->buf = uwsgi_malloc(len);
		ub->len = len;
	}
	return ub;

}

int uwsgi_buffer_fix(struct uwsgi_buffer *ub, size_t len) {
	if (ub->limit > 0 && len > ub->limit)
		return -1;
	if (ub->len < len) {
		char *new_buf = realloc(ub->buf, len);
		if (!new_buf) {
			uwsgi_error("uwsgi_buffer_fix()");
			return -1;
		}
		ub->buf = new_buf;
		ub->len = len;
	}
	return 0;
}

int uwsgi_buffer_ensure(struct uwsgi_buffer *ub, size_t len) {
	size_t remains = ub->len - ub->pos;
	if (remains < len) {
		size_t new_len = ub->len + (len - remains);
		if (ub->limit > 0 && new_len > ub->limit) {
			new_len = ub->limit;
			if (new_len == ub->len)
				return -1;
		}
		char *new_buf = realloc(ub->buf, new_len);
		if (!new_buf) {
			uwsgi_error("uwsgi_buffer_ensure()");
			return -1;
		}
		ub->buf = new_buf;
		ub->len = new_len;
	}
	return 0;
}


int uwsgi_buffer_append(struct uwsgi_buffer *ub, char *buf, size_t len) {

	size_t remains = ub->len - ub->pos;

	if (len > remains) {
		size_t chunk_size = UMAX(len, (size_t) uwsgi.page_size);
		if (ub->limit > 0 && ub->len + chunk_size > ub->limit) {
			// retry with another minimal size
			if (len < (size_t) uwsgi.page_size) {
				chunk_size = len;
			}
			if (ub->len + chunk_size > ub->limit)
				return -1;
		}
		char *new_buf = realloc(ub->buf, ub->len + chunk_size);
		if (!new_buf) {
			uwsgi_error("uwsgi_buffer_append()");
			return -1;
		}
		ub->buf = new_buf;
		ub->len += chunk_size;
	}

	memcpy(ub->buf + ub->pos, buf, len);
	ub->pos += len;
	return 0;
}

void uwsgi_buffer_destroy(struct uwsgi_buffer *ub) {
	if (ub->buf)
		free(ub->buf);
	free(ub);
}

int uwsgi_buffer_send(struct uwsgi_buffer *ub, int fd) {
	size_t remains = ub->pos;
	char *ptr = ub->buf;

	while (remains > 0) {
		int ret = uwsgi_waitfd_write(fd, uwsgi.shared->options[UWSGI_OPTION_SOCKET_TIMEOUT]);
		if (ret > 0) {
			ssize_t len = write(fd, ptr, remains);
			if (len > 0) {
				ptr += len;
				remains -= len;
			}
			else if (len == 0) {
				return -1;
			}
			else {
				uwsgi_error("uwsgi_buffer_send()");
				return -1;
			}
		}
		else if (ret == 0) {
			uwsgi_log("timeout while sending buffer !!!\n");
			return -1;
		}
		else {
			return -1;
		}
	}

	return 0;
}

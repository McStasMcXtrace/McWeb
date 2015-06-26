#include "uwsgi.h"

extern struct uwsgi_server uwsgi;

struct uwsgi_gateway *register_gateway(char *name, void (*loop) (int, void *), void *data) {

	struct uwsgi_gateway *ug;
	int num = 1, i;

	if (ushared->gateways_cnt >= MAX_GATEWAYS) {
		uwsgi_log("you can register max %d gateways\n", MAX_GATEWAYS);
		return NULL;
	}

	for (i = 0; i < ushared->gateways_cnt; i++) {
		if (!strcmp(name, ushared->gateways[i].name)) {
			num++;
		}
	}

	char *fullname = uwsgi_concat3(name, " ", uwsgi_num2str(num));

	ug = &ushared->gateways[ushared->gateways_cnt];
	ug->pid = 0;
	ug->name = name;
	ug->loop = loop;
	ug->num = num;
	ug->fullname = fullname;
	ug->data = data;

	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, ug->internal_subscription_pipe)) {
		uwsgi_error("socketpair()");
	}

	if (!uwsgi.master_process)
		gateway_respawn(ushared->gateways_cnt);

	ushared->gateways_cnt++;


	return ug;
}

void gateway_respawn(int id) {

	pid_t gw_pid;
	struct uwsgi_gateway *ug = &ushared->gateways[id];

	if (uwsgi.master_process)
		uwsgi.shared->gateways_harakiri[id] = 0;

	gw_pid = uwsgi_fork(ug->fullname);
	if (gw_pid < 0) {
		uwsgi_error("fork()");
		return;
	}

	if (gw_pid == 0) {
		uwsgi_fixup_fds(0, 0, ug);
		if (uwsgi.master_as_root)
			uwsgi_as_root();
#ifdef __linux__
		if (prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0)) {
			uwsgi_error("prctl()");
		}
#endif
		uwsgi.mypid = getpid();
		signal(SIGALRM, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
		signal(SIGINT, end_me);
		signal(SIGTERM, end_me);
		signal(SIGUSR1, SIG_IGN);
		signal(SIGUSR2, SIG_IGN);
		signal(SIGPIPE, SIG_IGN);
		signal(SIGSTOP, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);

		ug->loop(id, ug->data);
		// never here !!! (i hope)
		exit(1);
	}

	ug->pid = gw_pid;
	ug->respawns++;
	if (ug->respawns == 1) {
		uwsgi_log("spawned %s %d (pid: %d)\n", ug->name, ug->num, (int) gw_pid);
	}
	else {
		uwsgi_log("respawned %s %d (pid: %d)\n", ug->name, ug->num, (int) gw_pid);
	}

}

struct uwsgi_gateway_socket *uwsgi_new_gateway_socket(char *name, char *owner) {

	struct uwsgi_gateway_socket *uwsgi_sock = uwsgi.gateway_sockets, *old_uwsgi_sock;

	if (!uwsgi_sock) {
		uwsgi.gateway_sockets = uwsgi_malloc(sizeof(struct uwsgi_gateway_socket));
		uwsgi_sock = uwsgi.gateway_sockets;
	}
	else {
		while (uwsgi_sock) {
			old_uwsgi_sock = uwsgi_sock;
			uwsgi_sock = uwsgi_sock->next;
		}

		uwsgi_sock = uwsgi_malloc(sizeof(struct uwsgi_gateway_socket));
		old_uwsgi_sock->next = uwsgi_sock;
	}

	memset(uwsgi_sock, 0, sizeof(struct uwsgi_gateway_socket));
	uwsgi_sock->fd = -1;
	uwsgi_sock->shared = 0;
	uwsgi_sock->name = name;
	uwsgi_sock->name_len = strlen(name);
	uwsgi_sock->owner = owner;

	return uwsgi_sock;
}

struct uwsgi_gateway_socket *uwsgi_new_gateway_socket_from_fd(int fd, char *owner) {

	struct uwsgi_gateway_socket *uwsgi_sock = uwsgi.gateway_sockets, *old_uwsgi_sock;

	if (!uwsgi_sock) {
		uwsgi.gateway_sockets = uwsgi_malloc(sizeof(struct uwsgi_gateway_socket));
		uwsgi_sock = uwsgi.gateway_sockets;
	}
	else {
		while (uwsgi_sock) {
			old_uwsgi_sock = uwsgi_sock;
			uwsgi_sock = uwsgi_sock->next;
		}

		uwsgi_sock = uwsgi_malloc(sizeof(struct uwsgi_gateway_socket));
		old_uwsgi_sock->next = uwsgi_sock;
	}

	memset(uwsgi_sock, 0, sizeof(struct uwsgi_gateway_socket));
	uwsgi_sock->fd = fd;
	uwsgi_sock->name = uwsgi_getsockname(fd);
	uwsgi_sock->name_len = strlen(uwsgi_sock->name);
	uwsgi_sock->owner = owner;

	return uwsgi_sock;
}


void uwsgi_gateway_go_cheap(char *gw_id, int queue, int *i_am_cheap) {

	uwsgi_log("[%s pid %d] no more nodes available. Going cheap...\n", gw_id, (int) uwsgi.mypid);
	struct uwsgi_gateway_socket *ugs = uwsgi.gateway_sockets;
	while (ugs) {
		if (!strcmp(ugs->owner, gw_id) && !ugs->subscription) {
			event_queue_del_fd(queue, ugs->fd, event_queue_read());
		}
		ugs = ugs->next;
	}
	*i_am_cheap = 1;
}

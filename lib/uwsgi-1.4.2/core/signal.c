#include "uwsgi.h"

extern struct uwsgi_server uwsgi;

int uwsgi_signal_handler(uint8_t sig) {

	struct uwsgi_signal_entry *use = NULL;

	use = &uwsgi.shared->signal_table[sig];

	if (!use->handler)
		return -1;

	if (!uwsgi.p[use->modifier1]->signal_handler) {
		return -1;
	}

	// check for COW
	if (uwsgi.master_process) {
		if (use->wid != 0 && use->wid != uwsgi.mywid) {
			uwsgi_log("[uwsgi-signal] you have registered this signal in worker %d memory area, only that process will be able to run it\n", use->wid);
			return -1;
		}
	}
	// in lazy mode (without a master), only the same worker will be able to run handlers
	else if (uwsgi.lazy) {
		if (use->wid != uwsgi.mywid) {
			uwsgi_log("[uwsgi-signal] you have registered this signal in worker %d memory area, only that process will be able to run it\n", use->wid);
			return -1;
		}
	}
	else {
		// when master is not active, worker1 is the COW-leader
		if (use->wid != 1 && use->wid != uwsgi.mywid) {
			uwsgi_log("[uwsgi-signal] you have registered this signal in worker %d memory area, only that process will be able to run it\n", use->wid);
			return -1;
		}
	}

	// set harakiri here (if required and if i am a worker)

	if (uwsgi.mywid > 0) {
		uwsgi.workers[uwsgi.mywid].sig = 1;
		uwsgi.workers[uwsgi.mywid].signum = sig;
		uwsgi.workers[uwsgi.mywid].signals++;
		if (uwsgi.shared->options[UWSGI_OPTION_HARAKIRI] > 0) {
			set_harakiri(uwsgi.shared->options[UWSGI_OPTION_HARAKIRI]);
		}
	}
	else if (uwsgi.muleid > 0) {
		uwsgi.mules[uwsgi.muleid - 1].sig = 1;
		uwsgi.mules[uwsgi.muleid - 1].signum = sig;
		uwsgi.mules[uwsgi.muleid - 1].signals++;
		if (uwsgi.shared->options[UWSGI_OPTION_MULE_HARAKIRI] > 0) {
			set_mule_harakiri(uwsgi.shared->options[UWSGI_OPTION_MULE_HARAKIRI]);
		}
	}
#ifdef UWSGI_SPOOLER
	else if (uwsgi.i_am_a_spooler && (getpid() == uwsgi.i_am_a_spooler->pid)) {
		if (uwsgi.shared->options[UWSGI_OPTION_SPOOLER_HARAKIRI] > 0) {
			set_spooler_harakiri(uwsgi.shared->options[UWSGI_OPTION_SPOOLER_HARAKIRI]);
		}
	}
#endif

	int ret = uwsgi.p[use->modifier1]->signal_handler(sig, use->handler);

	if (uwsgi.mywid > 0) {
		uwsgi.workers[uwsgi.mywid].sig = 0;
		if (uwsgi.workers[uwsgi.mywid].harakiri > 0) {
			set_harakiri(0);
		}
	}
	else if (uwsgi.muleid > 0) {
		uwsgi.mules[uwsgi.muleid - 1].sig = 0;
		if (uwsgi.mules[uwsgi.muleid - 1].harakiri > 0) {
			set_mule_harakiri(0);
		}
	}
#ifdef UWSGI_SPOOLER
	else if (uwsgi.i_am_a_spooler && (getpid() == uwsgi.i_am_a_spooler->pid)) {
		if (uwsgi.shared->options[UWSGI_OPTION_SPOOLER_HARAKIRI] > 0) {
			set_spooler_harakiri(0);
		}
	}
#endif

	return ret;
}

int uwsgi_signal_registered(uint8_t sig) {

	if (uwsgi.shared->signal_table[sig].handler != NULL)
		return 1;

	return 0;
}

int uwsgi_register_signal(uint8_t sig, char *receiver, void *handler, uint8_t modifier1) {

	struct uwsgi_signal_entry *use = NULL;

	if (strlen(receiver) > 63)
		return -1;

	uwsgi_lock(uwsgi.signal_table_lock);

	use = &uwsgi.shared->signal_table[sig];

	if (use->handler) {
		uwsgi_log("[uwsgi-signal] you cannot re-register a signal !!!\n");
		uwsgi_unlock(uwsgi.signal_table_lock);
		return -1;
	}

	strncpy(use->receiver, receiver, strlen(receiver) + 1);
	use->handler = handler;
	use->modifier1 = modifier1;
	use->wid = uwsgi.mywid;

	if (use->receiver[0] == 0) {
		uwsgi_log("[uwsgi-signal] signum %d registered (wid: %d modifier1: %d target: default, any worker)\n", sig, uwsgi.mywid, modifier1);
	}
	else {
		uwsgi_log("[uwsgi-signal] signum %d registered (wid: %d modifier1: %d target: %s)\n", sig, uwsgi.mywid, modifier1, receiver);
	}

	uwsgi_unlock(uwsgi.signal_table_lock);

	return 0;
}


int uwsgi_add_file_monitor(uint8_t sig, char *filename) {

	if (strlen(filename) > (0xff - 1)) {
		uwsgi_log("uwsgi_add_file_monitor: invalid filename length\n");
		return -1;
	}

	uwsgi_lock(uwsgi.fmon_table_lock);

	if (ushared->files_monitored_cnt < 64) {

		// fill the fmon table, the master will use it to add items to the event queue
		memcpy(ushared->files_monitored[ushared->files_monitored_cnt].filename, filename, strlen(filename));
		ushared->files_monitored[ushared->files_monitored_cnt].registered = 0;
		ushared->files_monitored[ushared->files_monitored_cnt].sig = sig;

		ushared->files_monitored_cnt++;
	}
	else {
		uwsgi_log("you can register max 64 file monitors !!!\n");
		uwsgi_unlock(uwsgi.fmon_table_lock);
		return -1;
	}

	uwsgi_unlock(uwsgi.fmon_table_lock);

	return 0;

}

struct uwsgi_probe *uwsgi_probe_register(struct uwsgi_probe **up, char *name, int (*func) (int, struct uwsgi_signal_probe *)) {

	struct uwsgi_probe *uwsgi_up = *up, *old_up;

	if (!uwsgi_up) {
		*up = uwsgi_malloc(sizeof(struct uwsgi_probe));
		uwsgi_up = *up;
	}
	else {
		while (uwsgi_up) {
			old_up = uwsgi_up;
			uwsgi_up = uwsgi_up->next;
		}

		uwsgi_up = uwsgi_malloc(sizeof(struct uwsgi_probe));
		old_up->next = uwsgi_up;
	}

	uwsgi_up->name = name;
	uwsgi_up->func = func;
	uwsgi_up->next = NULL;

	uwsgi_log("registered new probe \"%s\" at %p\n", name, uwsgi_up);

	return uwsgi_up;
}


int uwsgi_add_probe(uint8_t sig, char *kind, char *args, int timeout, int freq) {

	uwsgi_lock(uwsgi.probe_table_lock);

	if (ushared->probes_cnt < MAX_PROBES) {

		struct uwsgi_probe *up = uwsgi.probes;
		while (up) {
			if (!strcmp(up->name, kind)) {
				break;
			}
			up = up->next;
		}

		if (!up) {
			uwsgi_log("unable to find probe \"%s\" !!!\n", kind);
			uwsgi_unlock(uwsgi.probe_table_lock);
			return -1;
		}

		// fill the probe table
		ushared->probes[ushared->probes_cnt].func = up->func;
		strncpy(ushared->probes[ushared->probes_cnt].args, args, 1024 - 1);
		ushared->probes[ushared->probes_cnt].registered = 0;
		ushared->probes[ushared->probes_cnt].sig = sig;
		ushared->probes[ushared->probes_cnt].fd = -1;
		ushared->probes[ushared->probes_cnt].state = 0;
		ushared->probes[ushared->probes_cnt].last_event = 0;
		ushared->probes[ushared->probes_cnt].data = NULL;
		ushared->probes[ushared->probes_cnt].cycles = 0;
		ushared->probes[ushared->probes_cnt].bad = 0;

		if (!timeout) {
			timeout = uwsgi.shared->options[UWSGI_OPTION_SOCKET_TIMEOUT];
		}
		ushared->probes[ushared->probes_cnt].timeout = timeout;
		if (!freq) {
			freq = 1;
		}
		ushared->probes[ushared->probes_cnt].freq = freq;
		ushared->probes_cnt++;
	}
	else {
		uwsgi_log("you can register max %d probes !!!\n", MAX_PROBES);
		uwsgi_unlock(uwsgi.probe_table_lock);
		return -1;
	}

	uwsgi_unlock(uwsgi.probe_table_lock);

	return 0;
}

int uwsgi_add_timer(uint8_t sig, int secs) {

	uwsgi_lock(uwsgi.timer_table_lock);

	if (ushared->timers_cnt < 64) {

		// fill the timer table, the master will use it to add items to the event queue
		ushared->timers[ushared->timers_cnt].value = secs;
		ushared->timers[ushared->timers_cnt].registered = 0;
		ushared->timers[ushared->timers_cnt].sig = sig;
		ushared->timers_cnt++;
	}
	else {
		uwsgi_log("you can register max 64 timers !!!\n");
		uwsgi_unlock(uwsgi.timer_table_lock);
		return -1;
	}

	uwsgi_unlock(uwsgi.timer_table_lock);

	return 0;

}

void uwsgi_opt_add_cron(char *opt, char *value, void *foobar) {

	int i;

	struct uwsgi_cron *old_uc, *uc = uwsgi.crons;
	if (!uc) {
		uc = uwsgi_malloc(sizeof(struct uwsgi_cron));
		uwsgi.crons = uc;
	}
	else {
		old_uc = uc;
		while (uc->next) {
			uc = uc->next;
			old_uc = uc;
		}

		old_uc->next = uwsgi_malloc(sizeof(struct uwsgi_cron));
		uc = old_uc->next;
	}

	memset(uc, 0, sizeof(struct uwsgi_cron));

	if (sscanf(value, "%d %d %d %d %d %n", &uc->minute, &uc->hour, &uc->day, &uc->month, &uc->week, &i) != 5) {
		uwsgi_log("invalid cron syntax\n");
		exit(1);
	}
	uc->command = value + i;
}

int uwsgi_signal_add_cron(uint8_t sig, int minute, int hour, int day, int month, int week) {

	if (!uwsgi.master_process)
		return -1;

	uwsgi_lock(uwsgi.cron_table_lock);

	if (ushared->cron_cnt < MAX_CRONS) {

		ushared->cron[ushared->cron_cnt].sig = sig;
		ushared->cron[ushared->cron_cnt].minute = minute;
		ushared->cron[ushared->cron_cnt].hour = hour;
		ushared->cron[ushared->cron_cnt].day = day;
		ushared->cron[ushared->cron_cnt].month = month;
		ushared->cron[ushared->cron_cnt].week = week;
		ushared->cron_cnt++;
	}
	else {
		uwsgi_log("you can register max %d cron !!!\n", MAX_CRONS);
		uwsgi_unlock(uwsgi.cron_table_lock);
		return -1;
	}

	uwsgi_unlock(uwsgi.cron_table_lock);

	return 0;
}

int uwsgi_signal_add_rb_timer(uint8_t sig, int secs, int iterations) {

	if (!uwsgi.master_process)
		return -1;

	uwsgi_lock(uwsgi.rb_timer_table_lock);

	if (ushared->rb_timers_cnt < 64) {

		// fill the timer table, the master will use it to add items to the event queue
		ushared->rb_timers[ushared->rb_timers_cnt].value = secs;
		ushared->rb_timers[ushared->rb_timers_cnt].registered = 0;
		ushared->rb_timers[ushared->rb_timers_cnt].iterations = iterations;
		ushared->rb_timers[ushared->rb_timers_cnt].iterations_done = 0;
		ushared->rb_timers[ushared->rb_timers_cnt].sig = sig;
		ushared->rb_timers_cnt++;
	}
	else {
		uwsgi_log("you can register max 64 rb_timers !!!\n");
		uwsgi_unlock(uwsgi.rb_timer_table_lock);
		return -1;
	}

	uwsgi_unlock(uwsgi.rb_timer_table_lock);

	return 0;

}

void create_signal_pipe(int *sigpipe) {

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigpipe)) {
		uwsgi_error("socketpair()\n");
		exit(1);
	}
	uwsgi_socket_nb(sigpipe[0]);
	uwsgi_socket_nb(sigpipe[1]);

	if (uwsgi.signal_bufsize) {
		if (setsockopt(sigpipe[0], SOL_SOCKET, SO_SNDBUF, &uwsgi.signal_bufsize, sizeof(int))) {
			uwsgi_error("setsockopt()");
		}
		if (setsockopt(sigpipe[0], SOL_SOCKET, SO_RCVBUF, &uwsgi.signal_bufsize, sizeof(int))) {
			uwsgi_error("setsockopt()");
		}

		if (setsockopt(sigpipe[1], SOL_SOCKET, SO_SNDBUF, &uwsgi.signal_bufsize, sizeof(int))) {
			uwsgi_error("setsockopt()");
		}
		if (setsockopt(sigpipe[1], SOL_SOCKET, SO_RCVBUF, &uwsgi.signal_bufsize, sizeof(int))) {
			uwsgi_error("setsockopt()");
		}
	}
}

int uwsgi_remote_signal_send(char *addr, uint8_t sig) {

	struct uwsgi_header uh;

	uh.modifier1 = 110;
	uh.pktsize = 0;
	uh.modifier2 = sig;

	int fd = uwsgi_connect(addr, uwsgi.shared->options[UWSGI_OPTION_SOCKET_TIMEOUT], 0);
	if (fd < 0)
		return -1;

	if (write(fd, (char *) &uh, 4) != 4) {
		uwsgi_error("uwsgi_remote_signal_send()");
		close(fd);
		return -1;
	}

	int ret = uwsgi_read_response(fd, &uh, uwsgi.shared->options[UWSGI_OPTION_SOCKET_TIMEOUT], NULL);

	close(fd);
	return ret;

}

int uwsgi_signal_send(int fd, uint8_t sig) {

	socklen_t so_bufsize_len = sizeof(int);
	int so_bufsize = 0;

	if (write(fd, &sig, 1) != 1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &so_bufsize, &so_bufsize_len)) {
				uwsgi_error("getsockopt()");
			}
			uwsgi_log("*** SIGNAL QUEUE IS FULL: buffer size %d bytes (you can tune it with --signal-bufsize) ***\n", so_bufsize);
		}
		else {
			uwsgi_error("uwsgi_signal_send()");
		}
		return -1;
	}
	return 0;

}

void uwsgi_route_signal(uint8_t sig) {

	struct uwsgi_signal_entry *use = &ushared->signal_table[sig];
	int i;

	// send to first available worker
	if (use->receiver[0] == 0 || !strcmp(use->receiver, "worker") || !strcmp(use->receiver, "worker0")) {
		if (uwsgi_signal_send(ushared->worker_signal_pipe[0], sig)) {
			uwsgi_log("could not deliver signal %d to workers pool\n", sig);
		}
	}
	// send to all workers
	else if (!strcmp(use->receiver, "workers")) {
		for (i = 1; i <= uwsgi.numproc; i++) {
			if (uwsgi_signal_send(uwsgi.workers[i].signal_pipe[0], sig)) {
				uwsgi_log("could not deliver signal %d to worker %d\n", sig, i);
			}
		}
	}
	// route to specific worker
	else if (!strncmp(use->receiver, "worker", 6)) {
		i = atoi(use->receiver + 6);
		if (i > uwsgi.numproc) {
			uwsgi_log("invalid signal target: %s\n", use->receiver);
		}
		if (uwsgi_signal_send(uwsgi.workers[i].signal_pipe[0], sig)) {
			uwsgi_log("could not deliver signal %d to worker %d\n", sig, i);
		}
	}
	// route to subscribed
	else if (!strcmp(use->receiver, "subscribed")) {
	}
	// route to spooler
#ifdef UWSGI_SPOOLER
	else if (!strcmp(use->receiver, "spooler")) {
		if (ushared->worker_signal_pipe[0] != -1) {
			if (uwsgi_signal_send(ushared->spooler_signal_pipe[0], sig)) {
				uwsgi_log("could not deliver signal %d to the spooler\n", sig);
			}
		}
	}
#endif
	else if (!strcmp(use->receiver, "mules")) {
		for (i = 0; i < uwsgi.mules_cnt; i++) {
			if (uwsgi_signal_send(uwsgi.mules[i].signal_pipe[0], sig)) {
				uwsgi_log("could not deliver signal %d to mule %d\n", sig, i + 1);
			}
		}
	}
	else if (!strncmp(use->receiver, "mule", 4)) {
		i = atoi(use->receiver + 4);
		if (i > uwsgi.mules_cnt) {
			uwsgi_log("invalid signal target: %s\n", use->receiver);
		}
		else if (i == 0) {
			if (uwsgi_signal_send(ushared->mule_signal_pipe[0], sig)) {
				uwsgi_log("could not deliver signal %d to a mule\n", sig);
			}
		}
		else {
			if (uwsgi_signal_send(uwsgi.mules[i - 1].signal_pipe[0], sig)) {
				uwsgi_log("could not deliver signal %d to mule %d\n", sig, i);
			}
		}
	}
	else if (!strncmp(use->receiver, "farm_", 5)) {
		char *name = use->receiver + 5;
		struct uwsgi_farm *uf = get_farm_by_name(name);
		if (!uf) {
			uwsgi_log("unknown farm: %s\n", name);
			return;
		}
		if (uwsgi_signal_send(uf->signal_pipe[0], sig)) {
			uwsgi_log("could not deliver signal %d to farm %d (%s)\n", sig, uf->id, uf->name);
		}
	}
	else if (!strncmp(use->receiver, "farm", 4)) {
		i = atoi(use->receiver + 4);
		if (i > uwsgi.farms_cnt || i <= 0) {
			uwsgi_log("invalid signal target: %s\n", use->receiver);
		}
		else {
			if (uwsgi_signal_send(uwsgi.farms[i - 1].signal_pipe[0], sig)) {
				uwsgi_log("could not deliver signal %d to farm %d (%s)\n", sig, i, uwsgi.farms[i - 1].name);
			}
		}
	}

	else {
		// unregistered signal, sending it to all the workers
		uwsgi_log("^^^ UNSUPPORTED SIGNAL TARGET: %s ^^^\n", use->receiver);
	}

}

int uwsgi_signal_wait(int signum) {

	int wait_for_specific_signal = 0;
	uint8_t uwsgi_signal = 0;
	int received_signal = -1;
	int ret;
	struct pollfd pfd[2];

	if (signum > -1) {
		wait_for_specific_signal = 1;
	}

	pfd[0].fd = uwsgi.signal_socket;
	pfd[0].events = POLLIN;
	pfd[1].fd = uwsgi.my_signal_socket;
	pfd[1].events = POLLIN;

cycle:
	ret = poll(pfd, 2, -1);
	if (ret > 0) {
		if (pfd[0].revents == POLLIN) {
			if (read(uwsgi.signal_socket, &uwsgi_signal, 1) != 1) {
				uwsgi_error("read()");
			}
			else {
				(void) uwsgi_signal_handler(uwsgi_signal);
				if (wait_for_specific_signal) {
					if (signum != uwsgi_signal)
						goto cycle;
				}
				received_signal = uwsgi_signal;
			}
		}
		if (pfd[1].revents == POLLIN) {
			if (read(uwsgi.my_signal_socket, &uwsgi_signal, 1) != 1) {
				uwsgi_error("read()");
			}
			else {
				(void) uwsgi_signal_handler(uwsgi_signal);
				if (wait_for_specific_signal) {
					if (signum != uwsgi_signal)
						goto cycle;
				}
			}
			received_signal = uwsgi_signal;
		}

	}

	return received_signal;
}

void uwsgi_receive_signal(int fd, char *name, int id) {

	uint8_t uwsgi_signal;

	ssize_t ret = read(fd, &uwsgi_signal, 1);

	if (ret == 0) {
		goto destroy;
	}
	else if (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
		uwsgi_error("[uwsgi-signal] read()");
		goto destroy;
	}
	else if (ret > 0) {
#ifdef UWSGI_DEBUG
		uwsgi_log_verbose("master sent signal %d to %s %d\n", uwsgi_signal, name, id);
#endif
		if (uwsgi_signal_handler(uwsgi_signal)) {
			uwsgi_log_verbose("error managing signal %d on %s %d\n", uwsgi_signal, name, id);
		}
	}

	return;

destroy:
	// better to kill the whole worker...
	uwsgi_log_verbose("uWSGI %s %d screams: UAAAAAAH my master disconnected: i will kill myself !!!\n", name, id);
	end_me(0);

}

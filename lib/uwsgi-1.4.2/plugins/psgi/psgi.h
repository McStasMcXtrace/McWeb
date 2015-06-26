#undef __USE_GNU
#include "../../uwsgi.h"

#ifdef __APPLE__
#define HAS_BOOL 1
#endif
#include <EXTERN.h>
#include <perl.h>
#include "XSUB.h"


struct uwsgi_perl {

	// path of the statically loaded main app
        char *psgi;
	// locallib path
	char *locallib;

	// perl argv for initialization
	char *embedding[3];

	// check for Devel::StackTrace
	int no_die_catch;
	int stacktrace_available;

	// this is a pointer to the main list of interpreters (required for signals, rpc....);
        PerlInterpreter **main;

	// a lock for dynamic apps
	pthread_mutex_t lock_loader;

	// this fields must be heavy protected in threaded modes
	int tmp_current_i;
	HV **tmp_streaming_stash;
	HV **tmp_input_stash;
	HV **tmp_error_stash;

	CV **tmp_psgix_logger;
	CV **tmp_stream_responder;
};

void init_perl_embedded_module(void);
void uwsgi_psgi_app(void);
int psgi_response(struct wsgi_request *, AV*);

#define psgi_xs(func) newXS("uwsgi::" #func, XS_##func, "uwsgi")
#define psgi_check_args(x) if (items < x) Perl_croak(aTHX_ "Usage: uwsgi::%s takes %d arguments", __FUNCTION__ + 3, x)

SV *uwsgi_perl_obj_call(SV *, char *);
int uwsgi_perl_obj_can(SV *, char *, size_t);
int uwsgi_perl_obj_isa(SV *, char *);
int init_psgi_app(struct wsgi_request *, char *, uint16_t, PerlInterpreter **);
PerlInterpreter *uwsgi_perl_new_interpreter(void);
int uwsgi_perl_mule(char *);

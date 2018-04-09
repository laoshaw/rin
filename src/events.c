#include "events.h"
#include "calibration.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef void (*EventHandler) (Event *in, Context *ctx);

static void sync_action (Event *in, Context *ctx);
static void set_point (Event *in, Context *ctx);
static void ignore (Event *ev, Context *ctx);

EventHandler handlers[] = {
	[EV_SYN] = sync_action,
	[EV_ABS] = set_point,
	[EV_KEY] = ignore
};
#define Handlers_Cnt sizeof(handlers) / sizeof(EventHandler)


static void event_loop (const int fd, Context *ctx) {
	Event	e;

	while ( ctx->point_handler != NULL
		&& read (fd, &e, sizeof(e)) == sizeof(e)) {

		if (e.type < Handlers_Cnt && handlers[e.type] != NULL)
			handlers[e.type](&e, ctx);
		else Unsupported(&e);
	}
}

bool event_app (const char *dev, Application app) {
	int fd = open (dev, O_RDONLY);
	if (fd < 0)
		return false;

	Context	ctx = { .point_handler = calibration_point };

	before_calibration (&ctx);
	event_loop (fd, &ctx);
	finish_calibration (&ctx);

	ctx.point_handler = app.point;
	if (app.init != NULL)
		app.init (&ctx);
	
	event_loop (fd, &ctx);

	if (app.conclusion != NULL)
		app.conclusion (&ctx);

	close(fd);
	return true;
}

#define detail(Ev_Code, Ctx_Flag, Ctx_Field) \
	if (in->code == Ev_Code) { \
		Testify (#Ctx_Flag ": %d\n", in->value); \
		Protocol_Assumption ( #Ctx_Flag "happens once", \
			(ctx->completeness & Ctx_Flag ) == 0 \
		); \
		ctx->completeness |= Ctx_Flag; \
		\
		ctx->Ctx_Field = in->value; \
	}

	static void set_point (Event *in, Context *ctx) {
		detail (ABS_X, CTX_X, point.x)
		else detail (ABS_Y, CTX_Y, point.y)
		else detail (ABS_PRESSURE, CTX_PRESSURE, pressure)
		else Unsupported(in);
	}

#undef detail

static void ignore (Event *ev, Context *ctx) {
	UNUSED(ev);
	UNUSED(ctx);
}

static void sync_action (Event *in, Context *ctx) {

	if (ctx->completeness & CTX_TOUCH) {
		ctx->point_handler(ctx);
	}

	fflush(stdout);
	ctx->completeness = 0;
}


#include "events.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


bool event_loop (const char *dev, const Handler *handlers, uint handlers_cnt ) {
	int fd = open (dev, O_RDONLY);
	if (fd < 0)
		return false;

	
	Event	e;
	Context	ctx;
	while (read (fd, &e, sizeof(e)) == sizeof(e)) {
		if (e.type < handlers_cnt && handlers[e.type] != NULL)
			handlers[e.type](&e, &ctx);
		else Unsupported(&e);
	}

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

	void set_point (Event *in, Context *ctx) {
		detail (ABS_X, CTX_X, x)
		else detail (ABS_Y, CTX_Y, y)
		else detail (ABS_PRESSURE, CTX_PRESSURE, pressure)
		else detail (ABS_MT_TRACKING_ID, CTX_UNIQ_PRESS, id)
		else Unsupported(in);
	}

#undef detail

void ignore (Event *ev, Context *ctx) {
	UNUSED(ev);
	UNUSED(ctx);
}


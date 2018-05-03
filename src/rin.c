#include "events.h"
#include "common.h"

#include <fblib.h>
#include <math.h>


#define sub_or_zero(A,B) ((A > B) ? A - B : 0)
#define add_or_max(A,B,Max) ((A+B < Max) ? A+B : Max)

#define around_square(S, X, Y, R) \
	for (uint y = sub_or_zero (Y,R); y <= add_or_max (Y,R,S.height); y++)\
		for (uint x = sub_or_zero(X,R); x <= add_or_max (X,R,S.width); x++)

void blur_point (Screen s, Point p, uint r, Color c) {
	around_square (s, p.x, p.y, r) {
		float	dx = (float)x - (float)p.x,
			dy = (float)y - (float)p.y;
		float	ar = sqrtf(dx*dx + dy*dy);

		if (ar <= r) {
			float	filling = ar/r;

			Color	found = get_rgb(s, x, y);
			float	r = (c.r * filling + found.r / filling)/2,
				g = (c.g * filling + found.g / filling)/2,
				b = (c.b * filling + found.b / filling)/2;

			dot_rgb (s, x, y, (r < 255)?r:255, (g<255)?r:255,
					(b < 255)?b:255 );
		}
	}
}

void calibrated_point (Context *ctx) {
	if (ctx->last.x - ctx->point.x < Calib(ctx)->threshold.x
		&& ctx->last.y - ctx->point.y < Calib(ctx)->threshold.y)

		return;

	float	x = get_x(ctx);
	float	y = get_y(ctx);

	if (x > 0.9 && y > 0.9)
		ctx->point_handler = NULL; // quit

	blur_point (ctx->screen, (Point){ .x = x * ctx->screen.width,
					  .y = y * ctx->screen.height },
		20, (Color) { .r = 25, .g = 142, .b = 100 }
	);

	ctx->last = ctx->point;
}

int fb_main (Screen screen, Strings args) {
	if (args.count != 2)
		return 1;

	if (!event_app (args.vals[1], (Application) {
		.point		= &calibrated_point,
		.screen		= screen
	}))
		return 2;

	return 0;
}

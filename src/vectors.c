#include "interface.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	s32 x, y;
} Vector;

Vector total_vector;

#define Max_points 30
Vector	points[Max_points] = {0};
size_t	points_cnt = 0;

void add_point (Vector v) {
	if (abs(v.x) < 15 || abs(v.y) < 15)
		return;

	if (++points_cnt >= Max_points)
		Die ("too many points!");

	points[points_cnt] = (Vector) {
		.x = points[points_cnt-1].x + v.x,
		.y = points[points_cnt-1].y + v.y
	};
}

void print_vector(Vector v) {
	if (abs(v.x) > 15 || abs(v.y) > 15)
		printf ("%d %d\n", v.x, v.y); 
}

/* returns direction value so that it takes values <-20;20>,
   0 -> horizontal, +/-20 -> vertical, +/-10 -> 45deg
   sign respective to sign of tan function. */
s32 direction (Vector v) {
	if (v.x == 0 && v.y == 0) return 0;
	if (v.x < 0) {
		v.x = -v.x;
		v.y = -v.y;
	}

	if (v.y > v.x * 10) return 20;
	if (v.y < v.x * -10) return -20;
	s32 tg = 10 * v.y / v.x;
	if (abs(tg) < 10)
		return tg;
	else if (tg > 0)
		return 10 + tg/10;
	else
		return -10 + tg/10;
}

bool same_direction (Vector a, Vector b) {
	if (a.x * b.x < 0) {
		s32 dira = direction(a);
		if (dira < 0) dira += 40;
		s32 dirb = direction(b);
		if (dirb < 0) dira += 40;

		Testify ("dira=%d dirb=%d\n", dira, dirb);

		return abs (dira - dirb) < 10;
	} else {
		Testify ("da=%d db=%d\n", direction(a), direction(b));
		return abs (direction(a)-direction(b)) < 10;
	}
}

int main (int argc, char **argv) {
	if (argc != 2)
		return 1;

	FILE *input = fopen (argv[1], "r");
	if (input == NULL)
		Die ("cannot open file (%s) for reading", argv[1]);

	total_vector = (Vector){0};
	SerializedPoint last_pt, new_pt;

	read_one (last_pt, input);
	while (read_one (new_pt, input) == 1) {
		Vector v = {
			.x = new_pt.x - last_pt.x,
			.y = new_pt.y - last_pt.y
		};
		
		Testify ("%d %d  ", v.x, v.y);

		if (!same_direction(v, total_vector)) {
			add_point(total_vector);
			total_vector = v;
		} else {
			total_vector.x += v.x;
			total_vector.y += v.y;
		}

		last_pt = new_pt;
	}

	add_point(total_vector);

	for (size_t i = 0; i <= points_cnt; i++) {
		s32 new_x = 0, new_y = 0;
		for (size_t j = 0; j <= points_cnt; j++) {
			if (i!=j) {
				if (points[i].x - points[j].x > 20) new_x++;
				if (points[i].y - points[j].y > 20) new_y++;
			}
		}
		printf ("%d %d\n", new_x, new_y);
	}

	fclose(input);	
	return 0;
}
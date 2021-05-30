#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gfx.h"

#define FOV 90
#define OFFSET 100

struct gfx_window window;

float point[3] = {0, 0, 0};
float xy[2] = {0};

float angles[3] = {1, 1, 1};
float distance = 5;

float toRad(float angle) {
	return angle * (M_PI / 180);
}

void drawLine(float x0, float y0, float x1, float y1) {
	gfx_line(&window, x0, y0, x1, y1);
}

void rotate(float x, float y, float angle) {
	xy[0] = (x * cosf(angle)) + (y * sinf(angle));
	xy[1] = (y * cosf(angle)) - (x * sinf(angle));
}

void rotatePoint(float x, float y, float z) {
	rotate(x, y, angles[2]);
	point[0] = xy[0];
	point[1] = xy[1];
	rotate(point[0], z, angles[1]);
	point[0] = xy[0];
	point[2] = xy[1];
	rotate(point[2], point[1], angles[0]);
	point[2] = xy[0];
	point[1] = xy[1];
}

void renderLine(float x1, float y1, float z1, float x2, float y2, float z2) {
	float multiplier;

	float line[4] = {0};

	rotatePoint(x1, y1, z1);
	multiplier = 240 / ((point[2] + distance) * tanf(toRad(FOV) / 2));
	line[0] = point[0] * multiplier;
	line[1] = point[1] * multiplier;

	rotatePoint(x2, y2, z2);
	multiplier = 240 / ((point[2] + distance) * tanf(toRad(FOV) / 2));
	line[2] = point[0] * multiplier;
	line[3] = point[1] * multiplier;

	drawLine(
		line[0] + OFFSET,
		line[1] + OFFSET,
		line[2] + OFFSET,
		line[3] + OFFSET
	);
}

int main() {
	window = gfx_open(500, 500, "GFX Ray Caster Demo");
	
	while (1) {
		renderLine(-1, -1, -1, 1, -1, -1);
		renderLine(-1, 1, -1, 1, 1, -1);
		renderLine(-1, -1, -1, -1, 1, -1);
		renderLine(1, -1, -1, 1, 1, -1);
		renderLine(-1, 1, 1, 1, 1, 1);
		renderLine(-1, -1, 1, 1, -1, 1);
		renderLine(-1, -1, 1, -1, 1, 1);
		renderLine(1, -1, 1, 1, 1, 1);
		renderLine(-1, -1, -1, -1, -1, 1);
		renderLine(1, -1, -1, 1, -1, 1);
		renderLine(1, -1, -1, 1, -1, 1);
		renderLine(1, 1, -1, 1, 1, 1);
		renderLine(-1, 1, -1, -1, 1, 1);
		angles[0] += 0.1;
		usleep(20000);
		gfx_clear(&window);
	}

	gfx_close(&window);
	
	return 0;
}

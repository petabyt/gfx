#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum InteractionType {
	MOUSE, KEY
};

struct gfx_interaction {
	int type;
	int value;
	unsigned int coord[2];
};

struct gfx_window {
	Window window;
	GC canvas;
	Colormap colors;
	bool fastMode;
};

struct gfx_window gfx_open(int width, int height, const char *title);

void gfx_pixel(struct gfx_window *window, int x, int y);
void gfx_line(struct gfx_window *window, int x1, int y1, int x2, int y2);

void gfx_setColor(struct gfx_window *window, int r, int g, int b);
void gfx_clear(struct gfx_window *window);
void gfx_setBackground(struct gfx_window *window, int r, int g, int b);

bool gfxEventWaiting();
struct gfx_interaction gfx_event();
void gfx_flush();

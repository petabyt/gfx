/*
MIT License

Copyright (c) 2020 Daniel C

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef GFX_H
#define GFX_H

#include <stdio.h>
#include <X11/Xlib.h>
#include <unistd.h>

enum InteractionType {
	MOUSE, KEY
};

struct gfx_interaction {
	int type;
	int value;
	unsigned int coord[2];
};

// Size: 32
struct gfx_window {
	Window window;
	GC canvas;
	Colormap colors;
	int fastMode;
};

// This will be initialized when a window is opened.
Display *gfx_display = NULL;

// Flush draw requests to the window
void gfx_flush() {
	XFlush(gfx_display);
}

// Open a new graphics window.
struct gfx_window gfx_open(int width, int height, const char *title) {
	struct gfx_window window;
	
	if (gfx_display == NULL) {
		gfx_display = XOpenDisplay(0);
	}	
	
	// If returned empty
	if(!gfx_display) {
		fprintf(stderr, "gfx_open: unable to open the graphics window.\n");
		return window;
	}
	
	// Detect if display can do fast color mode.
	Visual *visual = DefaultVisual(gfx_display, 0);
	if(visual && visual -> class == TrueColor) {
		window.fastMode = 1;
	} else {
		window.fastMode = 0;
	}
	
	// Create a simple X graphical window
	window.window = XCreateSimpleWindow(
		gfx_display, 
		DefaultRootWindow(gfx_display), 
		0, 0, 
		width, height, 
		0,
		WhitePixel(gfx_display, DefaultScreen(gfx_display)),
		BlackPixel(gfx_display, DefaultScreen(gfx_display))
	);
	
	// Set backing store
	XSetWindowAttributes attr;
	attr.backing_store = Always;
	
	// Change the title and other window attributes
	XChangeWindowAttributes(gfx_display, window.window, CWBackingStore, &attr);
	XStoreName(gfx_display, window.window, title);
	
	// Setup input
	XSelectInput(gfx_display, window.window, StructureNotifyMask|KeyPressMask|ButtonPressMask);
	
	// Map the window
	XMapWindow(gfx_display, window.window);

	// Create graphics context
	window.canvas = XCreateGC(gfx_display, window.window, 0, 0);

	// Setup color
	window.colors = DefaultColormap(gfx_display, 0);
	XSetForeground(gfx_display, window.canvas, WhitePixel(gfx_display, DefaultScreen(gfx_display)));

	// Wait for the MapNotify event
	while (1) {
		XEvent e;
		XNextEvent(gfx_display, &e);
		if (e.type == MapNotify) {
			break;
		}
	}

	return window;
}

// Plot a single pixel based on RGB
void gfx_pixel(struct gfx_window *window, int x, int y) {
	XDrawPoint(gfx_display, window->window, window->canvas, x, y);
	gfx_flush(&window);
}

// Draw a line with whatever algorithm X11 uses
void gfx_line(struct gfx_window *window, int x1, int y1, int x2, int y2) {
	XDrawLine(gfx_display, window->window, window->canvas, x1, y1, x2, y2);
	gfx_flush(&window);
}

// Change the current drawing color.
void gfx_setColor(struct gfx_window *window, int r, int g, int b) {
	XColor color;

	// If this is a truecolor display, we can just pick the color directly.
	if(window->fastMode) {
		color.pixel = ((b&0xff) | ((g&0xff) << 8) | ((r&0xff) << 16));
	} else {
		// Otherwise, we have to allocate it from the colormap of the display.
		color.pixel = 0;
		color.red = r << 8;
		color.green = g << 8;
		color.blue = b << 8;
		XAllocColor(gfx_display, window->colors, &color);
	}

	XSetForeground(gfx_display, window->canvas, color.pixel);
}

// Clear the graphics window to the background color.
void gfx_clear(struct gfx_window *window) {
	XClearWindow(gfx_display, window->window);
}

// Wait for the user to press a key or mouse button.
// This does not check for a key
struct gfx_interaction gfx_event() {
	struct gfx_interaction interaction;
	XEvent event;

	gfx_flush();

	while (1) {
		XNextEvent(gfx_display, &event);
		if (event.type == KeyPress) {
			interaction.type = KEY;
			interaction.value = XLookupKeysym(&event.xkey, 0);
			break;
		} else if (event.type == ButtonPress) {
			interaction.coord[0] = event.xkey.x;
			interaction.coord[1] = event.xkey.y;
			interaction.type = MOUSE;
			interaction.value = event.xbutton.button;
			break;
		}
	}

	return interaction;
}

// Free all gfx resources
void gfx_close(struct gfx_window *window) {
	XFreeGC(gfx_display, window->canvas);
	XCloseDisplay(gfx_display);
}

#endif

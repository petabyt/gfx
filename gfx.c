#include "gfx.h"

// This will be initialized when a window is opened.
Display *gfx_display = NULL;

// Open a new graphics window.
struct gfx_window gfx_open(int width, int height, const char *title) {
	struct gfx_window window;
	
	if (gfx_display == NULL) {
		gfx_display = XOpenDisplay(0);
	}	
	
	// If returned empty
	if(!gfx_display) {
		fprintf(stderr, "gfx_open: unable to open the graphics window.\n");
		exit(1);
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
	
	// Ask for user inputs
	XSelectInput(gfx_display, window.window, StructureNotifyMask|KeyPressMask|ButtonPressMask);
	
	// Map the window
	XMapWindow(gfx_display, window.window);

	window.canvas = XCreateGC(gfx_display, window.window, 0, 0);

	//
	window.colors = DefaultColormap(gfx_display, 0);
	XSetForeground(gfx_display, window.canvas, WhitePixel(gfx_display, DefaultScreen(gfx_display)));

	// Wait for the MapNotify event
	// Create infinite loop until done
	while (1) {
		XEvent e;
		XNextEvent(gfx_display, &e);
		if (e.type == MapNotify) {break;}
	}

	return window;
}

void gfx_pixel(struct gfx_window *window, int x, int y) {
	XDrawPoint(gfx_display, window->window, window->canvas, x, y);
}

void gfx_line(struct gfx_window *window, int x1, int y1, int x2, int y2) {
	XDrawLine(gfx_display, window->window, window->canvas, x1, y1, x2, y2);
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

// Change the current background color.
void gfx_setBackground(struct gfx_window *window, int r, int g, int b) {
	XColor color;
	color.pixel = 0;
	color.red = r << 8;
	color.green = g << 8;
	color.blue = b << 8;
	XAllocColor(gfx_display, window->colors, &color);

	XSetWindowAttributes attr;
	attr.background_pixel = color.pixel;
	XChangeWindowAttributes(gfx_display, window->window, CWBackPixel, &attr);
}

bool gfxEventWaiting() {
	XEvent event;
	
	gfx_flush();
	
	while (1) {
		if(XCheckMaskEvent(gfx_display, -1, &event)) {
			if(event.type == KeyPress) {
				XPutBackEvent(gfx_display, &event);
				return 1;
			} else if (event.type == ButtonPress) {
				XPutBackEvent(gfx_display, &event);
				return 1;
			} else {
				return 0;
			}
		}
	}
}

// Wait for the user to press a key or mouse button.
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

// Flush all previous output to the window.
void gfx_flush() {
	XFlush(gfx_display);
}

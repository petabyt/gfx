#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"

static Display *gfxDisplay=0;
static Window  gfxWindow;
static GC      gfxGc;
static Colormap gfxColormap;
static int      gfxFastColorMode = 0;

// These values are saved by gfx_wait then retrieved later by gfx_xpos and gfx_ypos.
static int savedXpos = 0;
static int savedYpos = 0;

// Open a new graphics window.
void gfxOpen(int width, int height, const char *title) {
	gfxDisplay = XOpenDisplay(0);	
	
	// If returned empty
	if(!gfxDisplay) {
		fprintf(stderr, "gfx_open: unable to open the graphics window.\n");
		exit(1);
	}
	
	// Detect if display can do fast color mode.
	Visual *visual = DefaultVisual(gfxDisplay, 0);
	if(visual && visual -> class == TrueColor) {
		gfxFastColorMode = 1;
	} else {
		gfxFastColorMode = 0;
	}

	int blackColor = BlackPixel(gfxDisplay, DefaultScreen(gfxDisplay));
	int whiteColor = WhitePixel(gfxDisplay, DefaultScreen(gfxDisplay));
	
	// Create a simple X graphical window
	gfxWindow = XCreateSimpleWindow(gfxDisplay, DefaultRootWindow(gfxDisplay), 0, 0, width, height, 0, blackColor, blackColor);
	
	//
	XSetWindowAttributes attr;
	attr.backing_store = Always;
	
	// Change the title and window attributes
	XChangeWindowAttributes(gfxDisplay, gfxWindow, CWBackingStore, &attr);
	XStoreName(gfxDisplay, gfxWindow, title);
	
	// Ask for user inputs
	XSelectInput(gfxDisplay, gfxWindow, StructureNotifyMask|KeyPressMask|ButtonPressMask);
	
	//
	XMapWindow(gfxDisplay, gfxWindow);

	gfxGc = XCreateGC(gfxDisplay, gfxWindow, 0, 0);

	//
	gfxColormap = DefaultColormap(gfxDisplay, 0);
	XSetForeground(gfxDisplay, gfxGc, whiteColor);

	// Wait for the MapNotify event
	// Create infinite loop until done
	while (1) {
		XEvent e;
		XNextEvent(gfxDisplay, &e);
		if (e.type == MapNotify)
			break;
	}
}

// Draw a single point at (x, y)
void gfxPoint(int x, int y) {
	XDrawPoint(gfxDisplay, gfxWindow, gfxGc, x, y);
}

// Draw a line from (x1,y1) to (x2,y2)
void gfxLine(int x1, int y1, int x2, int y2) {
	XDrawLine(gfxDisplay, gfxWindow, gfxGc, x1, y1, x2, y2);
}

// Change the current drawing color.
void gfxColor(int r, int g, int b) {
	XColor color;

	// If this is a truecolor display, we can just pick the color directly.
	if(gfxFastColorMode) {
		color.pixel = ((b&0xff) | ((g&0xff) << 8) | ((r&0xff) << 16) );
	} else {
		// Otherwise, we have to allocate it from the colormap of the display.
		color.pixel = 0;
		color.red = r << 8;
		color.green = g << 8;
		color.blue = b << 8;
		XAllocColor(gfxDisplay, gfxColormap, &color);
	}

	XSetForeground(gfxDisplay, gfxGc, color.pixel);
}

// Clear the graphics window to the background color.
void gfxClear() {
	XClearWindow(gfxDisplay, gfxWindow);
}

// Change the current background color.
void gfxClearColor(int r, int g, int b) {
	XColor color;
	color.pixel = 0;
	color.red = r << 8;
	color.green = g << 8;
	color.blue = b << 8;
	XAllocColor(gfxDisplay, gfxColormap, &color);

	XSetWindowAttributes attr;
	attr.background_pixel = color.pixel;
	XChangeWindowAttributes(gfxDisplay, gfxWindow, CWBackPixel, &attr);
}

int gfxEventWaiting() {
	XEvent event;
	
	gfxFlush();
	
	while (1) {
		if(XCheckMaskEvent(gfxDisplay, -1, &event)) {
			if(event.type == KeyPress) {
				XPutBackEvent(gfxDisplay, &event);
				return 1;
			} else if (event.type == ButtonPress) {
				XPutBackEvent(gfxDisplay, &event);
				return 1;
			} else {
				return 0;
			}
		}
	}
}

// Wait for the user to press a key or mouse button.
char gfxWait() {
	XEvent event;

	gfxFlush();

	while (1) {
		XNextEvent(gfxDisplay, &event);

		if (event.type == KeyPress) {
			savedXpos = event.xkey.x;
			savedYpos = event.xkey.y;
			return XLookupKeysym(&event.xkey, 0);
		} else if (event.type == ButtonPress) {
			savedXpos = event.xkey.x;
			savedYpos = event.xkey.y;
			return event.xbutton.button;
		}
	}
}

/* Return the X and Y coordinates of the last event. */

int gfxXpos() {
	return savedXpos;
}

int gfxYpos() {
	return savedYpos;
}

// Flush all previous output to the window.
void gfxFlush() {
	XFlush(gfxDisplay);
}

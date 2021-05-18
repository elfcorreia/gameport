#include "framebuffer.h"
#include <cstdio>
#include <cstdlib>

#include <X11/Xlib.h>

static int* user_buffer = nullptr;
static int width = 0;
static int height = 0;

static Display* display;
static int screen_num;
static char* appname;

void init(int* buffer, int width, int height) {
    display = XOpenDisplay(nullptr);
    if (display == NULL ) {
	    fprintf(stderr, "XOpenDisplay: fail to connect\n");
	    exit(0);
    }

    screen_num = DefaultScreen(display);
    Window win = XCreateSimpleWindow(
        display, 
        RootWindow(display, screen_num),
		0, 0, width, height, 1,
		BlackPixel(display, screen_num),
		WhitePixel(display, screen_num)
    );
    XSizeHints* size_hints;

    XGCValues values;
    GC gc;
    gc = XCreateGC(display, win, 0, &values);
    XSetForeground(display, gc, BlackPixel(display, screen_num));

    XMapWindow(display, win);

    while (1) {
        XEvent event;
        XNextEvent(display, &event);
        switch (event.type) {

        }
    }
}

void refresh() {

}
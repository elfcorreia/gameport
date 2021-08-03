#include "framebuffer.h"
#include <iostream>
#include <thread>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using std::cerr;
using std::cout;
using std::thread;

static struct {
    int* buffer = nullptr;
    int width = 0;
    int height = 0;
    int pixel_scale = 1;
    thread t;
} context;

void start_x();

int* create(int width, int height, int pixel_scale) {
    context.width = width;
    context.height = height;    
    context.buffer = new int[width * height];
    for (int i = 0, n = width * height; i < n; i++) {
        context.buffer[i] = 0;
    }
    context.pixel_scale = pixel_scale;  
    return context.buffer;
}

void show() {
    context.t = thread(start_x);
}

/*********************
 * X11 implementation
 *********************/
static struct {
    Display* display;
    Window parent;
    Window window;
    unsigned char* buffer;
    int width = 0;
    int height = 0;
    XImage *ximage;
    GC gc;    
} x_context;

void x_main_loop();

void start_x() {
    Display* display = XOpenDisplay(nullptr);           // connect to x server
    if (display == NULL ) {
	    cerr << "XOpenDisplay: fail to connect\n";
	    return;
    }    
    x_context.display = display;

    XVisualInfo vinfo;                                  // verify if 24-bits color is available
    if (!XMatchVisualInfo(
            display,
            XDefaultScreen(display), 24, 
            TrueColor,
            &vinfo)) {
        cerr << "No such visual\n";
        return;
    }

    Window parent = XDefaultRootWindow(display);       // Creates a window        
    XSync(display, True);    
    XSetWindowAttributes attrs = {
        .colormap = XCreateColormap(
            display, parent, 
            vinfo.visual, AllocNone),
    };
    int width = context.width * context.pixel_scale;
    int height = context.height * context.pixel_scale;
    Window window = XCreateWindow(
        display, parent,
        100, 100, width, height, 0, vinfo.depth, 
        InputOutput, vinfo.visual,
        CWBackPixel | CWColormap | CWBorderPixel,
        &attrs
    );    
    x_context.parent = parent;    
    x_context.width = width;
    x_context.height = height;
    x_context.window = window;

    int flatSize = width * height * 4;                      // Creates ximage data buffer
    unsigned char* buffer = new unsigned char[flatSize];
    for (int i = 0, n = width * height; i < n; i += 4) {        
        buffer[i    ] = 0;
        buffer[i + 1] = 0;
        buffer[i + 2] = 0;
        buffer[i + 3] = 0;
    }
    x_context.buffer = buffer;

    int scanline = width * 4;
    cout << "width: " << width << " depth: " << vinfo.depth << " scanline: " << scanline << "\n";

    XImage* ximage = XCreateImage(                     // Creates XImage
        display, vinfo.visual, vinfo.depth,
        ZPixmap, 0, (char*) buffer, 
        width, height,
        8, width * 4
    );
    if (ximage == 0) {
        cerr << "ximage is null!\n";
        return;
    }
    x_context.ximage = ximage;
    
    XSync(display, True);

    XSelectInput(display, window,                       // Listen events
        ExposureMask | KeyPressMask);
    
    XGCValues gcv = { .graphics_exposures = 0 };        // Create Graphic Context
    GC gc = XCreateGC(display, parent,
        GCGraphicsExposures, &gcv
    );
    x_context.gc = gc;

    XMapWindow(display, window);                        // Show

    XEvent event;                                       // Event loop
    while (!XNextEvent(display, &event)) {
        switch(event.type) {
        case Expose:
            cout << "Expose:\n";                        
            for (int y = 0; y < context.height; y++) {
                for (int x = 0; x < context.width; x++) {
                    int color = context.buffer[y * context.width + x];
                    unsigned char red = color >> 16;
                    unsigned char blue = color >> 8;
                    unsigned char green = color;
                    // buffer[y * scanline + x * 4 + 1] = red;
                    // buffer[y * scanline + x * 4 + 2] = blue;
                    // buffer[y * scanline + x * 4 + 3] = green;                    
                    cout << "(" << x << ", " << y << ") red: " << +red << " blue: " << +blue << " green: " << +green << "\n";
                    for (int offsetX = 0; offsetX < context.pixel_scale; offsetX++) {
                        int tx = x * context.pixel_scale + offsetX;
                        for (int offsetY = 0; offsetY < context.pixel_scale; offsetY++) {
                            int ty = (y * context.pixel_scale) + offsetY;
                            buffer[ty * scanline + tx * 4 + 0] = green;
                            buffer[ty * scanline + tx * 4 + 1] = blue;
                            buffer[ty * scanline + tx * 4 + 2] = red;
                            // cout << "(" << tx << ", " << ty << ") red: " << +red << " blue: " << +blue << " green: " << +green << "\n";
                        }
                    }
                }
            }
            XPutImage(display, window,
                gc, ximage,
                0, 0, 0, 0,
                width,
                height
            );
            // XPutBackEvent(display, &event);
            break;
        }
    }
    cout << "No error\n";
}

void refresh() {

}
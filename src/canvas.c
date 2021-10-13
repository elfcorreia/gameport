#include "canvas.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

canvas* canvas_create(const char* options) {
    canvas* c = new canvas;
    c->width = 360;
    c->height = 240;
    c->verbose = false;
    c->pixel_scale = 1;

    std::istringstream ss(options);
    std::string token;
    while (std::getline(ss, token, ';')) {
        std::istringstream ssToken(token);
        std::string name;
        std::getline(ssToken, name, '=');
        std::string value;
        std::getline(ssToken, value, '=');

        std::cout << "canvas_create option " << name << ": " << value << "\n";
        
        if (name == "width") {
            c->width = std::stoi(value);
        } else if (name == "height") {
            c->height = std::stoi(value);
        } else if (name == "pixel_scale") {
            c->pixel_scale = std::stoi(value);
        } else if (name == "verbose" && value == "true") {
            c->verbose = true;
        }
    }

    c->scaled_width = c->width * c->pixel_scale;
    c->scaled_height = c->height * c->pixel_scale;

    engine_init(c);
    c->gui_thread = std::thread([c]() {
        engine_main_loop(c);
    });
    return c;
}

void* canvas_get_context(const canvas* instance) {
    return nullptr;
}

void canvas_destroy(canvas* instance) {    
    engine_dispose(instance);
    delete instance;
}

struct framebuffer {

};

void  framebuffer_set(framebuffer* instance, int index, int color) {

}

void  framebuffer_swap(framebuffer*) {

}



// static struct {
//     int* buffer;
//     int width;
//     int height;
//     int pixel_scale;
//     bool terminate_requested;
//     pthread_t thread;
// } context;

// static void panic(const char* msg);
// static void interrupt_handler(int signal);

// static void  x_create();
// static void  x_request_close();
// static void  x_request_repaint();
// static void* x_main_loop(void*);
// static void* x_repaint_loop(void*);

// int* create(int width, int height, int pixel_scale) {    
//     context.width = width;
//     context.height = height;    
//     context.buffer = (int*) malloc(width * height * sizeof(int));
//     for (int i = 0, n = width * height; i < n; i++) {
//         context.buffer[i] = 00;
//     }
//     context.pixel_scale = pixel_scale;
//     context.terminate_requested = false;

//     signal(SIGINT, &interrupt_handler);
//     x_create();
//     return context.buffer;
// }

// void sync() {
//     x_request_repaint();
// }

// static void panic(const char* msg) {
//     fprintf(stderr, "panic: %s\n", msg);
//     exit(1);
// }

// static void interrupt_handler(int signal) {
//     x_request_close();
//     pthread_join(context.thread, NULL);    
//     raise(SIGTERM);
// }

// /********************************
//  * X11 implementation
//  ********************************/
// static struct {
//     Display* display;
//     Window parent;
//     Window window;
//     unsigned char* buffer;
//     int width;
//     int height;
//     XImage *ximage;
//     GC gc;
//     Atom wmDeleteMessage;
// } x_context;

// void* x_main_loop(void* args) {
//     int scanline = x_context.width * 4;
//     XEvent event;
//     while (!context.terminate_requested) {
//         bool paint = false;        
//         while (XPending(x_context.display) > 0) {            
//             XNextEvent(x_context.display, &event);            
//             switch(event.type) {
//                 case ClientMessage:
//                     if (event.xclient.data.l[0] == x_context.wmDeleteMessage) {                        
//                         context.terminate_requested = true;
//                         paint = false;
//                         break;
//                     }                    
//                 case Expose:
//                     //fprintf(stderr, "Expose\n");
//                     paint = true;
//                     break;
//                 default:
//                     break;
//             }
//         }

//         if (paint) {
//             fprintf(stderr, "h: %d w: %d\n", context.height, context.width);
//             for (int y = 0; y < context.height; y++) {
//                 for (int x = 0; x < context.width; x++) {
//                     int color = context.buffer[y * context.width + x];
//                     //fprintf(stderr, "%x\n", color);
//                     unsigned char red = color >> 16;
//                     unsigned char blue = color >> 8;
//                     unsigned char green = color;
//                     //fprintf(stderr, "(%d, %d) (%d, %d, %d)\n", x, y, red, blue, green);
//                     for (int offsetX = 0; offsetX < context.pixel_scale; offsetX++) {
//                         int tx = x * context.pixel_scale + offsetX;
//                         for (int offsetY = 0; offsetY < context.pixel_scale; offsetY++) {
//                             int ty = (y * context.pixel_scale) + offsetY;
//                             x_context.buffer[ty * scanline + tx * 4 + 0] = green;
//                             x_context.buffer[ty * scanline + tx * 4 + 1] = blue;
//                             x_context.buffer[ty * scanline + tx * 4 + 2] = red;
//                             // cout << "(" << tx << ", " << ty << ") red: " << +red << " blue: " << +blue << " green: " << +green << "\n";
//                             //fprintf(stderr, "(%d, %d) (%d, %d, %d)\n", tx, ty, red, blue, green);
//                         }
//                     }
//                 }
//             }
//             XPutImage(x_context.display, x_context.window,
//                 x_context.gc, x_context.ximage,
//                 0, 0, 0, 0,
//                 x_context.width,
//                 x_context.height
//             );
//             //
//         }
//     }    
//     XCloseDisplay(x_context.display);
// }

// void x_create() {    
//     XInitThreads();

//     // Conecta ao servidor X
//     Display* display = XOpenDisplay(0);           
//     if (display == 0) {
//         panic("XOpenDisplay: fail to connect\n");
//     }
//     x_context.display = display;

//     // Recupera informações visuais
//     XVisualInfo vinfo;
//     if (!XMatchVisualInfo(display, XDefaultScreen(display), 24, TrueColor, &vinfo)) {
//         panic("No such visual\n");
//     }

//     // Cria a janela
//     Window parent = XDefaultRootWindow(display);    
//     XSetWindowAttributes attrs = {
//         .colormap = XCreateColormap(display, parent, vinfo.visual, AllocNone),
//     };
//     int width = context.width * context.pixel_scale;
//     int height = context.height * context.pixel_scale;    
//     Window window = XCreateWindow(
//         display, parent,
//         100, 100, width, height, 0, vinfo.depth, 
//         InputOutput, vinfo.visual,
//         CWBackPixel | CWColormap | CWBorderPixel,
//         &attrs
//     );
//     x_context.parent = parent;    
//     x_context.width = width;
//     x_context.height = height;
//     x_context.window = window;

//     // Creates ximage data buffer
//     int flatSize = width * height * 4;
//     unsigned char* buffer = (unsigned char*) malloc(flatSize);
//     for (int i = 0, n = width * height; i < n; i += 4) {        
//         buffer[i    ] = 0;
//         buffer[i + 1] = 0;
//         buffer[i + 2] = 0;
//         buffer[i + 3] = 0;
//     }
//     x_context.buffer = buffer;    
//     fprintf(stderr, "width: %d, depth: %d\n", width, vinfo.depth);

//     // Creates XImage
//     XImage* ximage = XCreateImage(
//         display, vinfo.visual, vinfo.depth,
//         ZPixmap, 0, (char*) buffer, 
//         width, height,
//         8, width * 4
//     );
//     if (ximage == 0) {
//         panic("Não foi possível criar ximage!");
//     }
//     x_context.ximage = ximage;
    
//     // Listen events
//     XSelectInput(display, window, ExposureMask | KeyPressMask);
    
//     // Create Graphic Context
//     XGCValues gcv = { .graphics_exposures = 0 };
//     GC gc = XCreateGC(display, parent,
//         GCGraphicsExposures, &gcv
//     );
//     x_context.gc = gc;

//     // Show
//     XMapWindow(display, window);
    
//     Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
//     XSetWMProtocols(display, window, &wmDeleteMessage, 1);
//     x_context.wmDeleteMessage = wmDeleteMessage;

//     XSync(x_context.display, false);

//     // Starts main loop
//     if (pthread_create(&(context.thread), NULL, x_main_loop, NULL)) {
//         panic("Não foi possível criar a thread de eventos!");
//     }
// }

// static void x_request_repaint() {
//     XEvent event;
//     event.type = Expose;
//     event.xexpose.x = 0;
//     event.xexpose.y = 0;
//     event.xexpose.width = x_context.width;
//     event.xexpose.height = x_context.height;    
//     XSendEvent(x_context.display, x_context.window, false, ExposureMask, &event);
//     XFlush(x_context.display);
// }

// static void x_request_close() {
//     XEvent event;
//     event.type = ClientMessage;    
//     event.xclient.message_type = x_context.wmDeleteMessage;    
//     event.xclient.format = 32;    
//     XSendEvent(x_context.display, x_context.window, false, SubstructureRedirectMask | SubstructureNotifyMask, &event);
//     XFlush(x_context.display);
// }
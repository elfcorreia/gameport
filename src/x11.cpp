#include "viewport.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <pthread.h>

typedef struct {
    unsigned int width;
    unsigned int height;
    Display* display;
    Window window;    
    XVisualInfo vinfo;
    Atom wm_delete_window;
    GC gc;

    unsigned int upsampling_factor;
    XImage* ximage;
    int* ximage_buffer;
    unsigned int ximage_width;
    unsigned int ximage_height;
    
    int main_thread;
    char finish_requested;
} viewport;

static void panic(const char* fmt, ...);
static void verb(const char* fmt, ...);
static void upsample(int* src, unsigned int width, unsigned height, int* dst, int factor);
static void x11_main_loop();

viewport* viewport_create(unsigned int width, unsigned int height, const char* options) {
    // Conecta ao servidor X
    Display* display = XOpenDisplay(0);           
    if (display == 0) {
        panic("XOpenDisplay() fail to connect\n");
    }    

    // Recupera informações visuais
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display, XDefaultScreen(display), 24, TrueColor, &vinfo)) {
        panic("No such visual\n");
    }
    verb("viewport_create(): XVisualInfo class: %d\n", vinfo.depth);
    verb("viewport_create(): XVisualInfo bits_per_rgb: %d\n", vinfo.bits_per_rgb);
    verb("viewport_create(): XVisualInfo masks: %x %x %x\n", vinfo.red_mask, vinfo.green_mask, vinfo.blue_mask);

    // Recupera a janela superior da hierarquia de janelas
    Window parent = XDefaultRootWindow(display);
    // Atributos da janela
    XSetWindowAttributes attrs;
    // Configura o mapa de cores
    attrs.colormap = XCreateColormap(display, parent, vinfo.visual, AllocNone);

    // Cria a janela
    Window window = XCreateWindow(
        display, parent,
        100, 100, width, height, 0, vinfo.depth, 
        InputOutput, vinfo.visual,
        CWBackPixel | CWColormap | CWBorderPixel,
        &attrs
    );

    // Registra os eventos que desejamos observar na janela
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    
    // Registra o evento de fechar janela
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);
    
    // Cria um contexto gráfico para a janela
    XGCValues gcv;
    gvc.graphics_exposures = 0;
    GC gc = XCreateGC(display, parent, GCGraphicsExposures, &gcv);

    // Exibe a janela
    XMapWindow(display, window);
    XSync(display, false);
    
    // cria o buffer
    unsigned int upsampling_factor = 1; 
    unsigned int ximage_width = upsampling_factor * width;
    unsigned int ximage_height = upsampling_factor * height;
    char* ximage_buffer = (char*) malloc(sizeof(int) * ximage_width * ximage_height);

    // cria o XImage
    XImage* ximage = XCreateImage(
        display,
        vinfo.visual,
        vinfo.depth,
        ZPixmap, 0, ximage_buffer,
        width, height,
        32, 0
    );
    if (!ximage) {
        panic("viewport_create(): XCreateImage() failed!");
    }
    
    // guardas as informações para serem utilizadas futuramente em outra operações
    viewport* instance = (viewport*) malloc(sizeof(viewport));
    
    instance->width = width;
    instance->height = height;    
    instance->display = display;
    instance->window = window;
    instance->wm_delete_window = wm_delete_window;
    instance->gc = gc;
    instance->vinfo = vinfo;

    instance->upsampling_factor = upsampling_factor;
    instance->ximage_buffer = ximage_buffer;
    instance->ximage_width = ximage_width;
    instance->ximage_height = ximage_height;

    instance->finish_requested = 0;
    pthread_create(&instance->main_thread, NULL, x11_main_loop, instance);
    return instance;
}

void viewport_destroy(viewport* instance) {
    free(instance);
}

void viewport_finish(viewport* instance) {    
    instance->finish_requested = 1;
    pthread_join(instance->main_thread, NULL);
}

void viewport_isfinished(viewport* instance) {
    return instance->finish_requested == 1;
}

void viewport_sync(viewport* instance, void* buffer) {
    upsample(
        buffer, instance->width, instance->height,
        instance->ximage_buffer, instance->upsampling_factor
    );    
}

static void x11_main_loop(viewport* instance) {    
    Atom a;
    XEvent event;
    while (!instance->finish_requested && !XNextEvent(instance->display, &event)) {
        switch(event.type) {
        case ClientMessage:
            a = (Atom) event.xclient.data.l[0];
            if (a == instance->wm_delete_window) {
                verb("x11_main_loop(): wm_delete_window event\n");
                instance->finish_requested = 1;
                break;
            }
        case Expose:
            verb("x11_main_loop(): Expose event\n");            
            XPutImage(instance->display, instance->window,
                instance->gc, instance->ximage,
                0, 0, 0, 0,
                instance->ximage_width,
                instance->ximage_height
            );
            break;
        }        
    }    
}

static void verb(const char* fmt, ...) {
    char msg[50];
    time_t agora = time(nullptr);
    strftime(msg, sizeof(msg), "%T", localtime(&agora));
    fprintf(stderr, "[%s] - ", msg);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args); 
}

static void panic(const char* fmt, ...) {
    char msg[50];
    time_t agora = time(nullptr);
    strftime(msg, sizeof(msg), "%T", localtime(&agora));
    fprintf(stderr, "[%s] PANIC! ", msg);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

static void upsample(int* src, unsigned int src_width, unsigned src_height, int* dst, int factor) {
    int dst_width = src_width * factor;    
    for (int y = 0; y < src_height; y++) {
       for (int x = 0; x < src_width; x++) {
            for (int ox = 0; ox < factor; ox++) {
                int tx = x * factor + ox;                
                for (int oy = 0; oy < factor; oy++) {
                    int ty = (y * factor) + oy;
                    dst[ty * dst_width + tx] = src[y * src_width + x];
                }
            }
        }
    }    
}

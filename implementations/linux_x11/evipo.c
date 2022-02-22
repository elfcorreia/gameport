#include "evipo.h"
#include <malloc.h>
//#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct evipo_port_struct {
    unsigned int width;
    unsigned int height;
    Display* display;
    Window window;    
    XVisualInfo vinfo;
    Atom wm_delete_window;
    GC gc;

    unsigned int verbose;

    XImage* ximage;
    int* ximage_buffer;
    unsigned int ximage_buffer_size;    
    
    pthread_t main_thread;
    char finish_requested;
};

static void panic(const char* fmt, ...);
static void verb(const char* fmt, ...);
static void parse_options(const char *str, evipo_port* instance);
//static void* x11_main_loop(void*);

evipo_port* evipo_port_create(unsigned int width, unsigned int height, const char* options) {    
    evipo_port* instance = (evipo_port*) malloc(sizeof(struct evipo_port_struct));
    instance->verbose = false;
    instance->width = width;
    instance->height = height;
    parse_options(options, instance);
    
    // Conecta ao servidor X
    Display* display = XOpenDisplay(0);
    if (display == 0) {
        panic("[evipo] - creating port: XOpenDisplay() returned 0\n");
    }
    verb("[evipo] - creating port: display %p opened\n", display);

    // Recupera informações visuais
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display, XDefaultScreen(display), 24, TrueColor, &vinfo)) {
        panic("[evipo] - creating port: No 24-bits TrueColor visual\n");
    }
    verb("[evipo] - creating port: visual info retrieved depth=%u bits_per_rgb=%u rgb_bitmask=(%x, %x, %x)\n", 
        vinfo.depth, vinfo.bits_per_rgb,
        vinfo.red_mask, vinfo.green_mask, vinfo.blue_mask
    );

    // Recupera a janela superior da hierarquia de janelas
    Window parent = XDefaultRootWindow(display);
    verb("[evipo] - creating port: default root window %u\n", parent);

    // Atributos da janela
    XSetWindowAttributes attrs;
    // Configura o mapa de cores
    attrs.colormap = XCreateColormap(display, parent, vinfo.visual, AllocNone);
    verb("[evipo] - creating port: colormap created using visual info\n");

    // Cria a janela
    Window window = XCreateWindow(
        display, parent,
        100, 100, // TODO: meio da tela
        width, height, 0, vinfo.depth, 
        InputOutput, vinfo.visual,
        CWBackPixel | CWColormap | CWBorderPixel,
        &attrs
    );
    verb("[evipo] - creating port: window %u created\n", window);

    // desabilita redimensionamento
    verb("[evipo] - disabling resizing of window\n");
    XSizeHints size_hints;
    size_hints.flags = PMinSize | PMaxSize;
    size_hints.min_width = width;
    size_hints.max_width = width;
    size_hints.min_height = height;
    size_hints.max_height = height;
    XSetWMNormalHints(display, window, &size_hints);

    // Registra os eventos que desejamos observar na janela
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    verb("[evipo] - creating port: Exposure and KeyPress events selected\n");

    // Registra o evento de fechar janela
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);
    verb("[evipo] - creating port: wm_delete_window event registered\n");

    // Cria um contexto gráfico para a janela
    XGCValues gcv;
    gcv.graphics_exposures = 0;
    GC gc = XCreateGC(display, parent, GCGraphicsExposures, &gcv);
    verb("[evipo] - creating port: graphic context %u created\n", gc);

    // Exibe a janela
    verb("[evipo] - creating port: showing window...\n");
    XMapWindow(display, window);
    XSync(display, false);    

    // cria o buffer
    //unsigned int upsampling_factor = 1; 
    //unsigned int ximage_width = upsampling_factor * width;
    //unsigned int ximage_height = upsampling_factor * height;
    unsigned int ximage_buffer_size = sizeof(int) * width * height;
    int* ximage_buffer = (int*) malloc(ximage_buffer_size);
    verb("[evipo] - creating port: ximage buffer created (width=%upx, height=%upx) at %p\n", width, height);

    // cria o XImage
    XImage* ximage = XCreateImage(
        display,
        vinfo.visual,
        vinfo.depth,
        ZPixmap, 0, (char*) ximage_buffer,
        width, height,
        32, 0
    );
    if (!ximage) {
        panic("[evipo] - creating port: XCreateImage returned %u\n", ximage);
    };
    verb("[evipo] - creating port: ximage created\n");
    
    // guardas as informações para serem utilizadas futuramente em outra operações    
    instance->display = display;
    instance->window = window;
    instance->wm_delete_window = wm_delete_window;
    instance->gc = gc;
    instance->vinfo = vinfo;
    instance->ximage = ximage;
    instance->ximage_buffer_size = ximage_buffer_size;
    instance->ximage_buffer = ximage_buffer;    
    instance->finish_requested = 0;

    return instance;
}

void evipo_port_draw(const evipo_port* instance, void* buffer) {
    verb("[evipo] - drawing %u bytes of buffer=%p into instance=%p\n", instance->ximage_buffer_size, buffer, instance);
    memcpy(instance->ximage_buffer, buffer, instance->ximage_buffer_size);
    XPutImage(instance->display, instance->window,
        instance->gc, instance->ximage,
        0, 0, 0, 0,
        instance->width,
        instance->height
    );
}

int evipo_port_event(const evipo_port* instance, struct evipo_event* out_event) {
    out_event->type = EV_NONE;

    if (instance->finish_requested) {
        verb("[evipo] - finish event\n");
        out_event->type = EV_FINISH;
        return 1;        
    }
    Atom a;
    bool is_a_wm_delete_window_event;
    XEvent event;
    if (!XNextEvent(instance->display, &event)) {
        switch(event.type) {
        case ClientMessage:
            a = (Atom) event.xclient.data.l[0];
            is_a_wm_delete_window_event = a == instance->wm_delete_window;
            if (is_a_wm_delete_window_event) {
                verb("[evipo] - finish event\n");
                out_event->type = EV_FINISH;
                break;
            }
        case Expose:
            verb("[evipo] - draw event\n");
            out_event->type = EV_DRAW;
            break;
        }
    }
    return out_event->type != EV_NONE;
}

void evipo_port_destroy(evipo_port* instance) {
    verb("[evipo] - destroying %p\n", instance);
    free(instance);
}

void evipo_port_finish(evipo_port* instance) {
    verb("[evipo] - request finishing %p\n", instance);
    instance->finish_requested = 1;
}

static void verb(const char* fmt, ...) {
    char msg[50];
    time_t agora = time(NULL);
    strftime(msg, sizeof(msg), "%T", localtime(&agora));
    fprintf(stderr, "%s - ", msg);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args); 
}

static void panic(const char* fmt, ...) {
    char msg[50];
    time_t agora = time(NULL);
    strftime(msg, sizeof(msg), "%T", localtime(&agora));
    fprintf(stderr, "%s - PANIC! ", msg);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

static void parse_options(const char *str, evipo_port* instance) {    
    char buf[513];
    strncpy(buf, str, 512);

    static const char* delimitadors = ";=";
    const char* token = strtok(buf, delimitadors);
    verb("parse_options: token %s\n", token);
    while (token) {
        if (strcmp(token, "verbose") == 0) {
            instance->verbose = true;
        }
        token = strtok(NULL, delimitadors);
        verb("parse_options: token %s\n", token);
    }
}
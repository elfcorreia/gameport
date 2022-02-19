#include "evipo.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <pthread.h>

struct viewport {
    unsigned int width;
    unsigned int height;
    Display* display;
    Window window;    
    XVisualInfo vinfo;
    Atom wm_delete_window;
    GC gc;

    unsigned int pixel_size;
    unsigned int verbose;

    XImage* ximage;
    int* ximage_buffer;
    unsigned int ximage_buffer_size;    
    
    pthread_t main_thread;
    char finish_requested;
};

static void panic(const char* fmt, ...);
static void verb(const char* fmt, ...);
static void parse_options(const char *str, viewport* instance);
static void validate_options(const viewport* instance);
static void* x11_main_loop(void*);

viewport* vcreate(unsigned int width, unsigned int height, 
    const char* options) {    
    
    viewport* instance = new viewport;
    instance->verbose = false;
    instance->pixel_size = 1;

    parse_options(options, instance);
    
    width *= instance->pixel_size;
    height *= instance->pixel_size;
    instance->width = width;
    instance->height = height;

    // Conecta ao servidor X
    Display* display = XOpenDisplay(0);
    if (display == 0) {
        panic("vcreate: XOpenDisplay() returned 0\n");
    }
    verb("vcreate: display %p opened\n", display);

    // Recupera informações visuais
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display, XDefaultScreen(display), 24, TrueColor, &vinfo)) {
        panic("vcreate: No 24-bits TrueColor visual\n");
    }
    verb("vcreate: visual info retrieved depth=%u bits_per_rgb=%u rgb_bitmask=(%x, %x, %x)\n", 
        vinfo.depth, vinfo.bits_per_rgb,
        vinfo.red_mask, vinfo.green_mask, vinfo.blue_mask
    );

    // Recupera a janela superior da hierarquia de janelas
    Window parent = XDefaultRootWindow(display);
    verb("vcreate: default root window %u\n", parent);

    // Atributos da janela
    XSetWindowAttributes attrs;
    // Configura o mapa de cores
    attrs.colormap = XCreateColormap(display, parent, vinfo.visual, AllocNone);
    verb("vcreate: colormap created using visual info\n");

    // Cria a janela
    Window window = XCreateWindow(
        display, parent,
        100, 100, // TODO: meio da tela
        width, height, 0, vinfo.depth, 
        InputOutput, vinfo.visual,
        CWBackPixel | CWColormap | CWBorderPixel,
        &attrs
    );
    verb("vcreate: window %u created\n", window);

    // Registra os eventos que desejamos observar na janela
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    verb("vcreate: Exposure and KeyPress events selected\n");

    // Registra o evento de fechar janela
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);
    verb("vcreate: wm_delete_window event registered\n");

    // Cria um contexto gráfico para a janela
    XGCValues gcv;
    gcv.graphics_exposures = 0;
    GC gc = XCreateGC(display, parent, GCGraphicsExposures, &gcv);
    verb("vcreate: graphic context %u created\n", gc);

    // Exibe a janela
    verb("vcreate: showing window...\n");
    XMapWindow(display, window);
    XSync(display, false);    

    // cria o buffer
    //unsigned int upsampling_factor = 1; 
    //unsigned int ximage_width = upsampling_factor * width;
    //unsigned int ximage_height = upsampling_factor * height;
    unsigned int ximage_buffer_size = sizeof(int) * width * height;
    int* ximage_buffer = (int*) malloc(ximage_buffer_size);
    verb("vcreate: ximage buffer created (width=%upx, height=%upx) at %p\n", width, height);

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
        panic("vcreate: XCreateImage returned %u\n", ximage);
    };
    verb("vcreate: ximage created\n");
    
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
    
    verb("vcreate: starting x11_main_loop thread\n");
    pthread_create(&instance->main_thread, NULL, x11_main_loop, instance);
    return instance;
}

void vdestroy(viewport* instance) {
    verb("vdestroy: destroy %p\n", instance);
    delete instance;
}

void vfinish(viewport* instance) {    
    verb("vfinish: finishing %p\n", instance);
    instance->finish_requested = 1;
    pthread_join(instance->main_thread, NULL);
}

int visfinished(viewport* instance) {
    bool is_finished = instance->finish_requested == 1;
    verb("vfinish: visfinished %p %u\n", instance, is_finished);
    return is_finished;
}

void vsync(viewport* instance, void* buffer) {
    verb("vsync: sync %p\n", instance, buffer);
    if (instance->pixel_size > 1) {
        size_t ps = instance->pixel_size;
        int dst_width = instance->width;
        int src_width = dst_width / ps;
        int src_height = instance->height / ps;

        verb("vsync: reescaling %upx,%upx to %upx,%upx\n", 
                src_width, src_height,
                instance->width, instance->height
        );
        int* src = (int*) buffer;
        int* dst = instance->ximage_buffer;
        size_t dst_linebytes = sizeof(int) * dst_width;

        for (int i = 0; i < src_height; i++) {           
            int* dst_beginline = dst;
            // expande a linha horizontalmente
            for (int* src_endline = src + src_width; src != src_endline; src++) {
                for (size_t j = 0; j < ps; j++) {
                    *dst++ = *src;
                }
            }
            // clona a linha (ps - 1) vezes
            for (int j = 0; j < ps - 1; j++) {
                memcpy(dst, dst_beginline, dst_linebytes);
                dst += dst_width;
            }
        }
    } else {
        memcpy(instance->ximage_buffer, buffer, instance->ximage_buffer_size);
    }
}

static void* x11_main_loop(void* userdata) {
    viewport* instance = (viewport*) userdata;    
    verb("x11_main_loop: called with userdata=%x\n", userdata);
    
    Atom a;
    bool is_a_wm_delete_window_event;
    XEvent event;
    while (!instance->finish_requested && !XNextEvent(instance->display, &event)) {
        switch(event.type) {
        case ClientMessage:
            verb("x11_main_loop - client message received\n");
            a = (Atom) event.xclient.data.l[0];
            is_a_wm_delete_window_event = a == instance->wm_delete_window;
            if (is_a_wm_delete_window_event) {
                verb("x11_main_loop - is a wm_delete_window event\n");
                instance->finish_requested = 1;
                break;
            }
        case Expose:
            verb("x11_main_loop - Expose event received\n");
            verb("x11_main_loop - XPutImage display=%u window=%u gc=%u ximage=%p ximage_width=%u ximage_height=%u\n",
                    instance->display,
                    instance->window,
                    instance->gc,
                    instance->ximage,
                    instance->width,
                    instance->height
                );
            XPutImage(instance->display, instance->window,
                instance->gc, instance->ximage,
                0, 0, 0, 0,
                instance->width,
                instance->height
            );
            break;
        }        
    }
    return nullptr;
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

static void parse_options(const char *str, viewport* instance) {    
    char buf[513];
    strncpy(buf, str, 512);

    static const char* delimitadors = ";=";
    const char* token = strtok(buf, delimitadors);
    verb("parse_options: token %s\n", token);
    while (token) {
        if (strcmp(token, "verbose") == 0) {
            instance->verbose = true;
        } else if (strcmp(token, "pixel_size") == 0) {
            token = strtok(nullptr, delimitadors);            
            instance->pixel_size = atoi(token);
        }
        token = strtok(nullptr, delimitadors);
        verb("parse_options: token %s\n", token);
    }
}
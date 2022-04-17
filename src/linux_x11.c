/**
 * 
 * How to compile to an Object File:
 * 
 * $ cc -Wall -pedantic -fPIC -I .. -c linux_x11.c -o gameport.o
 * 
 * How to build a Shared Library:
 * 
 * $ cc -Wall -pedantic -fPIC -I .. -shared linux_x11.c -o libgameport.so
 * 
 * How to build a Static Library:
 * 
 * $ cc -Wall -pedantic -fPIC -I .. -c linux_x11.c -o gameport.o
 * $ ar rcs libgameport.a gameport.o
 * 
 */
#include "gameport.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static void panic(gameport_t const* instance, const char* fmt, ...);
static void verb(gameport_t const* instance, const char* fmt, ...);
static void parse_options(gameport_t* instance, const char* str);

struct gameport_s {
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
    
    char finish_requested;
};

gameport_t* gameport_create(unsigned int width, unsigned int height, const char* options) {    
    gameport_t* instance = (gameport_t*) malloc(sizeof(gameport_t));
    instance->verbose = false;
    instance->width = width;
    instance->height = height;
    parse_options(instance, options);
    
    // Conecta ao servidor X
    Display* display = XOpenDisplay(0);
    if (display == 0) {
        panic(instance, "gameport_create: XOpenDisplay() returned 0\n");
    }
    verb(instance, "gameport_create: display %p opened\n", display);

    // Recupera informações visuais
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display, XDefaultScreen(display), 24, TrueColor, &vinfo)) {
        panic(instance, "gameport_create: No 24-bits TrueColor visual\n");
    }
    verb(instance, "gameport_create: visual info retrieved depth=%u bits_per_rgb=%u rgb_bitmask=(%x, %x, %x)\n", 
        vinfo.depth, vinfo.bits_per_rgb,
        vinfo.red_mask, vinfo.green_mask, vinfo.blue_mask
    );

    // Recupera a janela superior da hierarquia de janelas
    Window parent = XDefaultRootWindow(display);
    verb(instance, "gameport_create: default root window %u\n", parent);

    // Atributos da janela
    XSetWindowAttributes attrs;
    // Configura o mapa de cores
    attrs.colormap = XCreateColormap(display, parent, vinfo.visual, AllocNone);
    verb(instance, "gameport_create: colormap created using visual info\n");

    // Cria a janela
    Window window = XCreateWindow(
        display, parent,
        100, 100, // TODO: meio da tela
        width, height, 0, vinfo.depth, 
        InputOutput, vinfo.visual,
        CWBackPixel | CWColormap | CWBorderPixel,
        &attrs
    );
    verb(instance, "gameport_create: window %u created\n", window);

    // desabilita redimensionamento
    verb(instance, "gameport_create: disabling resizing of window\n");
    XSizeHints size_hints;
    size_hints.flags = PMinSize | PMaxSize;
    size_hints.min_width = width;
    size_hints.max_width = width;
    size_hints.min_height = height;
    size_hints.max_height = height;
    XSetWMNormalHints(display, window, &size_hints);

    // Registra os eventos que desejamos observar na janela
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    verb(instance, "gameport_create: Exposure and KeyPress events selected\n");

    // Registra o evento de fechar janela
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);
    verb(instance, "gameport_create: wm_delete_window event registered\n");

    // Cria um contexto gráfico para a janela
    XGCValues gcv;
    gcv.graphics_exposures = 0;
    GC gc = XCreateGC(display, parent, GCGraphicsExposures, &gcv);
    verb(instance, "gameport_create: graphic context %u created\n", gc);

    // Exibe a janela
    verb(instance, "gameport_create: showing window...\n");
    XMapWindow(display, window);
    XSync(display, false);    

    // cria o buffer
    unsigned int ximage_buffer_size = sizeof(int) * width * height;
    int* ximage_buffer = (int*) malloc(ximage_buffer_size);
    verb(instance, "gameport_create: ximage buffer created (width=%upx, height=%upx, size=%u bytes) at %p\n", width, height, sizeof(int)*width*height, ximage_buffer);

    // cria um objeto XImage
    XImage* ximage = XCreateImage(
        display,
        vinfo.visual,
        vinfo.depth,
        ZPixmap, 0, (char*) ximage_buffer,
        width, height,
        32, 0
    );
    if (!ximage) {
        panic(instance, "gameport_create: XCreateImage returned %u\n", ximage);
    };
    verb(instance, "gameport_create: ximage created\n");
    
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

void gameport_draw(gameport_t const* instance, void* buffer) {
    verb(instance, "gameport_draw: drawing %u bytes of buffer=%p into instance=%p\n", instance->ximage_buffer_size, buffer, instance);
    memcpy(instance->ximage_buffer, buffer, instance->ximage_buffer_size);
    int* ibuf = (int*) buffer;
    int* xbuf = (int*) instance->ximage_buffer;
    printf("(200, 200) xbuffer: %u buffer: %u\n", *(xbuf + 4200), *(ibuf + 4200));

    XPutImage(instance->display, instance->window,
        instance->gc, instance->ximage,
        0, 0, 0, 0,
        instance->width,
        instance->height
    );
    XSync(instance->display, 0);
}

int gameport_next_event(gameport_t* instance, gameport_event_t* out_event) {
    out_event->type = GAMEPORT_NONE;

    if (instance->finish_requested) {
        verb(instance, "gameport_next_event: finish event received\n");
        out_event->type = GAMEPORT_EXIT;
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
                verb(instance, "gameport_next_event: finish event received\n");
                out_event->type = GAMEPORT_EXIT;
                break;
            }
        case Expose:
            verb(instance, "gameport_next_event: draw event received\n");
            out_event->type = GAMEPORT_DRAW;
            break;
        }
    }
    return out_event->type != GAMEPORT_NONE;
}

void gameport_destroy(gameport_t* instance) {
    verb(instance, "gameport_destroy: destroying %p\n", instance);
    free(instance);
}

static void verb(gameport_t const* instance, const char* fmt, ...) {
    char msg[50];
    time_t agora = time(NULL);
    strftime(msg, sizeof(msg), "%T", localtime(&agora));
    fprintf(stderr, "V %s [GAMEPORT] ", msg);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args); 
}

static void panic(gameport_t const* instance, const char* fmt, ...) {
    char msg[50];
    time_t agora = time(NULL);
    strftime(msg, sizeof(msg), "%T", localtime(&agora));
    fprintf(stderr, "P %s [GAMEPORT] ", msg);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

static void parse_options(gameport_t* instance, const char *str) {
    if (!str) return;
    char buf[513];
    strncpy(buf, str, 512);

    static const char* delimitadors = ";=";
    const char* token = strtok(buf, delimitadors);
    verb(instance, "parse_options: token %s\n", token);
    while (token) {
        if (strcmp(token, "verbose") == 0) {
            instance->verbose = true;
        }
        token = strtok(NULL, delimitadors);
        verb(instance, "parse_options: token %s\n", token);
    }
}
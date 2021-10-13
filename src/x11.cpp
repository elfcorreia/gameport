#include "../framebuffer.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <thread>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <time.h>
#include <stdarg.h>

using std::thread;

struct buffer {
    int* raw_data;
    int* colored_data;
    int* rescaled_data;
    XImage* ximage;
    unsigned int width;
    unsigned int height;
};

static const unsigned int MAX_BUFFERS = 4;

struct {
    Display* display;
    Window window;    
    XVisualInfo vinfo;
    GC gc;
    Atom wm_delete_window;
    thread main_thread;
    unsigned int width;
    unsigned int height;
    
    unsigned buffers_count;
    buffer buffers[MAX_BUFFERS];
    unsigned int active_buffer;
} instance;

static void panic(const char* fmt, ...);
static void verb(const char* fmt, ...);
static void x11_main_loop();

void fb_init(unsigned int width, unsigned int height, const char* options) {
    XInitThreads();

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
    verb("fb_init(): class: %d\n", vinfo.depth);
    verb("fb_init(): bits_per_rgb: %d\n", vinfo.bits_per_rgb);
    verb("fb_init(): masks = %x %x %x\n", vinfo.red_mask, vinfo.green_mask, vinfo.blue_mask);

    // janela hierarquicamente superior
    Window parent = XDefaultRootWindow(display);
    XSetWindowAttributes attrs = { // attributos da janela
        // mapa de cores
        .colormap = XCreateColormap(display, parent, vinfo.visual, AllocNone),
    };

    // Cria a janela
    Window window = XCreateWindow(
        display, parent,
        100, 100, width, height, 0, vinfo.depth, 
        InputOutput, vinfo.visual,
        CWBackPixel | CWColormap | CWBorderPixel,
        &attrs
    );

    // Registra eventos que desejamos escutar na janela
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    
    // Registra o evento de fechar janela
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);
    
    // Cria um contexto gráfico para a janela
    XGCValues gcv = { .graphics_exposures = 0 };
    GC gc = XCreateGC(display, parent,
        GCGraphicsExposures, &gcv
    );

    // Exibe
    XMapWindow(display, window);
    XSync(display, false);

    // guardas as informações para serem utilizadas depois
    instance.display = display;
    instance.window = window;
    instance.wm_delete_window = wm_delete_window;
    instance.gc = gc;
    instance.vinfo = vinfo;
    instance.width = width;
    instance.height = height;
    instance.buffers_count = 0;

    // inicia uma thead com o loop principal
    instance.main_thread = thread(x11_main_loop);
}

void fb_finish() {
    if (instance.main_thread.joinable()) {        
        verb("finish(): joining main thread...\n");
        instance.main_thread.join();
    }    
}

void fb_buffer(void* data, int width, int height) {    
    verb("fb_buffer()");
    if (instance.buffers_count == MAX_BUFFERS) {
        panic("No more space for buffers!\n");
    }

    buffer* novo = &(instance.buffers[instance.buffers_count]);
    instance.buffers_count++;

    novo->raw_data = static_cast<int*>(data);
    novo->colored_data = new int[width * height];
    novo->rescaled_data = new int[instance.width * instance.height];
    novo->width = width;
    novo->height = height;

    XImage* ximage = XCreateImage(
        instance.display,
        instance.vinfo.visual,
        instance.vinfo.depth,
        ZPixmap, 0, (char*) novo->rescaled_data,
        instance.width, instance.height,
        32, 0
    );
    if (ximage == 0) {
        panic("Não foi possível criar ximage!");
    }
    novo->ximage = ximage;

    if (instance.buffers_count == 1) {
        instance.active_buffer = 0;
    }
}

void fb_sync(void* data) {
    buffer* b = nullptr;
    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (data == instance.buffers[i].raw_data) {
            b = &(instance.buffers[i]);
        }
    }
    if (b == nullptr) {
        panic("buffer not found for %x", data);
    }
    auto vw = instance.width;
    auto vh = instance.height;
    auto w = b->width;
    auto h = b->height;
    float scale_width_factor = 1 / ((float) w / instance.width);
    float scale_heigth_factor = 1 / ((float) h / instance.height);
    int swf = (int) scale_width_factor;
    int shf = (int) scale_heigth_factor;
    verb("fb_sync(): scale factor w=%f h=%d swf=%d shf=%d\n", 
        scale_width_factor, scale_heigth_factor,
        swf, shf
    );
    int* src = static_cast<int*>(b->raw_data);
    int* dst = b->rescaled_data;    

    //memcpy(b->rescaled_data, b->raw_data, b->width * b->height * b->bytes_per_pixel);
    for (int y = 0; y < h; y++) {
       for (int x = 0; x < w; x++) {
            for (int ox = 0; ox < swf; ox++) {
                int tx = x * swf + ox;                
                for (int oy = 0; oy < shf; oy++) {
                    int ty = (y * shf) + oy;
                    dst[ty * vw + tx] = src[y * w + x];
                }
            }
        }
    }    
}

static void x11_main_loop() {    
    XEvent event;
    bool fim = false;
    Atom a;
    while (!fim && !XNextEvent(instance.display, &event)) {
        switch(event.type) {
        case ClientMessage:
            a = (Atom) event.xclient.data.l[0];
            if (a == instance.wm_delete_window) {
                verb("x11_main_loop(): received wm_delete_window event\n");
                fim = true;
                break;
            }
        case Expose:
            verb("x11_main_loop(): received expose event\n");
            if (instance.buffers_count > 0) {
                verb("x11_main_loop(): XPutImage\n");
                XPutImage(instance.display, instance.window,
                    instance.gc, instance.buffers[instance.active_buffer].ximage,
                    0, 0, 0, 0,
                    instance.width,
                    instance.height
                );
            }
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

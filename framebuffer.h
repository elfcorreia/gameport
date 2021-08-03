#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <thread>

struct framebuffer_options {
    int width;
    int height;
    int pixel_scale;
    bool verbose = false;
};

struct framebuffer {
    framebuffer_options options;
    int* buffer = nullptr;
    std::thread gui_thread;
    int scaled_width;
    int scaled_height;
    int buffer_size;
};

int* fb_init(framebuffer_options*, framebuffer*);
void fb_update(framebuffer*);
void fb_close(framebuffer*);

#endif
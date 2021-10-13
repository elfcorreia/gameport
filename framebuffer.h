#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <thread>

void fb_init(unsigned int width, unsigned int height, const char* options);
void fb_buffer(void* buffer, int width, int height);
void fb_sync(void* buffer);
void fb_swap(void* buffer, void* new_buffer);
void fb_finish();

#endif
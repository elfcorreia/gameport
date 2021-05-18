#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

int* create(int width, int height, int pixel_scale);
void show();
void refresh();

#endif
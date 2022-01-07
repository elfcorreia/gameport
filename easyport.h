#ifndef VIEWPORT_H
#define VIEWPORT_H

struct viewport;

viewport* evcreate(unsigned int width, unsigned int height, const char* options);
void evsync(viewport* instance, void* framebuffer);
void evfinish(viewport* instance);
int evisfinished(viewport* instance);
void evdestroy(viewport* instance);

#endif
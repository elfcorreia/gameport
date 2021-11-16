#ifndef VIEWPORT_H
#define VIEWPORT_H

struct viewport;

viewport* viewport_create(unsigned int width, unsigned int height, const char* options);
void viewport_sync(viewport* instance, void* framebuffer);
void viewport_finish(viewport* instance);
int viewport_isfinished(viewport* instance);
void viewport_destroy(viewport* instance);

#endif
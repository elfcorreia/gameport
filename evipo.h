#ifndef EVIPO_H
#define EVIPO_H

struct viewport;
viewport* vcreate(unsigned int width, unsigned int height, 
							const char* options);
void vsync(viewport* instance, void* buffer);
void vfinish(viewport* instance);
int visfinished(viewport* instance);
void vdestroy(viewport* instance);

#endif
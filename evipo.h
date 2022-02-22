#ifndef EVIPO_H
#define EVIPO_H

#define EV_NONE		 0
#define EV_FINISH    9
#define EV_DRAW		10
#define EV_KEY		20
#define EV_MOUSE	30

#ifdef __cplusplus
extern "C" {
#endif

struct evipo_port_struct;
typedef struct evipo_port_struct evipo_port;
struct evipo_event {
	char type;
	int x, y;
	char key_char;
	int key_code;
};

evipo_port* evipo_port_create(unsigned int width, unsigned int height, const char* options);
void evipo_port_draw(const evipo_port* instance, void* buffer);
int evipo_port_event(const evipo_port* instance, struct evipo_event* out_event);
void evipo_port_finish(evipo_port* instance);
void evipo_port_destroy(evipo_port* instance);

#ifdef __cplusplus
}
#endif

#endif
#ifndef GAMEPORT_H
#define GAMEPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define GAMEPORT_NONE			 0
#define GAMEPORT_DRAW			11
#define GAMEPORT_KEYPRESS		21
#define GAMEPORT_KEYRELEASE		22
#define GAMEPORT_MOUSEMOVE		31
#define GAMEPORT_MOUSECLICK		32
#define GAMEPORT_MOUSEPRESS		33
#define GAMEPORT_MOUSERELEASE	34
#define GAMEPORT_MOUSESCROLL	35
#define GAMEPORT_EXIT			90

struct gameport_event_s {
	char type;
	int x, y;
	char key_char;
	int key_code;
};
typedef struct gameport_event_s gameport_event_t;

struct gameport_s;
typedef struct gameport_s gameport_t;
gameport_t* gameport_create(unsigned int viewport_width, unsigned int viewport_height, const char* options);
	void  gameport_draw(gameport_t const* instance, void* framebuffer);
	 int  gameport_next_event(gameport_t* instance, gameport_event_t* out_event);
	void  gameport_destroy(gameport_t* instance);

#ifdef __cplusplus
}
#endif

#endif
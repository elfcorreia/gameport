#ifndef GAMEPORT_H
#define GAMEPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define GAMEPORT_DRAW			11
#define GAMEPORT_KEYPRESS		21
#define GAMEPORT_KEYRELEASE		22
#define GAMEPORT_MOUSEMOVE		31
#define GAMEPORT_MOUSECLICK		32
#define GAMEPORT_MOUSEPRESS		33
#define GAMEPORT_MOUSERELEASE	34
#define GAMEPORT_MOUSESCROLL	35
#define GAMEPORT_QUIT			99

typedef struct {
	char type;
	int x, y;
	char key_char;
	int key_code;
} gameport_event;

struct gameport_struct;
typedef struct gameport_struct gameport;
gameport* gameport_create(unsigned int viewport_width, unsigned int viewport_height, const char* options);
	void  gameport_draw(gameport* instance, void* framebuffer);
	 int  gameport_next_event(gameport* instance, gameport_event* out_event);
	void  gameport_destroy(gameport* instance);

#endif

#ifdef __cplusplus
}
#endif
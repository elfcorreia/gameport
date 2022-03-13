![](https://img.shields.io/github/release/elfcorreia/gameport.svg) ![](https://img.shields.io/github/license/elfcorreia/gameport.svg)

# GamePort

Gameport is a tiny multiplatform C/C++ library that's aims to enable programmers to display graphics and to handle keyboard and mouse inputs.

## Quick start

Download the `gameport.h` and the `gameport.c` corresponding to your target platform.

Write the following program:

```c
int buffer[320][200];
buffer[100][50] = 0x0f0ff0;

gameport* gp = gameport_create(320, 200, "verbose");
gameport_draw(gp, buffer);
scanf("%*s");
gameport_destroy(gp);
```
build your code with the instructions of your target platform.

### Display Graphics through a Framebuffer

The *framebuffer* abstraction consist of a piece of memory (buffer) to store the pixels that will be rendered on a *viewport*. This *viewport* is platform dependent. So, it can be an X11 window, a Win32 window or an OpenGL context.

The memory layout of an `framebuffer` should be one of:

- a pointer to a unidimensional array: `int framebuffer[width*height];`
- a pointer to a bidimensional array: `int framebuffer[width][height];`
- a pointer to a heap allocated memory: `int* framebuffer = new int[width*height];`

The memory layout of framebuffer is row major: `framebuffer[lines][columns]`

### Pixels and colors

Each pixel color is represented by an 32-bits RGBA value (an `int`). Is easy to think in colors using hexadecimal literals, like colors in HTML, and write its as C/C++ literals:

```c
    int red = 0xff0000;
    int green = 0x00ff00;
    int blue = 0x0000ff;
```
### Viewport size and pixel size

A viewport is an area where the framebuffer will be rendered. In many situations, a 1:1 escale is desired, for example, to render an framebuffer of 320 by 240 in a viewport of 320 by 240 is trivial. However, for a framebuffer of 32x32 pixels is better visualized in an 320x320 pixels viewport. This is possible by scalling pixels.

    gameport_create(32, 32, "verbose;pixel_scale=1")

    gameport_create(320, 320, "verbose;pixel_scale=10")

## Erro handling

On errors a panic function is called. It's prints a message and terminates the program with a call for `exit(1)`.

### API

- `gameport* gameport_create(int viewport_width, int viewport_height, const char* options);`
  
  Creates a new gameport instance
  
- `void gameport_draw(gameport* instance, void* framebuffer);`
  
  Draws the `framebuffer` memory in the gameport's viewport.

- `int gameport_next_event(gameport* instance, gameport_event* out_event);`
  
  Feed the `out_event` structure with the next event.

- `void gameport_destroy(viewport* instance);`
  
  Destroy the gameport `instance`

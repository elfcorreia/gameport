![](https://img.shields.io/github/release/elfcorreia/gameport.svg) ![](https://img.shields.io/github/license/elfcorreia/gameport.svg)

# GamePort

Gameport is a tiny multiplatform C/C++ library that's aims to enable programmers to display graphics and to handle keyboard and mouse inputs.

## Quick start

Download the `gameport.h` and the `gameport.c` corresponding to your target platform.

Write the following program:

```c
int buffer[320][200];
buffer[100][50] = 0x0f0ff0;

gameport_t* gp = gameport_create(320, 200, "verbose");
gameport_draw(gp, buffer);
scanf("%*s");
gameport_destroy(gp);
```
build your code with the instructions of your target platform.

### Display graphics through a framebuffer abstraction

In the gameport you will need a *framebuffer* to display your graphics. A *framebuffer* is a piece of memory (buffer) thats stores the pixels that will be rendered in the graphical output. While the *framebuffer* is an abstraction, showing it it's not. We have many ways of display graphics, an X11 window, a Win32 window, an OpenGL context, for example. This platform provides aims to provide this abstraction. So you can start rigth now programming with graphics. And if you want learn the internals you have a tiny library implementation source code ready to dive in.

### Pixels and colors

Each pixel color is represented by an 32-bits RGBA value (an `int`). Is easy to think in colors using hexadecimal literals, like colors in HTML, and write its as C/C++ literals:

```c
    int red = 0xff0000;
    int green = 0x00ff00;
    int blue = 0x0000ff;
```

### The memory layout of a framebuffer

In this library, the memory is up to you. The memory layout of an `framebuffer` should be one of:

- a pointer to a unidimensional array: `int framebuffer[width*height];`
- a pointer to a bidimensional array: `int framebuffer[width][height];`
- a pointer to a heap allocated memory: `int* framebuffer = new int[width*height];`

The memory layout of framebuffer is row major: `framebuffer[lines][columns]`

### Framebuffer size and viewport size

A *viewport* is an area where the *framebuffer* will be rendered and not necessarily their sizes matches exactly. 

A trivial case is a 320x200 *framebuffer* in a 320x200 *viewport*.

A case in such you are working with a 32x32 *framebuffer* and want to visualize it

Other case is a 320x200 *framebuffer* in a 640x400 *viewport*.

    gameport_create(32, 32, "verbose;pixel_scale=1")
    gameport_create(320, 320, "verbose;pixel_scale=10")

## Erro handling

On errors a panic function is called. It's prints a message and terminates the program with a call for `exit(1)`.

### API

- `gameport_t* gameport_create(int width, int height, const char* options);`
  
  Creates a new gameport instance
  
- `void gameport_draw(gameport_t* instance, void* framebuffer);`
  
  Draws the `framebuffer` memory in the gameport's viewport.

- `int gameport_next_event(gameport_t* instance, gameport_event* out_event);`
  
  Feed the `out_event` structure with the next event.

- `void gameport_destroy(gameport_t* instance);`
  
  Destroy the gameport `instance`

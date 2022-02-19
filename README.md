![](https://img.shields.io/github/release/elfcorreia/viewport.svg) ![](https://img.shields.io/github/license/elfcorreia/viewport.svg)

## Introduction

Evipo cames from "easy viewport" and was designed to provide a viewport for framebuffers.

## Quick start

Place the `evipo.h` and the `evipo.c` for your compile and platform and write something like:

```c
const int width = 320;
const int height = 240;
const int darkcafe = 0x3b2f2f;
viewport* v = evcreate(width, heigth, "verbose");

int framebuffer[width][heigth];
framebuffer[100][50] = darkcafe;

evsync(v, buffer);
getc(stdin);
evdestroy(v);
```

Now you can read more, see some examples or read the source.

## Framebuffer and Viewport abstractions

The framebuffer abstraction consist of a piece of memory (buffer) to store pixels that will be render on a viewport. This viewport is platform dependent. So, it can be an X11 window, a Win32 window or an OpenGL context.

The memory layout of an `framebuffer` should be one of:

- a pointer to a unidimensional array: `int framebuffer[width*height];`
- a pointer to a bidimensional array: `int framebuffer[width][height];`
- a pointer to a heap allocated memory: `int* framebuffer = new int[width*height];`

The memory layout of framebuffer MUST BE row-first: `framebuffer[lines][columns]`

## Pixels and colors

Each pixel color is represented by an RGB value in an `int`. Is easy to think in colors using hexadecimal literals, like HTML colors:

    int red = 0xff0000;
    int green = 0x00ff00;
    int blue = 0x0000ff;

## Erro handling

On errors a panic function is called. It's prints a message and terminates the program with a call for `exit(1)`.

## API

- `viewport* evcreate(unsigned int width, unsigned int height, const char* options);`
  
  Creates and initialize a new viewport object.

- `void evsync(viewport* instance, void* framebuffer);`
  
  Does the magic.

- `bool evisfinished(viewport* instance);`

  Checks if there is a finish request triggered. A finish request can be trigger by a user or by a call of `evfinish`;

- `void evfinish();`
  
  Trigger a finish request.

- `void evdestroy(viewport* instance);`

  Destroy the viewport object.

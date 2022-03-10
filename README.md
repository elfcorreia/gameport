![](https://img.shields.io/github/release/elfcorreia/viewport.svg) ![](https://img.shields.io/github/license/elfcorreia/gameport.svg)

# GamePort Library

Evipo cames from "easy viewport" and was designed to provide a viewport for framebuffers.

## Quick start

Place the `evipo.h` and the `evipo.c` for your compile and platform and write something like:

```c
gameport* gp = gameport_create(320, 240, "verbose");
int buffer[320][240];
buffer[100][50] = 0x0f0ff0;

gameport_event e;
while (gameport_next_event(gp, &e)) {
  switch (e.type) {
    case GAMEPORT_DRAW:
      gameport_draw(gp, buffer);
      break;
    case GAMEPORT_QUIT:
      printf("quiting...\n");
      break;
  }
}
gameport_destroy(gp);
```

Now you can read more, see some examples or read the source.

### Graphics through Framebuffer

The *framebuffer* abstraction consist of a piece of memory (buffer) to store pixels that will be render on a *viewport*. This *viewport* is platform dependent. So, it can be an X11 window, a Win32 window or an OpenGL context.

The memory layout of an `framebuffer` should be one of:

<<<<<<< HEAD
- a pointer to a unidimensional array: `int framebuffer[width*height];`
- a pointer to a bidimensional array: `int framebuffer[width][height];`
- a pointer to a heap allocated memory: `int* framebuffer = new int[width*height];`

The memory layout of framebuffer MUST BE row-first: `framebuffer[lines][columns]`

## Pixels and colors

Each pixel color is represented by an RGB value in an `int`. Is easy to think in colors using hexadecimal literals, like HTML colors:
=======
The memory layout of an `framebuffer` can be:

- a pointer to a unidimensional array: `int framebuffer[32*32];`
- a pointer to a bidimensional array: `int framebuffer[32][32];`
- a pointer to a heap allocated memory: `int* framebuffer = new int[32*32];`
- etc...      

It must be always major row: `framebuffer[lines][columns]`

### Pixels and colors

Each pixel color is represented by an RGBA value in an `int`. Is easy to think in colors using hexadecimal literals, like HTML colors:
>>>>>>> 1fcac96 (refatoração)

    int red = 0xff0000;
    int green = 0x00ff00;
    int blue = 0x0000ff;

<<<<<<< HEAD
## Erro handling

On errors a panic function is called. It's prints a message and terminates the program with a call for `exit(1)`.

## API

- `viewport* evcreate(unsigned int width, unsigned int height, const char* options);`
=======
### Viewport size and pixel size

A viewport is an area where the framebuffer will be rendered. In many situations, a 1:1 escale is desired, for example, to render an framebuffer of 320 by 240 in a viewport of 320 by 240 is trivial. However, for a framebuffer of 32x32 pixels is better visualized in an 320x320 pixels viewport. This is possible by scalling pixels.

    gameport_create(32, 32, "verbose;pixel_scale=1")

    gameport_create(320, 320, "verbose;pixel_scale=10")

### Erro handling

On errors a panic function is called. It's prints a message and terminates the program with a call for `exit(1)`.

### API

- `gameport* gameport_create(int viewport_width, int viewport_height, const char* options);`
>>>>>>> 1fcac96 (refatoração)
  
  Creates and initialize a new viewport object.

- `void gameport_draw(gameport* instance, void* framebuffer);`
  
  Does the magic.

<<<<<<< HEAD
- `bool evisfinished(viewport* instance);`

  Checks if there is a finish request triggered. A finish request can be trigger by a user or by a call of `evfinish`;

- `void evfinish();`
  
  Trigger a finish request.
=======
- `int gameport_next_event(gameport* instance, gameport_event* out_event);`
  
  Feed in `out_event` the next event.
>>>>>>> 1fcac96 (refatoração)

- `void gameport_destroy(viewport* instance);`

<<<<<<< HEAD
  Destroy the viewport object.
=======
  Destroy the gameport instance
>>>>>>> 1fcac96 (refatoração)

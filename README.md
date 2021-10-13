# Framebuffer Library

This library was designed for educational purposes in such a way that:

* Enables C/C++ programming students works with graphics as early as possible
* Provides a framebuffer abstraction
* Provides multiple implementations to embrace any student's software resources
* Is an one header library for easier compilation and linkage

## Framebuffer abstraction

The framebuffer abstraction consist of a piece of memory (buffer) to store pixels that will be render on a graphical device. This graphical device is implementation dependent and can be an X11 Window in a Linux Machine, an OpenGL context, an Window in Windows.

## Pixels and colors

Each pixel color represents an RGB value in an `int` value. Like HTML colors, using hexadecimal literals are easy to represent colors:

    int red = 0xff0000;
    int green = 0x00ff00;
    int blue = 0x0000ff;

## Viewport size and buffer size

A Viewport is an area where the buffer will be rendered. In many situations, to render an buffer of 320 by 240 in a viewport of 320 by 240 is trivial.

However, you can too render an buffer of 9x9 pixels in a viewport of 100x100. In such situation there is a upsampling. Downsampling is not available.

## Quick start
    
    fb_init(320, 240, "");
    int buffer[320][240];
    fb_buffer(buffer, 320, 240);
    buffer[100][50] = 0x0f0ff0;
    fb_sync(buffer);
    fb_finish();

## Operations

- `void fb_init(viewport_width, viewport_height, options);`
  initialize and prepares the viewport.

- `void fb_buffer(void* data, int width, int height);`
  register a new buffer with the memory pointed by `data` of `width` and `height` pixels.

  `data` can be:
    - a pointer to a unidimensional array of `int`:
        `int screen[32*32];`
    - a pointer to a bidimensional array pf `int`: 
        `int screen[32][32];`
    - a pointer to a heap allocated memory:
        `int* screen = new int[32*32];`
    - etc...

- `fb_sync(void* data);`
  writes the buffer data to the internals buffers.

- `fb_finish();`
  finalize the viewport.

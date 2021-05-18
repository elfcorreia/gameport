#include "framebuffer.h"
#include <cstdio>

static int* user_buffer = nullptr;
static int width = 0;
static int height = 0;

static Display* display;
static int screen_num;
static char* appname;

void init(int* buffer, int width, int height) {
    display = XOpenDisplay(nullptr);
    if (display == NULL ) {
	    fprintf(stderr, "XOpenDisplay: fail to connect\n");
	    exit(EXIT_FAILURE);
    }

        
}

int refresh() {

}
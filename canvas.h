/**
 * @file canvas.h
 *
 * Functions to draw in a canvas.
 *
 * This header define functions for drawing and managing in a canvas.
 */
#ifndef CANVAS_H
#define CANVAS_H

#include <string>
#include <utility>

/**
 * Initializes the canvas
 */
void canvas_create();
/**
 * Destroy the canvas
 */
void canvas_destroy();

unsigned short canvas_top();
void canvas_set_top(unsigned short top);
unsigned short canvas_left();
void canvas_set_left(unsigned short left);
std::pair<unsigned short, unsigned short> canvas_pos();
void canvas_set_pos(unsigned short top, unsigned short left);
unsigned short canvas_width();
void canvas_set_width(unsigned short width);
unsigned short canvas_height();
void canvas_set_height(unsigned short height);
std::pair<unsigned short, unsigned short> canvas_size();
void canvas_set_size(unsigned short width, unsigned short height);
void canvas_set_title(const std::string& title);
const std::string& canvas_title();
void canvas_kill();

void set_color(unsigned char red, unsigned char green, unsigned char blue);
void set_alpha(unsigned char alpha);
void set_line_width(unsigned char width);
//void draw_set_line_cap(const std::string& cap);

// shapes
int text(int x, int y, const std::string& text);
int stroke_rect(int x, int y, int width, int height);
int fill_rect(int x, int y, int width, int height);

int begin_path();
int line_to(int x, int y);
int move_to(int x, int y);
int arc(int center_x, int center_y, int radius, float start_angle, float end_angle);
int ellipse(int x, int y, int radius_x, int radius_y, float start_angle, float end_angle);
int close_path();
int fill();
int stroke();

int stroke_ellipse(int center_x, int center_y, int radius, float start_angle, float end_angle);
int fill_ellipse(int center_x, int center_y, int radius, float start_angle, float end_angle);

int path();

int pen_size();
int pen_color();
int fill_color();

int fill();

#endif
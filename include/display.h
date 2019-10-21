//
// Created by Matthew Emerson on 10/20/19.
//

#ifndef CHIP8_EMULATOR_DISPLAY_H
#define CHIP8_EMULATOR_DISPLAY_H

#include <SDL.h>

#define DISP_HEIGHT 32
#define DISP_WIDTH 64
#define DISP_SCALE 10

typedef struct {
    SDL_Window *window;
    SDL_Surface *window_surface;
} display;
display *_display;

void *init_display();
void free_display();

uint32_t get_pixel(int x, int y);
void set_pixel(int x, int y, uint32_t pixel);
void test_pixel(uint8_t x, uint8_t y);



#endif //CHIP8_EMULATOR_DISPLAY_H

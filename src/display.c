//
// Created by Matthew Emerson on 10/20/19.
//

#include <display.h>

#include <SDL.h>

void *init_display() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        return NULL;
    }

    _display = malloc(sizeof(display));

    _display->window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISP_WIDTH * DISP_SCALE, DISP_HEIGHT * DISP_SCALE, SDL_WINDOW_SHOWN);
    if(_display->window == NULL) {
        return NULL;
    }

    _display->window_surface = SDL_GetWindowSurface(_display->window);
    return _display->window_surface;
}

void free_display() {
    // window_surface is freed when the window is destroyed
    SDL_DestroyWindow(_display->window);
    _display->window = NULL;
}

uint32_t get_pixel(int x, int y) {
    //Convert the pixels to 32 bit
    uint32_t *pixels = (uint32_t *)_display->window_surface->pixels;

    //Get the requested pixel
    return pixels[(x * DISP_SCALE) + ((_display->window_surface->w * y) * DISP_SCALE)];
}

void _set_pixel(int x, int y, uint32_t pixel) {
    //Convert the pixels to 32 bit
    uint32_t *pixels = (uint32_t *)_display->window_surface->pixels;

    //Set the pixel
    pixels[(y * _display->window_surface->w) + x] = pixel;
}

void set_pixel(uint8_t x, uint8_t y) {
    // if (SDL_MUSTLOCK(_display->window_surface)) {
    //     SDL_UnlockSurface(_display->window_surface);
    // }

    uint32_t *pixels = (uint32_t *)_display->window_surface->pixels;
    for (int i = x * DISP_SCALE; i < (x * DISP_SCALE) + DISP_SCALE; i++) {
        for (int j = y * DISP_SCALE; j < (y * DISP_SCALE) + DISP_SCALE; j++) {
            pixels[i + (j * _display->window_surface->w)] ^= ~((uint32_t)0);
        }
    }

    // if (SDL_MUSTLOCK(_display->window_surface)) {
    //     SDL_LockSurface(_display->window_surface);
    // }
    // SDL_UpdateWindowSurface(_display->window);
}

void update_window() {
    if (SDL_MUSTLOCK(_display->window_surface)) {
        SDL_LockSurface(_display->window_surface);
    }

    SDL_UpdateWindowSurface(_display->window);

    if (SDL_MUSTLOCK(_display->window_surface)) {
        SDL_UnlockSurface(_display->window_surface);
    }
}

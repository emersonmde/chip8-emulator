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

uint32_t get_pixel(SDL_Surface *surface, int x, int y )
{
    //Convert the pixels to 32 bit
    uint32_t *pixels = (uint32_t *)surface->pixels;

    //Get the requested pixel
    return pixels[(y * surface->w) + x];
}

void set_pixel(SDL_Surface *surface, int x, int y, uint32_t pixel )
{
    //Convert the pixels to 32 bit
    uint32_t *pixels = (uint32_t *)surface->pixels;

    //Set the pixel
    pixels[(y * surface->w) + x] = pixel;
}

void test_pixel(uint8_t x, uint8_t y) {
    if (SDL_MUSTLOCK(_display->window_surface)) {
        SDL_UnlockSurface(_display->window_surface);
    }
    printf("bytes per pixel %d", _display->window_surface->format->BitsPerPixel);

    for (int i = x * DISP_SCALE; i < (x * DISP_SCALE) + DISP_SCALE; i++) {
        for (int j = y * DISP_SCALE; j < (y * DISP_SCALE) + DISP_SCALE; j++) {
            set_pixel(_display->window_surface, i, j, ~0x0);
        }
    }

    if (SDL_MUSTLOCK(_display->window_surface)) {
        SDL_LockSurface(_display->window_surface);
    }
    SDL_UpdateWindowSurface(_display->window);
}

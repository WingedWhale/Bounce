#pragma once
#include <SDL2/SDL.h>

typedef enum BOUNCE_TYPE {
    RIGHT = 1,
    LEFT = 2,
    TOP = 3,
    BOTTOM = 4
} BOUNCE_TYPE;

SDL_bool in_bounds(SDL_Window *window, SDL_Rect *rect, BOUNCE_TYPE *bounce_type);
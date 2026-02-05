#pragma once
#include <SDL2/SDL.h>
#include <math.h>

typedef enum DIGITAL_ROUND {
    UP,
    DOWN
} DIGITAL_ROUND;

typedef struct ColoredRect {
    double x, y;
    int w;
    SDL_Color color;
    double dx, dy;
    int dir_x, dir_y;
} ColoredRect;

SDL_Color rainbow(double percentage);
SDL_Color rainbow_digital(double percentage, DIGITAL_ROUND round);
SDL_Color blend(SDL_Color color1, SDL_Color color2, double factor);
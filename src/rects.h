#pragma once
#include <SDL2/SDL.h>
#include <math.h>

typedef enum DIGITAL_ROUND {
    UP,
    DOWN
} DIGITAL_ROUND;

typedef struct {
    double x, y;
    int w, h;
    SDL_Color color;
    double dx, dy;
} ColoredRect;

SDL_Color rainbow(double percentage);
SDL_Color rainbow_digital(double percentage, DIGITAL_ROUND round);
SDL_Color blend(SDL_Color color1, SDL_Color color2, double factor);
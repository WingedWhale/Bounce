#include "rects.h"

SDL_Color rainbow(double percentage)
{
    double factor;
    if (percentage < 0) {
        factor = 0;
    }
    if (percentage >= 1) {
        factor = 1;
    }
    else {
        factor = fmod(percentage, (1.0/6.0)) / (1.0/6.0);
    }

    SDL_Color digit_color_1 = rainbow_digital(percentage, DOWN);
    SDL_Color digit_color_2 = rainbow_digital(percentage, UP);
    
    return (blend(digit_color_1, digit_color_2, factor));
}

/* 0 - (1/6)
 * (1/6) - (2/6)
 * (2/6) - (3/6)
 * (3/6) - (4/6)
 * (4/6) - (5/6)
 * (5/6) - 1
*/

SDL_Color rainbow_digital(double percentage, DIGITAL_ROUND round)
{
    SDL_Color rainbow;
    if (percentage < (1.0/6.0)) {
        if (round == UP) {
            rainbow = (SDL_Color){255, 255, 0, 255}; // YELLOW
        }
        else {
            rainbow = (SDL_Color){255, 0, 0, 255}; // RED
        }
    }
    else if (percentage >= (1.0/6.0) && percentage < (2.0/6.0)) {
        if (round == UP) {
            rainbow = (SDL_Color){0, 255, 0, 255}; // GREEN
        }
        else {
            rainbow = (SDL_Color){255, 255, 0, 255}; // YELLOW
        }
    }
    else if (percentage >= (2.0/6.0) && percentage < (3.0/6.0)) {
        if (round == UP) {
            rainbow = (SDL_Color){0, 255, 255, 255}; // CYAN
        }
        else {
            rainbow = (SDL_Color){0, 255, 0, 255}; // GREEN
        }
    }
    else if (percentage >= (3.0/6.0) && percentage < (4.0/6.0)) {
        if (round == UP) {
            rainbow = (SDL_Color){0, 0, 255, 255}; // BLUE
        }
        else {
            rainbow = (SDL_Color){0, 255, 255, 255}; // CYAN
        }
    }
    else if (percentage >= (4.0/6.0) && percentage < (5.0/6.0)) {
        if (round == UP) {
            rainbow = (SDL_Color){255, 0, 255, 255}; // MAGENTA
        }
        else {
            rainbow = (SDL_Color){0, 0, 255, 255}; // BLUE
        }
    }
    else {
        if (round == UP) {
            rainbow = (SDL_Color){255, 0, 0, 255};
        }
        else {
            rainbow = (SDL_Color){255, 0, 255, 255};
        }
    }

    return rainbow;
}

SDL_Color blend(SDL_Color color1, SDL_Color color2, double factor)
{
    int newR = color1.r + factor * (color2.r - color1.r);
    int newG = color1.g + factor * (color2.g - color1.g);
    int newB = color1.b + factor * (color2.b - color1.b);

    SDL_Color new = {newR, newG, newB, 255};

    return new;
}
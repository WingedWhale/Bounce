#include "bounce.h"

SDL_bool in_bounds(SDL_Window *window, SDL_Rect *rect, BOUNCE_TYPE *bounce_type)
{
    int w_width;
    int w_height;
    SDL_GetWindowSize(window, &w_width, &w_height);

    if (rect->x < 0) {
        *bounce_type = LEFT;
        return SDL_FALSE;
    }
    if (rect->x > (w_width - rect->w)) {
        *bounce_type = RIGHT;
        return SDL_FALSE;
    }
    if (rect->y < 0) {
        *bounce_type = TOP;
        return SDL_FALSE;
    }
    if (rect->y > (w_height - rect->h)) {
        *bounce_type = BOTTOM;
        return SDL_FALSE;
    }
    return SDL_TRUE;
}
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "bounce.h"
#include "rects.h"

#ifndef M_PI
    #define M_PI 3.1415926535897932384626
#endif

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1000

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_bool running = SDL_TRUE;

    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // 2. Create a window
    window = SDL_CreateWindow(
        "SDL2 Learning",                     // Window title
        SDL_WINDOWPOS_CENTERED,              // X position
        SDL_WINDOWPOS_CENTERED,              // Y position
        SCREEN_WIDTH,                                 // Width
        SCREEN_HEIGHT,                                 // Height
        SDL_WINDOW_SHOWN                     // Flags
    );

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 3. Create a renderer (for drawing)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int n_rects = 100;
    ColoredRect rects[n_rects];
    int rect_size = SCREEN_HEIGHT / n_rects;

    double color_range = .6;
    // printf("Enter color range (0.0-1.0): ");
    // if (scanf("%lf", &color_range) <= 0) {
    //     printf("Input error\n");
    //     return 1;
    // }

    for (int i = 0; i < n_rects; i++) {
        rects[i].x = SCREEN_WIDTH / 2;
        rects[i].y = i * rect_size;
        rects[i].w = rect_size;
        rects[i].h = rect_size;
        rects[i].color = rainbow((double)i / (n_rects - 1) * 0.15 + (color_range) * 0.85);
        // rects[i].dx = (double)SCREEN_WIDTH / 100 +  5 * (double)i / n_rects;
        rects[i].dx = 10 * sin((((double)n_rects - i) / n_rects) * 2 * M_PI);
//         double ratio = (double)i / n_rects;
// rects[i].dx = 15 * sin(ratio * 8 * M_PI) * exp(-ratio * 3);


        
        rects[i].dy = 0;
    }

    // 4. Main event loop
    SDL_Event event;
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = SDL_FALSE;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = SDL_FALSE;
                }
            }
        }

        // Clear screen (dark blue)
        SDL_Color background_color = {0, 0, 0, 255};
        SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, 255);
        SDL_RenderClear(renderer);

        // Draw colored rectangles
        for (int i = 0; i < n_rects; i++) {
            ColoredRect *cur = &rects[i];
            SDL_SetRenderDrawColor(renderer, cur->color.r, cur->color.g, cur->color.b, 255);
            SDL_Rect rect = {cur->x, cur->y, cur->w, cur->h};
            SDL_RenderFillRect(renderer, &rect);
        
            BOUNCE_TYPE bounce_type;

            if (!in_bounds(window, &rect, &bounce_type)) {
                switch (bounce_type) {
                    case LEFT:
                        cur->x *= -1;
                        cur->dx *= -1;
                        break;
                    case RIGHT:
                        cur->x -= 2 * ((cur->x + cur->w) - SCREEN_WIDTH);
                        cur->dx *= -1;
                        break;
                    case TOP:
                        cur->y *= -1;
                        cur->dy *= -1;
                        break;
                    case BOTTOM:
                        cur->y -= 2 * ((cur->y + cur->h) - SCREEN_HEIGHT);
                        cur->dy *= -1;
                        break;
                }
            }

            cur->x += cur->dx;
            cur->y += cur->dy;

            // if (i == 0) {
            //     printf("%d %d\n", cur->x, cur->y);
            //     printf("%d %d\n", cur->dx, cur->dy);
            // }
        }

        // Present the rendered frame
        SDL_RenderPresent(renderer);

        // Small delay to avoid 100% CPU usage
        SDL_Delay(16);  // ~60 FPS
    }

    // 5. Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
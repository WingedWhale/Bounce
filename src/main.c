#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "bounce.h"
#include "rects.h"
#include "expression.h"
#include "config.h"

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

    window = SDL_CreateWindow(
        "Bounce",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Config cfg;
    if (!load_config("config.txt", &cfg)) {
        printf("No config.txt found, using default settings\n");
    }

    int n_rects = cfg.n_rects;
    ColoredRect *rects = malloc(n_rects * sizeof(ColoredRect));
    if (!rects) {
        printf("Failed to allocate rectangles\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    int rect_size = SCREEN_HEIGHT / n_rects;

    for (int i = 0; i < n_rects; i++) {
        rects[i].x = SCREEN_WIDTH / 2;
        rects[i].y = i * rect_size;
        rects[i].w = rect_size;
        rects[i].h = rect_size;
        rects[i].color = rainbow((double)i / (n_rects - 1) * 0.15 + (cfg.color_range) * 0.85);
        rects[i].dx = 0;
        rects[i].dy = 0;
        rects[i].dir_x = 1;
        rects[i].dir_y = 1;
    }

    double t = 0.0;

    SDL_Event event;
    while (running) {
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

        // Evaluate dx expression and draw rectangles
        set_variable("n", (double)n_rects);
        set_variable("t", t);

        for (int i = 0; i < n_rects; i++) {
            ColoredRect *cur = &rects[i];

            // Evaluate dx from expression
            set_variable("i", (double)i);
            char expr_buf[MAX_EXPR_LEN];
            strncpy(expr_buf, cfg.dx_expr, MAX_EXPR_LEN);
            char *expr_ptr = expr_buf;
            CalcError err = SUCCESS;
            double eval_dx = parse_expression(&expr_ptr, &err);
            if (err == SUCCESS) {
                cur->dx = eval_dx;
            }

            cur->x += cur->dir_x * cur->dx;
            cur->y += cur->dir_y * cur->dy;

            if (cur->x < 0) {
                cur->x = -cur->x;
                cur->dir_x *= -1;
            }
            else if (cur->x + cur->w > SCREEN_WIDTH) {
                cur->x -= 2 * ((cur->x + cur->w) - SCREEN_WIDTH);
                cur->dir_x *= -1;
            }
            if (cur->y < 0) {
                cur->y = -cur->y;
                cur->dir_y *= -1;
            }
            else if (cur->y + cur->h > SCREEN_HEIGHT) {
                cur->y -= 2 * ((cur->y + cur->h) - SCREEN_HEIGHT);
                cur->dir_y *= -1;
            }

            SDL_SetRenderDrawColor(renderer, cur->color.r, cur->color.g, cur->color.b, 255);
            SDL_Rect rect = {(int)cur->x, (int)cur->y, cur->w, cur->h};
            SDL_RenderFillRect(renderer, &rect);
        }

        t += 16.0 / 1000.0;

        // Present the rendered frame
        SDL_RenderPresent(renderer);

        // Small delay to avoid 100% CPU usage
        SDL_Delay(16);  // ~60 FPS
    }

    // 5. Cleanup
    free(rects);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
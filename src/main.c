#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "rects.h"
#include "expression.h"
#include "config.h"

#define MIN(a, b) ((a) <= (b)) ? (a) : (b);

#ifndef M_PI
    #define M_PI 3.1415926535897932384626
#endif

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1000

SDL_bool in_bounds(ColoredRect *rect, double point_width);
void check_for_collision(ColoredRect *rect, double point_size);
void DrawSquare(SDL_Renderer *renderer, double x, double y, double side);
void DrawCircle(SDL_Renderer *renderer, double x, double y, double radius);

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_bool running = SDL_TRUE;

    SDL_bool wall_collision = (argc > 1) ? strtod(argv[1], NULL) : 1;
    SDL_bool is_square = (argc > 2) ? strcmp(argv[2], "circle") : 1;

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

    SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Config cfg;
    if (!load_config("../config.txt", &cfg) && !load_config("config.txt", &cfg)) {
        printf("No config.txt found, using default settings\n");
    }

    int n_points = cfg.n_rects;
    ColoredRect *points = malloc(n_points * sizeof(ColoredRect));
    if (!points) {
        printf("Failed to allocate rectangles\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    int smaller_dimension = MIN(SCREEN_HEIGHT, SCREEN_WIDTH);
    double point_width = smaller_dimension / n_points;

    if (point_width < 2) {
        point_width = 2;
    }

    for (int i = 0; i < n_points; i++) {
        points[i].x = SCREEN_WIDTH / 2;
        if (i < n_points - 1) {
            points[i].y = point_width / 2 + i * point_width;
        }
        else {
            points[i].y = SCREEN_HEIGHT - point_width / 2 - 1;
        }
        points[i].w = point_width;
        points[i].h = point_width;
        points[i].color = rainbow((double)i / (n_points - 1) * 0.15 + (cfg.color_range) * 0.85);
        points[i].dx = 0;
        points[i].dy = 0;
        points[i].dir_x = 1;
        points[i].dir_y = 1;
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
        set_variable("n", (double)n_points);
        set_variable("t", t);

        for (int i = 0; i < n_points; i++) {
            ColoredRect *cur = &points[i];

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

            // cur->color = rainbow( (double)abs((double)cur->x - SCREEN_WIDTH/2) / (double)(SCREEN_WIDTH/2) );

            if (wall_collision) {
                while (!in_bounds(cur, point_width)) {
                    check_for_collision(cur, point_width);
                }
            }

            SDL_SetRenderDrawColor(renderer, cur->color.r, cur->color.g, cur->color.b, 255);

            if (is_square) {
                DrawSquare(renderer, cur->x, cur->y, cur->w);
            } else {
                DrawCircle(renderer, cur->x, cur->y, cur->w);
            }
            
        }

        t += 16.0 / 1000.0;

        // Present the rendered frame
        SDL_RenderPresent(renderer);

        // Small delay to avoid 100% CPU usage
        SDL_Delay(16);  // ~60 FPS
    }

    // 5. Cleanup
    free(points);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

SDL_bool in_bounds(ColoredRect *rect, double point_width) {
    double x = rect->x;
    double y = rect->y;

    if (x < point_width/2 || x > SCREEN_WIDTH - point_width/2
     || y < point_width/2 || y > SCREEN_HEIGHT - point_width/2) {
        return SDL_FALSE;
     }

     return SDL_TRUE;
}

void check_for_collision(ColoredRect *rect, double point_width) {
    if (rect->x <= point_width / 2) {
        rect->x = 2 * point_width/2 - rect->x;
        rect->dir_x *= -1;
    }
    else if (rect->x >= SCREEN_WIDTH - point_width/2) {
        rect->x -= 2 * (rect->x - (SCREEN_WIDTH - point_width/2));
        rect->dir_x *= -1;
    }
    if (rect->y < point_width/2) {
        rect->y = -rect->y;
        rect->dir_y *= -1;
    }
    else if (rect->y >= SCREEN_HEIGHT - point_width/2) {
        rect->y -= 2 * ((rect->y + rect->h) - SCREEN_HEIGHT);
        rect->dir_y *= -1;
    }
}

void DrawSquare(SDL_Renderer *renderer, double x, double y, double side)
{
    SDL_RenderFillRect(renderer, &(SDL_Rect){x - side/2, y - side/2, side, side});
}

void DrawCircle(SDL_Renderer *renderer, double x, double y, double radius)
{
    for (int w = 0; w <= radius * 2; w++)
    {
        for (int h = 0; h <= radius * 2; h++)
        {
            double dx = radius - w; // horizontal offset
            double dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}
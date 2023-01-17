//
// Created by Natty on 02.08.20.
//

#ifndef SR_CLONE_STARS_H
#define SR_CLONE_STARS_H

#include <SDL2/SDL_render.h>

#include "defs.h"

#define STARS_IN_LEVEL 512

typedef struct Star {
    float x;
    float y;
    float z;

    float size;
    SDL_Texture* texture;
} Star_t;

void st_load_star_textures(SDL_Renderer* renderer);

void st_gen_stars(float xMin, float xMax, float yMin, float yMax);

void st_draw_stars(SDL_Renderer* renderer, float camX, float camY, float window_width, float window_height);

void st_free_star_textures();

#endif//SR_CLONE_STARS_H

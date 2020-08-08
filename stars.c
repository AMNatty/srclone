//
// Created by michal on 02.08.20.
//

#include <SDL2/SDL_image.h>
#include <time.h>

#include "mtwister.h"
#include "stars.h"

struct Star st_level_stars[STARS_IN_LEVEL];

#define STAR_TEXTURE_COUNT 5
SDL_Texture *starTextures[STAR_TEXTURE_COUNT];

void st_load_star_textures(SDL_Renderer *renderer)
{
    starTextures[0] = IMG_LoadTexture(renderer, "data/textures/stars/blue.png");
    starTextures[1] = IMG_LoadTexture(renderer, "data/textures/stars/red.png");
    starTextures[2] = IMG_LoadTexture(renderer, "data/textures/stars/redDwarf.png");
    starTextures[3] = IMG_LoadTexture(renderer, "data/textures/stars/whiteDwarf.png");
    starTextures[4] = IMG_LoadTexture(renderer, "data/textures/stars/blue.png");
}

void st_gen_stars(float xMin, float xMax, float yMin, float yMax)
{
    MTRand mt = seedRand((unsigned long) time(NULL));

    for (size_t i = 0; i < STARS_IN_LEVEL; i++)
    {
        Star_t *star = &st_level_stars[i];

        star->x = (float) (xMin + xMax * genRand(&mt));
        star->y = (float) (yMin + yMax * genRand(&mt));
        star->z = (float) genRand(&mt) * 0.9f + 0.1f;
        star->texture = starTextures[(size_t) genRandLong(&mt) % STAR_TEXTURE_COUNT];
        star->size = (float) (genRand(&mt) * 20.0 + 10.0);
    }
}

void st_draw_stars(SDL_Renderer *renderer, float camX, float camY, float windowWidth, float windowHeight)
{
    SDL_FRect rect;

    for (size_t i = 0; i < STARS_IN_LEVEL; i++)
    {
        Star_t *star = &st_level_stars[i];

        float size = star->size * star->z;

        rect.x = star->x - size - star->z * camX;
        rect.y = star->y - size - star->z * camY;
        rect.w = size;
        rect.h = size;

        rect.x = fmodf(fmodf(rect.x, windowWidth) + windowWidth, windowWidth);
        rect.y = fmodf(fmodf(rect.y, windowHeight) + windowHeight, windowHeight);

        SDL_RenderCopyF(renderer, star->texture, NULL, &rect);
    }
}

void st_free_star_textures()
{
    for (size_t i = 0; i < STAR_TEXTURE_COUNT; i++)
    {
        if (starTextures[i] == NULL)
            continue;

        SDL_DestroyTexture(starTextures[i]);
        starTextures[i] = NULL;
    }
}

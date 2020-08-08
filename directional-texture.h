//
// Created by michal on 02.08.20.
//

#ifndef SR_CLONE_DIRECTIONAL_TEXTURE_H
#define SR_CLONE_DIRECTIONAL_TEXTURE_H

#include <SDL2/SDL.h>

#include "defs.h"


typedef struct DirectionalTexture {
    unsigned int directions;
    SDL_Texture *textures[1];
} DirectionalTexture_t;

DirectionalTexture_t* dt_create(SDL_Renderer *renderer, unsigned int directions, const char *location);
void dt_free(DirectionalTexture_t* texture);

#endif //SR_CLONE_DIRECTIONAL_TEXTURE_H

//
// Created by michal on 02.08.20.
//

#include "directional-texture.h"

#include <SDL2/SDL_image.h>
#include <assert.h>

DirectionalTexture_t * dt_create(SDL_Renderer *renderer, unsigned int directions, const char *location)
{
    assert(directions < 64);

    char *file = (char *) malloc((size_t) strlen(location) + (size_t) strlen("/0000.png") + 1);

    if (file == NULL)
    {
        fprintf(stderr, "Memory allocation failed!");
        exit(EXIT_FAILURE);
    }

    DirectionalTexture_t *tex = (DirectionalTexture_t *) malloc(sizeof(DirectionalTexture_t) + sizeof(SDL_Texture*) * directions);

    if (tex == NULL)
    {
        fprintf(stderr, "Memory allocation for a directional texture failed!");
        exit(EXIT_FAILURE);
    }

    for (unsigned int i = 0; i < directions; i++)
    {
        sprintf(file, "%s/%04u.png", location, i + 1);
        printf("Loading texture %s...\n", file);
        SDL_Texture *texture = IMG_LoadTexture(renderer, file);

        if (texture == NULL)
        {
            fprintf(stderr, "Failed to load texture %s!\n", file);
            free(file);
            exit(EXIT_FAILURE);
        }

        tex->textures[i] = texture;
    }

    free(file);

    tex->directions = directions;
    return tex;
}

void dt_free(DirectionalTexture_t* texture)
{
    if (texture == NULL)
        return;

    for (unsigned int i = 0; i < texture->directions; i++)
        SDL_DestroyTexture(texture->textures[i]);

    free(texture);
}
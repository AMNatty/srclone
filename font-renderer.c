//
// Created by michal on 04.08.20.
//

#include <SDL2/SDL_image.h>
#include "font-renderer.h"
#include "projectile.h"

#include <stdio.h>
#include <stdlib.h>

struct FontRenderer {
    SDL_Texture *sprite_font;
};

FontRenderer_t *fr_init(SDL_Renderer *renderer)
{
    FontRenderer_t *font_renderer = (FontRenderer_t *) malloc(sizeof(FontRenderer_t));

    if (font_renderer == NULL)
    {
        fprintf(stderr, "Failed to allocate a font renderer!");
        exit(EXIT_FAILURE);
    }

    const char font_sprite_sheet[] = "data/textures/font.png";
    font_renderer->sprite_font = IMG_LoadTexture(renderer, font_sprite_sheet);

    if (font_renderer->sprite_font == NULL)
    {
        fprintf(stderr, "Failed to load the following texture: %s\n", font_sprite_sheet);
        exit(EXIT_FAILURE);
    }

    return font_renderer;
}

void fr_draw(SDL_Renderer *renderer, FontRenderer_t *font_renderer, float x, float y, float size, const char *text)
{
    const int c_size = 8;

    SDL_Rect src_rect = {
            .w = c_size,
            .h = c_size
    };

    SDL_FRect dst_rect = {
            x,
            y,
            size,
            size
    };

    for (; *text != '\0'; text++)
    {
        char c = *text;

        switch (c)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                src_rect.x = (c - '0') * c_size;
                src_rect.y = 0;

                break;

            case '+':
                src_rect.x = 10 * c_size;
                src_rect.y = 0;
                break;
            case '-':
                src_rect.x = 11 * c_size;
                src_rect.y = 0;
                break;
            case '.':
                src_rect.x = 12 * c_size;
                src_rect.y = 0;
                break;
            case 'x':
                src_rect.x = 13 * c_size;
                src_rect.y = 0;
                break;
            case 'h':
                src_rect.x = 14 * c_size;
                src_rect.y = 0;
                break;
            case 'i':
                src_rect.x = 15 * c_size;
                src_rect.y = 0;
                break;
            case 'S':
                src_rect.x = 0;
                src_rect.y = 1 * c_size;
                break;
            case '&':
                src_rect.x = 14 * c_size;
                src_rect.y = 1 * c_size;
                break;
            case ' ':
                dst_rect.x += size;
                continue;

            case PT_HEAT_STAR:
                src_rect.x = 1 * c_size;
                src_rect.y = 1 * c_size;
                break;

            case PT_PLASMA_DISC:
                src_rect.x = 2 * c_size;
                src_rect.y = 1 * c_size;
                break;

            case PT_LASER_BEAM:
                src_rect.x = 3 * c_size;
                src_rect.y = 1 * c_size;
                break;

            case PT_POLY_SWARM:
                src_rect.x = 4 * c_size;
                src_rect.y = 1 * c_size;
                break;

            case PT_ELECTRON_FLARE:
                src_rect.x = 5 * c_size;
                src_rect.y = 1 * c_size;
                break;

            case PT_TACHYON_DISC:
                src_rect.x = 6 * c_size;
                src_rect.y = 1 * c_size;
                break;

            default:
                src_rect.x = 15 * c_size;
                src_rect.y = c_size;
                break;
        }

        SDL_RenderCopyF(renderer, font_renderer->sprite_font, &src_rect, &dst_rect);
        dst_rect.x += (1.0f / (float) c_size) * size + size;
    }
}

void fr_free(FontRenderer_t *renderer)
{
    if (renderer == NULL)
        return;

    SDL_DestroyTexture(renderer->sprite_font);

    free(renderer);
}
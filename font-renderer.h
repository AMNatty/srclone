//
// Created by michal on 04.08.20.
//

#ifndef SR_CLONE_FONT_RENDERER_H
#define SR_CLONE_FONT_RENDERER_H

#include <SDL2/SDL_render.h>

#include "defs.h"

typedef struct FontRenderer FontRenderer_t;

FontRenderer_t *fr_init(SDL_Renderer *renderer);
void fr_draw(SDL_Renderer *renderer, FontRenderer_t *font_renderer, float x, float y, float size, const char *text);
void fr_free(FontRenderer_t *renderer);

#endif //SR_CLONE_FONT_RENDERER_H

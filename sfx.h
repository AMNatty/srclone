//
// Created by michal on 07.08.20.
//

#ifndef SR_CLONE_SFX_H
#define SR_CLONE_SFX_H

#include <SDL2/SDL_mixer.h>
#include "mtwister.h"

#define VOLUME_MUSIC 40
#define VOLUME_SFX 128

extern Mix_Music *mus_game_st0;


#define SFX_EXPLOSION_VARIATIONS 5
extern Mix_Chunk *sfx_explosion[];

extern Mix_Chunk *sfx_hit;
extern Mix_Chunk *sfx_player_engine;

void sfx_init();
Mix_Chunk *sfx_get_variation(Mix_Chunk **sfx, unsigned int var_count, MTRand *rand);
void sfx_free();

#endif //SR_CLONE_SFX_H

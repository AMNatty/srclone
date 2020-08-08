//
// Created by michal on 07.08.20.
//

#include "sfx.h"

#include <stdio.h>
#include <stdlib.h>

Mix_Music *mus_game_st0 = NULL;

Mix_Chunk *sfx_player_engine = NULL;
Mix_Chunk *sfx_explosion[SFX_EXPLOSION_VARIATIONS] = { 0 };

Mix_Chunk *sfx_hit = NULL;

void sfx_init()
{
    SDL_version compile_version;
    const SDL_version *link_version = Mix_Linked_Version();
    SDL_MIXER_VERSION(&compile_version);
    printf("Compiled SDL_mixer version: %d.%d.%d\n", compile_version.major, compile_version.minor, compile_version.patch);
    printf("Runtime SDL_mixer version: %d.%d.%d\n", link_version->major, link_version->minor, link_version->patch);

    int mix_flags = MIX_INIT_OGG | MIX_INIT_OPUS;
    int mix_init_result = Mix_Init(mix_flags);
    if ((mix_init_result & mix_flags) == 0)
    {
        fprintf(stderr, "Failed to initialize SDL2_mixer! Error: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }

    const int channels = 2;
    const int chunk_size = 1024;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, channels, chunk_size) == -1)
    {
        fprintf(stderr, "Failed to open SDL2_mixer audio! Error: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }

    int mixing_channels = 128;
    Mix_AllocateChannels(mixing_channels);
    Mix_VolumeMusic(VOLUME_MUSIC);
    Mix_Volume(-1, VOLUME_SFX);

    mus_game_st0 = Mix_LoadMUS("data/sound/game_st0.opus");

    sfx_player_engine = Mix_LoadWAV("data/sound/engine.ogg");
    if (sfx_player_engine != NULL)
        Mix_VolumeChunk(sfx_player_engine, 8);

    sfx_hit = Mix_LoadWAV("data/sound/hit.ogg");
    if (sfx_hit != NULL)
        Mix_VolumeChunk(sfx_hit, 80);

    char buf[64];
    for (unsigned int i = 0; i < SFX_EXPLOSION_VARIATIONS; i++)
    {
        sprintf(buf, "data/sound/explosion%01u.ogg", i);
        sfx_explosion[i] = Mix_LoadWAV(buf);

        if (sfx_explosion[i] != NULL)
            Mix_VolumeChunk(sfx_explosion[i], 12);
    }
}

Mix_Chunk *sfx_get_variation(Mix_Chunk **sfx, unsigned int var_count, MTRand *rand)
{
    return sfx[genRandLong(rand) % var_count];
}

void sfx_free()
{
    for (unsigned int i = 0; i < SFX_EXPLOSION_VARIATIONS; i++)
    {
        if (sfx_explosion[i] != NULL)
            Mix_FreeChunk(sfx_explosion[i]);
    }

    Mix_FreeChunk(sfx_hit);
    Mix_FreeChunk(sfx_player_engine);
}
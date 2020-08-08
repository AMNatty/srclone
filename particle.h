//
// Created by michal on 06.08.20.
//

#ifndef SR_CLONE_PARTICLE_H
#define SR_CLONE_PARTICLE_H

#include <SDL2/SDL_render.h>

#include "defs.h"
#include "directional-texture.h"
#include "mtwister.h"
#include "font-renderer.h"

typedef struct ParticleInstance ParticleInstance_t;

typedef struct ParticleManager ParticleManager_t;

typedef unsigned int particle_id;

typedef void (*particle_tick_func)(ParticleManager_t *, struct ParticleInstance *, float);
typedef void (*particle_render_func)(SDL_Renderer *, struct ParticleInstance *, FontRenderer_t *, float, float);

typedef struct Particle {
    DirectionalTexture_t *sprites;
    float size;
    float initial_scale;
    float final_scale;
    float max_rotational_velocity;
    float mean_lifetime;
    float rotation_variation;
    float lifetime_variation;
    particle_render_func render_func;
    particle_tick_func tick_func;
    bool animation_bound_to_lifetime;
    float time_per_frame;
} Particle_t;

#define MAX_PARTICLES 32
#define PARTICLE_DATA 64

struct ParticleDataFloatingText
{
    float lifetime;
    float size;
    char text[32];
};

extern Particle_t par_explosion1;
extern Particle_t par_hit;
extern Particle_t par_floating_text;

ParticleManager_t *particle_manager_create(MTRand *rand);
void particle_manager_render(ParticleManager_t *particle_manager, SDL_Renderer *renderer, FontRenderer_t *font_renderer, float cam_x, float cam_y);
void particle_manager_tick(ParticleManager_t *particle_manager, float delta_time);
void particle_manager_free(ParticleManager_t *particle_manager);

ParticleInstance_t *particle_create(ParticleManager_t *particle_manager, const Particle_t *particle, float x, float y, float rotation, const char data[PARTICLE_DATA]);
void particle_destroy(ParticleManager_t *particle_manager, particle_id particle);

void part_init(SDL_Renderer * renderer);
void part_free();


#endif //SR_CLONE_PARTICLE_H

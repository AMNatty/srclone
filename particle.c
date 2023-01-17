//
// Created by Natty on 06.08.20.
//

#include "particle.h"
#include "entity.h"

struct ParticleInstance {
    bool active;
    particle_id id;

    float x;
    float y;
    float rotation;

    float rotational_velocity;

    float initial_lifetime;
    float lifetime;

    unsigned int animation_frame;
    float animation_timer;

    const Particle_t* prototype;

    char data[PARTICLE_DATA];
};

struct ParticleManager {
    ParticleInstance_t particles[MAX_PARTICLES];
    MTRand* random;
};


ParticleManager_t* particle_manager_create(MTRand* mt_rand) {
    ParticleManager_t* manager = (ParticleManager_t*) calloc(1, sizeof(ParticleManager_t));

    if (manager == NULL) {
        fprintf(stderr, "Failed to allocate memory for a particle manager!\n");
        exit(EXIT_FAILURE);
    }

    manager->random = mt_rand;

    return manager;
}

void particle_manager_free(ParticleManager_t* particle_manager) {
    free(particle_manager);
}

void particle_manager_tick(ParticleManager_t* particle_manager, float delta_time) {

    for (particle_id i = 0; i < MAX_PARTICLES; i++) {
        ParticleInstance_t* instance = &particle_manager->particles[i];

        if (instance->active) {
            instance->prototype->tick_func(particle_manager, instance, delta_time);
        }
    }
}

void particle_manager_render(ParticleManager_t* particle_manager, SDL_Renderer* renderer, FontRenderer_t* font_renderer, float cam_x, float cam_y) {
    int j = 0;

    for (particle_id i = 0; i < MAX_PARTICLES; i++) {
        ParticleInstance_t* instance = &particle_manager->particles[i];

        if (instance->active) {
            j++;
            instance->prototype->render_func(renderer, instance, font_renderer, cam_x, cam_y);
        }
    }
}


ParticleInstance_t* particle_create(ParticleManager_t* particle_manager, const Particle_t* particle, float x, float y, float rotation, const char data[PARTICLE_DATA]) {
    for (particle_id i = 0; i < MAX_PARTICLES; i++) {
        ParticleInstance_t* instance = &particle_manager->particles[i];

        if (!instance->active) {
            instance->id = i;
            instance->prototype = particle;
            instance->x = x;
            instance->y = y;
            instance->rotation = rotation + ((float) genRand(particle_manager->random) * 2 - 1) * particle->rotation_variation;
            instance->rotational_velocity = ((float) genRand(particle_manager->random) * 2 - 1) * particle->max_rotational_velocity;
            instance->initial_lifetime = ((float) genRand(particle_manager->random) * 2 - 1) * particle->lifetime_variation + particle->mean_lifetime;
            instance->lifetime = instance->initial_lifetime;

            if (data != NULL)
                memcpy(instance->data, data, PARTICLE_DATA);

            instance->animation_frame = 0;
            instance->animation_timer = particle->time_per_frame;

            instance->active = true;

            return instance;
        }
    }

    return NULL;
}

void particle_destroy(ParticleManager_t* particle_manager, particle_id particle) {
    memset(&particle_manager->particles[particle], 0, sizeof(ParticleInstance_t));
}

static void particle_standard_tick_func(ParticleManager_t* manager, ParticleInstance_t* instance, float delta_time) {
    instance->rotation += instance->rotational_velocity * delta_time;
    instance->lifetime -= delta_time;

    if (instance->lifetime <= 0) {
        particle_destroy(manager, instance->id);
        return;
    }

    if (instance->prototype->animation_bound_to_lifetime) {
        const float particle_progress = 1 - instance->lifetime / instance->initial_lifetime;

        if (particle_progress >= 0 && particle_progress <= 1) {
            instance->animation_frame = (unsigned int) ((float) (instance->prototype->sprites->directions - 1) * particle_progress);
        }
    } else {
        instance->animation_timer -= delta_time;

        if (instance->animation_timer <= 0) {
            instance->animation_frame++;

            if (instance->prototype->sprites != NULL)
                instance->animation_frame %= instance->prototype->sprites->directions;

            instance->animation_timer = instance->prototype->time_per_frame;
        }
    }
}

static void particle_standard_render_func(SDL_Renderer* renderer, ParticleInstance_t* instance, FontRenderer_t* font_renderer, float cam_x, float cam_y) {
    const float particle_progress = 1 - instance->lifetime / instance->initial_lifetime;
    const float scale = instance->prototype->initial_scale * (1 - particle_progress) + instance->prototype->final_scale * particle_progress;

    static SDL_FRect rect;
    rect.w = scale * instance->prototype->size;
    rect.h = scale * instance->prototype->size;
    rect.x = instance->x - cam_x - rect.w / 2.0f;
    rect.y = instance->y - cam_y - rect.h / 2.0f;

    SDL_Texture* sprite = instance->prototype->sprites->textures[instance->animation_frame];
    SDL_RenderCopyExF(renderer, sprite, NULL, &rect, instance->rotation * 360.0f / (2 * PI), NULL, 0);
}

static void particle_floating_text_render(SDL_Renderer* renderer, ParticleInstance_t* instance, FontRenderer_t* font_renderer, float cam_x, float cam_y) {
    const float particle_progress = 1 - instance->lifetime / instance->initial_lifetime;
    const float scale = instance->prototype->initial_scale * (1 - particle_progress) + instance->prototype->final_scale * particle_progress;
    const struct ParticleDataFloatingText* data = (struct ParticleDataFloatingText*) instance->data;
    fr_draw(renderer, font_renderer, instance->x - cam_x, instance->y - cam_y, scale * data->size * instance->prototype->size, data->text);
}

Particle_t par_explosion1;
Particle_t par_hit;
Particle_t par_floating_text;

void part_init(SDL_Renderer* renderer) {
    par_explosion1.initial_scale = 1;
    par_explosion1.final_scale = 1.1f;
    par_explosion1.render_func = particle_standard_render_func;
    par_explosion1.tick_func = particle_standard_tick_func;
    par_explosion1.size = 256;
    par_explosion1.mean_lifetime = 0.7f;
    par_explosion1.animation_bound_to_lifetime = true;
    par_explosion1.sprites = dt_create(renderer, 16, "data/textures/particles/explosion1");
    par_explosion1.max_rotational_velocity = PI / 2;
    par_explosion1.lifetime_variation = 0.2f;
    par_explosion1.rotation_variation = 2 * PI;

    par_hit.initial_scale = 1;
    par_hit.final_scale = 1;
    par_hit.render_func = particle_standard_render_func;
    par_hit.tick_func = particle_standard_tick_func;
    par_hit.size = 64;
    par_hit.mean_lifetime = 0.15f;
    par_hit.animation_bound_to_lifetime = true;
    par_hit.sprites = dt_create(renderer, 10, "data/textures/particles/impact");

    par_floating_text.initial_scale = 1.0f;
    par_floating_text.final_scale = 2.0f;
    par_floating_text.render_func = particle_floating_text_render;
    par_floating_text.tick_func = particle_standard_tick_func;
    par_floating_text.size = 8;
    par_floating_text.mean_lifetime = 2.0f;
}

void part_free() {
}
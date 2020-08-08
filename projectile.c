//
// Created by michal on 03.08.20.
//

#include <SDL2/SDL_render.h>

#include "projectile.h"
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159

struct Projectile
{
    projectile_id id;
    float x;
    float y;
    float angle;
    float velocity;
    float lifetime;
    float damage;
    float hit_sound_timer;
    bool friendly;
    ProjectileType_t type;
    projectile_tick_function tick;
    SDL_Texture *sprite;
};

struct ProjectileManager
{
    Projectile_t projectiles[MAX_PROJECTILES];
    bool used_slots[MAX_PROJECTILES];
};

ProjectileManager_t *projectile_manager_create()
{
    ProjectileManager_t *manager = (ProjectileManager_t *) calloc(1, sizeof(ProjectileManager_t));

    if (manager == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for a projectile manager!\n");
        exit(EXIT_FAILURE);
    }

    return manager;
}

void projectile_manager_tick(ProjectileManager_t *projectile_manager, Player_t *player, EntityManager_t *entity_manager, float time_delta, ParticleManager_t *particle_manager)
{
    for (entity_id i = 0; i < MAX_PROJECTILES; i++)
    {
        if (projectile_manager->used_slots[i])
            projectile_manager->projectiles[i].tick(player, entity_manager, projectile_manager, &projectile_manager->projectiles[i], time_delta, particle_manager);
    }
}

void projectile_manager_render(ProjectileManager_t *projectile_manager, SDL_Renderer *renderer, float cam_x, float cam_y)
{
    for (entity_id i = 0; i < MAX_PROJECTILES; i++)
    {
        if (projectile_manager->used_slots[i])
        {
            Projectile_t *p = &projectile_manager->projectiles[i];
            SDL_Texture *sprite = p->sprite;

            static const float w = 64, h = 64;
            SDL_FRect rect = {
                    p->x - w / 2 - cam_x,
                    p->y - h / 2 - cam_y,
                    w,
                    h
            };

            const SDL_FPoint center = { w / 2, h / 2 };
            SDL_RenderCopyExF(renderer, sprite, NULL, &rect, (p->angle + PI / 4) / (2 * PI) * 360, &center, 0);
        }
    }
}

void projectile_manager_free(ProjectileManager_t *projectile_manager)
{
    if (projectile_manager == NULL)
        return;

    for (entity_id i = 0; i < MAX_PROJECTILES; i++)
    {
        if (projectile_manager->used_slots[i])
            projectile_destroy(projectile_manager, i);
    }

    free(projectile_manager);
}

float projectile_dist(Projectile_t *projectile, float x, float y)
{
    return hypotf(projectile->x - x, projectile->y - y);
}

float projectile_get_x(const Projectile_t *projectile)
{
    return projectile->x;
}

float projectile_get_y(const Projectile_t *projectile)
{
    return projectile->y;
}

entity_id projectile_get_id(const Projectile_t *projectile)
{
    return projectile->id;
}

float projectile_get_velocity(const Projectile_t *projectile)
{
    return projectile->velocity;
}

void projectile_set_x(Projectile_t *projectile, float x)
{
    projectile->x = x;
}

void projectile_set_y(Projectile_t *projectile, float y)
{
    projectile->y = y;
}

void projectile_set_velocity(Projectile_t *projectile, float velocity)
{
    projectile->velocity = velocity;
}

float projectile_get_lifetime(const Projectile_t *projectile)
{
    return projectile->lifetime;
}

void projectile_age(Projectile_t *projectile, float age)
{
    projectile->lifetime -= age;
}

float projectile_get_angle(const Projectile_t *projectile)
{
    return projectile->angle;
}

void projectile_set_angle(Projectile_t *projectile, float angle)
{
    projectile->angle = angle;
}

bool projectile_is_friendly(const Projectile_t *projectile)
{
    return projectile->friendly;
}

float projectile_get_damage(const Projectile_t *projectile)
{
    return projectile->damage;
}

bool projectile_create(ProjectileManager_t *projectile_manager, float x, float y, float angle, const ProjectileTemplate_t *projectile_template)
{
    /**
     * This isn't particularly efficient either...
     * */
    for (projectile_id i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!projectile_manager->used_slots[i])
        {
            Projectile_t *p = &projectile_manager->projectiles[i];

            p->id = i;
            p->x = x;
            p->y = y;
            p->angle = angle;
            p->type = projectile_template->type;
            p->tick = projectile_template->tick_func;
            p->lifetime = projectile_template->lifetime;
            p->velocity = projectile_template->initial_velocity;
            p->friendly = projectile_template->friendly;
            p->damage = projectile_template->damage;
            p->sprite = projectile_template->sprite;
            p->hit_sound_timer = 0;

            projectile_manager->used_slots[i] = true;
            return true;
        }
    }

    return false;
}

void projectile_set_hit_sound_timer(Projectile_t *projectile, float timer)
{
    projectile->hit_sound_timer = timer;
}

float projectile_get_hit_sound_timer(const Projectile_t *projectile)
{
    return projectile->hit_sound_timer;
}

bool projectile_destroy(ProjectileManager_t *projectile_manager, projectile_id id)
{
    projectile_manager->used_slots[id] = false;
    memset(&projectile_manager->projectiles[id], 0, sizeof(Projectile_t));
}


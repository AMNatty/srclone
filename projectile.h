//
// Created by michal on 03.08.20.
//

#ifndef SR_CLONE_PROJECTILE_H
#define SR_CLONE_PROJECTILE_H

#include "defs.h"
#include "player.h"
#include "entity.h"

typedef enum ProjectileType {
    PT_HEAT_STAR = 'A',
    PT_PLASMA_DISC = 'B',
    PT_LASER_BEAM = 'C',
    PT_POLY_SWARM = 'D',
    PT_ELECTRON_FLARE = 'E',
    PT_TACHYON_DISC = 'F'
} ProjectileType_t;

#define PROJECTILE_INFINITY ULONG_MAX
#define MAX_PROJECTILE_TYPES 256

typedef unsigned int projectile_id;
#define MAX_PROJECTILES ((projectile_id) 256)

typedef struct Projectile Projectile_t;

typedef struct ProjectileManager ProjectileManager_t;

typedef void (*projectile_tick_function)(Player_t *, EntityManager_t *, ProjectileManager_t *, Projectile_t *, float, ParticleManager_t *);

typedef struct ProjectileTemplate {
    ProjectileType_t type;
    float initial_velocity;
    float lifetime; // Lifetime in seconds
    float cooldown;
    float damage;
    SDL_Texture *sprite;
    projectile_tick_function tick_func;
    bool friendly;
} ProjectileTemplate_t;

ProjectileManager_t *projectile_manager_create();
void projectile_manager_tick(ProjectileManager_t *projectile_manager, Player_t *player, EntityManager_t *entity_manager, float time_delta, ParticleManager_t *particle_manager);
void projectile_manager_render(ProjectileManager_t *projectile_manager, SDL_Renderer *renderer, float cam_x, float cam_y);
void projectile_manager_free(ProjectileManager_t *projectile_manager);

float projectile_dist(Projectile_t *projectile, float x, float y);
float projectile_get_x(const Projectile_t *projectile);
float projectile_get_y(const Projectile_t *projectile);
float projectile_get_velocity(const Projectile_t *projectile);
float projectile_get_lifetime(const Projectile_t *projectile);
float projectile_get_angle(const Projectile_t *projectile);
bool projectile_is_friendly(const Projectile_t *projectile);
float projectile_get_damage(const Projectile_t *projectile);
void projectile_set_x(Projectile_t *projectile, float x);
void projectile_set_y(Projectile_t *projectile, float y);
void projectile_set_velocity(Projectile_t *projectile, float velocity);
void projectile_set_angle(Projectile_t *projectile, float angle);
void projectile_set_hit_sound_timer(Projectile_t *projectile, float timer);
float projectile_get_hit_sound_timer(const Projectile_t *projectile);
void projectile_age(Projectile_t *projectile, float age);
entity_id projectile_get_id(const Projectile_t *projectile);

bool projectile_create(ProjectileManager_t *projectile_manager, float x, float y, float angle, const ProjectileTemplate_t *projectile_template);
bool projectile_destroy(ProjectileManager_t *projectile_manager, projectile_id id);


#endif //SR_CLONE_PROJECTILE_H

//
// Created by Natty on 03.08.20.
//

#ifndef SR_CLONE_ENTITY_H
#define SR_CLONE_ENTITY_H

#include <SDL2/SDL_render.h>

#include "defs.h"
#include "mtwister.h"
#include "particle.h"
#include "player.h"

typedef unsigned int entity_id;

struct ProjectileManager;

typedef struct Entity Entity_t;

typedef struct EntityManager EntityManager_t;

typedef void (*entity_tick_function)(Player_t*, EntityManager_t*, struct ProjectileManager* projectile_manager, Entity_t*, float, MTRand*, ParticleManager_t*);
typedef void (*entity_render_function)(SDL_Renderer*, Entity_t*, float, float);
typedef void (*entity_initializer)(Entity_t*);

typedef enum EntityClass {
    ENTITY_LOOT = 0b00000001u,
    ENTITY_NEUTRAL = 0b00000010u,
    ENTITY_ENEMY = 0b00000100u
} EntityClass_t;

#define ALL_ENTITY_CLASSES ((EntityClass_t) UINT_MAX)

typedef struct EntityTemplate {
    EntityClass_t entity_class;
    size_t entity_data_size;
    entity_tick_function tick_func;
    entity_render_function renderer;
    entity_initializer initializer;
    float max_health;
    float size;
} EntityTemplate_t;

typedef void* entity_data;

EntityManager_t* en_create_manager();
void en_tick(EntityManager_t* manager, struct ProjectileManager* projectile_manager, Player_t* player, float delta_time, MTRand* rand, ParticleManager_t* particle_manager);
void en_render(EntityManager_t* manager, SDL_Renderer* renderer, float cam_x, float cam_y);
Entity_t* en_find_entity(EntityManager_t* manager, float x, float y, float search_range, EntityClass_t entity_type_filter, bool nearest);
void en_free_manager(EntityManager_t* manager);

float en_get_max_health(const Entity_t* entity);
float en_get_health(const Entity_t* entity);
float en_dist(const Entity_t* entity, float x, float y);
float en_get_x(const Entity_t* entity);
float en_get_y(const Entity_t* entity);
float en_get_rotation(const Entity_t* entity);
entity_id en_get_id(const Entity_t* entity);
void en_set_x(Entity_t* entity, float x);
void en_set_y(Entity_t* entity, float y);
void en_set_rotation(Entity_t* entity, float rot);
void en_apply_damage(Entity_t* entity, float damage);

void* en_get_data(Entity_t* entity);

Entity_t* en_create_entity(EntityManager_t* manager, float x, float y, const EntityTemplate_t* entity_template);
bool en_destroy_entity(EntityManager_t* manager, entity_id id);

void en_tick_do_nothing(Player_t* player, EntityManager_t* manager, Entity_t* entity, float delta_time);

#endif //SR_CLONE_ENTITY_H

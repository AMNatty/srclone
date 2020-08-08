//
// Created by michal on 03.08.20.
//

#include "entity.h"
#include "projectile.h"

#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159f

struct Entity
{
    entity_id id;

    EntityClass_t entity_class;

    float x;
    float y;
    float rotation;

    float health;
    float max_health;

    float size;

    entity_tick_function tick;
    entity_render_function renderer;

    entity_data data;
};

#define MAX_ENTITIES ((entity_id) 256)

struct EntityManager
{
    Entity_t *entities[MAX_ENTITIES];
};

EntityManager_t * en_create_manager()
{
    EntityManager_t *manager = (EntityManager_t *) calloc(1, sizeof(EntityManager_t));

    if (manager == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for an entity manager!\n");
        exit(EXIT_FAILURE);
    }

    return manager;
}

void en_tick(EntityManager_t *manager, struct ProjectileManager *projectile_manager, Player_t *player, float delta_time, MTRand *rand, ParticleManager_t *particle_manager)
{
    for (entity_id i = 0; i < MAX_ENTITIES; i++)
    {
        Entity_t *entity = manager->entities[i];

        if (entity != NULL)
            entity->tick(player, manager, projectile_manager, entity, delta_time, rand, particle_manager);
    }
}

void en_render(EntityManager_t *manager, SDL_Renderer *renderer, float cam_x, float cam_y)
{
    for (entity_id i = 0; i < MAX_ENTITIES; i++)
    {
        Entity_t *entity = manager->entities[i];

        if (entity != NULL)
            entity->renderer(renderer, entity, cam_x, cam_y);
    }
}

void en_free_manager(EntityManager_t *manager)
{
    if (manager == NULL)
        return;

    for (entity_id i = 0; i < MAX_ENTITIES; i++)
    {
        Entity_t *entity = manager->entities[i];

        if (entity != NULL)
            en_destroy_entity(manager, i);
    }

    free(manager);
}

Entity_t *en_create_entity(EntityManager_t *manager, float x, float y, const EntityTemplate_t *template)
{
    /**
     * This isn't particularly efficient, but whatever.
     * */
    for (entity_id i = 0; i < MAX_ENTITIES; i++)
    {
        Entity_t *entity = manager->entities[i];

        if (entity == NULL)
        {
            Entity_t *e = (Entity_t *) calloc(1, sizeof(Entity_t));

            if (e == NULL)
            {
                fprintf(stderr, "Failed to allocate memory for an entity!");
                return NULL;
            }

            e->id = i;
            e->x = x;
            e->y = y;
            e->renderer = template->renderer;
            e->tick = template->tick_func;
            e->max_health = template->max_health;
            e->size = template->size;
            e->health = e->max_health;
            e->rotation = 0;
            e->entity_class = template->entity_class;
            e->data = calloc(1, template->entity_data_size);

            if (e->data == NULL)
            {
                fprintf(stderr, "Failed to allocate memory for an entity data!");
                free(e);
                return NULL;
            }

            if (template->initializer != NULL)
                template->initializer(e);

            manager->entities[i] = e;

            return e;
        }
    }

    return NULL;
}


bool en_destroy_entity(EntityManager_t *manager, entity_id id)
{
    Entity_t *entity = manager->entities[id];

    if (entity == NULL)
        return false;

    free(entity->data);
    free(entity);

    manager->entities[id] = NULL;
}

Entity_t *en_find_entity(EntityManager_t *manager, float x, float y, float search_range, EntityClass_t entity_type_filter, bool nearest)
{
    /**
     * TODO: Optimize this
     * */

    Entity_t *nearest_entity = NULL;
    float nearest_dist = INFINITY;

    for (entity_id i = 0; i < MAX_ENTITIES; i++)
    {
        Entity_t *entity = manager->entities[i];

        if (entity != NULL)
        {
            if (entity->entity_class & entity_type_filter)
            {
                float dist = en_dist(entity, x, y) - entity->size / 2;

                if (dist < search_range)
                {
                    if (nearest)
                    {
                        if (nearest_dist > dist)
                        {
                            nearest_dist = dist;
                            nearest_entity = entity;
                        }
                    }
                    else
                    {
                        return entity;
                    }
                }
            }
        }
    }

    return nearest_entity;
}

float en_get_health(const Entity_t *entity)
{
    return entity->health;
}

float en_dist(const Entity_t *entity, float x, float y)
{
    return hypotf(entity->x - x, entity->y - y);
}

float en_get_x(const Entity_t *entity)
{
    return entity->x;
}

float en_get_y(const Entity_t *entity)
{
    return entity->y;
}

entity_id en_get_id(const Entity_t *entity)
{
    return entity->id;
}

void en_set_x(Entity_t *entity, float x)
{
    entity->x = x;
}

void en_set_y(Entity_t *entity, float y)
{
    entity->y = y;
}

float en_get_rotation(const Entity_t *entity)
{
    return entity->rotation;
}

void en_set_rotation(Entity_t *entity, float rot)
{
    entity->rotation = fmodf(fmodf(rot, 2 * PI) + 2 * PI, 2 * PI);
}

void *en_get_data(Entity_t *entity)
{
    return entity->data;
}

float en_get_max_health(const Entity_t *entity)
{
    return entity->max_health;
}

void en_apply_damage(Entity_t *entity, float damage)
{
    entity->health = fminf(entity->health - damage, entity->max_health);
}

void en_tick_do_nothing(Player_t *player, EntityManager_t *manager, Entity_t *entity, float delta_time)
{

}

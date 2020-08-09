//
// Created by michal on 03.08.20.
//

#include <time.h>

#include "mtwister.h"
#include "base-entities-impl.h"
#include "directional-texture.h"
#include "base-projectiles-impl.h"
#include "projectile.h"
#include "sfx.h"


EntityTemplate_t et_box;

EntityTemplate_t et_enemy_scout;
EntityTemplate_t et_enemy_small_bomber;

DirectionalTexture_t *et_box_texture;
DirectionalTexture_t *et_scout_texture;
DirectionalTexture_t *et_small_bomber_texture;

struct BoxData {
    float lifetime;
};

static void et_box_initializer(Entity_t *entity)
{
    struct BoxData *box_data = (struct BoxData *) en_get_data(entity);
    box_data->lifetime = 20;
}

static void en_tick_box(Player_t *player, EntityManager_t *manager, ProjectileManager_t *projectile_manager, Entity_t *entity, float delta_time, MTRand *rand, ParticleManager_t *particle_manager)
{
    struct BoxData *box_data = (struct BoxData *) en_get_data(entity);

    const float PICKUP_RANGE = 60;

    box_data->lifetime -= delta_time;

    if (box_data->lifetime < 0)
    {
        en_destroy_entity(manager, en_get_id(entity));
        return;
    }

    float player_x = player_get_x(player);
    float player_y = player_get_y(player);

    if (en_dist(entity, player_x, player_y) < PICKUP_RANGE)
    {
        en_destroy_entity(manager, en_get_id(entity));

        double heal_chance = genRand(rand);
        double r = genRand(rand);
        double mul = genRand(rand);

        static struct ParticleDataFloatingText loot_text;
        loot_text.size = 2.0f;

        if (heal_chance < 0.2 && player_get_health(player) < player_get_max_health(player))
        {
            float heal_amount = 15.0f;
            sprintf(loot_text.text, "+%.0fh", heal_amount);
            player_take_damage(player, -heal_amount);
        }
        else
        {
            ProjectileTemplate_t *ammo;
            unsigned long count;

            if (r > 0.95)
            {
                ammo = &pt_tachyon_disc;
                count = (unsigned long) (5 + 3 * mul);
            }
            else if (r > 0.85)
            {
                ammo = &pt_electron_flare;
                count = (unsigned long) (4 + 2 * mul);
            }
            else if (r > 0.70)
            {
                ammo = &pt_plasma_disc;
                count = (unsigned long) (3 + 3 * mul);
            }
            else if (r > 0.40)
            {
                ammo = &pt_heat_star;
                count = (unsigned long) (20 + 20 * mul);
            }
            else
            {
                ammo = &pt_poly_swarm;
                count = (unsigned long) (100 + 50 * mul);
            }

            player_add_ammo(player, ammo, count);
            sprintf(loot_text.text, "+%lu%c", count, ammo->type);
        }

        particle_create(particle_manager, &par_floating_text, player_x, player_y, 0, (const char *) &loot_text);
    }
}

static void en_render_box(SDL_Renderer * renderer, Entity_t *entity, float cam_x, float cam_y)
{
    const unsigned int animation_steps_per_sec = 8;
    const unsigned int time = animation_steps_per_sec * SDL_GetTicks() / 1000;
    unsigned int dir = time % et_box_texture->directions;
    const float size = 128;
    SDL_FRect rect = { en_get_x(entity) - cam_x - size / 2, en_get_y(entity) - cam_y - size / 2, size, size};
    SDL_RenderCopyF(renderer, et_box_texture->textures[dir], NULL, &rect);
}

struct EnemyData {
    DirectionalTexture_t *texture;
    float box_drop_chance;
    float size;
    float ammo_cooldown;
    float speed;
    float movement_change_timer;
    float target_angle;
    float vx;
    float vy;
    ProjectileTemplate_t *ammo;
};

static void et_scout_initializer(Entity_t *entity)
{
    struct EnemyData *enemy_data = (struct EnemyData *) en_get_data(entity);
    enemy_data->texture = et_scout_texture;
    enemy_data->box_drop_chance = 0.05f;
    enemy_data->size = 128;
    enemy_data->speed = 90;
    enemy_data->ammo = &pt_enemy_laser_beam;
    enemy_data->ammo_cooldown = enemy_data->ammo->cooldown * 10;
}

static void et_small_bomber_initializer(Entity_t *entity)
{
    struct EnemyData *enemy_data = (struct EnemyData *) en_get_data(entity);
    enemy_data->texture = et_small_bomber_texture;
    enemy_data->box_drop_chance = 0.20f;
    enemy_data->size = 128;
    enemy_data->speed = 180;
    enemy_data->ammo = &pt_enemy_heat_star;
    enemy_data->ammo_cooldown = enemy_data->ammo->cooldown * 2;
}

static void en_generic_enemy_check_death(Player_t *player, EntityManager_t *manager, ProjectileManager_t *projectile_manager, Entity_t *entity, float delta_time, MTRand *rand, ParticleManager_t *particle_manager)
{
    struct EnemyData *enemy_data = (struct EnemyData *) en_get_data(entity);
    float x = en_get_x(entity);
    float y = en_get_y(entity);

    if (en_get_health(entity) <= 0)
    {
        float r = (float) genRand(rand);

        if (r > 1 - enemy_data->box_drop_chance)
            en_create_entity(manager, x, y, &et_box);

        particle_create(particle_manager, &par_explosion1, x, y, 0, NULL);

        player_add_score(player, (score_t) en_get_max_health(entity));

        en_destroy_entity(manager, en_get_id(entity));

        Mix_PlayChannel(-1, sfx_get_variation(sfx_explosion, SFX_EXPLOSION_VARIATIONS, rand), 0);
    }
}

static void en_generic_enemy_shoot(Player_t *player, EntityManager_t *manager, ProjectileManager_t *projectile_manager, Entity_t *entity, float delta_time, MTRand *rand, bool should_shoot)
{
    struct EnemyData *enemy_data = (struct EnemyData *) en_get_data(entity);
    float x = en_get_x(entity);
    float y = en_get_y(entity);
    float angle = en_get_rotation(entity);

    enemy_data->ammo_cooldown = fmaxf(enemy_data->ammo_cooldown - delta_time, 0);

    if (enemy_data->ammo_cooldown <= 0 && should_shoot)
    {
        projectile_create(projectile_manager, x, y, -angle + PI / 2, enemy_data->ammo);

        enemy_data->ammo_cooldown += enemy_data->ammo->cooldown;
    }
}

static float angle_dist(float dest, float src)
{
    float dist = fmodf(dest - src, 2 * PI);

    if (dist > PI)
        dist -= 2 * PI;

    if (dist < -PI)
        dist += 2 * PI;

    return dist;
}

static void en_generic_enemy_movement(Player_t *player, EntityManager_t *manager, ProjectileManager_t *projectile_manager, Entity_t *entity, float delta_time, MTRand *rand, float precision, float flee_range, float turning_rate, float movement_timer)
{
    struct EnemyData *enemy_data = (struct EnemyData *) en_get_data(entity);

    float x = en_get_x(entity);
    float y = en_get_y(entity);

    enemy_data->movement_change_timer -= delta_time;

    float player_x = player_get_x(player);
    float player_y = player_get_y(player);

    float dx = player_x - x;
    float dy = player_y - y;

    if (enemy_data->movement_change_timer <= 0)
    {
        float general_angle = atan2f(dx, dy);
        enemy_data->target_angle = general_angle + precision * ((float) genRand(rand) - 0.5f);

        enemy_data->movement_change_timer += movement_timer * (1 + (float) genRand(rand) * 2);
    }

    if (en_dist(entity, player_x, player_y) < flee_range)
        enemy_data->target_angle = atan2f(dx, dy) - PI;

    float current_angle = en_get_rotation(entity);
    float error_correction = angle_dist(enemy_data->target_angle, current_angle);

    float clamped_error_correction = fmaxf(-turning_rate * delta_time, fminf(turning_rate * delta_time, error_correction));
    float angle = current_angle + clamped_error_correction;

    en_set_rotation(entity, angle);

    enemy_data->vx = cosf(-angle + PI / 2.0f) * enemy_data->speed * delta_time;
    enemy_data->vy = sinf(-angle + PI / 2.0f) * enemy_data->speed * delta_time;

    en_set_x(entity, x + enemy_data->vx);
    en_set_y(entity, y + enemy_data->vy);
}

static void en_tick_enemy_scout(Player_t *player, EntityManager_t *manager, ProjectileManager_t *projectile_manager, Entity_t *entity, float delta_time, MTRand *rand, ParticleManager_t *particle_manager)
{
    en_generic_enemy_movement(player, manager, projectile_manager, entity, delta_time, rand, PI / 4, 200, PI / 2, 0.5f);

    en_generic_enemy_shoot(player, manager, projectile_manager, entity, delta_time, rand, true);

    en_generic_enemy_check_death(player, manager, projectile_manager, entity, delta_time, rand, particle_manager);
}

static void en_tick_enemy_small_bomber(Player_t *player, EntityManager_t *manager, ProjectileManager_t *projectile_manager, Entity_t *entity, float delta_time, MTRand *rand, ParticleManager_t *particle_manager)
{
    en_generic_enemy_movement(player, manager, projectile_manager, entity, delta_time, rand, PI / 90, 350, PI, 0.3f);

    struct EnemyData *enemy_data = (struct EnemyData *) en_get_data(entity);

    float fire_angle = PI / 22.5; // 8°
    float rot = en_get_rotation(entity);

    float dx = player_get_x(player) - en_get_x(entity);
    float dy = player_get_y(player) - en_get_y(entity);

    float player_angle = atan2f(dx, dy);

    en_generic_enemy_shoot(player, manager, projectile_manager, entity, delta_time, rand, fabsf(angle_dist(player_angle, rot)) < fire_angle);

    en_generic_enemy_check_death(player, manager, projectile_manager, entity, delta_time, rand, particle_manager);
}

static void en_render_enemy_generic(SDL_Renderer * renderer, Entity_t *entity, float cam_x, float cam_y)
{
    struct EnemyData *enemy_data = (struct EnemyData *) en_get_data(entity);

    const float size = enemy_data->size;
    SDL_FRect rect = { en_get_x(entity) - cam_x - size / 2, en_get_y(entity) - cam_y - size / 2, size, size};
    unsigned int dir = (unsigned int) roundf((float) enemy_data->texture->directions * en_get_rotation(entity) / (2 * PI)) % enemy_data->texture->directions;
    SDL_RenderCopyF(renderer, enemy_data->texture->textures[dir], NULL, &rect);

    float en_health = en_get_health(entity);
    float en_maxhealth = en_get_max_health(entity);

    if (en_health < en_maxhealth && en_health > 0)
    {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        rect.x += size / 4.0f;
        rect.y += size;
        rect.h = 5;
        rect.w /= 2.0f;
        SDL_RenderFillRectF(renderer, &rect);
        rect.w *= en_health / en_maxhealth;
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderFillRectF(renderer, &rect);
    }
}


void et_init(SDL_Renderer *renderer)
{
    et_box.max_health = INFINITY;
    et_box.entity_data_size = sizeof(struct BoxData);
    et_box.tick_func = en_tick_box;
    et_box.renderer = en_render_box;
    et_box.initializer = et_box_initializer;
    et_box.size = 48;
    et_box.entity_class = ENTITY_LOOT;
    et_box_texture = dt_create(renderer, 16, "data/textures/box");

    et_enemy_scout.max_health = 50;
    et_enemy_scout.entity_data_size = sizeof(struct EnemyData);
    et_enemy_scout.tick_func = en_tick_enemy_scout;
    et_enemy_scout.renderer = en_render_enemy_generic;
    et_enemy_scout.initializer = et_scout_initializer;
    et_enemy_scout.size = 64;
    et_enemy_scout.entity_class = ENTITY_ENEMY;
    et_scout_texture = dt_create(renderer, 16, "data/textures/e_scout");

    et_enemy_small_bomber.max_health = 150;
    et_enemy_small_bomber.entity_data_size = sizeof(struct EnemyData);
    et_enemy_small_bomber.tick_func = en_tick_enemy_small_bomber;
    et_enemy_small_bomber.renderer = en_render_enemy_generic;
    et_enemy_small_bomber.initializer = et_small_bomber_initializer;
    et_enemy_small_bomber.size = 64;
    et_enemy_small_bomber.entity_class = ENTITY_ENEMY;
    et_small_bomber_texture = dt_create(renderer, 16, "data/textures/e_small_bomber");
}

void et_free()
{
    dt_free(et_box_texture);
    dt_free(et_scout_texture);
    dt_free(et_small_bomber_texture);
}
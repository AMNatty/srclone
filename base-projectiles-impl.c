//
// Created by michal on 03.08.20.
//

#include <SDL2/SDL_image.h>

#include "base-projectiles-impl.h"
#include "sfx.h"

ProjectileTemplate_t pt_heat_star;
ProjectileTemplate_t pt_laser_beam;
ProjectileTemplate_t pt_electron_flare;
ProjectileTemplate_t pt_poly_swarm;
ProjectileTemplate_t pt_plasma_disc;
ProjectileTemplate_t pt_tachyon_disc;

ProjectileTemplate_t pt_enemy_laser_beam;
ProjectileTemplate_t pt_enemy_heat_star;

static void pt_shared_movement(ProjectileManager_t *projectile_manager, Projectile_t *projectile, float delta_time, float movement_retained)
{
    float px = projectile_get_x(projectile);
    float py = projectile_get_y(projectile);

    float theta = projectile_get_angle(projectile);
    float v = projectile_get_velocity(projectile);
    float vd = v * delta_time;
    float vx = vd * cosf(theta);
    float vy = vd * sinf(theta);

    // Slight slowdown of projectiles
    projectile_set_velocity(projectile, v * powf(movement_retained, delta_time));

    projectile_set_x(projectile, px + vx);
    projectile_set_y(projectile, py + vy);

    projectile_age(projectile, delta_time);
}

static void pt_shared_hitcheck(Player_t *player, EntityManager_t *entity_manager, ParticleManager_t *particle_manager, ProjectileManager_t *projectile_manager, Projectile_t *projectile, float delta_time, float age_on_hit, float damage, float projectile_size)
{
    float hit_sound_timer = projectile_get_hit_sound_timer(projectile);
    hit_sound_timer = fmaxf(hit_sound_timer - delta_time, 0);

    float px = projectile_get_x(projectile);
    float py = projectile_get_y(projectile);
    float rot = projectile_get_angle(projectile);

    if (projectile_is_friendly(projectile))
    {
        Entity_t *hit = en_find_entity(entity_manager, px, py, projectile_size / 2, ENTITY_ENEMY | ENTITY_NEUTRAL, false);

        if (hit)
        {
            if (hit_sound_timer == 0)
            {
                Mix_PlayChannel(-1, sfx_hit, 0);
                hit_sound_timer = PROJECTILE_HIT_TICK_LIMIT;
                particle_create(particle_manager, &par_hit, px, py, rot - 1.75f * PI, NULL);
            }

            projectile_age(projectile, age_on_hit);
            en_apply_damage(hit, damage);
        }
    }
    else
    {
        float player_distance = player_dist(player, px, py);

        if (player_distance < projectile_size / 2)
        {
            if (hit_sound_timer == 0)
            {
                Mix_PlayChannel(-1, sfx_hit, 0);
                hit_sound_timer = PROJECTILE_HIT_TICK_LIMIT;
                particle_create(particle_manager, &par_hit, px, py, rot - 1.75f * PI, NULL);
            }

            projectile_age(projectile, age_on_hit);
            player_take_damage(player, damage);
        }
    }

    projectile_set_hit_sound_timer(projectile, hit_sound_timer);

    if (projectile_get_lifetime(projectile) < 0)
    {
        projectile_destroy(projectile_manager, projectile_get_id(projectile));
    }
}


static void pt_tick_basic(Player_t *player, EntityManager_t *entity_manager, ProjectileManager_t *projectile_manager, Projectile_t *projectile, float delta_time, ParticleManager_t *particle_manager)
{
    pt_shared_movement(projectile_manager, projectile, delta_time, 0.95f);

    float age_on_hit = 4;
    float projectile_size = 24;
    float damage = projectile_get_damage(projectile);

    pt_shared_hitcheck(player, entity_manager, particle_manager, projectile_manager, projectile, delta_time, age_on_hit, damage, projectile_size);
}

static void pt_tick_enemy_basic(Player_t *player, EntityManager_t *entity_manager, ProjectileManager_t *projectile_manager, Projectile_t *projectile, float delta_time, ParticleManager_t *particle_manager)
{
    pt_shared_movement(projectile_manager, projectile, delta_time, 0.95f);

    float age_on_hit = 999;
    float projectile_size = 22;
    float damage = projectile_get_damage(projectile);

    pt_shared_hitcheck(player, entity_manager, particle_manager, projectile_manager, projectile, delta_time, age_on_hit, damage, projectile_size);
}

static void pt_autoseek_enemy(EntityManager_t *entity_manager, Projectile_t *projectile, float delta_time, float steering_rate)
{
    float px = projectile_get_x(projectile);
    float py = projectile_get_y(projectile);
    float theta = projectile_get_angle(projectile);

    Entity_t *seek = en_find_entity(entity_manager, px, py, INFINITY, ENTITY_ENEMY | ENTITY_NEUTRAL, true);

    if (seek)
    {
        float ex = en_get_x(seek);
        float ey = en_get_y(seek);

        float dx = ex - px;
        float dy = ey - py;

        float target_angle = atan2f(dy, dx);

        float error_correction = target_angle - theta;

        if (error_correction > PI)
            error_correction -= 2 * PI;
        if (error_correction < -PI)
            error_correction += 2 * PI;

        float clamped_error_correction = fmaxf(-steering_rate * delta_time, fminf(steering_rate * delta_time, error_correction));
        float angle = theta + clamped_error_correction;

        projectile_set_angle(projectile, angle);
    }
    else
    {
        projectile_set_angle(projectile, projectile_get_angle(projectile) + steering_rate / 2.0f * delta_time);
    }
}

static void pt_tick_swarm(Player_t *player, EntityManager_t *entity_manager, ProjectileManager_t *projectile_manager, Projectile_t *projectile, float delta_time, ParticleManager_t *particle_manager)
{
    pt_shared_movement(projectile_manager, projectile, delta_time, 1);

    float age_on_hit = 10;
    float projectile_size = 24;
    float damage = projectile_get_damage(projectile);
    float steering_rate = 180.0f / 360.0f * 2 * PI;

    pt_autoseek_enemy(entity_manager, projectile, delta_time, steering_rate);

    pt_shared_hitcheck(player, entity_manager, particle_manager, projectile_manager, projectile, delta_time, age_on_hit, damage, projectile_size);
}

static void pt_tick_disc(Player_t *player, EntityManager_t *entity_manager, ProjectileManager_t *projectile_manager, Projectile_t *projectile, float delta_time, float movement_retained, ParticleManager_t *particle_manager)
{

    pt_shared_movement(projectile_manager, projectile, delta_time, movement_retained);

    float age_on_hit = 0;
    float projectile_size = 24;
    float damage = projectile_get_damage(projectile) * delta_time * (projectile_get_velocity(projectile) / 100.0f);

    pt_shared_hitcheck(player, entity_manager, particle_manager, projectile_manager, projectile, delta_time, age_on_hit, damage, projectile_size);
}

static void pt_tick_spark(Player_t *player, EntityManager_t *entity_manager, ProjectileManager_t *projectile_manager, Projectile_t *projectile, float delta_time, ParticleManager_t *particle_manager)
{
    pt_shared_movement(projectile_manager, projectile, delta_time, 1);

    float age_on_hit = 0.2f;
    float projectile_size = 24;
    float damage = projectile_get_damage(projectile);
    float steering_rate = 2 * PI;

    pt_autoseek_enemy(entity_manager, projectile, delta_time, steering_rate);

    pt_shared_hitcheck(player, entity_manager, particle_manager, projectile_manager, projectile, delta_time, age_on_hit, damage, projectile_size);
}

static void pt_tick_disc_accel(Player_t *player, EntityManager_t *entity_manager, ProjectileManager_t *projectile_manager, Projectile_t *projectile, float delta_time, ParticleManager_t *particle_manager)
{
    pt_tick_disc(player, entity_manager, projectile_manager, projectile, delta_time, 4.0f, particle_manager);
}

static void pt_tick_disc_rotate(Player_t *player, EntityManager_t *entity_manager, ProjectileManager_t *projectile_manager, Projectile_t *projectile, float delta_time, ParticleManager_t *particle_manager)
{
    float steering_rate = 180.0f / 360.0f * 2 * PI;
    projectile_set_angle(projectile, projectile_get_angle(projectile) + steering_rate * delta_time);
    pt_tick_disc(player, entity_manager, projectile_manager, projectile, delta_time, 0.9f, particle_manager);
}

void pt_init(SDL_Renderer *renderer)
{
    pt_heat_star.initial_velocity = 250;
    pt_heat_star.type = PT_HEAT_STAR;
    pt_heat_star.sprite = IMG_LoadTexture(renderer, "data/textures/particles/heatStar.png");
    pt_heat_star.lifetime =  8;
    pt_heat_star.cooldown = 0.4f;
    pt_heat_star.tick_func = pt_tick_basic;
    pt_heat_star.damage = 800;
    pt_heat_star.friendly = true;

    pt_laser_beam.initial_velocity = 1000;
    pt_laser_beam.type = PT_LASER_BEAM;
    pt_laser_beam.sprite = IMG_LoadTexture(renderer, "data/textures/particles/laserBeam.png");
    pt_laser_beam.lifetime =  4;
    pt_laser_beam.damage = 25;
    pt_laser_beam.cooldown = 0.2f;
    pt_laser_beam.tick_func = pt_tick_basic;
    pt_laser_beam.friendly = true;

    pt_plasma_disc.initial_velocity = 500;
    pt_plasma_disc.type = PT_PLASMA_DISC;
    pt_plasma_disc.sprite = IMG_LoadTexture(renderer, "data/textures/particles/plasmaDisc.png");
    pt_plasma_disc.lifetime =  20;
    pt_plasma_disc.damage = 150;
    pt_plasma_disc.cooldown = 2.5f;
    pt_plasma_disc.tick_func = pt_tick_disc_rotate;
    pt_plasma_disc.friendly = true;

    pt_electron_flare.initial_velocity = 400;
    pt_electron_flare.type = PT_ELECTRON_FLARE;
    pt_electron_flare.sprite = IMG_LoadTexture(renderer, "data/textures/particles/electronFlare.png");
    pt_electron_flare.lifetime = 5;
    pt_electron_flare.damage = 50;
    pt_electron_flare.cooldown = 2.0f;
    pt_electron_flare.tick_func = pt_tick_spark;
    pt_electron_flare.friendly = true;

    pt_poly_swarm.initial_velocity = 300;
    pt_poly_swarm.type = PT_POLY_SWARM;
    pt_poly_swarm.sprite = IMG_LoadTexture(renderer, "data/textures/particles/polySwarm.png");
    pt_poly_swarm.lifetime =  10;
    pt_poly_swarm.damage = 5;
    pt_poly_swarm.cooldown = 0.1f;
    pt_poly_swarm.tick_func = pt_tick_swarm;
    pt_poly_swarm.friendly = true;

    pt_tachyon_disc.initial_velocity = 100;
    pt_tachyon_disc.type = PT_TACHYON_DISC;
    pt_tachyon_disc.sprite = IMG_LoadTexture(renderer, "data/textures/particles/tachyonDisc.png");
    pt_tachyon_disc.lifetime =  10;
    pt_tachyon_disc.damage = 1000;
    pt_tachyon_disc.cooldown = 2.0f;
    pt_tachyon_disc.tick_func = pt_tick_disc_accel;
    pt_tachyon_disc.friendly = true;

    pt_enemy_laser_beam.initial_velocity = 400;
    pt_enemy_laser_beam.type = PT_LASER_BEAM;
    pt_enemy_laser_beam.sprite = IMG_LoadTexture(renderer, "data/textures/particles/enemyLaserBeam.png");
    pt_enemy_laser_beam.lifetime =  4;
    pt_enemy_laser_beam.damage = 5;
    pt_enemy_laser_beam.cooldown = 2.0f;
    pt_enemy_laser_beam.tick_func = pt_tick_enemy_basic;
    pt_enemy_laser_beam.friendly = false;

    pt_enemy_heat_star.initial_velocity = 175;
    pt_enemy_heat_star.type = PT_HEAT_STAR;
    pt_enemy_heat_star.sprite = IMG_LoadTexture(renderer, "data/textures/particles/enemyHeatStar.png");
    pt_enemy_heat_star.lifetime =  4;
    pt_enemy_heat_star.cooldown = 3.0f;
    pt_enemy_heat_star.tick_func = pt_tick_enemy_basic;
    pt_enemy_heat_star.damage = 20;
    pt_enemy_heat_star.friendly = false;
}

void pt_free()
{
    SDL_DestroyTexture(pt_enemy_heat_star.sprite);
    SDL_DestroyTexture(pt_heat_star.sprite);
    SDL_DestroyTexture(pt_laser_beam.sprite);
    SDL_DestroyTexture(pt_plasma_disc.sprite);
    SDL_DestroyTexture(pt_electron_flare.sprite);
    SDL_DestroyTexture(pt_poly_swarm.sprite);
    SDL_DestroyTexture(pt_laser_beam.sprite);
    SDL_DestroyTexture(pt_tachyon_disc.sprite);
}
//
// Created by Natty on 03.08.20.
//

#include <SDL2/SDL_image.h>

#include "base-projectiles-impl.h"
#include "directional-texture.h"
#include "input.h"
#include "player.h"
#include "projectile.h"
#include "sfx.h"

struct Player {
    float x;
    float y;

    float x_speed;
    float y_speed;

    float angle;

    unsigned int direction;

    float shoot_cooldown;

    float health;
    float max_health;

    score_t score;

    bool is_dead;

    unsigned long ammo[MAX_PROJECTILE_TYPES];
    const ProjectileTemplate_t* ammo_templates[MAX_PROJECTILE_TYPES];
    ProjectileType_t active_ammo;

    int engine_sound_channel;
};

DirectionalTexture_t* ship = NULL;
SDL_Texture* flames = NULL;

Player_t* player_create(SDL_Renderer* renderer) {
    Player_t* player = (Player_t*) calloc(1, sizeof(Player_t));

    if (player == NULL) {
        fprintf(stderr, "Failed to allocate a player!");
        exit(EXIT_FAILURE);
    }

    player->engine_sound_channel = -1;

    player->max_health = 100;
    player->health = player->max_health;

    player_add_ammo(player, &pt_laser_beam, PROJECTILE_INFINITY);
    player_add_ammo(player, &pt_heat_star, 20);
    player_add_ammo(player, &pt_plasma_disc, 5);
    player_add_ammo(player, &pt_electron_flare, 5);
    player_add_ammo(player, &pt_poly_swarm, 150);
    player->active_ammo = pt_laser_beam.type;

    player->x_speed = 0;
    player->y_speed = 0;

    return player;
}

static void player_prev_ammo(Player_t* player);
static void player_next_ammo(Player_t* player);

void player_tick(Player_t* player, ProjectileManager_t* projectileManager, float time_delta, ParticleManager_t* particle_manager, MTRand* rand) {
    if (player->health <= 0) {
        particle_create(particle_manager, &par_explosion1, player->x, player->y, 0, NULL);
        player->is_dead = true;

        if (player->engine_sound_channel != -1)
            Mix_HaltChannel(player->engine_sound_channel);

        Mix_PlayChannel(-1, sfx_get_variation(sfx_explosion, SFX_EXPLOSION_VARIATIONS, rand), 0);

        return;
    }

    // Yes, I know there is no friction is space, but it makes the movement smoother
    const float friction = powf(0.05f, time_delta);
    const float max_speed = 150 * time_delta;
    const float accel = 400 * time_delta * time_delta;

    if (GameInput.next_ammo)
        player_next_ammo(player);
    else if (GameInput.prev_ammo)
        player_prev_ammo(player);

    player->x_speed *= friction;
    player->y_speed *= friction;

    if (GameInput.up)
        player->y_speed += -accel;
    else if (GameInput.down)
        player->y_speed += accel;
    if (GameInput.left)
        player->x_speed += -accel;
    else if (GameInput.right)
        player->x_speed += accel;

    player->x_speed = fmaxf(fminf(max_speed, player->x_speed), -max_speed);
    player->y_speed = fmaxf(fminf(max_speed, player->y_speed), -max_speed);

    player->x += player->x_speed;
    player->y += player->y_speed;

    if (GameInput.up || GameInput.down || GameInput.right || GameInput.left) {
        if (player->engine_sound_channel == -1) {
            player->engine_sound_channel = Mix_PlayChannel(-1, sfx_player_engine, -1);
        }
    } else if (player->engine_sound_channel != -1) {
        Mix_HaltChannel(player->engine_sound_channel);
        player->engine_sound_channel = -1;
    }

    const float w = 128, h = 128;

    player->shoot_cooldown -= time_delta;

    const ProjectileTemplate_t* active = player->ammo_templates[player->active_ammo];

    if (active != NULL) {
        if (GameInput.shoot && player->shoot_cooldown <= 0 && player->ammo[active->type] > 0) {
            float angle = -((float) player->direction / (float) ship->directions) * (2 * PI) + PI / 2;
            projectile_create(projectileManager, player->x, player->y, angle, active);
            player->shoot_cooldown += active->cooldown;

            if (player->ammo[active->type] != PROJECTILE_INFINITY)
                player->ammo[active->type]--;

            if (player->ammo[active->type] == 0)
                player_next_ammo(player);
        }
    }

    player->shoot_cooldown = fmaxf(player->shoot_cooldown, 0);

    if (fabsf(player->x_speed) > 0 || fabsf(player->y_speed) > 0)
        player->direction = ((unsigned int) roundf((PI - atan2f(player->x_speed, -player->y_speed)) / (PI * 2) * (float) ship->directions)) % ship->directions;
}

void player_render(const Player_t* player, SDL_Renderer* renderer, float cam_x, float cam_y) {
    const float w = 64, h = 64;
    SDL_FRect rect = {
            player->x - w / 2 - cam_x,
            player->y - h / 2 - cam_y,
            w,
            h};

    SDL_FRect engineRect = rect;
    float angle = 2 * PI * (float) player->direction / (float) ship->directions;
    engineRect.x -= sinf(angle) * 15;
    engineRect.y -= cosf(angle) * 15;
    const SDL_FPoint center = {engineRect.w / 2, engineRect.h / 2};

    if (player->direction >= ship->directions / 4 * 3 || player->direction <= ship->directions / 4) {
        SDL_RenderCopyExF(renderer, flames, NULL, &engineRect, (double) -angle / 2 / (double) PI * 360 + 135, &center, 0);
        SDL_RenderCopyF(renderer, ship->textures[player->direction], NULL, &rect);
    } else {
        SDL_RenderCopyF(renderer, ship->textures[player->direction], NULL, &rect);
        SDL_RenderCopyExF(renderer, flames, NULL, &engineRect, (double) -angle / 2 / (double) PI * 360 + 135, &center, 0);
    }
}

void player_set_max_health(Player_t* player, float max_hp) {
    player->max_health = max_hp;
}

float player_get_max_health(const Player_t* player) {
    return player->max_health;
}

void player_take_damage(Player_t* player, float damage) {
    player->health = fminf(player->health - damage, player->max_health);
}

bool player_is_dead(const Player_t* player) {
    return player->is_dead;
}

void player_set_x(Player_t* player, float x) {
    player->x = x;
}

void player_set_y(Player_t* player, float y) {
    player->y = y;
}

float player_get_x(const Player_t* player) {
    return player->x;
}

float player_get_y(const Player_t* player) {
    return player->y;
}

float player_get_shoot_cooldown(const Player_t* player) {
    return player->shoot_cooldown;
}

void player_set_shoot_cooldown(Player_t* player, float cooldown) {
    player->shoot_cooldown = cooldown;
}


score_t player_get_score(const Player_t* player) {
    return player->score;
}

void player_set_score(Player_t* player, score_t score) {
    player->score = score;
}

void player_add_score(Player_t* player, score_t score) {
    player->score += score;
}

const struct ProjectileTemplate* player_get_selected_ammo(const Player_t* player) {
    return player->ammo_templates[player->active_ammo];
}

unsigned long player_get_ammo_count(const Player_t* player, const struct ProjectileTemplate* pt) {
    return player->ammo[pt->type];
}

void player_add_ammo(Player_t* player, const struct ProjectileTemplate* projectile_template, unsigned long count) {
    if (player->ammo_templates[projectile_template->type] != projectile_template) {
        player->ammo_templates[projectile_template->type] = projectile_template;
        player->ammo[projectile_template->type] = count;
    } else {
        unsigned long* ammo_count = &player->ammo[projectile_template->type];

        if (PROJECTILE_INFINITY - *ammo_count < count) {
            *ammo_count = PROJECTILE_INFINITY;
        } else {
            *ammo_count += count;
        }
    }
}

static void player_prev_ammo(Player_t* player) {
    ProjectileType_t current = player->active_ammo;

    for (int i = 1; i <= MAX_PROJECTILE_TYPES; i++) {
        ProjectileType_t idx = (current + MAX_PROJECTILE_TYPES - i) % MAX_PROJECTILE_TYPES;

        if (player->ammo[idx] > 0) {
            player->active_ammo = idx;
            break;
        }
    }
}

static void player_next_ammo(Player_t* player) {
    ProjectileType_t current = player->active_ammo;

    for (int i = 1; i <= MAX_PROJECTILE_TYPES; i++) {
        ProjectileType_t idx = (current + i) % MAX_PROJECTILE_TYPES;

        if (player->ammo[idx] > 0) {
            player->active_ammo = idx;
            break;
        }
    }
}

float player_get_health(const Player_t* player) {
    return player->health;
}

float player_dist(const Player_t* player, float x, float y) {
    float size = 10;
    return hypotf(player->x - x, player->y - y) - size;
}

void player_free(Player_t* player) {
    if (player == NULL)
        return;

    if (player->engine_sound_channel != 1)
        Mix_HaltChannel(player->engine_sound_channel);

    free(player);
}

void plr_init(SDL_Renderer* renderer) {
    ship = dt_create(renderer, 32, "data/textures/ship");
    const char tex[] = "data/textures/particles/rocketNozzle.png";
    flames = IMG_LoadTexture(renderer, tex);

    if (flames == NULL) {
        fprintf(stderr, "Failed to load the following texture: %s\n", tex);
        exit(EXIT_FAILURE);
    }
}

void plr_free() {
    dt_free(ship);
    SDL_DestroyTexture(flames);
}
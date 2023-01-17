//
// Created by Natty on 03.08.20.
//

#ifndef SR_CLONE_PLAYER_H
#define SR_CLONE_PLAYER_H

#include <SDL2/SDL_render.h>

#include "defs.h"
#include "particle.h"

typedef struct Player Player_t;

struct ProjectileManager;
struct ProjectileTemplate;

typedef unsigned long score_t;

Player_t* player_create(SDL_Renderer* renderer);
void player_tick(Player_t* player, struct ProjectileManager* projectileManager, float time_delta, ParticleManager_t* particle_manager, MTRand* random);
void player_render(const Player_t* player, SDL_Renderer* renderer, float cam_x, float cam_y);
void player_take_damage(Player_t* player, float damage);
float player_get_health(const Player_t* player);
void player_set_max_health(Player_t* player, float max_hp);
float player_get_max_health(const Player_t* player);
bool player_is_dead(const Player_t* player);
void player_set_x(Player_t* player, float x);
void player_set_y(Player_t* player, float y);
float player_get_x(const Player_t* player);
float player_get_y(const Player_t* player);
float player_get_shoot_cooldown(const Player_t* player);
score_t player_get_score(const Player_t* player);
void player_set_score(Player_t* player, score_t score);
void player_add_score(Player_t* player, score_t score);
const struct ProjectileTemplate* player_get_selected_ammo(const Player_t* player);
void player_add_ammo(Player_t* player, const struct ProjectileTemplate* projectile_template, unsigned long count);
unsigned long player_get_ammo_count(const Player_t* player, const struct ProjectileTemplate* projectile_template);
void player_set_shoot_cooldown(Player_t* player, float cooldown);
float player_dist(const Player_t* player, float x, float y);
void player_free(Player_t* player);

void plr_init(SDL_Renderer* renderer);
void plr_free();

#endif//SR_CLONE_PLAYER_H

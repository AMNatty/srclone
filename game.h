//
// Created by michal on 05.08.20.
//

#ifndef SR_CLONE_GAME_H
#define SR_CLONE_GAME_H

#include "entity.h"
#include "projectile.h"

typedef struct Game Game_t;

Game_t *game_create(SDL_Renderer *renderer);
ProjectileManager_t *game_get_projectile_manager(Game_t *game);
EntityManager_t *game_get_entity_manager(Game_t *game);
ParticleManager_t *game_get_particle_manager(Game_t *game);
Player_t *game_get_player(Game_t *game);
MTRand *game_get_random(Game_t *game);
void game_tick(Game_t *game, float delta_time, float visible_area_width, float visible_area_height);
void game_destroy(Game_t *game);

#endif //SR_CLONE_GAME_H

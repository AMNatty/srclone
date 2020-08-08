//
// Created by michal on 03.08.20.
//

#ifndef SR_CLONE_BASE_PROJECTILES_IMPL_H
#define SR_CLONE_BASE_PROJECTILES_IMPL_H

#include "defs.h"
#include "projectile.h"

#define PROJECTILE_HIT_TICK_LIMIT 0.05f

extern ProjectileTemplate_t pt_heat_star;
extern ProjectileTemplate_t pt_laser_beam;
extern ProjectileTemplate_t pt_electron_flare;
extern ProjectileTemplate_t pt_poly_swarm;
extern ProjectileTemplate_t pt_plasma_disc;
extern ProjectileTemplate_t pt_tachyon_disc;

extern ProjectileTemplate_t pt_enemy_laser_beam;
extern ProjectileTemplate_t pt_enemy_heat_star;

void pt_init(SDL_Renderer *renderer);
void pt_free();


#endif //SR_CLONE_BASE_PROJECTILES_IMPL_H

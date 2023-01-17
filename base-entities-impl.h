//
// Created by Natty on 03.08.20.
//

#ifndef SR_CLONE_BASE_ENTITIES_IMPL_H
#define SR_CLONE_BASE_ENTITIES_IMPL_H

#include "defs.h"
#include "entity.h"

extern EntityTemplate_t et_box;

extern EntityTemplate_t et_enemy_scout;
extern EntityTemplate_t et_enemy_small_bomber;

void et_init(SDL_Renderer* renderer);
void et_free();

#endif //SR_CLONE_BASE_ENTITIES_IMPL_H

//
// Created by Natty on 02.08.20.
//

#ifndef SR_CLONE_INPUT_H
#define SR_CLONE_INPUT_H

#include <SDL2/SDL_events.h>

#include "defs.h"

typedef enum KeyEventType {
    KT_RELEASED,
    KT_PRESSED
} KeyEventType_t;

typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;

    bool shoot;

    bool reset;

    bool prev_ammo;
    bool next_ammo;
} GameInput_t;

extern GameInput_t GameInput;

void input_kb_event(SDL_Scancode scancode, KeyEventType_t event);
void input_kb_reset();
void input_kb_clear();

#endif //SR_CLONE_INPUT_H

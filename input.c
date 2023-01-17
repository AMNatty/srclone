//
// Created by Natty on 02.08.20.
//

#include "input.h"

GameInput_t GameInput = {0};

void input_kb_event(SDL_Scancode scancode, KeyEventType_t event) {
    switch (scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP:
            GameInput.up = event == KT_PRESSED;
            break;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            GameInput.left = event == KT_PRESSED;
            break;
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN:
            GameInput.down = event == KT_PRESSED;
            break;
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            GameInput.right = event == KT_PRESSED;
            break;
        case SDL_SCANCODE_SPACE:
            GameInput.shoot = event == KT_PRESSED;
            break;
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_Q:
            GameInput.prev_ammo = event == KT_PRESSED;
            break;
        case SDL_SCANCODE_R:
            GameInput.reset = event == KT_PRESSED;
            break;
        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_E:
            GameInput.next_ammo = event == KT_PRESSED;
            break;

        default:
            break;
    }
}

void input_kb_reset() {
    GameInput.next_ammo = false;
    GameInput.prev_ammo = false;
    GameInput.reset = false;
}

void input_kb_clear() {
    memset(&GameInput, 0, sizeof(GameInput));
}

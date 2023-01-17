#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>
#undef main
#include <SDL2/SDL_image.h>

#include "base-entities-impl.h"
#include "base-projectiles-impl.h"
#include "defs.h"
#include "font-renderer.h"
#include "game.h"
#include "input.h"
#include "particle.h"
#include "player.h"
#include "sfx.h"
#include "stars.h"

int main() {
    const char title[] = "SR Clone";

    printf("Initializing SDL2.\n");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize SDL2! Error: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    printf("Opening a window...\n");

    int window_width = 1280;
    int window_height = 720;

    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_ShowWindow(window);

    if (window == NULL) {
        fprintf(stderr, "Failed to create the window! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        fprintf(stderr, "Failed to create a renderer! Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    sfx_init();

    SDL_Texture* ui_you_died = IMG_LoadTexture(renderer, "data/textures/ui/youDied.png");

    if (ui_you_died == NULL) {
        fprintf(stderr, "Failed to load a UI sprite: data/textures/ui/youDied.png\n");
        return EXIT_FAILURE;
    }

    st_load_star_textures(renderer);
    FontRenderer_t* font_renderer = fr_init(renderer);

    et_init(renderer);
    pt_init(renderer);
    part_init(renderer);
    plr_init(renderer);

    st_gen_stars(0, 8192, 0, 8192);

    Game_t* game = game_create(renderer);

    Player_t* player = game_get_player(game);
    ProjectileManager_t* projectile_manager = game_get_projectile_manager(game);
    EntityManager_t* entity_manager = game_get_entity_manager(game);
    ParticleManager_t* particle_manager = game_get_particle_manager(game);

    SDL_Event event;
    bool shouldQuit = false;

    clock_t lastFrame = clock();
    float last_health = player_get_health(player);
    float hit_overlay = 0;
    float dead_timer = 0;

    if (Mix_PlayMusic(mus_game_st0, -1) == -1) {
        fprintf(stderr, "Failed to play music: %s\n", Mix_GetError());
    }

    while (!shouldQuit) {
        clock_t now = clock();
        float timeDelta = (float) (now - lastFrame) / CLOCKS_PER_SEC;

        if (!SHOULD_SKIP_TICK(timeDelta))
            lastFrame = now;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        /**
         * Entity and player ticks
         * */
        game_tick(game, timeDelta, window_width / 2, window_height / 2);

        /**
         * Do some rendering here
         * */
        float cam_x = player_get_x(player) - (float) window_width / 2.0f, cam_y = player_get_y(player) - (float) window_height / 2.0f;
        st_draw_stars(renderer, cam_x, cam_y, (float) window_width, (float) window_height);
        en_render(entity_manager, renderer, cam_x, cam_y);
        projectile_manager_render(projectile_manager, renderer, cam_x, cam_y);
        particle_manager_render(particle_manager, renderer, font_renderer, cam_x, cam_y);

        if (!player_is_dead(player)) {
            player_render(player, renderer, cam_x, cam_y);
            dead_timer = 0;
        } else if (!SHOULD_SKIP_TICK(timeDelta)) {
            dead_timer += timeDelta;
        }

        if (dead_timer > 1.0f) {
            Mix_FadeOutMusic(500);

            float w = 512;
            float h = 256;
            SDL_FRect ui_game_over_location = {(float) window_width / 2 - w / 2, (float) window_height / 2 - h / 2, w, (float) h};
            SDL_RenderCopyF(renderer, ui_you_died, NULL, &ui_game_over_location);

            if (GameInput.reset) {
                dead_timer = 0;

                game_destroy(game);
                game = game_create(renderer);

                player = game_get_player(game);
                projectile_manager = game_get_projectile_manager(game);
                entity_manager = game_get_entity_manager(game);
                particle_manager = game_get_particle_manager(game);

                if (Mix_FadeInMusic(mus_game_st0, -1, 500) == -1) {
                    fprintf(stderr, "Failed to play music: %s\n", Mix_GetError());
                }
            }
        }

        hit_overlay = fmaxf(hit_overlay - timeDelta, 0);

        float player_hp = player_get_health(player);
        if (player_hp < last_health)
            hit_overlay = 0.1f;
        last_health = player_hp;

        if (hit_overlay > 0) {
            const float cover = 0.025f;
            float w = cover * (float) window_width;
            float h = cover * (float) window_height;
            SDL_FRect hp_penalty_shadow = {0, 0, (float) window_width, (float) h};
            SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
            SDL_RenderFillRectF(renderer, &hp_penalty_shadow);
            hp_penalty_shadow.y = (float) window_height - h;
            SDL_RenderFillRectF(renderer, &hp_penalty_shadow);
            hp_penalty_shadow.x = 0;
            hp_penalty_shadow.y = 0;
            hp_penalty_shadow.w = (float) w;
            hp_penalty_shadow.h = (float) window_height;
            SDL_RenderFillRectF(renderer, &hp_penalty_shadow);
            hp_penalty_shadow.x = (float) window_width - w;
            SDL_RenderFillRectF(renderer, &hp_penalty_shadow);
        }

        char buf[64];
        sprintf(buf, "S %010ld", player_get_score(player));
        fr_draw(renderer, font_renderer, 5, 5, 24, buf);

        const ProjectileTemplate_t* ammo = player_get_selected_ammo(player);
        unsigned long ammo_count = player_get_ammo_count(player, ammo);
        if (ammo_count == PROJECTILE_INFINITY)
            sprintf(buf, "%c x&", ammo->type);
        else if (ammo_count > 999999)
            sprintf(buf, "%c x999999", ammo->type);
        else
            sprintf(buf, "%c x%06ld", ammo->type, ammo_count);
        fr_draw(renderer, font_renderer, 5, (float) window_height - 24 - 48 - 10, 24, buf);

        sprintf(buf, "h %03.0f", player_hp);
        fr_draw(renderer, font_renderer, 5, (float) window_height - 48 - 5, 48, buf);

        SDL_RenderPresent(renderer);

        if (!SHOULD_SKIP_TICK(timeDelta))
            input_kb_reset();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                {
                    SDL_KeyboardEvent* kbEvent = &event.key;

                    if (kbEvent->repeat)
                        break;

                    input_kb_event(kbEvent->keysym.scancode, KT_PRESSED);
                } break;

                case SDL_KEYUP:
                {
                    SDL_KeyboardEvent* kbEvent = &event.key;
                    input_kb_event(kbEvent->keysym.scancode, KT_RELEASED);
                } break;

                case SDL_WINDOWEVENT:
                {
                    SDL_WindowEvent* windowEvent = &event.window;

                    switch (windowEvent->event) {
                        case SDL_WINDOWEVENT_FOCUS_GAINED:
                            Mix_VolumeMusic(VOLUME_MUSIC);
                            Mix_Volume(-1, VOLUME_SFX);
                            break;

                        case SDL_WINDOWEVENT_FOCUS_LOST:
                            Mix_VolumeMusic(0);
                            Mix_Volume(-1, 0);
                            input_kb_clear();
                            break;

                        case SDL_WINDOWEVENT_CLOSE:
                            shouldQuit = true;
                            break;

                        case SDL_WINDOWEVENT_RESIZED:
                            window_width = windowEvent->data1;
                            window_height = windowEvent->data2;

                            printf("Resized to %dx%d\n", window_width, window_height);
                            break;

                        default:
                            break;
                    }
                } break;

                case SDL_QUIT:
                    shouldQuit = true;
                    break;

                default:
                    break;
            }
        }
    }

    fr_free(font_renderer);

    game_destroy(game);

    plr_free();
    part_free();
    pt_free();
    et_free();
    st_free_star_textures();

    SDL_DestroyTexture(ui_you_died);

    sfx_free();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    printf("Exitting...\n");

    return 0;
}

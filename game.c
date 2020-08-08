//
// Created by michal on 05.08.20.
//

#include <time.h>

#include "game.h"
#include "projectile.h"
#include "base-entities-impl.h"
#include "particle.h"


struct Game {
    ProjectileManager_t *projectile_manager;
    EntityManager_t *entity_manager;
    ParticleManager_t * particle_manager;
    Player_t *player;
    MTRand rand;

    float spawn_timer;
    float run_time;
    float difficulty;
};

Game_t *game_create(SDL_Renderer *renderer)
{
    Game_t *game = (Game_t *) calloc(1, sizeof(Game_t));

    if (game == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the game struct.");
        exit(EXIT_FAILURE);
    }


    game->projectile_manager = projectile_manager_create();
    game->entity_manager = en_create_manager();
    game->rand = seedRand(time(NULL));
    game->particle_manager = particle_manager_create(&game->rand);

    Player_t *player = player_create(renderer);
    player_set_x(player, 640);
    player_set_y(player, 360);
    game->player = player;

    game->spawn_timer = 5;

    return game;
}

ProjectileManager_t *game_get_projectile_manager(Game_t *game)
{
    return game->projectile_manager;
}

EntityManager_t *game_get_entity_manager(Game_t *game)
{
    return game->entity_manager;
}

Player_t *game_get_player(Game_t *game)
{
    return game->player;
}

ParticleManager_t *game_get_particle_manager(Game_t *game)
{
    return game->particle_manager;
}

void game_tick(Game_t *game, float delta_time, float visible_area_width, float visible_area_height)
{
    if (!player_is_dead(game->player))
    {
        en_tick(game->entity_manager, game->projectile_manager, game->player, delta_time, &game->rand, game->particle_manager);
        projectile_manager_tick(game->projectile_manager, game->player, game->entity_manager, delta_time, game->particle_manager);
        player_tick(game->player, game->projectile_manager, delta_time, game->particle_manager, &game->rand);
    }

    particle_manager_tick(game->particle_manager, delta_time);


    if (!player_is_dead(game->player))
    {
        game->run_time += delta_time;
        game->difficulty = 1 + powf(game->run_time, 1.5f);

        game->spawn_timer -= delta_time;

        if (game->spawn_timer <= 0)
        {
            const float min_spawn_radius = 200 + hypotf(visible_area_height, visible_area_width);
            const float spawn_timer = 160 / (4 + log10f(game->difficulty));

            const float variation = 0.75f + (float) genRand(&game->rand) / 2.0f;
            const float rest = (1.0f + sinf(game->difficulty / 120.0f) / 4.0f);
            const float difficulty_modifier = sqrtf(game->difficulty / 25.0f);
            const unsigned int wave_size = (unsigned int) (1 + difficulty_modifier * variation * rest);
            printf("---------------\n");
            printf("Wave size: %u\n", wave_size);
            printf("Next wave in: %.0f seconds\n", spawn_timer);

            const float px = player_get_x(game->player);
            const float py = player_get_y(game->player);

            float tdir = (float) genRand(&game->rand) * 2 * PI;

            for (unsigned int i = 0; i < wave_size; i++)
            {
                EntityTemplate_t *enemy;

                const int enemy_types = 2;
                switch (genRandLong(&game->rand) % enemy_types)
                {
                    case 1:
                        if (wave_size >= 5)
                        {
                            enemy = &et_enemy_small_bomber;
                            i += 2;
                            break;
                        }
                    case 0:
                    default:
                        enemy = &et_enemy_scout;
                }

                float vardir = ((float) genRand(&game->rand) - 0.5f) * (PI / 8);

                float t = tdir + vardir;
                float x = px + cosf(t) * min_spawn_radius;
                float y = py + sinf(t) * min_spawn_radius;

                en_create_entity(game->entity_manager, x, y, enemy);
            }

            game->spawn_timer += spawn_timer;
        }
    }
}

void game_destroy(Game_t *game)
{
    if (game == NULL)
        return;

    player_free(game->player);
    particle_manager_free(game->particle_manager);
    projectile_manager_free(game->projectile_manager);
    en_free_manager(game->entity_manager);

    free(game);
}
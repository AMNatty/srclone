cmake_minimum_required(VERSION 3.16)
project(sr_clone C)

include(CheckLibraryExists)

check_library_exists(m powf "" LINK_LIBM)

if (LINK_LIBM)
    set(LIBM ${LIBM} m)
endif (LINK_LIBM)

set(CMAKE_C_STANDARD 99)

add_executable(sr_clone main.c input.c input.h directional-texture.c directional-texture.h stars.c stars.h mtwister.c mtwister.h entity.c entity.h player.c player.h projectile.c projectile.h base-entities-impl.c base-entities-impl.h base-projectiles-impl.c base-projectiles-impl.h font-renderer.c font-renderer.h game.c game.h particle.c particle.h sfx.c sfx.h defs.h)

target_link_libraries(sr_clone ${LIBM} SDL2 SDL2_image SDL2_mixer)

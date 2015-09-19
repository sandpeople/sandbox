
#include <stdio.h>
#include <stdint.h>
#include <SDL/SDL_image.h>

#include "images.h"

static int simulation_initialized = 0;
void initialize_simulation() {
    if (simulation_initialized) {
        return;
    }

    images_init();

    simulation_initialized = 1;
}


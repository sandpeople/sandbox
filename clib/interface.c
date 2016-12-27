
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "fluid.h"
#include "images.h"
#include "simulation.h"
#include "topology.h"

uint8_t *intermediate_colors_buf = NULL;
void interface_run(
        const void *depth_array_v,
        int xsize, int ysize,
        void *output_colors_v) {
    if (!intermediate_colors_buf) {
        intermediate_colors_buf = malloc(xsize * ysize * 3);
    }

    const uint8_t *depth_array = (uint8_t*)depth_array_v;

    simulation_initialize(xsize, ysize);
    assert(gradient_x > 0);
    images_init_simulation_image(xsize, ysize);
    fluid_init(xsize, ysize);
    topology_init(xsize, ysize);

    // Draw basic topology coloring:
    simulation_lockSurface();
    topology_drawToSimImage(depth_array, xsize, ysize);
    simulation_unlockSurface();

    // Draw stuff (grass, ..) below the water:
    assert(!simulation_isSurfaceLocked());
    simulation_drawBeforeWater();

    // Fluid updates:
    simulation_lockSurface();
    assert(simulation_isSurfaceLocked());
    int fluidUpdates = simulation_getFluidUpdateCount();
    int x = 0;
    int y = 0;
    for (int i = 0; i < xsize * ysize; ++i) {
        // Update fluid simulation in this spot:
        for (int k = 0; k < fluidUpdates; k++) {
            fluid_updateAll(x, y);
        }
        fluid_drawAllIfThere(x, y);

        // Advance coordinates:
        x++;
        if (x >= xsize) {
            x -= xsize;
            y++;
        }
    }

    //simulation_updateMovingObjects();
    simulation_unlockSurface();

    // Draw particles on top of water: 
    simulation_drawAfterWater();

    // Draw simulation image with fixed orientation:
    uint8_t *output_colors = (uint8_t*)intermediate_colors_buf;
    uint8_t *output_colors_final = (uint8_t*)output_colors_v;
    simulation_finalRenderToArray(output_colors, xsize, ysize); 

    // Rotate image again since OpenCV uses a stupid format:
    for (int x = 0; x < xsize; x++) {
        for (int y = 0; y < ysize; y++) {
            int offset = 3 * (x + y * xsize);
            int cv_offset = 3 * (y + x * ysize);
            output_colors_final[cv_offset + 0] = output_colors[offset + 0];
            output_colors_final[cv_offset + 1] = output_colors[offset + 1];
            output_colors_final[cv_offset + 2] = output_colors[offset + 2];
        }
    }
}


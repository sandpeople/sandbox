
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

    simulation_initialize(xsize, ysize);
    assert(gradient_x > 0);
    images_init_simulation_image(xsize, ysize);
    fluid_init(xsize, ysize);

    simulation_drawBeforeWater();

    const uint8_t *depth_array = (uint8_t*)depth_array_v;
    uint8_t *output_colors = (uint8_t*)intermediate_colors_buf;
    uint8_t *output_colors_final = (uint8_t*)output_colors_v;

    topology_init(xsize, ysize);

    int fluidUpdates = simulation_getFluidUpdateCount();

    int x = 0;
    int y = 0;
    int depth_source_x = -1;
    int depth_source_y = 0;
    int i;
    for (i = 0; i < xsize * ysize; ++i) {
        int offset = i * 3;
        depth_source_x += 1;
        if (depth_source_x >= xsize) {
            depth_source_y += 1;
            depth_source_x -= xsize;
        }
        int depth_offset = (depth_source_y +
            depth_source_x * ysize);

        // Set height:
        int height = 255 - depth_array[depth_offset];
        height_map[x + y * xsize] = (double)height;

        // Calculate gradient offset:
        int height_color_range_min = 60;
        int height_color_range_max = 100;
        float height_color_value = ((float)(height - height_color_range_min))/
            ((float)(height_color_range_max - height_color_range_min));
        float gradient_relative_x_pos = height_color_value;
        int gradient_abs_y_pos = 5;
        int gradient_abs_x_pos = ((float)(gradient_relative_x_pos *
            ((float)gradient_x)));
        
        // Truncate position:
        if (gradient_abs_x_pos < 0) gradient_abs_x_pos = 0;
        if (gradient_abs_x_pos >= gradient_x) {
            gradient_abs_x_pos = gradient_x - 1;
        }
        assert(gradient_abs_y_pos < gradient_y && gradient_abs_y_pos >= 0);
        assert(gradient_abs_x_pos < gradient_x && gradient_abs_x_pos >= 0);
        assert(gradient_abs_x_pos < 256); 

        // Get gradient color:
        int baseindex = 3 * (gradient_abs_x_pos +
            gradient_abs_y_pos * gradient_x);
        int gradient_c0 = raw_gradient_data[baseindex];
        int gradient_c1 = raw_gradient_data[baseindex + 1];
        int gradient_c2 = raw_gradient_data[baseindex + 2];

        // Offset+0: blue, offset+1: green, offset+2: red
        output_colors[offset+0] = gradient_c0;
        output_colors[offset+1] = gradient_c1;
        output_colors[offset+2] = gradient_c2;

        // Update topology map:
        topology_map[i] = TOPOLOGY_NONE;
        if (gradient_abs_x_pos < 110 && gradient_abs_x_pos > 65) {
            topology_map[i] = TOPOLOGY_GRASS;
            //output_colors[offset+0] = 150;
            //output_colors[offset+1] = 0;
            //output_colors[offset+2] = 255;
        }

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

    simulation_updateMovingObjects();

    // Draw particles on top of water: 
    simulation_drawAfterWater();

    // Draw entire particle/fluid layer onto ground and fix orientation:
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


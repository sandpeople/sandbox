
#include <stdio.h>
#include <stdint.h>
#include <SDL/SDL_image.h>

#include "images.h"
#include "simulation.h"

void run_simulation(const void *depth_array_v, int xsize, int ysize,
        void *output_colors_v) {
    initialize_simulation();

    const uint8_t *depth_array = (uint8_t*)depth_array_v;
    uint8_t *output_colors = (uint8_t*)output_colors_v;
    int i;
    for (i = 0; i < xsize * ysize; ++i) {
        int offset = i * 3;

        // calculate gradient offset:
        int height = 255 - depth_array[i];
        int height_color_range_min = 20;
        int height_color_range_max = 100;
        float height_color_value = ((float)(height - height_color_range_min))/
            ((float)(height_color_range_max - height_color_range_min));
        float gradient_relative_x_pos = height_color_value;
        int gradient_abs_y_pos = 5;
        int gradient_abs_x_pos = ((float)(gradient_relative_x_pos *
            ((float)gradient_x)));
        // truncate position:
        if (gradient_abs_x_pos < 0) gradient_abs_x_pos = 0;
        if (gradient_abs_x_pos >= gradient_x) {
            gradient_abs_x_pos = gradient_x - 1;
        }

        // get gradient color:
        int baseindex = 3 * (gradient_abs_x_pos +
            gradient_abs_y_pos * gradient_x);
        int gradient_c0 = raw_gradient_data[baseindex];
        int gradient_c1 = raw_gradient_data[baseindex + 1];
        int gradient_c2 = raw_gradient_data[baseindex + 2];

        // offset+0: blue, offset+1: green, offset+2: red
        output_colors[offset+0] = gradient_c0;
        output_colors[offset+1] = gradient_c1;
        output_colors[offset+2] = gradient_c2;
    }
    printf("processing complete\n");
}


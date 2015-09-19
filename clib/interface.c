
#include <stdio.h>
#include <stdint.h>

void run_simulation(const void *depth_array_v, int xsize, int ysize,
        void *output_colors_v) {
    const uint8_t *depth_array = (uint8_t*)depth_array_v;
    uint8_t *output_colors = (uint8_t*)output_colors_v;
    int i;
    for (i = 0; i < xsize * ysize; ++i) {
        int offset = i * 3;
        output_colors[offset] = depth_array[i] * 2;
        output_colors[offset+1] = depth_array[i] * 1;
        output_colors[offset+2] = 0;
    }
    printf("processing complete\n");
}


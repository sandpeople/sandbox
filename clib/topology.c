
#include <assert.h>
#include <stdlib.h>

#include "images.h"
#include "particle.h"
#include "simulation.h"
#include "topology.h"


double *topology_drift_cache_height = NULL;
double *topology_drift_cache_value_x = NULL;
double *topology_drift_cache_value_y = NULL;

char *topology_map = NULL;
double *height_map = NULL;
int topology_map_x = 0;
int topology_map_y = 0;
int require_topology_rebuild = 1;
void topology_init(int size_x, int size_y) {
    if (topology_map) {
        if (size_x == topology_map_x &&
                size_y == topology_map_y) {
            return;
        }
        free(topology_map);
        free(height_map);
        particle_wipeAll(PARTICLE_GRASS);
    }
    require_topology_rebuild = 1;
    topology_map_x = size_x;
    topology_map_y = size_y;
    topology_map = malloc(size_x * size_y);
    height_map = (double*)malloc(size_x * size_y * sizeof(double));
    free(topology_drift_cache_height);
    topology_drift_cache_height = NULL;
    free(topology_drift_cache_value_x);
    topology_drift_cache_value_x = NULL;
    free(topology_drift_cache_value_y);
    topology_drift_cache_value_y = NULL;
}

void topology_calculate_drift(int x, int y, double *vx, double *vy) {
    // Don't allow invalid values:
    if (x < 0 || x >= topology_map_x || y < 0 || y >= topology_map_y) {
        *vx = 0; *vy = 0;
        return;
    }

    // Ensure cache:
    if (topology_drift_cache_height == NULL) {
        topology_drift_cache_height = (double*)malloc(topology_map_x *
            topology_map_y * sizeof(double));
        memset(topology_drift_cache_height, 0,
            topology_map_x *
            topology_map_y * sizeof(double));
        topology_drift_cache_value_x = (double*)malloc(topology_map_x *
            topology_map_y * sizeof(double));
        topology_drift_cache_value_y = (double*)malloc(topology_map_x *
            topology_map_y * sizeof(double));   
    }

    // Prepare stuff for cache access:
    int currentindex = x + y * topology_map_x;
    double cache_match_height = 1000.0f + height_map[currentindex];

    // See if we have a cached value:
    if (fabs(topology_drift_cache_height[currentindex] - cache_match_height) <
            1.0f) {
        // we do!
        *vx = topology_drift_cache_value_x[currentindex];
        *vy = topology_drift_cache_value_y[currentindex];
        return;
    }
    topology_drift_cache_height[currentindex] = cache_match_height;

    // No cached value if we arrive here. Calculate one:
    int radius = 20;
    int scan_start_x = x + 0.5 - (radius / 2.0);
    int scan_start_y = y + 0.5 - (radius / 2.0);
    double vec_x = 0;
    double vec_y = 0;
    double center_height = height_map[x + y * topology_map_x]; 
    for (int px = scan_start_x; px < scan_start_x + radius; px++) {
        if (px < 0 || px >= topology_map_x) continue;
        for (int py = scan_start_y; py < scan_start_y + radius; py++) {
            if (py < 0 || py >= topology_map_y) continue;
            double height_diff = center_height -
                height_map[px + py * topology_map_x];
            double height_diff_fac = height_diff / 10.0;
            if (height_diff_fac > 1.0) {
                height_diff_fac = 1.0;
            }
            if (height_diff_fac < -1.0) {
                height_diff_fac = -1.0;
            }
            if (height_diff_fac > 0) {
                height_diff_fac *= height_diff_fac;
            } else {
                height_diff_fac *= height_diff_fac;
                height_diff_fac = -height_diff_fac;
            }
            vec_x += (px - x) * height_diff_fac * 5;
            vec_y += (py - y) * height_diff_fac * 5;
        }
    }
    double max = 25.0f;
    if (vec_x > max) {
        vec_x = max;
    }
    if (vec_y > max) {
        vec_y = max;
    }
    if (vec_x < -max) {
        vec_x = -max;
    }
    if (vec_y < -max) {
        vec_y = -max;
    }
    *vx = vec_x;
    *vy = vec_y;
    topology_drift_cache_value_x[currentindex] = vec_x;
    topology_drift_cache_value_y[currentindex] = vec_y;
}

double topology_scan_type(int type, int x, int y, int size) {
    int scan_start_x = x - (size / 2.0);
    int scan_start_y = y - (size / 2.0);
    double positive = 0;
    double negative = 0;
    double total = 0;
    for (int x = scan_start_x; x < scan_start_x + size; x++) {
        if (x < 0 || x >= topology_map_x) continue;
        for (int y = scan_start_y; y < scan_start_y + size; y++) {
            if (y < 0 || y >= topology_map_y) continue;
            int index = x + y * topology_map_x;
            total += 1.0;
            if (topology_map[index] == type) {
                positive += 1.0;
            } else {
                negative += 1.0;
            }
        }
    }
    return positive / total;
}

int get_topology(int x, int y) {
    if (x < 0 || x >= topology_map_x || y < 0 || y >= topology_map_y) {
        return TOPOLOGY_NONE;
    }
    return topology_map[x + y * topology_map_x];
}

void topology_drawToSimImage(const uint8_t* depth_array, int xsize, int ysize) {
    assert(simulation_isSurfaceLocked());

	// Draw topology gradient:
    int x = 0;
    int y = 0;
    int depth_source_x = -1;
    int depth_source_y = 0;
    for (int i = 0; i < xsize * ysize; ++i) {
        int offset = i * 4;
        depth_source_x += 1;
        if (depth_source_x >= xsize) {
            depth_source_y++;
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
		((char*)images_simulation_image->pixels)[offset+1] = gradient_c0;
        ((char*)images_simulation_image->pixels)[offset+2] = gradient_c1;
        ((char*)images_simulation_image->pixels)[offset+3] = gradient_c2;
        ((char*)images_simulation_image->pixels)[offset+0] = 255; // alpha

        // Update topology map:
        topology_map[i] = TOPOLOGY_NONE;
        if (gradient_abs_x_pos < 110 && gradient_abs_x_pos > 65) {
            topology_map[i] = TOPOLOGY_GRASS;
        }

        // Advance coordinates:
        x++;
        if (x >= xsize) {
            x -= xsize;
            y++;
        }
	}
}

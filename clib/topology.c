
#include <assert.h>
#include <stdlib.h>

#include "particle.h"
#include "topology.h"

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
}

void topology_calculate_drift(int x, int y, int radius,
        double *vx, double *vy) {
    int scan_start_x = x - (radius / 2.0);
    int scan_start_y = y - (radius / 2.0);
    double vec_x = 0;
    double vec_y = 0;
    if (x < 0 || x >= topology_map_x || y < 0 || y >= topology_map_y) {
        return;
    }
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



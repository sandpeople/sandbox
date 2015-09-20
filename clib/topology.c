
#include <stdlib.h>

#include "particle.h"
#include "topology.h"

char *topology_map = NULL;
static int topology_map_x = 0;
static int topology_map_y = 0;
int require_topology_rebuild = 1;
void topology_init(int size_x, int size_y) {
    if (topology_map) {
        if (size_x == topology_map_x &&
                size_y == topology_map_y) {
            return;
        }
        free(topology_map);
        particle_wipeAll(PARTICLE_GRASS);
    }
    require_topology_rebuild = 1;
    topology_map_x = size_x;
    topology_map_y = size_y;
    topology_map = malloc(size_x * size_y);
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



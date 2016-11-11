
#include <assert.h>

#include "fluid.h"
#include "images.h"
#include "random.h"
#include "simulation.h"
#include "topology.h"

static int fluid_map_x = 0;
static int fluid_map_y = 0;
double *fluid_map[FLUID_COUNT] = { 0 };

void fluid_init(int width, int height) {
    if (fluid_map[0]) {
        if (fluid_map_x == width && fluid_map_y == height) {
            return;
        }
        for (int i = 0; i < FLUID_COUNT; i++) {
            free(fluid_map[i]);
        }
    }
    fluid_map_x = width;
    fluid_map_y = height;
    for (int i = 0; i < FLUID_COUNT; i++) {
        fluid_map[i] = (double *)malloc(sizeof(double) * width * height);
        memset(fluid_map[i], 0, sizeof(double) * width * height);
    }
}

void fluid_spawn(int type, int x, int y, double amount) {
    if (x < 0 || x >= fluid_map_x || y < 0 || y > fluid_map_y) return;
    assert(type >= 0 && type < FLUID_COUNT);
    fluid_map[type][x + y * fluid_map_x] += amount;
}

double fluid_check(int type, int x, int y) {
    if (x < 0 || x >= fluid_map_x || y < 0 || y >= fluid_map_y) {
        return 0.0;
    }
    if (fluid_map[type][x + y * fluid_map_x] < 5.0) {
        return fluid_map[type][x + y * fluid_map_x] / 5.0;
    }
    return 5.0;
}

void fluid_drawIfThere(int type, int x, int y) {
    double alpha = fluid_check(type, x, y) +
        fluid_check(type, x - 1, y - 1) * 0.5 +
        fluid_check(type, x - 1, y) * 0.5 +
        fluid_check(type, x - 1, y + 1) * 0.5 +
        fluid_check(type, x + 1, y) * 0.5 +
        fluid_check(type, x + 1, y) * 0.5 +
        fluid_check(type, x + 1, y + 1) * 0.5;
    alpha = alpha * alpha;
    if (alpha > 0.7) alpha = 0.7;
    simulation_addPixel(x + y * fluid_map_x, 255, 0, 0, alpha * 255);
}

void fluid_drawAllIfThere(int x, int y) {
    for (int i = 0; i < FLUID_COUNT; i++) {
        fluid_drawIfThere(i, x, y);
    }
}

double fluid_tryTransfer(int type, int target_x, int target_y,
        double amount, double max) {
    if (target_x < 0 || target_x >= fluid_map_x ||
            target_y < 0 || target_y >= fluid_map_y) {
        return;
    }
    if (fluid_map[type][target_x + target_y * fluid_map_x] + amount > max) {
        amount = max - fluid_map[type][target_x + target_y * fluid_map_x];
        if (amount < 0) {
            amount = 0;
        }
    }
    fluid_map[type][target_x + target_y * fluid_map_x] += amount;
    return amount;
}

void fluid_update(int type, int x, int y) {
    /* Do one update step of the physics simulation of the fluid located in
       this spot, possibly making it spread to neighboring points.
       Make sure to call this in a fixed time step for consistent physics.
    */
    if (x < 0 || x >= fluid_map_x || y < 0 || y >= fluid_map_y) return;

    if (fluid_map[type][x + y * fluid_map_x] <= 0.01) {
        return;
    }

    double jumpLength = 3;
    double velocity_x = 0;
    double velocity_y = 0;

    topology_calculate_drift(x, y,
        &velocity_x, &velocity_y);

    velocity_x /= 10.0;
    velocity_y /= 10.0;
    
    if (velocity_x > jumpLength) {
        velocity_x = jumpLength;
    }
    if (velocity_y > jumpLength) {
        velocity_y = jumpLength;
    }
    if (velocity_x < -jumpLength) {
        velocity_x = -jumpLength;
    }
    if (velocity_y < -jumpLength) {
        velocity_y = -jumpLength;
    }

    double xfac = 1000.0 / ((double)fluid_map_x);
    double yfac = 1000.0 / ((double)fluid_map_y);

    int target_x = (double)x + velocity_x * xfac;
    int target_y = (double)y + velocity_y * yfac;

    if (target_x > x + 2) {
        target_x = x + 2;
    }
    if (target_y > y + 2) {
        target_y = y + 2;
    }
    if (target_x < x - 2) {
        target_x = x - 2;
    }
    if (target_y < y - 2) {
        target_y = y - 2;
    }

    // transfer along the slope of the ground:
    double ownAmount = fluid_map[type][x + y * fluid_map_x];
    if (target_x != x || target_y != y) {
        if (target_x >= 0 && target_x < fluid_map_x && target_y >= 0 &&
                target_y < fluid_map_y &&
                x >= 0 && x < fluid_map_x && y >= 0 &&
                y < fluid_map_y) {
            double transfer = 0.2 * ownAmount;
            if (transfer > fluid_map[type][x + y * fluid_map_x] * 0.5) {
                transfer = fluid_map[type][x + y * fluid_map_x] * 0.5;
            }
            fluid_map[type][target_x + target_y * fluid_map_x] += transfer;
            fluid_map[type][x + y * fluid_map_x] -= transfer;
            ownAmount = fluid_map[type][x + y * fluid_map_x];
        }
    }

    // transfer evenly to all neighboring pixels:
    for (int neighbor_x = -1; neighbor_x <= 1; neighbor_x += 2) {
        for (int neighbor_y = -1; neighbor_y <= 1; neighbor_y += 2) {
            // transfer to around 10% of own amount:
            double transfer = 0.1 * ownAmount;
            if (transfer > ownAmount) {
                transfer = ownAmount;
            }

            // transfer target limit should be fmax(ownAmount, 10.0):
            double limit = fmax(ownAmount, 10.0); 

            // do transfer and see how much we managed to transfer:
            double gone = fluid_tryTransfer(type, x + neighbor_x,
                y + neighbor_y, transfer, limit);

            // reduce transferred fluid from ourselves:
            fluid_map[type][x + y * fluid_map_x] -= gone;
            ownAmount = fluid_map[type][x + y * fluid_map_x];
        }
    } 
}

void fluid_randomSpawns() {
    for (int i = 0; i < 20; i++) {
        double x = rand0to1();
        double y = rand0to1();
        int world_x = x * fluid_map_x;
        int world_y = y * fluid_map_y;
        fluid_spawn(FLUID_WATER, world_x, world_y, 5000 + rand0to1() * 1000);
    }
}

void fluid_updateAll(int x, int y) {
    for (int i = 0; i < FLUID_COUNT; i++) {
        fluid_update(i, x, y);
    }
}





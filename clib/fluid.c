
#include <assert.h>
#include <pthread.h>

#include "fluid.h"
#include "images.h"
#include "random.h"
#include "simulation.h"
#include "topology.h"

static int fluid_map_x = 0;
static int fluid_map_y = 0;
double *fluid_map[FLUID_COUNT] = { 0 };

pthread_mutex_t *fluid_access = NULL;
pthread_t *fluid_thread = NULL;

double reduce_factor = 5;

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

double fluid_checkWorld(int type, int x, int y) {
    int x2 = (int)((double)x / reduce_factor);
    int y2 = (int)((double)y / reduce_factor);
    return fluid_check(type, x2, y2);
}

void fluid_drawIfThere(int type, int worldX, int worldY,
        int xsize) {
    int x = worldX;
    int y = worldY;
    int drawx = x;
    int drawy = y;
    int jitterx = (int)(rand0to1() * 4) - 2;
    int jittery = (int)(rand0to1() * 4) - 2;
    x += jitterx;
    y += jittery;
    double alpha = fluid_checkWorld(type, x, y) +
        fluid_checkWorld(type, x - 1, y - 1) * 0.5 +
        fluid_checkWorld(type, x - 1, y) * 0.5 +
        fluid_checkWorld(type, x - 1, y + 1) * 0.5 +
        fluid_checkWorld(type, x + 1, y) * 0.5 +
        fluid_checkWorld(type, x + 1, y) * 0.5 +
        fluid_checkWorld(type, x + 1, y + 1) * 0.5;
    alpha = alpha * alpha;
    if (alpha > 0.7) alpha = 0.7;
    simulation_addPixel(drawx + drawy * xsize, 255, 0, 0, sqrt(alpha) * 255);
}

void fluid_drawAllIfThere(int x, int y, int xsize) {
    for (int i = 0; i < FLUID_COUNT; i++) {
        fluid_drawIfThere(i, x, y, xsize);
    }
}

double fluid_tryTransfer(int type, int target_x, int target_y,
        double amount, double max, double hard_max) {
    if (target_x < 0 || target_x >= fluid_map_x ||
            target_y < 0 || target_y >= fluid_map_y) {
        return 0;
    }
    if (fluid_map[type][target_x + target_y * fluid_map_x] + amount >
            hard_max) {
        amount = hard_max - fluid_map[type][target_x + target_y *
            fluid_map_x];
        if (amount < 0) {
            amount = 0;
        }
    }
    if (fluid_map[type][target_x + target_y * fluid_map_x] + amount > max) {
        amount = amount * 0.5;
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

    int worldX = ((double)x * reduce_factor);
    int worldY = ((double)y * reduce_factor);

    double jumpLength = 3 / reduce_factor;
    double velocity_x = 0;
    double velocity_y = 0;

    // Get basic velocity from ground:
    topology_calculate_drift(worldX, worldY,
        &velocity_x, &velocity_y);

    // Scale velocity:
    velocity_x /= 2.0;
    velocity_y /= 2.0;
    velocity_x += rand0to1() * 5.0 - 10.0;   
    velocity_y += rand0to1() * 5.0 - 10.0; 

    // Limit to jump length:
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

    if (target_x > x + 5.0 / reduce_factor) {
        target_x = x + 5.0 / reduce_factor;
    }
    if (target_y > y + 5.0 / reduce_factor) {
        target_y = y + 5.0 / reduce_factor;
    }
    if (target_x < x - 5.0 / reduce_factor) {
        target_x = x - 5.0 / reduce_factor;
    }
    if (target_y < y - 5.0 / reduce_factor) {
        target_y = y - 5.0 / reduce_factor;
    }

    double miss_factor = 1.0f;

    // Transfer along the slope of the ground:
    double ownAmount = fluid_map[type][x + y * fluid_map_x];
    if ((target_x != x || target_y != y) && ownAmount > 0.01) {
        if (target_x >= 0 && target_x < fluid_map_x && target_y >= 0 &&
                target_y < fluid_map_y &&
                x >= 0 && x < fluid_map_x && y >= 0 &&
                y < fluid_map_y) {
            double sMapX = ((double)x) * reduce_factor;
            double sMapY = ((double)y) * reduce_factor;
            double tMapX = ((double)target_x) * reduce_factor;
            double tMapY = ((double)target_y) * reduce_factor;
            double heightDiff = topology_heightAt(sMapX + 0.5, sMapY + 0.5) -
                topology_heightAt(tMapX + 0.5, tMapY + 0.5);

            double fac = fmax(0, fmin(1.0, heightDiff / 40.0)) * 0.4 + 0.6;

            double transfer = 0.9 * ownAmount * fac;
            if (transfer > fluid_map[type][x + y * fluid_map_x] * 0.5) {
                transfer = fluid_map[type][x + y * fluid_map_x] * 0.5;
            }
            double amount = fmax(0.01 * ownAmount * fac, fmin((10.0 * reduce_factor) -
                fluid_map[type][target_x + target_y * fluid_map_x],
                transfer));
            fluid_map[type][target_x + target_y * fluid_map_x] += amount;
            fluid_map[type][x + y * fluid_map_x] -= amount * miss_factor;
            ownAmount = fluid_map[type][x + y * fluid_map_x];
        }
    }

    // Transfer evenly to all neighboring pixels:
    int range = (double)30.0 / reduce_factor;
    if (range < 1) range = 1;
    for (int k = 0; k < 10; k++) {
        double fneighbor_x = (double)(rand0to1() * 2.0 - 1.0);
        double fneighbor_y = (double)(rand0to1() * 2.0 - 1.0);
        if (fluid_map[type][x + y * fluid_map_x]  < 0.5)
            continue;

        // Scale in a circle:
        double vecLength = sqrt(fneighbor_x * fneighbor_x +
            fneighbor_y * fneighbor_y);
        double targetVecLength = (double)(rand0to1() * range);
        double scale_fac = targetVecLength / vecLength;
        fneighbor_x *= scale_fac;
        fneighbor_y *= scale_fac;
        int neighbor_x = (int)(fneighbor_x + 0.5);
        int neighbor_y = (int)(fneighbor_y + 0.5);
        if (neighbor_x == 0 && neighbor_y == 0)
            continue;

        // Transfer to around 1% of own amount:
        double transfer = 0.2 * ownAmount / fmin(10.0 / reduce_factor, fabs(neighbor_x) + fabs(neighbor_y));
        transfer /= range;

        // Transfer target limit should be fmax(ownAmount, 10.0):
        double limit = fmin(fmin(ownAmount, 100.0 * reduce_factor),
            fluid_map[type][x + y * fluid_map_x]);

        // Do transfer and see how much we managed to transfer:
        double gone = fluid_tryTransfer(type, x + neighbor_x,
            y + neighbor_y, transfer, limit, fluid_map[type][x + y * fluid_map_x]);

        // Reduce transferred fluid from ourselves:
        fluid_map[type][x + y * fluid_map_x] -= gone * miss_factor;
    } 
}

void fluid_randomSpawns() {
    for (int i = 0; i < 500; i++) {
        double x = rand0to1();
        double y = rand0to1();
        int fluid_x = x * fluid_map_x;
        int fluid_y = y * fluid_map_y;
        fluid_spawn(FLUID_WATER, fluid_x, fluid_y, (60.0 + rand0to1() * 10.0) / reduce_factor);
    }
}

void fluid_updateAll() {
	int fluidUpdates = simulation_getFluidUpdateCount();
    if (fluidUpdates <= 0)
        return;
    int x = 0;
    int y = 0;
	pthread_mutex_lock(fluid_access);
    for (int j = 0; j < fluidUpdates; j++) {
        x = 0;
        y = 0;
        for (int i = 0; i < fluid_map_x * fluid_map_y; ++i) {
            // Update fluid simulation in this spot:
            for (int k = 0; k < FLUID_COUNT; k++) {
                fluid_update(k, x, y);
            }

            // Advance coordinates:
            x++;
            if (x >= fluid_map_x) {
                x -= fluid_map_x;
                y++;
            }
        }
    }
	pthread_mutex_unlock(fluid_access);
}

void fluid_drawAll(int xsize, int ysize) {
	int x = 0;
    int y = 0; 
	pthread_mutex_lock(fluid_access);
    for (int i = 0; i < xsize * ysize; ++i) {
		fluid_drawAllIfThere(x, y, xsize);

        // Advance coordinates:
        x++;
        if (x >= xsize) {
            x -= xsize;
            y++;
        }
    }
	pthread_mutex_unlock(fluid_access);
}

static void *fluid_simulationThread(void *userdata) {
    while (1) {
	    fluid_updateAll();
    }
    return NULL;
}

void fluid_init(int width, int height) {
    int new_fluid_map_x = (int)((double)width / reduce_factor);
    int new_fluid_map_y = (int)((double)height / reduce_factor);
    if (fluid_map[0]) {
        if (fluid_map_x == new_fluid_map_x &&
                fluid_map_y == new_fluid_map_y) {
            return;
        }
        for (int i = 0; i < FLUID_COUNT; i++) {
            free(fluid_map[i]);
        }
    }
    if (!fluid_access) {
        fluid_access = malloc(sizeof(*fluid_access));
        pthread_mutex_init(fluid_access, NULL);
    }
    pthread_mutex_lock(fluid_access);
    fluid_map_x = new_fluid_map_x;
    fluid_map_y = new_fluid_map_y;
    for (int i = 0; i < FLUID_COUNT; i++) {
        fluid_map[i] = (double *)malloc(sizeof(double) *
            fluid_map_x * fluid_map_y);
        memset(fluid_map[i], 0, sizeof(double) *
            fluid_map_x * fluid_map_y);
    }
    pthread_mutex_unlock(fluid_access);
    if (!fluid_thread) {
        fluid_thread = malloc(sizeof(*fluid_thread));
        pthread_create(fluid_thread, NULL, fluid_simulationThread, NULL);
    } 
}


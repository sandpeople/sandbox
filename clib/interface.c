
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>

#include "fluid.h"
#include "images.h"
#include "simulation.h"
#include "topology.h"

// Main compute thread communication variables:
uint8_t *_depth_input_transfer_buf = NULL;
uint8_t *_color_output_transfer_buf = NULL;
static volatile int shutdown_signal = 0;

uint8_t *intermediate_colors_buf = NULL;
uint8_t *output_colors_final = NULL;
uint8_t *depth_array_buf = NULL;
static int xsize, ysize;

static pthread_mutex_t *main_compute_data_access = NULL;
static pthread_t *main_compute_thread = NULL;

static void *interface_mainComputeThread(
            __attribute__((unused)) void *userdata
        ) {
    while (!shutdown_signal) {
        // Get depth input data:
        pthread_mutex_lock(main_compute_data_access);
        memcpy(depth_array_buf, _depth_input_transfer_buf,
            xsize * ysize * 1);
        pthread_mutex_unlock(main_compute_data_access);

        // Make sure everything is initialized:
        simulation_initialize(xsize, ysize);
        assert(gradient_x > 0);
        images_init_simulation_image(xsize, ysize);
        fluid_init(xsize, ysize);
        topology_init(xsize, ysize);

        // Draw basic topology coloring:
        simulation_lockSurface();
        topology_drawToSimImage(depth_array_buf, xsize, ysize);
        simulation_unlockSurface();

        // Draw stuff (grass, ..) below the water:
        assert(!simulation_isSurfaceLocked());
        simulation_drawBeforeWater();

        // Fluid updates:
        simulation_lockSurface();
        assert(simulation_isSurfaceLocked());
        fluid_drawAll(xsize, ysize);

        simulation_updateMovingObjects();
        simulation_unlockSurface();

        // Draw particles on top of water: 
        simulation_drawAfterWater();

        // Draw simulation image with fixed orientation:
        uint8_t *output_colors = (uint8_t*)intermediate_colors_buf;
        simulation_finalRenderToArray(output_colors, xsize, ysize); 

        // Rotate image again since OpenCV uses a stupid format:
        for (int x = 0; x < xsize; x++) {
            for (int y = 0; y < ysize; y++) {
                int offset = 3 * (x + y * xsize);
                int cv_offset = 3 * (y + x * ysize);
                output_colors_final[cv_offset + 0] =
                    output_colors[offset + 0];
                output_colors_final[cv_offset + 1] =
                    output_colors[offset + 1];
                output_colors_final[cv_offset + 2] =
                    output_colors[offset + 2];
            }
        }

        // Output color data:
        pthread_mutex_lock(main_compute_data_access);
        memcpy(_color_output_transfer_buf, output_colors_final,
            xsize * ysize * 3);
        pthread_mutex_unlock(main_compute_data_access);
    }
    return NULL;
}

void interface_run(
        const void *depth_array_v,
        int _xsize, int _ysize,
        void *output_colors_v) {
    // Initialize all the data buffers we need:
    if (!_depth_input_transfer_buf) {
        _depth_input_transfer_buf = malloc(_xsize * _ysize);
    }
    if (!_color_output_transfer_buf) {
        _color_output_transfer_buf = malloc(_xsize * _ysize * 3);
    }
    if (!intermediate_colors_buf) {
        intermediate_colors_buf = malloc(_xsize * _ysize * 3);
    }
    if (!output_colors_final) {
        output_colors_final = malloc(_xsize * _ysize * 3);
    }
    if (!depth_array_buf) {
        depth_array_buf = malloc(_xsize * _ysize);
        memset(depth_array_buf, 0, _xsize * _ysize);
    }

    // Initialize mutex and compute thread:
    if (!main_compute_data_access) {
        main_compute_data_access = malloc(sizeof(*main_compute_data_access));
        pthread_mutex_init(main_compute_data_access, NULL);
    }
    if (!main_compute_thread) {
        main_compute_thread = malloc(sizeof(*main_compute_thread));
        xsize = _xsize;
        ysize = _ysize;
        printf("[clib/interfacae.c] SCREEN DIMENSIONS: %d, %d\n",
            xsize, ysize);
        pthread_create(
            main_compute_thread, NULL,
            interface_mainComputeThread, NULL);
    }

    // Lock data access, update depth buffer and copy back colors:
    if (xsize != _xsize || ysize != _ysize) {
        fprintf(stderr, "[clib/interface.c] FATAL ERROR: "
            "window resizing is not supported. "
            "Initial size was %d, %d, requested size is %d, %d\n",
            xsize, ysize, _xsize, _ysize);
        memset(output_colors_v, 0, _xsize * _ysize * 3);
        return;
    }
    pthread_mutex_lock(main_compute_data_access);
    memcpy(_depth_input_transfer_buf, depth_array_v, xsize * ysize);
    memcpy(output_colors_v, _color_output_transfer_buf, xsize * ysize * 3);
    pthread_mutex_unlock(main_compute_data_access);
}

void interface_stop() {
    shutdown_signal = 1;
    sleep(1);   
}

void interface_setHeightConfig(double heightShift, double heightScale) {
    topology_setHeightConfig(heightShift, heightScale);
}

void interface_mapOffset(double x, double y) {
    simulation_addMapOffset(y, x); 
}

void interface_resetMapOffset() {
    simulation_resetMapOffset();
}

void interface_setMapZoom(double zoom) {
    if (zoom < 0.001 || zoom > 1000)
        return;
    simulation_setMapZoom(zoom);
}

void interface_spawnWater(double x, double y) {
    int wX = (int)x;
    int wY = (int)y;
    if (wX < 0) wX = 0;
    if (wX >= images_simulation_image->w) wX = images_simulation_image->w;
    if (wY < 0) wY = 0;
    if (wY >= images_simulation_image->h) wY = images_simulation_image->h; 
    fluid_spawn(FLUID_WATER, wX, wY, 500);   
}

void interface_resetWater() {
    fluid_resetAll();    
}


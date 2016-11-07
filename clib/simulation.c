
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "fluid.h"
#include "images.h"
#include "particle.h"
#include "simulation.h"
#include "topology.h"

SDL_Window *hiddenWindow = NULL;
SDL_Renderer *acceleratedRenderer = NULL;

SDL_Renderer *simulation_getRenderer() {
    return acceleratedRenderer;
}

static int simulation_surface_locked = 0;
static int simulation_initialized = 0;
void simulation_initialize(int width, int height) {
    if (simulation_initialized) {
        return;
    }

    SDL_Init(SDL_INIT_VIDEO);

    hiddenWindow = SDL_CreateWindow("A SDL2 window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!hiddenWindow) {
        fprintf(stderr, "[simulation.c] FAILED TO INITIALIZE WINDOW");
        exit(1);
        return;
    }
    acceleratedRenderer = SDL_CreateRenderer(hiddenWindow, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!acceleratedRenderer) {
        fprintf(stderr, "[simulation.c] FAILED TO INITIALIZE "
            "ACCELERATED RENDERER");
        exit(1);
        return;
    }

    images_init();

    fluid_init(width, height);
    fluid_randomSpawns();

    particle_addRandomCrowd(PARTICLE_GRASS, 5000);
    particle_addRandomCrowd(PARTICLE_CAR, 50);

    simulation_initialized = 1;
}

void simulation_lockSurface() {
    if (!simulation_surface_locked) {
        if (SDL_LockSurface(images_simulation_image) != 0) {
            printf("ERROR FAILED TO LOCK SIMULATION IMAGE\n");
            exit(1);
        }
        simulation_surface_locked = 1;
    }
}


static int64_t lastMovingObjectsUpdate = -1;
void simulation_updateMovingObjects() {
    if (lastMovingObjectsUpdate < 0) {
        lastMovingObjectsUpdate = SDL_GetTicks();
    }
    int timestep = (1000.0 / 10.0);
    while (lastMovingObjectsUpdate + timestep < SDL_GetTicks()) {
        particle_updateAll();
        lastMovingObjectsUpdate += timestep;
    }
}

static int64_t lastFluidUpdate = -1;
int simulation_getFluidUpdateCount() {
    if (lastFluidUpdate < 0) {
        lastFluidUpdate = SDL_GetTicks();
    }
    int timestep = (1000.0 / 15.0);
    int count = 0;
    while (lastFluidUpdate + timestep < SDL_GetTicks()) {
        count += 1;
        lastFluidUpdate += timestep;
    }
    return count;
}

void simulation_drawBeforeWater() {
    //simulation_unlockSurface();

    // clear simulation image:
    SDL_FillRect(images_simulation_image, NULL, 0x000000);   

    // draw particles below fluid simulations:
    images_simulation_2d_to_3d_upload();
    particle_renderAllToSurface(0, PARTICLE_BELOW_WATER,
        images_simulation_image);
    images_simulation_3d_to_2d_download();
}

void simulation_drawAfterWater() {
    //simulation_unlockSurface();

    // draw particles on top of fluid simulations:
    images_simulation_2d_to_3d_upload();
    particle_renderAllToSurface(PARTICLE_BELOW_WATER, PARTICLE_TYPE_COUNT, 
        images_simulation_image);
    images_simulation_3d_to_2d_download();
}

void simulation_finalRenderToArray(uint8_t *render_data,
        int width, int height) {
    simulation_lockSurface();
    assert(width == images_simulation_image->w);
    assert(height == images_simulation_image->h);
    assert(images_simulation_image->format->BitsPerPixel == 32);
    simulation_surface_locked = 1;
    uint8_t *pix = (unsigned char*)images_simulation_image->pixels;
    int bpp = images_simulation_image->format->BytesPerPixel;
    assert(bpp == 4);
    int pitch = images_simulation_image->pitch;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i1 = 3 * (x + y * width);
            int i2 = 4 * x + y * pitch;

            uint8_t a = pix[i2 + 0];
            double alpha = ((double)a)/(255.0);
            double new_c;

            new_c = ((double)((uint8_t)render_data[i1 + 0]) *
                (1.0 - alpha)) +
                ((double)((uint8_t)pix[i2 + 1])) * alpha;
            render_data[i1 + 0] = new_c;

            new_c = ((double)((uint8_t)render_data[i1 + 1]) *
                (1.0 - alpha)) +
                ((double)((uint8_t)pix[i2 + 2])) * alpha;
            render_data[i1 + 1] = (unsigned char) new_c;

            new_c = ((double)((uint8_t)render_data[i1 + 2]) *
                (1.0 - alpha)) +
                ((double)((uint8_t)pix[i2 + 3])) * alpha;
            render_data[i1 + 2] = (unsigned char) new_c;
        }
    }

    SDL_UnlockSurface(images_simulation_image);

    simulation_surface_locked = 0;
}

void simulation_addPixel(int i, int r, int g, int b, int a) {
    simulation_lockSurface();
    char *pix = (char*)images_simulation_image->pixels;
    r = (int)(((float)r) * ((float)a / 255.0));
    g = (int)(((float)g) * ((float)a / 255.0));
    b = (int)(((float)b) * ((float)a / 255.0));

    int new_a = pix[4*i + 0] + a;
    if (new_a > 255) {new_a = 255;}
    pix[4*i + 0] = new_a;

    int new_r = pix[4*i + 1] + r;
    if (new_r > 255) {new_r = 255;}
    pix[4*i + 1] = new_r;

    int new_g = pix[4*i + 2] + g;
    if (new_g > 255) {new_g = 255;}
    pix[4*i + 2] = new_g; 

    int new_b = pix[4*i + 3] + b;
    if (new_b > 255) {new_b = 255;}
    pix[4*i + 3] = new_b;
}

void simulation_unlockSurface() {
    if (simulation_surface_locked) {
        SDL_UnlockSurface(images_simulation_image);
        simulation_surface_locked = 0;
    }
}


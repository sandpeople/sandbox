
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "fluid.h"
#include "images.h"
#include "particle.h"
#include "simulation.h"
#include "topology.h"
#include "transform.h"

static const int renderTransformGridX = 10;
static const int renderTransformGridY = 10;

struct rendergrid *renderTransformGrid = NULL;
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

    glewExperimental = GL_TRUE;
    glewInit();
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

    hiddenWindow = SDL_CreateWindow(
        "IGNORE THIS, SUPPOSEDLY HIDDEN TEMPORARY RENDER TARGET",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        (int)(width * 1.2), (int)(height * 1.2),
        SDL_WINDOW_OPENGL);
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

    if (!renderTransformGrid)
        renderTransformGrid = transform_createNewGrid(
            renderTransformGridX, renderTransformGridY); 

    simulation_initialized = 1;
}

void simulation_lockSurface() {
    assert(!simulation_surface_locked);
    if (SDL_LockSurface(images_simulation_image) != 0) {
        printf("ERROR FAILED TO LOCK SIMULATION IMAGE\n");
        exit(1);
    }
    simulation_surface_locked = 1;
}

int simulation_isSurfaceLocked() {
    return simulation_surface_locked;
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
    if (count == 0) {
        SDL_Delay(timestep);
    }
    return count;
}

void simulation_drawBeforeWater() {
    assert(!simulation_isSurfaceLocked());

    // Draw particles below fluid simulations:
    images_simulation_2d_to_3d_upload();
    assert(!simulation_isSurfaceLocked());
    particle_renderAll(0, PARTICLE_BELOW_WATER);
    images_simulation_3d_to_2d_download();
}

void simulation_drawAfterWater() {
    // Draw particles on top of fluid simulations:
    images_simulation_2d_to_3d_upload();
    particle_renderAll(PARTICLE_BELOW_WATER, PARTICLE_TYPE_COUNT);
    // Apply final transform for runtime calibration:
    transform_draw(renderTransformGrid, images_simulation_3d_image);
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

            render_data[i1 + 0] = pix[i2 + 1];
            render_data[i1 + 1] = pix[i2 + 2];
            render_data[i1 + 2] = pix[i2 + 3];
        }
    }

    simulation_unlockSurface();

    simulation_surface_locked = 0;
}

void simulation_addPixel(int i, int r, int g, int b, int a) {
    if (i < 0 || 4 * i + 3 >= images_simulation_image->w *
            images_simulation_image->h * 4) {
        return;
    }
    assert(simulation_isSurfaceLocked());

    unsigned char *pix = (unsigned char*)images_simulation_image->pixels;
    r = (int)(((float)r) * ((float)a / 255.0));
    g = (int)(((float)g) * ((float)a / 255.0));
    b = (int)(((float)b) * ((float)a / 255.0));

    double fa = ((double)a) / 255.0;

    int new_a = pix[4*i + 0] + a;
    if (new_a < 0) {new_a = 0;}
    if (new_a > 255) {new_a = 255;}
    pix[4*i + 0] = new_a;

    int new_r = ((double)pix[4*i + 1]) * (1.0 - fa)  + r * fa;
    if (new_r < 0) {new_r = 0;}
    if (new_r > 255) {new_r = 255;}
    pix[4*i + 1] = new_r;

    int new_g = ((double)pix[4*i + 2]) * (1.0 - fa) + g * fa;
    if (new_g < 0) {new_g = 0;}
    if (new_g > 255) {new_g = 255;}
    pix[4*i + 2] = new_g;

    int new_b = ((double)pix[4*i + 3]) * (1.0 - fa) + b * fa;
    if (new_b < 0) {new_b = 0;}
    if (new_b > 255) {new_b = 255;}
    pix[4*i + 3] = new_b;
}

void simulation_addMapOffset(double x, double y) {
    if (!renderTransformGrid)
        renderTransformGrid = transform_createNewGrid(
            renderTransformGridX, renderTransformGridY);
    transform_addRenderOffset(renderTransformGrid, x, y);
}

void simulation_resetMapOffset() {
    if (!renderTransformGrid)
        renderTransformGrid = transform_createNewGrid(
            renderTransformGridX, renderTransformGridY);
    transform_resetRenderOffset(renderTransformGrid);
}

void simulation_setMapZoom(double zoom) {
    if (!renderTransformGrid)
        renderTransformGrid = transform_createNewGrid(
            renderTransformGridX, renderTransformGridY);
    transform_setRenderScale(renderTransformGrid, zoom);
}

void simulation_unlockSurface() {
    assert(simulation_surface_locked == 1);
    SDL_UnlockSurface(images_simulation_image);
    simulation_surface_locked = 0;
}



#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_image.h>

#include "particle.h"

SDL_Surface *images_simulation_image = NULL;
SDL_Texture *images_simulation_3d_image = NULL;
static int simulation_screen_width = 0;
static int simulation_screen_height = 0;

void images_init_simulation_image(int screen_width, int screen_height) {
    if (images_simulation_image) {
        if (screen_width == simulation_screen_width &&
                screen_height == simulation_screen_height) {
            return;
        }
        SDL_FreeSurface(images_simulation_image);
        images_simulation_image = NULL;
    }
    printf("CREATING IMAGE\n");
    simulation_screen_width = screen_width;
    simulation_screen_height = screen_height;
    images_simulation_image = SDL_CreateRGBSurface(
        0, screen_width, screen_height, 32,
        0xff000000,
        0x00ff0000,
        0x0000ff00,
        0x000000ff
    );
    assert(images_simulation_image->format->BitsPerPixel == 32);
}

void images_simulation_3d_clear() {
    // FIXME
    assert(0);  // FIXME implement this
}

void images_simulation_2d_to_3d_upload() {
    int texture_pitch = 0;
    void **texptr;
    SDL_LockTexture(
        images_simulation_3d_image,
        0, (void**)&texptr,
        &texture_pitch);
    assert(texture_pitch == 0);
    memcpy(texptr, images_simulation_image->pixels,
        images_simulation_image->w *
        images_simulation_image->h *
        4);
    SDL_UnlockTexture(images_simulation_3d_image);

    // FIXME !!!!
    assert(0);   // FIXME: set render to texture target
}

void images_simulation_3d_to_2d_download() {
    int texture_pitch = 0;
    void **texptr;
    SDL_LockTexture(
        images_simulation_3d_image,
        0, (void**)&texptr,
        &texture_pitch);
    assert(texture_pitch == 0);
    memcpy(images_simulation_image->pixels, texptr,
        images_simulation_image->w *
        images_simulation_image->h *
        4);
    SDL_UnlockTexture(images_simulation_3d_image); 
}

void images_simulation_3d_to_2d_blit_ontop() {
    int texture_pitch = 0;
    void **texptr;
    SDL_LockTexture(
        images_simulation_3d_image,
        0, (void**)&texptr,
        &texture_pitch);
    assert(texture_pitch == 0);
    memcpy(images_simulation_image->pixels, texptr,
        images_simulation_image->w *
        images_simulation_image->h *
        4);
    SDL_UnlockTexture(images_simulation_3d_image);

    // FIXME !!!!
    assert(0); /// FIXME implement blit on top process !!!
}

SDL_Surface *image_load_converted(const char *path, int alpha) {
    SDL_Surface *original = IMG_Load(path);
    if (!original) {
        return NULL;
    }
    SDL_Surface *converted;
    if (alpha) {
        // convert to 32bit argb:
        converted = SDL_ConvertSurfaceFormat(
            original, SDL_PIXELFORMAT_RGBA8888, 0);
        if (!converted) {
            SDL_FreeSurface(original);
            return NULL;
        }
        assert(SDL_ISPIXELFORMAT_ALPHA(converted->format->format));
        assert(converted->format->BitsPerPixel == 32);
    } else {
        // convert to 24bit rgb:
        converted = SDL_ConvertSurfaceFormat(
            original, SDL_PIXELFORMAT_RGB888, 0);
        if (!converted) {
            SDL_FreeSurface(original);
            return NULL;
        }
        assert(!SDL_ISPIXELFORMAT_ALPHA(converted->format->format));

        // Sadly, SDL seems to fail this conversion in current versions.
        // If SDL failed us, correct the epic failure >:-( and do it manually:
        if (converted->format->BitsPerPixel == 32) {
            SDL_FreeSurface(original);
            char *fixed_pixels = malloc(3 * converted->w * converted->h);
            if (!fixed_pixels) {
                SDL_FreeSurface(converted);
                return NULL;
            }
            if (SDL_LockSurface(converted)) {
                free(fixed_pixels);
                SDL_FreeSurface(converted);
                return NULL;
            }
            char *p = (converted->pixels);
            for (int i = 0; i < converted->w * converted->h; i++) {
                int target_index = 3 * i;
                int source_index = 4 * i;
                fixed_pixels[target_index + 0] =
                    p[source_index + 0];
                fixed_pixels[target_index + 1] =
                    p[source_index + 1];
                fixed_pixels[target_index + 2] =
                    p[source_index + 2];
            }
            SDL_UnlockSurface(converted);
            SDL_Surface *converted2 = SDL_CreateRGBSurface(
                0, converted->w, converted->h, 24,
                0xff000000,
                0x00ff0000,
                0x0000ff00,
                0x00000000
            );
            SDL_FreeSurface(converted);
            if (!converted2) {
                free(fixed_pixels);
                return NULL;
            }
            if (SDL_LockSurface(converted2)) {
                SDL_FreeSurface(converted2);
                free(fixed_pixels);
                return NULL;
            }
            memcpy(converted2->pixels, fixed_pixels, 3 * converted2->w *
                converted2->h);
            SDL_UnlockSurface(converted2);
            return converted2;
        }
    }
    SDL_FreeSurface(original);
    return converted;
}
char *image_load_raw(const char *path, int alpha, int *width, int *height) {
    SDL_Surface *srf = image_load_converted(path, alpha);
    if (!srf) {
        return NULL;
    }
    SDL_UnlockSurface(srf);
    char *raw_data = malloc((3 + alpha) * srf->w * srf->h);
    memcpy(raw_data, srf->pixels, (3 + alpha) * srf->w * srf->h);
    SDL_LockSurface(srf);
    *width = srf->w;
    *height = srf->h;
    SDL_FreeSurface(srf);
    return raw_data;
}

SDL_Surface *images_duplicate(SDL_Surface *old) {
    return SDL_ConvertSurface(old, old->format, 0);
}

SDL_Surface *grass;

char *raw_gradient_data = NULL;
int gradient_x = 0;
int gradient_y = 0;
static int images_initialized = 0;
void images_init() {
    if (images_initialized) return;

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    // load gradient image:
    raw_gradient_data = image_load_raw("images/gradient.png", 0, &gradient_x,
        &gradient_y);
    if (!raw_gradient_data) goto images_init_error;

    grass = image_load_converted("images/grass.png", 1);

    // load particle images:
    if (!particle_loadImage(PARTICLE_GRASS, "images/grass.png")) {
        goto images_init_error;
    }
    if (!particle_loadImage(PARTICLE_CAR, "images/car.png")) {
        goto images_init_error;
    }

    images_initialized = 1;
    return;
images_init_error:
    printf("[images] loading images failed. are you in the correct folder?\n");
    exit(1);
}


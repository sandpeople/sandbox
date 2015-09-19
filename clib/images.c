
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_image.h>

SDL_Surface *images_simulation_image = NULL;
static int simulation_screen_width = 0;
static int simulation_screen_height = 0;

void image_init_simulation_image(int screen_width, int screen_height) {
    if (images_simulation_image) {
        if (screen_width == simulation_screen_width &&
                screen_height == simulation_screen_height) {
            return;
        }
        SDL_FreeSurface(images_simulation_image);
        images_simulation_image = NULL;
    }
    simulation_screen_width = screen_width;
    simulation_screen_height = screen_height;
    images_simulation_image = SDL_CreateRGBSurface(
        0, screen_width, screen_height, 24,
        0xff000000,
        0x00ff0000,
        0x0000ff00,
        0x000000ff
    );
}

SDL_Surface *image_load_converted(const char *path, int alpha) {
    SDL_Surface *original = IMG_Load(path);
    if (!original) {
        return NULL;
    }
    SDL_Surface *converted;
    if (alpha) {
        converted = SDL_ConvertSurfaceFormat(
            original, SDL_PIXELFORMAT_BGRA8888, 0);
    } else {
        converted = SDL_ConvertSurfaceFormat(
            original, SDL_PIXELFORMAT_BGR888, 0);
    }
    SDL_FreeSurface(original);
    return converted;
}
char *image_load_raw(const char *path, int alpha, int *width, int *height) {
    SDL_Surface *srf = image_load_converted(path, alpha);
    SDL_UnlockSurface(srf);
    char *raw_data = malloc((3 + alpha) * srf->w * srf->h);
    memcpy(raw_data, srf->pixels, (3 + alpha) * srf->w * srf->h);
    SDL_LockSurface(srf);
    *width = srf->w;
    *height = srf->h;
    SDL_FreeSurface(srf);
    return raw_data;
}

char *raw_gradient_data = NULL;
int gradient_x, gradient_y;
static int images_initialized = 0;
void images_init() {
    if (images_initialized) return;

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    // load gradient image:
    raw_gradient_data = image_load_raw("images/gradient.png", 0, &gradient_x,
        &gradient_y);
    if (!raw_gradient_data) goto images_init_error;

    images_initialized = 1;
    return;
images_init_error:
    printf("[images] loading images failed. are you in the correct folder?\n");
    exit(1);
}


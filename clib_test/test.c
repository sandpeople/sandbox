
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "interface.h"
#include "random.h"

int main(int args, const char **argsv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("OpenCV OUTPUT",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, 480, 640, 0);
    SDL_Surface *screen = SDL_GetWindowSurface(window);
    SDL_Surface *to_screen = SDL_CreateRGBSurface(
        0, 480, 640, 32,
        0xff000000,
        0x00ff0000,
        0x0000ff00,
        0x000000ff
    );

    char *test_output = malloc(480 * 640 * 3);
    memset(test_output, 0, 480 * 640 * 3);
    uint8_t *test_input = malloc(480 * 640 * 3);
    memset(test_input, 0, 480 * 640);
    for (int i = 0; i < 480 * 640; i++) {
        assert(rand0to1() <= 1.0);
        test_input[i] = (int)(155.0 * rand0to1() + 70.0);
    }
    int f = 0;
    while (1) {
        printf("frame %d\n", f);
        f++;
        run_simulation(test_input, 480, 640, test_output);
        SDL_LockSurface(to_screen);
        for (int i = 0; i < 480 * 640; i++) {
            int offset1 = i * 4;
            int offset2 = i * 3;
            char *pix = (char *)to_screen->pixels;
            pix[offset1 + 1] = test_output[offset2 + 0];
            pix[offset1 + 2] = test_output[offset2 + 1];
            pix[offset1 + 3] = test_output[offset2 + 2];
            pix[offset1 + 0] = 255;
        }
        SDL_UnlockSurface(to_screen);
        SDL_BlitSurface(to_screen, NULL, screen, NULL);
        SDL_UpdateWindowSurface(window);
    } 
}


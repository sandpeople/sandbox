
#ifndef _SANDBOX_PARTICLE_H_
#define _SANDBOX_PARTICLE_H_

#include <SDL2/SDL.h>

#define PARTICLE_GRASS 0
#define PARTICLE_CAR 1
#define PARTICLE_TYPE_COUNT 2

#define PARTICLE_BELOW_WATER 1

// overall particle management:
int particle_loadImage(int type, const char *path);
void particle_wipeAll(int type);
void particle_renderToSurface(int type, SDL_Surface *srf);
void particle_renderAllToSurface(int from_type, int to_type, SDL_Surface *srf);
void particle_updateAll(void);

// managing particle instances:
struct particle_instance;
void particle_remove(struct particle_instance* inst);
struct particle_instance* particle_add(int type, double x, double y,
    double angle);
void particle_move(struct particle_instance* inst, double x, double y);

// convenience functions for particle spawning:
struct particle_instance *particle_addRandom(int type);
void particle_addRandomCrowd(int type, int amount);

#endif  // _SANDBOX_PARTICLE_H_


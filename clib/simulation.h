
#ifndef _SANDBOX_SIMULATION_H_
#define _SANDBOX_SIMULATION_H_

#include <stdint.h>

SDL_Renderer *simulation_getRenderer();
void simulation_initialize(int width, int height);
void simulation_drawBeforeWater();
void simulation_drawAfterWater();
void simulation_finalRenderToArray(uint8_t *render_data,
    int width, int height);
void simulation_addPixel(int i, int r, int g, int b, int a);
void simulation_lockSurface();
void simulation_unlockSurface();
int simulation_isSurfaceLocked();
void simulation_updateMovingObjects();
int simulation_getFluidUpdateCount();
void simulation_addMapOffset();
void simulation_resetMapOffset();
void simulation_setMapZoom(double z);

#endif  // _SANDBOX_SIMULATION_H_



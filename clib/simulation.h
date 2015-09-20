
#include <stdint.h>

void initialize_simulation();
void simulation_drawBeforeWater();
void simulation_drawAfterWater();
void simulation_finalRenderToArray(uint8_t *render_data,
    int width, int height);
void simulation_addPixel(int i, int r, int g, int b, int a);
void simulation_unlockSurface();
void simulation_updateMovingObjects();




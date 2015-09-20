
#define FLUID_WATER 0
#define FLUID_COUNT 1

void fluid_init(int xsize, int ysize);
void fluid_spawn(int type, int x, int y, double amount);
void fluid_updateAll(int x, int y);
void fluid_drawAllIfThere(int x, int y);
void fluid_randomSpawns();



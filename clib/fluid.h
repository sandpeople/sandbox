
#define FLUID_WATER 0
#define FLUID_COUNT 1

void fluid_init(int xsize, int ysize);
void fluid_spawn(int type, int x, int y, double amount);
void fluid_randomSpawns();
void fluid_drawAll(int xsize, int ysize);
void fluid_resetAll();
void fluid_waterColorAt(int x, int y,
        int *r, int *g, int *b);
double fluid_getCoverage(int type);
void fluid_autoDrain();



#include <glew.h>

struct grid {
    int nodesX, nodesY;
    double *nodesPosX;
    double *nodesPosY;   
};

struct grid *transform_createNewGrid(int nodesX, int nodesY) {

}

void transform_warp(struct grid *g, double sX, double sY, double tx, double tY, double strength);

void transform_reset(struct grid *g);

void transform_draw(struct grid *g, SDL_Texture *t) {
    // Prepare main window:
    glClear(GL_COLOR_BUFFER_BIT);

    
}


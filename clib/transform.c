
/*
 sandbox
 Copyright (C) 2016    CCCFr Sandbox Team

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "images.h"
#include "simulation.h"
#include "transform.h"

static uint32_t format = SDL_PIXELFORMAT_RGBA8888;

struct rendergrid {
    int nodesX, nodesY;
    double *nodesPosX;
    double *nodesPosY;
    double renderOffsetX, renderOffsetY, renderScale;
};

void transform_addRenderOffset(struct rendergrid *g,
        double x, double y) {
    g->renderOffsetX += x;
    g->renderOffsetY += y;
}

void transform_setRenderOffset(struct rendergrid *g,
        double x, double y) {
    g->renderOffsetX = x;
    g->renderOffsetY = y;
}

void transform_resetRenderOffset(struct rendergrid *g) {
    g->renderOffsetX = 0;
    g->renderOffsetY = 0;
}

void transform_setRenderScale(struct rendergrid *g, double scale) {
    g->renderScale = scale;
}

struct rendergrid *transform_createNewGrid(int nodesX, int nodesY) {
    struct rendergrid *g = malloc(sizeof(*g));
    memset(g, 0, sizeof(*g));
    g->renderScale = 1.0;
    g->nodesX = nodesX;
    g->nodesY = nodesY;
    return g;
}

void transform_warp(struct rendergrid *g, double sX, double sY,
        double tx, double tY, double strength) {

}

void transform_reset(struct rendergrid *g) {

}

static SDL_Texture *renderTempTarget = NULL;
void transform_draw(struct rendergrid *g, SDL_Texture *t) {
    // Prepare main window:
    //glClear(GL_COLOR_BUFFER_BIT);

    // Draw with rendergrid transform, map offset and zoom:
    if (!renderTempTarget) {
        renderTempTarget = SDL_CreateTexture(
            simulation_getRenderer(), format,
            SDL_TEXTUREACCESS_TARGET,
            images_simulation_image->w,
            images_simulation_image->h);
    }

    SDL_Rect rect;
    memset(&rect, 0, sizeof(rect));
    rect.x = -((int)(g->renderOffsetX + 0.5)) - (
        (images_simulation_image->w * g->renderScale) -
        images_simulation_image->w) * 0.5;
    rect.y = -((int)(g->renderOffsetY + 0.5)) - (
        (images_simulation_image->h * g->renderScale) -
        images_simulation_image->h) * 0.5;
    rect.w = ((double)images_simulation_image->w * g->renderScale);
    rect.h = ((double)images_simulation_image->h * g->renderScale);

    SDL_Texture *oldTarget = SDL_GetRenderTarget(simulation_getRenderer());
    SDL_SetRenderTarget(simulation_getRenderer(), renderTempTarget);

    // Render image with rendergrid and all transformations applied:
    SDL_RenderClear(simulation_getRenderer());
    if (SDL_RenderCopyEx(simulation_getRenderer(),
            t, NULL, &rect,
            0, NULL, SDL_FLIP_NONE) != 0) {
        fprintf(stderr, "SDL error in SDL_RenderCopy: %s\n",
            SDL_GetError());
        return;
    }

    // Render result to image_simulation_3d_image:
    SDL_SetRenderTarget(simulation_getRenderer(),
        images_simulation_3d_image);
    SDL_RenderClear(simulation_getRenderer());
    if (SDL_RenderCopy(simulation_getRenderer(),
            renderTempTarget, NULL, NULL) != 0) {
        fprintf(stderr, "SDL error in SDL_RenderCopy: %s\n",
            SDL_GetError());
        return;
    }

    SDL_SetRenderTarget(simulation_getRenderer(), oldTarget);

}


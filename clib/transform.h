
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

#ifndef _SANDBOX_TRANSFORM_H_
#define _SANDBOX_TRANSFORM_H_

#include <SDL2/SDL.h>

struct rendergrid;

struct rendergrid *transform_createNewGrid(int nodesX, int nodesY);

void transform_warp(struct rendergrid *g, double sX, double sY, double tx, double tY, double strength);

void transform_reset(struct rendergrid *g);

void transform_draw(struct rendergrid *g, SDL_Texture *t);


void transform_addRenderOffset(struct rendergrid *g,
    double x, double y);
void transform_setRenderOffset(struct rendergrid *g,
    double x, double y);
void transform_resetRenderOffset(struct rendergrid *g);
void transform_setRenderScale(struct rendergrid *g, double scale);

#endif  // _SANDBOX_TRANSFORM_H_


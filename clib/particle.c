
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "images.h"
#include "particle.h"
#include "random.h"
#include "simulation.h"
#include "topology.h"

struct particle_type {
    SDL_Texture *image;
    float base_scale;
};
struct particle_type ptypes[PARTICLE_TYPE_COUNT] = { 0 };

struct particle_instance {
    int type;
    double x;
    double y;
    double vx, vy;
    double angle;
    struct particle_instance *next, *prev;
};
struct particle_instance *plist[PARTICLE_TYPE_COUNT] = { 0 };

int particle_loadImage(int type, const char *path) {
    struct particle_type *ptype = &ptypes[type];
    if (ptype->image) {
        SDL_DestroyTexture(ptype->image);
    }
    SDL_Surface *img = image_load_converted(path, 1);
    if (!img) {
        fprintf(stderr,
            "[particle] image_load_converted fail, texture "
            "load failed\n");
        return 0;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(
        simulation_getRenderer(), img);
    SDL_FreeSurface(img);
    if (!tex) {
        fprintf(stderr,
            "[particle] SDL_CreateTextureFromSurface "
            "fail, texture "
            "load failed\n");
        return 0;
    }
    printf("[particle] Image loaded: %s\n", path);
    int w,h;
    if (SDL_QueryTexture(tex, NULL, NULL, &w, &h) != 0) {
        fprintf(stderr,
            "[particle] SDL_QueryTexture fail, texture "
            "load failed\n");
        SDL_DestroyTexture(tex);
        return 0;
    }
    printf("[particle] Image dimensions: %d, %d\n",
        w, h);
    ptype->image = tex;
    return 1; 
}

static void particle_free(struct particle_instance *inst) {
    free(inst);
}

void particle_remove(struct particle_instance *inst) {
    if (inst->prev) {
        inst->prev->next = inst->next;
    } else {
        plist[inst->type] = inst->next;
    }
    if (inst->next) {
        inst->next->prev = inst->prev;
    }
    particle_free(inst);
}

void particle_wipeAll(int type) {
    while (plist[type]) {
        struct particle_instance *nextp = plist[type]->next;
        particle_free(plist[type]);
        plist[type] = nextp;
    }
}

struct particle_instance *particle_add(int type, double x, double y,
        double angle) {
    struct particle_instance *inst = malloc(sizeof(struct particle_instance));
    memset(inst, 0, sizeof(*inst));
    inst->type = type;
    if (plist[type] != NULL) {
        inst->next = plist[type];
        plist[type]->prev = inst;
    }
    plist[type] = inst;
    inst->x = x;
    inst->y = y;
    inst->angle = angle;
    return inst;
}

struct particle_instance *particle_addRandom(int type) {
    double x = rand0to1();
    double y = rand0to1();
    return particle_add(type, x, y, rand0to1() * 360.0); 
}

void particle_addRandomCrowd(int type, int amount) {
    for (int i = 0; i < amount; i++) {
        particle_addRandom(type);
    }
}

void particle_move(struct particle_instance *inst, double x, double y) {
    inst->x = x;
    inst->y = y;        
}

void particle_render(int type) {
    struct particle_instance* inst = plist[type];
    SDL_Rect dest = {0};
    while (inst) {
        SDL_Texture *i = ptypes[type].image;
        assert(i != NULL);
        uint32_t format;
        int access;
        int w,h;
        assert(SDL_QueryTexture(
            i, &format, &access,
            &w, &h) == 0);
        double abs_pos_x = inst->x * ((double)images_simulation_image->w);
        double abs_pos_y = inst->y * ((double)images_simulation_image->h);
        dest.x = abs_pos_x;
        dest.y = abs_pos_y;
        if (type == PARTICLE_GRASS) {
            if (topology_scan_type(TOPOLOGY_GRASS, dest.x, dest.y, 15) < 0.5) {
                inst = inst->next;
                continue;
            }
        }
        dest.x -= w / 2;
        dest.y -= h / 2;
        dest.w = w;
        dest.h = h;
        SDL_RenderCopyEx(simulation_getRenderer(),
            i, NULL, &dest,
            inst->angle, NULL, SDL_FLIP_NONE);
        inst = inst->next;
    }
}

void particle_update(struct particle_instance *inst) {
    if (inst->type == PARTICLE_CAR) {
        double abs_pos_x = inst->x * ((double)topology_map_x);
        double abs_pos_y = inst->y * ((double)topology_map_y);
        int pixel_pos_x = abs_pos_x;
        int pixel_pos_y = abs_pos_y;

        double drift_x = 0;
        double drift_y = 0;
        topology_calculate_drift(pixel_pos_x, pixel_pos_y,
            &drift_x, &drift_y);
 
        double move_x = 1.0 / ((double)topology_map_x);
        double move_y = 1.0 / ((double)topology_map_y);

        inst->vx += drift_x * move_x * 0.005;
        inst->vy += drift_y * move_y * 0.005;
        inst->vx *= 0.95;
        inst->vy *= 0.95;

        double vmax = 8.0;
        if (inst->vx > vmax) {
            inst->vx = vmax;
        }
        if (inst->vy > vmax) {
            inst->vy = vmax;
        }
        if (inst->vx < -vmax) {
            inst->vx = -vmax;
        }
        if (inst->vy < -vmax) {
            inst->vy = -vmax;
        }

        inst->x += inst->vx * 5;
        inst->y += inst->vy * 5;
    }
}

void particle_updateAll(void) {
    for (int i = 0; i < PARTICLE_TYPE_COUNT; i++) {
        struct particle_instance *inst = plist[i];
        while (inst) {
            struct particle_instance *ninst = inst->next;
            particle_update(inst);
            inst = ninst;
        }
    }
}

void particle_renderAll(int from_type, int to_type) {
    SDL_SetRenderTarget(simulation_getRenderer(), images_simulation_3d_image);
    if (to_type <= from_type) return;
    for (int i = from_type; i < to_type; i++) {
        particle_render(i);
    }
    SDL_RenderPresent(simulation_getRenderer());
    SDL_SetRenderTarget(simulation_getRenderer(), NULL);
}



#include <assert.h>
#include <stdio.h>

#include "images.h"
#include "particle.h"
#include "random.h"
#include "topology.h"

struct particle_type {
    SDL_Surface *image;
    float base_scale;
};
struct particle_type ptypes[PARTICLE_TYPE_COUNT] = { 0 };

struct particle_instance {
    int type;
    double x;
    double y;
    struct particle_instance *next, *prev;
};
struct particle_instance *plist[PARTICLE_TYPE_COUNT] = { 0 };

int particle_loadImage(int type, const char *path) {
    struct particle_type *ptype = &ptypes[type];
    if (ptype->image) {
        SDL_FreeSurface(ptype->image);
        ptype->image = NULL;
    }
    SDL_Surface *img = image_load_converted(path, 1);
    if (!img) {
        return 0;
    }
    ptype->image = img;
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

struct particle_instance *particle_add(int type, double x, double y) {
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
    return inst;
}

struct particle_instance *particle_addRandom(int type) {
    double x = rand0to1();
    double y = rand0to1();
    return particle_add(type, x, y); 
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

void particle_renderToSurface(int type, SDL_Surface *srf) {
    struct particle_instance* inst = plist[type];
    SDL_Rect dest = {0};
    while (inst) {
        double abs_pos_x = inst->x * ((double)srf->w);
        double abs_pos_y = inst->y * ((double)srf->h);
       
        dest.x = abs_pos_x;
        dest.y = abs_pos_y;
        if (topology_scan_type(TOPOLOGY_GRASS, dest.x, dest.y, 5) < 0.5) {
            inst = inst->next;
            continue;
        }
        dest.x -= ptypes[type].image->w / 2;
        dest.y -= ptypes[type].image->h / 2;
        SDL_BlitSurface(ptypes[type].image, NULL,
            srf, &dest);
        inst = inst->next;
    }
}

void particle_renderAllToSurface(int from_type, int to_type, SDL_Surface *srf) {
    if (to_type <= from_type) return;
    for (int i = from_type; i < to_type; i++) {
        particle_renderToSurface(i, srf);
    }
}


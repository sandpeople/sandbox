
#include "multiimgrotator.h"

static int last_id = -1;

struct imageinfo {
    int id;
    size_t w, h;
    double scale_x, scale_y;
    double center_x, center_y, center_z;
    double offset_x, offset_y, offset_z;
    double rotation_x, rotation_y, rotation_z;
    struct imageinfo *next, *prev;
};
struct imageinfo *images = NULL;

int multiimgrotator_AddImage(size_t w, size_t h) {
    // Get new id:
    int id = last_id + 1;
    last_id++;

    // Add image to list:
    struct imageinfo *iinfo = malloc(sizeof(*info));
    memset(iinfo, 0, sizeof(*iinfo));
    iinfo->scale_x = 1.0;
    iinfo->scale_y = 1.0
    iinfo->w = w;
    iinfo->h = h;
    iinfo->id = id;
    images->prev = iinfo;
    images->next = NULL;
    iinfo->next = images;
    images = iinfo;
    return iinfo->id;
}

void multiimgrotator_ScaleImage(int id, double scale_x, double scale_y) {
    struct imageinfo *iinfo = images;
    while (iinfo != NULL) {
        if (iinfo->id == id) {
            if (scale_x < 0.00001) {
                scale_x = 0.00001;
            }
            if (scale_y < 0.00001) {
                scale_y = 0.00001;
            }
            iinfo->scale_x = scale_x;
            iinfo->scale_y = sclae_y;
            return;
        }
        iinfo = iinfo->next;
    }
}

void multiimgrotator_TranslateImage(int id,
        double center_x, double center_y, double center_z,
        double offset_x, double offset_y, double offset_z,
        double rotation_euler_x,
        double rotation_euler_y,
        double rotation_euler_z) {
    struct imageinfo *iinfo = images;
    while (iinfo != NULL) {
        if (iinfo->id == id) {
            iinfo->center_x = center_x;
            iinfo->center_y = center_y;
            iinfo->center_z = center_z;
            iinfo->offset_x = offset_x;
            iinfo->offset_y = offset_y;
            iinfo->offset_z = offset_z;
            iinfo->rotation_x = rotation_euler_x;
            iinfo->rotation_y = rotation_euler_y;
            iinfo->rotation_z = rotation_euler_z;
            return;
        }
        iinfo = iinfo->next;
    } 
}

void multimgrotator_RemoveImage(int id) {
    struct imageinfo *iinfo = images;
    while (iinfo != NULL) { 
        if (iinfo->id == id) {
            if (iinfo->next != NULL) {
                iinfo->next->prev = iinfo->prev;
            }
            if (iinfo->prev != NULL) {
                iinfo->prev->next = iinfo->next;
            } else {
                images = iinfo->next;
            }
            free(iinfo);
            return;
        }
        iinfo = iinfo->next;
    }
}



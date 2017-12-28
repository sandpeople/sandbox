
#ifndef MULTIIMGROTATOR_H_
#define MULTIIMGROTATOR_H_

#include <SDL2/SDL.h>

int multiimgrotator_AddImage(size_t w, size_t h);

void multiimgrotator_ScaleImage(int id, double scale_x, double scale_y);

void multiimgrotator_TranslateImage(int id,
        double center_x, double center_y, double center_z,
        double offset_x, double offset_y, double offset_z,
        double rotation_euler_x,
        double rotation_euler_y,
        double rotation_euler_z);

void multiimgrotator_RemoveImage(int id);

void multiimgrotator_Draw();

#endif  // MULTIIMGROTATOR_H_


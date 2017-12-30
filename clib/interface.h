
#ifndef CLIB_INTERFACE_H_
#define CLIB_INTERFACE_H_

void interface_run(const void *depth_array_v, void *output_colors_v);

void interface_mapOffset(double x, double y);

void interface_resetWater();

void interface_stop();

void interface_setInputAmount(int amount);

struct inputconfig {
    size_t w,h;
    double world_x, world_y, world_z;
    double size_w, size_h;
    double rotation_x, rotation_y, rotation_z;
    double height_shift, height_scale;
};

void interface_setInputConfig(int number, const struct inputconfig* config);

void interface_setInputImg(int number, const void *data,
    int columns_rows_swapped);

#endif  // CLIB_INTERFACE_H_



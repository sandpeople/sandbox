
#ifndef CLIB_VMATH_H_
#define CLIB_VMATH_H_

void vmath_rotatePos(double x, double y, double z,
        double euler_x, double euler_y, double euler_z,
        double *result_x, double *result_y, double *result_z);

#endif  // CLIB_VMATH_H_


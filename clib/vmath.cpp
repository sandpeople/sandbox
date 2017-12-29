
extern "C" {
#include "vmath.h"
}

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/euler_angles.hpp>

extern "C" {

/// Rotates a point around the origin with the given euler angles,
/// and returns the resulting new point.
/// Euler order: pitch, yaw, roll
void vmath_rotatePos(double x, double y, double z,
        double euler_x, double euler_y, double euler_z,
        double *result_x, double *result_y, double *result_z) {
    glm::dmat4 rot_transform = glm::eulerAngleXYZ(euler_x, euler_y, euler_z);
    glm::dvec4 pos = glm::dvec4(0.0, x, y, z);
    pos = rot_transform * pos;
    *result_x = pos.x;
    *result_y = pos.y;
    *result_z = pos.z;
}

}  // extern "C"


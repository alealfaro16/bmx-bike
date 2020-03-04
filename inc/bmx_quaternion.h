/// \file quaternion.c
/// \brief A file to perform quaternion math. It is meant to be used with the
/// BNO055 IMU driver.

#ifndef BMX_QUATERNION_HG
#define BMX_QUATERNION_HG

#include "bno055.h"

/// \brief Calculate the magnitude of a quaternion.
/// \param *q: a pointer to a quaternion
/// \return the magnitude
float magnitude(struct bno055_quaternion_t *q);

/// \brief Normalize a quaternion.
/// \param *q: a pointer to a quaternion
void normalize(struct bno055_quaternion_t *q);

/// \brief Scale down a quaternion through division.
/// \param *q: a pointer to a quaternion
/// \param val: the number to divde by
void scale_divide(struct bno055_quaternion_t *q, float val);

/// \brief Calculate the corresponding euler angles of a quaternion.
/// \param *q: a pointer to a quaternion
/// \return ea: a struct containing euler angles
struct bno055_euler_float_t toEuler(struct bno055_quaternion_t *q);

#endif

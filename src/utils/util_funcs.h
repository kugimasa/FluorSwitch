#ifndef RTCAMP2022_SRC_UTILS_UTIL_FUNCS_H_
#define RTCAMP2022_SRC_UTILS_UTIL_FUNCS_H_

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

#include "ray.h"
#include "vec3.h"

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
  return degrees * PI / 180.0;
}

inline double clamp(double x, double min, double max) {
  if (x < min) return min;
  if (x > max) return max;
  return x;
}

// [0,1)の値をランダムで返す
inline double random_double() {
  return rand() / (RAND_MAX + 1.0);
}

// [min,max)のdoubleをランダムで返す
inline double random_double(double min, double max) {
  return min + (max - min) * random_double();
}

// [min,max)のintをランダムで返す
inline int random_int(int min, int max) {
  return static_cast<int>(random_double(min, max + 1));
}

#endif //RTCAMP2022_SRC_UTILS_UTIL_FUNCS_H_

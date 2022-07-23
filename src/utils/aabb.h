// Using the code of Ray Tracing: The Next Week
// https://raytracing.github.io/books/RayTracingTheNextWeek.html

#ifndef RTCAMP2022_SRC_UTILS_AABB_H_
#define RTCAMP2022_SRC_UTILS_AABB_H_

#include "util_funcs.h"

inline double ffmin(double a, double b) { return a < b ? a : b; }
inline double ffmax(double a, double b) { return a > b ? a : b; }

class aabb {
 public:
  aabb() {}
  aabb(const vec3 &a, const vec3 &b) {
    minimum = a;
    maximum = b;
  }

  vec3 min() const { return minimum; }
  vec3 max() const { return maximum; }

  // 交差判定
  bool hit(const ray &r, double tmin, double tmax) const {
    for (int i = 0; i < 3; i++) {
      double invD = 1.0 / r.direction()[i];
      double t0 = (min()[i] - r.origin()[i]) * invD;
      double t1 = (max()[i] - r.origin()[i]) * invD;
      // 反転
      if (invD < 0.0) std::swap(t0, t1);
      tmin = ffmax(t0, tmin);
      tmax = ffmin(t1, tmax);
      if (tmax <= tmin) {
        return false;
      }
    }
    return true;
  }

  vec3 minimum{ZERO_VEC3};
  vec3 maximum{ZERO_VEC3};
};

aabb surrounding_box(aabb box0, aabb box1) {
  vec3 small(ffmin(box0.min().x(), box1.min().x()),
             ffmin(box0.min().y(), box1.min().y()),
             ffmin(box0.min().z(), box1.min().z()));

  vec3 big(ffmax(box0.max().x(), box1.max().x()),
           ffmax(box0.max().y(), box1.max().y()),
           ffmax(box0.max().z(), box1.max().z()));
  return aabb(small, big);
}

#endif //RTCAMP2022_SRC_UTILS_AABB_H_

// Using the code of Ray Tracing in One Weekend
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RAY_UTILS_HITABLE_H_
#define RAY_UTILS_HITABLE_H_

#include "util_funcs.h"
#include "aabb.h"

class material;

struct hit_record {
  float t;
  vec3 p;
  vec3 normal;
  shared_ptr<material> mat_ptr;
  double u;
  double v;
};

class hitable {
 public:
  virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const = 0;
  virtual bool bounding_box(float t0, float t1, aabb &box) const = 0;
};

#endif //RAY_UTILS_HITABLE_H_

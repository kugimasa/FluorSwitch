#ifndef RTCAMP2022_SRC_OBJECTS_AARECT_H_
#define RTCAMP2022_SRC_OBJECTS_AARECT_H_

#include "../utils/util_funcs.h"
#include "../utils/hitable.h"

class xY_rect : public hitable {
 public:
  xY_rect() {}

  xY_rect(double x_0, double x_1, double y_0, double y_1, double z, shared_ptr<material> mat)
      : x0(x_0), x1(x_1), y0(y_0), y1(y_1), k(z), mp(mat) {};

  bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override;
  bool bounding_box(float time0, float time1, aabb &box) const override {
    box = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
    return true;
  }

 public:
  shared_ptr<material> mp;
  double x0, x1, y0, y1, k;
};

bool xY_rect::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
  auto t = (k - r.origin().z()) / r.direction().z();
  if (t < t_min || t_max < t) {
    return false;
  }
  auto x = r.origin().x() + t * r.direction().x();
  auto y = r.origin().y() + t * r.direction().y();
  if (x < x0 || x1 < x || y < y0 || y1 < y) {
    return false;
  }
  rec.u = (x - x0) / (x1 - x0);
  rec.v = (y - y0) / (y1 - y0);
  rec.t = t;
  auto outward_normal = vec3(0, 0, 1);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mp;
  rec.p = r.point_at_parameter(t);
  return true;
}

#endif //RTCAMP2022_SRC_OBJECTS_AARECT_H_

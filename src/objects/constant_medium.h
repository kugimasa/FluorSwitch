#ifndef RTCAMP2022_SRC_OBJECTS_CONSTANT_MEDIUM_H_
#define RTCAMP2022_SRC_OBJECTS_CONSTANT_MEDIUM_H_

#include "../utils/hittable.h"
#include "../material/material.h"

class constant_medium : public hittable {
 public:
  constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
      : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(make_shared<isotropic>(a)) {}

  constant_medium(shared_ptr<hittable> b, double d, color c)
      : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(make_shared<isotropic>(c)) {}

  bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;

  bool bounding_box(double t0, double t1, aabb &box) const override {
    return boundary->bounding_box(t0, t1, box);
  }

 public:
  shared_ptr<hittable> boundary;
  shared_ptr<material> phase_function;
  double neg_inv_density;
};

bool constant_medium::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {

  hit_record rec1, rec2;

  if (!boundary->hit(r, -INF, INF, rec1)) {
    return false;
  }
  if (!boundary->hit(r, rec1.t + 0.001, INF, rec2)) {
    return false;
  }

  if (rec1.t < t_min) rec1.t = t_min;
  if (rec2.t > t_max) rec2.t = t_max;

  if (rec1.t >= rec2.t) {
    return false;
  }

  if (rec1.t < 0) {
    rec1.t = 0;
  }

  const auto ray_length = r.direction().length();
  const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
  const auto hit_distance = neg_inv_density * log(random_double());

  if (hit_distance > distance_inside_boundary) {
    return false;
  }

  // レイを進める
  rec.t = rec1.t + hit_distance / ray_length;
  rec.p = r.point_at_parameter(rec.t);
  // 法線情報は任意な方向
  rec.normal = X_UP;
  rec.front_face = true;
  rec.mat_ptr = phase_function;

  return true;
}

#endif //RTCAMP2022_SRC_OBJECTS_CONSTANT_MEDIUM_H_

// Using the code of Ray Tracing in One Weekend
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RAY_UTILS_HITTABLE_H_
#define RAY_UTILS_HITTABLE_H_

#include "util_funcs.h"
#include "aabb.h"

class material;

template<typename mat>
struct hit_record {
  double t;
  vec3 p;
  vec3 normal;
  shared_ptr<mat> mat_ptr;
  double u;
  double v;
  bool front_face;

  inline void set_face_normal(const ray &r, const vec3 &outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

template<typename mat>
class hittable {
 public:
  virtual bool hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const = 0;
  virtual bool bounding_box(double t0, double t1, aabb &box) const = 0;
  virtual double pdf_value(const point3 &o, const vec3 &v) const {
    return 0.0;
  }

  virtual vec3 random(const vec3 &o) const {
    return X_UP;
  }
};

template<typename mat>
class translate : public hittable<mat> {
 public:
  translate(shared_ptr<hittable<mat>> p, const vec3 &displacement) : ptr(p), offset(displacement) {}

  bool hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const override;
  bool bounding_box(double time0, double time1, aabb &output_box) const override;

 public:
  shared_ptr<hittable<mat>> ptr;
  vec3 offset;
};

template<typename mat>
bool translate<mat>::hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const {
  ray moved_r(r.origin() - offset, r.direction(), r.time());
  if (!ptr->hit(moved_r, t_min, t_max, rec)) {
    return false;
  }

  rec.p += offset;
  rec.set_face_normal(moved_r, rec.normal);

  return true;
}

template<typename mat>
bool translate<mat>::bounding_box(double time0, double time1, aabb &output_box) const {
  if (!ptr->bounding_box(time0, time1, output_box)) {
    return false;
  }

  output_box = aabb(output_box.min() + offset, output_box.max() + offset);
  return true;
}

template<typename mat>
class rotate_y : public hittable<mat> {
 public:
  rotate_y(shared_ptr<hittable<mat>> p, double angle);

  bool hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const override;
  bool bounding_box(double t0, double t1, aabb &box) const override {
    box = bbox;
    return has_box;
  }

 public:
  shared_ptr<hittable<mat>> ptr;
  double sin_theta;
  double cos_theta;
  bool has_box;
  aabb bbox;
};

template<typename mat>
rotate_y<mat>::rotate_y(shared_ptr<hittable<mat>> p, double angle) : ptr(p) {
  auto radians = degrees_to_radians(angle);
  sin_theta = sin(radians);
  cos_theta = cos(radians);
  has_box = ptr->bounding_box(0, 1, bbox);

  point3 min(INF, INF, INF);
  point3 max(-INF, -INF, -INF);

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < 2; ++k) {
        auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
        auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
        auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

        auto newx = cos_theta * x + sin_theta * z;
        auto newz = -sin_theta * x + cos_theta * z;

        vec3 tester(newx, y, newz);

        for (int c = 0; c < 3; ++c) {
          min[c] = fmin(min[c], tester[c]);
          max[c] = fmax(max[c], tester[c]);
        }
      }
    }
  }

  bbox = aabb(min, max);
}

template<typename mat>
bool rotate_y<mat>::hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const {
  auto origin = r.origin();
  auto direction = r.direction();

  origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
  origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

  direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
  direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

  ray rotated_r(origin, direction, r.time());

  if (!ptr->hit(rotated_r, t_min, t_max, rec)) {
    return false;
  }

  auto p = rec.p;
  auto normal = rec.normal;

  p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
  p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

  normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
  normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

  rec.p = p;
  rec.set_face_normal(rotated_r, normal);

  return true;
}

template<typename mat>
class flip_face : public hittable<mat> {
 public:
  flip_face(shared_ptr<hittable<mat>> p) : ptr(p) {}

  bool hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const override {
    if (!ptr->hit(r, t_min, t_max, rec)) {
      return false;
    }

    rec.front_face = !rec.front_face;
    return true;
  }

  bool bounding_box(double t0, double t1, aabb &box) const override {
    return ptr->bounding_box(t0, t1, box);
  }

 public:
  shared_ptr<hittable<mat>> ptr;
};

#endif //RAY_UTILS_HITTABLE_H_

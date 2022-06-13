#ifndef RTCAMP2022_SRC_OBJECTS_AARECT_H_
#define RTCAMP2022_SRC_OBJECTS_AARECT_H_

#include "../utils/util_funcs.h"
#include "../utils/hittable.h"

void get_aa_rect_uv(const vec3 &origin, const vec3 &end,
                    const vec3 &p,
                    const vec3 &s, const vec3 &t,
                    double &u, double &v) {
  auto pos = p - origin;
  auto n = cross(s, t);
  u = dot(pos, s) / dot(end, n);
  v = dot(pos, t) / dot(end, n);
}

/// XY面
class xy_rect : public hittable {
 public:
  xy_rect() {}

  xy_rect(double x_0, double x_1, double y_0, double y_1, double z, shared_ptr<material> mat)
      : x0(x_0), x1(x_1), y0(y_0), y1(y_1), k(z), mp(mat), tex_o(x_0, y_0, z), tex_e(x_1, y_1, z) {};

  bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;
  bool bounding_box(double time0, double time1, aabb &box) const override {
    box = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
    return true;
  }

 public:
  shared_ptr<material> mp;
  double x0, x1, y0, y1, k;
  point3 tex_o, tex_e;
};

bool xy_rect::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
  auto t = (k - r.origin().z()) / r.direction().z();
  if (t < t_min || t_max < t) {
    return false;
  }
  auto x = r.origin().x() + t * r.direction().x();
  auto y = r.origin().y() + t * r.direction().y();
  if (x < x0 || x1 < x || y < y0 || y1 < y) {
    return false;
  }
  rec.t = t;
  auto outward_normal = vec3(0, 0, 1);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mp;
  rec.p = r.point_at_parameter(t);
  get_aa_rect_uv(tex_o, tex_e, rec.p, X_UP, Y_UP, rec.u, rec.v);
  return true;
}

/// XZ面
class xz_rect : public hittable {
 public:
  xz_rect() {}

  xz_rect(double x_0, double x_1, double z_0, double z_1, double y, shared_ptr<material> mat)
      : x0(x_0), x1(x_1), z0(z_0), z1(z_1), k(y), mp(mat) {};

  bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;
  bool bounding_box(double time0, double time1, aabb &box) const override {
    box = aabb(point3(x0, k - 0.0001, z0), point3(x1, k + 0.0001, z1));
    return true;
  }

 public:
  shared_ptr<material> mp;
  double x0, x1, z0, z1, k;
};

bool xz_rect::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
  auto t = (k - r.origin().y()) / r.direction().y();
  if (t < t_min || t_max < t) {
    return false;
  }
  auto x = r.origin().x() + t * r.direction().x();
  auto z = r.origin().z() + t * r.direction().z();
  if (x < x0 || x1 < x || z < z0 || z1 < z) {
    return false;
  }
  rec.u = (x - x0) / (x1 - x0);
  rec.v = (z - z0) / (z1 - z0);
  rec.t = t;
  auto outward_normal = vec3(0, 1, 0);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mp;
  rec.p = r.point_at_parameter(t);
  return true;
}

/// YZ面
class yz_rect : public hittable {
 public:
  yz_rect() {}

  yz_rect(double y_0, double y_1, double z_0, double z_1, double x, shared_ptr<material> mat)
      : y0(y_0), y1(y_1), z0(z_0), z1(z_1), k(x), mp(mat) {};

  bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;
  bool bounding_box(double time0, double time1, aabb &box) const override {
    box = aabb(point3(k - 0.0001, y0, z0), point3(k + 0.0001, y1, z1));
    return true;
  }

 public:
  shared_ptr<material> mp;
  double y0, y1, z0, z1, k;
};

bool yz_rect::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
  auto t = (k - r.origin().x()) / r.direction().x();
  if (t < t_min || t_max < t) {
    return false;
  }
  auto y = r.origin().y() + t * r.direction().y();
  auto z = r.origin().z() + t * r.direction().z();
  if (y < y0 || y1 < y || z < z0 || z1 < z) {
    return false;
  }
  rec.u = (y - y0) / (y1 - y0);
  rec.v = (z - z0) / (z1 - z0);
  rec.t = t;
  auto outward_normal = vec3(1, 0, 0);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mp;
  rec.p = r.point_at_parameter(t);
  return true;
}

#endif //RTCAMP2022_SRC_OBJECTS_AARECT_H_

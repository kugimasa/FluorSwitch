#ifndef RTCAMP2022_SRC_OBJECTS_AARECT_H_
#define RTCAMP2022_SRC_OBJECTS_AARECT_H_

#include "../utils/util_funcs.h"
#include "../utils/hittable.h"

/// XY面
class xy_rect : public hittable {
 public:
  xy_rect() {}

  xy_rect(double x_0, double x_1, double y_0, double y_1, double z, shared_ptr<material> mat)
      : x0(x_0), x1(x_1), y0(y_0), y1(y_1), k(z), mp(mat) {};

  bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;
  bool bounding_box(double time0, double time1, aabb &box) const override {
    box = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
    return true;
  }

  double pdf_value(const point3 &o, const vec3 &v) const override {
    hit_record rec;
    if (!this->hit(ray(o, v), 0.001, INF, rec)) {
      return 0;
    }

    auto area = (x1 - x0) * (y1 - y0);
    auto distance_squared = rec.t * rec.t * v.squared_length();
    auto cosine = fabs(dot(v, rec.normal) / v.length());

    return distance_squared / (cosine * area);
  }

  vec3 random(const vec3 &o) const override {
    auto random_point = point3(random_double(x0, x1), random_double(y0, y1), k);
    return random_point - o;
  }

 public:
  shared_ptr<material> mp;
  double x0, x1, y0, y1, k;
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
  rec.u = (x - x0) / (x1 - x0);
  rec.v = (y - y0) / (y1 - y0);
  rec.t = t;
  auto outward_normal = vec3(0, 0, 1);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mp;
  rec.p = r.point_at_parameter(t);
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

  double pdf_value(const point3 &o, const vec3 &v) const override {
    hit_record rec;
    if (!this->hit(ray(o, v), 0.001, INF, rec)) {
      return 0;
    }

    auto area = (x1 - x0) * (z1 - z0);
    auto distance_squared = rec.t * rec.t * v.squared_length();
    auto cosine = fabs(dot(v, rec.normal) / v.length());

    return distance_squared / (cosine * area);
  }

  vec3 random(const vec3 &o) const override {
    auto random_point = point3(random_double(x0, x1), k, random_double(z0, z1));
    return random_point - o;
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

  double pdf_value(const point3 &o, const vec3 &v) const override {
    hit_record rec;
    if (!this->hit(ray(o, v), 0.001, INF, rec)) {
      return 0;
    }

    auto area = (y1 - y0) * (z1 - z0);
    auto distance_squared = rec.t * rec.t * v.squared_length();
    auto cosine = fabs(dot(v, rec.normal) / v.length());

    return distance_squared / (cosine * area);
  }

  vec3 random(const vec3 &o) const override {
    auto random_point = point3(k, random_double(y0, y1), random_double(z0, z1));
    return random_point - o;
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

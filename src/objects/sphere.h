// Using the code of Ray Tracing in One Weekend
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RAY_OBJECTS_SPHERE_H_
#define RAY_OBJECTS_SPHERE_H_

#include "../utils/hitable.h"

class sphere : public hitable {
 public:
  sphere();
  sphere(vec3 cen, double r) : center(cen), radius(r) {};
  sphere(vec3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};
  virtual bool hit(const ray &r, double tmin, double tmax, hit_record &rec) const override;
  virtual bool bounding_box(double time0, double time1, aabb &box) const override;
  vec3 center;
  double radius;
  shared_ptr<material> mat_ptr;

 private:
  static void get_sphere_uv(const point3 &p, double &u, double &v) {
    auto theta = acos(-p.y());
    auto phi = atan2(-p.z(), p.x()) + PI;
    /// 回転あり
    u = phi / (2 * PI) + 0.1;
    v = theta / PI;
  }
};

bool sphere::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
  vec3 oc = r.origin() - center;
  double a = dot(r.direction(), r.direction());
  double b = dot(oc, r.direction());
  double c = dot(oc, oc) - radius * radius;
  double discriminant = b * b - a * c;
  if (discriminant > 0) {
    double temp = (-b - sqrt(b * b - a * c)) / a;
    if (t_min < temp && temp < t_max) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
      // TODO: 法線反転
      get_sphere_uv(rec.normal, rec.u, rec.v);
      rec.mat_ptr = mat_ptr;
      return true;
    }
    ///another intersecting point
    temp = (-b + sqrt(b * b - a * c)) / a;
    if (t_min < temp && temp < t_max) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      // TODO: 法線反転
      rec.normal = (rec.p - center) / radius;
      get_sphere_uv(rec.normal, rec.u, rec.v);
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

bool sphere::bounding_box(double t0, double t1, aabb &box) const {
  vec3 scale = vec3(radius, radius, radius);
  box = aabb(center - scale, center + scale);
  return true;
}

/// 移動球
class moving_sphere : public hitable {
 public:
  moving_sphere() {}
  moving_sphere(vec3 cen0, vec3 cen1,
                double t0, double t1,
                double r,
                shared_ptr<material> m) :
      center0(cen0), center1(cen1),
      time0(t0), time1(t1),
      radius(r),
      mat_ptr(m) {};
  virtual bool hit(const ray &r, double tmin, double tmax, hit_record &rec) const override;
  virtual bool bounding_box(double t0, double t1, aabb &box) const override;
  vec3 center(double time) const;
  vec3 center0, center1;
  double time0, time1;
  double radius;
  shared_ptr<material> mat_ptr;
};

/// time0 から time1の間で移動する球の
/// 時刻 t における中心座標
vec3 moving_sphere::center(double time) const {
  return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

/// 交差判定
bool moving_sphere::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
  vec3 oc = r.origin() - center(r.time());
  double a = dot(r.direction(), r.direction());
  double b = dot(oc, r.direction());
  double c = dot(oc, oc) - radius * radius;
  double discriminant = b * b - a * c;
  if (discriminant > 0) {
    double temp = (-b - sqrt(discriminant)) / a;
    if (t_min < temp && temp < t_max) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    /// 別の交差点
    temp = (-b + sqrt(discriminant)) / a;
    if (t_min < temp && temp < t_max) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

bool moving_sphere::bounding_box(double t0, double t1, aabb &box) const {
  vec3 scale = vec3(radius, radius, radius);
  aabb box0(center(t0) - scale, center(t0) + scale);
  aabb box1(center(t1) - scale, center(t1) + scale);
  box = surrounding_box(box0, box1);
  return true;
}

#endif //RAY_OBJECTS_SPHERE_H_

// Using the code of Ray Tracing in One Weekend
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RAY_OBJECTS_SPHERE_H_
#define RAY_OBJECTS_SPHERE_H_

#include "../utils/hitable.h"

class sphere : public hitable {
 public:
  sphere();
  sphere(vec3 cen, float r) : center(cen), radius(r) {};
  sphere(vec3 cen, float r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};
  virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const override;
  virtual bool bounding_box(float time0, float time1, aabb &box) const override;
  vec3 center;
  float radius;
  shared_ptr<material> mat_ptr;

 private:
  static void get_sphere_uv(const point3 &p, double &u, double &v) {
    auto theta = acos(-p.y());
    auto phi = atan2(-p.z(), p.x()) + PI;
    u = phi / (2 * PI);
    v = theta / PI;
  }
};

bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - a * c;
  if (discriminant > 0) {
    float temp = (-b - sqrt(b * b - a * c)) / a;
    if (t_min < temp && temp < t_max) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
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

bool sphere::bounding_box(float t0, float t1, aabb &box) const {
  vec3 scale = vec3(radius, radius, radius);
  box = aabb(center - scale, center + scale);
  return true;
}

/// 移動球
class moving_sphere : public hitable {
 public:
  moving_sphere() {}
  moving_sphere(vec3 cen0, vec3 cen1,
                float t0, float t1,
                float r,
                shared_ptr<material> m) :
      center0(cen0), center1(cen1),
      time0(t0), time1(t1),
      radius(r),
      mat_ptr(m) {};
  virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const override;
  virtual bool bounding_box(float t0, float t1, aabb &box) const override;
  vec3 center(float time) const;
  vec3 center0, center1;
  float time0, time1;
  float radius;
  shared_ptr<material> mat_ptr;
};

/// time0 から time1の間で移動する球の
/// 時刻 t における中心座標
vec3 moving_sphere::center(float time) const {
  return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

/// 交差判定
bool moving_sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
  vec3 oc = r.origin() - center(r.time());
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - a * c;
  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant)) / a;
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

bool moving_sphere::bounding_box(float t0, float t1, aabb &box) const {
  vec3 scale = vec3(radius, radius, radius);
  aabb box0(center(t0) - scale, center(t0) + scale);
  aabb box1(center(t1) - scale, center(t1) + scale);
  box = surrounding_box(box0, box1);
  return true;
}

#endif //RAY_OBJECTS_SPHERE_H_

// Using the code of Ray Tracing in One Weekend
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RAY_OBJECTS_SPHERE_H_
#define RAY_OBJECTS_SPHERE_H_

#include "../utils/hittable.h"

template<typename mat>
class sphere : public hittable<mat> {
 public:
  sphere();
  sphere(vec3 cen, double r, shared_ptr<mat> m) : center(cen), radius(r), mat_ptr(m) {};
  bool hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const override;
  bool bounding_box(double time0, double time1, aabb &box) const override;
  double pdf_value(const point3 &o, const vec3 &v) const override;
  vec3 random(const vec3 &o) const override;

 public:
  vec3 center;
  double radius;
  shared_ptr<mat> mat_ptr;

 private:
  static void get_sphere_uv(const point3 &p, double &u, double &v) {
    auto theta = acos(-p.y());
    auto phi = atan2(-p.z(), p.x()) + M_PI;

    u = phi * M_1_PI * 0.5;
    v = theta * M_1_PI;
  }
};

template<typename mat>
bool sphere<mat>::hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const {
  vec3 oc = r.origin() - center;
  double a = r.direction().squared_length();
  double half_b = dot(oc, r.direction());
  double c = oc.squared_length() - radius * radius;

  double discriminant = half_b * half_b - a * c;
  if (discriminant < 0) {
    return false;
  }
  auto sqrt_d = sqrt(discriminant);

  // 最近傍のrootを探す
  auto root = (-half_b - sqrt_d) / a;
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrt_d) / a;
    if (root < t_min || t_max < root) {
      return false;
    }
  }

  rec.t = root;
  rec.p = r.point_at_parameter(rec.t);
  vec3 outward_normal = (rec.p - center) / radius;
  rec.set_face_normal(r, outward_normal);
  get_sphere_uv(outward_normal, rec.u, rec.v);
  rec.mat_ptr = mat_ptr;

  return true;
}

template<typename mat>
bool sphere<mat>::bounding_box(double t0, double t1, aabb &box) const {
  vec3 scale = vec3(radius, radius, radius);
  box = aabb(center - scale, center + scale);
  return true;
}

template<typename mat>
vec3 sphere<mat>::random(const vec3 &o) const {
  vec3 direction = center - o;
  auto distance_squared = direction.squared_length();
  onb uvw;
  uvw.build_from_w(direction);
  return uvw.local(random_to_sphere(radius, distance_squared));
}

template<typename mat>
double sphere<mat>::pdf_value(const point3 &o, const vec3 &v) const {
  hit_record<mat> rec;
  if (!this->hit(ray(o, v), 0.001, INF, rec)) {
    return 0;
  }

  auto cos_theta_max = sqrt(1 - radius * radius / (center - o).squared_length());
  auto solid_angle = 2 * M_PI * (1 - cos_theta_max);

  return 1 / solid_angle;
}

/// 移動球
template<typename mat>
class moving_sphere : public hittable<mat> {
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
  virtual bool hit(const ray &r, double tmin, double tmax, hit_record<mat> &rec) const override;
  virtual bool bounding_box(double t0, double t1, aabb &box) const override;
  vec3 center(double time) const;
  vec3 center0, center1;
  double time0, time1;
  double radius;
  shared_ptr<mat> mat_ptr;
};

/// time0 から time1の間で移動する球の
/// 時刻 t における中心座標
template<typename mat>
vec3 moving_sphere<mat>::center(double time) const {
  return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

/// 交差判定
template<typename mat>
bool moving_sphere<mat>::hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const {
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

template<typename mat>
bool moving_sphere<mat>::bounding_box(double t0, double t1, aabb &box) const {
  vec3 scale = vec3(radius, radius, radius);
  aabb box0(center(t0) - scale, center(t0) + scale);
  aabb box1(center(t1) - scale, center(t1) + scale);
  box = surrounding_box(box0, box1);
  return true;
}

#endif //RAY_OBJECTS_SPHERE_H_

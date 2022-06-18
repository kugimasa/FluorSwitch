// Using the code of Ray Tracing in One Weekend
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RAY_MATERIAL_MATERIAL_H_
#define RAY_MATERIAL_MATERIAL_H_

#include "../utils/texture.h"

///reflection
vec3 reflect(const vec3 &v, const vec3 &n) {
  return v - 2 * dot(v, n) * n;
}

///refraction
bool refract(const vec3 &v, const vec3 &n, double ni_over_nt, vec3 &refracted) {
  vec3 uv = unit_vector(v);
  double dt = dot(uv, n);
  double discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
  if (discriminant > 0) {
    refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
    return true;
  } else
    return false;
}

/// Schlick Approximation
double schlick(double cosine, double ref_idx) {
  double r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 = r0 * r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5);
}

class material {
 public:
  virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const = 0;
  virtual color emitted(double u, double v, const point3 &p) const {
    return BLACK;
  };
};

/// 拡散反射面
class lambertian : public material {
 public:
  lambertian(const color &a) : albedo(make_shared<solid_color>(a)) {}
  lambertian(shared_ptr<texture> a) : albedo(a) {}
  virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const {

    auto scatter_direction = rec.normal + random_unit_vector();

    if (scatter_direction.near_zero()) {
      scatter_direction = rec.normal;
    }

    scattered = ray(rec.p, scatter_direction, r_in.time());
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }
  shared_ptr<texture> albedo;
};

///perfect specular reflectance
class metal : public material {
 public:
  metal(const vec3 &a) : albedo(a) {}
  metal(const vec3 &a, double f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
  virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }
  vec3 albedo;
  double fuzz;
};

///dielectric
class dielectric : public material {
 public:
  dielectric(double ri) : ref_idx(ri) {}
  virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const {
    vec3 outward_normal;
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    double ni_over_nt;
    attenuation = vec3(1.0, 1.0, 1.0);
    vec3 refracted;
    double reflect_prob;
    double cosine;
    if (dot(r_in.direction(), rec.normal) > 0) {
      outward_normal = -rec.normal;
      ni_over_nt = ref_idx;
      //cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
      cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
      cosine = sqrt(1 - ref_idx * ref_idx * (1 - cosine * cosine));
    } else {
      outward_normal = rec.normal;
      ni_over_nt = 1.0 / ref_idx;
      cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
    }
    if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
      reflect_prob = schlick(cosine, ref_idx);
    else
      reflect_prob = 1.0;
    if (drand48() < reflect_prob)
      scattered = ray(rec.p, reflected);
    else
      scattered = ray(rec.p, refracted);
    return true;
  }

  double ref_idx;
};

class isotropic : public material {
 public:
  isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
  isotropic(shared_ptr<texture> a) : albedo(a) {}

  bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override {
    // TODO: Henyey and Greensteinの位相関数
    // 位相関数
    scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  };
 public:
  shared_ptr<texture> albedo;
};
#endif //RAY_MATERIAL_MATERIAL_H_

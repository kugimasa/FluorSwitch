// Using the code of Ray Tracing in One Weekend
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RAY_MATERIAL_MATERIAL_H_
#define RAY_MATERIAL_MATERIAL_H_

#include "../utils/texture.h"
#include "../utils/onb.h"
#include "../utils/hittable.h"
#include "../sampling/pdf.h"

///reflection
vec3 reflect(const vec3 &v, const vec3 &n) {
  return v - 2 * dot(v, n) * n;
}

///refraction
vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat) {
  auto cos_theta = fmin(dot(-uv, n), 1.0);
  vec3 r_out_prep = etai_over_etat * (uv + cos_theta * n);
  vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_prep.squared_length())) * n;
  return r_out_prep + r_out_parallel;
}

/// Schlick Approximation
double schlick(double cosine, double ref_idx) {
  double r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 = r0 * r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5);
}

struct scattered_record {
  ray specular_ray;
  bool is_specular;
  color attenuation;
  shared_ptr<pdf> pdf_ptr;
};

class material {
 public:
  virtual bool scatter(const ray &r_in, const hit_record &rec, scattered_record &s_rec) const {
    return false;
  }

  virtual double scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered) const {
    return 0;
  }

  virtual color emitted(const ray &r_in, const hit_record &rec, double u, double v, const point3 &p) const {
    return BLACK;
  };
};

/// 拡散反射面
class lambertian : public material {
 public:
  lambertian(const color &a) : albedo(make_shared<solid_color>(a)) {}
  lambertian(shared_ptr<texture> a) : albedo(a) {}

  virtual bool scatter(const ray &r_in, const hit_record &rec, scattered_record &s_rec) const {
    s_rec.is_specular = false;
    s_rec.attenuation = albedo->value(rec.u, rec.v, rec.p);
    s_rec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
    return true;
  }

  double scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered) const {
    auto cos = dot(rec.normal, unit_vector(scattered.direction()));
    return cos < 0 ? 0 : cos * M_1_PI;
  }

  shared_ptr<texture> albedo;
};

///perfect specular reflectance
class metal : public material {
 public:
  metal(const vec3 &a) : albedo(a) {}
  metal(const vec3 &a, double f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }

  bool scatter(const ray &r_in, const hit_record &rec, scattered_record &s_rec) const override {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    s_rec.specular_ray = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    s_rec.attenuation = albedo;
    s_rec.is_specular = true;
    s_rec.pdf_ptr = 0;
    return true;
  }

  vec3 albedo;
  double fuzz;
};

///dielectric
class dielectric : public material {
 public:
  dielectric(double refraction_index) : ref_idx(refraction_index) {}
  virtual bool scatter(const ray &r_in, const hit_record &rec, scattered_record &s_rec) const {
    s_rec.is_specular = true;
    s_rec.pdf_ptr = nullptr;
    s_rec.attenuation = WHITE;
    double refraction_ratio = rec.front_face ? (1.0 / ref_idx) : ref_idx;

    vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract || schlick(cos_theta, refraction_ratio) > random_double()) {
      direction = reflect(unit_direction, rec.normal);
    } else {
      direction = refract(unit_direction, rec.normal, refraction_ratio);
    }

    s_rec.specular_ray = ray(rec.p, direction, r_in.time());
    return true;
  }

 public:
  double ref_idx;
};

class isotropic : public material {
 public:
  isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
  isotropic(shared_ptr<texture> a) : albedo(a) {}

  bool scatter(const ray &r_in, const hit_record &rec, scattered_record &s_rec) const override {
    // TODO: Henyey and Greensteinの位相関数
    // 位相関数
    s_rec.is_specular = true;
    s_rec.specular_ray = ray(rec.p, random_in_unit_sphere(), r_in.time());
    s_rec.attenuation = albedo->value(rec.u, rec.v, rec.p);
    s_rec.pdf_ptr = nullptr;
    return true;
  }

 public:
  shared_ptr<texture> albedo;
};
#endif //RAY_MATERIAL_MATERIAL_H_

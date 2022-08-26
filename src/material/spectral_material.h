#ifndef RTCAMP2022_SRC_MATERIAL_SPECTRAL_MATERIAL_H_
#define RTCAMP2022_SRC_MATERIAL_SPECTRAL_MATERIAL_H_
#include "../utils/texture.h"
#include "../utils/onb.h"
#include "../utils/hittable.h"
#include "../sampling/pdf.h"
#include "../utils/spectral_distribution.h"

struct spectral_scattered_record {
  bool is_fluor = false;
  spectral_distribution attenuation;
  spectral_distribution excitation;
  spectral_distribution emission;
  shared_ptr<pdf> pdf_ptr;
};

class spectral_material {
 public:
  virtual bool scatter(const ray &r_in, const hit_record<spectral_material> &rec, spectral_scattered_record &s_rec) const {
    return false;
  }

  virtual double scattering_pdf(const ray &r_in, const hit_record<spectral_material> &rec, const ray &scattered) const {
    return 0;
  }

  virtual spectral_distribution emitted(const ray &r_in, const hit_record<spectral_material> &rec, double u, double v, const point3 &p) const {
    return zero_spectra;
  };
};

/// 拡散反射面
class spectral_lambertian : public spectral_material {
 public:
  spectral_lambertian(const spectral_distribution &a) : albedo(a) {}

  virtual bool scatter(const ray &r_in, const hit_record<spectral_material> &rec, spectral_scattered_record &s_rec) const {
    s_rec.is_fluor = false;
    s_rec.attenuation = albedo;
    s_rec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
    return true;
  }

  virtual spectral_distribution emitted(const ray &r_in, const hit_record<spectral_material> &rec, double u, double v, const point3 &p) const {
    return zero_spectra;
  }

  double scattering_pdf(const ray &r_in, const hit_record<spectral_material> &rec, const ray &scattered) const {
    auto cos = dot(rec.normal, unit_vector(scattered.direction()));
    return cos < 0 ? 0 : cos * M_1_PI;
  }

  spectral_distribution albedo;
};

#endif //RTCAMP2022_SRC_MATERIAL_SPECTRAL_MATERIAL_H_

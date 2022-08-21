#ifndef RTCAMP2022_SRC_MATERIAL_FLUORESCENT_MATERIAL_H_
#define RTCAMP2022_SRC_MATERIAL_FLUORESCENT_MATERIAL_H_

#include "spectral_material.h"
/// 拡散反射面
class fluorescent_material : public spectral_material {
 public:
  fluorescent_material(spectral_distribution a) : albedo(a) {}

  virtual bool scatter(const ray &r_in, const hit_record<spectral_material> &rec, spectral_scattered_record &s_rec) const {
    s_rec.attenuation = albedo + (emission * excitation * eta);
    s_rec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
    return true;
  }

  virtual spectral_distribution emitted(const ray &r_in, const hit_record<spectral_material> &rec, double u, double v, const point3 &p) const {
    // 外側に発光
    if (rec.front_face) {
      return emission;
    } else {
      return zero_spectra;
    }
  }

  double scattering_pdf(const ray &r_in, const hit_record<spectral_material> &rec, const ray &scattered) const {
    auto cos = dot(rec.normal, unit_vector(scattered.direction()));
    return cos < 0 ? 0 : cos * M_1_PI;
  }

  double eta = 0.02;
  spectral_distribution albedo;
  spectral_distribution excitation = spectral_distribution("./assets/spectra/fluor/qdot545in.csv");
  spectral_distribution emission = spectral_distribution("./assets/spectra/fluor/qdot545out.csv");;
};

#endif //RTCAMP2022_SRC_MATERIAL_FLUORESCENT_MATERIAL_H_

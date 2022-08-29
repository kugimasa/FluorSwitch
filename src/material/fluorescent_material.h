#ifndef FLUORSWITCH_SRC_MATERIAL_FLUORESCENT_MATERIAL_H_
#define FLUORSWITCH_SRC_MATERIAL_FLUORESCENT_MATERIAL_H_

#include "spectral_material.h"
/// 拡散反射面
class fluorescent_material : public spectral_material {
 public:
  fluorescent_material(const spectral_distribution &a, std::vector<size_t> &sample_indices) {
    albedo = a;
    sample_excitation = spectral_distribution(excitation_spectra, sample_indices);
    sample_emission = spectral_distribution(emission_spectra, sample_indices);
  }

  fluorescent_material(const spectral_distribution &a) {
    albedo = a;
    sample_excitation = excitation_spectra;
    sample_emission = emission_spectra;
  }

  virtual bool scatter(const ray &r_in, const hit_record<spectral_material> &rec, spectral_scattered_record &s_rec) const {
    s_rec.is_fluor = true;
    s_rec.attenuation = albedo;
    s_rec.excitation = sample_excitation;
    s_rec.emission = sample_emission * eta;
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

  double eta = 0.2;
  spectral_distribution albedo;
  spectral_distribution sample_excitation;
  spectral_distribution sample_emission;
};

#endif //FLUORSWITCH_SRC_MATERIAL_FLUORESCENT_MATERIAL_H_

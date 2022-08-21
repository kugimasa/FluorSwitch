#ifndef RTCAMP2022_SRC_MATERIAL_SPECTRAL_LIGHT_H_
#define RTCAMP2022_SRC_MATERIAL_SPECTRAL_LIGHT_H_

#include "spectral_material.h"
#include "../utils/hittable.h"
#include "../utils/spectral_distribution.h"

class spectral_diffuse_light : public spectral_material {
 public:
  spectral_diffuse_light(spectral_distribution c) : emit(c) {}

  virtual bool scatter(const ray &r_in, const hit_record<spectral_material> &rec, color &attenuation, ray &scattered) const {
    return false;
  }

  virtual spectral_distribution emitted(const ray &r_in, const hit_record<spectral_material> &rec, double u, double v, const point3 &p) const {
    // 外側に発光
    if (rec.front_face) {
      return emit;
    } else {
      return zero_spectra;
    }
  }
 public:
  spectral_distribution emit;
};

#endif //RTCAMP2022_SRC_MATERIAL_SPECTRAL_LIGHT_H_

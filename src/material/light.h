#ifndef RTCAMP2022_SRC_MATERIAL_LIGHT_H_
#define RTCAMP2022_SRC_MATERIAL_LIGHT_H_

#include "material.h"
#include "../utils/hittable.h"

class diffuse_light : public material {
 public:
  diffuse_light(shared_ptr<texture> a) : emit(a) {}
  diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

  virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const {
    return false;
  }

  virtual color emitted(double u, double v, const point3 &p) const {
    return emit->value(u, v, p);
  }
 public:
  shared_ptr<texture> emit;
};

#endif //RTCAMP2022_SRC_MATERIAL_LIGHT_H_

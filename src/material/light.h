#ifndef RTCAMP2022_SRC_MATERIAL_LIGHT_H_
#define RTCAMP2022_SRC_MATERIAL_LIGHT_H_

#include "material.h"
#include "../utils/hittable.h"

class diffuse_light : public material {
 public:
  diffuse_light(shared_ptr<texture> a) : emit(a) {}
  diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

  virtual bool scatter(const ray &r_in, const hit_record<material> &rec, color &attenuation, ray &scattered) const {
    return false;
  }

  virtual color emitted(const ray &r_in, const hit_record<material> &rec, double u, double v, const point3 &p) const {
    // 外側に発光
    if (rec.front_face) {
      return emit->value(u, v, p);
    } else {
      return BLACK;
    }
  }
 public:
  shared_ptr<texture> emit;
};

#endif //RTCAMP2022_SRC_MATERIAL_LIGHT_H_

#ifndef FLUORSWITCH_SRC_OBJECTS_BOX_H_
#define FLUORSWITCH_SRC_OBJECTS_BOX_H_

#include "../utils/util_funcs.h"
#include "../utils/hittable_list.h"

#include "aarect.h"

template<typename mat>
class box : public hittable<mat> {
 public:
  box() {}
  box(const point3 &p0, const point3 &p1, shared_ptr<mat> ptr);

  bool hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const override;
  bool bounding_box(double time0, double time1, aabb &output_box) const override {
    output_box = aabb(box_min, box_max);
    return true;
  }

  double pdf_value(const point3 &o, const vec3 &v) const override {
    hit_record<mat> rec;
    if (!this->hit(ray(o, v), 0.001, INF, rec)) {
      return 0;
    }
    auto xy_face = (box_max.x() - box_min.x()) * (box_max.y() - box_min.y());
    auto xz_face = (box_max.x() - box_min.x()) * (box_max.z() - box_min.z());
    auto yz_face = (box_max.y() - box_min.y()) * (box_max.z() - box_min.z());
    auto area = 2 * (xy_face + xz_face + yz_face);
    auto distance_squared = rec.t * rec.t * v.squared_length();
    auto cosine = fabs(dot(v, rec.normal) / v.length());
    return distance_squared / (cosine * area);
  }

 public:
  point3 box_min;
  point3 box_max;
  hittable_list<mat> sides;
};

template<typename mat>
box<mat>::box(const point3 &p0, const point3 &p1, shared_ptr<mat> ptr) {
  box_min = p0;
  box_max = p1;

  sides.add(make_shared<xy_rect<mat>>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
  sides.add(make_shared<xy_rect<mat>>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

  sides.add(make_shared<xz_rect<mat>>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
  sides.add(make_shared<xz_rect<mat>>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

  sides.add(make_shared<yz_rect<mat>>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
  sides.add(make_shared<yz_rect<mat>>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

template<typename mat>
bool box<mat>::hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const {
  return sides.hit(r, t_min, t_max, rec);
}

#endif //FLUORSWITCH_SRC_OBJECTS_BOX_H_

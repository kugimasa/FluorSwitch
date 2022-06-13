// Using the code of Ray Tracing in One Weekend
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

#ifndef RAY_UTILS_HITTABLE_LIST_H_
#define RAY_UTILS_HITTABLE_LIST_H_
#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
 public:
  hittable_list() {}
  hittable_list(shared_ptr<hittable> object) { add(object); }
  hittable_list(shared_ptr<hittable_list> object_list) { add(object_list); }

  void clear() { objects.clear(); }
  void add(shared_ptr<hittable> object) { objects.push_back(object); }
  void add(shared_ptr<hittable_list> object_list) {
    for (shared_ptr<hittable> o: object_list->objects) {
      objects.push_back(o);
    }
  }

  bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;
  bool bounding_box(double t0, double t1, aabb &box) const override;

 public:
  std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
  hit_record temp_rec;
  bool hit_anything = false;
  double closest_so_far = t_max;

  for (const auto &object: objects) {
    if (object->hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }

  return hit_anything;
}

bool hittable_list::bounding_box(double t0, double t1, aabb &box) const {
  // リストが空の場合、リターン
  if (objects.empty()) {
    return false;
  }

  aabb temp_box;
  bool first_box = true;
  for (const auto &object: objects) {
    // 最初のオブジェクトにAABBが設定されていなかった場合
    if (!object->bounding_box(t0, t1, temp_box)) {
      return false;
    }
    // オブジェクト2個以上の場合
    box = first_box ? temp_box : surrounding_box(box, temp_box);
    first_box = false;
  }
  return true;
}
#endif //RAY_UTILS_HITTABLE_LIST_H_

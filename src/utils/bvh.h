#ifndef FLUORSWITCH_SRC_UTILS_BVH_H_
#define FLUORSWITCH_SRC_UTILS_BVH_H_

#include <algorithm>
#include "hittable.h"
#include "hittable_list.h"
#include "../objects/geometry.h"

template<typename mat>
class bvh_node : public hittable<mat> {
 public:
  bvh_node() {}
  bvh_node(const hittable_list<mat> &list, double time0, double time1)
      : bvh_node(list.objects, 0, list.objects.size(), time0, time1) {}
  bvh_node(const shared_ptr<geometry<mat>> &obj, double t0, double t1)
      : bvh_node(obj->tris, 0, obj->tris.size(), t0, t1) {}

  bvh_node(const std::vector<shared_ptr<hittable<mat>>> &src_objects, size_t start, size_t end, double t0, double t1);

  bool hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const override;
  bool bounding_box(double t0, double t1, aabb &box) const override;

 public:
  // 子ノード
  shared_ptr<hittable<mat>> left;
  shared_ptr<hittable<mat>> right;
  aabb box;
};

template<typename mat>
inline bool box_compare(const shared_ptr<hittable<mat>> a, const shared_ptr<hittable<mat>> b, int axis) {
  aabb box_a;
  aabb box_b;
  if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b)) {
    std::cerr << "No bounding box in bvh_node constructor\n";
  }
  return box_a.min().e[axis] < box_b.min().e[axis];
}

template<typename mat>
bool box_x_compare(const shared_ptr<hittable<mat>> a, const shared_ptr<hittable<mat>> b) {
  return box_compare(a, b, 0);
}
template<typename mat>
bool box_y_compare(const shared_ptr<hittable<mat>> a, const shared_ptr<hittable<mat>> b) {
  return box_compare(a, b, 1);
}
template<typename mat>
bool box_z_compare(const shared_ptr<hittable<mat>> a, const shared_ptr<hittable<mat>> b) {
  return box_compare(a, b, 2);
}

template<typename mat>
bvh_node<mat>::bvh_node(const std::vector<shared_ptr<hittable<mat>>> &src_objects,
                        size_t start,
                        size_t end,
                        double t0,
                        double t1) {

  auto objects = src_objects;
  // チェックする軸をX,Y,Zからランダムで選択
  int axis = int(3 * drand48());
  auto comparator = (axis == 0) ? box_x_compare<mat>
                                : (axis == 1) ? box_y_compare<mat>
                                              : box_z_compare<mat>;

  size_t object_span = end - start;

  // オブジェクト１個の場合
  if (object_span == 1) {
    left = right = objects[start];
  }
    // オブジェクト２個の場合
  else if (object_span == 2) {
    if (comparator(objects[start], objects[start + 1])) {
      left = objects[start];
      right = objects[start + 1];
    } else {
      left = objects[start + 1];
      right = objects[start];
    }
  }
    // オブジェクト３個以上
  else {
    std::sort(objects.begin() + start, objects.begin() + end, comparator);

    auto mid = start + object_span / 2;
    left = make_shared<bvh_node>(objects, start, mid, t0, t1);
    right = make_shared<bvh_node>(objects, mid, end, t0, t1);
  }

  aabb box_left, box_right;
  // AABBの設定
  if (!left->bounding_box(t0, t1, box_left) || !right->bounding_box(t0, t1, box_right)) {
    std::cerr << "No bounding box in bvh_node constructor\n";
  }
  box = surrounding_box(box_left, box_right);
}

template<typename mat>
bool bvh_node<mat>::bounding_box(double t0, double t1, aabb &output_box) const {
  output_box = box;
  return true;
}

template<typename mat>
bool bvh_node<mat>::hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const {
  if (!box.hit(r, t_min, t_max)) {
    return false;
  }
  bool hit_left = left->hit(r, t_min, t_max, rec);
  bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
  return hit_left || hit_right;
}

#endif //FLUORSWITCH_SRC_UTILS_BVH_H_

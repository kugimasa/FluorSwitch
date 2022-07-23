#ifndef RTCAMP2022_SRC_UTILS_BVH_H_
#define RTCAMP2022_SRC_UTILS_BVH_H_

#include <algorithm>
#include "hittable.h"
#include "hittable_list.h"
#include "../objects/geometry.h"

class bvh_node : public hittable {
 public:
  bvh_node() {}
  bvh_node(const shared_ptr<geometry> &obj, double t0, double t1)
      : bvh_node(obj->tris, 0, obj->tris.size(), t0, t1) {}

  bvh_node(const std::vector<shared_ptr<triangle>> &tris, size_t start, size_t end, double t0, double t1);

  bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;
  bool bounding_box(double t0, double t1, aabb &box) const override;

 public:
  // 子ノード
  shared_ptr<hittable> left;
  shared_ptr<hittable> right;
  aabb box;
};

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
  aabb box_a;
  aabb box_b;
  if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b)) {
    std::cerr << "No bounding box in bvh_node constructor\n";
  }
  return box_a.min().e[axis] < box_b.min().e[axis];
}

bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
  return box_compare(a, b, 0);
}
bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
  return box_compare(a, b, 1);
}
bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
  return box_compare(a, b, 2);
}

bvh_node::bvh_node(const std::vector<shared_ptr<triangle>> &tris,
                   size_t start,
                   size_t end,
                   double t0,
                   double t1) {

  auto objects = tris;
  // チェックする軸をX,Y,Zからランダムで選択
  int axis = int(3 * drand48());
  auto comparator = (axis == 0) ? box_x_compare
                                : (axis == 1) ? box_y_compare
                                              : box_z_compare;

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

bool bvh_node::bounding_box(double t0, double t1, aabb &output_box) const {
  output_box = box;
  return true;
}

bool bvh_node::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
  if (!box.hit(r, t_min, t_max)) {
    return false;
  }
  bool hit_left = left->hit(r, t_min, t_max, rec);
  bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
  return hit_left || hit_right;
}

#endif //RTCAMP2022_SRC_UTILS_BVH_H_

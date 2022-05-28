#ifndef RTCAMP2022_SRC_UTILS_BVH_H_
#define RTCAMP2022_SRC_UTILS_BVH_H_

#include "hitable.h"
#include "hitable_list.h"

class bvh_node : public hitable{
 public:
  bvh_node() {}
  bvh_node(hitable **l, int n, float t0, float t1);
  virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override;
  virtual bool bounding_box(float t0, float t1, aabb &box) const override;
  // 子ノード
  hitable *_left;
  hitable *_right;
  aabb _box;
};

// TODO: 冗長なので要リファクタ
int box_x_compare (const void *a, const void *b) {
  aabb box_left, box_right;
  hitable *ah = *(hitable**)a;
  hitable *bh = *(hitable**)b;
  if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right)) {
    std::cerr << "No bounding box in bvh_node constructor\n";
  }
  if (box_left.min().x() - box_right.min().x() < 0.0) {
    return -1;
  }
  return 1;
}

int box_y_compare (const void *a, const void *b) {
  aabb box_left, box_right;
  hitable *ah = *(hitable**)a;
  hitable *bh = *(hitable**)b;
  if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right)) {
    std::cerr << "No bounding box in bvh_node constructor\n";
  }
  if (box_left.min().y() - box_right.min().y() < 0.0) {
    return -1;
  }
  return 1;
}

int box_z_compare (const void *a, const void *b) {
  aabb box_left, box_right;
  hitable *ah = *(hitable**)a;
  hitable *bh = *(hitable**)b;
  if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right)) {
    std::cerr << "No bounding box in bvh_node constructor\n";
  }
  if (box_left.min().z() - box_right.min().z() < 0.0) {
    return -1;
  }
  return 1;
}

bvh_node::bvh_node(hitable **l, int n, float t0, float t1) {
  // チェックする軸をX,Y,Zからランダムで選択
  int axis = int(3 * drand48());
  if (axis == 0) {
    qsort(l, n, sizeof(hitable *), box_x_compare);
  } else if (axis == 1) {
    qsort(l, n, sizeof(hitable *), box_y_compare);
  } else {
    qsort(l, n, sizeof(hitable *), box_z_compare);
  }

  if (n == 1) {
    _left = _right = l[0];
  }
  else if (n == 2) {
    _left = l[0];
    _right = l[1];
  }
  else {
    // ノードを生やす
    _left = new bvh_node(l, n/2, t0, t1);
    _right = new bvh_node(l + n/2, n - n/2, t0, t1);
  }
  aabb box_left, box_right;
  // AABBの設定
  if(!_left->bounding_box(t0, t1, box_left) || !_right->bounding_box(t0, t1, box_right)) {
    std::cerr << "No bounding box in bvh_node constructor\n";
  }
  _box = surrounding_box(box_left, box_right);




}

bool bvh_node::bounding_box(float t0, float t1, aabb &box) const {
  box = _box;
  return true;
}

bool bvh_node::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
  if(_box.hit(r, t_min, t_max)) {
    hit_record left_rec, right_rec;
    bool hit_left = _left->hit(r, t_min, t_max, left_rec);
    bool hit_right = _right->hit(r, t_min, t_max, right_rec);
    // 両方の子ノードに交差
    if (hit_left && hit_right) {
      if (left_rec.t < right_rec.t) {
        rec = left_rec;
      } else {
        rec = right_rec;
      }
      return true;
    }
    // 左の子ノードに交差
    else if (hit_left) {
      rec = left_rec;
      return true;
    }
    // 右の子ノードに交差
    else if (hit_right) {
      rec = right_rec;
      return true;
    }
  }
  // どの子ノードにもヒットしなかった
  return false;
}

#endif //RTCAMP2022_SRC_UTILS_BVH_H_

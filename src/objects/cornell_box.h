#ifndef RTCAMP2022_SRC_OBJECTS_CORNELL_BOX_H_
#define RTCAMP2022_SRC_OBJECTS_CORNELL_BOX_H_

#include "../utils/hittable_list.h"
#include "../material/material.h"
#include "../material/light.h"
#include "aarect.h"

class cornell_box : public hittable_list {
 public:
  cornell_box() {}
  cornell_box(double box_size, double light_size,
              shared_ptr<material> right_mat, shared_ptr<material> left_mat,
              shared_ptr<material> top_mat, shared_ptr<material> bottom_mat,
              shared_ptr<material> back_mat, shared_ptr<material> light_mat) {

    /// Cornell壁
    objects.push_back(make_shared<yz_rect>(0, box_size, 0, box_size, 0, right_mat));
    objects.push_back(make_shared<yz_rect>(0, box_size, 0, box_size, box_size, left_mat));
    objects.push_back(make_shared<xz_rect>(0, box_size, 0, box_size, 0, bottom_mat));
    objects.push_back(make_shared<xz_rect>(0, box_size, 0, box_size, box_size, top_mat));
    objects.push_back(make_shared<xy_rect>(0, box_size, 0, box_size, box_size, back_mat));

    /// 光源
    double x_0 = (box_size - light_size) / 2;
    double x_1 = x_0 + light_size;
    auto diffuse_light = make_shared<flip_face>(make_shared<xz_rect>(x_0, x_1, x_0, x_1, box_size - 1, light_mat));
    objects.push_back(diffuse_light);
  }

  /// 光源なし
  cornell_box(double box_size,
              shared_ptr<material> right_mat, shared_ptr<material> left_mat,
              shared_ptr<material> top_mat, shared_ptr<material> bottom_mat,
              shared_ptr<material> back_mat) {

    /// Cornell壁
    objects.push_back(make_shared<yz_rect>(0, box_size, 0, box_size, 0, right_mat));
    objects.push_back(make_shared<yz_rect>(0, box_size, 0, box_size, box_size, left_mat));
    objects.push_back(make_shared<xz_rect>(0, box_size, 0, box_size, 0, bottom_mat));
    objects.push_back(make_shared<xz_rect>(0, box_size, 0, box_size, box_size, top_mat));
    objects.push_back(make_shared<xy_rect>(0, box_size, 0, box_size, box_size, back_mat));
  }

};

#endif //RTCAMP2022_SRC_OBJECTS_CORNELL_BOX_H_

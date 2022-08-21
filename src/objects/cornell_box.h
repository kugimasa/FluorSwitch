#ifndef RTCAMP2022_SRC_OBJECTS_CORNELL_BOX_H_
#define RTCAMP2022_SRC_OBJECTS_CORNELL_BOX_H_

#include "../utils/hittable_list.h"
#include "../material/material.h"
#include "../material/light.h"
#include "aarect.h"

template<typename mat>
class cornell_box : public hittable_list<mat> {
 public:
  cornell_box() {}
  cornell_box(double box_size, double light_size,
              shared_ptr<mat> right_mat, shared_ptr<mat> left_mat,
              shared_ptr<mat> top_mat, shared_ptr<mat> bottom_mat,
              shared_ptr<mat> back_mat, shared_ptr<mat> light_mat) {
    /// Cornell壁
    this->objects.push_back(make_shared<yz_rect<mat>>(0, box_size, 0, box_size, 0, right_mat));
    this->objects.push_back(make_shared<yz_rect<mat>>(0, box_size, 0, box_size, box_size, left_mat));
    this->objects.push_back(make_shared<xz_rect<mat>>(0, box_size, 0, box_size, 0, bottom_mat));
    this->objects.push_back(make_shared<xz_rect<mat>>(0, box_size, 0, box_size, box_size, top_mat));
    this->objects.push_back(make_shared<xy_rect<mat>>(0, box_size, 0, box_size, box_size, back_mat));

    /// 光源
    double x_0 = (box_size - light_size) / 2;
    double x_1 = x_0 + light_size;
    auto diffuse_light = make_shared<flip_face<mat>>(make_shared<xz_rect<mat>>(x_0, x_1, x_0, x_1, box_size - 1, light_mat));
    this->objects.push_back(diffuse_light);
  }

  /// 光源なし
  cornell_box(double box_size,
              shared_ptr<mat> right_mat, shared_ptr<mat> left_mat,
              shared_ptr<mat> top_mat, shared_ptr<mat> bottom_mat,
              shared_ptr<mat> back_mat) {

    /// Cornell壁
    this->objects.push_back(make_shared<yz_rect<mat>>(0, box_size, 0, box_size, 0, right_mat));
    this->objects.push_back(make_shared<yz_rect<mat>>(0, box_size, 0, box_size, box_size, left_mat));
    this->objects.push_back(make_shared<xz_rect<mat>>(0, box_size, 0, box_size, 0, bottom_mat));
    this->objects.push_back(make_shared<xz_rect<mat>>(0, box_size, 0, box_size, box_size, top_mat));
    this->objects.push_back(make_shared<xy_rect<mat>>(0, box_size, 0, box_size, box_size, back_mat));
  }

};

#endif //RTCAMP2022_SRC_OBJECTS_CORNELL_BOX_H_

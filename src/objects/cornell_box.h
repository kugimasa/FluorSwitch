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
              color right_c, color left_c,
              color top_c, color bottom_c,
              color back_c, color light_c) {
    /// マテリアル設定
    auto right_mat = make_shared<lambertian>(right_c);
    auto left_mat = make_shared<lambertian>(left_c);
    auto top_mat = make_shared<lambertian>(top_c);
    auto bottom_mat = make_shared<lambertian>(bottom_c);
    auto back_mat = make_shared<lambertian>(back_c);
    auto light_mat = make_shared<diffuse_light>(light_c);

    /// Cornell壁
    objects.push_back(make_shared<yz_rect>(0, box_size, 0, box_size, 0, right_mat));
    objects.push_back(make_shared<yz_rect>(0, box_size, 0, box_size, box_size, left_mat));
    objects.push_back(make_shared<xz_rect>(0, box_size, 0, box_size, 0, bottom_mat));
    objects.push_back(make_shared<xz_rect>(0, box_size, 0, box_size, box_size, top_mat));
    objects.push_back(make_shared<xy_rect>(0, box_size, 0, box_size, box_size, back_mat));

    /// 光源
    double x_0 = (box_size - light_size) / 2;
    double x_1 = x_0 + light_size;
    objects.push_back(make_shared<xz_rect>(x_0, x_1, x_0, x_1, box_size - 1, light_mat));
  }

};

#endif //RTCAMP2022_SRC_OBJECTS_CORNELL_BOX_H_

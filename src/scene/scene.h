#ifndef FLUORSWITCH_SRC_SCENE_SCENE_H_
#define FLUORSWITCH_SRC_SCENE_SCENE_H_

#include "../material/fluorescent_material.h"
#include "../material/spectral_material.h"
#include "../material/spectral_light.h"
#include "../objects/cornell_box.h"
#include "../objects/sphere.h"
#include "../objects/geometry.h"
#include "../utils/hittable_list.h"
#include "../utils/bvh.h"
#include "../utils/util_funcs.h"

// t = [0, 1]
inline double spectral_sphere_x(double t) {
  return SPHERE_SPECTRAL_START_X * (1 - t) + SPHERE_SPECTRAL_END_X * t;
}

/// マテリアル設定
auto blue_mat = make_shared<spectral_lambertian>(blue_spectra);
auto red_mat = make_shared<spectral_lambertian>(red_spectra);
auto white_mat = make_shared<spectral_lambertian>(white_spectra);
auto black_mat = make_shared<spectral_lambertian>(black_spectra);
// NEED FIX
auto fluo_mat = make_shared<fluorescent_material>(black_spectra);

inline hittable_list<spectral_material> construct_spectral_scene(int frame, int max_frame) {
  hittable_list<spectral_material> world;
  /// アニメーションパラメータ
  double move_t = (double) (frame - RGB_END_FRAME) / (double) (max_frame - RGB_END_FRAME); // [0, 1];
  double uv_t = 1.0;
  if (frame < UV_LIGHT_ON_FRAME) {
    uv_t = (double) (frame - RGB_END_FRAME) / (double) (UV_LIGHT_ON_FRAME - RGB_END_FRAME);
  }
  double x_t = spectral_sphere_x(move_t);
  auto uv_light_mat = make_shared<spectral_diffuse_light>(uv_spectra * uv_t);

  /// コーネルボックス
  cornell_box<spectral_material> cb = cornell_box<spectral_material>(555, LIGHT_WIDTH, red_mat, red_mat, white_mat, white_mat, blue_mat, uv_light_mat);
  world.add(make_shared<hittable_list<spectral_material>>(cb));
  /// 移動する球
  world.add(make_shared<sphere<spectral_material>>(vec3(x_t, SPHERE_RADIUS, SPHERE_Z), SPHERE_RADIUS, fluo_mat));
  /// 蛍光スイッチ
  world.add(make_shared<box<spectral_material>>(vec3(545, SPHERE_RADIUS - 10, SPHERE_Z - 50), vec3(555, SPHERE_RADIUS + 10, SPHERE_Z + 50), black_mat));
  return world;
}

inline shared_ptr<hittable_list<spectral_material>> construct_spectral_light_sampler() {
  auto lights = make_shared<hittable_list<spectral_material>>();
  /// 光源サンプル用
  lights->add(make_shared<xz_rect<spectral_material>>(202.5, 352.5, 202.5, 352.5, 554, shared_ptr<spectral_material>()));
  return lights;
}

/// マテリアル設定
auto rgb_blue_mat = make_shared<lambertian>(MACBETH_BLUE);
auto rgb_red_mat = make_shared<lambertian>(MACBETH_RED);
auto rgb_white_mat = make_shared<lambertian>(MACBETH_WHITE);
auto rgb_black_mat = make_shared<lambertian>(MACBETH_BLACK);

// t = [0, 1]
inline double rgb_sphere_x(double t, bool light_on) {
  if (light_on) {
    return SPHERE_RGB_START_X + (545 - SPHERE_RGB_START_X - SPHERE_RADIUS) * t;
  } else {
    return 545 - SPHERE_RADIUS - 1.5 * SPHERE_RADIUS * t;
  }
}

inline hittable_list<material> construct_scene(int frame, int max_frame) {
  /// シーンデータ
  hittable_list<material> world;

  /// アニメーションパラメータ
  double move_t = 0;
  double light_t = 1;
  bool light_on = true;
  /// 移動する球
  if (RGB_STOP_FRAME < frame && frame < RGB_LIGHT_OFF_FRAME) {
    move_t = (double) (frame - RGB_STOP_FRAME) / (double) (RGB_LIGHT_OFF_FRAME - RGB_STOP_FRAME); // [0, 1]
  } else if (RGB_LIGHT_OFF_FRAME <= frame) {
    light_on = false;
    move_t = (double) (frame - RGB_LIGHT_OFF_FRAME) / (double) (max_frame - RGB_LIGHT_OFF_FRAME); // [0, 1]
    light_t = (1 - move_t) * (1 - move_t) * (1 - move_t) * (1 - move_t) * (1 - move_t); // [1, 0] easeInQuint
  }

  /// 光源設定
  auto rgb_light_mat = make_shared<diffuse_light>(D65_LIGHT * light_t);
  cornell_box<material> cb = cornell_box<material>(555, LIGHT_WIDTH, rgb_red_mat, rgb_red_mat, rgb_white_mat, rgb_white_mat, rgb_blue_mat, rgb_light_mat);
  world.add(make_shared<hittable_list<material>>(cb));

  double x_t = rgb_sphere_x(move_t, light_on);
  world.add(make_shared<sphere<material>>(vec3(x_t, SPHERE_RADIUS, SPHERE_Z), SPHERE_RADIUS, rgb_black_mat));
  /// 蛍光スイッチ
  world.add(make_shared<box<material>>(vec3(545, SPHERE_RADIUS - 10, SPHERE_Z - 50), vec3(555, SPHERE_RADIUS + 10, SPHERE_Z + 50), rgb_black_mat));
  return world;
}

inline shared_ptr<hittable_list<material>> construct_light_sampler() {
  auto lights = make_shared<hittable_list<material>>();
  /// 光源サンプル用
  lights->add(make_shared<xz_rect<material>>(202.5, 352.5, 202.5, 352.5, 554, shared_ptr<material>()));
  return lights;
}

/// OBJモデル用
//  std::cout << "+++++++++ Load Obj +++++++++" << std::endl;
//  // OBJモデルの読み込み
//  shared_ptr<geometry> obj = make_shared<geometry>("./assets/obj/kugizarashi.obj", glass);
//  auto obj_bvh = make_shared<translate>(make_shared<bvh_node>(obj, 0, 1), vec3(265, 50, 265));
//  world.add(obj_bvh);
//  std::cout << "++++++++++ Finish ++++++++++" << std::endl;

#endif //FLUORSWITCH_SRC_SCENE_SCENE_H_

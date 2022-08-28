#ifndef RTCAMP2022_SRC_SCENE_SCENE_H_
#define RTCAMP2022_SRC_SCENE_SCENE_H_

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
  return SPHERE_SPECTRAL_END_X * t + SPHERE_SPECTRAL_START_X * (1 - t);
}

inline hittable_list<spectral_material> construct_spectral_scene(int frame, int max_frame, std::vector<size_t> wavelength_indices) {
  double t = (double) (frame) / (double) (max_frame); // [0, 1]
  hittable_list<spectral_material> world;
  auto blue = spectral_distribution(blue_spectra, wavelength_indices);
  auto red = spectral_distribution(red_spectra, wavelength_indices);
  auto white = spectral_distribution(white_spectra, wavelength_indices);
  auto black = spectral_distribution(black_spectra, wavelength_indices);
  auto d65_light = spectral_distribution(d65_spectra, wavelength_indices);
  auto uv_light = spectral_distribution(uv_spectra, wavelength_indices);

  /// マテリアル設定
  auto blue_mat = make_shared<spectral_lambertian>(blue);
  auto red_mat = make_shared<spectral_lambertian>(red);
  auto white_mat = make_shared<spectral_lambertian>(white);
  auto black_mat = make_shared<spectral_lambertian>(black);
  auto d65_light_mat = make_shared<spectral_diffuse_light>(d65_light);
  auto uv_light_mat = make_shared<spectral_diffuse_light>(uv_light);
  // NEED FIX
  auto fluo_mat = make_shared<fluorescent_material>(black, wavelength_indices);
  /// コーネルボックス
  cornell_box<spectral_material> cb = cornell_box<spectral_material>(555, LIGHT_WIDTH, red_mat, red_mat, white_mat, white_mat, blue_mat, uv_light_mat);
  world.add(make_shared<hittable_list<spectral_material>>(cb));
  /// 移動する球
  double x_t = spectral_sphere_x(t);
  world.add(make_shared<sphere<spectral_material>>(vec3(x_t, SPHERE_RADIUS, SPHERE_Z), SPHERE_RADIUS, fluo_mat));
  /// 蛍光スイッチ
  world.add(make_shared<box<spectral_material>>(vec3(545, SPHERE_RADIUS - 10, SPHERE_Z - 50), vec3(555, SPHERE_RADIUS + 10, SPHERE_Z + 50), black_mat));
  return world;
}

inline shared_ptr<hittable_list<spectral_material>> construct_spectral_light_sampler() {
  auto lights = make_shared<hittable_list<spectral_material>>();
  /// 光源サンプル用
  lights->add(make_shared<xz_rect<spectral_material>>(202.5, 352.5, 202.5, 352.5, 554, shared_ptr<spectral_material>()));
  // TODO: 余裕があったらBOXで実装
  // lights->add(make_shared<box<spectral_material>>(vec3(545, SPHERE_RADIUS - 10, SPHERE_Z - 50), vec3(555, SPHERE_RADIUS + 10, SPHERE_Z + 50), shared_ptr<spectral_material>()));
  return lights;
}

/// マテリアル設定
auto rgb_blue_mat = make_shared<lambertian>(MACBETH_BLUE);
auto rgb_red_mat = make_shared<lambertian>(MACBETH_RED);
auto rgb_white_mat = make_shared<lambertian>(MACBETH_WHITE);
auto rgb_black_mat = make_shared<lambertian>(MACBETH_BLACK);
/// 光源設定
auto rgb_light_mat = make_shared<diffuse_light>(D65_LIGHT);

// t = [0, 1]
inline double rgb_sphere_x(double t) {
  return SPHERE_RGB_START_X + (545 - SPHERE_RGB_START_X - SPHERE_RADIUS) * t;
}

inline hittable_list<material> construct_scene(int frame, int stop_frame, int max_frame) {
  /// シーンデータ
  hittable_list<material> world;
  cornell_box<material> cb = cornell_box<material>(555, LIGHT_WIDTH, rgb_red_mat, rgb_red_mat, rgb_white_mat, rgb_white_mat, rgb_blue_mat, rgb_light_mat);
  world.add(make_shared<hittable_list<material>>(cb));
  double t = 0;
  /// 移動する球
  if (frame > stop_frame) {
    t = (double) (frame - stop_frame) / (double) (max_frame - stop_frame); // [0, 1]

  }
  double x_t = rgb_sphere_x(t);
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

#endif //RTCAMP2022_SRC_SCENE_SCENE_H_

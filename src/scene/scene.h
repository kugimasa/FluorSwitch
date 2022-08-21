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

inline hittable_list<spectral_material> construct_spectral_scene(int frame, int max_frame) {
  hittable_list<spectral_material> world;
  /// マテリアル設定
  auto blue_mat = make_shared<spectral_lambertian>(blue_spectra);
  auto red_mat = make_shared<spectral_lambertian>(red_spectra);
  auto white_mat = make_shared<spectral_lambertian>(white_spectra);
  auto fluor_mat = make_shared<fluorescent_material>(black_spectra);
  auto light_mat = make_shared<spectral_diffuse_light>(uv_spectra);
  /// コーネルボックス
  cornell_box<spectral_material> cb = cornell_box<spectral_material>(555, 150, red_mat, red_mat, white_mat, white_mat, blue_mat, light_mat);
  world.add(make_shared<hittable_list<spectral_material>>(cb));
  /// 拡大縮小/移動する球
  double f = (double) frame / (max_frame * 2);
  double offset = 50;
  double radius = 90 + sin(f * 2 * M_PI) * offset;
  world.add(make_shared<sphere<spectral_material>>(vec3(radius + 100, 90, radius + 100), radius, fluor_mat));
  return world;
}

inline shared_ptr<hittable_list<spectral_material>> construct_spectral_light_sampler() {
  auto lights = make_shared<hittable_list<spectral_material>>();
  /// 光源サンプル用
  lights->add(make_shared<xz_rect<spectral_material>>(202.5, 352.5, 202.5, 352.5, 554, shared_ptr<spectral_material>()));
  return lights;
}

inline hittable_list<material> construct_scene(int frame, int max_frame) {
  /// シーンデータ
  hittable_list<material> world;

  /// マテリアル設定
  auto blue_mat = make_shared<lambertian>(MACBETH_BLUE);
  auto red_mat = make_shared<lambertian>(MACBETH_RED);
  auto white_mat = make_shared<lambertian>(MACBETH_WHITE);
  auto black_mat = make_shared<lambertian>(MACBETH_BLACK);
  /// 光源設定
  auto light_mat = make_shared<diffuse_light>(D65_LIGHT);
  cornell_box<material> cb = cornell_box<material>(555, 150, red_mat, red_mat, white_mat, white_mat, blue_mat, light_mat);
  world.add(make_shared<hittable_list<material>>(cb));
  return world;
}

inline shared_ptr<hittable_list<material>> construct_light_sampler() {
  auto lights = make_shared<hittable_list<material>>();
  /// 光源サンプル用
  lights->add(make_shared<xz_rect<material>>(202.5, 352.5, 202.5, 352.5, 554, shared_ptr<material>()));
  return lights;
}


//  std::cout << "+++++++++ Load Obj +++++++++" << std::endl;
//  // OBJモデルの読み込み
//  shared_ptr<geometry> obj = make_shared<geometry>("./assets/obj/kugizarashi.obj", glass);
//  auto obj_bvh = make_shared<translate>(make_shared<bvh_node>(obj, 0, 1), vec3(265, 50, 265));
//  world.add(obj_bvh);
//  std::cout << "++++++++++ Finish ++++++++++" << std::endl;

#endif //RTCAMP2022_SRC_SCENE_SCENE_H_

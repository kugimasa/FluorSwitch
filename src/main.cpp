#include <iostream>
#include <chrono>
#include "objects/sphere.h"
#include "objects/aarect.h"
#include "objects/box.h"
#include "objects/constant_medium.h"
#include "utils/hittable_list.h"
#include "utils/output_file.h"
#include "utils/my_print.h"
#include "camera/camera.h"
#include "material/material.h"
#include "material/light.h"
#include "objects/cornell_box.h"

vec3 ray_color(const ray &r, const color &background, const hittable &world, int depth) {
  hit_record rec;

  /// レイの最大反射後
  if (depth <= 0) {
    return BLACK;
  }

  /// 背景色
  if (!world.hit(r, 0.001, INF, rec)) {
    return background;
  }

  /// レイの反射
  ray scattered;
  color attenuation;
  color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;
  /// 再起処理
  return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

void flush_progress(double progress) {
  int bar_width = 20;
  std::cout << "\r [";
  int pos = bar_width * progress;
  for (int i = 0; i < bar_width; ++i) {
    if (i < pos) std::cout << "=";
    else if (i == pos) std::cout << ">";
    else std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " %" << std::flush;
}

void drawPix(unsigned char *data,
             unsigned int w, unsigned int h,
             unsigned int x, unsigned int y,
             unsigned int r, unsigned int g, unsigned int b) {
  unsigned char *p;
  p = data + (h - y - 1) * w * 3 + x * 3;
  p[0] = (unsigned char) r;
  p[1] = (unsigned char) g;
  p[2] = (unsigned char) b;
}

void render(unsigned char *data, unsigned int nx, unsigned int ny, int ns) {
  /// シーンデータ
  hittable_list world;

  auto red = color(.65, .05, .05);
  auto white = color(.73, .73, .73);
  auto green = color(.12, .45, .15);
  auto light = color(3, 3, 3);

  /// マテリアル設定
  auto red_mat = make_shared<lambertian>(red);
  auto white_mat = make_shared<lambertian>(white);
  auto green_mat = make_shared<lambertian>(green);
  /// 光源設定
  auto light_mat = make_shared<diffuse_light>(light);

  auto img_text = make_shared<image_texture>("../img/chill.jpg");
  auto img_mat = make_shared<lambertian>(img_text);

  cornell_box cb = cornell_box(555, 350, red_mat, green_mat, white_mat, white_mat, white_mat, light_mat);
  world.add(make_shared<hittable_list>(cb));

  shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white_mat);
  shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white_mat);

  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3(265, 0, 295));

  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3(130, 0, 65));

  /// 関与媒質で書き換え
  world.add(make_shared<constant_medium>(box1, 0.01, BLACK));
  world.add(make_shared<constant_medium>(box2, 0.01, KUGI_COLOR));

  /// 背景
  color background = BLACK;

  /// カメラ設定
  point3 lookfrom(278.0, 278.0, -800.0);
  point3 lookat(278.0, 278.0, 0.0);
  double vfov{40.0};
  double dist_to_focus{10.0};
  double aperture{0.0};
  int max_depth = 4;
  double aspect = double(nx) / double(ny);
  double t0{0.0}, t1{1.0};
  camera cam(lookfrom, lookat, Y_UP, vfov, aspect, aperture, dist_to_focus, t0, t1);
  double progress{0.0};
  int img_size = nx * ny;
  std::cout << "========== Render ==========" << std::endl;

  // chrono変数
  std::chrono::system_clock::time_point start, end;
  // 時間計測開始
  start = std::chrono::system_clock::now();

  for (int j = 0; j < ny; j++) {
    for (int i = 0; i < nx; i++) {
      vec3 col(0, 0, 0);
      for (int s = 0; s < ns; s++) {
        double u = double(i + drand48()) / double(nx);
        double v = double(j + drand48()) / double(ny);
        ray r = cam.get_ray(u, v);
        col += ray_color(r, background, world, max_depth);
      }
      col /= double(ns);
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
      int ir = int(255.99 * col[0]);
      int ig = int(255.99 * col[1]);
      int ib = int(255.99 * col[2]);
      progress = double(i + j * nx) / img_size;
      flush_progress(progress);
      drawPix(data, nx, ny, i, j, ir, ig, ib);
    }
  }

  // 時間計測終了
  end = std::chrono::system_clock::now();
  std::cout << "\n========== Finish ==========" << std::endl;
  // 経過時間の算出
  double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "Rendered Time: " << elapsed / 1000.0 << "(sec)" << std::endl;
}

int main() {
  int nx = 600;
  int ny = 600;
  int ns = 200;

  /// BitMap
  BITMAPDATA_t output;
  output.width = nx;
  output.height = ny;
  output.ch = 3;
  /// Malloc
  output.data = (unsigned char *) malloc(sizeof(unsigned char) * output.width * output.height * output.ch);
  if (output.data == NULL) {
    error_print("Memory Allocation Error");
    return -1;
  }

  /// 背景色の指定
  memset(output.data, 0xFF, output.width * output.height * output.ch);
  /// 描画処理
  render(output.data, nx, ny, ns);

  if (pngFileEncodeWrite(&output, "output.png")) {
    freeBitmapData(&output);
    return -1;
  }
  freeBitmapData(&output);
  return 0;
}
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
             const color pix_color,
             int samples_per_pixel) {

  auto r = pix_color.x();
  auto g = pix_color.y();
  auto b = pix_color.z();

  // サンプル数の平均 + ガンマ補正(gamma=2.0)
  auto scale = 1.0 / samples_per_pixel;
  r = sqrt(scale * r);
  g = sqrt(scale * g);
  b = sqrt(scale * b);

  unsigned char *p;
  p = data + (h - y - 1) * w * 3 + x * 3;
  p[0] = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
  p[1] = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
  p[2] = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
}

void render(unsigned char *data, unsigned int nx, unsigned int ny, int ns) {
  /// シーンデータ
  hittable_list world;

  auto red = color(.65, .05, .05);
  auto white = color(.73, .73, .73);
  auto green = color(.12, .45, .15);
  auto blue = color(.05, .05, .65);
  auto light = color(7, 7, 7);

  /// マテリアル設定
  auto red_mat = make_shared<lambertian>(red);
  auto white_mat = make_shared<lambertian>(white);
  auto green_mat = make_shared<lambertian>(green);
  auto blue_mat = make_shared<lambertian>(blue);
  /// 光源設定
  auto light_mat = make_shared<diffuse_light>(light);

  auto img_text = make_shared<image_texture>("../img/chill_centered.jpg");
  auto img_mat = make_shared<lambertian>(img_text);

  cornell_box cb = cornell_box(555, 350, red_mat, green_mat, white_mat, white_mat, blue_mat, light_mat);
  world.add(make_shared<hittable_list>(cb));

  auto chill_ball = make_shared<sphere>(point3(277.5, 277.5, 277.5), 150, img_mat);
  auto chill_medium = make_shared<constant_medium>(chill_ball, 0.005, img_text);
  world.add(chill_medium);

  /// 背景
  color background = BLACK;

  /// カメラ設定
  point3 lookfrom(278.0, 278.0, -800.0);
  point3 lookat(278.0, 278.0, 0.0);
  double vfov{40.0};
  double dist_to_focus{10.0};
  double aperture{0.0};
  int max_depth = 50;
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
      progress = double(i + j * nx) / img_size;
      flush_progress(progress);
      drawPix(data, nx, ny, i, j, col, ns);
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
  int ns = 10000;

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
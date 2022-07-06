#include <iostream>
#include <chrono>
#include "camera/camera.h"
#include "material/material.h"
#include "material/light.h"
#include "objects/sphere.h"
#include "objects/aarect.h"
#include "objects/box.h"
#include "objects/constant_medium.h"
#include "objects/cornell_box.h"
#include "sampling/pdf.h"
#include "utils/hittable_list.h"
#include "utils/output_file.h"
#include "utils/my_print.h"

vec3 ray_color(const ray &r,
               const color &background,
               const hittable &world,
               shared_ptr<hittable> &lights,
               int depth) {
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
  scattered_record s_rec;
  color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

  /// 光源にヒットした場合
  if (!rec.mat_ptr->scatter(r, rec, s_rec))
    return emitted;

  /// 鏡面
  if (s_rec.is_specular) {
    return s_rec.attenuation * ray_color(s_rec.specular_ray, background, world, lights, depth - 1);
  }

  /// TODO: 蛍光実装する場合はここで分岐??
  /// if(s_rec.is_fluor) {}

  auto light_pdf = make_shared<hittable_pdf>(lights, rec.p);
  mixture_pdf mixture_pdf(light_pdf, s_rec.pdf_ptr);

  ray scattered = ray(rec.p, mixture_pdf.generate(), r.time());
  auto pdf_val = mixture_pdf.value(scattered.direction());

  /// 再起処理
  return emitted + s_rec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
      * ray_color(scattered, background, world, lights, depth - 1) / pdf_val;
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
  auto light = color(3, 3, 3);

  /// マテリアル設定
  auto red_mat = make_shared<lambertian>(red);
  auto white_mat = make_shared<lambertian>(white);
  auto green_mat = make_shared<lambertian>(green);
  auto blue_mat = make_shared<lambertian>(blue);
  auto aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
  auto glass = make_shared<dielectric>(1.5);

  /// 光源設定
  auto light_mat = make_shared<diffuse_light>(light);

  cornell_box cb = cornell_box(555, 150, red_mat, green_mat, white_mat, white_mat, blue_mat, light_mat);
  world.add(make_shared<hittable_list>(cb));

  shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3(265, 0, 295));
  world.add(box1);

  world.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

  /// 光源サンプル用
  shared_ptr<hittable> lights = make_shared<xz_rect>(202.5, 352.5, 202.5, 352.5, 554, shared_ptr<material>());
  /// ガラス球サンプル用
  // shared_ptr<hittable> lights = make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>());

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
  std::cout << "PPS: " << ns << std::endl;
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
        col += ray_color(r, background, world, lights, max_depth);
      }
      progress = double(i + j * nx) / img_size;
#ifndef NDEBUG
      flush_progress(progress);
#endif
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

#define CHANNEL_NUM 3

int main() {
  int nx = 600;
  int ny = 600;
  int ns = 10;

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

  /// PNG出力
  if (stbi_write_png("output.png", nx, ny, CHANNEL_NUM, output.data, nx * CHANNEL_NUM)
      != 1) {
    error_print("Image Save Error");
    return -1;
  }

  freeBitmapData(&output);
  return 0;
}
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include "camera/camera.h"
#include "material/material.h"
#include "material/light.h"
#include "objects/sphere.h"
#include "objects/aarect.h"
#include "objects/box.h"
#include "objects/constant_medium.h"
#include "objects/cornell_box.h"
#include "objects/geometry.h"
#include "objects/triangle.h"
#include "sampling/pdf.h"
#include "utils/hittable_list.h"
#include "utils/output_file.h"
#include "utils/my_print.h"
#include "utils/bvh.h"

vec3 ray_color(const ray &r,
               const color &background,
               const hittable &world,
               shared_ptr<hittable_list> &lights,
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

  auto ray_c = ray_color(scattered, background, world, lights, depth - 1);

  /// 再起処理
  return emitted + s_rec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) * ray_c / pdf_val;
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

  // NaNを除外
  // NaN同士の比較は成り立たない
  if (r != r) r = 0.0;
  if (b != b) b = 0.0;
  if (g != g) g = 0.0;

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

// 30fps * 5 sec
#define MAX_FRAME 150
void render(unsigned char *data, unsigned int nx, unsigned int ny, int ns, int frame = 1) {
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
  auto kugi_mat = make_shared<lambertian>(KUGI_COLOR);

  /// 光源設定
  auto light_mat = make_shared<diffuse_light>(light);

  cornell_box cb = cornell_box(555, 150, red_mat, green_mat, white_mat, white_mat, blue_mat, light_mat);
  world.add(make_shared<hittable_list>(cb));

//  std::cout << "+++++++++ Load Obj +++++++++" << std::endl;
//  // OBJモデルの読み込み
//  shared_ptr<geometry> obj = make_shared<geometry>("./assets/obj/kugizarashi.obj", glass);
//  auto obj_bvh = make_shared<translate>(make_shared<bvh_node>(obj, 0, 1), vec3(265, 50, 265));
//  world.add(obj_bvh);
//  std::cout << "++++++++++ Finish ++++++++++" << std::endl;

  /// 拡大縮小/移動する球
  double f = (double) frame / (MAX_FRAME * 2);
  double offset = 50;
  double radius = 90 + sin(f * 2 * PI) * offset;
  world.add(make_shared<sphere>(vec3(radius + 100, 90, radius + 100), radius, kugi_mat));

  auto lights = make_shared<hittable_list>();
  /// 光源サンプル用
  lights->add(make_shared<xz_rect>(202.5, 352.5, 202.5, 352.5, 554, shared_ptr<material>()));

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

  for (int j = 0; j < ny; j++) {
    for (int i = 0; i < nx; i++) {
      vec3 col(0, 0, 0);
      for (int s = 0; s < ns; s++) {
        double u = double(i + drand48()) / double(nx);
        double v = double(j + drand48()) / double(ny);
        ray r = cam.get_ray(u, v);
        col += ray_color(r, background, world, lights, max_depth);
      }
#ifndef NDEBUG
      progress = double(i + j * nx) / img_size;
      flush_progress(progress);
#endif
      drawPix(data, nx, ny, i, j, col, ns);
    }
  }
}

#define CHANNEL_NUM 3

// メインの処理
void execute() {
  int nx = 600;
  int ny = 600;
  int ns = 1;
  std::cout << "PPS: " << ns << std::endl;
  std::cout << "========== Render ==========" << std::endl;

  /// BitMap
  BITMAPDATA_t output;
  output.width = nx;
  output.height = ny;
  output.ch = 3;

  // chrono変数
  std::chrono::system_clock::time_point start, end;

  for (int frame = 1; frame <= MAX_FRAME; ++frame) {
    // 時間計測開始
    start = std::chrono::system_clock::now();
    /// Malloc
    output.data = (unsigned char *) malloc(sizeof(unsigned char) * output.width * output.height * output.ch);
    if (output.data == NULL) {
      error_print("Memory Allocation Error");
      exit(-1);
    }

    /// 背景色の指定
    memset(output.data, 0xFF, output.width * output.height * output.ch);
    /// 描画処理
    render(output.data, nx, ny, ns, frame);

    /// PNG出力
    std::ostringstream sout;
    sout << std::setw(3) << std::setfill('0') << frame;
    std::string output_file = sout.str() + ".png";
    if (stbi_write_png(output_file.c_str(), nx, ny, CHANNEL_NUM, output.data, nx * CHANNEL_NUM) != 1) {
      error_print("Image Save Error");
      exit(-1);
    }

    freeBitmapData(&output);
    // 時間計測終了
    end = std::chrono::system_clock::now();
    // 経過時間の算出
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\n[" << sout.str() << "]: " << elapsed / 1000.0 << "(sec)s" << std::endl;
  }
  std::cout << "\n========== Finish ==========" << std::endl;
  exit(0);
}

int main() {
  // 実行開始
  std::thread timer(program_timer);
  std::thread exec(execute);
  timer.join();
  exec.join();
  return 0;
}
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include "camera/camera.h"
#include "material/material.h"
#include "material/light.h"
#include "material/spectral_material.h"
#include "material/spectral_light.h"
#include "objects/sphere.h"
#include "objects/aarect.h"
#include "objects/box.h"
#include "objects/constant_medium.h"
#include "objects/cornell_box.h"
#include "objects/geometry.h"
#include "objects/triangle.h"
#include "render/path_trace.h"
#include "render/spectral_path_trace.h"
#include "sampling/pdf.h"
#include "scene/scene.h"
#include "utils/hittable_list.h"
#include "utils/output_file.h"
#include "utils/spectral_distribution.h"
#include "utils/my_print.h"
#include "utils/bvh.h"

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
#define MAX_FRAME 1
template<typename color_type, typename mat>
void render(unsigned char *data, unsigned int nx, unsigned int ny, int ns,
            color_type background, hittable_list<mat> world, shared_ptr<hittable_list<mat>> &lights,
            int frame = 1) {

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

  for (int j = 0; j < ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      vec3 col(0, 0, 0);
      for (int s = 0; s < ns; ++s) {
        double u = double(i + drand48()) / double(nx);
        double v = double(j + drand48()) / double(ny);
        ray r = cam.get_ray(u, v);
        // col += path_trace(r, background, world, lights, max_depth);
        auto spectra(spectral_path_trace(r, background, world, lights, max_depth));
        col += spectralToRgb(spectra);
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
  int ns = 25;
  std::cout << "PPS: " << ns << std::endl;
  std::cout << "========== Render ==========" << std::endl;

  /// BitMap
  BITMAPDATA_t output;
  output.width = nx;
  output.height = ny;
  output.ch = 3;

#ifndef NDEBUG
  // chrono変数
  std::chrono::system_clock::time_point start, end;
#endif

  /// シーン背景
  spectral_distribution background_spectra{black_spectra, 0.0};
  color background_rgb = BLACK;

  for (int frame = 1; frame <= MAX_FRAME; ++frame) {
#ifndef NDEBUG
    // 時間計測開始
    start = std::chrono::system_clock::now();
#endif
    /// Malloc
    output.data = (unsigned char *) malloc(sizeof(unsigned char) * output.width * output.height * output.ch);
    if (output.data == NULL) {
      error_print("Memory Allocation Error");
      exit(-1);
    }

    /// 背景色の指定
    memset(output.data, 0xFF, output.width * output.height * output.ch);
    /// シーンデータ
    auto world = construct_spectral_scene(frame, MAX_FRAME);
    auto lights = construct_spectral_light_sampler();
    /// 描画処理
    render(output.data, nx, ny, ns, background_spectra, world, lights, frame);

    /// PNG出力
    std::ostringstream sout;
    sout << std::setw(3) << std::setfill('0') << frame;
    std::string output_file = sout.str() + ".png";
    if (stbi_write_png(output_file.c_str(), nx, ny, CHANNEL_NUM, output.data, nx * CHANNEL_NUM) != 1) {
      error_print("Image Save Error");
      exit(-1);
    }

    freeBitmapData(&output);
#ifndef NDEBUG
    // 時間計測終了
    end = std::chrono::system_clock::now();
    // 経過時間の算出
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\n[" << sout.str() << "]: " << elapsed * 0.001 << "(sec)s" << std::endl;
#endif
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
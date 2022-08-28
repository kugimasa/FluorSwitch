#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <omp.h>
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
#include "sampling/spectral_pdf.h"

// メインの処理
void execute() {
//#ifndef NDEBUG
  // chrono変数
  std::chrono::system_clock::time_point exec_start, start, end;
  exec_start = std::chrono::system_clock::now();
//#endif
  int nx = 600;
  int ny = 600;
  std::cout << "PPS(RGB): " << RGB_PPS << std::endl;
  std::cout << "PPS(SPECTRAL): " << SPECTRAL_PPS << std::endl;
  std::cout << "ray bounce(RGB): " << RGB_MAX_RAY_DEPTH << std::endl;
  std::cout << "ray bounce(SPECTRAL): " << SPECTRAL_MAX_RAY_DEPTH << std::endl;
  std::cout << "wavelength sample: " << WAVELENGTH_SAMPLE_SIZE << std::endl;
  std::cout << "OpenMP threads: " << MAX_THREAD_NUM << " / " << omp_get_max_threads() << std::endl;
  std::cout << "========== Render ==========" << std::endl;

  /// BitMap
  BITMAPDATA_t output;
  output.width = nx;
  output.height = ny;
  output.ch = CHANNEL_NUM;

  // 描画開始
  auto rgb_lights = construct_light_sampler();
  auto spectral_lights = construct_spectral_light_sampler();
  // 波長はフルセットを使用
  auto sample_wavelengths = full_wavelengths();

  for (int frame = 1; frame <= MAX_FRAME; ++frame) {
//#ifndef NDEBUG
    // 時間計測開始
    start = std::chrono::system_clock::now();
//#endif
    /// Malloc
    output.data = (unsigned char *) malloc(sizeof(unsigned char) * output.width * output.height * output.ch);
    if (output.data == NULL) {
      error_print("Memory Allocation Error");
      exit(-1);
    }

    /// 背景色の指定
    memset(output.data, 0xFF, output.width * output.height * output.ch);

    if (frame <= RGB_END_FRAME) {
      /// RGBレンダリング
      auto world = construct_scene(frame, RGB_END_FRAME);
      rgb_render(output.data, nx, ny, RGB_PPS, world, rgb_lights, frame);
    } else {
      /// スペクトラルレンダリング
      // auto sample_wavelengths = random_sample_wavelengths();
      // auto sample_wavelengths = importance_sample_wavelengths();
      auto world = construct_spectral_scene(frame, MAX_FRAME);
      spectral_render(output.data, nx, ny, SPECTRAL_PPS, sample_wavelengths, world, spectral_lights, frame);
    }

    /// PNG出力
    std::ostringstream sout;
    sout << std::setw(3) << std::setfill('0') << frame;
    std::string output_file = sout.str() + ".png";
    if (stbi_write_png(output_file.c_str(), nx, ny, CHANNEL_NUM, output.data, nx * CHANNEL_NUM) != 1) {
      error_print("Image Save Error");
      exit(-1);
    }

    freeBitmapData(&output);
//#ifndef NDEBUG
    // 時間計測終了
    end = std::chrono::system_clock::now();
    // 経過時間の算出
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\n[" << sout.str() << "]: " << elapsed * 0.001 << "(sec)" << std::endl;
//#endif
  }
  std::cout << "\n========== Finish ==========" << std::endl;
  // 時間計測終了
  end = std::chrono::system_clock::now();
  // 経過時間の算出
  double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - exec_start).count();
  std::cout << "\nExecution Time: " << elapsed * 0.001 << "(sec)" << std::endl;
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
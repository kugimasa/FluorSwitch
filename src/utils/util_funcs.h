#ifndef FLUORSWITCH_SRC_UTILS_UTIL_FUNCS_H_
#define FLUORSWITCH_SRC_UTILS_UTIL_FUNCS_H_

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr long LIMIT_SEC = 599;

inline double degrees_to_radians(double degrees) {
  // 1 / 180.0 = 0.00555555555
  return degrees * M_PI * 0.00555555555;
}

inline double clamp(double x, double min, double max) {
  if (x < min) return min;
  if (x > max) return max;
  return x;
}

// [0,1)の値をランダムで返す
inline double random_double() {
  return rand() / (RAND_MAX + 1.0);
}

// [min,max)のdoubleをランダムで返す
inline double random_double(double min, double max) {
  return min + (max - min) * random_double();
}

// [min,max)のintをランダムで返す
inline int random_int(int min, int max) {
  return static_cast<int>(random_double(min, max + 1));
}

// プログラムタイマー
inline void program_timer() {
#ifndef NDEBUG
  // chrono変数
  std::chrono::system_clock::time_point start, end;
  // 時間計測開始
  start = std::chrono::system_clock::now();
#endif

  int count = LIMIT_SEC * 0.1;
  for (int i = 0; i < count; ++i) {
    std::this_thread::sleep_for(std::chrono::seconds(10));
#ifndef NDEBUG
    end = std::chrono::system_clock::now();
    // 経過時間の算出
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    std::cout << "\n[Timer] elapsed time: " << elapsed << "(sec)" << std::endl;
#endif
  }
  // 正常終了
  exit(0);
}

#include "ray.h"
#include "vec3.h"
#include "colors.h"

#define RGB_PPS 15 // 15
#define SPECTRAL_PPS 8 // 8
#define RGB_STOP_FRAME 5
#define RGB_MAX_FRAME 60
#define SPECTRAL_STOP_FRAME (90 - RGB_MAX_FRAME)
#define MAX_FRAME 100
#define MAX_THREAD_NUM 1
#define MAX_RAY_DEPTH 8 // 8
#define CHANNEL_NUM 3

// シーン用の情報
#define SPHERE_RADIUS 55
#define LIGHT_WIDTH 150
#define SPHERE_RGB_START_X 100
#define SPHERE_SPECTRAL_START_X (SPHERE_RGB_START_X + (545 - SPHERE_RGB_START_X - SPHERE_RADIUS))
#define SPHERE_SPECTRAL_END_X 300
#define SPHERE_Z 200

inline color gamma_correct(const color col) {
  auto r = col.x();
  auto g = col.y();
  auto b = col.z();

  // NaNを除外
  // NaN同士の比較は成り立たない
  if (r != r) r = 0.0;
  if (b != b) b = 0.0;
  if (g != g) g = 0.0;

  // サンプル数の平均 + ガンマ補正(gamma=2.0
  r = sqrt(r);
  g = sqrt(g);
  b = sqrt(b);

  return {r, g, b};
}

inline void drawPix(unsigned char *data,
                    unsigned int w, unsigned int h,
                    unsigned int x, unsigned int y,
                    const color pix_color) {

  auto r = pix_color.x();
  auto g = pix_color.y();
  auto b = pix_color.z();

  unsigned char *p;
  p = data + (h - y - 1) * w * 3 + x * 3;
  p[0] = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
  p[1] = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
  p[2] = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
}

#endif //FLUORSWITCH_SRC_UTILS_UTIL_FUNCS_H_

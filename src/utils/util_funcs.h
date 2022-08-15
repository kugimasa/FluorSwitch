#ifndef RTCAMP2022_SRC_UTILS_UTIL_FUNCS_H_
#define RTCAMP2022_SRC_UTILS_UTIL_FUNCS_H_

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

#endif //RTCAMP2022_SRC_UTILS_UTIL_FUNCS_H_

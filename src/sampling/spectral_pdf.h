#ifndef RTCAMP2022_SRC_SAMPLING_SPECTRAL_PDF_H_
#define RTCAMP2022_SRC_SAMPLING_SPECTRAL_PDF_H_

#include <numeric>
#include <random>
#include <algorithm>
#include "../utils/spectral_distribution.h"

class spectral_pdf {
 public:
  virtual ~spectral_pdf() {}

  virtual double value() const = 0;
  virtual std::vector<size_t> generate() const = 0;
};

/// 一様サンプル
inline std::vector<size_t> random_sample_wavelengths(size_t full_size, size_t sample_size) {
  std::vector<size_t> indices(full_size), out;
  std::iota(indices.begin(), indices.end(), 0);
  std::sample(indices.begin(), indices.end(), std::back_inserter(out), sample_size, std::mt19937{std::random_device{}()});
  // 昇順ソート
  std::sort(out.begin(), out.end());
  return out;
}

/// 波長を考慮したサンプル
inline std::vector<size_t> importance_sample_wavelengths(size_t full_size, size_t sample_size) {
  std::vector<size_t> indices(full_size), out;
  /// TODO: 波長をインポータンスサンプル
  return out;
}

#endif //RTCAMP2022_SRC_SAMPLING_SPECTRAL_PDF_H_

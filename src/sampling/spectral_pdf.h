#ifndef RTCAMP2022_SRC_SAMPLING_SPECTRAL_PDF_H_
#define RTCAMP2022_SRC_SAMPLING_SPECTRAL_PDF_H_

#include <numeric>
#include <random>
#include <algorithm>
#include "../utils/spectral_distribution.h"

class spectral_pdf {
 public:
  spectral_pdf(const spectral_distribution &p) {
    pdf = p;
    // CDFを計算
    cdf = pdf.calc_cdf();
  }

 public:
  spectral_distribution pdf;
  spectral_distribution cdf;
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
inline std::vector<size_t> importance_sample_wavelengths(const spectral_pdf &mix_pdf, size_t sample_size) {
  std::vector<double> random_num;
  std::vector<size_t> indices;
  for (int i = 0; i < sample_size; ++i) {
    random_num.push_back(random_double());
  }
  // 昇順ソート
  std::sort(random_num.begin(), random_num.end());
  int x = 0;
  for (int y = 0; y < random_num.size(); ++y) {
    double tmp_y = random_num[y];
    while (x < mix_pdf.cdf.size()) {
      double cdf_val = mix_pdf.cdf.get_intensity(x);
      if (tmp_y <= cdf_val) {
        indices.push_back(x);
        ++x;
        break;
      }
      ++x;
    }
  }
  return indices;
}

#endif //RTCAMP2022_SRC_SAMPLING_SPECTRAL_PDF_H_

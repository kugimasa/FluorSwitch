#ifndef FLUORSWITCH_SRC_SAMPLING_SPECTRAL_PDF_H_
#define FLUORSWITCH_SRC_SAMPLING_SPECTRAL_PDF_H_

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

/// フルサンプル
inline std::vector<size_t> full_wavelengths() {
  std::vector<size_t> indices(full_wavelength_size);
  std::iota(indices.begin(), indices.end(), 0);
  return indices;
}

/// 一様サンプル
inline std::vector<size_t> random_sample_wavelengths() {
  std::vector<size_t> indices(full_wavelength_size), out;
  std::iota(indices.begin(), indices.end(), 0);
  std::sample(indices.begin(), indices.end(), std::back_inserter(out), WAVELENGTH_SAMPLE_SIZE, std::mt19937{std::random_device{}()});
  // 昇順ソート
  std::sort(out.begin(), out.end());
  return out;
}

/// 波長を考慮したサンプル

inline spectral_distribution calc_fluorescent_pdf() {
  double inv_k_f = 1 / emission_spectra.sum();
  double w_f = 4 * M_PI * SPHERE_RADIUS * SPHERE_RADIUS * inv_k_f;
  return emission_spectra * w_f;
}

inline spectral_distribution calc_light_pdf() {
  double inv_k_l = 1 / uv_spectra.sum();
  double w_l = LIGHT_WIDTH * LIGHT_WIDTH * inv_k_l;
  return uv_spectra * w_l;
}

auto fluor_light_pdf = spectral_pdf(calc_light_pdf() * 0.5 + calc_fluorescent_pdf() * 0.5);

inline std::vector<size_t> importance_sample_wavelengths() {
  std::vector<double> random_num;
  std::vector<size_t> indices;
  for (int i = 0; i < WAVELENGTH_SAMPLE_SIZE; ++i) {
    random_num.push_back(random_double());
  }
  // 昇順ソート
  std::sort(random_num.begin(), random_num.end());
  int x = 0;
  for (int y = 0; y < random_num.size(); ++y) {
    double tmp_y = random_num[y];
    while (x < fluor_light_pdf.cdf.size()) {
      double cdf_val = fluor_light_pdf.cdf.get_intensity(x);
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

#endif //FLUORSWITCH_SRC_SAMPLING_SPECTRAL_PDF_H_

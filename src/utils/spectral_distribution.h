#ifndef RTCAMP2022_SRC_UTILS_SPECTRAL_DISTRIBUTION_H_
#define RTCAMP2022_SRC_UTILS_SPECTRAL_DISTRIBUTION_H_
#include <algorithm>
#include <numeric>
#include <random>
#include "csv.h"

const vec3 srgb_d65_vec0{3.2404542, -1.5371385, -0.4985314};
const vec3 srgb_d65_vec1{-0.9692660, 1.8760108, 0.0415560};
const vec3 srgb_d65_vec2{0.0556434, -0.2040259, 1.0572252};

class spectral_distribution {
 public:
  spectral_distribution() {}
  spectral_distribution(const spectral_distribution &distribution);
  spectral_distribution(const spectral_distribution &distribution, std::vector<size_t> sample_indices);
  spectral_distribution(const spectral_distribution &distribution, const double intensity);
  spectral_distribution(const char *file_path);

  inline size_t get_index_wavelength() const {
    return index_wavelength;
  }

  inline size_t get_wavelength(const size_t index) const {
    return wavelengths[index];
  }

  inline double get_intensity(const size_t index) const {
    return intensities[index];
  }

  inline size_t size() const {
    return wavelengths.size();
  }

  inline double sum() const {
    double sum = 0;
    for (int i = 0; i < wavelengths.size(); ++i) {
      sum += intensities[i];
    }
    return sum;
  }

  inline spectral_distribution operator+(const spectral_distribution &other) const;
  inline spectral_distribution operator+(double t) const;
  inline spectral_distribution operator-(const spectral_distribution &other) const;
  inline spectral_distribution operator-(double t) const;
  inline spectral_distribution operator*(const spectral_distribution &other) const;
  inline spectral_distribution operator*(double t) const;
  inline spectral_distribution operator/(const spectral_distribution &other) const;
  inline spectral_distribution operator/(double t) const;

 private:
  size_t index_wavelength{0};
  std::vector<size_t> wavelengths;
  std::vector<double> intensities;
};

spectral_distribution::spectral_distribution(const spectral_distribution &distribution) {
  if (distribution.wavelengths.size() > 0) {
    index_wavelength = distribution.wavelengths[0];
  }
  wavelengths = distribution.wavelengths;
  intensities = distribution.intensities;
}

spectral_distribution::spectral_distribution(const spectral_distribution &distribution, std::vector<size_t> sample_indices) {
  std::vector<size_t> _wavelengths;
  std::vector<double> _intensities;
  for (size_t index = 0; index != sample_indices.size(); ++index) {
    _wavelengths.push_back(distribution.get_wavelength(sample_indices.at(index)));
    _intensities.push_back(distribution.get_intensity(sample_indices.at(index)));
  }
  index_wavelength = _wavelengths[0];
  wavelengths = _wavelengths;
  intensities = _intensities;
}

spectral_distribution::spectral_distribution(const spectral_distribution &distribution, const double intensity) {
  if (distribution.wavelengths.size() > 0) {
    index_wavelength = distribution.wavelengths[0];
  }
  wavelengths = distribution.wavelengths;
  intensities = distribution.intensities;
  for (int i = 0; i < wavelengths.size(); ++i) {
    intensities[i] = intensity;
  }
}

spectral_distribution::spectral_distribution(const char *file_path) {
  io::CSVReader<2> in(file_path);
  in.next_line();
  in.read_header(io::ignore_extra_column, "Wavelength", "Intensity");
  size_t wavelength;
  double intensity;
  while (in.read_row(wavelength, intensity)) {
    wavelengths.push_back(wavelength);
    intensities.push_back(intensity);
  }
  index_wavelength = wavelengths[0];
}

spectral_distribution spectral_distribution::operator+(const spectral_distribution &other) const {
  spectral_distribution distribution{*this};
  for (size_t index = 0; index < distribution.size(); ++index) {
    distribution.intensities[index] = intensities[index] + other.intensities[index];
  }
  return distribution;
}
spectral_distribution spectral_distribution::operator+(const double t) const {
  spectral_distribution distribution{*this};
  for (size_t index = 0; index < distribution.size(); ++index) {
    distribution.intensities[index] = intensities[index] + t;
  }
  return distribution;
}
spectral_distribution spectral_distribution::operator-(const spectral_distribution &other) const {
  spectral_distribution distribution{*this};
  for (size_t index = 0; index < distribution.size(); ++index) {
    distribution.intensities[index] = ffmax(intensities[index] - other.intensities[index], 0.0);
  }
  return distribution;
}
spectral_distribution spectral_distribution::operator-(const double t) const {
  spectral_distribution distribution{*this};
  for (size_t index = 0; index < distribution.size(); ++index) {
    distribution.intensities[index] = ffmax(intensities[index] - t, 0.0);
  }
  return distribution;
}
spectral_distribution spectral_distribution::operator*(const spectral_distribution &other) const {
  spectral_distribution distribution{*this};
  for (size_t index = 0; index < distribution.size(); ++index) {
    distribution.intensities[index] = intensities[index] * other.intensities[index];
  }
  return distribution;
}
spectral_distribution spectral_distribution::operator*(const double t) const {
  spectral_distribution distribution{*this};
  for (size_t index = 0; index < distribution.size(); ++index) {
    distribution.intensities[index] = intensities[index] * t;
  }
  return distribution;
}
spectral_distribution spectral_distribution::operator/(const spectral_distribution &other) const {
  spectral_distribution distribution{*this};
  for (size_t index = 0; index < distribution.size(); ++index) {
    if (other.intensities[index] != 0.0) {
      distribution.intensities[index] = intensities[index] / other.intensities[index];
    }
  }
  return distribution;
}
spectral_distribution spectral_distribution::operator/(const double t) const {
  spectral_distribution distribution{*this};
  if (t != 0.0) {
    for (size_t index = 0; index < distribution.size(); ++index) {
      distribution.intensities[index] = intensities[index] / t;
    }
  }
  return distribution;
}

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

// 事前に読み込み
const auto x_bar = spectral_distribution("./assets/spectra/xyz/cie_sco_2degree_xbar.csv");
const auto y_bar = spectral_distribution("./assets/spectra/xyz/cie_sco_2degree_ybar.csv");
const auto z_bar = spectral_distribution("./assets/spectra/xyz/cie_sco_2degree_zbar.csv");
const auto blue_spectra = spectral_distribution("./assets/spectra/macbeth_08_purplish_blue.csv");
const auto red_spectra = spectral_distribution("./assets/spectra/macbeth_09_moderate_red.csv");
const auto white_spectra = spectral_distribution("./assets/spectra/macbeth_19_white.csv");
const auto black_spectra = spectral_distribution("./assets/spectra/macbeth_24_black.csv");
const auto d65_spectra = spectral_distribution("./assets/spectra/cie_si_d65.csv");
const auto uv_spectra = spectral_distribution("./assets/spectra/black_light.csv") * 100;
const auto zero_spectra = spectral_distribution(black_spectra, 0.0);
const auto wavelength_sample_size = d65_spectra.size();
const auto integral_y = 106.85691688599991; // y_bar.sum()
#define WAVELENGTH_SAMPLE_SIZE 16
const double sample_factor = x_bar.size() / (integral_y * WAVELENGTH_SAMPLE_SIZE);
const auto sample_indices_k = random_sample_wavelengths(wavelength_sample_size, WAVELENGTH_SAMPLE_SIZE);
const auto zero_sample_spectra = spectral_distribution(zero_spectra, sample_indices_k);

color inline getXYZFromWavelength(size_t lambda) {
  auto index = lambda - x_bar.get_index_wavelength();
  color xyz(x_bar.get_intensity(index), y_bar.get_intensity(index), z_bar.get_intensity(index));
  return xyz;
}

color inline spectralToRgb(const spectral_distribution &distribution) {
  double X = 0, Y = 0, Z = 0;
  size_t wavelength_size = distribution.size();
  for (size_t index = 0; index < wavelength_size; ++index) {
    size_t lambda = distribution.get_wavelength(index);
    color xyz = getXYZFromWavelength(lambda);
    X += distribution.get_intensity(index) * xyz.x() * sample_factor;
    Y += distribution.get_intensity(index) * xyz.y() * sample_factor;
    Z += distribution.get_intensity(index) * xyz.z() * sample_factor;
  }
  vec3 XYZ{X, Y, Z};
  /// srgb_d65
  return {dot(srgb_d65_vec0, XYZ), dot(srgb_d65_vec1, XYZ), dot(srgb_d65_vec2, XYZ)};
}

static auto MACBETH_BLUE = spectralToRgb(blue_spectra);
static auto MACBETH_RED = spectralToRgb(red_spectra);
static auto MACBETH_WHITE = spectralToRgb(white_spectra);
static auto MACBETH_BLACK = spectralToRgb(black_spectra);
static auto D65_LIGHT = spectralToRgb(d65_spectra);
#endif //RTCAMP2022_SRC_UTILS_SPECTRAL_DISTRIBUTION_H_

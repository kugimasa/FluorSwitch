#ifndef RTCAMP2022_SRC_UTILS_SPECTRAL_DISTRIBUTION_H_
#define RTCAMP2022_SRC_UTILS_SPECTRAL_DISTRIBUTION_H_

#include "csv.h"

const vec3 srgb_d65_vec0{3.2404542, -1.5371385, -0.4985314};
const vec3 srgb_d65_vec1{-0.9692660, 1.8760108, 0.0415560};
const vec3 srgb_d65_vec2{0.0556434, -0.2040259, 1.0572252};

class spectral_distribution {
 public:
  spectral_distribution() {}
  spectral_distribution(const spectral_distribution &distribution);
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

  inline void fill(double intensity, size_t size) {

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

// 読み込みのテスト
const auto x_bar = spectral_distribution("./assets/spectra/xyz/cie_sco_2degree_xbar.csv");
const auto y_bar = spectral_distribution("./assets/spectra/xyz/cie_sco_2degree_ybar.csv");
const auto z_bar = spectral_distribution("./assets/spectra/xyz/cie_sco_2degree_zbar.csv");
const auto blue_spectra = spectral_distribution("./assets/spectra/macbeth_08_purplish_blue.csv");
const auto red_spectra = spectral_distribution("./assets/spectra/macbeth_09_moderate_red.csv");
const auto white_spectra = spectral_distribution("./assets/spectra/macbeth_19_white.csv");
const auto black_spectra = spectral_distribution("./assets/spectra/macbeth_24_black.csv");
const auto d65_spectra = spectral_distribution("./assets/spectra/cie_si_d65.csv");
const auto wavelength_sample_size = d65_spectra.size();
const auto integral_y = 106.85691688599991; // y_bar.sum()
double sample_factor = 472 / (integral_y * wavelength_sample_size);
#endif //RTCAMP2022_SRC_UTILS_SPECTRAL_DISTRIBUTION_H_

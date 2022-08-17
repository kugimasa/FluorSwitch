#ifndef RTCAMP2022_SRC_UTILS_SPECTRAL_DISTRIBUTION_H_
#define RTCAMP2022_SRC_UTILS_SPECTRAL_DISTRIBUTION_H_

#include "csv.h"

class spectral_distribution {
 public:
  spectral_distribution() {}
  spectral_distribution(const char *file_path);

  inline size_t get_wavelength(const size_t index) {
    return wavelengths[index];
  }

  inline double get_intensity(const size_t index) {
    return intensities[index];
  }

 private:
  std::vector<size_t> wavelengths;
  std::vector<double> intensities;
};

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
}

#endif //RTCAMP2022_SRC_UTILS_SPECTRAL_DISTRIBUTION_H_

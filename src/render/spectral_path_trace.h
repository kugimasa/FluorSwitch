#ifndef RTCAMP2022_SRC_RENDER_SPECTRAL_PATH_TRACE_H_
#define RTCAMP2022_SRC_RENDER_SPECTRAL_PATH_TRACE_H_

#include "../utils/spectral_distribution.h"
#include "../utils/ray.h"
#include "../utils/hittable.h"
#include "../utils/hittable_list.h"
#include "../material/spectral_material.h"

spectral_distribution inline spectral_path_trace(const ray &r,
                                                 const spectral_distribution &background,
                                                 const hittable<spectral_material> &world,
                                                 shared_ptr<hittable_list<spectral_material>> &lights,
                                                 int depth) {
  hit_record<spectral_material> rec;

  /// レイの最大反射後
  if (depth <= 0) {
    // TODO: BLACK
    return black_spectra;
  }

  /// 背景色
  if (!world.hit(r, 0.001, INF, rec)) {
    return background;
  }

  /// レイの反射
  spectral_scattered_record s_s_rec;
  spectral_distribution emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

  /// 光源にヒットした場合
  if (!rec.mat_ptr->scatter(r, rec, s_s_rec))
    return emitted;

  /// TODO: 蛍光実装する場合はここで分岐??
  /// if(s_rec.is_fluor) {}

  auto light_pdf = make_shared<hittable_pdf<spectral_material>>(lights, rec.p);
  mixture_pdf mixture_pdf(light_pdf, s_s_rec.pdf_ptr);

  ray scattered = ray(rec.p, mixture_pdf.generate(), r.time());
  auto inv_pdf_val = 1 / mixture_pdf.value(scattered.direction());

  auto ray_c = spectral_path_trace(scattered, background, world, lights, depth - 1);

  /// 再起処理
  return emitted + s_s_rec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) * ray_c * inv_pdf_val;
}

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

#endif //RTCAMP2022_SRC_RENDER_SPECTRAL_PATH_TRACE_H_

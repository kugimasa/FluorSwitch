#ifndef FLUORSWITCH_SRC_RENDER_SPECTRAL_PATH_TRACE_H_
#define FLUORSWITCH_SRC_RENDER_SPECTRAL_PATH_TRACE_H_

#include "../utils/spectral_distribution.h"
#include "../utils/ray.h"
#include "../utils/hittable.h"
#include "../utils/hittable_list.h"
#include "../utils/util_funcs.h"
#include "../material/spectral_material.h"

spectral_distribution inline spectral_path_trace(const ray &r,
                                                 const hittable<spectral_material> &world,
                                                 shared_ptr<hittable_list<spectral_material>> &lights,
                                                 int depth) {
  hit_record<spectral_material> rec;

  /// レイの最大反射後
  if (depth <= 0) {
    return zero_spectra;
  }

  /// 背景色
  if (!world.hit(r, 0.001, INF, rec)) {
    return zero_spectra;
  }

  /// レイの反射
  spectral_scattered_record s_s_rec;
  spectral_distribution emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

  /// 光源にヒットした場合
  if (!rec.mat_ptr->scatter(r, rec, s_s_rec))
    return emitted;

  auto light_pdf = make_shared<hittable_pdf<spectral_material>>(lights, rec.p);
  mixture_pdf mixture_pdf(light_pdf, s_s_rec.pdf_ptr);

  ray scattered = ray(rec.p, mixture_pdf.generate(), r.time());
  auto inv_pdf_val = 1 / mixture_pdf.value(scattered.direction());

  auto ray_c = spectral_path_trace(scattered, world, lights, depth - 1);
  /// TODO: 波長に対しての係数は必要???
  auto reflectance_spectra = s_s_rec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) * ray_c * inv_pdf_val;

  /// 蛍光の場合
  if (s_s_rec.is_fluor) {
    auto K = s_s_rec.excitation * ray_c;
    return emitted + reflectance_spectra + s_s_rec.emission * K.sum() * inv_wave_pdf_val;
  }

  /// 再起処理
  return emitted + reflectance_spectra;
}

void inline spectral_render(unsigned char *data, unsigned int nx, unsigned int ny, int ns,
                            std::vector<size_t> sample_wavelengths,
                            hittable_list<spectral_material> world, shared_ptr<hittable_list<spectral_material>> &lights,
                            int frame = 1) {
  spectral_distribution spectra{zero_spectra, sample_wavelengths};
  spectral_distribution zero{spectra};
  #pragma omp parallel for private(spectra) schedule(dynamic, 1) num_threads(MAX_THREAD_NUM)
  for (int j = 0; j < ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      spectra = zero;
      for (int s = 0; s < ns; ++s) {
        double u = double(i + drand48()) / double(nx);
        double v = double(j + drand48()) / double(ny);
        ray r = SCENE_CAMERA.get_ray(u, v);
        spectra = spectra + spectral_path_trace(r, world, lights, SPECTRAL_MAX_RAY_DEPTH);
      }

      auto col = spectralToRgb(spectra / ns);
      col = gamma_correct(col);
      drawPix(data, nx, ny, i, j, col);
    }
  }
}

#endif //FLUORSWITCH_SRC_RENDER_SPECTRAL_PATH_TRACE_H_

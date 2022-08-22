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
    return spectral_distribution{black_spectra, 0.0};
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

  auto light_pdf = make_shared<hittable_pdf<spectral_material>>(lights, rec.p);
  mixture_pdf mixture_pdf(light_pdf, s_s_rec.pdf_ptr);

  ray scattered = ray(rec.p, mixture_pdf.generate(), r.time());
  auto inv_pdf_val = 1 / mixture_pdf.value(scattered.direction());

  auto ray_c = spectral_path_trace(scattered, background, world, lights, depth - 1);

  /// 再起処理
  return emitted + s_s_rec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) * ray_c * inv_pdf_val;
}

#endif //RTCAMP2022_SRC_RENDER_SPECTRAL_PATH_TRACE_H_
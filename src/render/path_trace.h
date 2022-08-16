#ifndef RTCAMP2022_SRC_RENDER_PATH_TRACE_H_
#define RTCAMP2022_SRC_RENDER_PATH_TRACE_H_

#include "../utils/vec3.h"
#include "../utils/ray.h"
#include "../utils/hittable.h"
#include "../utils/hittable_list.h"
#include "../material/material.h"

color inline path_trace(const ray &r,
                        const color &background,
                        const hittable<material> &world,
                        shared_ptr<hittable_list<material>> &lights,
                        int depth) {
  hit_record<material> rec;

  /// レイの最大反射後
  if (depth <= 0) {
    return BLACK;
  }

  /// 背景色
  if (!world.hit(r, 0.001, INF, rec)) {
    return background;
  }

  /// レイの反射
  scattered_record s_rec;
  color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

  /// 光源にヒットした場合
  if (!rec.mat_ptr->scatter(r, rec, s_rec))
    return emitted;

  /// 鏡面
  if (s_rec.is_specular) {
    return s_rec.attenuation * path_trace(s_rec.specular_ray, background, world, lights, depth - 1);
  }

  /// TODO: 蛍光実装する場合はここで分岐??
  /// if(s_rec.is_fluor) {}

  auto light_pdf = make_shared<hittable_pdf<material>>(lights, rec.p);
  mixture_pdf mixture_pdf(light_pdf, s_rec.pdf_ptr);

  ray scattered = ray(rec.p, mixture_pdf.generate(), r.time());
  auto pdf_val = mixture_pdf.value(scattered.direction());

  auto ray_c = path_trace(scattered, background, world, lights, depth - 1);

  /// 再起処理
  return emitted + s_rec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) * ray_c / pdf_val;
}

#endif //RTCAMP2022_SRC_RENDER_PATH_TRACE_H_

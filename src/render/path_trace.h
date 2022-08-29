#ifndef FLUORSWITCH_SRC_RENDER_PATH_TRACE_H_
#define FLUORSWITCH_SRC_RENDER_PATH_TRACE_H_

#include "../utils/vec3.h"
#include "../utils/ray.h"
#include "../utils/hittable.h"
#include "../utils/hittable_list.h"
#include "../material/material.h"

color inline path_trace(const ray &r,
                        const hittable<material> &world,
                        shared_ptr<hittable_list<material>> &lights,
                        int depth) {
  hit_record<material> rec;

  /// レイの最大反射後
  if (depth <= 0) {
    return ZERO;
  }

  /// 背景色
  if (!world.hit(r, 0.001, INF, rec)) {
    return ZERO;
  }

  /// レイの反射
  scattered_record s_rec;
  color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

  /// 光源にヒットした場合
  if (!rec.mat_ptr->scatter(r, rec, s_rec))
    return emitted;

  /// 鏡面(今回のシーンには無いのでコメントアウト)
//  if (s_rec.is_specular) {
//    return s_rec.attenuation * path_trace(s_rec.specular_ray, world, lights, depth - 1);
//  }

  auto light_pdf = make_shared<hittable_pdf<material>>(lights, rec.p);
  mixture_pdf mixture_pdf(light_pdf, s_rec.pdf_ptr);

  ray scattered = ray(rec.p, mixture_pdf.generate(), r.time());
  auto pdf_val = mixture_pdf.value(scattered.direction());

  auto ray_c = path_trace(scattered, world, lights, depth - 1);

  /// 再起処理
  return emitted + s_rec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) * ray_c / pdf_val;
}

void rgb_render(unsigned char *data, unsigned int nx, unsigned int ny, int ns,
                hittable_list<material> world, shared_ptr<hittable_list<material>> &lights,
                int frame = 1) {

  color col = ZERO;
  #pragma omp parallel for private(col) schedule(dynamic, 1) num_threads(MAX_THREAD_NUM)
  for (int j = 0; j < ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      col = ZERO;
      for (int s = 0; s < ns; ++s) {
        double u = double(i + drand48()) / double(nx);
        double v = double(j + drand48()) / double(ny);
        ray r = SCENE_CAMERA.get_ray(u, v);
        col += path_trace(r, world, lights, RGB_MAX_RAY_DEPTH);
      }
      col /= double(ns);
      col = gamma_correct(col);
      drawPix(data, nx, ny, i, j, col);
    }
  }
}

#endif //FLUORSWITCH_SRC_RENDER_PATH_TRACE_H_

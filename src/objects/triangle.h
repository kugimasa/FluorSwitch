#ifndef FLUORSWITCH_SRC_OBJECTS_TRIANGLE_H_
#define FLUORSWITCH_SRC_OBJECTS_TRIANGLE_H_

#include "../utils/hittable.h"

struct vertex {
 public:
  point3 p;
  vec3 n;
  double u, v;
};

template<typename mat>
class triangle : public hittable<mat> {
 public:
  triangle();
  triangle(vertex v0, vertex v1, vertex v2, shared_ptr<material> m) {
    // 頂点
    vert[0] = v0.p;
    vert[1] = v1.p;
    vert[2] = v2.p;
    // 法線
    norm[0] = v0.n;
    norm[1] = v1.n;
    norm[2] = v2.n;
    // 面法線
    e1 = vert[1] - vert[0];
    e2 = vert[2] - vert[0];
    face_norm = unit_vector(cross(e1, e2));
    // UV
    u[0] = v0.u;
    v[0] = v0.v;
    u[1] = v1.u;
    v[1] = v1.v;
    u[2] = v2.u;
    v[2] = v2.v;
    // マテリアル
    mat_ptr = m;
  };
  triangle(vec3 p0, vec3 p1, vec3 p2, shared_ptr<material> m) {
    // 頂点
    vert[0] = p0;
    vert[1] = p1;
    vert[2] = p2;
    // 面法線
    e1 = vert[1] - vert[0];
    e2 = vert[2] - vert[0];
    face_norm = unit_vector(cross(e1, e2));
    // マテリアル
    mat_ptr = m;
  };

  bool hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const override;
  bool bounding_box(double time0, double time1, aabb &box) const override {
    vec3 a, b;
    for (int i = 0; i < 3; ++i) {
      a[i] = std::min(std::min(vert[0][i], vert[1][i]), vert[2][i]);
      b[i] = std::max(std::max(vert[0][i], vert[1][i]), vert[2][i]);
    }
    box = aabb(a, b);
    return true;
  }
  double pdf_value(const point3 &o, const vec3 &v) const override;
  vec3 random(const vec3 &o) const override;
  vec3 barycentric(vec3 &p);

 public:
  point3 vert[3];
  vec3 norm[3];
  vec3 face_norm, e1, e2;
  double u[3];
  double v[3];
  shared_ptr<mat> mat_ptr;
};

template<typename mat>
bool triangle<mat>::hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const {
  /// Möller–Trumbore intersection algorithm
  vec3 p_vec = cross(r.direction(), e2);
  // 行列式
  double det = dot(e1, p_vec);
  // レイと平面の平行判定
  if (det <= 0.0f) {
    return false;
  }
  double inv_det = 1.0 / det;
  vec3 t_vec = r.origin() - vert[0];
  double u_ = dot(t_vec, p_vec) * inv_det;
  if (u_ < 0.0 || 1.0 < u_) {
    return false;
  }
  vec3 q_vec = cross(t_vec, e1);
  double v_ = dot(r.direction(), q_vec) * inv_det;
  if (v_ < 0.0 || 1.0 < u_ + v_) {
    return false;
  }
  double t = dot(e2, q_vec) * inv_det;
  if (t < t_min || t_max < t) {
    return false;
  }
  rec.t = t;
  rec.p = r.point_at_parameter(rec.t);
  rec.set_face_normal(r, face_norm);
  rec.mat_ptr = mat_ptr;
  return true;
}

template<typename mat>
double triangle<mat>::pdf_value(const point3 &o, const vec3 &v) const {
  hit_record<mat> rec;
  if (!this->hit(ray(o, v), 0.001, INF, rec)) {
    return 0;
  }

  auto area = 0.5 * sqrt(e1.squared_length() * e2.squared_length() - pow(dot(e1, e2), 2));
  auto distance_squared = rec.t * rec.t * v.squared_length();
  auto cosine = fabs(dot(v, rec.normal) / v.length());

  return distance_squared / (cosine * area);
}

template<typename mat>
vec3 triangle<mat>::random(const vec3 &o) const {
  auto k = random_double(0, 1);
  auto random_point = e1 * k + e2 * (1 - k);
  return random_point - o;
}

template<typename mat>
vec3 triangle<mat>::barycentric(vec3 &p) {
  vec3 t = p - vert[0];
  double d00 = dot(e1, e1);
  double d01 = dot(e1, e2);
  double d11 = dot(e2, e2);
  double d20 = dot(t, e1);
  double d21 = dot(t, e2);
  double d = d00 * d11 - d01 * d01;
  double v_ = (d11 * d20 - d01 * d21) / d;
  double w_ = (d00 * d21 - d01 * d20) / d;
  double u_ = 1 - v_ - w_;
  return vec3(u_, v_, w_);
}

#endif //FLUORSWITCH_SRC_OBJECTS_TRIANGLE_H_

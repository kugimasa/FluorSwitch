#ifndef RTCAMP2022_SRC_OBJECTS_GEOMETRY_H_
#define RTCAMP2022_SRC_OBJECTS_GEOMETRY_H_

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
// Optional. define TINYOBJLOADER_USE_MAPBOX_EARCUT gives robust trinagulation. Requires C++11
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "../../external/tinyobjloader/tiny_obj_loader.h"
#include "../utils/util_funcs.h"
#include "../utils/hittable.h"

class geometry : hittable {
 public:
  geometry();
  geometry(const char *file_path);

  bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;
  bool bounding_box(double t0, double t1, aabb &box) const override;
  vec3 random(const vec3 &o) const override;
  double pdf_value(const point3 &o, const vec3 &v) const override;

 public:
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  shared_ptr<material> mat_ptr;
};

geometry::geometry(const char *file_path) {
  tinyobj::ObjReaderConfig reader_config;
  tinyobj::ObjReader reader;
  if (!reader.ParseFromFile(file_path, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    exit(1);
  }

  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader: " << reader.Warning();
  }

  shapes = reader.GetShapes();
  attrib = reader.GetAttrib();
}

bool geometry::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
  for (size_t s = 0; s < shapes.size(); s++) {
    // ポリゴンでループ
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      // ポリゴン内の頂点でループ
      for (size_t v = 0; v < fv; v++) {
        // 頂点取得
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
        tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
        tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

        // 法線判定
        if (idx.normal_index >= 0) {
          tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
          tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
          tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
        }
      }
      index_offset += fv;
    }
  }
  return false;
}
bool geometry::bounding_box(double t0, double t1, aabb &box) const {
  return false;
}
vec3 geometry::random(const vec3 &o) const {
  return hittable::random(o);
}
double geometry::pdf_value(const point3 &o, const vec3 &v) const {
  return hittable::pdf_value(o, v);
}

#endif //RTCAMP2022_SRC_OBJECTS_GEOMETRY_H_

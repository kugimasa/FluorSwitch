#ifndef RTCAMP2022_SRC_OBJECTS_GEOMETRY_H_
#define RTCAMP2022_SRC_OBJECTS_GEOMETRY_H_

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
// Optional. define TINYOBJLOADER_USE_MAPBOX_EARCUT gives robust trinagulation. Requires C++11
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "../../external/tinyobjloader/tiny_obj_loader.h"
#include "../utils/util_funcs.h"
#include "../utils/hittable.h"
#include "triangle.h"

template<typename mat>
class geometry : public hittable<mat> {
 public:
  geometry();
  geometry(const char *file_path);
  geometry(const char *file_path, shared_ptr<mat> m);

  bool hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const override;
  bool bounding_box(double t0, double t1, aabb &box) const override;
  vec3 random(const vec3 &o) const override;
  double pdf_value(const point3 &o, const vec3 &v) const override;
 private:
  void load_obj(const char *file_path, std::vector<vertex> &vertices);

 public:
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::vector<shared_ptr<triangle<mat>>> tris;
};

// TODO blenderマテリアルを取得
template<typename mat>
geometry<mat>::geometry(const char *file_path) {
  std::vector<vertex> vertices;
  load_obj(file_path, vertices);
  for (int i = 0; i < vertices.size() / 3; ++i) {
    shared_ptr<material> m = make_shared<lambertian>(CYAN);
    tris.push_back(make_shared<triangle<mat>>(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2], m));
  }
}

template<typename mat>
geometry<mat>::geometry(const char *file_path, shared_ptr<mat> m) {
  std::vector<vertex> vertices;
  load_obj(file_path, vertices);
  for (int i = 0; i < vertices.size() / 3; ++i) {
    tris.push_back(make_shared<triangle<mat>>(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2], m));
  }
}

template<typename mat>
void geometry<mat>::load_obj(const char *file_path, std::vector<vertex> &vertices) {
  tinyobj::ObjReaderConfig reader_config;
  tinyobj::ObjReader reader;
  reader_config.mtl_search_path = "./assets/obj/";
  if (!reader.ParseFromFile(file_path, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    exit(1);
  }

  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader: " << reader.Warning();
  }

  attrib = reader.GetAttrib();
  shapes = reader.GetShapes();
  materials = reader.GetMaterials();

  // 頂点の登録
  for (size_t s = 0; s < shapes.size(); ++s) {
    // ポリゴンでループ
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      // ポリゴン内の頂点でループ
      for (size_t v = 0; v < fv; ++v) {
        // 頂点取得
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        auto vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
        auto vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
        auto vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
        auto nx = 0;
        auto ny = 0;
        auto nz = 1;
        auto tx = 0;
        auto ty = 0;

        // 法線判定
        if (idx.normal_index >= 0) {
          nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
          ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
          nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
        }

        // テクスチャ座標
        if (idx.texcoord_index >= 0) {
          tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
          ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
        }

        vertex vert;
        vert.p = vec3(vx, vy, vz);
        vert.n = vec3(nx, ny, nz);
        vert.u = tx;
        vert.v = ty;
        vertices.push_back(vert);
      }
      index_offset += fv;
    }
  }
  shapes.clear();
}

template<typename mat>
bool geometry<mat>::hit(const ray &r, double t_min, double t_max, hit_record<mat> &rec) const {
  long triangles_size = tris.size();
  for (long i = 0; i < triangles_size; ++i) {
    if (tris[i]->hit(r, t_min, t_max, rec)) {
      return true;
    }
  }
  return false;
}

template<typename mat>
bool geometry<mat>::bounding_box(double t0, double t1, aabb &box) const {
  return true;
}

template<typename mat>
vec3 geometry<mat>::random(const vec3 &o) const {
  return hittable<mat>::random(o);
}

template<typename mat>
double geometry<mat>::pdf_value(const point3 &o, const vec3 &v) const {
  return hittable<mat>::pdf_value(o, v);
}

#endif //RTCAMP2022_SRC_OBJECTS_GEOMETRY_H_

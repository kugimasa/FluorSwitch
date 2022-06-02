#ifndef RTCAMP2022_SRC_UTILS_TEXTURE_H_
#define RTCAMP2022_SRC_UTILS_TEXTURE_H_

#include "util_funcs.h"
#include "rtw_stb_image.h"
#include "perlin.h"

#include <iostream>

class texture {
 public:
  virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public texture {
 public:
  solid_color() {}
  solid_color(color c) : color_value(c) {}

  solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

  color value(double u, double v, const vec3 &p) const override {
    return color_value;
  }

 private:
  color color_value;
};

class checker_texture : public texture {
 public:
  checker_texture() {}

  checker_texture(shared_ptr<texture> color_a, shared_ptr<texture> color_b)
      : even(color_a), odd(color_b) {}

  checker_texture(color c1, color c2)
      : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

  color value(double u, double v, const point3 &p) const override {
    auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
    if (sines < 0) {
      return odd->value(u, v, p);
    } else {
      return even->value(u, v, p);
    }
  }

 public:
  shared_ptr<texture> even;
  shared_ptr<texture> odd;
};

class noise_texture : public texture {
 public:
  noise_texture() {}
  noise_texture(color c) : texture_color(c) {}
  noise_texture(color c, double sc) : texture_color(c), scale(sc) {}

  color value(double u, double v, const point3 &p) const override {
    // noise : [0,1]
    return texture_color * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
  }

 public:
  perlin noise;
  color texture_color{WHITE};
  double scale{1};
};

class image_texture : public texture {
 public:
  // 1pxあたり3byte
  const static int bytes_per_pixel = 3;

  image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

  image_texture(const char *filename) {
    auto components_per_pixel = bytes_per_pixel;

    data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

    if (!data) {
      std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
      width = height = 0;
    }
    bytes_per_scanline = bytes_per_pixel * width;
  }

  ~image_texture() {
    delete data;
  }

  color value(double u, double v, const vec3 &p) const override {
    if (data == nullptr) {
      return MAGENTA;
    }

    u = clamp(u, 0.0, 1.0);
    v = 1.0 - clamp(v, 0.0, 1.0);

    auto i = static_cast<int>(u * width);
    auto j = static_cast<int>(v * height);

    if (i >= width) {
      i = width - 1;
    }
    if (j >= height) {
      j = height - 1;
    }

    const auto color_scale = 1.0 / 255.0;
    auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

    return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
  }

 private:
  unsigned char *data;
  int width, height;
  int bytes_per_scanline;
};

#endif //RTCAMP2022_SRC_UTILS_TEXTURE_H_

#ifndef RAY_UTILS_OUTPUT_FILE_H_
#define RAY_UTILS_OUTPUT_FILE_H_

#include <iostream>
#include <cstdlib>
#include <cstring>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct {
  unsigned char *data;
  unsigned int width;
  unsigned int height;
  unsigned int ch;
} BITMAPDATA_t;

int freeBitmapData(BITMAPDATA_t *bitmap) {
  if (bitmap->data != nullptr) {
    free(bitmap->data);
    bitmap->data = nullptr;
  }
  return 0;
}

#endif //RAY_UTILS_OUTPUT_FILE_H_
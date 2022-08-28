#ifndef FLUORSWITCH_SRC_UTILS_RTW_STB_IMAGE_H_
#define FLUORSWITCH_SRC_UTILS_RTW_STB_IMAGE_H_

// Disable pedantic warnings for this external library.
#ifdef _MSC_VER
// Microsoft Visual C++ Compiler
#pragma warning (push, 0)
#endif

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

// Restore warning levels.
#ifdef _MSC_VER
// Microsoft Visual C++ Compiler
#pragma warning (pop)
#endif

#endif //FLUORSWITCH_SRC_UTILS_RTW_STB_IMAGE_H_

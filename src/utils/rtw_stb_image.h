#ifndef RTCAMP2022_SRC_UTILS_RTW_STB_IMAGE_H_
#define RTCAMP2022_SRC_UTILS_RTW_STB_IMAGE_H_

// Disable pedantic warnings for this external library.
#ifdef _MSC_VER
// Microsoft Visual C++ Compiler
#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Restore warning levels.
#ifdef _MSC_VER
// Microsoft Visual C++ Compiler
#pragma warning (pop)
#endif

#endif //RTCAMP2022_SRC_UTILS_RTW_STB_IMAGE_H_

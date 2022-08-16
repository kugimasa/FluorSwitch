#ifndef RAY_UTILS_MY_PRINT_H_
#define RAY_UTILS_MY_PRINT_H_

#include <iostream>

void inline flush_progress(double progress) {
  int bar_width = 20;
  std::cout << "\r [";
  int pos = bar_width * progress;
  for (int i = 0; i < bar_width; ++i) {
    if (i < pos) std::cout << "=";
    else if (i == pos) std::cout << ">";
    else std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " %" << std::flush;
}

void inline error_print(const char *error_str) {
  std::cout << "🛑 " << error_str << std::endl;
}

#endif //RAY_UTILS_MY_PRINT_H_

cmake_minimum_required(VERSION 3.15)

# The project information
project(rtcamp2022 VERSION 0.1.0 LANGUAGES C CXX)
message(STATUS "rtcamp2022🏕 version: ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 14)

add_executable(rtcamp2022
               src/main.cpp
               src/utils/hitable.h
               src/utils/hitable_list.h
               src/utils/ray.h
               src/utils/vec3.h
               src/material/material.h
               src/objects/sphere.h
               src/camera/camera.h
               src/utils/output_file.h
               src/utils/output_file.cpp
               src/utils/my_print.h
               src/utils/colors.h
               src/utils/aabb.h
               src/utils/bvh.h
               src/utils/util_funcs.h)

find_package(PNG REQUIRED)
include_directories("../Utils/libpng")
target_link_libraries(rtcamp2022 ${PNG_LIBRARY})
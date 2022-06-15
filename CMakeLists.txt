cmake_minimum_required(VERSION 3.15)

# The project information
project(rtcamp2022 VERSION 0.1.0 LANGUAGES C CXX)
message(STATUS "rtcamp2022🏕 version: ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 14)

add_executable(rtcamp2022
               src/main.cpp
               src/utils/hittable.h
               src/utils/hittable_list.h
               src/utils/ray.h
               src/utils/vec3.h
               src/utils/output_file.h
               src/utils/output_file.cpp
               src/utils/my_print.h
               src/utils/colors.h
               src/utils/aabb.h
               src/utils/bvh.h
               src/utils/util_funcs.h
               src/utils/texture.h
               src/utils/perlin.h
               src/utils/rtw_stb_image.h
               src/camera/camera.h
               src/objects/aarect.h
               src/objects/box.h
               src/objects/cornell_box.h
               src/objects/sphere.h
               src/material/material.h
               src/material/light.h)

find_package(PNG REQUIRED)
include_directories("../Utils/libpng")
include_directories("../Utils/stb")
target_link_libraries(rtcamp2022 ${PNG_LIBRARY})
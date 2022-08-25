cmake_minimum_required(VERSION 3.15)

# The project information
project(rtcamp2022 VERSION 0.1.0 LANGUAGES C CXX)
message(STATUS "rtcamp2022🏕 version: ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 17)

add_executable(rtcamp2022
               src/main.cpp
               src/camera/camera.h
               src/material/fluorescent_material.h
               src/material/material.h
               src/material/light.h
               src/material/spectral_material.h
               src/material/spectral_light.h
               src/objects/aarect.h
               src/objects/box.h
               src/objects/constant_medium.h
               src/objects/cornell_box.h
               src/objects/geometry.h
               src/objects/sphere.h
               src/objects/triangle.h
               src/render/path_trace.h
               src/render/spectral_path_trace.h
               src/sampling/pdf.h
               src/sampling/spectral_pdf.h
               src/scene/scene.h
               src/utils/hittable.h
               src/utils/hittable_list.h
               src/utils/ray.h
               src/utils/vec3.h
               src/utils/output_file.h
               src/utils/my_print.h
               src/utils/colors.h
               src/utils/aabb.h
               src/utils/bvh.h
               src/utils/onb.h
               src/utils/perlin.h
               src/utils/rtw_stb_image.h
               src/utils/spectral_distribution.h
               src/utils/texture.h
               src/utils/util_funcs.h)

include_directories("external/stb")
include_directories("external/tinyobjloader")
include_directories("external/fast-cpp-csv-parser")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(rtcamp2022 PUBLIC "-pthread")

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
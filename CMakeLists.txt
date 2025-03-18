cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(astro VERSION 0.1)

include("cmake/rapidjson.cmake")
include("cmake/cpr.cmake")
include("cmake/sherpa-onnx.cmake")

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -g -pthread")
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${portaudio_SOURCE_DIR}/include"
    "${sherpa_onnx_SOURCE_DIR}"
)

link_directories(
    "${onnxruntime_SOURCE_DIR}/lib"
    ${portaudio_BINARY_DIR}
)

add_library(astro STATIC
    src/audio_stream.cpp
    src/sensevoice.cpp
    src/paraformer.cpp
    src/kwspotter.cpp
    src/vad.cpp
)

add_executable(astro-bin src/main.cpp)
target_link_libraries(astro-bin PUBLIC astro PRIVATE onnxruntime sherpa-onnx-c-api portaudio cpr::cpr)

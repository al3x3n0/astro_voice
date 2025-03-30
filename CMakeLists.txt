cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(astro VERSION 0.1)

include("cmake/nlohmann.cmake")
include("cmake/cpr.cmake")
include("cmake/sherpa-onnx.cmake")
include("cmake/ws.cmake")

# Add ASIO library
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
)

FetchContent_MakeAvailable(asio)

# Add include directories for ASIO
include_directories(${asio_SOURCE_DIR}/asio/include)


# Add OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
#target_link_libraries(astro-bin PRIVATE )


set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -g -pthread -DASIO_STANDALONE")
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${json_SOURCE_DIR}/include"
    "${cpr_SOURCE_DIR}/include"
    "${cpr_BINARY_DIR}/cpr_generated_includes"
    "${portaudio_SOURCE_DIR}/include"
    "${sherpa_onnx_SOURCE_DIR}"
)

link_directories(
    "${onnxruntime_SOURCE_DIR}/lib"
    ${portaudio_BINARY_DIR}
)

add_library(astro STATIC
    src/audio_stream.cpp
    src/base64.cpp
    src/sensevoice.cpp
    src/paraformer.cpp
    src/kwspotter.cpp
    src/vad.cpp
    src/astro_client.cpp
    src/player.cpp
    src/pipe.cpp
    src/ws_client.cpp
    src/ws_client_factory.cpp
    src/tts.cpp
    src/zipformer_stt.cpp
)

add_executable(astro-bin src/main.cpp)
target_link_libraries(astro-bin PUBLIC astro PRIVATE onnxruntime sherpa-onnx-c-api portaudio cpr::cpr OpenSSL::SSL OpenSSL::Crypto)

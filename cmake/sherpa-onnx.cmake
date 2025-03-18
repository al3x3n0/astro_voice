include(FetchContent)

FetchContent_Declare(sherpa_onnx GIT_REPOSITORY "https://github.com/k2-fsa/sherpa-onnx.git" GIT_TAG v1.11.1)
FetchContent_MakeAvailable(sherpa_onnx)
FetchContent_MakeAvailable(onnxruntime)
FetchContent_MakeAvailable(portaudio)
target_include_directories(sherpa-onnx-core PUBLIC ${sherpa_onnx_SOURCE_DIR})

include(FetchContent)

FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
    GIT_TAG 0.8.2
)

FetchContent_MakeAvailable(websocketpp)

# Add include directories for websocketpp
include_directories(${websocketpp_SOURCE_DIR})

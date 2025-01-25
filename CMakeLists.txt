cmake_minimum_required(VERSION 3.30)
project(crawlengine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "5.5"
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    raylib-cpp
    GIT_REPOSITORY "https://github.com/RobLoach/raylib-cpp"
    GIT_TAG "v5.5.0"
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib-cpp)

FetchContent_Declare(
    entt
    GIT_REPOSITORY "https://github.com/skypjack/entt"
    GIT_TAG "v3.14.0"
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(entt)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets/")

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)

target_include_directories(${PROJECT_NAME} PRIVATE ${entt_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${raylib-cpp_SOURCE_DIR}/include)

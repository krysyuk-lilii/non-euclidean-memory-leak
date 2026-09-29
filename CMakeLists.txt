cmake_minimum_required(VERSION 3.20)
project(portals CXX)

set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")

find_package(SDL3 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(glm REQUIRED)

set(ENV{PKG_CONFIG_PATH} "C:/msys64/ucrt64/lib/pkgconfig")
pkg_check_modules(ANGLE REQUIRED angleproject)

set(SOURCES
    src/main.cpp
    src/render/camera.cpp
    src/render/frame_buffer.cpp
    src/render/render_pass.cpp
    src/render/portal_renderer.cpp
    src/gl/gl_check.cpp
    src/gl/mesh.cpp
    src/gl/shader.cpp
    src/gl/obj_loader.cpp
    src/core/time.cpp
    src/core/engine.cpp
    src/core/scene.cpp
    src/core/game_object.cpp
    src/core/portal.cpp
)

add_executable(portals WIN32 ${SOURCES})
target_include_directories(portals PUBLIC src)
target_include_directories(portals PRIVATE ${ANGLE_INCLUDE_DIRS})
target_compile_definitions(portals PUBLIC
    PORTAL_GLES_MAJOR=3
    PORTAL_GLES_MINOR=1
)
target_link_libraries(portals PRIVATE SDL3::SDL3 ${ANGLE_LIBRARIES} glm::glm)

cmake_minimum_required(VERSION 3.10)
project(kege_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

file(GLOB_RECURSE MATH_SOURCES CONFIGURE_DEPENDS kege/src/core/math/*.cpp)
add_library(vector_math   ${MATH_SOURCES})

file(GLOB_RECURSE CORE_ENGINE_SOURCES CONFIGURE_DEPENDS kege/src/core/engine/*.cpp)
add_library(engine   ${CORE_ENGINE_SOURCES})

file(GLOB_RECURSE GRAPHICS_SOURCES CONFIGURE_DEPENDS kege/src/core/graphics/*.cpp)
add_library(graphics   ${GRAPHICS_SOURCES})

# file(GLOB_RECURSE GAME_SOURCES   CONFIGURE_DEPENDS kege/src/game/*.cpp)
# add_library(engine ${ENGINE_SOURCES})


add_executable(kege-engine kege/src/main.cpp)

# --- Manual GLFW Configuration ---

# 1. Tell the project where to find the GLFW header files.
#    Replace this path with your actual 'include' directory path.
target_include_directories(kege-engine PRIVATE "/usr/local/glfw/3.3.8/lib-x86_64/include")

# 2. Link the project against the Vulkan libraries and the specific GLFW .dylib file.
#    Replace this path with the full path to your .dylib file.
target_link_libraries(kege-engine PRIVATE 
    ${Vulkan_LIBRARIES}
    "/usr/local/glfw/3.3.8/lib-x86_64/libglfw.3.dylib"
)

# On macOS, you also need to link against the CoreFoundation framework for windowing.
target_link_libraries(kege-engine PRIVATE "-framework CoreFoundation")


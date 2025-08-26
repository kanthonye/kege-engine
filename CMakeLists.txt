cmake_minimum_required(VERSION 3.10)
project(kege_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

# Find shaderc
find_library(SHADERC_LIBRARY 
    NAMES shaderc_combined shaderc_shared libshaderc_combined libshaderc_shared
    HINTS /usr/local/lib
)
if (NOT SHADERC_LIBRARY)
    message(FATAL_ERROR "shaderc library not found in /usr/local/lib. Ensure vulkan-sdk is installed via Homebrew (brew install vulkan-sdk).")
endif()

find_path(SHADERC_INCLUDE_DIR 
    NAMES shaderc/shaderc.h shaderc/shaderc.hpp
    HINTS /usr/local/include
)
if (NOT SHADERC_INCLUDE_DIR)
    message(FATAL_ERROR "shaderc headers not found in /usr/local/include. Ensure vulkan-sdk is installed via Homebrew (brew install vulkan-sdk).")
endif()



# --- Kege Engine Libraries ---
file(GLOB_RECURSE UTILS_SOURCES CONFIGURE_DEPENDS kege/src/core/utils/*.cpp)
add_library(utils   ${UTILS_SOURCES})

file(GLOB_RECURSE IO_SOURCES CONFIGURE_DEPENDS kege/src/core/io/*.cpp)
add_library(io   ${IO_SOURCES})

file(GLOB_RECURSE MATH_SOURCES CONFIGURE_DEPENDS kege/src/core/math/*.cpp)
add_library(vector_math   ${MATH_SOURCES})

file(GLOB_RECURSE CORE_ENGINE_SOURCES CONFIGURE_DEPENDS kege/src/core/engine/*.cpp)
add_library(engine   ${CORE_ENGINE_SOURCES})

file(GLOB_RECURSE GRAPHICS_SOURCES CONFIGURE_DEPENDS kege/src/core/graphics/*.cpp)
add_library(graphics   ${GRAPHICS_SOURCES})

file(GLOB_RECURSE INPUT_SOURCES CONFIGURE_DEPENDS kege/src/core/input/*.cpp)
add_library(input   ${INPUT_SOURCES})

file(GLOB_RECURSE ECS_SOURCES CONFIGURE_DEPENDS kege/src/core/ecs/*.cpp)
add_library(ecs   ${ECS_SOURCES})

file(GLOB_RECURSE SYSTEM_SOURCES CONFIGURE_DEPENDS kege/src/core/system/*.cpp)
add_library(system   ${SYSTEM_SOURCES})

file(GLOB_RECURSE ESM_SOURCES CONFIGURE_DEPENDS kege/src/core/esm/*.cpp)
add_library(esm   ${ESM_SOURCES})

file(GLOB_RECURSE SCENE_SOURCES CONFIGURE_DEPENDS kege/src/core/scene/*.cpp)
add_library(scene   ${SCENE_SOURCES})

file(GLOB_RECURSE GUI_SOURCES CONFIGURE_DEPENDS kege/src/core/gui/*.cpp)
add_library(gui   ${GUI_SOURCES})

file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS kege/src/editor/*.cpp)
add_library(editor   ${EDITOR_SOURCES})




file(GLOB_RECURSE CAMERA_SOURCES CONFIGURE_DEPENDS kege/src/systems/camera/*.cpp)
add_library(camera   ${CAMERA_SOURCES})

file(GLOB_RECURSE PHYSICS_SOURCES CONFIGURE_DEPENDS kege/src/systems/physics/*.cpp)
add_library(physics   ${PHYSICS_SOURCES})

file(GLOB_RECURSE PARTICLE_SOURCES CONFIGURE_DEPENDS kege/src/systems/particle/*.cpp)
add_library(particle   ${PARTICLE_SOURCES})

file(GLOB_RECURSE PICKING_SOURCES CONFIGURE_DEPENDS kege/src/systems/picking/*.cpp)
add_library(picking_systems   ${PICKING_SOURCES})


add_executable(kege-engine kege/src/main.cpp)


# 1. Tell the project where to find the GLFW header files.
#    Replace this path with your actual 'include' directory path.
target_include_directories(kege-engine 
    PRIVATE 
        "/usr/local/glfw/3.3.8/lib-x86_64/include"
        ${CMAKE_SOURCE_DIR}/kege/src/editor
        ${CMAKE_SOURCE_DIR}/kege/src/core/utils
        ${CMAKE_SOURCE_DIR}/kege/src/core/io
        ${CMAKE_SOURCE_DIR}/kege/src/core/gui
        ${CMAKE_SOURCE_DIR}/kege/src/core/ecs
        ${CMAKE_SOURCE_DIR}/kege/src/core/esm
        ${CMAKE_SOURCE_DIR}/kege/src/core/system
        ${CMAKE_SOURCE_DIR}/kege/src/core/input
        ${CMAKE_SOURCE_DIR}/kege/src/core/scene
        ${CMAKE_SOURCE_DIR}/kege/src/core/graphics
        ${CMAKE_SOURCE_DIR}/kege/src/core/engine
        ${CMAKE_SOURCE_DIR}/kege/src/core/math
        ${CMAKE_SOURCE_DIR}/kege/src/core/esm
        ${CMAKE_SOURCE_DIR}/kege/src/systems/camera
        ${CMAKE_SOURCE_DIR}/kege/src/systems/physics
        ${CMAKE_SOURCE_DIR}/kege/src/systems/particle
        ${CMAKE_SOURCE_DIR}/kege/src/systems/picking

        ${SHADERC_INCLUDE_DIR}
)

# 2. Link the project against the Vulkan libraries and the specific GLFW .dylib file.
#    Replace this path with the full path to your .dylib file.
target_link_libraries(kege-engine PRIVATE 
    utils
    io
    graphics
    input
    engine
    vector_math
    system
    ecs
    esm
    scene
    gui
    editor

    particle
    camera
    physics
    picking_systems

    ${Vulkan_LIBRARIES}
    ${SHADERC_LIBRARY}
    "/usr/local/glfw/3.3.8/lib-x86_64/libglfw.3.dylib"
)

# On macOS, you also need to link against the CoreFoundation framework for windowing.
target_link_libraries(kege-engine PRIVATE "-framework CoreFoundation")


# cmake version
cmake_minimum_required(VERSION 3.8)

project(water-app LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

#precompiled headers
include("deps/cotire/CMake/cotire.cmake")

#
find_package(OpenGL REQUIRED)

#
include_directories(
        deps/glfw-3.2.1/include/
        deps/glm/
        deps/glew-2.1.0/include/
        deps/termcolor/
        deps/
        .
)

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
        -DGL_GLEXT_PROTOTYPES
)

# profiling with gprof
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#SET(CMAKE_CC_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

# -------------------------------------------------------------------------------------------------
# Compile external dependencies (GLEW, GLFW, GLM)
add_subdirectory (deps)

# -------------------------------------------------------------------------------------------------
#Generate the static library from the sources
add_library(water STATIC
    "src/water/render/Program.cpp"
    "src/water/render/ScreenRectangle.cpp"
    "src/water/render/GLWrappers.cpp"
    "src/water/render/Window.cpp"
    "src/water/render/ScreenToNDC.cpp"
    "src/water/render/FrameBuffer.cpp"
    "src/water/render/Texture.cpp"
    "src/water/render/CubeMap.cpp"
    "src/water/math/FourierTransform.cpp"
    "src/water/math/FourierTransform_GPU.cpp"
    "src/water/anim/PhillipsSpectrum.cpp"
    "src/water/anim/Animator.cpp"
    "src/water/anim/AnimatorGerstner.cpp"
    "src/water/anim/AnimatorDFT.cpp"
    "src/water/anim/AnimatorDFT_GPU.cpp"
    "src/water/anim/Utils.cpp"
    "src/water/scene/Patch.cpp"
    "src/water/scene/Ocean.cpp"
    "src/water/scene/Sky.cpp"
    "src/water/scene/Scene.cpp"
    "src/water/scene/Settings.cpp"
    "src/water/utils/SettingsReader.cpp"
    "src/water/utils/Log.cpp"
    )

target_link_libraries(water
        ${OPENGL_LIBRARY}
        glfw
        GLEW_210
        freeimage
        yaml-cpp
    )

set_target_properties(water PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/headers.h")
cotire(water)

# -------------------------------------------------------------------------------------------------
# хочу сделать исполняемый файл water из перечисленных исходников
add_executable(water-app
    src/main.cpp
    )

target_link_libraries(water-app water)
set_target_properties(water-app PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/headers.h")
cotire(water-app)

# -------------------------------------------------------------------------------------------------

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(water-tests
                "test/water-tests/main.cpp"
                "test/water-tests/tst_full.h"
                "test/water-tests/TestPhillips.h"
                "test/water-tests/TestScreenToNDC.h"
                "test/water-tests/TestFFTGPU.h")
add_test(water-tests COMMAND water-tests)

target_link_libraries(
    water-tests PRIVATE
    Threads::Threads
    water
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    )

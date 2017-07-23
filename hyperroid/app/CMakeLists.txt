# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add.library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

add_library( # Specifies the name of the library.
             hyper

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/jni/hyper.cpp )

# GLESv2
find_path(GLES2_INCLUDE_DIR GLES2/gl2.h
    HINTS ${ANDROID_NDK})
find_library(GLES2_LIBRARY libGLESv1_CM.so
    HINTS ${GLES2_INCLUDE_DIR}/../lib)
target_include_directories(hyper PUBLIC ${GLES2_INCLUDE_DIR})


find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       hyper

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} 
                       # ${GLES2_LIBRARY}
                       GLESv1_CM
                       )

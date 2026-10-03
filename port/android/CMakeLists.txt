cmake_minimum_required(VERSION 3.22.1)

project(khcom_android_platform C)

add_library(khcom_android_platform STATIC
    game/gba_memory.c
)

target_include_directories(khcom_android_platform
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(khcom_android_platform PRIVATE c_std_11)

# Pointer truncation is especially dangerous on the RP5's arm64-v8a target.
target_compile_options(khcom_android_platform PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror=pointer-to-int-cast
    -Werror=int-to-pointer-cast
)

set_target_properties(khcom_android_platform PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

cmake_minimum_required(VERSION 3.22.1)

project(khcom_android_platform C)

if(ANDROID AND NOT ANDROID_ABI STREQUAL "armeabi-v7a")
    message(FATAL_ERROR
        "KHCoM's native game core intentionally targets armeabi-v7a. "
        "AArch64 changes the game's pointer-sized structure layouts and cannot "
        "directly execute the original ARM32 FMV decoder.")
endif()

add_library(khcom_android_platform STATIC
    game/gba_memory.c
)

target_include_directories(khcom_android_platform
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_compile_definitions(khcom_android_platform PUBLIC
    PLATFORM_ANDROID=1
)

target_compile_features(khcom_android_platform PRIVATE c_std_11)

target_compile_options(khcom_android_platform PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -fno-strict-aliasing
    -fwrapv
)

set_target_properties(khcom_android_platform PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

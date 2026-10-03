#ifndef KHCOM_ANDROID_GAME_ABI_H
#define KHCOM_ANDROID_GAME_ABI_H

/*
 * NDK Clang follows AAPCS record layout, where a record containing only byte
 * or halfword members may have 1- or 2-byte alignment/size boundaries.
 *
 * The original GBA toolchain, and the Vita native port via
 * -mstructure-size-boundary=32, use a minimum 4-byte boundary for records.
 * That matters for arrays/nesting even though pointers are already 32-bit.
 *
 * Include the C runtime headers BEFORE redefining the record keywords so their
 * ABI is never changed. This file is force-included only for decompiled game
 * code and port/android/game/*.c, never for Android/JNI host code.
 */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#if defined(PLATFORM_ANDROID) && defined(__clang__)
#define struct struct __attribute__((aligned(4)))
#define union union __attribute__((aligned(4)))
#endif

#endif

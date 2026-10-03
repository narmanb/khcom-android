#ifndef GUARD_TYPES_H
#define GUARD_TYPES_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
#ifdef PLATFORM_ANDROID
/* Preserve agbcc's four-byte alignment for 64-bit integer fields. */
typedef unsigned long long u64 __attribute__((aligned(4)));
typedef signed long long s64 __attribute__((aligned(4)));
#else
typedef unsigned long long u64;
typedef signed long long s64;
#endif

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;

#endif /* GUARD_TYPES_H */

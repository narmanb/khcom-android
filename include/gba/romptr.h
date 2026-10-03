#ifndef GUARD_GBA_ROMPTR_H
#define GUARD_GBA_ROMPTR_H

#ifdef PLATFORM_ANDROID
#include "android_gba_memory.h"
#define GBA_PTR(p) ((__typeof__(p))AndroidGbaPointerToHost((const void*)(p)))
#else
#define GBA_PTR(p) (p)
#endif

#endif

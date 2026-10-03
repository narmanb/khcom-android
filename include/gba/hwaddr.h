#ifndef GUARD_GBA_HWADDR_H
#define GUARD_GBA_HWADDR_H

#ifdef PLATFORM_ANDROID
#include <stdint.h>
extern unsigned char gGbaPltt[0x400];
extern unsigned char gGbaVram[0x18000];
extern unsigned char gGbaOam[0x400];
extern unsigned char gGbaSram[0x10000];
#define HW_PLTT(off) ((uintptr_t)gGbaPltt + (uintptr_t)(off))
#define HW_VRAM(off) ((uintptr_t)gGbaVram + (uintptr_t)(off))
#define HW_OAM(off) ((uintptr_t)gGbaOam + (uintptr_t)(off))
#define HW_SRAM(off) ((uintptr_t)gGbaSram + (uintptr_t)(off))
#else
#define HW_PLTT(off) (0x05000000 + (off))
#define HW_VRAM(off) (0x06000000 + (off))
#define HW_OAM(off) (0x07000000 + (off))
#define HW_SRAM(off) (0x0E000000 + (off))
#endif

#endif

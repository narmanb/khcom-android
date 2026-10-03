#ifndef GUARD_GBA_DEFINES_H
#define GUARD_GBA_DEFINES_H

#include "gba/hwaddr.h"

#define INTR_VECTOR (*(void**)0x03007FFC)

#define EWRAM_START 0x02000000
#define EWRAM_SIZE 0x40000

#define IWRAM_START 0x03000000
#define IWRAM_SIZE 0x8000

#define PLTT HW_PLTT(0)
#define PLTT_SIZE 0x400
#define BG_PLTT PLTT
#define BG_PLTT_SIZE 0x200
#define OBJ_PLTT (PLTT + 0x200)

#define VRAM HW_VRAM(0)
#define VRAM_SIZE 0x18000
#define BG_VRAM VRAM
#define BG_CHAR_SIZE 0x4000
#define BG_CHAR_ADDR(n) (BG_VRAM + BG_CHAR_SIZE * (n))
#define OBJ_VRAM0 (VRAM + 0x10000)

#define OAM HW_OAM(0)

#define TILE_SIZE_4BPP 32
#define PLTT_SIZE_4BPP 32

#endif

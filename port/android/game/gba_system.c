/*
 * GBA hardware/BIOS compatibility used by the native Android build.
 *
 * This follows the proven Vita port architecture, but DMA endpoints use
 * uintptr_t so the platform boundary never depends on lossy pointer casts.
 */
#include "types.h"
#include "intr.h"
#include "malloc.h"
#include "gba/io_reg.h"
#include "gba/syscall.h"
#include "android_gba_memory.h"
#include "port.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

extern IntrFunc gIntrTable[14];

struct SoundInfo;
struct SoundInfo* gSoundInfoPtr;
vu16 gIntrCheck;

/* Linker-provided RAM areas on the GBA become native storage on Android. */
u8 gEwramHeapStart[EWRAM_HEAP_SIZE] __attribute__((aligned(16)));
u8 gIwramHeapStart[IWRAM_HEAP_SIZE] __attribute__((aligned(16)));

/* crt0's IRQ dispatcher is never executed natively; main.c only references it
 * in the original GBA path. Keep the symbol for compatibility. */
u8 IrqHandler[0x800];

#define IO16(off) (*(vu16*)&gGbaIo[(off)])
#define IO32(off) (*(vu32*)&gGbaIo[(off)])

static int IsBiosPointer(const void* pointer) {
    uintptr_t raw = (uintptr_t)pointer;
    return raw <= UINT32_MAX && raw < 0x02000000u;
}

static void* TranslatePointer(const void* pointer) {
    void* mapped;

    if (!AndroidGbaPointerIsBusAddress(pointer)) {
        return (void*)pointer;
    }

    mapped = AndroidGbaPointerToHost(pointer);
    if (mapped == NULL) {
        PortFatal("unmapped GBA bus pointer %08X", (unsigned)(uintptr_t)pointer);
    }
    return mapped;
}

/* BIOS calls ---------------------------------------------------------------- */

static const u32 sZeroWord;

void CpuSet(const void* csrc, void* dst, u32 ctrl) {
    void* src = (void*)csrc;
    u32 count = ctrl & 0x1FFFFFu;
    int fixed = (ctrl & CPU_SET_SRC_FIXED) != 0;

    if (IsBiosPointer(dst)) {
        return;
    }
    if (IsBiosPointer(src)) {
        src = (void*)&sZeroWord;
        fixed = 1;
    }

    src = TranslatePointer(src);
    dst = TranslatePointer(dst);

    if (ctrl & CPU_SET_32BIT) {
        u32* s = (u32*)((uintptr_t)src & ~(uintptr_t)3u);
        u32* d = (u32*)((uintptr_t)dst & ~(uintptr_t)3u);

        if (fixed) {
            u32 value = *s;
            while (count-- != 0) {
                *d++ = value;
            }
        } else {
            memmove(d, s, (size_t)count * 4u);
        }
    } else {
        u16* s = (u16*)((uintptr_t)src & ~(uintptr_t)1u);
        u16* d = (u16*)((uintptr_t)dst & ~(uintptr_t)1u);

        if (fixed) {
            u16 value = *s;
            while (count-- != 0) {
                *d++ = value;
            }
        } else {
            memmove(d, s, (size_t)count * 2u);
        }
    }
}

void CpuFastSet(void* src, void* dst, s32 ctrl) {
    u32 count = (u32)ctrl & 0x1FFFFFu;
    u32* s;
    u32* d;

    if (IsBiosPointer(dst)) {
        return;
    }
    if (IsBiosPointer(src)) {
        src = (void*)&sZeroWord;
        ctrl |= CPU_SET_SRC_FIXED;
    }

    s = (u32*)((uintptr_t)TranslatePointer(src) & ~(uintptr_t)3u);
    d = (u32*)((uintptr_t)TranslatePointer(dst) & ~(uintptr_t)3u);
    count = (count + 7u) & ~7u;

    if (ctrl & CPU_SET_SRC_FIXED) {
        u32 value = *s;
        while (count-- != 0) {
            *d++ = value;
        }
    } else {
        memmove(d, s, (size_t)count * 4u);
    }
}

static void Lz77Uncomp(const void* srcp, void* dstp) {
    const u8* src = (const u8*)TranslatePointer(srcp);
    u8* dst = (u8*)TranslatePointer(dstp);
    u32 size = src[1] | ((u32)src[2] << 8) | ((u32)src[3] << 16);
    u32 written = 0;

    src += 4;
    while (written < size) {
        u8 flags = *src++;
        int i;

        for (i = 0; i < 8 && written < size; i++, flags <<= 1) {
            if (flags & 0x80) {
                u32 len = (src[0] >> 4) + 3u;
                u32 disp = (((u32)src[0] & 0xFu) << 8 | src[1]) + 1u;
                src += 2;
                while (len-- != 0 && written < size) {
                    dst[written] = dst[written - disp];
                    written++;
                }
            } else {
                dst[written++] = *src++;
            }
        }
    }
}

void LZ77UnCompWram(const void* src, void* dst) {
    Lz77Uncomp(src, dst);
}

void LZ77UnCompVram(const void* src, void* dst) {
    Lz77Uncomp(src, dst);
}

void BgAffineSet(BgAffineSrcData* src, BgAffineDstData* dst, s32 count) {
    while (count-- > 0) {
        float ox = src->texX / 256.0f;
        float oy = src->texY / 256.0f;
        float cx = src->scrX;
        float cy = src->scrY;
        float sx = src->sx / 256.0f;
        float sy = src->sy / 256.0f;
        float theta = (src->alpha >> 8) / 128.0f * (float)M_PI;
        float a = cosf(theta);
        float c = sinf(theta);
        float b = -c;
        float d = a;

        a *= sx;
        b *= sx;
        c *= sy;
        d *= sy;

        dst->pa = (s16)(a * 256);
        dst->pb = (s16)(b * 256);
        dst->pc = (s16)(c * 256);
        dst->pd = (s16)(d * 256);
        dst->dx = (s32)((ox - (a * cx + b * cy)) * 256);
        dst->dy = (s32)((oy - (c * cx + d * cy)) * 256);
        src++;
        dst++;
    }
}

u32 Sqrt(u32 value) {
    u32 root = 0;
    u32 bit = 1u << 30;

    while (bit > value) {
        bit >>= 2;
    }
    while (bit != 0) {
        if (value >= root + bit) {
            value -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }
    return root;
}

void RegisterRamReset(u32 flags) {
    if (flags & RESET_PALETTE) {
        memset(gGbaPltt, 0, sizeof(gGbaPltt));
    }
    if (flags & RESET_VRAM) {
        memset(gGbaVram, 0, sizeof(gGbaVram));
    }
    if (flags & RESET_OAM) {
        memset(gGbaOam, 0, sizeof(gGbaOam));
    }
    if (flags & RESET_REGS) {
        u16 keys = IO16(REG_OFFSET_KEYINPUT);
        memset(gGbaIo, 0, sizeof(gGbaIo));
        IO16(REG_OFFSET_KEYINPUT) = keys;
        IO16(REG_OFFSET_DISPCNT) = DISPCNT_FORCED_BLANK;
        IO16(REG_OFFSET_BG2PA) = 0x100;
        IO16(REG_OFFSET_BG2PD) = 0x100;
        IO16(REG_OFFSET_BG3PA) = 0x100;
        IO16(REG_OFFSET_BG3PD) = 0x100;
    }
}

void SoftReset(s32 flags) {
    (void)flags;
    PortSoftReset();
}

/* DMA ----------------------------------------------------------------------- */

typedef struct AndroidDma {
    uintptr_t src;
    uintptr_t dst;
    uintptr_t initialDst;
    u32 count;
    u16 ctrl;
    u8 active;
} AndroidDma;

static AndroidDma sDma[4];

#define DMA_REG_BASE(ch) (REG_OFFSET_DMA0 + (ch) * 12)

static void DmaTransfer(int channel) {
    AndroidDma* dma = &sDma[channel];
    int size = (dma->ctrl & DMA_32BIT) ? 4 : 2;
    int dstMode = (dma->ctrl >> 5) & 3;
    int srcMode = (dma->ctrl >> 7) & 3;
    intptr_t dstStep = dstMode == 1 ? -size : dstMode == 2 ? 0 : size;
    intptr_t srcStep = srcMode == 1 ? -size : srcMode == 2 ? 0 : size;
    u32 count = dma->count;
    u8* src;
    u8* dst;

    if (IsBiosPointer((void*)dma->dst)) {
        return;
    }

    if (IsBiosPointer((void*)dma->src)) {
        srcStep = 0;
        src = (u8*)&sZeroWord;
    } else {
        src = (u8*)TranslatePointer((void*)dma->src);
    }
    dst = (u8*)TranslatePointer((void*)dma->dst);

    if (size == 4) {
        src = (u8*)((uintptr_t)src & ~(uintptr_t)3u);
        dst = (u8*)((uintptr_t)dst & ~(uintptr_t)3u);
    } else {
        src = (u8*)((uintptr_t)src & ~(uintptr_t)1u);
        dst = (u8*)((uintptr_t)dst & ~(uintptr_t)1u);
    }

    if (srcStep == size && dstStep == size) {
        memmove(dst, src, (size_t)count * (size_t)size);
    } else {
        while (count-- != 0) {
            if (size == 4) {
                *(u32*)dst = *(u32*)src;
            } else {
                *(u16*)dst = *(u16*)src;
            }
            src += srcStep;
            dst += dstStep;
        }
    }

    dma->src = (uintptr_t)((intptr_t)dma->src + srcStep * (intptr_t)dma->count);
    if (dstMode != 3) {
        dma->dst = (uintptr_t)((intptr_t)dma->dst + dstStep * (intptr_t)dma->count);
    }
}

void AndroidDmaSet(int channel, const void* src, void* dst, u32 control) {
    AndroidDma* dma;
    u32 base;
    u32 count;
    u16 ctrl;

    if (channel < 0 || channel > 3) {
        PortFatal("invalid DMA channel %d", channel);
    }

    dma = &sDma[channel];
    base = DMA_REG_BASE(channel);
    count = control & 0xFFFFu;
    ctrl = (u16)(control >> 16);
    if (count == 0) {
        count = channel == 3 ? 0x10000u : 0x4000u;
    }

    dma->src = (uintptr_t)src;
    dma->dst = (uintptr_t)dst;
    dma->initialDst = (uintptr_t)dst;
    dma->count = count;
    dma->ctrl = ctrl;
    dma->active = 1;

    /* The register shadow is useful to game code that inspects DMA state.
     * This build is deliberately 32-bit, so native pointers fit these words. */
    IO32(base) = (u32)(uintptr_t)src;
    IO32(base + 4) = (u32)(uintptr_t)dst;
    IO16(base + 8) = (u16)(control & 0xFFFFu);
    IO16(base + 10) = ctrl;

    switch ((ctrl >> 12) & 3) {
    case 0:
        DmaTransfer(channel);
        dma->active = 0;
        IO16(base + 10) &= (u16)~DMA_ENABLE;
        break;
    case 3:
        /* FIFO/video-capture DMA is handled by the native audio/video paths. */
        dma->active = 0;
        break;
    default:
        break;
    }
}

void AndroidDmaStop(int channel) {
    u32 base;

    if (channel < 0 || channel > 3) {
        return;
    }
    base = DMA_REG_BASE(channel);
    sDma[channel].active = 0;
    IO16(base + 10) &= (u16)~(DMA_START_MASK | DMA_DREQ_ON | DMA_REPEAT | DMA_ENABLE);
}

static void DmaRunTiming(int timing) {
    int channel;

    for (channel = 0; channel < 4; channel++) {
        AndroidDma* dma = &sDma[channel];
        u32 base = DMA_REG_BASE(channel);

        if (!dma->active || ((dma->ctrl >> 12) & 3) != timing) {
            continue;
        }

        DmaTransfer(channel);

        if (dma->ctrl & DMA_REPEAT) {
            if (((dma->ctrl >> 5) & 3) == 3) {
                dma->dst = dma->initialDst;
            }
        } else {
            dma->active = 0;
            IO16(base + 10) &= (u16)~DMA_ENABLE;
        }
    }
}

static void GbaDmaHBlank(void) {
    DmaRunTiming(2);
}

static void GbaDmaVBlank(void) {
    DmaRunTiming(1);
}

/* Interrupts and frame timing ------------------------------------------------ */

static const u8 sIrqTableIndex[14] = {
    1, 2, 3, 4, 5, 6, 7, 0, 8, 9, 10, 11, 12, 13
};

volatile uint32_t gPortVBlankIrqs;
volatile uint32_t gPortModeUpdates;

static void GbaRaiseIrq(int bit) {
    u16 mask = (u16)(1u << bit);
    IntrFunc func;

    if (!(IO16(REG_OFFSET_IE) & mask) || !(IO16(REG_OFFSET_IME) & 1)) {
        return;
    }

    IO16(REG_OFFSET_IF) |= mask;
    func = gIntrTable[sIrqTableIndex[bit]];
    if (bit == 0) {
        gPortVBlankIrqs++;
    }

    IO16(REG_OFFSET_IME) = 0;
    if (func != NULL) {
        func();
    }
    IO16(REG_OFFSET_IME) = 1;
    IO16(REG_OFFSET_IF) &= (u16)~mask;
}

void VBlankIntrWait(void) {
    int y;
    u16 dispstat;

    IO16(REG_OFFSET_VCOUNT) = 227;
    if (IO16(REG_OFFSET_DISPSTAT) & DISPSTAT_HBLANK_INTR) {
        GbaRaiseIrq(1);
    }

    for (y = 0; y < GBA_SCREEN_HEIGHT; y++) {
        dispstat = IO16(REG_OFFSET_DISPSTAT) &
                   (u16)~(DISPSTAT_VBLANK | DISPSTAT_HBLANK | DISPSTAT_VCOUNT);
        IO16(REG_OFFSET_VCOUNT) = (u16)y;

        if ((dispstat >> 8) == (u16)y) {
            dispstat |= DISPSTAT_VCOUNT;
            IO16(REG_OFFSET_DISPSTAT) = dispstat;
            if (dispstat & DISPSTAT_VCOUNT_INTR) {
                IO16(REG_OFFSET_DISPSTAT) = dispstat | DISPSTAT_HBLANK;
                GbaRaiseIrq(2);
                IO16(REG_OFFSET_DISPSTAT) &= (u16)~DISPSTAT_HBLANK;
            }
        }

        IO16(REG_OFFSET_DISPSTAT) = dispstat;
        PortCaptureLine(y);
        IO16(REG_OFFSET_DISPSTAT) |= DISPSTAT_HBLANK;
        GbaDmaHBlank();

        if (IO16(REG_OFFSET_DISPSTAT) & DISPSTAT_HBLANK_INTR) {
            GbaRaiseIrq(1);
        }
    }

    PortCaptureSubmit();
    PortVBlankWait();
    IO16(REG_OFFSET_KEYINPUT) = (u16)(~PortReadKeys()) & 0x03FFu;

    IO16(REG_OFFSET_VCOUNT) = GBA_SCREEN_HEIGHT;
    IO16(REG_OFFSET_DISPSTAT) =
        (IO16(REG_OFFSET_DISPSTAT) & (u16)~DISPSTAT_HBLANK) | DISPSTAT_VBLANK;

    GbaDmaVBlank();
    if (IO16(REG_OFFSET_DISPSTAT) & DISPSTAT_VBLANK_INTR) {
        GbaRaiseIrq(0);
    }

    IO16(REG_OFFSET_DISPSTAT) &= (u16)~DISPSTAT_VBLANK;
}

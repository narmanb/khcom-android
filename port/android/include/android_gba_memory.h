#ifndef KHCOM_ANDROID_GBA_MEMORY_H
#define KHCOM_ANDROID_GBA_MEMORY_H

#include <stddef.h>
#include <stdint.h>

/*
 * GBA bus addresses are always 32-bit values. Keep them separate from host
 * pointers: Android arm64 pointers must never be truncated to u32.
 */
typedef uint32_t GbaAddress;

enum {
    GBA_IO_BASE = 0x04000000u,
    GBA_PLTT_BASE = 0x05000000u,
    GBA_VRAM_BASE = 0x06000000u,
    GBA_OAM_BASE = 0x07000000u,
    GBA_ROM_BASE = 0x08000000u,
    GBA_ROM_END = 0x0A000000u,
    GBA_SRAM_BASE = 0x0E000000u,

    GBA_IO_SIZE = 0x400u,
    GBA_PLTT_SIZE = 0x400u,
    GBA_VRAM_SIZE = 0x18000u,
    GBA_OAM_SIZE = 0x400u,
    GBA_SRAM_SIZE = 0x10000u,
};

extern uint8_t gGbaIo[GBA_IO_SIZE];
extern uint8_t gGbaPltt[GBA_PLTT_SIZE];
extern uint8_t gGbaVram[GBA_VRAM_SIZE];
extern uint8_t gGbaOam[GBA_OAM_SIZE];
extern uint8_t gGbaSram[GBA_SRAM_SIZE];

/* ROM addresses are resolved later from the generated native symbol map. */
typedef void* (*AndroidRomResolver)(GbaAddress address);

void AndroidGbaSetRomResolver(AndroidRomResolver resolver);

/* Translate a known GBA bus address. Returns NULL for unsupported regions. */
void* AndroidGbaAddressToHost(GbaAddress address);

/*
 * Translate a pointer-shaped value only when it is actually inside a known
 * GBA bus range. Real Android pointers are returned unchanged.
 */
void* AndroidGbaPointerToHost(const void* pointer);

int AndroidGbaPointerIsBusAddress(const void* pointer);
int AndroidGbaAddressIsBios(GbaAddress address);

#endif

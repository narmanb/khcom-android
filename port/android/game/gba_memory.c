#include "android_gba_memory.h"

#include <limits.h>
#include <stdint.h>

_Static_assert(sizeof(GbaAddress) == 4, "GBA addresses must stay 32-bit");
_Static_assert(sizeof(uintptr_t) >= sizeof(void*), "uintptr_t must hold a host pointer");

uint8_t gGbaIo[GBA_IO_SIZE] __attribute__((aligned(64))) = {
    /* KEYINPUT is active-low and powers up with every key released. */
    [0x130] = 0xFF,
    [0x131] = 0x03,
};
uint8_t gGbaPltt[GBA_PLTT_SIZE] __attribute__((aligned(64)));
uint8_t gGbaVram[GBA_VRAM_SIZE] __attribute__((aligned(64)));
uint8_t gGbaOam[GBA_OAM_SIZE] __attribute__((aligned(64)));
uint8_t gGbaSram[GBA_SRAM_SIZE] __attribute__((aligned(64)));

static AndroidRomResolver sRomResolver;

void AndroidGbaSetRomResolver(AndroidRomResolver resolver) {
    sRomResolver = resolver;
}

static int IsKnownBusRange(GbaAddress address) {
    switch (address >> 24) {
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0E:
        return 1;
    default:
        return 0;
    }
}

void* AndroidGbaAddressToHost(GbaAddress address) {
    uint32_t offset;

    switch (address >> 24) {
    case 0x04:
        offset = address & 0x00FFFFFFu;
        return offset < GBA_IO_SIZE ? &gGbaIo[offset] : NULL;

    case 0x05:
        return &gGbaPltt[address & (GBA_PLTT_SIZE - 1u)];

    case 0x06:
        /*
         * Match the GBA's 96 KiB VRAM mirroring used by the Vita port:
         * 0x06018000-0x0601FFFF mirrors 0x06010000-0x06017FFF.
         */
        offset = address & 0x1FFFFu;
        if (offset >= GBA_VRAM_SIZE) {
            offset -= 0x8000u;
        }
        return &gGbaVram[offset];

    case 0x07:
        return &gGbaOam[address & (GBA_OAM_SIZE - 1u)];

    case 0x08:
    case 0x09:
        return sRomResolver != NULL ? sRomResolver(address) : NULL;

    case 0x0E:
        return &gGbaSram[address & (GBA_SRAM_SIZE - 1u)];

    default:
        return NULL;
    }
}

void* AndroidGbaPointerToHost(const void* pointer) {
    uintptr_t raw = (uintptr_t)pointer;
    void* mapped;

    /*
     * A real arm64 pointer normally cannot fit in 32 bits. More importantly,
     * we do not classify values by "low address" alone: only explicit GBA bus
     * ranges are candidates for translation.
     */
    if (raw > UINT32_MAX) {
        return (void*)pointer;
    }
    if (!IsKnownBusRange((GbaAddress)raw)) {
        return (void*)pointer;
    }

    mapped = AndroidGbaAddressToHost((GbaAddress)raw);
    return mapped;
}

int AndroidGbaPointerIsBusAddress(const void* pointer) {
    uintptr_t raw = (uintptr_t)pointer;

    return raw <= UINT32_MAX && IsKnownBusRange((GbaAddress)raw);
}

int AndroidGbaAddressIsBios(GbaAddress address) {
    return address < 0x02000000u;
}

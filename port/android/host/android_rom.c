/*
 * Runtime ROM restoration and ROM-address translation for Android.
 *
 * The native library is built with ROM-derived bytes zeroed. A compact map
 * tells us where those bytes came from; the user's verified ROM supplies them
 * before the game starts.
 */
#include "android_rom.h"
#include "android_gba_memory.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROMMAP_VERSION 1u
#define ROM_MAX_SIZE (64u * 1024u * 1024u)

typedef struct AndroidRomXlate {
    uint32_t gba;
    const uint8_t* host;
} AndroidRomXlate;

extern const AndroidRomXlate gRomXlate[];
extern const uint32_t gRomXlateCount;

typedef struct RomMapHeader {
    char magic[4];
    uint32_t version;
    uint32_t anchor;
    uint32_t runCount;
    uint8_t sha1[20];
    uint32_t romSize;
} RomMapHeader;

typedef struct RomRun {
    uint32_t addr;
    uint32_t romOffset;
    uint32_t length;
} RomRun;

static void SetError(char* error, size_t errorSize, const char* fmt, ...) {
    va_list ap;

    if (error == NULL || errorSize == 0) {
        return;
    }
    va_start(ap, fmt);
    vsnprintf(error, errorSize, fmt, ap);
    va_end(ap);
}

static void* ResolveRomAddress(GbaAddress address) {
    uint32_t lo = 0;
    uint32_t hi = gRomXlateCount;

    if (gRomXlateCount == 0) {
        return NULL;
    }

    while (hi - lo > 1) {
        uint32_t mid = (lo + hi) / 2;
        if (gRomXlate[mid].gba <= address) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    if (gRomXlate[lo].gba > address) {
        return NULL;
    }
    return (void*)(gRomXlate[lo].host + (address - gRomXlate[lo].gba));
}

void AndroidRomResolverInit(void) {
    AndroidGbaSetRomResolver(ResolveRomAddress);
}

/* Minimal SHA-1 ------------------------------------------------------------- */

#define ROL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

static void Sha1Block(uint32_t h[5], const uint8_t* p) {
    uint32_t w[80];
    uint32_t a, b, c, d, e;
    int i;

    for (i = 0; i < 16; i++) {
        w[i] = ((uint32_t)p[i * 4] << 24) |
               ((uint32_t)p[i * 4 + 1] << 16) |
               ((uint32_t)p[i * 4 + 2] << 8) |
               p[i * 4 + 3];
    }
    for (; i < 80; i++) {
        w[i] = ROL32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }

    a = h[0];
    b = h[1];
    c = h[2];
    d = h[3];
    e = h[4];

    for (i = 0; i < 80; i++) {
        uint32_t f;
        uint32_t k;
        uint32_t t;

        if (i < 20) {
            f = (b & c) | (~b & d);
            k = 0x5A827999u;
        } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1u;
        } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDCu;
        } else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6u;
        }

        t = ROL32(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = ROL32(b, 30);
        b = a;
        a = t;
    }

    h[0] += a;
    h[1] += b;
    h[2] += c;
    h[3] += d;
    h[4] += e;
}

static void Sha1(const uint8_t* data, uint32_t len, uint8_t out[20]) {
    uint32_t h[5] = {
        0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u
    };
    uint8_t tail[128];
    uint32_t i;
    uint32_t rest = len % 64u;
    uint32_t tailLen = rest < 56u ? 64u : 128u;
    uint64_t bits = (uint64_t)len * 8u;

    for (i = 0; i + 64u <= len; i += 64u) {
        Sha1Block(h, data + i);
    }

    memset(tail, 0, sizeof(tail));
    memcpy(tail, data + i, rest);
    tail[rest] = 0x80;

    for (i = 0; i < 8; i++) {
        tail[tailLen - 1u - i] = (uint8_t)(bits >> (i * 8));
    }
    for (i = 0; i < tailLen; i += 64u) {
        Sha1Block(h, tail + i);
    }
    for (i = 0; i < 20; i++) {
        out[i] = (uint8_t)(h[i / 4] >> (24 - (i % 4) * 8));
    }
}

static uint8_t* ReadWholeFile(const char* path, uint32_t* size,
                              char* error, size_t errorSize) {
    FILE* fp;
    long length;
    uint8_t* data;
    size_t got;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        SetError(error, errorSize, "could not open ROM: %s", path);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0 ||
        (length = ftell(fp)) <= 0 ||
        (unsigned long)length > ROM_MAX_SIZE ||
        fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        SetError(error, errorSize, "could not determine ROM size");
        return NULL;
    }

    data = (uint8_t*)malloc((size_t)length);
    if (data == NULL) {
        fclose(fp);
        SetError(error, errorSize, "not enough memory to read ROM");
        return NULL;
    }

    got = fread(data, 1, (size_t)length, fp);
    fclose(fp);
    if (got != (size_t)length) {
        free(data);
        SetError(error, errorSize, "could not read complete ROM");
        return NULL;
    }

    *size = (uint32_t)length;
    return data;
}

int AndroidRomLoad(const char* romPath, const void* mapData, size_t mapSize,
                   char* error, size_t errorSize) {
    const uint8_t* map = (const uint8_t*)mapData;
    const RomMapHeader* header;
    const RomRun* runs;
    uint8_t* rom;
    uint8_t digest[20];
    uint32_t romSize;
    uintptr_t slide;
    uint32_t i;
    size_t required;

    if (romPath == NULL || map == NULL) {
        SetError(error, errorSize, "ROM path or relocation map is missing");
        return 0;
    }
    if (mapSize < sizeof(RomMapHeader)) {
        SetError(error, errorSize, "ROM relocation map is truncated");
        return 0;
    }

    header = (const RomMapHeader*)map;
    if (memcmp(header->magic, "KHRM", 4) != 0 ||
        header->version != ROMMAP_VERSION) {
        SetError(error, errorSize, "ROM relocation map has an unsupported format");
        return 0;
    }

    if (header->runCount > (SIZE_MAX - sizeof(RomMapHeader)) / sizeof(RomRun)) {
        SetError(error, errorSize, "ROM relocation map run count is invalid");
        return 0;
    }
    required = sizeof(RomMapHeader) + (size_t)header->runCount * sizeof(RomRun);
    if (mapSize < required) {
        SetError(error, errorSize, "ROM relocation map is truncated");
        return 0;
    }

    rom = ReadWholeFile(romPath, &romSize, error, errorSize);
    if (rom == NULL) {
        return 0;
    }

    Sha1(rom, romSize, digest);
    if (romSize != header->romSize || memcmp(digest, header->sha1, 20) != 0) {
        free(rom);
        SetError(error, errorSize,
                 "ROM is not the exact game revision expected by this build");
        return 0;
    }

    /*
     * The shared object's loadable image uses one relocation slide. gGbaIo is
     * the anchor recorded by the build tool, so this converts link-time map
     * addresses into their runtime locations.
     */
    slide = (uintptr_t)gGbaIo - (uintptr_t)header->anchor;
    runs = (const RomRun*)(map + sizeof(RomMapHeader));

    for (i = 0; i < header->runCount; i++) {
        uint64_t romEnd = (uint64_t)runs[i].romOffset + runs[i].length;
        uintptr_t target;

        if (romEnd > romSize) {
            free(rom);
            SetError(error, errorSize, "ROM relocation map contains an invalid run");
            return 0;
        }

        target = slide + (uintptr_t)runs[i].addr;
        memcpy((void*)target, rom + runs[i].romOffset, runs[i].length);
    }

    free(rom);
    AndroidRomResolverInit();
    return 1;
}

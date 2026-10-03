/*
 * Minimal Android host for native bring-up.
 *
 * This deliberately has no Java/UI dependency yet. It runs the software PPU,
 * audio/PSG mix and 60 Hz pacing so AgbMain() can execute against a real host
 * while the Activity/OpenGL/AAudio front end is built separately.
 */
#include "port.h"
#include "ppu.h"
#include "android_host.h"

#include <android/log.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#define LOG_TAG "KHCOM"
#define FRAME_NS 16666667LL
#define CODE_BLOCK_SIZE (1024u * 1024u)
#define MAX_AUDIO_SAMPLES 4096
#define AUDIO_OUT_RATE 48000
#define AUDIO_RING_SIZE 16384
#define AUDIO_TARGET_FILL 2048

static PpuFrame sFrame;
static uint32_t sRgba[GBA_SCREEN_WIDTH * GBA_SCREEN_HEIGHT];
static volatile uint16_t sKeys;
static volatile uint32_t sFrameCounter;

static char sSavePath[1024];
static char sSaveTmpPath[1032];
static int sSramDirty;
static int sSramFlushFrames;

static int16_t sAudioPsg[MAX_AUDIO_SAMPLES * 2];
static int16_t sAudioRing[AUDIO_RING_SIZE * 2];
static volatile uint32_t sAudioWritePos;
static volatile uint32_t sAudioReadPos;
static volatile int sAudioSrcRate = 15768;

static uint8_t* sCodeBase;
static uint32_t sCodeUsed;
static int sCodeLive;

static int64_t sNextFrameNs;

static int64_t MonotonicNs(void) {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static void SleepUntil(int64_t targetNs) {
    struct timespec ts;

    if (targetNs <= 0) {
        return;
    }
    ts.tv_sec = targetNs / 1000000000LL;
    ts.tv_nsec = targetNs % 1000000000LL;
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL) == EINTR) {
    }
}

static int16_t Clamp16(int value) {
    if (value > 32767) {
        return 32767;
    }
    if (value < -32768) {
        return -32768;
    }
    return (int16_t)value;
}

void AndroidHostSetKeys(uint16_t keys) {
    sKeys = keys & 0x03FFu;
}

int AndroidHostInitSram(const char* romPath, char* error, unsigned errorSize) {
    const char* slash;
    FILE* fp;
    size_t dirLen;
    size_t got;

    if (romPath == NULL) {
        if (error && errorSize) snprintf(error, errorSize, "ROM path is missing");
        return 0;
    }

    slash = strrchr(romPath, '/');
    dirLen = slash != NULL ? (size_t)(slash - romPath) : 0;
    if (dirLen + sizeof("/khcom.sav") >= sizeof(sSavePath)) {
        if (error && errorSize) snprintf(error, errorSize, "save path is too long");
        return 0;
    }

    if (dirLen != 0) {
        memcpy(sSavePath, romPath, dirLen);
    }
    sSavePath[dirLen] = '\0';
    snprintf(sSavePath + dirLen, sizeof(sSavePath) - dirLen, "/khcom.sav");
    snprintf(sSaveTmpPath, sizeof(sSaveTmpPath), "%s.tmp", sSavePath);

    memset(gGbaSram, 0xFF, sizeof(gGbaSram));
    fp = fopen(sSavePath, "rb");
    if (fp == NULL) {
        return 1;
    }

    got = fread(gGbaSram, 1, sizeof(gGbaSram), fp);
    fclose(fp);
    if (got != sizeof(gGbaSram)) {
        memset(gGbaSram, 0xFF, sizeof(gGbaSram));
        if (error && errorSize) snprintf(error, errorSize, "existing khcom.sav is not 64 KiB");
        return 0;
    }
    return 1;
}

void AndroidHostFlushSram(void) {
    FILE* fp;
    size_t wrote;

    if (!sSramDirty || sSavePath[0] == '\0') {
        return;
    }

    fp = fopen(sSaveTmpPath, "wb");
    if (fp == NULL) {
        PortLog("save: cannot open temporary save");
        return;
    }

    wrote = fwrite(gGbaSram, 1, sizeof(gGbaSram), fp);
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);

    if (wrote != sizeof(gGbaSram)) {
        unlink(sSaveTmpPath);
        PortLog("save: incomplete write");
        return;
    }

    if (rename(sSaveTmpPath, sSavePath) != 0) {
        unlink(sSaveTmpPath);
        PortLog("save: rename failed");
        return;
    }

    sSramDirty = 0;
    sSramFlushFrames = 0;
}

const uint32_t* AndroidHostGetFrame(void) {
    return sRgba;
}

uint32_t AndroidHostGetFrameCounter(void) {
    return sFrameCounter;
}

void PortCaptureLine(int y) {
    if ((unsigned)y >= GBA_SCREEN_HEIGHT) {
        return;
    }
    memcpy(sFrame.io[y], gGbaIo, PPU_LINE_IO_SIZE);
}

void PortCaptureSubmit(void) {
    memcpy(sFrame.pltt, gGbaPltt, sizeof(sFrame.pltt));
    memcpy(sFrame.oam, gGbaOam, sizeof(sFrame.oam));
    memcpy(sFrame.vram, gGbaVram, sizeof(sFrame.vram));
}

void PortVBlankWait(void) {
    int64_t now;

    PpuSetOutput(sRgba, GBA_SCREEN_WIDTH, GBA_SCREEN_WIDTH);
    PpuRenderFrame(&sFrame);
    sFrameCounter++;

    now = MonotonicNs();
    if (sNextFrameNs == 0 || now - sNextFrameNs > FRAME_NS * 4) {
        sNextFrameNs = now + FRAME_NS;
    } else {
        sNextFrameNs += FRAME_NS;
    }
    SleepUntil(sNextFrameNs);

    if (sSramDirty && sSramFlushFrames > 0 && --sSramFlushFrames == 0) {
        AndroidHostFlushSram();
    }
}

uint16_t PortReadKeys(void) {
    return sKeys;
}

void PortSramWritten(void) {
    sSramDirty = 1;
    sSramFlushFrames = 10;
}

void PortAudioPush(const int8_t* right, const int8_t* left, int samples, int rate) {
    uint32_t write = sAudioWritePos;
    int i;

    if (samples <= 0) {
        return;
    }
    if (rate > 0) {
        sAudioSrcRate = rate;
    }
    if (samples > MAX_AUDIO_SAMPLES) {
        samples = MAX_AUDIO_SAMPLES;
    }

    /* PSG advances even when the DirectSound frame must be dropped. */
    PsgRender(sAudioPsg, samples, sAudioSrcRate);

    if ((uint32_t)(write - sAudioReadPos) + (uint32_t)samples >= AUDIO_RING_SIZE) {
        return;
    }

    for (i = 0; i < samples; i++, write++) {
        uint32_t idx = (write & (AUDIO_RING_SIZE - 1u)) * 2u;
        sAudioRing[idx] = Clamp16((int)left[i] * 256 + sAudioPsg[i * 2]);
        sAudioRing[idx + 1] = Clamp16((int)right[i] * 256 + sAudioPsg[i * 2 + 1]);
    }
    sAudioWritePos = write;
}

int AndroidHostReadAudio(int16_t* out, int frames) {
    static uint32_t frac;
    static int16_t prev[2];
    uint32_t avail;
    int64_t step;
    int i;

    if (out == NULL || frames <= 0) {
        return 0;
    }

    avail = sAudioWritePos - sAudioReadPos;
    step = ((int64_t)sAudioSrcRate << 16) / AUDIO_OUT_RATE;
    step += step * ((int32_t)avail - AUDIO_TARGET_FILL) /
            (AUDIO_TARGET_FILL * 200);

    for (i = 0; i < frames; i++) {
        uint32_t read = sAudioReadPos;
        int16_t current[2];

        if (sAudioWritePos == read) {
            out[i * 2] = prev[0];
            out[i * 2 + 1] = prev[1];
            continue;
        }

        current[0] = sAudioRing[(read & (AUDIO_RING_SIZE - 1u)) * 2u];
        current[1] = sAudioRing[(read & (AUDIO_RING_SIZE - 1u)) * 2u + 1u];
        out[i * 2] = (int16_t)(prev[0] +
            (((current[0] - prev[0]) * (int32_t)frac) >> 16));
        out[i * 2 + 1] = (int16_t)(prev[1] +
            (((current[1] - prev[1]) * (int32_t)frac) >> 16));

        frac += (uint32_t)step;
        while (frac >= 0x10000u && sAudioWritePos != sAudioReadPos) {
            frac -= 0x10000u;
            prev[0] = sAudioRing[(sAudioReadPos & (AUDIO_RING_SIZE - 1u)) * 2u];
            prev[1] = sAudioRing[(sAudioReadPos & (AUDIO_RING_SIZE - 1u)) * 2u + 1u];
            sAudioReadPos++;
        }
        if (frac >= 0x10000u) {
            frac = 0xFFFFu;
        }
    }

    return frames;
}

void* PortCodeAlloc(uint32_t size) {
    void* result;

    if (sCodeBase == NULL) {
        sCodeBase = mmap(NULL, CODE_BLOCK_SIZE, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (sCodeBase == MAP_FAILED) {
            sCodeBase = NULL;
            PortFatal("movie: mmap for executable memory failed");
        }
    }

    size = (size + 63u) & ~63u;
    if (sCodeUsed + size > CODE_BLOCK_SIZE) {
        PortFatal("movie: executable memory exhausted (%u + %u)",
                  (unsigned)sCodeUsed, (unsigned)size);
    }

    if (mprotect(sCodeBase, CODE_BLOCK_SIZE, PROT_READ | PROT_WRITE) != 0) {
        PortFatal("movie: cannot make codec memory writable");
    }

    result = sCodeBase + sCodeUsed;
    sCodeUsed += size;
    sCodeLive++;
    return result;
}

void PortCodeFree(void* p) {
    (void)p;

    if (sCodeLive > 0) {
        sCodeLive--;
    }
    if (sCodeLive == 0) {
        sCodeUsed = 0;
    }
}

void PortCodeBeginWrite(void) {
    if (sCodeBase != NULL &&
        mprotect(sCodeBase, CODE_BLOCK_SIZE, PROT_READ | PROT_WRITE) != 0) {
        PortFatal("movie: cannot reopen codec memory for writing");
    }
}

void PortCodeEndWrite(void) {
    if (sCodeBase == NULL || sCodeUsed == 0) {
        return;
    }

    __builtin___clear_cache((char*)sCodeBase, (char*)sCodeBase + sCodeUsed);
    if (mprotect(sCodeBase, CODE_BLOCK_SIZE, PROT_READ | PROT_EXEC) != 0) {
        PortFatal("movie: cannot make codec memory executable");
    }
}

void PortLog(const char* fmt, ...) {
    char buffer[1024];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", buffer);
}

void PortFatal(const char* fmt, ...) {
    char buffer[1024];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);
    __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, "%s", buffer);
    abort();
}

void PortSoftReset(void) {
    PortFatal("soft reset requested before Android lifecycle restart is wired");
}

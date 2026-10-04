#ifndef GUARD_ANIM_H
#define GUARD_ANIM_H

#include "types.h"

typedef struct AnimDef {
    void* gfxTable;
    void* anims;
    void* tiles;
    u8 animId;
    u8 unk_0D[0x03];
} AnimDef;

#if defined(PLATFORM_ANDROID) && defined(__clang__)
/* These on-ROM records explicitly override the default GBA record boundary. */
#pragma push_macro("struct")
#undef struct
#endif

typedef struct AnimFrame {
    u16 gfxIndex;
    u16 duration;
} __attribute__((packed, aligned(2))) AnimFrame;

typedef struct AnimHeader {
    u16 unk_00;
    u16 unk_02;
    u16 frameCount;
    AnimFrame frames[0];
} __attribute__((packed, aligned(2))) AnimHeader;

typedef char AnimHeader_size[(sizeof(AnimHeader) == 6) ? 1 : -1];
typedef char AnimFrame_size[(sizeof(AnimFrame) == 4) ? 1 : -1];

#if defined(PLATFORM_ANDROID) && defined(__clang__)
#pragma pop_macro("struct")
#endif

enum AnimFlag {
    ANIM_FLAG_LOOP = 0x1,
    ANIM_FLAG_RANDOM_START = 0x2,
    ANIM_FLAG_KEEP_FRAME = 0x4,
    ANIM_FLAG_FINISHED = 0x1000
};

typedef struct AnimState {
    AnimHeader** anims;
    void** gfxTable;
    u16 flags;
    u16 timer;
    u16 frameCount;
    u16 frame;
    u16 animId;
    u16 unk_12;
    AnimFrame* frames;
} AnimState;

void AnimInit(AnimState* anim, void* anims, void* gfxTable);
void AnimChangeWithTables(AnimState* anim, u16 animId, u16 flags, void* anims, void* gfxTable);
u8 AnimIsFrameEnding(AnimState* anim);

void AnimStart(AnimState* anim, u16 animId, u16 flags);
void AnimChange(AnimState* anim, u16 animId, u16 flags);
void* AnimUpdate(AnimState* anim);
void* AnimGetGfx(AnimState* anim);
u8 AnimIsFinished(AnimState* anim);
u16 AnimGetId(AnimState* anim);
u16 AnimGetFrame(AnimState* anim);
u16 AnimGetGfxIndex(AnimState* anim);
void AnimSetFrame(AnimState* anim, u16 frame);
void AnimReset(AnimState* anim);

#endif

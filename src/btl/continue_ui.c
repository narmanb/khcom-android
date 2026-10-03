#include "system_state.h"
#include "anim.h"
#include "types.h"
#include "bg_animation_data.h"
#include "mode_battle_data.h"
#include "engine_math.h"
#include "display.h"
#include "fade.h"
#include "obj_api.h"
#include "pallet.h"
#include "key.h"
#include "m4a_song.h"
#include "sprites_continue.h"
#include "gba/keys.h"
#include "gba/io_reg.h"
#include "songs.h"
#include "battle.h"
#include "continue_types.h"
#include "obj.h"
#include "sprite_palettes.h"
#include <stddef.h>
#include "taskpool.h"

static const s32 sContinueCursorY[2] = {
    0x4000, 0x5600,
};

#ifdef VERSION_EU
static void* sContinueLanguageBgTiles[5] = {
    gUnk_0941A418,
    gUnkEu_0954C7B4,
    gUnkEu_0954D7A0,
    gUnkEu_0954D238,
    gUnkEu_0954CD1C,
};
#endif

void LoadContinueCursorPalette(s32 a) {
    switch (a) {
    case 0:
        LoadBgPalette(0, gUnk_096145D8, 0x40);
        break;
    case 1:
        LoadBgPalette(0, gUnk_09614618, 0x40);
        break;
    }
}
#ifdef VERSION_JP
#define MSG_CONT_BG_TILES 0x1A40
#define MSG_CONT_X 0xA400
#else
#define MSG_CONT_BG_TILES 0x1AA0
#define MSG_CONT_X 0xBC00
#endif

void ContinueSora_0(ContinueWork* p) {
    u8 i;

    SetBgMode1();
    p->cursor = 0;
    SetBackdropColor(0, 0, 0);
    SetupBg(0, 0, 31, 0);
    SetupBg(2, 2, 28, 10);
    SetBgPriority(2, 0);
    SetBgPriority(0, 1);
    SetBgPriority(1, 2);
#ifdef VERSION_EU
    LoadBgTilesLz77(0, sContinueLanguageBgTiles[gLanguage]);
    LoadBgMapLz77(0, gUnk_0951CAB8);
#else
    LoadBgTiles(0, gUnk_0941A418, MSG_CONT_BG_TILES);
    LoadBgMap(0, gUnk_0951CAB8, 0x800);
#endif
    BgAnimInit(2, 0x8000, 128);
    BgAnimStart(&gBgAnimDefCharaDefeatEnd, 120, 46);
    BgAnimSetLoopStartFrame(0);
    p->tiles3 = LoadObjTiles(gUnk_090A7D9A, 192);
    p->palette3 = LoadObjPalette(gUnk_096146F8, 32);
    LoadContinueCursorPalette(p->cursor);
    p->tiles = AllocObjTiles(512, NULL);
    PushPaletteEffect(0);
    p->palette = LoadObjPalette(gUnk_09614658, 160);
    PopPaletteEffect();
    SetObjTileSource(p->tiles, gUnk_090A6B26);
    AnimInit(&p->anim, gUnk_09EEB108, gUnk_09EEB0C4);
    AnimStart(&p->anim, 0, ANIM_FLAG_LOOP);
    p->tiles2 = AllocObjTiles(1024, NULL);
    p->palette2 = LoadObjPalette(gSoraPalette, 32);
    SetObjTileSource(p->tiles2, gSoraContinueTiles);
    AnimInit(&p->anim2, gSoraContinueAnims, gSoraContinueFrames);
    AnimStart(&p->anim2, 0, ANIM_FLAG_LOOP);
    p->unk_58 = -2048;
    p->unk_5C = 0xA000;
    p->steps = 16;

#ifdef VERSION_EU
    switch (gLanguage) {
    case LANGUAGE_ENGLISH:
    case LANGUAGE_FRENCH:
    case LANGUAGE_SPANISH:
    case 5:
    case 6:
        p->x = 0xBC00;
        break;
    case LANGUAGE_GERMAN:
    case LANGUAGE_ITALIAN:
        p->x = 0xC000;
        break;
    }
#else
    p->x = MSG_CONT_X;
#endif
    p->y = 0x4000;
    p->unk_64 = 0;
    p->blendAlpha = 0;
    FadeStartIn(FADE_MODE_WHITE, 24);

    for (i = 0; i < 5; i++) {
        FadeSetPaletteExcluded(p->palette->index + i, 0);
    }

    p->blendAlpha = 0x1000;
    p->state = 0;
}

void ContinueRiku_0(ContinueWork* p) {
    u8 i;

    SetBgMode1();
    p->cursor = 0;
    SetBackdropColor(0, 0, 0);
    SetupBg(0, 0, 31, 0);
    SetupBg(2, 2, 28, 10);
    SetBgPriority(2, 0);
    SetBgPriority(0, 1);
    SetBgPriority(1, 2);
#ifdef VERSION_EU
    LoadBgTilesLz77(0, sContinueLanguageBgTiles[gLanguage]);
    LoadBgMapLz77(0, gUnk_0951CAB8);
#else
    LoadBgTiles(0, gUnk_0941A418, MSG_CONT_BG_TILES);
    LoadBgMap(0, gUnk_0951CAB8, 0x800);
#endif
    BgAnimInit(2, 0x8000, 128);
    BgAnimStart(&gBgAnimDefCharaDefeatEnd, 120, 46);
    BgAnimSetLoopStartFrame(0);
    p->tiles3 = LoadObjTiles(gUnk_090A7D9A, 192);
    p->palette3 = LoadObjPalette(gUnk_096146F8, 32);
    LoadContinueCursorPalette(p->cursor);
    p->tiles = AllocObjTiles(512, NULL);
    PushPaletteEffect(0);
    p->palette = LoadObjPalette(gUnk_09614658, 160);
    PopPaletteEffect();
    SetObjTileSource(p->tiles, gUnk_090A6B26);
    AnimInit(&p->anim, gUnk_09EEB108, gUnk_09EEB0C4);
    AnimStart(&p->anim, 0, ANIM_FLAG_LOOP);
    p->tiles2 = AllocObjTiles(1024, NULL);
    p->palette2 = LoadObjPalette(gRikuPalette, 32);
    SetObjTileSource(p->tiles2, gRikuContinueTiles);
    AnimInit(&p->anim2, gRikuContinueAnims, gRikuContinueFrames);
    AnimStart(&p->anim2, 0, ANIM_FLAG_LOOP);
    p->unk_58 = -2048;
    p->unk_5C = 0xA000;
    p->steps = 16;

#ifdef VERSION_EU
    switch (gLanguage) {
    case LANGUAGE_ENGLISH:
    case LANGUAGE_FRENCH:
    case LANGUAGE_SPANISH:
    case 5:
    case 6:
        p->x = 0xBC00;
        break;
    case LANGUAGE_GERMAN:
    case LANGUAGE_ITALIAN:
        p->x = 0xC000;
        break;
    }
#else
    p->x = MSG_CONT_X;
#endif
    p->y = 0x4000;
    p->unk_64 = 0;
    p->blendAlpha = 0;
    FadeStartIn(FADE_MODE_WHITE, 24);

    for (i = 0; i < 5; i++) {
        FadeSetPaletteExcluded(p->palette->index + i, 0);
    }

    p->blendAlpha = 0x1000;
    p->state = 0;
}

static s32 Continue_1(ContinueWork* p) {
    const s32* t;

    BgAnimUpdate();
    p->gfx = AnimUpdate(&p->anim);
    p->gfx2 = AnimUpdate(&p->anim2);
    gBldCnt = (BLDCNT_TGT1_BG2 | BLDCNT_TGT1_OBJ | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG0 | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG3);
    gBldAlpha = p->blendAlpha;

    if (p->state == 0) {
        if (FadeIsActive() == 0) {
            p->state = 1;
        }
    }

    if (p->state == 1) {
        if (p->steps > 0) {
            ApproachValue(&p->unk_58, 0, p->steps);
            ApproachValue(&p->unk_5C, 0x9800, p->steps);
            p->steps--;
        }

        if (p->blendAlpha < 0x1010) {
            p->blendAlpha++;
        } else {
            p->blendAlpha = 0x1010;
        }

        if ((GetKeysPressed() & DPAD_UP) != 0) {
            if (p->cursor == 1) {
                p->cursor = 0;
                m4aSongNumStart(SONG_SYS_CLICK);
            }
        }

        if ((GetKeysPressed() & DPAD_DOWN) != 0) {
            if (p->cursor == 0) {
                p->cursor = 1;
                m4aSongNumStart(SONG_SYS_CLICK);
            }
        }

        if ((GetKeysHeld() & A_BUTTON) != 0) {
            switch (p->cursor) {
            case 0:
                FadeStartOut(FADE_MODE_BLACK, 96);
                break;
            case 1:
                FadeStartOut(FADE_MODE_BLACK, 96);
                break;
            }

            m4aSongNumStart(SONG_SYS_KETTEI);
            p->state = 2;
            p->steps = 16;
        }
    }

    if (p->state == 2) {
        if (p->steps > 0) {
            ApproachValue(&p->unk_58, -2048, p->steps);
            ApproachValue(&p->unk_5C, 0xA000, p->steps);
            p->steps--;
        }

        if (p->blendAlpha > 0x1000) {
            p->blendAlpha--;
        } else {
            p->blendAlpha = 0x1000;
        }

        if (FadeIsActive() == 0) {
            DisableBg(0);
            DisableBg(2);
            LoadBgMap(0, gUnk_08125E24, 0x800);
            LoadBgMap(2, gUnk_08125E24, 0x800);
            p->state = 3;
        }
    }

    LoadContinueCursorPalette(p->cursor);
    t = sContinueCursorY;
    p->y += (t[p->cursor] - p->y) >> 3;
    p->unk_64 += 4;
#ifdef PLATFORM_ANDROID
    /* agbcc left r0 non-zero when this task update fell off the end. */
    return 1;
#endif
}

static void Continue_2(ContinueWork* p) {
    DrawSprite(p->x >> 8, p->y >> 8, p->gfx, p->tiles, p->palette, NULL, SPRITE_FLAG_BLEND, 100);
    DrawSprite(120, 120, p->gfx2, p->tiles2, p->palette2, NULL, 0, 100);
}

static void Continue_3(ContinueWork* p) {
    DisableBg(0);
    DisableBg(2);
    LoadBgMap(0, gUnk_08125E24, 0x800);
    LoadBgMap(2, gUnk_08125E24, 0x800);
    ReleaseObjTiles(p->tiles2);
    ReleaseObjPalette(p->palette2);
    ReleaseObjPalette(p->palette);
    ReleaseObjTiles(p->tiles);
    ReleaseObjTiles(p->tiles3);
    ReleaseObjPalette(p->palette3);
    gBldCnt = 0;
}

TaskDesc gTaskDescContinueSora = {
    "Continue",
    (TaskInitFunc)ContinueSora_0,
    (TaskUpdateFunc)Continue_1,
    (TaskDrawFunc)Continue_2,
    (TaskDestroyFunc)Continue_3,
    sizeof(ContinueWork),
};

TaskDesc gTaskDescContinueRiku = {
    "Continue",
    (TaskInitFunc)ContinueRiku_0,
    (TaskUpdateFunc)Continue_1,
    (TaskDrawFunc)Continue_2,
    (TaskDestroyFunc)Continue_3,
    sizeof(ContinueWork),
};

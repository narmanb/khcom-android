#include "task_descriptors.h"
#include "card_battle.h"
#include "engine_math.h"
#include "display.h"
#include "obj_api.h"
#include "btl.h"
#include "btl_effect.h"
#include "btl_api.h"
#include "sprites_btl.h"
#include "sprites_fld.h"
#include "sprites_sora.h"
#include "gba/keys.h"
#include "system_state.h"
#include "player_progression.h"
#include "fade.h"
#include "songs.h"
#include <string.h>
#include "btl_tasks.h"
#include "anim.h"
#include "battle.h"
#include "battle_actor.h"
#include "battle_actor_types.h"
#include "battle_work.h"
#include "btl_collision.h"
#include "card_api.h"
#include "fld_types.h"
#include "game_state.h"
#include "key.h"
#include "listpool.h"
#include "m4a_song.h"
#include "obj.h"
#include "player_progression_types.h"
#include "taskpool.h"
#include "types.h"
#include "util.h"
#include <stddef.h>

static const AnimDef sBtlSoraAnimDefs[77] = {
    { gSor1ll50Frames, gSor1ll50Anims, gSor1ll50Tiles, 0, { 0, 0, 0 } },
    { gSor1ll51Frames, gSor1ll51Anims, gSor1ll51Tiles, 0, { 0, 0, 0 } },
    { gSor1ll55Frames, gSor1ll55Anims, gSor1ll55Tiles, 4, { 0, 0, 0 } },
    { gSor1ll55Frames, gSor1ll55Anims, gSor1ll55Tiles, 3, { 0, 0, 0 } },
    { gSor1ll55Frames, gSor1ll55Anims, gSor1ll55Tiles, 0, { 0, 0, 0 } },
    { gSor1ll55Frames, gSor1ll55Anims, gSor1ll55Tiles, 1, { 0, 0, 0 } },
    { gSor1ll55Frames, gSor1ll55Anims, gSor1ll55Tiles, 2, { 0, 0, 0 } },
    { gSor1ll57Frames, gSor1ll57Anims, gSor1ll57Tiles, 4, { 0, 0, 0 } },
    { gSor1ll57Frames, gSor1ll57Anims, gSor1ll57Tiles, 3, { 0, 0, 0 } },
    { gSor1ll57Frames, gSor1ll57Anims, gSor1ll57Tiles, 0, { 0, 0, 0 } },
    { gSor1ll57Frames, gSor1ll57Anims, gSor1ll57Tiles, 1, { 0, 0, 0 } },
    { gSor1ll57Frames, gSor1ll57Anims, gSor1ll57Tiles, 2, { 0, 0, 0 } },
    { gSor1ll56Frames, gSor1ll56Anims, gSor1ll56Tiles, 4, { 0, 0, 0 } },
    { gSor1ll56Frames, gSor1ll56Anims, gSor1ll56Tiles, 3, { 0, 0, 0 } },
    { gSor1ll56Frames, gSor1ll56Anims, gSor1ll56Tiles, 0, { 0, 0, 0 } },
    { gSor1ll56Frames, gSor1ll56Anims, gSor1ll56Tiles, 1, { 0, 0, 0 } },
    { gSor1ll56Frames, gSor1ll56Anims, gSor1ll56Tiles, 2, { 0, 0, 0 } },
    { gSor1ll58Frames, gSor1ll58Anims, gSor1ll58Tiles, 4, { 0, 0, 0 } },
    { gSor1ll58Frames, gSor1ll58Anims, gSor1ll58Tiles, 3, { 0, 0, 0 } },
    { gSor1ll58Frames, gSor1ll58Anims, gSor1ll58Tiles, 0, { 0, 0, 0 } },
    { gSor1ll58Frames, gSor1ll58Anims, gSor1ll58Tiles, 1, { 0, 0, 0 } },
    { gSor1ll58Frames, gSor1ll58Anims, gSor1ll58Tiles, 2, { 0, 0, 0 } },
    { gSor1ll59Frames, gSor1ll59Anims, gSor1ll59Tiles, 4, { 0, 0, 0 } },
    { gSor1ll59Frames, gSor1ll59Anims, gSor1ll59Tiles, 3, { 0, 0, 0 } },
    { gSor1ll59Frames, gSor1ll59Anims, gSor1ll59Tiles, 0, { 0, 0, 0 } },
    { gSor1ll59Frames, gSor1ll59Anims, gSor1ll59Tiles, 1, { 0, 0, 0 } },
    { gSor1ll59Frames, gSor1ll59Anims, gSor1ll59Tiles, 2, { 0, 0, 0 } },
    { gSor1ll61Frames, gSor1ll61Anims, gSor1ll61Tiles, 5, { 0, 0, 0 } },
    { gSor1ll61Frames, gSor1ll61Anims, gSor1ll61Tiles, 4, { 0, 0, 0 } },
    { gSor1ll61Frames, gSor1ll61Anims, gSor1ll61Tiles, 1, { 0, 0, 0 } },
    { gSor1ll61Frames, gSor1ll61Anims, gSor1ll61Tiles, 2, { 0, 0, 0 } },
    { gSor1ll61Frames, gSor1ll61Anims, gSor1ll61Tiles, 3, { 0, 0, 0 } },
    { gSor1ll60Frames, gSor1ll60Anims, gSor1ll60Tiles, 4, { 0, 0, 0 } },
    { gSor1ll60Frames, gSor1ll60Anims, gSor1ll60Tiles, 3, { 0, 0, 0 } },
    { gSor1ll60Frames, gSor1ll60Anims, gSor1ll60Tiles, 0, { 0, 0, 0 } },
    { gSor1ll60Frames, gSor1ll60Anims, gSor1ll60Tiles, 1, { 0, 0, 0 } },
    { gSor1ll60Frames, gSor1ll60Anims, gSor1ll60Tiles, 2, { 0, 0, 0 } },
    { gSor1ll65Frames, gSor1ll65Anims, gSor1ll65Tiles, 0, { 0, 0, 0 } },
    { gSor1ll50Frames, gSor1ll50Anims, gSor1ll50Tiles, 2, { 0, 0, 0 } },
    { gSor1ll50Frames, gSor1ll50Anims, gSor1ll50Tiles, 3, { 0, 0, 0 } },
    { gSor1ll50Frames, gSor1ll50Anims, gSor1ll50Tiles, 4, { 0, 0, 0 } },
    { gSor1ll50Frames, gSor1ll50Anims, gSor1ll50Tiles, 6, { 0, 0, 0 } },
    { gSor1ll08Frames, gSor1ll08Anims, gSor1ll08Tiles, 0, { 0, 0, 0 } },
    { gSor1ll09Frames, gSor1ll09Anims, gSor1ll09Tiles, 0, { 0, 0, 0 } },
    { gSor1ll09Frames, gSor1ll09Anims, gSor1ll09Tiles, 1, { 0, 0, 0 } },
    { gSor1ll09Frames, gSor1ll09Anims, gSor1ll09Tiles, 2, { 0, 0, 0 } },
    { gSor1ll09Frames, gSor1ll09Anims, gSor1ll09Tiles, 3, { 0, 0, 0 } },
    { gSor1ll54Frames, gSor1ll54Anims, gSor1ll54Tiles, 3, { 0, 0, 0 } },
    { gSor1ll54Frames, gSor1ll54Anims, gSor1ll54Tiles, 2, { 0, 0, 0 } },
    { gSor1ll54Frames, gSor1ll54Anims, gSor1ll54Tiles, 0, { 0, 0, 0 } },
    { gSor1ll54Frames, gSor1ll54Anims, gSor1ll54Tiles, 1, { 0, 0, 0 } },
    { gSor1ll66Frames, gSor1ll66Anims, gSor1ll66Tiles, 0, { 0, 0, 0 } },
    { gSor1ll07Frames, gSor1ll07Anims, gSor1ll07Tiles, 0, { 0, 0, 0 } },
    { gSor1ll67Frames, gSor1ll67Anims, gSor1ll67Tiles, 0, { 0, 0, 0 } },
    { gSor1ll68Frames, gSor1ll68Anims, gSor1ll68Tiles, 0, { 0, 0, 0 } },
    { gSor1ll68Frames, gSor1ll68Anims, gSor1ll68Tiles, 1, { 0, 0, 0 } },
    { gSor1ll68Frames, gSor1ll68Anims, gSor1ll68Tiles, 2, { 0, 0, 0 } },
    { gSor1ll68Frames, gSor1ll68Anims, gSor1ll68Tiles, 3, { 0, 0, 0 } },
    { gSor1ll67Frames, gSor1ll67Anims, gSor1ll67Tiles, 1, { 0, 0, 0 } },
    { gSor1ll67Frames, gSor1ll67Anims, gSor1ll67Tiles, 3, { 0, 0, 0 } },
    { gSor1ll69Frames, gSor1ll69Anims, gSor1ll69Tiles, 0, { 0, 0, 0 } },
    { gSor1ll69Frames, gSor1ll69Anims, gSor1ll69Tiles, 1, { 0, 0, 0 } },
    { gSor1ll69Frames, gSor1ll69Anims, gSor1ll69Tiles, 2, { 0, 0, 0 } },
    { gSor1ll69Frames, gSor1ll69Anims, gSor1ll69Tiles, 3, { 0, 0, 0 } },
    { gSor1ll70Frames, gSor1ll70Anims, gSor1ll70Tiles, 0, { 0, 0, 0 } },
    { gSor1ll70Frames, gSor1ll70Anims, gSor1ll70Tiles, 1, { 0, 0, 0 } },
    { gSor1ll70Frames, gSor1ll70Anims, gSor1ll70Tiles, 2, { 0, 0, 0 } },
    { gSor1ll70Frames, gSor1ll70Anims, gSor1ll70Tiles, 3, { 0, 0, 0 } },
    { gSor1ll71Frames, gSor1ll71Anims, gSor1ll71Tiles, 0, { 0, 0, 0 } },
    { gSor1ll72Frames, gSor1ll72Anims, gSor1ll72Tiles, 0, { 0, 0, 0 } },
    { gSor1ll73Frames, gSor1ll73Anims, gSor1ll73Tiles, 0, { 0, 0, 0 } },
    { gSor1ll74Frames, gSor1ll74Anims, gSor1ll74Tiles, 0, { 0, 0, 0 } },
    { gSor1ll74Frames, gSor1ll74Anims, gSor1ll74Tiles, 1, { 0, 0, 0 } },
    { gSor1ll74Frames, gSor1ll74Anims, gSor1ll74Tiles, 2, { 0, 0, 0 } },
    { gSor1ll75Frames, gSor1ll75Anims, gSor1ll75Tiles, 0, { 0, 0, 0 } },
    { gSor1ll75Frames, gSor1ll75Anims, gSor1ll75Tiles, 1, { 0, 0, 0 } },
    { gSor1ll75Frames, gSor1ll75Anims, gSor1ll75Tiles, 2, { 0, 0, 0 } },
};

static const AnimDef sBtlSoraDirAnimDefs[6][5] = {
    { { gSor1ff02Frames, gSor1ff02Anims, gSor1ff02Tiles, 0, { 0, 0, 0 } }, { gSor1bb02Frames, gSor1bb02Anims, gSor1bb02Tiles, 0, { 0, 0, 0 } }, { gSor1fl02Frames, gSor1fl02Anims, gSor1fl02Tiles, 0, { 0, 0, 0 } }, { gSor1ll02Frames, gSor1ll02Anims, gSor1ll02Tiles, 0, { 0, 0, 0 } }, { gSor1bl02Frames, gSor1bl02Anims, gSor1bl02Tiles, 0, { 0, 0, 0 } } },
    { { gSor1ff03Frames, gSor1ff03Anims, gSor1ff03Tiles, 0, { 0, 0, 0 } }, { gSor1bb03Frames, gSor1bb03Anims, gSor1bb03Tiles, 0, { 0, 0, 0 } }, { gSor1fl03Frames, gSor1fl03Anims, gSor1fl03Tiles, 0, { 0, 0, 0 } }, { gSor1ll03Frames, gSor1ll03Anims, gSor1ll03Tiles, 0, { 0, 0, 0 } }, { gSor1bl03Frames, gSor1bl03Anims, gSor1bl03Tiles, 0, { 0, 0, 0 } } },
    { { gSor1ff03Frames, gSor1ff03Anims, gSor1ff03Tiles, 1, { 0, 0, 0 } }, { gSor1bb03Frames, gSor1bb03Anims, gSor1bb03Tiles, 1, { 0, 0, 0 } }, { gSor1fl03Frames, gSor1fl03Anims, gSor1fl03Tiles, 1, { 0, 0, 0 } }, { gSor1ll03Frames, gSor1ll03Anims, gSor1ll03Tiles, 1, { 0, 0, 0 } }, { gSor1bl03Frames, gSor1bl03Anims, gSor1bl03Tiles, 1, { 0, 0, 0 } } },
    { { gSor1ff03Frames, gSor1ff03Anims, gSor1ff03Tiles, 2, { 0, 0, 0 } }, { gSor1bb03Frames, gSor1bb03Anims, gSor1bb03Tiles, 2, { 0, 0, 0 } }, { gSor1fl03Frames, gSor1fl03Anims, gSor1fl03Tiles, 2, { 0, 0, 0 } }, { gSor1ll03Frames, gSor1ll03Anims, gSor1ll03Tiles, 2, { 0, 0, 0 } }, { gSor1bl03Frames, gSor1bl03Anims, gSor1bl03Tiles, 2, { 0, 0, 0 } } },
    { { gSor1ff03Frames, gSor1ff03Anims, gSor1ff03Tiles, 3, { 0, 0, 0 } }, { gSor1bb03Frames, gSor1bb03Anims, gSor1bb03Tiles, 3, { 0, 0, 0 } }, { gSor1fl03Frames, gSor1fl03Anims, gSor1fl03Tiles, 3, { 0, 0, 0 } }, { gSor1ll03Frames, gSor1ll03Anims, gSor1ll03Tiles, 3, { 0, 0, 0 } }, { gSor1bl03Frames, gSor1bl03Anims, gSor1bl03Tiles, 3, { 0, 0, 0 } } },
    { { gSor1ff03Frames, gSor1ff03Anims, gSor1ff03Tiles, 4, { 0, 0, 0 } }, { gSor1bb03Frames, gSor1bb03Anims, gSor1bb03Tiles, 4, { 0, 0, 0 } }, { gSor1fl03Frames, gSor1fl03Anims, gSor1fl03Tiles, 4, { 0, 0, 0 } }, { gSor1ll03Frames, gSor1ll03Anims, gSor1ll03Tiles, 4, { 0, 0, 0 } }, { gSor1bl03Frames, gSor1bl03Anims, gSor1bl03Tiles, 4, { 0, 0, 0 } } },
};

static const u16 sBtlSoraGroundSongs[4][4] = {
    { SONG_BTL_SR_FOOTL, SONG_BTL_SR_FOOTR, SONG_BTL_SR_JUMP, SONG_BTL_SR_LAND },
    { SONG_BTL_SR_STONEL, SONG_BTL_SR_STONER, SONG_BTL_SR_STONEJP, SONG_BTL_SR_STONELD },
    { SONG_BTL_SR_MUDL, SONG_BTL_SR_MUDR, SONG_BTL_SR_MUDJP, SONG_BTL_SR_MUDLD },
    { SONG_SYS_SR_STONEL, SONG_SYS_SR_STONER, SONG_SYS_SR_STONEJP, SONG_SYS_SR_STONELD },
};

static const s32 sUnk_0813C0FC[18] = {
    12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57, 60, 63,
};

static const s32 sUnk_0813C144[18] = {
    13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 52, 55, 58, 61, 64,
};

static const s32 sUnk_0813C18C[18] = {
    14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44, 47, 50, 53, 56, 59, 62, 65,
};

static const SoraAttackDef sUnk_0813C1D4 = { 2, sUnk_0813C0FC, SONG_VO_SR_ATTACK01, SONG_BTL_SR_ATT00, 0, 0, 0, NULL };

static const SoraAttackDef sUnk_0813C1EC = { 7, sUnk_0813C144, SONG_VO_SR_ATTACK03, SONG_BTL_SR_ATT01, 0, 0, 0, NULL };

static const SoraAttackDef sUnk_0813C204 = { 12, sUnk_0813C0FC, SONG_VO_SR_ATTACK01, SONG_BTL_SR_ATT00, 0, 0, 0, NULL };

static const SoraAttackDef sUnk_0813C21C = { 17, sUnk_0813C18C, SONG_VO_SR_ATTACK05, SONG_BTL_SR_ATT02, 0, COMBO_FLAG_ZOOM_ON_HIT, 0, NULL };

static const SoraAttackDef sUnk_0813C234 = { 27, sUnk_0813C0FC, SONG_VO_SR_ATTACK02, SONG_BTL_SR_ATT01, -640, COMBO_FLAG_AERIAL_SWING, 0, &sUnk_0813C1D4 };

static const SoraAttackDef sUnk_0813C24C = { 22, sUnk_0813C144, SONG_VO_SR_ATTACK00, SONG_BTL_SR_ATT00, 0, COMBO_FLAG_AERIAL_SWING, 0, &sUnk_0813C1D4 };

static const SoraAttackDef sUnk_0813C264 = { 27, sUnk_0813C0FC, SONG_VO_SR_ATTACK02, SONG_BTL_SR_ATT01, 0, COMBO_FLAG_AERIAL_SWING, 0, &sUnk_0813C1EC };

static const SoraAttackDef sUnk_0813C27C = { 32, sUnk_0813C18C, SONG_VO_SR_ATTACK05, SONG_BTL_SR_ATT02, 0, COMBO_FLAG_AERIAL_SWING | COMBO_FLAG_ZOOM_ON_HIT, 0, &sUnk_0813C21C };

static const u8 sBtlSoraSwingHitFrames[5] = {
    10, 12, 15, 18, 20,
};

TaskDesc gTaskDescBtlSora = {
    "task_btl_sora",
    (TaskInitFunc)task_btl_sora_0,
    (TaskUpdateFunc)task_btl_sora_1,
    (TaskDrawFunc)task_btl_sora_2,
    (TaskDestroyFunc)task_btl_sora_3,
    sizeof(BtlSoraWork),
};

void EnableBtlSoraPassThrough(BtlSoraWork* work) {
    u16 a = work->flags | BTL_SORA_FLAG_PASS_THROUGH;
    u16 b;

    work->flags = a;
    b = work->actor.collider.flags | COLLIDER_FLAG_PASS_THROUGH;
    work->actor.collider.flags = b;
}

void DisableBtlSoraPassThrough(BtlSoraWork* work) {
    u16 a = work->flags & ~BTL_SORA_FLAG_PASS_THROUGH;
    u16 b;

    work->flags = a;
    b = work->actor.collider.flags & ~COLLIDER_FLAG_PASS_THROUGH;
    work->actor.collider.flags = b;
}

u16 GetBtlSoraComboType(BtlSoraWork* work) {
    BtlObj* a;
    BtlObj* b;
    s32 d;

    a = work->actor.btl->actor;
    b = work->actor.btl->actor2;

    if (work->actor.btl->flags & BTL_FLAG_PLAYER_AIRBORNE) {
        return 3;
    }

    if (b == NULL) {
        return 0;
    }

    if (a->z - b->z > 0x2000) {
        return 2;
    }

    d = b->x - a->x;

    if (d >= 0 ? d > 0x2800 : a->x - b->x > 0x2800) {
        return 1;
    }

    d = b->y - a->y;

    if (d >= 0 ? d > 0xC00 : a->y - b->y > 0xC00) {
        return 4;
    }

    return 0;
}

void FocusBtlSoraCameraOnTarget(BtlSoraWork* work) {
    BtlObj* c;
    s32 x;
    s32 y;
    s32 z;

    if (work->mainSide == 0) {
        return;
    }

    c = work->actor.btl->actor2;

    if (c != NULL) {
        if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
            x = (work->actor.x + c->x) >> 1;
            x = (work->actor.x + x) >> 1;
            y = work->actor.y;
            z = work->actor.z;

            if (z < -0x3200) {
                z = -0x3200;
            }
        } else {
            x = (work->actor.x + c->x) >> 1;
            y = (work->actor.y + c->y) >> 1;
            z = (work->actor.z + c->z) >> 1;
        }

        BtlMapFollowPosition(x, y, z);
    } else {
        BtlMapFollowPosition(work->actor.x, work->actor.y, work->actor.z);
    }
}

void FocusBtlSoraCameraOnBgFx(BtlSoraWork* work) {
    s32 x;
    s32 y;
    s32 z;

    if (work->mainSide != 0) {
        BgFxGetPosition(&x, &y, &z);
        BtlMapFollowPosition(x, gBtlWork->actor->y, gBtlWork->actor->z);
    }
}

void FocusBtlSoraCamera(BtlSoraWork* work) {
    if (work->mainSide != 0) {
        BtlMapFollowPosition(work->actor.x, work->actor.y, work->actor.z);
    }
}

void SetBtlSoraAnimation(BtlSoraWork* work, u16 a, u16 b) {
    const FldAnimDef* e;

    e = &sBtlSoraAnimDefs[a];
    AnimChangeWithTables(&work->anim, e->animId, b, e->anims, e->gfxTable);
    SetObjTileSource(work->tiles, e->tiles);
}

void SetBtlSoraDirAnimation(BtlSoraWork* work, u16 a, u16 b) {
    const FldAnimDef* e;
    s32 idx;

    idx = 0;

    switch (((work->angle + 16) & 0xFF) >> 5) {
    case 0:
        idx = 1;
        break;
    case 4:
        idx = 0;
        break;
    case 3:
    case 5:
        idx = 2;
        break;
    case 2:
    case 6:
        idx = 3;
        break;
    case 1:
    case 7:
        idx = 4;
        break;
    }

    e = &sBtlSoraDirAnimDefs[a][idx];
    AnimChangeWithTables(&work->anim, e->animId, b, e->anims, e->gfxTable);
    SetObjTileSource(work->tiles, e->tiles);
}

void LoadBtlSoraPalette(BtlSoraWork* work) {
    work->tiles = work->actor.btl->tiles;

    if (work->mainSide != 0) {
        work->palette = LoadObjPalette(gSoraPalette, 0x20);
    } else {
        work->palette = LoadObjPalette(gUnk_096FAC64, 0x20);
    }
}

void ReleaseBtlSoraPalette(BtlSoraWork* work) {
    if (work->palette != NULL) {
        ReleaseObjPalette(work->palette);
    }

    work->tiles = NULL;
    work->palette = NULL;
}

void UpdateBtlSoraWalk(BtlSoraWork* work, u16 a) {
    BtlObj* p;

    p = &work->actor;

    if ((a & 0x10) && (a & 0x40)) {
        work->angle = 0x20;
        p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
    } else if ((a & 0x10) && (a & 0x80)) {
        work->angle = 0x60;
        p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
    } else if ((a & 0x20) && (a & 0x80)) {
        work->angle = 0xA0;
        p->flags |= BTLOBJ_FLAG_FACING_LEFT;
    } else if ((a & 0x20) && (a & 0x40)) {
        work->angle = 0xE0;
        p->flags |= BTLOBJ_FLAG_FACING_LEFT;
    } else if (a & 0x40) {
        work->angle = 0;
    } else if (a & 0x10) {
        work->angle = 0x40;
        p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
    } else if (a & 0x80) {
        work->angle = 0x80;
    } else if (a & 0x20) {
        work->angle = 0xC0;
        p->flags |= BTLOBJ_FLAG_FACING_LEFT;
    }

    if (a & 0xF0) {
        SetBtlSoraDirAnimation(work, 0, 1);

        if (work->anim.timer == 0) {
            switch (work->anim.frame) {
            case 3:
                m4aSongNumStart(work->groundSongs[0]);
                break;
            case 7:
                m4aSongNumStart(work->groundSongs[1]);
                break;
            }
        }
    } else {
        SetBtlSoraAnimation(work, 1, 1);
    }

    if (a & 0xF0) {
        if (p->btl->hcEffect == 50) {
            work->speed += 256;

            if (work->speed > 1024) {
                work->speed = 1024;
            }
        } else {
            work->speed += 128;

            if (work->speed > 614) {
                work->speed = 614;
            }
        }
    } else {
        work->speed -= 128;

        if (work->speed < 0) {
            work->speed = 0;
        }
    }
}

void task_btl_sora_0(BtlSoraWork* work, BtlTaskArg* arg) {
    BtlObj* e;

    e = &work->actor;
    work->flags = 0;

    if (arg != NULL) {
        if (arg->side == 0) {
            e->x = 0xC000;
            e->flags = 0;
            work->sioKeysA = 1;
        } else {
            e->x = 0x14000;
            e->flags = BTLOBJ_FLAG_FACING_LEFT;
            work->sioKeysA = 0;
        }

        if (arg->mainSide != 0) {
            work->mainSide = 1;
            e->btl = gBtlWork;
            e->maxHp = gGameState.linkMaxHp;
            e->hp = gGameState.linkMaxHp;
            e->attack = gGameState.linkAp;
        } else {
            work->mainSide = 0;
            e->btl = gRikuBtlWork;
            e->maxHp = gGameState.linkPartnerMaxHp;
            e->hp = gGameState.linkPartnerMaxHp;
            e->attack = gGameState.linkPartnerAp;
        }
    } else {
        work->mainSide = 1;
        work->sioKeysA = 1;
        e->btl = gBtlWork;

        if ((e->btl->flags & BTL_FLAG_HUM_BATTLE) || (e->btl->flags & BTL_FLAG_TUTORIAL)) {
            e->x = 0xC000;
        } else {
            e->x = 0x10000;
        }

        e->flags = 0;
        e->attack = gGameState.progression.ap;
        e->maxHp = gGameState.progression.maxHp;
        e->hp = gGameState.hp;

        if (e->hp > e->maxHp) {
            e->hp = e->maxHp;
        }
    }

    e->flags |= BTLOBJ_FLAG_IMMUNE_TERROR;
    e->flags |= BTLOBJ_FLAG_PLAYER;
    e->flags |= (BTLOBJ_FLAG_INTANGIBLE | BTLOBJ_FLAG_CARD_USE_BLOCKED);

    if (gBtlWork->flags & BTL_FLAG_BOSS_BATTLE) {
        e->y = 0x16000;
    } else {
        e->y = 0x18100;
    }

    e->invincibleTimer = 0;
    e->delayedDamage = 0;
    e->z = 0;
    e->groundZ = 0;
    e->damage = 0;
    e->height = 32;
    e->radiusX = 12;
    e->radiusY = 6;
    e->centerHeight = 12;
    e->kind = 55;
    e->badStatusTimer = 0;
    e->floorZ = 0;
    e->parent = NULL;
    e->badStatus = BAD_STATUS_NONE;
    e->popCooldown = 0;
    e->vx = e->vy = 0;

    // @bug arg is NULL in normal battles (NULL read).
#ifdef PLATFORM_ANDROID
    /* GBA BIOS/open-bus data makes this path act non-zero on a NULL arg. */
    if (arg == NULL || arg->mainSide != 0) {
#else
    if (arg->mainSide != 0) {
#endif
        ColliderInit(&e->collider, 1, e->radiusX, e->height);
    } else {
        ColliderInit(&e->collider, 2, e->radiusX, e->height);
    }

    gBtlWork->targetX = e->x;
    gBtlWork->targetY = e->y;
    gBtlWork->targetZ = e->z;
    LoadBtlSoraPalette(work);
    e->btl->actor = e;
    AnimInit(&work->anim, NULL, NULL);
    SetBtlSoraAnimation(work, 1, 1);
    work->gfx = AnimGetGfx(&work->anim);
    work->state = 0;
    work->nextState = 0;
    work->vz = 0;
    e->vx = 0;
    e->vy = 0;
    work->stateTimer = 0;
    work->steps = 0;
    work->speed = 0;
    work->angle = 0;
    work->comboCount = 0;
    work->tapTimers[0] = 0;
    work->tapTimers[1] = 0;
    work->task = NULL;
    work->unk_191[0] = 2;
    work->scaleX = work->scaleY = 0x100;
    work->frameCount = 0;

    if (gBtlWork->flags & (BTL_FLAG_BOSS_BATTLE | BTL_FLAG_HUM_BATTLE)) {
        switch (gBtlWork->battleId) {
        case 148:
        case 150:
        case 155:
        case 161:
        case 162:
        case 163:
        case 164:
        case 165:
        case 167:
        case 168:
        case 169:
        case 170:
        case 171:
        case 172:
        case 173:
        case 174:
            work->groundSongs = sBtlSoraGroundSongs[1];
            break;
        case 152:
            work->groundSongs = sBtlSoraGroundSongs[2];
            break;
        case 158:
            work->groundSongs = sBtlSoraGroundSongs[3];
            break;
        default:
            work->groundSongs = sBtlSoraGroundSongs[0];
            break;
        }
    } else {
        switch (gGameState.battleStage) {
        case BATTLE_STAGE_WONDERLAND:
        case 2:
            work->groundSongs = sBtlSoraGroundSongs[0];
            break;
        case BATTLE_STAGE_AGRABAH:
        case BATTLE_STAGE_OLYMPUS_COLISEUM:
        case BATTLE_STAGE_HALLOWEEN_TOWN:
            work->groundSongs = sBtlSoraGroundSongs[1];
            break;
        case BATTLE_STAGE_ATLANTICA:
        case BATTLE_STAGE_MONSTRO:
            work->groundSongs = sBtlSoraGroundSongs[2];
            break;
        default:
            work->groundSongs = sBtlSoraGroundSongs[0];
            break;
        }
    }

    TaskPoolInit(&work->tasks, 7);
    TaskCreate(&work->tasks, &gTaskDescBtlShadow, e);
    TaskCreate(&work->tasks, &gTaskDescBtlBadstatus, e);
}

void SetBtlSoraState(BtlSoraWork* work, u32 a) {
    work->state = a;
    work->steps = 0;
    work->stateTimer = 0;
    ClearBtlObjActionFlags(&work->actor);
}

void SetBtlSoraStateNoReset(BtlSoraWork* work, u32 a) {
    work->state = a;
    ClearBtlObjActionFlags(&work->actor);
}

void StartBtlSoraCombo(BtlSoraWork* work) {
    u16 t;

    if (work->state == 16 && work->comboCount <= 1) {
        work->comboCount++;
        work->stateTimer = 0;
        work->steps = 0;
    } else {
        switch (GetBtlSoraComboType(work)) {
        case 0:
            work->attacks[0] = &sUnk_0813C1D4;
            work->attacks[1] = &sUnk_0813C1EC;
            work->attacks[2] = &sUnk_0813C21C;
            break;
        case 1:
            work->attacks[0] = &sUnk_0813C1EC;
            work->attacks[1] = &sUnk_0813C1D4;
            work->attacks[2] = &sUnk_0813C21C;
            break;
        case 2:
            work->attacks[0] = &sUnk_0813C24C;
            work->attacks[1] = &sUnk_0813C264;
            work->attacks[2] = &sUnk_0813C27C;
            break;
        case 3:
            work->attacks[0] = &sUnk_0813C234;
            work->attacks[1] = &sUnk_0813C24C;
            work->attacks[2] = &sUnk_0813C27C;
            break;
        case 4:
        default:
            work->attacks[0] = &sUnk_0813C204;
            work->attacks[1] = &sUnk_0813C1EC;
            work->attacks[2] = &sUnk_0813C21C;
            break;
        }

        work->state = 16;
        work->steps = 0;
        work->stateTimer = 0;
        work->comboCount = 0;
        t = work->flags & ~BTL_SORA_FLAG_COMBO_EXTENDED;
        work->flags = t;
    }

    if (work->actor.btl->hcEffect == 44) {
        work->swingSpeed = 0;
    }
}

void StartBtlSoraKnockback(BtlSoraWork* work) {
    work->vz = -work->actor.knockbackLift * 3;
    work->actor.vx = ((gSineTable[work->actor.angle] << 1) * work->actor.knockbackSpeed) >> 8;
    work->actor.vy = ((-gSineTable[work->actor.angle + 0x40] << 1) * work->actor.knockbackSpeed) >> 8;
}

BtlObj* PickBtlSoraTarget(BtlSoraWork* work) {
    BtlObj* list[10];
    BtlObj* e;
    s16 n;

    if (work->actor.btl->actor2 != NULL) {
        return work->actor.btl->actor2;
    }

    if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
        if (work->mainSide != 0) {
            e = gRikuBtlWork->actor;
        } else {
            e = gBtlWork->actor;
        }

        if (e->hp <= 0) {
            return NULL;
        }

        return e;
    }

    n = 0;
    e = ListPoolFirst(&gBtlWork->pool);

    if (e != NULL) {
        list[0] = e;
        n = 1;

        do {
            e = ListPoolNext(&e->node);

            if (e == NULL) {
                break;
            }

            list[n] = e;
            n++;
        } while (n <= 9);
    }

    if (n == 0) {
        return NULL;
    }

    e = list[GetRandom() % n];
    return e;
}

u16 SwapBtlSoraKeyBits(u16 a, u16 b, u16 c) {
    u16 d;

    d = b;

    if (a & b) {
        if ((a & c) == 0) {
            a &= ~b;
        }

        a |= c;
    } else if (a & c) {
        a &= ~c;
        a |= d;
    }

    return a;
}

BtlObj* GetBtlSoraActiveOpponent(BtlSoraWork* work) {
    if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
        if (work->mainSide != 0) {
            if (gBtlWork->flags & BTL_FLAG_OPPONENT_CARD_ACTION) {
                return gRikuBtlWork->actor;
            }
        } else {
            if (gBtlWork->flags & BTL_FLAG_PLAYER_CARD_ACTION) {
                return gBtlWork->actor;
            }
        }
    } else {
        if (gBtlWork->flags & BTL_FLAG_OPPONENT_CARD_ACTION) {
            return gBtlWork->actor3;
        }
    }

    return NULL;
}

s32 task_btl_sora_1(BtlSoraWork* work) {
    BtlObj* p;
    BtlObj* e;
    s16 n;
    u16 held;
    u16 pressed;
    u64 f;
    s16 st;
    u16 uv;
    s32 t3;
    s32 d;
    s32 t4;
    u32 id;
    const SoraAttackDef* a;
    s32 sel[6];
    u8 buf[5];
    BtlObj* e3;
    BtlTaskArgs args;
    BtlSpawnArgs spawn;
    BtlSpawnArgs spawn2;

    p = &work->actor;

    if (gBtlWork->phase == 4 && (p->flags & BTLOBJ_FLAG_IN_CARD_ACTION)) {
        switch (work->state) {
        case 23:
        case 24:
        case 25:
        case 26:
            p->x = p->originX;
            p->y = p->originY;
            p->z = p->originZ;

            if (work->flags & BTL_SORA_FLAG_HIDDEN) {
                work->flags &= ~BTL_SORA_FLAG_HIDDEN;
                LoadBtlSoraPalette(work);
            }

            SetBtlSoraState(work, 3);
            break;
        case 67:
            m4aSongNumStop(SONG_EF_DAMBO_SPLOOP);
            SetBtlSoraState(work, 82);
            break;
        case 79:
            m4aSongNumStop(SONG_EF_TRUNEDO);
            SetBtlSoraState(work, 82);
            break;
        default:
            SetBtlSoraState(work, 82);
            break;
        }

        ColliderSetDisabled(&p->collider, 0);
        DisableBtlSoraPassThrough(work);
        p->flags &= ~0x0000200400800000LL;
        gBtlWork->flags |= BTL_FLAG_STOP_BGFX;
        p->btl->flags |= BTL_FLAG_DISMISS_SUMMONS;
    }

    if (CanLevelUp() != 0) {
        if (LevelUp() != 0) {
            CreateLevelUpEffectTask(p, &work->tasks);
        }
    }

    if (work->flags & BTL_SORA_FLAG_HC_STATUS) {
        work->flags &= ~BTL_SORA_FLAG_HC_STATUS;
        p->flags &= ~BTLOBJ_FLAGS_ELEMENT_AFFINITY;
    }

    switch (p->btl->hcEffect) {
    case 26:
        work->flags |= BTL_SORA_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_WEAK_FIRE | BTLOBJ_FLAG_RESIST_THUNDER);
        break;
    case 8:
        work->flags |= BTL_SORA_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_WEAK_BLIZZARD | BTLOBJ_FLAG_RESIST_FIRE);
        break;
    case 18:
        work->flags |= BTL_SORA_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_IMMUNE_FIRE | BTLOBJ_FLAG_WEAK_BLIZZARD);
        break;
    case 50:
        work->flags |= BTL_SORA_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_IMMUNE_THUNDER | BTLOBJ_FLAG_WEAK_NEUTRAL);
        break;
    case 27:
        work->flags |= BTL_SORA_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_IMMUNE_BLIZZARD | BTLOBJ_FLAG_WEAK_FIRE);
        break;
    case 47:
        work->flags |= BTL_SORA_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_WEAK_PHYSICAL | BTLOBJ_FLAG_RESIST_FIRE | BTLOBJ_FLAG_RESIST_BLIZZARD | BTLOBJ_FLAG_RESIST_THUNDER | BTLOBJ_FLAG_RESIST_NEUTRAL);
        break;
    case 49:
        work->flags |= BTL_SORA_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_IMMUNE_BLIZZARD | BTLOBJ_FLAG_RESIST_PHYSICAL | BTLOBJ_FLAG_WEAK_NEUTRAL);
        break;
    case 15:
    case 28:
        work->flags |= BTL_SORA_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_RESIST_FIRE | BTLOBJ_FLAG_RESIST_BLIZZARD | BTLOBJ_FLAG_RESIST_THUNDER);
        break;
    }

    p->flags &= ~BTLOBJ_FLAG_GUARD_PHYSICAL;

    switch ((u32)p->btl->hcEffect) {
    case 51:
        e = GetBtlSoraActiveOpponent(work);

        if (e != NULL) {
            if ((e->x < p->x && (p->flags & BTLOBJ_FLAG_FACING_LEFT)) ||
                (e->x > p->x && !(p->flags & BTLOBJ_FLAG_FACING_LEFT))) {
                p->flags |= BTLOBJ_FLAG_GUARD_PHYSICAL;
            }
        }

        break;
    case 23: {
        u16 hp;
        s16 max;
        s32 t;
        hp = p->hp;

        if ((s16)hp > 0) {
            max = p->maxHp;

            if ((s16)hp < max && work->frameCount % 120 == 0) {
                n = (p->maxHp - p->hp) << 13 >> 16;

                if (n <= 0) {
                    n = 1;
                }

                if (p->badStatus != BAD_STATUS_STOP) {
                    t = n + hp;
                    p->hp = t;
                }

                if (p->hp > max) {
                    p->hp = max;
                }

                p->btl->hcEffectCount--;
            }
        }

        break;
    }
    case 24: {
        BtlObj* e;
        u16 hp;

        if (work->frameCount % 20 == 0) {
            if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
                if (work->mainSide != 0) {
                    e = gRikuBtlWork->actor;
                } else {
                    e = gBtlWork->actor;
                }

                hp = e->hp;

                if ((s16)hp > 1 && e->badStatus != BAD_STATUS_STOP) {
                    e->hp = hp - 1;
                }
            } else {
                e = ListPoolFirst(&gBtlWork->pool);

                while (e != NULL) {
                    hp = e->hp;

                    if ((s16)hp > 1 && e->badStatus != BAD_STATUS_STOP) {
                        e->hp = hp - 1;
                    }

                    e = ListPoolNext(&e->node);
                }
            }
        }

        break;
    }
    }

    if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
        if (work->sioKeysA != 0) {
            held = SioKeyGetHeldA();
            pressed = SioKeyGetPressedA();
        } else {
            held = SioKeyGetHeldB();
            pressed = SioKeyGetPressedB();
        }
    } else {
        held = GetKeysHeld();
        pressed = GetKeysPressed();
    }

    f = gBtlWork->flags;

    if (f & BTL_FLAG_TUTORIAL) {
        if (f & BTL_FLAG_TUTORIAL_NO_CONTROL) {
            held &= ~A_BUTTON;
            held &= ~B_BUTTON;
            held &= ~DPAD_UP;
            held &= ~DPAD_DOWN;
            held &= ~DPAD_LEFT;
            held &= ~DPAD_RIGHT;
            pressed &= ~A_BUTTON;
            pressed &= ~B_BUTTON;
            pressed &= ~DPAD_UP;
            pressed &= ~DPAD_DOWN;
            pressed &= ~DPAD_LEFT;
            pressed &= ~DPAD_RIGHT;
        }

        if (f & BTL_FLAG_TUTORIAL_NO_JUMP) {
            pressed &= ~B_BUTTON;
        }

        if (f & BTL_FLAG_TUTORIAL_NO_DODGE) {
            pressed &= ~DPAD_LEFT;
            pressed &= ~DPAD_RIGHT;
        }

        if (f & BTL_FLAG_TUTORIAL_NO_CARD_USE) {
            pressed &= ~A_BUTTON;
            held &= ~A_BUTTON;
        }
    }

    if (p->badStatus == BAD_STATUS_CONFUSE) {
        held = SwapBtlSoraKeyBits(held, 32, 16);
        held = SwapBtlSoraKeyBits(held, 64, 128);
        pressed = SwapBtlSoraKeyBits(pressed, 32, 16);
        pressed = SwapBtlSoraKeyBits(pressed, 64, 128);
    }

    if (work->state != 15 && (p->btl->flags & BTL_FLAG_ESCAPED)) {
        p->flags |= BTLOBJ_FLAG_IGNORE_BOUNDS;
        work->state = 15;
        work->steps = 0;
        work->stateTimer = 0;
        p->flags |= BTLOBJ_FLAG_INTANGIBLE;
    } else {
        p->btl->flags &= ~BTL_FLAG_CAN_CHARGE_RELOAD;
    }

    switch (UpdateBtlObjReaction(p)) {
    case BTL_REACTION_GRAVITY:
        work->speed = 0;
        work->state = 87;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case BTL_REACTION_HURT:
        work->speed = 0;
        work->state = 13;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case BTL_REACTION_DEFEATED:
    case BTL_REACTION_GRAVITY_DEFEATED:
        work->speed = 0;

        if (p->btl->hcEffect == 27) {
            work->state = 84;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->state = 14;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case BTL_REACTION_HEALED:
        p->flags &= ~BTLOBJ_FLAG_HURT;
        work->state = 1;
        work->steps = 0;
        work->stateTimer = 0;
        p->flags &= ~BTLOBJ_FLAG_CARD_ACTION_PENDING;
        break;
    case BTL_REACTION_CARD_ACTION:
        p->btl->flags &= ~BTL_FLAG_DISMISS_SUMMONS;
        p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
        work->flags |= BTL_SORA_FLAG_PASS_THROUGH;

        if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
            if (work->mainSide != 0) {
                id = ResolveLinkActiveCardsMove(sel, 0);
            } else {
                id = ResolveLinkActiveCardsMove(sel, 1);
            }
        } else {
            id = ResolveActiveCardsMove(sel);
        }

        if (id == 145) {
            if (!(p->btl->flags & BTL_FLAG_STOCK_SEQUENCE)) {
                p->btl->flags |= BTL_FLAG_STOCK_SEQUENCE;
                p->btl->stockMove = 0;
            }

            id = sel[p->btl->stockMove];
            p->btl->stockMove++;
        }

        work->unk_191[0] = 2;

        switch (id) {
        case 0:
            work->unk_191[1] = 0;
            work->swingSpeed = 2;
            work->unk_191[0] = 2;
            StartBtlSoraCombo(work);
            break;
        case 1:
            work->unk_191[1] = 1;
            work->swingSpeed = 0;
            work->unk_191[0] = 3;
            StartBtlSoraCombo(work);
            break;
        case 2:
            work->unk_191[1] = 2;
            work->swingSpeed = 3;
            work->unk_191[0] = 1;
            StartBtlSoraCombo(work);
            break;
        case 3:
            work->unk_191[1] = 3;
            work->swingSpeed = 1;
            work->unk_191[0] = 2;
            StartBtlSoraCombo(work);
            break;
        case 4:
            work->unk_191[1] = 4;
            work->swingSpeed = 2;
            work->unk_191[0] = 0;
            StartBtlSoraCombo(work);
            break;
        case 5:
            work->unk_191[1] = 5;
            work->swingSpeed = 2;
            work->unk_191[0] = 1;
            StartBtlSoraCombo(work);
            break;
        case 6:
            work->unk_191[1] = 6;
            work->swingSpeed = 0;
            work->unk_191[0] = 2;
            StartBtlSoraCombo(work);
            break;
        case 7:
            work->unk_191[1] = 7;
            work->swingSpeed = 1;
            work->unk_191[0] = 1;
            StartBtlSoraCombo(work);
            break;
        case 8:
            work->unk_191[1] = 8;
            work->swingSpeed = 3;
            work->unk_191[0] = 1;
            StartBtlSoraCombo(work);
            break;
        case 9:
            work->unk_191[1] = 9;
            work->swingSpeed = 3;
            work->unk_191[0] = 2;
            StartBtlSoraCombo(work);
            break;
        case 10:
            work->unk_191[1] = 10;
            work->swingSpeed = 4;
            work->unk_191[0] = 1;
            StartBtlSoraCombo(work);
            break;
        case 11:
            work->unk_191[1] = 11;
            work->swingSpeed = 1;
            work->unk_191[0] = 2;
            StartBtlSoraCombo(work);
            break;
        case 12:
            work->unk_191[1] = 12;
            work->swingSpeed = 1;
            work->unk_191[0] = 3;
            StartBtlSoraCombo(work);
            break;
        case 13:
            work->unk_191[1] = 13;
            work->swingSpeed = 2;
            work->unk_191[0] = 2;
            StartBtlSoraCombo(work);
            break;
        case 14:
            work->unk_191[1] = 14;
            work->swingSpeed = 3;
            work->unk_191[0] = 1;
            StartBtlSoraCombo(work);
            break;
        case 15:
            work->unk_191[1] = 15;
            work->swingSpeed = 2;
            work->unk_191[0] = 2;
            StartBtlSoraCombo(work);
            break;
        case 16:
            work->unk_191[1] = 16;
            work->swingSpeed = 0;
            work->unk_191[0] = 0;
            StartBtlSoraCombo(work);
            break;
        case 17:
            work->unk_191[1] = 17;
            work->swingSpeed = 1;
            work->unk_191[0] = 3;
            StartBtlSoraCombo(work);
            break;
        case 19:
            work->state = 6;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 0;
            work->unk_191[0] = 1;
            break;
        case 0x8002ACAB:
            work->state = 6;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 1;
            work->unk_191[0] = 2;
            break;
        case 0xCAB2ACAB:
            work->state = 6;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 2;
            work->unk_191[0] = 3;
            break;
        case 20:
            work->state = 7;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 0;
            work->unk_191[0] = 1;
            break;
        case 0x8002D4B5:
            work->state = 7;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 1;
            work->unk_191[0] = 2;
            break;
        case 0xCB52D4B5:
            work->state = 7;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 2;
            work->unk_191[0] = 3;
            break;
        case 21:
            work->state = 8;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 0;
            work->unk_191[0] = 1;
            break;
        case 0x8002FCBF:
            work->state = 8;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 1;
            work->unk_191[0] = 2;
            break;
        case 0xCBF2FCBF:
            work->state = 8;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 2;
            work->unk_191[0] = 3;
            break;
        case 22:
            work->state = 10;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 0;
            work->unk_191[0] = 0;
            break;
        case 0x800324C9:
            work->state = 10;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 1;
            work->unk_191[0] = 0;
            break;
        case 0xCC9324C9:
            work->state = 10;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 2;
            work->unk_191[0] = 0;
            break;
        case 24:
            work->state = 11;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 0;
            work->unk_191[0] = 1;
            break;
        case 0x800374DD:
            work->state = 11;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 1;
            work->unk_191[0] = 2;
            break;
        case 0xCDD374DD:
            work->state = 11;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 2;
            work->unk_191[0] = 3;
            break;
        case 23:
            work->state = 9;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 0;
            work->unk_191[0] = 1;
            break;
        case 0x80034CD3:
            work->state = 9;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 1;
            work->unk_191[0] = 2;
            break;
        case 0xCD334CD3:
            work->state = 9;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 2;
            work->unk_191[0] = 3;
            break;
        case 27:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdGoofy;
            work->variant[0] = 0;
            break;
        case 0x8003ECFB:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdGoofy;
            work->variant[0] = 1;
            break;
        case 0xCFB3ECFB:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdGoofy;
            work->variant[0] = 2;
            break;
        case 28:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdDonald;
            work->variant[0] = 0;
            break;
        case 0x8003C4F1:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdDonald;
            work->variant[0] = 1;
            break;
        case 0xCF13C4F1:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdDonald;
            work->variant[0] = 2;
            break;
        case 33:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnTink;
            work->variant[0] = 0;
            work->unk_191[0] = 1;
            break;
        case 0x8004B52D:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnTink;
            work->variant[0] = 1;
            work->unk_191[0] = 1;
            break;
        case 0xD2D4B52D:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnTink;
            work->variant[0] = 2;
            work->unk_191[0] = 1;
            break;
        case 41:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdAriel;
            work->variant[0] = 0;
            break;
        case 0x80055555:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdAriel;
            work->variant[0] = 1;
            break;
        case 0xD5555555:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdAriel;
            work->variant[0] = 2;
            break;
        case 34:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnMushu;
            work->variant[0] = 0;
            break;
        case 0x8004DD37:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnMushu;
            work->variant[0] = 1;
            break;
        case 0xD374DD37:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnMushu;
            work->variant[0] = 2;
            break;
        case 29:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnSimba;
            work->variant[0] = 0;
            break;
        case 0x80041505:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnSimba;
            work->variant[0] = 1;
            break;
        case 0xD0541505:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnSimba;
            work->variant[0] = 2;
            break;
        case 35:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnCloud;
            work->variant[0] = 0;
            break;
        case 0x80050541:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnCloud;
            work->variant[0] = 1;
            break;
        case 0xD4150541:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnCloud;
            work->variant[0] = 2;
            break;
        case 31:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnBambi;
            work->variant[0] = 0;
            break;
        case 0x80046519:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnBambi;
            work->variant[0] = 1;
            break;
        case 0xD1946519:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnBambi;
            work->variant[0] = 2;
            break;
        case 42:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdJack;
            work->variant[0] = 0;
            break;
        case 0x80057D5F:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdJack;
            work->variant[0] = 1;
            break;
        case 0xD5F57D5F:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdJack;
            work->variant[0] = 2;
            break;
        case 40:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdAladdin;
            work->variant[0] = 0;
            break;
        case 0x80052D4B:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdAladdin;
            work->variant[0] = 1;
            break;
        case 0xD4B52D4B:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdAladdin;
            work->variant[0] = 2;
            break;
        case 43:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdPan;
            work->variant[0] = 0;
            break;
        case 0x8005A569:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdPan;
            work->variant[0] = 1;
            break;
        case 0xD695A569:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdPan;
            work->variant[0] = 2;
            break;
        case 32:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnDumbo;
            work->variant[0] = 0;
            break;
        case 0x80048D23:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnDumbo;
            work->variant[0] = 1;
            break;
        case 0xD2348D23:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnDumbo;
            work->variant[0] = 2;
            break;
        case 30:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnGenie;
            work->variant[0] = 0;
            break;
        case 0x80043D0F:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnGenie;
            work->variant[0] = 1;
            break;
        case 0xD0F43D0F:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnGenie;
            work->variant[0] = 2;
            break;
        case 44:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdBeast;
            work->variant[0] = 0;
            break;
        case 0x8005CD73:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdBeast;
            work->variant[0] = 1;
            break;
        case 0xD735CD73:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdBeast;
            work->variant[0] = 2;
            break;
        case 47:
            work->state = 12;
            work->steps = 0;
            work->stateTimer = 0;
            work->unk_191[0] = 0;
            work->variant[0] = 0;
            break;
        case 48:
            work->state = 12;
            work->steps = 0;
            work->stateTimer = 0;
            work->unk_191[0] = 0;
            work->variant[0] = 1;
            break;
        case 49:
            work->state = 12;
            work->steps = 0;
            work->stateTimer = 0;
            work->unk_191[0] = 0;
            work->variant[0] = 2;
            break;
        case 50:
            work->state = 12;
            work->steps = 0;
            work->stateTimer = 0;
            work->unk_191[0] = 0;
            work->variant[0] = 3;
            break;
        case 51:
            work->state = 12;
            work->steps = 0;
            work->stateTimer = 0;
            work->unk_191[0] = 0;
            work->variant[0] = 4;
            break;
        case 52:
            work->state = 12;
            work->steps = 0;
            work->stateTimer = 0;
            work->unk_191[0] = 0;
            work->variant[0] = 5;
            break;
        case 53:
            work->state = 12;
            work->steps = 0;
            work->stateTimer = 0;
            work->unk_191[0] = 0;
            work->variant[0] = 6;
            break;
        case 25:
            work->state = 86;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 0;
            break;
        case 0x80039CE7:
            work->state = 86;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 1;
            break;
        case 0xCE739CE7:
            work->state = 86;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 2;
            break;
        case 0xC0100401:
            work->state = 5;
            work->steps = 0;
            work->stateTimer = 0;
            work->comboCount = 0;
            break;
        case 100:
            work->state = 31;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 0;
            break;
        case 101:
            work->state = 52;
            work->steps = 0;
            work->stateTimer = 0;
            work->comboCount = 2;
            break;
        case 104:
            work->state = 39;
            work->steps = 0;
            work->stateTimer = 0;
            work->unk_191[0] = 3;
            break;
        case 102:
            work->state = 50;
            work->steps = 0;
            work->stateTimer = 0;
            work->nextState = 51;
            work->comboCount = 8;
            break;
        case 103:
            work->state = 45;
            work->steps = 0;
            work->stateTimer = 0;
            work->unk_191[0] = 3;
            break;
        case 105:
            work->state = 56;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 106:
            work->state = 55;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 107:
            work->state = 59;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 126:
            work->state = 63;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 124:
            work->state = 68;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 130:
            work->state = 69;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 131:
            work->state = 70;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 108:
            work->state = 44;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 109:
            work->state = 76;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 116:
            work->state = 31;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 2;
            break;
        case 113:
            work->state = 31;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 1;
            break;
        case 117:
            work->state = 31;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 3;
            break;
        case 118:
            work->state = 31;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 4;
            break;
        case 119:
            work->state = 31;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 5;
            break;
        case 115:
            work->state = 31;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 6;
            break;
        case 122:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescFrdDonald;
            work->variant[0] = 3;
            break;
        case 135:
            work->state = 71;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 128:
            work->state = 17;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnBambi;
            work->variant[0] = 3;
            break;
        case 129:
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            work->summonDesc = &gTaskDescSmnCloud;
            work->variant[0] = 3;
            break;
        case 120:
            work->state = 64;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 121:
            work->state = 78;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 132:
            work->state = 72;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 125:
            work->state = 62;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 127:
            work->state = 73;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 136:
            work->state = 74;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 110:
            work->state = 77;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 114:
            work->state = 31;
            work->steps = 0;
            work->stateTimer = 0;
            work->variant[0] = 7;
            break;
        case 134:
            work->state = 79;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 133:
            work->state = 81;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 111:
            work->state = 80;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        case 112:
        case 123:
        default:
            SetBtlSoraState(work, 1);
            break;
        }

        e3 = p->btl->actor2;

        if (e3 != NULL) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                if (p->x < e3->x) {
                    p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                }
            } else {
                if (p->x > e3->x) {
                    p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                }
            }
        }

        AnimReset(&work->anim);
        work->speed = 0;
        p->vx = p->vy = 0;
        break;
    case BTL_REACTION_HAZARD:
        switch (work->state) {
        case 67:
            m4aSongNumStop(SONG_EF_DAMBO_SPLOOP);
            break;
        case 79:
            m4aSongNumStop(SONG_EF_TRUNEDO);
            break;
#ifdef VERSION_EU
        case 78:
            m4aSongNumStop(SONG_EF_HOLLY);
            break;
        case 68:
            m4aSongNumStop(SONG_EF_XMAS);
            break;
#endif
        }

        if (!(gBtlWork->flags & BTL_FLAG_OPPONENT_CARD_ACTION)) {
            gBtlWork->flags |= BTL_FLAG_STOP_BGFX;
        }

        SetBattleZoom(12, 0x100, gBtlWork->x2, gBtlWork->y2);
        ColliderSetDisabled(&p->collider, 0);
        DisableBtlSoraPassThrough(work);
        p->flags &= 0xFFFFDFFBFF7FFFFFLL;
        p->btl->flags |= BTL_FLAG_DISMISS_SUMMONS;
        work->speed = 0;
        work->scaleX = 0x100;
        work->scaleY = 0x100;
        work->state = 38;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case BTL_REACTION_CARD_BROKEN:
        switch (work->state) {
        case 23:
        case 24:
        case 25:
        case 26:
            p->x = p->originX;
            p->y = p->originY;
            p->z = p->originZ;
#ifndef VERSION_EU
            p->btl->flags &= ~BTL_FLAG_PLAYER_OFFSCREEN;
#endif
            p->flags &= ~BTLOBJ_FLAG_NO_BREAK_POP;
            CreateBtlPopTask(p, 9);
            break;
        case 67:
            m4aSongNumStop(SONG_EF_DAMBO_SPLOOP);
            break;
        case 79:
            m4aSongNumStop(SONG_EF_TRUNEDO);
            break;
#ifdef VERSION_EU
        case 78:
            m4aSongNumStop(SONG_EF_HOLLY);
            break;
        case 68:
            m4aSongNumStop(SONG_EF_XMAS);
            break;
#endif
        }

#ifdef VERSION_EU
        p->btl->flags &= ~BTL_FLAG_PLAYER_OFFSCREEN;
#endif
        work->scaleX = work->scaleY = 0x100;
        ColliderSetDisabled(&p->collider, 0);
        DisableBtlSoraPassThrough(work);
        p->flags &= 0xFFFFDFFBFF7FFFFFLL;
        p->btl->flags |= BTL_FLAG_DISMISS_SUMMONS;
        work->speed = 0;
        work->state = 29;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case BTL_REACTION_WARPED:
        FadeStartIn(FADE_MODE_ADD_WHITE, 20);
        gBtlWork->hitStop = 15;

        if (p->badStatus != BAD_STATUS_STUN) {
            p->badStatus = BAD_STATUS_STUN;
            p->badStatusTimer = 360;
        }

        work->speed = 0;
        work->state = 34;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case BTL_REACTION_STUNNED:
        StartBtlSoraKnockback(work);
        work->speed = 0;
        work->state = 34;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case BTL_REACTION_STOPPED:
        if (work->state != 43) {
            work->flags |= BTL_SORA_FLAG_PASS_THROUGH;
            work->speed = 0;
            p->vx = p->vy = 0;
            work->state = 43;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    }

    if (p->btl->flags & BTL_FLAG_FIELD_HIDDEN) {
        work->state = 35;
        work->steps = 0;
        work->stateTimer = 0;
    } else if (p->flags & BTLOBJ_FLAG_FREEZE_PENDING) {
        p->flags &= ~BTLOBJ_FLAG_FREEZE_PENDING;
        work->state = 83;
        work->steps = 0;
        work->stateTimer = 0;
    }

    switch (work->state) {
    case 0:
        if ((s16)work->stateTimer == 0) {
            if (work->steps == 0) {
                SetBtlSoraAnimation(work, 0, 0);
            }

            if (work->steps <= 29) {
                AnimReset(&work->anim);
            }

            if (AnimIsFinished(&work->anim) != 0) {
                work->stateTimer = 1;
            } else {
                work->steps++;
            }

            break;
        }

        SetBtlSoraAnimation(work, 1, 1);

        if (gBtlWork->phase == 0) {
            break;
        }

        p->flags &= ~BTLOBJ_FLAG_INTANGIBLE;
        p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
        work->state = 1;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case 18:
        if (!(p->btl->flags & BTL_FLAG_SUMMON_ACTIVE)) {
            SetBtlSoraStateNoReset(work, 1);
        }
    case 1: {
        s32 t2;

        FocusBtlSoraCameraOnTarget(work);
        DisableBtlSoraPassThrough(work);

        if (p->z < p->groundZ) {
            if (work->state == 18) {
                work->state = 20;
                work->steps = 0;
                work->stateTimer = 0;
            } else {
                work->state = 3;
                work->steps = 0;
                work->stateTimer = 0;
            }

            break;
        }

        t2 = work->state;

        if (t2 != 18) {
            p->btl->flags |= BTL_FLAG_CAN_CHARGE_RELOAD;

            if ((p->btl->flags & BTL_FLAG_RELOAD_CHARGING) && p->btl->hcEffect != 30) {
                SetBtlSoraAnimation(work, 51, 0);

                if (AnimIsFinished(&work->anim) != 0) {
                    AnimSetFrame(&work->anim, 3);
                }

                work->speed = 0;
                break;
            }
        }

        if ((u16)(pressed & DPAD_LEFT) != 0) {
            if (work->tapTimers[0] != 0) {
                p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                p->originX = p->x;

                if (t2 == 18) {
                    work->state = 22;
                    work->steps = 0;
                    work->stateTimer = 0;
                } else {
                    work->state = 30;
                    work->steps = 0;
                    work->stateTimer = 0;
                }

                break;
            }
        } else if (pressed & DPAD_RIGHT) {
            if (work->tapTimers[1] != 0) {
                p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                p->originX = p->x;

                if (t2 == 18) {
                    work->state = 22;
                    work->steps = 0;
                    work->stateTimer = 0;
                } else {
                    work->state = 30;
                    work->steps = 0;
                    work->stateTimer = 0;
                }

                break;
            }
        }

        UpdateBtlSoraWalk(work, held);

        if (!(pressed & B_BUTTON)) {
            break;
        }

        m4aSongNumStart(work->groundSongs[2]);

        if (work->state == 18) {
            work->state = 19;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->state = 2;
            work->steps = 0;
            work->stateTimer = 0;
        }
    }

        break;
    case 19:
        if (!(p->btl->flags & BTL_FLAG_SUMMON_ACTIVE)) {
            SetBtlSoraStateNoReset(work, 2);
        }
    case 2: {
        s32 uv;

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraDirAnimation(work, 1, 1);

            if (!(held & DPAD_ANY)) {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    work->angle = 192;
                } else {
                    work->angle = 64;
                }
            }

            work->speed >>= 1;
        }

        uv = work->stateTimer;

        if ((s16)uv <= 3) {
            work->stateTimer = uv + 1;
            break;
        }

        if (work->state == 19) {
            work->state = 20;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->state = 3;
            work->steps = 0;
            work->stateTimer = 0;
        }

        work->vz = -1344;
        p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;
        work->speed <<= 1;
    }

        break;
    case 20:
        if (!(p->btl->flags & BTL_FLAG_SUMMON_ACTIVE)) {
            SetBtlSoraStateNoReset(work, 3);
        }
    case 3:
        FocusBtlSoraCameraOnTarget(work);
        p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;

        if (work->vz < 0) {
            if (work->vz <= -512) {
                SetBtlSoraDirAnimation(work, 2, 1);
            } else {
                SetBtlSoraDirAnimation(work, 3, 1);
            }
        } else if (work->vz <= 511) {
            SetBtlSoraDirAnimation(work, 3, 1);
        } else {
            SetBtlSoraDirAnimation(work, 4, 1);
        }

        if (work->vz < 0 && !(held & B_BUTTON)) {
            work->vz += 64;
        }

        if (((s16)held & (DPAD_RIGHT | DPAD_UP)) == (DPAD_RIGHT | DPAD_UP)) {
            if (work->angle != 32) {
                work->angle = 32;
                work->speed = 0;
            }

            p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
        } else if (((s16)held & (DPAD_RIGHT | DPAD_DOWN)) == (DPAD_RIGHT | DPAD_DOWN)) {
            if (work->angle != 96) {
                work->angle = 96;
                work->speed = 0;
            }

            p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
        } else if (((s16)held & (DPAD_LEFT | DPAD_DOWN)) == (DPAD_LEFT | DPAD_DOWN)) {
            if (work->angle != 160) {
                work->angle = 160;
                work->speed = 0;
            }

            p->flags |= BTLOBJ_FLAG_FACING_LEFT;
        } else if (((s16)held & (DPAD_LEFT | DPAD_UP)) == (DPAD_LEFT | DPAD_UP)) {
            if (work->angle != 224) {
                work->angle = 224;
                work->speed = 0;
            }

            p->flags |= BTLOBJ_FLAG_FACING_LEFT;
        } else if (held & DPAD_UP) {
            if (work->angle != 0) {
                work->angle = 0;
                work->speed = 0;
            }
        } else if (held & DPAD_RIGHT) {
            if (work->angle != 64) {
                work->angle = 64;
                work->speed = 0;
            }

            p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
        } else if (held & DPAD_DOWN) {
            if (work->angle != 128) {
                work->angle = 128;
                work->speed = 0;
            }
        } else if (held & DPAD_LEFT) {
            if (work->angle != 192) {
                work->angle = 192;
                work->speed = 0;
            }

            p->flags |= BTLOBJ_FLAG_FACING_LEFT;
        }

        if (held & DPAD_ANY) {
            work->speed += 17;

            if (work->speed > 614) {
                work->speed = 614;
            }
        } else {
            work->speed -= 38;

            if (work->speed < 0) {
                work->speed = 0;
            }
        }

        work->stateTimer++;
        break;
    case 21:
        if (!(p->btl->flags & BTL_FLAG_SUMMON_ACTIVE)) {
            SetBtlSoraStateNoReset(work, 4);
        }
    case 4: {
        s32 uv;

        FocusBtlSoraCameraOnTarget(work);
        st = work->stateTimer;

        if (st == 0) {
            m4aSongNumStart(work->groundSongs[3]);
            work->speed = 0;
            SetBtlSoraDirAnimation(work, 5, 0);
            gBtlWork->flags |= BTL_FLAG_JUMP_LANDED;
        } else if (pressed & B_BUTTON) {
            m4aSongNumStart(work->groundSongs[2]);
            p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;

            if (work->state == 21) {
                work->state = 19;
                work->steps = 0;
                work->stateTimer = 0;
            } else {
                work->state = 2;
                work->steps = 0;
                work->stateTimer = 0;
            }
        } else if ((u16)(pressed & DPAD_LEFT) != 0) {
            if (work->tapTimers[0] != 0) {
                p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                p->originX = p->x;

                if (work->state == 21) {
                    work->state = 22;
                    work->steps = 0;
                    work->stateTimer = 0;
                } else {
                    work->state = 30;
                    work->steps = 0;
                    work->stateTimer = 0;
                }

                break;
            }
        } else if (pressed & DPAD_RIGHT) {
            if (work->tapTimers[1] != 0) {
                p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                p->originX = p->x;

                if (work->state == 21) {
                    work->state = 22;
                    work->steps = 0;
                    work->stateTimer = 0;
                } else {
                    work->state = 30;
                    work->steps = 0;
                    work->stateTimer = 0;
                }

                break;
            }
        }

        uv = work->stateTimer;

        if ((s16)uv <= 6) {
            work->stateTimer = uv + 1;
            break;
        }

        if (work->state == 21) {
            work->state = 18;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    }
    case 15:
        SetBtlSoraDirAnimation(work, 0, 1);
        work->speed = 0;

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            work->angle = 192;
            p->x -= 614;
        } else {
            work->angle = 64;
            p->x += 614;
        }

        break;
    case 28:
        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 44, 0);
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer++;
        }

        break;
    case 29:
        if (work->flags & BTL_SORA_FLAG_HIDDEN) {
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, work->unk_191[0] + 47, 0);
        }

        FocusBtlSoraCameraOnTarget(work);

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 22:
        if (!(p->btl->flags & BTL_FLAG_SUMMON_ACTIVE)) {
            SetBtlSoraStateNoReset(work, 30);
        }
    case 30:
        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            work->speed = 0;
            work->unk_194 = 1664;
            SetBtlSoraAnimation(work, 53, 0);
            work->steps = 32;

            if (work->state != 22) {
                p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            }
        }

        if ((s16)work->stateTimer == 4) {
            EnableBtlSoraPassThrough(work);

            if (work->state != 22) {
                p->flags |= BTLOBJ_FLAG_HIT_LOCKED;
            }

            work->vz = -460;
            m4aSongNumStart(SONG_VO_SR_ATTACK09);
        } else if ((s16)work->stateTimer > 4 && work->steps != 0) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x -= work->unk_194;
            } else {
                p->x += work->unk_194;
            }

            ApproachValue(&work->unk_194, 0, work->steps);
            work->steps--;

            if (p->z < p->groundZ) {
                if (held & DPAD_UP) {
                    p->y -= 384;
                } else if (held & DPAD_DOWN) {
                    p->y += 384;
                }
            }

            if (work->anim.timer == 0 && AnimGetFrame(&work->anim) == 5) {
                m4aSongNumStart(work->groundSongs[3]);
            }

#ifndef VERSION_EU
            if (work->steps == 8) {
            } else
#endif
            if (work->steps == 0) {
                DisableBtlSoraPassThrough(work);

                if (work->state != 22) {
                    p->flags &= ~BTLOBJ_FLAG_HIT_LOCKED;
                }
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            if (work->state == 22) {
                work->state = 18;
                work->steps = 0;
                work->stateTimer = 0;
            } else {
                work->state = 1;
                work->steps = 0;
                work->stateTimer = 0;
                p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
            }

            gBtlWork->flags |= BTL_FLAG_DODGE_ROLL_DONE;
        } else {
            work->stateTimer++;
        }

        break;
    case 33:
        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, work->unk_191[0] + 47, 0);
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 10:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 44, 0);
            m4aSongNumStart(SONG_VO_SR_CAREL00);
        } else if ((s16)work->stateTimer == 27) {
            switch (work->variant[0]) {
            case 0:
                BgFxStartCure(0, p->x, p->y, p->z - 11264);
                break;
            case 1:
                BgFxStartCure(1, p->x, p->y, p->z - 11264);
                break;
            case 2:
                BgFxStartCure(2, p->x, p->y, p->z - 11264);
                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        if ((s16)work->stateTimer > 27 && BgFxIsActive() == 0) {
            switch (p->btl->hcEffect) {
            case 13:
                switch (work->variant[0]) {
                case 0:
                    p->hp += 75;
                    break;
                case 1:
                    p->hp += 225;
                    break;
                case 2:
                    p->hp += 450;
                    break;
                }

                break;
            case 38:
                switch (work->variant[0]) {
                case 0:
                    p->hp += 65;
                    break;
                case 1:
                    p->hp += 195;
                    break;
                case 2:
                    p->hp += 390;
                    break;
                }

                break;
            default:
                switch (work->variant[0]) {
                case 0:
                    p->hp += 50;
                    break;
                case 1:
                    p->hp += 150;
                    break;
                case 2:
                    p->hp += 300;
                    break;
                }

                break;
            }

            if (p->hp > p->maxHp) {
                p->hp = p->maxHp;
            }

            CreateBtlPopTask(p, 10);
            SetBtlSoraState(work, 1);
            break;
        }

        work->stateTimer++;
        break;
    case 11: {
        s32 t;
        s32 t2;
        s32 t3;

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 44, 0);
            m4aSongNumStart(SONG_VO_SR_STOP00);
            FadeStartOut(FADE_MODE_GRAY, 8);
        } else if ((s16)work->stateTimer == 25) {
            if (p->btl->actor2 != NULL) {
                e = p->btl->actor2;
                t = e->x;
                t2 = e->y;
                t3 = e->z - (e->centerHeight << 8);
            } else {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    t = p->x - 12800;
                } else {
                    t = p->x + 12800;
                }

                t2 = p->y;
                t3 = p->z - 4096;
            }

            switch (work->variant[0]) {
            case 0:
                BgFxStartStop(0, t, t2, t3, 78);
                break;
            case 1:
                BgFxStartStop(1, t, t2, t3, 79);
                break;
            case 2:
            default:
                BgFxStartStop(work->variant[0], t, t2, t3, 80);
                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        if ((s16)work->stateTimer > 25 && BgFxIsActive() == 0) {
            FadeStartIn(FADE_MODE_GRAY, 8);
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer++;
        }
    }

        break;
    case 12:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 52, 0);
        }

        if ((s16)work->stateTimer == 23) {
            BgFxStartPotion(p->x, p->y, p->z);
        }

        if ((s16)work->stateTimer > 23 && BgFxIsActive() == 0) {
            switch (work->variant[0]) {
            case 0:
                if (work->mainSide != 0) {
                    RequestSoraPotion();
                } else {
                    RequestRikuPotion();
                }

                break;
            case 1:
                if (work->mainSide != 0) {
                    RequestSoraHiPotion();
                } else {
                    RequestRikuHiPotion();
                }

                break;
            case 2:
                if (work->mainSide != 0) {
                    RequestSoraMegaPotion();
                } else {
                    RequestRikuMegaPotion();
                }

                break;
            case 3:
                if (work->mainSide != 0) {
                    RequestSoraEther();
                } else {
                    RequestRikuEther();
                }

                break;
            case 4:
                if (work->mainSide != 0) {
                    RequestSoraMegaEther();
                } else {
                    RequestRikuMegaEther();
                }

                break;
            case 5:
                if (work->mainSide != 0) {
                    RequestSoraElixir();
                } else {
                    RequestRikuElixir();
                }

                break;
            default:
                if (work->mainSide != 0) {
                    RequestSoraMegalixir();
                } else {
                    RequestRikuMegalixir();
                }

                break;
            }

            SetBtlSoraState(work, 1);
            break;
        }

        work->stateTimer++;
        break;
    case 16: {
        s32 t;
        s32 t2;

        d = 0;
        FocusBtlSoraCameraOnTarget(work);
        memcpy(buf, sBtlSoraSwingHitFrames, 5);

        if (p->btl->hcEffect == 3) {
            if ((work->flags & BTL_SORA_FLAG_COMBO_EXTENDED) == 0) {
                if (work->comboCount == 2) {
                    work->comboCount = 1;
                    work->flags |= BTL_SORA_FLAG_COMBO_EXTENDED;
                    a = work->attacks[0];
                } else {
                    a = work->attacks[work->comboCount];
                }
            } else {
                a = work->attacks[work->comboCount];
            }
        } else if (p->btl->hcEffect == 5) {
            work->comboCount = 2;
            a = work->attacks[2];
        } else {
            a = work->attacks[work->comboCount];
        }

        if (work->comboCount != 0) {
            if (a->flags & COMBO_FLAG_AERIAL_SWING) {
                if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) == 0) {
                    a = a->next;
                }
            }
        }

        if ((s16)work->stateTimer == 0) {
#ifdef VERSION_EU
            SetBtlSoraAnimation(work, a->animId + work->swingSpeed, 0);
#else
            SetBtlSoraAnimation(work, a->animId + work->swingSpeed, 0);
#endif
            m4aSongNumStart(a->swingSound);
            work->vz = a->vz;

            switch (p->btl->hcEffect) {
            case 8:
            case 34:
            case 43:
            case 44:
            case 49:
                p->btl->hcEffectCount--;
                break;
            }
        } else if ((s16)work->stateTimer == buf[work->swingSpeed]) {
            MakeOpponentsHittable();

            if (p->btl->hcEffect == 34) {
                switch (a->animId) {
                case 22:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(a->attackIds[work->unk_191[1]], p->x - 5120, p->y,
                                          p->z - 7168, 40, 16, 44);
                    } else {
                        d = ApplyAttackBox(a->attackIds[work->unk_191[1]], p->x + 5120, p->y,
                                          p->z - 7168, 40, 16, 44);
                    }

                    break;
                case 12:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(a->attackIds[work->unk_191[1]], p->x - 8192, p->y,
                                          p->z, 28, 20, 32);
                    } else {
                        d = ApplyAttackBox(a->attackIds[work->unk_191[1]], p->x + 8192, p->y,
                                          p->z, 28, 20, 32);
                    }

                    break;
                default:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(a->attackIds[work->unk_191[1]], p->x - 9216, p->y,
                                          p->z, 32, 12, 32);
                    } else {
                        d = ApplyAttackBox(a->attackIds[work->unk_191[1]], p->x + 9216, p->y,
                                          p->z, 32, 12, 32);
                    }

                    break;
                }
            } else {
                if (p->btl->hcEffect == 49 && work->comboCount == 2) {
                    if (GetRandom() % 3 != 0) {
                        t = 164;
                    } else {
                        CreateBtlPopTask(p, 2);
                        t = a->attackIds[work->unk_191[1]];
                    }
                } else {
                    t = a->attackIds[work->unk_191[1]];
                }

                switch (a->animId) {
                case 22:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(t, p->x - 5120, p->y, p->z - 7168, 28, 16, 44);
                    } else {
                        d = ApplyAttackBox(t, p->x + 5120, p->y, p->z - 7168, 28, 16, 44);
                    }

                    break;
                case 12:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(t, p->x - 8192, p->y, p->z, 16, 20, 32);
                    } else {
                        d = ApplyAttackBox(t, p->x + 8192, p->y, p->z, 16, 20, 32);
                    }

                    break;
                default:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(t, p->x - 9216, p->y, p->z, 20, 12, 32);
                    } else {
                        d = ApplyAttackBox(t, p->x + 9216, p->y, p->z, 20, 12, 32);
                    }

                    break;
                }
            }

            if (d == 1) {
                m4aSongNumStart(a->hitSound);

                if (a->flags & COMBO_FLAG_ZOOM_ON_HIT) {
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        SetBattleZoom(8, 384, p->x - 5120, (p->y - 5120) + p->z);
                    } else {
                        SetBattleZoom(8, 384, p->x + 5120, (p->y - 5120) + p->z);
                    }
                }

                work->flags |= BTL_SORA_FLAG_SWING_HIT;
            } else {
                work->flags &= ~BTL_SORA_FLAG_SWING_HIT;
            }
        } else if ((s16)work->stateTimer == buf[work->swingSpeed] + 2) {
            if (a->flags & COMBO_FLAG_ZOOM_ON_HIT) {
                SetBattleZoom(15, 256, gBtlWork->x2, gBtlWork->y2);
            }

            if (work->comboCount <= 1) {
                if (work->flags & BTL_SORA_FLAG_SWING_HIT) {
                    p->flags &= ~BTLOBJ_FLAG_IN_CARD_ACTION;
                }
            }
        }

        if (d == 2) {
            SetBattleZoom(8, 256, gBtlWork->x2, gBtlWork->y2);
            SetBtlSoraState(work, 33);
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            break;
        }

        if (AnimIsFinished(&work->anim) != 0 && (p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) == 0) {
            SetBtlSoraState(work, 1);
            break;
        }

        if (AnimGetFrame(&work->anim) <= 2) {
            e = p->btl->actor2;

            if (e != NULL) {
                if (AnimGetFrame(&work->anim) > 1) {
                    s32 t;

                    t = e->x - p->x;
                    t2 = e->y - p->y;

                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        s32 target;
                        s32 origin;

                        if (t > 0) {
                            t = 0;
                        }

                        target = p->originX + t;
                        origin = p->x - 4096;
                        p->x += (target - origin) >> 3;
                    } else {
                        s32 target;
                        s32 origin;

                        if (t < 0) {
                            t = 0;
                        }

                        target = p->originX + t;
                        origin = p->x + 4096;
                        p->x += (target - origin) >> 3;
                    }

                    p->y += ((p->originY + t2) - p->y) >> 4;
                }

                if (a->flags & COMBO_FLAG_AERIAL_SWING) {
                    s32 t3 = (e->z - (e->centerHeight << 8)) - p->z;

                    if (t3 < 0) {
                        p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;
                        p->z += ((p->originZ + t3) - p->z) >> 3;
                        work->vz = 0;
                    }
                }
            } else {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    s32 t4 = p->originX - 10240;
                    p->x += (t4 - p->x) >> 3;
                } else {
                    s32 t4 = p->originX + 10240;
                    p->x += (t4 - p->x) >> 3;
                }
            }
        }

        work->stateTimer++;
        break;
    }
    case 68: {
        BtlObj* e;

        FocusBtlSoraCamera(work);

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 51, 0);
            FadeStartOut(FADE_MODE_BLACK, 30);
        }

        if ((s16)work->stateTimer == 30) {
            BgFxStartXmas(60);
            m4aSongNumStart(SONG_EF_XMAS);
        } else if ((s16)work->stateTimer > 30 && BgFxIsActive() == 0) {
            e = gBtlWork->actor;
            CreateBtlPopTask(e, 10);
            e->hp += 50;

            if (e->hp > e->maxHp) {
                e->hp = e->maxHp;
            }

            if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
                e = gRikuBtlWork->actor;

                if (e->badStatus != BAD_STATUS_STOP) {
                    CreateBtlPopTask(e, 10);
                    e->hp += 50;

                    if (e->hp > e->maxHp) {
                        e->hp = e->maxHp;
                    }
                }
            } else {
                e = ListPoolFirst(&gBtlWork->pool);

                while (e != NULL) {
                    if (e->badStatus != BAD_STATUS_STOP && e->parent == NULL) {
                        CreateBtlPopTask(e, 10);
                        e->hp += 50;

                        if (e->hp > e->maxHp) {
                            e->hp = e->maxHp;
                        }
                    }

                    e = ListPoolNext(&e->node);
                }
            }

            if (work->mainSide != 0) {
                RequestSoraMegalixir();
            } else {
                RequestRikuMegalixir();
            }

            SetBtlSoraState(work, 1);
            FadeStartIn(FADE_MODE_BLACK, 16);
            break;
        }

        work->stateTimer++;
        break;
    }
    case 76: {
        u16 uv;

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 43, 0);
            m4aSongNumStart(SONG_VO_SR_SUMMON06);
        }

        if ((s16)work->stateTimer == 40) {
            if ((GetRandom() & 1) || (gBtlWork->flags & (BTL_FLAG_BOSS_BATTLE | BTL_FLAG_HUM_BATTLE | BTL_FLAG_VS_BATTLE))) {
                CreateBtlPopTask(p, 2);
            } else {
                s32 t3;
                BtlObj* e2;

                t3 = 0x40000;
                e2 = NULL;
                e = ListPoolFirst(&gBtlWork->pool);

                while (e != NULL) {
                    s32 t;
                    s32 t2;
                    s32 d;

                    t = (p->x - e->x) >> 8;
                    t2 = (p->y - e->y) >> 8;
                    d = Sqrt8(t * t + t2 * t2);

                    if (d < t3) {
                        t3 = d;
                        e2 = e;
                    }

                    e = ListPoolNext(&e->node);
                }

                if (e2 != NULL) {
                    FadeFromAmount(FADE_MODE_BLUE, 15, 32);

                    if (e2->flags & BTLOBJ_FLAG_IMMUNE_WARP) {
                        CreateBtlPopTask(e2, 0);
                    } else {
                        e2->flags |= BTLOBJ_FLAG_WARP_PENDING;
                        e2->hitFlags = 0;
                        m4aSongNumStart(SONG_EF_TELEP);
                    }
                } else {
                    CreateBtlPopTask(p, 2);
                }
            }
        }

        uv = work->stateTimer;

        if ((s16)uv > 41) {
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    }
    case 77:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 43, 0);
            m4aSongNumStart(SONG_VO_SR_ATTACK08);
        }

        if ((s16)work->stateTimer == 40) {
            FadeStartIn(FADE_MODE_WHITE_BLEND, 8);
            FadeLock();
            m4aSongNumStart(SONG_EF_TELER);
            gBtlWork->hitStop = 8;
        } else if ((s16)work->stateTimer == 41) {
            ApplyAttackBox(110, p->x, p->y, p->z, 256, 256, 256);
        }

        uv = work->stateTimer;

        if ((s16)uv > 70) {
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    case 86:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 45, 0);
            m4aSongNumStart(SONG_VO_SR_AIRO00);
        }

        if ((s16)work->stateTimer == 27) {
            switch (work->variant[0]) {
            case 0:
                BgFxStartAero(0, p->x, p->y, p->z, 81);
                break;
            case 1:
                BgFxStartAero(1, p->x, p->y, p->z, 82);
                break;
            case 2:
                BgFxStartAero(2, p->x, p->y, p->z, 83);
                break;
            }

            m4aSongNumStart(SONG_EF_AIRO_HIT);
        } else if ((s16)work->stateTimer > 27 && BgFxIsActive() == 0) {
            SetBtlSoraAnimation(work, 46, 0);
            SetBtlSoraState(work, 82);
            break;
        }

        work->stateTimer++;
        break;
    case 78:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 45, 0);
            m4aSongNumStart(SONG_VO_SR_SUMMON00);
        }

        if ((s16)work->stateTimer == 27) {
            BtlObj* e;
            s32 x;
            s32 y;
            s32 z;

            e = p->btl->actor2;

            if (e != NULL) {
                x = e->x;
                y = e->y;
                z = e->groundZ;
            } else {
                x = p->flags & BTLOBJ_FLAG_FACING_LEFT ? p->x - 6144 : p->x + 6144;
                y = p->y;
                z = p->groundZ;
            }

            BgFxStartHoly(x, y, z, 112);

            m4aSongNumStart(SONG_EF_HOLLY);
        } else if ((s16)work->stateTimer > 27 && BgFxIsActive() == 0) {
            SetBtlSoraAnimation(work, 46, 0);
            SetBtlSoraState(work, 82);
            break;
        }

        work->stateTimer++;
        break;
    case 79:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 45, 0);
            FocusBtlSoraCamera(work);
            m4aSongNumStart(SONG_VO_SR_SUMMON06);
        }

        if ((s16)work->stateTimer == 27) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartTornado(p->x, p->y, 0, 113, 1);
            } else {
                BgFxStartTornado(p->x, p->y, 0, 113, 0);
            }

            m4aSongNumStart(SONG_EF_TRUNEDO);
        } else if ((s16)work->stateTimer > 27) {
            FocusBtlSoraCameraOnBgFx(work);

            if (held & DPAD_LEFT) {
                BgFxAddPosition(-256, 0, 0);
            } else if (held & DPAD_RIGHT) {
                BgFxAddPosition(256, 0, 0);
            }

            if (held & DPAD_UP) {
                BgFxAddPosition(0, -128, 0);
            } else if (held & DPAD_DOWN) {
                BgFxAddPosition(0, 128, 0);
            }

            if (BgFxIsActive() == 0) {
                SetBtlSoraAnimation(work, 46, 0);
                SetBtlSoraState(work, 82);
                m4aSongNumStop(SONG_EF_TRUNEDO);
                break;
            }
        }

        work->stateTimer++;
        break;
    case 80:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 45, 0);
            m4aSongNumStart(SONG_VO_SR_ATTACK08);
        }

        if ((s16)work->stateTimer == 27) {
            work->unk_158 = 20;
            BtlMapStartShake();
            ApplyAttackBox(114, p->x, p->y, p->z, 256, 256, 256);
            FadeFromAmount(FADE_MODE_BLUE, 16, 20);
            m4aSongNumStart(SONG_BTL_JF_BALLTHR);
        } else if ((s16)work->stateTimer > 27 && (s16)--work->unk_158 <= 0) {
            SetBtlSoraAnimation(work, 46, 0);
            SetBtlSoraState(work, 82);
            break;
        }

        work->stateTimer++;
        break;
    case 81:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 45, 0);
            m4aSongNumStart(SONG_VO_SR_STOP00);
        }

        if ((s16)work->stateTimer == 27) {
            work->unk_158 = 20;
            BgFxStartBind(p->x, 115);
            m4aSongNumStart(SONG_EF_BIND);
        } else if ((s16)work->stateTimer > 27 && BgFxIsActive() == 0) {
            if ((s16)--work->unk_158 <= 0) {
                SetBtlSoraAnimation(work, 46, 0);
                SetBtlSoraState(work, 82);
                break;
            }
        }

        work->stateTimer++;
        break;
    case 44:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 43, 0);
        }

        if ((s16)work->stateTimer == 40) {
            m4aSongNumStart(SONG_BTL_GMIC_OK);
            FadeStartIn(FADE_MODE_CONTRAST, 8);
            FadeLock();
            gBtlWork->hitStop = 8;
        } else if ((s16)work->stateTimer == 41) {
            if (gBtlWork->flags & (BTL_FLAG_BOSS_BATTLE | BTL_FLAG_HUM_BATTLE | BTL_FLAG_VS_BATTLE)) {
                CreateBtlPopTask(p, 2);
            } else {
                ApplyAttackBox(98, p->x, p->y, p->z, 256, 256, 256);
            }
        }

        if ((s16)work->stateTimer > 41 && FadeIsActive() == 0) {
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer++;
        }

        break;
    case 74:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 1, 0);
            work->steps = 16;
            FadeStartOut(FADE_MODE_GRAY, 1);
        }

        ApproachValue(&work->scaleX, 10, work->steps);
        ApproachValue(&work->scaleY, 512, work->steps);

        if (--work->steps > 0) {
            work->stateTimer++;
        } else {
            work->state = 75;
            work->steps = 0;
            work->stateTimer = 0;
            m4aSongNumStart(SONG_EF_TELEP);
        }

        break;
    case 75:
        if ((s16)work->stateTimer == 0) {
            e = PickBtlSoraTarget(work);
            work->steps = 16;

            if (e != NULL) {
                p->y = e->y;
                p->z = e->groundZ;
                p->groundZ = e->groundZ;

                if (e->x > p->x) {
                    p->x = e->x + 8192;
                    p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                } else {
                    p->x = e->x - 8192;
                    p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                }

                ApplyAttackBox(109, e->x, e->y, e->z, 4, 4, 4);
            }
        }

        ApproachValue(&work->scaleX, 256, work->steps);
        ApproachValue(&work->scaleY, 256, work->steps);

        if (--work->steps <= 0) {
            FadeStartIn(FADE_MODE_GRAY, 1);
            MakeOpponentsHittable();
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer++;
        }

        break;
    case 72:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 45, 0);
        }

        if ((s16)work->stateTimer == 27) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartSync(p->x + 768, p->y, p->z - 16384);
            } else {
                BgFxStartSync(p->x - 768, p->y, p->z - 16384);
            }
        } else if ((s16)work->stateTimer > 27 && BgFxIsActive() == 0) {
            SetBtlSoraAnimation(work, 46, 0);
            SetBtlSoraState(work, 82);
            break;
        }

        work->stateTimer++;
        break;
    case 8: {
        s32 t;
        s32 t2;

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        if ((s16)work->stateTimer == 0) {
            FocusBtlSoraCameraOnTarget(work);
            SetBtlSoraAnimation(work, 43, 0);
            m4aSongNumStart(SONG_VO_SR_THNDER00);
        }

        if ((s16)work->stateTimer == 27) {
            switch (work->variant[0]) {
            case 0:
                e = p->btl->actor2;

                if (e != NULL) {
                    t = e->x;
                    t2 = e->y;
                } else {
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        t = p->x - 20480;
                    } else {
                        t = p->x + 20480;
                    }

                    t2 = p->y;
                }

                d = 0;
                BgFxStartThunder(0, p->x, p->y, p->z - 16384, t, t2, d, 72);
                break;
            case 1:
                BgFxStartWideThunder(1, p->x, p->y, p->z - 16384, p->groundZ, 73);
                break;
            case 2:
            default:
                BgFxStartWideThunder(2, p->x, p->y, p->z - 16384, p->groundZ, 74);
                break;
            }
        } else if (work->variant[0] != 0 && (s16)work->stateTimer == 47) {
            SetBattleZoom(15, 148, 65536, 76800);
        } else if ((s16)work->stateTimer > 27 && BgFxIsActive() == 0) {
            SetBattleZoom(15, 256, gBtlWork->x2, gBtlWork->y2);
            SetBtlSoraState(work, 1);
            break;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        work->stateTimer++;
    }

        break;
    case 9:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        if ((s16)work->stateTimer == 0) {
            FocusBtlSoraCameraOnTarget(work);
            SetBtlSoraAnimation(work, 42, 0);
            m4aSongNumStart(SONG_VO_SR_GURABI00);
            FadeToAmount(FADE_MODE_ADD_WHITE, 13, 60);
        }

        if ((s16)work->stateTimer == 27) {
            e = p->btl->actor2;

            if (e != NULL) {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    BgFxStartGravity(work->variant[0], p->x - 8192, p->y, p->z - 3584,
                                  e->x, e->y, 0, 1, work->variant[0] + 75);
                } else {
                    BgFxStartGravity(work->variant[0], p->x + 8192, p->y, p->z - 3584,
                                  e->x, e->y, 0, 0, work->variant[0] + 75);
                }
            } else {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    BgFxStartGravity(work->variant[0], p->x - 8192, p->y, p->z - 3584,
                                  p->originX - 15360, p->originY, p->z, 1,
                                  work->variant[0] + 75);
                } else {
                    BgFxStartGravity(work->variant[0], p->x + 8192, p->y, p->z - 3584,
                                  p->originX + 15360, p->originY, p->z, 0,
                                  work->variant[0] + 75);
                }
            }
        } else if ((s16)work->stateTimer > 27 && BgFxIsActive() == 0) {
            SetBtlSoraState(work, 1);
            FadeToOriginal(FADE_MODE_ADD_WHITE, 20);
            break;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        work->stateTimer++;
        break;
    case 70: {
        s32 t;
        s32 t2;
        s32 t3;
        s32 t4;

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        if ((s16)work->stateTimer == 0) {
            FocusBtlSoraCameraOnTarget(work);
            SetBtlSoraAnimation(work, 42, 0);
            m4aSongNumStart(SONG_VO_SR_BURIZAD00);
            work->target = PickBtlSoraTarget(work);
        }

        if ((s16)work->stateTimer == 27) {
            t4 = 70;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                t = p->x - 23040;
            } else {
                t = p->x + 23040;
            }

            t2 = p->y;
            t3 = p->z - 4096;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartBlizzard(1, p->x - 18432, t2, t3, t, t2, t3, 1, t4);
            } else {
                BgFxStartBlizzard(1, p->x + 18432, t2, t3, t, t2, t3, 0, t4);
            }
        } else if ((s16)work->stateTimer > 27) {
            e = work->target;

            if (e != NULL) {
                s32 d;

                d = 0;

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    if (e->x < p->x - 8192) {
                        d = 1;
                    }
                } else {
                    if (e->x > p->x + 8192) {
                        d = 1;
                    }
                }

                if (d != 0) {
                    BgFxSetTarget(e->x, e->y, e->z - (e->centerHeight << 8));
                }
            }

            FocusBtlSoraCameraOnBgFx(work);

            if (BgFxIsActive() == 0) {
                SetBtlSoraState(work, 1);
                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        work->stateTimer++;
        break;
    }
    case 69: {
        s32 t;
        s32 t2;
        s32 t3;
        s32 t4;

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        if ((s16)work->stateTimer == 0) {
            FocusBtlSoraCameraOnTarget(work);
            SetBtlSoraAnimation(work, 42, 0);
            m4aSongNumStart(SONG_VO_SR_FIRE00);
            work->target = PickBtlSoraTarget(work);
        }

        if ((s16)work->stateTimer == 27) {
            t4 = 67;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                t = p->x - 51200;
            } else {
                t = p->x + 51200;
            }

            t2 = p->y;
            t3 = p->z - 3584;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartFire(1, p->x - 18432, t2, t3, t, t2, t3, 1, t4);
            } else {
                BgFxStartFire(1, p->x + 18432, t2, t3, t, t2, t3, 0, t4);
            }
        } else if ((s16)work->stateTimer > 27) {
            e = work->target;

            if (e != NULL) {
                s32 d;

                d = 0;

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    if (e->x < p->x - 8192) {
                        d = 1;
                    }
                } else {
                    if (e->x > p->x + 8192) {
                        d = 1;
                    }
                }

                if (d != 0) {
                    BgFxSetTarget(e->x, e->y, e->z - (e->centerHeight << 8));
                }
            }

            FocusBtlSoraCameraOnBgFx(work);

            if (BgFxIsActive() == 0) {
                SetBtlSoraState(work, 1);
                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        work->stateTimer++;
        break;
    }
    case 6: {
        BtlObj* e;

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        if ((s16)work->stateTimer == 0) {
            FocusBtlSoraCameraOnTarget(work);
            SetBtlSoraAnimation(work, 42, 0);
            m4aSongNumStart(SONG_VO_SR_FIRE00);
        }

        if ((s16)work->stateTimer == 27) {
            switch (work->variant[0]) {
            default:
                t3 = 68;
                break;
            case 0:
                t3 = 66;
                break;
            case 1:
                t3 = 67;
                break;
            }

            e = p->btl->actor2;

            if (e != NULL) {
                t4 = e->z - (e->centerHeight << 8);
            } else {
                t4 = p->z - 3584;
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartFire(work->variant[0], p->x - 18432, p->y, p->z - 3584,
                              p->originX - 51200, p->originY, t4, 1, t3);
            } else {
                BgFxStartFire(work->variant[0], p->x + 18432, p->y, p->z - 3584,
                              p->originX + 51200, p->originY, t4, 0, t3);
            }
        } else if ((s16)work->stateTimer > 27) {
            FocusBtlSoraCameraOnBgFx(work);

            if (BgFxIsActive() == 0) {
                SetBtlSoraState(work, 1);
                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        work->stateTimer++;
        break;
    }
    case 62:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        if ((s16)work->stateTimer == 0) {
            FocusBtlSoraCameraOnTarget(work);
            SetBtlSoraAnimation(work, 42, 0);
            m4aSongNumStart(SONG_VO_SR_FIRE00);
        }

        if ((s16)work->stateTimer == 27) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartFireBurst(p->x - 18432, p->y, p->z - 3584,
                              p->originX - 51200, p->originY,
                              p->z - 3584, 1, 96);
            } else {
                BgFxStartFireBurst(p->x + 18432, p->y, p->z - 3584,
                              p->originX + 51200, p->originY,
                              p->z - 3584, 0, 96);
            }
        } else if ((s16)work->stateTimer > 27) {
            FocusBtlSoraCameraOnBgFx(work);

            if (BgFxIsActive() == 0) {
                SetBtlSoraState(work, 1);
                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        work->stateTimer++;
        break;
    case 7: {
        s32 t;
        s32 t2;

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        if ((s16)work->stateTimer == 0) {
            FocusBtlSoraCameraOnTarget(work);
            SetBtlSoraAnimation(work, 42, 0);
            m4aSongNumStart(SONG_VO_SR_BURIZAD00);
        }

        if ((s16)work->stateTimer == 27) {
            switch (work->variant[0]) {
            case 0:
                t2 = 69;
                break;
            case 1:
                t2 = 70;
                break;
            default:
                t2 = 71;
                break;
            }

            t = p->z - 4096;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartBlizzard(work->variant[0], p->x - 18432, p->y, t,
                              p->originX - 23040, p->originY,
                              t, 1, t2);
            } else {
                BgFxStartBlizzard(work->variant[0], p->x + 18432, p->y, t,
                              p->originX + 23040, p->originY,
                              t, 0, t2);
            }
        } else if ((s16)work->stateTimer > 27) {
            FocusBtlSoraCameraOnBgFx(work);

            if (BgFxIsActive() == 0) {
                SetBtlSoraState(work, 1);
                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        work->stateTimer++;
    }

        break;
    case 5: {
        s32 uv;

        d = 0;
        FocusBtlSoraCameraOnTarget(work);
        uv = work->stateTimer;

        if ((s16)uv == 0) {
            EnableBtlSoraPassThrough(work);
            AnimReset(&work->anim);
            SetBtlSoraAnimation(work, 37, 0);

            if (work->comboCount == 0) {
                m4aSongNumStart(SONG_VO_SR_ATTACK08);
            }

            if (work->comboCount > 5) {
                m4aSongNumStart(SONG_VO_SR_ATTACK07);

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    SetBattleZoom(8, 384, p->x - 5120, (p->y - 5120) + p->z);
                } else {
                    SetBattleZoom(8, 384, p->x + 5120, (p->y - 5120) + p->z);
                }
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartLimit(p->x + 3072, p->y, p->z - 4096, 1);
            } else {
                BgFxStartLimit(p->x - 3072, p->y, p->z - 4096, 0);
            }

            m4aSongNumStart(SONG_EF_LIMIMOV);
            FadeToAmount(FADE_MODE_BLACK, gBtlWork->fadeAmount, 8);
        } else if ((u16)(uv - 22) <= 42) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x -= 1280;
            } else {
                p->x += 1280;
            }

            if (work->comboCount > 5) {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    d = ApplyAttackBox(85, p->x - 8192, p->y, p->z, 16, 16, 32);
                } else {
                    d = ApplyAttackBox(85, p->x + 8192, p->y, p->z, 16, 16, 32);
                }
            } else {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    d = ApplyAttackBox(84, p->x - 8192, p->y, p->z, 16, 16, 32);
                } else {
                    d = ApplyAttackBox(84, p->x + 8192, p->y, p->z, 16, 16, 32);
                }
            }

            if (d == 1) {
                m4aSongNumStart(SONG_BTL_LT_HIT00);
            }

            if (held & DPAD_UP) {
                p->y -= 384;
            } else if (held & DPAD_DOWN) {
                p->y += 384;
            }
        }

        if ((s16)work->stateTimer == 22) {
            if (work->comboCount > 5) {
                SetBattleZoom(15, 256, gBtlWork->x2, gBtlWork->y2);
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                func_080137C8(p->x - 15360, p->y, p->z, 0);
            } else {
                func_080137C8(p->x + 15360, p->y, p->z, 1);
            }
        }

        if (d == 2) {
            FadeToOriginal(FADE_MODE_BLACK, 8);
            SetBtlSoraState(work, 33);
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            DisableBtlSoraPassThrough(work);
            break;
        }

        if ((s16)work->stateTimer > 38 && (pressed & A_BUTTON) && work->comboCount <= 5) {
            p->flags ^= BTLOBJ_FLAG_FACING_LEFT;
            work->comboCount++;
            MakeOpponentsHittable();
            SelectLockonTarget();
            work->state = 5;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            FadeToOriginal(FADE_MODE_BLACK, 8);
            SetBtlSoraState(work, 1);
            DisableBtlSoraPassThrough(work);
            break;
        }

        work->stateTimer++;
        break;
    }
    case 17: {
        s32 uv;

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        switch ((s16)work->stateTimer) {
        case 0:
            work->flags &= ~BTL_SORA_FLAG_PASS_THROUGH;

            if (p->btl->hcEffect == 42
                && work->summonDesc != &gTaskDescSmnBambi
                && work->summonDesc != &gTaskDescSmnTink
                && work->summonDesc != &gTaskDescSmnMushu) {
                CreateBtlPopTask(p, 10);
                uv = (u16)p->maxHp;
                p->hp = ((s16)uv >> 2) + p->hp;

                if (p->hp > (s16)uv) {
                    p->hp = uv;
                }
            }

            work->stateTimer++;
            break;
        case 1:
            work->stateTimer++;
            break;
        case 2:
            spawn2.mainSide = work->mainSide;
            spawn2.variant = work->variant[0];
            p->originX = p->x;
            p->originY = p->y;
            p->originZ = p->z;
            TaskCreate(&gBtlWork->taskPools[0], work->summonDesc, &spawn2);
            work->state = 18;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }
    }

        break;
    case 23:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);
        st = work->stateTimer;

        if (st == 0) {
            p->btl->flags |= BTL_FLAG_PLAYER_OFFSCREEN;
            p->btl->actor2 = NULL;
            SetBtlSoraDirAnimation(work, 1, 0);

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                work->angle = 192;
            } else {
                work->angle = 64;
            }
        }

        uv = work->stateTimer;

        if ((s16)uv <= 3) {
            work->stateTimer = uv + 1;
            break;
        }

        work->state = 24;
        work->steps = 0;
        work->stateTimer = 0;
        work->vz = -1024;
        break;
    case 24:
        if ((s16)work->stateTimer == 0) {
            work->steps = 20;
            ColliderSetDisabled(&p->collider, 1);
            p->flags |= BTLOBJ_FLAG_IGNORE_BOUNDS;
            p->flags |= BTLOBJ_FLAG_NO_BREAK_POP;
            p->originZ = p->z;
            m4aSongNumStart(SONG_VO_SR_SUMMON00);
        }

        switch ((s16)work->stateTimer) {
        case 0:
            SetBtlSoraDirAnimation(work, 2, 0);
            break;
        case 10:
            SetBtlSoraDirAnimation(work, 3, 0);
            break;
        }

        if (p->originX <= 65535) {
            ApproachValue(&p->x, -8192, work->steps);
        } else {
            ApproachValue(&p->x, 139264, work->steps);
        }

        st = --work->steps;

        if (st == 0) {
            work->state = 25;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 25:
        if ((s16)work->stateTimer == 0) {
            work->flags |= BTL_SORA_FLAG_HIDDEN;
            ReleaseBtlSoraPalette(work);
            spawn.mainSide = work->mainSide;
            spawn.variant = work->variant[0];
            TaskCreate(&gBtlWork->taskPools[0], work->summonDesc, &spawn);
            p->x = p->originX;
            p->y = p->originY;
            p->z = -65536;
        }

        work->vz = 0;

        if (p->btl->flags & BTL_FLAG_SUMMON_ACTIVE) {
            work->stateTimer++;
            break;
        }

        work->flags &= ~BTL_SORA_FLAG_HIDDEN;
        LoadBtlSoraPalette(work);

        if (p->originX <= 65535) {
            p->x = -8192;
        } else {
            p->x = 139264;
        }

        p->z = p->originZ - 12800;
        p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;
        work->vz = 0;
        SetBtlSoraAnimation(work, 1, 1);
        work->state = 26;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case 26:
        if ((s16)work->stateTimer == 0) {
            work->steps = 20;
        }

        switch ((s16)work->stateTimer) {
        case 0:
            SetBtlSoraDirAnimation(work, 3, 0);
            break;
        case 10:
            SetBtlSoraDirAnimation(work, 4, 0);
            break;
        case 18:
            ColliderSetDisabled(&p->collider, 0);
            p->flags &= ~BTLOBJ_FLAG_IGNORE_BOUNDS;
            p->flags &= ~BTLOBJ_FLAG_NO_BREAK_POP;
            break;
        }

        if (work->steps > 0) {
            ApproachValueHalfSteps(&p->x, p->originX, work->steps);
            work->steps--;
        }

        if (!(p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE)) {
            work->state = 27;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 27: {
        u16 uv;
        s16 st;

        st = work->stateTimer;

        if (st == 0) {
            ColliderSetDisabled(&p->collider, 0);
            p->flags &= ~BTLOBJ_FLAG_IGNORE_BOUNDS;
            p->flags &= ~BTLOBJ_FLAG_NO_BREAK_POP;
            SetBtlSoraDirAnimation(work, 5, 0);
        }

        uv = work->stateTimer;

        if ((s16)uv > 6) {
            p->btl->flags &= ~BTL_FLAG_PLAYER_OFFSCREEN;
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer = uv + 1;
        }
    }

        break;
    case 38: {
        u16 uv;

        FocusBtlSoraCameraOnTarget(work);
        st = work->stateTimer;

        if (st == 0) {
            FadeFromAmount(FADE_MODE_RED, 4, 10);
            AnimReset(&work->anim);
            SetBtlSoraAnimation(work, 38, 0);
            p->flags |= BTLOBJ_FLAG_HURT;
            work->speed = 0;
            uv = p->hp;

            if ((s16)uv > 1) {
                p->hp = uv - 1;
            }

            switch (GetRandom() % 3) {
            case 0:
                m4aSongNumStart(SONG_VO_SR_DAMAGE00);
                break;
            case 1:
                m4aSongNumStart(SONG_VO_SR_DAMAGE02);
                break;
            case 2:
            default:
                m4aSongNumStart(SONG_VO_SR_DAMAGE03);
                break;
            }
        }

        uv = work->stateTimer;

        if ((s16)uv > 24) {
            p->flags &= ~BTLOBJ_FLAG_HURT;
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    }
    case 13:
        FocusBtlSoraCameraOnTarget(work);
        p->originX = p->x;
        p->originY = p->y;
        st = work->stateTimer;

        if (st == 0) {
            AnimReset(&work->anim);
            gBtlWork->hitStop = gBtlWork->pendingHitStop;
            StartBtlSoraKnockback(work);

            if (p->btl->hcEffect == 18) {
                SetBtlSoraAnimation(work, 39, 0);
                p->btl->hcEffectCount--;
                work->steps = 0;
            } else {
                SetBtlSoraAnimation(work, 38, 0);
                work->steps = 15;
            }
        } else if (st == 6) {
            switch (GetRandom() % 3) {
            case 0:
                m4aSongNumStart(SONG_VO_SR_DAMAGE00);
                break;
            case 1:
                m4aSongNumStart(SONG_VO_SR_DAMAGE02);
                break;
            case 2:
            default:
                m4aSongNumStart(SONG_VO_SR_DAMAGE03);
                break;
            }
        }

        if ((s16)work->stateTimer >= work->steps) {
            p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            p->originX = p->x;
            p->originY = p->y;
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 82:
        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    case 84:
        st = work->stateTimer;

        if (st == 0) {
            StartBtlSoraKnockback(work);
            work->flags |= BTL_SORA_FLAG_PASS_THROUGH;
            SetBtlSoraAnimation(work, 41, 0);
            m4aSongNumStart(SONG_VO_SR_DEATH00);
            work->speed = 0;
            FadeFromAmount(FADE_MODE_RED, 16, 60);
            work->stateTimer++;
            gBtlWork->hitStop = 30;
            p->flags &= ~BTLOBJ_FLAG_HURT;

            if (p->vx > 0) {
                p->flags |= BTLOBJ_FLAG_FACING_LEFT;
            } else if (p->vx < 0) {
                p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
            }
        } else {
            if (!(gBtlWork->flags & BTL_FLAG_VS_BATTLE)) {
                gBtlWork->hitStop = 3;
            }

            if (work->vz > 0) {
                work->vz = 0;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            FadeStartIn(FADE_MODE_ADD_WHITE, 30);
            p->btl->hcEffectCount--;
            p->invincibleTimer = 60;
            p->hp = p->maxHp / 4;
            p->flags &= ~BTLOBJ_FLAG_INTANGIBLE;
            ClearBtlObjActionFlags(p);
            CreateBtlPopTask(p, 10);

            if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
                work->state = 1;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }

            if (gBtlWork->enemyCount == 0 && gBtlWork->pendingEnemies <= 0) {
                work->state = 85;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }

            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 85: {
        u16 uv;

        SetBtlSoraAnimation(work, 1, 0);
        uv = work->stateTimer;

        if ((s16)uv > 60) {
            gBtlWork->flags |= BTL_FLAG_BATTLE_OVER;
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    }
    case 87:
        st = work->stateTimer;

        if (st == 0) {
            AnimReset(&work->anim);
            ColliderSetDisabled(&p->collider, 1);
            p->flags |= BTLOBJ_FLAG_INTANGIBLE;
            work->anim.frame = 0;
            work->anim.timer = 0;
            work->vz = 1024;
            p->vx = 0;
            p->vy = 0;
            work->steps = 10;
        }

        ApproachValue(&work->scaleY, 64, (* &work->steps)--);

        if (work->steps > 0) {
            work->stateTimer++;
        } else {
            work->stateTimer = 0;
            work->state = 88;
        }

        break;
    case 88:
        uv = work->stateTimer;

        if ((s16)uv > 44) {
            if (p->hp <= 0) {
                work->state = 14;
            } else {
                work->state = 89;
            }

            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    case 89:
        if ((s16)work->stateTimer == 0) {
            ColliderSetDisabled(&p->collider, 0);
            work->steps = 10;
        }

        ApproachValueHalfSteps(&work->scaleY, 256, (* &work->steps)--);

        if (work->steps > 0) {
            work->stateTimer++;
        } else {
            p->flags &= ~BTLOBJ_FLAG_INTANGIBLE;
            ClearBtlObjActionFlags(p);
            work->state = 1;
            work->stateTimer = 0;
        }

        break;
    case 14:
        FocusBtlSoraCameraOnTarget(work);
        st = work->stateTimer;

        if (st == 0) {
            gBtlWork->flags |= BTL_FLAG_PLAYER_DEFEATED;
            StartBtlSoraKnockback(work);
            work->flags |= BTL_SORA_FLAG_PASS_THROUGH;
            SetBtlSoraAnimation(work, 41, 0);
            m4aSongNumStart(SONG_VO_SR_DEATH00);
            work->speed = 0;

            if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
                FadeStartIn(FADE_MODE_ADD_WHITE, 20);
                FadeLock();
            } else {
                FadeFromAmount(FADE_MODE_RED, 16, 60);
            }

            work->stateTimer++;
            gBtlWork->hitStop = 30;
            p->flags &= ~BTLOBJ_FLAG_HURT;

            if (p->vx > 0) {
                p->flags |= BTLOBJ_FLAG_FACING_LEFT;
            } else if (p->vx < 0) {
                p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
            }

            break;
        }

        gBtlWork->hitStop = 3;

        if (work->vz > 0) {
            work->vz = 0;
        }

        break;
    case 50: {
        u16 uv;

        FocusBtlSoraCameraOnTarget(work);
        st = work->stateTimer;

        if (st == 0) {
            BtlObj* e;

            if (p->z < p->groundZ) {
                switch (work->comboCount % 3) {
                case 0:
                    SetBtlSoraAnimation(work, 23, 0);
                    break;
                case 1:
                    SetBtlSoraAnimation(work, 28, 0);
                    break;
                case 2:
                    SetBtlSoraAnimation(work, 33, 0);
                    break;
                }
            } else {
                switch (work->comboCount % 3) {
                case 0:
                    SetBtlSoraAnimation(work, 3, 0);
                    break;
                case 1:
                    SetBtlSoraAnimation(work, 8, 0);
                    break;
                case 2:
                    SetBtlSoraAnimation(work, 13, 0);
                    break;
                }
            }

            e = p->btl->actor2;

            if (e != NULL) {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    if (p->x < e->x) {
                        p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                    }
                } else {
                    if (p->x > e->x) {
                        p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                    }
                }
            }
        } else if (st == 5) {
            m4aSongNumStart(GetRandom() % 4 + SONG_VO_SR_ATTACK00);
        }

        uv = work->stateTimer;

        if (uv >= 9 && uv <= 11) {
            e = p->btl->actor2;

            if (uv == 15) {
                work->vz = -1152;
            }

            if (e != NULL) {
                p->x += (e->x - p->x) >> 3;
                p->y += (e->y - p->y) >> 3;
            } else if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                s32 t = p->x + 8192;
                p->x += (p->originX - t) >> 3;
            } else {
                s32 t = p->x - 8192;
                p->x += (p->originX - t) >> 3;
            }
        }

        if ((s16)work->stateTimer == 12) {
            MakeOpponentsHittable();

            if (work->comboCount == 1) {
                if ((p->flags & BTLOBJ_FLAG_FACING_LEFT)
                    ? ApplyAttackBox(89, p->x - 5120, p->y, p->z, 16, 16, 48) != 0
                    : ApplyAttackBox(89, p->x + 5120, p->y, p->z, 16, 16, 48) != 0) {
                    m4aSongNumStart(SONG_BTL_SR_ATT01);
                }
            } else {
                if ((p->flags & BTLOBJ_FLAG_FACING_LEFT)
                    ? ApplyAttackBox(88, p->x - 5120, p->y, p->z, 16, 16, 48) != 0
                    : ApplyAttackBox(88, p->x + 5120, p->y, p->z, 16, 16, 48) != 0) {
                    m4aSongNumStart(SONG_BTL_SR_ATT00);
                }
            }
        }

        {
            u16 uv = work->stateTimer;

            if ((s16)uv > 14) {
                work->comboCount--;
                p->originX = p->x;
                p->originY = p->y;
                p->originZ = p->z;

                if (work->comboCount == 0) {
                    work->state = work->nextState;
                    work->steps = 0;
                    work->stateTimer = 0;
                } else {
                    work->state = 50;
                    work->steps = 0;
                    work->stateTimer = 0;
                    work->steps = 1;
                }
            } else {
                work->stateTimer = uv + 1;
            }

            break;
        }
    }
    case 73:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        switch ((s16)work->stateTimer) {
        case 0:
            SetBtlSoraAnimation(work, 14, 0);
            FadeToAmount(FADE_MODE_BLACK, gBtlWork->fadeAmount, 8);
            m4aSongNumStart(SONG_VO_SR_ATTACK08);
            break;
        case 15:
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartShockwave(p->x - 5120, p->y + p->z - 4096, 1);
            } else {
                BgFxStartShockwave(p->x + 5120, p->y + p->z - 4096, 0);
            }

            m4aSongNumStart(SONG_EF_SHOCFLOR);
            break;
        case 20:
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                ApplyAttackBox(107, p->x - 32768, p->y, p->z, 160, 256, 256);
            } else {
                ApplyAttackBox(107, p->x + 32768, p->y, p->z, 160, 256, 256);
            }

            break;
        case 40:
            MakeOpponentsHittable();

            if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
                if (work->mainSide != 0) {
                    e = gRikuBtlWork->actor;
                } else {
                    e = gBtlWork->actor;
                }

                if (e->vx != 0 || e->vy != 0) {
                    ApplyAttackToBtlObj(108, e);
                }
            } else {
                e = ListPoolFirst(&gBtlWork->pool);

                while (e != NULL) {
                    if (e->vx != 0 || e->vy != 0) {
                        ApplyAttackToBtlObj(108, e);
                    }

                    e = ListPoolNext(&e->node);
                }

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    ApplyAttackBox(108, p->x - 32768, p->y, p->z, 160, 256, 256);
                } else {
                    ApplyAttackBox(108, p->x + 32768, p->y, p->z, 160, 256, 256);
                }
            }

            break;
        }

        uv = work->stateTimer;

        if ((s16)uv == 41) {
            FadeToOriginal(FADE_MODE_BLACK, 8);
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    case 56:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 71, 0);
            m4aSongNumStart(SONG_VO_SR_SUMMON05);
            work->unk_194 = 0;
        }

        if (work->anim.timer == 0) {
            switch (AnimGetFrame(&work->anim)) {
            case 4:
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    p->x -= 2560;
                } else {
                    p->x += 2560;
                }

                break;
            case 5:
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    p->x += 1024;
                } else {
                    p->x -= 1024;
                }

                break;
            case 6:
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    p->x += 1024;
                } else {
                    p->x -= 1024;
                }

                break;
            }
        }

        if (AnimGetFrame(&work->anim) > 4) {
            work->unk_194 += 128;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x -= work->unk_194;
            } else {
                p->x += work->unk_194;
            }

            if ((p->flags & BTLOBJ_FLAG_FACING_LEFT)
                ? ApplyAttackBox(94, p->x - 6400, p->y, p->z, 10, 12, 32) != 0
                : ApplyAttackBox(94, p->x + 6400, p->y, p->z, 10, 12, 32) != 0) {
                m4aSongNumStart(SONG_BTL_LT_HIT00);
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x -= 2048;
            } else {
                p->x += 2048;
            }

            work->state = 57;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 57: {
        u16 uv;

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 72, 0);
            EnableBtlSoraPassThrough(work);
        }

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            p->x -= work->unk_194;
        } else {
            p->x += work->unk_194;
        }

        if (held & DPAD_UP) {
            p->y -= 384;
        } else if (held & DPAD_DOWN) {
            p->y += 384;
        }

        if ((p->flags & BTLOBJ_FLAG_FACING_LEFT)
            ? ApplyAttackBox(94, p->x - 6400, p->y, p->z, 10, 12, 12) != 0
            : ApplyAttackBox(94, p->x + 6400, p->y, p->z, 10, 12, 12) != 0) {
            m4aSongNumStart(SONG_BTL_LT_HIT00);
        }

        uv = work->stateTimer;

        if ((s16)uv > 18) {
            work->state = 58;
            DisableBtlSoraPassThrough(work);
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    }
    case 58:
        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 73, 0);
            p->flags &= ~BTLOBJ_FLAG_IN_CARD_ACTION;
        }

        work->unk_194 -= 128;

        if (work->unk_194 < 0) {
            work->unk_194 = 0;
        }

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            p->x -= work->unk_194;
        } else {
            p->x += work->unk_194;
        }

        if (AnimGetFrame(&work->anim) == 1) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x += 128;
            } else {
                p->x -= 128;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->stateTimer = 0;
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer++;
        }

        break;
    case 59:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            m4aSongNumStart(SONG_VO_SR_SUMMON06);
            SetBtlSoraAnimation(work, 74, 0);
        }

        switch (AnimGetFrame(&work->anim)) {
        case 0:
            if (work->anim.timer == 0) {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    p->x += 768;
                } else {
                    p->x -= 768;
                }
            }

            break;
        case 2:
        case 3:
            if (work->anim.timer == 0) {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    p->x += 512;
                } else {
                    p->x -= 512;
                }
            }

            break;
        case 4:
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                s32 t = p->x + 16384;
                p->x += (p->originX - t) >> 4;
            } else {
                s32 t = p->x - 16384;
                p->x += (p->originX - t) >> 4;
            }

            break;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 60;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 60:
        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 75, 0);
            work->steps = 5;
        }

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            s32 t2 = p->x + 16384;
            p->x += (p->originX - t2) >> 4;
        } else {
            s32 t2 = p->x - 16384;
            p->x += (p->originX - t2) >> 4;
        }

        st = --work->steps;

        if (st <= 1) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartZantetsuken(p->x - 5120, p->y, p->z - 7680, 0);
            } else {
                BgFxStartZantetsuken(p->x + 5120, p->y, p->z - 7680, 1);
            }

            FadeStartIn(FADE_MODE_RED, 10);

            if ((p->flags & BTLOBJ_FLAG_FACING_LEFT)
                ? ApplyAttackBox(95, p->x - 6400, p->y, p->z, 10, 12, 12) != 0
                : ApplyAttackBox(95, p->x + 6400, p->y, p->z, 10, 12, 12) != 0) {
                m4aSongNumStart(SONG_EF_ZANTETSU);
                SetBtlSoraAnimation(work, 76, 0);
                work->state = 61;
                work->stateTimer = 0;
                break;
            }
        }

        if (work->steps <= 0) {
            work->state = 61;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 61:
        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 76, 0);
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->stateTimer = 0;
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer++;
        }

        break;
    case 52:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);
        FadeToAmount(FADE_MODE_BLACK, gBtlWork->fadeAmount, 8);
        work->state = 53;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case 53:
        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            AnimReset(&work->anim);
            SetBtlSoraAnimation(work, 70, 0);
        }

        if (AnimGetFrame(&work->anim) == 5 && work->anim.timer == 0) {
            work->vz = 4096;
        }

        switch (AnimGetFrame(&work->anim)) {
        case 1:
            if (work->anim.timer != 0) {
                break;
            }

            switch (work->comboCount) {
            case 0:
                work->vz = -1536;
                m4aSongNumStart(SONG_VO_SR_ATTACK07);
                break;
            case 1:
                work->vz = -640;
                m4aSongNumStart(SONG_VO_SR_ATTACK00);
                break;
            case 2:
            default:
                work->vz = -640;
                m4aSongNumStart(SONG_VO_SR_ATTACK01);
                break;
            }

            e = PickBtlSoraTarget(work);

            if (e != NULL) {
                work->unk_194 = e->x;
                work->unk_198 = e->y;
            } else {
                work->unk_194 = p->x;
                work->unk_198 = p->y;
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                work->angle = 192;
            } else {
                work->angle = 64;
            }

            if (p->x > work->unk_194) {
                p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                work->unk_194 += 5120;
            } else {
                p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                work->unk_194 -= 5120;
            }

            break;
        case 2:
        case 3:
        case 4:
        case 5:
            p->x += (work->unk_194 - p->x) >> 3;
            p->y += (work->unk_198 - p->y) >> 3;
            break;
        case 6:
            if (work->anim.timer % 6 == 0) {
                MakeOpponentsHittable();
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                if (ApplyAttackBox(93, p->x - 8192, p->y, p->z - 8192, 20, 25, 32) != 0) {
                    if (work->comboCount == 0) {
                        m4aSongNumStart(SONG_BTL_LT_HIT00);
                    } else {
                        m4aSongNumStart(SONG_BTL_SR_ATT02);
                    }
                }
            } else {
                if (ApplyAttackBox(93, p->x + 8192, p->y, p->z - 8192, 20, 25, 32) != 0) {
                    if (work->comboCount == 0) {
                        m4aSongNumStart(SONG_BTL_LT_HIT00);
                    } else {
                        m4aSongNumStart(SONG_BTL_SR_ATT02);
                    }
                }
            }

            break;
        case 7:
            if (p->z < p->groundZ) {
                work->anim.timer = 0;
                work->anim.frame--;
            }

            break;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            if (work->comboCount == 0) {
                SetBtlSoraState(work, 1);
                FadeToOriginal(FADE_MODE_BLACK, 8);
                break;
            }

            work->comboCount--;
            work->state = 53;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 51: {
        s32 t;
        u16 uv;

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 68, 0);
            EnableBtlSoraPassThrough(work);
            SetBattleZoom(4, 256, gBtlWork->x2, gBtlWork->y2);
        }

        uv = work->stateTimer;

        if (uv >= 15 && uv <= 39) {
            e = p->btl->actor2;

            if (uv == 15) {
                work->vz = -1152;
            }

            if ((s16)work->stateTimer == 22) {
                m4aSongNumStart(SONG_VO_SR_ATTACK07);
            }

            if (e != NULL) {
                p->x += (e->x - p->x) >> 3;
                p->y += (e->y - p->y) >> 3;
            } else if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                s32 t2 = p->x + 12288;
                p->x += (p->originX - t2) >> 3;
            } else {
                s32 t2 = p->x - 12288;
                p->x += (p->originX - t2) >> 3;
            }

            if ((s16)work->stateTimer == 38) {
                MakeOpponentsHittable();

                if ((p->flags & BTLOBJ_FLAG_FACING_LEFT)
                    ? ApplyAttackBox(90, p->x - 1024, p->y, p->z, 32, 24, 32) != 0
                    : ApplyAttackBox(90, p->x + 1024, p->y, p->z, 32, 24, 32) != 0) {
                    m4aSongNumStart(SONG_BTL_SR_ATT02);

                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        SetBattleZoom(8, 384, p->x - 2048, p->y + p->z);
                    } else {
                        SetBattleZoom(8, 384, p->x + 2048, p->y + p->z);
                    }

                    FadeStartIn(FADE_MODE_ADD_WHITE, 20);
                }
            } else if ((s16)work->stateTimer == 39) {
                SetBattleZoom(5, 256, gBtlWork->x2, gBtlWork->y2);
            }
        }

        t = AnimGetFrame(&work->anim);

        switch (t) {
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            work->vz = 0;
            break;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            DisableBtlSoraPassThrough(work);
            SetBtlSoraState(work, 1);
            break;
        }

        work->stateTimer++;
    }

        break;
    case 45:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);
        SetBtlSoraAnimation(work, 64, 0);

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 46;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    case 46:
        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                work->angle = 192;
            } else {
                work->angle = 64;
            }

            SetBtlSoraDirAnimation(work, 2, 0);
            work->vz = -896;
            m4aSongNumStart(SONG_VO_SR_SUMMON06);
        }

        if (work->vz >= 0) {
            work->state = 47;
            work->steps = 0;
            work->stateTimer = 0;
            p->originZ = p->z;
            work->unk_158 = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 47:
        FocusBtlSoraCamera(work);
        work->vz = 0;

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 65, 0);
        }

        {
            const s16* sine = gSineTable;
            u16 uv = work->unk_158;
            work->unk_158 = uv + 1;
            p->z += (p->originZ + (sine[(uv * 2) & 0xFF] << 3) - p->z) >> 3;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 48;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 48: {
        u16 uv;

        FocusBtlSoraCamera(work);
        work->vz = 0;

        if ((s16)work->stateTimer == 0) {
            FadeToAmount(FADE_MODE_BLACK, gBtlWork->fadeAmount, 8);
            SetBtlSoraAnimation(work, 66, 0);

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartRagnarokCharge(p->x - 10240, p->y, p->z - 6144);
            } else {
                BgFxStartRagnarokCharge(p->x + 10240, p->y, p->z - 6144);
            }

            work->steps = 120;
        }

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            BgFxSetPosition(p->x - 10240, p->y, p->z - 6144);
        } else {
            BgFxSetPosition(p->x + 10240, p->y, p->z - 6144);
        }

        if (held & DPAD_LEFT) {
            p->x -= 640;
        } else if (held & DPAD_RIGHT) {
            p->x += 640;
        }

        {
            const s16* sine = gSineTable;
            u16 uv = work->unk_158;
            work->unk_158 = uv + 1;
            p->z += (p->originZ + (sine[(uv * 2) & 0xFF] << 3) - p->z) >> 3;
        }

        if (held & DPAD_UP) {
            p->y -= 320;
        } else if (held & DPAD_DOWN) {
            p->y += 320;
        }

        uv = work->stateTimer;
        st = work->stateTimer;

        if ((st > 9 && (pressed & A_BUTTON)) || st > 120) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartRagnarokShot(p->x - 10240, p->y, p->z - 6144, 1);
            } else {
                BgFxStartRagnarokShot(p->x + 10240, p->y, p->z - 6144, 0);
            }

            work->state = 49;
            work->steps = 0;
            work->stateTimer = 0;
            p->originX = p->x;
            p->originY = p->y;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    }
    case 49: {
        FocusBtlSoraCamera(work);
        SetBtlSoraAnimation(work, 67, 0);
        work->vz = 0;

        {
            const s16* sine = gSineTable;
            u16 uv = work->unk_158;
            work->unk_158 = uv + 1;
            p->z += (p->originZ + (sine[(uv * 2) & 0xFF] << 3) - p->z) >> 3;
        }

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            s32 t = p->x - 8192;
            p->x += (p->originX - t) >> 3;
        } else {
            s32 t = p->x + 8192;
            p->x += (p->originX - t) >> 3;
        }

        if (AnimIsFinished(&work->anim) != 0 && BgFxIsActive() == 0) {
            FadeToOriginal(FADE_MODE_BLACK, 8);
            SetBtlSoraState(work, 3);
        } else {
            work->stateTimer++;
        }

        break;
    }
    case 64:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);
        SetBtlSoraAnimation(work, 64, 0);

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 65;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    case 65:
        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                work->angle = 192;
            } else {
                work->angle = 64;
            }

            SetBtlSoraDirAnimation(work, 2, 0);
            work->vz = -896;
            m4aSongNumStart(SONG_VO_SR_SUMMON06);
        }

        if (work->vz >= 0) {
            work->state = 66;
            work->steps = 0;
            work->stateTimer = 0;
            p->originZ = p->z;
            work->unk_158 = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 66:
        FocusBtlSoraCamera(work);
        work->vz = 0;

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 65, 0);
        }

        {
            const s16* sine = gSineTable;
            u16 uv = work->unk_158;
            work->unk_158 = uv + 1;
            p->z += (p->originZ + (sine[(uv * 2) & 0xFF] << 3) - p->z) >> 3;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 67;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 67: {
        s32 t;
        s32 t2;
        s32 t3;

        FocusBtlSoraCamera(work);
        work->vz = 0;
        st = work->stateTimer;

        if (st == 0) {
            SetBtlSoraAnimation(work, 66, 0);
            m4aSongNumStart(SONG_EF_DAMBO_SPLOOP);

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartDumboSplash(1, p->x - 9728, p->y, p->z - 6656, 0, 99);
            } else {
                BgFxStartDumboSplash(1, p->x + 9728, p->y, p->z - 6656, 1, 99);
            }
        }

#ifdef VERSION_EU
        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            BgFxSetPosition(p->x - 9728, p->y, p->z - 6656);
        } else {
            BgFxSetPosition(p->x + 9728, p->y, p->z - 6656);
        }
#else
        t = p->x;
        t2 = p->y;
        t3 = p->z;

        {
            const s16* sine = gSineTable;
            u16 uv = work->unk_158;
            work->unk_158 = uv + 1;
            p->z += (p->originZ + (sine[(uv * 2) & 0xFF] << 3) - p->z) >> 3;
        }

        if (held & DPAD_UP) {
            p->y -= 128;
        } else if (held & DPAD_DOWN) {
            p->y += 128;
        }

        if (held & DPAD_LEFT) {
            p->x -= 256;
        } else if (held & DPAD_RIGHT) {
            p->x += 256;
        }

        ClampBattlePosition(&p->x, &p->y, -16, 0);
        BgFxAddPosition(p->x - t, p->y - t2, p->z - t3);
#endif

        if (BgFxIsActive() == 0) {
            m4aSongNumStop(SONG_EF_DAMBO_SPLOOP);
            SetBtlSoraState(work, 3);
        } else {
            work->stateTimer++;
        }
    }

        break;
    case 63:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 21, 0);
            m4aSongNumStart(SONG_VO_SR_FIRE00);
        }

        if (work->anim.timer == 0) {
            switch (AnimGetFrame(&work->anim)) {
            case 3:
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    BgFxStartFireExplosion(p->x - 12288, p->y, p->z - 4096);
                } else {
                    BgFxStartFireExplosion(p->x + 12288, p->y, p->z - 4096);
                }

                m4aSongNumStart(SONG_EF_FIRE03);
                break;
            case 5:
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    ApplyAttackBox(97, p->x - 12288, p->y, p->z, 40, 40, 50);
                } else {
                    ApplyAttackBox(97, p->x + 12288, p->y, p->z, 40, 40, 50);
                }

                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0 && BgFxIsActive() == 0) {
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer++;
        }

        break;
    case 55:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 69, 0);
            m4aSongNumStart(SONG_VO_SR_ATTACK08);
            FadeToAmount(FADE_MODE_BLACK, gBtlWork->fadeAmount, 8);
        }

        if (work->anim.timer == 0) {
            switch (AnimGetFrame(&work->anim)) {
            case 2:
                BgFxStartLimit(p->x, p->y, p->z - 17920, 0);
                m4aSongNumStart(SONG_EF_LIMITST);
                break;
            case 5:
                BgFxStartStunImpact(p->x, p->y, p->z);
                m4aSongNumStart(SONG_EF_STANIMP);
                break;
            case 6:
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    ApplyAttackBox(92, p->x, p->y, p->z, 64, 64, 100);
                } else {
                    ApplyAttackBox(92, p->x, p->y, p->z, 64, 64, 100);
                }

                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0 && BgFxIsActive() == 0) {
            FadeToOriginal(FADE_MODE_BLACK, 8);
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer++;
        }

        break;
    case 71:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 69, 0);
            m4aSongNumStart(SONG_VO_SR_ATTACK08);
            FadeToAmount(FADE_MODE_BLACK, gBtlWork->fadeAmount, 8);
        }

        if (work->anim.timer == 0) {
            switch (AnimGetFrame(&work->anim)) {
            case 2:
                BgFxStartLimit(p->x, p->y, p->z - 17920, 0);
                m4aSongNumStart(SONG_EF_LIMITST);
                break;
            case 6:
                BtlMapStartShake();
                m4aSongNumStart(SONG_EF_KUEIK);
                FadeFromAmount(FADE_MODE_DARK_MAGENTA, 15, 30);
                MakeOpponentsHittable();
                ApplyAttackBox(106, p->x, p->y, p->z, 512, 512, 1);
                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0 && FadeIsActive() == 0) {
            FadeToOriginal(FADE_MODE_BLACK, 8);
            SetBtlSoraState(work, 1);
        } else {
            work->stateTimer++;
        }

        break;
    case 83:
        st = work->stateTimer;

        if (st == 0) {
            p->flags |= BTLOBJ_FLAG_INTANGIBLE;
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            work->speed = 0;
            work->flags |= BTL_SORA_FLAG_HIDDEN;
        }

        if ((s16)work->stateTimer > 285) {
            gBtlWork->flags |= 0x100000;
        } else {
            if (pressed & DPAD_ANY) {
                p->vx = GetRandom() % 257 - 128;
                p->vy = GetRandom() % 257 - 128;
                work->stateTimer += 2;
            }

            if (p->z >= p->groundZ && (pressed & B_BUTTON)) {
                work->vz = -256;
                work->stateTimer += 2;
            }

            if (work->stateTimer % 8 == 0) {
                u16 uv;

                uv = p->hp;

                if ((s16)uv > 1) {
                    p->hp = uv - 1;
                }
            }
        }

        uv = work->stateTimer;

        if ((s16)uv > 300) {
            p->flags &= ~BTLOBJ_FLAG_INTANGIBLE;
            p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
            work->flags &= ~BTL_SORA_FLAG_HIDDEN;
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    case 39:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCamera(work);

        switch ((s16)work->stateTimer) {
        case 0:
            FadeToAmount(FADE_MODE_BLACK, gBtlWork->fadeAmount, 8);
            m4aSongNumStart(SONG_VO_SR_SUMMON01);
            SetBtlSoraAnimation(work, 60, 0);
            break;
        case 15:
            BgFxStartLimit(p->x, p->y, p->z - 17920, 0);
            m4aSongNumStart(SONG_EF_LIMIMOV);
            break;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 40;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 40: {
        s32 t;
        s32 t2;

        FocusBtlSoraCamera(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 61, 0);
            work->steps = 20;
        }

        if (work->anim.timer == 0) {
            switch (AnimGetFrame(&work->anim)) {
            case 2:
                work->vz = -768;
                break;
            case 14:
                m4aSongNumStart(SONG_EF_TLIMIT02);
                break;
            }
        }

        if (AnimGetFrame(&work->anim) > 1 && work->steps > 0) {
            s32 target_x = 0x10000;

            t = (gBtlWork->yMin + gBtlWork->yMax) << 7;
            ApproachValueHalfSteps(&p->x, target_x, work->steps);
            ApproachValueHalfSteps(&p->y, t, work->steps);

            if (--work->steps == 0) {
                m4aSongNumStart(SONG_EF_TLIMIT01);
            }
        }

        if (p->z >= p->groundZ && work->steps == 0) {
            t2 = (gBtlWork->yMin + gBtlWork->yMax) << 7;

            if (p->groundZ != 0 || p->x != 0x10000 || p->y != t2) {
                gBtlWork->flags |= BTL_FLAG_STOP_BGFX;
                CreateBtlPopTask(p, 2);
                ClearBtlObjActionFlags(p);
                work->state = 1;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        }

        if ((s16)work->stateTimer == 18) {
            BgFxStartTrinityLimit(0x10000, (gBtlWork->yMin + gBtlWork->yMax) << 7, 0);
            p->flags |= 0x0000000400000000LL;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 41;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }
    }

        break;
    case 41:
        FocusBtlSoraCamera(work);

        switch ((s16)work->stateTimer) {
        case 0:
            SetBtlSoraAnimation(work, 62, 0);
            m4aSongNumStart(SONG_EF_TLIMIT03);
            break;
        case 30:
            BgFxStartTrinityLimitCharge(0x10000, ((gBtlWork->yMin + gBtlWork->yMax) << 7) + 512, 0);
            m4aSongNumStart(SONG_EF_TLIMIT04);
            p->flags &= ~0x0000000400000000LL;
            break;
        case 44:
            FadeToOriginal(FADE_MODE_BLACK, 8);
            break;
        }

        uv = work->stateTimer;

        if ((s16)uv > 59) {
            work->state = 42;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    case 42:
        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 63, 0);
            BgFxStartTrinityLimitBlast(0x10000, (gBtlWork->yMin + gBtlWork->yMax) << 7, -15872);
        }

        FocusBtlSoraCamera(work);

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlSoraAnimation(work, 1, 1);
        }

        if (BgFxIsActive() == 0) {
            ClearBtlObjActionFlags(p);
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 31:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlSoraCameraOnTarget(work);
            break;
        }

        FocusBtlSoraCameraOnTarget(work);

        switch ((s16)work->stateTimer) {
        case 0:
            SetBtlSoraAnimation(work, 54, 0);

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                BgFxStartLimit(p->x + 3072, p->y, p->z - 4096, 1);
            } else {
                BgFxStartLimit(p->x - 3072, p->y, p->z - 4096, 0);
            }

            m4aSongNumStart(SONG_EF_LIMIMOV);
            FadeToAmount(FADE_MODE_BLACK, gBtlWork->fadeAmount, 8);
            break;
        case 25:
            SetBtlSoraAnimation(work, 55, 0);
            break;
        case 37:
            args.mainSide = work->mainSide;
            args.variant = work->variant[0];
            args.y = p->y;
            args.z = p->z - 6144;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                args.facingLeft = 1;
                args.x = p->x - 6144;
            } else {
                args.facingLeft = 0;
                args.x = p->x + 6144;
            }

            work->task = TaskCreate(&work->tasks, &gTaskDescBtlRaid, &args);
            m4aSongNumStart(SONG_VO_SR_SUMMON06);
            break;
        case 67:
            SetBtlSoraAnimation(work, 56, 0);
            break;
        }

        if ((s16)work->stateTimer > 37 && IsTaskActiveNamed(work->task, gTaskDescBtlRaid.name) == 0) {
            work->state = 32;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 32:
        FocusBtlSoraCameraOnTarget(work);
        SetBtlSoraAnimation(work, 57, 0);

        if (AnimIsFinished(&work->anim) != 0) {
            FadeToOriginal(FADE_MODE_BLACK, 8);
            SetBtlSoraState(work, 1);
        }

        break;
    case 43:
        FocusBtlSoraCamera(work);
        work->vz = 0;

        if (p->badStatus != BAD_STATUS_STOP) {
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
            ClearBtlObjActionFlags(p);
            work->flags &= ~BTL_SORA_FLAG_PASS_THROUGH;
        } else {
            work->stateTimer++;
        }

        break;
    case 34:
        FocusBtlSoraCamera(work);
        st = work->stateTimer;

        if (st == 0) {
            AnimReset(&work->anim);
            SetBtlSoraAnimation(work, 40, 0);
            work->stateTimer++;
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            work->speed = 0;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            p->flags &= ~BTLOBJ_FLAG_HIT_LOCKED;
        }

        if (pressed & (A_BUTTON | B_BUTTON | DPAD_ANY)) {
            p->badStatusTimer -= 1;
        }

        if (p->badStatus != BAD_STATUS_STUN) {
            ClearBtlObjActionFlags(p);
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    case 36:
        FocusBtlSoraCameraOnTarget(work);
        st = work->stateTimer;

        if (st == 0) {
            m4aSongNumStart(SONG_BTL_SR_SLIP);
            p->vx = 0;
            p->vy = 0;
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            SetBtlSoraAnimation(work, 58, 0);
            work->speed = 256;
        } else {
            p->x += (gSineTable[work->angle] * work->speed) >> 8;
        }

        p->y += (-gSineTable[work->angle + 64] * (work->speed >> 1)) >> 8;
        work->speed -= 10;

        if (work->speed < 0) {
            work->speed = 0;
        }

        uv = work->stateTimer;

        if ((s16)uv > 40) {
            work->state = 37;
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    case 37:
        FocusBtlSoraCameraOnTarget(work);

        if ((s16)work->stateTimer == 0) {
            SetBtlSoraAnimation(work, 59, 0);
            work->speed = 256;
        }

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            p->x -= work->speed;
        } else {
            p->x += work->speed;
        }

        work->speed -= 12;

        if (work->speed < 0) {
            work->speed = 0;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
            work->speed = 0;
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 35:
        p->flags &= ~BTLOBJ_FLAG_HURT;
        p->vx = p->vy = 0;
        work->speed = 0;
        break;
    }

    if ((u16)(pressed & DPAD_LEFT) != 0) {
        work->tapTimers[0] = 13;
        work->tapTimers[1] = 0;
    } else if (pressed & DPAD_RIGHT) {
        work->tapTimers[1] = 13;
        work->tapTimers[0] = 0;
    }

    if (work->tapTimers[1] != 0) {
        work->tapTimers[1]--;
    }

    if (work->tapTimers[0] != 0) {
        work->tapTimers[0]--;
    }

    if (p->badStatus == BAD_STATUS_BIND) {
        work->tapTimers[0] = 0;
        work->tapTimers[1] = 0;
        work->speed = 0;
        p->vx = p->vy = 0;

        if (work->vz < 0) {
            work->vz = 768;
        }
    } else {
        p->x += (gSineTable[work->angle] * work->speed) >> 8;
        p->y += (-gSineTable[work->angle + 64] * (work->speed >> 1)) >> 8;
    }

    if (p->collider.colliding != 0) {
        if (p->collider.otherType == 12) {
            if (work->speed > 0 && work->state == 1) {
                work->state = 36;
                work->steps = 0;
                work->stateTimer = 0;
            }
        } else if ((work->flags & BTL_SORA_FLAG_ON_PLATFORM) && p->collider.otherType == 7) {
            p->collider.standFlags |= COLLIDER_STAND_OVER_PLATFORM;
        } else if ((work->flags & BTL_SORA_FLAG_PASS_THROUGH) == 0 && p->collider.otherType != 5 &&
                   (p->collider.other->flags & COLLIDER_FLAG_PASS_THROUGH) == 0) {
            p->x += p->collider.pushX >> 1;
            p->y += p->collider.pushY >> 1;
        }
    }

    p->z += work->vz;
    work->vz += gBtlWork->gravity;

    if (p->collider.standFlags & COLLIDER_STAND_OVER_PLATFORM) {
        p->groundZ = p->collider.platformZ;
        work->flags |= BTL_SORA_FLAG_OVER_PLATFORM;
        work->platformPriority = -4100 - (((p->collider.platformY + 1024) >> 8) * 4);
    } else {
        work->flags &= ~BTL_SORA_FLAG_OVER_PLATFORM;
        p->groundZ = p->floorZ;
    }

    if (work->flags & BTL_SORA_FLAG_ON_PLATFORM) {
        if (gBtlWork->platform == p->collider.other) {
            p->x += p->collider.platformX - work->platformX;
            p->y += p->collider.platformY - work->platformY;
            p->z += p->collider.platformZ - work->platformZ;
        }
    }

    if (p->z >= p->groundZ) {
        if (p->collider.standFlags & COLLIDER_STAND_OVER_PLATFORM) {
            work->flags |= BTL_SORA_FLAG_ON_PLATFORM;
            work->platformX = p->collider.platformX;
            work->platformY = p->collider.platformY;
            work->platformZ = p->collider.platformZ;
            gBtlWork->platform = p->collider.other;
        } else {
            work->flags &= ~BTL_SORA_FLAG_ON_PLATFORM;
            gBtlWork->platform = NULL;
        }

        work->vz = 0;
        p->z = p->groundZ;
        p->btl->flags &= ~BTL_FLAG_PLAYER_AIRBORNE;

        if (work->state == 3) {
            work->state = 4;
            work->steps = 0;
            work->stateTimer = 0;
        } else if (work->state == 20) {
            work->state = 21;
            work->steps = 0;
            work->stateTimer = 0;
        }
    } else {
        if (work->flags & BTL_SORA_FLAG_ON_PLATFORM) {
            work->flags &= ~BTL_SORA_FLAG_ON_PLATFORM;

            if (work->state == 1) {
                work->state = 3;
                work->steps = 0;
                work->stateTimer = 0;
            } else if (work->state == 18) {
                work->state = 20;
                work->steps = 0;
                work->stateTimer = 0;
            }
        }

        gBtlWork->platform = NULL;
    }

    if (p->vx > 0) {
        p->x += p->vx;
        p->vx -= 17;

        if (p->vx < 0) {
            p->vx = 0;
        }
    } else if (p->vx < 0) {
        p->x += p->vx;
        p->vx += 17;

        if (p->vx > 0) {
            p->vx = 0;
        }
    }

    if (p->vy > 0) {
        p->y += p->vy;
        p->vy -= 17;

        if (p->vy < 0) {
            p->vy = 0;
        }
    } else if (p->vy < 0) {
        p->y += p->vy;
        p->vy += 17;

        if (p->vy > 0) {
            p->vy = 0;
        }
    }

    if ((p->flags & BTLOBJ_FLAG_IGNORE_BOUNDS) == 0) {
        switch (ClampBattlePosition(&p->x, &p->y, -16, 0)) {
        case 3:
        case 4:
            p->vy = -(p->vy >> 1);
            p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
            break;
        case 1:
            p->vx = -(p->vx >> 1);

            if (p->z == 0 && (held & DPAD_LEFT)) {
                p->btl->flags |= BTL_FLAG_PUSHING_EDGE;
            } else {
                p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
            }

            work->flags |= BTL_SORA_FLAG_AT_SIDE_EDGE;
            break;
        case 2:
            p->vx = -(p->vx >> 1);

            if (p->z == 0 && (held & DPAD_RIGHT)) {
                p->btl->flags |= BTL_FLAG_PUSHING_EDGE;
            } else {
                p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
            }

            work->flags |= BTL_SORA_FLAG_AT_SIDE_EDGE;
            break;
        default:
            p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
            work->flags &= ~BTL_SORA_FLAG_AT_SIDE_EDGE;
            break;
        }

        if (p->btl->flags & BTL_FLAG_SUMMON_ACTIVE) {
            p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
        }

        if (p->btl->flags & BTL_FLAG_PUSHING_EDGE) {
            if (work->mainSide != 0) {
                FocusBtlSoraCamera(work);
            }
        }
    }

    ApplyBattleBounds(&p->x, &p->y, &p->z, &p->floorZ);
    TaskPoolUpdate(&work->tasks);

    if (work->state == 29 && (work->flags & BTL_SORA_FLAG_HIDDEN)) {
        work->flags &= ~BTL_SORA_FLAG_HIDDEN;
        LoadBtlSoraPalette(work);
        SetBtlSoraAnimation(work, work->unk_191[0] + 47, 0);
        p->x = p->originX;
        p->y = p->originY;
        p->z = p->originZ;
    }

    if (p->badStatus != BAD_STATUS_STOP) {
        work->gfx = AnimUpdate(&work->anim);
    }

    ColliderSetPosition(&p->collider, p->x, p->y, p->z);
    work->frameCount++;
    return 1;
}

void task_btl_sora_2(BtlSoraWork* work) {
    BtlObj* p;
    ObjAffine* affine;
    s32 sx;
    s32 sy;
    u16 attr;
    u16 attr2;
    s16 x;
    s16 y;

    p = &work->actor;

    if (work->flags & BTL_SORA_FLAG_HIDDEN) {
        return;
    }

#ifndef VERSION_EU
    if (work->actor.btl->hcEffect == 19) {
        if (work->mainSide != 0) {
            if (gFrameCounter & 1) {
                return;
            }
        } else if (gFrameCounter % 120 <= 59) {
            return;
        }
    }
#endif

    attr = GetBattleSpritePriorityFlags(p->y);

    if (work->scaleX == 0x100 && work->scaleY == 0x100) {
        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            sy = gBtlWork->scale;
            sx = sy;
        } else {
            sy = gBtlWork->scale;

            if (sy == 0x100) {
                sx = sy;
                attr |= 1;
            } else {
                sx = -sy;
            }
        }
    } else {
        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            sx = gBtlWork->scale * work->scaleX >> 8;
            sy = gBtlWork->scale * work->scaleY >> 8;
        } else {
            sx = -(gBtlWork->scale * work->scaleX >> 8);
            sy = gBtlWork->scale * work->scaleY >> 8;
        }
    }

    if (sy == 0x100 && sx == 0x100) {
        affine = NULL;
    } else if (sy <= 255) {
        affine = AllocObjAffine(0, sx, sy, 0);
    } else {
        affine = AllocObjAffine(0, sx, sy, 1);
    }

    if (work->flags & BTL_SORA_FLAG_OVER_PLATFORM) {
        attr2 = work->platformPriority | 1;

        if (p->collider.penetration <= p->collider.radius) {
            if (p->groundZ != 0) {
                p->shadowPriority = 0;
            } else {
                p->shadowPriority = 0xEFFF;
            }
        } else {
            p->shadowPriority = work->platformPriority | 2;
        }
    } else {
        attr2 = (-0x1004 - ((p->y >> 8) << 2)) | 1;
        p->shadowPriority = 0xEFFF;
    }

    WorldToScreen(&x, &y, p->x, p->y, p->z);

    if (StepHitFlash(p) != 0) {
        u16 t = work->flags | BTL_SORA_FLAG_HIT_FLASH;

        work->flags = t;
        LoadObjPaletteBank(work->palette->index, gUnk_08F69BC4);
    } else if (work->flags & BTL_SORA_FLAG_HIT_FLASH) {
        u16 t = work->flags & ~BTL_SORA_FLAG_HIT_FLASH;

        work->flags = t;

        if (work->mainSide != 0) {
            LoadObjPaletteBank(work->palette->index, gSoraPalette);
        } else {
            LoadObjPaletteBank(work->palette->index, gUnk_096FAC64);
        }
    }

#ifdef VERSION_EU
    if (p->btl->hcEffect == 19) {
        if (work->mainSide != 0) {
            if (gFrameCounter & 1) {
                return;
            }
        } else if (gFrameCounter % 120 <= 59) {
            return;
        }
    }
#endif

    DrawSprite(x, y, work->gfx, work->tiles, work->palette, affine, attr, attr2);
    TaskPoolDraw(&work->tasks);
}

void task_btl_sora_3(BtlSoraWork* work) {
    BtlObj* p;

    p = &work->actor;
    m4aSongNumStop(SONG_EF_DAMBO_SPLOOP);

    if (!(gBtlWork->flags & BTL_FLAG_VS_BATTLE)) {
        if (gBtlWork->phase == 3) {
            gGameState.hp = gGameState.progression.maxHp;
        } else {
            gGameState.hp = p->hp;
        }
    }

    ColliderUnregister(&p->collider);
    ReleaseBtlSoraPalette(work);
    TaskPoolDestroy(&work->tasks);
}

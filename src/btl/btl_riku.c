#include "task_descriptors.h"
#include "card_battle.h"
#include "engine_math.h"
#include "display.h"
#include "obj_api.h"
#include "btl.h"
#include "btl_effect.h"
#include "btl_api.h"
#include "sprites_btl.h"
#include "sprites_evt.h"
#include "sprites_fld.h"
#include "sprites_hum.h"
#include "sprites_riku.h"
#include "gba/keys.h"
#include "gba/io_reg.h"
#include "system_state.h"
#include "player_progression.h"
#include "fade.h"
#include "songs.h"
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

static const AnimDef sBtlRikuAnimDefs[35] = {
    { gRikuBt00Frames, gRikuBt00Anims, gRikuBt00Tiles, 0, { 0, 0, 0 } },
    { gRikuBt11Frames, gRikuBt11Anims, gRikuBt11Tiles, 1, { 0, 0, 0 } },
    { gRikuBt10Frames, gRikuBt10Anims, gRikuBt10Tiles, 3, { 0, 0, 0 } },
    { gRikuBt11Frames, gRikuBt11Anims, gRikuBt11Tiles, 2, { 0, 0, 0 } },
    { gRikuBt13Frames, gRikuBt13Anims, gRikuBt13Tiles, 0, { 0, 0, 0 } },
    { gRikuBt11Frames, gRikuBt11Anims, gRikuBt11Tiles, 2, { 0, 0, 0 } },
    { gRikuLl17Frames, gRikuLl17Anims, gRikuLl17Tiles, 1, { 0, 0, 0 } },
    { gRikuBt02Frames, gRikuBt02Anims, gRikuBt02Tiles, 0, { 0, 0, 0 } },
    { gRikuBt02Frames, gRikuBt02Anims, gRikuBt02Tiles, 1, { 0, 0, 0 } },
    { gRikuBt04Frames, gRikuBt04Anims, gRikuBt04Tiles, 0, { 0, 0, 0 } },
    { gRikuBt05Frames, gRikuBt05Anims, gRikuBt05Tiles, 0, { 0, 0, 0 } },
    { gUnk_09EDF3E8, gUnk_09EDF400, gUnk_08925B44, 0, { 0, 0, 0 } },
    { gNiserikuIdolFrames, gNiserikuIdolAnims, gNiserikuIdolTiles, 0, { 0, 0, 0 } },
    { gNiserikuRunFrames, gNiserikuRunAnims, gNiserikuRunTiles, 0, { 0, 0, 0 } },
    { gNiserikuDamageFrames, gNiserikuDamageAnims, gNiserikuDamageTiles, 0, { 0, 0, 0 } },
    { gNiserikuJumpFrames, gNiserikuJumpAnims, gNiserikuJumpTiles, 0, { 0, 0, 0 } },
    { gNiserikuJumpFrames, gNiserikuJumpAnims, gNiserikuJumpTiles, 1, { 0, 0, 0 } },
    { gNiserikuJumpFrames, gNiserikuJumpAnims, gNiserikuJumpTiles, 2, { 0, 0, 0 } },
    { gNiserikuJumpFrames, gNiserikuJumpAnims, gNiserikuJumpTiles, 3, { 0, 0, 0 } },
    { gNiserikuJumpFrames, gNiserikuJumpAnims, gNiserikuJumpTiles, 4, { 0, 0, 0 } },
    { gNiserikuDashFrames, gNiserikuDashAnims, gNiserikuDashTiles, 0, { 0, 0, 0 } },
    { gNiserikuFuriharaiFrames, gNiserikuFuriharaiAnims, gNiserikuFuriharaiTiles, 0, { 0, 0, 0 } },
    { gNiserikuTategiriFrames, gNiserikuTategiriAnims, gNiserikuTategiriTiles, 0, { 0, 0, 0 } },
    { gNiserikuKabutoFrames, gNiserikuKabutoAnims, gNiserikuKabutoTiles, 0, { 0, 0, 0 } },
    { gNiserikuKabutoFrames, gNiserikuKabutoAnims, gNiserikuKabutoTiles, 2, { 0, 0, 0 } },
    { gNiserikuDarkfigaFrames, gNiserikuDarkfigaAnims, gNiserikuDarkfigaTiles, 0, { 0, 0, 0 } },
    { gNiserikuYamiStartFrames, gNiserikuYamiStartAnims, gNiserikuYamiStartTiles, 0, { 0, 0, 0 } },
    { gNiserikuYamiStartFrames, gNiserikuYamiStartAnims, gNiserikuYamiStartTiles, 1, { 0, 0, 0 } },
    { gNiserikuYamiStartFrames, gNiserikuYamiStartAnims, gNiserikuYamiStartTiles, 2, { 0, 0, 0 } },
    { gNiserikuYamiIngFrames, gNiserikuYamiIngAnims, gNiserikuYamiIngTiles, 0, { 0, 0, 0 } },
    { gNiserikuYamiIngFrames, gNiserikuYamiIngAnims, gNiserikuYamiIngTiles, 1, { 0, 0, 0 } },
    { gNiserikuYamiIngFrames, gNiserikuYamiIngAnims, gNiserikuYamiIngTiles, 2, { 0, 0, 0 } },
    { gNiserikuYamiEndFrames, gNiserikuYamiEndAnims, gNiserikuYamiEndTiles, 0, { 0, 0, 0 } },
    { gNiserikuDashFrames, gNiserikuDashAnims, gNiserikuDashTiles, 1, { 0, 0, 0 } },
    { gNiserikuDashFrames, gNiserikuDashAnims, gNiserikuDashTiles, 3, { 0, 0, 0 } },
};

static const AnimDef sBtlRikuDirAnimDefs[6][5] = {
    { { gRik1ff02Frames, gRik1ff02Anims, gRik1ff02Tiles, 0, { 0, 0, 0 } }, { gRik1bb02Frames, gRik1bb02Anims, gRik1bb02Tiles, 0, { 0, 0, 0 } }, { gRik1fl02Frames, gRik1fl02Anims, gRik1fl02Tiles, 0, { 0, 0, 0 } }, { gRik1ll02Frames, gRik1ll02Anims, gRik1ll02Tiles, 0, { 0, 0, 0 } }, { gRik1bl02Frames, gRik1bl02Anims, gRik1bl02Tiles, 0, { 0, 0, 0 } } },
    { { gRik1ff03Frames, gRik1ff03Anims, gRik1ff03Tiles, 0, { 0, 0, 0 } }, { gRik1bb03Frames, gRik1bb03Anims, gRik1bb03Tiles, 0, { 0, 0, 0 } }, { gRik1fl03Frames, gRik1fl03Anims, gRik1fl03Tiles, 0, { 0, 0, 0 } }, { gRik1ll03Frames, gRik1ll03Anims, gRik1ll03Tiles, 0, { 0, 0, 0 } }, { gRik1bl03Frames, gRik1bl03Anims, gRik1bl03Tiles, 0, { 0, 0, 0 } } },
    { { gRik1ff03Frames, gRik1ff03Anims, gRik1ff03Tiles, 1, { 0, 0, 0 } }, { gRik1bb03Frames, gRik1bb03Anims, gRik1bb03Tiles, 1, { 0, 0, 0 } }, { gRik1fl03Frames, gRik1fl03Anims, gRik1fl03Tiles, 1, { 0, 0, 0 } }, { gRik1ll03Frames, gRik1ll03Anims, gRik1ll03Tiles, 1, { 0, 0, 0 } }, { gRik1bl03Frames, gRik1bl03Anims, gRik1bl03Tiles, 1, { 0, 0, 0 } } },
    { { gRik1ff03Frames, gRik1ff03Anims, gRik1ff03Tiles, 2, { 0, 0, 0 } }, { gRik1bb03Frames, gRik1bb03Anims, gRik1bb03Tiles, 2, { 0, 0, 0 } }, { gRik1fl03Frames, gRik1fl03Anims, gRik1fl03Tiles, 2, { 0, 0, 0 } }, { gRik1ll03Frames, gRik1ll03Anims, gRik1ll03Tiles, 2, { 0, 0, 0 } }, { gRik1bl03Frames, gRik1bl03Anims, gRik1bl03Tiles, 2, { 0, 0, 0 } } },
    { { gRik1ff03Frames, gRik1ff03Anims, gRik1ff03Tiles, 3, { 0, 0, 0 } }, { gRik1bb03Frames, gRik1bb03Anims, gRik1bb03Tiles, 3, { 0, 0, 0 } }, { gRik1fl03Frames, gRik1fl03Anims, gRik1fl03Tiles, 3, { 0, 0, 0 } }, { gRik1ll03Frames, gRik1ll03Anims, gRik1ll03Tiles, 3, { 0, 0, 0 } }, { gRik1bl03Frames, gRik1bl03Anims, gRik1bl03Tiles, 3, { 0, 0, 0 } } },
    { { gRik1ff03Frames, gRik1ff03Anims, gRik1ff03Tiles, 4, { 0, 0, 0 } }, { gRik1bb03Frames, gRik1bb03Anims, gRik1bb03Tiles, 4, { 0, 0, 0 } }, { gRik1fl03Frames, gRik1fl03Anims, gRik1fl03Tiles, 4, { 0, 0, 0 } }, { gRik1ll03Frames, gRik1ll03Anims, gRik1ll03Tiles, 4, { 0, 0, 0 } }, { gRik1bl03Frames, gRik1bl03Anims, gRik1bl03Tiles, 4, { 0, 0, 0 } } },
};

static const u16 sBtlRikuGroundSongs[4][4] = {
    { SONG_BTL_SR_FOOTL, SONG_BTL_SR_FOOTR, SONG_BTL_SR_JUMP, SONG_BTL_SR_LAND },
    { SONG_BTL_SR_STONEL, SONG_BTL_SR_STONER, SONG_BTL_SR_STONEJP, SONG_BTL_SR_STONELD },
    { SONG_BTL_SR_MUDL, SONG_BTL_SR_MUDR, SONG_BTL_SR_MUDJP, SONG_BTL_SR_MUDLD },
    { SONG_SYS_SR_STONEL, SONG_SYS_SR_STONER, SONG_SYS_SR_STONEJP, SONG_SYS_SR_STONELD },
};

static const s32 sBtlRikuAttackIds[3] = {
    0, 1, 2,
};

static const RikuAttackDef sUnk_0813C6E8 = { 1, 15, 0, sBtlRikuAttackIds, 255, SONG_BTL_RK_HIT00, 0, 0, 0, NULL };

static const RikuAttackDef sUnk_0813C704 = { 2, 17, 0, &sBtlRikuAttackIds[1], 254, SONG_BTL_RK_HIT01, 0, 0, 0, NULL };

static const RikuAttackDef sUnk_0813C720 = { 3, 15, 0, sBtlRikuAttackIds, 256, SONG_BTL_RK_HIT00, 0, 0, 0, NULL };

static const RikuAttackDef sUnk_0813C73C = { 4, 21, 0, &sBtlRikuAttackIds[2], 258, SONG_BTL_RK_HIT02, 0, 0, 0, NULL };

static const RikuAttackDef sUnk_0813C758 = { 6, 15, 0, sBtlRikuAttackIds, 256, SONG_BTL_RK_HIT01, -640, COMBO_FLAG_AERIAL_SWING, 0, &sUnk_0813C6E8 };

static const RikuAttackDef sUnk_0813C774 = { 5, 15, 0, &sBtlRikuAttackIds[1], 254, SONG_BTL_RK_HIT00, 0, COMBO_FLAG_AERIAL_SWING, 0, &sUnk_0813C6E8 };

static const RikuAttackDef sUnk_0813C790 = { 5, 15, 0, sBtlRikuAttackIds, 255, SONG_BTL_RK_HIT01, 0, COMBO_FLAG_AERIAL_SWING, 0, &sUnk_0813C704 };

static const RikuAttackDef sUnk_0813C7AC = { 6, 15, 0, &sBtlRikuAttackIds[2], 257, SONG_BTL_RK_HIT02, 0, COMBO_FLAG_AERIAL_SWING, 0, &sUnk_0813C73C };

void EnableBtlRikuPassThrough(BtlRikuWork* work) {
    u16 a = work->flags | BTL_RIKU_FLAG_PASS_THROUGH;
    u16 b;

    work->flags = a;
    b = work->actor.collider.flags | COLLIDER_FLAG_PASS_THROUGH;
    work->actor.collider.flags = b;
}

void DisableBtlRikuPassThrough(BtlRikuWork* work) {
    u16 a = work->flags & ~BTL_RIKU_FLAG_PASS_THROUGH;
    u16 b;

    work->flags = a;
    b = work->actor.collider.flags & ~COLLIDER_FLAG_PASS_THROUGH;
    work->actor.collider.flags = b;
}

u16 GetBtlRikuComboType(BtlRikuWork* work) {
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

void FocusBtlRikuCameraOnTarget(BtlRikuWork* work) {
    BtlObj* c;

    if (work->mainSide == 0) {
        return;
    }

    c = work->actor.btl->actor2;

    if (c != NULL) {
        BtlMapFollowPosition((work->actor.x + c->x) >> 1, (work->actor.y + c->y) >> 1,
                      (work->actor.z + c->z) >> 1);
    } else {
        BtlMapFollowPosition(work->actor.x, work->actor.y, work->actor.z);
    }
}

void FocusBtlRikuCameraOnBgFx(BtlRikuWork* work) {
    s32 x;
    s32 y;
    s32 z;

    if (work->mainSide != 0) {
        BgFxGetPosition(&x, &y, &z);
        BtlMapFollowPosition(x, gBtlWork->actor->y, gBtlWork->actor->z);
    }
}

void SaveBtlRikuAfterimage(BtlRikuWork* work, BtlDrawInfo* out) {
    BtlObj* a;

    a = &work->actor;
    out->x = a->x;
    out->y = a->y;
    out->z = a->z;

    if (a->flags & BTLOBJ_FLAG_FACING_LEFT) {
        out->flags |= BTL_DRAW_INFO_FLAG_FACING_LEFT;
    } else {
        out->flags &= ~BTL_DRAW_INFO_FLAG_FACING_LEFT;
    }

    out->anim = work->anim;
    out->tileSrc = work->tiles2->src;
    out->scale = gBtlWork->scale;
}

void DrawBtlRikuAfterimage(BtlRikuWork* work, BtlDrawInfo* out) {
    BtlObj* a;
    void* gfx;
    u16 flags;
    ObjAffine* affine;
    s32 p;
    s32 q;
    s16 x;
    s16 y;
    s32 z;
    s32 v;

    gfx = AnimGetGfx(&out->anim);
    a = &work->actor;

    if (BgFxIsActive() == 0) {
        gBldCnt = (BLDCNT_TGT1_OBJ | BLDCNT_TGT2_BG0 | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3);
        SetBlendAlpha(6, 12);
        flags = SPRITE_PRIORITY(2) | SPRITE_FLAG_BLEND;
    } else {
        flags = GetBattleSpritePriorityFlags(a->y);
    }

    if (out->flags & BTL_DRAW_INFO_FLAG_FACING_LEFT) {
        p = out->scale;
        q = p;
    } else {
        p = out->scale;

        if (p == 256) {
            q = p;
            flags |= SPRITE_FLAG_HFLIP;
        } else {
            v = gBtlWork->scale;
            q = -v;
            p = v;
        }
    }

    if (p == 256 && q == p) {
        affine = NULL;
    } else if (p <= 255) {
        affine = AllocObjAffine(0, q, p, 0);
    } else {
        affine = AllocObjAffine(0, q, p, 1);
    }

    z = 0xFFF0;
    WorldToScreen(&x, &y, out->x, out->y, out->z);
    SetObjTileSource(work->tiles, out->tileSrc);
    DrawSprite(x, y, gfx, work->tiles, work->palette, affine, flags, z);
}

void SetBtlRikuAnimation(BtlRikuWork* work, u16 a, u16 b) {
    const FldAnimDef* e;

    e = &sBtlRikuAnimDefs[a];
    AnimChangeWithTables(&work->anim, e->animId, b, e->anims, e->gfxTable);
    SetObjTileSource(work->tiles2, e->tiles);
}

void SetBtlRikuDirAnimation(BtlRikuWork* work, u16 a, u16 b) {
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

    e = &sBtlRikuDirAnimDefs[a][idx];
    AnimChangeWithTables(&work->anim, e->animId, b, e->anims, e->gfxTable);
    SetObjTileSource(work->tiles2, e->tiles);
}

void LoadBtlRikuPalette(BtlRikuWork* work) {
    work->tiles2 = work->actor.btl->tiles;

    if (work->mainSide != 0) {
        work->palette = LoadObjPalette(work->paletteData, 0x20);
    } else {
        work->palette = LoadObjPalette(gUnk_096FAC64, 0x20);
    }
}

void ReleaseBtlRikuPalette(BtlRikuWork* work) {
    ReleaseObjPalette(work->palette);
    work->tiles2 = NULL;
    work->palette = NULL;
}

void UpdateBtlRikuWalk(BtlRikuWork* work, u16 a) {
    BtlObj* p;

    p = &work->actor;

    if ((a & 0x10) && (a & 0x40)) {
        work->angle = 0x20;
        p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
        p->vx += 25;
        p->vy -= 12;
    } else if ((a & 0x10) && (a & 0x80)) {
        work->angle = 0x60;
        p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
        p->vx += 25;
        p->vy += 12;
    } else if ((a & 0x20) && (a & 0x80)) {
        work->angle = 0xA0;
        p->flags |= BTLOBJ_FLAG_FACING_LEFT;
        p->vx -= 25;
        p->vy += 12;
    } else if ((a & 0x20) && (a & 0x40)) {
        work->angle = 0xE0;
        p->flags |= BTLOBJ_FLAG_FACING_LEFT;
        p->vx -= 25;
        p->vy -= 12;
    } else if (a & 0x40) {
        work->angle = 0;
        p->vy -= 12;
    } else if (a & 0x10) {
        work->angle = 0x40;
        p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
        p->vx += 25;
    } else if (a & 0x80) {
        work->angle = 0x80;
        p->vy += 12;
    } else if (a & 0x20) {
        work->angle = 0xC0;
        p->flags |= BTLOBJ_FLAG_FACING_LEFT;
        p->vx -= 25;
    }

    if (p->vx > 512) {
        p->vx = 512;
    } else if (p->vx < -512) {
        p->vx = -512;
    }

    if (p->vy > 256) {
        p->vy = 256;
    } else if (p->vy < -256) {
        p->vy = -256;
    }

    if (a & 0xF0) {
        SetBtlRikuDirAnimation(work, 0, 1);

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
        SetBtlRikuAnimation(work, 0, 1);
    }

    if (a & 0xF0) {
        if (p->btl->hcEffect == 50) {
            work->speed += 256;

            if (work->speed > 768) {
                work->speed = 768;
            }
        } else {
            work->speed += 128;

            if (work->speed > 512) {
                work->speed = 512;
            }
        }
    } else {
        work->speed -= 128;

        if (work->speed < 0) {
            work->speed = 0;
        }
    }
}

void UpdateBtlRikuDarkWalk(BtlRikuWork* work, u16 a) {
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
        SetBtlRikuAnimation(work, 13, 1);

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
        SetBtlRikuAnimation(work, 12, 1);
    }

    if (a & 0xF0) {
        if (p->btl->hcEffect == 50) {
            work->speed += 256;

            if (work->speed > 768) {
                work->speed = 768;
            }
        } else {
            work->speed += 128;

            if (work->speed > 640) {
                work->speed = 640;
            }
        }
    } else {
        work->speed -= 128;

        if (work->speed < 0) {
            work->speed = 0;
        }
    }
}

void task_btl_riku_0(BtlRikuWork* work, BtlTaskArg* arg) {
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
        } else {
            work->mainSide = 0;
            e->btl = gRikuBtlWork;
        }

        e->maxHp = 1000;
        e->hp = 1000;
        e->attack = 10;
    } else {
        work->mainSide = 1;
        work->sioKeysA = 1;
        e->btl = gBtlWork;

        if (e->btl->flags & BTL_FLAG_HUM_BATTLE) {
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

    e->flags |= (BTLOBJ_FLAG_IMMUNE_GRAVITY | BTLOBJ_FLAG_IMMUNE_WARP);
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
    e->height = 40;
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
    work->paletteData = gRikuPalette;
    work->tiles = AllocObjTiles(0x640, NULL);
    LoadBtlRikuPalette(work);
    e->btl->actor = e;
    AnimInit(&work->anim, NULL, NULL);
    SetBtlRikuAnimation(work, 0, 1);
    work->gfx = AnimGetGfx(&work->anim);
    work->state = 0;
    work->unk_040 = 0;
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
    work->unk_18C = 0;
    work->scaleX = work->scaleY = 0x100;
    work->frameCount = 0;

    if (gBtlWork->flags & (BTL_FLAG_BOSS_BATTLE | BTL_FLAG_HUM_BATTLE)) {
        switch (gBtlWork->battleId) {
        case 149:
        case 151:
        case 153:
        case 154:
        case 156:
        case 157:
            work->groundSongs = sBtlRikuGroundSongs[0];
            break;
        case 148:
        case 150:
        case 155:
            work->groundSongs = sBtlRikuGroundSongs[1];
            break;
        case 152:
            work->groundSongs = sBtlRikuGroundSongs[2];
            break;
        case 158:
            work->groundSongs = sBtlRikuGroundSongs[3];
            break;
        default:
            work->groundSongs = sBtlRikuGroundSongs[0];
            break;
        }
    } else {
        switch (gGameState.battleStage) {
        case BATTLE_STAGE_WONDERLAND:
        case 2:
            work->groundSongs = sBtlRikuGroundSongs[0];
            break;
        case BATTLE_STAGE_AGRABAH:
        case BATTLE_STAGE_OLYMPUS_COLISEUM:
        case BATTLE_STAGE_HALLOWEEN_TOWN:
            work->groundSongs = sBtlRikuGroundSongs[1];
            break;
        case BATTLE_STAGE_ATLANTICA:
        case BATTLE_STAGE_MONSTRO:
            work->groundSongs = sBtlRikuGroundSongs[2];
            break;
        default:
            work->groundSongs = sBtlRikuGroundSongs[0];
            break;
        }
    }

    TaskPoolInit(&work->tasks, 7);
    TaskCreate(&work->tasks, &gTaskDescBtlShadow, e);
    TaskCreate(&work->tasks, &gTaskDescBtlBadstatus, e);
    work->drawCount = 0;
    SaveBtlRikuAfterimage(work, &work->drawInfo[0]);
    work->drawInfo[1] = work->drawInfo[0];
    work->drawInfo[2] = work->drawInfo[0];
    work->drawInfo[3] = work->drawInfo[0];
    work->drawInfo[4] = work->drawInfo[0];
    work->drawInfo[5] = work->drawInfo[0];
    work->drawInfo[6] = work->drawInfo[0];
    work->drawInfo[7] = work->drawInfo[0];
    work->drawInfo[8] = work->drawInfo[0];
}

void SetBtlRikuState(BtlRikuWork* work, u32 a) {
    work->state = a;
    work->steps = 0;
    work->stateTimer = 0;
    ClearBtlObjActionFlags(&work->actor);
}

void StartBtlRikuCombo(BtlRikuWork* work) {
    u16 t;

    if (work->state == 9 && work->comboCount <= 1) {
        work->comboCount++;
        work->stateTimer = 0;
        work->steps = 0;
    } else {
        switch (GetBtlRikuComboType(work)) {
        case 0:
            work->attacks[0] = &sUnk_0813C6E8;
            work->attacks[1] = &sUnk_0813C704;
            work->attacks[2] = &sUnk_0813C73C;
            break;
        case 1:
            work->attacks[0] = &sUnk_0813C704;
            work->attacks[1] = &sUnk_0813C6E8;
            work->attacks[2] = &sUnk_0813C73C;
            break;
        case 2:
            work->attacks[0] = &sUnk_0813C774;
            work->attacks[1] = &sUnk_0813C790;
            work->attacks[2] = &sUnk_0813C7AC;
            break;
        case 3:
            work->attacks[0] = &sUnk_0813C758;
            work->attacks[1] = &sUnk_0813C774;
            work->attacks[2] = &sUnk_0813C7AC;
            break;
        case 4:
        default:
            work->attacks[0] = &sUnk_0813C720;
            work->attacks[1] = &sUnk_0813C704;
            work->attacks[2] = &sUnk_0813C73C;
            break;
        }

        work->state = 9;
        work->steps = 0;
        work->stateTimer = 0;
        work->comboCount = 0;
        t = work->flags & ~BTL_RIKU_FLAG_COMBO_EXTENDED;
        work->flags = t;
    }
}

void StartBtlRikuKnockback(BtlRikuWork* work) {
    work->vz = -work->actor.knockbackLift * 3;
    work->actor.vx = ((gSineTable[work->actor.angle] << 1) * work->actor.knockbackSpeed) >> 8;
    work->actor.vy = ((-gSineTable[work->actor.angle + 0x40] << 1) * work->actor.knockbackSpeed) >> 8;
}

BtlObj* GetBtlRikuActiveOpponent(BtlRikuWork* work) {
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

BtlObj* FindHighestEnemy(BtlRikuWork* work) {
    BtlObj* e;
    BtlObj* best;
    s32 d;
    s32 min;

    min = 0x10000;
    best = NULL;
    e = ListPoolFirst(&gBtlWork->pool);

    while (e != NULL) {
        if (!(e->flags & BTLOBJ_FLAG_UNHITTABLE)) {
            d = e->z - (e->centerHeight << 8);

            if (min > d) {
                best = e;
                min = d;
            }
        }

        e = ListPoolNext(&e->node);
    }

    return best;
}

BtlObj* PickBtlRikuTarget(BtlRikuWork* work) {
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

u16 SwapBtlRikuKeyBits(u16 a, u16 b, u16 c) {
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

void EndRikuDarkMode(BtlRikuWork* work) {
    if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
        work->paletteData = gRikuPalette;
        LoadObjPaletteBank(work->palette->index, gRikuPalette);
        gBtlWork->flags &= ~BTL_FLAG_DARK_MODE;
        gBtlWork->flags |= BTL_FLAG_DARK_MODE_CHANGED;
    }
}

void AddDarkPoints(s16 a) {
    if (gGameState.flags & GAME_FLAG_DARK_POINTS_LOCKED) {
        return;
    }

    gBtlWork->darkPoints += a;

    if (gBtlWork->darkPoints < 0) {
        gBtlWork->darkPoints = 0;
    } else if (gBtlWork->darkPoints > 999) {
        gBtlWork->darkPoints = 999;
    }
}

s32 task_btl_riku_1(BtlRikuWork* work) {
    BtlObj* p;
    BtlObj* e;
    u16 held;
    u16 pressed;
    u16 uv;
    s16 n;
    s32 t;
    s32 t2;
    s32 tx;
    s32 ty;
    s32 tz;
    s32 flag;
    s16 dx;
    s16 dy;
    s16 dz;
    s32 mode;
    s32 mode2;
    u16 fr;
    s32 ex;
    s32 ey;
    s32 oa;
    s32 ob;
    s32 oc;
    s16 od;
    s32 t3;
    s32 d;
    s32 t4;
    s32 t5;
    const RikuAttackDef* a;
    BtlSpawnArgs spawn;
    u32 id;
    s32 sel[6];

    p = &work->actor;

    if (gBtlWork->phase == 4 && (p->flags & BTLOBJ_FLAG_IN_CARD_ACTION)) {
        switch (work->state) {
        case 24:
        case 25:
        case 26:
        case 27:
            p->x = p->originX;
            p->y = p->originY;
            p->z = p->originZ;

#ifndef VERSION_EU
            p->flags &= ~BTLOBJ_FLAG_NO_BREAK_POP;
            CreateBtlPopTask(p, 9);
#endif

            if (work->flags & BTL_RIKU_FLAG_HIDDEN) {
                work->flags &= ~BTL_RIKU_FLAG_HIDDEN;
                LoadBtlRikuPalette(work);
            }

            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuState(work, 38);
            } else {
                SetBtlRikuState(work, 3);
            }

            break;
        default:
            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuState(work, 19);
            } else {
                SetBtlRikuState(work, 18);
            }

            break;
        }

        work->scaleX = work->scaleY = 256;
        ColliderSetDisabled(&p->collider, 0);
        DisableBtlRikuPassThrough(work);
        p->flags &= 0xFFFFDFFBFF7FFFFFLL;
        work->speed = 0;
        gBtlWork->flags |= BTL_FLAG_STOP_BGFX;
        p->btl->flags |= BTL_FLAG_DISMISS_SUMMONS;
    }

    if (CanLevelUp() && LevelUp()) {
        CreateLevelUpEffectTask(p, &work->tasks);
    }

    if (work->flags & BTL_RIKU_FLAG_HC_STATUS) {
        work->flags &= ~BTL_RIKU_FLAG_HC_STATUS;
        p->flags &= ~BTLOBJ_FLAGS_ELEMENT_AFFINITY;
    }

    switch (p->btl->hcEffect) {
    case 26:
        work->flags |= BTL_RIKU_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_WEAK_FIRE | BTLOBJ_FLAG_RESIST_THUNDER);
        break;
    case 8:
        work->flags |= BTL_RIKU_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_WEAK_BLIZZARD | BTLOBJ_FLAG_RESIST_FIRE);
        break;
    case 15:
        work->flags |= BTL_RIKU_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_RESIST_FIRE | BTLOBJ_FLAG_RESIST_BLIZZARD | BTLOBJ_FLAG_RESIST_THUNDER);
        break;
    case 18:
        work->flags |= BTL_RIKU_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_IMMUNE_FIRE | BTLOBJ_FLAG_WEAK_BLIZZARD);
        break;
    case 50:
        work->flags |= BTL_RIKU_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_IMMUNE_THUNDER | BTLOBJ_FLAG_WEAK_NEUTRAL);
        break;
    case 27:
        work->flags |= BTL_RIKU_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_IMMUNE_BLIZZARD | BTLOBJ_FLAG_WEAK_FIRE);
        break;
    case 47:
        work->flags |= BTL_RIKU_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_WEAK_PHYSICAL | BTLOBJ_FLAG_RESIST_FIRE | BTLOBJ_FLAG_RESIST_BLIZZARD | BTLOBJ_FLAG_RESIST_THUNDER | BTLOBJ_FLAG_RESIST_NEUTRAL);
        break;
    case 49:
        work->flags |= BTL_RIKU_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_IMMUNE_BLIZZARD | BTLOBJ_FLAG_RESIST_PHYSICAL | BTLOBJ_FLAG_WEAK_NEUTRAL);
        break;
    case 28:
        work->flags |= BTL_RIKU_FLAG_HC_STATUS;
        p->flags |= (BTLOBJ_FLAG_RESIST_FIRE | BTLOBJ_FLAG_RESIST_BLIZZARD | BTLOBJ_FLAG_RESIST_THUNDER);
        break;
    }

    p->flags &= ~BTLOBJ_FLAG_GUARD_PHYSICAL;

    switch ((u32)p->btl->hcEffect) {
    case 51:
        e = GetBtlRikuActiveOpponent(work);

        if (e != NULL) {
            if ((e->x < p->x && (p->flags & BTLOBJ_FLAG_FACING_LEFT)) ||
                (e->x > p->x && !(p->flags & BTLOBJ_FLAG_FACING_LEFT))) {
                p->flags |= BTLOBJ_FLAG_GUARD_PHYSICAL;
            }
        }

        break;
    case 23: {
        u16 hp;
        u16 max;
        s32 t;

        hp = p->hp;

        if ((s16)hp > 0 && (s16)hp < (s16)(max = p->maxHp) && work->frameCount % 120 == 0) {
            n = (p->maxHp - p->hp) << 13 >> 16;

            if (n <= 0) {
                n = 1;
            }

            t = n + hp;
            p->hp = t;

            if ((s16)t > (s16)max) {
                p->hp = max;
            }

            p->btl->hcEffectCount--;
        }

        break;
    }
    case 24:
        if (work->frameCount % 180 == 0) {
            if (gBtlWork->flags & BTL_FLAG_VS_BATTLE) {
                BtlObj* e;
                u16 uv;

                if (work->mainSide != 0) {
                    e = gRikuBtlWork->actor;
                } else {
                    e = gBtlWork->actor;
                }

                uv = e->hp;

                if (e->hp > 1) {
                    e->hp = uv - 1;
                }
            } else {
                BtlObj* e;
                u16 uv;

                e = ListPoolFirst(&gBtlWork->pool);

                while (e != NULL) {
                    uv = e->hp;

                    if (e->hp > 1) {
                        e->hp = uv - 1;
                    }

                    e = ListPoolNext(&e->node);
                }
            }
        }

        break;
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

    if (p->badStatus == BAD_STATUS_CONFUSE) {
        held = SwapBtlRikuKeyBits(held, 0x20, 0x10);
        held = SwapBtlRikuKeyBits(held, 0x40, 0x80);
        pressed = SwapBtlRikuKeyBits(pressed, 0x20, 0x10);
        pressed = SwapBtlRikuKeyBits(pressed, 0x40, 0x80);
    }

    if (work->state != 8 && (p->btl->flags & BTL_FLAG_ESCAPED)) {
        p->flags |= BTLOBJ_FLAG_IGNORE_BOUNDS;
        work->state = 8;
        work->steps = 0;
        work->stateTimer = 0;
        p->flags |= BTLOBJ_FLAG_INTANGIBLE;
    } else {
        switch (UpdateBtlObjReaction(p)) {
        case BTL_REACTION_HURT:
        case BTL_REACTION_GRAVITY:
            work->speed = 0;

            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                AddDarkPoints(-5);
                work->state = 36;
                work->steps = 0;
                work->stateTimer = 0;
            } else {
                AddDarkPoints(1);
                work->state = 6;
                work->steps = 0;
                work->stateTimer = 0;
            }

            break;
        case BTL_REACTION_DEFEATED:
        case BTL_REACTION_GRAVITY_DEFEATED:
            EndRikuDarkMode(work);
            work->speed = 0;

            if (p->btl->hcEffect == 27) {
                work->state = 20;
                work->steps = 0;
                work->stateTimer = 0;
            } else {
                work->state = 7;
                work->steps = 0;
                work->stateTimer = 0;
            }

            break;
        case BTL_REACTION_HEALED:
            p->flags &= ~BTLOBJ_FLAG_HURT;
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
            p->flags &= ~BTLOBJ_FLAG_CARD_ACTION_PENDING;
            break;
        case BTL_REACTION_CARD_ACTION: {
            BtlObj* e;

            DisableBtlRikuPassThrough(work);
            work->stateTimer = 0;
            p->btl->flags &= ~BTL_FLAG_DISMISS_SUMMONS;
            p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
            work->flags |= BTL_RIKU_FLAG_PASS_THROUGH;
            id = ResolveActiveCardsMove(sel);

            if (id == 145) {
                if (!(p->btl->flags & BTL_FLAG_STOCK_SEQUENCE)) {
                    p->btl->flags |= BTL_FLAG_STOCK_SEQUENCE;
                    p->btl->stockMove = 0;
                }

                id = sel[p->btl->stockMove];
                p->btl->stockMove++;
            }

            switch (id) {
            case 48:
                work->state = 5;
                work->steps = 0;
                work->stateTimer = 0;
                work->variant[0] = 1;
                break;
            case 47:
                work->state = 5;
                work->steps = 0;
                work->stateTimer = 0;
                work->variant[0] = 0;
                break;
            case 49:
                work->state = 5;
                work->steps = 0;
                work->stateTimer = 0;
                work->variant[0] = 2;
                break;
            case 50:
                work->state = 5;
                work->steps = 0;
                work->stateTimer = 0;
                work->variant[0] = 3;
                break;
            case 51:
                work->state = 5;
                work->steps = 0;
                work->stateTimer = 0;
                work->variant[0] = 4;
                break;
            case 52:
                work->state = 5;
                work->steps = 0;
                work->stateTimer = 0;
                work->variant[0] = 5;
                break;
            case 53:
                work->state = 5;
                work->steps = 0;
                work->stateTimer = 0;
                work->variant[0] = 6;
                break;
            case 45:
                work->state = 24;
                work->steps = 0;
                work->stateTimer = 0;
                work->summonDesc = &gTaskDescSmnKing;
                work->variant[0] = 0;
                break;
            case 0x800A7E9F:
                work->state = 24;
                work->steps = 0;
                work->stateTimer = 0;
                work->summonDesc = &gTaskDescSmnKing;
                work->variant[0] = 1;
                break;
            case 0xE9FA7E9F:
                work->state = 24;
                work->steps = 0;
                work->stateTimer = 0;
                work->summonDesc = &gTaskDescSmnKing;
                work->variant[0] = 2;
                break;
            case 137:
                work->state = 43;
                break;
            case 138:
                work->state = 48;
                break;
            case 139:
                work->state = 49;
                break;
            case 46:
                work->state = 31;
                break;
            case 18:
                if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                    switch (work->state) {
                    case 56:
                        work->state = 42;
                        work->steps = 0;
                        work->stateTimer = 0;
                        break;
                    case 37:
                    case 38:
                        work->comboCount = 0;
                        work->state = 41;
                        work->steps = 0;
                        work->stateTimer = 0;
                        break;
                    case 41:
                        work->comboCount++;
                        work->state = 41;
                        work->steps = 0;
                        work->stateTimer = 0;
                        break;
                    case 60:
                        work->state = 61;
                        work->steps = 0;
                        work->stateTimer = 0;
                        break;
                    case 61:
                        work->state = 42;
                        work->steps = 0;
                        work->stateTimer = 0;
                        break;
                    default:
                        work->state = 60;
                        work->steps = 0;
                        work->stateTimer = 0;
                        break;
                    }
                } else {
                    StartBtlRikuCombo(work);
                }

                break;
            default:
                if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                    SetBtlRikuState(work, 35);
                } else {
                    SetBtlRikuState(work, 1);
                }

                break;
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

            AnimReset(&work->anim);
            work->speed = 0;
            p->vx = p->vy = 0;
            break;
        }
        case BTL_REACTION_HAZARD:
            if ((gBtlWork->flags & BTL_FLAG_OPPONENT_CARD_ACTION) == 0) {
                gBtlWork->flags |= BTL_FLAG_STOP_BGFX;
            }

            SetBattleZoom(12, 256, gBtlWork->x2, gBtlWork->y2);
            ColliderSetDisabled(&p->collider, 0);
            p->flags &= 0xFFFFDFFBFF7FFFFFLL;
            DisableBtlRikuPassThrough(work);
            p->btl->flags |= BTL_FLAG_DISMISS_SUMMONS;
            work->speed = 0;
            work->scaleX = work->scaleY = 256;

            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                work->state = 16;
                work->steps = 0;
                work->stateTimer = 0;
            } else {
                work->state = 15;
                work->steps = 0;
                work->stateTimer = 0;
            }

            break;
        case BTL_REACTION_CARD_BROKEN:
            switch (work->state) {
            case 24:
            case 25:
            case 26:
            case 27:
                p->x = p->originX;
                p->y = p->originY;
                p->z = p->originZ;
#ifdef VERSION_EU
                p->flags &= ~BTLOBJ_FLAG_NO_BREAK_POP;
                CreateBtlPopTask(p, 9);
#endif
                break;
            }

            work->scaleX = work->scaleY = 256;
            ColliderSetDisabled(&p->collider, 0);
            DisableBtlRikuPassThrough(work);
            p->flags &= 0xFFFFDFFBFF7FFFFFLL;
            p->btl->flags |= BTL_FLAG_DISMISS_SUMMONS;
            work->speed = 0;

            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                if (gBtlWork->darkPoints <= 0) {
                    EndRikuDarkMode(work);
                    work->state = 11;
                    work->steps = 0;
                    work->stateTimer = 0;
                } else {
                    work->state = 55;
                    work->steps = 0;
                    work->stateTimer = 0;
                }
            } else {
                work->state = 11;
                work->steps = 0;
                work->stateTimer = 0;
            }

            break;
        case BTL_REACTION_STUNNED:
        case BTL_REACTION_WARPED:
            work->speed = 0;

            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                AddDarkPoints(-5);
                work->state = 62;
                work->steps = 0;
                work->stateTimer = 0;
            } else {
                AddDarkPoints(1);
                work->state = 13;
                work->steps = 0;
                work->stateTimer = 0;
            }

            break;
        case BTL_REACTION_STOPPED:
            if (work->state != 17) {
                work->flags |= BTL_RIKU_FLAG_PASS_THROUGH;
                work->speed = 0;
                p->vx = p->vy = 0;
                work->state = 17;
                work->steps = 0;
                work->stateTimer = 0;
            }

            break;
        }
    }

    if (p->btl->flags & BTL_FLAG_FIELD_HIDDEN) {
        if (work->state != 14) {
            EndRikuDarkMode(work);
            work->state = 14;
            work->steps = 0;
            work->stateTimer = 0;
        }
    } else if (p->flags & BTLOBJ_FLAG_FREEZE_PENDING) {
        p->flags &= ~BTLOBJ_FLAG_FREEZE_PENDING;
        work->state = 30;
        work->steps = 0;
        work->stateTimer = 0;
    }

    work->flags &= ~BTL_RIKU_FLAG_AFTERIMAGE;

    switch (work->state) {
    case 49:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 26, 0);
            m4aSongNumStart(SONG_VO_RK_ATTACK08);
            m4aSongNumStart(SONG_BTL_AN_STANDENTRY);
            FadeStartOut(FADE_MODE_DARK_MAGENTA, 80);
        }

        work->vz = 0;
        p->z += (-10240 - p->z) >> 5;

        if (AnimIsFinished(&work->anim) == 0) {
            work->stateTimer++;
            break;
        }

        work->state = 50;
        work->stateTimer = 0;
        break;
    case 50:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 27, 0);
        }

        switch (AnimGetFrame(&work->anim)) {
        case 1:
        case 2:
            work->vz -= 179;
            break;
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            s32 t;

            t = p->x - 12288;
            p->x += (p->originX - t) >> 3;
        } else {
            s32 t2;

            t2 = p->x + 12288;
            p->x += (p->originX - t2) >> 3;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 51;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 51:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 28, 0);
            p->flags |= BTLOBJ_FLAG_IGNORE_BOUNDS;
        }

        work->vz = 0;
        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            p->x -= 3072;
        } else {
            p->x += 3072;
        }

        if (p->x < ((gBtlWork->xMin - 48) << 8) || p->x > ((gBtlWork->xMax + 48) << 8)) {
            work->state = 52;
            work->stateTimer = 0;
            work->unk_1B0 = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 52: {
        s32 tx;
        s32 ty;
        s32 tz;

        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            e = PickBtlRikuTarget(work);

            if (e != NULL) {
                tx = e->x;
                ty = e->y;
                tz = e->z - 4096;
            } else {
                tx = p->originX;
                ty = p->originY;
                tz = -4096;
            }

            p->flags ^= BTLOBJ_FLAG_FACING_LEFT;

            switch (GetRandom() % 3) {
            case 0:
                SetBtlRikuAnimation(work, 29, 1);

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    work->unk_194 = GetRandom() % 17 + 184;
                } else {
                    work->unk_194 = GetRandom() % 17 + 56;
                }

                p->y = ty - 4096 + (GetRandom() % 33 << 8);
                break;
            case 1:
                SetBtlRikuAnimation(work, 30, 1);

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    work->unk_194 = GetRandom() % 17 + 203;
                } else {
                    work->unk_194 = GetRandom() % 17 + 37;
                }

                p->y = ty + 4096 + (GetRandom() % 17 << 8);
                break;
            case 2:
                SetBtlRikuAnimation(work, 31, 1);

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    work->unk_194 = GetRandom() % 17 + 165;
                } else {
                    work->unk_194 = GetRandom() % 17 + 75;
                }

                p->y = ty - 4096 - (GetRandom() % 17 << 8);
                break;
            }

            p->z = tz;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x = tx + 25344;
                BgFxStartRikuLimit(p->x, p->y, tz, 192);
            } else {
                p->x = tx - 25344;
                BgFxStartRikuLimit(p->x, p->y, tz, 64);
            }

            m4aSongNumStart(SONG_BTL_RK_LIMITENTRY);
            work->steps = 10;
            work->scaleX = 10;
        }

        work->vz = 0;
        ApproachValue(&work->scaleX, 256, work->steps);

        if (--work->steps <= 0) {
            work->state = 53;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    }
    case 53:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            m4aSongNumStart(SONG_EF_RK_LIMITMOV);
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;
        p->x += gSineTable[(u8)work->unk_194] * 12;
        p->y += -gSineTable[(u8)work->unk_194 + 64] * 12;

        if (ApplyAttackBox(9, p->x, p->y, p->z, 24, 16, 24) != 0) {
            m4aSongNumStart(SONG_BTL_RK_HIT03);
        }

        work->vz = 0;

        if (work->stateTimer == 15 && (s16)work->unk_1B0 > 4) {
            work->state = 54;
            work->stateTimer = 0;
            break;
        }

        uv = work->stateTimer;

        if (work->stateTimer <= 30) {
            work->stateTimer = uv + 1;
            break;
        }

        work->state = 52;
        work->stateTimer = 0;
        work->unk_1B0++;
        break;
    case 54:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 32, 0);
            work->steps = 40;
        }

        if (work->steps > 0) {
            ApproachValue(&p->x, p->originX, work->steps);
            ApproachValue(&p->y, p->originY, work->steps);
            ApproachValue(&p->z, p->originZ, work->steps);

            if (--work->steps <= 0) {
                BgFxStartRikuLimitFinish(p->x, p->y - 8192, 0);
            }
        }

        if (BgFxIsActive() == 0 && work->steps <= 0 && AnimIsFinished(&work->anim) != 0) {
            p->flags &= ~BTLOBJ_FLAG_IGNORE_BOUNDS;
            ClearBtlObjActionFlags(p);
            work->state = 35;
            FadeStartIn(FADE_MODE_DARK_MAGENTA, 30);
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 42:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 22, 0);
            m4aSongNumStart(SONG_VO_RK_ATTACK02);
        }

        switch (AnimGetFrame(&work->anim)) {
        case 1:
            if (work->anim.timer == 0) {
                work->vz = -972;
            }

            break;
        case 2:
        case 3:
            if (ApplyAttackBox(7, p->x, p->y, p->z - 12288, 24, 20, 16) != 0) {
                m4aSongNumStart(SONG_BTL_RK_HIT00);
            }

            break;
        case 4:
            if (work->anim.timer == 0) {
                work->vz = 4096;
            }

            if (work->anim.timer % 6 == 0) {
                MakeOpponentsHittable();
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                if (ApplyAttackBox(7, p->x - 8192, p->y, p->z - 8192, 20, 25, 32) != 0) {
                    m4aSongNumStart(SONG_BTL_RK_HIT00);
                }
            } else {
                if (ApplyAttackBox(7, p->x + 8192, p->y, p->z - 8192, 20, 25, 32) != 0) {
                    m4aSongNumStart(SONG_BTL_RK_HIT00);
                }
            }

            break;
        case 5:
            if (p->z < p->groundZ) {
                work->anim.timer = 0;
                work->anim.frame--;
            }

            break;
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

        switch (AnimGetFrame(&work->anim)) {
        case 1:
        case 2:
        case 3: {
            s32 tx;
            s32 ty;

            e = p->btl->actor2;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                if (e != NULL) {
                    tx = e->x + 4096;
                    ty = e->y;
                } else {
                    tx = p->originX - 10240;
                    ty = p->y;
                }
            } else {
                if (e != NULL) {
                    tx = e->x - 4096;
                    ty = e->y;
                } else {
                    tx = p->originX + 10240;
                    ty = p->y;
                }
            }

            p->x += (tx - p->x) >> 3;
            p->y += (ty - p->y) >> 3;
            break;
        }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 48:
        if (work->stateTimer == 0) {
            FocusBtlRikuCameraOnTarget(work);
            SetBtlRikuAnimation(work, 25, 0);
            m4aSongNumStart(SONG_VO_RK_ATTACK08);
            work->flags &= ~BTL_RIKU_FLAG_FIRE_LAUNCHED;
            work->target = PickBtlRikuTarget(work);
        }

        if (work->flags & BTL_RIKU_FLAG_FIRE_LAUNCHED) {
            FocusBtlRikuCameraOnBgFx(work);
            e = work->target;

            if (e != NULL) {
                flag = 0;

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    if (e->x < p->x - 8192) {
                        flag = 1;
                    }
                } else {
                    if (e->x > p->x + 8192) {
                        flag = 1;
                    }
                }

                if (flag != 0) {
                    BgFxSetTarget(e->x, e->y, e->z - (e->centerHeight << 8));
                }
            }
        }

        if ((work->flags & BTL_RIKU_FLAG_FIRE_LAUNCHED) == 0 && work->anim.timer == 0) {
            s16 dx;
            s32 flag;

            dx = 0;
            flag = 0;

            switch (AnimGetFrame(&work->anim)) {
            case 0:
                dx = -10;
                break;
            case 1:
                dx = -24;
                break;
            case 4:
                dx = 12;
                break;
            case 5:
                dx = 15;
                break;
            case 6:
                dx = 7;
                flag = 1;
                break;
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x -= dx << 8;
            } else {
                p->x += dx << 8;
            }

            if (flag != 0) {
                work->flags |= BTL_RIKU_FLAG_FIRE_LAUNCHED;

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    BgFxStartFire(3, p->x - 18944, p->y, p->z - 6144, p->originX - 51200, p->originY, p->z - 6144, 1, 10);
                } else {
                    BgFxStartFire(3, p->x + 18944, p->y, p->z - 6144, p->originX + 51200, p->originY, p->z - 6144, 0, 10);
                }
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlRikuAnimation(work, 12, 1);
        }

        if (AnimIsFinished(&work->anim) != 0 && BgFxIsActive() == 0) {
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 41:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 21, 0);
            m4aSongNumStart(GetRandom() % 2 + SONG_VO_RK_ATTACK04);
            work->vz = -384;
            work->flags &= ~BTL_RIKU_FLAG_SWING_HIT;

            switch (p->btl->hcEffect) {
            case 8:
            case 34:
            case 43:
            case 44:
            case 49:
                p->btl->hcEffectCount--;
                break;
            }
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

        if (work->anim.timer == 0) {
            dy = 0;

            switch (AnimGetGfxIndex(&work->anim)) {
            case 3:
                dy = 16;
                break;
            case 4:
                dy = 5;
                break;
            case 6:
                dy = 1;
                break;
            case 2:
            case 7:
                dy = 4;
                break;
            case 8:
                dy = 6;
                break;
            case 9:
            case 10:
                dy = 2;
                break;
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x -= dy << 8;
            } else {
                p->x += dy << 8;
            }

            switch (AnimGetFrame(&work->anim)) {
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    p->x -= 256;
                } else {
                    p->x += 256;
                }

                break;
            }
        }

        fr = AnimGetFrame(&work->anim);

        if (fr >= 1 && fr <= 9) {
            if (p->btl->hcEffect == 34) {
                if (ApplyAttackBox(6, p->x, p->y, p->z - 5120, 65, 30, 12) != 0) {
                    m4aSongNumStart(SONG_BTL_RK_HIT01);
                    work->flags |= BTL_RIKU_FLAG_SWING_HIT;
                }
            } else {
                if (ApplyAttackBox(6, p->x, p->y, p->z - 5120, 45, 20, 12) != 0) {
                    m4aSongNumStart(SONG_BTL_RK_HIT01);
                    work->flags |= BTL_RIKU_FLAG_SWING_HIT;
                }
            }

            gBtlWork->damageScale = 0;
        } else if (fr == 10) {
            if (work->comboCount <= 1) {
                if (work->flags & BTL_RIKU_FLAG_SWING_HIT) {
                    p->flags &= ~BTLOBJ_FLAG_IN_CARD_ACTION;
                }
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 40:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 20, 0);
            m4aSongNumStart(GetRandom() % 2 + SONG_VO_RK_ATTACK04);
        }

        if (AnimGetGfxIndex(&work->anim) == 6) {
            work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                s32 t;

                t = p->x + 22528;
                p->x += (p->originX - t) >> 2;
            } else {
                s32 t2;

                t2 = p->x - 22528;
                p->x += (p->originX - t2) >> 2;
            }
        }

        if (work->anim.timer == 0) {
            s16 dz;
            s32 flag;
            s32 mode;

            dz = 0;
            flag = 0;
            mode = 4;

            switch (AnimGetGfxIndex(&work->anim)) {
            case 1:
                dz = 15;
                break;
            case 2:
                dz = 5;
                flag = 1;
                mode = 4;
                break;
            case 4:
                dz = -5;
                break;
            case 5:
                dz = 10;
                break;
            case 6:
                dz = 20;
                flag = 1;
                mode = 5;
                break;
            case 7:
                dz = -9;
                break;
            case 8:
                dz = 6;
                break;
            case 9:
                dz = 1;
                break;
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x -= dz << 8;
            } else {
                p->x += dz << 8;
            }

            if (flag != 0) {
                MakeOpponentsHittable();

                if ((p->flags & BTLOBJ_FLAG_FACING_LEFT) ? ApplyAttackBox(mode, p->x - 5120, p->y, p->z, 20, 8, 16)
                                   : ApplyAttackBox(mode, p->x + 5120, p->y, p->z, 20, 8, 16)) {
                    m4aSongNumStart(SONG_BTL_RK_HIT01);

                    if (mode == 5) {
                        FadeStartIn(FADE_MODE_ADD_WHITE, 45);

                        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                            SetBattleZoom(6, 332, p->x - 8192, p->y - 6144 + p->z);
                        } else {
                            SetBattleZoom(6, 332, p->x + 8192, p->y - 6144 + p->z);
                        }
                    }
                }
            }
        }

        if (work->anim.timer == 2 && AnimGetGfxIndex(&work->anim) == 6) {
            SetBattleZoom(8, 256, gBtlWork->x2, gBtlWork->y2);
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 60:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 33, 0);
            m4aSongNumStart(GetRandom() % 2 + SONG_VO_RK_ATTACK04);
            MakeOpponentsHittable();
            work->flags &= ~BTL_RIKU_FLAG_SWING_HIT;

            switch (p->btl->hcEffect) {
            case 8:
            case 34:
            case 43:
            case 44:
            case 49:
                p->btl->hcEffectCount--;
                break;
            }
        }

        if (work->flags & BTL_RIKU_FLAG_SWING_HIT) {
            p->flags &= ~BTLOBJ_FLAG_IN_CARD_ACTION;
        }

        if (work->anim.timer == 0) {
            s16 dz;
            s32 flag;
            s32 mode;

            dz = 0;
            flag = 0;
            mode = 4;

            switch (AnimGetGfxIndex(&work->anim)) {
            case 1:
                dz = 15;
                break;
            case 2:
                dz = 5;
                flag = 1;
                mode = 4;
                break;
            case 4:
                dz = -5;
                break;
            case 5:
                dz = 10;
                break;
            case 6:
                dz = 20;
                break;
            case 7:
                dz = -9;
                break;
            case 8:
                dz = 6;
                break;
            case 9:
                dz = 1;
                break;
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x -= dz << 8;
            } else {
                p->x += dz << 8;
            }

            if (flag != 0) {
                if (p->btl->hcEffect == 34) {
                    if ((p->flags & BTLOBJ_FLAG_FACING_LEFT) ? ApplyAttackBox(mode, p->x - 8960, p->y, p->z, 35, 25, 40)
                                       : ApplyAttackBox(mode, p->x + 8960, p->y, p->z, 35, 25, 40)) {
                        work->flags |= BTL_RIKU_FLAG_SWING_HIT;
                        m4aSongNumStart(SONG_BTL_RK_HIT01);
                    }
                } else {
                    if ((p->flags & BTLOBJ_FLAG_FACING_LEFT) ? ApplyAttackBox(mode, p->x - 5120, p->y, p->z, 20, 18, 40)
                                       : ApplyAttackBox(mode, p->x + 5120, p->y, p->z, 20, 18, 40)) {
                        work->flags |= BTL_RIKU_FLAG_SWING_HIT;
                        m4aSongNumStart(SONG_BTL_RK_HIT01);
                    }
                }
            }

            gBtlWork->damageScale = 0;
        }

        switch (AnimGetFrame(&work->anim)) {
        case 1:
        case 2:
            e = p->btl->actor2;

            if (e != NULL) {
                ex = e->x - p->x;
                ey = e->y - p->y;

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    s32 target;
                    s32 origin;

                    if (ex > 0) {
                        ex = 0;
                    }

                    target = p->originX + ex;
                    origin = p->x - 4096;
                    p->x += (target - origin) >> 3;
                } else {
                    s32 target;
                    s32 origin;

                    if (ex < 0) {
                        ex = 0;
                    }

                    target = p->originX + ex;
                    origin = p->x + 4096;
                    p->x += (target - origin) >> 3;
                }

                p->y += ((p->originY + ey) - p->y) >> 4;
            }

            break;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 61: {
        s16 dz;
        s32 flag;
        s32 mode2;
        s32 oa;
        s32 ob;
        s32 oc;
        s16 od;

        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 34, 0);
            m4aSongNumStart(GetRandom() % 2 + SONG_VO_RK_ATTACK04);
            work->flags &= ~BTL_RIKU_FLAG_SWING_HIT;

            switch (p->btl->hcEffect) {
            case 8:
            case 34:
            case 43:
            case 44:
            case 49:
                p->btl->hcEffectCount--;
                break;
            }
        }

        if ((work->flags & BTL_RIKU_FLAG_SWING_HIT) && work->anim.timer == 2 && AnimGetGfxIndex(&work->anim) == 6) {
            p->flags &= ~BTLOBJ_FLAG_IN_CARD_ACTION;
            SetBattleZoom(8, 256, gBtlWork->x2, gBtlWork->y2);
        }

        if (AnimGetGfxIndex(&work->anim) == 6) {
            work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                s32 t;

                t = p->x + 22528;
                p->x += (p->originX - t) >> 2;
            } else {
                s32 t2;

                t2 = p->x - 22528;
                p->x += (p->originX - t2) >> 2;
            }
        }

        if (work->anim.timer == 0) {
            dz = 0;
            flag = 0;
            mode2 = 4;

            switch (AnimGetGfxIndex(&work->anim)) {
            case 1:
                dz = 15;
                break;
            case 2:
                dz = 5;
                break;
            case 4:
                dz = -5;
                break;
            case 5:
                dz = 10;
                break;
            case 6:
                dz = 20;
                flag = 1;
                mode2 = 5;
                break;
            case 7:
                dz = -9;
                break;
            case 8:
                dz = 6;
                break;
            case 9:
                dz = 1;
                break;
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x -= dz << 8;
            } else {
                p->x += dz << 8;
            }

            if (flag != 0) {
                MakeOpponentsHittable();

                if (p->btl->hcEffect == 34) {
                    oa = 35;
                    ob = 35;
                    oc = 25;
                    od = 32;
                } else {
                    oa = 20;
                    ob = 20;
                    oc = 16;
                    od = 32;
                }

                if ((p->flags & BTLOBJ_FLAG_FACING_LEFT)
                    ? ApplyAttackBox(mode2, p->x - (oa << 8), p->y, p->z, ob, oc, od)
                    : ApplyAttackBox(mode2, p->x + (oa << 8), p->y, p->z, ob, oc, od)) {
                    work->flags |= BTL_RIKU_FLAG_SWING_HIT;
                    m4aSongNumStart(SONG_BTL_RK_HIT01);

                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        SetBattleZoom(6, 307, p->x - 8192, p->y - 6144 + p->z);
                    } else {
                        SetBattleZoom(6, 307, p->x + 8192, p->y - 6144 + p->z);
                    }
                }

                gBtlWork->damageScale = 0;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    }
    case 43:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (p->z < p->groundZ) {
            break;
        }

        if (work->stateTimer == 0) {
            FadeToAmount(FADE_MODE_BLACK, gBtlWork->fadeAmount, 8);
            SetBtlRikuAnimation(work, 15, 0);
            m4aSongNumStart(SONG_VO_RK_ATTACK02);
        }

        if (AnimIsFinished(&work->anim) == 0) {
            work->stateTimer++;
            break;
        }

        work->stateTimer = 0;
        work->state = 44;
        work->vz = -3072;
        break;
    case 44: {
        BtlObj* e;

        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            work->target = FindHighestEnemy(work);
        }

        e = work->target;

        if (e != NULL) {
            p->x += (e->x - p->x) >> 3;
            p->y += (e->y - p->y) >> 3;
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

        if (work->vz < 0) {
            if (work->vz > -512) {
                SetBtlRikuAnimation(work, 17, 0);
            } else {
                SetBtlRikuAnimation(work, 16, 0);
            }
        } else {
            work->stateTimer = 0;
            work->steps = 0;
            work->state = 45;
            break;
        }

        work->stateTimer++;
        break;
    }
    case 45:
        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 23, 0);
            work->vz = 1792;
            work->flags &= ~BTL_RIKU_FLAG_SWING_HIT;
            MakeOpponentsHittable();
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

        if (work->target != NULL) {
            BtlMapFollowPosition(p->x, p->y,
                          work->target->z - (work->target->centerHeight << 8));
            p->x += (work->target->x - p->x) >> 3;
            p->y += (work->target->y - p->y) >> 3;
        } else {
            BtlMapFollowPosition(p->x, p->y, p->groundZ);
        }

        if (AnimGetFrame(&work->anim) > 1) {
            if (ApplyAttackBox(8, p->x, p->y, p->z, 32, 16, 24) != 0) {
                m4aSongNumStart(SONG_BTL_RK_HIT02);
                work->flags |= BTL_RIKU_FLAG_SWING_HIT;
                func_08018FE4(p->x, p->y, p->z);
            }
        }

        if ((work->flags & BTL_RIKU_FLAG_SWING_HIT) != 0 || p->z >= p->groundZ) {
            work->stateTimer = 0;

            if (work->steps > 4) {
                work->state = 47;
                break;
            }

            work->state = 46;
            work->steps++;
        } else {
            work->stateTimer++;
        }

        break;
    case 46: {
        BtlObj* e;

        if (work->stateTimer == 0) {
            work->target = FindHighestEnemy(work);
            SetBtlRikuAnimation(work, 24, 0);
            work->vz = -2176;
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;
        e = work->target;

        if (e != NULL) {
            p->x += (e->x - p->x) >> 3;
            p->y += (e->y - p->y) >> 3;
        }

        if (AnimGetFrame(&work->anim) != 4) {
            work->stateTimer++;
            break;
        }

        work->state = 45;
        work->stateTimer = 0;
        break;
    }
    case 47:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 24, 0);
            work->vz = -1536;

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                work->targetX = p->x + 12288;
            } else {
                work->targetX = p->x - 12288;
            }
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;
        p->x += (work->targetX - p->x) >> 3;

        if (AnimIsFinished(&work->anim) != 0 && p->z >= p->groundZ) {
            FadeToOriginal(FADE_MODE_BLACK, 8);
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 37:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 15, 0);

            if ((held & DPAD_ANY) == 0) {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    work->angle = 192;
                } else {
                    work->angle = 64;
                }
            }

            work->speed >>= 1;
        }

        if (pressed & B_BUTTON) {
            work->state = 56;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 38;
            work->steps = 0;
            work->stateTimer = 0;
            work->vz = -1600;
            p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;
            work->speed <<= 1;
            break;
        }

        work->stateTimer++;
        break;
    case 38:
        FocusBtlRikuCameraOnTarget(work);
        p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;

        if (work->vz < 0) {
            if (work->vz <= -512) {
                SetBtlRikuAnimation(work, 16, 0);
            } else {
                SetBtlRikuAnimation(work, 17, 0);
            }
        } else if (work->vz <= 511) {
            SetBtlRikuAnimation(work, 17, 0);
        } else {
            SetBtlRikuAnimation(work, 18, 0);
        }

        if (work->vz < 0 && (held & B_BUTTON) == 0) {
            work->vz += 64;
        }

        if (pressed & B_BUTTON) {
            work->state = 56;
            work->steps = 0;
            work->stateTimer = 0;
            break;
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

            if (work->speed > 512) {
                work->speed = 512;
            }
        } else {
            work->speed -= 38;

            if (work->speed < 0) {
                work->speed = 0;
            }
        }

        work->stateTimer++;
        break;
    case 39:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            m4aSongNumStart(work->groundSongs[3]);
            work->speed = 0;
            SetBtlRikuAnimation(work, 19, 0);
        } else if (pressed & B_BUTTON) {
            m4aSongNumStart(work->groundSongs[2]);
            p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;
            work->state = 37;
            work->steps = 0;
            work->stateTimer = 0;
        } else if (pressed & DPAD_LEFT) {
            if (work->tapTimers[0] != 0) {
                p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                work->state = 57;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        } else if (pressed & DPAD_RIGHT) {
            if (work->tapTimers[1] != 0) {
                p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                work->state = 57;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        } else if (pressed & DPAD_UP) {
            if (work->tapTimers[2] != 0) {
                work->flags |= BTL_RIKU_FLAG_DASH_UP;
                work->state = 58;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        } else if (pressed & DPAD_DOWN) {
            if (work->tapTimers[3] != 0) {
                work->flags &= ~BTL_RIKU_FLAG_DASH_UP;
                work->state = 58;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        }

        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 35;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 36:
        FocusBtlRikuCameraOnTarget(work);
        p->originX = p->x;
        p->originY = p->y;

        if (work->stateTimer == 0) {
            AnimReset(&work->anim);
            SetBtlRikuAnimation(work, 14, 0);
            gBtlWork->hitStop = gBtlWork->pendingHitStop;
            StartBtlRikuKnockback(work);
            func_0807B3C4(30);

            if (gBtlWork->hcEffect == 18) {
                p->btl->hcEffectCount--;
                work->steps = 0;
            } else {
                work->steps = 15;
            }
        } else if (work->stateTimer == 6) {
            switch (GetRandom() % 3) {
            case 0:
                m4aSongNumStart(SONG_VO_RK_DAMAGE00);
                break;
            case 1:
                m4aSongNumStart(SONG_VO_RK_DAMAGE01);
                break;
            case 2:
            default:
                m4aSongNumStart(SONG_VO_RK_DAMAGE02);
                break;
            }
        }

        if (work->stateTimer >= work->steps && gBtlWork->darkPoints > 0) {
            p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            p->originX = p->x;
            p->originY = p->y;

            if (gBtlWork->darkPoints > 0) {
                work->state = 35;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }

            p->flags |= (BTLOBJ_FLAG_INTANGIBLE | BTLOBJ_FLAG_CARD_USE_BLOCKED);
            work->state = 33;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 35:
        FocusBtlRikuCameraOnTarget(work);
        DisableBtlRikuPassThrough(work);

        if (gBtlWork->darkPoints <= 0) {
            p->flags |= (BTLOBJ_FLAG_INTANGIBLE | BTLOBJ_FLAG_CARD_USE_BLOCKED);
            work->state = 33;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        if (pressed & DPAD_LEFT) {
            if (work->tapTimers[0] != 0) {
                p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                work->state = 57;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        } else if (pressed & DPAD_RIGHT) {
            if (work->tapTimers[1] != 0) {
                p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                work->state = 57;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        } else if (pressed & DPAD_UP) {
            if (work->tapTimers[2] != 0) {
                work->flags |= BTL_RIKU_FLAG_DASH_UP;
                work->state = 58;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        } else if (pressed & DPAD_DOWN) {
            if (work->tapTimers[3] != 0) {
                work->flags &= ~BTL_RIKU_FLAG_DASH_UP;
                work->state = 58;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        }

        UpdateBtlRikuDarkWalk(work, held);

        if (pressed & B_BUTTON) {
            m4aSongNumStart(work->groundSongs[2]);
            work->state = 37;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    case 31:
        if (p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) {
            FocusBtlRikuCameraOnTarget(work);
            break;
        }

        if (p->z < p->groundZ) {
            FocusBtlRikuCameraOnTarget(work);
            break;
        }

        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 0, 1);
            FadeToAmount(FADE_MODE_DARK_MAGENTA, 17, 16);
            m4aSongNumStart(SONG_SND_702);
            BgFxStartRikuDarkModeFlash(p->x, p->y, p->z);
            work->steps = 6;
            p->vx = p->vy = work->speed = 0;
        }

        work->vz = 0;

        if (work->stateTimer > 35 && work->steps != 0) {
            ApproachValue(&work->scaleY, 5, work->steps);
            work->steps--;
        }

        if (work->steps > 0) {
            work->stateTimer++;
            break;
        }

        work->state = 32;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case 32:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 24, 0);
            work->paletteData = gUnk_08F6DD04;
            LoadObjPaletteBank(work->palette->index, gUnk_08F6DD04);
            gBtlWork->flags |= BTL_FLAG_DARK_MODE;
            gBtlWork->flags |= BTL_FLAG_DARK_MODE_CHANGED;
            gBtlWork->darkPoints = gGameState.progression.dp;
            RequestSoraRemoveItemCards();
            work->scaleY = 5;
            work->steps = 6;
            work->vz = -1536;
            m4aSongNumStart(SONG_SND_715);
            m4aSongNumStart(SONG_SND_292);
            BgFxStartRikuDarkMode(p->x, p->y, p->z - 10240);
        }

        if (work->vz < 0) {
            if (work->vz > -256) {
                gBtlWork->hitStop = 4;
            } else {
                gBtlWork->hitStop = 1;
            }
        }

        BgFxSetPosition(p->x, p->y, p->z - 10240);

        if (work->scaleY == 256) {
            work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;
        }

        if (work->steps != 0) {
            ApproachValue(&work->scaleY, 256, work->steps);
            work->steps--;
        }

        if (work->steps <= 0 && p->z >= p->groundZ) {
            FadeToOriginal(FADE_MODE_DARK_MAGENTA, 10);
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 33:
        if (p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) {
            FocusBtlRikuCameraOnTarget(work);
            break;
        }

        if (p->z < p->groundZ) {
            FocusBtlRikuCameraOnTarget(work);
            break;
        }

        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 14, 0);
            m4aSongNumStart(SONG_SND_716);
            BgFxStartRikuDarkModeFlash(p->x, p->y, p->z);
            work->steps = 6;
            p->vx = p->vy = work->speed = 0;
        }

        work->vz = 0;

        if (work->stateTimer > 20 && work->steps != 0) {
            ApproachValue(&work->scaleY, 5, work->steps);
            work->steps--;
        }

        if (work->steps > 0) {
            work->stateTimer++;
            break;
        }

        work->state = 34;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case 34:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            EndRikuDarkMode(work);
            SetBtlRikuAnimation(work, 0, 1);
            work->scaleY = 5;
            work->steps = 6;
        }

        if (work->scaleY == 256) {
            work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;
        }

        if (work->steps != 0) {
            ApproachValue(&work->scaleY, 256, work->steps);
            work->steps--;
        }

        if (work->steps <= 0) {
            p->flags &= ~(BTLOBJ_FLAG_INTANGIBLE | BTLOBJ_FLAG_CARD_USE_BLOCKED);
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 0:
        if (work->stateTimer == 0) {
            if (work->steps == 0) {
                SetBtlRikuAnimation(work, 11, 0);
            }

            if (work->steps <= 69) {
                AnimReset(&work->anim);
            }

            if (AnimIsFinished(&work->anim) == 0) {
                work->steps++;
                break;
            }

            work->stateTimer = 1;
            break;
        }

        SetBtlRikuAnimation(work, 0, 1);

        if (gBtlWork->phase == 0) {
            break;
        }

        p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
        p->flags &= ~BTLOBJ_FLAG_INTANGIBLE;
        work->state = 1;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case 1:
        FocusBtlRikuCameraOnTarget(work);
        DisableBtlRikuPassThrough(work);

        if (p->z < p->groundZ) {
            work->state = 3;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        if (pressed & DPAD_LEFT) {
            if (work->tapTimers[0] != 0) {
                p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                p->originX = p->x;
                work->state = 22;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        } else if (pressed & DPAD_RIGHT) {
            if (work->tapTimers[1] != 0) {
                p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                p->originX = p->x;
                work->state = 22;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        }

        UpdateBtlRikuWalk(work, held);

        if ((pressed & B_BUTTON) == 0) {
            break;
        }

        m4aSongNumStart(work->groundSongs[2]);
        work->state = 2;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case 2:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuDirAnimation(work, 1, 1);

            if ((held & DPAD_ANY) == 0) {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    work->angle = 192;
                } else {
                    work->angle = 64;
                }
            }

            work->speed >>= 1;
        }

        uv = work->stateTimer;

        if (work->stateTimer <= 3) {
            work->stateTimer = uv + 1;
            break;
        }

        work->state = 3;
        work->steps = 0;
        work->stateTimer = 0;
        work->vz = -1664;
        p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;
        work->speed <<= 1;
        break;
    case 30:
        if (work->stateTimer == 0) {
            p->flags |= BTLOBJ_FLAG_INTANGIBLE;
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            work->speed = 0;
            work->flags |= BTL_RIKU_FLAG_HIDDEN;
        }

        if (work->stateTimer > 285) {
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

                if (p->hp > 1) {
                    p->hp = uv - 1;
                }
            }
        }

        uv = work->stateTimer;

        if (work->stateTimer > 300) {
            p->flags &= ~BTLOBJ_FLAG_INTANGIBLE;
            p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
            work->flags &= ~BTL_RIKU_FLAG_HIDDEN;

            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                work->state = 35;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }

            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    case 3:
        FocusBtlRikuCameraOnTarget(work);
        p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;

        if (work->vz < 0) {
            if (work->vz <= -512) {
                SetBtlRikuDirAnimation(work, 2, 1);
            } else {
                SetBtlRikuDirAnimation(work, 3, 1);
            }
        } else if (work->vz <= 511) {
            SetBtlRikuDirAnimation(work, 3, 1);
        } else {
            SetBtlRikuDirAnimation(work, 4, 1);
        }

        if (work->vz < 0 && (held & B_BUTTON) == 0) {
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

            if (work->speed > 512) {
                work->speed = 512;
            }
        } else {
            work->speed -= 38;

            if (work->speed < 0) {
                work->speed = 0;
            }
        }

        work->stateTimer++;
        break;
    case 4:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            m4aSongNumStart(work->groundSongs[3]);
            work->speed = 0;
            SetBtlRikuDirAnimation(work, 5, 0);
        } else if (pressed & B_BUTTON) {
            m4aSongNumStart(work->groundSongs[2]);
            p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;
            work->state = 2;
            work->steps = 0;
            work->stateTimer = 0;
        } else if (pressed & DPAD_LEFT) {
            if (work->tapTimers[0] != 0) {
                p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                p->originX = p->x;
                work->state = 22;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        } else if (pressed & DPAD_RIGHT) {
            if (work->tapTimers[1] != 0) {
                p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                p->originX = p->x;
                work->state = 22;
                work->steps = 0;
                work->stateTimer = 0;
                break;
            }
        }

        uv = work->stateTimer;

        if (work->stateTimer <= 6) {
            work->stateTimer = uv + 1;
            break;
        }

        work->state = 1;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case 8:
        SetBtlRikuDirAnimation(work, 0, 1);
        work->speed = 0;

        if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
            work->angle = 192;
            p->x -= 512;
        } else {
            work->angle = 64;
            p->x += 512;
        }

        break;
    case 10:
        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 10, 0);
        }

        if (AnimIsFinished(&work->anim) != 0) {
            SetBtlRikuState(work, 1);
            break;
        }

        work->stateTimer++;
        break;
    case 11:
        if (work->flags & BTL_RIKU_FLAG_HIDDEN) {
            break;
        }

        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 9, 0);
        }

        FocusBtlRikuCameraOnTarget(work);

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 55:
        if (work->flags & BTL_RIKU_FLAG_HIDDEN) {
            break;
        }

        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 14, 0);
        }

        FocusBtlRikuCameraOnTarget(work);

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 56:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            work->actor2 = PickBtlRikuTarget(work);
            SetBtlRikuAnimation(work, 24, 0);
            EnableBtlRikuPassThrough(work);
            e = work->actor2;

            if (e != NULL) {
                if (p->x < e->x) {
                    p->flags |= BTLOBJ_FLAG_FACING_LEFT;
                } else {
                    p->flags &= ~BTLOBJ_FLAG_FACING_LEFT;
                }

                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    work->targetX = e->x + 0x2D00;
                } else {
                    work->targetX = e->x - 0x2D00;
                }

                work->targetY = e->y;
            } else {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    work->targetX = p->x;
                } else {
                    work->targetX = p->x;
                }

                p->flags ^= BTLOBJ_FLAG_FACING_LEFT;
                work->targetY = p->y;
            }

            work->vz = -1152;
        }

        if (AnimGetFrame(&work->anim) != 0) {
            p->x += (work->targetX - p->x) >> 3;
            p->y += (work->targetY - p->y) >> 3;
            work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;
        }

        if (AnimIsFinished(&work->anim) != 0 && p->z >= p->groundZ) {
            DisableBtlRikuPassThrough(work);
            work->state = 35;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 22:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            work->speed = 0;
            work->unk_194 = 0x880;
            work->steps = 20;
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            work->vz = -819;

            if (GetRandom() % 2) {
                m4aSongNumStart(SONG_VO_RK_ATTACK01);
            } else {
                m4aSongNumStart(SONG_VO_RK_ATTACK03);
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                work->angle = 192;
            } else {
                work->angle = 64;
            }
        }

        if (work->vz < 0) {
            if (work->vz <= -512) {
                SetBtlRikuDirAnimation(work, 2, 1);
            } else {
                SetBtlRikuDirAnimation(work, 3, 1);
            }
        } else if (work->vz <= 511) {
            SetBtlRikuDirAnimation(work, 3, 1);
        } else {
            SetBtlRikuDirAnimation(work, 4, 1);
        }

        if (work->stateTimer == 4) {
            EnableBtlRikuPassThrough(work);
            p->flags |= BTLOBJ_FLAG_HIT_LOCKED;
        }

        if (work->steps != 0) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x += work->unk_194;
            } else {
                p->x -= work->unk_194;
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
        }

        if (work->steps == 0 && p->z >= p->groundZ) {
            p->flags &= ~BTLOBJ_FLAG_HIT_LOCKED;
            DisableBtlRikuPassThrough(work);
            work->state = 23;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 23:
        if (work->stateTimer == 0) {
            m4aSongNumStart(work->groundSongs[3]);
            SetBtlRikuDirAnimation(work, 5, 0);
        }

        if (AnimIsFinished(&work->anim) == 0) {
            work->stateTimer++;
            break;
        }

        work->state = 1;
        work->steps = 0;
        work->stateTimer = 0;
        p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
        break;
    case 57:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            work->speed = 0;
            work->unk_194 = 0x1080;
            work->steps = 20;
            work->vz = -819;
            m4aSongNumStart(GetRandom() % 2 + SONG_VO_RK_ATTACK04);
        }

        if (work->vz < 0) {
            if (work->vz <= -512) {
                SetBtlRikuAnimation(work, 16, 0);
            } else {
                SetBtlRikuAnimation(work, 17, 0);
            }
        } else if (work->vz <= 511) {
            SetBtlRikuAnimation(work, 17, 0);
        } else {
            SetBtlRikuAnimation(work, 18, 0);
        }

        if (work->stateTimer == 4) {
            EnableBtlRikuPassThrough(work);
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

        if (work->steps != 0) {
            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                p->x += work->unk_194;
            } else {
                p->x -= work->unk_194;
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
        }

        if (work->steps == 0 && p->z >= p->groundZ) {
            DisableBtlRikuPassThrough(work);
            work->state = 59;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 59:
        if (work->stateTimer == 0) {
            m4aSongNumStart(work->groundSongs[3]);
            SetBtlRikuAnimation(work, 19, 0);
        }

        if (AnimIsFinished(&work->anim) == 0) {
            work->stateTimer++;
            break;
        }

        work->state = 35;
        work->steps = 0;
        work->stateTimer = 0;
        break;
    case 58:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            work->speed = 0;
            work->unk_194 = 0x580;
            work->steps = 20;
            work->vz = -819;
            m4aSongNumStart(GetRandom() % 2 + SONG_VO_RK_ATTACK04);
        }

        if (work->vz < 0) {
            if (work->vz <= -512) {
                SetBtlRikuAnimation(work, 16, 0);
            } else {
                SetBtlRikuAnimation(work, 17, 0);
            }
        } else if (work->vz <= 511) {
            SetBtlRikuAnimation(work, 17, 0);
        } else {
            SetBtlRikuAnimation(work, 18, 0);
        }

        if (work->stateTimer == 4) {
            EnableBtlRikuPassThrough(work);
        }

        work->flags |= BTL_RIKU_FLAG_AFTERIMAGE;

        if (work->steps != 0) {
            if (work->flags & BTL_RIKU_FLAG_DASH_UP) {
                p->y -= work->unk_194;
            } else {
                p->y += work->unk_194;
            }

            ApproachValue(&work->unk_194, 0, work->steps);
            work->steps--;

            if (p->z < p->groundZ) {
                if (held & DPAD_UP) {
                    p->x -= 640;
                } else if (held & DPAD_DOWN) {
                    p->x += 640;
                }
            }
        }

        if (work->steps == 0 && p->z >= p->groundZ) {
            DisableBtlRikuPassThrough(work);
            work->state = 59;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 12:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 9, 0);
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 5:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlRikuCameraOnTarget(work);
            break;
        }

        FocusBtlRikuCameraOnTarget(work);

#ifdef VERSION_EU
        if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
            SetBtlRikuState(work, 35);
            break;
        }
#endif

        if (work->stateTimer == 0) {
            SetBtlRikuAnimation(work, 10, 0);
        }

        if (work->stateTimer == 23) {
            BgFxStartPotion(p->x, p->y, p->z);
        }

        if (work->stateTimer > 23 && BgFxIsActive() == 0) {
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

            SetBtlRikuState(work, 1);
            break;
        }

        work->stateTimer++;
        break;
    case 9: {
        s32 t;
        s32 t2;
        d = 0;
        FocusBtlRikuCameraOnTarget(work);

        if (p->btl->hcEffect == 3) {
            if ((work->flags & BTL_RIKU_FLAG_COMBO_EXTENDED) == 0) {
                if (work->comboCount == 2) {
                    work->comboCount = 1;
                    work->flags |= BTL_RIKU_FLAG_COMBO_EXTENDED;
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

        if (work->stateTimer == 0) {
            MakeOpponentsHittable();
            SetBtlRikuAnimation(work, a->animId, 0);

            if (work->comboCount == 2) {
                m4aSongNumStart(GetRandom() % 2 + SONG_VO_RK_ATTACK05);
            } else {
                m4aSongNumStart(GetRandom() % 3 + SONG_VO_RK_ATTACK00);
            }

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
        } else if (work->stateTimer == a->hitFrame) {
            if (p->btl->hcEffect == 34) {
                switch (a->animId) {
                case 5:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(a->attackIds[0], p->x - 5120, p->y, p->z - 7168, 40, 24, 44);
                    } else {
                        d = ApplyAttackBox(a->attackIds[0], p->x + 5120, p->y, p->z - 7168, 40, 24, 44);
                    }

                    break;
                case 3:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(a->attackIds[0], p->x - 8192, p->y, p->z, 32, 35, 32);
                    } else {
                        d = ApplyAttackBox(a->attackIds[0], p->x + 8192, p->y, p->z, 32, 35, 32);
                    }

                    break;
                default:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(a->attackIds[0], p->x - 9216, p->y, p->z, 32, 20, 32);
                    } else {
                        d = ApplyAttackBox(a->attackIds[0], p->x + 9216, p->y, p->z, 32, 20, 32);
                    }

                    break;
                }
            } else {
                if (p->btl->hcEffect == 49 && work->comboCount == 2) {
                    if (GetRandom() % 2 != 0) {
                        t = 164;
                    } else {
                        CreateBtlPopTask(p, 2);
                        t = a->attackIds[0];
                    }
                } else {
                    t = a->attackIds[0];
                }

                switch (a->animId) {
                case 5:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(t, p->x - 5120, p->y, p->z - 7168, 28, 20, 44);
                    } else {
                        d = ApplyAttackBox(t, p->x + 5120, p->y, p->z - 7168, 28, 20, 44);
                    }

                    break;
                case 3:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(t, p->x - 8192, p->y, p->z, 20, 26, 32);
                    } else {
                        d = ApplyAttackBox(t, p->x + 8192, p->y, p->z, 20, 26, 32);
                    }

                    break;
                default:
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        d = ApplyAttackBox(t, p->x - 9216, p->y, p->z, 20, 16, 32);
                    } else {
                        d = ApplyAttackBox(t, p->x + 9216, p->y, p->z, 20, 16, 32);
                    }

                    break;
                }

                gBtlWork->damageScale = 0;
            }

            if (d == 1) {
                m4aSongNumStart(a->song);

                if (a->flags & COMBO_FLAG_ZOOM_ON_HIT) {
                    if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                        SetBattleZoom(8, 384, p->x - 5120, (p->y - 5120) + p->z);
                    } else {
                        SetBattleZoom(8, 384, p->x + 5120, (p->y - 5120) + p->z);
                    }
                }

                work->flags |= BTL_RIKU_FLAG_SWING_HIT;
            } else {
                work->flags &= ~BTL_RIKU_FLAG_SWING_HIT;
            }
        } else if (work->stateTimer == a->hitFrame + 2) {
            if (a->flags & COMBO_FLAG_ZOOM_ON_HIT) {
                SetBattleZoom(15, 256, gBtlWork->x2, gBtlWork->y2);
            }

            if (work->comboCount <= 1) {
                if (work->flags & BTL_RIKU_FLAG_SWING_HIT) {
                    p->flags &= ~BTLOBJ_FLAG_IN_CARD_ACTION;
                    MakeOpponentsHittable();
                }
            }
        }

        if (d == 2) {
            SetBattleZoom(8, 256, gBtlWork->x2, gBtlWork->y2);
            SetBtlRikuState(work, 12);
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            break;
        }

        if (AnimIsFinished(&work->anim) != 0 && (p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) == 0) {
            SetBtlRikuState(work, 1);
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
                    t3 = (e->z - (e->centerHeight << 8)) - p->z;

                    if (t3 < 0) {
                        p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;
                        p->z += ((p->originZ + t3) - p->z) >> 3;
                        work->vz = 0;
                    }
                }
            } else {
                if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                    t4 = p->originX - 4096;
                    p->x += (t4 - p->x) >> 3;
                } else {
                    t4 = p->originX + 4096;
                    p->x += (t4 - p->x) >> 3;
                }
            }
        }

        work->stateTimer++;
        break;
    }
    case 24:
        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) || p->z < p->groundZ) {
            FocusBtlRikuCameraOnTarget(work);
            break;
        }

        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            p->btl->actor2 = NULL;

            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuAnimation(work, 15, 0);
            } else {
                SetBtlRikuDirAnimation(work, 1, 0);
            }

            if (p->flags & BTLOBJ_FLAG_FACING_LEFT) {
                work->angle = 192;
            } else {
                work->angle = 64;
            }
        }

        uv = work->stateTimer;

        if (work->stateTimer > 3) {
            work->state = 25;
            work->steps = 0;
            work->stateTimer = 0;
            work->vz = -1024;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    case 25:
        if (work->stateTimer == 0) {
            work->steps = 20;
            ColliderSetDisabled(&p->collider, 1);
            p->flags |= BTLOBJ_FLAG_IGNORE_BOUNDS;
            p->flags |= BTLOBJ_FLAG_NO_BREAK_POP;
            p->originZ = p->z;
        }

        switch (work->stateTimer) {
        case 0:
            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuAnimation(work, 16, 0);
            } else {
                SetBtlRikuDirAnimation(work, 2, 0);
            }

            break;
        case 10:
            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuAnimation(work, 17, 0);
            } else {
                SetBtlRikuDirAnimation(work, 3, 0);
            }

            break;
        }

        if (p->originX < 0x10000) {
            ApproachValue(&p->x, -8192, work->steps);
        } else {
            ApproachValue(&p->x, 0x22000, work->steps);
        }

        work->steps--;

        if (work->steps == 0) {
            work->state = 26;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 26:
        if (work->stateTimer == 0) {
            work->flags |= BTL_RIKU_FLAG_HIDDEN;
            ReleaseBtlRikuPalette(work);
            spawn.mainSide = work->mainSide;
            spawn.variant = work->variant[0];
            TaskCreate(&gBtlWork->taskPools[0], work->summonDesc, &spawn);
            p->x = p->originX;
            p->y = p->originY;
            p->z = -65536;
        }

        work->vz = 0;

        if ((p->btl->flags & BTL_FLAG_SUMMON_ACTIVE) == 0) {
            work->flags &= ~BTL_RIKU_FLAG_HIDDEN;
            LoadBtlRikuPalette(work);

            if (p->originX < 0x10000) {
                p->x = -8192;
            } else {
                p->x = 0x22000;
            }

            p->z = p->originZ - 12800;
            p->btl->flags |= BTL_FLAG_PLAYER_AIRBORNE;
            work->vz = 0;

            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuAnimation(work, 12, 1);
            } else {
                SetBtlRikuAnimation(work, 0, 1);
            }

            work->state = 27;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer++;
        }

        break;
    case 27:
        if (work->stateTimer == 0) {
            work->steps = 20;
        }

        switch (work->stateTimer) {
        case 0:
            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuAnimation(work, 17, 0);
            } else {
                SetBtlRikuDirAnimation(work, 3, 0);
            }

            break;
        case 10:
            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuAnimation(work, 18, 0);
            } else {
                SetBtlRikuDirAnimation(work, 4, 0);
            }

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

        if ((p->btl->flags & BTL_FLAG_PLAYER_AIRBORNE) == 0) {
            work->state = 28;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
    case 28:
        if (work->stateTimer == 0) {
            ColliderSetDisabled(&p->collider, 0);
            p->flags &= ~BTLOBJ_FLAG_IGNORE_BOUNDS;
            p->flags &= ~BTLOBJ_FLAG_NO_BREAK_POP;

            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuAnimation(work, 19, 0);
            } else {
                SetBtlRikuDirAnimation(work, 5, 0);
            }
        }

        uv = work->stateTimer;

        if (work->stateTimer > 6) {
            if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                SetBtlRikuState(work, 35);
            } else {
                SetBtlRikuState(work, 1);
            }

            break;
        }

        work->stateTimer = uv + 1;
        break;
    case 15: {
        u16 uv;

        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            FadeFromAmount(FADE_MODE_RED, 4, 10);
            SetBtlRikuAnimation(work, 7, 0);
            p->flags |= BTLOBJ_FLAG_HURT;
            work->speed = 0;
            uv = p->hp;

            if (p->hp > 1) {
                p->hp = uv - 1;
            }

            switch (GetRandom() % 3) {
            case 0:
                m4aSongNumStart(SONG_VO_RK_DAMAGE00);
                break;
            case 1:
                m4aSongNumStart(SONG_VO_RK_DAMAGE01);
                break;
            case 2:
            default:
                m4aSongNumStart(SONG_VO_RK_DAMAGE02);
                break;
            }
        }

        uv = work->stateTimer;

        if (work->stateTimer > 24) {
            p->flags &= ~BTLOBJ_FLAG_HURT;
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    }
    case 16: {
        u16 uv;

        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            FadeFromAmount(FADE_MODE_RED, 4, 10);
            SetBtlRikuAnimation(work, 14, 0);
            p->flags |= BTLOBJ_FLAG_HURT;
            work->speed = 0;
            uv = p->hp;

            if (p->hp > 1) {
                p->hp = uv - 1;
            }

            switch (GetRandom() % 3) {
            case 0:
                m4aSongNumStart(SONG_VO_RK_DAMAGE00);
                break;
            case 1:
                m4aSongNumStart(SONG_VO_RK_DAMAGE01);
                break;
            case 2:
            default:
                m4aSongNumStart(SONG_VO_RK_DAMAGE02);
                break;
            }
        }

        uv = work->stateTimer;

        if (work->stateTimer > 24) {
            p->flags &= ~BTLOBJ_FLAG_HURT;
            work->state = 35;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    }
    case 6:
        FocusBtlRikuCameraOnTarget(work);
        p->originX = p->x;
        p->originY = p->y;

        if (work->stateTimer == 0) {
            AnimReset(&work->anim);
            SetBtlRikuAnimation(work, 7, 0);
            gBtlWork->hitStop = gBtlWork->pendingHitStop;
            StartBtlRikuKnockback(work);

            if (gBtlWork->hcEffect == 18) {
                p->btl->hcEffectCount--;
                work->steps = 0;
            } else {
                work->steps = 15;
            }
        } else if (work->stateTimer == 6) {
            switch (GetRandom() % 3) {
            case 0:
                m4aSongNumStart(SONG_VO_RK_DAMAGE00);
                break;
            case 1:
                m4aSongNumStart(SONG_VO_RK_DAMAGE01);
                break;
            case 2:
            default:
                m4aSongNumStart(SONG_VO_RK_DAMAGE02);
                break;
            }
        }

        if (work->stateTimer >= work->steps) {
            p->flags &= ~BTLOBJ_FLAG_CARD_USE_BLOCKED;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            ClearBtlObjActionFlags(p);
            p->originX = p->x;
            p->originY = p->y;
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
            break;
        }

        work->stateTimer++;
        break;
        break;
    case 19:
        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 35;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    case 18:
        if (AnimIsFinished(&work->anim) != 0) {
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    case 20:
        if (work->stateTimer == 0) {
            StartBtlRikuKnockback(work);
            work->flags |= BTL_RIKU_FLAG_PASS_THROUGH;
            SetBtlRikuAnimation(work, 8, 0);
            m4aSongNumStart(SONG_VO_RK_DEATH00);
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
            gBtlWork->hitStop = 3;

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

            if (gBtlWork->enemyCount == 0 && gBtlWork->pendingEnemies <= 0) {
                work->state = 21;
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
    case 21: {
        u16 uv;

        SetBtlRikuAnimation(work, 0, 0);
        uv = work->stateTimer;

        if (work->stateTimer > 60) {
            gBtlWork->flags |= BTL_FLAG_BATTLE_OVER;
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        } else {
            work->stateTimer = uv + 1;
        }

        break;
    }
    case 7:
        FocusBtlRikuCameraOnTarget(work);

        if (work->stateTimer == 0) {
            gBtlWork->flags |= BTL_FLAG_PLAYER_DEFEATED;
            StartBtlRikuKnockback(work);
            work->flags |= BTL_RIKU_FLAG_PASS_THROUGH;
            SetBtlRikuAnimation(work, 8, 0);
            m4aSongNumStart(SONG_VO_RK_DEATH00);
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
            gBtlWork->hitStop = 3;

            if (work->vz > 0) {
                work->vz = 0;
            }
        }

        break;
    case 17:
        BtlMapFollowPosition(p->x, p->y, p->z);
        work->vz = 0;

        if (p->badStatus != BAD_STATUS_STOP) {
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
            ClearBtlObjActionFlags(p);
            work->flags &= ~BTL_RIKU_FLAG_PASS_THROUGH;
        } else {
            work->stateTimer++;
        }

        break;
    case 13:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            AnimReset(&work->anim);
            SetBtlRikuAnimation(work, 7, 0);
            work->stateTimer++;
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            StartBtlRikuKnockback(work);
            work->speed = 0;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            p->flags &= ~BTLOBJ_FLAG_HIT_LOCKED;
        }

        if ((A_BUTTON | B_BUTTON | DPAD_ANY) & pressed) {
            p->badStatusTimer -= 1;
        }

        if (p->badStatus != BAD_STATUS_STUN) {
            ClearBtlObjActionFlags(p);
            work->state = 1;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    case 62:
        BtlMapFollowPosition(p->x, p->y, p->z);

        if (work->stateTimer == 0) {
            AnimReset(&work->anim);
            SetBtlRikuAnimation(work, 14, 0);
            work->stateTimer++;
            p->flags |= BTLOBJ_FLAG_CARD_USE_BLOCKED;
            StartBtlRikuKnockback(work);
            work->speed = 0;
        }

        if (AnimIsFinished(&work->anim) != 0) {
            p->flags &= ~BTLOBJ_FLAG_HIT_LOCKED;
        }

        if ((A_BUTTON | B_BUTTON | DPAD_ANY) & pressed) {
            p->badStatusTimer -= 1;
        }

        if (p->badStatus != BAD_STATUS_STUN) {
            ClearBtlObjActionFlags(p);
            work->state = 35;
            work->steps = 0;
            work->stateTimer = 0;
        }

        break;
    case 14:
        p->flags &= ~BTLOBJ_FLAG_HURT;
#ifdef VERSION_EU
        p->vx = p->vy = 0;
        work->speed = 0;
#endif
        break;
    }

    if (pressed & DPAD_LEFT) {
        work->tapTimers[0] = 13;
        work->tapTimers[1] = 0;
    } else if (pressed & DPAD_RIGHT) {
        work->tapTimers[1] = 13;
        work->tapTimers[0] = 0;
    } else if (pressed & DPAD_UP) {
        work->tapTimers[2] = 13;
        work->tapTimers[3] = 0;
    } else if (pressed & DPAD_DOWN) {
        work->tapTimers[3] = 13;
        work->tapTimers[2] = 0;
    }

    if (work->tapTimers[1] != 0) {
        work->tapTimers[1]--;
    }

    if (work->tapTimers[0] != 0) {
        work->tapTimers[0]--;
    }

    if (work->tapTimers[2] != 0) {
        work->tapTimers[2]--;
    }

    if (work->tapTimers[3] != 0) {
        work->tapTimers[3]--;
    }

    if (p->badStatus != BAD_STATUS_BIND) {
        p->x += gSineTable[work->angle] * work->speed >> 8;
        p->y += -gSineTable[work->angle + 64] * (work->speed >> 1) >> 8;
    }

    if (p->collider.colliding != 0 && p->collider.otherType != 12) {
        if ((work->flags & BTL_RIKU_FLAG_ON_PLATFORM) && p->collider.otherType == 7) {
            p->collider.standFlags |= COLLIDER_STAND_OVER_PLATFORM;
        } else if (!(work->flags & BTL_RIKU_FLAG_PASS_THROUGH)) {
            p->x += p->collider.pushX >> 1;
            p->y += p->collider.pushY >> 1;
        }
    }

    if (!(p->flags & BTLOBJ_FLAG_IGNORE_BOUNDS)) {
        ApplyBattleBounds(&p->x, &p->y, &p->z, &p->floorZ);
    }

    p->z += work->vz;
    work->vz += gBtlWork->gravity;

    if (p->collider.standFlags & COLLIDER_STAND_OVER_PLATFORM) {
        p->groundZ = p->collider.platformZ;
        work->flags |= BTL_RIKU_FLAG_OVER_PLATFORM;
        work->platformPriority = -4100 - ((p->collider.platformY + 0x400) >> 8) * 4;
    } else {
        work->flags &= ~BTL_RIKU_FLAG_OVER_PLATFORM;
        p->groundZ = p->floorZ;
    }

    if ((work->flags & BTL_RIKU_FLAG_ON_PLATFORM) && gBtlWork->platform == p->collider.other) {
        p->x += p->collider.platformX - work->platformX;
        p->y += p->collider.platformY - work->platformY;
        p->z += p->collider.platformZ - work->platformZ;
    }

    if (p->z >= p->groundZ) {
        if (p->collider.standFlags & COLLIDER_STAND_OVER_PLATFORM) {
            work->flags |= BTL_RIKU_FLAG_ON_PLATFORM;
            work->platformX = p->collider.platformX;
            work->platformY = p->collider.platformY;
            work->platformZ = p->collider.platformZ;
            gBtlWork->platform = p->collider.other;
        } else {
            work->flags &= ~BTL_RIKU_FLAG_ON_PLATFORM;
            gBtlWork->platform = NULL;
        }

        work->vz = 0;
        p->z = p->groundZ;
        p->btl->flags &= ~BTL_FLAG_PLAYER_AIRBORNE;

        if (work->state == 3) {
            work->state = 4;
            work->steps = 0;
            work->stateTimer = 0;
        } else if (work->state == 38) {
            work->state = 39;
            work->steps = 0;
            work->stateTimer = 0;
        }
    } else {
        if (work->flags & BTL_RIKU_FLAG_ON_PLATFORM) {
            work->flags &= ~BTL_RIKU_FLAG_ON_PLATFORM;

            if (work->state == 1) {
                if (gBtlWork->flags & BTL_FLAG_DARK_MODE) {
                    work->state = 38;
                    work->steps = 0;
                    work->stateTimer = 0;
                } else {
                    work->state = 3;
                    work->steps = 0;
                    work->stateTimer = 0;
                }
            }
        }

        gBtlWork->platform = NULL;
    }

    t = p->vx;

    if (t > 0) {
        p->x += t;
        p->vx -= 17;

        if (p->vx < 0) {
            p->vx = 0;
        }
    } else if (t < 0) {
        p->x += t;
        p->vx += 17;

        if (p->vx > 0) {
            p->vx = 0;
        }
    }

    t2 = p->vy;

    if (t2 > 0) {
        p->y += t2;
        p->vy -= 17;

        if (p->vy < 0) {
            p->vy = 0;
        }
    } else if (t2 < 0) {
        p->y += t2;
        p->vy += 17;

        if (p->vy > 0) {
            p->vy = 0;
        }
    }

    if (!(p->flags & BTLOBJ_FLAG_IGNORE_BOUNDS)) {
        switch (ClampBattlePosition(&p->x, &p->y, -16, 0)) {
        case 1:
            p->vx = 0;

            if (p->z == 0 && (held & DPAD_LEFT)) {
                p->btl->flags |= BTL_FLAG_PUSHING_EDGE;
            } else {
                p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
            }

            work->flags |= BTL_RIKU_FLAG_AT_SIDE_EDGE;
            break;
        case 2:
            p->vx = 0;

            if (p->z == 0 && (held & DPAD_RIGHT)) {
                p->btl->flags |= BTL_FLAG_PUSHING_EDGE;
            } else {
                p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
            }

            work->flags |= BTL_RIKU_FLAG_AT_SIDE_EDGE;
            break;
        case 3:
        case 4:
            p->vy = 0;
            p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
            break;
        default:
            p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
            work->flags &= ~BTL_RIKU_FLAG_AT_SIDE_EDGE;
            break;
        }

        if (p->btl->flags & BTL_FLAG_SUMMON_ACTIVE) {
            p->btl->flags &= ~BTL_FLAG_PUSHING_EDGE;
        }

        if (p->btl->flags & BTL_FLAG_PUSHING_EDGE) {
            if (work->mainSide != 0) {
                BtlMapFollowPosition(p->x, p->y, p->z);
            }
        }
    }

    TaskPoolUpdate(&work->tasks);

    if (work->state == 11 || work->state == 55) {
        if (work->flags & BTL_RIKU_FLAG_HIDDEN) {
            work->flags &= ~BTL_RIKU_FLAG_HIDDEN;
            LoadBtlRikuPalette(work);
            SetBtlRikuAnimation(work, 9, 0);
            p->x = p->originX;
            p->y = p->originY;
            p->z = p->originZ;
        }
    }

    if (p->badStatus != BAD_STATUS_STOP) {
        work->gfx = AnimUpdate(&work->anim);
    }

    ColliderSetPosition(&p->collider, p->x, p->y, p->z);
    work->frameCount++;

    return 1;
}

void task_btl_riku_2(BtlRikuWork* work) {
    BtlObj* p;
    ObjAffine* affine;
    s32 sx;
    s32 sy;
    u16 attr;
    u16 attr2;
    s16 x;
    s16 y;

    p = &work->actor;

    if (work->flags & BTL_RIKU_FLAG_HIDDEN) {
        return;
    }

    if (work->actor.btl->hcEffect == 19) {
        if (work->mainSide != 0) {
            if (gFrameCounter & 1) {
                return;
            }
        } else if (gFrameCounter % 120 <= 59) {
            return;
        }
    }

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

    if (work->flags & BTL_RIKU_FLAG_OVER_PLATFORM) {
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
        u16 t = work->flags | BTL_RIKU_FLAG_HIT_FLASH;

        work->flags = t;
        LoadObjPaletteBank(work->palette->index, gUnk_08F69BC4);
    } else if (work->flags & BTL_RIKU_FLAG_HIT_FLASH) {
        u16 t = work->flags & ~BTL_RIKU_FLAG_HIT_FLASH;

        work->flags = t;

        if (work->mainSide != 0) {
            LoadObjPaletteBank(work->palette->index, work->paletteData);
        } else {
            LoadObjPaletteBank(work->palette->index, gUnk_096FAC64);
        }
    }

    DrawSprite(x, y, work->gfx, work->tiles2, work->palette, affine, attr, attr2);

    if (work->flags & BTL_RIKU_FLAG_AFTERIMAGE) {
        switch (work->drawCount % 2) {
        case 0:
            DrawBtlRikuAfterimage(work, &work->drawInfo[3]);
            break;
        case 1:
            DrawBtlRikuAfterimage(work, &work->drawInfo[6]);
            break;
        }

        work->drawCount++;
    }

    work->drawInfo[6] = work->drawInfo[5];
    work->drawInfo[5] = work->drawInfo[4];
    work->drawInfo[4] = work->drawInfo[3];
    work->drawInfo[3] = work->drawInfo[2];
    work->drawInfo[2] = work->drawInfo[1];
    work->drawInfo[1] = work->drawInfo[0];
    SaveBtlRikuAfterimage(work, &work->drawInfo[0]);
    TaskPoolDraw(&work->tasks);
}

void task_btl_riku_3(BtlRikuWork* work) {
    BtlObj* p;

    p = &work->actor;

    if (gBtlWork->phase == 3) {
        gGameState.hp = gGameState.progression.maxHp;
    } else {
        gGameState.hp = p->hp;
    }

    ColliderUnregister(&p->collider);
    ReleaseBtlRikuPalette(work);
    ReleaseObjTiles(work->tiles);
    TaskPoolDestroy(&work->tasks);
}

TaskDesc gTaskDescBtlRiku = {
    "task_btl_riku",
    (TaskInitFunc)task_btl_riku_0,
    (TaskUpdateFunc)task_btl_riku_1,
    (TaskDrawFunc)task_btl_riku_2,
    (TaskDestroyFunc)task_btl_riku_3,
    sizeof(BtlRikuWork),
};

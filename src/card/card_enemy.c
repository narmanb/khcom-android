#include "registration_data.h"
#include "card_battle.h"
#include "m4a_song.h"
#include "game_state.h"
#include "obj_api.h"
#include "engine_math.h"
#include "listpool.h"
#include "obj.h"
#include "taskpool.h"
#include "card.h"
#include "sprites_card_pictures.h"
#include "songs.h"
#include "battle_work.h"
#include "boss_card_data.h"
#include "card_def_data.h"
#include "card_types.h"
#include "types.h"
#include <stddef.h>

static s16 sBossCardValue;


u8 EnemyCardDeal(CardDisplayWork* p, void* a);
u8 EnemyCardClosed(CardDisplayWork* p, void* a);
u8 EnemyCardShrinkAway(CardDisplayWork* p);

static const s32 sEnemyCardLayout[10] = {
    0x11000, 0xBC00, 0xDC00, 0x5800, 0xDC00, 0x4400, 0xDC00, 0x3000, 0x10400, 0xB800,
};

void LookupEnemyCardDef(CardDisplayArgs* a, const CardDef** b, u8 c) {
    CardSlot* t;
    s32 v;
    s32 id;

    t = a->slot;
    v = a->variant;

    if (v != -1) {
        ((CardDisplayWork*)((u8*)b - offsetof(CardDisplayWork, cardDef)))->enemyKind = v;
    }

    if (t != NULL) {
        id = t[c].cardId;

        if (id != 0xFFFF) {
            *b = &gCardDefs[id];
        }
    }
}

void LinkEnemyCardDisplay(CardDisplayWork* p) {
    ListNodeInit(&p->node, p->args.pool, p);
    ListPoolAppend(&p->node, p->args.pool);
}

void card_enemy_0(CardDisplayWork* p, CardDisplayArgs* a) {
    p->tiles = NULL;
    p->tiles2 = NULL;
    p->tiles3 = NULL;
    p->palette = NULL;
    p->command = 0;
    p->args = *a;
    p->flags = 0;
    p->priority = 0x50;
    p->timer = 0;
    LookupEnemyCardDef(&p->args, &p->cardDef, p->args.index);
    p->scaleX = 0x100;
    p->scaleY = 0x100;
    p->bobAngle = GetRandom();
    p->angle = 0;
    p->ringRadius = 0;
    p->ringRadiusTarget = 0x2400;
    p->ringCenterX = sEnemyCardLayout[0];
    p->ringCenterY = sEnemyCardLayout[1];
    p->x = 0xDC00;
    p->y = 0x8400;
    p->value = p->cardDef->value;
    LinkEnemyCardDisplay(p);
}

u8 card_enemy_1(CardDisplayWork* p, void* a) {
    if (!(p->flags & CARD_DISP_FLAG_VISIBLE)) {
        if (p->flags & CARD_DISP_FLAG_GFX_LOADED) {
            ReleaseCardDisplayGfx(p);
            p->flags &= ~CARD_DISP_FLAG_GFX_LOADED;
            p->flags |= CARD_DISP_FLAG_FACE_DOWN;
        }
    }

    UpdateCardDisplayFlip(p);

    if (p->flags & CARD_DISP_FLAG_DEALING) {
        p->timer = 8;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardDeal);
    } else if (!(p->flags & CARD_DISP_FLAG_FROZEN)) {
        UpdateEnemyCardRingPosition(p);
        p->bobAngle += 4;
        DispatchEnemyCardCommand(p, a);

        if (!(p->flags & CARD_DISP_FLAG_OPEN)) {
            p->flags &= ~CARD_DISP_FLAG_SETTLED;
            SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardClosed);
        }
    }

    return 1;
}

void EnemyCardDraw(CardDisplayWork* p) {
    void* gfx;
    ObjAffine* affine;
    u16 flags;

    gfx = p->cardDef->gfx;

    if (p->flags & CARD_DISP_FLAG_VISIBLE) {
        if (!(p->flags & CARD_DISP_FLAG_FACE_DOWN)) {
            if (p->flags & CARD_DISP_FLAG_GFX_LOADED) {
                if ((p->flags & CARD_DISP_FLAG_DOUBLE_SIZE) == 0) {
                    affine = AllocObjAffine(p->angle, p->scaleX, p->scaleY, 0);
                } else {
                    affine = AllocObjAffine(p->angle, p->scaleX, p->scaleY, 1);
                }

                flags = SPRITE_PRIORITY(1) | SPRITE_FLAG_NO_MOSAIC;
                DrawSprite(p->x >> 8, (p->y >> 8) + (gSineTable[p->bobAngle] >> 8),
                           gEnemyCardBacks[0].gfx, gCardBattleState->tiles[p->cardDef->category],
                           gCardBattleState->palette, affine, flags, p->priority - 1);
                DrawSprite(p->x >> 8, (p->y >> 8) + (gSineTable[p->bobAngle] >> 8),
                           gfx, p->tiles, p->palette, affine, flags, p->priority);

                if (p->valueModified != 0) {
                    DrawSprite(p->x >> 8, (p->y >> 8) + (gSineTable[p->bobAngle] >> 8),
                               gUnk_09EE981C[p->value], gCardBattleState->tiles7,
                               gCardBattleState->palette2, affine, flags, p->priority - 2);
                } else {
                    DrawSprite(p->x >> 8, (p->y >> 8) + (gSineTable[p->bobAngle] >> 8),
                               gUnk_09EE981C[p->value], gCardBattleState->tiles5,
                               gCardBattleState->palette, affine, flags, p->priority - 2);
                }
            }
        }
    }
}

void EnemyCardDestroy(CardDisplayWork* p) {
    if (p->tiles != NULL) {
        ReleaseCardDisplayGfx(p);
    }

    if (p->palette2 != NULL) {
        ReleaseObjPalette(p->palette2);
    }
}

u8 EnemyCardWaitPlayEnd(CardDisplayWork* p, void* a) {
    if (gBtlWork->flags & BTL_FLAG_CARD_PLAY_ENDED) {
        p->timer = 8;
        p->spinSpeed = 8;
        gCardBattleState->activeCardCount = 0;
        gCardBattleState->activeValue = 0;
        gBtlWork->flags &= ~BTL_FLAG_CARD_PLAY_ENDED;
        gBtlWork->flags &= ~BTL_FLAG_CARD_ACTIVE;
        gBtlWork->flags &= ~BTL_FLAG_OPPONENT_CARD_BUSY;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardShrinkAway);
    } else if (p->flags & CARD_DISP_FLAG_BROKEN) {
        p->priority -= 4;
        p->ringRadius = 0x500;
        p->timer = 0x100;
        p->ringAngle = (u16)(GetRandom() % 33) - 16;
        p->spinSpeed = GetRandom() % 5 + 254;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardBreakFall);
    }

    return 1;
}

u8 EnemyUsecard_1(CardDisplayWork* p, void* a) {
    p->priority = 80;
    ApproachValue(&p->x, 0x7800, p->timer);
    ApproachValue(&p->y, 0x8400, p->timer);

    if ((s16)p->timer > 0) {
        p->timer--;
    }

    if (gBtlWork->flags & BTL_FLAG_CARD_ACTIVE) {
        if (p->flags & CARD_DISP_FLAG_IN_PLAY) {
            if ((s16)p->timer == 0) {
                SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardWaitPlayEnd);
            }
        } else if ((s16)p->timer <= 2) {
            p->priority -= 4;
            p->ringRadius = 0x500;
            p->timer = 0x100;
            p->ringAngle = (u16)(GetRandom() % 33) - 16;
            p->spinSpeed = GetRandom() % 5 + 254;
            SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardFlyOff);
            return 1;
        }
    } else if ((s16)p->timer <= 2) {
        p->priority -= 4;
        p->ringRadius = 0x500;
        p->timer = 0x100;
        p->ringAngle = (u16)(GetRandom() % 33) - 16;
        p->spinSpeed = GetRandom() % 5 + 254;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardFlyOff);
    }

    return 1;
}

u8 EnemyCardDeal(CardDisplayWork* p, void* a) {
    ApproachValue(&p->x, gSineTable[((p->ringAngle >> 8) - 32) & 0xFF] * (p->ringRadius >> 8) + sEnemyCardLayout[0],
                  p->timer);
    ApproachValue(&p->y, -gSineTable[(((p->ringAngle >> 8) - 32) & 0xFF) + 0x40] * (p->ringRadius >> 8) + sEnemyCardLayout[1],
                  p->timer);
    p->timer--;

    if ((s16)p->timer <= 1) {
        p->timer = 0;
        p->flags &= ~CARD_DISP_FLAG_DEALING;
        SetTaskUpdate(a, (TaskUpdateFunc)card_enemy_1);
    }

    return 1;
}

u8 EnemyCardClosed(CardDisplayWork* p, void* a) {
    if (p->command == 7) {
        return 0;
    }

    p->ringRadius += -p->ringRadius >> 1;
    p->x += (sEnemyCardLayout[8] - p->x) >> 1;
    p->y += (sEnemyCardLayout[9] - p->y) >> 1;

    if (p->flags & CARD_DISP_FLAG_OPEN) {
        SetTaskUpdate(a, (TaskUpdateFunc)card_enemy_1);
    }

    return 1;
}

void UpdateEnemyCardRingPosition(CardDisplayWork* p) {
    s32 t;

    if (p->ringAngleTarget - p->ringAngle > 0x7F00) {
        p->ringAngle += 0x10000;
    }

    t = p->ringAngle - 0x10000;

    if (p->ringAngleTarget - t < p->ringAngle - p->ringAngleTarget) {
        p->ringAngle = t;
    }

    p->swingAngle += (p->swingAngleTarget - p->swingAngle) >> 2;
    p->ringRadius += (p->ringRadiusTarget - p->ringRadius) >> 1;
    ApproachValue(&p->ringAngle, p->ringAngleTarget, p->timer);
    p->timer--;

    if ((s16)p->timer <= 1) {
        p->timer = 0;
        p->flags |= CARD_DISP_FLAG_SETTLED;
    } else {
        p->flags &= ~CARD_DISP_FLAG_SETTLED;
    }

    p->x = gSineTable[((p->ringAngle >> 8) - 32) & 0xFF] * (p->ringRadius >> 8) + p->ringCenterX;
    p->y = -gSineTable[(((p->ringAngle >> 8) - 32) & 0xFF) + 64] * (p->ringRadius >> 8) + p->ringCenterY;
}

u8 EnemyCardShrinkAway(CardDisplayWork* p) {
    ApproachValue(&p->y, 0x8200, p->timer);

    if ((s16)p->timer > 0) {
        p->timer--;
    } else {
        p->timer = 0;
    }

    if ((s16)p->timer == 0) {
        p->timer = 0;
        p->angle += p->spinSpeed;
        p->spinSpeed++;

        if (p->scaleX <= 25) {
            return 0;
        }

        p->scaleX -= 25;
        p->scaleY -= 25;
    }

    return 1;
}

u8 EnemyCardFlyOff(CardDisplayWork* p) {
    p->command = 0;
    p->y -= p->ringRadius;
    p->ringRadius -= (s16)p->timer;
    p->timer++;
    p->x -= gSineTable[(p->ringAngle & 0xFF) + 0x40];
    p->angle += p->spinSpeed;
    p->scaleX -= 5;
    p->scaleY -= 5;

    if (IsCardDisplayOffScreen(p)) {
        p->flags &= ~CARD_DISP_FLAG_VISIBLE;
        ReleaseCardDisplayGfx(p);
        p->flags &= ~CARD_DISP_FLAG_GFX_LOADED;
        gBtlWork->flags &= ~BTL_FLAG_OPPONENT_CARD_BUSY;
        return 0;
    }

    return 1;
}

#ifdef PLATFORM_ANDROID
u8 func_08090A54(CardDisplayWork* p, void* a) {
#else
void func_08090A54(CardDisplayWork* p, void* a) {
#endif
    p->x -= gSineTable[p->spinSpeed] * 3;
    UpdateCardDisplayFlip(p);

    if (p->spinSpeed != 0) {
        p->spinSpeed -= 8;
    } else {
        p->spinSpeed = 0;
        p->flags &= ~CARD_DISP_FLAG_VISIBLE;
        SetTaskUpdate(a, (TaskUpdateFunc)card_enemy_1);
    }

    if (!(p->flags & CARD_DISP_FLAG_OPEN)) {
        p->flags &= ~CARD_DISP_FLAG_SETTLED;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardClosed);
    }
#ifdef PLATFORM_ANDROID
    return 1;
#endif
}

#ifdef PLATFORM_ANDROID
u8 func_08090ACC(CardDisplayWork* p, void* a) {
#else
void func_08090ACC(CardDisplayWork* p, void* a) {
#endif
    p->x += gSineTable[p->spinSpeed] * 3;
    UpdateCardDisplayFlip(p);

    if ((s8)p->spinSpeed >= 0) {
        p->spinSpeed += 8;
    } else {
        p->spinSpeed = 0x80;
        p->flags &= ~CARD_DISP_FLAG_SELECTED;
        p->priority = 100;
        SetTaskUpdate(a, (TaskUpdateFunc)func_08090A54);
    }

    if (!(p->flags & CARD_DISP_FLAG_OPEN)) {
        p->flags &= ~CARD_DISP_FLAG_SETTLED;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardClosed);
    }
#ifdef PLATFORM_ANDROID
    return 1;
#endif
}

void DispatchEnemyCardCommand(CardDisplayWork* p, void* a) {
    switch (p->command) {
    case 5:
        p->timer = 16;
        p->priority -= 4;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyUsecard_1);
        break;
    case 6:
        p->timer = 8;
        p->priority -= 4;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyStockMoveToSlot);
        break;
    case 8:
        p->priority -= 4;
        p->ringRadius = 0x500;
        p->timer = 0x100;
        p->ringAngle = (u16)(GetRandom() % 33) - 16;
        p->spinSpeed = GetRandom() % 5 + 254;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardFlyOff);
        break;
    case 7:
        p->ringRadius = 0x500;
        p->timer = 0x100;
        p->ringAngle = (u16)(GetRandom() % 33) - 16;
        p->spinSpeed = GetRandom() % 5 + 254;
        SetTaskUpdate(a, (TaskUpdateFunc)EnemyCardFlyOff);
        break;
    case 9:
        p->spinSpeed = 0;
        p->priority -= 4;
        SetTaskUpdate(a, (TaskUpdateFunc)func_08090ACC);
        p->command = 0;
        break;
    }
}

u8 EnemyStockMoveToSlot(CardDisplayWork* p, void* a) {
    s32 (*tbl)[2]; s32* q;

    if (gBtlWork->paused == 1) {
        return 1;
    }

    UpdateCardDisplayFlip(p);

    if (p->flags & CARD_DISP_FLAG_OPEN) {
        q = &p->x; tbl = (s32 (*)[2])sEnemyCardLayout; ApproachValue(q, tbl[3 - p->stockIndex][0], p->timer); ApproachValue(&p->y, ((s32 (*)[2])sEnemyCardLayout)[3 - p->stockIndex][1], p->timer);
    } else {
        ApproachValue(&p->x, sEnemyCardLayout[8], p->timer);
        ApproachValue(&p->y, sEnemyCardLayout[9], p->timer);
    }

    if ((s16)p->timer > 0) {
        p->timer--;
        p->flags &= ~CARD_DISP_FLAG_SETTLED;
    } else {
        p->timer = 0;
        p->flags |= CARD_DISP_FLAG_SETTLED;
    }

    if (p->command == 5) {
        if (!(gBtlWork->flags & BTL_FLAG_CARD_ACTIVE) && p->stockIndex == 0) {
            gBtlWork->flags |= BTL_FLAG_CARD_ACTIVE;
        }

        if (p->flags & CARD_DISP_FLAG_UNOPPOSED) {
            SetTaskUpdate(a, (TaskUpdateFunc)SoraStockStartUnopposedPlay);
        } else {
            p->timer = 15;
            p->ringRadiusTarget = 0x800;
            p->ringRadius = 0;
            p->ringAngleTarget = gPlayedCardAngles[p->stockIndex] * 2;
            p->ringAngle = 0;
            p->ringCenterX = p->x;
            p->ringCenterY = p->y;
            SetTaskUpdate(a, (TaskUpdateFunc)SoraStockMoveToPlay);
        }
    }

    p->bobAngle += 4;
    return 1;
}

u8 EnemyCardBreakFall(CardDisplayWork* p, void* a) {
    p->command = 0;
    p->y -= p->ringRadius;
    p->ringRadius -= (s16)p->timer >> 1;
    p->timer++;
    p->x += 0x200;
    p->angle += 16;

    if (!(p->flags & CARD_DISP_FLAG_SPIN_MIRRORED)) {
        p->scaleX -= 20;

        if (p->scaleX >= -2 && p->scaleX <= 2) {
            p->scaleX = -20;
        }

        if (p->scaleX <= -0x100) {
            p->scaleX = -0x100;
            p->flags |= CARD_DISP_FLAG_SPIN_MIRRORED;
        }
    } else {
        p->scaleX -= 20;

        if (p->scaleX >= -2 && p->scaleX <= 2) {
            p->scaleX = 20;
        }

        if (p->scaleX >= 0x100) {
            p->scaleX = 0x100;
            p->flags &= ~CARD_DISP_FLAG_SPIN_MIRRORED;
        }
    }

    if (IsCardDisplayOffScreen(p)) {
        p->flags &= ~CARD_DISP_FLAG_VISIBLE;
        ReleaseCardDisplayGfx(p);
        p->flags &= ~CARD_DISP_FLAG_GFX_LOADED;
        gBtlWork->flags &= ~BTL_FLAG_OPPONENT_CARD_BUSY;
        return 0;
    }

    return 1;
}

void func_08090EA0(CardDisplayWork* p, CardDisplayArgs* a) {
    const s32* tbl;
    u8 n;
    s32 id;

    p->tiles = NULL;
    p->tiles2 = NULL;
    p->tiles3 = NULL;
    p->tiles4 = NULL;
    p->tiles5 = NULL;
    p->palette2 = NULL;
    p->palette = NULL;
    p->children = NULL;
    p->command = 0;
    p->args = *a;
    p->flags = 0;
    p->priority = 50;
    p->timer = 0;
    tbl = gEnemyCardIds[p->args.variant];
    n = gEnemyCardCounts[p->args.variant];
    p->enemyKind = p->args.variant;

    if (n == 1) {
        id = tbl[0];
    } else if ((s16)p->args.index != -1) {
        if ((s16)p->args.index > n) {
            id = tbl[GetRandom() % n];
        } else {
            id = tbl[(s16)p->args.index - 1];
        }
    } else {
        if (gCardBattleState->nextEnemyCardIndex > n) {
            gCardBattleState->nextEnemyCardIndex = n;
        }

        id = tbl[gCardBattleState->nextEnemyCardIndex];
        gCardBattleState->nextEnemyCardIndex = GetRandom() % n;
    }

    p->cardDef = &gCardDefs[id];
    p->scaleX = 0x100;
    p->scaleY = 0x100;
    p->bobAngle = GetRandom();
    p->angle = 0;
    p->ringRadius = 0;
    p->ringRadiusTarget = 0x2400;
    p->flags |= (CARD_DISP_FLAG_SELECTED | CARD_DISP_FLAG_VISIBLE);
    p->ringCenterX = 0xDC00;
    p->ringCenterY = 0x8800;
    p->x = 0xDC00;
    p->y = 0x8800;
    p->timer = 10;
    p->priority -= 4;
    LoadCardDisplayGfx(p);
    p->value = p->cardDef->value;

    switch (gGameState.roomEffect) {
    case 1:
        p->value += 2;

        if (p->value > 9) {
            p->value = 9;
        }

        p->valueModified = 1;
        break;
    case 2:
        if (p->value > 2) {
            p->value -= 2;
        } else {
            p->value = 1;
        }

        p->valueModified = 1;
        break;
    default:
        p->valueModified = 0;
        break;
    }

    p->flags |= CARD_DISP_FLAG_GFX_LOADED;
}

void func_08091048(CardDisplayWork* p, CardDisplayArgs* a) {
    const s32* tbl;
    u8 n;
    s32 id;

    p->tiles = NULL;
    p->tiles2 = NULL;
    p->tiles3 = NULL;
    p->palette = NULL;
    p->command = 0;
    p->args = *a;
    p->flags = 0;
    p->priority = 50;
    p->timer = 0;
    tbl = gEnemyCardIds[p->args.variant];
    n = gEnemyCardCounts[p->args.variant];
    p->enemyKind = p->args.variant;

    if (n == 1) {
        id = tbl[0];
    } else if ((s16)p->args.index < n) {
        id = tbl[(s16)p->args.index];
    } else {
        id = tbl[GetRandom() % n];
    }

    p->cardDef = &gCardDefs[id];
    p->scaleX = 0x100;
    p->scaleY = 0x100;
    p->bobAngle = GetRandom();
    p->angle = 0;
    p->ringRadius = 0;
    p->ringRadiusTarget = 0x2400;
    p->flags |= (CARD_DISP_FLAG_FACE_DOWN | CARD_DISP_FLAG_VISIBLE);
    p->ringCenterX = 0x10000;
    p->ringCenterY = 0x8800;
    p->x = 0x10000;
    p->y = 0x8800;
    p->timer = 0x10;
    p->priority -= 4;
    p->value = p->cardDef->value;
}

void func_08091138(CardDisplayWork* p, CardDisplayArgs* a) {
    const s32* tbl;
    u8 n;
    s32 id;

    p->tiles = NULL;
    p->tiles2 = NULL;
    p->tiles3 = NULL;
    p->palette = NULL;
    p->command = 0;
    p->args = *a;
    p->flags = 0;
    p->priority = 50;
    p->timer = 0;
    tbl = gEnemyCardIds[p->args.variant];
    n = gEnemyCardCounts[p->args.variant];
    p->enemyKind = p->args.variant;

    if (n == 1) {
        id = tbl[0];
    } else if ((s16)p->args.index < n) {
        id = tbl[GetRandom() % (s16)p->args.index];
    } else {
        id = tbl[GetRandom() % n];
    }

    p->cardDef = &gCardDefs[id];
    p->scaleX = 0x100;
    p->scaleY = 0x100;
    p->bobAngle = GetRandom();
    p->angle = 0;
    p->ringRadius = 0;
    p->ringRadiusTarget = 0x2400;
    p->flags |= (CARD_DISP_FLAG_FACE_DOWN | CARD_DISP_FLAG_VISIBLE);
    p->ringCenterX = 0x10000;
    p->ringCenterY = 0x8800;
    p->x = 0x10000;
    p->y = 0x8800;
    p->timer = 0x10;
    p->priority -= 4;
    p->value = p->cardDef->value;
}

void UseEnemyCard(u16 arg) {
    CardDisplayArgs args;
    CardDisplayWork* p;
    u8 i;
    u8 flag;
    u8 found;
#ifdef VERSION_EU
    s32 j;
    s32 k;
#endif

    args.pool = NULL;
    args.slot = NULL;
    args.variant = arg;
    args.index = sBossCardValue;
    args.listIndex = 0;
    p = TaskCreate(&gCardBattleState->tasks, &gTaskDescEnemyUsecard, &args)->work;
    gBtlWork->flags |= BTL_FLAG_OPPONENT_CARD_BUSY;
    gCardBattleState->enemyCardUsed = 1;

    if ((gBtlWork->flags & BTL_FLAG_CARD_ACTIVE) == 0) {
        p->flags |= CARD_DISP_FLAG_IN_PLAY;
        gCardBattleState->activeCards[0] = p;
        gCardBattleState->activeValue = p->value;
        gCardBattleState->activeCardCount = 1;
        gBtlWork->soraOwnsPlay = 0;
        gBtlWork->flags |= BTL_FLAG_CARD_PLAY_START;
        gBtlWork->flags |= BTL_FLAG_CARD_ACTIVE;
    } else if ((gBtlWork->flags & BTL_FLAG_CARD_PLAY_ENDED) == 0) {
#ifdef VERSION_EU
        if (gCardBattleState->activeValue <= p->value || p->value == 0) {
#else
        if (gCardBattleState->activeValue <= p->value) {
#endif
            found = 0;

            if (gBtlWork->hcEffect == 2) {
#ifdef VERSION_EU
                if (gCardBattleState->activeCards[0]->cardDef->category == 0 && gCardBattleState->soraStockActive == 0) {
                    found = 1;
                }
#else
                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    if (gCardBattleState->activeCards[i]->cardDef->category == 0) {
                        found = 1;
                        break;
                    }
                }
#endif
            }

            if (gBtlWork->hcEffect == 20) {
#ifdef VERSION_EU
                for (j = 0; j < gCardBattleState->activeCardCount; j++) {
                    if (gCardBattleState->activeCards[j]->cardDef->move == 22) {
                        found = 1;
                    }
                }
#else
                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    if (gCardBattleState->activeCards[i]->cardDef->move == 22) {
                        found = 1;
                        break;
                    }
                }
#endif
            }

            if (gBtlWork->hcEffect == 29) {
#ifdef VERSION_EU
                for (k = 0; k < gCardBattleState->activeCardCount; k++) {
                    if (gCardBattleState->activeCards[k]->cardDef->category == 2 && !(gCardBattleState->activeCards[k]->cardDef->flags & CARD_DEF_FLAG_FRIEND)) {
                        found = 1;
                    }
                }
#else
                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    if (gCardBattleState->activeCards[i]->cardDef->category == 2) {
                        found = 1;
                        break;
                    }
                }
#endif
            }

            if (found == 0) {
                gBtlWork->flags |= BTL_FLAG_CARD_BREAK;

                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    gCardBattleState->activeCards[i]->flags |= CARD_DISP_FLAG_BROKEN;
                }

                if (gCardBattleState->activeValue != p->value) {
                    if (p->value == 0) {
                        gBtlWork->breakDifference = -(s8)gCardBattleState->activeValue;
                    } else {
                        gBtlWork->breakDifference = gCardBattleState->activeValue - p->value;
                    }

                    m4aSongNumStart(SONG_SYS_CARDLOSE);
                    gBtlWork->flags |= BTL_FLAG_CARD_PLAY_START;
                    gBtlWork->flags |= BTL_FLAG_CARD_ACTIVE;
                    gBtlWork->flags &= ~BTL_FLAG_CARD_PLAY_ENDED;
                    gCardBattleState->activeCards[0] = p;

                    if (gBtlWork->hcEffect == 48) {
                        if (p->value != 0) {
                            gCardBattleState->activeValue = p->value - gCardBattleState->activeValue;
                            p->value = gCardBattleState->activeValue;
                        } else {
                            gCardBattleState->activeValue = 0;
                        }

                        if (gCardBattleState->activeValue < 0) {
                            gCardBattleState->activeValue = 0;
                        }

                        gBtlWork->hcEffectCount--;
                    } else {
                        gCardBattleState->activeValue = p->value;
                    }

                    gCardBattleState->activeCardCount = 1;
                    gBtlWork->soraOwnsPlay = 0;
                    p->flags |= CARD_DISP_FLAG_IN_PLAY;
                    AddBreakDarkPoints();
                } else {
                    m4aSongNumStart(SONG_SYS_DROW);
                    gBtlWork->flags &= ~BTL_FLAG_CARD_ACTIVE;
                    gBtlWork->flags &= ~BTL_FLAG_CARD_PLAY_ENDED;
                    gBtlWork->flags &= ~BTL_FLAG_CARD_PLAY_START;
                    gBtlWork->soraOwnsPlay = 0;
                }
            }
        }
    } else {
#ifdef VERSION_EU
        if (gCardBattleState->activeValue <= p->value || p->value == 0) {
#else
        if (gCardBattleState->activeValue <= p->value) {
#endif
            flag = 0;

            if (gBtlWork->hcEffect == 2) {
#ifdef VERSION_EU
                if (gCardBattleState->activeCards[0]->cardDef->category == 0 && gCardBattleState->soraStockActive == 0) {
                    flag = 1;
                }
#else
                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    if (gCardBattleState->activeCards[i]->cardDef->category == 0) {
                        flag = 1;
                        break;
                    }
                }
#endif
            }

#ifndef VERSION_EU
            if (gBtlWork->hcEffect == 54) {
                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    if (gCardBattleState->activeCards[i]->cardDef->category == 1) {
                        flag = 1;
                        break;
                    }
                }
            }
#endif

            if (gBtlWork->hcEffect == 20) {
                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    if (gCardBattleState->activeCards[i]->cardDef->move == 22) {
                        flag = 1;
                        break;
                    }
                }
            }

            if (gBtlWork->hcEffect == 29) {
                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
#ifdef VERSION_EU
                    if (gCardBattleState->activeCards[i]->cardDef->category == 2 && !(gCardBattleState->activeCards[i]->cardDef->flags & CARD_DEF_FLAG_FRIEND)) {
#else
                    if (gCardBattleState->activeCards[i]->cardDef->category == 2) {
#endif
                        flag = 1;
                        break;
                    }
                }
            }

            if (flag == 0) {
                gBtlWork->flags |= BTL_FLAG_CARD_BREAK;

                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    gCardBattleState->activeCards[i]->flags |= CARD_DISP_FLAG_BROKEN;
                }

                if (gCardBattleState->activeValue != p->value) {
                    if (p->value == 0) {
                        gBtlWork->breakDifference = -(s8)gCardBattleState->activeValue;
                    } else {
                        gBtlWork->breakDifference = gCardBattleState->activeValue - p->value;
                    }

                    m4aSongNumStart(SONG_SYS_CARDLOSE);
                    gBtlWork->flags |= BTL_FLAG_CARD_PLAY_START;
                    gBtlWork->flags |= BTL_FLAG_CARD_ACTIVE;
                    gBtlWork->flags &= ~BTL_FLAG_CARD_PLAY_ENDED;
                    gCardBattleState->activeCards[0] = p;

#ifdef VERSION_EU
                    if (gBtlWork->hcEffect == 48) {
                        if (p->value != 0) {
                            gCardBattleState->activeValue = p->value - gCardBattleState->activeValue;
                            p->value = gCardBattleState->activeValue;
                        } else {
                            gCardBattleState->activeValue = 0;
                        }

                        if (gCardBattleState->activeValue < 0) {
                            gCardBattleState->activeValue = 0;
                        }

                        gBtlWork->hcEffectCount--;
                    } else {
                        gCardBattleState->activeValue = p->value;
                    }

#else
                    gCardBattleState->activeValue = p->value;
#endif
                    gCardBattleState->activeCardCount = 1;
                    gBtlWork->soraOwnsPlay = 0;
                    p->flags |= CARD_DISP_FLAG_IN_PLAY;
                    AddBreakDarkPoints();
                } else {
                    m4aSongNumStart(SONG_SYS_DROW);
                    gBtlWork->flags &= ~BTL_FLAG_CARD_ACTIVE;
                    gBtlWork->flags &= ~BTL_FLAG_CARD_PLAY_ENDED;
                    gBtlWork->flags &= ~BTL_FLAG_CARD_PLAY_START;
                    gBtlWork->soraOwnsPlay = 0;
                }
            }
        }
    }

    p->flags = (p->flags | CARD_DISP_FLAG_SELECTED) & ~CARD_DISP_FLAG_SETTLED;
}

void func_080917C8(u16 a, u8 b) {
    CardDisplayArgs arg;
    CardDisplayWork* p;
    u8 i;

    arg.pool = NULL;
    arg.slot = NULL;
    arg.variant = a;
    arg.index = b;
    arg.listIndex = 0;
    p = TaskCreate(&gCardBattleState->tasks, &gUnk_09EE4B70, &arg)->work;
    gBtlWork->flags |= BTL_FLAG_OPPONENT_CARD_BUSY;

    if ((gBtlWork->flags & BTL_FLAG_CARD_ACTIVE) == 0) {
        p->flags |= CARD_DISP_FLAG_IN_PLAY;
        gCardBattleState->activeCards[0] = p;
        gCardBattleState->activeValue = p->cardDef->value;
        gCardBattleState->activeCardCount = 1;
        gBtlWork->soraOwnsPlay = 0;
        gBtlWork->flags |= BTL_FLAG_CARD_PLAY_START;
        gBtlWork->flags |= BTL_FLAG_CARD_ACTIVE;
    } else if ((gBtlWork->flags & BTL_FLAG_CARD_PLAY_ENDED) == 0) {
        if (gCardBattleState->soraHcEffect != 2) {
            if (gCardBattleState->activeValue < p->cardDef->value) {
                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    gCardBattleState->activeCards[i]->flags |= CARD_DISP_FLAG_BROKEN;
                }

                m4aSongNumStart(SONG_SYS_CARDLOSE);
                gBtlWork->flags |= BTL_FLAG_CARD_BREAK;
                gBtlWork->flags |= BTL_FLAG_CARD_PLAY_START;
                gBtlWork->flags |= BTL_FLAG_CARD_ACTIVE;
                gCardBattleState->activeCards[0] = p;
                gCardBattleState->activeValue = p->cardDef->value;
                gCardBattleState->activeCardCount = 1;
                gBtlWork->soraOwnsPlay = 0;
                p->flags |= CARD_DISP_FLAG_IN_PLAY;
            }
        }
    }

    p->flags |= CARD_DISP_FLAG_SELECTED;
    p->flags &= ~CARD_DISP_FLAG_SETTLED;
}

void func_08091978(u16 a, u8 b) {
    CardDisplayArgs arg;
    CardDisplayWork* p;
    u8 i;

    arg.pool = NULL;
    arg.slot = NULL;
    arg.variant = a;
    arg.index = b;
    arg.listIndex = 0;
    p = TaskCreate(&gCardBattleState->tasks, &gUnk_09EE4B88, &arg)->work;
    gBtlWork->flags |= BTL_FLAG_OPPONENT_CARD_BUSY;

    if ((gBtlWork->flags & BTL_FLAG_CARD_ACTIVE) == 0) {
        p->flags |= CARD_DISP_FLAG_IN_PLAY;
        gCardBattleState->activeCards[0] = p;
        gCardBattleState->activeValue = p->cardDef->value;
        gCardBattleState->activeCardCount = 1;
        gBtlWork->soraOwnsPlay = 0;
        gBtlWork->flags |= BTL_FLAG_CARD_PLAY_START;
        gBtlWork->flags |= BTL_FLAG_CARD_ACTIVE;
    } else if (gBtlWork->flags & BTL_FLAG_CARD_PLAY_ENDED) {
        if (gBtlWork->hcEffect == 2) {
            if (gCardBattleState->activeValue < p->cardDef->value) {
                for (i = 0; i < gCardBattleState->activeCardCount; i++) {
                    gCardBattleState->activeCards[i]->flags |= CARD_DISP_FLAG_BROKEN;
                }

                m4aSongNumStart(SONG_SYS_CARDLOSE);
                gBtlWork->flags |= BTL_FLAG_CARD_BREAK;
                gBtlWork->flags |= BTL_FLAG_CARD_PLAY_START;
                gBtlWork->flags |= BTL_FLAG_CARD_ACTIVE;
                gCardBattleState->activeCards[0] = p;
                gCardBattleState->activeValue = p->cardDef->value;
                gCardBattleState->activeCardCount = 1;
                gBtlWork->soraOwnsPlay = 0;
                p->flags |= CARD_DISP_FLAG_IN_PLAY;
            }
        }
    }

    p->flags |= CARD_DISP_FLAG_SELECTED;
    p->flags &= ~CARD_DISP_FLAG_SETTLED;
}

void ResetBossCardValue() {
    sBossCardValue = -1;
}

void SetBossCardValue(u16 a) {
    sBossCardValue = a;
}

u16 GetBossCardValue() {
    if (sBossCardValue != -1) {
        return sBossCardValue;
    }

    return gCardBattleState->nextEnemyCardIndex;
}

TaskDesc gTaskDescCardEnemy = {
    "card_enemy",
    (TaskInitFunc)card_enemy_0,
    (TaskUpdateFunc)card_enemy_1,
    (TaskDrawFunc)EnemyCardDraw,
    (TaskDestroyFunc)EnemyCardDestroy,
    sizeof(CardDisplayWork),
};

TaskDesc gTaskDescEnemyUsecard = {
    "EnemyUsecard",
    (TaskInitFunc)func_08090EA0,
    (TaskUpdateFunc)EnemyUsecard_1,
    (TaskDrawFunc)EnemyCardDraw,
    (TaskDestroyFunc)EnemyCardDestroy,
    sizeof(CardDisplayWork),
};

TaskDesc gUnk_09EE4B70 = {
    "EnemyUsecard",
    (TaskInitFunc)func_08091048,
    (TaskUpdateFunc)EnemyUsecard_1,
    (TaskDrawFunc)EnemyCardDraw,
    (TaskDestroyFunc)EnemyCardDestroy,
    sizeof(CardDisplayWork),
};

TaskDesc gUnk_09EE4B88 = {
    "EnemyUsecard",
    (TaskInitFunc)func_08091138,
    (TaskUpdateFunc)EnemyUsecard_1,
    (TaskDrawFunc)EnemyCardDraw,
    (TaskDestroyFunc)EnemyCardDestroy,
    sizeof(CardDisplayWork),
};

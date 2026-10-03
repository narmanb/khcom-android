#include "macros.h"
#include "player_progression.h"
#include "game_state.h"
#include "display.h"
#include "taskpool.h"
#include "gba/syscall.h"
#include "malloc.h"
#include "card.h"
#include "map_tile_animations.h"
#include "evt.h"
#include "card_deck.h"
#include "card_api.h"
#include "card_def_data.h"
#include "card_types.h"
#include "evt_types.h"
#include "map_animation_types.h"
#include "player_progression_types.h"
#include "types.h"
#include "gba/macro.h"
#include <stddef.h>

Deck gDecks[3] EWRAM_COMMON(16);

u16 gCardCollection[999] EWRAM_COMMON(16);

Deck* gLinkPartnerDeck EWRAM_COMMON(4);

Deck* gLinkSendDeck EWRAM_COMMON(4);

u16 gCardCount EWRAM_COMMON(4);

void map_anim_0(MapTileAnimationWork* p) {
    u8 i;

#ifdef PLATFORM_ANDROID
    /* Out-of-range entries read zeroed/open-bus-adjacent data on GBA. */
    p->definition = (u32)gEventState->mapAnim < 6
        ? gMapTileAnimationDefs[gEventState->mapAnim]
        : NULL;
#else
    p->definition = gMapTileAnimationDefs[gEventState->mapAnim];
#endif

    if (p->definition != NULL) {
        for (i = 0; i < p->definition->trackCount; i++) {
            p->frameTimers[i] = 0;
            p->frameIndices[i] = 0;
        }
    }

    p->firstTrack = 0;
}

u8 map_anim_1(MapTileAnimationWork* w) {
    const MapTileAnimationDef* a;
    const MapTileAnimationTrack* e;
    const MapTileAnimationFrame* f;
    const MapTileAnimationFrame* f2;
    u8 i;
    u8* dst;

    a = w->definition;

    if (a == NULL) {
        return 1;
    }

    for (i = w->firstTrack; i < (a = w->definition)->trackCount; i++) {
        e = &a->tracks[i];
        f = &e->frames[w->frameIndices[i]];
        w->frameTimers[i]++;

        if (w->frameTimers[i] == f->duration) {
            w->frameIndices[i]++;

            if (w->frameIndices[i] == e->frameCount) {
                w->frameIndices[i] = 0;
            }

            f2 = &e->frames[w->frameIndices[i]];
            dst = (u8*)GetBgCharBase(3) + 0x7000;
            RequestDma3Copy(e->tiles + f2->tileOffset, dst + e->destOffset, e->copySize);
            w->frameTimers[i] = 0;
        }
    }

    return 1;
}

void map_anim_2() {
}

void map_anim_3() {
}

Deck* CreateLinkSendDeck() {
    Deck* active;
    s32 i;

    active = GetActiveDeck();
    gLinkSendDeck = EwramAlloc(sizeof(Deck));

    for (i = 0; i < 99; i++) {
        gLinkSendDeck->cards[i] |= 0xFFFF;
    }

    for (i = 0; i < 99; i++) {
        if (active->cards[i] != 0xFFFF) {
            gLinkSendDeck->cards[i] = gCardCollection[active->cards[i]];
        } else {
            gLinkSendDeck->cards[i] |= 0xFFFF;
        }
    }

    for (i = 0; i < 20; i++) {
        gLinkSendDeck->name[i] = gDecks[GetActiveDeckIndex()].name[i];
    }

    gLinkSendDeck->cpCost = GetDeckCpCost(GetActiveDeckIndex());
    gLinkSendDeck->cardCount = GetDeckCardCount(GetActiveDeckIndex());
    return gLinkSendDeck;
}

void FreeLinkSendDeck() {
    EwramFree(gLinkSendDeck);
}

Deck* CreateLinkPartnerDeck() {
    s32 i;

    gLinkPartnerDeck = EwramAlloc(sizeof(Deck));

    for (i = 0; i < 99; i++) {
        gLinkPartnerDeck->cards[i] |= 0xFFFF;
    }

    for (i = 0; i < 20; i++) {
        gLinkPartnerDeck->name[i] = 0;
    }

    gLinkPartnerDeck->cpCost = 0;
    gLinkPartnerDeck->cardCount = 0;
    return gLinkPartnerDeck;
}

void FreeLinkPartnerDeck() {
    EwramFree(gLinkPartnerDeck);
}

u16 GetLinkPartnerDeckCardCount() {
    return gLinkPartnerDeck->cardCount;
}

u16 CountLinkPartnerDeckCardsOfCategory(u8 slot) {
    u16* cards;
    u16 count;
    u16 i;

    count = 0;
    cards = gLinkPartnerDeck->cards;

    for (i = 0; i < DECK_SIZE; i++) {
        if (cards[i] != 0xFFFF) {
            if (gCardDefs[cards[i] & CARD_ID_MASK].category == slot) {
                count++;
            }
        }
    }

    return count;
}

u16 CountLinkPartnerDeckCards(u8 mode) {
    u8 slot;
    u16* cards;
    u16 count;
    u16 i;

    count = 0;
    cards = gLinkPartnerDeck->cards;

    switch (mode) {
    case 0:
        for (i = 0; i < DECK_SIZE; i++) {
            if (cards[i] != 0xFFFF) {
                slot = gCardDefs[cards[i] & CARD_ID_MASK].category;

                if (slot <= 2) {
                    count++;
                }
            }
        }

        break;
    case 3:
        for (i = 0; i < DECK_SIZE; i++) {
            if (cards[i] != 0xFFFF) {
                if (gCardDefs[cards[i] & CARD_ID_MASK].category == 3) {
                    count++;
                }
            }
        }

        break;
    }

    return count;
}

Deck* GetLinkPartnerDeck() {
    return gLinkPartnerDeck;
}

void CopyLinkPartnerDeckCards(u8 kind, u16* out) {
    Deck* deck;
    u16 i;

    deck = GetLinkPartnerDeck();

    for (i = 0; i < 99; i++) {
        if (deck->cards[i] != 0xFFFF) {
            switch (kind) {
            case 0:
                if (gCardDefs[deck->cards[i] & CARD_ID_MASK].category <= 2) {
                    *out++ = deck->cards[i];
                }

                break;
            case 3:
                if (gCardDefs[deck->cards[i] & CARD_ID_MASK].category == 3) {
                    *out++ = deck->cards[i];
                }

                break;
            }
        }
    }
}

void ObtainCardIntoActiveDeck(u16 a) {
    s16 v;

    v = ObtainCard(a);

    if (gCardDefs[a].value + GetDeckCpCost(GetActiveDeckIndex()) <=
            gGameState.progression.cp &&
        v != -1) {
        AddCardToActiveDeck(v);
    }
}

void InitCardCollection() {
    u16 i;

    for (i = 0; i < 999; i++) {
        gCardCollection[i] = CARD_ID_MASK;
    }

    gCardCount = 911;
}

u16 CountCardsById(u16 cardId) {
    u16 i;
    u16 count;

    i = 0;
    count = 0;

    for (; i < gCardCount; i++) {
        if ((gCardCollection[i] & CARD_ID_MASK) == cardId) {
            count++;
        }
    }

    return count;
}

s16 AddCardToCollection(u16 cardId) {
    u16 i;

    i = 0;

    if (CountCardsById(cardId) > 98) {
        return -1;
    }

    while (gCardCollection[i] != CARD_ID_MASK) {
        i++;

        if (i == gCardCount) {
            return -1;
        }
    }

    if (gCardDefs[cardId & CARD_ID_MASK].flags & CARD_DEF_FLAG_FRIEND) {
        return -1;
    }

    gCardCollection[i] = cardId;

    return i;
}

u8 IsCardCollectionFull() {
    s32 count;
    s32 i;

    count = 0;

    for (i = 0; i < gCardCount; i++) {
        if (gCardCollection[i] == CARD_ID_MASK) {
            count++;
        }
    }

    if (count > 0) {
        return 0;
    }

    return 1;
}

void ExpandCardCollectionForNewCard(u16 cardId) {
    u16 v;
    u8 n;

    v = gCardDefs[cardId & CARD_ID_MASK].kind;

    if (v > 0x61) {
        if (gCardCount > 999) {
            gCardCount = 999;
        }

        return;
    }

    switch (v) {
    case 0:
        n = 119;
        break;
    case 8:
        n = 127;
        break;
    case 1:
        n = 120;
        break;
    case 2:
        n = 121;
        break;
    case 3:
        n = 122;
        break;
    case 4:
        n = 123;
        break;
    case 5:
        n = 124;
        break;
    case 6:
        n = 125;
        break;
    case 7:
        n = 126;
        break;
    case 9:
        n = 128;
        break;
    case 10:
        n = 129;
        break;
    case 11:
        n = 130;
        break;
    case 12:
        n = 131;
        break;
    case 13:
        n = 132;
        break;
    case 16:
        n = 133;
        break;
    case 14:
        n = 134;
        break;
    case 15:
        n = 135;
        break;
    case 18:
        n = 136;
        break;
    case 19:
        n = 137;
        break;
    case 20:
        n = 138;
        break;
    case 21:
        n = 139;
        break;
    case 22:
        n = 140;
        break;
    case 23:
        n = 141;
        break;
    case 24:
        n = 142;
        break;
    case 25:
        n = 143;
        break;
    case 26:
        n = 144;
        break;
    case 27:
        n = 145;
        break;
    case 28:
        n = 146;
        break;
    case 29:
        n = 147;
        break;
    case 30:
        n = 148;
        break;
    case 31:
        n = 149;
        break;
    case 32:
        n = 150;
        break;
    case 33:
        n = 151;
        break;
    case 34:
        n = 152;
        break;
    case 35:
        n = 153;
        break;
    case 36:
        n = 154;
        break;
    case 37:
        n = 155;
        break;
    case 38:
        n = 156;
        break;
    case 47:
        n = 164;
        break;
    case 50:
        n = 165;
        break;
    case 51:
        n = 166;
        break;
    case 52:
        n = 167;
        break;
    case 53:
        n = 168;
        break;
    case 61:
        n = 169;
        break;
    case 73:
        n = 170;
        break;
    case 74:
        n = 171;
        break;
    case 48:
        n = 172;
        break;
    case 54:
        n = 173;
        break;
    case 55:
        n = 174;
        break;
    case 56:
        n = 175;
        break;
    case 57:
        n = 176;
        break;
    case 59:
        n = 177;
        break;
    case 60:
        n = 178;
        break;
    case 62:
        n = 179;
        break;
    case 64:
        n = 180;
        break;
    case 65:
        n = 181;
        break;
    case 66:
        n = 182;
        break;
    case 67:
        n = 183;
        break;
    case 68:
        n = 184;
        break;
    case 70:
        n = 185;
        break;
    case 71:
        n = 186;
        break;
    case 72:
        n = 187;
        break;
    case 49:
        n = 188;
        break;
    case 58:
        n = 189;
        break;
    case 63:
        n = 190;
        break;
    case 69:
        n = 191;
        break;
    case 76:
        n = 192;
        break;
    case 77:
        n = 193;
        break;
    case 75:
        n = 194;
        break;
    case 81:
        n = 204;
        break;
    case 78:
        n = 195;
        break;
    case 86:
        n = 200;
        break;
    case 80:
        n = 197;
        break;
    case 79:
        n = 201;
        break;
    case 85:
        n = 198;
        break;
    case 87:
        n = 199;
        break;
    case 89:
        n = 203;
        break;
    case 88:
        n = 202;
        break;
    case 84:
        n = 196;
        break;
    case 90:
        n = 247;
        break;
    case 91:
        n = 206;
        break;
    case 92:
        n = 205;
        break;
    case 93:
        n = 207;
        break;
    case 94:
        n = 208;
        break;
    case 82:
    case 83:
        n = 246;
        break;
    case 96:
        n = 248;
        break;
    case 97:
        n = 249;
        break;
    default:
        if (gCardCount > 999) {
            gCardCount = 999;
        }

        return;
    }

    if (IsJiminyFlagSet(n) == 0) {
        gCardCount++;
    }

    if (gCardCount > 999) {
        gCardCount = 999;
    }
}

s16 ObtainCard(u16 cardId) {
    u16 i = 0;

    if (CountCardsById(cardId) > 98) {
        return -1;
    }

    ExpandCardCollectionForNewCard(cardId);

    while (gCardCollection[i] != CARD_ID_MASK) {
        i++;

        if (i == gCardCount) {
            return -1;
        }
    }

    if (gCardDefs[cardId & CARD_ID_MASK].flags & CARD_DEF_FLAG_FRIEND) {
        return -1;
    }

    gCardCollection[i] = cardId;

    switch (gCardDefs[cardId & CARD_ID_MASK].kind) {
    case 0:
        SetCardKindObtained(0);
        SetJiminyFlag(119);
        break;
    case 8:
        SetCardKindObtained(8);
        SetJiminyFlag(127);
        break;
    case 1:
        SetCardKindObtained(1);
        SetJiminyFlag(120);
        break;
    case 2:
        SetCardKindObtained(2);
        SetJiminyFlag(121);
        break;
    case 3:
        SetCardKindObtained(3);
        SetJiminyFlag(122);
        break;
    case 4:
        SetCardKindObtained(4);
        SetJiminyFlag(123);
        break;
    case 5:
        SetCardKindObtained(5);
        SetJiminyFlag(124);
        break;
    case 6:
        SetCardKindObtained(6);
        SetJiminyFlag(125);
        break;
    case 7:
        SetCardKindObtained(7);
        SetJiminyFlag(126);
        break;
    case 9:
        SetCardKindObtained(9);
        SetJiminyFlag(128);
        break;
    case 10:
        SetCardKindObtained(10);
        SetJiminyFlag(129);
        break;
    case 11:
        SetCardKindObtained(11);
        SetJiminyFlag(130);
        break;
    case 12:
        SetCardKindObtained(12);
        SetJiminyFlag(131);
        break;
    case 13:
        SetCardKindObtained(13);
        SetJiminyFlag(132);
        break;
    case 16:
        SetCardKindObtained(14);
        SetJiminyFlag(133);
        break;
    case 14:
        SetCardKindObtained(15);
        SetJiminyFlag(134);
        break;
    case 15:
        SetCardKindObtained(16);
        SetJiminyFlag(135);
        break;
    case 18:
        SetCardKindObtained(17);
        LearnStock(9);
        LearnStock(10);
        SetJiminyFlag(136);
        break;
    case 19:
        SetCardKindObtained(18);
        LearnStock(11);
        LearnStock(12);
        SetJiminyFlag(137);
        break;
    case 20:
        SetCardKindObtained(19);
        LearnStock(13);
        LearnStock(14);
        SetJiminyFlag(138);
        break;
    case 21:
        SetCardKindObtained(20);
        LearnStock(15);
        LearnStock(16);
        SetJiminyFlag(139);
        break;
    case 22:
        SetCardKindObtained(21);
        LearnStock(17);
        LearnStock(18);
        SetJiminyFlag(140);
        break;
    case 23:
        SetCardKindObtained(22);
        LearnStock(19);
        LearnStock(20);
        SetJiminyFlag(141);
        break;
    case 24:
        SetCardKindObtained(23);
        LearnStock(21);
        LearnStock(22);
        SetJiminyFlag(142);
        break;
    case 25:
        SetCardKindObtained(24);
        LearnStock(47);
        SetJiminyFlag(143);
        SetJiminyFlag(23);
        break;
    case 26:
        SetCardKindObtained(25);
        LearnStock(52);
        SetJiminyFlag(144);
        break;
    case 27:
        SetCardKindObtained(26);
        LearnStock(49);
        LearnStock(50);
        SetJiminyFlag(145);
        SetJiminyFlag(25);
        break;
    case 28:
        SetCardKindObtained(27);
        LearnStock(48);
        SetJiminyFlag(146);
        SetJiminyFlag(24);
        break;
    case 29:
        SetCardKindObtained(28);
        LearnStock(53);
        SetJiminyFlag(147);
        break;
    case 30:
        SetCardKindObtained(29);
        LearnStock(51);
        SetJiminyFlag(148);
        SetJiminyFlag(26);
        break;
    case 31:
        SetCardKindObtained(30);
        LearnStock(54);
        LearnStock(55);
        SetJiminyFlag(149);
        break;
    case 32:
        SetCardKindObtained(31);
        SetJiminyFlag(150);
        break;
    case 33:
        SetCardKindObtained(32);
        SetJiminyFlag(151);
        break;
    case 34:
        SetCardKindObtained(33);
        SetJiminyFlag(152);
        break;
    case 35:
        SetCardKindObtained(34);
        SetJiminyFlag(153);
        break;
    case 36:
        SetCardKindObtained(35);
        SetJiminyFlag(154);
        break;
    case 37:
        SetCardKindObtained(36);
        SetJiminyFlag(155);
        break;
    case 38:
        SetCardKindObtained(37);
        SetJiminyFlag(156);
        break;
    case 47:
        SetJiminyFlag(164);
        break;
    case 50:
        SetJiminyFlag(165);
        break;
    case 51:
        SetJiminyFlag(166);
        break;
    case 52:
        SetJiminyFlag(167);
        break;
    case 53:
        SetJiminyFlag(168);
        break;
    case 61:
        SetJiminyFlag(169);
        break;
    case 73:
        SetJiminyFlag(170);
        break;
    case 74:
        SetJiminyFlag(171);
        break;
    case 48:
        SetJiminyFlag(172);
        break;
    case 54:
        SetJiminyFlag(173);
        break;
    case 55:
        SetJiminyFlag(174);
        break;
    case 56:
        SetJiminyFlag(175);
        break;
    case 57:
        SetJiminyFlag(176);
        break;
    case 59:
        SetJiminyFlag(177);
        break;
    case 60:
        SetJiminyFlag(178);
        break;
    case 62:
        SetJiminyFlag(179);
        break;
    case 64:
        SetJiminyFlag(180);
        break;
    case 65:
        SetJiminyFlag(181);
        break;
    case 66:
        SetJiminyFlag(182);
        break;
    case 67:
        SetJiminyFlag(183);
        break;
    case 68:
        SetJiminyFlag(184);
        break;
    case 70:
        SetJiminyFlag(185);
        break;
    case 71:
        SetJiminyFlag(186);
        break;
    case 72:
        SetJiminyFlag(187);
        break;
    case 49:
        SetJiminyFlag(188);
        break;
    case 58:
        SetJiminyFlag(189);
        break;
    case 63:
        SetJiminyFlag(190);
        break;
    case 69:
        SetJiminyFlag(191);
        break;
    case 76:
        SetJiminyFlag(192);
        break;
    case 77:
        SetJiminyFlag(193);
        break;
    case 75:
        SetJiminyFlag(194);
        break;
    case 81:
        SetJiminyFlag(204);
        break;
    case 78:
        SetJiminyFlag(195);
        break;
    case 86:
        SetJiminyFlag(200);
        break;
    case 80:
        SetJiminyFlag(197);
        break;
    case 79:
        SetJiminyFlag(201);
        break;
    case 85:
        SetJiminyFlag(198);
        break;
    case 87:
        SetJiminyFlag(199);
        break;
    case 89:
        SetJiminyFlag(203);
        break;
    case 88:
        SetJiminyFlag(202);
        break;
    case 84:
        SetJiminyFlag(196);
        break;
    case 90:
        SetJiminyFlag(247);
        break;
    case 91:
        SetJiminyFlag(206);
        break;
    case 92:
        SetJiminyFlag(205);
        break;
    case 93:
        SetJiminyFlag(207);
        break;
    case 94:
        SetJiminyFlag(208);
        break;
    case 82:
    case 83:
        SetJiminyFlag(246);
        break;
    case 96:
        SetJiminyFlag(248);
        break;
    case 97:
        SetJiminyFlag(249);
        break;
    }

    SetRikuCardKindObtained(gCardDefs[cardId].kind);
    return i;
}

void SetRikuCardKindObtained(u16 a) {
    if (gGameState.flags & GAME_FLAG_RIKU) {
        switch (a) {
        case 0x51:
            SetCardKindObtained(44);
            break;
        case 0x4E:
            SetCardKindObtained(38);
            break;
        case 0x56:
            SetCardKindObtained(45);
            break;
        case 0x50:
            SetCardKindObtained(40);
            break;
        case 0x4F:
            SetCardKindObtained(42);
            break;
        case 0x55:
            SetCardKindObtained(39);
            break;
        case 0x57:
            SetCardKindObtained(41);
            break;
        case 0x59:
            SetCardKindObtained(43);
            break;
        case 0x58:
            SetCardKindObtained(48);
            break;
        case 0x54:
            SetCardKindObtained(50);
            break;
        case 0x5A:
            SetCardKindObtained(51);
            break;
        case 0x5D:
            SetCardKindObtained(54);
            break;
        case 0x60:
            SetCardKindObtained(57);
            break;
        }
    }
}

u16 CountCollectionCards() {
    u16 count;
    u16 i;

    count = i = 0;

    for (i = 0; i < gCardCount; i++) {
        if (gCardCollection[i] != CARD_ID_MASK) {
            count++;
        }
    }

    return count;
}

u16 CountCardsInDecks() {
    u16 count;
    u16 i;

    count = i = 0;

    for (i = 0; i < gCardCount; i++) {
        if (gCardCollection[i] != CARD_ID_MASK && (gCardCollection[i] & 0x7000)) {
            count++;
        }
    }

    return count;
}

u16 ListCardKindsNotInDeck(u8 deck, u8 mode, u16* out) {
    u16 count;
    u16 total;
    u16 mask;
    u16* present;
    u16 i;

    mask = total = count = 0;
    present = EwramAlloc(0x23C);
    CpuFill32(0, present, 0x23C);

    if (mode == 1) {
        switch (deck) {
        case 0:
            mask = 0x1000;
            break;
        case 1:
            mask = 0x2000;
            break;
        case 2:
            mask = 0x4000;
            break;
        }
    } else {
        mask = 0x7000;
    }

    for (i = 0; i < gCardCount; i++) {
        if (gCardCollection[i] == CARD_ID_MASK) {
            continue;
        }

        if (gCardCollection[i] & mask) {
            continue;
        }

        if (!(gCardCollection[i] & 0x8000)) {
            present[gCardDefs[gCardCollection[i] & CARD_ID_MASK].kind] = 1;
        } else {
            present[gCardDefs[gCardCollection[i] & CARD_ID_MASK].kind + 0x8F] = 1;
        }
    }

    for (i = 0; i < 0x11E; i++) {
        if (present[i] != 0) {
            total += present[i];
            out[count++] = i;
        }
    }

    EwramFree(present);
    return total;
}

static const MapTileAnimationFrame sMapTileAnim0Frames0[4] = {
    {0, 18, 0},
    {1024, 12, 0},
    {2048, 12, 0},
    {1024, 12, 0},
};

static const MapTileAnimationFrame sMapTileAnim0Frames1[4] = {
    {0, 15, 0},
    {3232, 15, 0},
    {6464, 15, 0},
    {9696, 15, 0},
};

static const MapTileAnimationTrack sMapTileAnim0Tracks[2] = {
    {sMapTileAnim0Frames0, gUnk_09468FF8, 4, 0, 3232, 864, {0, 0}},
    {sMapTileAnim0Frames1, gUnk_09469B58, 4, 0, 0, 3232, {0, 0}},
};

static const MapTileAnimationDef sMapTileAnim0Def = {
    sMapTileAnim0Tracks, 2, 1, 0,
};

static const MapTileAnimationFrame sMapTileAnim1Frames0[4] = {
    {0, 70, 0},
    {192, 7, 0},
    {384, 15, 0},
    {192, 7, 0},
};

static const MapTileAnimationFrame sMapTileAnim1Frames1[4] = {
    {0, 25, 0},
    {192, 7, 0},
    {384, 15, 0},
    {192, 7, 0},
};

static const MapTileAnimationFrame sMapTileAnim1Frames2[4] = {
    {0, 10, 0},
    {96, 10, 0},
    {192, 10, 0},
    {96, 10, 0},
};

static const MapTileAnimationFrame sMapTileAnim1Frames3[4] = {
    {0, 50, 0},
    {128, 7, 0},
    {256, 10, 0},
    {128, 10, 0},
};

static const MapTileAnimationFrame sMapTileAnim1Frames4[4] = {
    {0, 10, 0},
    {352, 20, 0},
    {0, 7, 0},
    {352, 150, 0},
};

static const MapTileAnimationFrame sMapTileAnim1Frames5[4] = {
    {0, 50, 0},
    {128, 5, 0},
    {0, 7, 0},
    {128, 5, 0},
};

static const MapTileAnimationTrack sMapTileAnim1Tracks[6] = {
    {sMapTileAnim1Frames0, gUnk_098EA844, 4, 0, 2048, 192, {0, 0}},
    {sMapTileAnim1Frames1, gUnk_098EAA84, 4, 0, 2240, 192, {0, 0}},
    {sMapTileAnim1Frames2, gUnk_098EACC4, 4, 0, 2432, 96, {0, 0}},
    {sMapTileAnim1Frames3, gUnk_098EADE4, 4, 0, 2528, 128, {0, 0}},
    {sMapTileAnim1Frames4, gUnk_098EAF64, 4, 0, 3072, 352, {0, 0}},
    {sMapTileAnim1Frames5, gUnk_098EB224, 4, 0, 3424, 128, {0, 0}},
};

static const MapTileAnimationDef sMapTileAnim1Def = {
    sMapTileAnim1Tracks, 6, 0, 0,
};

static const MapTileAnimationFrame sMapTileAnim2Frames[5] = {
    {0, 6, 0},
    {1024, 6, 0},
    {2048, 6, 0},
    {3072, 6, 0},
    {4096, 6, 0},
};

static const MapTileAnimationTrack sMapTileAnim2Track = {
    sMapTileAnim2Frames, gUnk_0948A918, 5, 0, 3072, 896, {0, 0},
};

static const MapTileAnimationDef sMapTileAnim2Def = {
    &sMapTileAnim2Track, 1, 1, 0,
};

static const MapTileAnimationFrame sMapTileAnim3Frames[4] = {
    {0, 30, 0},
    {3072, 30, 0},
    {6144, 30, 0},
    {9216, 30, 0},
};

static const MapTileAnimationTrack sMapTileAnim3Track = {
    sMapTileAnim3Frames, gUnk_094EABF8, 4, 0, -15360, 3072, {0, 0},
};

static const MapTileAnimationDef sMapTileAnim3Def = {
    &sMapTileAnim3Track, 1, 1, 0,
};

static const MapTileAnimationFrame sMapTileAnim4Frames[6] = {
    {0, 20, 0},
    {1024, 20, 0},
    {2048, 20, 0},
    {3072, 20, 0},
    {4096, 20, 0},
    {5120, 20, 0},
};

static const MapTileAnimationTrack sMapTileAnim4Track = {
    sMapTileAnim4Frames, gUnk_094F4238, 6, 0, -5120, 1024, {0, 0},
};

static const MapTileAnimationDef sMapTileAnim4Def = {
    &sMapTileAnim4Track, 1, 1, 0,
};

const MapTileAnimationDef* gMapTileAnimationDefs[6] = {
    &sMapTileAnim0Def,
    &sMapTileAnim1Def,
    &sMapTileAnim2Def,
    &sMapTileAnim3Def,
    &sMapTileAnim4Def,
    NULL,
};

const MapTileAnimationDef* gUnk_09EE4A44 = {
    NULL,
};

const MapTileAnimationDef* gUnk_09EE4A48 = {
    NULL,
};

const MapTileAnimationDef* gUnk_09EE4A4C = {
    NULL,
};

TaskDesc gTaskDescMapAnim = {
    "map_anim",
    (TaskInitFunc)map_anim_0,
    (TaskUpdateFunc)map_anim_1,
    (TaskDrawFunc)map_anim_2,
    (TaskDestroyFunc)map_anim_3,
    sizeof(MapTileAnimationWork),
};

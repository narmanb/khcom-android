#ifndef GUARD_CARD_H
#define GUARD_CARD_H

#include "card_help_data.h"
#include "card_message_data.h"
#include "anim.h"
#include "card_description_data.h"
#include "msg_types.h"
#include "card_ui_types.h"
#include "types.h"
#include "text_types.h"
#include "obj.h"
#include "taskpool.h"
#include "listpool.h"
#include "battle_actor_types.h"
#include "card_types.h"
#include "fld_types.h"
#include "mode.h"
#include "event_background_types.h"
#include "map_animation_types.h"

typedef struct PrizeMapCardBackAnimStep {
    u8 sprite;
    u8 duration;
    u8 unk_02;
    u8 unk_03;
} PrizeMapCardBackAnimStep;

void LevelUpSplitDigits2(u16 a, u16* p);
void LevelUpSplitDigits3(u16 a, u16* p);

typedef struct CardSlot {
    u32 cardId;
    u16 index;
    u8 unk_06;
    u8 stocked;
    u8 used;
    u8 restoreOnReload;
    u8 removed;
    u8 unk_0B;
} CardSlot;

typedef char CardSlot_size[(sizeof(CardSlot) == 0xC) ? 1 : -1];

typedef struct CardDisplayArgs {
    ListPool* pool;
    CardSlot* slot;
    s32 variant;
    u16 index;
    u8 listIndex;
    u8 reloadCount;
} CardDisplayArgs;

typedef char CardDisplayArgs_size[(sizeof(CardDisplayArgs) == 0x10) ? 1 : -1];

enum CardDisplayFlag {
    CARD_DISP_FLAG_FACE_DOWN = 0x1,
    CARD_DISP_FLAG_NO_CARD = 0x2,
    CARD_DISP_FLAG_SELECTED = 0x4,
    CARD_DISP_FLAG_DOUBLE_SIZE = 0x8,
    CARD_DISP_FLAG_DEALING = 0x10,
    CARD_DISP_FLAG_OPEN = 0x20,
    CARD_DISP_FLAG_SETTLED = 0x40,
    CARD_DISP_FLAG_GFX_LOADED = 0x80,
    CARD_DISP_FLAG_STOCKED = 0x200,
    CARD_DISP_FLAG_VISIBLE = 0x800,
    CARD_DISP_FLAG_FROZEN = 0x1000,
    CARD_DISP_FLAG_IN_PLAY = 0x2000,
    CARD_DISP_FLAG_REMOVE = 0x4000,
    CARD_DISP_FLAG_UNOPPOSED = 0x8000,
    CARD_DISP_FLAG_RELOAD_CARD = 0x100000,
    CARD_DISP_FLAG_BROKEN = 0x200000,
    CARD_DISP_FLAG_SPIN_MIRRORED = 0x400000,
    CARD_DISP_FLAG_RELOAD_GAUGE = 0x1000000,
    CARD_DISP_FLAG_RELOAD_DONE = 0x4000000,
    CARD_DISP_FLAG_STOCK_NAMED = 0x10000000
};

typedef struct CardDisplayWork {
    void* tiles;
    void* tiles2;
    void* tiles3;
    void* tiles4;
    void* tiles5;
    void* palette;
    void* palette2;
    void* children;
    struct ReloadGauge* reloadGauge;
    TaskPool tasks;
    CardDisplayArgs args;
    const CardDef* cardDef;
    s32 x;
    s32 y;
    s32 scaleX;
    s32 scaleY;
    u16 enemyKind;
    u8 angle;
    u8 bobAngle;
    u8 unk_60[0x04];
    ListNode node;
    u32 flags;
    s32 ringAngle;
    s32 ringAngleTarget;
    s32 ringRadius;
    s32 ringRadiusTarget;
    s32 ringCenterX;
    s32 ringCenterY;
    s32 swingAngle;
    s32 swingAngleTarget;
    u16 timer;
    u8 spinSpeed;
    u8 stockIndex;
    u8 priority;
    u8 command;
    u8 phase;
    u8 swingSteps;
    s8 ringIndex;
    u8 value;
    u8 premium;
    u8 valueModified;
} CardDisplayWork;

typedef char CardDisplayWork_size[(sizeof(CardDisplayWork) == 0xA8) ? 1 : -1];

typedef struct CardBattleWork {
    TaskPool tasks;
    void* tiles;
    void* palette;
    CardDisplayWork* playedCards[3];
    CardDisplayWork* stock[3];
    CardDisplayWork* selectedCards[4];
    CardSlot* slots[4];
    ListPool cardDisplays[4];
    u16 cursors[4];
    s16 reloadCounts[4];
    s16 x;
    s16 timer;
    s16 slotCounts[4];
    s16 cardsLeft[4];
    s8 listIndex;
    u8 stockCount;
    u8 stockValue;
    u8 unk_BB;
    u8 revCountShown[4];
    u8 reloadPending[4];
    u8 unk_C4[5];
    u8 cardsClosed;
} CardBattleWork;

typedef char CardBattleWork_size[(sizeof(CardBattleWork) == 0xCC) ? 1 : -1];
extern u16 gUnk_096148D8[];
extern u8 gRiCardF0RedTiles[];

typedef struct CardListWork {
    ListPool cards;
    struct PremireChanceCardWork* selectedCard;
    TaskPool effectTasks;
    u8 effectCount;
    u8 unk_29;
    u8 unk_2A[2];
} CardListWork;

typedef char CardListWork_size[(sizeof(CardListWork) == 0x2C) ? 1 : -1];

extern u16 gUnk_09614118[];
extern u16 gUnk_096142F8[];
extern u8 gUnk_05000160[];
extern void* gLvupEffectSprites[];
extern u16 gUnk_09613F78[];

typedef struct EventMapObjectWork {
    u8 background;
    u8 unk_01[0x03];
    void* tiles[0x0A];
    ObjPalette* palettes[0x0A];
    u8 unk_54[0x04];
    struct EventMapObjectDef* definition;
} EventMapObjectWork;

struct MapCardDef;
struct MapCardBackDef;

typedef struct PrizeMapCardWork {
    void* tiles;
    ObjPalette* palette;
    void* tiles2;
    void* tiles3;
    ObjPalette* palette2;
    void* tiles4;
    void* tiles5;
    ObjPalette* palette3;
    struct MapCardDef* cardDef;
    struct MapCardBackDef* cardBack;
    TaskPool tasks;
    u16 kind;
    u16 value;
    s32 unk_40;
    Collider collider;
    s32 posX;
    s32 posY;
    s32 posZ;
    s32 groundZ;
    s32 cardId;
    s32 vz;
    s32 speed;
    s32 dirX;
    s32 dirY;
    s32 distance;
    s16 scaleX;
    s16 scaleY;
    u16 priority;
    s16 x;
    s16 y2;
    s16 x3;
    s16 y3;
    s16 x2;
    s16 y;
    s16 scale;
    u16 moveAngle;
    u8 rotation;
    u8 flipAngleY;
    u8 flipAngleX;
    u8 timer;
    u8 steps;
    u8 holdTimer;
    u8 unk_E4;
    u8 collected;
    u8 backAnimTimer;
    u8 backAnimStep;
    u8 backFrame;
} PrizeMapCardWork;

typedef char PrizeMapCardWork_sizechk[(sizeof(struct PrizeMapCardWork) == 0xEC) ? 1 : -1];

typedef struct PickupCardWork {
    void* tiles;
    void* palette;
    void* tiles2;
    void* palette2;
    void* tiles3;
    void* tiles4;
    void* palette3;
    const CardDef* cardDef;
    TaskPool tasks;
    u8 unk_34[0x04];
    s32 posX;
    s32 posY;
    s32 posZ;
    s32 floor;
    u8 unk_48[0xFC];
    Collider collider;
    s32 cardId;
    s32 vz;
    s32 speed;
    s32 distance;
    s32 dirX;
    s32 dirY;
    s16 scaleX;
    s16 scaleY;
    s16 scale;
    s16 x;
    s16 y;
    u16 priority;
    u16 timer;
    u8 moveAngle;
    u8 flipAngleX;
    u8 flipAngleY;
    u8 angle;
    u8 screenSpace;
    u8 unk_1CB;
    u8 unk_1CC;
    u8 visible;
    u8 backCategory;
    u8 unk_1CF;
    u16 spriteFlags;
} PickupCardWork;

typedef char PickupCardWork_sizechk[(sizeof(struct PickupCardWork) == 0x1D4) ? 1 : -1];

typedef char CardStat_sizechk[(sizeof(struct CardStat) == 0x18) ? 1 : -1];

typedef struct UnkStruct_080993D4 {
    u8 unk_000[0xE4];
    s16 unk_0E4;
} UnkStruct_080993D4;

typedef struct RevCountArgs {
    u8* shownList;
    s16* count;
    u8* visible;
    u8 list;
    u8 side;
    u8 unk_0E[0x02];
} RevCountArgs;

typedef struct RevCountWork {
    void* tiles;
    void* palette;
    void* gfx;
    AnimState anim;
    u8 list;
    u8 unk_25;
    u16 shownCount;
    u8 steps;
    u8 unk_29[0x03];
    RevCountArgs args;
    s32 x;
    s32 y;
} RevCountWork;

typedef char UnkStruct_08098CE4_sizechk[(sizeof(struct RevCountWork) == 0x44) ? 1 : -1];

extern const s32 gLvupEffectStartOffsetX[];
extern const s32 gLvupEffectStartOffsetY[];
extern const u16 gLvupEffectStartAngles[];
extern u16 gRandomHcEffects[47];
extern u16 gUnk_09619178[];

typedef struct WorldSelAnim {
    u8 palette;
    u8 duration;
    u8 unk_02[0x2];
} WorldSelAnim;

extern WorldSelAnim gWorldSelAnims[30];
extern u16 gUnk_09619378[];
extern const s32 gSysmsgwinChoiceCursorX[];
extern u8 gUnk_0815C1C2[];
extern u16 gUnk_09614418[];
extern u16 gUnk_09614438[];
extern u8 gFEventTiles[];
extern u16 gUnk_08F69BE4[];

typedef struct CardMessageArgs {
    u32 bg;
    u32 messageId : 16;
    u32 unk_06 : 8;
    u32 mode : 8;
} CardMessageArgs;

typedef struct DeckCard2Args {
    void* pool;
    u16 cardId;
    s16 col;
    s16 row;
    u8 panel;
    u8 unk_0B;
    u16* slot;
} DeckCard2Args;

enum DeckCard2Flag {
    DECK_CARD2_FLAG_GFX_LOADED = 0x1
};

typedef struct DeckCard2Work {
    u8 unk_00[0x04];
    ObjPalette* palette2;
    ObjTiles* tiles;
    ObjPalette* palette;
    ObjTiles* tiles2;
    const CardDef* cardDef;
    const CardBack* cardBack;
    DeckCard2Args args;
    ListNode node;
    s32 x;
    s32 y;
    u16 flags;
    u8 done;
    u8 unk_4B[0x02];
    u8 premium;
} DeckCard2Work;

typedef struct PremireChanceCardWork {
    const CardDef* cardDef;
    const CardBack* cardBack;
    void* tiles;
    void* tiles2;
    void* tiles3;
    void* unk_14;
    void* tiles4;
    void* tiles5;
    ObjPalette* palette;
    ObjPalette* palette2;
    ObjPalette* palette3;
    AnimState anim;
    void* gfx;
    u16 deckIndex;
    s16 x;
    s16 y;
    s16 angle;
    s16 radius;
    s8 position;
    u8 steps;
    u8 gfxLoaded;
    u8 state;
    u8 unk_56[0x02];
    ListNode node;
    s16 scaleX;
    s16 scaleY;
    u16 x2;
    u16 y2;
    u8 premium;
} PremireChanceCardWork;

typedef struct DeckExchangeWork {
    ObjTiles* tiles;
    ObjTiles* tiles2;
    ObjTiles* tiles3;
    ObjPalette* palette;
    ObjTiles* tiles4;
    ObjTiles* tiles5;
    ObjTiles* tiles6;
    ObjTiles* tiles7;
    ObjPalette* palette2;
    ObjPalette* palette3;
    TextSlot textSlots[8];
    TextSlot textSlots2[8];
    TextSlot textSlots3[8];
    TextSlot textSlots4[30];
    TextSlot textSlots5[90];
    ObjTiles* tiles8;
    ObjPalette* palette5;
    ObjTiles* tiles9;
    ObjPalette* palette6;
    ObjPalette* palette7;
    ObjPalette* palette4;
    void* unk_4C0;
    void* unk_4C4;
    u8 unk_4C8[4];
    struct CardKindEntry* entries;
    struct CardKindEntry* kindEntries;
    void* gfx3;
    void* gfx4;
    void* gfx5;
    u8 unk_4E0[8];
    void* gfx;
    void* gfx2;
    u8 unk_4F0[4];
    u8 unk_4F4[0x120];
    TaskPool tasks;
    TaskPool tasks2;
    ListPool pool;
    AnimState anim;
    AnimState anim2;
    u8 unk_67C[0x18];
    s32 x2;
    s32 y2;
    s32 x;
    s32 y;
    s32 unk_6A4;
    s32 unk_6A8;
    s32 unk_6AC;
    s32 unk_6B0;
    s32 unk_6B4;
    s32 x3;
    s32 y3;
    u8 unk_6C0[2];
    u16 heldRow;
    u16 unk_6C4;
    u16 unk_6C6;
    u16 unk_6C8;
    u16 unk_6CA;
    u16 entryIndex;
    u16 handFlags;
    s16 cursorCol;
    s16 cursorRow;
    s16 x4;
    s16 x5;
    s16 x6;
    s16 y4;
    s16 y5;
    s16 y6;
    u16 entryCount;
    u16 collectionCategoryCounts[4];
    u8 unk_6EA[2];
    s16 scrollRowEnd;
    s16 rowCount;
    u8 view;
    u8 unk_6F1;
    u8 unk_6F2;
    u8 unk_6F3;
    u8 savedCol;
    u8 savedRow;
    u8 timer;
    u8 deckAttackCount;
    u8 deckMagicCount;
    u8 deckItemCount;
    u8 deckEnemyCount;
    u8 unk_6FB;
    u8* resultOut;
    u8 deckIndex;
    u8 categoryFilter;
    u8 textSlotCount;
    u8 textSlotCount2;
    u8 textSlotCount3;
    u8 textSlotCount4;
    u8 mode;
    u8 unk_707;
    s16 x7;
    s16 y7;
    u8 textSlotCount5;
    u8 popupActive;
    u8 unk_70E;
    u8 unk_70F;
    u8 exitRequested;
    u8 unk_711;
    u8 unk_712;
    u8 unk_713;
    u8 holding;
    u8 step;
    u16 gridEntryCount;
} DeckExchangeWork;

typedef char DeckExchangeWork_size[(sizeof(DeckExchangeWork) == 0x718) ? 1 : -1];

typedef struct CardKindEntry {
    u16 valueCounts[0x0A];
    u16 kind;
    u16 count;
    u16 indexCount;
    u8 unk_1A[0x02];
    u16* indices;
} CardKindEntry;

typedef struct DeckMenuWork {
    ObjTiles* tiles;
    ObjTiles* tiles2;
    ObjTiles* tiles3;
    ObjPalette* palette2;
    ObjTiles* tiles4;
    ObjPalette* palette;
    ObjTiles* tiles12;
    ObjTiles* tiles7;
    ObjTiles* tiles8;
    ObjTiles* tiles9;
    ObjTiles* tiles10;
    void* unk_02C;
    ObjPalette* palette5;
    ObjPalette* palette6;
    TextSlot textSlots[8];
    TextSlot textSlots2[8];
    TextSlot textSlots3[8];
    TextSlot textSlots4[30];
    TextSlot textSlots5[90];
    ObjTiles* tiles5;
    ObjTiles* tiles6;
    ObjPalette* palette3;
    ObjPalette* palette4;
    DeckCard2Work* cursorCard;
    DeckCard2Work* prevCursorCard;
    void* unk_4D0;
    CardKindEntry* entries;
    CardKindEntry* kindEntries;
    void* gfx4;
    void* gfx5;
    void* gfx6;
    void* gfx7;
    void* gfx8;
    void* gfx;
    void* gfx2;
    void* gfx3;
    u8 unk_4FC[0x23C];
    ObjTiles* tiles11;
    ObjTiles* tiles13;
    ObjPalette* palette7;
    TextSlot textSlots6[8];
    u8 nameBuffer[20];
    AnimState anim4;
    void* gfx9;
    s32 x9;
    s32 y8;
    s32 x10;
    union {
        struct {
            s16 x;
            s16 y;
        } parts;
        u32 packed;
    } cursor;
    u8 textSlotCount6;
    u8 unk_7C5;
    u8 keyCursorSteps;
    u8 keyboardPage;
#ifdef VERSION_EU
    u8 onEndKey;
    u8 unk_7C9[3];
#endif
    TaskPool taskpool;
    TaskPool cardpool;
    ListPool pool;
    AnimState anim2;
    AnimState anim3;
    AnimState anim;
    s32 x;
    s32 y;
    s32 x2;
    s32 y2;
    s32 x5;
    s32 x6;
    s32 y5;
    s32 y6;
    s32 x7;
    s32 x3;
    s32 y3;
    s16 heldCol;
    s16 heldRow;
    s16 removeLabelX;
    s16 removeLabelY;
    s16 addLabelX;
    s16 addLabelY;
    u16 entryIndex;
    u16 handFlags;
    s16 cursorCol;
    s16 cursorRow;
    s16 deckNameX;
    s16 deckName2X;
    s16 deckName3X;
    s16 deckNameY;
    s16 deckName2Y;
    s16 deckName3Y;
    s16 descriptionX;
    s16 descriptionY;
    u16 entryCount;
    u16 collectionCategoryCounts[4];
    u16 deckAttackCount;
    u16 deckMagicCount;
    u16 deckItemCount;
    u16 deckEnemyCount;
    u8 unk_8AA[2];
    s16 scrollRowEnd;
    s16 rowCount;
    u8 handVisible;
    u8 view;
    u8 prevView;
    u8 prevCursor[2];
    s8 savedCol;
    s8 savedRow;
    u8 timer;
    u8 unk_8B8[4];
    u8* resultOut;
    u8 deckIndex;
    u8 categoryFilter;
    u8 textSlotCount;
    u8 textSlotCount2;
    u8 textSlotCount3;
    u8 textSlotCount4;
    u8 textSlotCount5;
    u8 mode;
    u8 commandCursor;
    u8 popupActive;
    u8 unk_8CA;
    u8 exitRequested;
    u8 barSlideTimer;
    u8 bannerSlideTimer;
    s8 inputDelay;
    u8 holding;
    u8 step;
    u8 promptChoice;
    u8 result;
    u8 unk_8D3;
    u16 gridEntryCount;
} DeckMenuWork;

typedef struct RikuDeckMenuWork {
    ObjTiles* tiles;
    ObjTiles* tiles2;
    ObjTiles* tiles3;
    ObjPalette* palette2;
    ObjTiles* tiles4;
    ObjPalette* palette;
    ObjTiles* tiles7;
    ObjTiles* tiles8;
    ObjTiles* tiles9;
    ObjTiles* tiles10;
    ObjPalette* palette5;
    ObjPalette* palette6;
    TextSlot textSlots[8];
    TextSlot textSlots2[8];
    TextSlot textSlots3[8];
    TextSlot textSlots4[30];
    TextSlot textSlots5[60];
    u8 unk_3C0[4];
    ObjTiles* tiles6;
    ObjPalette* palette3;
    ObjTiles* tiles12;
    u8 unk_3D0[8];
    ObjPalette* palette4;
    DeckCard2Work* cursorCard;
    DeckCard2Work* prevCursorCard;
    u8 unk_3E4[4];
    CardKindEntry* entries;
    void* gfx4;
    void* gfx5;
    void* gfx6;
    u8 unk_3F8[8];
    void* gfx;
    void* gfx2;
    void* gfx3;
    TaskPool taskpool;
    TaskPool cardpool;
    ListPool pool;
    AnimState anim2;
    AnimState anim3;
    AnimState anim;
    s32 x;
    s32 y;
    s32 x2;
    s32 y2;
    s32 x5;
    s32 x6;
    s32 y5;
    s32 y6;
    s32 x7;
    u8 unk_4B0[4];
    s32 y3;
    u8 unk_4B8[2];
    s16 heldRow;
    u16 unk_4BC;
    u16 unk_4BE;
    u16 unk_4C0;
    u16 unk_4C2;
    u8 unk_4C4[2];
    u16 handFlags;
    s16 cursorCol;
    s16 cursorRow;
    u8 unk_4CC[0xC];
    s16 descriptionX;
    s16 descriptionY;
    u16 entryCount;
    u8 unk_4DE[8];
    u8 view;
    u8 unk_4E7;
    u8 prevCursorCol;
    u8 prevCursorRow;
    u8 unk_4EA[2];
    u8 timer;
    u8 unk_4ED;
    u8 scrollRowEnd;
    u8 deckAttackCount;
    u8 deckMagicCount;
    u8 deckItemCount;
    u8 deckEnemyCount;
    u8 unk_4F3;
    u8* resultOut;
    u8 deckIndex;
    u8 unk_4F9;
    u8 textSlotCount;
    u8 textSlotCount2;
    u8 textSlotCount3;
    u8 textSlotCount4;
    u8 textSlotCount5;
    u8 unk_4FF;
    u8 unk_500;
    u8 popupActive;
    u8 unk_502;
    u8 unk_503;
    u8 exitRequested;
    u8 barSlideTimer;
    u8 bannerSlideTimer;
    s8 inputDelay;
    u8 holding;
    u8 step;
    u8 handVisible;
    u8 previewShown;
    u8 result;
} RikuDeckMenuWork;

typedef struct HcEffectNameWork {
    s16 x;
    u8 unk_02[0x06];
    void* tiles2;
    void* tiles3;
    void* palette;
    void* tiles;
    u8 unk_18;
    u8 side;
    u16 timer;
    u16 blinkInterval;
    u16 effect;
    u16 randomIndex;
    u8 countThousands;
    u8 countHundreds;
    u8 countTens;
    u8 countOnes;
    u8 countUnit;
    u8 visible;
} HcEffectNameWork;

struct PremireChanceWork;

typedef struct StockNameWork {
    u8 unk_00[4];
    u16 unk_04;
    u8 unk_06[2];
    ObjTiles* tiles;
    ObjPalette* palette;
    u8 unk_10;
    u8 stockNameIndex;
    u8 unk_12[2];
    u32 stockName;
    s32 stockNames[6];
    u8 cycling;
    u8 visible;
    u8 unk_32[2];
} StockNameWork;

enum ReloadChildFlag {
    RELOAD_CHILD_FLAG_SHIFTED = 0x1,
    RELOAD_CHILD_FLAG_IDLE = 0x2
};

typedef struct ReloadChildArgs {
    ListPool* pool;
    s32* parentX;
    s32* parentY;
    u8 index;
    u8 listIndex;
    u8 side;
    u8 unk_0F;
    u16 flags;
    u8 unk_12[0x02];
} ReloadChildArgs;

typedef struct ReloadChildWork {
    void* tiles;
    void* palette;
    void* tiles2;
    ReloadChildArgs args;
    s32 offsetX;
    s32 offsetY;
    s32 scale;
    u8 unk_2C[0x04];
    ListNode node;
    u8 steps;
    u8 angle;
    u8 retractTimer;
} ReloadChildWork;

typedef char UnkStruct_08098BE8_sizechk[(sizeof(struct ReloadChildWork) == 0x48) ? 1 : -1];

typedef struct PremiumCardEffectWork {
    void* tiles;
    void* palette;
    void* gfx;
    AnimState anim;
    s32 centerX;
    s32 centerY;
    s32 fallY;
    s32 x;
    s32 y;
    s32 unk_38;
    s32 angle;
    s32 radius;
    s32 vx;
    s32 vy;
    s32 speed;
    s32 fallSpeed;
} PremiumCardEffectWork;

typedef char PremiumCardEffectWork_sizechk[(sizeof(struct PremiumCardEffectWork) == 0x54) ? 1 : -1];

typedef struct CardNameWork {
    void* tiles;
    ObjPalette* palette2;
    TextSlot textSlots[32];
    TextSlot textSlots2[32];
#ifdef VERSION_EU
    TextSlot textSlots3[32];
#else
    TextSlot textSlots3[2];
#endif
    ObjPalette* textPalette;
    ObjPalette* palette;
    s16 nameX;
    s16 messageX;
    s16 suffixX;
    u8 textSlotCount;
    u8 textSlotCount2;
    u8 textSlotCount3;
} CardNameWork;

typedef struct PrintWork {
    u32 unk_00;
} PrintWork;

#ifdef VERSION_EU
typedef char CardNameWork_sizechk[(sizeof(struct CardNameWork) == 0x31C) ? 1 : -1];
#else
typedef char CardNameWork_sizechk[(sizeof(struct CardNameWork) == 0x22C) ? 1 : -1];
#endif

typedef struct DarkPointWork {
    ObjTiles* tiles;
    s32 x;
    u8 unk_08[0x02];
    s8 slideTimer;
    u8 thousands;
    u8 hundreds;
    u8 tens;
    u8 ones;
} DarkPointWork;

typedef char DarkPointWork_sizechk[(sizeof(struct DarkPointWork) == 0x10) ? 1 : -1];

typedef struct MapcardArgs {
    u8 baseCardId;
    u8 index;
    u8 kindCount;
    u8 count;
    struct MapSelectWork* parent;
    ListPool* pool;
    u8 unk_0C[0x0C];
} MapcardArgs;

typedef char MapcardArgs_size[(sizeof(MapcardArgs) == 0x18) ? 1 : -1];

enum MapcardFlag {
    MAPCARD_FLAG_GFX_LOADED = 0x1,
    MAPCARD_FLAG_RAISED = 0x2,
    MAPCARD_FLAG_CHOSEN = 0x40,
    MAPCARD_FLAG_DELIVERED = 0x80,
    MAPCARD_FLAG_CURSOR = 0x100,
    MAPCARD_FLAG_OPENED = 0x200,
    MAPCARD_FLAG_REMOVED = 0x400
};

typedef struct MapcardWork {
    void* tiles;
    void* unk_04;
    void* tiles2;
    ObjPalette* palette;
    void* tiles3;
    ObjPalette* palette2;
    MapCardDef* cardDef;
    MapCardBackDef* cardBack;
    MapcardArgs args;
    ListNode node;
    s32 x;
    s32 y;
    s32 dirX;
    s32 dirY;
    s32 deceleration;
    s32 speed;
    s32 distance;
    u16 scale;
    u16 priority;
    u16 flags;
    u8 angle;
    u8 steps;
    u8 holdTimer;
    u8 unk_71;
    u8 unk_72;
    u8 unk_73;
    u8 value;
    u8 unk_75[0x03];
} MapcardWork;

typedef char MapcardWork_size[(sizeof(MapcardWork) == 0x78) ? 1 : -1];

typedef struct ReloadGauge {
    s16 unk_00;
    u16 angle;
    s32 offsetX;
    u8 unk_08[0x05];
    u8 gaugeAnim;
    u8 unk_0E[0x02];
    AnimState anim;
    AnimState anim2;
    AnimState anim3;
    void* gfx;
    void* gfx2;
    void* gfx3;
    s8 reloadCounter;
    u8 chargeTick;
    u8 unk_66[0x02];
} ReloadGauge;

typedef char ReloadGauge_size[(sizeof(ReloadGauge) == 0x68) ? 1 : -1];

typedef struct PrintLine {
    u8 length;
    u8 x;
    u8 y;
    u8 palette;
    u16 tilemap[32];
} PrintLine;

typedef struct NumberPlusArgs {
    s32 unk_00;
    s32 x;
    s32 y;
    s32 unk_0C;
    s32 unk_10;
    s32 unk_14;
    s32 unk_18;
} NumberPlusArgs;

typedef struct NumberPlusWork {
    void* tiles;
    void* palette;
    NumberPlusArgs args;
    s16 x;
    s16 y;
    u8 steps;
    u8 unk_29;
} NumberPlusWork;

typedef struct MapTileAnimationWork {
    u8 firstTrack;
    u8 frameTimers[8];
    u8 frameIndices[8];
    u8 unk_11[3];
    const MapTileAnimationDef* definition;
} MapTileAnimationWork;

typedef struct PrizeCardArgs {
    s32 unk_00[8];
} PrizeCardArgs;

typedef struct PrizeCardTaskArgs {
    s32 x;
    s32 y;
    s32 z;
    u8 unk_0C[0x14];
    s32 cardId;
} PrizeCardTaskArgs;

typedef struct PrizeCardInitWork {
    TaskPool tasks;
    u8 spawned;
    PrizeCardArgs args;
} PrizeCardInitWork;

typedef struct ScrollBarWork {
    u8 unk_00[0x08];
    u16 unk_08;
    u16 unk_0A;
    u16 unk_0C;
    u16 unk_0E;
    u16 position;
    u16 remaining;
    u16 count;
    u8 active;
    u8 unk_17;
} ScrollBarWork;

typedef char ScrollBarWork_sizechk[(sizeof(struct ScrollBarWork) == 0x18) ? 1 : -1];

typedef struct PrizeCardWork {
    ObjTiles* tiles;
    ObjPalette* palette;
    ObjTiles* tiles2;
    ObjTiles* tiles3;
    ObjPalette* palette2;
    ObjTiles* tiles4;
    ObjTiles* tiles5;
    ObjPalette* palette3;
    TaskPool tasks;
    CardStat stat;
    Collider collider;
    FldPos pos;
    FldPos prevPos;
    u32 cardId;
    s32 vz;
    s32 speed;
    s32 dirX;
    s32 dirY;
    s32 distance;
    s16 scaleX;
    s16 scaleY;
    s16 priority;
    s16 x;
    s16 y2;
    s16 targetX;
    s16 targetY;
    s16 x2;
    s16 y;
    s16 scale;
    s16 moveAngle;
    u8 rotation;
    u8 flipAngleY;
    u8 flipAngleX;
    u8 timer;
    u8 steps;
    u8 holdTimer;
    u8 collected[0x04];
} PrizeCardWork;

typedef struct PrizeMapCardEntry {
    u16 cardId;
    u16 unk_02;
} PrizeMapCardEntry;

typedef struct PrizeMapCardGroup {
    const PrizeMapCardEntry* entries;
    u16 count;
    u16 chance;
} PrizeMapCardGroup;

typedef struct PrizeMapCardGroupList {
    const PrizeMapCardGroup* data;
    u16 size;
    u16 unk_06;
} PrizeMapCardGroupList;

extern Deck gUnk_09041FA0;
extern u16 gUnk_09041F70[];
extern u8 gRikuBt00Tiles[];
extern u8 gSor1ll51Tiles[];
extern u16 gUnk_09618D38[];
extern const s32 gSoraCardLayout[][2];
extern const s32 gSoraCardRingAngles[];
extern const s32 gSoraCardSwingAngles[];
extern const s32 gRikuCardLayout[][2];
extern const s32 gPlayedCardCenter[];
extern const s32 gPlayedCardAngles[];
extern u16 gUnk_096144D8[];
extern u16 gUnk_096FBA04[];
extern u8 gUnk_09628DC0[];

#ifdef VERSION_EU
extern u8 gUnkEu_095EDAAA[];
extern u8 gUnkEu_095EE0E2[];
extern u8 gUnkEu_095EE71A[];
extern u8 gUnkEu_095EED52[];
extern u8 gUnkEu_095EF38A[];
extern u8 gUnkEu_095EF9C2[];
extern u8 gUnkEu_095EFFFA[];
extern u8 gUnkEu_095F0632[];
extern u8 gUnkEu_095F0C6A[];
extern u8 gUnkEu_095F12A2[];
#endif

extern const s16 gDeckGridColumnX[];
extern const s16 gDeckGridRowY[];

typedef struct PromptChoiceLayout {
    s32 x[2];
} PromptChoiceLayout;

extern u8 gUnk_09618C58[];
extern u8 gUnk_09619098[];
extern u8 gUnk_090A3E46[];
extern u16 gUnk_096144F8[];
extern u16 gUnk_09619158[];
extern u16 gUnk_09614458[];
extern u16 gUnk_09614478[];
extern u16 gUnk_09614498[];
extern u16 gUnk_096144B8[];
extern u16 gUnk_09614406[];
extern u16 gCard00Palette[];
extern u8 gSor1ff00Tiles[];
extern u8 gRikuFf00Tiles[];
extern u16 gUnk_09614798[];
extern u16 gUnk_09618C38[];
extern u8 gUnk_050001A0[];
extern u8 gUnk_050001C0[];
extern u8 gUnk_0500016C[];
extern u8 gUnk_06010000[];

typedef struct MapSelectKindEntry {
    u16 baseCardId;
    u16 count;
} MapSelectKindEntry;

typedef struct SpotlightWork {
    u8 steps;
    u8 unk_01[0x03];
    s32 blendB;
    s32 blendA;
    u16 bldAlpha;
    u8 unk_0E[0x02];
    u8* endFlag;
    u8 ownEndFlag;
} SpotlightWork;

typedef struct DispCardnameWork {
    TextSlot textSlots[32];
    void* tiles;
    ObjPalette* textPalette;
    ObjPalette* palette;
    s16 x;
    u8 textSlotCount;
} DispCardnameWork;

typedef char DispCardnameWork_size[(sizeof(DispCardnameWork) == 0x110) ? 1 : -1];

typedef struct VersionWork {
    void* tiles;
    void* palette;
    u16 text[16];
    u8 textLength;
} VersionWork;

typedef char VersionWork_size[(sizeof(VersionWork) == 0x2C) ? 1 : -1];

typedef struct CardEffectArgs {
    s32 x;
    s32 y;
    s32 z;
    u8 screenSpace;
    u8* count;
} CardEffectArgs;

typedef struct CardEffectWork {
    void* tiles;
    void* palette;
    AnimState anim;
    void* gfx;
    s32 posX;
    s32 posY;
    s32 posZ;
    s16 x;
    s16 y;
    u16 priority;
    u8 unk_36[0x02];
    CardEffectArgs args;
} CardEffectWork;

typedef struct BossPrizeWork {
    ObjTiles* tiles;
    ObjPalette* palette;
    ObjTiles* tiles2;
    ObjTiles* tiles3;
    ObjPalette* palette2;
    ObjTiles* tiles4;
    ObjTiles* tiles5;
    ObjPalette* palette3;
    TaskPool tasks;
    CardStat stat;
    Collider collider;
    s32 posX;
    s32 posY;
    s32 posZ;
    s32 groundZ;
    s32 cardId;
    s32 vz;
    s32 speed;
    s32 dirX;
    s32 dirY;
    s32 distance;
    s16 scaleX;
    s16 scaleY;
    u16 priority;
    s16 x;
    s16 y;
    s16 x3;
    s16 y3;
    s16 x2;
    s16 y2;
    s16 scale;
    s16 unk_E4;
    u8 rotation;
    u8 unk_E7;
    u8 flipAngleY;
    u8 flipAngleX;
    u8 timer;
    u8 steps;
    u8 holdTimer;
    u8 collected;
    u8 effectCount;
    u8 effectTimer;
} BossPrizeWork;

typedef char BossPrizeWork_sizechk[(sizeof(struct BossPrizeWork) == 0xF0) ? 1 : -1];

typedef struct EventMapObjectPlacement {
    s32 x : 24;
    s32 unk_03 : 8;
    s32 y : 24;
    s32 unk_07 : 8;
    u8 spriteIndex;
    u8 unk_09[0x03];
} EventMapObjectPlacement;

typedef struct EventMapObjectDef {
    PrizeMapCardGroupList* tileResources;
    PrizeMapCardGroupList* paletteResources;
    void** sprites;
    EventMapObjectPlacement* placements;
    u16 placementCount;
} EventMapObjectDef;

typedef struct SelmapEventKeyArgs {
    ObjPalette* palette;
    void* unk_04;
    s32 closeMode;
} SelmapEventKeyArgs;

typedef struct MapSelectWork {
    TaskPool tasks;
    ListPool cards;
    u8 unk_024[8];
    ObjTiles* tiles4;
    ObjPalette* palette;
    void* tiles2;
    void* tiles3;
    void* tiles;
    ObjPalette* palette2;
    void* tiles5;
    ObjPalette* palette3;
    TextSlot textSlots[48];
    ObjPalette* palette4;
    u8 unk_1D0[0x10];
    void* tiles6;
    void* unk_1E4;
    void* tiles7;
    MapcardWork* card;
    MapcardWork* prevCard;
    MapcardWork* card2;
    AnimState anim;
    AnimState anim2;
    u8 unk_228[0x10];
    struct SelmapEventKeyWork* eventKey;
    s32 openedCardX;
    s32 bgScrollY;
    s32 nameY;
    s32 x3;
    s32 titleY;
    s32 x;
    s32 y;
    s32 x2;
    s32 y2;
    s32 unk_260;
    s32 cursorTargetX;
    s32 unk_268;
    s32 y3;
    s32 y4;
    void* gfx;
    void* gfx2;
    u16 kindCount;
    u16 requiredValue;
    s16 savedCursorX;
    s16 savedCursorY;
    u8 page;
    u8 lastPage;
    u8 messageTimer;
    u8 textSlotCounts[0x04];
    u8 steps;
    u8 steps2;
    u8 unk_28D[0x02];
    u8 slideSteps;
    u8 barSteps;
    u8 unk_291[0x03];
    u8* status;
    u8 pageScroll;
    u8 mosaicX;
    u8 mosaicY;
    u8 mosaicTimer;
    s8 valueColumn;
    s8 valueRow;
    u8 paletteBuffer[0x20];
    u8 isEventDoor;
    u8 cancelled;
    u8 scrollBarVisible;
    u8 inTutorial;
    u8 tutorialMessage;
    u8 unk_2C3;
    SelmapEventKeyArgs eventKeyArgs;
    u8 valueCounts[0x0A];
    u8 remainingKeys;
    u8 unk_2DB;
    void* unk_2DC;
    MapSelectKindEntry* kindEntries;
} MapSelectWork;

typedef char MapSelectWork_size[(sizeof(struct MapSelectWork) == 0x2E4) ? 1 : -1];

typedef struct LvupMsgWork {
    TextSlot textSlots[20];
    TextSlot textSlots2[20];
    TextSlot textSlots3[20];
#ifndef VERSION_JP
    TextSlot textSlots4[20];
#endif
    ObjPalette* textPalette;
    void* tiles;
    void* palette;
    u16 amount;
    u8 unk_28E[2];
    s32 y;
    s32 x;
    s32 x2;
    s32 x3;
    s32 y2;
    s32 y3;
    s32 y4;
    s8 slideSteps;
    u8 textSlotCount;
    u8 textSlotCount2;
    u8 textSlotCount3;
#ifndef VERSION_JP
    u8 textSlotCount4;
#endif
    u8 unk_2B1;
    u8* active;
} LvupMsgWork;

typedef struct DeckConfirmWork {
    TextSlot textSlots[80];
    TextSlot textSlots2[80];
    TextSlot textSlots3[80];
    ObjTiles* tiles;
    ObjPalette* palette;
    ObjPalette* palette2;
    u8 textSlotCount;
    u8 textSlotCount2;
    u8 textSlotCount3;
    u8 unk_78F;
    s16 unk_790;
    s16 x;
    s16 x2;
    s16 y;
    s16 y2;
    s16 x3;
    s16 y3;
    u8 unk_79E[2];
    u8* active;
    u8 unk_7A4;
} DeckConfirmWork;

struct BtlObj;

typedef struct LevelUpEffectArgs {
    s32 x;
    s32 y;
    u8 unk_08;
    u8 unk_09[3];
    struct BtlObj* target;
    void* tiles;
    ObjPalette* palette;
} LevelUpEffectArgs;

typedef struct LevelUpEffectWork {
    void* tiles;
    void* palette;
    void* unk_08;
    struct BtlObj* target;
    s32 centerX[4];
    s32 centerY[4];
    s32 radius;
    s32 x[4];
    s32 y[4];
    s32 unk_54[4];
    s32 targetX;
    s32 targetY;
    s32 vy[4];
    s32 speed[4];
    u16 angle[4];
    s8 frame;
    u8 timer;
    u8 gatherSteps;
    u8 unk_97;
    TaskPool tasks;
} LevelUpEffectWork;

typedef struct StockInfoWork {
    void* tiles;
    void* palette;
    s32 x;
    s32 y;
    s8 timer;
    u8 unk_11[3];
    u8* active;
    TaskPool tasks;
} StockInfoWork;

typedef struct UnkStruct_080ABA80 {
    s32 keys[6];
} UnkStruct_080ABA80;

extern const UnkStruct_080ABA80 gTutorialEmptyKeys;
extern const UnkStruct_080ABA80 gSoraEmptyKeys;

typedef struct GimmickCardArgs {
    s32 x;
    s32 y;
    s32 z;
    s32 cardId;
} GimmickCardArgs;

typedef struct WorldSelBeforeArgs {
    s32 x;
    s32 y;
    s32 z;
} WorldSelBeforeArgs;

typedef struct WorldSelBeforeWork {
    void* tiles;
    ObjPalette* palette;
    void* tiles2;
    ObjPalette* palette2;
    WorldSelBeforeArgs pos;
    u8 animStep;
    u8 animTimer;
    u8 unk_1E[0x02];
    s32 x2[10];
    s32 y2[10];
    s32 z2[10];
    u8 angle[10];
    u8 spriteCount;
    u8 risenCount;
    u8 unk_A4[0x14];
} WorldSelBeforeWork;

typedef struct EventBgEffectFrame {
    u16 duration;
    u16 tilesOffset;
} EventBgEffectFrame;

typedef struct EventBgEffectDef {
    void** maps;
    u8* tiles;
    void* palette;
    u16 tilesSize;
    u16 paletteSize;
    u8 unk_10[0x04];
    const EventBgEffectFrame* frames;
    u8 frameCount;
    s8 loopFrame;
} EventBgEffectDef;

typedef struct EventBgEffectWork {
    const EventBgEffectEntry* entries;
    u8 unk_04[0x08];
    u16 frameTimer;
    u16 frame;
    u8 unk_10[0x02];
    u8 effect;
    u8 eventId;
    u8 entry;
    u8 animating;
    u8 fadingIn;
} EventBgEffectWork;

extern const EventBgEffectDef* gEventBgEffectDefs[];

typedef struct ReloadArgs {
    u8 slot;
    u8 mode;
    u8 unk_02[0x02];
    u8* state;
} ReloadArgs;

typedef struct ReloadWork {
    void* tiles;
    void* palette;
    void* gfx;
    AnimState anim;
    s32 x;
    s32 y;
    ReloadArgs args;
    u8 steps;
} ReloadWork;

typedef struct SysMsgWinWork {
    ObjTiles* tiles3;
    ObjPalette* palette;
    ObjTiles* tiles4;
    ObjPalette* palette2;
    ObjTiles* tiles;
    ObjPalette* palette3;
    ObjTiles* tiles2;
    ObjPalette* palette4;
    TextSlot textSlots[10];
    TextSlot textSlots2[10];
    ObjPalette* textPalette;
    AnimState anim;
    AnimState anim2;
    AnimState anim3;
    CardMessageArgs args;
    CardMessageDef* messageDef;
    s32 x;
    s32 cursorY;
    s32 frameX;
    s32 frameY;
    void* gfx4;
    void* gfx;
    TextChar* nextText;
    u16 glyphPaletteIndex;
    s16 closeTimer;
    u8 unk_138[0x05];
    u8 choice;
    u8 cursorSteps;
    u8 textSlotCount;
    u8 textSlotCount2;
    u8 waitIconVisible;
    u8 unk_142;
    u8 unk_143;
    u8 choiceVisible;
    u8 messagePending;
    u8 unk_146[0x02];
} SysMsgWinWork;

typedef char SysMsgWinWork_size[(sizeof(SysMsgWinWork) == 0x148) ? 1 : -1];

typedef struct CardMsgWinWork {
    ObjTiles* tiles3;
    ObjPalette* palette;
    ObjTiles* tiles4;
    ObjPalette* palette2;
    ObjTiles* tiles;
    ObjPalette* palette3;
    ObjTiles* tiles2;
    ObjPalette* palette4;
    TextSlot textSlots[10];
    TextSlot textSlots2[10];
    ObjPalette* textPalette;
    AnimState anim;
    AnimState anim2;
    AnimState anim3;
    CardMessageArgs args;
    CardMessageDef* messageDef;
    s32 x;
    s32 faceX;
    s32 faceY;
    s32 cursorX;
    s32 cursorY;
    void* gfx;
    void* gfx2;
    void* gfx3;
    TextChar* nextText;
    u16 glyphPaletteIndex;
    s16 closeTimer;
    u8 steps;
    u8 shownChars;
    u8 charTimer;
    u8 charCount;
    u8 choice;
    u8 cursorSteps;
    u8 textSlotCounts[0x02];
    u8 waitIconVisible;
    u8 textVisible;
    u8 unk_14A[0x02];
    u8 unk_14C;
    u8 faceFlip;
    u8 messagePending;
    u8 keepOpen;
} CardMsgWinWork;

typedef char CardMsgWinWork_size[(sizeof(CardMsgWinWork) == 0x150) ? 1 : -1];

typedef struct BossCardWork {
    const CardDef* cardDef;
    const CardBack* cardBack;
    const s32* cardIds;
    u8 unk_0C[0x18];
    s32 enemyKind;
    s16 x;
    u16 y;
    s16 flipScale;
    u8 bobAngle;
    u8 unk_2F;
    u8 unk_30;
    u8 slideSteps;
    u8 flipShrinking;
    u8 flipTimer;
    u8 flipDelay;
} BossCardWork;

extern const s16 gCollectionGridColumnX[];
extern const s16 gCollectionGridRowY[];

s32 AppendKeyboardChar(DeckMenuWork* w);

void func_0807E230();
void RequestCycleRikuCardList();

Deck* GetActiveDeck();
Deck* GetDeck(u8 index);
u8 GetCollectionCardCategory(u16 index);
u16 GetCollectionCardKind(u16 index);
void SetActiveDeckIndex(u8 index);
u16 GetDeckCpCost(u8 index);

void CreateBosscardTask(TaskPool* pool);
void ListOwnedMapCardKinds(MapSelectKindEntry* p);
u8 CanUseSoraSelectedCard();
u8 CanUseRikuSelectedCard();
u16 CountAvailableCardSlots(CardBattleWork* w, u8 n);
u8 UpdatePremireChanceResult(struct PremireChanceWork* w, void* a);
u8 SoraCardUpdateLoaded(CardDisplayWork* p, void* a);
u8 UpdateDeckMenuOpenCommands(DeckMenuWork* w, void* a);
void LoadActiveDeckCardSlots(CardSlot* slots, s32 deckIndex);
u8 SoraCardClosed(CardDisplayWork* p, void* a);
u8 SoraReloadCardClosed(CardDisplayWork* p, void* a);
u8 UpdateMapcardToCenter(MapcardWork* w, void* a);
u8 SpotLight_1(SpotlightWork* w, void* a);
u8 UpdateRevCountListChanged(RevCountWork* w);
void REV_COUNT_0(RevCountWork* w, RevCountArgs* a);
u8 REV_COUNT_1(RevCountWork* w, void* a);
void REV_COUNT_2(RevCountWork* w);
void REV_COUNT_3(RevCountWork* w);
u8 UpdateRevCountEmpty(RevCountWork* w, void* a);
void StartPickupCardFlight(PickupCardWork* w, u8 kind);
void Friend_card_0(PickupCardWork* w, s32* args);
void Heartless_card_0(PickupCardWork* w, s32* args);
s32 Friend_card_1(PickupCardWork* w, void* a);
s32 Gimmick_card_1(PickupCardWork* w, void* a);
u8 FlyPickupCardToDeck(PickupCardWork* w);
s32 WaitHeartlessCardName(PickupCardWork* w, void* a);
s32 FlyHeartlessCardToCenter(PickupCardWork* w, void* a);
s32 Heartless_card_1(PickupCardWork* w, void* a);
void PickupCardDraw(PickupCardWork* w);
void Heartless_card_2(PickupCardWork* w);
void PickupCardDestroy(PickupCardWork* w);
void Heartless_card_3(PickupCardWork* w);
void DeckCard2LoadGfx(DeckCard2Work* n);
u8 Card_EFFECT_1(CardEffectWork* w);
void InitPrintLayer(u8 bg);
u8 FlipBossCard(BossCardWork* w, u8 b);
void AimPrizeMapCardAtCenter(PrizeMapCardWork* w);
void AimBossPrizeAtCenter(BossPrizeWork* w);
u8 StockInfo_1(StockInfoWork* w, void* a);
#ifdef PLATFORM_ANDROID
u8 func_08090A54(CardDisplayWork* p, void* a);
#else
void func_08090A54(CardDisplayWork* p, void* a);
#endif
u8 UpdateMapSelectValueTutorial(MapSelectWork* w, void* a);
void LoadRikuDeckNameTexts(RikuDeckMenuWork* w);
void LoadDeckExchangeDeckNameTexts(DeckExchangeWork* w);
void CopyLinkPartnerDeckCards(u8 kind, u16* out);
void SetDeckMenuFrameCursor(DeckMenuWork* w, u8 kind);
void UpdateFieldPrizeCardScale(PrizeCardWork* w);
void UpdatePrizeMapCardScale(PrizeMapCardWork* w);
void UpdateBossPrizeScale(BossPrizeWork* w);
void PrizeBoss_0(BossPrizeWork* w, PrizeCardTaskArgs* args);
u8 PrizeBoss_1(BossPrizeWork* w, void* a);
void PrizeBoss_2(BossPrizeWork* w);
void PrizeBoss_3(BossPrizeWork* w);
u8 PremireChanceCard_1(PremireChanceCardWork* w, void* a);
void SetDeckExchangeFrameCursor(DeckExchangeWork* w, u8 kind);
u8 UpdateDeckMenuCloseKeyboard(DeckMenuWork* w, void* a);
void OpenRikuCards(CardBattleWork* w);
void InitDecks();
#ifdef PLATFORM_ANDROID
u8 func_08090ACC(CardDisplayWork* p, void* a);
#else
void func_08090ACC(CardDisplayWork* p, void* a);
#endif
u8 UpdateMapcardMoveBack(MapcardWork* w, void* a);
void SpotLight_0(SpotlightWork* w, u8* src);
u8 UpdateRevCountHidden(RevCountWork* w, void* a);
u8 FlyHeartlessCardToPlayer(PickupCardWork* w);
u8 UpdatePremireChanceClose(struct PremireChanceWork* w, void* a);
void PrintBinary16(u16 a, u16 b, u16 c, u16 bits);
void LevelUpSplitDigits4(u16 n, u16* out);
u8 UpdateRikuDeckMenuEnterDeckGrid(RikuDeckMenuWork* w, void* a);
u8 UpdatePrizeMapCardShrink(PrizeMapCardWork* w);
u8 UpdatePremireChanceCardMoveAway(PremireChanceCardWork* w, void* a);
u8 UpdateRikuDeckMenuSlideOut(RikuDeckMenuWork* w, void* a);
void RecalculateInactiveDeckCpCosts();
u8 RELOAD_1(ReloadWork* w, void* a);
u8 UpdateBossPrizeShrink(BossPrizeWork* w);
void LoadEventMapObjectGfx(EventMapObjectWork* w, EventBackgroundDef* t);
void Mode_riku_deckTutorial_1();
void OpenSoraCards(CardBattleWork* w);
u8 UpdateSoraAutoCycle(CardBattleWork* w, void* a);
void CloseRikuCards(CardBattleWork* w);
u8 RikuCardDeal(CardDisplayWork* p, void* a);
void card_enemy_0(CardDisplayWork* p, CardDisplayArgs* a);
void card_not_have_2(CardDisplayWork* p);
u8 Premire_Chance_1(struct PremireChanceWork* w, void* a);
void PrintHex32(u16 a, u16 b, u16 c, u32 v);
void ScrollRikuGridDown(RikuDeckMenuWork* w);
void ScrollDeckExchangeGridDown(DeckExchangeWork* w);
void SelectOtherSoraCard(CardBattleWork* w, u8 kind, u8 c);
u8 SoraCardDeal(CardDisplayWork* p, void* a);
u8 card_enemy_1(CardDisplayWork* p, void* a);
void SetRikuDeckMenuHandAnim(RikuDeckMenuWork* w);
void DrawRikuDeckCategoryCount(u8 a, u8 b);
void SetDeckExchangeHandAnim(DeckExchangeWork* w);
u8 SoraHeartlessCardShow(CardDisplayWork* p);
u8 RikuHeartlessCardShow(CardDisplayWork* p);
void SetDeckMenuHandAnim(DeckMenuWork* w);
void Mode_Premire_0();
void LoadEventBgEffect(EventBgEffectWork* w);
u8 StepEventBgEffectAnim(EventBgEffectWork* w);
u8 UpdateRikuDeckMenuLoadBgs(RikuDeckMenuWork* w, void* a);
void UpdateRikuCardValue(CardDisplayWork* p);
u8 map_anim_1(MapTileAnimationWork* w);
u8 Mapcard_1(MapcardWork* w, void* a);
void RELOAD_0(ReloadWork* w, ReloadArgs* a);
void Premire_Chance_3(struct PremireChanceWork* w);
void NO_Card_2(CardDisplayWork* p);
void ClearDeck(u8 deck);
void CloseSoraCards(CardBattleWork* w);
u8 AddBreakDarkPoints();
u8 SoraCardFlyOff(CardDisplayWork* p);
u8 RikuCardFlyOff(CardDisplayWork* p);
void RemoveCardFromDeck(u16* p, u8 deck);
void DrawCollectionFilterTab(u8 kind, u8 slot);
u8 EnemyCardFlyOff(CardDisplayWork* p);
u8 ScrollRikuGridUp(RikuDeckMenuWork* w);
u8 ScrollDeckExchangeGridUp(DeckExchangeWork* w);
void DrawDeckExchangeCollectionFilterTab(u8 kind, u8 slot);
void Ev_mapObj_2(EventMapObjectWork* w);
void UpdateEventKeyTotal(EventKeyCard* w);
u8 card_reload_1(CardDisplayWork* p, void* a);
void ResetGridScroll(DeckMenuWork* w);
u8 UpdateMapcardMoveToFront(MapcardWork* w, void* a);
u8 UpdateRikuDeckMenuStartSlideOut(RikuDeckMenuWork* w, void* a);
s16 CountDeckCards(s32 mode, const Deck* d);
u8 UpdateCardMsgwinClose(CardMsgWinWork* w);
u8 UpdateReloadChildRetracted(ReloadChildWork* w, void* a);
void DeckCard2_2(DeckCard2Work* n);
void ApplyRikuHcEffect(CardBattleWork* w);
void UpdateMapcardGfx(MapcardWork* w);
void DrawDeckCardCount(u8 deck);
void DrawDeckExchangeDeckCardCount(u8 deck);
void DrawRikuDeckCardCount(u8 deck);
u8 StockNameSora_1(StockNameWork* w);
u8 StockNameRiku_1(StockNameWork* w);
u8 DeckCard2_1(DeckCard2Work* n);
void ResetCardSlotsForReload(CardBattleWork* w, u8 n);
u8 RikuCardWaitPlayEnd(CardDisplayWork* p, void* a);
void Bosscard_0(BossCardWork* w, u32* a);
void DrawLayeredCardSpriteScaled(LayeredCardSprite* w, u16 b, s16 c, s16 d);
void RemoveCardFromActiveDeck(u16 slot);
u8 SoraCardWaitPlayEnd(CardDisplayWork* p, void* a);
u8 FindCardInDirection(DeckMenuWork* w, s16 x, s16 y, u16 dir);
void Mapcard_0(MapcardWork* w, MapcardArgs* a);
u8 AddCardToDeck(u16 card, u8 deck);
void CountCardsNotInDeckByKind(CardKindEntry* out, u8 deck, u8 mode, u16 n, void* p);
void FillDebugCardCollection();
u16 CountOwnedMapCardKinds();
void LVUP_EFFECT_0(LevelUpEffectWork* w, LevelUpEffectArgs* a);
void Deck_Clear_0(DeckConfirmWork* w, u8* a);
void Deck_Yes_No_0(DeckConfirmWork* w, u8* a);
void LVUP_EFFECT_2(LevelUpEffectWork* w);
u8 UpdatePremireChanceCardToCenter(PremireChanceCardWork* w, void* a);
void Lvup_Logo_0(LevelUpEffectWork* w, LevelUpEffectArgs* a);
u8 UpdateCardMsgwinChoice(CardMsgWinWork* w, void* a);
u8 UpdateRikuDeckMenuSlideIn(RikuDeckMenuWork* w, void* a);
void sysmsgwin_3(SysMsgWinWork* w);
void sysmsgwinChoice_3(SysMsgWinWork* w);
s32 ReplaceSysmsgwinMessage(CardMessageArgs* src);
s32 CloseSysmsgwin();
void UpdateEnemyCardRingPosition(CardDisplayWork* p);
void SpawnBossPrizeCardEffects(BossPrizeWork* w);
void Premire_Chance_2(struct PremireChanceWork* w);
void PremireEffectInit(PremiumCardEffectWork* w, s16* a);
void PremireEffectConvergeInit(PremiumCardEffectWork* w, s16* a);
void RELOAD_CHILDREN_2(ReloadChildWork* w);
u8 FindDeckExchangeCardInDirection(DeckExchangeWork* w, s16 x, s16 y, u16 dir);
u8 EnemyCardWaitPlayEnd(CardDisplayWork* p, void* a);
u8 AddCardToActiveDeck(u16 card);
void SetRikuCardKindObtained(u16 a);
void CopyActiveDeckCards(s32 a, u16* out);
void DeckConfirmDraw(DeckConfirmWork* w);
void DeckConfirmDestroy(DeckConfirmWork* w);
u8 ScrollGridUp(DeckMenuWork* w, u8 a);
void DeckMenuDestroy(DeckMenuWork* w);
void HCEffectName_2(HcEffectNameWork* w);
u8 UpdateCardMsgwinOpen(CardMsgWinWork* w, void* a);
void DispatchEnemyCardCommand(CardDisplayWork* p, void* a);
void RELOAD_CHILDREN_0(ReloadChildWork* w, ReloadChildArgs* a);
u8 Bosscard_1(BossCardWork* w, void* a);
void RemoveMapSelectCard(MapSelectWork* w);
u8 UpdateBossPrizeShow(BossPrizeWork* w, void* a);
void LoadRikuReloadCardGfx(CardDisplayWork* w);
void InitRikuReloadCounterAnim(ReloadGauge* p, void* a, u8 b, s8 c);
u8 RikuCardClosed(CardDisplayWork* p, void* a);
void RemoveCursorCardFromDeck(DeckMenuWork* w);
u8 UpdateLevelUpEffectScatter(LevelUpEffectWork* w);
u8 UpdatePrizeMapCardShow(PrizeMapCardWork* w, void* a);
void StockNameSora_0(StockNameWork* w, const s32* src);
void StockNameRiku_0(StockNameWork* w, const s32* src);
void DrawDeckFilterTab(u8 kind, u8 slot);
u8 SoraCardUpdate(CardDisplayWork* p, void* a);
u8 SoraCardBreakFall(CardDisplayWork* p, void* a);
u8 RikuCardBreakFall(CardDisplayWork* p, void* a);
u8 EnemyCardBreakFall(CardDisplayWork* p, void* a);
void card_reload_0(CardDisplayWork* p, CardDisplayArgs* a);
void func_08091048(CardDisplayWork* p, CardDisplayArgs* a);
void LoadSoraReloadCardGfx(CardDisplayWork* p);
void Reload_Card_0(CardDisplayWork* p, CardDisplayArgs* a);
void func_08091138(CardDisplayWork* p, CardDisplayArgs* a);
u8 UpdatePremireChanceCardSpin(PremireChanceCardWork* w, void* a);
u8 EV_BG_EFFECT_1(EventBgEffectWork* w, void* a);
void PremireChanceCard_0(PremireChanceCardWork* w, CardSlot* a);
void Card_EFFECT_0(CardEffectWork* w, CardEffectArgs* a);
void deckexchange_3(DeckExchangeWork* w);
void TickSoraHcEffectOnReload();
void TickRikuHcEffectOnReload();
u8 GetHcEffectCountUnit(HcEffectNameWork* w, u16 n);
void StockInfo_0(StockInfoWork* w, u8* active);
u8 UpdateCardMsgwinLoadFace(CardMsgWinWork* w, void* a);
u8 DispatchRikuCardCommand(CardDisplayWork* p, void* a);
u8 Reload_Card_1(CardDisplayWork* p, void* a);
u8 UpdateDeckMenuSlideIn(DeckMenuWork* w, void* a);
u8 SoraCardMoveToPlay(CardDisplayWork* p, void* a);
u8 UpdateSysmsgwinChoiceSetup(SysMsgWinWork* w, void* a);
u8 CheckCardDeletable(DeckMenuWork* w);
u8 UpdateCardMsgwinTyping(CardMsgWinWork* w, void* a);
u8 RikuCardMoveToPlay(CardDisplayWork* p, void* a);
u8 RikuCardUpdate(CardDisplayWork* p, void* a);
u8 SoraGimmickCardFly(CardDisplayWork* p, void* a);
u8 UpdateDeckMenuOpenDeleteMode(DeckMenuWork* w, void* a);
u8 UpdateSysmsgwinChoiceInput(SysMsgWinWork* w, void* a);
void sysmsgwinChoice_2(SysMsgWinWork* w);
void DrawRikuCardTotals();
void WorldSel_Before_0(WorldSelBeforeWork* w, WorldSelBeforeArgs* a);
void SoraCardInit(CardDisplayWork* p, CardDisplayArgs* a);
void RikuCardInit(CardDisplayWork* p, CardDisplayArgs* a);
u8 RELOAD_CHILDREN_1(ReloadChildWork* w, void* a);
u8 SoraStockMoveToPlay(CardDisplayWork* p, void* a);
u8 UpdatePrizeMapCardFlight(PrizeMapCardWork* w, void* a);
u8 RikuStockMoveToPlay(CardDisplayWork* p, void* a);
u8 UpdateMapSelectTutorial(MapSelectWork* w, void* a);
u8 UpdateBossPrizeFlight(BossPrizeWork* w, void* a);
void HCEffectName_0(HcEffectNameWork* w, u8* a);
u8 RikuStockWaitPlayEnd(CardDisplayWork* p, void* a);
u8 UpdatePremireChanceStop(struct PremireChanceWork* w, void* a);
u8 EnemyStockMoveToSlot(CardDisplayWork* p, void* a);

void CardName_0(CardNameWork* w);
void DarkPoint_0(DarkPointWork* w);
s32 DarkPoint_1(DarkPointWork* w);
void DarkPoint_2(DarkPointWork* w);
void DarkPoint_3(DarkPointWork* w);

#ifndef VERSION_EU
void DrawDeckExchangeDeckNames(DeckExchangeWork* w, u8 b);
#ifndef VERSION_EU
u8 UpdateDeckExchangeLoadDeckInfo(DeckExchangeWork* w, void* a);
void HighlightDeckExchangeDeckTab(DeckExchangeWork* w, u8 b);
#endif
#endif

typedef struct SelmapEventKeyWork {
    void* tiles;
    ObjPalette* palette;
    EventKeyCard cards[4];
    SelmapEventKeyArgs* args;
    AnimState anim;
    void* gfx;
    void* unk_F8;
    s32 rowX;
    s32 rowY;
    s32 rowTargetX;
    s32 unk_108;
    s32 unk_10C;
    s32 unk_110;
    u16 unk_114;
    u16 unk_116;
    u16 pulseAngle;
    u8 slideSteps;
    u8 unk_11B;
    u8 unk_11C;
    u8 unk_11D;
    u8 mosaicX;
    u8 mosaicY;
    u8 mosaicTimer;
    u8 keyCount;
    u8 paidCount;
    u8 unk_123;
} SelmapEventKeyWork;

typedef char SelmapEventKeyWork_size[(sizeof(SelmapEventKeyWork) == 0x124) ? 1 : -1];

typedef struct KeyboardLineLayout {
    const s16* positions;
    s16 count;
    u8 unk_06[2];
} KeyboardLineLayout;

extern const s16 gKeyboardKeyX[];
extern const s16 gKeyboardKeyY[];
extern const KeyboardLineLayout gKeyboardRowLayouts[];
extern const KeyboardLineLayout gKeyboardColumnLayouts[];

#ifdef VERSION_EU
extern const KeyboardLineLayout gKeyboardSymbolRowLayouts[];
extern const KeyboardLineLayout gKeyboardSymbolColumnLayouts[];
extern const s16 gKeyboardPageTabXEu[];
#endif

#ifdef VERSION_JP
extern const s16 gKeyboardPageTabXJp[];
extern const u8 gUnkJp_090089B0[];
extern const u8 gUnkJp_090089BC[];
extern const u8 gUnkJp_090089C8[];
#endif

typedef struct PremireChanceWork {
    void* tiles;
    ObjPalette* palette;
    void* tiles2;
    ObjPalette* palette2;
    void* tiles3;
    ObjPalette* palette3;
    void* tiles4;
    ObjPalette* palette4;
    void* tiles5;
    CardSlot* slots;
    void* gfx;
    void* gfx2;
    s16 titleX;
    u8 unk_32[2];
    s32 topY;
    s32 bottomY;
    TaskPool tasks;
    u8 cardCount;
    u8 spinDelay;
    u8 advanced;
    u8 unk_53;
    AnimState anim;
    AnimState anim2;
    u8 stopped;
    u8 inputEnabled;
    u8 resultPending;
    u8 unk_87;
    u8 bgAnimDuration;
    u8 resultTimer;
    u8 stopTimer;
    u8 titleSteps;
    u8 slideSteps;
    u8 unk_8D[3];
} PremireChanceWork;

typedef struct LevelUpWork {
    void* unk_000[8];
#ifdef VERSION_EU
    void* tiles5[3];
    u8 unk_02C[0xC];
#else
    TextSlot textSlots[6][36];
#endif
    ObjPalette* palette;
    ObjPalette* palette2;
    void* tiles;
    ObjPalette* palette3;
    void* tiles2;
    void* tiles3;
    ObjPalette* palette4;
    TaskPool pool;
    AnimState anim;
    void* tiles4;
    ObjPalette* palette5;
    void* gfx;
    AnimState anim2;
    void* gfx2;
    s16 x4[3];
    s16 y4[3];
    s16 x5[3];
    s16 y5[3];
    s16 x;
    s16 x2;
    s32 y;
    s32 y2;
    s16 x3;
    s16 y3;
    s16 x6;
    s16 bgScrollX;
    s16 statsOffsetX;
    u16 levelDigits[3];
    u16 maxHpDigits[4];
    u16 cpDigits[4];
    u16 dpDigits[4];
    u16 apDigits[4];
    s16 timer;
    s16 unk_7A6;
    s32 x7;
    s32 y6;
    s8 cursor;
    s8 headerSteps;
    s8 optionSteps[3];
    s8 slideSteps;
    s8 cursorSteps;
    u8 textSlotCounts[6];
    u8 state;
    u8 blinkTimer;
    u8 barSteps;
    u8 playerSteps;
    u8 blinkOn;
    u8 loaded[2];
    u8 messageActive;
    u8 effectShown;
    u8 bossBattle;
    u8 applied;
    u8 optionEnabled[3];
} LevelUpWork;

enum StatIncreaseFlag {
    STAT_INCREASE_FLAG_DP = 0x4000,
    STAT_INCREASE_FLAG_MAX_HP = 0x8000
};

typedef struct StatIncreaseDisplayArgs {
    u8* done;
    u32 flags : 16;
    u32 amount : 16;
} StatIncreaseDisplayArgs;

extern s8 gLinkDecksAllocated;

extern const MapTileAnimationDef* gMapTileAnimationDefs[6];
extern const MapTileAnimationDef* gUnk_09EE4A44;
extern const MapTileAnimationDef* gUnk_09EE4A48;
extern const MapTileAnimationDef* gUnk_09EE4A4C;
extern const u16* gRikuDeckCards[12];
extern const u16* gRikuDeckEnemyCards[12];
#ifdef VERSION_EU
extern u8 gUnkEu_09F6FD74[7];
extern u8 gUnkEu_09F6FD7B[7];
extern u8 gUnkEu_09F6FD82[7];
extern void* gDeckButtonLabelTiles[5];
extern void** gDeckButtonLabelSprites[5];
extern void* gDeckCommandMenuTiles[5];
extern void* gDeckTitleBannerTiles[5];
extern u8* gDeckEquipMarkerTiles[5];
extern void* gDeckKeyboardCursorTiles[5];
extern void** gDeckKeyboardCursorSprites[5];
extern void* gDeckKeyboardCursorAnims[5];
extern const u8* gDeckKeyboardLetterRows[8];
extern const u8* gDeckKeyboardSymbolRows[7];
extern void* gMapCardUiExtraTilesByLanguage[5];
extern void** gMapCardUiSpritesByLanguage[5];
extern void** gMapSelectTitleSpritesByLanguage[5];
extern u8 gUnkEu_09189F36[];
extern u8 gUnkEu_0918A73A[];
extern u8 gUnkEu_0918A48E[];
extern u8 gUnkEu_0918A1E2[];
extern u8 gUnkEu_0918B8F2[];
extern u8 gUnkEu_0919016A[];
extern u8 gUnkEu_0918E942[];
extern u8 gUnkEu_0918D11A[];
extern u8 gUnkEu_09191992[];
extern u8 gUnkEu_0919236A[];
extern u8 gUnkEu_09192022[];
extern u8 gUnkEu_09191CDA[];
extern u8 gUnkEu_094C9860[];
extern u8 gUnkEu_094C9C20[];
#else
extern u16 gUnk_09EE4AC8[7];
extern u16 gUnk_09EE4AD6[7];
extern u16 gUnk_09EE4AE4[7];
extern const u8* gDeckKeyboardRows[7];
#endif
#ifdef VERSION_JP
extern const u8* gDeckKeyboardKatakanaRows[7];
extern const u8* gDeckKeyboardAlphanumericRows[7];
#endif
extern const u16 gUnk_096102B8[];
extern const void* gMapSelectBgMapBlocks[2];
extern s16 gMapSelectValueColumnX[5];
extern s16 gMapSelectValueRowY[2];
extern u16 gMapSelectCountTileIndices[10];

extern Mode gModePremire;
#ifdef VERSION_EU
extern void* gHcEffectCountUnitTilesByLanguage[5];
extern void** gHcEffectCountUnitSpritesByLanguage[5];
extern void* gLevelUpBgTilesByLanguage[5];
extern void* gLevelUpHeaderTilesByLanguage[5];
extern void** gLvupEffectSpritesByLanguage[5];
extern u8 gUnkEu_094CE490[];
extern u8 gUnkEu_094CE820[];
extern u8 gUnkEu_094CE6F0[];
extern u8 gUnkEu_094CE5C0[];
extern u8 gUnkEu_094CF704[];
extern u8 gUnkEu_094D72E4[];
extern u8 gUnkEu_094DB664[];
extern u8 gUnkEu_094D9FE4[];
extern u8 gUnkEu_094D8964[];
#else
#ifdef VERSION_JP
extern const u8 gLevelUpDisabledText[];
#else
extern const u16 gLevelUpDisabledText[];
#endif
extern u16 gUnk_0815A066[];
extern u16 gUnk_0815A0BA[];
extern u16 gUnk_0815B1D2[];
extern u16 gUnk_0815A078[];
extern u16 gUnk_0815A0CC[];
extern u16 gUnk_0815B1A8[];
extern u16 gUnk_0815A116[];
extern u16 gUnk_0815A158[];
extern u16 gUnk_0815A0F4[];
extern u16 gUnk_0815A130[];
extern u16 gUnk_0815A176[];
extern u16* gLevelUpSoraTexts[7];
extern u16* gLevelUpRikuTexts[7];
#endif
extern const void* gLevelUpBgMapBlocks[2];
extern void* gLevelUpOptionBgMaps[3];
extern void* gEventBgEffectMaps[7];

extern const CardHelpText* gUnk_09EE79EC[];
extern const CardHelpText* gUnk_09EE79F4[];
extern const CardHelpText* gUnk_09EE7A00[];
extern const CardHelpText* gUnk_09EE7A08[];
extern const CardHelpText* gUnk_09EE7A10[];
extern const CardHelpText* gUnk_09EE7A18[];
extern const CardHelpText* gUnk_09EE7A20[];
extern const CardHelpText* gUnk_09EE7A28[];
extern const CardHelpText* gUnk_09EE7A30[];
extern const CardHelpText* gUnk_09EE7A38[];
extern const CardHelpText* gUnk_09EE7A40[];
extern const CardHelpText* gUnk_09EE7A48[];
extern const CardHelpText* gUnk_09EE7A50[];
extern const CardHelpText* gUnk_09EE7A58[];
extern const CardHelpText* gUnk_09EE7A60[];
extern const CardHelpText* gUnk_09EE7A68[];
extern const CardHelpText* gUnk_09EE7A70[];
extern const CardHelpText* gUnk_09EE7A78[];
extern const CardHelpText* gUnk_09EE7A80[];
extern const CardHelpText* gUnk_09EE7A88[];
extern const CardHelpText* gUnk_09EE7A90[];
extern const CardHelpText* gUnk_09EE7A98[];
extern const CardHelpText* gUnk_09EE7AA0[];
extern const CardHelpText* gUnk_09EE7AA8[];
extern const CardHelpText* gUnk_09EE7AB8[];
extern const CardHelpText* gUnk_09EE7AC8[];
extern const CardHelpText* gUnk_09EE7AD8[];
extern const CardHelpText* gUnk_09EE7AE8[];
extern const CardHelpText* gUnk_09EE7AF8[];
extern const CardHelpText* gUnk_09EE7B08[];
extern const CardHelpText* gUnk_09EE7B18[];
extern const CardHelpText* gUnk_09EE7B30[];
extern const CardHelpText* gUnk_09EE7B28[];
extern const CardHelpText* gUnk_09EE7B38[];
extern const CardHelpText* gUnk_09EE7B48[];
extern const CardHelpText* gUnk_09EE7B58[];
extern const CardHelpText* gUnk_09EE7B68[];
extern const CardHelpText* gUnk_09EE7B78[];
extern const CardHelpText* gUnk_09EE7B88[];
extern const CardHelpText* gUnk_09EE7B98[];
extern const CardHelpText* gUnk_09EE7BA0[];
extern const CardHelpText* gUnk_09EE7BA8[];
extern const CardHelpText* gUnk_09EE7BB8[];
extern const CardHelpText* gUnk_09EE7BC8[];
extern const CardHelpText* gUnk_09EE7BD8[];
extern const CardHelpText* gUnk_09EE7BE8[];
extern const CardHelpText* gUnk_09EE7BF8[];
extern const CardHelpText* gUnk_09EE7C08[];
extern const CardHelpText* gUnk_09EE7C18[];
extern const CardHelpText* gUnk_09EE7C28[];
extern const CardHelpText* gUnk_09EE7C38[];
extern const CardHelpText* gUnk_09EE7D54[];
extern const CardHelpText* gUnk_09EE7D44[];
extern const CardHelpText* gUnk_09EE7D64[];
extern const CardHelpText* gUnk_09EE7C48[];
extern const CardHelpText* gUnk_09EE7C50[];
extern const CardHelpText* gUnk_09EE7C58[];
extern const CardHelpText* gUnk_09EE7C64[];
extern const CardHelpText* gUnk_09EE7D74[];
extern const CardHelpText* gUnk_09EE7C6C[];
extern const CardHelpText* gUnk_09EE7C74[];
extern const CardHelpText* gUnk_09EE7C7C[];
extern const CardHelpText* gUnk_09EE7C84[];
extern const CardHelpText* gUnk_09EE7C8C[];
extern const CardHelpText* gUnk_09EE7C94[];
extern const CardHelpText* gUnk_09EE7C9C[];
extern const CardHelpText* gUnk_09EE7CA4[];
extern const CardHelpText* gUnk_09EE7CAC[];
extern const CardHelpText* gUnk_09EE7CB4[];
extern const CardHelpText* gUnk_09EE7D7C[];
extern const CardHelpText* gUnk_09EE7CBC[];
extern const CardHelpText* gUnk_09EE7CC8[];
extern const CardHelpText* gUnk_09EE7CD0[];
extern const CardHelpText* gUnk_09EE7CD8[];
extern const CardHelpText* gUnk_09EE7CE0[];
extern const CardHelpText* gUnk_09EE7CE8[];
extern const CardHelpText* gUnk_09EE7CF0[];
extern const CardHelpText* gUnk_09EE7CF8[];
extern const CardHelpText* gUnk_09EE7D00[];
extern const CardHelpText* gUnk_09EE7D08[];
extern const CardHelpText* gUnk_09EE7D10[];
extern const CardHelpText* gUnk_09EE7D18[];
extern const CardHelpText* gUnk_09EE7D20[];
extern const CardHelpText* gUnk_09EE7D2C[];
extern const CardHelpText* gUnk_09EE7D34[];
extern const CardHelpText* gUnk_09EE7D3C[];
extern const CardHelpDef* gCardHelpDefs[];
#ifdef VERSION_EU
extern u8 gUnkEu_090D1DA5[];
extern u8* gUnkEu_09F73464[5];
extern u8 gUnkEu_091926B2[];
extern u8 gUnkEu_0919308A[];
extern u8 gUnkEu_09192D42[];
extern u8 gUnkEu_091929FA[];
extern void* gRikuDeckTitleBannerTiles[5];
extern u8* gRikuDeckEquipMarkerTiles[5];
extern Mode gModeTextCheck;
#else
extern Mode gModeDeckExchange;
#endif

#ifndef VERSION_EU
#endif
#ifdef VERSION_EU
#endif
extern u8 gBossCardRequestValue;
extern u8 gBossCardRequest;
extern Deck gDecks[3];
extern u16 gCardCollection[999];
extern Deck* gLinkPartnerDeck;
extern u16 gCardCount;
extern CardUiSpriteState gCardUiSpriteState;
extern MapCardUiResources gMapCardUiResources;
extern u8 gMapCardCounts[270];
extern struct CardListWork* gCardListWork;
extern u8 gMessageWindowOpen;
extern u8 gMessageWindowAnswerYes;
#ifndef VERSION_EU
extern u16 gSioTradeCardId;
#endif
extern u8 gRikuDeckTutorialState;
extern TaskDesc gTaskDescCardSora;
extern TaskDesc gTaskDescCardNotHave;
extern TaskDesc gTaskDescCardReload;
extern TaskDesc gTaskDescCardRiku;
extern TaskDesc gTaskDescNOCard;
extern TaskDesc gTaskDescReloadCard;
extern TaskDesc gTaskDescBosscard;
extern const MapTileAnimationDef* gMapTileAnimationDefs[6];
extern const u16* gRikuDeckCards[12];
extern const u16* gRikuDeckEnemyCards[12];
#ifndef VERSION_EU
extern u16 gUnk_09EE4AC8[7];
extern u16 gUnk_09EE4AD6[7];
extern u16 gUnk_09EE4AE4[7];
#endif
#ifdef VERSION_EU
extern void* gDeckButtonLabelTiles[5];
extern void** gDeckButtonLabelSprites[5];
extern void* gDeckCommandMenuTiles[5];
extern void** gDeckCommandMenuSprites[5];
extern void* gDeckTitleBannerTiles[5];
extern void** gDeckTitleBannerSprites[5];
extern u8* gDeckEquipMarkerTiles[5];
extern void* gDeckKeyboardCursorTiles[5];
extern void** gDeckKeyboardCursorSprites[5];
extern void* gDeckKeyboardCursorAnims[5];
#endif
#ifdef VERSION_US
extern const u8* gDeckKeyboardRows[7];
#endif
#ifdef VERSION_JP
extern const u8* gDeckKeyboardKatakanaRows[7];
extern const u8* gDeckKeyboardAlphanumericRows[7];
#endif
#ifdef VERSION_EU
extern const u8* gDeckKeyboardLetterRows[8];
extern const u8* gDeckKeyboardSymbolRows[7];
#endif
extern TaskDesc gTaskDescDeckCard2;
extern TaskDesc gTaskDescEnemyUsecard;
extern TaskDesc gUnk_09EE4B70;
extern TaskDesc gUnk_09EE4B88;
#ifdef VERSION_EU
extern void* gMapCardUiExtraTilesByLanguage[5];
extern void** gMapCardUiSpritesByLanguage[5];
#endif
extern const void* gMapSelectBgMapBlocks[2];
extern s16 gMapSelectValueColumnX[5];
extern s16 gMapSelectValueRowY[2];
#ifdef VERSION_EU
extern void** gMapSelectTitleSpritesByLanguage[5];
#endif
extern TaskDesc gTaskDescMapSelect;
extern u16 gMapSelectCountTileIndices[10];
extern MapCardBackDef gMapCardBackDefs[5];
extern MapCardDef gMapCardDefs[260];
extern s16 gMapcardSlotX[6];
extern PrizeMapCardBackAnimStep gPrizeMapCardBackAnim[7];
extern TaskDesc gTaskDescMapcard;
extern TaskDesc gTaskDescReloadGage;
extern void* gReloadCounterTiles[4];
extern AnimHeader** gReloadCounterAnims[4];
extern void** gReloadCounterFrames[4];
extern void* gReloadCardTiles[4];
extern void** gReloadGaugeFrames[4];
extern AnimHeader** gReloadGaugeAnims[4];
extern TaskDesc gTaskDescFieldPrizeCard;
extern TaskDesc gTaskDescPrizeCardInit;
extern TaskDesc gTaskDescPrizeCardInitBoss;
extern TaskDesc gTaskDescDispCardname;
extern TaskDesc gTaskDescVersion;
extern TaskDesc gTaskDescPrizeMapCard;
#ifdef VERSION_EU
extern void* gSelmapEventKeyTitleAnimsByLanguage[5];
extern void* gSelmapEventKeyTitleFramesByLanguage[5];
extern void* gSelmapEventKeyTitleTilesByLanguage[5];
#endif
extern TaskDesc gTaskDescSELMAPEVKEY;
extern void* gReloadChildTiles[4];
extern TaskDesc gTaskDescReloadChildren;
extern void* gRevCountTileSources[4];
extern void** gRevCountSprites[4];
extern TaskDesc gTaskDescREVCOUNT;
extern void* gReloadTiles[3];
extern AnimHeader** gReloadAnims[3];
extern void** gReloadFrames[3];
extern TaskDesc gTaskDescRELOAD;
extern TaskDesc gTaskDescPrizeBoss;
extern TaskDesc gTaskDescCardEFFECT;
extern TaskDesc gTaskDescScrollbar;
extern TaskDesc gTaskDescFriendCard;
extern TaskDesc gTaskDescHeartlessCard;
extern TaskDesc gTaskDescGimmickCard;
extern TaskDesc gTaskDescStockNameRiku;
#ifdef VERSION_EU
extern void** gPremireChanceTitles[5];
#endif
extern TaskDesc gTaskDescPremireChance;
extern TaskDesc gTaskDescPremireChanceCard;
extern TaskDesc gTaskDescCardName;
#ifdef VERSION_EU
extern void* gHcEffectCountUnitTilesByLanguage[5];
extern void** gHcEffectCountUnitSpritesByLanguage[5];
#endif
extern TaskDesc gTaskDescHCEffectName;
extern TaskDesc gTaskDescNumberPlus;
#ifdef VERSION_EU
extern void* gLevelUpBgTilesByLanguage[5];
extern void* gLevelUpHeaderTilesByLanguage[5];
extern void** gUnk_09EEA1BC[5];
extern void* gLevelUpOptionTilesByLanguage[5];
extern void** gLevelUpOptionSpritesByLanguage[5];
#endif
#ifndef VERSION_EU
extern u16* gLevelUpSoraTexts[7];
extern u16* gLevelUpRikuTexts[7];
#endif
extern const void* gLevelUpBgMapBlocks[2];
extern void* gLevelUpOptionBgMaps[3];
extern TaskDesc gTaskDescLevelUp;
#ifdef VERSION_EU
extern void* gLvupEffectSprites[6];
extern void** gLvupEffectSpritesByLanguage[5];
#endif
extern TaskDesc gTaskDescLVUPEFFECT;
extern TaskDesc gTaskDescLvupLogo;
extern const EventBgEffectDef* gEventBgEffectDefs[8];
extern TaskDesc gTaskDescEVBGEFFECT;
extern const CardHelpDef* gCardHelpDefs[];
extern TaskDesc gTaskDescStockInfo;
extern TaskDesc gTaskDescLvupMsg;
extern TaskDesc gTaskDescDeckEquip;
extern TaskDesc gTaskDescDeckYesNo;
extern TaskDesc gTaskDescDeckClear;
extern TaskDesc gTaskDescDeckErrorCp;
extern TaskDesc gTaskDescDeckErrorNoAttackCard;
extern TaskDesc gTaskDescDeckErrorLastAttackCard;
extern TaskDesc gTaskDescDeckErrorDeckFull;
extern CardMessageDef gCardMessageDefs[];
extern TaskDesc gTaskDescCardMsgwin;
extern TaskDesc gTaskDescSysmsgwin;
extern TaskDesc gTaskDescSysmsgwinChoice;
extern WorldSelAnim gWorldSelAnims[30];
extern TaskDesc gTaskDescWorldSelBefore;
#ifdef VERSION_EU
extern void* gRikuDeckTitleBannerTiles[5];
extern void** gRikuDeckTitleBannerSprites[5];
extern u8* gRikuDeckEquipMarkerTiles[5];
#endif
#ifndef VERSION_EU
extern TaskDesc gTaskDescDeckexchange;
#endif
extern CardDescriptionText* gCardKindDescriptions[98];

void AddPickedCardToSoraDeck(CardBattleWork* w);
u8 AreCardsSettled(CardDisplayWork** p, u8 n);
void BeginSoraReloadDeal(CardBattleWork* w);
void BuildDebugKingdomKeyDeck(u8 a);
void ClearStockedCardSlots(CardBattleWork* w);
void ClearUsedCardSlots(CardBattleWork* w, u8 b);
u8 CollectionHasCard(u16 id);
void ConvertActiveDeckCardToPremium(u16 index);
u16 CountActiveDeckCardsOfCategory(u8 slot);
u16 CountAvailableCards(CardBattleWork* w, u8 n);
void CountCardsNotInDeckByCategory(u8 mode, u16* out);
u16 CountMapCardsOfKind(u16 a);
u16 CountRemainingAttackCards(CardBattleWork* w, u8 b);
u16 CountZeroValueMapCards();
void CreateCardNameDisplay(void* a, const void* b);
void CycleSoraCardList(CardBattleWork* w);
void DeckCard2ReleaseGfx(DeckCard2Work* node);
void DrawCollectionCategoryCount(u16 a, u8 b);
void DrawDeckCategoryCount(u8 a, u8 b);
void DrawValueCount(u8 a, u16 b);
void FillStarterDeck();
void FreePrintLayer();
u16 GetNextRandomHcEffect(u16* p);
u16 GetRandomHcEffect();
u8 HasMapCard(u16 a);
void IncrementReloadCount(CardBattleWork* w);
void InitSoraCardList(CardBattleWork* w, s32 mode);
void InitSoraTutorialCardList(CardBattleWork* w, s32 mode);
u8 IsCardDisplayOffScreen(CardDisplayWork* p);
u8 IsLevelUpStockUnlocked();
u16 ListCardsNotInDeckByKind(CardKindEntry* out, u8 deck, u8 mode, u16 n, void* p);
void LoadCardDisplayGfx(CardDisplayWork* p);
void ObtainStarterCards();
u16 PickPrizeMapCardForWorld(u16 a, s32 b);
void ReleaseCardDisplayGfx(CardDisplayWork* p);
void RemoveItemCards(CardBattleWork* w);
void RemoveSoraCardDisplays(CardBattleWork* w);
void ResetBossCardValue();
void ResetPrintLines();
void ResetSoraReloadGauge(CardBattleWork* w);
void RestoreCardsForElixir(CardBattleWork* w);
void RestoreCardsForEther(CardBattleWork* w);
void RestoreCardsForHiPotion(CardBattleWork* w);
void RestoreCardsForMegaEther(CardBattleWork* w);
void RestoreCardsForMegaPotion(CardBattleWork* w);
void RestoreCardsForPotion(CardBattleWork* w);
void SelectNextSoraCard(CardBattleWork* w, u8 b);
void ShuffleCardSlots(CardSlot* slots, u8 n);
s32 StockSoraCard(CardBattleWork* w);
void SwitchSoraCardList(CardBattleWork* w);
void SyncCardDisplayGfx(CardDisplayWork* p);
void TrackLevelUpEffectTarget(LevelUpEffectWork* w);
void UpdateCardDisplayFlip(CardDisplayWork* p);
s32 UseSoraCard(CardBattleWork* w);
s32 UseSoraGimmickCard(CardBattleWork* w);
s32 UseSoraHeartlessCard(CardBattleWork* w);
void UseSoraStock(CardBattleWork* w);
void ClearSoraCardPlayFlags();
u8 SoraStockStartUnopposedPlay(CardDisplayWork* p, void* a);
void func_080AB22C(u8 a);
void func_080AB4AC(u8 a);
void func_080AB964();
void func_080AB968();
#ifdef VERSION_EU
extern AnimHeader gUnk_090A44BA;
#endif
#ifdef VERSION_JP
extern u8 gUnk_0814FBD4[];
#endif

#endif /* GUARD_CARD_H */

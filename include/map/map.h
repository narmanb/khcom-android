#ifndef GUARD_MAP_H
#define GUARD_MAP_H

#include "map_types.h"
#include "map_room_types.h"
#include "types.h"
#include "game_state.h"
#include "anim.h"
#include "mode.h"
#include "taskpool.h"
#include "fld_types.h"
#include "text_types.h"
#include "battle_actor_types.h"
#include "obj.h"

typedef struct MapPlatform {
    u16 left;
    u16 right;
    s32 z;
    u8 hasStairs;
    u8 unk_09;
    u16 x;
    u16 y;
    u8 spotType;
    u8 unk_0F;
    s32 spotUpperZ;
    s32 spotLowerZ;
} MapPlatform;

enum MapEnmDefFlag {
    MAP_ENM_DEF_FLAG_NO_SHADOW = 0x1,
    MAP_ENM_DEF_FLAG_AIRBORNE = 0x2,
    MAP_ENM_DEF_FLAG_SPAWN_ANYWHERE = 0x4,
    MAP_ENM_DEF_FLAG_GUARD = 0x8
};

typedef struct MapEnmDef {
    const AnimDef* animDef;
    void* palette;
    u16 tileCount;
    u16 height;
    u16 radius;
    u16 unk_0E;
    TaskDesc* desc;
    u16 flags;
    u16 unk_16;
} MapEnmDef;

typedef struct MapGmkDef {
    void* palette;
    const void* tiles;
    u16 tilesSize;
    u8 unk_0A;
    u8 unk_0B;
    void* gfxTable;
    void* anims;
    u8 ownTiles;
    u8 spotFinder;
    s16 offsetX;
    s16 offsetY;
    s16 offsetZ;
    u16 radius;
    u16 height;
    u16 hitSong;
    u16 unk_22;
    void* desc;
} MapGmkDef;

typedef struct MapAnmSlot {
    const void* tiles;
    u16 frameSize;
    u8 unk_06[0x02];
    u8* dest;
    u8* pending;
    s16 timer;
    u8 unk_12[0x02];
    void* script;
    s16* scriptPos;
} MapAnmSlot;

enum GmkFlag {
    GMK_FLAG_DESTROYED = 0x1,
    GMK_FLAG_USED = 0x2,
    GMK_FLAG_TOGGLED = 0x4,
    GMK_FLAG_HAS_ENEMY = 0x8
};

typedef struct MapGmkPlacement {
    u16 flags;
    u8 unk_02[0x02];
    FldPos pos;
    const MapGmkDef* def;
} MapGmkPlacement;

typedef struct MapCellPattern {
    s16 dx;
    s16 dy;
    u8 bg3Piece;
    u8 bg2Piece;
    u16 edgeFlags;
    u16 edgeIgnoreMask;
    u8 unk_0A[0x02];
} MapCellPattern;

typedef struct MapCardAttributes {
    u16 kind;
    u16 value;
    u16 color;
} MapCardAttributes;

typedef struct UnkStruct_080E8E24 {
    u8 unk_00[0x02];
    u16 unk_02;
} UnkStruct_080E8E24;

typedef struct PrzCardChance {
    u8 cardIndex;
    u8 unk_01;
    u16 weight;
    u16 weight2;
    u8 unk_06[0x02];
} PrzCardChance;

typedef struct MapPrizeArgs {
    u8 worldPrize;
    u8 unk_01[0x03];
    s32 x;
    s32 y;
    s32 z;
    u8 unk_10[0x04];
    u16 id;
    u8 unk_16[0x02];
} MapPrizeArgs;

typedef struct MapEnmArgs {
    const MapEnmDef* def;
    void (*update)(struct MapEnmWork*);
    FldPos pos;
    u8 angle;
    u8 unk_19[0x03];
    s32 speed;
} MapEnmArgs;

enum MapEnmFlag {
    MAP_ENM_FLAG_HFLIP = 0x1,
    MAP_ENM_FLAG_PERSISTENT = 0x2,
    MAP_ENM_FLAG_REMOVED = 0x4,
    MAP_ENM_FLAG_AGGRESSIVE = 0x8,
    MAP_ENM_FLAG_SLOW = 0x10,
    MAP_ENM_FLAG_ASLEEP = 0x20,
    MAP_ENM_FLAG_FIRST_STRIKE = 0x40,
    MAP_ENM_FLAG_WHITE_MUSHROOM = 0x100,
    MAP_ENM_FLAG_BLACK_FUNGUS = 0x200
};

typedef struct MapEnmWork {
    const MapEnmDef* def;
    u16 flags;
    u16 paletteBank;
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    s16 radius;
    s16 height;
    void (*update)(struct MapEnmWork*);
    u16 timer;
    u16 unk_D2;
    s32 targetX;
    s32 targetY;
    s32 targetZ;
    s16 colliderDelay;
    u8 unk_E2[0x02];
    TaskPool tasks;
} MapEnmWork;

typedef struct MapEnm01Work {
    MapEnmWork enm;
    u8 wasOnScreen;
    u8 unk_F9[0x03];
} MapEnm01Work;

typedef struct MapEnm03Work {
    MapEnmWork enm;
    FldPos home;
} MapEnm03Work;

typedef struct LoadGameMenuWork {
    ObjPalette* palette2;
    void* tiles2;
    s32 y;
    s32 y2;
    s32 x;
    AnimState anim;
    ObjPalette* palette;
    void* tiles;
    s32 y3;
    ObjPalette* palette7;
    TextSlot textSlots[0x24];
    u8 textSlotCount;
    u8 unk_15D;
    u16 slotBaseY;
    ObjPalette* palette3;
    void* tiles3;
    ObjPalette* palette4;
    void* tiles4;
    ObjPalette* palette5;
    void* tiles5;
    ObjPalette* palette6;
    void* tiles6;
    u8 showRikuSlots;
    u8 forSioBattle;
    u8 loaded;
    u8 selectedSlot;
    u8 lastSlot;
    u8 unk_185;
    u16 timer;
    void (*update)(struct LoadGameMenuWork*);
} LoadGameMenuWork;

typedef struct MenuMsgWork {
    u8 toTitle;
    u8 unk_01[0x03];
    void (*update)(struct MenuMsgWork*);
    TaskPool tasks;
} MenuMsgWork;

typedef struct NewGameSlotMenuWork {
    ObjPalette* palette2;
    void* tiles2;
    s32 y;
    s32 y2;
    ObjPalette* palette3;
    void* tiles3;
    AnimState anim;
    ObjPalette* palette;
    void* tiles;
    s32 y3;
    ObjPalette* palette8;
    TextSlot textSlots[0x24];
    u8 textSlotCount;
    u8 unk_161;
    u16 slotBaseY;
    ObjPalette* palette9;
    TextSlot textSlots2[0x36];
    u8 textSlotCount2;
    u8 unk_319[0x03];
    ObjPalette* palette4;
    void* tiles4;
    ObjPalette* palette5;
    void* tiles5;
    ObjPalette* palette6;
    void* tiles6;
    ObjPalette* palette7;
    void* tiles7;
    u8 confirmed;
    u8 isRiku;
    u8 selectedSlot;
    u8 unk_33F;
    u16 timer;
    u8 unk_342[0x02];
    void (*update)(struct NewGameSlotMenuWork*);
} NewGameSlotMenuWork;

typedef struct MapRndWork {
    TaskPool tasks;
} MapRndWork;

typedef struct MapMenuWork {
    ObjPalette* palette2;
    void* tiles2;
    s32 y;
    s32 y2;
    void* tiles3;
    s32 x;
    void* tiles4;
    s32 x2;
    ObjPalette* palette3;
    void* tiles5;
    s32 x3;
    s32 x4;
    s32 x5;
    u8 cursorVisible;
    u8 unk_035[0x03];
    ObjPalette* palette4;
    ObjPalette* palette5;
    void* tiles6;
    s32 x6;
    AnimState anim;
    ObjPalette* palette;
    void* tiles;
    s32 x7;
    s32 y3;
    ObjPalette* palette8;
    TextSlot textSlots[0x18];
    u8 textSlotCount;
    u8 unk_135[0x03];
    ObjPalette* palette6;
    void* tiles8;
    ObjPalette* palette7;
    void* tiles7;
    s32 x8;
    s32 y4;
    s32 playerStartX;
    s32 playerStartY;
    ObjPalette* palette9[0x03];
    void* tiles9[0x03];
    void* gfx[0x03];
    ObjPalette* confirmPalette;
#ifdef VERSION_EU
    TextSlot textSlots2[0x42];
#else
    TextSlot textSlots2[0x21];
#endif
    u8 textSlotCount2;
    u8 unk_289[0x03];
    TextSlot textSlots3[0x06];
    u8 textSlotCount3;
    u8 unk_2BD[0x03];
    TextSlot textSlots4[0x09];
    u8 textSlotCount4;
    u8 cursor;
    u8 confirmCursor;
    u8 unk_30B;
    u16 steps;
    u8 panelsVisible;
    u8 reopened;
    s32 (*update)(struct MapMenuWork*);
} MapMenuWork;

typedef struct MapSaveWork {
    FldRes* palette2;
    void* tiles2;
    s32 y;
    s32 y2;
    s32 x;
    FldRes* palette3;
    void* tiles3;
    AnimState anim;
    FldRes* palette;
    void* tiles;
    s32 x2;
    FldRes* palette4;
    TextSlot textSlots[0x24];
    u8 textSlotCount;
    u8 unk_165[0x03];
    FldRes* palette5;
    void* tiles4;
    s32 x3;
    s32 y3;
    s32 playerStartX;
    s32 playerStartY;
    FldRes* palette6;
    void* tiles5;
    FldRes* palette7;
    void* tiles6;
    FldRes* palette8;
#ifdef VERSION_EU
    TextSlot textSlots2[0x36];
#else
    TextSlot textSlots2[0x1B];
#endif
    u8 textSlotCount2;
    u8 unk_26D[0x03];
    TextSlot textSlots3[0x06];
    u8 textSlotCount3;
    u8 unk_2A1[0x03];
    TextSlot textSlots4[0x09];
    u8 textSlotCount4;
    u8 unk_2ED[0x03];
    s32 (*update)(struct MapSaveWork*);
    u8 confirmCursor;
    u8 unk_2F5;
    u16 steps;
    u8 dialogVisible;
    u8 unk_2F9[0x03];
    TaskPool tasks;
} MapSaveWork;

typedef struct MapAnmEntry {
    void* script;
    void* tiles;
    u16 frameSize;
    u8 tileOffset;
    u8 unk_0B;
} MapAnmEntry;

typedef struct MapAnmWork {
    MapAnmSlot slots[8];
} MapAnmWork;

typedef struct MapDbgWork {
    u8 visible;
    u8 unk_01[0x03];
    u8* editing;
    void (*update)(struct MapDbgWork*);
    u8 seedCursor;
    u8 codeCursor;
    u8 unk_0E[0x02];
    void* tiles;
    void* palette;
    u16 seedText[0x0A];
    u8 seedTextLength;
    u8 unk_2D;
    u16 codeText[0x0A];
    u8 codeTextLength;
    u8 unk_43;
    u16 cursorText;
    u8 cursorTextLength;
    u8 unk_47;
} MapDbgWork;

typedef struct MapGmkEnmWork {
    FldObj obj;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    u8 (*update)(struct MapGmkEnmWork*);
    u8 flipX;
    u8 unk_069[0x03];
    s32 targetZ;
    u16 timer;
    u8 unk_072[0x02];
} MapGmkEnmWork;

typedef struct MapGmkDmyWork {
    void* tiles;
} MapGmkDmyWork;

typedef struct MapGmkJumpWork {
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    u8 unk_0BC[0x04];
    u8 state;
    u8 unk_0C1[0x03];
    s32 jumpHeight;
    void (*update)(struct MapGmkJumpWork*);
} MapGmkJumpWork;

typedef struct MapGmkTutorialWork {
    FldObj obj;
    Collider collider;
    void* tiles;
    ObjPalette* palette;
    u8 unk_0A4[0x04];
    u8 opened;
    u8 unk_0A9[0x03];
    u8 (*update)(struct MapGmkTutorialWork*);
    TaskPool tasks;
} MapGmkTutorialWork;

typedef struct MapGmk01Work {
    MapGmkPlacement* placement;
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    u16 timer;
    u16 unk_0C6;
    u8 (*update)(struct MapGmk01Work*);
} MapGmk01Work;

typedef struct MapGmkSpiderWork {
    FldObj obj;
    u8 unk_040[0x5C];
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    u8 (*update)(struct MapGmkSpiderWork*);
    u8 flipX;
    u8 unk_0C5[0x03];
} MapGmkSpiderWork;

typedef struct MapGmkGpWork {
    MapGmkPlacement* placement;
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    u16 hitSong;
    u16 timer;
    u8 (*update)(struct MapGmkGpWork*);
} MapGmkGpWork;

typedef struct MapGmkGp1Work {
    MapGmkPlacement* placement;
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    u16 hitSong;
    u8 visible;
    u8 unk_0C7;
    u8 (*update)(struct MapGmkGp1Work*);
} MapGmkGp1Work;

typedef struct MapGmk00Work {
    MapGmkPlacement* placement;
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    u16 radius;
    u16 unk_0C6;
    u8 visible;
    u8 stoodOn;
    u8 unk_0CA[0x02];
} MapGmk00Work;

typedef struct MapGmkBarrelWork {
    MapGmkPlacement* placement;
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    u8 visible;
    u8 unk_0C5[0x03];
    u8 (*update)(struct MapGmkBarrelWork*);
} MapGmkBarrelWork;

typedef struct MapPrizeWork {
    s32 x;
    s32 y;
    s32 z;
    s32 ground;
    Collider collider;
    void* tiles;
    ObjPalette* palette;
    u8* gfx;
    u8* gfx2;
    void (*update)(struct MapPrizeWork*);
    u16 kind;
    u16 timer;
    s32 vz;
    s32 speed;
    u8 angle;
    u8 angleStep;
    u8 unk_8E[0x02];
    s32 scale;
    u16 amount;
    u8 visible;
    u8 collected;
} MapPrizeWork;

typedef struct MapGmkGp07Work {
    MapGmkPlacement* placement;
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    s32 (*update)(struct MapGmkGp07Work*);
} MapGmkGp07Work;

typedef struct MapGmkGp08Work {
    MapGmkPlacement* placement;
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    void* gfx2;
    u16 hitSong;
    u8 overlayVisible;
    u8 unk_0CB;
    s32 (*update)(struct MapGmkGp08Work*);
} MapGmkGp08Work;

typedef struct MapGmkGp09Work {
    MapGmkPlacement* placement;
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    void* gfx2;
    u8 overlayVisible;
    u8 unk_0C9;
    u8 unk_0CA;
    u8 unk_0CB;
    s32 (*update)(struct MapGmkGp09Work*);
} MapGmkGp09Work;

typedef struct MapGmk04Work {
    MapGmkPlacement* placement;
    FldObj obj;
    Collider collider;
    ObjPalette* palette;
    void* tiles;
    AnimState anim;
    void* gfx;
    void (*update)(struct MapGmk04Work*);
    TaskPool tasks;
} MapGmk04Work;

typedef struct MapGmk05Work {
    u8 unk_000[0x04];
    FldObj obj;
    Collider collider;
    ObjPalette* palette;
    void* tiles;
    AnimState anim;
    void* gfx;
    void (*update)(struct MapGmk05Work*);
    u8 targeted;
    u8 unk_0C9[0x03];
    TaskPool tasks;
    TaskPool tasks2;
} MapGmk05Work;

typedef struct MapGmk06Work {
    u8 unk_000[0x04];
    FldObj obj;
    Collider collider;
    AnimState anim;
    ObjPalette* palette;
    void* tiles;
    void* gfx;
    void (*update)(struct MapGmk06Work*);
    TaskPool tasks;
} MapGmk06Work;

typedef struct UnkStruct_08F70ACC {
    u8 unk_00[0x04];
    u8 value;
    u8 unk_05[0x09];
    u8 category;
    u8 unk_0F[0x09];
} UnkStruct_08F70ACC;

typedef struct MapPrzCardWork {
    s32 posX;
    s32 posY;
    s32 posZ;
    s32 ground;
    Collider collider;
    FldRes* palette;
    void* tiles;
    FldRes* palette2;
    void* tiles2;
    void* tiles3;
    void* tiles4;
    ObjPalette* palette3;
    u16 spriteFlags;
    u16 timer;
    void (*update)(struct MapPrzCardWork*);
    UnkStruct_08F70ACC stat;
    u16 cardId;
    u8 unk_0AA[0x02];
    s32 unk_0AC;
    s32 speed;
    s16 scaleX;
    s16 scaleY;
    u8 angle;
    u8 unk_0B9;
    s16 x;
    s16 y;
    u16 priority;
    s32 dirX;
    s32 dirY;
    s32 distance;
    s16 scale;
    u8 rotation;
    u8 phaseY;
    u8 phaseX;
    u8 worldPrize;
    u8 collected;
    u8 unk_0D3;
    TaskPool tasks;
} MapPrzCardWork;

typedef struct MapPrzStockWork {
    u16* stock;
    void (*update)(struct MapPrzStockWork*);
    TaskPool tasks;
} MapPrzStockWork;

typedef struct MapSparkWork {
    FldObj* obj;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    u8 unk_24[0x04];
} MapSparkWork;

typedef struct MapFaintWork {
    FldObj* obj;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    u8 unk_24[0x04];
} MapFaintWork;

typedef struct MapDmgWork {
    ObjPalette* palette;
    void* tiles;
    u8 visible;
    u8 enabled;
    u16 timer;
} MapDmgWork;

typedef struct MapTalkWork {
    FldObj* obj;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    u8 unk_24[0x04];
    u8 playerOnRight;
    u8 unk_29[0x03];
} MapTalkWork;

typedef struct MapDonaldWork {
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void (*update)(struct MapDonaldWork*);
    u8 targeted;
    u8 visible;
    u8 unk_0C2[0x02];
    TaskPool tasks;
    TaskPool tasks2;
} MapDonaldWork;

typedef struct MapGoofyWork {
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void (*update)(struct MapGoofyWork*);
    u8 targeted;
    u8 visible;
    u8 unk_0C2[0x02];
    TaskPool tasks;
    TaskPool tasks2;
} MapGoofyWork;

typedef struct MapNamineWork {
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void (*update)(struct MapNamineWork*);
    u8 registered;
    u8 targeted;
    u8 visible;
    u8 unk_0C3;
    u16 spriteFlags;
    u8 unk_0C6[0x02];
    TaskPool tasks;
    TaskPool tasks2;
} MapNamineWork;

typedef struct MapMickeyWork {
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void (*update)(struct MapMickeyWork*);
    u8 targeted;
    u8 visible;
    u16 spriteFlags;
    TaskPool tasks;
    TaskPool tasks2;
} MapMickeyWork;

typedef struct MapTutorialWork {
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void* gfx;
    u8 visible;
    u8 shadowVisible;
    u8 flip;
    u8 unk_0C3;
    void (*update)(struct MapTutorialWork*);
    TaskPool tasks;
    TaskPool tasks2;
} MapTutorialWork;

typedef struct MapNiserikuWork {
    FldObj obj;
    Collider collider;
    AnimState anim;
    void* tiles;
    ObjPalette* palette;
    void (*update)(struct MapNiserikuWork*);
    u8 registered;
    u8 targeted;
    u8 visible;
    u8 unk_0C3;
    TaskPool tasks;
    TaskPool tasks2;
} MapNiserikuWork;

typedef struct MapFloorWork {
    void* tiles;
    void* palette;
    void* gfx;
    s16 textX;
    u16 timer;
    u8 textSlotCount;
    u8 unk_11[0x03];
    void* palette2;
#ifdef VERSION_EU
    TextSlot textSlots[0x3C];
#else
    TextSlot textSlots[0x28];
#endif
} MapFloorWork;

typedef struct MapMsgWork {
    FldRes* palette;
    TextSlot textSlots[0x30];
    u8 textSlotCount;
    u8 unk_185;
    s16 textX;
    u16 timer;
    u8 unk_18A[0x02];
} MapMsgWork;

typedef struct MapStairWork {
    FldObj obj;
    ObjPalette* palette;
    void* tiles;
    u8 visible;
    u8 unk_49[0x03];
    void (*update)(struct MapStairWork*);
    TaskPool tasks;
} MapStairWork;

typedef struct MapFixWork {
    u8 colliderCount;
    u8 unk_01[0x03];
    Collider colliders[5];
    TaskPool tasks;
#ifdef VERSION_EU
    u8 bg3MapLoaded;
    u8 bg2MapLoaded;
    u8 bg1MapLoaded;
#endif
} MapFixWork;

typedef struct MapDoorGfx {
    void* palette;
    void* side0Closed;
    void* side0Open;
    void* side3Closed;
    void* side3Open;
    void* side2Closed;
    void* side2Open;
    void* side1Closed;
    void* side1Open;
} MapDoorGfx;

typedef struct MapDoorWork {
    MapDoor* door;
    FldObj obj;
    void* tiles;
    ObjPalette* palette;
    void* sprite;
    void* closedSrc;
    void* openSrc;
    void* tiles2;
    ObjPalette* palette2;
    void* sprite2;
    void* openSrc2;
    void* closedSrc2;
    u8 (*update)(struct MapDoorWork*);
    u8 visible;
    u8 triggered;
    u8 unk_72[0x02];
    TaskPool tasks;
} MapDoorWork;

extern u8 gMickeyFl00Tiles[];
extern u16 gMickeyPalette[];
extern u8 gEmy01L00Tiles[];
extern u16 gEmy01Palette[];
extern u8 gEmy0600Tiles[];
extern u16 gEmy06Palette[];
extern u8 gEmy2103Tiles[];
extern u8 gEmy00L09Tiles[];
extern u8 gUnk_08B1E974[];
extern u8 gUnk_08B1E97E[];
extern u8 gUnk_08B1E988[];
extern u8 gUnk_08B1E992[];
extern u8 gUnk_08B1E9A6[];
extern MapRoomState* gMapRoomState;

extern u16 gRikuPalette[];
extern u16 gSoraPalette[];
extern u8 gSor1ff00Tiles[];
extern u16 gDonaldPalette[];
extern u16 gGoofyPalette[];
extern const MapFormDef gUnk_0984D1F4[];
extern const MapGmkDef gWorldMapGmkDefs[];
extern const MapGmkDef gMapGmkBarrelDef;
extern const MapGmkDef gMapGmk01Def;
extern const MapGmkDef gMapGmk04Def;
extern const MapGmkDef gMapGmkDefs[];
extern const MapGmkDef gMapGmk05Def;
extern const MapGmkDef gMapGmk06Def;
extern const u8 gDonaldTalkMessages[28];
extern const u8 gGoofyTalkMessages[28];
extern u8 gUnk_0985ADAA[];
extern u8 gUnk_0985BDEA[];
extern EventKeyList gEventKeyLists[];
extern const u8 gSoraFloorEvents[13];
extern const u8 gRikuFloorEvents[13];
extern const u8 gCellMasks[][8];
extern const u8 gUnk_0984D134[][8];
extern const u8 gUnk_0984D314[][4];
extern const u8 gUnk_0984D32C[][4];
extern const u8 gUnk_0984D3F8[][4];
extern const u8 gUnk_09961A64[][320];
extern const u8 gUnk_09962BE4[][320];
extern const u8 gUnk_09963D64[][320];
extern const u8 gUnk_09964EE4[][320];
extern const u8 gUnk_099581A4[];
extern const u8 gUnk_09966064[];
extern u8 gUnk_050001C0[];
extern u8 gUnk_0815A03A[];
extern u8 gUnk_0815B5A6[];
extern u8 gNamiF00Tiles[];
extern u16 gNaminePalette[];
extern u8 gUnk_08159E1E[];
extern u16 gUnk_09991D24[];
extern u16 gGoofy2Palette[];
extern u16 gDonald2Palette[];
extern u16 gUnk_09991BE4[];
extern u16 gUnk_09991C04[];
extern u16 gUnk_09991D04[];
extern u8 gRikuFf00Tiles[];
extern u8 gUnk_08B1EA00[];
extern u16 gUnk_08F69BE4[];
extern u8 gUnk_098A4B68[];
extern u16 gCard00Palette[];
extern u16 gUnk_099910C4[];
extern u16 gUnk_09991984[];
extern u8 gGoofyFl00Tiles[];
extern u16 gUnk_09991924[];
extern u16 gUnk_09991944[];
extern u16 gUnk_09991964[];
extern u16 gUnk_099919A4[];
extern u8 gUnk_098A8628[];
extern u16 gUnk_09991C44[];
extern u16 gUnk_09991C84[];
extern u8 gUnk_08159DF0[];
extern u16 gUnk_099919C4[];
extern u16 gUnk_09991BC4[];
extern u8 gUnk_098A5C90[];
extern u8 gUnk_098A5C9A[];
extern u8 gUnk_098A5CA4[];
extern u8 gUnk_098A5CAE[];
extern u8 gUnk_098A5CB8[];
extern u8 gUnk_098A5CF4[];
extern u8 gDonaFl00Tiles[];
extern const MapDoorGfx gWorldMapDoorGfx[14];
extern u16 gUnk_09991284[];
extern u16 gUnk_09991104[];
extern u16 gUnk_09991124[];
extern u16 gUnk_09991144[];
extern u16 gUnk_09991164[];
extern u16 gUnk_09991184[];
extern u16 gUnk_099911A4[];
extern u16 gUnk_099911C4[];
extern u16 gUnk_099911E4[];
extern u16 gUnk_09991204[];
extern u16 gUnk_09991224[];
extern u16 gUnk_09991244[];
extern u16 gUnk_09991264[];

void MapMenuInitConfirm(MapMenuWork* w);
void LoadGameMenuSlideInX(LoadGameMenuWork* work);
void LoadGameMenuMoveCursor(LoadGameMenuWork* work);
void LoadGameMenuInput(LoadGameMenuWork* work);
void LoadGameMenuSlideOutX(LoadGameMenuWork* work);
void LoadGameMenuExit(LoadGameMenuWork* work);
void LoadGameMenuSlideOutY(LoadGameMenuWork* work);
void MapDrawBg1(s32 a, s32 b);
void MapFindLowestEdgeRightward(s32 a, s16* px, s16* py, s16* pz, s16 lo, s16 hi);
extern u8 gNiseFl00Tiles[];
extern u8 gNiserikuHizaFTiles[];
extern u8 gNiserikuDownFTiles[];
extern u16 gNiserikuPalette[];
void MapEnmInit(MapEnmWork* p, MapEnmArgs* q);

s32 LoadGameMenuLoadFile(u8 a);
s32 MapSaveSlideInX(MapSaveWork* w);
s32 MapMenuSlideInX(MapMenuWork* w);
s32 MapMenuSoraInput(MapMenuWork* w);
s32 MapMenuSlideInY(MapMenuWork* w);
s32 MapMenuRikuInput(MapMenuWork* w);
s32 MapMenuOpenSubMode(MapMenuWork* w);
s32 MapMenuSlideOutX(MapMenuWork* w);
s32 MapMenuConfirmInput(MapMenuWork* w);
s32 MapMenuSlideOutY(MapMenuWork* w);
s32 MapMenuResume(MapMenuWork* w);
s32 Task_MapMenu_1(MapMenuWork* w);
s32 MapMenuOpen(MapMenuWork* w);
s32 MapSaveSlideInY(MapSaveWork* w);
s32 MapSaveInput(MapSaveWork* w);
s32 MapSaveWaitClose(MapSaveWork* w);
s32 MapSaveSlideOutX(MapSaveWork* w);
s32 MapSaveSlideOutY(MapSaveWork* w);
void MapPlaceDoorsOnLastPlatform();
void NewGameSlotMenuSlideIn(NewGameSlotMenuWork* w);
void NewGameSlotMenuInput(NewGameSlotMenuWork* w);
void NewGameSlotMenuSlideOut(NewGameSlotMenuWork* w);
void NewGameSlotMenuExit(NewGameSlotMenuWork* w);
void MapMenuFreeConfirm(MapMenuWork* w);
s32 Task_MapSave_1(MapSaveWork* w);
void MapFreeRoom();
void MapDrawBgs(s16 x, s16 y);
u8 RequestTilemapStripCopy(void* a, void* b, u8 c, u8 d, u8 e);
void* GetSelectedMapCard();
u8 MapDoorWaitHit(MapDoorWork* p);
u8 MapDoorWaitCard(MapDoorWork* p);
u8 MapDoorWaitOpen(MapDoorWork* p);
void CreateWorldPrize(s32 x, s32 y, s32 z);
MapCell* MapGetCell(s16 x, s16 y);
#ifdef PLATFORM_ANDROID
MapCell* MapGetCellBus(s16 x, s16 y);
#else
/* The GBA reads the open bus through NULL itself. */
#define MapGetCellBus MapGetCell
#endif
void MapComputeRowBounds();
void MapCellSetBg3Piece(MapCell* p, s32 n);
void MapCellSetBg2CornerPiece(MapCell* p, s32 n);
void MapPlaceDoorOnPlatform(MapPlatform* p, s32 a);
void MapBuildBgColumn(u16* a, u16* b, u16* c, s16 d, s16 e);
void MapBuildBgRow(u16* a, u16* b, u16* c, s16 d, s16 e);
u8 MapPickFreeFloorPos(FldPos* a, s32* b);
u8 MapPickFreeFloorPosInView(FldPos* a, s32* b);
void MapFixCreateGimmicks(void* a);
MapCell* MapFixGetCell(s16 x, s16 y);
void MapEnmSetupArgs(MapEnmArgs* p, const MapEnmDef* q);
u8 MapEnmPlaceAtStairs(MapEnmArgs* w);
u8 MapEnmPlaceAboveGmk01(MapEnmArgs* w);
void MapEnmSpawnFixed(MapEnmArgs* w, u8 a, u8 b);
void MapEnmStartBattle(MapEnmWork* p);
const u8* GetCellMaskBlock(void* a, u16 b, u16 c);
void* GetCellMaskTable(u8 a);
u8 FldObjIsOutOfView(FldObj* p);
u16 MapGmkGetFreeTiles();
void CreateRandomMapPrizes(s32 a, s32 b, s32 c);
void MapFixLoadCellTypes(const u8* src);
void MapFixSnapCamera();
void MapFixInitCells(MapFixedDef* p);
void MapApplyLayer1DecorRule(MapDecorRule* p);
u8 MapPatternFits(s16 x, s16 y, const MapCellPattern* p);
void MapPlaceLayer1DecorPiece(s16 x, s16 y, const u8* p, u16* base);
void MapPlaceLayer2DecorPiece(s16 x, s16 y, const u8* p, u16* base);
void MapApplyLayer2DecorRule(MapDecorRule* p);
const UnkStruct_080E8E24* PickRandomPrzCard(u8 a);
void NewGameSlotMenuLoadFloorTiles(u8 a, u8 b, u8 c);
void NewGameSlotMenuLoadLevelTiles(u8 a, u16 v);
void NewGameSlotMenuLoadTimeTiles(u8 a, u32 b);

s32 FieldGroundAt(s32 x, s32 y, s32 z);
s32 GetFldPosFloor(FldPos* p);
void FldPosPlaceAtCell(FldPos* p, s16 x, s16 y, u8 a, u8 b);
void FldPosPlaceOnFreeFloor(FldPos* p);
s32 MapClampCameraX(s32 x);
s32 MapClampCameraY(s32 y);
void MapSnapCamera();
void InitFieldState();
void SpawnMapPlayer();
u8 IsHitByMapAttack(FldPos* p, s16 a, s16 b);
u8 GetCurrentRoomCardValue();
s32 IsMapInterrupted();
s32 IsFldObjTalkTarget(FldObj* obj);
void CreateMapRndTask();
void UpdateMapField();
void DrawMapField();
void MenuMsgWaitMessage(MenuMsgWork* w);
void MenuMsgWaitFade(MenuMsgWork* w);
void Mode_MenuMsg_0(s32 arg);
void StartFieldTransition();
void DestroyMapField();
void MapCellSetType(MapCell* p, s32 a, s32 b);
u8 FldPosHeightExceeds(FldPos* p, u16 a);
u8 GetRandomPieceVariant(u8 a);
void MapCellSetBg2Piece(MapCell* p, u8 n, u8 v);
void MapCellSetFloorBg3Piece(MapCell* p);
void MapCellSetBg2EdgePiece(MapCell* p, s32 n);
void MapCellSetBg2PieceVariant(MapCell* p, s32 n, u8 v);
void MapSetCornerCellPieces(s16 x, s16 y, s32 a, s32 b);
s16 MapOutlineNextRowRightToLeft(u8 a, u8 b, s16 c);
u8 MapCellHasType(s16 x, s16 y, u8 n);
s16 MapOutlineNextRowLeftToRight(u8 a, u8 b, s16 c);
void MapCellSetBg1Piece(s16 x, s16 y, u8 n);
u8 MapCellIsUnbounded(s16 x, s16 y);
void MapAssignWallTopPieces(s16 x, s16 y);
void MapAssignLedgePieces(s16 x, s16 y);
void MapAssignLeftBorderPiece(s16 y);
void MapAssignRightBorderPiece(s16 y);
void MapAssignBg1Pieces();
u8 MapPlaceFirstPlatformDoor(u8 a);
s32 MapPlaceLastPlatformDoor();
void MapPlaceRightPlatformDoor(u8 a);
void MapPlaceLeftPlatformDoor(u8 a);
s32 PickRandomTopEdgeType(s16 a, s16 b, s16 c);
s32 PickRandomBottomEdgeType(s16 a, s16 b, s16 c);
s32 GetMatchingBottomEdgeType(s16 x, s16 y);
s32 PickEdgeTypeByHalf(s16 a, s16 b, s16 c, u8 d);
s32 PickEdgeTypeByThird(s16 a, s16 b, s16 c, u8 d);
s32 GetTopEdgeTypeBelowLedge(u8 d, s16 x, s16 y);
u8 MapFindSpanBelowPlatforms(s16* a, s16* b, s16* c, s16* d);
void MapSetPlatform(u8 i, u16 a, u16 b, s16 c);
void MapTracePlatformLeftToRight(u8 i, s16 a, s16 b, s16 c, u8 e);
void MapTracePlatformOutward(u8 i, s16 a, s16 b, s16 c, s16 d, u8 e);
void MapTracePlatformRightToLeft(u8 i, s16 a, s16 b, s16 c, u8 e);
void MapGenerateLayout1();
void MapDrawBgColumn(void* p, s16 a, s16 b);
void MapDrawBgRow(void* p, s16 a, s16 b);
MapPlatform* GetMapPlatform(u8 a);
void* GetMapBgBuffer();
u16 GetRandomMapWidth();
void MapGenerateRoom(u16 a, u16 b);
void MapEnmPlaceInView(MapEnmArgs* p);
void MapEnmPlaceInViewAbove(MapEnmArgs* p);
s32 MapEnmPlaceInRoom(MapEnmArgs* p);
void MapEnmApplyRoomFlags(MapEnmWork* p);
void MapEnmSetAnim(MapEnmWork* p, u8 n, u16 a);
void MapEnmUpdateAnim(MapEnmWork* p);
u8 GetRandomBattleId();
void MapEnmCheckContact(MapEnmWork* p);
s32 MapEnmCheckAttacked(MapEnmWork* p);
void MapEnmSaveToCache(MapEnmWork* p);
void MapEnmRestoreFromCache();
void MapEnmSpawnRoomSet();
void MapEnmInitRoom();
void MapEnmUpdateSpawner();
MapCell* MapCellAtPos(s32 x, s32 y);
u8 MapCellIsFreeOfType(s16 x, s16 y, u8 n);
s32 MapAreaIsFreeOfType(s16 x, s16 y, u8 w, u8 h, u8 n);
s32 MapCellHeightExceeds(s16 a, s16 b, u8 c);
s32 MapWallFaceIsUnreserved(s16 x, s16 y, u16 n);
void MapEnmDestroy(MapEnmWork* p);
s32 MapGmkIsAreaSparse(s16 x, s16 y);
void MapReserveArea(s16 x, s16 y, u8 w, u8 h);
s16 MapRowsToWallBase(s16 x, s16 y);
void MapAnmSetupSlot(MapAnmSlot* p, const MapGmkDef* q);
void MapAnmStepScript(MapAnmSlot* p);
void MapAnmFlushSlot(MapAnmSlot* p);
void MapAnmUpdateSlot(MapAnmSlot* p);
s32 Task_MapAnm_1(MapAnmWork* w);
void MapAnmResetSlot(MapAnmSlot* p);
u8 MapAnmCmdFrame(MapAnmSlot* p);
void MapFixFreeCells();
void NewGameSlotMenuSelectSlot(u8 a);
u8 GetRandomMapGmkIndex(u8 a);
u8 MapGmkFindSpot(FldPos* a, u8 b);
s32 MapGmkIsPaletteUnused(void* a);
s32 MapGmkNeedsTiles(u8 flag, const void* a);
void MapGmkReserveJump();
void MapGmkPlaceGmk01();
void MapGmkPlaceGmk04();
void MapGmkPlaceMoogle();
void MapGmkPlaceWorldGimmicks();
void MapGmkPlaceRandomGimmicks();
void MapGmkInitRoom();
void MapGmkCreateTasks();
void DropMapGmkPrize(FldPos* p);
void MapGmkFree();
void MapApplyLayer1DecorRules(MapDecorRule* p);
u8 MapDecorCheckFits(s16 x, s16 y, const u8* p);
void MapApplyLayer2DecorRules(MapDecorRule* p);
void MapApplyRoomDecor();
u8 IsEventDoor(u8 a, u8 b);
u8 RollCardValue();
s32 CreateMapPrzCardTask(const UnkStruct_080E8E24* a, u8 b, s32 c, s32 d, s32 e);
u8 TryCreateRandomPrzCard(u8 a, s32 b, s32 c, s32 d);
void MapDbgSetUpdate(ModeFunc a);
void MapDbgSetUpdateAndRun(ModeFunc a);
void CreateMapPrizeTasks(u8 a, u8 b, s32 c, s32 d, s32 e);
void MapDbgFreeCameraInput();
void MapDbgMain();
void MapDbgExitRoom();
void MapDbgFreeCameraMode();
void MapDbgWaitEdit();
void MapDbgWaitMenu();
void MapDbgWaitRoomCreate();
void Mode_MapDbg_0();
void Mode_MapDbg_1();
void Mode_MapDbg_2();
void MapFldCreateWorldLogo();
void MapFldDestroyAllmapRoom();
void StartWorldBossBattle();
void MapFldShowWorldLogo();
void MapFldMain();
void MapFldExitRoom();
void MapFldOpenAllmap();
void MapFldWaitMenu();
void MapFldWaitRoomCreate();
void func_080E9F30();
void Mode_MapFld_0();
void Mode_MapFld_1();
void Mode_MapFld_2();
MapFixedDef* GetMapFixedDef();
void MapFixCreateCharaTasks();
u8 GetWorldEntryEventId();
u8 GetFloorEventId();
void MapFixMain();
void MapFixEnterMapFld();
void MapFixLeaveEntranceHall();
void MapFixLeaveExitHall();
void MapFixWaitMenu();
void MapFixWaitWalkOut();
void MapFixWaitRoomCreate();
void Mode_MapFix_0();
void MapFixWaitWorldEvent();
void Mode_MapFix_1();
void Mode_MapFix_2();
void MapFldStartBattle();
void MapFldSetUpdate(ModeFunc a);
void MapFldSetUpdateAndRun(ModeFunc a);
void MapFixSetUpdate(ModeFunc a);
void MapFixSetUpdateAndRun(ModeFunc a);
s32 NewGameSlotMenuShowSummary(u8 i);
void LoadGameMenuLoadFloorTiles(u8 a, u8 b, u8 c);
void LoadGameMenuLoadLevelTiles(u8 a, u16 b);
void LoadGameMenuLoadTimeTiles(u8 a, u32 v);
void MapMenuWriteDigits3(ObjTiles* p, u8 a, u16 v);
void MapMenuWriteDigits5(ObjTiles* p, u8 a, u32 v);
s32 Task_MapFix_1(MapFixWork* w);
void MapDoorShowOpen(MapDoorWork* p);
u8 MapDoorIdle(MapDoorWork* p);
void MapMenuSetPanelPalettesExcluded(MapMenuWork* p, u8 a);
void MapMenuSetCharaPalettesExcluded(MapMenuWork* p, u8 a);
void MapSaveSetPanelPalettesExcluded(MapSaveWork* p, u8 a);
void MapSaveSetCharaPalettesExcluded(MapSaveWork* p, u8 a);
void MapSaveLoadFloorTiles(u8 a);

void MapFindLowestEdgeLeftward(s32 a, s16* px, s16* py, s16* pz, s16 e, s16 f);
void MapEnmDraw(MapEnmWork* p);
void LoadGameMenuSelectSlot(u8 a);
void MapSaveShowSummary(MapSaveWork* w, u8 i);
void MapUpdateCamera(s32 a, s32 b);
void NewGameSlotMenuDraw();
void LoadGameMenuDraw();
void NewGameSlotMenuMoveCursor(NewGameSlotMenuWork* w);
void Mode_MenuNew_1();
void Mode_MenuLoad_1();
s32 Task_MapRnd_1(MapRndWork* w);
void MapDoorShowClosed(MapDoorWork* p);
void MapSaveLoadLevelTiles(u16 v);
void MapSaveLoadTimeTiles(u32 t);
void MapBuildStairs(u16 a, u16 b);
void MapMarkJumpSpot(MapPlatform* p);
void MapFindPlatformStairs(MapPlatform* p);
void MapPlacePlatformStairs();

#ifdef VERSION_EU
extern const u8 gUnkEu_09953BF0[];
extern const u8 gUnkEu_099543F0[];
extern const u8 gUnkEu_09954BF0[];
extern const u8 gUnkEu_09955250[][320];
extern const u8 gUnkEu_09959850[][320];
extern const u8 gUnkEu_0995A9D0[][320];
extern const u8 gUnkEu_0995BB50[][320];
extern const u8 gUnkEu_0995CCD0[][320];
extern const u8 gUnkEu_0995DE50[][320];
extern const u8 gUnkEu_0995EFD0[][320];
extern const u8 gUnkEu_09960150[][320];
extern const u8 gUnkEu_099612D0[][320];
extern const u8 gUnkEu_09962450[][320];
extern const u8 gUnkEu_099635D0[][320];
extern const u8 gUnkEu_09964750[][320];
extern const u8 gUnkEu_099658D0[][320];
extern const u8 gUnkEu_09966A50[][320];
extern const u8 gUnkEu_09967BD0[][320];
extern const u8 gUnkEu_09968D50[][320];
extern const u8 gUnkEu_09969ED0[][320];
extern const u8 gUnkEu_0996D130[];
extern const u8 gUnkEu_0996D930[];
extern const u8 gUnkEu_0996E130[];
extern const u8 gUnkEu_0996E930[];
#endif

#endif /* GUARD_MAP_H */

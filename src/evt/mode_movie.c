#include "mode_chkmov.h"
#include "registration_data.h"
#include "system_state.h"
#include "main.h"
#include "movie.h"
#include "movie_text.h"
#include "msg_api.h"
#include "mode.h"
#include "display.h"
#include "pallet.h"
#include "mode_movie.h"
#include "sprite_palettes.h"
#include "gba/io_reg.h"
#include "malloc.h"
#include "fade.h"
#include <stddef.h>
#include "gba/keys.h"
#include "gba/oam.h"
#include "engine.h"
#include "gba/defines.h"
#include "gba/macro.h"
#include "gba/syscall.h"
#include "intr.h"
#include "m4a.h"
#include "obj_api.h"
#include "sprite.h"
#include "types.h"
#include "util.h"
#include "movie_subtitle_text.h"
#include "movies.h"
#ifdef PLATFORM_ANDROID
#include "port.h"
#endif

static vu16 sMovieModeState;
static s32 sMovieId;
static u16 sUnk_02034940;
static volatile s16 sMovieFrame;
static volatile s16 sMovieSubIndex;
static volatile u16 sMovieSubCount;
static const MovieSub* volatile sMovieSubUpper;
static const MovieSub* volatile sMovieSubLower;
static const MovieSub* sMovieSubs;
static volatile s16 sMovieSubUpperTimer;
static volatile u16 sMovieSubUpperLength;
static volatile u16 sMovieFlags;
static volatile u16 sMovieSubUpperAlpha;
static volatile s16 sMovieSubLowerTimer;
static volatile u16 sMovieSubLowerLength;
static volatile u16 sMovieSubLowerAlpha;

#ifdef VERSION_US
#include "movie_subtitles_1.inc"

const MovieSub gMovieSubsOpening[3] = {
    { 673, 0, gMovieSubTextUs_0886A6D4, 1, 0, 45, 1, 0 },
    { 825, 0, gMovieSubTextUs_0886A712, 0, 0, 50, 1, 0 },
    { 825, 0, gMovieSubTextUs_0886A752, 1, 0, 50, 1, 0 },
};

const MovieSub gUnk_0886AB70[1] = {
    { 30, 20, gMovieSubTextUs_0886AB3A, 1, 0, 50, 1, 0 },
};

const MovieSub gUnk_0886AB80[1] = {
    { 30, 20, gMovieSubTextUs_0886AB3A, 1, 0, 50, 1, 0 },
};

const MovieSub gMovieSubsEnding[14] = {
    { 337, 0, gMovieSubTextUs_0886A78A, 1, 0, 8, 0, 0 },
    { 405, 0, gMovieSubTextUs_0886A798, 1, 0, 30, 0, 0 },
    { 450, 0, gMovieSubTextUs_0886A7B8, 1, 0, 10, 0, 0 },
    { 465, 0, gMovieSubTextUs_0886A7C8, 0, 0, 60, 0, 0 },
    { 465, 0, gMovieSubTextUs_0886A7E2, 1, 0, 60, 0, 0 },
    { 545, 0, gMovieSubTextUs_0886A806, 0, 0, 40, 0, 0 },
    { 545, 0, gMovieSubTextUs_0886A83A, 1, 0, 40, 0, 0 },
    { 605, 0, gMovieSubTextUs_0886A870, 1, 0, 25, 0, 0 },
    { 645, 0, gMovieSubTextUs_0886A8A0, 0, 0, 90, 0, 0 },
    { 645, 0, gMovieSubTextUs_0886A8E0, 1, 0, 90, 0, 0 },
    { 750, 0, gMovieSubTextUs_0886A91E, 0, 0, 50, 0, 0 },
    { 750, 0, gMovieSubTextUs_0886A960, 1, 0, 50, 0, 0 },
    { 810, 0, gMovieSubTextUs_0886A978, 0, 0, 45, 0, 0 },
    { 810, 0, gMovieSubTextUs_0886A988, 1, 0, 45, 0, 0 },
};

const MovieSub gMovieSubsRikuEnding[10] = {
    { 405, 0, gMovieSubTextUs_0886A9BA, 0, 0, 35, 1, 0 },
    { 405, 0, gMovieSubTextUs_0886A9E8, 1, 0, 35, 1, 0 },
    { 450, 0, gMovieSubTextUs_0886AA00, 1, 0, 30, 1, 0 },
    { 490, 0, gMovieSubTextUs_0886AA42, 1, 0, 30, 1, 0 },
    { 525, 0, gMovieSubTextUs_0886AA76, 1, 0, 25, 1, 0 },
    { 580, 0, gMovieSubTextUs_0886AA88, 1, 0, 28, 1, 0 },
    { 650, 0, gMovieSubTextUs_0886AAC0, 0, 0, 50, 1, 0 },
    { 650, 0, gMovieSubTextUs_0886AAF2, 1, 0, 50, 1, 0 },
    { 710, 0, gMovieSubTextUs_0886AB0E, 1, 0, 15, 1, 0 },
    { 755, 0, gMovieSubTextUs_0886AB16, 1, 0, 25, 1, 0 },
};
#endif

#ifdef VERSION_JP
const MovieSub gMovieSubsOpening[3] = {
    { 673, 35, gMovieSubTextJp_0885DFE4, 1, 0, 45, 1, 0 },
    { 825, 50, gMovieSubTextJp_0885DFC4, 1, 0, 43, 1, 0 },
    { 872, 60, gMovieSubTextJp_0885DFA8, 1, 0, 45, 1, 0 },
};

#include "movie_subtitles_1.inc"

const MovieSub gUnk_0886AB70[1] = {
    { 30, 20, gMovieSubTextJp_0885E018, 1, 0, 50, 1, 0 },
};

#include "movie_subtitles_2.inc"

const MovieSub gUnk_0886AB80[1] = {
    { 30, 20, gMovieSubTextJp_0885E018, 1, 0, 50, 1, 0 },
};

const MovieSub gMovieSubsEnding[12] = {
    { 337, 100, gMovieSubTextJp_0885E1CC, 1, 0, 8, 0, 0 },
    { 405, 95, gMovieSubTextJp_0885E1C0, 1, 0, 30, 0, 0 },
    { 450, 95, gMovieSubTextJp_0885E1B4, 1, 0, 10, 0, 0 },
    { 465, 60, gMovieSubTextJp_0885E1AC, 0, 0, 60, 0, 0 },
    { 465, 60, gMovieSubTextJp_0885E190, 1, 0, 60, 0, 0 },
    { 545, 40, gMovieSubTextJp_0885E16C, 1, 0, 40, 0, 0 },
    { 605, 85, gMovieSubTextJp_0885E15C, 1, 0, 25, 0, 0 },
    { 645, 50, gMovieSubTextJp_0885E144, 0, 0, 90, 0, 0 },
    { 645, 50, gMovieSubTextJp_0885E124, 1, 0, 90, 0, 0 },
    { 750, 60, gMovieSubTextJp_0885E108, 1, 0, 50, 0, 0 },
    { 810, 75, gMovieSubTextJp_0885E100, 0, 0, 45, 0, 0 },
    { 810, 75, gMovieSubTextJp_0885E0EC, 1, 0, 45, 0, 0 },
};

#include "movie_subtitles_3.inc"

const MovieSub gMovieSubsRikuEnding[8] = {
    { 405, 60, gMovieSubTextJp_0885E2DC, 1, 0, 35, 1, 0 },
    { 450, 85, gMovieSubTextJp_0885E2CC, 1, 0, 30, 1, 0 },
    { 490, 95, gMovieSubTextJp_0885E2C0, 1, 0, 30, 1, 0 },
    { 525, 80, gMovieSubTextJp_0885E2AC, 1, 0, 25, 1, 0 },
    { 580, 85, gMovieSubTextJp_0885E29C, 1, 0, 28, 1, 0 },
    { 650, 40, gMovieSubTextJp_0885E278, 1, 0, 50, 1, 0 },
    { 710, 90, gMovieSubTextJp_0885E268, 1, 0, 15, 1, 0 },
    { 755, 90, gMovieSubTextJp_0885E258, 1, 0, 25, 1, 0 },
};

#include "movie_subtitles_4.inc"
#endif

#ifdef VERSION_EU
#include "movie_subtitles_1.inc"

const MovieSub gMovieSubsOpeningEn[3] = {
    { 673, 0, gMovieSubTextEu_0883DE0C, 1, 0, 45, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883DE2B, 0, 0, 50, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883DE4B, 1, 0, 50, 1, 0 },
};

const MovieSub gMovieSubsEndingEn[14] = {
    { 337, 0, gMovieSubTextEu_0883DE67, 1, 0, 8, 0, 0 },
    { 405, 0, gMovieSubTextEu_0883DE6E, 1, 0, 30, 0, 0 },
    { 450, 0, gMovieSubTextEu_0883DE7E, 1, 0, 10, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883DE86, 0, 0, 60, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883DE93, 1, 0, 60, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883DEA5, 0, 0, 40, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883DEBF, 1, 0, 40, 0, 0 },
    { 605, 0, gMovieSubTextEu_0883DEDA, 1, 0, 25, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883DEF2, 0, 0, 90, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883DF12, 1, 0, 90, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883DF31, 0, 0, 50, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883DF52, 1, 0, 50, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883DF5E, 0, 0, 45, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883DF66, 1, 0, 45, 0, 0 },
};

const MovieSub gMovieSubsRikuEndingEn[10] = {
    { 405, 0, gMovieSubTextEu_0883DF7F, 0, 0, 35, 1, 0 },
    { 405, 0, gMovieSubTextEu_0883DF96, 1, 0, 35, 1, 0 },
    { 450, 0, gMovieSubTextEu_0883DFA2, 1, 0, 30, 1, 0 },
    { 490, 0, gMovieSubTextEu_0883DFC3, 1, 0, 30, 1, 0 },
    { 525, 0, gMovieSubTextEu_0883DFDD, 1, 0, 25, 1, 0 },
    { 580, 0, gMovieSubTextEu_0883DFE6, 1, 0, 28, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883E002, 0, 0, 50, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883E01B, 1, 0, 50, 1, 0 },
    { 710, 0, gMovieSubTextEu_0883E029, 1, 0, 15, 1, 0 },
    { 755, 0, gMovieSubTextEu_0883E02D, 1, 0, 25, 1, 0 },
};

#include "movie_subtitles_2.inc"

const MovieSub gMovieSubsOpeningFr[4] = {
    { 673, 0, gMovieSubTextEu_0883E1F0, 0, 0, 45, 1, 0 },
    { 673, 0, gMovieSubTextEu_0883E205, 1, 0, 45, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883E21B, 0, 0, 50, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883E23A, 1, 0, 50, 1, 0 },
};

const MovieSub gMovieSubsEndingFr[14] = {
    { 337, 0, gMovieSubTextEu_0883E255, 1, 0, 8, 0, 0 },
    { 405, 0, gMovieSubTextEu_0883E25D, 1, 0, 30, 0, 0 },
    { 450, 0, gMovieSubTextEu_0883E26C, 1, 0, 10, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883E275, 0, 0, 60, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883E287, 1, 0, 60, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883E297, 0, 0, 40, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883E2B4, 1, 0, 40, 0, 0 },
    { 605, 0, gMovieSubTextEu_0883E2CC, 1, 0, 25, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883E2EB, 0, 0, 90, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883E303, 1, 0, 90, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883E311, 0, 0, 50, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883E32B, 1, 0, 50, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883E340, 0, 0, 45, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883E354, 1, 0, 45, 0, 0 },
};

const MovieSub gMovieSubsRikuEndingFr[12] = {
    { 405, 0, gMovieSubTextEu_0883E36D, 0, 0, 35, 1, 0 },
    { 405, 0, gMovieSubTextEu_0883E385, 1, 0, 35, 1, 0 },
    { 450, 0, gMovieSubTextEu_0883E392, 0, 0, 30, 1, 0 },
    { 450, 0, gMovieSubTextEu_0883E3A9, 1, 0, 30, 1, 0 },
    { 490, 0, gMovieSubTextEu_0883E3B5, 1, 0, 30, 1, 0 },
    { 525, 0, gMovieSubTextEu_0883E3CF, 1, 0, 25, 1, 0 },
    { 580, 0, gMovieSubTextEu_0883E3E4, 0, 0, 28, 1, 0 },
    { 580, 0, gMovieSubTextEu_0883E3FE, 1, 0, 28, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883E409, 0, 0, 50, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883E41F, 1, 0, 50, 1, 0 },
    { 710, 0, gMovieSubTextEu_0883E434, 1, 0, 15, 1, 0 },
    { 755, 0, gMovieSubTextEu_0883E439, 1, 0, 25, 1, 0 },
};

#include "movie_subtitles_3.inc"

const MovieSub gMovieSubsOpeningDe[4] = {
    { 673, 0, gMovieSubTextEu_0883E634, 0, 0, 45, 1, 0 },
    { 673, 0, gMovieSubTextEu_0883E649, 1, 0, 45, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883E65B, 0, 0, 50, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883E676, 1, 0, 50, 1, 0 },
};

const MovieSub gMovieSubsEndingDe[14] = {
    { 337, 0, gMovieSubTextEu_0883E69A, 1, 0, 8, 0, 0 },
    { 405, 0, gMovieSubTextEu_0883E6A1, 1, 0, 30, 0, 0 },
    { 450, 0, gMovieSubTextEu_0883E6B5, 1, 0, 10, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883E6BD, 0, 0, 60, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883E6CA, 1, 0, 60, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883E6E5, 0, 0, 40, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883E70B, 1, 0, 40, 0, 0 },
    { 605, 0, gMovieSubTextEu_0883E729, 1, 0, 25, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883E746, 0, 0, 90, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883E767, 1, 0, 90, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883E784, 0, 0, 50, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883E7A5, 1, 0, 50, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883E7C0, 0, 0, 45, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883E7CA, 1, 0, 45, 0, 0 },
};

const MovieSub gMovieSubsRikuEndingDe[11] = {
    { 405, 0, gMovieSubTextEu_0883E7EA, 0, 0, 35, 1, 0 },
    { 405, 0, gMovieSubTextEu_0883E802, 1, 0, 35, 1, 0 },
    { 450, 0, gMovieSubTextEu_0883E815, 1, 0, 30, 1, 0 },
    { 490, 0, gMovieSubTextEu_0883E832, 1, 0, 30, 1, 0 },
    { 525, 0, gMovieSubTextEu_0883E852, 1, 0, 25, 1, 0 },
    { 580, 0, gMovieSubTextEu_0883E85E, 1, 0, 28, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883E877, 0, 0, 50, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883E891, 1, 0, 50, 1, 0 },
    { 710, 0, gMovieSubTextEu_0883E8AC, 1, 0, 15, 1, 0 },
    { 755, 0, gMovieSubTextEu_0883E8B2, 0, 0, 25, 1, 0 },
    { 755, 0, gMovieSubTextEu_0883E8C1, 1, 0, 25, 1, 0 },
};

#include "movie_subtitles_4.inc"

const MovieSub gMovieSubsOpeningIt[4] = {
    { 673, 0, gMovieSubTextEu_0883EAA4, 0, 0, 45, 1, 0 },
    { 673, 0, gMovieSubTextEu_0883EAC1, 1, 0, 45, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883EAD7, 0, 0, 50, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883EAF2, 1, 0, 50, 1, 0 },
};

const MovieSub gMovieSubsEndingIt[14] = {
    { 337, 0, gMovieSubTextEu_0883EB0A, 1, 0, 8, 0, 0 },
    { 405, 0, gMovieSubTextEu_0883EB11, 1, 0, 30, 0, 0 },
    { 450, 0, gMovieSubTextEu_0883EB27, 1, 0, 10, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883EB2F, 0, 0, 60, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883EB42, 1, 0, 60, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883EB56, 0, 0, 40, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883EB6B, 1, 0, 40, 0, 0 },
    { 605, 0, gMovieSubTextEu_0883EB85, 1, 0, 25, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883EBA2, 0, 0, 90, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883EBBC, 1, 0, 90, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883EBDA, 0, 0, 50, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883EBF0, 1, 0, 50, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883EC05, 0, 0, 45, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883EC0C, 1, 0, 45, 0, 0 },
};

const MovieSub gMovieSubsRikuEndingIt[10] = {
    { 405, 0, gMovieSubTextEu_0883EC29, 0, 0, 35, 1, 0 },
    { 405, 0, gMovieSubTextEu_0883EC36, 1, 0, 35, 1, 0 },
    { 450, 0, gMovieSubTextEu_0883EC48, 1, 0, 30, 1, 0 },
    { 490, 0, gMovieSubTextEu_0883EC67, 1, 0, 30, 1, 0 },
    { 525, 0, gMovieSubTextEu_0883EC7F, 1, 0, 25, 1, 0 },
    { 580, 0, gMovieSubTextEu_0883EC92, 1, 0, 28, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883ECAC, 0, 0, 50, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883ECBD, 1, 0, 50, 1, 0 },
    { 710, 0, gMovieSubTextEu_0883ECCD, 1, 0, 15, 1, 0 },
    { 755, 0, gMovieSubTextEu_0883ECD3, 1, 0, 25, 1, 0 },
};

#include "movie_subtitles_5.inc"

const MovieSub gMovieSubsOpeningEs[4] = {
    { 673, 0, gMovieSubTextEu_0883EEA8, 0, 0, 45, 1, 0 },
    { 673, 0, gMovieSubTextEu_0883EEC1, 1, 0, 45, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883EECC, 0, 0, 50, 1, 0 },
    { 825, 0, gMovieSubTextEu_0883EEE6, 1, 0, 50, 1, 0 },
};

const MovieSub gMovieSubsEndingEs[14] = {
    { 337, 0, gMovieSubTextEu_0883EEFF, 1, 0, 8, 0, 0 },
    { 405, 0, gMovieSubTextEu_0883EF07, 1, 0, 30, 0, 0 },
    { 450, 0, gMovieSubTextEu_0883EF19, 1, 0, 10, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883EF22, 0, 0, 60, 0, 0 },
    { 465, 0, gMovieSubTextEu_0883EF33, 1, 0, 60, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883EF43, 0, 0, 40, 0, 0 },
    { 545, 0, gMovieSubTextEu_0883EF5D, 1, 0, 40, 0, 0 },
    { 605, 0, gMovieSubTextEu_0883EF7A, 1, 0, 25, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883EF96, 0, 0, 90, 0, 0 },
    { 645, 0, gMovieSubTextEu_0883EFB2, 1, 0, 90, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883EFC8, 0, 0, 50, 0, 0 },
    { 750, 0, gMovieSubTextEu_0883EFE6, 1, 0, 50, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883EFFC, 0, 0, 45, 0, 0 },
    { 810, 0, gMovieSubTextEu_0883F003, 1, 0, 45, 0, 0 },
};

const MovieSub gMovieSubsRikuEndingEs[11] = {
    { 405, 0, gMovieSubTextEu_0883F01E, 0, 0, 35, 1, 0 },
    { 405, 0, gMovieSubTextEu_0883F029, 1, 0, 35, 1, 0 },
    { 450, 0, gMovieSubTextEu_0883F039, 1, 0, 30, 1, 0 },
    { 490, 0, gMovieSubTextEu_0883F05B, 1, 0, 30, 1, 0 },
    { 525, 0, gMovieSubTextEu_0883F075, 1, 0, 25, 1, 0 },
    { 580, 0, gMovieSubTextEu_0883F07E, 0, 0, 28, 1, 0 },
    { 580, 0, gMovieSubTextEu_0883F09C, 1, 0, 28, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883F0AC, 0, 0, 50, 1, 0 },
    { 650, 0, gMovieSubTextEu_0883F0C6, 1, 0, 50, 1, 0 },
    { 710, 0, gMovieSubTextEu_0883F0DA, 1, 0, 15, 1, 0 },
    { 755, 0, gMovieSubTextEu_0883F0DE, 1, 0, 25, 1, 0 },
};
#endif

static const u8 sMovieHeapName[] = "MOVIE";

void mode_movie_0(s32 a) {
    sMovieModeState = 0;
    sMovieId = a;
    sUnk_02034940 = 0;
    sMovieFrame = 0;
    sMovieSubIndex = 0;
    sMovieSubCount = 0;
    sMovieSubs = NULL;
    sMovieFlags = 0;
    sMovieSubUpperTimer = 0;
    sMovieSubUpperLength = 0;
    sMovieSubUpperAlpha = 0;
    sMovieSubUpper = NULL;
    sMovieSubLowerTimer = 0;
    sMovieSubLowerLength = 0;
    sMovieSubLowerAlpha = 0;
    sMovieSubLower = NULL;
}

#ifdef VERSION_JP
#define MOVIE_SUB_MAX_CHARS 24
#elif defined(VERSION_EU)
#define MOVIE_SUB_MAX_CHARS 48
#else
#define MOVIE_SUB_MAX_CHARS 40
#endif

#ifndef VERSION_JP
static u16 sMovieSubUpperWidths[MOVIE_SUB_MAX_CHARS];
static u16 sMovieSubLowerWidths[MOVIE_SUB_MAX_CHARS];
#endif

s32 HandleMovieFrame(s32 arg) {
    s32 i;
    u16 keys;

    keys = ~REG_KEYINPUT;

    if ((keys & SOFT_RESET_KEYS) == SOFT_RESET_KEYS) {
        sMovieFlags |= MOVIE_FLAG_SOFT_RESET;
        return 1;
    }

#ifdef PLATFORM_ANDROID
    /* The movie player already treats a non-zero callback result as a clean
     * end-of-playback request. Limit Start-to-skip to the opening movie and
     * suppress it until physical release so it cannot leak into the next mode. */
    if (sMovieId == 1 && (keys & START_BUTTON) != 0) {
        PortSuppressKeys(START_BUTTON);
        return 1;
    }
#endif

    if (sMovieSubs != NULL) {
        for (i = 0; i < 2; i++) {
            if (sMovieSubs[sMovieSubIndex].frame == sMovieFrame) {
                if (sMovieSubs[sMovieSubIndex].line == 0) {
                    const MovieSub* e;

                    sMovieSubUpper = e = &sMovieSubs[sMovieSubIndex];
                    sMovieFlags |= MOVIE_FLAG_UPPER_SUB_PENDING;
                    sMovieSubUpperTimer = e->duration;

                    if (sMovieSubIndex < sMovieSubCount - 1) {
                        sMovieSubIndex++;
                    }

                    sMovieSubUpperLength = CountNonSpaceChars(sMovieSubUpper->text);

                    if (sMovieSubUpperLength > MOVIE_SUB_MAX_CHARS) {
                        sMovieSubUpperLength = MOVIE_SUB_MAX_CHARS;
                    }
                } else {
                    const MovieSub* e;

                    sMovieSubLower = e = &sMovieSubs[sMovieSubIndex];
                    sMovieFlags |= MOVIE_FLAG_LOWER_SUB_PENDING;
                    sMovieSubLowerTimer = e->duration;

                    if (sMovieSubIndex < sMovieSubCount - 1) {
                        sMovieSubIndex++;
                    }

                    sMovieSubLowerLength = CountNonSpaceChars(e->text);

                    if (sMovieSubLowerLength > MOVIE_SUB_MAX_CHARS) {
                        sMovieSubLowerLength = MOVIE_SUB_MAX_CHARS;
                    }
                }
            }
        }

        if (sMovieSubUpperTimer > 0) {
            sMovieSubUpperTimer--;
        }

        if (sMovieSubLowerTimer > 0) {
            sMovieSubLowerTimer--;
        }
    }

    sMovieFrame++;
    return 0;
}

void MovieVBlankIntr() {
    u16* oam;
#ifndef VERSION_JP
    s16 x;
    s16 y;
#endif
    u16 i;
    u32 attr0;
    u32 attr1;

    if (sMovieFlags & MOVIE_FLAG_PLAYING) {
        REG_DISPCNT = (DISPCNT_MODE_3 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON);
        MovieUpdate();

        if (sMovieSubs != NULL) {
            if (sMovieFlags & MOVIE_FLAG_UPPER_SUB_PENDING) {
                sMovieFlags &= ~MOVIE_FLAG_UPPER_SUB_PENDING;
                sMovieSubUpperAlpha = 0;
#ifdef VERSION_JP
                CopySjisGlyphsToVram(sMovieSubUpper->text);
#else
                CopyLatinGlyphsToVram(sMovieSubUpper->text, sMovieSubUpperWidths, 0);
#endif
            }

            if (sMovieFlags & MOVIE_FLAG_LOWER_SUB_PENDING) {
                sMovieFlags &= ~MOVIE_FLAG_LOWER_SUB_PENDING;
                sMovieSubLowerAlpha = 0;
#ifdef VERSION_JP
                CopySjisGlyphsToVramAt(sMovieSubLower->text, 0x100);
#else
                CopyLatinGlyphsToVram(sMovieSubLower->text, sMovieSubLowerWidths, 0x100);
#endif
            }

            if (sMovieSubUpperTimer > 0 || sMovieSubUpperAlpha != 0 ||
                sMovieSubLowerTimer > 0 || sMovieSubLowerAlpha != 0) {
                REG_DISPCNT |= DISPCNT_OBJ_ON;
                REG_BLDCNT = (BLDCNT_TGT1_OBJ | BLDCNT_TGT2_BG0 | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3);

                if (sMovieSubUpperAlpha < 16) {
                    if (sMovieSubUpperAlpha == 0) {
                        attr0 = OAM_DISABLE;
                    } else {
                        REG_BLDALPHA = ((16 - sMovieSubUpperAlpha) << 8) | sMovieSubUpperAlpha;
                        attr0 = OAM_BLEND;
                    }
                } else {
                    attr0 = 0;
                }

                if (sMovieSubLowerAlpha < 16) {
                    if (sMovieSubLowerAlpha == 0) {
                        attr1 = OAM_DISABLE;
                    } else {
                        REG_BLDALPHA = ((16 - sMovieSubLowerAlpha) << 8) | sMovieSubLowerAlpha;
                        attr1 = OAM_BLEND;
                    }
                } else {
                    attr1 = 0;
                }

                oam = (u16*)OAM;

#ifndef VERSION_JP
                x = 0;
                y = 0;

                if (sMovieSubUpperLength != 0) {
                    x = sMovieSubUpper->x;
                    x += GetCenteredTextX(sMovieSubUpperWidths, sMovieSubUpperLength);
                    y = 0x74;
                }
#endif

                for (i = 0; i < sMovieSubUpperLength; i++) {
                    u16 tile;
                    u16 palette;
#ifdef VERSION_JP
                    const MovieSub* sub;
                    s16 x;
#endif

                    tile = i * 4;
#ifdef VERSION_JP
                    sub = sMovieSubUpper;
                    x = sub->x;
                    palette = (sub->palette & 15) << 12;
                    oam[0] = attr0 | 0x74;
                    oam[1] = (x + i * 10) | 0x4000;
#else
                    palette = (sMovieSubUpper->palette & 15) << 12;
                    oam[0] = attr0 | y;
                    oam[1] = x | 0x4000;
#endif
                    oam[2] = palette | (tile + 0x200);
                    oam += 4;
#ifndef VERSION_JP
                    x += sMovieSubUpperWidths[i];
#endif
                }

#ifndef VERSION_JP
                if (sMovieSubLowerLength != 0) {
                    x = sMovieSubLower->x;
                    x += GetCenteredTextX(sMovieSubLowerWidths, sMovieSubLowerLength);
                    y = 0x84;
                }
#endif

                for (i = 0; i < sMovieSubLowerLength; i++) {
                    u16 tile;
                    u16 palette;
#ifdef VERSION_JP
                    const MovieSub* sub;
                    s16 x;
#endif

                    tile = i * 4;
#ifdef VERSION_JP
                    sub = sMovieSubLower;
                    x = sub->x;
                    palette = (sub->palette & 15) << 12;
                    oam[0] = attr1 | 0x84;
                    oam[1] = (x + i * 10) | 0x4000;
#else
                    palette = (sMovieSubLower->palette & 15) << 12;
                    oam[0] = attr1 | y;
                    oam[1] = x | 0x4000;
#endif
                    oam[2] = palette | (tile + 0x300);
                    oam += 4;
#ifndef VERSION_JP
                    x += sMovieSubLowerWidths[i];
#endif
                }

                for (i = sMovieSubUpperLength + sMovieSubLowerLength;
#ifdef VERSION_JP
                     i < 48;
#elif defined(VERSION_EU)
                     i < 96;
#else
                     i < 80;
#endif
                     i++) {
                    oam[0] = OAM_DISABLE;
                    oam += 4;
                }

                if (sMovieSubUpperTimer > 0) {
                    if (sMovieSubUpperAlpha < 16) {
                        sMovieSubUpperAlpha += 4;
                    }
                } else if (sMovieSubUpperAlpha != 0) {
                    sMovieSubUpperAlpha -= 4;

                    if (sMovieSubUpperAlpha == 0) {
                        sMovieSubUpperLength = 0;
                    }
                }

                if (sMovieSubLowerTimer > 0) {
                    if (sMovieSubLowerAlpha < 16) {
                        sMovieSubLowerAlpha += 4;
                    }
                } else if (sMovieSubLowerAlpha != 0) {
                    sMovieSubLowerAlpha -= 4;

                    if (sMovieSubLowerAlpha == 0) {
                        sMovieSubLowerLength = 0;
                    }
                }
            } else {
                REG_DISPCNT &= ~DISPCNT_OBJ_ON;
            }
        }
    }

    gIntrCheck |= INTR_FLAG_VBLANK;
}

void mode_movie_1() {
    void* p;

    switch (sMovieModeState) {
    case 0: {
        InitDisplayRegs();
        gDispCnt &= ~(DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON);
        CpuFill32(0, (void*)VRAM, VRAM_SIZE);
        sMovieModeState++;
        break;
    }
    case 1:
        sMovieModeState++;
        break;
    case 2:
        m4aSoundVSyncOff();
        gVBlankHandlerOverride = MovieVBlankIntr;
        IwramHeapInit(GetIwramHeapStart(), GetIwramHeapSize());
        EwramHeapInit(GetEwramHeapStart(), GetEwramHeapSize());
        SetEwramHeapName(sMovieHeapName);
        SetIwramHeapName(sMovieHeapName);
        CpuCopy16(gUnk_08F69C04, (void*)OBJ_PLTT, 32);
        CpuCopy16(gUnk_09614718, (void*)(OBJ_PLTT + PLTT_SIZE_4BPP), 32);
        MovieSetCallbacks(IwramAlloc, EwramAlloc, IwramFree, EwramFree);

        switch (sMovieId) {
        case 1:
            p = gUnk_0815C3EC;

#ifdef VERSION_EU
            switch (gLanguage) {
            case LANGUAGE_ENGLISH:
                sMovieSubs = gMovieSubsOpeningEn;
                sMovieSubCount = 3;
                break;
            case LANGUAGE_FRENCH:
                sMovieSubs = gMovieSubsOpeningFr;
                sMovieSubCount = 4;
                break;
            case LANGUAGE_GERMAN:
                sMovieSubs = gMovieSubsOpeningDe;
                sMovieSubCount = 4;
                break;
            case LANGUAGE_ITALIAN:
                sMovieSubs = gMovieSubsOpeningIt;
                sMovieSubCount = 4;
                break;
            case LANGUAGE_SPANISH:
            default:
                sMovieSubs = gMovieSubsOpeningEs;
                sMovieSubCount = 4;
                break;
            }
#else
            sMovieSubs = gMovieSubsOpening;
            sMovieSubCount = 3;
#endif
            break;
        case 2:
            p = gUnk_084E0F34;
            sMovieSubs = NULL;
            sMovieSubCount = 0;
            break;
        case 3:
            p = gUnk_084F4660;
            sMovieSubs = NULL;
            sMovieSubCount = 0;
            break;
        case 4:
            p = gUnk_0855CCB4;

#ifdef VERSION_EU
            switch (gLanguage) {
            case LANGUAGE_ENGLISH:
                sMovieSubs = gMovieSubsEndingEn;
                sMovieSubCount = 14;
                break;
            case LANGUAGE_FRENCH:
                sMovieSubs = gMovieSubsEndingFr;
                sMovieSubCount = 14;
                break;
            case LANGUAGE_GERMAN:
                sMovieSubs = gMovieSubsEndingDe;
                sMovieSubCount = 14;
                break;
            case LANGUAGE_ITALIAN:
                sMovieSubs = gMovieSubsEndingIt;
                sMovieSubCount = 14;
                break;
            case LANGUAGE_SPANISH:
            default:
                sMovieSubs = gMovieSubsEndingEs;
                sMovieSubCount = 14;
                break;
            }
#else
            sMovieSubs = gMovieSubsEnding;
#ifdef VERSION_JP
            sMovieSubCount = 12;
#else
            sMovieSubCount = 14;
#endif
#endif
            break;
#ifdef VERSION_EU
        default:
#endif
        case 5:
            p = gUnk_086FBA14;

#ifdef VERSION_EU
            switch (gLanguage) {
            case LANGUAGE_ENGLISH:
                sMovieSubs = gMovieSubsRikuEndingEn;
                sMovieSubCount = 10;
                break;
            case LANGUAGE_FRENCH:
                sMovieSubs = gMovieSubsRikuEndingFr;
                sMovieSubCount = 12;
                break;
            case LANGUAGE_GERMAN:
                sMovieSubs = gMovieSubsRikuEndingDe;
                sMovieSubCount = 11;
                break;
            case LANGUAGE_ITALIAN:
                sMovieSubs = gMovieSubsRikuEndingIt;
                sMovieSubCount = 10;
                break;
            case LANGUAGE_SPANISH:
            default:
                sMovieSubs = gMovieSubsRikuEndingEs;
                sMovieSubCount = 11;
                break;
            }
#else
            sMovieSubs = gMovieSubsRikuEnding;
#ifdef VERSION_JP
            sMovieSubCount = 8;
#else
            sMovieSubCount = 10;
#endif
#endif
            break;
#ifndef VERSION_EU
        default:
            p = gUnk_0855CCB4;
            sMovieSubs = gMovieSubsOpening;
            sMovieSubCount = 3;
            break;
#endif
        }

        if (MovieStart(p)) {
            sMovieFlags |= MOVIE_FLAG_PLAYING;
            MoviePlay(HandleMovieFrame, 0);
            sMovieFlags &= ~MOVIE_FLAG_PLAYING;
        }

        MovieClose();
        IwramHeapInit(GetIwramHeapStart(), GetIwramHeapSize());
        EwramHeapInit(GetEwramHeapStart(), GetEwramHeapSize());
        VTransInit();
        SpriteInit();
        BgInit();
        FadeInit();
        PalletInit();
        SioKeyInit();
        VTransReset();
        BgReset();
        SpriteReset();
        FadeReset();
        MosaicReset();
        InitDisplayRegs();
        gVBlankHandlerOverride = NULL;
        m4aSoundInit();
        m4aSoundVSyncOn();
        sMovieModeState++;
        break;
    case 3: {
        CpuFill32(0, (void*)VRAM, VRAM_SIZE);

        if (sMovieFlags & MOVIE_FLAG_SOFT_RESET) {
#ifdef VERSION_EU
            DoSoftReset();
#else
            SoftReset(RESET_ALL);
#endif
#ifdef VERSION_EU
        } else if (gDebugFlags & DEBUG_FLAG_DEBUG_MENU) {
            ModeRequest(&gModeMovieDebugEu, 0);
#endif
        } else {
            switch (sMovieId) {
            case 1:
                RequestEventMode(0);
                break;
            case 2:
                RequestEventMode(26);
                break;
            case 3:
                RequestEventMode(57);
                break;
            case 4:
                ModeRequest(&gModeStaffRoll, 0);
                break;
            case 5:
                ModeRequest(&gModeStaffRoll, 0);
                break;
            default:
                ModeRequest(&gModeDebug, 0);
                break;
            }
        }

        sMovieModeState++;
        break;
    }
    }
}

void mode_movie_2() {
    gVBlankHandlerOverride = NULL;
}

Mode gModeMovie = {
    "mode_movie",
    mode_movie_0,
    mode_movie_1,
    mode_movie_2,
};

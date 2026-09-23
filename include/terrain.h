// terrain.h (our name): the terrain manager of GoTerrain.c (TW06: Ter_TerrainGameDataMgr): the
// current hole's ground as it is loaded from the 'ter ' chunk, its patches, the lists it draws them
// from, and the renderer-state setters that share the file. ball.h includes it.

#ifndef TERRAIN_H
#define TERRAIN_H

#include "game_types.h"
#include "platform.h"

struct CourseInfo;
struct UStreamObject;

// One entry of TerrainMgr.p18 (0x30 bytes); fn_8003185C sorts them by f10, smallest first.
typedef struct TerSortItem {
    u8   unk0[0x10];
    f32  f10;                   // 0x10  the sort key
    u8   unk14[0x30 - 0x14];
} TerSortItem;
LAYOUT_ASSERT(TerSortItem, 0x30);

#define TER_NUM_PATCHES 650     // TerrainMgr.p34's length

// One patch of the ground (0x40 bytes). TerrainMgr.p34 holds TER_NUM_PATCHES of them, row by row;
// aRowStart gives each row's first.
typedef struct TerPatch {
    u8   unk0[0x10];
    f32  f10;                   // 0x10  } fn_800335F8 resets f14 to f10, or to 0 with n18
    f32  f14;                   // 0x14  }
    s32  n18;                   // 0x18
    s32  n1C;                   // 0x1C
    s32  a20[4];                // 0x20  four flag words read from the patch's model (fn_800354D0, 0..3)
    u8   unk30[0x40 - 0x30];
} TerPatch;
LAYOUT_ASSERT(TerPatch, 0x40);

// The data of the chunk fn_80034720 is given (by UKernel.c): the position of one tee.
typedef struct TerTeeData {
    u8   unk0[0x10];
    f32  vPos[3];               // 0x10
    u8   nTeeSet;               // 0x1C  its row of CourseInfo.tee
} TerTeeData;

// The data of the 'tLOD' chunk (fn_800341A4); only two values are read.
typedef struct TerLODData {
    u8   unk0[0xC];
    f32  fC;                    // 0xC  } made whole numbers into lbl_802810E4 and lbl_802810E8
    f32  f10;                   // 0x10 }
} TerLODData;

// The terrain manager (0x11C8 bytes). TW06: Ter_TerrainGameDataMgr, whose GetTGD (fn_8000C594)
// returns pCourse.
typedef struct TerrainMgr {
    void*        p0;            // 0x000  made from the 'ter ' chunk's data (fn_800073B4); NULL until loaded
    struct UStreamObject* p4;   // 0x004  the 'ter ' chunk
    struct CourseInfo* pCourse; // 0x008  NULL until loaded
    struct UStreamObject* pC;   // 0x00C  the chunk pCourse came in
    void*        p10;           // 0x010  } allocated arrays, freed by fn_800306B8
    void*        p14;           // 0x014  }
    TerSortItem* p18;           // 0x018  } n10D0 of them
    void*        p1C;           // 0x01C  } lists fn_80032F88 draws, n10D4, n10D8, n10DC and n10E4 long
    void*        p20;           // 0x020  }
    void*        p24;           // 0x024  }
    void*        p28;           // 0x028  }
    void*        p2C;           // 0x02C  }
    void*        p30;           // 0x030  }
    TerPatch*    p34;           // 0x034  } the patches
    u8           unk38[0xC8 - 0x38];
    u32          aRowStart[1024];   // 0x0C8  each row's first patch in p34
    u8           unk10C8[0x10D0 - 0x10C8];
    s32          n10D0;         // 0x10D0
    s32          n10D4;         // 0x10D4
    s32          n10D8;         // 0x10D8  0 when p20 is not drawn
    s32          n10DC;         // 0x10DC
    u8           unk10E0[0x10E4 - 0x10E0];
    s32          n10E4;         // 0x10E4
    u8           unk10E8[0x1110 - 0x10E8];
    f32          a1110[6];      // 0x1110  filled by fn_80031938
    u8           unk1128[0x1140 - 0x1128];
    f32          v1140[3];      // 0x1140  a row of lbl_801876D8 (fn_80034648)
    u8           unk114C[0x1154 - 0x114C];
    s32          n1154;         // 0x1154  -1 once unloaded
    u8           unk1158[0x116C - 0x1158];
    f32          f116C;         // 0x116C  -1 after fn_800335F8
    f32          f1170;         // 0x1170  GameManager wants at least 0.5 for its crowd flyby
    u8           unk1174[0x1178 - 0x1174];
    f32          f1178;         // 0x1178  GameManager wants at least 5 for its crowd flyby
    u8           unk117C[0x11A0 - 0x117C];
    f32          f11A0;         // 0x11A0
    f32          f11A4;         // 0x11A4
    f32          f11A8;         // 0x11A8  (f11A0 * f11A4) squared
    u8           b11AC;         // 0x11AC  fn_8003272C passes its value on only when set
    u8           unk11AD[0x11B8 - 0x11AD];
    s32          n11B8;         // 0x11B8  } passed on to fn_80032F88, which does not read them
    s32          n11BC;         // 0x11BC  }
    u8           unk11C0[0x11C8 - 0x11C0];
} TerrainMgr;
LAYOUT_ASSERT(TerrainMgr, 0x11C8);

// The settings fn_80035440 copies in (0x54 bytes); fn_80035398 makes the renderer's colour
// (RenderState.a30) and its distances f28 and f2C from them.
typedef struct TerSettings {
    u8   unk0[0x44];
    f32  f44;                   // 0x44  } the colour, 0..255 each
    f32  f48;                   // 0x48  }
    f32  f4C;                   // 0x4C  }
    f32  f50;                   // 0x50  RenderState.f28 is 0.375 of it, f2C 4.15 x (it + 10)
} TerSettings;
LAYOUT_ASSERT(TerSettings, 0x54);

extern TerrainMgr lbl_801D3CB0;
extern TerSettings* lbl_802811E0;
extern f32 lbl_801876D8[21][3];     // rows fn_80034648 copies into TerrainMgr.v1140
extern f32 lbl_802810C8;
extern s8  lbl_802810CC;
extern s32 lbl_802810D0;            // } fn_80031938's arguments; 26 and 16 once unloaded, else set
extern s32 lbl_802810D4;            // } by fn_80031A08 from the 'tLOD' chunk
extern f32 lbl_802810D8;            // }
extern s32 lbl_802810DC;            // }
extern s32 lbl_802810E0;            // }
extern s32 lbl_802810E4;            // } from the 'tLOD' chunk (fn_800341A4); -1 until then
extern s32 lbl_802810E8;            // }

// GoTerrain.c
void fn_800306B8(void);             // frees the terrain
void fn_800335F8(u8 bReset);
f32  fn_800336E4(void);
f32  fn_800336F4(void);
f32  fn_80035074(f32 x);            // floor
void fn_80034720(struct UStreamObject* pObject);   // a tee's position (TerTeeData)
void fn_80035118(int a, int b);     // renderer state: n10 and n14
void fn_80035138(int a);            // renderer state: uFC

#endif

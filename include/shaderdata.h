// shaderdata.h (our name): GoShaderObjectCommon_ShaderObjectsData_Gc.c (EA's name, from its asserts),
// which turns a shader object's drawing commands into a GX display list.

#ifndef SHADERDATA_H
#define SHADERDATA_H

#include "engine.h"
#include "gx.h"
#include "morphanim.h"

// A shader object's vertex arrays (its callers pass the pointer at their object + 0x24).
typedef struct ShaderVtxArrays {
    f32* apPos[5];              // 0x00  the positions (three f32s a vertex): ShaderCmds.nFrame picks
                                //       one; at most five, as p14 follows
    s16* p14;                   // 0x14  the texture coordinates (two s16s a vertex)
    GXColor* p18;               // 0x18  the colours
    f32* p1C;                   // 0x1C  the normals (three f32s a vertex)
} ShaderVtxArrays;

// A shader object's drawing commands: u16s, each command a code and its arguments
// (ShaderData_BuildDisplayList).
typedef struct ShaderCmds {
    s16  nFrames;               // 0x0  above 1: the list sets no position array (MorphAnim.bNoPosArray)
    u8   unk2[2];
    u32  nWords;                // 0x4  the commands' length in u16s
    u16  nFrame;                // 0x8  which ShaderVtxArrays.apPos the list draws
    u16  uFlags;                // 0xA  bit 0: RenderState_SetConstantAlphaActive(1) while the list is built
    u16* pCmds;                 // 0xC  the commands; the finished list is copied over them
} ShaderCmds;

// Build pAnim's display list from pCmds; eType is the object's shader type (its callers' object
// + 0x0).
void ShaderData_BuildDisplayList(ShaderVtxArrays* pArrays, int eType, MorphAnim* pAnim, ShaderCmds* pCmds);

// A static shader object (our name, after row 17's SD_vShaderObject_Grass_Static_Init/_Close/
// _Render at +0x24..+0x2C of its row of lbl_80188E88): drawn from a display list.
typedef struct StaticShaderObject {
    s32  eType;                 // 0x00  its shader type
    MorphAnim anim;             // 0x04  its display list and morph animation
    ShaderVtxArrays* pArrays;   // 0x24
} StaticShaderObject;

extern f32 lbl_80281E70;        // row 3's morph weight, handed in by fn_80070D7C

void ShaderData_CloseDisplayList(MorphAnim* pAnim);

struct TerWaveData;

// GoShaderObject_PrelitRGBAUVAnimation_Gc.c: row 4's hooks.
void SD_vShaderObject_PrelitRGBAUVAnimation_Type_SetParameters(struct TerWaveData* pWave);
void SD_vShaderObject_PrelitRGBAUVAnimation_Static_Init(StaticShaderObject* pObj, ShaderCmds* pCmds);
void SD_vShaderObject_PrelitRGBAUVAnimation_Static_Close(StaticShaderObject* pObj);
void SD_vShaderObject_PrelitRGBAUVAnimation_Static_Render(StaticShaderObject* pObj);

// GoShaderObject_PrelitTexturedMorph_Gc.c: row 2's hooks.
void SD_vShaderObject_PrelitTexturedMorph_Type_SetParameters(f32* pfWeight);
void SD_vShaderObject_PrelitTexturedMorph_Static_Init(StaticShaderObject* pObj, ShaderCmds* pCmds);
void SD_vShaderObject_PrelitTexturedMorph_Static_Close(StaticShaderObject* pObj);
void SD_vShaderObject_PrelitTexturedMorph_Static_Render(StaticShaderObject* pObj);

#endif

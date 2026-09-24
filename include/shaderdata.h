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

// A shader object's drawing commands: u16s, each command a code and its arguments (fn_80074DA8).
typedef struct ShaderCmds {
    s16  nFrames;               // 0x0  above 1: the list sets no position array (MorphAnim.b8)
    u8   unk2[2];
    u32  nWords;                // 0x4  the commands' length in u16s
    u16  nFrame;                // 0x8  which ShaderVtxArrays.apPos the list draws
    u16  uFlags;                // 0xA  bit 0: fn_8001618C(1) while the list is built
    u16* pCmds;                 // 0xC  the commands; the finished list is copied over them
} ShaderCmds;

// Build pAnim's display list from pCmds; eType is the object's shader type (its callers' object
// + 0x0).
void fn_80074DA8(ShaderVtxArrays* pArrays, int eType, MorphAnim* pAnim, ShaderCmds* pCmds);

#endif

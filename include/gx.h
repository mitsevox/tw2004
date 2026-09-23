// gx.h (our name): the GameCube graphics library (GX) calls the game code makes, declared once for
// every file. Only the calls the game uses, with parameter types from how the game calls them: GX's
// on/off switches are a byte, its enumerations an int, sizes and positions in pixels a u16. Add a
// call here when a file needs it, never a local declaration.

#ifndef GX_H
#define GX_H

#include "game_types.h"

// ---- what drawing writes ----------------------------------------------------------------------

void GXSetAlphaUpdate(u8 bUpdate);
void GXSetColorUpdate(u8 bUpdate);
void GXSetZMode(u8 bCompare, int eCompare, u8 bUpdate);

// ---- the texture environment (TEV) ------------------------------------------------------------

void GXSetTevColorIn(int eStage, int eA, int eB, int eC, int eD);
void GXSetTevAlphaIn(int eStage, int eA, int eB, int eC, int eD);
void GXSetTevColorOp(int eStage, int eOp, int eBias, int eScale, u8 bClamp, int eOutReg);
void GXSetTevAlphaOp(int eStage, int eOp, int eBias, int eScale, u8 bClamp, int eOutReg);

// ---- copying the screen into a texture --------------------------------------------------------

void GXSetTexCopySrc(u16 nLeft, u16 nTop, u16 nWidth, u16 nHeight);
void GXSetTexCopyDst(u16 nWidth, u16 nHeight, int eFormat, u8 bMipmap);
void GXCopyTex(void* pDest, u8 bClear);
void GXPixModeSync(void);
void GXInvalidateTexAll(void);

#endif

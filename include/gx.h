// gx.h (our name): the GameCube graphics library (GX) calls the game code makes, declared once for
// every file. Only the calls the game uses, with parameter types from how the game calls them: GX's
// on/off switches are a byte, its enumerations an int, sizes and positions in pixels a u16. Add a
// call here when a file needs it, never a local declaration.

#ifndef GX_H
#define GX_H

#include "game_types.h"
#include "platform.h"

// GX's objects, filled in by GX and only passed back to it: their contents are GX's own.
typedef struct GXTexObj {
    u32 unk0[8];
} GXTexObj;                     // a texture (0x20 bytes)
typedef struct GXTlutObj {
    u32 unk0[3];
} GXTlutObj;                    // a texture's palette (0xC bytes)
typedef struct GXColor {
    u8 r, g, b, a;
} GXColor;

// ---- what drawing writes ----------------------------------------------------------------------

void GXSetAlphaUpdate(u8 bUpdate);
void GXSetColorUpdate(u8 bUpdate);
void GXSetZMode(u8 bCompare, int eCompare, u8 bUpdate);
void GXSetBlendMode(int eType, int eSrcFactor, int eDstFactor, int eLogicOp);

// ---- vertex arrays ----------------------------------------------------------------------------

void GXInvalidateVtxCache(void);

// ---- display lists ----------------------------------------------------------------------------

void GXBeginDisplayList(void* pList, u32 uSize);
u32  GXEndDisplayList(void);            // the list's size
void GXResetWriteGatherPipe(void);

// ---- matrices --------------------------------------------------------------------------------

void GXLoadPosMtxImm(f32 (*pMtx)[4], int nId);
void GXSetClipMode(int eMode);
void GXSetArray(int eAttr, void* pBase, u8 nStride);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int eAttr, int eType);
void GXSetVtxAttrFmt(int eFmt, int eAttr, int eCompCnt, int eCompType, u8 nFrac);
void GXBegin(int ePrim, int eFmt, u16 nVerts);

// ---- textures ---------------------------------------------------------------------------------

void GXInitTexObj(GXTexObj* pObj, void* pImage, u16 nWidth, u16 nHeight, int eFormat, int eWrapS,
                  int eWrapT, u8 bMipmap);
void GXInitTexObjCI(GXTexObj* pObj, void* pImage, u16 nWidth, u16 nHeight, int eFormat, int eWrapS,
                    int eWrapT, u8 bMipmap, u32 nTlut);
void GXInitTlutObj(GXTlutObj* pObj, void* pLut, int eFormat, u16 nEntries);
void GXLoadTexObj(GXTexObj* pObj, int eMap);
void GXSetNumTexGens(u8 nGens);
void GXLoadTexMtxIndx(u16 nIndex, u32 nId, int eType);
void GXSetTexCoordGen2(int nDst, int nFunc, int nSrc, int nMtx, u8 bNormalize, int nPostMtx);
void GXSetNumTexGens(u8 nGens);
void GXLoadTexObj(GXTexObj* pObj, int eMap);
void GXLoadTlut(GXTlutObj* pObj, u32 nTlut);
void GXSetAlphaCompare(int eComp0, u8 nRef0, int eOp, int eComp1, u8 nRef1);
void GXSetZCompLoc(u8 bBeforeTex);

// ---- the texture environment (TEV) ------------------------------------------------------------

void GXSetNumTevStages(u8 nStages);
void GXSetTevOrder(int eStage, int eCoord, int eMap, int eColour);
void GXSetTevColor(int eReg, GXColor colour);
void GXSetTevColorIn(int eStage, int eA, int eB, int eC, int eD);
void GXSetTevAlphaIn(int eStage, int eA, int eB, int eC, int eD);
void GXSetTevColorOp(int eStage, int eOp, int eBias, int eScale, u8 bClamp, int eOutReg);
void GXSetTevAlphaOp(int eStage, int eOp, int eBias, int eScale, u8 bClamp, int eOutReg);

// ---- lighting -----------------------------------------------------------------------------------

typedef struct GXLightObj {
    u32 unk0[16];
} GXLightObj;                   // a light (0x40 bytes)

void GXSetChanAmbColor(int eChan, GXColor colour);
void GXSetChanMatColor(int eChan, GXColor colour);
void GXSetChanCtrl(int eChan, u8 bEnable, int eAmbSrc, int eMatSrc, u32 uLightMask, int eDiffFn,
                   int eAttnFn);
void GXInitLightPos(GXLightObj* pLight, f32 x, f32 y, f32 z);
void GXInitLightColor(GXLightObj* pLight, GXColor colour);
void GXLoadLightObjImm(GXLightObj* pLight, int eLight);

// ---- copying the screen into a texture --------------------------------------------------------

void GXSetTexCopySrc(u16 nLeft, u16 nTop, u16 nWidth, u16 nHeight);
void GXSetTexCopyDst(u16 nWidth, u16 nHeight, int eFormat, u8 bMipmap);
void GXCopyTex(void* pDest, u8 bClear);
void GXPixModeSync(void);
void GXInvalidateTexAll(void);
u32  GXGetTexBufferSize(u16 nWidth, u16 nHeight, int eFormat, u8 bMipmap, u8 nMaxLod);

// ---- the game's own GX helpers (GxUtil.c) ----------------------------------------------------

// A texture ready to draw (0x30 bytes; our name): GX's texture object and, for a palette
// (colour-index) texture, its palette object. fn_8002A528 fills one in.
typedef struct GxTexture {
    GXTexObj  tex;              // 0x00
    GXTlutObj tlut;             // 0x20  only for a palette texture
    u8        bPalette;         // 0x2C  1: tex is a palette texture
    u8        unk2D[3];
} GxTexture;
LAYOUT_ASSERT(GxTexture, 0x30);

// Fill in pTex for an image of nWidth x nHeight; with a palette pLut, a palette texture.
void fn_8002A528(GxTexture* pTex, int nWidth, int nHeight, void* pImage, void* pLut, int eFormat,
                 int eLutFormat, int eWrapS, int eWrapT);
void fn_8002A608(GxTexture* pTex);  // make pTex the texture of the next draw

#endif

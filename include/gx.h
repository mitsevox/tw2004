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
typedef struct GXColorS10 {
    s16 r, g, b, a;
} GXColorS10;                   // a signed colour for a TEV register (10 bits a component)
typedef struct GXFifoObj {
    u8 unk0[0x80];
} GXFifoObj;                    // the command FIFO (0x80 bytes)

// The video mode GX and VI render with (the SDK's layout, 0x3C bytes).
typedef struct GXRenderModeObj {
    u32 viTVmode;               // 0x00
    u16 fbWidth;                // 0x04  the frame's width in pixels
    u16 efbHeight;              // 0x06  its height
    u16 xfbHeight;              // 0x08
    u16 viXOrigin;              // 0x0A
    u16 viYOrigin;              // 0x0C
    u16 viWidth;                // 0x0E
    u16 viHeight;               // 0x10
    u8  unk12[2];
    u32 xFBmode;                // 0x14
    u8  field_rendering;        // 0x18  one field per frame: the viewport jitters by a line
    u8  aa;                     // 0x19
    u8  sample_pattern[12][2];  // 0x1A
    u8  vfilter[7];             // 0x32
    u8  unk39[3];
} GXRenderModeObj;
extern GXRenderModeObj GXNtsc480IntDf;  // the SDK's video modes
extern GXRenderModeObj GXPal528IntDf;
extern GXRenderModeObj GXMpal480IntDf;
void GXAdjustForOverscan(GXRenderModeObj* pIn, GXRenderModeObj* pOut, u16 nHor, u16 nVer);
typedef struct GXTexRegion {
    u32 unk0[4];
} GXTexRegion;                  // a texture cache region (0x10 bytes)

// ---- setting GX up and copying the frame out ------------------------------------------------------

GXFifoObj* GXInit(void* pBase, u32 uSize);
void GXInitTexCacheRegion(GXTexRegion* pRegion, u8 b32Bit, u32 uEvenAddr, int eEvenSize, u32 uOddAddr,
                          int eOddSize);
void GXSetScissor(u32 nLeft, u32 nTop, u32 nWidth, u32 nHeight);
void GXSetDispCopySrc(u16 nLeft, u16 nTop, u16 nWidth, u16 nHeight);
void GXSetDispCopyDst(u16 nWidth, u16 nHeight);
u32  GXSetDispCopyYScale(f32 fScale);
void GXSetCopyFilter(u8 bAA, u8 aSamples[12][2], u8 bVFilter, u8 aVFilter[7]);
void GXSetCopyClear(GXColor cClear, u32 uZ);
void GXSetDither(u8 bDither);
void GXSetPixelFmt(int ePixelFmt, int eZFmt);

// ---- the matrix library (MTX), for the matrices GX takes -------------------------------------

void PSMTXIdentity(f32 (*pMtx)[4]);
void PSMTXConcat(f32 (*pA)[4], f32 (*pB)[4], f32 (*pOut)[4]);
void PSMTXScale(f32 (*pMtx)[4], f32 fX, f32 fY, f32 fZ);
void PSMTXTrans(f32 (*pMtx)[4], f32 fX, f32 fY, f32 fZ);
u32  PSMTXInvXpose(f32 (*pSrc)[4], f32 (*pInvX)[4]);   // 0: pSrc has no inverse
void C_MTXOrtho(f32 (*pMtx)[4], f32 fTop, f32 fBottom, f32 fLeft, f32 fRight, f32 fNear, f32 fFar);

// ---- the command FIFO -------------------------------------------------------------------------

GXFifoObj* GXGetCPUFifo(void);
void GXGetFifoPtrs(GXFifoObj* pFifo, void** ppRead, void** ppWrite);
void GXEnableBreakPt(void* pBreak);
void GXDisableBreakPt(void);
void GXSetDrawDone(void);
void GXDrawDone(void);
void GXFlush(void);
void GXCopyDisp(void* pDest, u8 bClear);
void GXSetViewportJitter(f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight, f32 fNear, f32 fFar, u32 uField);
void GXGetFifoStatus(GXFifoObj* pFifo, u8* pbOverHigh, u8* pbUnderLow, u32* puCount, u8* pbCpuWrite,
                     u8* pbGpRead, u8* pbWrapped);

// ---- the viewport and projection ----------------------------------------------------------------

void GXSetCullMode(int eMode);
void GXSetProjection(f32 (*pMtx)[4], int eType);
void GXSetCurrentMtx(u32 nId);
void GXSetViewport(f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight, f32 fNear, f32 fFar);
void GXGetViewportv(f32* pViewport);    // 6 values: GXSetViewport's arguments
void GXSetProjectionv(f32* pProj);      // 7 values: the type, then the matrix's terms
void GXGetProjectionv(f32* pProj);

// ---- what drawing writes ----------------------------------------------------------------------

void GXSetAlphaUpdate(u8 bUpdate);
void GXSetColorUpdate(u8 bUpdate);
void GXSetZMode(u8 bCompare, int eCompare, u8 bUpdate);
void GXSetBlendMode(int eType, int eSrcFactor, int eDstFactor, int eLogicOp);
void GXSetFog(int eType, f32 fStartZ, f32 fEndZ, f32 fNearZ, f32 fFarZ, GXColor colour);

// ---- vertex arrays ----------------------------------------------------------------------------

void GXInvalidateVtxCache(void);

// ---- display lists ----------------------------------------------------------------------------

void GXBeginDisplayList(void* pList, u32 uSize);
u32  GXEndDisplayList(void);            // the list's size
void GXResetWriteGatherPipe(void);

// ---- matrices --------------------------------------------------------------------------------

void GXLoadPosMtxImm(f32 (*pMtx)[4], int nId);
void GXLoadNrmMtxImm(f32 (*pMtx)[4], int nId);
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
void GXInitTexObjLOD(GXTexObj* pObj, int eMinFilter, int eMagFilter, f32 fMinLod, f32 fMaxLod,
                     f32 fLodBias, u8 bBiasClamp, u8 bEdgeLod, int eMaxAniso);
void GXLoadTexObj(GXTexObj* pObj, int eMap);
void GXLoadTlut(GXTlutObj* pObj, u32 nTlut);
void GXSetNumTexGens(u8 nGens);
void GXLoadTexMtxIndx(u16 nIndex, u32 nId, int eType);
void GXLoadTexMtxImm(f32 (*pMtx)[4], u32 nId, int eType);
void GXSetTexCoordGen2(int nDst, int nFunc, int nSrc, int nMtx, u8 bNormalize, int nPostMtx);
void GXSetAlphaCompare(int eComp0, u8 nRef0, int eOp, int eComp1, u8 nRef1);
void GXSetZCompLoc(u8 bBeforeTex);

// ---- the texture environment (TEV) ------------------------------------------------------------

void GXSetNumTevStages(u8 nStages);
void GXSetTevOrder(int eStage, int eCoord, int eMap, int eColour);
void GXSetTevColor(int eReg, GXColor colour);
void GXSetTevKColor(int eReg, GXColor colour);
void GXSetTevColorS10(int eReg, GXColorS10 colour);
void GXSetTevKColorSel(int eStage, int eSel);
void GXSetTevKAlphaSel(int eStage, int eSel);
void GXSetTevSwapMode(int eStage, int eRasSel, int eTexSel);
void GXSetTevSwapModeTable(int eTable, int eRed, int eGreen, int eBlue, int eAlpha);
void GXSetTevColorIn(int eStage, int eA, int eB, int eC, int eD);
void GXSetTevAlphaIn(int eStage, int eA, int eB, int eC, int eD);
void GXSetTevColorOp(int eStage, int eOp, int eBias, int eScale, u8 bClamp, int eOutReg);
void GXSetTevAlphaOp(int eStage, int eOp, int eBias, int eScale, u8 bClamp, int eOutReg);

// ---- lighting -----------------------------------------------------------------------------------

typedef struct GXLightObj {
    u32 unk0[16];
} GXLightObj;                   // a light (0x40 bytes)

void GXSetNumChans(u8 nChans);
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
void fn_8002A024(u8 bOn, f32 x0, f32 y0, f32 x1, f32 y1);
void fn_8002A164(int nMode);
void fn_8002A2FC(void);             // the end of fn_8002A164's drawing
u8   fn_8002A3A4(void);             // fn_8002A164's screen copy is being drawn with
void fn_8002A3AC(u8 b);

// DepthField.c's textures: the half-size screen copy, and the two full-size image buffers.
extern GxTexture lbl_801D5198;
extern GxTexture lbl_801D51C8[2];

// GoPostFx.c's textures: of its own screen copy (twice), and of the shared one.
extern GxTexture lbl_801D4F80;
extern GxTexture lbl_801D4FB0[2];

#endif

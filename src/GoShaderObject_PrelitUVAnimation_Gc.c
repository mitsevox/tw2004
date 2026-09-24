// GoShaderObject_PrelitUVAnimation_Gc.c (EA's name, from its asserts): the shader object of row 5
// of lbl_80188E88, prelit geometry whose texture scrolls (engine.h's PrelitUVObject), the static
// hooks of rows 6 and 10, and the code that inits and closes every shader type (pfn0 / pfn4).

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "gx.h"
#include "shaderdata.h"

void fn_8007110C(u32* pnFrame);
void fn_80071148(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_8007117C(StaticShaderObject* pObj);
void fn_800711A4(StaticShaderObject* pObj);
void fn_800711F8(PrelitUVObject* pObj, const DynRenderSize* pSize);
void fn_800712B4(PrelitUVObject* pObj);
void fn_800712EC(PrelitUVObject* pObj);
void fn_800713B4(PrelitUVObject* pObj, PrelitUVFill* pFill, u8 bRestart);
void fn_800717AC(int nRow);
void fn_800717E8(int nRow);
void fn_80071914(void);
void fn_80071994(void);
void fn_80071A54(int nRow);
void fn_80071A90(int nRow);
void fn_80070168(void);     // sweep_80070168.c: calls a display list
void fn_8007524C(void);     // GoShaderObjectCommon_ShaderObjectsData_Gc.c: empty

s32 lbl_80281E80;
s32* lbl_802811F8 = &lbl_80281E80;

// Row 5's data hook: set the texture animation clock from the frame count.
void fn_8007110C(u32* pnFrame) {
    fn_80076C20(*pnFrame);
}

// Row 5's static init: build the object's display list from pCmds.
void fn_80071148(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    fn_80074DA8(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 5's static close; it also forgets every texture animation.
void fn_8007117C(StaticShaderObject* pObj) {
    // port: EA passes an argument fn_8007524C ignores
    ((void (*)(MorphAnim*))fn_8007524C)(&pObj->anim);
    fn_80076E24();
}

// Row 5's static render: the display list (with the object's positions when the list sets none).
void fn_800711A4(StaticShaderObject* pObj) {
    if (pObj->anim.b8 != 0) {
        GXSetArray(9, pObj->pArrays->apPos[0], 12);
    }
    // port: fn_80070168 (sweep_80070168.c) is defined without parameters but hands r3 and r4
    // on to GXCallDisplayList
    ((void (*)(void*, u32))fn_80070168)(pObj->anim.p4, pObj->anim.n0);
}

// Make the object's buffer (at the given sizes, or 50 vertices and one draw) and its texture
// matrix, unmoved.
void fn_800711F8(PrelitUVObject* pObj, const DynRenderSize* pSize) {
    if (pSize != NULL) {
        pObj->pBuf = fn_8007018C(pSize->nMaxVerts, pSize->nMaxDraws);
    } else {
        pObj->pBuf = fn_8007018C(50, 1);
    }
    pObj->pMtx = fn_80009B34(0x20, 2, 32, "GoShaderObject_PrelitUVAnimation_Gc.c", 172);
    pObj->pMtx[0][0] = 1.0f;
    pObj->pMtx[0][1] = 0.0f;
    pObj->pMtx[0][2] = 0.0f;
    pObj->pMtx[0][3] = 0.0f;
    pObj->pMtx[1][0] = 0.0f;
    pObj->pMtx[1][1] = 1.0f;
    pObj->pMtx[1][2] = 0.0f;
    pObj->pMtx[1][3] = 0.0f;
}

// Free the object's texture matrix and buffer.
void fn_800712B4(PrelitUVObject* pObj) {
    fn_80009E70(pObj->pMtx);
    fn_80070348(pObj->pBuf);
}

// Draw every draw in the object's buffer, texture coordinates through its matrix.
void fn_800712EC(PrelitUVObject* pObj) {
    u32 nDraws;
    DynRenderDraw* pDraw;
    u32 i;
    DynRenderDrawList* pList;

    GXSetTexCoordGen2(0, 1, 4, 0x39, 0, 0x7D);
    GXLoadTexMtxImm(pObj->pMtx, 0x39, 1);
    i = 0;
    pList = pObj->pBuf->pDraws;
    nDraws = pList->nDraws;
    pDraw = pList->aDraws;
    for (; i < nDraws; i++) {
        u32 nStart = pDraw->nStart;
        u32 nCount = pDraw->nCount;
        int nPrim = pDraw->nPrim;
        pDraw++;
        fn_800703B8(pObj->pBuf, nStart, nCount, nPrim);
    }
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
}

// Add a frame's draws, indices and vertices to the object's buffer (bRestart: replace what is
// there) and move its matrix like a texture animation's (fn_80076C20).
void fn_800713B4(PrelitUVObject* pObj, PrelitUVFill* pFill, u8 bRestart) {
    f32 fTime;
    f32 fU;
    f32 fV;
    f32 fStepU;
    f32 fStepV;
    f32 fScale;
    u16 nIndices;
    u16 i;

    fn_80070764(pObj->pBuf, pFill->fill.pDraws, pFill->fill.nCount, 0, bRestart);
    fTime = fn_80076E18();
    fU = pFill->anim.fU * fTime;
    fV = pFill->anim.fV * fTime;
    fStepU = fU - (s32)fU;
    fStepV = fV - (s32)fV;
    fScale = pFill->anim.fScale;
    fStepU = fStepU * fScale;
    fStepV = fStepV * fScale;
    pObj->pMtx[0][3] = (s32)fStepU * pFill->anim.fInv;
    pObj->pMtx[1][3] = (s32)fStepV * pFill->anim.fInv;
    if (pFill->fill.pDraws == NULL) {
        fn_800705F0(pObj->pBuf, pFill->fill.pIndices, pFill->fill.nCount, bRestart);
    } else {
        nIndices = 0;
        for (i = 0; i < pFill->fill.nCount; i++) {
            if (pFill->fill.pDraws[i].nStart + pFill->fill.pDraws[i].nCount > nIndices) {
                nIndices = pFill->fill.pDraws[i].nStart + pFill->fill.pDraws[i].nCount;
            }
        }
        fn_800705F0(pObj->pBuf, pFill->fill.pIndices, nIndices, bRestart);
    }
    fn_800704C4(pObj->pBuf, pFill->fill.pPos, pFill->fill.pColour, pFill->fill.pTexCoord, pFill->fill.nVerts,
                bRestart);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80071574(void);
void fn_80071578(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_800715AC(StaticShaderObject* pObj);
void fn_800715D0(StaticShaderObject* pObj);
void fn_80071624(void);
void fn_80071628(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_8007165C(StaticShaderObject* pObj);
void fn_80071680(StaticShaderObject* pObj);
void fn_80097208(void);     // GoShaderObjectCommon_MorphAnimManager_Gc.c: allocate the manager
void fn_80097250(void);     // GoShaderObjectCommon_MorphAnimManager_Gc.c: free the manager
void fn_8009F780(void);     // MC_Gc.c: set vertex format 2
void fn_8009F7E4(void);     // MC_Gc.c: empty
void fn_800716D4(void);
void fn_800716FC(void);
void fn_80071724(void);
void fn_80071748(void);
void fn_8007176C(void);
void fn_8007178C(void);
void fn_80071828(void);
void fn_80071850(void);
s32* fn_80071854(void);
void fn_8006FCD4(void);     // GoShaderObjectContainer_OBFData_Gc.c: empty
void fn_8006FCD8(void);     // GoShaderObjectContainer_OBFData_Gc.c: empty
void fn_8007185C(void);
void fn_80071890(void);
void fn_800718C4(void);
void fn_800718E4(void);
void fn_8007190C(void);
void fn_80071910(void);
void fn_80071A14(void);
void fn_80071A34(void);

// Row 10's data hook: nothing.
void fn_80071574(void) {
}

// Row 10's static init: build the object's display list from pCmds.
void fn_80071578(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    fn_80074DA8(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 10's static close.
void fn_800715AC(StaticShaderObject* pObj) {
    // port: EA passes an argument fn_8007524C ignores
    ((void (*)(MorphAnim*))fn_8007524C)(&pObj->anim);
}

// Row 10's static render: the display list (with the object's positions when the list sets none).
void fn_800715D0(StaticShaderObject* pObj) {
    if (pObj->anim.b8 != 0) {
        GXSetArray(9, pObj->pArrays->apPos[0], 12);
    }
    // port: fn_80070168 (sweep_80070168.c) is defined without parameters but hands r3 and r4
    // on to GXCallDisplayList
    ((void (*)(void*, u32))fn_80070168)(pObj->anim.p4, pObj->anim.n0);
}

// Row 6's data hook: nothing.
void fn_80071624(void) {
}

// Row 6's static init: build the object's display list from pCmds.
void fn_80071628(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    fn_80074DA8(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 6's static close.
void fn_8007165C(StaticShaderObject* pObj) {
    // port: EA passes an argument fn_8007524C ignores
    ((void (*)(MorphAnim*))fn_8007524C)(&pObj->anim);
}

// Row 6's static render: the display list (with the object's positions when the list sets none).
void fn_80071680(StaticShaderObject* pObj) {
    if (pObj->anim.b8 != 0) {
        GXSetArray(9, pObj->pArrays->apPos[0], 12);
    }
    // port: fn_80070168 (sweep_80070168.c) is defined without parameters but hands r3 and r4
    // on to GXCallDisplayList
    ((void (*)(void*, u32))fn_80070168)(pObj->anim.p4, pObj->anim.n0);
}

void fn_800716D4(void) {
    fn_8009F780();
    fn_80076B7C();
    fn_80097208();
}

void fn_800716FC(void) {
    fn_80097250();
    fn_80076BC4();
    fn_8009F7E4();
}

void fn_80071724(void) {
    fn_800717AC(0);
}

void fn_80071748(void) {
    fn_800717E8(0);
}

void fn_8007176C(void) {
    fn_80071724();
}

void fn_8007178C(void) {
    fn_80071748();
}

// ---- end of sweep code ----

// Run the first hook of lbl_80188E78's set nRow, if it has one.
void fn_800717AC(int nRow) {
    if (lbl_80188E78[nRow].pfn0 != NULL) {
        lbl_80188E78[nRow].pfn0();
    }
}

// Run the second hook of lbl_80188E78's set nRow, if it has one.
void fn_800717E8(int nRow) {
    if (lbl_80188E78[nRow].pfn4 != NULL) {
        lbl_80188E78[nRow].pfn4();
    }
}

// ---- sweep code (not yet cleaned up) ----

// Zero the copy of lbl_80281B88 (fn_800718E4).
void fn_80071828(void) {
    s32* pnField = fn_80071854();

    *pnField = 0;
}

void fn_80071850(void) {
}

// Where the copy of lbl_80281B88 is kept.
s32* fn_80071854(void) {
    return lbl_802811F8;
}

void fn_8007185C(void) {
    fn_8006FCD4();
    fn_800716D4();
    fn_8007190C();
    fn_8007176C();
    fn_80071A14();
    fn_80071828();
}

void fn_80071890(void) {
    fn_80071850();
    fn_80071A34();
    fn_8007178C();
    fn_80071910();
    fn_800716FC();
    fn_8006FCD8();
}

void fn_800718C4(void) {
    fn_800718E4();
}

// Copy lbl_80281B88 (displayed-frame count; bit 0 is the video field) into fn_80071854's slot.
void fn_800718E4(void) {
    s32* pnField = fn_80071854();

    *pnField = lbl_80281B88;
}

void fn_8007190C(void) {
}

void fn_80071910(void) {
}

// ---- end of sweep code ----

// Init every shader type (ModuleHooks.pfn0): all rows in game types 4 to 8, else only those
// lbl_801893D8 marks.
void fn_80071914(void) {
    Session* pSession = &gSession;
    u8* pbRows = lbl_801893D8;
    int i;

    for (i = 0; i < 20; i++) {
        if ((pSession->nGameType >= 4 && pSession->nGameType <= 8) || pbRows[i] != 0) {
            fn_80071A54(i);
        }
    }
}

// Close every shader type (ModuleHooks.pfn4), the same rows as fn_80071914.
void fn_80071994(void) {
    Session* pSession = &gSession;
    u8* pbRows = lbl_801893D8;
    int i;

    for (i = 0; i < 20; i++) {
        if ((pSession->nGameType >= 4 && pSession->nGameType <= 8) || pbRows[i] != 0) {
            fn_80071A90(i);
        }
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80071A14(void) {
    fn_80071914();
}

void fn_80071A34(void) {
    fn_80071994();
}

// ---- end of sweep code ----

// Init shader type nRow, if it has an init.
void fn_80071A54(int nRow) {
    if (lbl_80188E88[nRow].pfn0 != NULL) {
        lbl_80188E88[nRow].pfn0();
    }
}

// Close shader type nRow, if it has a close.
void fn_80071A90(int nRow) {
    if (lbl_80188E88[nRow].pfn4 != NULL) {
        lbl_80188E88[nRow].pfn4();
    }
}

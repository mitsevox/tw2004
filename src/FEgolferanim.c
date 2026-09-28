// FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree, TW06 and TW07, where it
// sits in ui_core/istudio_runtime): the golfer shown on the menu screens, above all the
// create-a-player (CrAP) screen's. His state is CrAPState (camera.h, gpCrAPState). In order:
// - the loader: a state machine (gFEStreamStateMgr, its states gFEStreamStates, FE_StreamFunc_*)
//   that streams the next golfer's file in, loads his textures, and swaps them when his clothes
//   change;
// - each frame: FE_vUpdateGolferAll animates, places and lights him and runs the CrAP
//   screen's queued animation; the render code draws him, straight into the frame or through a
//   screen-copy texture that fades him in and out (gFEOffscreenBufferRender);
// - what the menus call: the CrAP camera's idle state, the queued animation and its camera shot,
//   the club he holds, what the screen shows (the golfer, his clubs or the ball), his handedness
//   and when a texture swap is switched in.

#include "game.h"
#include "camera.h"
#include "character.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "gx.h"
#include "lighting.h"
#include "terrain.h"
#include "charstate.h"
#include "dynobj.h"
#include "lldyntex.h"
#include "ustream.h"

// The golfers the menus show in turn when none is picked: four rows of five golfer ids, the row
// picked at random.
s32 gFEGolferCycle[4][5] = {
    { 0, 5, 11, 14, 16 },
    { 1, 2, 20, 17, 10 },
    { 0, 8, 21, 24, 26 },
    { 1, 27, 4, 15, 25 },
};

// Where the menu golfer stands (TW07's defaultPos): FE_vUpdateGolferAll places him
// there, char.c's front-end golfer stream callback a golfer that has just come in.
f32 gFEGolferPos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

// Per screen kind: two vectors copied into CrAPState.v120 and v130 (all three are the same).
f32 lbl_80189A40[2][4] = { { 0.5f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } };
f32 lbl_80189A60[2][4] = { { 0.5f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } };
f32 lbl_80189A80[2][4] = { { 0.5f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } };

void FE_StreamFunc_IdleInit(void);
void FE_StreamFunc_IdleUpdate(void);
void FE_StreamFunc_IdleClose(void);
void FE_StreamFunc_IdleInterrupt(void);
void FE_StreamFunc_SkinInit(void);
void FE_StreamFunc_SkinUpdate(void);
void FE_StreamFunc_SkinClose(void);
void FE_StreamFunc_SkinInterrupt(void);
void FE_StreamFunc_TexturesInit(void);
void FE_StreamFunc_TexturesUpdate(void);
void FE_StreamFunc_TexturesClose(void);
void FE_StreamFunc_TexturesInterrupt(void);
void FE_StreamFunc_SwapTexturesInit(void);
void FE_StreamFunc_SwapTexturesUpdate(void);
void FE_StreamFunc_SwapTexturesClose(void);
void FE_StreamFunc_SwapTexturesInterrupt(void);

// The loader's states: 1 wait for a golfer to load, 2 stream him in, 3 set him up, 4 set up the
// golfer already loaded.
FEGolferState gFEStreamStates[FE_NUM_GOLFER_STATES] = {
    { NULL, NULL, NULL, NULL, 0 },
    { FE_StreamFunc_IdleInit, FE_StreamFunc_IdleUpdate,
      FE_StreamFunc_IdleClose, FE_StreamFunc_IdleInterrupt, 2 },
    { FE_StreamFunc_SkinInit, FE_StreamFunc_SkinUpdate,
      FE_StreamFunc_SkinClose, FE_StreamFunc_SkinInterrupt, 3 },
    { FE_StreamFunc_TexturesInit, FE_StreamFunc_TexturesUpdate,
      FE_StreamFunc_TexturesClose, FE_StreamFunc_TexturesInterrupt, 1 },
    { FE_StreamFunc_SwapTexturesInit, FE_StreamFunc_SwapTexturesUpdate,
      FE_StreamFunc_SwapTexturesClose, FE_StreamFunc_SwapTexturesInterrupt, 1 },
};

// .bss and .sbss are defined in reverse address order: CodeWarrior lays them out last-defined-first.
GxTexture gFEBufferTextures[2];     // over lbl_80281BA4's two image buffers (FE_InitGolferTextures)
// the golfer copied out of the frame (FE_CopyGolferToTexture; fn_8002A624's pixels), drawn faded
// by FE_DrawGolferTexture
GxTexture gFEScreenCopyTex;
FEGolferMachine gFEStreamStateMgr;  // the loader's state machine (FE_StreamInitStateMgr)

s32 gFEOffscreenBufferRender = 1;   // draw the golfer through gFEScreenCopyTex (FE_SetOffscreenBufferRender)
f32 gFEDimAlphaMax = 0.17f;         // while b83 is set: the most the display's alpha f14C may be
f32 lbl_80281338 = 0.1f;            // while b83 is set: f140, f144 and f148 (written only)
u8  gFEGolferEnabled = 1;           // never cleared; 0 would stop animating and drawing the golfer
s32 gFELastDrawnGolfer = -1;        // } the golfer and profile slot last drawn (FE_RenderGolfer); -1 after
s32 gFELastDrawnSlot = -1;          // } a load, so he is given his ball and textures again
f32 lbl_80281348 = 0.918f;          // the share of the 448-line frame FE_RenderGolfer sets for screen kind 3

Character* gFEGolferChars[CRAP_NUM_GOLFERS];    // per golfer slot: its starting character (NULL)
CourseLights* gFEGolferLights;      // the golfer's lights ('LITE' stream object, FE_lite_vStreamCallback)
CrAPState* gpCrAPState;             // the menu golfer's state (allocated by FE_CharMgrInit)

void FE_CharMgrClose(void);
void FE_StreamInterruptState(void);
void FE_StreamSetNextState(int nNext);
void FE_StreamInitStateMgr(void);
void FE_StreamWaitForState(int nState);
void FE_StreamStopForClose(void);
void FE_StreamPopState(void);
void FE_StreamUpdateState(void);
void FE_BeginRenderGolferPhase1(void);
void FE_ClearGolferFrame(void);
void FE_DrawGolferTexture(void);
void FE_DrawGolferAlphaMask(void);
void FE_CopyGolferToTexture(void);
void FE_RenderGolfer(u8 bFull);
void FE_SetupCharState(void);
void FE_CharPositionOverwrite(void);
void FE_RotateCrAPModel(f32 fTurn);
void FE_lite_vStreamCallback(UStreamObject* pObject);
void FE_vExecuteClearGolferCache(void);
void FE_vFreeUnusedCharacters(void);
u8   FE_IsGolferInOtherSlot(int nGolfer, CrAPGolfer* pGolfer);
void FE_vLoadNextCrAPAnim(u8 bNoBlend);
Clip* FE_CrapGetIdleAnim(void);
void FE_ZoomCrAPModel(u8 bZoom);
void FE_CRAPSetHandednessForScreen(u8 bLefty);
void FE_OpenGolferStream(void);
void FE_CloseGolferStream(void);
void FE_Vec4Sub(f32* pA, f32* pB, f32* pOut);
void FE_CharMgrInit(void);

void fn_80007254(void);
void fn_80008380(void);
void LLMath_IdentifyMat(f32 (*m)[4]);          // identity matrix
void RenderState_SetRenderSurface(int a, int nWidth, int nHeight, int nField, int b, int c);
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1);
void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY);
void ViewController_Update(int nView);
void RC_UpdateCurrentScreenMatrices(void);
void SKN_DrawCharacterParts(Character* pChar);
void SKN_DrawClubParts(Character* pChar);
void LI_ResetLights(void);
void LI_SetObjectLights(UObject* pObj);
void fn_800760B0(int nX, int nY, int nWidth, int nHeight);
void SKN_BeginFrame(void);
void fn_800B9EB8(char* szBall);
void Character_ExecuteTextureSwapFE(Character* pChar);
char* fn_800484E0(int i);
void fn_80035600(void);
void Character_UpdateAnimation(Character* pChar, int a, f32 f);
void SKN_PoseCharacter(Character* pChar, int n);
void LF_LoadCurrentLights(void);
void fn_80079974(void);
void fn_800B9CF0(int n);
void SkinPart_SetChangeAllCopies(u8 b);
void fn_8010B098(void* p);
void fn_8010B9BC(void);
u8   fn_8010BFE0(void);
void UStream_Stop(void);
int  Stream_OpenStreamFiles(const UStreamParams* pParams);     // UStream.c
int  UStream_Close(int nStream);                                // UStream.c

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0, before the 0.5 and 0.0 FE_CharMgrInit uses first; its body is unknown, this one only
// reproduces the order.
static void FEgolferanim_StrippedFn(f32* pValue) {
    *pValue += 1.0f;
}

// Allocate and set up the menu golfer's state (gpCrAPState): every golfer slot empty and given the
// next golfer of gFEGolferCycle (a random row), slot 0 shown and loading first; then the loader's
// state machine starts in its idle state.
void FE_CharMgrInit(void) {
    int i;
    int nPrev;
    int nNext;

    gpCrAPState = StaticMem_Alloc(sizeof(CrAPState), 2, 0, "FEgolferanim.c", 337);
    gpCrAPState->n0 = 0;
    gpCrAPState->n4 = 0;
    gpCrAPState->n1B8 = 0;
    gpCrAPState->n1BC = -1;
    gpCrAPState->n1B4 = 0;
    gpCrAPState->sz10[0] = '\0';
    gpCrAPState->b85 = 1;
    gpCrAPState->b86 = 0;
    gpCrAPState->b91 = 1;
    gpCrAPState->b88 = 0;
    gpCrAPState->b89 = 0;
    gpCrAPState->b84 = 1;
    gpCrAPState->b82 = 0;
    gpCrAPState->b83 = 0;
    gpCrAPState->f14C = 0.5f;
    gpCrAPState->nBC = 0;
    gpCrAPState->b87 = 0;
    gpCrAPState->b8A = 0;
    gpCrAPState->n8C = -1;
    gpCrAPState->n194 = 0;
    gpCrAPState->n198 = Misc_RandFunc(0) & 3;
    gpCrAPState->f19C = 0.0f;
    gpCrAPState->f1A0 = 0.0f;
    gpCrAPState->b1B0 = 0;
    gpCrAPState->b90 = 0;
    gpCrAPState->b1D1 = 1;
    gpCrAPState->b1D2 = 0;
    gpCrAPState->b1DC = 0;
    gpCrAPState->aGolfer[0].nC = -1;
    for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
        nPrev = i - 1;
        if (nPrev < 0) {
            nPrev = CRAP_NUM_GOLFERS - 1;
        }
        nNext = i + 1;
        if (nNext > CRAP_NUM_GOLFERS - 1) {
            nNext = 0;
        }
        gFEGolferChars[i] = NULL;
        gpCrAPState->aGolfer[i].pChar = gFEGolferChars[i];
        gpCrAPState->aGolfer[i].n10 = i;
        gpCrAPState->aGolfer[i].n14 = -1;
        gpCrAPState->aGolfer[i].n1C = -1;
        gpCrAPState->aGolfer[i].b19 = 0;
        gpCrAPState->aGolfer[i].pPrev = &gpCrAPState->aGolfer[nPrev];
        gpCrAPState->aGolfer[i].pNext = &gpCrAPState->aGolfer[nNext];
        gpCrAPState->aGolfer[i].b18 = 0;
        gpCrAPState->aGolfer[i].nC = gFEGolferCycle[gpCrAPState->n198][gpCrAPState->n194];
        gpCrAPState->n194++;
        if (gpCrAPState->n194 >= 5) {
            gpCrAPState->n194 = 0;
            gpCrAPState->n198++;
            if (gpCrAPState->n198 >= 4) {
                gpCrAPState->n198 = 0;
            }
        }
    }
    gpCrAPState->pB4 = &gpCrAPState->aGolfer[0];
    gPlayers[0].pChar = gpCrAPState->pB4->pChar;
    gpCrAPState->pB8 = &gpCrAPState->aGolfer[0];
    gpCrAPState->n190 = 0;
    FE_StreamInitStateMgr();
    gpCrAPState->n74 = 0;
}

// Stop the loader (FE_StreamStopForClose), free every golfer slot's character, and free the state.
void FE_CharMgrClose(void) {
    FE_StreamStopForClose();
    FE_vExecuteClearGolferCache();
    FE_vFreeUnusedCharacters();
    StaticMem_Free(gpCrAPState);
    gpCrAPState = NULL;
}

// Show golfer nGolfer (TW07's midModel; the menus' message fn_8007D810): the loader is interrupted
// and sent back to idle; if the shown slot's pNext or pPrev holds him, that becomes the shown slot,
// else the shown slot gets his id to load. Golfers 7 and 29 (as nGolfer, nOtherA or nOtherB) set
// b90: every slot's character is freed before the next load (FE_StreamManageCRaPMemory). On screen
// kind 3 his animation starts again (FE_vLoadNextCrAPAnim).
void FE_setupStreaming(int nGolfer, int nOtherA, int nOtherB) {
    FE_StreamInterruptState();
    FE_StreamSetNextState(1);
    if (nGolfer == 7 || nGolfer == 29) {
        nOtherA = -1;
        nOtherB = -1;
        gpCrAPState->b90 = 1;
    }
    if (nOtherB == 7 || nOtherB == 29) {
        gpCrAPState->b90 = 1;
    }
    if (nOtherA == 7 || nOtherA == 29) {
        gpCrAPState->b90 = 1;
    }
    if (gpCrAPState->pB4->nC != nGolfer) {
        if (gpCrAPState->pB4->pNext->nC == nGolfer) {
            gpCrAPState->pB4 = gpCrAPState->pB4->pNext;
            gPlayers[0].pChar = gpCrAPState->pB4->pChar;
        } else if (gpCrAPState->pB4->pPrev->nC == nGolfer) {
            gpCrAPState->pB4 = gpCrAPState->pB4->pPrev;
            gPlayers[0].pChar = gpCrAPState->pB4->pChar;
        } else {
            gpCrAPState->pB4->nC = nGolfer;
            gpCrAPState->pB4->b18 = 0;
            gpCrAPState->pB4->n1C = -1;
        }
    }
    gpCrAPState->b87 = 1;
    FE_vFreeUnusedCharacters();
    if (gpCrAPState->n0 == 3) {
        FE_vLoadNextCrAPAnim(0);
    }
}

// Before golfer 7 or 29 loads (b90): flag every slot that still holds a character to be freed (b19)
// and return 0 until none does, then clear b90. Returns 1 when loading may go on.
u8 FE_StreamManageCRaPMemory(void) {
    u8 bAllFree;
    int i;

    if (gpCrAPState->b90) {
        bAllFree = 1;
        for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
            if (gpCrAPState->aGolfer[i].pChar != NULL) {
                gpCrAPState->aGolfer[i].b19 = 1;
                bAllFree = 0;
            }
        }
        if (bAllFree) {
            gpCrAPState->b90 = 0;
            return 1;
        }
        return 0;
    }
    return 1;
}

void FE_StreamFunc_IdleInit(void) {
}

void FE_StreamFunc_IdleClose(void) {
}

// The idle state's update: pick the next golfer to load, the shown one first, then its pNext and
// pPrev, skipping one already loaded (b18) or with no id. The pick becomes pB8 (nBC says which); a
// slot that still holds a character is flagged to be freed first (b19), and once the memory is free
// (FE_StreamManageCRaPMemory) the state finishes, on to the skin state. A golfer another slot
// already has is not loaded again (for the shown one, FE_vClearGolferCache stops the loader). Nothing
// happens while the loader is being stopped (b8A) or slot 0 waits to be freed; an interrupt
// finishes the state at once.
void FE_StreamFunc_IdleUpdate(void) {
    CrAPGolfer* pGolfer;

    if (gFEStreamStateMgr.bAbort) {
        FE_StreamPopState();
        return;
    }
    if (gpCrAPState->b8A == 1) return;
    if (gpCrAPState->aGolfer[0].b19 == 1) return;
    pGolfer = gpCrAPState->pB4;
    if (pGolfer->b18 == 0 && pGolfer->nC != -1) {
        if (FE_IsGolferInOtherSlot(pGolfer->nC, pGolfer)) {
            FE_vClearGolferCache();
            return;
        }
        gpCrAPState->pB8 = gpCrAPState->pB4;
        gpCrAPState->nBC = 0;
        if (gpCrAPState->pB8->pChar != NULL) {
            gpCrAPState->pB8->b19 = 1;
            return;
        }
        if (FE_StreamManageCRaPMemory()) {
            FE_StreamPopState();
        }
    } else if (pGolfer->pNext->b18 == 0 && pGolfer->pNext->nC != -1) {
        if (FE_IsGolferInOtherSlot(pGolfer->pNext->nC, pGolfer->pNext)) return;
        gpCrAPState->pB8 = gpCrAPState->pB4->pNext;
        gpCrAPState->nBC = 1;
        if (gpCrAPState->pB8->pChar != NULL) {
            gpCrAPState->pB8->b19 = 1;
            return;
        }
        if (FE_StreamManageCRaPMemory()) {
            FE_StreamPopState();
        }
    } else if (pGolfer->pPrev->b18 == 0 && pGolfer->pPrev->nC != -1) {
        if (FE_IsGolferInOtherSlot(pGolfer->pPrev->nC, pGolfer->pPrev)) return;
        gpCrAPState->pB8 = gpCrAPState->pB4->pPrev;
        gpCrAPState->nBC = 2;
        if (gpCrAPState->pB8->pChar != NULL) {
            gpCrAPState->pB8->b19 = 1;
            return;
        }
        if (FE_StreamManageCRaPMemory()) {
            FE_StreamPopState();
        }
    }
}

void FE_StreamFunc_IdleInterrupt(void) {
}

// The skin state's start: open the stream of the front-end character file of the golfer being
// loaded (pB8; fn_80014DFC, FE_OpenGolferStream) and note his id as streamed (n14, n8C); n190 counts the
// loads.
void FE_StreamFunc_SkinInit(void) {
    fn_80014DFC(gpCrAPState->pB8->nC, gpCrAPState->pB8->n10);
    FE_OpenGolferStream();
    gpCrAPState->pB8->n14 = gpCrAPState->pB8->nC;
    gpCrAPState->n8C = gpCrAPState->pB8->nC;
    gpCrAPState->n190++;
}

// Close the golfer's stream (FE_CloseGolferStream).
void FE_StreamFunc_SkinClose(void) {
    FE_CloseGolferStream();
}

// Run the stream until it has nothing left to do, then finish the state; after an interrupt the
// golfer is marked not loaded (b18).
void FE_StreamFunc_SkinUpdate(void) {
    if (!UStream_Update()) {
        if (gFEStreamStateMgr.bAbort) {
            gpCrAPState->pB8->b18 = 0;
        }
        FE_StreamPopState();
    }
}

void FE_StreamFunc_SkinInterrupt(void) {
    UStream_Stop();
}

// The texture state's start, for the golfer just streamed in (pB8): profiles set up, his clubs and
// clothes set for the current profile's slot (golfers 7 and 29 first get fn_80079974's parts), his
// dynamic textures emptied and a texture load requested (the front end's load callbacks).
void FE_StreamFunc_TexturesInit(void) {
    int nGolfer;

    Session_SetupProfiles();
    SkinPart_SetChangeAllCopies(1);
    nGolfer = gpCrAPState->pB8->pChar->nC;
    if (nGolfer == 7 || nGolfer == 29) {
        fn_80079974();
    }
    Character_SetClubsAndClothes(gpCrAPState->pB8->pChar, lbl_80281ED4->nSlot);
    SkinPart_SetChangeAllCopies(0);
    fn_8010B098(gpCrAPState->pB8->pChar->apDynTex[gpCrAPState->pB8->pChar->nCurDynTex]);
    Character_AddTextureLoadRequest(gpCrAPState->pB8->pChar, Character_BeginLoadTexturesCallbackFE,
                                    Character_EndLoadTexturesCallbackFE);
}

void FE_StreamFunc_TexturesClose(void) {
}

// Wait for the texture load (fn_8010BFE0); then the golfer is loaded (b18, and the display draws
// him afresh: gFELastDrawnGolfer and gFELastDrawnSlot reset), or after an interrupt not, and the state
// finishes.
void FE_StreamFunc_TexturesUpdate(void) {
    if (!fn_8010BFE0()) {
        if (gFEStreamStateMgr.bAbort) {
            gpCrAPState->pB8->b18 = 0;
        } else {
            gpCrAPState->pB8->b18 = 1;
            gFELastDrawnSlot = -1;
            gFELastDrawnGolfer = -1;
            gpCrAPState->pB8->n1C = -1;
        }
        gpCrAPState->n8C = -1;
        FE_StreamPopState();
    }
}

// Drop the texture jobs (fn_8010B9BC).
void FE_StreamFunc_TexturesInterrupt(void) {
    fn_8010B9BC();
}

// The texture-swap state's start, for the golfer shown (pB4): his clubs and clothes set for the
// current profile's slot and his textures loaded again with the front end's swap callbacks (into
// his other texture set, then switched).
void FE_StreamFunc_SwapTexturesInit(void) {
    Character_SetClubsAndClothes(gpCrAPState->pB4->pChar, lbl_80281ED4->nSlot);
    Character_AddTextureLoadRequest(gpCrAPState->pB4->pChar, Character_BeginSwapTexturesCallbackFE,
                                    Character_EndSwapTexturesCallbackFE);
}

// Clear the texture loader's list of textures in use (fn_8010BEC4).
void FE_StreamFunc_SwapTexturesClose(void) {
    fn_8010BEC4();
}

// Wait for the texture load (fn_8010BFE0), then finish the state; after an interrupt the shown
// golfer is marked not loaded (b18).
void FE_StreamFunc_SwapTexturesUpdate(void) {
    if (!fn_8010BFE0()) {
        if (gFEStreamStateMgr.bAbort) {
            gpCrAPState->pB4->b18 = 0;
        }
        FE_StreamPopState();
    }
}

// Drop the texture jobs (fn_8010B9BC).
void FE_StreamFunc_SwapTexturesInterrupt(void) {
    fn_8010B9BC();
}

// Interrupt the loader's running state: bAbort is set and the state's abort handler called; its
// update then finishes it early.
void FE_StreamInterruptState(void) {
    gFEStreamStateMgr.bAbort = 1;
    // port: EA passes an argument the abort handlers (FE_StreamFunc_IdleInterrupt,
    //       FE_StreamFunc_SkinInterrupt, FE_StreamFunc_TexturesInterrupt,
    //       FE_StreamFunc_SwapTexturesInterrupt) ignore
    ((void (*)(int))gFEStreamStates[gFEStreamStateMgr.nState].pfnAbort)(0);
}

void FE_StreamSetNextState(int nNext) {
    gFEStreamStateMgr.nNext = nNext;
}

// Interrupt the loader and run it until it is back in its idle state (1).
void FE_StreamStopAllStreaming(void) {
    FE_StreamSetNextState(1);
    FE_StreamInterruptState();
    FE_StreamWaitForState(1);
}

// Start the loader's state machine in its idle state (1), not paused.
void FE_StreamInitStateMgr(void) {
    gFEStreamStateMgr.nState = 1;
    gFEStreamStateMgr.nNext = gFEStreamStates[gFEStreamStateMgr.nState].nNext;
    gFEStreamStateMgr.bDone = 0;
    gFEStreamStateMgr.bEnter = 1;
    gFEStreamStateMgr.bAbort = 0;
    gFEStreamStateMgr.bPaused = 0;
}

// Run the golfer loader's state machine until it reaches state nState (a busy wait; fn_80007254,
// called each turn, is empty).
void FE_StreamWaitForState(int nState) {
    while (gFEStreamStateMgr.nState != nState) {
        FE_StreamUpdateState();
        fn_80007254();
    }
}

// The same as FE_StreamStopAllStreaming (interrupt the loader and wait until it is idle), in a copy
// of its own that only FE_CharMgrClose calls.
void FE_StreamStopForClose(void) {
    FE_StreamSetNextState(1);
    FE_StreamInterruptState();
    FE_StreamWaitForState(1);
}

// Mark the loader's running state finished: FE_StreamUpdateState leaves it for the next one.
void FE_StreamPopState(void) {
    gFEStreamStateMgr.bDone = 1;
}

// Run the golfer loader's state machine once (each front-end frame, gomainloop): enter the running
// state, update it, and when it is finished (FE_StreamPopState) leave it for the next one. Nothing
// runs while it is paused or in state 0.
void FE_StreamUpdateState(void) {
    // port: EA passes an argument the state handlers (gFEStreamStates) ignore
    if (gFEStreamStateMgr.bPaused == 0 && gFEStreamStateMgr.nState != 0) {
        if (gFEStreamStateMgr.bEnter) {
            ((void (*)(int))gFEStreamStates[gFEStreamStateMgr.nState].pfnEnter)(0);
            gFEStreamStateMgr.bEnter = 0;
        }
        ((void (*)(int))gFEStreamStates[gFEStreamStateMgr.nState].pfnUpdate)(0);
        if (gFEStreamStateMgr.bDone) {
            ((void (*)(int))gFEStreamStates[gFEStreamStateMgr.nState].pfnExit)(0);
            gFEStreamStateMgr.nState = gFEStreamStateMgr.nNext;
            gFEStreamStateMgr.nNext = gFEStreamStates[gFEStreamStateMgr.nState].nNext;
            gFEStreamStateMgr.bDone = 0;
            gFEStreamStateMgr.bEnter = 1;
            gFEStreamStateMgr.bAbort = 0;
        }
    }
}

// Pause the golfer loader (or let it run on); returns the old setting. DiscCheck.c pauses it.
u8 FE_PauseFECharStreaming(u8 bPaused) {
    u8 bOld = gFEStreamStateMgr.bPaused;

    gFEStreamStateMgr.bPaused = bPaused;
    return bOld;
}

// The loader's running state: 1 idle, 2 skin (streaming), 3 textures, 4 texture swap (0: stopped).
int FE_StreamGetCurrentState(void) {
    return gFEStreamStateMgr.nState;
}

// Each front-end frame, after FE_vUpdateGolferAll: update the current view and take its camera into
// the golfer display: an identity matrix in mC0, the camera's position and look point (View v0,
// v10) in v100 and v110, and the screen kind's (n0) pair of vectors (lbl_80189A40, 60, 80) in v120
// and v130.
void FE_SetupCamera(void) {
    View* pView;

    pView = ViewController_GetCameraControl(ViewController_GetCurrentViewControllerID());
    ViewController_Update(ViewController_GetCurrentViewControllerID());
    LLMath_IdentifyMat(gpCrAPState->mC0);
    LLMath_CopyVec(pView->v0, gpCrAPState->v100);
    LLMath_CopyVec(pView->v10, gpCrAPState->v110);
    switch (gpCrAPState->n0) {
    case 0:
        LLMath_CopyVec(lbl_80189A40[0], gpCrAPState->v120);
        LLMath_CopyVec(lbl_80189A40[1], gpCrAPState->v130);
        break;
    case 1:
    case 3:
    case 4:
        LLMath_CopyVec(lbl_80189A60[0], gpCrAPState->v120);
        LLMath_CopyVec(lbl_80189A60[1], gpCrAPState->v130);
        break;
    case 2:
        LLMath_CopyVec(lbl_80189A80[0], gpCrAPState->v120);
        LLMath_CopyVec(lbl_80189A80[1], gpCrAPState->v130);
        break;
    }
}

// Each front-end frame, before FE_SetupCamera and the render passes: fade the golfer display (f14C,
// 0..0.5) in and out with his animation; on screen kind 3 (create-a-player) run its queued
// animation (n1C0), and let the pad turn him (FE_RotateCrAPModel) and zoom (FE_ZoomCrAPModel); set
// him up again for a new screen kind (FE_SetupCharState) and place him (gFEGolferPos; in the club
// close-up raised by his club's offset); turn to the next golfer when his animation ends (b91);
// give a golfer shown afresh his ball logo and textures; then animate, pose and light him.
void FE_vUpdateGolferAll(void) {
    f32 vSaved[4];
    LightParams params;
    LightParams* pLight;
    View* pView;
    Character* pChar;
    f32 fEnd;
    f32 fTime;
    f32 f180;
    f32 fStart;
    f32 fBlend;
    f32 fHalf;
    f32 fLeft;
    f32 fFade;
    int i;

    fBlend = 1.0f;
    pView = ViewController_GetCameraControl(ViewController_GetCurrentViewControllerID());
    fn_8008F24C();
    SKN_BeginFrame();
    for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
        if (gpCrAPState->aGolfer[i].b19 || gpCrAPState->b8A) {
            gpCrAPState->aGolfer[i].b18 = 0;
        }
    }
    if (gpCrAPState->b8A || (gpCrAPState->pB4 != NULL && gpCrAPState->pB4->b19)) {
        gpCrAPState->pB4->b18 = 0;
    }
    // EA bug: without a character fEnd, fTime and f180 (and fStart) are read unset below.
    if (gpCrAPState->pB4->pChar != NULL) {
        fEnd = gpCrAPState->pB4->pChar->fAnimEnd;
        fTime = gpCrAPState->pB4->pChar->fAnimTime;
        f180 = gpCrAPState->pB4->pChar->fAnimStart;
    }
    if (gpCrAPState->n0 == 0) {
        fFade = 1.0f;
        if (gpCrAPState->pB4->pChar != NULL) {
            fStart = fEnd - 1.0f;
        }
        if (fTime >= fStart) {
            gpCrAPState->f14C = 0.5f * ((fEnd - fTime) / fFade);
            if (gpCrAPState->f14C < 0.0f) {
                gpCrAPState->f14C = 0.0f;
            }
        } else {
            gpCrAPState->f14C = 0.5f * fTime / fFade;
            if (gpCrAPState->f14C > 0.5f) {
                gpCrAPState->f14C = 0.5f;
            }
        }
    } else if (gpCrAPState->n0 == 3 && gpCrAPState->pB4->pChar != NULL) {
        if (gpCrAPState->b1C8 == 1 && gpCrAPState->n1B4 != -1
            && (gpCrAPState->n1C0 == gpCrAPState->n1C4 || gpCrAPState->n1C0 == 4)) {
            gpCrAPState->b1C8 = 0;
        }
        if (gpCrAPState->n1C0 == 0) {
            if (fEnd - fTime < 0.6f) {
                gpCrAPState->n1C0 = 1;
            }
        } else if (gpCrAPState->n1C0 == 1 && gpCrAPState->sz20[0] != '\0') {
            gpCrAPState->f14C = 0.5f * gpCrAPState->f1CC / 0.5f;
            gpCrAPState->f1CC -= FRAME_TIME;
            if (gpCrAPState->f1CC < 0.0f) {
                if ((gpCrAPState->n8 == 2 && gpCrAPState->nC != 1) || gpCrAPState->nC == 2) {
                    if (fn_800484F4(gpCrAPState->sz54) >= 0) {
                        fn_800B9EB8(gpCrAPState->sz54);
                    } else {
                        fn_800B9EB8(NULL);
                    }
                    FE_SetCrapRotation(0, 0.0f);
                    FE_ResetCrAPZoom();
                    gpCrAPState->n1C0 = 2;
                    FE_vTriggerCrAPAnimAndCamera(gpCrAPState->sz20, gpCrAPState->sz30, 0);
                    if (gpCrAPState->b80) {
                        gpCrAPState->n8 = gpCrAPState->nC;
                        GolfCamera_SwitchCrAPCamera(pView, NULL, 4, 0, 0, 0);
                    }
                } else if (gpCrAPState->n74 == 2 || gpCrAPState->b78 == 0) {
                    if (gpCrAPState->f7C <= 0.0f) {
                        Character_ExecuteTextureSwapFE(gpCrAPState->pB4->pChar);
                        gpCrAPState->b81 = 0;
                    }
                    FE_SetCrapRotation(0, 0.0f);
                    FE_ResetCrAPZoom();
                    gpCrAPState->n1C0 = 2;
                    FE_vTriggerCrAPAnimAndCamera(gpCrAPState->sz20, gpCrAPState->sz30, 0);
                    if (gpCrAPState->b80) {
                        gpCrAPState->n8 = gpCrAPState->nC;
                        if (gpCrAPState->n8 == 1) {
                            GolfCamera_SwitchCrAPCamera(pView, NULL, 3, 0, 0, 0);
                        } else if (gpCrAPState->n8 == 2) {
                            GolfCamera_SwitchCrAPCamera(pView, NULL, 4, 0, 0, 0);
                        }
                    }
                    if (gpCrAPState->n1BC >= 0) {
                        Character_SelectClub(gpCrAPState->pB4->pChar, gpCrAPState->n1BC);
                    }
                }
            }
        } else if (gpCrAPState->n1C0 == 2) {
            if (gpCrAPState->n74 == 2 && (gpCrAPState->b78 == 0 || fTime > gpCrAPState->f7C)) {
                gpCrAPState->f7C = 0.0f;
                Character_ExecuteTextureSwapFE(gpCrAPState->pB4->pChar);
                gpCrAPState->b81 = 0;
            }
            if (fTime >= fEnd - 0.5f) {
                fLeft = fEnd - fTime;
                gpCrAPState->f14C = 0.5f * (fLeft / 0.5f);
                if (gpCrAPState->f14C <= 0.01f || fLeft < FRAME_TIME) {
                    gpCrAPState->f14C = 0.0f;
                    gpCrAPState->n1C0 = 3;
                    if (gpCrAPState->b80) {
                        gpCrAPState->n8 = 0;
                        gpCrAPState->b80 = 0;
                    }
                    if (gpCrAPState->n1BC >= 0) {
                        Character_SelectClub(gpCrAPState->pB4->pChar, gpCrAPState->n1B8);
                        gpCrAPState->n1BC = -1;
                    }
                    FE_vLoadNextCrAPAnim(0);
                }
                if (gpCrAPState->n1D0 == 0) {
                    gpCrAPState->f14C = 0.5f;
                }
            } else {
                gpCrAPState->f14C = 0.5f * fTime / 0.5f;
                if (gpCrAPState->f14C >= 0.5f) {
                    gpCrAPState->f14C = 0.5f;
                }
            }
        } else if (gpCrAPState->n1C0 == 3) {
            if (gpCrAPState->n74 == 2) {
                Character_ExecuteTextureSwapFE(gpCrAPState->pB4->pChar);
                gpCrAPState->b81 = 0;
            }
            gpCrAPState->f14C = 0.5f * fTime / 0.5f;
            if (gpCrAPState->f14C >= 0.5f) {
                gpCrAPState->n1C0 = 4;
                gpCrAPState->f14C = 0.5f;
            }
            if (gpCrAPState->n1D0 == 0) {
                gpCrAPState->f14C = 0.5f;
            }
        } else {
            if (gpCrAPState->n74 == 2) {
                Character_ExecuteTextureSwapFE(gpCrAPState->pB4->pChar);
                gpCrAPState->b81 = 0;
            }
            gpCrAPState->f14C = 0.5f;
        }
    } else {
        gpCrAPState->f14C = 0.5f;
    }
    gpCrAPState->f14C = (gpCrAPState->f14C < 0.0f) ? 0.0f
                       : (gpCrAPState->f14C > 0.5f) ? 0.5f : gpCrAPState->f14C;
    if (gpCrAPState->b83) {
        if (gpCrAPState->f14C > gFEDimAlphaMax) {
            gpCrAPState->f14C = gFEDimAlphaMax;
        }
        gpCrAPState->f140 = lbl_80281338;
        gpCrAPState->f144 = lbl_80281338;
        gpCrAPState->f148 = lbl_80281338;
    } else {
        gpCrAPState->f140 = 0.5f;
        gpCrAPState->f144 = 0.5f;
        gpCrAPState->f148 = 0.5f;
    }
    if (fEnd - f180 < 0.5f) {
        fBlend = (fEnd - f180) / 2.0f;
    }
    fLeft = fEnd - fTime;
    fHalf = fBlend / 2.0f;
    if (fLeft > fHalf) {
        gpCrAPState->b82 = 0;
    }
    if (gpCrAPState->pB4->pChar != NULL
        && (gpCrAPState->n0 == 2 || gpCrAPState->n0 == 1 || gpCrAPState->n0 == 4)
        && fLeft < fHalf && gpCrAPState->b82 == 0 && gpCrAPState->pB4->pChar->nClubClass == 3) {
        gpCrAPState->b82 = 1;
        Character_PlayClip(gpCrAPState->pB4->pChar, gpCrAPState->pB4->pChar->pCurClip, 0, 0.5f);
    } else if (gpCrAPState->pB4->pChar != NULL && gpCrAPState->n0 == 3 && fLeft < fHalf
               && gpCrAPState->b82 == 0 && gpCrAPState->n1C0 == 4 && gpCrAPState->n8 == 0) {
        gpCrAPState->b82 = 1;
        FE_vLoadNextCrAPAnim(1);
    }
    // Screen kind 3: the pad turns the golfer (buttons 0x33 and 0x34) and button 0x35 does
    // FE_ZoomCrAPModel.
    if (gpCrAPState->pB4->pChar != NULL && gpCrAPState->n0 == 3) {
        if (gpCrAPState->b1C8 == 0) {
            if (Controller_AnyPadHasButtons(Controller_GetButtonMask(0x33, 0))
                || Controller_AnyPadHasButtons(Controller_GetButtonMask(0x33, 1))) {
                FE_RotateCrAPModel(0.05f);
            } else if (Controller_AnyPadHasButtons(Controller_GetButtonMask(0x34, 0))
                       || Controller_AnyPadHasButtons(Controller_GetButtonMask(0x34, 1))) {
                FE_RotateCrAPModel(-0.05f);
            } else {
                FE_RotateCrAPModel(0.0f);
            }
        } else {
            FE_RotateCrAPModel(0.0f);
        }
        if (gpCrAPState->b1C8 == 0 && gpCrAPState->n8 == 0
            && (Controller_AnyPadHasButtons(Controller_GetButtonMask(0x35, 0))
                || Controller_AnyPadHasButtons(Controller_GetButtonMask(0x35, 1)))) {
            FE_ZoomCrAPModel(1);
        } else {
            FE_ZoomCrAPModel(0);
        }
    }
    if (gpCrAPState->pB4->b18 && gpCrAPState->b86 == 0) {
        if (gpCrAPState->pB4->n1C != gpCrAPState->n0) {
            FE_SetupCharState();
        }
        if (gpCrAPState->n8 == 1) {
            // Raise him by his club class's amount while he is placed, then put the spot back.
            LLMath_CopyVec(gFEGolferPos, vSaved);
            if (gpCrAPState->pB4->pChar->nClubClass == 3) {
                gFEGolferPos[1] += 0.13166f;
            } else if (gpCrAPState->pB4->pChar->nClubClass == 4) {
                gFEGolferPos[1] += 0.19583f;
            } else if (gpCrAPState->pB4->pChar->nClubClass == 5) {
                gFEGolferPos[1] += 0.21944f;
            } else if (gpCrAPState->pB4->pChar->nClubClass == 2) {
                gFEGolferPos[1] += 0.23167f;
            }
            Character_SetPosition(gpCrAPState->pB4->pChar, gFEGolferPos, 1);
            LLMath_CopyVec(vSaved, gFEGolferPos);
        } else {
            Character_SetPosition(gpCrAPState->pB4->pChar, gFEGolferPos, 1);
        }
    }
    // His animation has ended: show the next golfer of the ring and pick the one after it from
    // gFEGolferCycle.
    if (gpCrAPState->b91 && fLeft < FRAME_TIME) {
        if (gpCrAPState->pB4->pNext->b18) {
            if (gpCrAPState->pB4->nC != gpCrAPState->pB4->pNext->nC) {
                gpCrAPState->pB4 = gpCrAPState->pB4->pNext;
                gPlayers[0].pChar = gpCrAPState->pB4->pChar;
            } else {
                gpCrAPState->pB4 = gpCrAPState->pB4->pPrev;
                gPlayers[0].pChar = gpCrAPState->pB4->pChar;
            }
            if (gpCrAPState->n0 == 0 || gpCrAPState->n0 == 2) {
                gSession.nGolfer[0] = gpCrAPState->pB4->nC;
            }
        } else if (FE_StreamGetCurrentState() != 1) {
            gpCrAPState->pB4 = gpCrAPState->pB8;
            gpCrAPState->pB4->b18 = 0;
        }
        gpCrAPState->pB4->pNext->nC = gFEGolferCycle[gpCrAPState->n198][gpCrAPState->n194];
        gpCrAPState->n194 = gpCrAPState->n194 + 1;
        if (gpCrAPState->n194 >= 5) {
            gpCrAPState->n194 = 0;
            gpCrAPState->n198 = gpCrAPState->n198 + 1;
            if (gpCrAPState->n198 >= 4) {
                gpCrAPState->n198 = 0;
            }
        }
        gpCrAPState->pB4->pNext->b18 = 0;
    }
    if (gFEGolferEnabled == 0) {
        return;
    }
    if (gpCrAPState->pB4->pChar != NULL) {
        gpCrAPState->pB4->pChar->pfnPreBones = FE_CharPositionOverwrite;
    }
    // Another golfer or profile slot than last drawn: give him his ball and textures.
    if ((gFELastDrawnGolfer != gpCrAPState->pB4->nC || gFELastDrawnSlot != lbl_80281ED4->nSlot || gpCrAPState->b87)
        && gpCrAPState->pB4->b18 && gpCrAPState->b86 == 0) {
        pChar = gpCrAPState->pB4->pChar;
        if (gpCrAPState->n0 == 0) {
            gSession.nGolfer[0] = gpCrAPState->pB4->nC;
        }
        fn_80008380();
        if (gpCrAPState->pB4->nC == 7 || gpCrAPState->pB4->nC == 29) {
            if (FE_GetCurrentProfile()->nGolferOutfit >= 0) {
                fn_800B9EB8(fn_800484E0(FE_GetCurrentProfile()->nGolferOutfit));
            } else {
                fn_800B9EB8(NULL);
            }
        } else {
            fn_800B9EB8(fn_800484E0(gGolferTable[gpCrAPState->pB4->nC].nOutfit));
        }
        gpCrAPState->b87 = 0;
        Character_ExecuteTextureSwapFE(pChar);
        for (i = 0; i < pChar->nSkins; i++) {
            SkinPart_SetupMaterials(pChar->apSkins[i], pChar->apDynTex[pChar->nCurDynTex]);
        }
        fn_8010BC64(pChar->apDynTex[pChar->nCurDynTex]);
    }
    if (gpCrAPState->pB4->b18 && gpCrAPState->b86 == 0) {
        fn_80035600();
        Character_UpdateAnimation(gpCrAPState->pB4->pChar, 1, 1.0f / 60.0f);
        if (gpCrAPState->pB4->pChar->bPosed == 0) {
            SKN_PoseCharacter(gpCrAPState->pB4->pChar, 0);
        }
        LF_vSetCurrentLightFogEnvironment(0);
        if (gpCrAPState->pB4->pChar->p44 != NULL) {
            // Screen kind 1 with b83 set lights him with all-zero settings; otherwise with his
            // own, which sit in p44's entry 21 (port: read as LightParams, both 0x30 bytes).
            pLight = (LightParams*)&gpCrAPState->pB4->pChar->p44[21];
            if (gpCrAPState->n0 == 1 && gpCrAPState->b83) {
                Mem_set(&params, 0, sizeof(params));
                fn_80093854(&params);
            } else {
                fn_80093854(pLight);
            }
        }
        LF_LoadCurrentLights();
    }
    fn_80035308();
}

// The golfer's first render pass, before the menu is drawn, when he is shown (b18, not hidden by
// b86) and drawn off screen (gFEOffscreenBufferRender): clear the frame (FE_ClearGolferFrame), draw him and the
// ball he holds, set the frame's alpha (FE_DrawGolferAlphaMask), copy him into the screen-copy
// texture (FE_CopyGolferToTexture) and clear the frame again. b18C notes that this pass drew him.
void FE_vRenderGolferAllPhase1(void) {
    if (gpCrAPState->pB4->b18 && gpCrAPState->b86 == 0 && gpCrAPState->b88 == 0 && gFEOffscreenBufferRender != 0) {
        gpCrAPState->b18C = 1;
        FE_BeginRenderGolferPhase1();
        FE_ClearGolferFrame();
        FE_RenderGolfer(0);
        fn_800B9CF0(0);
        FE_DrawGolferAlphaMask();
        FE_CopyGolferToTexture();
        FE_ClearGolferFrame();
    }
}

// The golfer's second render pass, after the menu is drawn: lay his copy from
// FE_vRenderGolferAllPhase1 over the menu (FE_DrawGolferTexture) or, when he is not drawn off
// screen (gFEOffscreenBufferRender), draw him and his ball straight into the frame.
void FE_vRenderGolferAllPhase2(void) {
    if (gpCrAPState->pB4->b18 && gpCrAPState->b86 == 0 && gpCrAPState->b88 == 0) {
        if (gFEOffscreenBufferRender != 0) {
            FE_DrawGolferTexture();
            return;
        }
        gpCrAPState->b18C = 0;
        FE_RenderGolfer(0);
        fn_800B9CF0(0);
    }
}

// Empty in this build: FE_vRenderGolferAllPhase1 calls it first when it draws the menu golfer off
// screen, before FE_ClearGolferFrame.
void FE_BeginRenderGolferPhase1(void) {
}

// Clear the 512 x 448 frame to transparent black: a full-frame quad of (0, 0, 0, 0) with colour and
// alpha written and the depth test always passing (and writing). Then the frame is set up again the
// usual way (colour writes only, as gomainloop.c does).
void FE_ClearGolferFrame(void) {
    f32 xy[8] = { 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    f32 colour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    DS_vSetAlphaTestMode(0, 6, 0x80);
    DS_vSetZBufferMode(7);
    DS_vEnableZBufferUpdate(1);
    RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 1, 1);
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RenderView_SetUseCurrentMatrices(0);
    RenderView_SetColor(colour);
    RenderState_SetDrawFlags(0);
    RenderState_Flush();
    RenderView_DrawPrimitive(0xA1, xy, 0, NULL, 2);
    DS_vSetZBufferMode(3);
    DS_vEnableZBufferUpdate(1);
    RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RenderState_Flush();
}

// Draw the golfer's screen copy (gFEScreenCopyTex) over the menu as a textured quad, grey with alpha
// f14C (the display's fade, 0..0.5).
void FE_DrawGolferTexture(void) {
    f32 xy[8] = { 0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f };
    f32 colour[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
    f32 uv[8] = { 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

    DS_vSetAlphaTestMode(0, 6, 0x80);
    DS_vSetZBufferMode(7);
    RenderState_SetConstantAlphaOn(0);
    RenderState_SetBlendFactors(4, 5);
    RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RenderState_SetDrawFlags(0x50);
    RenderView_SetUseCurrentMatrices(0);
    colour[3] = gpCrAPState->f14C;
    RenderView_SetColor(colour);
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_8002A608(&gFEScreenCopyTex);
    RenderState_Flush();
    RenderView_DrawPrimitive(0xA1, xy, 0, uv, 2);
    fn_80016B54(512, 448, 1.0f, 1.0f);
    RenderState_Flush();
}

// Set the frame's alpha before the golfer is copied to his texture (FE_vRenderGolferAllPhase1):
// with only alpha written, draw a quad of black at half alpha with the depth test off, then a
// full-frame one of (0, 0, 0, 0) tested against the depth he left (compare 3). Then the frame is
// set up again the usual way.
void FE_DrawGolferAlphaMask(void) {
    f32 xy2[8] = { 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    f32 xy1[8] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f };
    f32 colour1[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
    f32 colour2[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    DS_vSetAlphaTestMode(0, 6, 0x80);
    DS_vSetZBufferMode(7);
    DS_vEnableZBufferUpdate(0);
    RenderState_SetBlendFactors(4, 5);
    RenderState_SetConstantAlphaOn(0);
    RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 4, 1);
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RenderView_SetUseCurrentMatrices(0);
    RenderView_SetColor(colour1);
    RenderState_SetDrawFlags(0);
    RenderState_Flush();
    RenderView_DrawPrimitive(0xA1, xy1, 0, NULL, 2);
    RenderView_SetColor(colour2);
    DS_vSetZBufferMode(3);
    RenderState_Flush();
    RenderView_DrawPrimitive(0xA1, xy2, 0, NULL, 2);
    DS_vSetZBufferMode(3);
    DS_vEnableZBufferUpdate(1);
    RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RenderState_Flush();
}

// Copy the frame buffer's golfer (from (128, 0), 384 x 448) into the screen-copy texture.
void FE_CopyGolferToTexture(void) {
    GXPixModeSync();
    GXSetTexCopySrc(128, 0, 384, 448);
    GXSetTexCopyDst(384, 448, 6, 0);           // RGBA8, no mipmap
    GXCopyTex(fn_8002A624(), 0);
    GXPixModeSync();
    GXInvalidateTexAll();
}

// Draw the golfer shown (loaded, b18, and not hidden, b86) with the display's camera (mC0): his
// body, or only his club in the club close-up (n8 1); on screen kind 3 the scissor keeps the top
// lbl_80281348 of the frame. bFull: into a 384 x 528 surface instead of the 512 x 448 frame (both
// passes pass 0). Then note which golfer and profile slot were drawn (gFELastDrawnGolfer, gFELastDrawnSlot).
void FE_RenderGolfer(u8 bFull) {
    if (gFEGolferEnabled == 0) {
        return;
    }
    if (gpCrAPState->pB4->b18 && gpCrAPState->b86 == 0) {
        LI_SetObjectLights(NULL);
        RC_vSetCurrentRenderCtxTransformationMatrix(gpCrAPState->mC0);
        RC_UpdateCurrentScreenMatrices();
        RC_vUpdateRenderCtxTransformationMatrices(RC_spGetCurrentRenderCtx());
        RenderState_SetCameraMatrices();
        RenderState_SetClipMode(1);
        DS_vSetAlphaTestMode(1, 6, 1);
        if (bFull) {
            RenderState_SetRenderSurface(1, 384, 528, 0, 1, 1);
        } else {
            RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 1, 1);
        }
        RenderState_SetViewport(RC_spGetCurrentRenderCtx());
        RenderState_Flush();
        DS_vEnableZBufferUpdate(1);
        DS_vSetZBufferMode(3);
        RenderState_Flush();
        if (gpCrAPState->n0 == 3) {
            fn_800760B0(0, 0, 512, 448.0f * lbl_80281348);
        }
        RenderState_Flush();
        if (gpCrAPState->n8 == 0) {
            SKN_DrawCharacterParts(gpCrAPState->pB4->pChar);
        } else if (gpCrAPState->n8 == 1) {
            SKN_DrawClubParts(gpCrAPState->pB4->pChar);
        }
        LI_ResetLights();
        RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 8, 1);
        RenderState_SetViewport(RC_spGetCurrentRenderCtx());
        RenderState_Flush();
    }
    if (gpCrAPState->pB4->b18 && gpCrAPState->b86 == 0) {
        gFELastDrawnGolfer = gpCrAPState->pB4->nC;
        gFELastDrawnSlot = lbl_80281ED4->nSlot;
    }
}

// Set the golfer shown up for the screen kind (n0; n1C notes the kind he was set up for): facing
// front, and on kinds 0, 1, 2 and 4 his morph state reset, a club selected (5 or 3), his first clip
// (Char_SetClip 0) played and the club switched to the one that clip holds (its u90 in
// gClubBoneIds; 0 if none); kinds 1 and 4 take the profile's handedness
// (FE_CRAPSetHandednessForScreen). Kind 3 (create-a-player) clears its queued animation and
// close-up state and starts his idle animation (FE_vLoadNextCrAPAnim). The display then fades in
// from 0 (f14C).
void FE_SetupCharState(void) {
    Clip* pClip;
    int i;

    gpCrAPState->pB4->n1C = gpCrAPState->n0;
    gpCrAPState->n8 = 0;
    gpCrAPState->b80 = 0;
    gpCrAPState->n74 = 0;
    gpCrAPState->b78 = 0;
    gpCrAPState->b81 = 0;
    gpCrAPState->b1D1 = 1;
    switch (gpCrAPState->n0) {
    case 0:
        gpCrAPState->f19C = 0.0f;
        gpCrAPState->f1A0 = 0.0f;
        CharacterState_ResetMorphState(gpCrAPState->pB4->pChar, 1);
        fn_800957B0(gpCrAPState->pB4->pChar, 1);
        Character_SelectClub(gpCrAPState->pB4->pChar, 5);
        pClip = Char_SetClip(gpCrAPState->pB4->pChar, 0, 0, NULL);
        Character_PlayClip(gpCrAPState->pB4->pChar, pClip, 1, 0.0f);
        for (i = 0; i < 6; i++) {
            if (pClip->u90 == gClubBoneIds[i]) {
                break;
            }
        }
        if (i == 6) {
            i = 0;
        }
        Character_SelectClub(gpCrAPState->pB4->pChar, i);
        Character_SetOrientation(gpCrAPState->pB4->pChar, gpCrAPState->f19C);
        gpCrAPState->b85 = 1;
        gpCrAPState->b91 = 1;
        gpCrAPState->b84 = 1;
        gpCrAPState->b83 = 0;
        break;
    case 1:
    case 4:
        gpCrAPState->f19C = 0.0f;
        gpCrAPState->f1A0 = 0.0f;
        FE_CRAPSetHandednessForScreen(FE_GetCurrentProfile()->choices.n113);
        CharacterState_ResetMorphState(gpCrAPState->pB4->pChar, 1);
        fn_800957B0(gpCrAPState->pB4->pChar, 1);
        Character_SelectClub(gpCrAPState->pB4->pChar, 3);
        pClip = Char_SetClip(gpCrAPState->pB4->pChar, 0, 0, NULL);
        Character_PlayClip(gpCrAPState->pB4->pChar, pClip, 1, 0.0f);
        for (i = 0; i < 6; i++) {
            if (pClip->u90 == gClubBoneIds[i]) {
                break;
            }
        }
        if (i == 6) {
            i = 0;
        }
        Character_SelectClub(gpCrAPState->pB4->pChar, i);
        Character_SetOrientation(gpCrAPState->pB4->pChar, gpCrAPState->f19C);
        gpCrAPState->b85 = 0;
        gpCrAPState->b91 = 0;
        gpCrAPState->b84 = 0;
        break;
    case 3:
        gpCrAPState->b78 = 1;
        gpCrAPState->f7C = 0.0f;
        gpCrAPState->b85 = 0;
        gpCrAPState->b91 = 0;
        gpCrAPState->b84 = 0;
        gpCrAPState->n1B4 = 0;
        gpCrAPState->n1B8 = 0;
        gpCrAPState->n1BC = -1;
        gpCrAPState->sz20[0] = '\0';
        gpCrAPState->sz30[0] = '\0';
        gpCrAPState->n1D0 = 0;
        gpCrAPState->n1C0 = 4;
        gpCrAPState->b1C8 = 0;
        gpCrAPState->n50 = 0;
        gpCrAPState->b1DC = 0;
        FE_SetLastCrAPAsset(-1);
        FE_SetLastCrAPCategory(-1);
        FE_CRAPSetHandednessForScreen(0);
        gpCrAPState->f19C = 0.0f;
        gpCrAPState->f1A0 = 0.0f;
        FE_vLoadNextCrAPAnim(0);
        Character_SetOrientation(gpCrAPState->pB4->pChar, gpCrAPState->f19C);
        break;
    case 2:
        gpCrAPState->f19C = 0.0f;
        gpCrAPState->f1A0 = 0.0f;
        CharacterState_ResetMorphState(gpCrAPState->pB4->pChar, 1);
        fn_800957B0(gpCrAPState->pB4->pChar, 1);
        Character_SelectClub(gpCrAPState->pB4->pChar, 3);
        pClip = Char_SetClip(gpCrAPState->pB4->pChar, 0, 0, NULL);
        Character_PlayClip(gpCrAPState->pB4->pChar, pClip, 1, 0.0f);
        for (i = 0; i < 6; i++) {
            if (pClip->u90 == gClubBoneIds[i]) {
                break;
            }
        }
        if (i == 6) {
            i = 0;
        }
        Character_SelectClub(gpCrAPState->pB4->pChar, i);
        Character_SetOrientation(gpCrAPState->pB4->pChar, gpCrAPState->f19C);
        gpCrAPState->b85 = 0;
        gpCrAPState->b91 = 0;
        gpCrAPState->b84 = 0;
        gpCrAPState->b83 = 0;
        break;
    }
    gpCrAPState->f14C = 0.0f;
}

// The shown golfer's pre-bones callback (pfnPreBones, set by FE_vUpdateGolferAll), with b85 set:
// pins him to his spot by zeroing the waist bone's (bone 1) x, z and w, and moves the club bone
// (0x52, when the character's bit 0x4000 is set) and the ball bone (0x54, when he holds the ball:
// Character_IsHoldingBall) with it, keeping their offsets from the waist.
void FE_CharPositionOverwrite(void) {
    f32 vClubPos[4];
    f32 vBallPos[4];
    CharModel* pModel;
    Bone* pWaistBone;
    Bone* pClubBone;
    Bone* pBallBone;
    u8 bBallExists;

    pModel = gpCrAPState->pB4->pChar->pModel;
    pWaistBone = &pModel->pBones[CharModel_GetBoneIndex(pModel, 1)];
    pClubBone = &pModel->pBones[CharModel_GetBoneIndex(pModel, 0x52)];
    CharModel_GetBoneIndex(pModel, 0x54);              // EA looks bone 0x54 up here without using it
    bBallExists = Character_IsHoldingBall(gpCrAPState->pB4->pChar);
    if (gpCrAPState->b85 == 0) {
        return;
    }
    if (bBallExists) {
        pBallBone = &pModel->pBones[CharModel_GetBoneIndex(pModel, 0x54)];
    }
    if (gpCrAPState->pB4->pChar->uCharFlags & 0x4000) {
        FE_Vec4Sub(pClubBone->v1C, pWaistBone->v1C, vClubPos);
    }
    if (bBallExists) {
        FE_Vec4Sub(pBallBone->v1C, pWaistBone->v1C, vBallPos);
    }
    // EA's code subtracts each value from itself, which zeroes it.
    if (gpCrAPState->pB4->pChar->uCharFlags & 0x4000) {
        pClubBone->v1C[0] -= pClubBone->v1C[0];
        pClubBone->v1C[2] -= pClubBone->v1C[2];
        pClubBone->v1C[3] -= pClubBone->v1C[3];
    }
    if (bBallExists) {
        pBallBone->v1C[0] -= pBallBone->v1C[0];
        pBallBone->v1C[2] -= pBallBone->v1C[2];
        pBallBone->v1C[3] -= pBallBone->v1C[3];
    }
    pWaistBone->v1C[0] -= pWaistBone->v1C[0];
    pWaistBone->v1C[2] -= pWaistBone->v1C[2];
    pWaistBone->v1C[3] -= pWaistBone->v1C[3];
    if (gpCrAPState->pB4->pChar->uCharFlags & 0x4000) {
        pClubBone->v1C[0] += vClubPos[0];
        pClubBone->v1C[2] += vClubPos[2];
        pClubBone->v1C[3] += vClubPos[3];
    }
    if (bBallExists) {
        pBallBone->v1C[0] += vBallPos[0];
        pBallBone->v1C[2] += vBallPos[2];
        pBallBone->v1C[3] += vBallPos[3];
    }
}

// Set up the menu golfer when the front end starts (FE_Manager): register the golfer stream client,
// set up the golfer's state and its loader (FE_CharMgrInit) and make the display's textures
// (FE_InitGolferTextures).
void FE_vInitFECharModule(void) {
    Character_RegisterGolferStreamClientFE();
    FE_CharMgrInit();
    FE_InitGolferTextures();
}

// Make the golfer display's textures, all 384 x 448 RGBA8: one on each of lbl_80281BA4's two
// buffers and the screen copy (gFEScreenCopyTex, on fn_8002A624's pixels). Made again after a movie
// (FE_Manager) and when the create-a-player screen loads (uiProcessInterface.c).
void FE_InitGolferTextures(void) {
    fn_8002A528(&gFEBufferTextures[0], 384, 448, lbl_80281BA4[0], NULL, 6, 0, 0, 0);
    fn_8002A528(&gFEBufferTextures[1], 384, 448, lbl_80281BA4[1], NULL, 6, 0, 0, 0);
    fn_8002A528(&gFEScreenCopyTex, 384, 448, fn_8002A624(), NULL, 6, 0, 0, 0);
}

// Register the 'LITE' stream handler (FE_lite_vStreamCallback): the golfer display's lights.
void FE_lite_vRegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('LITE', FE_lite_vStreamCallback);
}

// A 'LITE' object: copy its lights (little-endian) into gFEGolferLights, swapping each value's
// bytes, and make them light set 0's.
void FE_lite_vStreamCallback(UStreamObject* pObject) {
    SwapField aLightHeaderDef[] = {
        { 4, 4 },                                           // nLights
        { 12, 4 },
    };
    SwapField aLightElemDef[] = {
        { 1, 1 },                                           // nType
        { 15, 1 },
        { 16, 4 },                                          // vColor
        { 16, 4 },                                          // vPos
    };
    void* pSrc;
    void* pDst;

    gFEGolferLights = StaticMem_Alloc(pObject->uSize, 2, 16, "FEgolferanim.c", 3143);
    pSrc = pObject->pData;
    pDst = gFEGolferLights;
    ByteSwap_Records(&pSrc, &pDst,
                     aLightHeaderDef, sizeof(aLightHeaderDef) / sizeof(aLightHeaderDef[0]), 1);
    ByteSwap_Records(&pSrc, &pDst,
                     aLightElemDef, sizeof(aLightElemDef) / sizeof(aLightElemDef[0]), gFEGolferLights->nLights);
    LF_vSetCurrentLightFogEnvironment(0);
    fn_800935CC(gFEGolferLights);
    fn_8003534C();
    StaticMem_Free(pObject);
}

// Drop the golfers loaded for the menus: b8A has FE_vFreeUnusedCharacters free them once the loader
// is idle, and meanwhile the loader is interrupted and sent back to idle, the load count (n190)
// cleared and the golfer shown marked not loaded. On screen kind 0 slot 0 gets the next golfer of
// gFEGolferCycle to load, on kind 4 none. The menus call it when the screen kind changes to 0 or 3,
// and before a movie.
void FE_vClearGolferCache(void) {
    gpCrAPState->b8A = 1;
    FE_StreamInterruptState();
    FE_StreamSetNextState(1);
    gpCrAPState->n190 = 0;
    gpCrAPState->pB4->b18 = 0;
    if (gpCrAPState->n0 == 0) {
        gpCrAPState->aGolfer[0].nC = gFEGolferCycle[gpCrAPState->n198][gpCrAPState->n194 % 5];
        gpCrAPState->n194++;
        if (gpCrAPState->n194 >= 5) {
            gpCrAPState->n194 = 0;
            gpCrAPState->n198++;
            if (gpCrAPState->n198 >= 4) {
                gpCrAPState->n198 = 0;
            }
        }
    } else if (gpCrAPState->n0 == 4) {
        gpCrAPState->aGolfer[0].nC = -1;
    }
}

// Flag every golfer slot's character to be freed (b19; FE_vFreeUnusedCharacters frees it) and mark
// the golfer shown not loaded (b18).
void FE_vExecuteClearGolferCache(void) {
    int i;

    for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
        gpCrAPState->aGolfer[i].b19 = 1;
        if (gpCrAPState->pB4 != NULL) {
            gpCrAPState->pB4->b18 = 0;
        }
    }
}

// Each frame (the main loop), while the loader is idle: finish a cache clear (b8A:
// FE_vExecuteClearGolferCache), else free slot 0's character when it is flagged (b19) and is not
// the golfer shown and loaded, and mark the slot empty.
void FE_vFreeUnusedCharacters(void) {
    if (FE_StreamGetCurrentState() == 1) {
        if (gpCrAPState->b8A && FE_StreamGetCurrentState() == 1) {
            FE_vExecuteClearGolferCache();
            gpCrAPState->b8A = 0;
            return;
        }
        if (gpCrAPState->aGolfer[0].b19
            && (gpCrAPState->pB4->b18 == 0 || &gpCrAPState->aGolfer[0] != gpCrAPState->pB4)
            && (FE_StreamGetCurrentState() == 1 || &gpCrAPState->aGolfer[0] != gpCrAPState->pB8)) {
            if (gpCrAPState->aGolfer[0].pChar != NULL) {
                fn_80008380();
                Character_Free(gpCrAPState->aGolfer[0].pChar);
            }
            gpCrAPState->aGolfer[0].b18 = 0;
            gpCrAPState->aGolfer[0].b19 = 0;
            gpCrAPState->aGolfer[0].pChar = NULL;
        }
    }
}

// Whether a golfer slot other than pGolfer holds golfer nGolfer, loaded (b18) or streamed (n14).
// Only slot 0 is looked at; with the game's one slot (CRAP_NUM_GOLFERS) every caller passes slot 0,
// so it returns 0.
u8 FE_IsGolferInOtherSlot(int nGolfer, CrAPGolfer* pGolfer) {
    if (&gpCrAPState->aGolfer[0] != pGolfer
        && ((gpCrAPState->aGolfer[0].nC == nGolfer && gpCrAPState->aGolfer[0].b18)
            || gpCrAPState->aGolfer[0].n14 == nGolfer)) {
        return 1;
    }
    return 0;
}

// Store whether profile nProfile's created golfer is left-handed (choices.n113); a create-a-player
// menu message sets it for the current profile.
void FE_SetProfileLeftHanded(int nProfile, s8 n) {
    gpSaveData[nProfile].choices.n113 = n;
}

// Start the shown golfer's next animation and switch the CrAP camera to it (bNoBlend: cut instead
// of blending). In a club or ball close-up (n8 1 or 2) that is his close-up clip (Char_SetClip 8)
// with camera 3 or 4. Otherwise it is his current clip again while n50 repeats are left, else the
// camera kind's idle clip (FE_CrapGetIdleAnim); n1B4 counts the idles, and after the fifth (not
// zoomed) the clip is picked with club 2 instead of 0, the zoom is reset and on camera kinds 0 and
// 2 he turns back to face front (b1C8 keeps the pad from turning him meanwhile). A clip with pD8
// set takes the profile's handedness (FE_CRAPSetHandednessForScreen; he is turned to face PI when he becomes
// left-handed), any other is played right-handed.
void FE_vLoadNextCrAPAnim(u8 bNoBlend) {
    View* pView;
    Clip* pClip;

    pView = ViewController_GetCameraControl(ViewController_GetCurrentViewControllerID());
    if (gpCrAPState->pB4 == NULL || gpCrAPState->pB4->pChar == NULL) return;
    if (gpCrAPState->n8 != 0) {
        if (gpCrAPState->n8 == 1) {
            Character_SelectClub(gpCrAPState->pB4->pChar, 0);
        } else {
            Character_SelectClub(gpCrAPState->pB4->pChar, 2);
        }
        pClip = Char_SetClip(gpCrAPState->pB4->pChar, 8, 0, NULL);
        gpCrAPState->sz10[0] = '\0';
        Character_PlayClip(gpCrAPState->pB4->pChar, pClip, !bNoBlend, 0.5f);
        Character_SelectClub(gpCrAPState->pB4->pChar, gpCrAPState->n1B8);
        if (gpCrAPState->n8 == 1) {
            GolfCamera_SwitchCrAPCamera(pView, NULL, 3, bNoBlend, 0, 0);
            return;
        }
        GolfCamera_SwitchCrAPCamera(pView, NULL, 4, bNoBlend, 0, 0);
        return;
    }
    if (gpCrAPState->n1B4 == 5 && gpCrAPState->b1DC == 0) {
        Character_SelectClub(gpCrAPState->pB4->pChar, 2);
        gpCrAPState->n1B4 = -1;
        if (gpCrAPState->n4 == 0 || gpCrAPState->n4 == 2) {
            gpCrAPState->b1C8 = 1;
            FE_SetCrapRotation(1, 0.0f);
        }
        FE_ZoomCrAPModel(0);
    } else {
        Character_SelectClub(gpCrAPState->pB4->pChar, 0);
        gpCrAPState->n1B4++;
        gpCrAPState->b1C8 = 0;
    }
    if (gpCrAPState->n50 > 0 && (pClip = gpCrAPState->pB4->pChar->pCurClip) != NULL) {
        gpCrAPState->n50--;
    } else {
        pClip = FE_CrapGetIdleAnim();
    }
    if (pClip->pD8 != NULL) {
        if (!Character_IsLeftHanded(gpCrAPState->pB4->pChar) && FE_GetCurrentProfile()->choices.n113 != 0) {
            FE_SetCrapRotation(0, PI);
        }
        FE_CRAPSetHandednessForScreen(FE_GetCurrentProfile()->choices.n113);
    } else {
        if (Character_IsLeftHanded(gpCrAPState->pB4->pChar)) {
            FE_SetCrapRotation(0, 0.0f);
        }
        FE_CRAPSetHandednessForScreen(0);
    }
    gpCrAPState->sz10[0] = '\0';
    Character_PlayClip(gpCrAPState->pB4->pChar, pClip, !bNoBlend, 0.5f);
    Character_SelectClub(gpCrAPState->pB4->pChar, gpCrAPState->n1B8);
    GolfCamera_SwitchCrAPCamera(pView, NULL, gpCrAPState->n4, bNoBlend, gpCrAPState->b1DC, 0);
}

// The shown golfer's idle clip for the CrAP camera kind (n4): clip 11 for kind 1, 7 for kind 2,
// else 1 (Char_SetClip). TW07's FE_CrapGetIdleAnimName returns its name instead.
Clip* FE_CrapGetIdleAnim(void) {
    if (gpCrAPState->n4 == 1) {
        return Char_SetClip(gpCrAPState->pB4->pChar, 11, 0, NULL);
    }
    if (gpCrAPState->n4 == 2) {
        return Char_SetClip(gpCrAPState->pB4->pChar, 7, 0, NULL);
    }
    return Char_SetClip(gpCrAPState->pB4->pChar, 1, 0, NULL);
}

// Turn the golfer by fTurn radians: f1A0 is where he should face, f19C where he faces; f19C
// follows it 0.05 a frame, the short way round.
void FE_RotateCrAPModel(f32 fTurn) {
    f32 fDiff;

    gpCrAPState->f19C += fTurn;
    gpCrAPState->f1A0 += fTurn;
    if (gpCrAPState->f19C < 0.0f) {
        gpCrAPState->f19C = 2.0f * PI + gpCrAPState->f19C;
    } else if (gpCrAPState->f19C > 2.0f * PI) {
        gpCrAPState->f19C = gpCrAPState->f19C - 2.0f * PI;
    }
    if (gpCrAPState->f1A0 < 0.0f) {
        gpCrAPState->f1A0 = 2.0f * PI + gpCrAPState->f1A0;
    } else if (gpCrAPState->f1A0 > 2.0f * PI) {
        gpCrAPState->f1A0 = gpCrAPState->f1A0 - 2.0f * PI;
    }
    fDiff = fabsf(gpCrAPState->f1A0 - gpCrAPState->f19C);
    if (fDiff < 2.0f * PI - fDiff) {
        if (gpCrAPState->f19C < gpCrAPState->f1A0 - 0.05f) {
            gpCrAPState->f19C += 0.05f;
        } else if (gpCrAPState->f19C > 0.05f + gpCrAPState->f1A0) {
            gpCrAPState->f19C -= 0.05f;
        }
    } else if (gpCrAPState->f19C < gpCrAPState->f1A0) {
        gpCrAPState->f19C = gpCrAPState->f19C - 0.05f;
        if (gpCrAPState->f19C < 0.0f) {
            gpCrAPState->f19C = 2.0f * PI + gpCrAPState->f19C;
        }
    } else if (gpCrAPState->f19C > gpCrAPState->f1A0) {
        gpCrAPState->f19C += 0.05f;
        if (gpCrAPState->f19C > 2.0f * PI) {
            gpCrAPState->f19C = gpCrAPState->f19C - 2.0f * PI;
        }
    }
    if (gpCrAPState->pB4->pChar != NULL) {
        Character_SetOrientation(gpCrAPState->pB4->pChar, gpCrAPState->f19C);
    }
}

// Forget the zoom (b1DC) without switching the camera.
void FE_ResetCrAPZoom(void) {
    gpCrAPState->b1DC = 0;
}

// Zoom the CrAP camera in (bZoom) or out: on a change, b1DC follows it and the camera is switched,
// blending.
void FE_ZoomCrAPModel(u8 bZoom) {
    View* pView = ViewController_GetCameraControl(ViewController_GetCurrentViewControllerID());

    if (bZoom) {
        if (gpCrAPState->b1DC != 1) {
            gpCrAPState->b1DC = 1;
            GolfCamera_SwitchCrAPCamera(pView, NULL, gpCrAPState->n4, 1, gpCrAPState->b1DC, 1);
        }
    } else if (gpCrAPState->b1DC != 0) {
        gpCrAPState->b1DC = 0;
        GolfCamera_SwitchCrAPCamera(pView, NULL, gpCrAPState->n4, 1, gpCrAPState->b1DC, 1);
    }
}

// Face the golfer fAngle radians round: at once, or (bTarget) over time (FE_RotateCrAPModel).
void FE_SetCrapRotation(u8 bTarget, f32 fAngle) {
    if (bTarget) {
        gpCrAPState->f1A0 = fAngle;
        return;
    }
    gpCrAPState->f19C = fAngle;
    gpCrAPState->f1A0 = fAngle;
    if (gpCrAPState->pB4->pChar != NULL) {
        Character_SetOrientation(gpCrAPState->pB4->pChar, gpCrAPState->f19C);
    }
}

// Empty in this build: the menu message FE_MessageTable.c fn_8007C254 calls it when it changes
// gpCrAPState->b86 (the golfer is not drawn while it is set) while the CrAP screen kind n0 is 3.
void FE_OnGolferHiddenChanged(void) {
}

// bOn: the menu golfer is drawn off screen, copied into the screen-copy texture
// (FE_vRenderGolferAllPhase1) and that is drawn faded by f14C (FE_vRenderGolferAllPhase2); 0 draws
// him straight into the frame. The CrAP screen-state message turns it on for screen kind 0 only.
void FE_SetOffscreenBufferRender(u8 bOn) {
    gFEOffscreenBufferRender = bOn;
}

// Set the CrAP camera's idle state (n4: 0 the "Crap Idle" shot, 1 the "Crap Face" shot, as the menu
// messages pass it). On a change the idle clip for the new state (FE_CrapGetIdleAnim) becomes the queued
// animation, due now with no fade at its end and the texture-swap delay off (FE_QueueCrAPAnim,
// FE_SetDelayTextureSwap), unless a queued animation is still waiting to start, whose name it then
// takes; the queued camera shot is dropped either way.
void FE_SetCrAPCameraIdleState(int nState) {
    Clip* pClip;

    if (nState != gpCrAPState->n4) {
        pClip = FE_CrapGetIdleAnim();
        if (pClip != NULL) {
            if (gpCrAPState->n1C0 != 1 && gpCrAPState->n1C0 != 0) {
                FE_QueueCrAPAnim(pClip->name, NULL, 0, 0);
                FE_SetDelayTextureSwap(0, 0.0f);
                gpCrAPState->n1C4 = 2;
            } else {
                strncpy(gpCrAPState->sz20, pClip->name, sizeof(gpCrAPState->sz20));
                gpCrAPState->n1C4 = 2;
            }
        }
        gpCrAPState->sz30[0] = '\0';
    }
    gpCrAPState->n4 = nState;
}

// 1 when the menu has a golfer slot (gpCrAPState->pB4) and that slot has its character made; 0
// otherwise.
int FE_HasGolferCharacter(void) {
    if (gpCrAPState->pB4 != NULL && gpCrAPState->pB4->pChar != NULL) {
        return 1;
    }
    return 0;
}

// Whether the golfer shown is loaded and ready to show (CrAPGolfer.b18, set once his textures are
// in).
int FE_IsGolferReady(void) {
    return gpCrAPState->pB4->b18 != 0;
}

// Play animation szAnim on the golfer shown and switch the CrAP camera to shot szShot, both blended
// in when bBlend (else cut). The camera takes the club (render state 1) or ball (2) screen's shot
// kind 3 or 4, else the idle state's (n4) with the zoom (b1DC). Golfer 29, the female created
// golfer, falls back to the 'f' version of an animation he lacks. A clip with pD8 set plays with
// the profile's handedness (choices.n113; a right-hander is turned round to PI for a left-handed
// profile), any other right-handed. Returns 1 when the animation started (its name is kept in
// sz10); 0 when asset animations are off (FE_CrAP_GetTriggerAnims: the camera does not switch
// either), or without a character, szAnim or such a clip.
u8 FE_vTriggerCrAPAnimAndCamera(char* szAnim, char* szShot, u8 bBlend) {
    char szName[0x20];
    View* pView;
    Clip* pClip;

    pView = ViewController_GetCameraControl(ViewController_GetCurrentViewControllerID());
    if (!FE_CrAP_GetTriggerAnims()) {
        return 0;
    }
    if (gpCrAPState->n8 == 1) {
        GolfCamera_SwitchCrAPCamera(pView, szShot, 3, bBlend, 0, 0);
    } else if (gpCrAPState->n8 == 2) {
        GolfCamera_SwitchCrAPCamera(pView, szShot, 4, bBlend, 0, 0);
    } else {
        GolfCamera_SwitchCrAPCamera(pView, szShot, gpCrAPState->n4, bBlend, gpCrAPState->b1DC, 0);
    }
    if (gpCrAPState->pB4 != NULL && gpCrAPState->pB4->pChar != NULL && szAnim != NULL) {
        strncpy(szName, szAnim, sizeof(szName));
        szName[sizeof(szName) - 1] = '\0';
        pClip = AnimLib_FindByName(gpCrAPState->pB4->pChar->pLib, szName);
        if (pClip == NULL && gpCrAPState->pB4->pChar->nC == 29 && szName != NULL && szName[0] != '\0') {
            szName[0] = 'f';
            pClip = AnimLib_FindByName(gpCrAPState->pB4->pChar->pLib, szName);
        }
        if (pClip == NULL) {
            return 0;
        }
        if (pClip->pD8 != NULL) {
            if (!Character_IsLeftHanded(gpCrAPState->pB4->pChar) && FE_GetCurrentProfile()->choices.n113
                != 0) {
                FE_SetCrapRotation(0, PI);
            }
            FE_CRAPSetHandednessForScreen(FE_GetCurrentProfile()->choices.n113);
        } else {
            if (Character_IsLeftHanded(gpCrAPState->pB4->pChar)) {
                FE_SetCrapRotation(0, 0.0f);
            }
            FE_CRAPSetHandednessForScreen(0);
        }
        Character_PlayClip(gpCrAPState->pB4->pChar, pClip, !bBlend, 0.5f);
        strcpy(gpCrAPState->sz10, szAnim);
        gpCrAPState->n1B4 = 0;
        return 1;
    }
    return 0;
}

// The name of the animation FE_vTriggerCrAPAnimAndCamera last started (sz10), or NULL once the idle
// animation has taken over (FE_vLoadNextCrAPAnim).
char* FE_GetCurrentAnimName(void) {
    if (gpCrAPState->sz10[0] == '\0') {
        return NULL;
    }
    return gpCrAPState->sz10;
}

// Give the golfer shown club nClub to hold (n1B8, a gClubPartNames class: 0 driver, 1 fairway wood,
// 2 putter, 3 3-iron, 4 7-iron, 5 wedge) and drop any temporary club (FE_SetTempCrapClub).
void FE_SetCrapClub(int nClub) {
    gpCrAPState->n1B8 = nClub;
    gpCrAPState->n1BC = -1;
    Character_SelectClub(gpCrAPState->pB4->pChar, gpCrAPState->n1B8);
}

// The club (a gClubPartNames class) the golfer holds while the queued animation plays (n1BC; -1:
// his own, FE_SetCrapClub's); he gets his own back when it ends. FE_CrAP_TurnOnAsset sets it for
// fairway woods, irons, wedges and putters.
void FE_SetTempCrapClub(int nClub) {
    gpCrAPState->n1BC = nClub;
}

// Queue animation szAnim with camera shot szShot (NULL: the idle state's) for the golfer on the
// CrAP screen (screen kind 3), run by FE_vUpdateGolferAll. With bWaitForEnd it waits
// until the current animation is within 0.6 s of its end, else it starts now: the golfer fades out
// over 0.5 s, the animation plays (FE_vTriggerCrAPAnimAndCamera), and with bFade he fades out at
// its end and back in (0: he stays in full view). The pad cannot turn him until it has played.
// szAnim NULL drops the queued animation.
void FE_QueueCrAPAnim(char* szAnim, char* szShot, s8 bFade, u8 bWaitForEnd) {
    if (szAnim == NULL) {
        gpCrAPState->sz20[0] = '\0';
        gpCrAPState->n1C0 = 4;
    } else {
        strncpy(gpCrAPState->sz20, szAnim, sizeof(gpCrAPState->sz20));
        gpCrAPState->f1CC = 0.5f;
        if (bWaitForEnd) {
            gpCrAPState->n1C0 = 0;
        } else {
            gpCrAPState->n1C0 = 1;
        }
        gpCrAPState->b1C8 = 1;
        gpCrAPState->n1C4 = 3;
    }
    if (szShot == NULL) {
        gpCrAPState->sz30[0] = '\0';
    } else {
        strncpy(gpCrAPState->sz30, szShot, sizeof(gpCrAPState->sz30));
    }
    gpCrAPState->n1D0 = bFade;
}

// How many more times the golfer's current animation plays before the idle one comes back (n50;
// FE_vLoadNextCrAPAnim counts it down).
void FE_SetAnimRepeatCount(int nCount) {
    gpCrAPState->n50 = nCount;
}

// Start the golfer's next animation at once, cut in (FE_vLoadNextCrAPAnim: the idle one, or the
// current one again while repeats are left), then drop the repeats left (FE_SetAnimRepeatCount) and
// the queued animation.
void FE_RestartCrAPAnim(void) {
    FE_vLoadNextCrAPAnim(0);
    gpCrAPState->n50 = 0;
    gpCrAPState->n1C0 = 4;
}

// Set what the CrAP screen shows (n8: 0 the golfer, 1 his clubs, 2 the ball) and drop a temporary
// state (FE_SetTempCrapRenderState). On a change the zoom is reset (FE_ResetCrAPZoom) and the next
// animation starts with its camera (FE_vLoadNextCrAPAnim). Nothing while asset animations are off
// (FE_CrAP_GetTriggerAnims).
void FE_SetCrapRenderState(int nState) {
    int nOld;

    if (FE_CrAP_GetTriggerAnims()) {
        nOld = gpCrAPState->n8;
        gpCrAPState->n8 = nState;
        gpCrAPState->nC = 0;
        gpCrAPState->b80 = 0;
        if (nOld != gpCrAPState->n8) {
            FE_ResetCrAPZoom();
            FE_vLoadNextCrAPAnim(0);
        }
    }
}

// Show render state nState (as FE_SetCrapRenderState) for the queued animation only: it takes over
// when that animation starts, and the screen goes back to the golfer (0) when it ends. Nothing
// while asset animations are off (FE_CrAP_GetTriggerAnims).
void FE_SetTempCrapRenderState(int nState) {
    if (FE_CrAP_GetTriggerAnims()) {
        gpCrAPState->nC = nState;
        gpCrAPState->b80 = 1;
    }
}

// How far the golfer's texture swap has got (n74): 1 the new textures are loading, 2 they are
// loaded and wait to be switched in, 0 done (char.c's front-end swap callbacks set it).
void FE_SetTextureSwapState(int nState) {
    gpCrAPState->n74 = nState;
}

u8 FE_IsTextureSwapDone(void) {
    return gpCrAPState->n74 == 0;
}

// Whether a texture swap waits for the queued animation instead of being switched in at once
// (FE_SetDelayTextureSwap).
u8 FE_GetDelayTextureSwap(void) {
    return gpCrAPState->b78;
}

// bDelay: a texture swap that has loaded is not switched in at once
// (Character_EndSwapTexturesCallbackFE) but by the queued animation, fTime seconds into it (0: as
// it starts). Returns the old setting. FE_CrAP_TurnOnAsset delays the swap for the parts that fade
// out (FE_CrAP_IsFadeOutCategory).
u8 FE_SetDelayTextureSwap(u8 bDelay, f32 fTime) {
    u8 bOld = gpCrAPState->b78;

    gpCrAPState->b78 = bDelay;
    gpCrAPState->f7C = fTime;
    return bOld;
}

// Queue ball texture szTex (NULL: none) for the ball screen: it goes on the ball (fn_800B9EB8, or
// none when the ball list lacks it) when the queued animation starts with the ball shown.
void FE_QueueBallChange(char* szTex) {
    if (szTex == NULL) {
        gpCrAPState->sz54[0] = '\0';
        return;
    }
    strncpy(gpCrAPState->sz54, szTex, sizeof(gpCrAPState->sz54));
}

int FE_GetCrapRenderState(void) {
    return gpCrAPState->n8;
}

// Blend the club screen's idle clip (clip state 8, looked up with the wedge in hand) in again from
// its start; the golfer keeps his club (n1B8, FE_SetCrapClub). FE_CrAP_TurnOnAsset uses it when a
// club asset goes on while the clubs are shown.
void FE_RestartClubIdleAnim(void) {
    Clip* pClip;

    Character_SelectClub(gpCrAPState->pB4->pChar, 5);
    pClip = Char_SetClip(gpCrAPState->pB4->pChar, 8, 0, NULL);
    if (pClip != NULL) {
        Character_PlayClip(gpCrAPState->pB4->pChar, pClip, 0, 0.5f);
    }
    Character_SelectClub(gpCrAPState->pB4->pChar, gpCrAPState->n1B8);
}

// Set b81, the flag for new textures on the golfer: a texture load's end and a new asset or logo
// set it, switching a swap in clears it. Nothing reads it in this build.
void FE_SetNewTexturesFlag(u8 bNew) {
    gpCrAPState->b81 = bNew;
}

// Make the golfer shown left-handed (bLefty) or right-handed and rebuild his skeleton to match;
// only the created golfers (7 and 29) change hands.
void FE_CRAPSetHandednessForScreen(u8 bLefty) {
    if (gpCrAPState->pB4 != NULL && gpCrAPState->pB4->pChar != NULL) {
        if (gpCrAPState->pB4->pChar->nC != 7) {
            // fake match: a one-case switch keeps the original's branch over a branch
            switch (gpCrAPState->pB4->pChar->nC) {
            case 29:
                break;
            default:
                return;
            }
        }
        Character_SetLeftHanded(gpCrAPState->pB4->pChar, bLefty);
        Character_SetSkeleton(gpCrAPState->pB4->pChar, gpCrAPState->pB4->pChar->pModel);
    }
}

u8 FE_GetClubStatesAllowed(void) {
    return gpCrAPState->b1D1;
}

// 0: the next Character_SetClubStatesForCharacter in the front end leaves the golfer's clubs as
// they are and sets it back to 1; FE_CrAP_TurnOnAsset clears it when a club asset goes on, so the
// new club skin is kept.
void FE_SetClubStatesAllowed(u8 bAllowed) {
    gpCrAPState->b1D1 = bAllowed;
}

// 1 once a texture swap has loaded and waits to be switched in
// (Character_EndSwapTexturesCallbackFE); Character_ExecuteTextureSwapFE switches only then, and
// clears it.
void FE_SetTextureSwapDue(u8 bDue) {
    gpCrAPState->b1D2 = bDue;
}

u8 FE_IsTextureSwapDue(void) {
    return gpCrAPState->b1D2;
}

// The slider asset last shown (its n0; -1: none): sApplySlider turns the golfer to the front when
// another comes.
void FE_SetLastCrAPAsset(int nAsset) {
    gpCrAPState->n1D4 = nAsset;
}

int FE_GetLastCrAPAsset(void) {
    return gpCrAPState->n1D4;
}

// The part (TW07's category) of the asset last put on (-1: none): FE_CrAP_TurnOnAsset turns the
// golfer to the front when the part changes.
void FE_SetLastCrAPCategory(int nPart) {
    gpCrAPState->n1D8 = nPart;
}

int FE_GetLastCrAPCategory(void) {
    return gpCrAPState->n1D8;
}

// Whether the menu golfer is updated and drawn this frame (gomainloop): the golfer shown is ready
// (b18) and b86, b88, lbl_80281F19, lbl_801D87C0.b0 and .b49 are all 0.
u8 FE_IsGolferRenderAllowed(void) {
    u8 bResult = 0;

    if (gpCrAPState->pB4->b18 && gpCrAPState->b86 == 0 && gpCrAPState->b88 == 0 && lbl_80281F19 == 0
        && lbl_801D87C0.b0 == 0 && lbl_801D87C0.b49 == 0) {
        bResult = 1;
    }
    return bResult;
}

// After an asset preview is undone (FE_CrAP_RestoreLastRemovedAsset): drop the queued animation,
// start the golfer's next animation at once, cut in (FE_vLoadNextCrAPAnim), and stop delaying
// texture swaps.
void FE_ResetCrAPGolferFromPreview(void) {
    FE_QueueCrAPAnim(NULL, NULL, 0, 0);
    FE_vLoadNextCrAPAnim(0);
    FE_SetDelayTextureSwap(0, 0.0f);
}

// Open the stream of stream list 3, the front-end golfer file fn_80014DFC puts there, and keep it
// in lbl_80280DF8->nStream.
void FE_OpenGolferStream(void) {
    lbl_80280DF8->nStream = Stream_OpenStreamFiles(&lbl_80280DF8->aParams[3]);
}

void FE_CloseGolferStream(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

// Four floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void FE_Vec4Sub(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void FE_Vec4Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

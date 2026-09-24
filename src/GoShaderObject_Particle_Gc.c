// GoShaderObject_Particle_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): the
// particle shader's buffers and drawing, and at its end the game's main-memory heap (made from the
// OS arena by fn_80095108) and a clock with five stopwatches that the main loop, golfer, UI and
// audio code time things with. The heap and the stopwatches are this file's by their data: its
// .sdata (0x802813A8-0x802813B8) and .sbss (0x80281F58-0x80281F70). Where it ends between
// 0x8009554C and char_state.c is not proven.

#include "engine.h"
#include "gx.h"
#include "camera.h"
#include "terrain.h"
#include "psmgr.h"
#include "unsorted/cull.h"

// The main-memory heap's globals (fn_80095108, fn_800951A0), defined last-address-first.
s32 lbl_80281F68;                       // the heap's size
int lbl_80281F64;                       // the heap
u8* lbl_80281F60;                       // the end of the last block allocated
u32 lbl_80281F5C;                       // the most ever allocated
u32 lbl_80281F58;                       // allocated now, headers and padding included

// The particles' four buffers, two of 90000 bytes and two of 10000.
void fn_8009414C(void) {
    lbl_802813A8->apBuffers[0] = fn_80009B34(90000, 2, 32, "GoShaderObject_Particle_Gc.c", 109);
    lbl_802813A8->apBuffers[1] = fn_80009B34(90000, 2, 32, "GoShaderObject_Particle_Gc.c", 111);
    lbl_802813A8->apBuffers[2] = fn_80009B34(10000, 2, 32, "GoShaderObject_Particle_Gc.c", 113);
    lbl_802813A8->apBuffers[3] = fn_80009B34(10000, 2, 32, "GoShaderObject_Particle_Gc.c", 115);
    lbl_802813A8->n14 = 0;
    lbl_802813A8->b10 = 0;
}

void fn_80094214(void) {
    lbl_802813A8->n14 = 0;
    lbl_802813A8->b10 = 0;
    fn_80009E70(lbl_802813A8->apBuffers[0]);
    fn_80009E70(lbl_802813A8->apBuffers[1]);
    fn_80009E70(lbl_802813A8->apBuffers[2]);
    fn_80009E70(lbl_802813A8->apBuffers[3]);
}

void fn_80094274(void) {
}

void fn_80094278(void) {
    lbl_802813A8->b10 = 1 - lbl_802813A8->b10;
}

// The create callback, called twice: first to allocate the system and give it the next run of
// particles in the buffers, then to fill it in from its settings (the vectors kept / 256).
void fn_8009428C(SD_SShaderObject_Static* pObject, ParticleCreate* pCreate) {
    ParticleSystem* pSys;

    if (pCreate->bAlloc) {
        pObject->pData = fn_80009B34(sizeof(ParticleSystem), 2, 32, "GoShaderObject_Particle_Gc.c", 201);
        ((ParticleSystem*)pObject->pData)->nFirst = lbl_802813A8->n14;
        lbl_802813A8->n14 += pCreate->pParams->nCount;
        return;
    }
    pSys = pObject->pData;
    pSys->nTexture = pCreate->pParams->nTexture;
    fn_800102DC(fn_8000BEE4(lbl_801F1640[pSys->nTexture]), &pSys->pBank, &pSys->pTex);
    pSys->nCount = pCreate->pParams->nCount;
    pSys->u18 = pCreate->pParams->u58;
    pSys->anStart[0] = 0;
    pSys->anLive[0] = 0;
    pSys->anStart[1] = 0;
    pSys->anLive[1] = 0;
    Vec_Copy(pCreate->pParams->vC0, pSys->shape.v10);
    Vec_Copy(pCreate->pParams->vD0, pSys->shape.v20);
    Vec_Copy(pCreate->pParams->vE0, pSys->shape.v30);
    Vec_Copy(pCreate->pParams->vF0, pSys->shape.v0);
    pSys->shape.f40 = pCreate->pParams->f104;
    pSys->shape.f44 = pCreate->pParams->f108;
    fn_8000AE28(1.0f / 256.0f, pSys->shape.v10, pSys->shape.v10);
    fn_8000AE28(1.0f / 256.0f, pSys->shape.v20, pSys->shape.v20);
    fn_8000AE28(1.0f / 256.0f, pSys->shape.v30, pSys->shape.v30);
    fn_8000AE28(1.0f / 256.0f, pSys->shape.v0, pSys->shape.v0);
    pSys->shape.f40 = pSys->shape.f40 / 256.0f;
    pSys->shape.f44 = pSys->shape.f44 / 256.0f;
    pSys->shape.f48 = 1.4142f * pCreate->pParams->f118;
    pSys->shape.f4C = 1.4142f * pCreate->pParams->f114;
    pSys->shape.f50 = 0.25f * pCreate->pParams->f110;
    pSys->shape.f54 = 1.0f / pCreate->pParams->f110;
    pSys->shape.f58 = 1.0f / (2.0f * PI);
    pSys->shape.f5C = 0.5f;
    fn_8000AE6C(pCreate->pParams->vB0, pCreate->pParams->vA0, pCreate->pParams->f110, pSys->shape.v60);
    fn_8000AE28(pSys->shape.f54, pSys->shape.v60, pSys->shape.v60);
    pSys->shape.v60[3] = 0.0f;
    pSys->shape.aSin[0] = 2.0f * PI;
    pSys->shape.aSin[1] = -(2.0f * PI) * (2.0f * PI) * (2.0f * PI) / 6.0f;
    pSys->shape.aSin[2] = (2.0f * PI) * (2.0f * PI) * (2.0f * PI) * (2.0f * PI) * (2.0f * PI) / 120.0f;
    pSys->shape.aSin[3] = -(2.0f * PI) * (2.0f * PI) * (2.0f * PI) * (2.0f * PI) * (2.0f * PI)
                          * (2.0f * PI) * (2.0f * PI) / 5040.0f;
    pSys->shape.aCos[0] = 1.0f;
    pSys->shape.aCos[1] = -(2.0f * PI) * (2.0f * PI) / 2.0f;
    pSys->shape.aCos[2] = (2.0f * PI) * (2.0f * PI) * (2.0f * PI) * (2.0f * PI) / 24.0f;
    pSys->shape.aCos[3] = -(2.0f * PI) * (2.0f * PI) * (2.0f * PI) * (2.0f * PI) * (2.0f * PI)
                          * (2.0f * PI) / 720.0f;
}

// The close callback: the buffers' cursor goes back to the start of the system's run (also over
// any runs given out after it, even while those are in use).
void fn_800944F8(SD_SShaderObject_Static* pObject) {
    ParticleSystem* pSys = pObject->pData;

    if (lbl_802813A8->n14 > pSys->nFirst) {
        lbl_802813A8->n14 = pSys->nFirst;
    }
    fn_80009E70(pSys);
}

void fn_800950CC(f32 s, f32 t);
void fn_800950DC(int r, int g, int b, int a);
void fn_800950F4(f32 x, f32 y, f32 z);
void fn_800124A8(void);                                 // LLFont.c: end the primitive

// Draws n particles as camera-facing quads. Each one's position runs from v0 along v60 with time,
// plus its own velocity's difference from v60 eased out by 1 / (1 + f50 t)^4; it is skipped when
// it ends up behind the camera. Its size, colour and alpha run linearly with time within limits,
// and it spins by f18 + f1C t turns.
void fn_80094534(f32 (*pMtx)[4], ParticleShape* pShape, ParticleVertex* pVerts, f32* pTimes, u32 n) {
    u32 i;
    f32 t;
    f32 fEase;
    f32 fDiff;
    f32 fSize;
    f32 fRed;
    f32 fGreen;
    f32 fBlue;
    f32 fAlpha;
    f32 fFade;
    f32 fAngle;
    f32 fSin;
    f32 fCos;
    int nRed;
    int nGreen;
    int nBlue;
    u8 uAlpha;
    Vec4 v;
    f32 m[3][4];

    v.w = 1.0f;
    m[0][2] = 0.0f;
    m[1][2] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -1.0f;
    for (i = 0; i < n; i++) {
        t = *pTimes;
        fEase = pShape->f50 * t + 1.0f;
        fEase = fEase * fEase;
        fEase = 1.0f / (fEase * fEase);
        fDiff = pVerts->vC[0] * pShape->f54;
        fDiff = fDiff - pShape->v60[0] * pShape->f54;
        v.x = pShape->v60[0] * t + pVerts->v0[0] + fDiff - fEase * fDiff;
        fDiff = pVerts->vC[1] * pShape->f54;
        fDiff = fDiff - pShape->v60[1] * pShape->f54;
        v.y = pShape->v60[1] * t + pVerts->v0[1] + fDiff - fEase * fDiff;
        fDiff = pVerts->vC[2] * pShape->f54;
        fDiff = fDiff - pShape->v60[2] * pShape->f54;
        v.z = pShape->v60[2] * t + pVerts->v0[2] + fDiff - fEase * fDiff;
        fn_800BAD60(pMtx, &v, &v);
        if (v.z > 0.0f) {
            fSize = pShape->f48 * t + pVerts->f20;
            fSize = (fSize <= pShape->f4C) ? fSize : pShape->f4C;
            fRed = pShape->v10[0] * t + pShape->v0[0];
            if (fRed <= pShape->v20[0]) {
                fRed = pShape->v20[0];
            }
            nRed = 255.0f * ((fRed < pShape->v20[0]) ? pShape->v20[0]
                             : (fRed > pShape->v30[0]) ? pShape->v30[0] : fRed);
            fGreen = pShape->v10[1] * t + pShape->v0[1];
            if (fGreen <= pShape->v20[1]) {
                fGreen = pShape->v20[1];
            }
            nGreen = 255.0f * ((fGreen < pShape->v20[1]) ? pShape->v20[1]
                               : (fGreen > pShape->v30[1]) ? pShape->v30[1] : fGreen);
            fBlue = pShape->v10[2] * t + pShape->v0[2];
            if (fBlue <= pShape->v20[2]) {
                fBlue = pShape->v20[2];
            }
            nBlue = 255.0f * ((fBlue < pShape->v20[2]) ? pShape->v20[2]
                              : (fBlue > pShape->v30[2]) ? pShape->v30[2] : fBlue);
            fAlpha = pShape->v10[3] * t + pShape->v0[3];
            if (fAlpha <= pShape->v20[3]) {
                fAlpha = pShape->v20[3];
            }
            uAlpha = 128.0f * ((fAlpha < pShape->v20[3]) ? pShape->v20[3]
                               : (fAlpha > pShape->v30[3]) ? pShape->v30[3] : fAlpha);
            fFade = 128.0f * (pShape->f44 * t + pShape->f40);
            if (uAlpha <= fFade) {
                fFade = uAlpha;
            }
            fAngle = (pVerts->f1C * t + pVerts->f18) / (2.0f * PI);
            fSin = fn_800095F0(fAngle);
            fCos = fn_80009638(fAngle);
            m[0][0] = -fCos;
            m[0][1] = fSin;
            m[0][3] = -v.x;
            m[1][0] = fSin;
            m[1][1] = fCos;
            m[1][3] = v.y;
            m[2][3] = -v.z;
            GXLoadPosMtxImm(m, 0);
            GXBegin(0x80, 4, 4);                // quads, vertex format 4
            fn_800950F4(-fSize, -fSize, 0.0f);
            fn_800950DC((u8)nRed, (u8)nGreen, (u8)nBlue, (u8)fFade);
            fn_800950CC(1.0f, 1.0f);
            fn_800950F4(fSize, -fSize, 0.0f);
            fn_800950DC((u8)nRed, (u8)nGreen, (u8)nBlue, (u8)fFade);
            fn_800950CC(0.0f, 1.0f);
            fn_800950F4(fSize, fSize, 0.0f);
            fn_800950DC((u8)nRed, (u8)nGreen, (u8)nBlue, (u8)fFade);
            fn_800950CC(0.0f, 0.0f);
            fn_800950F4(-fSize, fSize, 0.0f);
            fn_800950DC((u8)nRed, (u8)nGreen, (u8)nBlue, (u8)fFade);
            fn_800950CC(1.0f, 0.0f);
            fn_800124A8();
        }
        pVerts++;
        pTimes++;
    }
}

// The draw callback: the live particles of the buffer in use, in two runs when they wrap past the
// end of the system's run.
void fn_800949D0(SD_SShaderObject_Static* pObject) {
    int nBuf;
    u16 nLive;
    u32 nStart;
    ParticleSystem* pSys;
    Camera* pCamera;
    ParticleVertex* pVerts;
    f32* pAges;
    u32 n;

    pSys = pObject->pData;
    nBuf = lbl_802813A8->b10;
    nLive = pSys->anLive[nBuf];
    nStart = pSys->anStart[nBuf];
    if (nLive != 0) {
        fn_80012F50(0, 6, 0x80);
        fn_80012F18(3);
        fn_80012F34(0);
        fn_80014118(0x70);
        fn_8005CC64(pSys->pBank, pSys->pTex);
        if (pSys->u18 & 0x80) {
            fn_80035118(4, 1);
        } else if (pSys->u18 & 0x100) {
            fn_80035118(1, 1);
        } else {
            fn_80035118(4, 5);
        }
        fn_80035138(0);
        fn_80012EF8();
        pCamera = fn_8001614C();
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);                     // position, colour, texture coordinates: direct
        GXSetVtxDesc(11, 1);
        GXSetVtxDesc(13, 1);
        GXInvalidateVtxCache();
        pVerts = (ParticleVertex*)lbl_802813A8->apBuffers[nBuf] + pSys->nFirst + nStart;
        pAges = (f32*)lbl_802813A8->apBuffers[nBuf + 2] + pSys->nFirst + nStart;
        n = pSys->nCount - nStart;
        if (nLive <= n) {
            n = nLive;
        }
        fn_80094534(pCamera->viewMtx, &pSys->shape, pVerts, pAges, n);
        n = pSys->nCount - nStart;
        if (nLive > n) {
            fn_80094534(pCamera->viewMtx, &pSys->shape,
                        (ParticleVertex*)lbl_802813A8->apBuffers[nBuf] + pSys->nFirst,
                        (f32*)lbl_802813A8->apBuffers[nBuf + 2] + pSys->nFirst, nLive - n);
        }
        fn_80012F34(1);
        fn_80012F50(1, 6, 0x80);
        fn_80012EF8();
    }
}

// The buffer not being drawn is rebuilt from the one drawn last: its own live particles age by
// fStep and those past the lifetime are dropped; then the particles the other buffer has beyond
// them are carried over, aged by fCarried.
void fn_80094B84(SD_SShaderObject_Static* pObject, ParticleMsg* pMsg) {
    ParticleSystem* pSys;
    int nBuf;
    u32 nDead;
    u32 nStart;
    u32 nLive;
    u32 i;
    u32 n;
    u32 nEnd;
    f32 fAge;
    ParticleVertex* pSrc;
    ParticleVertex* pDst;

    nDead = 0;
    pSys = pObject->pData;
    nBuf = 1 - lbl_802813A8->b10;
    nStart = pSys->anStart[nBuf];
    nLive = pSys->anLive[nBuf];
    i = nStart;
    for (n = nLive; n != 0; n--) {
        fAge = *((f32*)lbl_802813A8->apBuffers[nBuf + 2] + pSys->nFirst + i) + pMsg->u.age.fStep;
        if (fAge > pMsg->pParams->f4) {
            nDead++;
        } else {
            *((f32*)lbl_802813A8->apBuffers[nBuf + 2] + pSys->nFirst + i) = fAge;
        }
        if (++i == pSys->nCount) {
            i = 0;
        }
    }
    if (nDead != 0) {
        nStart += nDead;
        if (nStart >= pSys->nCount) {
            nStart -= pSys->nCount;
        }
        nLive -= nDead;
    }
    nEnd = pSys->anStart[1 - nBuf] + pSys->anLive[1 - nBuf];
    if (nEnd >= pSys->nCount) {
        nEnd -= pSys->nCount;
    }
    while (i != nEnd) {
        *((f32*)lbl_802813A8->apBuffers[nBuf + 2] + pSys->nFirst + i) =
            *((f32*)lbl_802813A8->apBuffers[1 - nBuf + 2] + pSys->nFirst + i) + pMsg->u.age.fCarried;
        // EA copies four slots from each particle's on; the next three are other particles'
        pSrc = (ParticleVertex*)lbl_802813A8->apBuffers[1 - nBuf] + pSys->nFirst + i;
        pDst = (ParticleVertex*)lbl_802813A8->apBuffers[nBuf] + pSys->nFirst + i;
        pDst[0] = pSrc[0];
        pDst[1] = pSrc[1];
        pDst[2] = pSrc[2];
        pDst[3] = pSrc[3];
        if (++i == pSys->nCount) {
            i = 0;
        }
        nLive++;
    }
    pSys->anStart[nBuf] = nStart;
    pSys->anLive[nBuf] = nLive;
    *pMsg->u.age.pnLive = nLive;
}

void fn_800BADB4(f32 (*pMtx)[4], f32* pIn, f32* pOut);     // VecMath.c: a vector through a matrix

// Emits pMsg's particles after the live ones of the buffer not being drawn (at most as many as
// fit, less one). Their ages run from fAgeSpread down; unless the settings' flag 0x1000 keeps a
// slot whose f18 is not 1024, each gets a new position and velocity through the matrix.
void fn_80094E34(SD_SShaderObject_Static* pObject, ParticleMsg* pMsg) {
    u32 i;
    u32 nTotal;
    ParticleSystem* pSys;
    ParticleVertex* pVert;
    int nBuf;
    int nFree;
    f32 fAge;
    Vec4 v;

    pSys = pObject->pData;
    nBuf = 1 - lbl_802813A8->b10;
    nFree = pSys->nCount - pSys->anLive[nBuf];
    if (nFree <= 1 || pMsg->u.emit.nCount == 0) {
        return;
    }
    if (pMsg->u.emit.nCount > nFree - 1) {
        pMsg->u.emit.nCount = nFree - 1;
    }
    i = pSys->anStart[nBuf] + pSys->anLive[nBuf];
    if (i >= pSys->nCount) {
        i -= pSys->nCount;
    }
    pSys->anLive[nBuf] += (u16)pMsg->u.emit.nCount;
    nTotal = pMsg->u.emit.nCount;
    do {
        if (pMsg->u.emit.fAgeSpread) {
            fAge = pMsg->u.emit.fAgeSpread * ((f32)pMsg->u.emit.nCount / (f32)nTotal);
        } else {
            fAge = 0.0f;
        }
        *((f32*)lbl_802813A8->apBuffers[nBuf + 2] + pSys->nFirst + i) = fAge;
        pVert = (ParticleVertex*)lbl_802813A8->apBuffers[nBuf] + pSys->nFirst + i;
        if (!(pMsg->pParams->u58 & 0x1000) || pVert->f18 == 1024.0f) {
            fn_80098CDC(pMsg->pParams, pVert->v0, pVert->vC, &pVert->f18, &pVert->f1C, &pVert->f20);
            pVert->f20 *= 1.4142f;
            v.x = pVert->v0[0];
            v.y = pVert->v0[1];
            v.z = pVert->v0[2];
            v.w = 1.0f;
            fn_800BAD60(pMsg->u.emit.pMtx, &v, &v);
            Vec3Copy(&v.x, pVert->v0);
            v.x = pVert->vC[0];
            v.y = pVert->vC[1];
            v.z = pVert->vC[2];
            v.w = 1.0f;
            fn_800BADB4(pMsg->u.emit.pMtx, &v.x, &v.x);
            Vec3Copy(&v.x, pVert->vC);
            // the next three slots get copies of it
            Mem_cpy(pVert + 1, pVert, sizeof(ParticleVertex));
            Mem_cpy(pVert + 2, pVert, 2 * sizeof(ParticleVertex));
        }
        if (++i == pSys->nCount) {
            i = 0;
        }
        pMsg->u.emit.nCount--;
    } while (pMsg->u.emit.nCount != 0);
}

// The message callback: pMsg->nWhat 0 ages the particles, 1 emits new ones.
void fn_80095088(SD_SShaderObject_Static* pObject, ParticleMsg* pMsg) {
    switch (pMsg->nWhat) {
    case 0:
        fn_80094B84(pObject, pMsg);
        break;
    case 1:
        fn_80094E34(pObject, pMsg);
        break;
    }
}

// The vertex writes for fn_80094534's quads, straight to the GX FIFO: a texture coordinate, a
// colour and a position.
void fn_800950CC(f32 s, f32 t) {
    *(f32*)0xCC008000 = s;
    *(f32*)0xCC008000 = t;
}

void fn_800950DC(int r, int g, int b, int a) {
    *(volatile u8*)0xCC008000 = r;
    *(volatile u8*)0xCC008000 = g;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = a;
}

void fn_800950F4(f32 x, f32 y, f32 z) {
    *(f32*)0xCC008000 = x;
    *(f32*)0xCC008000 = y;
    *(f32*)0xCC008000 = z;
}

// Makes the main-memory heap from the arena: all of it but the top 4 MB when there is more than
// 24 MB, else all but the top 16 KB (the arena then starts at the heap's end).
void fn_80095108(void) {
    u8* pLo;
    u8* pHi;
    u8* pEnd;

    pLo = OSGetArenaLo();
    pHi = OSGetArenaHi();
    lbl_80281F68 = pHi - pLo;
    if (lbl_80281F68 > 0x1800000) {
        pEnd = pHi - 0x400000;
    } else {
        pEnd = pHi - 0x4000;
    }
    lbl_80281F68 = pEnd - pLo;
    pLo = OSInitAlloc(pLo, pEnd, 1);
    OSSetArenaLo(pLo);
    // port: the heap's bounds rounded to 32 bytes
    lbl_80281F64 = OSCreateHeap((void*)(((uptr)pLo + 0x1F) & ~0x1F), (void*)((uptr)pEnd & ~0x1F));
    OSSetCurrentHeap(lbl_80281F64);
    OSSetArenaLo(pEnd);
}

// Allocates uSize bytes aligned to nAlign (0: 16). A HeapBlockHead sits just before the result,
// and the padding's size in the byte after it (for fn_8009527C).
void* fn_800951A0(u32 uSize, u16 nAlign, int n) {
    u32 uPad;
    u32 uTotal;
    u8* pBlock;
    u32 uOff;
    u8* pData;
    u32 uPeak;

    if (uSize == 0) return NULL;
    if (nAlign == 0) {
        nAlign = 16;
    }
    uPad = nAlign - 1;
    uTotal = sizeof(HeapBlockHead) + 1 + uPad + uSize;
    pBlock = OSAllocFromHeap(__OSCurrHeap, uTotal);
    if (pBlock == NULL) return NULL;
    pData = pBlock;
    // port: aligned by the address
    uOff = (uptr)(pBlock + sizeof(HeapBlockHead)) % nAlign;
    if (uOff != 0) {
        pData += nAlign - uOff;
    }
    ((HeapBlockHead*)pData)->pBlock = pBlock;
    ((HeapBlockHead*)pData)->uSize = uSize;
    pData = (u8*)((HeapBlockHead*)pData + 1);
    *(pData + uSize) = uPad;
    uPeak = lbl_80281F5C;
    lbl_80281F58 += uTotal;
    if (lbl_80281F58 > uPeak) {
        uPeak = lbl_80281F58;
    }
    lbl_80281F5C = uPeak;
    lbl_80281F60 = pData + uTotal;
    return pData;
}

void fn_8009527C(void* p) {
    HeapBlockHead* pHead;
    u32 uSize;
    u8 uPad;

    if (p != NULL) {
        pHead = (HeapBlockHead*)p - 1;
        uSize = pHead->uSize;
        uPad = ((u8*)p)[uSize];
        OSFreeToHeap(__OSCurrHeap, pHead->pBlock);
        lbl_80281F58 = lbl_80281F58 - uSize - (sizeof(HeapBlockHead) + 1) - uPad;
    }
}

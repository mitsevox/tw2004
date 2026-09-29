// Code800B1AA8.c (a placeholder named by its address: no EA file name is known; TW06 / TW07 have
// none of these functions): the ball against the course's dynamic objects. Ball.c asks whether the
// ball, moving between two points, went into an object's bounding sphere (DynObj_FindBallHit) and
// reads the flagstick's sphere (DynObj_GetBoundingSphere). It touches only DynObj and the ball's
// radius. Its two paired-single vector helpers are the unit's own copies, emitted at its end.
// Linked between UAudVector.c and Code800B1D3C.c.

#include "ball.h"
#include "dynobj.h"

void   Startup_Vec3Add(f32* pA, f32* pB, f32* pOut);
void   Startup_Vec3Sub(f32* pA, f32* pB, f32* pOut);

// Whether the ball can hit the object; every object can.
u32 DynObj_CanBallHit(UObject* pObj, f32* pPos) {
    return 1;
}

// An object's bounding sphere: its centre (the object's position plus its first LOD mesh's sphere
// centre) into pCenter and its radius into pRadius; either may be NULL.
void DynObj_GetBoundingSphere(DynObj* pObj, f32* pCenter, f32* pRadius) {
    UObjMesh* pMesh = pObj->obj.pModel->apLod[0];
    if (pCenter != NULL) {
        Startup_Vec3Add(pObj->obj.m80[3], pMesh->pInfo->v58, pCenter);
    }
    if (pRadius != NULL) {
        *pRadius = pMesh->pInfo->f64;
    }
}

// Did the ball, moving from pFrom to pTo, hit an object? Of the dynamic objects with flag 8 whose
// bounding sphere, grown by the ball's radius (in yards), holds pTo, take the one nearest pFrom:
// tell it (its handler, message 12, with the player number), and give the hit point on its sphere,
// the sphere's normal there and the object (each output may be NULL). Returns whether there was
// one.
u8 DynObj_FindBallHit(int nPlayer, f32* pTo, f32* pFrom, f32* pHit, f32* pNormal, HitObject** ppWhat) {
    f32 vNormal[3];
    f32 vCenter[4];     // fake match: three floats are used; the frame has room for four
    f32 fRadius;
    f32 fBest;
    f32 fDX;
    f32 fDY;
    f32 fDZ;
    f32 fFlat;
    f32 fReach;
    f32 fDist;
    DynObj* pObj;
    DynObj* pBest = NULL;

    for (pObj = fn_80048E44(); pObj != NULL; pObj = pObj->pNext) {
        if (pObj->uFlags & 8) {
            DynObj_GetBoundingSphere(pObj, vCenter, &fRadius);
            fDZ = pTo[2] - vCenter[2];
            fDX = pTo[0] - vCenter[0];
            fFlat = fDX * fDX + fDZ * fDZ;
            fDist = Math_Sqrt(fFlat);
            fReach = gRealBallRadiusIn / 36.0f + fRadius;
            if (fDist < fReach) {
                fDY = pTo[1] - vCenter[1];
                if ((f32)Math_Sqrt(fDY * fDY + fFlat) < fReach && DynObj_CanBallHit(&pObj->obj, pTo)) {
                    fDist = LLMath_DistanceBetween3(pFrom, vCenter);
                    if (pBest == NULL || fDist < fBest) {
                        fBest = fDist;
                        pBest = pObj;
                    }
                }
            }
        }
    }
    if (pBest != NULL) {
        // port: the player number goes through the handler's pointer argument
        pBest->pfnHandler(12, pBest, (void*)nPlayer, NULL);
        DynObj_GetBoundingSphere(pBest, vCenter, &fRadius);
        Startup_Vec3Sub(pTo, vCenter, vNormal);
        LLMath_Normalize3(vNormal, vNormal);
        if (pHit != NULL) {
            fn_8000C5D4(vCenter, vNormal, fRadius, pHit);
        }
        if (pNormal != NULL) {
            Vec3Copy(vNormal, pNormal);
        }
        if (ppWhat != NULL) {
            *ppWhat = (HitObject*)pBest;    // HitObject is Ball.c's view of a DynObj
        }
        return 1;
    }
    return 0;
}

// a + b into out (three floats)
#ifdef __MWERKS__
asm void Startup_Vec3Add(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void Startup_Vec3Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// a - b into out (three floats)
#ifdef __MWERKS__
asm void Startup_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void Startup_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

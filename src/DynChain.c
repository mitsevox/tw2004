// DynChain.c (EA's name, from its asserts; TW06): bone chains that swing on their own (hair,
// cloth), made from a bone down through its children and updated with the model.

#include "engine.h"
#include "character.h"
#include "charstate.h"
#include "unsorted/cull.h"

f32  fn_800BAF58(f32* pSrc, f32* pDst);     // VecMath.c: normalises, gives the length (0 if near zero)
void fn_80029BC8(f32* pVec);                // GoLighting.c
void fn_801164D4(f32* pA, f32* pB, f32* pOut);
void fn_801164F8(f32* pA, f32* pB, f32* pOut);
void fn_8011651C(f32* pA, f32* pB, f32* pOut);
void Quat_RotateVector(f32* pQuat, f32* pIn, f32* pOut);           // Quaternion.c: a vector turned by it
void Quat_ExtractEulerAngles(f32* pQ, f32* pA, f32* pB, f32* pC);        // Quaternion.c: a rotation as angles
f32  fn_80055F80(void);                     // Ball.c
void fn_800561CC(f32* pOut);                // Ball.c: the wind's direction
void SKEL_TransformBones(CharModel* pModel, u32* auBits);
f32  fn_80116304(u32 nFrame, f32 fPhase, f32 fStrength);
f32  fn_80116468(void);
void Quat_QuatToMatrix(f32* pQ, f32 (*pMtx)[4]);                                    // Quaternion.c
void fn_8000ADC0(f32 (*pMtx)[4]);                                             // identity
void fn_800BAE5C(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);  // VecMath.c
void fn_80114540(CharModel* pModel, DynChain* pChain, f32 f);
void fn_80114A84(CharModel* pModel, DynChain* pChain, f32 f);
void fn_80115348(CharModel* pModel, DynChain* pChain, f32 f);
void fn_80115B2C(CharModel* pModel, DynChain* pChain, f32 f);

// A direction per chain kind (DynChain.n10), in the model's root space.
f32 lbl_80193DE8[6][4] = {
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { -1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f, 0.0f },
};

// An angle per chain kind, in degrees (fn_80115B2C).
f32 lbl_80193E48[6] = { 0.0f, 260.0f, 130.0f, 0.0f, 260.0f, 130.0f };

DynChainSettings* lbl_802824F8;

// Make the chains' settings, with their starting values.
void fn_80113E60(void) {
    lbl_802824F8 = fn_80009B34(sizeof(DynChainSettings), 2, 0, "DynChain.c", 173);
    lbl_802824F8->aParams[1].a0[0] = 3.6f;
    lbl_802824F8->aParams[1].a0[1] = 6.0f;
    lbl_802824F8->aParams[1].a0[2] = 9.9f;
    lbl_802824F8->aParams[1].aC[0] = 0.5f;
    lbl_802824F8->aParams[1].aC[1] = 0.5f;
    lbl_802824F8->aParams[1].aC[2] = 0.5f;
    lbl_802824F8->aParams[1].a18[0] = 0.0f;
    lbl_802824F8->aParams[1].a18[1] = 0.9f;
    lbl_802824F8->aParams[1].a18[2] = 0.7f;
    lbl_802824F8->aParams[1].a24[0] = 0.0f;
    lbl_802824F8->aParams[1].a24[1] = 0.0f;
    lbl_802824F8->aParams[1].a24[2] = 0.0f;
    lbl_802824F8->aParams[0].a0[0] = 2.0f;
    lbl_802824F8->aParams[0].a0[1] = 4.0f;
    lbl_802824F8->aParams[0].a0[2] = 8.0f;
    lbl_802824F8->aParams[0].aC[0] = 0.25f;
    lbl_802824F8->aParams[0].aC[1] = 0.25f;
    lbl_802824F8->aParams[0].aC[2] = 0.25f;
    lbl_802824F8->aParams[0].a18[0] = 0.0f;
    lbl_802824F8->aParams[0].a18[1] = 0.9f;
    lbl_802824F8->aParams[0].a18[2] = 0.7f;
    lbl_802824F8->aParams[0].a24[0] = -2.0f;
    lbl_802824F8->aParams[0].a24[1] = 0.0f;
    lbl_802824F8->aParams[0].a24[2] = 3.0f;
    lbl_802824F8->an9C[0] = 1;
    lbl_802824F8->an9C[1] = 1;
    lbl_802824F8->an9C[2] = 1;
    lbl_802824F8->an9C[3] = 1;
    lbl_802824F8->an9C[4] = 1;
    lbl_802824F8->an9C[5] = 1;
    lbl_802824F8->nB4 = 0;
    lbl_802824F8->nB8 = -1;
    lbl_802824F8->nBC = 10;
    lbl_802824F8->f90 = 0.9f;
    lbl_802824F8->f94 = 0.2f;
    lbl_802824F8->f98 = 1.5f;
    lbl_802824F8->f78 = 0.02f;
    lbl_802824F8->f7C = 1.0f;
    lbl_802824F8->f80 = 0.6f;
    lbl_802824F8->f84 = 1.0f;
    lbl_802824F8->f88 = 0.6f;
    lbl_802824F8->f8C = 1.0f;
    lbl_802824F8->f68 = 14.0f;
    lbl_802824F8->f70 = 180.0f;
    lbl_802824F8->f74 = 1.0f;
    lbl_802824F8->f60 = 5.0f;
    lbl_802824F8->f6C = 0.5f;
    lbl_802824F8->f64 = 0.2f;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8011407C(void);
void fn_80114080(void);

void fn_8011407C(void) {
}

void fn_80114080(void) {
    fn_80009E70(lbl_802824F8);
    lbl_802824F8 = NULL;
}

// ---- end of sweep code ----

// Set up a link on bone nBone as the model stands now.
void fn_801140AC(DynChainLink* pLink, CharModel* pModel, int nBone, s32 nType) {
    f32 vParent[4];
    f32 vBone[4];
    f32 vDiff[4];

    if (nBone != -1) {
        pLink->nBone = nBone;
        pLink->nParent = pModel->pBones[nBone].nParent;
        Vec_Copy(pModel->pMatrices[nBone][3], pLink->v04);
        if (nType == 0) {
            fn_8001E880(pModel->pPoses[pLink->nParent].v10, vParent);
            fn_8001E880(pModel->pPoses[nBone].v10, vBone);
            fn_801164D4(vBone, vParent, vDiff);
            pLink->fLength = fn_800BAFC0(vDiff, vDiff);
        } else {
            pLink->fLength = 0.5f;
        }
        pLink->f8C = 0.0f;
        fn_8001E85C(pModel->pPoses[nBone].q0, pLink->q44);
        fn_8001E85C(pModel->pPoses[nBone].q0, pLink->q54);
        fn_8001E85C(pModel->pPoses[nBone].v10, pLink->v64);
        fn_8001E85C(pModel->pPoses[nBone].v10, pLink->v74);
        fn_80029BC8(pLink->v24);
    }
}

// Set up every link again, from the top bone down.
void fn_801141F8(DynChain* pChain, CharModel* pModel) {
    int i;
    int nBone;

    if (pChain != NULL) {
        pChain->bReset = 1;
        nBone = pChain->nBone;
        for (i = 0; i < pChain->nLinks; i++) {
            fn_801140AC(&pChain->pLinks[i], pModel, nBone++, pChain->nType);
        }
    }
}

// A chain from bone nBone down: a link for it and each first child below it.
DynChain* fn_80114270(CharModel* pModel, int nBone, s32 nType, s32 n10) {
    DynChain* pChain;
    int nLinks;
    int nNext;
    int i;
    u8 bFound;

    nNext = nBone;
    if (nBone == 0xFF || nBone == -1) {
        return NULL;
    }
    bFound = 1;
    nLinks = 1;
    while (bFound == 1) {
        bFound = 0;
        for (i = 0; i < pModel->nBones; i++) {
            if (nNext == pModel->pBones[i].nParent) {
                nNext = i;
                bFound = 1;
                nLinks++;
                break;
            }
        }
    }
    pChain = fn_80009B34(sizeof(DynChain), 2, 64, "DynChain.c", 390);
    pChain->pLinks = fn_80009B34(nLinks * sizeof(DynChainLink), 2, 64, "DynChain.c", 392);
    pChain->nLinks = nLinks;
    pChain->nBone = nBone;
    pChain->nType = nType;
    pChain->n10 = n10;
    pChain->n14 = 0;
    pChain->n18 = 0;
    fn_801141F8(pChain, pModel);
    return pChain;
}

// Free a chain.
void fn_80114398(DynChain* pChain) {
    fn_80009E70(pChain->pLinks);
    fn_80009E70(pChain);
}

// pIn (x, y, z, w) through pMtx's rotation into pOut; w is copied.
void fn_801143D0(const f32 (*pMtx)[4], const f32* pIn, f32* pOut) {
    pOut[0] = pIn[2] * pMtx[2][0] + (pIn[0] * pMtx[0][0] + pIn[1] * pMtx[1][0]);
    pOut[1] = pIn[2] * pMtx[2][1] + (pIn[0] * pMtx[0][1] + pIn[1] * pMtx[1][1]);
    pOut[2] = pIn[2] * pMtx[2][2] + (pIn[0] * pMtx[0][2] + pIn[1] * pMtx[1][2]);
    pOut[3] = pIn[3];
}

// Update a chain by fDelta: set it up again first if asked, then the update of its type.
void fn_8011443C(CharModel* pModel, DynChain* pChain, f32 fDelta) {
    if (fDelta && pChain != NULL) {
        if (pChain->bReset) {
            fn_801141F8(pChain, pModel);
            pChain->bReset = 0;
        }
        if (pChain->nBone == 0xFF || pChain->nBone == -1) {
            return;
        }
        if (pChain->nType == 0) {
            fn_80114A84(pModel, pChain, fDelta);
        }
        if (pChain->nType == 1) {
            fn_80114540(pModel, pChain, fDelta);
        }
        if (pChain->nType == 2) {
            fn_80115348(pModel, pChain, fDelta);
        }
        if (pChain->nType == 3) {
            fn_80115B2C(pModel, pChain, fDelta);
        }
    }
}

// The type 1 update: the chain's first link hangs from its bone's matrix fLength along the matrix's
// z axis. It falls and swings with a velocity (v24) that gravity pulls down, limited to a top speed
// and slowed by drag; it may lag at most 0.4 (times the model's f12C) behind where it hangs, but
// never along the matrix's y axis. The bone then turns to point at it. Not on game type 3.
void fn_80114540(CharModel* pModel, DynChain* pChain, f32 fDelta) {
    f32 mScale[4][4];
    f32 mRot[4][4];
    f32 qTurn[4];
    f32 vAxis[4];
    f32 vHang[4];
    f32 vZ[4];
    f32 vUnitY[4];
    f32 vUp[4];
    f32 vX[4];
    f32 vUnitX[4];
    f32 vUnitZ[4];
    f32 vDiff[4];
    f32 vTo[4];
    f32 vWas[4];
    f32 vPos[4];
    f32 vBack[4];
    f32 vDrag[4];
    f32 vMove[4];
    f32 fScale;
    f32 fMaxLag;
    f32 fMaxSpeed;
    f32 fLag;
    f32 fSpeed;
    f32 fDrag;
    f32 fAngle;

    if (gSession.nGameType == 3) {
        return;
    }
    fScale = pModel->f12C;
    if (0.0f == fScale) {
        return;
    }
    fMaxLag = 0.4f * fScale;
    // 0x40A66667, one bit above 5.2f; EA's expression for it is not known.
    fMaxSpeed = 5.2000003f * fDelta * fScale;
    // EA bug: only x, y and z of the unit axes are set; fn_801143D0 copies w as it is.
    vUnitY[0] = 0.0f;
    vUnitY[1] = 1.0f;
    vUnitY[2] = 0.0f;
    vUnitX[0] = 1.0f;
    vUnitX[1] = 0.0f;
    vUnitX[2] = 0.0f;
    vUnitZ[0] = 0.0f;
    vUnitZ[1] = 0.0f;
    vUnitZ[2] = 1.0f;
    fn_801143D0(pModel->pMatrices[pChain->nBone], vUnitY, vUp);
    fn_801143D0(pModel->pMatrices[pChain->nBone], vUnitX, vX);
    fn_801143D0(pModel->pMatrices[pChain->nBone], vUnitZ, vZ);
    fn_800BAF04(vZ, vZ);
    fn_800BAF04(vX, vX);
    fn_800BAF04(vUp, vUp);
    Vec3Copy(vZ, vWas);
    vHang[0] = vZ[0] * pChain->pLinks->fLength + pModel->pMatrices[pChain->nBone][3][0];
    vHang[1] = vZ[1] * pChain->pLinks->fLength + pModel->pMatrices[pChain->nBone][3][1];
    vHang[2] = vZ[2] * pChain->pLinks->fLength + pModel->pMatrices[pChain->nBone][3][2];

    // Pulled toward where it hangs, and by gravity.
    fn_801164D4(vHang, pChain->pLinks->v04, vDiff);
    fLag = fn_80009680(fn_80009744(vDiff));
    fn_8001EF34(fDelta, vDiff, vDiff);
    fn_801164F8(pChain->pLinks->v24, vDiff, pChain->pLinks->v24);
    pChain->pLinks->v24[1] -= 0.018f * fDelta;
    fSpeed = fn_80009680(fn_80009744(pChain->pLinks->v24));
    if (fSpeed > fMaxSpeed) {
        fSpeed = fMaxSpeed;
        fn_800BAF04(pChain->pLinks->v24, pChain->pLinks->v24);
        fn_8001EF34(fMaxSpeed, pChain->pLinks->v24, pChain->pLinks->v24);
    }
    if (fLag > fMaxLag) {
        // Too far behind: put it back at the limit, moving toward where it hangs.
        fn_800BAF04(vDiff, vDiff);
        fn_8001EF34(-fMaxLag, vDiff, vBack);
        fn_801164F8(vHang, vBack, pChain->pLinks->v04);
        fn_801164D4(vHang, pChain->pLinks->v04, vDiff);
        fn_800BAF04(vDiff, vDiff);
        fn_8001EF34(fSpeed, vDiff, pChain->pLinks->v24);
    }
    fDrag = -0.069f * fDelta;
    if (fabs(fDrag) < fabs(fSpeed)) {
        fn_800BAF04(pChain->pLinks->v24, vDrag);
        fn_8001EF34(fDrag, vDrag, vDrag);
        fn_801164F8(pChain->pLinks->v24, vDrag, pChain->pLinks->v24);
        fn_801164F8(pChain->pLinks->v24, pChain->pLinks->v04, pChain->pLinks->v04);
    }

    // Drop the part of its offset along the matrix's y axis.
    fn_801164D4(vHang, pChain->pLinks->v04, vDiff);
    fn_8001EF34(fn_8000C5FC(vDiff, vUp), vUp, vMove);
    fn_801164F8(vMove, pChain->pLinks->v04, pChain->pLinks->v04);
    fn_801164D4(pChain->pLinks->v04, pModel->pMatrices[pChain->nBone][3], vTo);
    fn_800BAF04(vTo, vTo);
    pChain->pLinks->f8C = fLag;

    // Turn the bone from its z axis to the link.
    if ((vWas[0] - vTo[0]) * (vWas[0] - vTo[0]) + (vWas[1] - vTo[1]) * (vWas[1] - vTo[1]) +
            (vWas[2] - vTo[2]) * (vWas[2] - vTo[2]) >
        0.00001f) {
        fn_800BAF04(vTo, vTo);
        fn_800BAF04(vWas, vWas);
        vec4flt_CrossProduct(vTo, vWas, vAxis);
        fAngle = fn_80009614(fn_8000C5FC(vTo, vWas));
        if (fAngle > 0.00001f) {
            fn_800BAF04(vAxis, vAxis);
            vAxis[0] *= fAngle;
            vAxis[1] *= fAngle;
            vAxis[2] *= fAngle;
            Quat_BuildFromVector(vAxis, qTurn);
            Quat_Multiply(pModel->pPoses[pChain->nBone].q0, qTurn, pModel->pPoses[pChain->nBone].q0);
        }
        Quat_QuatToMatrix(pModel->pPoses[pChain->nBone].q0, mRot);
        fn_8000ADC0(mScale);
        mScale[0][0] = pModel->a140[pChain->nBone][0];
        mScale[1][1] = pModel->a140[pChain->nBone][1];
        mScale[2][2] = pModel->a140[pChain->nBone][2];
        Vec_Copy(pModel->pMatrices[pChain->nBone][3], vPos);
        fn_800BAE5C(mRot, mScale, pModel->pMatrices[pChain->nBone], 3);
        Vec_Copy(vPos, pModel->pMatrices[pChain->nBone][3]);
        SKEL_UpdateSkinningMatrix(pModel, pModel->pMatrices[pChain->nBone], pChain->nBone);
    }
    Vec3Copy(pChain->pLinks->v04, pChain->pLinks->v14);
}

// The type 0 update: each link below the top one trails behind, pulled back toward where it was
// (v64) the less the model's f130 and the further down the chain it is, kept from passing 0.025
// along the top bone's z axis and held at fLength from its parent. Each bone then turns by how its
// direction to the next one moved in this update (from v34, taken before the links move).
void fn_80114A84(CharModel* pModel, DynChain* pChain, f32 fDelta) {
    f32 mScale[4][4];
    f32 mRot[4][4];
    f32 vBone[4];
    f32 vWas[4];
    f32 vAxis[4];
    f32 vPos[4];
    f32 qTurn[4];
    f32 vParent[4];
    f32 vDiff[4];
    f32 vUnitZ[4];
    f32 vZ[4];
    f32 vTop[4];
    f32 vOff[4];
    f32 vPoint[4];
    f32 vSave[4];
    f32 fDrag;
    f32 fLoose;
    f32 fPull;
    f32 fStiff;
    f32 fDown;
    f32 fDist;
    f32 fFall;
    f32 fStep;
    f32 fDot;
    f32 fAngle;
    int i;

    fStiff = pModel->f130;
    // EA bug: vUnitZ's w is never set; fn_801143D0 copies it as it is.
    vUnitZ[0] = 0.0f;
    vUnitZ[1] = 0.0f;
    vUnitZ[2] = 1.0f;
    fn_801143D0(pModel->pMatrices[pChain->nBone], vUnitZ, vZ);
    fn_800BAF04(vZ, vZ);
    Vec3Copy(pModel->pMatrices[pChain->nBone][3], vTop);
    fn_8001EF34(0.025f, vZ, vOff);
    fn_801164F8(vTop, vOff, vTop);

    // Each link's offset to the next bone as the model stands now.
    for (i = 0; i < pChain->nLinks - 1; i++) {
        pChain->pLinks[i].v34[0] =
            pModel->pPoses[pChain->nBone + i + 1].v10[0] - pModel->pPoses[pChain->nBone + i].v10[0];
        pChain->pLinks[i].v34[1] =
            pModel->pPoses[pChain->nBone + i + 1].v10[1] - pModel->pPoses[pChain->nBone + i].v10[1];
        pChain->pLinks[i].v34[2] =
            pModel->pPoses[pChain->nBone + i + 1].v10[2] - pModel->pPoses[pChain->nBone + i].v10[2];
        pChain->pLinks[i].v34[3] = 0.0f;
    }

    // Move each link below the top one.
    fDrag = 0.5f * fDelta;
    fLoose = 1.0f - fStiff;
    fPull = 30.0f * fStiff * fDelta;
    for (i = 1; i < pChain->nLinks; i++) {
        fDown = (f32)i / (f32)(pChain->nLinks + 1);
        fn_8001E85C(pModel->pPoses[pChain->nBone + i].v10, vPos);
        pChain->pLinks[i].v64[3] = 1.0f;
        vPos[3] = 1.0f;
        vDiff[0] = vPos[0] - pChain->pLinks[i].v64[0];
        vDiff[1] = vPos[1] - pChain->pLinks[i].v64[1];
        vDiff[2] = vPos[2] - pChain->pLinks[i].v64[2];
        vDiff[3] = 0.0f;
        fDist = fn_800BAF58(vDiff, vDiff);
        if (fDist < 0.4f) {
            // Close to where it was: stay back, the more so the further down the chain.
            fFall = 1.0f - fDown;
            fStep = fFall * fDrag;
            fStep *= fLoose;
            fStep = fDist / 0.4f * (fFall * fPull) + fStep;
            if (fStep > fDist) {
                fStep = fDist;
            }
            vPos[0] = vDiff[0] * fStep + pChain->pLinks[i].v64[0];
            vPos[1] = vDiff[1] * fStep + pChain->pLinks[i].v64[1];
            vPos[2] = vDiff[2] * fStep + pChain->pLinks[i].v64[2];
        }
        vPoint[0] = vPos[0];
        vPoint[1] = vPos[1];
        vPoint[2] = vPos[2];
        fn_801164D4(vTop, vPoint, vOff);
        fDot = fn_8000C5FC(vOff, vZ);
        if (fDot < 0.0f) {
            vPos[0] += vZ[0] * fDot;
            vPos[1] += vZ[1] * fDot;
            vPos[2] += vZ[2] * fDot;
        }
        fn_8001E85C(pModel->pPoses[pChain->pLinks[i].nParent].v10, vParent);
        vDiff[0] = vPos[0] - vParent[0];
        vDiff[1] = vPos[1] - vParent[1];
        vDiff[2] = vPos[2] - vParent[2];
        if (0.0f != vDiff[0] && 0.0f != vDiff[1] && 0.0f != vDiff[2]) {
            fn_800BAF04(vDiff, vDiff);
        }
        vPos[0] = vDiff[0] * pChain->pLinks[i].fLength + vParent[0];
        vPos[1] = vDiff[1] * pChain->pLinks[i].fLength + vParent[1];
        vPos[2] = vDiff[2] * pChain->pLinks[i].fLength + vParent[2];
        fn_8001E85C(vPos, pChain->pLinks[i].v64);
        fn_8001E85C(vPos, pModel->pPoses[pChain->nBone + i].v10);
        fn_8001E880(vPos, pModel->pMatrices[pChain->nBone + i][3]);
        SKEL_UpdateSkinningMatrix(pModel, pModel->pMatrices[pChain->nBone + i], pChain->nBone + i);
    }

    // Turn each bone toward the next one, and rebuild its matrix.
    for (i = 0; i < pChain->nLinks; i++) {
        if (i + 1 < pChain->nLinks) {
            vBone[0] =
                pModel->pPoses[pChain->nBone + i + 1].v10[0] - pModel->pPoses[pChain->nBone + i].v10[0];
            vBone[1] =
                pModel->pPoses[pChain->nBone + i + 1].v10[1] - pModel->pPoses[pChain->nBone + i].v10[1];
            vBone[2] =
                pModel->pPoses[pChain->nBone + i + 1].v10[2] - pModel->pPoses[pChain->nBone + i].v10[2];
            vBone[3] = 0.0f;
            Vec3Copy(pChain->pLinks[i].v34, vWas);
            if (0.0f != vBone[0] || 0.0f != vBone[1] || 0.0f != vBone[2]) {
                fn_800BAF04(vBone, vBone);
            }
            if (0.0f != vWas[0] || 0.0f != vWas[1] || 0.0f != vWas[2]) {
                fn_800BAF04(vWas, vWas);
            }
            if ((f32)fn_80009680((vWas[0] - vBone[0]) * (vWas[0] - vBone[0]) +
                                 (vWas[1] - vBone[1]) * (vWas[1] - vBone[1]) +
                                 (vWas[2] - vBone[2]) * (vWas[2] - vBone[2])) > 0.001f) {
                vec4flt_CrossProduct(vWas, vBone, vAxis);
                fAngle = fn_80009614(fn_8000C5FC(vBone, vWas));
                if (fabs(fAngle) > 0.01f) {
                    if (0.0f != vAxis[0] || 0.0f != vAxis[1] || 0.0f != vAxis[2]) {
                        fn_800BAF04(vAxis, vAxis);
                    }
                    vAxis[0] *= fAngle;
                    vAxis[1] *= fAngle;
                    vAxis[2] *= fAngle;
                    Quat_BuildFromVector(vAxis, qTurn);
                    Quat_Multiply(pModel->pPoses[pChain->nBone + i].q0, qTurn,
                                pModel->pPoses[pChain->nBone + i].q0);
                }
            }
            Quat_QuatToMatrix(pModel->pPoses[pChain->nBone + i].q0, mRot);
            fn_8000ADC0(mScale);
            mScale[0][0] = pModel->a140[pChain->nBone + i][0];
            mScale[1][1] = pModel->a140[pChain->nBone + i][1];
            mScale[2][2] = pModel->a140[pChain->nBone + i][2];
            Vec_Copy(pModel->pMatrices[pChain->nBone + i][3], vSave);
            fn_800BAE5C(mRot, mScale, pModel->pMatrices[pChain->nBone + i], 3);
            Vec_Copy(vSave, pModel->pMatrices[pChain->nBone + i][3]);
            SKEL_UpdateSkinningMatrix(pModel, pModel->pMatrices[pChain->nBone + i], pChain->nBone + i);
        } else {
            // The last bone takes the rotation of the one above it (not in 4-link chains).
            if (pChain->nLinks != 4) {
                fn_8001E85C(pModel->pPoses[pChain->nBone + i - 1].q0, pModel->pPoses[pChain->nBone + i].q0);
            }
            Quat_QuatToMatrix(pModel->pPoses[pChain->nBone + i].q0, mRot);
            fn_8000ADC0(mScale);
            mScale[0][0] = pModel->a140[pChain->nBone + i][0];
            mScale[1][1] = pModel->a140[pChain->nBone + i][1];
            mScale[2][2] = pModel->a140[pChain->nBone + i][2];
            Vec_Copy(pModel->pMatrices[pChain->nBone + i][3], vSave);
            fn_800BAE5C(mRot, mScale, pModel->pMatrices[pChain->nBone + i], 3);
            Vec_Copy(vSave, pModel->pMatrices[pChain->nBone + i][3]);
            SKEL_UpdateSkinningMatrix(pModel, pModel->pMatrices[pChain->nBone + i], pChain->nBone + i);
        }
        fn_8001E85C(pChain->pLinks[i].q44, pChain->pLinks[i].q54);
        fn_8001E85C(pChain->pLinks[i].v64, pChain->pLinks[i].v74);
    }
}

// The type 2 update: the chain sways in the wind. Each link turns about two axes by sine waves
// (the two DynChainParams), bigger when the chain faces the wind (the model's f134) and the stronger
// the wind is (fn_80116468). The bones are posed with the sway and then set back to their rest
// rotations. Not on game type 3.
void fn_80115348(CharModel* pModel, DynChain* pChain, f32 fDelta) {
    f32 vAxis1[4];
    f32 vAxis2[4];
    f32 vDir[4];
    f32 vWind[4];
    f32 vFace[4];
    f32 qTurn[4];
    f32 qOut[4];
    u32 auBits[4];
    f32 fPeriod;
    f32 fPhase;
    f32 fSway;
    f32 fFacing;
    f32 fSpeedA;
    f32 fSizeA;
    f32 fSpeedB;
    f32 fSizeB;
    f32 fStrength;
    f32 fFaceAmt;
    f32 fT;
    f32 fSize;
    f32 fAngle;
    s32 nFrames;
    u32 nPeriod;
    int i;

    fFaceAmt = pModel->f134;
    if (gSession.nGameType == 3) {
        return;
    }
    fn_8001E938(auBits, 128);
    fStrength = fn_80116468();
    nFrames = 60.0f * (FRAME_RATE * fDelta);
    if (pChain->n10 < 3) {
        fPhase = 0.72f / 180.0f * PI * pChain->n10;
    } else {
        fPhase = 0.72f / 180.0f * PI * (pChain->n10 - 3) + PI / 4.0f;
    }
    fSway = fn_80116304(pChain->n14, fPhase, fStrength);
    pChain->n14++;
    nFrames = nFrames * fSway;
    pChain->n18 += nFrames;

    // Where the wind blows from.
    if (lbl_802824F8->nB4 == 0) {
        fn_800561CC(vWind);
    } else {
        // A fixed direction in the model's root space (1 is the same as 5).
        if (lbl_802824F8->nB4 == 2) {
            vDir[0] = 1.0f;
            vDir[1] = 0.0f;
            vDir[2] = 0.0f;
            vDir[3] = 0.0f;
        } else if (lbl_802824F8->nB4 == 3) {
            vDir[0] = 0.0f;
            vDir[1] = 0.0f;
            vDir[2] = 1.0f;
            vDir[3] = 0.0f;
        } else if (lbl_802824F8->nB4 == 4) {
            vDir[0] = -1.0f;
            vDir[1] = 0.0f;
            vDir[2] = 0.0f;
            vDir[3] = 0.0f;
        } else {
            vDir[0] = 0.0f;
            vDir[1] = 0.0f;
            vDir[2] = -1.0f;
            vDir[3] = 0.0f;
        }
        fn_800BAD60(pModel->pMatrices[0], (Vec4*)vDir, (Vec4*)vWind);
    }
    fn_800BAD60(pModel->pMatrices[0], (Vec4*)lbl_80193DE8[pChain->n10], (Vec4*)vFace);
    vFace[1] = 0.0f;
    vWind[1] = 0.0f;
    fFacing = fn_8000C5FC(vFace, vWind);
    fFacing = (1.0f + fFacing) / 2.0f;
    fFacing *= fFaceAmt;
    fSpeedA = (1.0f - lbl_802824F8->f7C) * fFacing + lbl_802824F8->f7C;
    fSizeA = (1.0f - lbl_802824F8->f78) * fFacing + lbl_802824F8->f78;
    if (fStrength < lbl_802824F8->nBC) {
        fT = fStrength / lbl_802824F8->nBC;
        fSpeedB = (1.0f - lbl_802824F8->f88) * fT + lbl_802824F8->f88;
        fSizeB = (1.0f - lbl_802824F8->f80) * fT + lbl_802824F8->f80;
    } else {
        if (35.0f == lbl_802824F8->nBC) {
            fT = 1.0f;
        } else {
            fT = (fStrength - lbl_802824F8->nBC) / (35.0f - lbl_802824F8->nBC);
        }
        fSpeedB = (lbl_802824F8->f8C - 1.0f) * fT + 1.0f;
        fSizeB = (lbl_802824F8->f84 - 1.0f) * fT + 1.0f;
    }

    // The first sway, keeping each bone's rest rotation.
    for (i = 0; i < pChain->nLinks; i++) {
        if (pChain->n10 == 2 || pChain->n10 == 5) {
            vAxis1[0] = 0.0f;
            vAxis1[1] = 0.0f;
            vAxis1[2] = 1.0f;
            vAxis1[3] = 0.0f;
        } else {
            vAxis1[0] = 1.0f;
            vAxis1[1] = 0.0f;
            vAxis1[2] = 0.0f;
            vAxis1[3] = 0.0f;
        }
        fPeriod = 60.0f * FRAME_RATE * lbl_802824F8->aParams[0].aC[i];
        if (0.0f != fSpeedA) {
            fPeriod = fPeriod * (1.0f / fSpeedA);
        } else {
            fPeriod = 100000000.0f;
        }
        if (0.0f != fSpeedB) {
            fPeriod = fPeriod * (1.0f / fSpeedB);
        } else {
            fPeriod = 100000000.0f;
        }
        fSize = DEG(lbl_802824F8->aParams[0].a0[i]);
        fSize *= fSizeA;
        fSize *= fSizeB;
        if (0.0f != fPeriod) {
            nPeriod = fPeriod;
            fT = (f32)(pChain->n18 % nPeriod) / fPeriod;
            fAngle = fSize * fn_800095F0(2.0f * PI * fT + PI * (2.0f * lbl_802824F8->aParams[0].a18[i]));
            fAngle += PI * lbl_802824F8->aParams[0].a24[i] / 180.0f;
        } else {
            fAngle = 0.0f;
        }
        fn_8000AE28(fAngle, vAxis1, vAxis1);
        Quat_BuildFromVector(vAxis1, qTurn);
        fn_8001E85C(pModel->pBones[pChain->pLinks[i].nBone].q0C, pChain->pLinks[i].q44);
        if (lbl_802824F8->an9C[pChain->n10] != 0) {
            Quat_Multiply(pModel->pBones[pChain->pLinks[i].nBone].q0C, qTurn, qOut);
            fn_8001E85C(qOut, pModel->pBones[pChain->pLinks[i].nBone].q0C);
            fn_8001EA34(auBits, pChain->pLinks[i].nBone);
        }
    }

    // The second sway, about the other axis.
    for (i = 0; i < pChain->nLinks; i++) {
        if (pChain->n10 == 2 || pChain->n10 == 5) {
            vAxis2[0] = 1.0f;
            vAxis2[1] = 0.0f;
            vAxis2[2] = 0.0f;
            vAxis2[3] = 0.0f;
        } else {
            vAxis2[0] = 0.0f;
            vAxis2[1] = 0.0f;
            vAxis2[2] = 1.0f;
            vAxis2[3] = 0.0f;
        }
        fPeriod = 60.0f * FRAME_RATE * lbl_802824F8->aParams[1].aC[i];
        if (0.0f != fSpeedA) {
            fPeriod = fPeriod * (1.0f / fSpeedA);
        } else {
            fPeriod = 100000000.0f;
        }
        if (0.0f != fSpeedB) {
            fPeriod = fPeriod * (1.0f / fSpeedB);
        } else {
            fPeriod = 100000000.0f;
        }
        fSize = DEG(lbl_802824F8->aParams[1].a0[i]);
        fSize *= fSizeA;
        fSize *= fSizeB;
        if (0.0f != fPeriod) {
            nPeriod = fPeriod;
            fT = (f32)(pChain->n18 % nPeriod) / fPeriod;
            fAngle = fSize * fn_800095F0(2.0f * PI * fT + PI * (2.0f * lbl_802824F8->aParams[1].a18[i]));
            fAngle += PI * lbl_802824F8->aParams[1].a24[i] / 180.0f;
        } else {
            fAngle = 0.0f;
        }
        fn_8000AE28(fAngle, vAxis2, vAxis2);
        Quat_BuildFromVector(vAxis2, qTurn);
        if (lbl_802824F8->an9C[pChain->n10] != 0) {
            Quat_Multiply(pModel->pBones[pChain->pLinks[i].nBone].q0C, qTurn, qOut);
            fn_8001E85C(qOut, pModel->pBones[pChain->pLinks[i].nBone].q0C);
            fn_8001EA34(auBits, pChain->pLinks[i].nBone);
        }
    }
    SKEL_TransformBones(pModel, auBits);
    for (i = 0; i < pChain->nLinks; i++) {
        fn_8001E85C(pChain->pLinks[i].q44, pModel->pBones[pChain->pLinks[i].nBone].q0C);
    }
}

// The type 3 update, for a one-link chain: the link bends down toward the ground the more it points
// the way lbl_80193E48 gives for its kind and the more level it is (the settings' f68 degrees at
// most, times the model's f13C), then sways about the axis across it and a reference bone (bone id
// 0x12 for kinds 0-2, 0x25 for 3-5) by a sine wave. Not on game type 3.
void fn_80115B2C(CharModel* pModel, DynChain* pChain, f32 fDelta) {
    f32 qTurn[4];
    f32 vBoneN[4];
    f32 vBone[4];
    f32 vDown[4];
    f32 vNew[4];
    f32 vSide[4];
    f32 vAxis[4];
    f32 vAxisN[4];
    f32 vTurn[4];
    f32 vRef[4];
    f32 vRefN[4];
    f32 vDir[4];
    f32 vDirN[4];
    f32 vSwung[4];
    f32 fX;
    f32 fY;
    f32 fZ;
    f32 fStrength;
    f32 fSway;
    f32 fLevel;
    f32 fBend;
    f32 fOff;
    f32 fT;
    f32 fSpeed;
    f32 fSize;
    f32 fPeriod;
    f32 fAngle;
    f32 fScale;
    s32 nFrames;
    f32 fAmp;
    f32 fWave;
    u32 nPeriod;
    int nRef;
    int i;

    if (gSession.nGameType == 3) {
        return;
    }
    fStrength = fn_80116468();
    nFrames = 60.0f * (FRAME_RATE * fDelta);
    fBend = pModel->f13C;
    fSway = fn_80116304(pChain->n14, PI * pChain->n10 / 8.0f, fStrength);
    pChain->n14++;
    nFrames = nFrames * fSway;
    pChain->n18 += nFrames;
    fn_8011651C(pModel->pMatrices[pChain->pLinks->nBone][3], pModel->pMatrices[pChain->pLinks->nParent][3],
                vBone);
    if (0.0f != vBone[0] || 0.0f != vBone[1] || 0.0f != vBone[2]) {
        // Bend down, by how level the link is and how near it points to the kind's direction.
        fn_800BAF04(vBone, vBoneN);
        fLevel = 1.0f - fabsf(vBoneN[1]);
        Quat_ExtractEulerAngles(pModel->pPoses[pChain->pLinks->nParent].q0, &fX, &fY, &fZ);
        fOff = fabsf(180.0f / PI * fY - lbl_80193E48[pChain->n10]);
        if (fOff > 360.0f) {
            fOff = (s32)fOff % 360;
        }
        if (fOff > lbl_802824F8->f70 || 0.0f == lbl_802824F8->f70) {
            fLevel = 0.0f;
        } else {
            fLevel *= 1.0f - fOff / lbl_802824F8->f70;
        }
        fLevel *= fBend;
        Vec_Copy(vBone, vDown);
        vDown[1] = vDown[1] - 1.0f;
        vec4flt_CrossProduct(vBone, vDown, vAxis);
        if (0.0f != vAxis[0] || 0.0f != vAxis[1] || 0.0f != vAxis[2]) {
            fn_800BAF04(vAxis, vAxisN);
            fn_8000AE28(DEG(lbl_802824F8->f68) * fLevel, vAxisN, vTurn);
            Quat_BuildFromVector(vTurn, qTurn);
            Quat_RotateVector(qTurn, vBone, vNew);
            fn_801164F8(vNew, pModel->pMatrices[pChain->pLinks->nParent][3],
                        pModel->pMatrices[pChain->pLinks->nBone][3]);

            // The sway, faster and bigger in a stronger wind.
            if (fStrength < lbl_802824F8->nBC) {
                fT = fStrength / lbl_802824F8->nBC;
                fSpeed = (1.0f - lbl_802824F8->f88) * fT + lbl_802824F8->f88;
                fSize = (1.0f - lbl_802824F8->f80) * fT + lbl_802824F8->f80;
            } else {
                if (35.0f == lbl_802824F8->nBC) {
                    fT = 1.0f;
                } else {
                    fT = (fStrength - lbl_802824F8->nBC) / (35.0f - lbl_802824F8->nBC);
                }
                // Unlike fn_80115348, both come from f8C, as 1 - f8C.
                fScale = (1.0f - lbl_802824F8->f8C) * fT + 1.0f;
                fSpeed = fScale;
                fSize = fScale;
            }
            fPeriod = 60.0f * FRAME_RATE * lbl_802824F8->f64;
            if (0.0f != fSpeed) {
                fPeriod = fPeriod * (1.0f / fSpeed);
            } else {
                fPeriod = 100000000.0f;
            }
            fAmp = DEG(lbl_802824F8->f60);
            fAmp *= fSize;
            if (0.0f != fPeriod) {
                nPeriod = fPeriod;
                fWave = fn_800095F0(2.0f * PI * ((f32)(pChain->n18 % nPeriod) / fPeriod) +
                                    pChain->n10 / 0.5f);
                fAngle = fAmp * fWave;
            } else {
                fAngle = 0.0f;
            }
            fn_8001E85C(pModel->pBones[pChain->pLinks->nBone].q0C, pChain->pLinks->q44);
            fn_8011651C(pModel->pMatrices[pChain->pLinks->nBone][3],
                        pModel->pMatrices[pChain->pLinks->nParent][3], vDir);
            if (!(fabsf(vDir[0]) < 0.001f) || !(fabsf(vDir[1]) < 0.001f) || !(fabsf(vDir[2]) < 0.001f)) {
                fn_800BAF04(vDir, vDirN);
                if (pChain->n10 < 3) {
                    nRef = fn_8001EED8(pModel, 0x12);
                } else {
                    nRef = fn_8001EED8(pModel, 0x25);
                }
                fn_8011651C(pModel->pMatrices[nRef][3], pModel->pMatrices[pChain->pLinks->nParent][3], vRef);
                if (!(fabsf(vRef[0]) < 0.001f) || !(fabsf(vRef[1]) < 0.001f) || !(fabsf(vRef[2]) < 0.001f)) {
                    fn_800BAF04(vRef, vRefN);
                    vec4flt_CrossProduct(vRefN, vDirN, vSide);
                    if (!(fabsf(vSide[0]) < 0.001f) || !(fabsf(vSide[1]) < 0.001f) ||
                        !(fabsf(vSide[2]) < 0.001f)) {
                        fn_800BAF04(vSide, vSide);
                        vec4flt_CrossProduct(vSide, vDirN, vAxis);
                        if (!(fabsf(vAxis[0]) < 0.001f) || !(fabsf(vAxis[1]) < 0.001f) ||
                            !(fabsf(vAxis[2]) < 0.001f)) {
                            fn_800BAF04(vAxis, vAxis);
                            fn_8001EF34(lbl_802824F8->f6C, vSide, vSide);
                            fn_801164F8(vSide, vAxis, vAxis);
                            if (!(fabsf(vAxis[0]) < 0.001f) || !(fabsf(vAxis[1]) < 0.001f) ||
                                !(fabsf(vAxis[2]) < 0.001f)) {
                                fn_800BAF04(vAxis, vAxis);
                                fn_8000AE28(fAngle, vAxis, vAxis);
                                Quat_BuildFromVector(vAxis, qTurn);
                                Quat_RotateVector(qTurn, vDir, vSwung);
                                fn_801164F8(vSwung, pModel->pMatrices[pChain->pLinks->nParent][3],
                                            pModel->pMatrices[pChain->pLinks->nBone][3]);
                                SKEL_UpdateSkinningMatrix(pModel, pModel->pMatrices[pChain->pLinks->nBone],
                                            pChain->pLinks->nBone);
                                for (i = 0; i < pChain->nLinks; i++) {
                                    fn_8001E85C(pChain->pLinks[i].q44,
                                                pModel->pBones[pChain->pLinks[i].nBone].q0C);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// The sway's speed at frame nFrame (its callers scale their frame step by it; 1: full speed):
// four sine waves with a 600-frame period (1, 3, 5 and 7 times the base frequency), offset by
// fPhase, on a base that grows with fStrength (0..35), limited to 0..1 and scaled by the settings'
// f90.
f32 fn_80116304(u32 nFrame, f32 fPhase, f32 fStrength) {
    f32 fPeriod = 600.0f;
    f32 fOne = 1.0f;            // the wave's scale and the most sway (EA kept the multiply)
    f32 fAngle;
    f32 fWave;
    f32 fSway;

    fAngle = PI * (2.0f * ((f32)(nFrame % (u32)fPeriod) / fPeriod));
    // Four waves; CodeWarrior calls the right operand first, so the 3x wave is made first.
    fWave = fn_800095F0(7.0f * fAngle + 6.0f + fPhase) +
            (fn_800095F0(5.0f * fAngle + 4.0f + fPhase) +
             (fn_800095F0(fAngle + fPhase) + fn_800095F0(3.0f * fAngle + 2.0f + fPhase)));
    fSway = fOne * fWave;
    fSway += lbl_802824F8->f94 + fStrength * (lbl_802824F8->f98 - lbl_802824F8->f94) / 35.0f;
    fSway = (fSway < 0.0f) ? 0.0f : ((fSway > fOne) ? fOne : fSway);
    return 1.0f - lbl_802824F8->f90 * (fOne - fSway);
}

// The strength the chains sway with: the settings' nB8, or fn_80055F80's when it is -1; at least 5.
f32 fn_80116468(void) {
    f32 fStrength;

    if (lbl_802824F8->nB8 == -1) {
        fStrength = fn_80055F80();
    } else if (lbl_802824F8->nB8 == -1) {
        fStrength = 0.0f;
    } else {
        fStrength = lbl_802824F8->nB8;
    }
    if (fStrength < 5.0f) {
        fStrength = 5.0f;
    }
    return fStrength;
}

// Three floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void fn_801164D4(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_801164D4(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// Three floats: pOut gets pB plus pA.
#ifdef __MWERKS__
asm void fn_801164F8(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_801164F8(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// Four floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void fn_8011651C(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_8011651C(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

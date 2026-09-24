// GoAnimalActors.c (TW06's golf/hi-rendering/goanimalactors.c; our spelling): the animals on the
// course (ActAnimal_*). Not yet decompiled; the unit covers the file's certain core, whose
// functions share its constant block (0x80283218-0x80283280).

#include "dynobj.h"
#include "ball.h"
#include "camera.h"

void fn_8000ADC0(f32 (*pMtx)[4]);                       // identity
void fn_8000A194(f32 (*pMtx)[4], f32 a, f32 b, f32 c);  // a rotation matrix from three angles
void fn_8000C5A4(f32 (*pMtx)[4]);
void ActAnimal_SetWorldMatrix(DynObjAnimal* pAnimal, f32 f);
f32  fn_800351D8(u32 n, f32 fPeriod);                   // GoTerrain.c
int  fn_8004AB90(UObjMesh* pMesh, int i);
UObjMesh* fn_8004ABA4(UObjMesh* pMesh, int i);
UObjMesh* fn_8004ABB4(UObjModelRoot* pRoot);
void fn_8004A24C(DynObjAnimal* pAnimal, DynObjSetup* pSetup);  // message 2: sets it up
void fn_8004A578(DynObjAnimal* pAnimal, void* pArg);            // message 6: pArg holds the frame
                                                                // time's bits

// Places the animal on its route at f194 (0..1 round it): a spline through the four points around
// that place (camera spline code, fn_800C7480) gives its position and heading, dropped onto the
// ground when b1BD is set and raised by f198. Then the matrix faces the heading, and f19C turns
// the animal about it (f1A0 the angle so far, fDt the frame time) until it comes back upright.
void ActAnimal_SetWorldMatrix(DynObjAnimal* pAnimal, f32 fDt) {
    f32 aPos[4][4];
    f32 aTan[4][4];
    AnimalNode* apNodes[5];
    f32 vDir[4];
    f32 vGround[4];
    f32 fUnused;
    f32 fA;
    f32 fB;
    f32 fC;
    AnimalRoute* pRoute;
    AnimalNode* pNode;
    CourseInfo* pCourse;
    f32 fPos;
    f32 fT;
    f32 fGround;
    int nSkip;
    int i;

    if (pAnimal->pRoute == NULL) {
        return;
    }
    while (pAnimal->f194 >= 1.0f) {
        pAnimal->f194 -= 1.0f;
    }
    pRoute = pAnimal->pRoute;
    fPos = pRoute->nNodes * pAnimal->f194;
    nSkip = fPos;
    fT = fPos - nSkip;
    nSkip -= 2;
    if (nSkip < 0) {
        nSkip += pRoute->nNodes;
    }
    pNode = &pRoute->aNodes[0];
    for (i = 0; i < nSkip; i++) {
        pNode = &pRoute->aNodes[pNode->nNext];
    }
    apNodes[0] = &pRoute->aNodes[pNode->nNext];
    apNodes[1] = &pAnimal->pRoute->aNodes[apNodes[0]->nNext];
    apNodes[2] = &pAnimal->pRoute->aNodes[apNodes[1]->nNext];
    apNodes[3] = &pAnimal->pRoute->aNodes[apNodes[2]->nNext];
    apNodes[4] = &pAnimal->pRoute->aNodes[apNodes[3]->nNext];

    // the four points and their tangents (half the steps in and out)
    for (i = 0; i < 4; i++) {
        aPos[i][0] = apNodes[i]->vPos[0];
        aPos[i][1] = apNodes[i]->vPos[1];
        aPos[i][2] = apNodes[i]->vPos[2];
        aPos[i][3] = 0.0f;
        if (i == 0) {
            aTan[i][0] = 0.5f * ((apNodes[i]->vPos[0] - pNode->vPos[0]) +
                                 (apNodes[i + 1]->vPos[0] - apNodes[i]->vPos[0]));
            aTan[i][1] = 0.5f * ((apNodes[i]->vPos[1] - pNode->vPos[1]) +
                                 (apNodes[i + 1]->vPos[1] - apNodes[i]->vPos[1]));
            aTan[i][2] = 0.5f * ((apNodes[i]->vPos[2] - pNode->vPos[2]) +
                                 (apNodes[i + 1]->vPos[2] - apNodes[i]->vPos[2]));
            aTan[i][3] = 0.0f;
        } else {
            aTan[i][0] = 0.5f * ((apNodes[i + 1]->vPos[0] - apNodes[i]->vPos[0]) +
                                 (apNodes[i]->vPos[0] - apNodes[i - 1]->vPos[0]));
            aTan[i][1] = 0.5f * ((apNodes[i + 1]->vPos[1] - apNodes[i]->vPos[1]) +
                                 (apNodes[i]->vPos[1] - apNodes[i - 1]->vPos[1]));
            aTan[i][2] = 0.5f * ((apNodes[i + 1]->vPos[2] - apNodes[i]->vPos[2]) +
                                 (apNodes[i]->vPos[2] - apNodes[i - 1]->vPos[2]));
            aTan[i][3] = 0.0f;
        }
        if (aTan[i][0] || aTan[i][1] || aTan[i][2]) {
            fn_800BAF04(aTan[i], aTan[i]);
        }
    }
    fn_800C7480(aPos[0], aPos[1], aPos[2], aPos[3], aTan[0], aTan[1], aTan[2], aTan[3],
                pAnimal->base.obj.m80[3], vDir, &fUnused, 0.0f, 0.0f, fT);
    vDir[3] = 0.0f;
    if (pAnimal->b1BD && (pCourse = fn_8000C594()) != NULL) {
        vGround[0] = pAnimal->base.obj.m80[3][0];
        vGround[1] = 10.0f + pAnimal->base.obj.m80[3][1];
        vGround[2] = pAnimal->base.obj.m80[3][2];
        vGround[3] = 1.0f;
        fGround = fn_8004D620(pCourse, vGround);
        if (-65536.125f != fGround) {
            pAnimal->base.obj.m80[3][1] = fGround;
        }
    }
    pAnimal->base.obj.m80[3][1] = pAnimal->base.obj.m80[3][1] + pAnimal->f198;

    // face the heading: row 2 along it, row 0 level across it, row 1 up
    if (vDir[0] || vDir[1] || vDir[2]) {
        fn_800BAF04(vDir, pAnimal->base.obj.m0[2]);
    }
    if (fabsf(pAnimal->base.obj.m0[2][1]) < 0.98f) {
        pAnimal->base.obj.m0[0][0] = pAnimal->base.obj.m0[2][2];
        pAnimal->base.obj.m0[0][1] = 0.0f;
        pAnimal->base.obj.m0[0][2] = -pAnimal->base.obj.m0[2][0];
        pAnimal->base.obj.m0[0][3] = 0.0f;
        fn_800BAF04(pAnimal->base.obj.m0[0], pAnimal->base.obj.m0[0]);
    }
    vec4flt_CrossProduct(pAnimal->base.obj.m0[2], pAnimal->base.obj.m0[0], pAnimal->base.obj.m0[1]);

    // the roll: f19C speeds up to at most 72 degrees a second each way, and stops once the angle
    // is back near upright
    if (pAnimal->f19C) {
        pAnimal->f1A0 = pAnimal->f19C * fDt + pAnimal->f1A0;
        if (pAnimal->f19C < 0.0f) {
            if (pAnimal->f1A0 < 0.0f) {
                pAnimal->f1A0 = pAnimal->f1A0 + 6.2831855f;
            }
            pAnimal->f19C = 6.2831855f * fDt + pAnimal->f19C;
            if (pAnimal->f19C > -72.0f / 180.0f * PI) {
                pAnimal->f19C = -72.0f / 180.0f * PI;
            }
        } else {
            if (pAnimal->f1A0 > 6.2831855f) {
                pAnimal->f1A0 = pAnimal->f1A0 - 6.2831855f;
            }
            pAnimal->f19C = pAnimal->f19C - 6.2831855f * fDt;
            if (pAnimal->f19C < 72.0f / 180.0f * PI) {
                pAnimal->f19C = 72.0f / 180.0f * PI;
            }
        }
        if (72.0f / 180.0f * PI == pAnimal->f19C || -72.0f / 180.0f * PI == pAnimal->f19C) {
            if (pAnimal->f1A0 < PI * 6.0f / 180.0f ||
                (pAnimal->f1A0 > PI * 177.0f / 180.0f && pAnimal->f1A0 < 183.0f / 180.0f * PI)) {
                pAnimal->f1A0 = 0.0f;
                pAnimal->f19C = 0.0f;
            }
        }
        fn_8000A4E0(pAnimal->base.obj.m0, &fA, &fB, &fC);
        fA = pAnimal->f1A0;
        if (fA > 6.2831855f) {
            fA = fA - 6.2831855f;
        }
        fn_8000A194(pAnimal->base.obj.m0, fA, fB, fC);
    }
    fn_8000C5A4(pAnimal->base.obj.m0);
}

// Scales f16C and f174 by the length of the animal's route (once round its points).
void fn_8004A14C(DynObjAnimal* pAnimal) {
    f32 vStep[4];
    f32 fLength = 0.0f;
    AnimalNode* pNode;
    int i;
    AnimalNode* pNext;

    if (pAnimal->pRoute != NULL) {
        pNode = &pAnimal->pRoute->aNodes[0];
        for (i = 0; i < pAnimal->pRoute->nNodes; i++) {
            pNext = &pAnimal->pRoute->aNodes[pNode->nNext];
            vStep[0] = pNext->vPos[0] - pNode->vPos[0];
            vStep[1] = pNext->vPos[1] - pNode->vPos[1];
            vStep[2] = pNext->vPos[2] - pNode->vPos[2];
            vStep[3] = 1.0f;
            fLength += (f32)fn_80009680(fn_80009744(vStep));
            pNode = pNext;
        }
        pAnimal->f16C /= fLength;
        pAnimal->f174 /= fLength;
    }
}

// Message 2: sets the animal up from its definition (type 0's setup first), picks its n1A4 from
// its model's mesh bits and its route from the second stream object, then places it: along the
// route, or at the definition's position raised by f198.
void fn_8004A24C(DynObjAnimal* pAnimal, DynObjSetup* pSetup) {
    f32 vPos[4];
    DynObjModel* pModel;
    int i;
    DynObjAnimalDef* pDef;
    DynObjModelRef* pRef;
    UObjMesh* pMesh;
    int nBitsA;
    int nBitsB;
    CourseInfo* pCourse;
    f32 fGround;
    f32 fY;

    fn_80049514(&pAnimal->base, pSetup);
    pDef = (DynObjAnimalDef*)pSetup->pDef;
    pAnimal->f16C = pDef->f1C;
    pAnimal->f170 = 0.0f;
    pAnimal->f174 = 2.0f * pAnimal->f16C;
    pAnimal->a178[0] = pDef->a20[0];
    pAnimal->a178[1] = pDef->a20[1];
    pAnimal->a178[2] = pDef->a20[2];
    pAnimal->a178[3] = pDef->a20[3];
    pAnimal->a178[4] = pDef->a20[4];
    pAnimal->f18C = 0.0f;
    pAnimal->f190 = 0.0f;
    pAnimal->f194 = pDef->f34;
    pAnimal->f198 = pDef->f38;
    pAnimal->n1AC = 0;
    pAnimal->n1B0 = 0;
    pAnimal->f1B4 = 0.5f;
    pAnimal->n1B8 = 0;
    pAnimal->b1BC = 0;

    pModel = pSetup->pModel;
    if (pModel != NULL) {
        pRef = pModel->aEntries[0].u.pRef;
    } else {
        pRef = NULL;
    }
    if (pRef != NULL) {
        pMesh = fn_8004ABB4(pRef->p4->p10);
        pMesh = fn_8004ABA4(pMesh, 0);
        pMesh = fn_8004ABA4(pMesh, 0);
        nBitsA = fn_8004AB90(pMesh, 0);
        nBitsB = fn_8004AB90(pMesh, 2);
        if (nBitsA & 1) {
            if (nBitsA & 2) {
                pAnimal->n1A4 = 2;
            } else if (nBitsB & 1) {
                pAnimal->n1A4 = 0;
            } else if (nBitsB & 2) {
                pAnimal->n1A4 = 1;
            } else {
                pAnimal->n1A4 = 3;
            }
        } else {
            pAnimal->n1A4 = 4;
        }
    } else {
        pAnimal->n1A4 = 4;
    }

    pAnimal->b1BD = 0;
    if (pModel != NULL && (u32)pModel->nEntries > 1) {  // fake match: EA compares unsigned here
        pAnimal->pRoute = (AnimalRoute*)(pModel->aEntries[1].u.pRef->pData + 12);
        pCourse = fn_8000C594();
        for (i = 0; i < pAnimal->pRoute->nNodes; i++) {
            vPos[0] = pAnimal->pRoute->aNodes[i].vPos[0];
            fY = pAnimal->pRoute->aNodes[i].vPos[1];
            vPos[1] = 10.0f + fY;
            vPos[2] = pAnimal->pRoute->aNodes[i].vPos[2];
            vPos[3] = 1.0f;
            if (pCourse != NULL) {
                fGround = fn_8004D620(pCourse, vPos);
                if (fGround != -65536.125f) {
                    if (fY - fGround < 0.5f && pAnimal->f198 != -999.0f) {
                        pAnimal->b1BD = 1;
                    } else {
                        pAnimal->b1BD = 0;
                    }
                    break;
                }
            }
        }
    } else {
        pAnimal->pRoute = NULL;
    }
    if (pAnimal->f198 == -999.0f) {
        pAnimal->f198 = 0.0f;
    }
    pAnimal->f19C = 0.0f;
    pAnimal->f1A0 = 0.0f;
    fn_8000A194(pAnimal->base.obj.m0, pDef->aAngles[1] * (PI / 180.0f), pDef->aAngles[0] * (PI / 180.0f),
                pDef->aAngles[2] * (PI / 180.0f));
    fn_8000ADC0(pAnimal->base.obj.m40);
    fn_8000C5A4(pAnimal->base.obj.m0);
    if (pAnimal->pRoute != NULL) {
        ActAnimal_SetWorldMatrix(pAnimal, 0.0f);
    } else {
        pAnimal->base.obj.m80[3][0] = pDef->base.aPos[0];
        pAnimal->base.obj.m80[3][1] = pDef->base.aPos[1] + pAnimal->f198;
        pAnimal->base.obj.m80[3][2] = pDef->base.aPos[2];
    }
    fn_8004A14C(pAnimal);
}

// A wave from 0 to 1 and back, fRate times a second, at frame nFrame (our macro: EA's code reads
// fRate twice at each use, which an inline function would not).
#define ANIMAL_WAVE(nFrame, fRate) \
    (0.5f * fn_800095F0(6.2831855f * (fRate) * fn_800351D8((nFrame), 1.0f / (fRate))) + 0.5f)

// Message 6, the per-frame update: moves the animal along its route, speeds it up or slows it
// down (b1BC: moving), counts down its moving (f190) and resting (f18C) times, and animates its
// pose (n1AC, blended by f1B4) by its kind n1A4.
void fn_8004A578(DynObjAnimal* pAnimal, void* pArg) {
    f32 fDt;
    f32 fDiff;
    f32 fMaxA;
    f32 fMinA;
    f32 fMaxB;
    f32 fMinB;
    u32 i;

    fDt = *(f32*)&pArg;  // port: the frame time's float bits arrive as the message argument
    if (fDt > 0.50050056f) {
        fDt = 0.50050056f;
    }
    if (fDt < 0.0f) {
        fDt = 0.0f;
    }
    if (0.0f == fDt) {
        return;
    }
    if (pAnimal->pRoute != NULL && pAnimal->f170) {
        pAnimal->f194 = pAnimal->f170 * fDt + pAnimal->f194;
        ActAnimal_SetWorldMatrix(pAnimal, fDt);
    }
    if (pAnimal->b1BC && pAnimal->f170 < pAnimal->f16C) {
        pAnimal->f170 = pAnimal->f174 * fDt + pAnimal->f170;
        if (pAnimal->f170 > pAnimal->f16C) {
            pAnimal->f170 = pAnimal->f16C;
        }
    }
    if (!pAnimal->b1BC && pAnimal->f170 > 0.0f) {
        pAnimal->f170 = pAnimal->f170 - pAnimal->f174 * fDt;
        if (pAnimal->f170 < 0.0f) {
            pAnimal->f170 = 0.0f;
        }
    }
    pAnimal->f190 -= fDt;
    if (pAnimal->f190 <= 0.0f) {
        pAnimal->b1BC = 0;
        switch (pAnimal->n1A4) {
        case 2:
            pAnimal->n1B0 = 0;
            break;
        case 1:
            pAnimal->n1B0 = 0;
            break;
        case 0:
        case 3:
            break;
        }
    }
    pAnimal->f18C -= fDt;
    if (pAnimal->f18C <= 0.0f) {
        pAnimal->b1BC = 1;
        pAnimal->f190 = pAnimal->a178[4];
        switch (pAnimal->n1A4) {
        case 2:
            pAnimal->n1B0 = 3;
            break;
        case 1:
            pAnimal->n1B0 = 3;
            break;
        case 0:
        case 3:
            break;
        }
        pAnimal->f18C = pAnimal->a178[3] * Rand_Float(1) + pAnimal->a178[2];
    }
    pAnimal->n1B8 += (u32)(59.94f * fDt);  // port: frames at the NTSC rate

    switch (pAnimal->n1A4) {
    case 3:
        pAnimal->f1B4 = ANIMAL_WAVE(pAnimal->n1B8, pAnimal->a178[0]);
        break;
    case 1:
        if ((pAnimal->n1AC == 0 && pAnimal->n1B0 == 0) || (pAnimal->n1AC >= 3 && pAnimal->n1B0 == 3) ||
            pAnimal->n1AC > 3) {
            if (pAnimal->n1AC >= 3) {
                pAnimal->f1B4 = 5.0f * pAnimal->a178[1] * fDt + pAnimal->f1B4;
                while (pAnimal->f1B4 > 1.0f) {
                    pAnimal->n1AC++;
                    if (pAnimal->n1AC > 7) {
                        pAnimal->n1AC = 3;
                    }
                    pAnimal->f1B4 -= 1.0f;
                }
            } else {
                pAnimal->f1B4 = ANIMAL_WAVE(pAnimal->n1B8, pAnimal->a178[0]);
            }
            break;
        }
        // fall through
    case 2:
        if (pAnimal->n1AC == pAnimal->n1B0) {
            if (pAnimal->n1AC == 0) {
                pAnimal->f1B4 = ANIMAL_WAVE(pAnimal->n1B8, pAnimal->a178[0]);
            } else {
                pAnimal->f1B4 = ANIMAL_WAVE(pAnimal->n1B8, pAnimal->a178[1]);
            }
        } else {
            fMaxA = 6.0f * fDt;
            fMinA = -6.0f * fDt;
            fMaxB = 4.0f * fDt;
            fMinB = -4.0f * fDt;
            for (i = 0; i < 6; i++) {
                if (pAnimal->n1AC == lbl_80187DF0[i].nFrom && pAnimal->n1B0 == lbl_80187DF0[i].nTo) {
                    fDiff = lbl_80187DF0[i].fTarget - pAnimal->f1B4;
                    if (pAnimal->n1AC == 2 || pAnimal->n1AC == 3) {
                        if (fDiff > fMaxA) {
                            fDiff = fMaxA;
                        }
                        if (fDiff < fMinA) {
                            fDiff = fMinA;
                        }
                    } else {
                        if (fDiff > fMaxB) {
                            fDiff = fMaxB;
                        }
                        if (fDiff < fMinB) {
                            fDiff = fMinB;
                        }
                    }
                    pAnimal->f1B4 += fDiff;
                    if (fabsf(pAnimal->f1B4 - lbl_80187DF0[i].fTarget) < 0.01f) {
                        pAnimal->n1AC = lbl_80187DF0[i].nNext;
                        pAnimal->f1B4 = lbl_80187DF0[i].fStart;
                        pAnimal->n1B8 = 0;
                        return;
                    }
                }
            }
        }
        break;
    case 0:
        pAnimal->f1B4 = ANIMAL_WAVE(pAnimal->n1B8, pAnimal->a178[0]);
        if (pAnimal->f1B4 < 0.5f) {
            pAnimal->f1B4 *= 2.0f;
            pAnimal->n1AC = 0;
        } else {
            pAnimal->f1B4 -= 0.5f;
            pAnimal->f1B4 *= 2.0f;
            pAnimal->n1AC = 1;
        }
        break;
    }
}

// Type 11's message handler; other messages go to type 0's.
int fn_8004AAEC(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObjAnimal);
    case 2:
        fn_8004A24C((DynObjAnimal*)pObj, pArg);
        return 0;
    case 6:
        fn_8004A578((DynObjAnimal*)pObj, pArg);
        return 0;
    case 3:
        if (pObj->obj.pModel != NULL) {
            pObj->obj.n108 = ((DynObjAnimal*)pObj)->n1AC;
            pObj->obj.f10C = ((DynObjAnimal*)pObj)->f1B4;
            fn_80048894(&pObj->obj);
        }
        return 0;
    default:
        return fn_80049820(nMsg, pObj, pArg, pArg2);
    }
}

// The same as UObject.c's fn_80048AD4, compiled into this file too.
int fn_8004AB90(UObjMesh* pMesh, int i) {
    return pMesh->pInfo->a24[i];
}

// The same as UObject.c's fn_80048AC4, compiled into this file too.
UObjMesh* fn_8004ABA4(UObjMesh* pMesh, int i) {
    return pMesh->p8[i];
}

UObjMesh* fn_8004ABB4(UObjModelRoot* pRoot) {
    return pRoot->pMesh;
}

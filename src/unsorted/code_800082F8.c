// Accessors for the camera and for drawable objects. Original file name and field meanings unknown.

#include "unsorted/cull.h"

extern void fn_800070DC(void);

Sphere* fn_800082F8(RenderObj* obj) {
    return &obj->data->bounds;
}

void Vec3Copy(const f32* pSrc, f32* pDst) {
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
}

float fn_80008320(Camera* cam) { return cam->unk220; }
float fn_80008328(Camera* cam) { return cam->unk21C; }
float fn_80008330(Camera* cam) { return cam->unk218; }
float fn_80008338(Camera* cam) { return cam->unk214; }
float fn_80008340(Camera* cam) { return cam->unk210; }
float fn_80008348(Camera* cam) { return cam->unk208; }
float fn_80008350(Camera* cam) { return cam->unk20C; }
float fn_80008358(Camera* cam) { return cam->unk204; }
float fn_80008360(Camera* cam) { return cam->unk1F4; }
float fn_80008368(Camera* cam) { return cam->unk1F8; }
CamLens* fn_80008370(Camera* cam) { return cam->unk10; }
int fn_80008378(CamLens* sub) { return sub->nType; }

void fn_80008380(void) {
    fn_800070DC();
}

void fn_800083A0(void) {
}

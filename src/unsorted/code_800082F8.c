// Accessors for the camera and for drawable objects. Original file name and field meanings unknown.

#include "unsorted/cull.h"

extern void fn_800070DC(void);

Sphere* fn_800082F8(RenderObj* obj) {
    return &obj->data->bounds;
}

void Vec3Copy(const Vec3* src, Vec3* dst) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
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
CameraSub* fn_80008370(Camera* cam) { return cam->unk10; }
int fn_80008378(CameraSub* sub) { return sub->unk0; }

void fn_80008380(void) {
    fn_800070DC();
}

void fn_800083A0(void) {
}

#ifndef UNSORTED_CULL_H
#define UNSORTED_CULL_H

// Types shared by the camera / visibility code around 0x80007BC4 - 0x800083A4.
// Original names unknown. Field names are unkOFFSET until their meaning is confirmed.
// The render camera's lens (CameraSub here before) is camera.h's CamLens.

#include "camera.h"

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float x, y, z, w;
} Vec4;

typedef struct {
    float x, y, z;
    float radius;
} Sphere;

typedef struct {
    /* 0x00 */ char pad0[0x58];
    /* 0x58 */ Sphere bounds;
} RenderObjData;

typedef struct {
    /* 0x00 */ RenderObjData* data;
} RenderObj;

typedef struct Camera {
    /* 0x000 */ float a0[4];         // 0, 0, 0.5, 0 at setup (GoRenderCtx_Gc.c fn_80013E48)
    /* 0x010 */ CamLens* unk10;
    /* 0x014 */ float* pRect;        // its screen rectangle (camera.h RenderCamera; fn_80013E28 sets it)
    /* 0x018 */ GoFrameBuf* pBuf;    // its frame buffer (GoRenderCtx_Gc.c fn_8001371C)
    /* 0x01C */ char pad1C[0x40];
    /* 0x05C */ float m5C[4][4];     // camera space to the screen (GoDynObj.c fn_8004787C divides by w)
    /* 0x09C */ float m9C[4][4];     // streammanagerhole.c fn_80016B9C copies it to lbl_801B8980.m74
    /* 0x0DC */ float mDC[4][4];     // world to the screen (GoCamCont.c fn_8006434C)
    /* 0x11C */ float viewMtx[4][4];
    /* 0x15C */ float m15C[4][4];    // GoShaderObject_Rain_Gc.c fn_800B4FA4 builds its position matrix from it
    /* 0x19C */ char pad19C[0x50];
    /* 0x1EC */ float f1EC;          // 1.0 at setup (GoRenderCtx_Gc.c fn_80013E48)
    /* 0x1F0 */ float f1F0;          // 16773216.0 at setup (fn_80013E48)
    /* 0x1F4 */ float unk1F4;
    /* 0x1F8 */ float unk1F8;
    /* 0x1FC */ char pad1FC[0x8];
    /* 0x204 */ float unk204;
    /* 0x208 */ float unk208;
    /* 0x20C */ float unk20C;
    /* 0x210 */ float unk210;
    /* 0x214 */ float unk214;
    /* 0x218 */ float unk218;
    /* 0x21C */ float unk21C;
    /* 0x220 */ float unk220;
} Camera;

Sphere* fn_800082F8(RenderObj* obj);
void fn_800BAD60(float mtx[4][4], Vec4* src, Vec4* dst);
int fn_80007D74(Sphere* sphere, Camera* cam, int mode);
float fn_80008320(Camera* cam);
float fn_80008328(Camera* cam);
float fn_80008330(Camera* cam);
float fn_80008338(Camera* cam);
float fn_80008340(Camera* cam);
float fn_80008348(Camera* cam);
float fn_80008350(Camera* cam);
float fn_80008358(Camera* cam);
float fn_80008360(Camera* cam);
float fn_80008368(Camera* cam);
CamLens* fn_80008370(Camera* cam);
int fn_80008378(CamLens* sub);

#endif

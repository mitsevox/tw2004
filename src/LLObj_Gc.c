// LLObj_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled.
// The code below is the visibility tests (0x80007BC4..: move an object's bounding sphere into
// camera space, then cull it), written earlier as unsorted/code_80007BC4.c and not yet cleaned up.

#include "unsorted/cull.h"
#include "dynobj.h"

int fn_80007BC4(RenderObj* obj, Camera* cam, float* outDepth, int mode, float scale);

// Culls a mesh by its bounding sphere scaled by fScale: 3 when it is out of view, 2 when it is
// wholly in view (mode 0, then mode 1), else 0. fDist and fHalfFovTan are not used.
int fn_80007B2C(UObjMesh* pMesh, void* pCamera, f32 fDist, f32 fHalfFovTan, f32 fScale) {
    int nClip;
    int nRet;

    nClip = fn_80007BC4((RenderObj*)pMesh, pCamera, NULL, 0, fScale);
    if (nClip == 2) {
        return 3;
    }
    if (nClip == 1) {
        return 2;
    }
    nClip = fn_80007BC4((RenderObj*)pMesh, pCamera, NULL, 1, fScale);
    nRet = 0;
    if (nClip == 1) {
        nRet = 2;
    }
    return nRet;
}

// ---- sweep code (not yet cleaned up) ----

int fn_80007BC4(RenderObj* obj, Camera* cam, float* outDepth, int mode, float scale) {
    Sphere sphere;
    Vec4 pos;
    Sphere* bounds = fn_800082F8(obj);

    Vec3Copy((f32*)bounds, (f32*)&pos);
    pos.w = 1.0f;
    fn_800BAD60(cam->viewMtx, &pos, &pos);
    Vec3Copy((f32*)&pos, (f32*)&sphere);
    if (outDepth != 0) {
        *outDepth = sphere.z;
    }
    sphere.radius = bounds->radius * scale;
    return fn_80007D74(&sphere, cam, mode);
}

void fn_80007C80(Camera* cam, const Vec3* src, Vec3* dst) {
    Vec4 pos;

    Vec3Copy((f32*)src, (f32*)&pos);
    pos.w = 1.0f;
    fn_800BAD60(cam->viewMtx, &pos, &pos);
    Vec3Copy((f32*)&pos, (f32*)dst);
}

int fn_80007CE8(RenderObj* obj, Camera* cam, int mode, float scale) {
    Sphere sphere;

    fn_80007C80(cam, (Vec3*)fn_800082F8(obj), (Vec3*)&sphere);
    sphere.radius = scale * fn_800082F8(obj)->radius;
    return fn_80007D74(&sphere, cam, mode);
}

// Sphere vs view frustum test. The sphere is already in camera space.
// Returns 1 = fully visible, 2 = not visible, 4 = touching a side edge, 8 = touching the near limit.
int fn_80007D74(Sphere* s, Camera* cam, int mode) {
    float a, b, c, d;
    float px, pz, dist;
    int result;

    if (fn_80008378(fn_80008370(cam)) == 0) {
        if (s->z + s->radius > fn_80008368(cam)) return 2;
        if (s->z + s->radius < fn_80008360(cam)) return 2;

        result = 1;
        switch (mode) {
        case 0:
            a = fn_80008358(cam); b = fn_80008350(cam);
            c = fn_80008348(cam); d = fn_80008340(cam);
            break;
        case 1:
            a = fn_80008338(cam); b = fn_80008330(cam);
            c = fn_80008328(cam); d = fn_80008320(cam);
            break;
        }

        px = s->x * b;
        pz = s->z * -a;
        dist = px + pz;
        if (dist >= s->radius) { result = 2; goto done; }
        if (-dist <= s->radius) result = 4;
        dist = -px + pz;
        if (dist >= s->radius) { result = 2; goto done; }
        if (-dist <= s->radius) result = 4;

        px = s->y * d;
        pz = s->z * -c;
        dist = px + pz;
        if (dist >= s->radius) { result = 2; goto done; }
        if (-dist <= s->radius) result = 4;
        dist = -px + pz;
        if (dist >= s->radius) { result = 2; goto done; }
        if (-dist <= s->radius) result = 4;

    done:
        if (result != 2) {
            if (s->z - s->radius <= fn_80008360(cam)) result = 8;
        }
        return result;
    } else {
        float top    =  fn_80008370(cam)->fB8 / 2.0f;
        float bottom = -fn_80008370(cam)->fB8 / 2.0f;
        float right  =  fn_80008370(cam)->fB4 / 2.0f;
        float left   = -fn_80008370(cam)->fB4 / 2.0f;

        if (s->z + s->radius > fn_80008368(cam)) return 2;
        if (s->z + s->radius < fn_80008360(cam)) return 2;
        if (s->x - s->radius > right || s->x + s->radius < left ||
            s->y - s->radius > top || s->y + s->radius < bottom) {
            return 2;
        }
        return 4;
    }
}

// ---- end of sweep code ----

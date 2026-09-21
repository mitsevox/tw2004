// Visibility tests: move an object's bounding sphere into camera space, then cull it.
// Original file name unknown.

#include "unsorted/cull.h"

int fn_80007BC4(RenderObj* obj, Camera* cam, float* outDepth, int mode, float scale) {
    Sphere sphere;
    Vec4 pos;
    Sphere* bounds = fn_800082F8(obj);

    Vec3Copy((Vec3*)bounds, (Vec3*)&pos);
    pos.w = 1.0f;
    fn_800BAD60(cam->viewMtx, &pos, &pos);
    Vec3Copy((Vec3*)&pos, (Vec3*)&sphere);
    if (outDepth != 0) {
        *outDepth = sphere.z;
    }
    sphere.radius = bounds->radius * scale;
    return fn_80007D74(&sphere, cam, mode);
}

void fn_80007C80(Camera* cam, const Vec3* src, Vec3* dst) {
    Vec4 pos;

    Vec3Copy(src, (Vec3*)&pos);
    pos.w = 1.0f;
    fn_800BAD60(cam->viewMtx, &pos, &pos);
    Vec3Copy((Vec3*)&pos, dst);
}

int fn_80007CE8(RenderObj* obj, Camera* cam, int mode, float scale) {
    Sphere sphere;

    fn_80007C80(cam, (Vec3*)fn_800082F8(obj), (Vec3*)&sphere);
    sphere.radius = scale * fn_800082F8(obj)->radius;
    return fn_80007D74(&sphere, cam, mode);
}

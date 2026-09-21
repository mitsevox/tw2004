// Accessors for a large game object. Original file name and field meanings unknown.

typedef struct {
    float x, y, z;
} Vec3;

typedef struct UnkObj {
    /* 0x000 */ void* unk0;
    /* 0x004 */ char pad4[0xC];
    /* 0x010 */ int unk10;
    /* 0x014 */ char pad14[0x1E0];
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
} UnkObj;

extern void fn_800070DC(void);

void* fn_800082F8(UnkObj* obj) {
    return (char*)obj->unk0 + 0x58;
}

void Vec3Copy(const Vec3* src, Vec3* dst) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

float fn_80008320(UnkObj* obj) { return obj->unk220; }
float fn_80008328(UnkObj* obj) { return obj->unk21C; }
float fn_80008330(UnkObj* obj) { return obj->unk218; }
float fn_80008338(UnkObj* obj) { return obj->unk214; }
float fn_80008340(UnkObj* obj) { return obj->unk210; }
float fn_80008348(UnkObj* obj) { return obj->unk208; }
float fn_80008350(UnkObj* obj) { return obj->unk20C; }
float fn_80008358(UnkObj* obj) { return obj->unk204; }
float fn_80008360(UnkObj* obj) { return obj->unk1F4; }
float fn_80008368(UnkObj* obj) { return obj->unk1F8; }
int fn_80008370(UnkObj* obj) { return obj->unk10; }
void* fn_80008378(UnkObj* obj) { return obj->unk0; }

void fn_80008380(void) {
    fn_800070DC();
}

void fn_800083A0(void) {
}

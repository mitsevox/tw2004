// Vertex format setup and per-type function-table dispatch. Original file name unknown.
// Sits between the visibility code (code_80007BC4.c) and the accessors (code_800082F8.c).

typedef struct UnkObj UnkObj;

typedef struct {
    /* 0x0 */ void (*unk0)(UnkObj* obj, int arg);
    /* 0x4 */ void (*unk4)(UnkObj* obj);
    /* 0x8 */ void (*unk8)(UnkObj* obj);
} UnkFuncs;

typedef struct {
    /* 0x0 */ void (*unk0)(UnkObj* obj, int arg);
    /* 0x4 */ void (*unk4)(UnkObj* obj);
} UnkModeFuncs;

// May be the same struct as CameraSub in cull.h: both have a type index at 0x0.
struct UnkObj {
    /* 0x00 */ int type;
    /* 0x04 */ char pad4[0x20];
    /* 0x24 */ int unk24;
    /* 0x28 */ UnkFuncs* typeFuncs;
    /* 0x2C */ char pad2C[0x4];
    /* 0x30 */ int mode;
    /* 0x34 */ UnkModeFuncs* modeFuncs;
};

typedef struct {
    /* 0x00 */ char pad0[0x8];
    /* 0x08 */ UnkModeFuncs funcs;
} UnkModeEntry;

typedef struct {
    /* 0x00 */ char pad0[0x24];
    /* 0x24 */ UnkFuncs funcs;
    /* 0x30 */ char pad30[0x14];
} UnkTypeEntry;

typedef struct {
    /* 0x00 */ char pad0[0x10];
    /* 0x10 */ int unk10;
    /* 0x14 */ int unk14;
} UnkGlobal;

extern UnkGlobal* lbl_80280DA0;
extern char lbl_80188E78[];
extern UnkTypeEntry lbl_80188E88[];

void GXSetVtxAttrFmt(int vtxfmt, int attr, int cnt, int type, unsigned char frac);

void fn_800080D0(void) {
}

void fn_800080D4(void) {
    lbl_80280DA0->unk14 = 0;
    lbl_80280DA0->unk10 = 0;
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 11, 1, 5, 0);
    GXSetVtxAttrFmt(0, 13, 1, 3, 9);
    GXSetVtxAttrFmt(0, 14, 1, 3, 9);
    GXSetVtxAttrFmt(0, 10, 0, 4, 0);
    GXSetVtxAttrFmt(4, 9, 1, 4, 0);
    GXSetVtxAttrFmt(4, 11, 1, 5, 0);
    GXSetVtxAttrFmt(4, 13, 1, 4, 0);
}

void fn_800081C4(void) {
}

void fn_800081C8(UnkObj* obj, int mode, int arg) {
    obj->mode = mode;
    obj->modeFuncs = &((UnkModeEntry*)lbl_80188E78)[mode].funcs;
    obj->modeFuncs->unk0(obj, arg);
}

void fn_80008214(UnkObj* obj) {
    if (obj->modeFuncs->unk4 != 0) {
        obj->modeFuncs->unk4(obj);
    }
}

void fn_80008248(UnkObj* obj) {
    if (obj->typeFuncs->unk4 != 0) {
        obj->typeFuncs->unk4(obj);
    }
}

void fn_8000827C(UnkObj* obj, int unk24, int type, int arg) {
    obj->type = type;
    obj->unk24 = unk24;
    obj->typeFuncs = &lbl_80188E88[type].funcs;
    obj->typeFuncs->unk0(obj, arg);
}

void fn_800082CC(UnkObj* obj) {
    obj->typeFuncs->unk8(obj);
}

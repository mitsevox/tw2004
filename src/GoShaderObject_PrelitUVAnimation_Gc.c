// GoShaderObject_PrelitUVAnimation_Gc.c (EA's name, from its asserts): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"
#include "engine.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80074DA8();
void fn_80071148(u8* p0, s32 p1);
void fn_8007524C();
void fn_80076E24();
void fn_8007117C(u8* p0);
s32 GXSetArray(s32, s32, s32);
s32 fn_80070168(s32, s32);
void fn_800711A4(void* arg0);
void fn_800712B4(u8* p0);

void fn_80071148(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_8007117C(u8* p0) {
    fn_8007524C((p0 + 0x4));
    fn_80076E24();
}

void fn_800711A4(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(s32**)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}

void fn_800712B4(u8* p0) {
    fn_80009E70(*(void**)(p0 + 0x8));
    fn_80070348(*(DynRenderBuffer**)(p0 + 0x4));
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_80071574(void);
void fn_80071578(u8* p0, s32 p1);
void fn_800715AC(u8* p0);
void fn_800715D0(void* arg0);
void fn_80071624(void);
void fn_80071628(u8* p0, s32 p1);
void fn_8007165C(u8* p0);
void fn_80071680(void* arg0);
void fn_800717AC();
void fn_800717E8();
void fn_80076B7C();
void fn_80076BC4();
void fn_80097208();
void fn_80097250();
void fn_8009F780();
void fn_8009F7E4();
void fn_800716D4(void);
void fn_800716FC(void);
void fn_80071724(void);
void fn_80071748(void);
void fn_8007176C(void);
void fn_8007178C(void);
void fn_80071828(void);
extern s32 lbl_802811F8;
void fn_80071850(void);
s32 fn_80071854(void);
void fn_8006FCD4();
void fn_8006FCD8();
void fn_8007185C(void);
void fn_80071890(void);
void fn_800718C4(void);
extern s32 lbl_80281B88;
void fn_800718E4(void);
void fn_8007190C(void);
void fn_80071910(void);
void fn_80071914();
void fn_80071994();
void fn_80071A14(void);
void fn_80071A34(void);

void fn_80071574(void) {
}

void fn_80071578(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_800715AC(u8* p0) {
    fn_8007524C((p0 + 0x4));
}

void fn_800715D0(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(s32**)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}

void fn_80071624(void) {
}

void fn_80071628(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_8007165C(u8* p0) {
    fn_8007524C((p0 + 0x4));
}

void fn_80071680(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(s32**)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}

void fn_800716D4(void) {
    fn_8009F780();
    fn_80076B7C();
    fn_80097208();
}

void fn_800716FC(void) {
    fn_80097250();
    fn_80076BC4();
    fn_8009F7E4();
}

void fn_80071724(void) {
    fn_800717AC(0);
}

void fn_80071748(void) {
    fn_800717E8(0);
}

void fn_8007176C(void) {
    fn_80071724();
}

void fn_8007178C(void) {
    fn_80071748();
}

void fn_80071828(void) {
    s32 t0;
    t0 = fn_80071854();
    *(s32*)((u8*)t0) = 0;
}

void fn_80071850(void) {
}

s32 fn_80071854(void) {
    return lbl_802811F8;
}

void fn_8007185C(void) {
    fn_8006FCD4();
    fn_800716D4();
    fn_8007190C();
    fn_8007176C();
    fn_80071A14();
    fn_80071828();
}

void fn_80071890(void) {
    fn_80071850();
    fn_80071A34();
    fn_8007178C();
    fn_80071910();
    fn_800716FC();
    fn_8006FCD8();
}

void fn_800718C4(void) {
    fn_800718E4();
}

void fn_800718E4(void) {
    s32 t0;
    t0 = fn_80071854();
    *(s32*)((u8*)t0) = lbl_80281B88;
}

void fn_8007190C(void) {
}

void fn_80071910(void) {
}

void fn_80071A14(void) {
    fn_80071914();
}

void fn_80071A34(void) {
    fn_80071994();
}

// ---- end of sweep code ----

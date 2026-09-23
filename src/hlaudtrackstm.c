// hlaudtrackstm.c (our name, after TW06's golf/audio/engine/hl/hlaudtrackstm.c; the Stm_ names are
// EA's own, from the name strings its functions pass to the audio locks): the streamed tracks of the
// sound engine (music and long sounds read from disc) and the queue of their disc reads.

#include "game_types.h"

extern u8 lbl_8018EB90[];
extern u8 lbl_801F18B8[];
void fn_800AE084();
void fn_800B59BC();
void fn_800B59EC();
void RemoveFromAudStreamQueue(void);
void fn_80005AE8();
void fn_800AE00C();
s32 fn_800ABBC8(void);
void fn_800ABC34();
void fn_800ABC54();
void fn_800ABD7C();
void fn_800AC310();
extern s32 lbl_80281468;
s32 fn_800AC328(void);

void RemoveFromAudStreamQueue(void) {
    fn_800B59BC(lbl_8018EB90);
    fn_800AE084(lbl_801F18B8);
    *(u8*)(lbl_801F18B8 + 0x18) = 0;
    fn_800B59EC(lbl_8018EB90, lbl_801F18B8);
}

s32 fn_800ABBC8(void) {
    fn_80005AE8((lbl_801F18B8 + 0x1C), 0, 224);
    fn_800AE00C(lbl_801F18B8, (lbl_801F18B8 + 0x1C), 8, 28);
    *(u8*)(lbl_801F18B8 + 0x18) = 0;
    return 1;
}

void fn_800ABC34(void) {
    fn_800ABC54();
}

void fn_800ABC54(u8* p0) {
    *(s32*)(p0 + 0x64) = 0;
    *(s32*)(p0 + 0x68) = 0;
    *(s32*)(p0 + 0x6C) = 0;
    *(s32*)(p0 + 0x70) = 0;
    *(s32*)(p0 + 0x74) = 0;
    *(s32*)(p0 + 0x78) = 0;
    *(s32*)(p0 + 0x7C) = 0;
    *(s32*)(p0 + 0x80) = 0;
    *(u16*)(p0 + 0x84) = (65536 - 1);
    *(u16*)(p0 + 0x86) = (65536 - 1);
    *(u8*)(p0 + 0x88) = 255;
    *(u8*)(p0 + 0x8A) = 0;
    *(u8*)(p0 + 0x89) = 0;
}

void fn_800ABD7C(u8* p0) {
    fn_800AC310();
    *(u8*)(p0 + 0x8A) = 0;
}

void fn_800AC310(u8* p0) {
    *(u8*)(p0 + 0x88) = 255;
    *(u16*)(p0 + 0x86) = (65536 - 1);
}

s32 fn_800AC328(void) {
    return lbl_80281468;
}

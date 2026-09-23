// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 memcpy();
s32 memset();
extern u8* lbl_802825B8;

s32 fn_8012D0D4(void);
s32 fn_8012D0D4(void) {
    (*(s8*)((u8*)(lbl_802825B8) + 0x11CF)) = 0;
    if ((u8* ) (*(u8**)((u8*)(lbl_802825B8) + 0x11D4)) != NULL) {
        memset((u8* ) (*(u8**)((u8*)(lbl_802825B8) + 0x11D4)), 0, 0x1B328);
        memcpy((*(u8**)((u8*)(lbl_802825B8) + 0x11D4)), lbl_802825B8 + 0x68, 0x1168);
        (*(s8*)((u8*)(lbl_802825B8) + 0x11D0)) = 0;
    }
    if ((u8* ) (*(u8**)((u8*)(lbl_802825B8) + 0x11D8)) != NULL) {
        memset((u8* ) (*(u8**)((u8*)(lbl_802825B8) + 0x11D8)), 0, 0);
        memcpy((*(u8**)((u8*)(lbl_802825B8) + 0x11D8)), (*(s32*)((u8*)(lbl_802825B8) + 0x11E4)) + 0x36, 0x400);
        memcpy((*(u8**)((u8*)(lbl_802825B8) + 0x11D8)) + 0x400, (*(s32*)((u8*)(lbl_802825B8) + 0x11E4)) + 0x436, 0x4000);
        (*(s8*)((u8*)((*(u8**)((u8*)(lbl_802825B8) + 0x11D8))) + 0x4400)) = 1;
    }
    return 0;
}
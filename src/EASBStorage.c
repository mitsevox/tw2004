// EASBStorage.c (our name): made by fold.py from 17 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80127F88();
s32 fn_80128CA0();
s32 fn_80127D84(void* arg0, void* arg1);
s32 fn_80128054();
s32 fn_80128DD0();
s32 fn_80127E44(void* arg0, void* arg1, s32 arg2);
u32 fn_80128468(u32 arg0, u32 arg1);
void fn_80128528(void* arg0, void* arg1);
u32 fn_80128BC4(u32 arg0);
void fn_80128F58(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
s32 TagFile_Delete(s32*, s32, s32);
s32 TagFile_GetSession(u8*);
s32 fn_8012C98C();
s32 fn_8012CB98();
extern s32 lbl_802819B0;
extern u8* lbl_802825B0;
s32 fn_8012A2A8(s32* arg0);
s32 TagFile_DeleteSession(u8*);
s32 fn_8012A364(s32* arg0);
s32 TagFile_End(u8*);
s32 fn_8012A434(s32* arg0);
s32 fn_8012A900(s32* arg0);
s32 TagFile_Write(u8*, s32, u8, s32*, s32);
s32 fn_8012B004(s32* arg0);
s32 fn_80129D70(s32, s32, s32*, s32);
s32 fn_8012B0D8(s32* arg0);
u8 fn_8012C83C(void);
u8 fn_8012C848(void);
s32 fn_8012C854(u8* arg0);
s32 fn_8012C888(s32* arg0);
s32 TagFile_FreeBuffer();
s32 TagFile_Shutdown();
void fn_8012CC48(void);
s32 fn_8012CCC0(void);
void fn_8012CCCC(s32 v);

s32 fn_80127D84(void* arg0, void* arg1) {
    s32 var_r3;
    u32 temp_r0;
    u32 temp_r3;
    u8 var_r31;

    var_r31 = 1;
    if (fn_80127F88() != 0) {
        var_r31 = 0;
    } else if (fn_80127F88(arg1) != 0) {
        var_r31 = 1;
    } else {
        temp_r0 = (*(u32*)((u8*)(arg0) + 0x4C));
        temp_r3 = (*(u32*)((u8*)(arg1) + 0x4C));
        if (temp_r0 < temp_r3) {
            var_r31 = 0;
        } else if (temp_r3 < temp_r0) {
            var_r31 = 1;
        } else if (fn_80128CA0(arg0, arg1, 0) > 0) {
            var_r31 = 0;
        }
    }
    var_r3 = 1;
    if (var_r31 != 0) {
        var_r3 = -1;
    }
    return var_r3;
}

s32 fn_80127E44(void* arg0, void* arg1, s32 arg2) {
    s32 var_r3;
    u32 temp_r0_2;
    u32 temp_r3_2;
    u8 temp_r0;
    u8 temp_r3;
    u8 var_r0;

    if (fn_80128054() != 0) {
        var_r0 = 0;
    } else if (fn_80128054(arg1) != 0) {
        var_r0 = 1;
    } else {
        temp_r0 = (*(u8*)((u8*)(arg0) + 0x86));
        temp_r3 = (*(u8*)((u8*)(arg1) + 0x86));
        if ((temp_r0 > temp_r3) && (arg2 == 0)) {
            var_r0 = 1;
        } else if ((temp_r3 > temp_r0) && (arg2 == 0)) {
            var_r0 = 0;
        } else {
            temp_r0_2 = (*(u32*)((u8*)(arg0) + 0x80));
            temp_r3_2 = (*(u32*)((u8*)(arg1) + 0x80));
            if (temp_r0_2 > temp_r3_2) {
                var_r0 = 1;
            } else if (temp_r3_2 > temp_r0_2) {
                var_r0 = 0;
            } else if (fn_80128DD0(arg0, arg1) > 0) {
                var_r0 = 1;
            } else {
                var_r0 = 0;
            }
        }
    }
    var_r3 = 1;
    if (var_r0 != 0) {
        var_r3 = -1;
    }
    return var_r3;
}

u32 fn_80128468(u32 arg0, u32 arg1) {
    u32 var_r0;

    var_r0 = arg0 + arg1;
    if ((var_r0 < arg0) || (var_r0 < arg1)) {
        var_r0 = -1U;
    }
    return var_r0;
}

void fn_80128528(void* arg0, void* arg1) {
    if ((u8) (*(u8*)((u8*)(arg1) + 0x1166)) != 0) {
        (*(s32*)((u8*)(arg0) + 0)) = (s32) (*(s32*)((u8*)(arg1) + 0x50));
        (*(s32*)((u8*)(arg0) + 4)) = (s32) (*(s32*)((u8*)(arg1) + 0x54));
        (*(s32*)((u8*)(arg0) + 8)) = (s32) (*(s32*)((u8*)(arg1) + 0x58));
        (*(s32*)((u8*)(arg0) + 0xC)) = (s32) (*(s32*)((u8*)(arg1) + 0x5C));
        (*(s8*)((u8*)(arg0) + 0x10)) = 1;
        (*(s16*)((u8*)(arg1) + 0x1160)) = 1;
        return;
    }
    (*(s32*)((u8*)(arg0) + 0)) = 0;
    (*(s32*)((u8*)(arg0) + 4)) = 0;
    (*(s32*)((u8*)(arg0) + 8)) = 0;
    (*(s32*)((u8*)(arg0) + 0xC)) = 0;
    (*(s8*)((u8*)(arg0) + 0x10)) = 0;
}

u32 fn_80128BC4(u32 arg0) {
    u32 var_r0;

    if (arg0 < 0x3E122F80U) {
        return 0x3E122F80U;
    }
    var_r0 = 0x63B0CD00U;
    if (arg0 <= 0x63B0CD00U) {
        var_r0 = arg0;
    }
    return var_r0;
}

void fn_80128F58(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    u32 temp_r5;
    u32 var_r7;
    u32 var_r8;

    var_r7 = 0;
    var_r8 = 0;
    if (arg3 != 0) {
        temp_r5 = arg0 / arg2;
        if ((u32) (-1U / arg3) >= temp_r5) {
            var_r8 = temp_r5 * arg3;
        } else {
            var_r8 = -1U;
        }
    }
    if (arg1 != 0) {
        if ((u32) (-1U / arg1) >= arg0) {
            var_r7 = arg0 * arg1;
        } else {
            var_r7 = -1U;
        }
    }
    fn_80128468(var_r8, var_r7);
}

s32 fn_8012A2A8(s32* arg0) {
    s32 temp_r0;
    s32 temp_r3;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        TagFile_Delete(&lbl_802819B0, -1, 0);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
        if ((var_r4 == 0) && ((s32) *arg0 == 2)) {
            TagFile_GetSession(lbl_802825B0 + 0x120);
            temp_r3 = fn_8012C98C();
            (*(s8*)((u8*)(lbl_802825B0) + 0x91)) = 1;
            var_r4 = temp_r3;
            (*(s8*)((u8*)(lbl_802825B0) + 0x92)) = 1;
            (*(s8*)((u8*)(lbl_802825B0) + 0x90)) = 1;
        }
    }
    return var_r4;
}

s32 fn_8012A364(s32* arg0) {
    s32 temp_r0;
    s32 var_r5;

    var_r5 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x92)) == 0) {
        return 0x11;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x90)) == 0) {
        return 0x11;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        TagFile_DeleteSession(lbl_802825B0 + 0x120);
        var_r5 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r5 = fn_8012CB98(arg0);
        if ((var_r5 == 0) && ((s32) *arg0 == 2)) {
            TagFile_GetSession(lbl_802825B0 + 0x120);
            var_r5 = fn_8012C98C();
            (*(s8*)((u8*)(lbl_802825B0) + 0x91)) = 1;
        }
    }
    return var_r5;
}

s32 fn_8012A434(s32* arg0) {
    s32 temp_r0;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        TagFile_End(lbl_802825B0 + 0x120);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
    }
    if ((s32) *arg0 == 2) {
        (*(s8*)((u8*)(lbl_802825B0) + 0x91)) = 0;
    }
    return var_r4;
}

s32 fn_8012A900(s32* arg0) {
    if (arg0 == NULL) {
        return 3;
    }
    if ((s32) *arg0 == 0) {
        fn_80128528((*(void**)((u8*)(lbl_802825B0) + 0x184)), (*(void**)((u8*)(lbl_802825B0) + 0x188)));
        *arg0 = 2;
    }
    return 0;
}

s32 fn_8012B004(s32* arg0) {
    s32 temp_r0;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x91)) == 0) {
        return 0x21;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        fn_80129754((*(u8**)((u8*)(lbl_802825B0) + 0x184)), (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), (*(s32*)((u8*)(lbl_802825B0) + 0xA0)));
        TagFile_Write(lbl_802825B0 + 0x120, 0x48454144, 0U, (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), 0x11);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
    }
    if (((s32) *arg0 == 2) && (var_r4 == 0)) {
        (*(s32*)((u8*)(lbl_802825B0) + 0xA4)) = 1;
    }
    return var_r4;
}

s32 fn_8012B0D8(s32* arg0) {
    s32 temp_r0;
    s32 var_r4;

    var_r4 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_802825B0) + 0x91)) == 0) {
        return 0x21;
    }
    temp_r0 = *arg0;
    if (temp_r0 == 0) {
        fn_80129D70((*(s32*)((u8*)(lbl_802825B0) + 0x190)), 1, (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), (*(s32*)((u8*)(lbl_802825B0) + 0xA0)));
        TagFile_Write(lbl_802825B0 + 0x120, 0x494D4147, (*(u8*)((u8*)(lbl_802825B0) + 0x98)), (*(s32**)((u8*)(lbl_802825B0) + 0x9C)), 0x4301);
        var_r4 = fn_8012C98C();
        *arg0 = 1;
    } else if (temp_r0 == 1) {
        var_r4 = fn_8012CB98(arg0);
    }
    return var_r4;
}

u8 fn_8012C83C(void) {
    return *(u8*)(lbl_802825B0 + 0x91);
}

u8 fn_8012C848(void) {
    return *(u8*)(lbl_802825B0 + 0x92);
}

s32 fn_8012C854(u8* arg0) {
    if (arg0 == NULL) {
        return 3;
    }
    if ((void* ) lbl_802825B0 == NULL) {
        return 0x10;
    }
    *arg0 = (*(u8*)((u8*)(lbl_802825B0) + 0x98));
    return 0;
}

s32 fn_8012C888(s32* arg0) {
    s32 var_r5;

    var_r5 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((void* ) lbl_802825B0 == NULL) {
        return 0x10;
    }
    *arg0 = (*(s32*)((u8*)(lbl_802825B0) + 0x94));
    if ((s32) *arg0 == 0) {
        var_r5 = 1;
    }
    return var_r5;
}

void fn_8012CC48(void) {
    s32 var_r31;
    s32 var_r3;

    var_r31 = 0;
    if ((u32) (*(u32*)((u8*)(lbl_802825B0) + 0x9C)) != 0U) {
        var_r31 = TagFile_FreeBuffer((*(u32*)((u8*)(lbl_802825B0) + 0x9C)), (*(s32*)((u8*)(lbl_802825B0) + 0x8C)), (*(s32*)((u8*)(lbl_802825B0) + 0xA0)), 0);
        var_r3 = fn_8012C98C();
        (*(u32*)((u8*)(lbl_802825B0) + 0x9C)) = 0U;
    } else {
        var_r3 = 0x22;
    }
    if (var_r3 == 0) {
        TagFile_Shutdown();
        fn_8012C98C(var_r31);
    }
}

s32 fn_8012CCC0(void) {
    return *(s32*)(lbl_802825B0 + 0x17C);
}

void fn_8012CCCC(s32 v) {
    *(s32*)(lbl_802825B0 + 0x120) = v;
}

// ---- end of sweep code ----

// frontend.h (our name): the front end (the HUD and menu screens) as the game code sees it.
// GameMessages.c sends its messages to the front end's handler; startUp.c uses it too.

#ifndef GAME_FRONTEND_H
#define GAME_FRONTEND_H

#include "game_types.h"

typedef struct FrontEnd {
    u8    unk0[4];
    void* pHandler;             // 0x4  where GameMessages.c sends its messages (fn_8016B09C)
    u8    unk8[4];
    void* pC;                   // 0xC  a block uiLoadFile.c frees (fn_8008F24C)
} FrontEnd;

extern FrontEnd* lbl_80281F1C;

// One value of a message: an int or a float (the mask passed with it says which), or a pointer.
typedef union MsgArg {
    s32   i;
    f32   f;
    void* p;
} MsgArg;

// Send message nMsg with nArgs values to a front-end handler (fn_8016B09C also sends through it).
void fn_8016B0F8(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);

#endif

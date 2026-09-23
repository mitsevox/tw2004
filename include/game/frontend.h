// frontend.h (our name): the front end (the HUD and menu screens) as the game code sees it.
// GameMessages.c sends its messages to the front end's handler; startUp.c uses it too.

#ifndef GAME_FRONTEND_H
#define GAME_FRONTEND_H

#include "game_types.h"

typedef struct FrontEnd {
    u8    unk0[4];
    void* pHandler;             // 0x4  where GameMessages.c sends its messages (fn_8016B09C)
} FrontEnd;

extern FrontEnd* lbl_80281F1C;

#endif

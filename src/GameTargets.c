// GameTargets.c (our name): the code the target games (modes 13..17) share: the target list sorted
// and aimed at, each player's targets hit and shot multiplier, the prize, the per-tee distance
// check, and dispatchers into each mode's own file. Split from GameModeReplay.c (mode 10) because
// the two halves each have their own copy of the int-to-float constant; the bytes cannot prove the
// exact split point, which is somewhere from fn_800F19D4 to fn_800F266C (the functions between use
// no float constants). fn_800F19D4 is the first function the target modes call.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/earnings.h"

// Score multipliers for fn_800F266C, one table per value of fn_80015464.
f32 lbl_80192810[13] = {
    1.0f, 1.0f, 1.3f, 1.1f, 1.0f, 1.1f, 1.2f, 1.3f, 1.3f, 1.4f, 1.4f, 1.3f, 1.0f,
};
f32 lbl_80192844[15] = {
    1.0f, 1.0f, 1.2f, 1.1f, 1.3f, 1.1f, 1.4f, 1.2f, 1.1f, 1.0f, 1.0f, 1.0f, 1.2f, 1.4f, 1.3f,
};
f32 lbl_80192880[15] = {
    1.1f, 1.1f, 1.0f, 1.0f, 1.0f, 1.1f, 1.2f, 1.3f, 1.0f, 1.3f, 1.4f, 1.4f, 1.3f, 1.0f, 1.0f,
};

Character* fn_8001D324(int nId);   // the character with this id (nPlayer 1000), or NULL
void  fn_800A631C(void);

u8   fn_800F2358(int nPlayer);

// Sort the targets by distance from the tee, nearest first.
void fn_800F19D4(void) {
    f32 tmp[4];
    int i;
    int j;
    f32* pTee = &fn_8000C594()->tee[gSession.nTeeSet[0]].x;
    for (i = 0; i < lbl_80282360 - 1; i++) {
        for (j = i + 1; j < lbl_80282360; j++) {
            if (Vec_Distance(lbl_80211D38[i], pTee) > Vec_Distance(lbl_80211D38[j], pTee)) {
                Vec_Copy(lbl_80211D38[i], tmp);
                Vec_Copy(lbl_80211D38[j], lbl_80211D38[i]);
                Vec_Copy(tmp, lbl_80211D38[j]);
            }
        }
    }
}

// Aim the player at target n (wrapping round): the pin moves there.
void fn_800F1ABC(int nPlayer, s8 n) {
    Character* pChar;
    gPlayers[nPlayer].nTarget = n % lbl_80282360;
    Vec_Copy(lbl_80211D38[gPlayers[nPlayer].nTarget], (f32*)fn_8000C594()->pin);
    pChar = fn_8001D324(100);
    if (pChar != NULL) {
        Character_SetPosition(pChar, lbl_80211D38[gPlayers[nPlayer].nTarget], 1);
    }
}

void fn_800F1B60(int nPlayer, s8 n) {
    fn_800F1ABC(nPlayer, n);
    AI_DefaultTarget(nPlayer);
    Shot_Prepare(nPlayer, 1);
    fn_8001C804(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    fn_80095744(gPlayers[nPlayer].pChar, 5);
    fn_80062C38();
}

// Previous target.
u8 fn_800F1BD8(int nPlayer) {
    if (gPlayers[nPlayer].nTarget == 0) {
        fn_800F1ABC(nPlayer, lbl_80282360 - 1);
    } else {
        fn_800F1ABC(nPlayer, gPlayers[nPlayer].nTarget - 1);
    }
    return 1;
}

// Next target.
u8 fn_800F1C34(int nPlayer) {
    fn_800F1ABC(nPlayer, gPlayers[nPlayer].nTarget + 1);
    return 1;
}

// The target nearest the ball.
s8 fn_800F1C74(int nPlayer) {
    f32* pBall = gPlayers[nPlayer].ball.vPos;
    s8 i;
    s8 nBest = 0;
    f32 fBest = Vec_Distance(lbl_80211D38[0], pBall);
    for (i = 1; i < lbl_80282360; i++) {
        f32 f = Vec_Distance(lbl_80211D38[i], pBall);
        if (f < fBest) {
            fBest = f;
            nBest = i;
        }
    }
    return nBest;
}

// The target nearest the player's aim point.
int fn_800F1D34(int nPlayer) {
    f32* pTarget = gPlayers[nPlayer].vTarget;
    int i;
    int nBest = 0;
    f32 fBest = Vec_Distance(lbl_80211D38[0], pTarget);
    for (i = 1; i < lbl_80282360; i++) {
        f32 f = Vec_Distance(lbl_80211D38[i], pTarget);
        if (f < fBest) {
            fBest = f;
            nBest = i;
        }
    }
    return nBest;
}

void fn_800F1DF0(void) {
    if (Game_GetMode() == 0xD) {
        fn_800F7DE8();
    }
}

void fn_800F1E1C(void) {
    if (Game_GetMode() == 0xE) {
        fn_800F3828();
    }
    if (Game_GetMode() == 0xF) {
        fn_800F48C4();
    }
}

s32 fn_800F1E58(s32 n) {
    switch (n) {
    case 0x85: return 0;
    case 0x86: return 1;
    case 0x87: return 2;
    case 0x88: return 0;
    case 0x89: return 1;
    case 0x8A: return 2;
    case 0x8B: return 3;
    case 0x8C: return 0;
    case 0x8D: return 1;
    case 0x8E: return 2;
    case 0x8F: return 3;
    case 0x90: return 4;
    default: return 5;
    }
}

// Every player's target-game state is cleared and aimed at the first target.
void fn_800F1EE4(void) {
    int i;
    int j;
    for (i = 0; i < 5; i++) {
        Player* p = PLAYER(i);
        p->nDC0 = 0;
        for (j = 0; j < 5; j++) {
            p->aDC4[j] = 0;
        }
        p->nDD8 = 0;
        p->nDDC = 0;
        p->nDE0 = 0;
        p->nE88 = 0;
        p->nE8C = 0;
        p->nE90 = 0;
        p->nE94 = 0;
        p->nE98 = 0;
        p->bE9D = 0;
        p->bE9E = 0;
        for (j = 0; j < 40; j++) {
            p->nDE4[j] = 0;
        }
        fn_800F1ABC(i, 0);
    }
    fn_800F2030();
}

void fn_800F2030(void) {
    int i;
    int j;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        PLAYER(i)->nCD0 = 0;
        PLAYER(i)->nDBC = 1;
        PLAYER(i)->nDB8 = 0;
        for (j = 0; j < 20; j++) {
            PLAYER(i)->aCD4[j] = 0;
        }
    }
}

// How many targets the player has hit.
s32 fn_800F20C0(int nPlayer) {
    int i;
    s32 n = 0;
    for (i = 0; i < 40; i++) {
        if (gPlayers[nPlayer].nDE4[i] != 0) {
            n++;
        }
    }
    return n;
}

// After a shot, maybe a multiplier for the next one: x5, x3 or x2, more often after 10 shots
// without one.
void fn_800F21B4(int nPlayer) {
    s32 nMsg = -1;
    s32 r;
    if (!fn_800F2358(nPlayer)) {
        if (gPlayers[nPlayer].nE98 >= 10) {
            r = Rand_Next(0) % 20;
        } else {
            r = Rand_Next(0) % 100;
        }
        if (r <= 5) {
            gPlayers[nPlayer].nDBC = 5;
        } else if (r <= 10) {
            gPlayers[nPlayer].nDBC = 3;
        } else if (r <= 20) {
            gPlayers[nPlayer].nDBC = 2;
        }
    }
    if (gPlayers[nPlayer].nDBC > 1) {
        gPlayers[nPlayer].nE98 = 0;
    } else {
        gPlayers[nPlayer].nE98++;
    }
    if (gPlayers[nPlayer].nDBC > 1) {
        fn_800A631C();
        switch (gPlayers[nPlayer].nDBC) {
        case 2:
            nMsg = 0x3A;
            break;
        case 3:
            nMsg = 0x3B;
            break;
        case 4:
            break;
        case 5:
            nMsg = 0x3D;
            break;
        }
    }
    if (nMsg != -1) {
        fn_800F2958((u16)nMsg, 0);
    }
}

// The player's game is over (all shots taken).
u8 fn_800F2358(int nPlayer) {
    if (Game_GetMode() == 0x10) {
        if (gPlayers[nPlayer].nDC0 == 20) {
            return 1;
        }
    } else if (Game_GetMode() == 0x11) {
        if (gPlayers[nPlayer].nDC0 == 5) {
            return 1;
        }
    } else if (gPlayers[nPlayer].nDC0 == 0) {
        return 1;
    }
    return 0;
}

s32 fn_800F2408(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F7D94(arg0);
    }
    if (Game_GetMode() == 0xE) {
        return fn_800F37F8(arg0);
    }
    if (Game_GetMode() == 0x10) {
        return fn_800F59CC(arg0);
    }
    if (Game_GetMode() == 0x11) {
        return fn_800F6A00(arg0);
    }
    return 0;
}

s32 fn_800F2494(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F7D9C(arg0);
    }
    return 0;
}

s32 fn_800F24D8(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F80A0(arg0);
    }
    if (Game_GetMode() == 0x10) {
        return fn_800F59D4(arg0);
    }
    return 0;
}

s32 fn_800F2534(s32 arg0) {
    if (Game_GetMode() == 0x11) {
        return fn_800F6A34(arg0);
    }
    return 0;
}

// The target game's prize (from the prize row with id 999).
s32 fn_800F2578(void) {
    int i;
    for (i = 0; i < 20; i++) {
        if (lbl_80200538.aMini[i].nId == 999) {
            if (Game_GetMode() == 0xD) {
                return lbl_80200538.aMini[i].n4;
            }
            if (Game_GetMode() == 0x10) {
                return lbl_80200538.aMini[i].n8;
            }
            if (Game_GetMode() == 0x11) {
                return lbl_80200538.aMini[i].nC;
            }
        }
    }
    return 0;
}

void fn_800F263C(s32 nMsg) {
    fn_800F2958((nMsg & 0xFFFF), 1);
}

void fn_800F2664(int nPlayer) {
}

void fn_800F2668(int nPlayer) {
}

// Scale n by table entry i; which table depends on fn_80015464 (0..2).
s32 fn_800F266C(s32 n, int i) {
    if (fn_80015464() == 0) {
        return n * lbl_80192810[i];
    }
    if (fn_80015464() == 1) {
        return n * lbl_80192844[i];
    }
    if (fn_80015464() == 2) {
        return n * lbl_80192880[i];
    }
    return n;
}

// Is f far enough for the player's tee set (0: 313, 1: 300, 2 and 3: 293)?
u8 fn_800F2788(int nPlayer, f32 f) {
    switch (gSession.nTeeSet[nPlayer]) {
    case 0:
        if (f >= 313.0f) {
            return 1;
        }
        break;
    case 1:
        if (f >= 300.0f) {
            return 1;
        }
        break;
    case 2:
    case 3:
        if (f >= 293.0f) {
            return 1;
        }
        break;
    }
    return 0;
}

s32 fn_800F2810(s32 n) {
    if (fn_80015464() == 0) {
        switch (n) {
        case 0xD7: return 0;
        case 0xD5: return 1;
        case 0xD4: return 2;
        case 0xD6: return 3;
        case 0xD8: return 4;
        }
    } else if (fn_80015464() == 1) {
        switch (n) {
        case 0x3B: return 0;
        case 0x3C: return 1;
        case 0x3D: return 2;
        case 0x3E: return 3;
        case 0x3F: return 4;
        }
    } else if (fn_80015464() == 2) {
        switch (n) {
        case 24: return 0;
        case 25: return 1;
        case 26: return 2;
        case 27: return 3;
        case 28: return 4;
        }
    }
    return 4;
}

void fn_800F2958(s32 nMsg, s32 a) {
    fn_800A7664(7, nMsg, a);
}

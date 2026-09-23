// GoDynamicCam.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the dynamic
// cameras. Loads the camera shots, sequences and shot choices from the camera files into
// lbl_80281D88's tables, and picks the sequence and shot that fit a golfer's situation (club,
// shot kind, course and hole, game mode).

#include "golfer.h"
#include "game.h"
#include "dyncam.h"

u8   fn_8001E9CC(u32* pBits, int nBit);         // the bit is set
u8   fn_8003CBE8(CamSequence* pSequence, int nPlayer);
u8   fn_8003CD9C(CamSequence* pSequence, int nPlayer, u8 b);
u8   fn_8003D0EC(CamSequence* pSequence, int nKind);

// Turns each sequence's follow-on index into a pointer; a sequence whose follow-on has no shot
// choices follows itself.
void fn_80039E58(void) {
    int i;

    for (i = 0; i < lbl_80281D88->nSequences; i++) {
        // port: the file keeps an index in the pointer field
        lbl_80281D88->pSequences[i].p20 = &lbl_80281D88->pSequences[(s32)lbl_80281D88->pSequences[i].p20];
        if (lbl_80281D88->pSequences[i].p20->nChoices <= 0) {
            lbl_80281D88->pSequences[i].p20 = &lbl_80281D88->pSequences[i];
        }
    }
}

// Allocates the dynamic cameras' tables, empty.
void fn_80039FF8(void) {
    DynCamTables* pTables = fn_80009B34(sizeof(DynCamTables), 2, 0, "GoDynamicCam.c", 938);

    lbl_80281D88 = pTables;
    pTables->pSequences = NULL;
    lbl_80281D88->pShots = NULL;
    lbl_80281D88->p8 = NULL;
    lbl_80281D88->pChoices = NULL;
    lbl_80281D88->nShots = 0;
    lbl_80281D88->nSequences = 0;
    lbl_80281D88->n18 = 0;
    lbl_80281D88->nChoicesUsed = 0;
}

// Frees the dynamic cameras' tables.
void fn_8003A074(void) {
    lbl_80281D88->nShots = 0;
    lbl_80281D88->nSequences = 0;
    lbl_80281D88->n18 = 0;
    lbl_80281D88->nChoicesUsed = 0;
    if (lbl_80281D88->pSequences != NULL) {
        fn_80009E70(lbl_80281D88->pSequences);
        lbl_80281D88->pSequences = NULL;
    }
    if (lbl_80281D88->pShots != NULL) {
        fn_80009E70(lbl_80281D88->pShots);
        lbl_80281D88->pShots = NULL;
    }
    if (lbl_80281D88->p8 != NULL) {
        fn_80009E70(lbl_80281D88->p8);
        lbl_80281D88->p8 = NULL;
    }
    if (lbl_80281D88->pChoices != NULL) {
        fn_80009E70(lbl_80281D88->pChoices);
        lbl_80281D88->pChoices = NULL;
    }
    lbl_80281D88->n1C = 0;
    fn_80009E70(lbl_80281D88);
}

// The shot is one of kinds 1, 3, 13, 28..34 or 40..45.
u8 fn_8003A76C(CamShot* pShot) {
    u8 nKind = pShot->bAD;

    if (nKind == 3 || nKind == 1 || (nKind >= 29 && nKind <= 33) || (nKind >= 40 && nKind <= 45)
        || nKind == 13 || nKind == 28 || nKind == 34) {
        return 1;
    }
    return 0;
}

// The choice may be used on the current hole.
u8 fn_8003AB94(CamChoice* pChoice) {
    if (fn_8001E9CC(pChoice->aNoHoles, Game_GetCourse() * 18 + fn_80015464())) return 0;
    return 1;
}

// The choice suits the player's golfer: a choice marked b17 is only for golfer models 0, 1, 8, 10,
// 12..15 and 17.
u8 fn_8003ABEC(CamChoice* pChoice, int nPlayer) {
    int nModel = gPlayers[nPlayer].golfer.nModelID;

    if (pChoice->b17) {
        if (nModel == 0 || nModel == 1 || nModel == 8 || nModel == 10 || nModel == 12 || nModel == 13
            || nModel == 14 || nModel == 15 || nModel == 17) {
            return 1;
        }
        return 0;
    }
    return 1;
}

// A default sequence of the kind: one whose name starts with "DEF".
u8 fn_8003CAFC(CamSequence* pSequence, int nKind) {
    if (pSequence == NULL) return 0;
    if (!fn_8003D0EC(pSequence, nKind)) return 0;
    if (pSequence->szName[0] != 'D') return 0;
    if (pSequence->szName[1] != 'E') return 0;
    if (pSequence->szName[2] != 'F') return 0;
    return 1;
}

// Maps n (0..19) through lbl_80187988; 1 depends on the ball's lie (1 on lie 0, else 2).
s32 fn_8003CB80(u32 n, int nPlayer) {
    s32 nRet;

    if (n > 19) return 0;
    if (n == 1) {
        nRet = 2;
        if (gPlayers[nPlayer].ball.nLie == 0) {
            nRet = 1;
        }
        return nRet;
    }
    return lbl_80187988[n];
}

// Maps n through lbl_801879D8.
s32 fn_8003CBD4(int n, int nPlayer) {
    return lbl_801879D8[n];
}

// The value is within the sequence's f2C..f30.
u8 fn_8003CD6C(CamSequence* pSequence, f32 f) {
    if (f <= pSequence->f30 && f >= pSequence->f2C) {
        return 1;
    }
    return 0;
}

// The sequence is used on the current course.
u8 fn_8003D00C(CamSequence* pSequence) {
    return (pSequence->uCourses & (1 << Game_GetCourse())) != 0;
}

// The sequence is used for fn_800D2B08's current value.
u8 fn_8003D054(CamSequence* pSequence) {
    return (pSequence->n4B & (1 << fn_800D2B08())) != 0;
}

// Bit nBit of the mask is set.
u8 fn_8003D0A0(int nMask, int nBit) {
    return (nMask & (1 << nBit)) != 0;
}

// The value is within the sequence's f24..f28.
u8 fn_8003D0BC(CamSequence* pSequence, f32 f) {
    if (f >= pSequence->f24 && f <= pSequence->f28) {
        return 1;
    }
    return 0;
}

// The sequence is of the kind: kind 14 takes any sequence, kind 10 any of kinds 6..9.
u8 fn_8003D0EC(CamSequence* pSequence, int nKind) {
    if (nKind == 14) return 1;
    if (nKind == 10) {
        switch (pSequence->b44) {
        case 6:
        case 7:
        case 8:
        case 9:
            return 1;
        }
        return 0;
    }
    return pSequence->b44 == nKind;
}

// The shot is of the kind: kind 14 takes any shot, kind 10 any of kinds 6..9.
u8 fn_8003D240(CamShot* pShot, int nKind) {
    if (nKind == 14) return 1;
    if (nKind == 10) {
        switch (pShot->bAD) {
        case 6:
        case 7:
        case 8:
        case 9:
            return 1;
        }
        return 0;
    }
    return pShot->bAD == nKind;
}

// The sequence suits the player's club and shot kind.
u8 fn_8003D7A0(CamSequence* pSequence, int nPlayer) {
    if (pSequence == NULL) return 0;
    if (fn_8003CBE8(pSequence, nPlayer) && fn_8003CD9C(pSequence, nPlayer, 1)) {
        return 1;
    }
    return 0;
}

// The shot's f64 and f60 into *pA and *pB; when fn_800453C8 holds for the player they are
// turned a quarter: (-f64, f60), or (f64, -f60) for a shot with bAF or bB0 set to 21.
void fn_8003DAC8(CamShot* pShot, int nPlayer, f32* pA, f32* pB) {
    if (pShot != NULL) {
        if (fn_800453C8(nPlayer, pShot)) {
            if (pShot->bAF == 21 || pShot->bB0 == 21) {
                if (pA != NULL) {
                    *pA = pShot->f64;
                }
                if (pB != NULL) {
                    *pB = -pShot->f60;
                }
            } else {
                if (pA != NULL) {
                    *pA = -pShot->f64;
                }
                if (pB != NULL) {
                    *pB = pShot->f60;
                }
            }
        } else {
            if (pA != NULL) {
                *pA = pShot->f64;
            }
            if (pB != NULL) {
                *pB = pShot->f60;
            }
        }
    }
}

#ifndef UIOBJECT_H
#define UIOBJECT_H

// uiobject.h (our name): uiobject.c's state, the 3D objects the in-game UI draws. Only what the
// code read so far uses.

#include "camera.h"
#include "dynobj.h"
#include "lighting.h"

// One object's settings (lbl_801F5B98: two of them, 0x38 bytes each; UI_Obj_RenderBoostUI picks one by
// its argument), filled with constants by UI_Obj_InitModule.
typedef struct UIObjSettings {
    f32  a0[10];                // 0x00  [0..2] the position, [5] its tilt (UI_Obj_RenderBoostUI sets it
                                //       from the spin asked for), [6] its scale, [9] its roll,
                                //       0..2 pi
    f32  a28[4];                // 0x28  object 0: the rings' largest size, the size they start
                                //       fading at, their growth per frame; [3] also fills
                                //       lbl_801F5B78 (fn_800AE3C4)
} UIObjSettings;
LAYOUT_ASSERT(UIObjSettings, 0x38);

extern UIObjSettings lbl_801F5B98[2];
extern TexBank*   lbl_802820BC; // the bank of the textures below
extern TexEntry*  lbl_802820C0; // "ring"
extern TexEntry*  lbl_802820C4; // } "toball", both
extern TexEntry*  lbl_802820C8; // }
extern f32        lbl_801F5B78[8];
extern UObject*   lbl_802820D0; // made from the 'TEO ' object 10003
extern CamLens*   lbl_802820CC; // the objects' lens (CA_spCreateCamera)
extern LightGroup lbl_801F5B40; // their lights
extern f32        lbl_802820B8; // } the light's colour (red, green, blue: fn_800AEFE4)
extern f32        lbl_80281470; // }   0.05
extern f32        lbl_80281474; // }   0.476
extern f32        lbl_80281478; // 0.19: fn_800AEFE4 hands 255 times it to fn_80035154
extern f32        lbl_8018830C[8][4];   // the rings' colours, one per power boost level
extern f32        lbl_8018EC10[4];      // the lens's second point (CA_vSetLookAt)

#endif

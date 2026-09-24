#ifndef UIOBJECT_H
#define UIOBJECT_H

// uiobject.h (our name): uiobject.c's state, the 3D objects the in-game UI draws. Only what the
// code read so far uses.

#include "camera.h"
#include "dynobj.h"
#include "lighting.h"

// The objects' settings (lbl_801F5B98, 0x70 bytes), filled with constants by UI_Obj_InitModule.
typedef struct UIObjSettings {
    f32  a0[10];                // 0x00
    f32  a28[4];                // 0x28  [3] also fills lbl_801F5B78 (fn_800AE3C4)
    f32  a38[10];               // 0x38
    u8   unk60[0x70 - 0x60];
} UIObjSettings;
LAYOUT_ASSERT(UIObjSettings, 0x70);

extern UIObjSettings lbl_801F5B98;
extern TexBank*   lbl_802820BC; // the bank of the textures below
extern TexEntry*  lbl_802820C0; // "ring"
extern TexEntry*  lbl_802820C4; // } "toball", both
extern TexEntry*  lbl_802820C8; // }
extern f32        lbl_801F5B78[8];
extern UObject*   lbl_802820D0; // made from the 'TEO ' object 10003
extern CamLens*   lbl_802820CC; // the objects' lens (fn_80076400)
extern LightGroup lbl_801F5B40; // their lights

#endif

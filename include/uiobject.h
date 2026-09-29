#ifndef UIOBJECT_H
#define UIOBJECT_H

// uiobject.h (our name): uiObject.c's state, the 3D objects the in-game UI draws. Only what the
// code read so far uses.

#include "camera.h"
#include "dynobj.h"
#include "lighting.h"

// One object's settings (gUIObjSettings: two of them, 0x38 bytes each; UI_Obj_RenderBoostUI picks one by
// its argument), filled with constants by UI_Obj_InitModule.
typedef struct UIObjSettings {
    f32  a0[10];                // 0x00  [0..2] the position, [5] its tilt (UI_Obj_RenderBoostUI sets it
                                //       from the spin asked for), [6] its scale, [9] its roll,
                                //       0..2 pi
    f32  a28[4];                // 0x28  object 0: the rings' largest size, the size they start
                                //       fading at, their growth per frame; [3] also fills
                                //       gUIObjBoostRingSize (UI_Obj_ResetBoostRings)
} UIObjSettings;
LAYOUT_ASSERT(UIObjSettings, 0x38);

extern UIObjSettings gUIObjSettings[2];
extern TexBank*   gpUIObjTexBank; // the bank of the textures below
extern TexEntry*  gpUIObjRingTexture; // "ring"
extern TexEntry*  gpUIObjBoostTexture; // } "toball", both
extern TexEntry*  gpUIObjBaseTexture; // }
extern f32        gUIObjBoostRingSize[8];
extern UObject*   gpUIObjModel; // made from the 'TEO ' object 10003
extern CamLens*   gpUIObjLens; // the objects' lens (CA_spCreateCamera)
extern LightGroup gUIObjLights; // their lights
extern f32        gUIObjLightRed; // } the light's colour (red, green, blue: UI_Obj_DrawSpinModel)
extern f32        gUIObjLightGreen; // }   0.05
extern f32        gUIObjLightBlue; // }   0.476
extern f32        gUIObjAlpha; // 0.19: UI_Obj_DrawSpinModel hands 255 times it to RenderState_SetConstantAlpha
extern f32        gBoostLevelColours[8][4];   // the rings' colours, one per power boost level
extern f32        gUIObjLookAtTarget[4];      // the lens's second point (CA_vSetLookAt)

#endif

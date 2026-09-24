#ifndef UIOBJECT_H
#define UIOBJECT_H

// uiobject.h (our name): uiobject.c's state, the 3D objects the in-game UI draws. Only what the
// code read so far uses.

#include "camera.h"
#include "dynobj.h"
#include "lighting.h"

extern UObject*   lbl_802820D0; // made from the 'TEO ' object 10003
extern CamLens*   lbl_802820CC; // the objects' lens (fn_80076400)
extern LightGroup lbl_801F5B40; // their lights

#endif

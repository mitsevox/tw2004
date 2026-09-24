#ifndef FRONTEND_UISVEC_H
#define FRONTEND_UISVEC_H

// The UI Studio's four-float values (uistudio.h includes this), in a file of their own so the menu
// UI's elements (uiText.c) can use them without the rest of uistudio.h.

#include "game_types.h"

// Four floats, copied as one (UISScreen.c).
typedef struct UISVec4 {
    f32 a[4];
} UISVec4;

// The values screen nodes are drawn with (UISScreen.c): each node adds its UISNodeInfo.afAdd to
// the first and multiplies its afMul into the second for its children.
extern UISVec4 lbl_80280628;
extern UISVec4 lbl_80280638;

UISVec4* fn_8016C18C(void);
UISVec4* fn_8016C198(void);

#endif

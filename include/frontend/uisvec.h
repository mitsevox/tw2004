#ifndef FRONTEND_UISVEC_H
#define FRONTEND_UISVEC_H

// The UI Studio's four-float values (uistudio.h includes this), in a file of their own so the menu
// UI's elements (uiText.c) can use them without the rest of uistudio.h.

#include "game_types.h"

// Four floats, copied as one (UISScreen.c). Names: EA's (Madden 2003 STABS).
typedef struct UISColorVector_t {
    f32 r;                          // 0x00
    f32 g;                          // 0x04
    f32 b;                          // 0x08
    f32 a;                          // 0x0C
} UISColorVectorT;

// The values screen controls are drawn with (UISScreen.c): each control adds its
// UISControlInfoT.Transform.AdditiveFactor to the first and multiplies its MultiplerFactor into the
// second for its children.
extern UISColorVectorT _AdditiveColorFactor;
extern UISColorVectorT _MultiplerColorFactor;

UISColorVectorT* UISGetColorAdditive(void);
UISColorVectorT* UISGetColorMultipler(void);

#endif

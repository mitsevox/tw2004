// GoObjShadow.c (our name): shadows drawn with the "shadow" texture into five mesh objects
// (lbl_801D96B8); fn_80093DB8 is called from GoDynObj.c. Its data starts on its own 8-byte
// boundaries (.sdata 0x80281390, .sdata2 0x80283C68).

#include "engine.h"

// Skin.c
void fn_80036054(void* pMesh, int n, s32* pDesc);
void fn_800360A0(void* pMesh);

#define NUM_SHADOW_MESHES 5

u8 lbl_801D96B8[NUM_SHADOW_MESHES][0x28];  // mesh objects, as Swing.c keeps its trail meshes
TexBank*  lbl_80281F50;         // the "shadow" texture's bank
TexEntry* lbl_80281F54;         // and the texture

void fn_80093D3C(void) {
    s32 desc[2];
    int i;
    fn_800102DC(fn_8000BEE4("shadow"), &lbl_80281F50, &lbl_80281F54);
    desc[0] = 4;
    desc[1] = 1;
    for (i = 0; i < NUM_SHADOW_MESHES; i++) {
        fn_80036054(lbl_801D96B8[i], 0, desc);
    }
}

void fn_800940FC(void) {
    int i;
    for (i = 0; i < NUM_SHADOW_MESHES; i++) {
        fn_800360A0(lbl_801D96B8[i]);
    }
}

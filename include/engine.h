// engine.h (our name): the services under the game code that more than one file calls: memory
// and strings, math, the random streams, the file streamer (UStream.c), views and cameras,
// events, sound and effects, and character animation. The game itself is in game.h.

#ifndef ENGINE_H
#define ENGINE_H

#include "game_types.h"

// ---- memory and strings ----------------------------------------------------------------------

void* Mem_cpy(void* pDst, const void* pSrc, u32 uLen);    // returns pDst
void* fn_80005AE8(void* pDst, int nValue, u32 uLen);      // memset; returns pDst
void* fn_80009B34(u32 uSize, u32 uFlags, u32 uAlign, const char* pFile, int nLine);  // alloc
void  fn_80009E70(void* p);             // free
void* fn_800951A0(u32 uSize, int nAlign, int a);
void  fn_800953C8(int a);
char* strcpy(char* pDst, const char* pSrc);

// ---- math and random numbers -----------------------------------------------------------------

f32  fn_80009744(f32* pVec);            // dot with itself
f32  fn_8000AD9C(f32 x);                // fabsf
void fn_8000B1D4(int nStream, u32 uSeed);   // seed a random stream

// ---- the file streamer (UStream.c) -----------------------------------------------------------

typedef struct UStreamObject UStreamObject;

int  UStream_RegisterHandler(u32 uType, void (*pfnHandler)(UStreamObject*));
int  UStream_UnregisterHandler(u32 uType);
u32  fn_8000E790(UStreamObject* pObject, u32 uMax, void* pDst);   // copy the data out, free the object
u32  fn_8000E81C(UStreamObject* pObject, void** ppData);          // the data and its size

// ---- views, events, sound, animation ---------------------------------------------------------

void* fn_80017028(int nView);           // the view
void fn_8001704C(int nView, int nPlayer);   // attach a player
void fn_8001C804(int nPlayer, int a, int b);
void View_SetCamera(void* pView, int nCamera, int nPlayer, int nView);
void fn_80063B98(void* pView, f32 f, f32* pVec);
void EVENT_Trigger(int nPlayer, int nEvent, int a, int b);    // through the event table at lbl_80188628
void Emotion_UpdatePlayerEmotion(int nPlayer);
void fn_80095744(int nHandle, int nAnim);   // play an animation
int  fn_80095780(int nHandle);          // the animation playing
void fn_800957D8(int nHandle);
void fn_800A30E4(int nKind, u8* pBall, int nPlayer, int a, f32 f);
void fn_800A6278(void);
void fn_800A62A4(void);
void fn_800A62E0(void);
void fn_800A6358(void);
void fn_800A63D0(void);
void fn_800A76E4(void);
void fn_80125910(u8 b);

#endif

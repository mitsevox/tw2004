#include "dolphin/mtx/mtx44ext.h"
#include "dolphin/os.h"
#include <math.h>

static f32 mtxUnit[] = {0.0f, 1.0f, 0.5f, 3.0f};

void C_MTXOrtho(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f) {
  f32 tmp;
#line 0xfe
  ASSERTMSG((m != 0), "MTXOrtho():  NULL Mtx44Ptr 'm' ");
  ASSERTMSG((t != b), "MTXOrtho():  't' and 'b' clipping planes are equal ");
  ASSERTMSG((l != r), "MTXOrtho():  'l' and 'r' clipping planes are equal ");
  ASSERTMSG((n != f), "MTXOrtho():  'n' and 'f' clipping planes are equal ");

  tmp = 1.0f / (r - l);
  m[0][0] = 2.0f * tmp;
  m[0][1] = 0.0f;
  m[0][2] = 0.0f;
  m[0][3] = -(r + l) * tmp;

  tmp = 1.0f / (t - b);
  m[1][0] = 0.0f;
  m[1][1] = 2.0f * tmp;
  m[1][2] = 0.0f;
  m[1][3] = -(t + b) * tmp;

  m[2][0] = 0.0f;
  m[2][1] = 0.0f;

  tmp = 1.0f / (f - n);
  m[2][2] = -(1.0f) * tmp;
  m[2][3] = -(f)*tmp;
  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 0.0f;
  m[3][3] = 1.0f;
}

#ifdef GEKKO
#endif // GEKKO

#ifdef GEKKO
#endif

#ifdef GEKKO
#endif

#ifdef GEKKO
#endif

#define SWAP(a, b)                                                                                 \
  {                                                                                                \
    f32 tmp;                                                                                       \
    tmp = (a);                                                                                     \
    (a) = (b);                                                                                     \
    (b) = tmp;                                                                                     \
  }

#undef SWAPF
#undef NUM

#ifdef GEKKO
#endif

#ifdef GEKKO
#endif // GEKKO

#ifdef GEKKO
#endif // GEKKO

#ifdef GEKKO
#endif // GEKKO

#ifdef GEKKO
#endif

#ifdef GEKKO
#endif // GEKKO

#ifdef GEKKO

#endif // GEKKO

// File written by stdump v2.1 on 2026-09-24
//
// Input file:
//   EA_DASH.ELF
// Toolchain version(s):
//   GNU as v2.9-ee-991111b
//   unknown
// Built-in types:
//   __int128_t               128-bit signed integer
//   __int128_t               128-bit integer
//   __uint128_t              128-bit unsigned integer
//   __uint128_t              128-bit integer
//   __wchar_t                16-bit unsigned integer
//   __wchar_t                32-bit signed integer
//   bool                     8-bit boolean
//   char                     8-bit integer
//   complex double           64-bit floating point
//   complex float            32-bit floating point
//   complex long double      64-bit floating point
//   double                   64-bit floating point
//   float                    32-bit floating point
//   int                      32-bit signed integer
//   long double              64-bit floating point
//   long int                 64-bit signed integer
//   long long int            64-bit signed integer
//   long long int            128-bit signed integer
//   long long unsigned int   64-bit unsigned integer
//   long long unsigned int   128-bit unsigned integer
//   long unsigned int        64-bit unsigned integer
//   long128                  128-bit integer
//   short int                16-bit signed integer
//   short unsigned int       16-bit unsigned integer
//   signed char              8-bit signed integer
//   u_long128                128-bit integer
//   unsigned char            8-bit unsigned integer
//   unsigned int             32-bit unsigned integer
//   void                     void

typedef struct { // 0x8
	/* 0x0 */ int real;
	/* 0x4 */ int imag;
} complex int;

typedef int ptrdiff_t;
typedef unsigned int size_t;
typedef int wchar_t;
typedef unsigned int wint_t;
typedef void* (*ALLOC_FUNC)(/* parameters unknown */);
typedef void (*FREE_FUNC)(/* parameters unknown */);
typedef void* (*REALLOC_FUNC)(/* parameters unknown */);
typedef int __int32_t;
typedef unsigned int __uint32_t;
typedef char *__gnuc_va_list;
// warning: multiple differing types with the same name (type name not equal)
typedef unsigned int clock_t;
// warning: multiple differing types with the same name (type name not equal)
typedef int time_t;

struct tm { // 0x24
	/* 0x00 */ int tm_sec;
	/* 0x04 */ int tm_min;
	/* 0x08 */ int tm_hour;
	/* 0x0c */ int tm_mday;
	/* 0x10 */ int tm_mon;
	/* 0x14 */ int tm_year;
	/* 0x18 */ int tm_wday;
	/* 0x1c */ int tm_yday;
	/* 0x20 */ int tm_isdst;
};

typedef __uint32_t ULong;

struct _glue { // 0xc
	/* 0x0 */ _glue *_next;
	/* 0x4 */ int _niobs;
	/* 0x8 */ __sFILE *_iobs;
};

// warning: multiple differing types with the same name (type name not equal)
struct _Bigint { // 0x18
	/* 0x00 */ _Bigint *_next;
	/* 0x04 */ int _k;
	/* 0x08 */ int _maxwds;
	/* 0x0c */ int _sign;
	/* 0x10 */ int _wds;
	/* 0x14 */ ULong _x[1];
};

struct _atexit { // 0x88
	/* 0x00 */ _atexit *_next;
	/* 0x04 */ int _ind;
	/* 0x08 */ void (*_fns[32])(/* parameters unknown */);
};

struct __sbuf { // 0x8
	/* 0x0 */ unsigned char *_base;
	/* 0x4 */ int _size;
};

// warning: multiple differing types with the same name (type name not equal)
typedef int _fpos_t;

struct __sFILE { // 0x58
	/* 0x00 */ unsigned char *_p;
	/* 0x04 */ int _r;
	/* 0x08 */ int _w;
	/* 0x0c */ short int _flags;
	/* 0x0e */ short int _file;
	/* 0x10 */ __sbuf _bf;
	/* 0x18 */ int _lbfsize;
	/* 0x1c */ void *_cookie;
	/* 0x20 */ int (*_read)(/* parameters unknown */);
	/* 0x24 */ int (*_write)(/* parameters unknown */);
	/* 0x28 */ _fpos_t (*_seek)(/* parameters unknown */);
	/* 0x2c */ int (*_close)(/* parameters unknown */);
	/* 0x30 */ __sbuf _ub;
	/* 0x38 */ unsigned char *_up;
	/* 0x3c */ int _ur;
	/* 0x40 */ unsigned char _ubuf[3];
	/* 0x43 */ unsigned char _nbuf[1];
	/* 0x44 */ __sbuf _lb;
	/* 0x4c */ int _blksize;
	/* 0x50 */ int _offset;
	/* 0x54 */ _reent *_data;
};

struct _reent { // 0x2ec
	/* 0x000 */ int _errno;
	/* 0x004 */ __sFILE *_stdin;
	/* 0x008 */ __sFILE *_stdout;
	/* 0x00c */ __sFILE *_stderr;
	/* 0x010 */ int _inc;
	/* 0x014 */ char _emergency[25];
	/* 0x030 */ int _current_category;
	/* 0x034 */ char *_current_locale;
	/* 0x038 */ int __sdidinit;
	/* 0x03c */ void (*__cleanup)(/* parameters unknown */);
	/* 0x040 */ _Bigint *_result;
	/* 0x044 */ int _result_k;
	/* 0x048 */ _Bigint *_p5s;
	/* 0x04c */ _Bigint **_freelist;
	/* 0x050 */ int _cvtlen;
	/* 0x054 */ char *_cvtbuf;
	/* 0x058 */ union { // 0xf0
		/* 0x058 */ struct { // 0x4c
			/* 0x058 */ unsigned int _rand_next;
			/* 0x05c */ char *_strtok_last;
			/* 0x060 */ char _asctime_buf[26];
			/* 0x07c */ tm _localtime_buf;
			/* 0x0a0 */ int _gamma_signgam;
		} _reent;
		/* 0x058 */ struct { // 0xf0
			/* 0x058 */ unsigned char *_nextf[30];
			/* 0x0d0 */ unsigned int _nmalloc[30];
		} _unused;
	} _new;
	/* 0x148 */ _atexit *_atexit;
	/* 0x14c */ _atexit _atexit0;
	/* 0x1d4 */ void (**_sig_func)(/* parameters unknown */);
	/* 0x1d8 */ _glue __sglue;
	/* 0x1e4 */ __sFILE __sf[3];
};

typedef _fpos_t fpos_t;
typedef __sFILE FILE;
typedef int int32;
typedef unsigned int uint32;
typedef short int int16;
typedef short unsigned int uint16;
typedef unsigned char uint8;
typedef signed char int8;
typedef int F26Dot6;
typedef int F16Dot16;
typedef int Fract;
typedef int Fixed;
typedef void* (*FF_GetCacheMemoryPtr)(/* parameters unknown */);
typedef struct jmp_buf[1] { // 0x24
	/* 0x000 */ unsigned int eax;
	/* 0x004 */ unsigned int ebx;
	/* 0x008 */ unsigned int ecx;
	/* 0x00c */ unsigned int edx;
	/* 0x010 */ unsigned int esi;
	/* 0x014 */ unsigned int edi;
	/* 0x018 */ unsigned int ebp;
	/* 0x01c */ unsigned int esp;
	/* 0x020 */ unsigned int eip;
};

typedef struct { // 0x94
	/* 0x00 */ unsigned int stamp1;
	/* 0x04 */ int numPointers;
	/* 0x08 */ int maxPointers;
	/* 0x0c */ void **base;
	/* 0x10 */ jmp_buf env;
	/* 0x34 */ void *fast_base[7];
	/* 0x50 */ unsigned int fast_size[7];
	/* 0x6c */ int fast_free[7];
	/* 0x88 */ unsigned int ii;
	/* 0x8c */ unsigned int state;
	/* 0x90 */ unsigned int stamp2;
} tsiMemObject;

typedef struct { // 0x20
	/* 0x00 */ unsigned char *privateBase;
	/* 0x04 */ unsigned int pos;
	/* 0x08 */ unsigned int maxPos;
	/* 0x0c */ unsigned int posZero;
	/* 0x10 */ char constructorType;
	/* 0x14 */ tsiMemObject *mem;
	/* 0x18 */ unsigned int bitBufferIn;
	/* 0x1c */ unsigned int bitCountIn;
} InputStream;

typedef struct { // 0x1c
	/* 0x00 */ unsigned char *base;
	/* 0x04 */ unsigned int maxPos;
	/* 0x08 */ unsigned int pos;
	/* 0x0c */ unsigned int maxLength;
	/* 0x10 */ tsiMemObject *mem;
	/* 0x14 */ unsigned int bitBufferOut;
	/* 0x18 */ unsigned int bitCountOut;
} OutputStream;

typedef struct { // 0x18
	/* 0x00 */ int isValid;
	/* 0x04 */ int16 Ascender;
	/* 0x06 */ int16 Descender;
	/* 0x08 */ int16 LineGap;
	/* 0x0a */ uint16 maxAW;
	/* 0x0c */ F16Dot16 caretDx;
	/* 0x10 */ F16Dot16 caretDy;
	/* 0x14 */ int16 underlinePosition;
	/* 0x16 */ int16 underlineThickness;
} T2K_FontWideMetrics;

typedef struct { // 0x8
	/* 0x0 */ uint32 leftRightIndex;
	/* 0x4 */ int16 value;
} kernPair0Struct;

typedef struct { // 0x10
	/* 0x0 */ tsiMemObject *mem;
	/* 0x4 */ uint16 nPairs;
	/* 0x6 */ uint16 searchRange;
	/* 0x8 */ uint16 entrySelector;
	/* 0xa */ uint16 rangeShift;
	/* 0xc */ kernPair0Struct *pairs;
} kernSubTable0Data;

typedef struct { // 0x14
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ uint16 version;
	/* 0x08 */ int32 length;
	/* 0x0c */ uint16 coverage;
	/* 0x10 */ void *kernData;
} kernSubTable;

typedef struct { // 0x10
	/* 0x0 */ tsiMemObject *mem;
	/* 0x4 */ uint16 version;
	/* 0x8 */ int nTables;
	/* 0xc */ kernSubTable **table;
} kernClass;

typedef struct { // 0x4
	/* 0x0 */ uint16 rangeMaxPPEM;
	/* 0x2 */ uint16 rangeGaspBehavior;
} gaspRangeType;

typedef struct { // 0xc
	/* 0x0 */ tsiMemObject *mem;
	/* 0x4 */ uint16 version;
	/* 0x6 */ uint16 numRanges;
	/* 0x8 */ gaspRangeType *gaspRange;
} gaspClass;

typedef struct { // 0x8
	/* 0x0 */ unsigned int offset;
	/* 0x4 */ unsigned int length;
} CFF_Class;

typedef struct { // 0x8
	/* 0x0 */ unsigned int offset;
	/* 0x4 */ unsigned int length;
} DSIGClass;

typedef struct { // 0x14
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int tag;
	/* 0x08 */ int checkSum;
	/* 0x0c */ unsigned int offset;
	/* 0x10 */ unsigned int length;
} sfnt_DirectoryEntry;

typedef struct { // 0x14
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int version;
	/* 0x08 */ short int numOffsets;
	/* 0x0a */ short int searchRange;
	/* 0x0c */ short int entrySelector;
	/* 0x0e */ short int rangeShift;
	/* 0x10 */ sfnt_DirectoryEntry **table;
} sfnt_OffsetTable;

typedef struct { // 0x8
	/* 0x0 */ uint16 platformID;
	/* 0x2 */ uint16 specificID;
	/* 0x4 */ uint32 offset;
} sfnt_platformEntry;

typedef struct { // 0x38
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int16 version;
	/* 0x06 */ int16 numEncodingTables;
	/* 0x08 */ int32 numGlyphs;
	/* 0x0c */ sfnt_platformEntry **platform;
	/* 0x10 */ uint8 *cmapData;
	/* 0x14 */ int length;
	/* 0x18 */ int16 preferedEncodingTable;
	/* 0x1a */ uint16 preferedFormat;
	/* 0x1c */ uint16 selectedPlatformID;
	/* 0x1e */ uint16 selectedPlatformSpecificID;
	/* 0x20 */ uint16 bytesConsumed;
	/* 0x22 */ uint16 figIndex[10];
} cmapClass;

typedef struct { // 0x10
	/* 0x0 */ tsiMemObject *mem;
	/* 0x4 */ uint32 version;
	/* 0x8 */ uint32 directoryCount;
	/* 0xc */ uint32 *tableOffsets;
} ttcfClass;

typedef struct { // 0x3c
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int32 version;
	/* 0x08 */ int32 fontRevision;
	/* 0x0c */ uint32 checkSumAdjustment;
	/* 0x10 */ uint32 magicNumber;
	/* 0x14 */ uint16 flags;
	/* 0x16 */ uint16 unitsPerEm;
	/* 0x18 */ int32 created_bc;
	/* 0x1c */ int32 created_ad;
	/* 0x20 */ int32 modified_bc;
	/* 0x24 */ int32 modified_ad;
	/* 0x28 */ int16 xMin;
	/* 0x2a */ int16 yMin;
	/* 0x2c */ int16 xMax;
	/* 0x2e */ int16 yMax;
	/* 0x30 */ uint16 macStyle;
	/* 0x32 */ uint16 lowestRecPPEM;
	/* 0x34 */ int16 fontDirectionHint;
	/* 0x36 */ int16 indexToLocFormat;
	/* 0x38 */ int16 glyphDataFormat;
} headClass;

typedef struct { // 0x28
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int32 version;
	/* 0x08 */ int16 Ascender;
	/* 0x0a */ int16 Descender;
	/* 0x0c */ int16 LineGap;
	/* 0x0e */ uint16 advanceWidthMax;
	/* 0x10 */ int16 minLeftSideBearing;
	/* 0x12 */ int16 minRightSideBearing;
	/* 0x14 */ int16 xMaxExtent;
	/* 0x16 */ int16 caretSlopeRise;
	/* 0x18 */ int16 caretSlopeRun;
	/* 0x1a */ int16 caretOffset;
	/* 0x1c */ int16 reserved2;
	/* 0x1e */ int16 reserved3;
	/* 0x20 */ int16 reserved4;
	/* 0x22 */ int16 reserved5;
	/* 0x24 */ int16 metricDataFormat;
	/* 0x26 */ uint16 numberOfHMetrics;
} hheaClass;

typedef struct { // 0x14
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int32 numGlyphs;
	/* 0x08 */ int32 numberOfHMetrics;
	/* 0x0c */ int16 *lsb;
	/* 0x10 */ uint16 *aw;
} hmtxClass;

typedef struct { // 0x18
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int32 version;
	/* 0x08 */ int32 N;
	/* 0x0c */ uint16 defaultWidth;
	/* 0x0e */ uint16 reserved;
	/* 0x10 */ uint16 *gIndex;
	/* 0x14 */ uint16 *aw;
} ffhmClass;

typedef struct { // 0x24
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int32 version;
	/* 0x08 */ uint16 numGlyphs;
	/* 0x0a */ uint16 maxPoints;
	/* 0x0c */ uint16 maxContours;
	/* 0x0e */ uint16 maxCompositePoints;
	/* 0x10 */ uint16 maxCompositeContours;
	/* 0x12 */ uint16 maxElements;
	/* 0x14 */ uint16 maxTwilightPoints;
	/* 0x16 */ uint16 maxStorage;
	/* 0x18 */ uint16 maxFunctionDefs;
	/* 0x1a */ uint16 maxInstructionDefs;
	/* 0x1c */ uint16 maxStackElements;
	/* 0x1e */ uint16 maxSizeOfInstructions;
	/* 0x20 */ uint16 maxComponentElements;
	/* 0x22 */ uint16 maxComponentDepth;
} maxpClass;

typedef struct { // 0x10
	/* 0x0 */ tsiMemObject *mem;
	/* 0x4 */ uint32 *offsets;
	/* 0x8 */ int n;
	/* 0xc */ short int indexToLocFormat;
} locaClass;

typedef struct { // 0x8
	/* 0x0 */ uint16 gIndex;
	/* 0x2 */ uint16 delta;
	/* 0x4 */ int32 offset;
} T2K_sloc_entry;

typedef struct { // 0x18
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ uint32 version;
	/* 0x08 */ uint16 num_sloc_entries;
	/* 0x0c */ T2K_sloc_entry *sloc;
	/* 0x10 */ uint32 numCorrections;
	/* 0x14 */ uint32 correctionOffset;
} slocClass;

typedef struct { // 0x14
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int16 majorVersion;
	/* 0x06 */ int16 minorVersion;
	/* 0x08 */ int16 numAxes;
	/* 0x0a */ int16 minRadius;
	/* 0x0c */ int16 maxRadius;
	/* 0x0e */ int16 gIndexFirstRoman;
	/* 0x10 */ int16 gIndexLastRoman;
} ffstClass;

typedef struct { // 0x78
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ short int contourCountMax;
	/* 0x08 */ int pointCountMax;
	/* 0x0c */ int colorPlaneCount;
	/* 0x10 */ int colorPlaneCountMax;
	/* 0x14 */ short int ctrBuffer[16];
	/* 0x34 */ short int curveType;
	/* 0x36 */ short int contourCount;
	/* 0x38 */ short int pointCount;
	/* 0x3c */ int16 *sp;
	/* 0x40 */ int16 *ep;
	/* 0x44 */ int16 *oox;
	/* 0x48 */ int16 *ooy;
	/* 0x4c */ uint8 *onCurve;
	/* 0x50 */ F26Dot6 *x;
	/* 0x54 */ F26Dot6 *y;
	/* 0x58 */ short int *componentData;
	/* 0x5c */ int componentSize;
	/* 0x60 */ int componentSizeMax;
	/* 0x64 */ uint8 *hintFragment;
	/* 0x68 */ int hintLength;
	/* 0x6c */ short int xmin;
	/* 0x6e */ short int ymin;
	/* 0x70 */ short int xmax;
	/* 0x72 */ short int ymax;
	/* 0x74 */ char dropOutControl;
	/* 0x76 */ uint16 myGlyphIndex;
} GlyphClass;

typedef short int ShortFrac;
typedef void (*voidFunc)(/* parameters unknown */);
typedef void (*fs_FuncType)(/* parameters unknown */);
typedef int16 LoopCount;
typedef int32 ArrayIndex;

typedef struct { // 0x4
	/* 0x0 */ ShortFrac x;
	/* 0x2 */ ShortFrac y;
} VECTOR;

typedef struct { // 0x2c
	/* 0x00 */ F26Dot6 *x;
	/* 0x04 */ F26Dot6 *y;
	/* 0x08 */ F26Dot6 *ox;
	/* 0x0c */ F26Dot6 *oy;
	/* 0x10 */ int16 *oox;
	/* 0x14 */ int16 *ooy;
	/* 0x18 */ uint8 *onCurve;
	/* 0x1c */ int16 nc;
	/* 0x1e */ int16 padWord;
	/* 0x20 */ int16 *sp;
	/* 0x24 */ int16 *ep;
	/* 0x28 */ uint8 *f;
} fnt_ElementType;

typedef struct { // 0x8
	/* 0x0 */ int32 start;
	/* 0x4 */ uint16 length;
	/* 0x6 */ uint16 pgmIndex;
} fnt_funcDef;

typedef struct { // 0x8
	/* 0x0 */ int32 start;
	/* 0x4 */ uint16 length;
	/* 0x6 */ uint8 pgmIndex;
	/* 0x7 */ uint8 opCode;
} fnt_instrDef;

typedef struct { // 0x8
	/* 0x0 */ Fract x;
	/* 0x4 */ Fract y;
} fnt_FractPoint;

typedef fnt_LocalGraphicStateType fnt_LocalGraphicStateType_t;
typedef fnt_GlobalGraphicStateType fnt_GlobalGraphicStateType_t;
typedef void (*FntFunc)(/* parameters unknown */);
typedef void (*InterpreterFunc)(/* parameters unknown */);
typedef void (*FntMoveFunc)(/* parameters unknown */);
typedef F26Dot6 (*FntRoundFunc)(/* parameters unknown */);
typedef F26Dot6 (*FntProjFunc)(/* parameters unknown */);
typedef F26Dot6 (*GetCVTEFunc)(/* parameters unknown */);
typedef F26Dot6 (*GetSWidFunc)(/* parameters unknown */);
typedef void (*TraceFuncType)(/* parameters unknown */);
typedef F26Dot6 (*ScaleFuncType)(/* parameters unknown */);

typedef struct { // 0x34
	/* 0x00 */ F26Dot6 wTCI;
	/* 0x04 */ F26Dot6 sWCI;
	/* 0x08 */ F26Dot6 scaledSW;
	/* 0x0c */ int32 scanControl;
	/* 0x10 */ int32 instructControl;
	/* 0x14 */ F26Dot6 minimumDistance;
	/* 0x18 */ FntRoundFunc RoundValue;
	/* 0x1c */ F26Dot6 periodMask;
	/* 0x20 */ Fract period45;
	/* 0x24 */ int16 period;
	/* 0x26 */ int16 phase;
	/* 0x28 */ int16 threshold;
	/* 0x2a */ int16 deltaBase;
	/* 0x2c */ int16 deltaShift;
	/* 0x2e */ int16 angleWeight;
	/* 0x30 */ int16 sW;
	/* 0x32 */ int8 autoFlip;
	/* 0x33 */ int8 pad;
} fnt_ParameterBlock;

typedef enum {
	PREPROGRAM = 0,
	FONTPROGRAM = 1,
	MAXPREPROGRAMS = 2
} fnt_ProgramIndex;

typedef struct { // 0xd4
	/* 0x00 */ F26Dot6 *stackBase;
	/* 0x04 */ F26Dot6 *store;
	/* 0x08 */ F26Dot6 *controlValueTable;
	/* 0x0c */ uint16 pixelsPerEm;
	/* 0x0e */ uint16 pointSize;
	/* 0x10 */ Fixed fpem;
	/* 0x14 */ F26Dot6 engine[4];
	/* 0x24 */ fnt_ParameterBlock defaultParBlock;
	/* 0x58 */ fnt_ParameterBlock localParBlock;
	/* 0x8c */ FntFunc *function;
	/* 0x90 */ fnt_funcDef *funcDef;
	/* 0x94 */ fnt_instrDef *instrDef;
	/* 0x98 */ ScaleFuncType ScaleFunc;
	/* 0x9c */ uint8 *pgmList[2];
	/* 0xa4 */ Fixed fixedScale;
	/* 0xa8 */ int32 nScale;
	/* 0xac */ int32 dScale;
	/* 0xb0 */ int32 dScaleDiv2;
	/* 0xb4 */ int16 dShift;
	/* 0xb6 */ int8 identityTransformation;
	/* 0xb7 */ int8 non90DegreeTransformation;
	/* 0xb8 */ Fixed xStretch;
	/* 0xbc */ Fixed yStretch;
	/* 0xc0 */ fnt_FractPoint *anglePoint;
	/* 0xc4 */ int16 *angleDistance;
	/* 0xc8 */ int8 init;
	/* 0xc9 */ uint8 pgmIndex;
	/* 0xca */ LoopCount instrDefCount;
	/* 0xcc */ int cvtCount;
	/* 0xd0 */ maxpClass *maxp;
} fnt_GlobalGraphicStateType;

typedef struct { // 0x80
	/* 0x00 */ uint8 *insPtr;
	/* 0x04 */ int opCode;
	/* 0x08 */ fnt_ElementType *CE0;
	/* 0x0c */ fnt_ElementType *CE1;
	/* 0x10 */ fnt_ElementType *CE2;
	/* 0x14 */ VECTOR proj;
	/* 0x18 */ VECTOR free;
	/* 0x1c */ VECTOR oldProj;
	/* 0x20 */ F26Dot6 *stackPointer;
	/* 0x24 */ fnt_ElementType *elements;
	/* 0x28 */ fnt_GlobalGraphicStateType *globalGS;
	/* 0x2c */ TraceFuncType TraceFunc;
	/* 0x30 */ ArrayIndex Pt0;
	/* 0x34 */ ArrayIndex Pt1;
	/* 0x38 */ ArrayIndex Pt2;
	/* 0x3c */ int16 roundToGrid;
	/* 0x3e */ LoopCount loop;
	/* 0x40 */ ShortFrac pfProj;
	/* 0x44 */ FntMoveFunc MovePoint;
	/* 0x48 */ FntProjFunc Project;
	/* 0x4c */ FntProjFunc OldProject;
	/* 0x50 */ InterpreterFunc Interpreter;
	/* 0x54 */ GetCVTEFunc GetCVTEntry;
	/* 0x58 */ GetSWidFunc GetSingleWidth;
	/* 0x5c */ jmp_buf env;
} fnt_LocalGraphicStateType;

typedef struct { // 0x154
	/* 0x000 */ tsiMemObject *mem;
	/* 0x004 */ int xPixelsPerEm;
	/* 0x008 */ int yPixelsPerEm;
	/* 0x00c */ int16 *ocvt;
	/* 0x010 */ int numCVTs;
	/* 0x014 */ F26Dot6 *ptr32;
	/* 0x018 */ fnt_GlobalGraphicStateType globalGS;
	/* 0x0ec */ uint16 UPEM;
	/* 0x0ee */ int16 maxTwilightPoints;
	/* 0x0f0 */ uint32 pgmLength[2];
	/* 0x0f8 */ int16 spZeroWord;
	/* 0x0fa */ int16 epZeroWord;
	/* 0x0fc */ fnt_ElementType elements[2];
} T2KTTClass;

typedef void (*StyleFuncPtr)(/* parameters unknown */);
typedef void (*StyleMetricsFuncPtr)(/* parameters unknown */);

typedef struct { // 0x14
	/* 0x00 */ int16 nScale;
	/* 0x02 */ int16 dShift;
	/* 0x04 */ int32 dScale;
	/* 0x08 */ int32 dScaleDiv2;
	/* 0x0c */ F16Dot16 fixedScale;
	/* 0x10 */ int16 scaleType;
} T2KScaleInfo;

typedef uint16 (*FF_GetAWFuncPtr)(/* parameters unknown */);

typedef struct { // 0xd8
	/* 0x00 */ sfnt_OffsetTable *offsetTable0;
	/* 0x04 */ FF_GetAWFuncPtr GetAWFuncPtr1;
	/* 0x08 */ void *GetAWParam1;
	/* 0x0c */ FF_GetAWFuncPtr GetAWFuncPtr2;
	/* 0x10 */ void *GetAWParam2;
	/* 0x14 */ uint16 *awCache_hashKey;
	/* 0x18 */ uint16 *awCache_aw;
	/* 0x1c */ uint16 upem;
	/* 0x20 */ T2KTTClass *t2kTT;
	/* 0x24 */ ttcfClass *ttcf;
	/* 0x28 */ headClass *head;
	/* 0x2c */ maxpClass *maxp;
	/* 0x30 */ locaClass *loca;
	/* 0x34 */ slocClass *sloc;
	/* 0x38 */ ffstClass *ffst;
	/* 0x3c */ ffhmClass *ffhm;
	/* 0x40 */ gaspClass *gasp;
	/* 0x44 */ hheaClass *hhea;
	/* 0x48 */ hheaClass *vhea;
	/* 0x4c */ hmtxClass *hmtx;
	/* 0x50 */ hmtxClass *vmtx;
	/* 0x54 */ cmapClass *cmap;
	/* 0x58 */ kernClass *kern;
	/* 0x5c */ CFF_Class CFF;
	/* 0x64 */ DSIGClass DSIG;
	/* 0x6c */ uint16 preferedPlatformID;
	/* 0x6e */ uint16 preferedPlatformSpecificID;
	/* 0x70 */ int16 post_underlinePosition;
	/* 0x72 */ int16 post_underlineThickness;
	/* 0x74 */ uint32 isFixedPitch;
	/* 0x78 */ uint16 firstCharCode;
	/* 0x7a */ uint16 lastCharCode;
	/* 0x7c */ int xPPEm;
	/* 0x80 */ int yPPEm;
	/* 0x84 */ void *globalHintsCache;
	/* 0x88 */ StyleFuncPtr StyleFunc;
	/* 0x8c */ StyleMetricsFuncPtr StyleMetricsFunc;
	/* 0x90 */ F16Dot16 params[4];
	/* 0xa0 */ F16Dot16 xScale;
	/* 0xa4 */ F16Dot16 yScale;
	/* 0xa8 */ int useNativeHints;
	/* 0xac */ int strokeGlyph;
	/* 0xb0 */ int greyScaleLevel;
	/* 0xb4 */ F16Dot16 currentCoordinate[2];
	/* 0xbc */ InputStream *in;
	/* 0xc0 */ InputStream *in2;
	/* 0xc4 */ OutputStream *out;
	/* 0xc8 */ tsiMemObject *mem;
	/* 0xcc */ void *model;
	/* 0xd0 */ int numGlyphs;
	/* 0xd4 */ int numberOfLogicalFonts;
} sfntClass;

typedef struct { // 0x18
	/* 0x00 */ StyleFuncPtr StyleFunc;
	/* 0x04 */ StyleMetricsFuncPtr StyleMetricsFunc;
	/* 0x08 */ F16Dot16 params[4];
} T2K_AlgStyleDescriptor;

typedef struct { // 0x18
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int xPPEm;
	/* 0x08 */ int yPPEm;
	/* 0x0c */ int length;
	/* 0x10 */ uint8 *noNeedForXDropout;
	/* 0x14 */ uint8 *noNeedForYDropout;
} dropoutAdaptationClass;

typedef struct { // 0x10
	/* 0x0 */ F16Dot16 t00;
	/* 0x4 */ F16Dot16 t01;
	/* 0x8 */ F16Dot16 t10;
	/* 0xc */ F16Dot16 t11;
} T2K_TRANS_MATRIX;

typedef void* (*FF_FM_RenderGlyphPtr)(/* parameters unknown */);
typedef void* (*FF_T2K_FilterFuncPtr)(/* parameters unknown */);

typedef struct { // 0x508
	/* 0x000 */ int stamp1;
	/* 0x004 */ tsiMemObject *mem;
	/* 0x008 */ F16Dot16 t00;
	/* 0x00c */ F16Dot16 t01;
	/* 0x010 */ F16Dot16 t10;
	/* 0x014 */ F16Dot16 t11;
	/* 0x018 */ dropoutAdaptationClass *tAdapt;
	/* 0x01c */ int is_Identity;
	/* 0x020 */ void *theCache;
	/* 0x024 */ FF_GetCacheMemoryPtr GetCacheMemory;
	/* 0x028 */ int okForBitCreationToTalkToCache;
	/* 0x02c */ int internal_baseAddr;
	/* 0x030 */ int internal_baseARGB;
	/* 0x034 */ void *theFM;
	/* 0x038 */ FF_FM_RenderGlyphPtr FMRenderGlyph;
	/* 0x03c */ FF_T2K_FilterFuncPtr BitmapFilter;
	/* 0x040 */ void *filterParamsPtr;
	/* 0x044 */ int bitRange255;
	/* 0x048 */ uint8 *remapBits;
	/* 0x04c */ uint8 *nameString8;
	/* 0x050 */ uint16 *nameString16;
	/* 0x054 */ int numberOfLogicalFonts;
	/* 0x058 */ uint32 isFixedPitch;
	/* 0x05c */ uint16 firstCharCode;
	/* 0x05e */ uint16 lastCharCode;
	/* 0x060 */ int horizontalFontMetricsAreValid;
	/* 0x064 */ F16Dot16 xAscender;
	/* 0x068 */ F16Dot16 yAscender;
	/* 0x06c */ F16Dot16 xDescender;
	/* 0x070 */ F16Dot16 yDescender;
	/* 0x074 */ F16Dot16 xLineGap;
	/* 0x078 */ F16Dot16 yLineGap;
	/* 0x07c */ F16Dot16 xMaxLinearAdvanceWidth;
	/* 0x080 */ F16Dot16 yMaxLinearAdvanceWidth;
	/* 0x084 */ F16Dot16 caretDx;
	/* 0x088 */ F16Dot16 caretDy;
	/* 0x08c */ F16Dot16 xUnderlinePosition;
	/* 0x090 */ F16Dot16 yUnderlinePosition;
	/* 0x094 */ F16Dot16 xUnderlineThickness;
	/* 0x098 */ F16Dot16 yUnderlineThickness;
	/* 0x09c */ int verticalFontMetricsAreValid;
	/* 0x0a0 */ F16Dot16 vert_xAscender;
	/* 0x0a4 */ F16Dot16 vert_yAscender;
	/* 0x0a8 */ F16Dot16 vert_xDescender;
	/* 0x0ac */ F16Dot16 vert_yDescender;
	/* 0x0b0 */ F16Dot16 vert_xLineGap;
	/* 0x0b4 */ F16Dot16 vert_yLineGap;
	/* 0x0b8 */ F16Dot16 vert_xMaxLinearAdvanceWidth;
	/* 0x0bc */ F16Dot16 vert_yMaxLinearAdvanceWidth;
	/* 0x0c0 */ F16Dot16 vert_caretDx;
	/* 0x0c4 */ F16Dot16 vert_caretDy;
	/* 0x0c8 */ int horizontalMetricsAreValid;
	/* 0x0cc */ F16Dot16 xAdvanceWidth16Dot16;
	/* 0x0d0 */ F16Dot16 yAdvanceWidth16Dot16;
	/* 0x0d4 */ F16Dot16 xLinearAdvanceWidth16Dot16;
	/* 0x0d8 */ F16Dot16 yLinearAdvanceWidth16Dot16;
	/* 0x0dc */ F26Dot6 fTop26Dot6;
	/* 0x0e0 */ F26Dot6 fLeft26Dot6;
	/* 0x0e4 */ int verticalMetricsAreValid;
	/* 0x0e8 */ F16Dot16 vert_xAdvanceWidth16Dot16;
	/* 0x0ec */ F16Dot16 vert_yAdvanceWidth16Dot16;
	/* 0x0f0 */ F16Dot16 vert_xLinearAdvanceWidth16Dot16;
	/* 0x0f4 */ F16Dot16 vert_yLinearAdvanceWidth16Dot16;
	/* 0x0f8 */ F26Dot6 vert_fTop26Dot6;
	/* 0x0fc */ F26Dot6 vert_fLeft26Dot6;
	/* 0x100 */ GlyphClass *glyph;
	/* 0x104 */ uint16 glyphIndex;
	/* 0x108 */ int width;
	/* 0x10c */ int height;
	/* 0x110 */ int rowBytes;
	/* 0x114 */ unsigned char *baseAddr;
	/* 0x118 */ uint32 *baseARGB;
	/* 0x11c */ int embeddedBitmapWasUsed;
	/* 0x120 */ int xPixelsPerEm;
	/* 0x124 */ int yPixelsPerEm;
	/* 0x128 */ F16Dot16 xPixelsPerEm16Dot16;
	/* 0x12c */ F16Dot16 yPixelsPerEm16Dot16;
	/* 0x130 */ F16Dot16 xMul;
	/* 0x134 */ F16Dot16 yMul;
	/* 0x138 */ T2KScaleInfo xScale;
	/* 0x14c */ T2KScaleInfo yScale;
	/* 0x160 */ F26Dot6 oneHalfFUnit;
	/* 0x164 */ int ag_xPixelsPerEm;
	/* 0x168 */ int ag_yPixelsPerEm;
	/* 0x16c */ char xWeightIsOne;
	/* 0x170 */ int fontCategory;
	/* 0x174 */ int enableSbits;
	/* 0x178 */ uint16 upem;
	/* 0x17c */ sfntClass *font;
	/* 0x180 */ void *hintHandle;
	/* 0x184 */ uint32 tag[149];
	/* 0x3d8 */ int16 kernAndAdvanceWidth[149];
	/* 0x504 */ int stamp2;
} T2K;

typedef struct { // 0x24
	/* 0x00 */ uint16 charCode;
	/* 0x02 */ uint16 glyphIndex;
	/* 0x04 */ F16Dot16 AdvanceWidth16Dot16[2];
	/* 0x0c */ F16Dot16 LinearAdvanceWidth16Dot16[2];
	/* 0x14 */ F26Dot6 Corner[2];
	/* 0x1c */ int Dimension[2];
} T2KCharInfo;

typedef struct { // 0x8
	/* 0x0 */ F16Dot16 BestAdvanceWidth16Dot16[2];
} T2KLayout;

typedef struct { // 0x3c
	/* 0x00 */ uint16 logFontIndex;
	/* 0x02 */ uint16 fontCode;
	/* 0x04 */ T2K_TRANS_MATRIX transM;
	/* 0x14 */ T2K_AlgStyleDescriptor styleDesc;
	/* 0x2c */ int nSubScalers;
	/* 0x30 */ T2K **subScalers;
	/* 0x34 */ sfntClass **subFonts;
	/* 0x38 */ tsiMemObject **subMems;
} NewFontEntry;

typedef struct { // 0xc
	/* 0x0 */ short int fontType;
	/* 0x4 */ InputStream *InputStreamA;
	/* 0x8 */ InputStream *InputStreamB;
} InputStreamPack;

typedef struct { // 0xc4
	/* 0x00 */ uint16 StreamIndex;
	/* 0x02 */ uint16 SubIndex;
	/* 0x04 */ char SubFontName8[64];
	/* 0x44 */ uint16 SubFontName16[64];
} PhysicalFontEntry;

typedef struct { // 0x8
	/* 0x0 */ int nFragments;
	/* 0x4 */ uint16 *fragmentList;
} LogicalFontEntry;

typedef struct { // 0x1e3c
	/* 0x0000 */ uint16 nStreams;
	/* 0x0004 */ InputStreamPack *inputStreamList;
	/* 0x0008 */ uint16 nPhysicalFonts;
	/* 0x000c */ PhysicalFontEntry *physicalFontList;
	/* 0x0010 */ uint16 nLogicalFonts;
	/* 0x0014 */ LogicalFontEntry *logicalFontList;
	/* 0x0018 */ uint16 nextNewFontKey;
	/* 0x001a */ uint16 nNewFonts;
	/* 0x001c */ NewFontEntry newFontList[128];
	/* 0x1e1c */ uint16 curFontCode;
	/* 0x1e20 */ T2K *theFMScaler;
	/* 0x1e24 */ sfntClass *theFMFont;
	/* 0x1e28 */ uint16 platformID;
	/* 0x1e2a */ uint16 platformSpecificID;
	/* 0x1e2c */ uint16 languageID;
	/* 0x1e2e */ uint16 nameID;
	/* 0x1e30 */ int xRes;
	/* 0x1e34 */ int yRes;
	/* 0x1e38 */ tsiMemObject *mem;
} FF_FM_Class;

typedef void* (*FF_FilterSizeFunc)(/* parameters unknown */);
typedef void* (*FF_FilterFunc)(/* parameters unknown */);

struct cacheSpecs_tag { // 0x4c
	/* 0x00 */ int horizontalMetricsAreValid;
	/* 0x04 */ F16Dot16 xAdvanceWidth16Dot16;
	/* 0x08 */ F16Dot16 yAdvanceWidth16Dot16;
	/* 0x0c */ F16Dot16 xLinearAdvanceWidth16Dot16;
	/* 0x10 */ F16Dot16 yLinearAdvanceWidth16Dot16;
	/* 0x14 */ F26Dot6 fTop26Dot6;
	/* 0x18 */ F26Dot6 fLeft26Dot6;
	/* 0x1c */ int verticalMetricsAreValid;
	/* 0x20 */ F16Dot16 vert_xAdvanceWidth16Dot16;
	/* 0x24 */ F16Dot16 vert_yAdvanceWidth16Dot16;
	/* 0x28 */ F16Dot16 vert_xLinearAdvanceWidth16Dot16;
	/* 0x2c */ F16Dot16 vert_yLinearAdvanceWidth16Dot16;
	/* 0x30 */ F26Dot6 vert_fTop26Dot6;
	/* 0x34 */ F26Dot6 vert_fLeft26Dot6;
	/* 0x38 */ int width;
	/* 0x3c */ int height;
	/* 0x40 */ int rowBytes;
	/* 0x44 */ int embeddedBitmapWasUsed;
	/* 0x48 */ int longbit;
};

typedef cacheSpecs_tag cacheSpecs_t;

typedef struct { // 0x60
	/* 0x00 */ int len;
	/* 0x04 */ chardata_hdr *next;
	/* 0x08 */ chardata_hdr *prev;
	/* 0x0c */ chardata_hdr *lruprev;
	/* 0x10 */ chardata_hdr *lrunext;
	/* 0x14 */ cacheSpecs_t cacheSpecs;
} chardata_hdr;

struct memory_seg { // 0xc
	/* 0x0 */ int len;
	/* 0x4 */ memory_seg *next;
	/* 0x8 */ memory_seg *prev;
};

typedef memory_seg memory_hdr;

struct char_desc_tag { // 0x8
	/* 0x0 */ uint16 fontCode;
	/* 0x2 */ uint16 charCode;
	/* 0x4 */ uint32 instCode;
};

typedef char_desc_tag char_desc_t;

typedef struct { // 0x34
	/* 0x00 */ int cacheSize;
	/* 0x04 */ uint8 *imagedata;
	/* 0x08 */ chardata_hdr *current_char;
	/* 0x0c */ chardata_hdr **hashtable;
	/* 0x10 */ memory_hdr *freelist;
	/* 0x14 */ chardata_hdr *lruhead;
	/* 0x18 */ chardata_hdr *lrutail;
	/* 0x1c */ uint16 FilterTag;
	/* 0x20 */ FF_FilterSizeFunc SetFilterSizeFunc;
	/* 0x24 */ FF_FilterFunc SetFilterFunc;
	/* 0x28 */ tsiMemObject *mem;
	/* 0x2c */ FF_T2K_FilterFuncPtr BitmapFilter;
	/* 0x30 */ void *filterParamsPtr;
} FF_CM_Class;

typedef int ag_FontCategory;

typedef struct { // 0x6
	/* 0x0 */ short int flat;
	/* 0x2 */ short int round;
	/* 0x4 */ short int overLap;
} ag_HeightType;

typedef struct { // 0x6c
	/* 0x00 */ ag_HeightType heights[10];
	/* 0x3c */ short int xWeight[12];
	/* 0x54 */ short int yWeight[12];
} ag_GlobalDataType;

typedef struct { // 0xe
	/* 0x0 */ short unsigned int maxZones;
	/* 0x2 */ short unsigned int maxTwilightPoints;
	/* 0x4 */ short unsigned int maxStorage;
	/* 0x6 */ short unsigned int maxFunctionDefs;
	/* 0x8 */ short unsigned int maxInstructionDefs;
	/* 0xa */ short unsigned int maxStackElements;
	/* 0xc */ short unsigned int maxSizeOfInstructions;
} ag_HintMaxInfoType;

typedef struct { // 0x28
	/* 0x00 */ short int contourCount;
	/* 0x02 */ short int pointCount;
	/* 0x04 */ short int *sp;
	/* 0x08 */ short int *ep;
	/* 0x0c */ short int *oox;
	/* 0x10 */ short int *ooy;
	/* 0x14 */ unsigned char *onCurve;
	/* 0x18 */ int *x;
	/* 0x1c */ int *y;
	/* 0x20 */ int advanceWidth26Dot6;
	/* 0x24 */ int advanceWidthInt;
} ag_ElementType;

typedef void *ag_HintHandleType;

typedef struct { // 0x8
	/* 0x0 */ int32 coordinate25Dot6_flag1;
	/* 0x4 */ void *next;
} T2KInterSectType;

typedef struct { // 0x2230
	/* 0x0000 */ int left;
	/* 0x0004 */ int right;
	/* 0x0008 */ int top;
	/* 0x000c */ int bottom;
	/* 0x0010 */ F26Dot6 fTop26Dot6;
	/* 0x0014 */ F26Dot6 fLeft26Dot6;
	/* 0x0018 */ int rowBytes;
	/* 0x001c */ unsigned char *baseAddr;
	/* 0x0020 */ int internal_baseAddr;
	/* 0x0024 */ int scanBBoxIsComputed;
	/* 0x0028 */ int xminSC;
	/* 0x002c */ int xmaxSC;
	/* 0x0030 */ int yminSC;
	/* 0x0034 */ int ymaxSC;
	/* 0x0038 */ int outlineXMin;
	/* 0x003c */ int outlineXMax;
	/* 0x0040 */ int outlineYMin;
	/* 0x0044 */ int outlineYMax;
	/* 0x0048 */ int couldOverflowIntegerMath;
	/* 0x004c */ T2KInterSectType *yBaseBuffer[32];
	/* 0x00cc */ T2KInterSectType **yEdgeHead;
	/* 0x00d0 */ T2KInterSectType **yBase;
	/* 0x00d4 */ int minYIndex;
	/* 0x00d8 */ int maxYIndex;
	/* 0x00dc */ T2KInterSectType *xBaseBuffer[32];
	/* 0x015c */ T2KInterSectType **xEdgeHead;
	/* 0x0160 */ T2KInterSectType **xBase;
	/* 0x0164 */ int minXIndex;
	/* 0x0168 */ int maxXIndex;
	/* 0x016c */ T2KInterSectType *free;
	/* 0x0170 */ T2KInterSectType *freeEnd;
	/* 0x0174 */ T2KInterSectType firstMemBlockBuffer[1024];
	/* 0x2174 */ T2KInterSectType *freeMemBlocksBuffer[32];
	/* 0x21f4 */ T2KInterSectType **freeMemBlocks;
	/* 0x21f8 */ int freeMemBlockMaxCount;
	/* 0x21fc */ int freeMemBlockN;
	/* 0x2200 */ int maxError;
	/* 0x2204 */ uint8 greyScaleLevel;
	/* 0x2205 */ char xDropOutControl;
	/* 0x2206 */ char yDropOutControl;
	/* 0x2207 */ char includeStubs;
	/* 0x2208 */ char smartDropout;
	/* 0x2209 */ char doXEdges;
	/* 0x220c */ int weDidXDropouts;
	/* 0x2210 */ int weDidYDropouts;
	/* 0x2214 */ short int *startPoint;
	/* 0x2218 */ short int *endPoint;
	/* 0x221c */ short int numberOfContours;
	/* 0x2220 */ int *x;
	/* 0x2224 */ int *y;
	/* 0x2228 */ char *onCurve;
	/* 0x222c */ tsiMemObject *mem;
} tsiScanConv;

typedef struct { // 0x50
	/* 0x00 */ unsigned char *baseAddr;
	/* 0x04 */ int internal_baseAddr;
	/* 0x08 */ int left;
	/* 0x0c */ int right;
	/* 0x10 */ int top;
	/* 0x14 */ int bottom;
	/* 0x18 */ F26Dot6 fTop26Dot6;
	/* 0x1c */ F26Dot6 fLeft26Dot6;
	/* 0x20 */ int rowBytes;
	/* 0x24 */ tsiMemObject *mem;
	/* 0x28 */ short int *startPoint;
	/* 0x2c */ short int *endPoint;
	/* 0x30 */ short int numberOfContours;
	/* 0x34 */ int *x;
	/* 0x38 */ int *y;
	/* 0x3c */ char *onCurve;
	/* 0x40 */ int xmin;
	/* 0x44 */ int xmax;
	/* 0x48 */ int ymin;
	/* 0x4c */ int ymax;
} ffStrkConv;

typedef struct { // 0x18
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ unsigned char *numBitsUsed;
	/* 0x08 */ unsigned int numEntries;
	/* 0x0c */ unsigned int maxBits;
	/* 0x10 */ unsigned char *LookUpSymbol;
	/* 0x14 */ short unsigned int *LookUpBits;
} SCODER;

typedef struct { // 0x30
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int OrionState;
	/* 0x08 */ int num_eb1;
	/* 0x0c */ int num_e;
	/* 0x10 */ int num_eb2;
	/* 0x14 */ short int *dx;
	/* 0x18 */ short int *dy;
	/* 0x1c */ char *onCurve;
	/* 0x20 */ SCODER **copy;
	/* 0x24 */ SCODER **literal;
	/* 0x28 */ SCODER *control;
	/* 0x2c */ SCODER *ep;
} OrionModelClass;

typedef struct { // 0x6
	/* 0x0 */ uint16 format;
	/* 0x2 */ uint16 length;
	/* 0x4 */ uint16 version;
} sfnt_mappingTable;

typedef struct { // 0x8
	/* 0x0 */ uint16 firstCode;
	/* 0x2 */ uint16 entryCount;
	/* 0x4 */ int16 idDelta;
	/* 0x6 */ uint16 idRangeOffset;
} sfnt_subHeader;

typedef struct { // 0x208
	/* 0x000 */ uint16 subHeadersKeys[256];
	/* 0x200 */ sfnt_subHeader subHeaders[1];
} sfnt_mappingTable2;

typedef struct { // 0xc
	/* 0x0 */ uint16 version;
	/* 0x2 */ uint16 numTables;
	/* 0x4 */ sfnt_platformEntry platform[1];
} sfnt_char2IndexDirectory;

typedef struct { // 0x206
	/* 0x000 */ uint16 format;
	/* 0x002 */ uint16 length;
	/* 0x004 */ uint16 version;
	/* 0x006 */ uint16 subHeaderKey[256];
} HighByteMappingTable;

typedef struct { // 0x8
	/* 0x0 */ uint16 startCount;
	/* 0x2 */ uint16 endCount;
	/* 0x4 */ int16 idDelta;
	/* 0x6 */ uint16 idRangeOffset;
} rangeCount;

typedef struct { // 0x8
	/* 0x0 */ uint16 firstCode;
	/* 0x2 */ uint16 entryCount;
	/* 0x4 */ int16 idDelta;
	/* 0x6 */ uint16 idRangeOffset;
} subHeaders;

typedef struct { // 0x10
	/* 0x0 */ uint16 format;
	/* 0x2 */ uint16 length;
	/* 0x4 */ uint16 version;
	/* 0x6 */ uint16 segCountX2;
	/* 0x8 */ uint16 searchRange;
	/* 0xa */ uint16 entrySelector;
	/* 0xc */ uint16 rangeShift;
	/* 0xe */ uint16 endCount;
} segMappingTable;

typedef struct { // 0xc
	/* 0x0 */ uint16 format;
	/* 0x2 */ uint16 length;
	/* 0x4 */ uint16 version;
	/* 0x6 */ uint16 firstCode;
	/* 0x8 */ uint16 entryCount;
	/* 0xa */ uint16 glyphIdArray[1];
} trimmedTableMapping;

typedef struct { // 0x8
	/* 0x0 */ unsigned char type;
	/* 0x1 */ unsigned char direction;
	/* 0x2 */ unsigned char forwardTo;
	/* 0x3 */ unsigned char priority;
	/* 0x4 */ short int from;
	/* 0x6 */ short int to;
} tp_LinkType;

typedef struct { // 0x3e4
	/* 0x000 */ unsigned int magic0xA5A0F5A5;
	/* 0x004 */ short int numberOfContours;
	/* 0x008 */ short int *startPoint;
	/* 0x00c */ short int *endPoint;
	/* 0x010 */ unsigned char *onCurve;
	/* 0x014 */ short int *oox;
	/* 0x018 */ short int *ooy;
	/* 0x01c */ short int isFigure;
	/* 0x020 */ int cvt[96];
	/* 0x1a0 */ short int ocvt[96];
	/* 0x260 */ short int unitsPerEm;
	/* 0x264 */ int xPixelsPerEm;
	/* 0x268 */ int yPixelsPerEm;
	/* 0x26c */ ag_FontCategory fontType;
	/* 0x270 */ int oy1;
	/* 0x274 */ int y1;
	/* 0x278 */ int y1Shift;
	/* 0x27c */ int oy2;
	/* 0x280 */ int y2;
	/* 0x284 */ int y2Shift;
	/* 0x288 */ int oy3;
	/* 0x28c */ int y3;
	/* 0x290 */ int y3Shift;
	/* 0x294 */ int oy4;
	/* 0x298 */ int y4;
	/* 0x29c */ int y4Shift;
	/* 0x2a0 */ short int ooy1;
	/* 0x2a2 */ short int ooy2;
	/* 0x2a4 */ short int ooy3;
	/* 0x2a6 */ short int ooy4;
	/* 0x2a8 */ int mul[5];
	/* 0x2bc */ int div[5];
	/* 0x2d0 */ int add[5];
	/* 0x2e4 */ int fmul[5];
	/* 0x2f8 */ int yBounds[5];
	/* 0x30c */ int fFastYAGDisabled;
	/* 0x310 */ int maxPointCount;
	/* 0x314 */ ag_GlobalDataType gData;
	/* 0x380 */ ag_HintMaxInfoType maxInfo;
	/* 0x390 */ int storage[16];
	/* 0x3d0 */ short int inX;
	/* 0x3d2 */ short int inY;
	/* 0x3d4 */ char cvtHasBeenSetUp;
	/* 0x3d5 */ char hintInfoHasBeenSetUp;
	/* 0x3d8 */ tsiMemObject *mem;
	/* 0x3dc */ int strat98;
	/* 0x3e0 */ unsigned int magic0x0FA55AF0;
} ag_DataType;

typedef struct { // 0x28
	/* 0x00 */ tsiMemObject *mem;
	/* 0x04 */ int leftCount;
	/* 0x08 */ int leftMaxCount;
	/* 0x0c */ short int *xyLeft;
	/* 0x10 */ int rightCount;
	/* 0x14 */ int rightMaxCount;
	/* 0x18 */ short int *xyRight;
	/* 0x1c */ int rot1Dx;
	/* 0x20 */ int rot1Dy;
	/* 0x24 */ int rot1IsForPoint;
} PathClass;

typedef void (*PF_ADD_POINT)(/* parameters unknown */);
// warning: multiple differing types with the same name (type name not equal)
typedef long unsigned int clock_t;
// warning: multiple differing types with the same name (type name not equal)
typedef long int time_t;
// warning: multiple differing types with the same name (type name not equal)
typedef long int _fpos_t;

union __dmath { // 0x8
	/* 0x0 */ __uint32_t i[2];
	/* 0x0 */ double d;
};

// warning: multiple differing types with the same name (size not equal)
struct exception { // 0x28
	/* 0x00 */ int type;
	/* 0x04 */ char *name;
	/* 0x08 */ double arg1;
	/* 0x10 */ double arg2;
	/* 0x18 */ double retval;
	/* 0x20 */ int err;
};

enum __fdlibm_version {
	__fdlibm_ieee = -1,
	__fdlibm_svid = 0,
	__fdlibm_xopen = 1,
	__fdlibm_posix = 2
};

typedef union { // 0x8
	/* 0x0 */ double value;
	/* 0x0 */ struct { // 0x8
		/* 0x0 */ __uint32_t lsw;
		/* 0x4 */ __uint32_t msw;
	} parts;
} ieee_double_shape_type;

typedef union { // 0x4
	/* 0x0 */ float value;
	/* 0x0 */ __uint32_t word;
} ieee_float_shape_type;

typedef struct { // 0x8
	/* 0x0 */ short int __delta;
	/* 0x2 */ short int __index;
	/* 0x4 */ void *__pfn;
	/* 0x4 */ short int __delta2;
} __vtbl_ptr_type;

struct TagValuePair { // 0xc
	/* 0x0 */ unsigned int mTag;
	/* 0x4 */ union { // 0x4
		/* 0x4 */ int mInt;
		/* 0x4 */ size_t mSize;
		/* 0x4 */ float mFloat;
		/* 0x4 */ void *mPointer;
	} mValue;
	/* 0x8 */ TagValuePair *mNext;

	TagValuePair& operator=();
	TagValuePair();
	TagValuePair();
	TagValuePair();
	TagValuePair();
	TagValuePair();
	TagValuePair& operator+();
};

enum AllocTvpTag {
	ATT_NULL = 0,
	ATT_NAME = 1,
	ATT_ALIGNMENT = 2,
	ATT_ALIGNMENT_OFFSET = 3,
	ATT_ALLOC_HIGH = 4,
	ATT_FILE = 5,
	ATT_LINE = 6,
	ATT_USER = 65536
};

struct IAllocator { // 0x4
	/* 0x0 */ __vtbl_ptr_type *$vf50;

	IAllocator& operator=();
	IAllocator();
	IAllocator();
	/* vtable[1] */ virtual void* Alloc();
	/* vtable[2] */ virtual void Free();
	/* vtable[3] */ virtual int AddRef();
	/* vtable[4] */ virtual int Release();
protected:
	/* vtable[5] */ virtual IAllocator(IAllocator*, int, void);
};

enum HintValue {
	HV_NONE = 0,
	HV_LOW = 1,
	HV_MEDIUM = 2,
	HV_HIGH = 3,
	HV_USER = 65536
};

enum QueryAllocatorTag {
	QAT_NULL = 0,
	QAT_REQUIRED_TAG = 16777216,
	QAT_ALIGNMENT = 16777217,
	QAT_ALIGNMENT_OFFSET = 16777218,
	QAT_HANDLE_ALLOC_TVP = 16777219,
	QAT_THREAD_SAFE = 16777220,
	QAT_OPTIONAL_TAG = 33554432,
	QAT_POOL_ID = 33554433,
	QAT_POOL_SIZE = 33554434,
	QAT_FIXED_BLOCK_SIZE = 33554435,
	QAT_NUM_FIXED_BLOCKS = 33554436,
	QAT_ALLOC_HIGH = 33554437,
	QAT_LIFESPAN = 33554438,
	QAT_FREQUENCY = 33554439,
	QAT_SIZE_COHERENCY = 33554440,
	QAT_USER_TAG = -2147483648,
	QAT_USER_REQUIRED_TAG = -2130706432,
	QAT_USER_OPTIONAL_TAG = -2113929216
};

enum QueryFlags {
	QF_RETURN_NULL_ON_ERROR = 0,
	QF_ASSERT_ON_ERROR = 1
};

struct IFactory { // 0x4
	/* 0x0 */ __vtbl_ptr_type *$vf64;

	IFactory& operator=();
	IFactory();
	IFactory();
	/* vtable[1] */ virtual IAllocator* QueryAllocator();
};

// warning: multiple differing types with the same name (type name not equal)
struct _Bigint { // 0x18
	/* 0x00 */ _Bigint *_next;
	/* 0x04 */ int _k;
	/* 0x08 */ int _maxwds;
	/* 0x0c */ int _sign;
	/* 0x10 */ int _wds;
	/* 0x14 */ unsigned int _x[1];
};

typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;
typedef short unsigned int uint16_t;
typedef unsigned int uint32_t;
typedef float float_t;
typedef double double_t;
typedef long int int64_t;
typedef long unsigned int uint64_t;
typedef bool bool8_t;
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
typedef int64_t intmax_t;
typedef uint64_t uintmax_t;
typedef char char8_t;
typedef __wchar_t char16_t;
typedef uint32_t char32_t;
typedef int (*ThreadEntryFunc)(/* parameters unknown */);

enum Priority {
	IDLE_PRIORITY = -3,
	LOW_PRIORITY = -2,
	BELOW_PRIORITY = -1,
	NORM_PRIORITY = 0,
	ABOVE_PRIORITY = 1,
	HIGH_PRIORITY = 2,
	CRIT_PRIORITY = 3
};

struct IThread { // 0x4
	/* 0x0 */ __vtbl_ptr_type *$vf191;

	IThread& operator=();
	IThread();
	IThread();
	/* vtable[1] */ virtual IThread* CreateInstance();
	/* vtable[2] */ virtual int AddRef();
	/* vtable[3] */ virtual int Release();
	/* vtable[4] */ virtual void SetStackSize();
	/* vtable[5] */ virtual void Begin();
	/* vtable[6] */ virtual void WaitForEnd();
	/* vtable[7] */ virtual void Sleep();
	/* vtable[8] */ virtual void SetPriority();
};

struct IMutex { // 0x4
	/* 0x0 */ __vtbl_ptr_type *$vf203;

	IMutex& operator=();
	IMutex();
	IMutex();
	/* vtable[1] */ virtual IMutex* CreateInstance();
	/* vtable[2] */ virtual int AddRef();
	/* vtable[3] */ virtual int Release();
	/* vtable[4] */ virtual void Lock();
	/* vtable[5] */ virtual void Unlock();
};

typedef char DirNameType;

enum SeekFrom {
	SF_SET = 0,
	SF_CUR = 1,
	SF_END = 2
};

enum FileOpenMode {
	FOM_UNKNOWN = 0,
	FOM_READ = 1,
	FOM_WRITE = 2,
	FOM_READ_WRITE = 3,
	FOM_CREATE = 512
};

enum McResult {
	MCR_UNKNOWN = 0,
	MCR_SUCCESS = 1,
	MCR_FAILED = 2,
	MCR_BUSY = 3
};

enum FileAttribute {
	FA_NONE = 0,
	FA_READABLE = 1,
	FA_WRITABLE = 2,
	FA_EXECUTABLE = 4,
	FA_COPY_PROTECTED = 8
};

struct DateTime { // 0x8
	/* 0x0 */ short unsigned int year;
	/* 0x2 */ unsigned char month;
	/* 0x3 */ unsigned char day;
	/* 0x4 */ unsigned char hour;
	/* 0x5 */ unsigned char minute;
	/* 0x6 */ unsigned char second;
};

struct FileTime { // 0x10
	/* 0x0 */ DateTime created;
	/* 0x8 */ DateTime lastModified;
};

typedef char* (*LocaleCallback)(/* parameters unknown */);

// warning: multiple differing types with the same name (size not equal)
struct ConfigSettings { // 0x10
	/* 0x0 */ IAllocator *allocator;
	/* 0x4 */ IThread *thread;
	/* 0x8 */ IMutex *mutex;
	/* 0xc */ LocaleCallback getstrCallback;
};

struct CardID { // 0x4
	/* 0x0 */ short unsigned int port;
	/* 0x2 */ short unsigned int slot;
};

// warning: multiple differing types with the same name (size not equal)
struct DirectoryInfo { // 0x30
	/* 0x00 */ DirNameType *directoryName;
	/* 0x04 */ char *iconSysData;
	/* 0x08 */ int iconSysDataSize;
	/* 0x0c */ char *staticIconData;
	/* 0x10 */ int staticIconDataSize;
	/* 0x14 */ char *staticIconFilename;
	/* 0x18 */ char *copyIconData;
	/* 0x1c */ int copyIconDataSize;
	/* 0x20 */ char *copyIconFilename;
	/* 0x24 */ char *deleteIconData;
	/* 0x28 */ int deleteIconDataSize;
	/* 0x2c */ char *deleteIconFilename;
};

// warning: multiple differing types with the same name (size not equal)
struct FileInfo { // 0xc
	/* 0x0 */ char *fileName;
	/* 0x4 */ int fileByteSize;
	/* 0x8 */ DirectoryInfo *directoryInfo;
};

struct StartGameInfo { // 0x20
	/* 0x00 */ FileInfo fileInfo;
	/* 0x0c */ unsigned int totalBlocksNeeded;
	/* 0x10 */ CardID checkCardID;
	/* 0x14 */ bool checkAllSlots;
	/* 0x18 */ __wchar_t *gameTitle;
	/* 0x1c */ bool autoSave;
};

struct BlockCalculator { // 0x4
	/* 0x0 */ __vtbl_ptr_type *$vf264;

	BlockCalculator& operator=();
	BlockCalculator();
	BlockCalculator();
	/* vtable[1] */ virtual void Clear();
	/* vtable[2] */ virtual unsigned int GetResult();
	/* vtable[3] */ virtual void SetDirectoryInfo();
	/* vtable[4] */ virtual void AddDataFile();
protected:
	/* vtable[5] */ virtual BlockCalculator(BlockCalculator*, int, void);
};

enum LibMessage {
	LMSG_NONE = 0,
	LMSG_TRC_STARTGAME_DONE = 1,
	LMSG_TRC_CARDEXISTS_DONE = 2,
	LMSG_TRC_GETCARDINFO_DONE = 3,
	LMSG_TRC_LOADFILE_DONE = 4,
	LMSG_TRC_SAVEFILE_DONE = 5,
	LMSG_TRC_DELETEFILE_DONE = 6,
	LMSG_TRC_LISTFILES_DONE = 7,
	LMSG_CARD_EXISTS_DONE = 8,
	LMSG_CARD_INFO_DONE = 9,
	LMSG_CREATE_DIRECTORY_DONE = 10,
	LMSG_DELETE_DIRECTORY_DONE = 11,
	LMSG_FIND_FILE_DONE = 12,
	LMSG_OPEN_FILE_DONE = 13,
	LMSG_CLOSE_FILE_DONE = 14,
	LMSG_DELETE_FILE_DONE = 15,
	LMSG_SET_ATTRIBUTE_DONE = 16,
	LMSG_SEEK_DONE = 17,
	LMSG_READ_DONE = 18,
	LMSG_WRITE_DONE = 19,
	LMSG_FLUSH_DONE = 20,
	LMSG_MOUNT_DONE = 21,
	LMSG_UNMOUNT_DONE = 22,
	LMSG_IS_IT_YOUR_DIRECTORY = 23,
	LMSG_IS_IT_YOUR_FILE = 24,
	LMSG_TRC = 25,
	LMSG_FILE_INFO = 26,
	LMSG_READ_READY = 27,
	LMSG_WRITE_READY = 28,
	LMSG_CHECK_DATA_INTEGRITY = 29,
	LMSG_FORMAT_DONE = 30,
	LMSG_TASK_UNKNOWN = 31
};

enum CardStatus {
	CS_OK = 0,
	CS_FAILED = 1,
	CS_NO_CARD = 2,
	CS_CARD_CHANGED = 3,
	CS_CARD_UNFORMATTED = 4,
	CS_CARD_DAMAGED = 5,
	CS_WRONG_DEVICE = 6,
	CS_CARD_FULL = 7,
	CS_ACCESS_DENIED = 8,
	CS_INSUFFICIENT_SPACE = 9,
	CS_FILE_NOT_FOUND = 10,
	CS_ENTRY_NOT_FOUND = 11,
	CS_ENTRY_ALREADY_EXISTS = 12,
	CS_FILE_NOT_OPENED = 13,
	CS_FILE_CORRUPTED = 14,
	CS_READ_FAILED = 15,
	CS_WRITE_FAILED = 16,
	CS_DIRECTORY_NOT_FOUND = 17,
	CS_DIRECTORY_NOT_EMPTY = 18,
	CS_TOO_MANY_OPENED_FILES = 19,
	CS_CANCELLED = 20,
	CS_FILE_DELETED = 21,
	CS_MOUNT_FAILED = 22,
	CS_RANGE_ERROR = 23,
	CS_RETRY = 24,
	CS_CARD_REMOVED = 25,
	CS_UNKNOWN = -1
};

struct OpenFileDescriptor { // 0x1
};

typedef OpenFileDescriptor *FileHandle;

struct Trc { // 0x10
	/* 0x0 */ int stringID;
	/* 0x4 */ __wchar_t *pString;
	/* 0x8 */ __wchar_t *pYes;
	/* 0xc */ __wchar_t *pNo;
};

struct CardInfo { // 0x8
	/* 0x0 */ CardID cardID;
	/* 0x4 */ int freeSpace;
};

// warning: multiple differing types with the same name (size not equal)
struct FileInfo { // 0x18
	/* 0x00 */ char *fileName;
	/* 0x04 */ int fileSize;
	/* 0x08 */ FileTime fileTime;
};

struct OpenResult { // 0x8
	/* 0x0 */ FileHandle fileHandle;
	/* 0x4 */ char *fileName;
};

struct ReadResult { // 0x4
	/* 0x0 */ int bytesRead;
};

struct WriteResult { // 0x4
	/* 0x0 */ int bytesWritten;
};

struct SeekResult { // 0x4
	/* 0x0 */ unsigned int filePosition;
};

// warning: multiple differing types with the same name (size not equal)
struct DirectoryInfo { // 0x4
	/* 0x0 */ DirNameType *name;
};

struct DetailInfo { // 0x48
	/* 0x00 */ Trc trc;
	/* 0x10 */ CardInfo cardInfo;
	/* 0x18 */ FileInfo fileInfo;
	/* 0x30 */ OpenResult openResult;
	/* 0x38 */ ReadResult readResult;
	/* 0x3c */ WriteResult writeResult;
	/* 0x40 */ SeekResult seekResult;
	/* 0x44 */ DirectoryInfo directoryInfo;
};

// warning: multiple differing types with the same name (size not equal)
struct Message { // 0x50
	/* 0x00 */ LibMessage msg;
	/* 0x04 */ CardStatus status;
	/* 0x08 */ DetailInfo info;
};

enum UserMessage {
	UMSG_NONE = 0,
	UMSG_WRITE_COMPLETE = 1,
	UMSG_READ_COMPLETE = 2,
	UMSG_YES = 3,
	UMSG_NO = 4,
	UMSG_OK = 5,
	UMSG_DATA_OK = 6,
	UMSG_DATA_CORRUPT = 7,
	UMSG_EXPIRE_DELAY = 8,
	UMSG_SHOW_CARD_CHECKING_MSG = 9,
	UMSG_TOTAL = 10
};

struct BaseInterface { // 0x4
	/* 0x0 */ __vtbl_ptr_type *$vf346;

	BaseInterface& operator=();
	BaseInterface();
	BaseInterface();
	/* vtable[1] */ virtual int AddRef();
	/* vtable[2] */ virtual int Release();
	/* vtable[3] */ virtual void TrcStartGame();
	/* vtable[4] */ virtual void TrcCardExists();
	/* vtable[5] */ virtual void TrcGetCardInfo();
	/* vtable[6] */ virtual void TrcLoadFile();
	/* vtable[7] */ virtual void TrcSaveFile();
	/* vtable[8] */ virtual void TrcDeleteFile();
	/* vtable[9] */ virtual void TrcListFiles();
	/* vtable[10] */ virtual void CardExists();
	/* vtable[11] */ virtual void GetCardInfo();
	/* vtable[12] */ virtual void OpenFile();
	/* vtable[13] */ virtual void CloseFile();
	/* vtable[14] */ virtual void DeleteFile();
	/* vtable[15] */ virtual void Read();
	/* vtable[16] */ virtual void Write();
	/* vtable[17] */ virtual void Seek();
	/* vtable[18] */ virtual void Flush();
	/* vtable[19] */ virtual void SetFileAttribute();
	/* vtable[20] */ virtual void FindFile();
	/* vtable[21] */ virtual Message* GetMessage();
	/* vtable[22] */ virtual void SendMessage();
	/* vtable[23] */ virtual bool IsBusy();
	/* vtable[24] */ virtual __wchar_t* GetCardName();
	/* vtable[25] */ virtual unsigned int GetBlockSize();
	/* vtable[26] */ virtual BlockCalculator* GetBlockCalculator();
protected:
	/* vtable[27] */ virtual BaseInterface(BaseInterface*, int, void);
};

enum PS2MessageMode {
	PS2_MSG_MODE_DELAY_BEFORE_EXIT = 0,
	PS2_MSG_MODE_PROMPT_OK_BEFORE_EXIT = 1
};

enum CharType {
	CT_ASCII = 0,
	CT_SJIS = 1,
	CT_UNKNOWN = 2
};

struct Sjis { // 0x1
	Sjis& operator=();
	Sjis();
	Sjis();
	unsigned char Sjis2Ascii();
	short unsigned int Ascii2Sjis();
	void Sjis2AsciiString();
	void Ascii2SjisString();
	CharType CheckChar();
};

struct Interface : /* 0x0 */ BaseInterface { // 0x4
	Interface& operator=();
	Interface();
	Interface();
	static Interface* CreateInstance(/* parameters unknown */);
	static void InitMultitap(/* parameters unknown */);
	/* vtable[28] */ virtual void CreateDirectory();
	/* vtable[29] */ virtual void DeleteDirectory();
	/* vtable[30] */ virtual Sjis* GetSjisInterface();
	/* vtable[31] */ virtual void Format();
protected:
	/* vtable[27] */ virtual Interface(Interface*, int, void);
};

struct InterfaceImp : /* 0x00 */ Interface { // 0x1c
protected:
	/* 0x04 */ ConfigSettings mConfigSettings;
	/* 0x14 */ int mRefcount;
	/* 0x18 */ IMutex *mMutex;

public:
	InterfaceImp& operator=();
	InterfaceImp();
	static void* operator new(/* parameters unknown */);
	static void operator delete(/* parameters unknown */);
	static void* operator new [](/* parameters unknown */);
	static void operator delete [](/* parameters unknown */);
	static void* operator new(/* parameters unknown */);
	static void operator delete(/* parameters unknown */);
	static void* operator new [](/* parameters unknown */);
	static void operator delete [](/* parameters unknown */);
	InterfaceImp();
	/* vtable[27] */ virtual InterfaceImp(InterfaceImp*, int, void);
	void LockInterfaceMutex();
	void UnlockInterfaceMutex();
	void CheckVersion();
	/* vtable[1] */ virtual int AddRef();
	/* vtable[2] */ virtual int Release();
	/* vtable[32] */ virtual void ClearTask();
};

struct DeviceDriver { // 0x4
	/* 0x0 */ __vtbl_ptr_type *$vf408;

	DeviceDriver& operator=();
	DeviceDriver();
	static void* operator new(/* parameters unknown */);
	static void operator delete(/* parameters unknown */);
	static void* operator new [](/* parameters unknown */);
	static void operator delete [](/* parameters unknown */);
	static void* operator new(/* parameters unknown */);
	static void operator delete(/* parameters unknown */);
	static void* operator new [](/* parameters unknown */);
	static void operator delete [](/* parameters unknown */);
	DeviceDriver();
	/* vtable[1] */ virtual DeviceDriver(DeviceDriver*, int, void);
};

struct CmnFileDescriptor : /* 0x00 */ OpenFileDescriptor { // 0x24
	/* 0x04 */ unsigned int mSignature;
	/* 0x08 */ CardID mCardID;
	/* 0x0c */ FileInfo mFileInfo;
	/* 0x18 */ FileOpenMode mMode;
	/* 0x1c */ bool mOpen;
	/* 0x20 */ __vtbl_ptr_type *$vf415;

	CmnFileDescriptor& operator=();
	CmnFileDescriptor();
	CmnFileDescriptor();
	/* vtable[1] */ virtual void Clear();
	/* vtable[2] */ virtual void Init();
	bool CheckSignature();
};

struct MessageTimer { // 0x10
private:
	/* 0x0 */ bool mIsActive;
	/* 0x4 */ unsigned int mNumTicksElapsed;
	/* 0x8 */ unsigned int mNumTicksToExpire;
	/* 0xc */ unsigned int mNumSecondsDefaultDelay;

public:
	MessageTimer& operator=();
	MessageTimer();
	MessageTimer();
	MessageTimer(MessageTimer*, int, void);
	void Init();
	void Set();
	void Stop();
	void AddElapsedTime();
	bool IsExpired();
};

typedef __gnuc_va_list va_list;
typedef unsigned char u_char;
typedef short unsigned int u_short;
typedef unsigned int u_int;
typedef long unsigned int u_long;
typedef short unsigned int ushort;
typedef unsigned int uint;
typedef long int daddr_t;
typedef char *caddr_t;
typedef short unsigned int ino_t;
typedef short int dev_t;
typedef long int off_t;
typedef short unsigned int uid_t;
typedef short unsigned int gid_t;
typedef int pid_t;
typedef long int key_t;
typedef long int ssize_t;
typedef unsigned int mode_t;
typedef short unsigned int nlink_t;
typedef long int fd_mask;

typedef struct { // 0x8
	/* 0x0 */ fd_mask fds_bits[1];
} _types_fd_set;

// warning: multiple differing types with the same name (descriptor not equal)
typedef long long int long128;
// warning: multiple differing types with the same name (descriptor not equal)
typedef long long unsigned int u_long128;

struct ThreadParam { // 0x30
	/* 0x00 */ int status;
	/* 0x04 */ void (*entry)(/* parameters unknown */);
	/* 0x08 */ void *stack;
	/* 0x0c */ int stackSize;
	/* 0x10 */ void *gpReg;
	/* 0x14 */ int initPriority;
	/* 0x18 */ int currentPriority;
	/* 0x1c */ u_int attr;
	/* 0x20 */ u_int option;
	/* 0x24 */ int waitType;
	/* 0x28 */ int waitId;
	/* 0x2c */ int wakeupCount;
};

struct SemaParam { // 0x18
	/* 0x00 */ int currentCount;
	/* 0x04 */ int maxCount;
	/* 0x08 */ int initCount;
	/* 0x0c */ int numWaitThreads;
	/* 0x10 */ u_int attr;
	/* 0x14 */ u_int option;
};

struct sce_stat { // 0x40
	/* 0x00 */ unsigned int st_mode;
	/* 0x04 */ unsigned int st_attr;
	/* 0x08 */ unsigned int st_size;
	/* 0x0c */ unsigned char st_ctime[8];
	/* 0x14 */ unsigned char st_atime[8];
	/* 0x1c */ unsigned char st_mtime[8];
	/* 0x24 */ unsigned int st_hisize;
	/* 0x28 */ unsigned int st_private[6];
};

struct sce_dirent { // 0x144
	/* 0x000 */ sce_stat d_stat;
	/* 0x040 */ char d_name[256];
	/* 0x140 */ void *d_private;
};

enum sceFsRWTYPE {
	sceFsREADING = 0,
	sceFsWRITING = 1
};

typedef struct { // 0x18
	/* 0x00 */ unsigned int lbn;
	/* 0x04 */ unsigned int nblk;
	/* 0x08 */ void *addr;
	/* 0x0c */ unsigned int blksiz;
	/* 0x10 */ sceFsRWTYPE type;
	/* 0x14 */ unsigned int mode;
} sceFsDevctlBlkIO;

enum SceStdioFunc {
	SCE_STDIO_FUNC_ANYTHING = 0
};

typedef struct { // 0x10
	/* 0x0 */ unsigned int epc;
	/* 0x4 */ unsigned int gp;
	/* 0x8 */ unsigned int sp;
	/* 0xc */ unsigned int dummy;
} sceExecData;

typedef void (*sceSifRebootNotifyHandler)(/* parameters unknown */);

typedef struct { // 0x10
	/* 0x0 */ sceSifRebootNotifyHandler func;
	/* 0x4 */ void *data;
	/* 0x8 */ void *gp;
	/* 0xc */ void *reserve;
} sceSifRebootNotifyData;

typedef int _iconVu0IVECTOR[4];
typedef float _iconVu0FVECTOR[4];

typedef struct { // 0x3c4
	/* 0x000 */ unsigned char Head[4];
	/* 0x004 */ short unsigned int Reserv1;
	/* 0x006 */ short unsigned int OffsLF;
	/* 0x008 */ unsigned int Reserv2;
	/* 0x00c */ unsigned int TransRate;
	/* 0x010 */ _iconVu0IVECTOR BgColor[4];
	/* 0x050 */ _iconVu0FVECTOR LightDir[3];
	/* 0x080 */ _iconVu0FVECTOR LightColor[3];
	/* 0x0b0 */ _iconVu0FVECTOR Ambient;
	/* 0x0c0 */ unsigned char TitleName[68];
	/* 0x104 */ unsigned char FnameView[64];
	/* 0x144 */ unsigned char FnameCopy[64];
	/* 0x184 */ unsigned char FnameDel[64];
	/* 0x1c4 */ unsigned char Reserve3[512];
} sceMcIconSys;

typedef struct { // 0x8
	/* 0x0 */ unsigned char Resv2;
	/* 0x1 */ unsigned char Sec;
	/* 0x2 */ unsigned char Min;
	/* 0x3 */ unsigned char Hour;
	/* 0x4 */ unsigned char Day;
	/* 0x5 */ unsigned char Month;
	/* 0x6 */ short unsigned int Year;
} sceMcStDateTime;

typedef struct { // 0x40
	/* 0x00 */ sceMcStDateTime _Create;
	/* 0x08 */ sceMcStDateTime _Modify;
	/* 0x10 */ unsigned int FileSizeByte;
	/* 0x14 */ short unsigned int AttrFile;
	/* 0x16 */ short unsigned int Reserve1;
	/* 0x18 */ unsigned int Reserve2;
	/* 0x1c */ unsigned int PdaAplNo;
	/* 0x20 */ unsigned char EntryName[32];
} sceMcTblGetDir;

enum SceMcFunc {
	SCE_MC_FUNC_GETINFO = 1,
	SCE_MC_FUNC_OPEN = 2,
	SCE_MC_FUNC_CLOSE = 3,
	SCE_MC_FUNC_SEEK = 4,
	SCE_MC_FUNC_READ = 5,
	SCE_MC_FUNC_WRITE = 6,
	SCE_MC_FUNC_FLUSH = 10,
	SCE_MC_FUNC_MKDIR = 11,
	SCE_MC_FUNC_CHDIR = 12,
	SCE_MC_FUNC_GETDIR = 13,
	SCE_MC_FUNC_SETFILEINFO = 14,
	SCE_MC_FUNC_DELETE = 15,
	SCE_MC_FUNC_FORMAT = 16,
	SCE_MC_FUNC_UNFORMAT = 17,
	SCE_MC_FUNC_GETENTSPACE = 18,
	SCE_MC_FUNC_RENAME = 19,
	SCE_MC_FUNC_CHANGETHREADPRIORITY = 20,
	SCE_MC_FUNC_GETSLOTMAX = 21,
	SCE_MC_FUNC_SYNC = 30,
	SCE_MC_FUNC_INIT = 31,
	SCE_MC_FUNC_END = 32
};

enum MSG_PROMPT {
	PROMPT_NONE = 0,
	PROMPT_YESNO = 1,
	PROMPT_OK = 2
};

struct PS2Message : /* 0x00 */ Message { // 0x54
	/* 0x50 */ __vtbl_ptr_type *$vf552;

	PS2Message& operator=();
	PS2Message();
	PS2Message();
	/* vtable[1] */ virtual PS2Message(PS2Message*, int, void);
	void Init();
	void Clear();
	void Set();
	void Set();
	void Set();
	void Set();
	void Set();
	void Set();
	void Set();
	void Set();
};

// warning: multiple differing types with the same name (enum constant not equal)
enum TaskID {
	TID_NONE = 0,
	TID_TRC_STARTGAME = 1,
	TID_TRC_CARDEXISTS = 2,
	TID_TRC_GETCARDINFO = 3,
	TID_TRC_LOADFILE = 4,
	TID_TRC_SAVEFILE = 5,
	TID_TRC_DELETEFILE = 6,
	TID_TRC_LISTFILES = 7,
	TID_CARD_EXISTS = 8,
	TID_GET_CARD_INFO = 9,
	TID_MOUNT = 10,
	TID_UNMOUNT = 11,
	TID_CREATE_DIRECTORY = 12,
	TID_DELETE_DIRECTORY = 13,
	TID_OPEN = 14,
	TID_CLOSE = 15,
	TID_DELETE = 16,
	TID_SEEK = 17,
	TID_READ = 18,
	TID_WRITE = 19,
	TID_FLUSH = 20,
	TID_SET_ATTRIBUTES = 21,
	TID_FIND = 22,
	TID_CHECK_SPACE = 23,
	TID_FORMAT = 24,
	TID_STARTGAME_COMMON = 25,
	TID_SAVE_FILE = 26,
	TID_CHECK_DIRECTORY = 27,
	TID_CHANGE_DIRECTORY = 28,
	TID_TOTAL = 29
};

// warning: multiple differing types with the same name (enum constant not equal)
enum TaskState {
	TS_START = 0,
	TS_RESULT = 1,
	TS_CHECK_RESULT = 2,
	TS_FAILED = 3,
	TS_SUCCESS = 4,
	TS_WAIT_FOR_USER_REPLY = 5,
	TS_WAIT_FOR_OK_REPLY = 6,
	TS_WAIT_FOR_USER_REPLY_B = 7,
	TS_FIND_DIRECTORY = 8,
	TS_CHANGE_DIRECTORY = 9,
	TS_CHANGE_DIRECTORY_RESULT = 10,
	TS_SEARCH = 11,
	TS_FIND_FIRST = 12,
	TS_FIND_NEXT = 13,
	TS_FIND_FILE = 14,
	TS_FIND_FILE_RESULT = 15,
	TS_FILE_NOT_FOUND = 16,
	TS_FIND_ICONSYS_RESULT = 17,
	TS_FIND_DUMMYFILE_RESULT = 18,
	TS_ENTRY_NOT_FOUND = 19,
	TS_CHECK_CARD = 20,
	TS_CHECK_CARD_RESULT = 21,
	TS_CHECK_NEXT_CARD = 22,
	TS_GET_CARD_INFO = 23,
	TS_NO_CARD_ASK_USER = 24,
	TS_NO_CARD_WAIT_FOR_USER_REPLY = 25,
	TS_CHECK_SPACE = 26,
	TS_CHECK_SPACE_RESULT = 27,
	TS_CHECK_MAX_FILES = 28,
	TS_CHECK_MAX_FILES_RESULT = 29,
	TS_INSUFFICIENT_SPACE_ASK_USER = 30,
	TS_INSUFFICIENT_SPACE_ASK_USER_RESULT = 31,
	TS_INSUFFICIENT_SPACE_INFORM_USER = 32,
	TS_NO_AUTOSAVE_CONFIRM = 33,
	TS_NO_AUTOSAVE_CONFIRM_WAIT_FOR_REPLY = 34,
	TS_CHECK_FOR_PREVIOUS_SAVE = 35,
	TS_AUTOSAVE_ENABLE_WAIT_FOR_REPLY = 36,
	TS_CONTINUE_NO_SAVE = 37,
	TS_CHECK_DIRECTORY = 38,
	TS_CHECK_DIRECTORY_RESULT = 39,
	TS_CREATE_DIRECTORY = 40,
	TS_CREATE_DIRECTORY_RESULT = 41,
	TS_CHECK_ICON_SYS = 42,
	TS_CHECK_ICON_SYS_RESULT = 43,
	TS_CHECK_STATIC_ICON = 44,
	TS_CHECK_STATIC_ICON_RESULT = 45,
	TS_CHECK_COPY_ICON = 46,
	TS_CHECK_COPY_ICON_RESULT = 47,
	TS_CHECK_DELETE_ICON = 48,
	TS_CHECK_DELETE_ICON_RESULT = 49,
	TS_CHECK_DUMMY_FILE = 50,
	TS_CHECK_DUMMY_FILE_RESULT = 51,
	TS_SAVE_ICON_SYS = 52,
	TS_SAVE_ICON_SYS_RESULT = 53,
	TS_SAVE_STATIC_ICON = 54,
	TS_SAVE_STATIC_ICON_RESULT = 55,
	TS_SAVE_COPY_ICON = 56,
	TS_SAVE_COPY_ICON_RESULT = 57,
	TS_SAVE_DELETE_ICON = 58,
	TS_SAVE_DELETE_ICON_RESULT = 59,
	TS_SAVE_FAILED_ASK_USER = 60,
	TS_CHECK_FORMAT_RESULT = 61,
	TS_OVERWRITE_ASK_USER = 62,
	TS_OVERWRITE_CHECK_USER_REPLY = 63,
	TS_SHOW_SAVE_WARNING = 64,
	TS_FORMAT_WAIT_FOR_USER_CONFIRMATION = 65,
	TS_FORMAT = 66,
	TS_FORMAT_WAIT_FOR_RESULT = 67,
	TS_WRITE_READY = 68,
	TS_WRITE_WAIT = 69,
	TS_SAVE_FAILED = 70,
	TS_SAVE_FAILED_INFORM_USER = 71,
	TS_SAVE_FAILED_WAIT_FOR_USER_REPLY = 72,
	TS_WRITE = 73,
	TS_WRITE_RESULT = 74,
	TS_SHOW_LOAD_WARNING_MSG = 75,
	TS_READ_READY = 76,
	TS_READ_WAIT = 77,
	TS_SHOW_RESULT = 78,
	TS_USER_CHECK_DATA = 79,
	TS_USER_CHECK_DATA_RESULT = 80,
	TS_OPEN_FILE = 81,
	TS_OPEN_FILE_RESULT = 82,
	TS_CLOSE_FILE = 83,
	TS_CLOSE_FILE_RESULT = 84,
	TS_FIND_FILE_SIZE = 85,
	TS_FIND_FILE_SIZE_RESULT = 86,
	TS_DELETE_FILE_ASK_USER = 87,
	TS_DELETE_WAIT_FOR_USER_REPLY = 88,
	TS_DELETE_FILE = 89,
	TS_DELETE_FILE_RESULT = 90,
	TS_DELETE_ALL_FILES = 91,
	TS_DELETE_DIRECTORY = 92,
	TS_DELETE_DIRECTORY_RESULT = 93,
	TS_DELETE_TEMPORARY_FILE = 94,
	TS_DISPLAY_RESULT = 95,
	TS_GET_FREE_SPACE = 96,
	TS_WAIT_FOR_RESULT = 97,
	TS_DELAY = 98,
	TS_UNFORMAT_RESULT = 99,
	TS_LOAD_READY = 100,
	TS_MOUNT = 101,
	TS_MOUNT_RESULT = 102,
	TS_CHECK_REMOUNT = 103,
	TS_SET_ATTRIBUTE = 104,
	TS_CHECK_MOUNT_RESULT = 105,
	TS_BAD_CARD_ASK_USER = 106,
	TS_ERROR_ASK_USER = 107,
	TS_MOUNT_RESULT_WAIT_USER_REPLY = 108,
	TS_RETRY = 109,
	TS_SAVE_READY = 110,
	TS_UPDATE_SPACE_USED = 111,
	TS_FOUND_FILE_ASK_USER = 112,
	TS_FILE_LIMIT_WAIT_FOR_REPLY = 113,
	TS_LOAD_FAILED_INFORM_USER = 114,
	TS_LOAD_FAILED_WAIT_FOR_USER_REPLY = 115,
	TS_DELETE_FAILED_INFORM_USER = 116,
	TS_DELETE_FAILED_WAIT_FOR_USER_REPLY = 117,
	TS_FAILED_CARD_REMOVED_INFORM_USER = 118,
	TS_FAILED_CARD_REMOVED_WAIT_FOR_USER_REPLY = 119,
	TS_SAVE_DIRECTROY_FILES = 120,
	TS_CREATE_DIRECTORY_TABLE = 121,
	TS_CONFIRM_CHECKING_DIRECTORY = 122,
	TS_DONE = 123,
	TS_END = 124
};

// warning: multiple differing types with the same name (size not equal)
struct Task { // 0x14
protected:
	/* 0x00 */ TaskManager *mParent;
	/* 0x04 */ TaskID mID;
	/* 0x08 */ TaskState mState;
	/* 0x0c */ TaskState mSubstate;
public:
	/* 0x10 */ __vtbl_ptr_type *$vf570;

	Task& operator=();
	Task();
	Task();
	Task();
	/* vtable[1] */ virtual void Init();
	TaskID GetID();
	TaskState GetState();
	TaskState SetState();
	int GetSubstate();
	void SetSubstate();
};

struct TaskStack { // 0x44
private:
	/* 0x00 */ Task *mBuf[16];
	/* 0x40 */ int mNumTasks;

public:
	TaskStack& operator=();
	TaskStack();
	TaskStack();
	void Push();
	void Pop();
};

struct TaskManager { // 0x50
protected:
	/* 0x00 */ Task *mCurrentTask;
	/* 0x04 */ TaskStack mTaskStack;
	/* 0x48 */ int mPublicTrcFunctionRefcount;
public:
	/* 0x4c */ __vtbl_ptr_type *$vf572;

	TaskManager& operator=();
	TaskManager();
	TaskManager();
	/* vtable[1] */ virtual void StartTask();
	/* vtable[2] */ virtual void EndTask();
	Task* GetCurrentTask();
	/* vtable[3] */ virtual void UpdateCurrentTask();
	void ClearTask();
protected:
	bool IsPublicTrcTask();
	void TrcSingletonAssert();
};

struct BlockCalculatorImp : /* 0x00 */ BlockCalculator { // 0x48
private:
	/* 0x04 */ CardID mCardID;
	/* 0x08 */ DirectoryInfo mDirectoryInfo;
	/* 0x38 */ unsigned int mBlockSize;
	/* 0x3c */ unsigned int mBlockCount;
	/* 0x40 */ unsigned int mFileCount;
	/* 0x44 */ unsigned int mDefaultDummyFileBlockSize;

public:
	BlockCalculatorImp& operator=();
	BlockCalculatorImp();
	BlockCalculatorImp();
	/* vtable[5] */ virtual BlockCalculatorImp(BlockCalculatorImp*, int, void);
	/* vtable[1] */ virtual void Clear();
	/* vtable[3] */ virtual void SetDirectoryInfo();
	/* vtable[4] */ virtual void AddDataFile();
	/* vtable[2] */ virtual unsigned int GetResult();
	void SetBlockSize();
	unsigned int CalculateBlockSize();
	unsigned int CalculateBlockSize();
private:
	unsigned int GetBlockCount();
};

struct Ps2FileDescriptor : /* 0x00 */ CmnFileDescriptor { // 0xb8
	/* 0x24 */ int mSceFileMode;
	/* 0x28 */ int mSceFileHandle;
	/* 0x2c */ int mBytesRead;
	/* 0x30 */ int mBytesWritten;
	/* 0x34 */ unsigned int mSeekPosition;
	/* 0x38 */ char mPath[128];

	Ps2FileDescriptor& operator=();
	Ps2FileDescriptor();
	Ps2FileDescriptor();
	/* vtable[1] */ virtual void Clear();
	/* vtable[2] */ virtual void Init();
};

struct Ps2Task : /* 0x00 */ Task { // 0x20
	/* 0x14 */ CardID mCardID;
	/* 0x18 */ CardStatus mStatus;
	/* 0x1c */ bool mNotifyUser;

	Ps2Task& operator=();
	Ps2Task();
	Ps2Task();
	/* vtable[1] */ virtual void Init();
	void End();
};

struct TaskTrcStartGame : /* 0x00 */ Ps2Task { // 0x4c
	/* 0x20 */ StartGameInfo mInfo;
	/* 0x40 */ bool mUsingAutosave;
	/* 0x44 */ bool mEnableAutosave;
	/* 0x48 */ int mSpaceNeeded;

	TaskTrcStartGame& operator=();
	TaskTrcStartGame();
	TaskTrcStartGame();
	void Start();
};

struct TaskTrcGetCardInfo : /* 0x00 */ Ps2Task { // 0x24
	/* 0x20 */ TaskID mTaskID;

	TaskTrcGetCardInfo& operator=();
	TaskTrcGetCardInfo();
	TaskTrcGetCardInfo();
	void Start();
};

struct TaskTrcSaveFile : /* 0x00 */ Ps2Task { // 0x48
	/* 0x20 */ FileInfo mFileInfo;
	/* 0x2c */ int mSpaceNeeded;
	/* 0x30 */ Ps2FileDescriptor *mFileDescriptor;
	/* 0x34 */ CardStatus mYesStatus;
	/* 0x38 */ CardStatus mNoStatus;
	/* 0x3c */ bool mExistingFileFound;
	/* 0x40 */ bool mConfirmedByUser;
	/* 0x44 */ bool mWriteStarted;

	TaskTrcSaveFile& operator=();
	TaskTrcSaveFile();
	TaskTrcSaveFile();
	void Start();
};

struct TaskTrcLoadFile : /* 0x000 */ Ps2Task { // 0x3fc
	/* 0x020 */ FileInfo mFileInfo;
	/* 0x02c */ Ps2FileDescriptor *mFileDescriptor;
	/* 0x030 */ bool mFileOpened;
	/* 0x034 */ sceMcIconSys mIconSys;
	/* 0x3f8 */ unsigned int mIconSysSize;

	TaskTrcLoadFile& operator=();
	TaskTrcLoadFile();
	TaskTrcLoadFile();
	void Start();
};

struct TaskTrcDeleteFile : /* 0x00 */ Ps2Task { // 0x3c
	/* 0x20 */ FileInfo mFileInfo;
	/* 0x2c */ bool mDeleteDirectory;
	/* 0x30 */ bool mCardFoundOnEntry;
	/* 0x34 */ bool mFileFound;
	/* 0x38 */ bool mDeleteStarted;

	TaskTrcDeleteFile& operator=();
	TaskTrcDeleteFile();
	TaskTrcDeleteFile();
	void Start();
};

struct TaskTrcListFiles : /* 0x00 */ Ps2Task { // 0x30
	/* 0x20 */ FileInfo mFileInfo;
	/* 0x2c */ bool mFileFound;

	TaskTrcListFiles& operator=();
	TaskTrcListFiles();
	TaskTrcListFiles();
	void Start();
};

struct TaskStartGameCommon : /* 0x00 */ Ps2Task { // 0x68
	/* 0x20 */ int mNewFileSpaceNeeded;
	/* 0x24 */ int mActualSpaceNeeded;
	/* 0x28 */ CardStatus mCardStatus[2][4];
	/* 0x48 */ bool mFileExists[2][4];

	TaskStartGameCommon& operator=();
	TaskStartGameCommon();
	TaskStartGameCommon();
	void Start();
	void RecordCardState();
};

struct TaskGetCardInfo : /* 0x00 */ Ps2Task { // 0x38
	/* 0x20 */ TaskID mProxyTaskID;
	/* 0x24 */ int mType;
	/* 0x28 */ int mFreeSpace;
	/* 0x2c */ int mFormatted;
	/* 0x30 */ int mRetry;
	/* 0x34 */ bool mCardFound;

	TaskGetCardInfo& operator=();
	TaskGetCardInfo();
	TaskGetCardInfo();
	void Start();
};

struct TaskCreateDirectory : /* 0x00 */ Ps2Task { // 0x44
	/* 0x20 */ char mDirectoryName[36];

	TaskCreateDirectory& operator=();
	TaskCreateDirectory();
	TaskCreateDirectory();
	void Start();
};

struct TaskOpen : /* 0x00 */ Ps2Task { // 0x74
	/* 0x20 */ char mDirectoryName[36];
	/* 0x44 */ char mFileName[36];
	/* 0x68 */ FileOpenMode mMode;
	/* 0x6c */ FileAttribute mAttribute;
	/* 0x70 */ Ps2FileDescriptor *mFileDescriptor;

	TaskOpen& operator=();
	TaskOpen();
	TaskOpen();
	void Start();
};

struct TaskClose : /* 0x00 */ Ps2Task { // 0x24
	/* 0x20 */ Ps2FileDescriptor *mFileDescriptor;

	TaskClose& operator=();
	TaskClose();
	TaskClose();
	void Start();
};

struct TaskSeek : /* 0x00 */ Ps2Task { // 0x2c
	/* 0x20 */ Ps2FileDescriptor *mFileDescriptor;
	/* 0x24 */ int mOffset;
	/* 0x28 */ SeekFrom mSeekFrom;

	TaskSeek& operator=();
	TaskSeek();
	TaskSeek();
	void Start();
};

struct TaskRead : /* 0x00 */ Ps2Task { // 0x30
	/* 0x20 */ Ps2FileDescriptor *mFileDescriptor;
	/* 0x24 */ void *mBuffer;
	/* 0x28 */ int mBufferSize;
	/* 0x2c */ int mBytesRead;

	TaskRead& operator=();
	TaskRead();
	TaskRead();
	void Start();
};

struct TaskWrite : /* 0x00 */ Ps2Task { // 0x30
	/* 0x20 */ Ps2FileDescriptor *mFileDescriptor;
	/* 0x24 */ void *mBuffer;
	/* 0x28 */ int mBufferSize;
	/* 0x2c */ int mBytesWritten;

	TaskWrite& operator=();
	TaskWrite();
	TaskWrite();
	void Start();
};

struct TaskFlush : /* 0x00 */ Ps2Task { // 0x24
	/* 0x20 */ Ps2FileDescriptor *mFileDescriptor;

	TaskFlush& operator=();
	TaskFlush();
	TaskFlush();
	void Start();
};

struct TaskDelete : /* 0x00 */ Ps2Task { // 0x74
	/* 0x20 */ char mPath[80];
	/* 0x70 */ bool mDeleteDirectory;

	TaskDelete& operator=();
	TaskDelete();
	TaskDelete();
	void Start();
};

struct TaskFormat : /* 0x00 */ Ps2Task { // 0x20
	TaskFormat& operator=();
	TaskFormat();
	TaskFormat();
	void Start();
};

struct TaskSetAttribute : /* 0x00 */ Ps2Task { // 0x6c
	/* 0x20 */ char mDirectoryName[36];
	/* 0x44 */ char mFileName[36];
	/* 0x68 */ FileAttribute mAttribute;

	TaskSetAttribute& operator=();
	TaskSetAttribute();
	TaskSetAttribute();
	void Start();
};

struct TaskCheckSpace : /* 0x00 */ Ps2Task { // 0xb8
	/* 0x20 */ FileInfo mFileInfo;
	/* 0x2c */ unsigned int mTotalBlocksRequired;
	/* 0x30 */ unsigned int mFreeCardSpace;
	/* 0x34 */ TaskID mCallingTaskID;
	/* 0x38 */ char *mFileNameFound;
	/* 0x3c */ unsigned int mFileBlockSizeFound;
	/* 0x40 */ unsigned int mExistingFileBlockSize;
	/* 0x44 */ unsigned int mNumFilesFound;
	/* 0x48 */ unsigned int mExtraBlocksNeeded;
	/* 0x4c */ unsigned int mSpaceNeeded;
	/* 0x50 */ char mPath[76];
	/* 0x9c */ char *mDirectoryTable[36];
	/* 0xa0 */ int mNumDirectories;
	/* 0xa4 */ int mDirIndex;
	/* 0xa8 */ bool mDirectoryFound;
	/* 0xac */ bool mFileExists;
	/* 0xb0 */ bool mIsMyFile;
	/* 0xb4 */ bool mFindFirst;

	TaskCheckSpace& operator=();
	TaskCheckSpace();
	TaskCheckSpace();
	void Start();
	void Start();
	void Clear();
};

struct TaskSaveFile : /* 0x00 */ Ps2Task { // 0x78
	/* 0x20 */ char mDirectoryName[36];
	/* 0x44 */ char mFileName[36];
	/* 0x68 */ void *mData;
	/* 0x6c */ int mDataSize;
	/* 0x70 */ Ps2FileDescriptor *mFileDescriptor;
	/* 0x74 */ int mBytesWritten;

	TaskSaveFile& operator=();
	TaskSaveFile();
	TaskSaveFile();
	void Start();
};

struct TaskFind : /* 0x00 */ Ps2Task { // 0xbc
	/* 0x20 */ char mDirectoryName[36];
	/* 0x44 */ char mFileName[36];
	/* 0x68 */ char mPath[76];
	/* 0xb4 */ int mFileSize;
	/* 0xb8 */ int mNumFilesFound;

	TaskFind& operator=();
	TaskFind();
	TaskFind();
	void Start();
};

struct TaskCheckDirectory : /* 0x00 */ Ps2Task { // 0x44
	/* 0x20 */ char mDirectoryName[36];

	TaskCheckDirectory& operator=();
	TaskCheckDirectory();
	TaskCheckDirectory();
	void Start();
};

struct TaskChangeDirectory : /* 0x00 */ Ps2Task { // 0x44
	/* 0x20 */ char mDirectoryName[36];

	TaskChangeDirectory& operator=();
	TaskChangeDirectory();
	TaskChangeDirectory();
	void Start();
};

struct PS2Interface : /* 0x000 */ InterfaceImp, /* 0x0e0 */ TaskManager { // 0xe78
private:
	/* 0x06c */ TaskTrcStartGame mTaskTrcStartGame;
	/* 0x0b8 */ TaskTrcGetCardInfo mTaskTrcGetCardInfo;
	/* 0x0dc */ TaskTrcSaveFile mTaskTrcSaveFile;
	/* 0x124 */ TaskTrcLoadFile mTaskTrcLoadFile;
	/* 0x520 */ TaskTrcDeleteFile mTaskTrcDeleteFile;
	/* 0x55c */ TaskTrcListFiles mTaskTrcListFiles;
	/* 0x58c */ TaskStartGameCommon mTaskStartGameCommon;
	/* 0x5f4 */ TaskGetCardInfo mTaskGetCardInfo;
	/* 0x62c */ TaskCreateDirectory mTaskCreateDirectory;
	/* 0x670 */ TaskChangeDirectory mTaskChangeDirectory;
	/* 0x6b4 */ TaskOpen mTaskOpen;
	/* 0x728 */ TaskClose mTaskClose;
	/* 0x74c */ TaskDelete mTaskDelete;
	/* 0x7c0 */ TaskSeek mTaskSeek;
	/* 0x7ec */ TaskRead mTaskRead;
	/* 0x81c */ TaskWrite mTaskWrite;
	/* 0x84c */ TaskFlush mTaskFlush;
	/* 0x870 */ TaskSetAttribute mTaskSetAttribute;
	/* 0x8dc */ TaskFind mTaskFind;
	/* 0x998 */ TaskFormat mTaskFormat;
	/* 0x9b8 */ TaskCheckSpace mTaskCheckSpace;
	/* 0xa70 */ TaskSaveFile mTaskSaveFile;
	/* 0xae8 */ TaskCheckDirectory mTaskCheckDirectory;
	static sceMcTblGetDir mPs2FileTableEntry;
	/* 0xb2c */ Ps2FileDescriptor mFileDescriptors[3];
	/* 0xd54 */ Ps2FileDescriptor *mCurrentFileDescriptor;
	/* 0xd58 */ Sjis mSjis;
	/* 0xd5c */ UserMessage mUserReply;
	/* 0xd60 */ PS2Message mMsg;
	/* 0xdb4 */ PS2Message mResultMsg;
	/* 0xe08 */ BlockCalculatorImp mBlockCalculator;
	/* 0xe50 */ MessageTimer mMsgTimer;
	/* 0xe60 */ int mSceAsyncResult;
	/* 0xe64 */ CardStatus mCardStatus;
	/* 0xe68 */ int mNumFilesFound;
	/* 0xe6c */ bool mWriteFailed;
	/* 0xe70 */ bool mReadFailed;
	/* 0xe74 */ bool mNoCheckingCardWarning;

public:
	PS2Interface& operator=();
	PS2Interface();
	static void* operator new(/* parameters unknown */);
	static void operator delete(/* parameters unknown */);
	static void* operator new [](/* parameters unknown */);
	static void operator delete [](/* parameters unknown */);
	static void* operator new(/* parameters unknown */);
	static void operator delete(/* parameters unknown */);
	static void* operator new [](/* parameters unknown */);
	static void operator delete [](/* parameters unknown */);
	PS2Interface();
	/* vtable[27] */ virtual PS2Interface(PS2Interface*, int, void);
	/* vtable[3] */ virtual void TrcStartGame();
	/* vtable[4] */ virtual void TrcCardExists();
	/* vtable[5] */ virtual void TrcGetCardInfo();
	/* vtable[6] */ virtual void TrcLoadFile();
	/* vtable[7] */ virtual void TrcSaveFile();
	/* vtable[8] */ virtual void TrcDeleteFile();
	/* vtable[9] */ virtual void TrcListFiles();
	/* vtable[10] */ virtual void CardExists();
	/* vtable[11] */ virtual void GetCardInfo();
	/* vtable[12] */ virtual void OpenFile();
	/* vtable[13] */ virtual void CloseFile();
	/* vtable[14] */ virtual void DeleteFile();
	/* vtable[15] */ virtual void Read();
	/* vtable[16] */ virtual void Write();
	/* vtable[17] */ virtual void Seek();
	/* vtable[18] */ virtual void Flush();
	/* vtable[19] */ virtual void SetFileAttribute();
	/* vtable[20] */ virtual void FindFile();
	/* vtable[28] */ virtual void CreateDirectory();
	/* vtable[29] */ virtual void DeleteDirectory();
	/* vtable[31] */ virtual void Format();
	/* vtable[21] */ virtual Message* GetMessage();
	/* vtable[22] */ virtual void SendMessage();
	/* vtable[23] */ virtual bool IsBusy();
	/* vtable[24] */ virtual __wchar_t* GetCardName();
	/* vtable[30] */ virtual Sjis* GetSjisInterface();
	/* vtable[25] */ virtual unsigned int GetBlockSize();
	/* vtable[26] */ virtual BlockCalculator* GetBlockCalculator();
	/* vtable[32] */ virtual void ClearTask();
	/* vtable[3] */ virtual void UpdateCurrentTask();
private:
	CardStatus SceResultToStatus();
	char* PortStr();
	char* PortStrAscii();
	void CheckName();
	void CheckFileInfo();
	void CheckIopModule();
	void CheckSceResult(PS2Interface*, int, void);
	bool CheckSyncResult();
	CardStatus CheckCard();
	bool MultitapConnected();
	bool CheckCurrentTask();
	void SetFileTime();
	Ps2FileDescriptor* SceOpenFile();
	void SceCloseFile();
	Ps2FileDescriptor* FindFreeFileDescriptor();
	bool GetNextCardID();
	void UpdateTaskTrcStartGame();
	void UpdateTaskTrcStartGameAutoSave();
	void UpdateTaskTrcGetCardInfo();
	void UpdateTaskTrcListFiles();
	void UpdateTaskTrcSaveFile();
	void UpdateTaskTrcLoadFile();
	void UpdateTaskTrcDeleteFile();
	void UpdateTaskTrcStartGameNoDelay();
	void UpdateTaskTrcStartGameAutoSaveNoDelay();
	void UpdateTaskTrcGetCardInfoNoDelay();
	void UpdateTaskTrcListFilesNoDelay();
	void UpdateTaskTrcSaveFileNoDelay();
	void UpdateTaskTrcLoadFileNoDelay();
	void UpdateTaskTrcDeleteFileNoDelay();
	void UpdateTaskStartGameCommon();
	void UpdateTaskGetCardInfo();
	void UpdateTaskCheckSpace();
	void UpdateTaskSaveFile();
	void UpdateTaskFind();
	void UpdateTaskWrite();
	void UpdateTaskRead();
	void UpdateTaskCreateDirectory();
	void UpdateTaskChangeDirectory();
	void UpdateTaskOpen();
	void UpdateTaskClose();
	void UpdateTaskFlush();
	void UpdateTaskSeek();
	void UpdateTaskSetAttribute();
	void UpdateTaskDelete();
	void UpdateTaskFormat();
	void UpdateTaskCheckDirectory();
};

typedef struct { // 0x8
	/* 0x0 */ int quot;
	/* 0x4 */ int rem;
} div_t;

typedef struct { // 0x10
	/* 0x0 */ long int quot;
	/* 0x8 */ long int rem;
} ldiv_t;

typedef void (*CdlCB)(/* parameters unknown */);

typedef struct { // 0x4
	/* 0x0 */ u_char trycount;
	/* 0x1 */ u_char spindlctrl;
	/* 0x2 */ u_char datapattern;
	/* 0x3 */ u_char pad;
} sceCdRMode;

typedef struct { // 0x4
	/* 0x0 */ u_char minute;
	/* 0x1 */ u_char second;
	/* 0x2 */ u_char sector;
	/* 0x3 */ u_char track;
} sceCdlLOCCD;

typedef struct { // 0x24
	/* 0x00 */ u_int lsn;
	/* 0x04 */ u_int size;
	/* 0x08 */ char name[16];
	/* 0x18 */ unsigned char date[8];
	/* 0x20 */ u_int flag;
} sceCdlFILE;

typedef struct { // 0x8
	/* 0x0 */ u_char stat;
	/* 0x1 */ u_char second;
	/* 0x2 */ u_char minute;
	/* 0x3 */ u_char hour;
	/* 0x4 */ u_char pad;
	/* 0x5 */ u_char day;
	/* 0x6 */ u_char month;
	/* 0x7 */ u_char year;
} sceCdCLOCK;

typedef struct { // 0xc
	/* 0x0 */ u_int bufmax;
	/* 0x4 */ u_int bankmax;
	/* 0x8 */ u_int iop_bufaddr;
} sceCdStmInit;

typedef struct { // 0x8
	/* 0x0 */ short int TimeZone;
	/* 0x2 */ u_char Aspect;
	/* 0x3 */ u_char DateNotation;
	/* 0x4 */ u_char Language;
	/* 0x5 */ u_char Spdif;
	/* 0x6 */ u_char SummerTime;
	/* 0x7 */ u_char TimeNotation;
} sceScfT10kConfig;

typedef signed char Int8;
typedef unsigned char Uint8;
typedef short int Int16;
typedef short unsigned int Uint16;
typedef int Int32;
typedef unsigned int Uint32;
typedef long int Int64;
typedef long unsigned int Uint64;
typedef long long int Int128;
typedef long long unsigned int Uint128;
typedef float Float32;
typedef double Float64;
typedef char Char;
typedef char *String;
typedef Uint8 Bool;
typedef Int32 Counter;
typedef void *Pointer;
typedef Uint16 EASLanguageT;
typedef Uint8 Uchar;
typedef Uint16 Wchar;
typedef Uint32 EASTimeT;

typedef enum {
	SFIO_DEVICE_INVALID = -1,
	SFIO_DEVICE_FIRST = 0,
	SFIO_DEVICE_SLOT1A = 0,
	SFIO_DEVICE_SLOT1B = 1,
	SFIO_DEVICE_SLOT1C = 2,
	SFIO_DEVICE_SLOT1D = 3,
	SFIO_DEVICE_SLOT2A = 4,
	SFIO_DEVICE_SLOT2B = 5,
	SFIO_DEVICE_SLOT2C = 6,
	SFIO_DEVICE_SLOT2D = 7,
	SFIO_DEVICE_LAST = 7,
	SFIO_DEVICE_MAX = 8,
	SFIO_DEVICE_ALL = 9,
	SFIO_DEVICE_FORCE32BIT = 2147483647
} SFIODeviceE;

struct _SFIODeviceInformation_t { // 0x2
	/* 0x0 */ Uint8 uPort;
	/* 0x1 */ Uint8 uSlot;
};

typedef _SFIODeviceInformation_t SFIODeviceInformationT;

struct _SFIOSystemData_t { // 0x3d0
	/* 0x000 */ void *pIconBuffer;
	/* 0x004 */ Uint32 uNumBytesIcon;
	/* 0x008 */ sceMcIconSys pSysData;
	/* 0x3cc */ Uint32 uNumBytesData;
};

typedef _SFIOSystemData_t SFIOSystemDataT;
typedef Uint32 SFIOSignatureT;

struct ChecksumResult_t { // 0x8
	/* 0x0 */ Uint32 uSize;
	/* 0x4 */ Uint8 *pChecksum;
};

typedef ChecksumResult_t ChecksumResultT;

typedef enum {
	CHECKSUM_ERROR_NONE = 0,
	CHECKSUM_ERROR_OUTOFMEMORY = 1,
	CHECKSUM_ERROR_INVALIDPARAMETERS = 2,
	CHECKSUM_ERROR_STARTED = 3,
	CHECKSUM_ERROR_NOTSTARTED = 4,
	CHECKSUM_ERROR_INITIALIZED = 5,
	CHECKSUM_ERROR_NOTINITIALIZED = 6,
	CHECKSUM_ERROR_INTERNAL = 7,
	CHECKSUM_ERROR_FORCE32BIT = 2147483647
} ChecksumErrorE;

typedef ChecksumErrorE (*ChecksumInitT)(/* parameters unknown */);
typedef ChecksumErrorE (*ChecksumShutdownT)(/* parameters unknown */);
typedef Uint8 (*ChecksumIsInitializedT)(/* parameters unknown */);
typedef ChecksumErrorE (*ChecksumStartT)(/* parameters unknown */);
typedef ChecksumErrorE (*ChecksumCalculateT)(/* parameters unknown */);
typedef ChecksumErrorE (*ChecksumStopT)(/* parameters unknown */);
typedef ChecksumErrorE (*ChecksumGetSizeT)(/* parameters unknown */);

struct ChecksumType_t { // 0x1c
	/* 0x00 */ ChecksumInitT Init;
	/* 0x04 */ ChecksumShutdownT Shutdown;
	/* 0x08 */ ChecksumIsInitializedT IsInitialized;
	/* 0x0c */ ChecksumStartT Start;
	/* 0x10 */ ChecksumCalculateT Calculate;
	/* 0x14 */ ChecksumStopT Stop;
	/* 0x18 */ ChecksumGetSizeT GetSize;
};

typedef ChecksumType_t ChecksumTypeT;

typedef enum {
	SFIO_SORT_INVALID = -1,
	SFIO_SORT_ASCENDING = 0,
	SFIO_SORT_DESCENDING = 1,
	SFIO_SORT_FORCE32BIT = 2147483647
} SFIOSortE;

typedef enum {
	SFIO_TERRITORY_US = 0,
	SFIO_TERRITORY_EUROPE = 1,
	SFIO_TERRITORY_JAPAN = 2,
	SFIO_TERRITORY_KOREAN = 3
} SFIOTerritoryE;

typedef enum {
	SFIO_PROCESS_NONE = 0,
	SFIO_PROCESS_CONTINUE = 1,
	SFIO_PROCESS_COMPLETE = 2,
	SFIO_PROCESS_FORCE32BIT = 2147483647
} SFIOProcessE;

typedef enum {
	SFIO_ERROR_NONE = 0,
	SFIO_ERROR_INITIALIZED = 1,
	SFIO_ERROR_NOTINITIALIZED = 2,
	SFIO_ERROR_NODEVICE = 3,
	SFIO_ERROR_NOFILE = 4,
	SFIO_ERROR_FILEINUSE = 5,
	SFIO_ERROR_NOTEMPTY = 6,
	SFIO_ERROR_OUTOFMEMORY = 7,
	SFIO_ERROR_FILEALREADYEXISTS = 8,
	SFIO_ERROR_INVALIDDEVICE = 9,
	SFIO_ERROR_RESULTOUTOFRANGE = 10,
	SFIO_ERROR_UNSUPPORTEDDEVICE = 11,
	SFIO_ERROR_INVALIDPARAMETERS = 12,
	SFIO_ERROR_INVALIDCALL = 13,
	SFIO_ERROR_NOTENOUGHSPACE = 14,
	SFIO_ERROR_DIDNOTTERMINATE = 15,
	SFIO_ERROR_UNEXPECTEDERROR = 16,
	SFIO_ERROR_UNKNOWN = 17,
	SFIO_ERROR_INTERNAL = 18,
	SFIO_ERROR_MAX = 19,
	SFIO_ERROR_FORCE32BIT = 2147483647
} SFIOErrorE;

typedef enum {
	SFIO_SEEK_SET = 0,
	SFIO_SEEK_CUR = 1,
	SFIO_SEEK_FORCE32BIT = 2147483647
} SFIOSeekE;

typedef void (*SFIOFindCallbackT)(/* parameters unknown */);
typedef void (*SFIOFreeSpaceCallbackT)(/* parameters unknown */);
typedef void (*SFIOFreeEntryCallbackT)(/* parameters unknown */);
typedef void (*SFIOMountCallbackT)(/* parameters unknown */);
typedef void (*SFIOUnMountCallbackT)(/* parameters unknown */);
typedef void (*SFIOOpenCallbackT)(/* parameters unknown */);
typedef void (*SFIOCloseCallbackT)(/* parameters unknown */);
typedef void (*SFIOCreateCallbackT)(/* parameters unknown */);
typedef void (*SFIODeleteCallbackT)(/* parameters unknown */);
typedef void (*SFIODestroyCallbackT)(/* parameters unknown */);
typedef void (*SFIOReadCallbackT)(/* parameters unknown */);
typedef void (*SFIOWriteCallbackT)(/* parameters unknown */);
typedef void (*SFIOSeekCallbackT)(/* parameters unknown */);
typedef void (*SFIOFlushCallbackT)(/* parameters unknown */);
typedef void (*SFIOSetAttrCallbackT)(/* parameters unknown */);
typedef SFIOErrorE (*SFIOProcessCallbackT)(/* parameters unknown */);
typedef void (*SFIOGetFileCallbackT)(/* parameters unknown */);
typedef SFIOTerritoryE (*SFIOGetTerritoryCallbackT)(/* parameters unknown */);

struct _SFIOCallbacks_t { // 0x48
	/* 0x00 */ SFIOFindCallbackT FindCallback;
	/* 0x04 */ SFIOFreeSpaceCallbackT FreeSpaceCallback;
	/* 0x08 */ SFIOFreeEntryCallbackT FreeEntryCallback;
	/* 0x0c */ SFIOMountCallbackT MountCallback;
	/* 0x10 */ SFIOUnMountCallbackT UnMountCallback;
	/* 0x14 */ SFIOOpenCallbackT OpenCallback;
	/* 0x18 */ SFIOCloseCallbackT CloseCallback;
	/* 0x1c */ SFIOCreateCallbackT CreateCallback;
	/* 0x20 */ SFIODeleteCallbackT DeleteCallback;
	/* 0x24 */ SFIODestroyCallbackT DestroyCallback;
	/* 0x28 */ SFIOReadCallbackT ReadCallback;
	/* 0x2c */ SFIOWriteCallbackT WriteCallback;
	/* 0x30 */ SFIOSeekCallbackT SeekCallback;
	/* 0x34 */ SFIOFlushCallbackT FlushCallback;
	/* 0x38 */ SFIOSetAttrCallbackT SetAttrCallback;
	/* 0x3c */ SFIOProcessCallbackT ProcessCallback;
	/* 0x40 */ SFIOGetFileCallbackT GetFileCallback;
	/* 0x44 */ SFIOGetTerritoryCallbackT GetTerritoryCallback;
};

typedef _SFIOCallbacks_t SFIOCallbacksT;

struct _SFIOFileDescriptor_t { // 0x60
	/* 0x00 */ Int32 iHandle;
	/* 0x04 */ SFIODeviceE eDevice;
	/* 0x08 */ Char aDirectory[32];
	/* 0x28 */ Char aFilename[32];
	/* 0x48 */ Char aSharedName[23];
};

typedef _SFIOFileDescriptor_t SFIOFileDescriptorT;

typedef enum {
	CRYPT_ERROR_NONE = 0,
	CRYPT_ERROR_OUTOFMEMORY = 1,
	CRYPT_ERROR_INVALIDPARAMETERS = 2,
	CRYPT_ERROR_INITIALIZED = 3,
	CRYPT_ERROR_NOTINITIALIZED = 4,
	CRYPT_ERROR_STARTED = 5,
	CRYPT_ERROR_NOTSTARTED = 6,
	CRYPT_ERROR_ENCRYPTING = 7,
	CRYPT_ERROR_DECRYPTING = 8,
	CRYPT_ERROR_INTERNAL = 9,
	CRYPT_ERROR_FORCE32BIT = 2147483647
} CryptErrorE;

typedef CryptErrorE (*CryptInitT)(/* parameters unknown */);
typedef CryptErrorE (*CryptShutdownT)(/* parameters unknown */);
typedef Uint8 (*CryptIsInitializedT)(/* parameters unknown */);
typedef CryptErrorE (*CryptStartT)(/* parameters unknown */);
typedef CryptErrorE (*CryptEncryptT)(/* parameters unknown */);
typedef CryptErrorE (*CryptDecryptT)(/* parameters unknown */);
typedef CryptErrorE (*CryptStopT)(/* parameters unknown */);

struct CryptType_t { // 0x1c
	/* 0x00 */ CryptInitT Init;
	/* 0x04 */ CryptShutdownT Shutdown;
	/* 0x08 */ CryptIsInitializedT IsInitialized;
	/* 0x0c */ CryptStartT Start;
	/* 0x10 */ CryptEncryptT Encrypt;
	/* 0x14 */ CryptDecryptT Decrypt;
	/* 0x18 */ CryptStopT Stop;
};

typedef CryptType_t CryptTypeT;
typedef Uint32 TagT;

typedef enum {
	TAG_ERROR_TAGFIRST = 0,
	TAG_ERROR_NONE = 0,
	TAG_ERROR_INITIALIZED = 1,
	TAG_ERROR_NOTINITIALIZED = 2,
	TAG_ERROR_EOF = 3,
	TAG_ERROR_INVALIDPARAMETERS = 4,
	TAG_ERROR_TOOMANYTAGS = 5,
	TAG_ERROR_OUTOFMEMORY = 6,
	TAG_ERROR_TAGNOTFOUND = 7,
	TAG_ERROR_DIDNOTTERMINATE = 8,
	TAG_ERROR_IDMISMATCH = 9,
	TAG_ERROR_INVALIDCHECKSUM = 10,
	TAG_ERROR_INTERNAL = 11,
	TAG_ERROR_SFIOFIRST = 100,
	TAG_ERROR_SFIO_INITIALIZED = 101,
	TAG_ERROR_SFIO_NOTINITIALIZED = 102,
	TAG_ERROR_SFIO_NODEVICE = 103,
	TAG_ERROR_SFIO_NOFILE = 104,
	TAG_ERROR_SFIO_FILEINUSE = 105,
	TAG_ERROR_SFIO_NOTEMPTY = 106,
	TAG_ERROR_SFIO_OUTOFMEMORY = 107,
	TAG_ERROR_SFIO_FILEALREADYEXISTS = 108,
	TAG_ERROR_SFIO_INVALIDDEVICE = 109,
	TAG_ERROR_SFIO_RESULTOUTOFRANGE = 110,
	TAG_ERROR_SFIO_UNSUPPORTEDDEVICE = 111,
	TAG_ERROR_SFIO_INVALIDPARAMETERS = 112,
	TAG_ERROR_SFIO_INVALIDCALL = 113,
	TAG_ERROR_SFIO_NOTENOUGHSPACE = 114,
	TAG_ERROR_SFIO_DIDNOTTERMINATE = 115,
	TAG_ERROR_SFIO_UNEXPECTEDERROR = 116,
	TAG_ERROR_SFIO_UNKNOWN = 117,
	TAG_ERROR_SFIO_INTERNAL = 118,
	TAG_ERROR_CRYPTFIRST = 200,
	TAG_ERROR_CRYPT_OUTOFMEMORY = 201,
	TAG_ERROR_CRYPT_INVALIDPARAMETERS = 202,
	TAG_ERROR_CRYPT_INITIALIZED = 203,
	TAG_ERROR_CRYPT_NOTINITIALIZED = 204,
	TAG_ERROR_CRYPT_STARTED = 205,
	TAG_ERROR_CRYPT_NOTSTARTED = 206,
	TAG_ERROR_CRYPT_ENCRYPTING = 207,
	TAG_ERROR_CRYPT_DECRYPTING = 208,
	TAG_ERROR_CRYPT_INTERNAL = 209,
	TAG_ERROR_CHECKSUMFIRST = 300,
	TAG_ERROR_CHECKSUM_OUTOFMEMORY = 301,
	TAG_ERROR_CHECKSUM_INVALIDPARAMETERS = 302,
	TAG_ERROR_CHECKSUM_INITIALIZED = 305,
	TAG_ERROR_CHECKSUM_NOTINITIALIZED = 306,
	TAG_ERROR_CHECKSUM_INTERNAL = 307,
	TAG_ERROR_MAX = 308,
	TAG_ERROR_FORCE32BIT = 2147483647
} TagErrorE;

typedef enum {
	TAG_MODE_ANY = 0,
	TAG_MODE_READ = 1,
	TAG_MODE_WRITE = 2,
	TAG_MODE_READWRITE = 3,
	TAG_MODE_NONEIO = 4,
	TAG_MODE_FORCE32BIT = 2147483647
} TagModeE;

struct _TagFileDescriptor_t { // 0x64
	/* 0x00 */ SFIOFileDescriptorT SFIODesc;
	/* 0x60 */ SFIOSignatureT FileID;
};

typedef _TagFileDescriptor_t TagFileDescriptorT;

struct _TagFileParams_t { // 0x1c
	/* 0x00 */ Uint32 uMaxTags;
	/* 0x04 */ SFIODeviceE *pDeviceList;
	/* 0x08 */ SFIOCallbacksT *pCallbacks;
	/* 0x0c */ void *pKey;
	/* 0x10 */ Uint32 uKeySize;
	/* 0x14 */ CryptTypeT *pCrypt;
	/* 0x18 */ Uint32 uMemID;
};

typedef _TagFileParams_t TagFileParamsT;

typedef enum {
	SLOGIN_PROCESS_NONE = 0,
	SLOGIN_PROCESS_CONTINUE = 1,
	SLOGIN_PROCESS_COMPLETE = 2,
	SLOGIN_PROCESS_FORCE32BIT = 2147483647
} SLoginProcessE;

typedef enum {
	SLOGIN_ERROR_NONE = 0,
	SLOGIN_ERROR_NOFILE = 1,
	SLOGIN_ERROR_NODEVICE = 2,
	SLOGIN_ERROR_NOT_ENOUGH_SPACE = 3,
	SLOGIN_ERROR_FILE_EXISTS = 4,
	SLOGIN_ERROR_FILE_CORRUPT = 5,
	SLOGIN_ERROR_WRONG_FILE = 6,
	SLOGIN_ERROR_INVALIDPARAMETERS = 7,
	SLOGIN_ERROR_CALLBACK_NOT_SET = 8,
	SLOGIN_ERROR_STRING_TOO_LARGE = 9,
	SLOGIN_ERROR_STRING_TOO_SMALL = 10,
	SLOGIN_ERROR_OUT_OF_MEMORY = 11,
	SLOGIN_ERROR_ACTION_INVALID = 12,
	SLOGIN_ERROR_ACTION_UNKNOWN = 13,
	SLOGIN_ERROR_STATE_INVALID = 14,
	SLOGIN_ERROR_STATE_UNKNOWN = 15,
	SLOGIN_ERROR_STEP_INVALID = 16,
	SLOGIN_ERROR_STEP_UNKNOWN = 17,
	SLOGIN_ERROR_PROCESS_UNKNOWN = 18,
	SLOGIN_ERROR_INITIALIZED = 19,
	SLOGIN_ERROR_NOTINITIALIZED = 20,
	SLOGIN_ERROR_ICON_INVALID = 21,
	SLOGIN_ERROR_PROCESS_IN_PROGRESS = 22,
	SLOGIN_ERROR_NO_PROCESS_IN_PROGRESS = 23,
	SLOGIN_ERROR_UNKNOWN = 24,
	SLOGIN_ERROR_FORCE32BIT = 2147483647
} SLoginErrorE;

typedef enum {
	SLOGIN_STATE_CREATE = 0,
	SLOGIN_STATE_SAVE = 1,
	SLOGIN_STATE_LOAD = 2,
	SLOGIN_STATE_DELETE = 3,
	SLOGIN_STATECONTROL_NONE = 4,
	SLOGIN_STATE_FORCE32BIT = 2147483647
} SLoginStateE;

typedef enum {
	SLOGIN_ACTION_TAGFILE_CREATE = 0,
	SLOGIN_ACTION_TAGFILE_OPEN = 1,
	SLOGIN_ACTION_TAGFILE_WRITE_ACCOUNT_NAME = 2,
	SLOGIN_ACTION_TAGFILE_WRITE_ACCOUNT_PASSWORD = 3,
	SLOGIN_ACTION_TAGFILE_READ_ACCOUNT_NAME = 4,
	SLOGIN_ACTION_TAGFILE_READ_ACCOUNT_PASSWORD = 5,
	SLOGIN_ACTION_TAGFILE_CLOSE = 6,
	SLOGIN_ACTION_TAGFILE_CLOSE_ERROR_OCCURRED = 7,
	SLOGIN_ACTION_TAGFILE_DELETE = 8,
	SLOGIN_ACTIONCONTROL_STARTUP = 9,
	SLOGIN_ACTIONCONTROL_SHUTDOWN = 10,
	SLOGIN_ACTION_FORCE32BIT = 2147483647
} SLoginActionE;

typedef enum {
	SLOGIN_STEP_INDEX_START = 0,
	SLOGIN_STEP_START = 0,
	SLOGIN_STEP_PROCESS = 1,
	SLOGIN_STEP_ABORT = 2,
	SLOGIN_STEP_INDEX_END = 2,
	SLOGIN_STEP_INDEX_MAX = 3,
	SLOGIN_STEPCONTROL_STARTUP = 4,
	SLOGIN_STEPCONTROL_SHUTDOWN = 5,
	SLOGIN_STEP_FORCE32BIT = 2147483647
} SLoginStepE;

struct SLoginAccountData_t { // 0x24
	/* 0x00 */ Char strAccountName[17];
	/* 0x11 */ Char strAccountPassword[17];
	/* 0x22 */ Uint8 uPad[2];
};

typedef SLoginAccountData_t SLoginAccountDataT;

struct SLoginInitParams_t { // 0x8
	/* 0x0 */ SFIOCallbacksT *pSFIOCallbacks;
	/* 0x4 */ Uint32 uHeapID;
};

typedef SLoginInitParams_t SLoginInitParamsT;

enum TerritoryID {
	TERRITORY_US = 0,
	TERRITORY_EUROPE = 1,
	TERRITORY_JAPAN = 2,
	TERRITORY_KOREAN = 3
};

enum UpdateStatus {
	UPDATE_IDLE = 0,
	UPDATE_BUSY = 2,
	UPDATE_MESSAGE_AVAILABLE = 4,
	UPDATE_FINISHED = 8,
	UPDATE_CLOSE_ON_CONFIRMATION = 16,
	UPDATE_TIMED_MESSAGE_SAVING = 32
};

enum SHARED_LOGIN_SIZES {
	SHARED_LOGIN_ACCOUNT_NAME_SIZE = 17,
	SHARED_LOGIN_ACCOUNT_PASSWORD_SIZE = 17
};

struct SharedLoginData { // 0x22
	/* 0x00 */ char mAccountName[17];
	/* 0x11 */ char mAccountPassword[17];

	SharedLoginData();
	SharedLoginData();
	SharedLoginData& operator=();
	void Reset();
};

enum CARD_ERROR {
	CARD_ERROR_NONE = 0,
	CARD_ERROR_NO_CARD = 1,
	CARD_ERROR_CARD_NOT_FORMATTED = 2,
	CARD_ERROR_CARD_CHANGED = 3,
	CARD_ERROR_CARD_UNKNOWN = 4
};

enum CARD_EXTENDED_ERROR {
	CARD_EXTENDED_ERROR_NONE = 0,
	CARD_EXTENDED_ERROR_CORRUPT = 1,
	CARD_EXTENDED_ERROR_DAMAGED = 2,
	CARD_EXTENDED_ERROR_WRONG_DEVICE = 3,
	CARD_EXTENDED_ERROR_UNKNOWN = 4
};

enum RESPONSE_OPTION {
	RESPONSE_OPTION_1 = 0,
	RESPONSE_OPTION_2 = 1,
	RESPONSE_OPTION_3 = 2,
	RESPONSE_OPTION_4 = 3,
	RESPONSE_OPTION_NUM_OPTIONS = 4,
	RESPONSE_OPTION_NONE = 5,
	RESPONSE_OPTION_OK = 6,
	RESPONSE_OPTION_SAVE_NEW = 7,
	RESPONSE_OPTION_SAVE_EXISTING = 8,
	RESPONSE_OPTION_SAVE_COMPLETE = 9,
	RESPONSE_OPTION_CANCEL_SAVE_NEW = 10,
	RESPONSE_OPTION_CANCEL_SAVE_EXISTING = 11,
	RESPONSE_OPTION_CANCEL = 12,
	RESPONSE_OPTION_DELETE = 13,
	RESPONSE_OPTION_DELETE_CORRUPTED = 14,
	RESPONSE_OPTION_CANCEL_DELETE_CORRUPTED = 15,
	RESPONSE_OPTION_INIT_RETRY = 16,
	RESPONSE_OPTION_DELETE_CONFIRM = 17,
	RESPONSE_OPTION_OVERWRITE = 18,
	RESPONSE_OPTION_OVERWRITE_CANCEL = 19,
	RESPONSE_OPTION_REPAIR = 20,
	RESPONSE_OPTION_CONTINUE_WITHOUT_SAVING = 21,
	RESPONSE_OPTION_FORMAT_YES = 22,
	RESPONSE_OPTION_FORMAT_NO = 23,
	RESPONSE_OPTION_OVERWRITE_THROUGH_DELETE = 24,
	RESPONSE_OPTION_INSUFFICIENT_SPACE_RETRY = 25,
	RESPONSE_OPTION_YES = 0,
	RESPONSE_OPTION_NO = 1,
	RESPONSE_OPTION_RETRY = 2
};

struct MessageOption { // 0x8
	/* 0x0 */ RESPONSE_OPTION mType;
	/* 0x4 */ char *mName;
};

// warning: multiple differing types with the same name (size not equal)
struct Message { // 0x3c
	/* 0x00 */ CARD_ERROR mCardError;
	/* 0x04 */ CARD_EXTENDED_ERROR mCardExtendedError;
	/* 0x08 */ UpdateStatus mUpdateStatus;
	/* 0x0c */ int mDefaultOption;
	/* 0x10 */ int mCurrentLibOp;
	/* 0x14 */ bool mMessageDirty;
	/* 0x18 */ char *mMessageID;
	/* 0x1c */ MessageOption mOptions[4];

	Message& operator=();
	Message();
	Message();
	void Reset();
};

// warning: multiple differing types with the same name (size not equal)
struct ConfigSettings { // 0x18
	/* 0x00 */ TerritoryID mTerritoryID;
	/* 0x04 */ IAllocator *mAllocator;
	/* 0x08 */ Interface *mMemcardInterface;
	/* 0x0c */ void *mLoginIcon;
	/* 0x10 */ int mTimerFrequency;
	/* 0x14 */ IThread *mThread;
};

struct SharedLoginInterface { // 0x4
	/* 0x0 */ __vtbl_ptr_type *$vf775;

	SharedLoginInterface& operator=();
	SharedLoginInterface();
	SharedLoginInterface();
	static SharedLoginInterface* CreateInstance(/* parameters unknown */);
	/* vtable[1] */ virtual int AddRef();
	/* vtable[2] */ virtual int Release();
	/* vtable[3] */ virtual void Startup();
	/* vtable[4] */ virtual void Shutdown();
	/* vtable[5] */ virtual void CheckCard();
	/* vtable[6] */ virtual void Load();
	/* vtable[7] */ virtual void Save();
	/* vtable[8] */ virtual void SetSharedLoginData();
	/* vtable[9] */ virtual void GetSharedLoginData();
	/* vtable[10] */ virtual unsigned int GetExpectedFileSize();
	/* vtable[11] */ virtual void GetMessage();
	/* vtable[12] */ virtual void SendUserResponse();
	/* vtable[13] */ virtual UpdateStatus Update();
};

typedef short unsigned int USTR;

enum DebugIOchannels {
	DBG_DEFAULT = 0,
	DBG_REAL_LIBRARY = 1,
	DBG_ERROR = 2,
	DBG_AI = 3,
	DBG_AI_ASS = 4,
	DBG_AI_EVENTS = 5,
	DBG_AI_INIT = 6,
	DBG_AI_INBOUND = 7,
	DBG_AI_SHOOT = 8,
	DBG_AI_PASS = 9,
	DBG_AI_SCENARIO = 10,
	DBG_AI_DRIVE = 11,
	DBG_AI_STEAL = 12,
	DBG_AI_SUB = 13,
	DBG_AI_PLAY = 14,
	DBG_AI_DYNOFF = 15,
	DBG_AI_TRANS = 16,
	DBG_AI_STATS = 17,
	DBG_AI_HEAD = 18,
	DBG_AI_ANIM = 19,
	DBG_AI_REPLAY = 20,
	DBG_AI_COLLISIONS = 21,
	DBG_ANIM = 22,
	DBG_ANIM_TE = 23,
	DBG_AUDIO = 24,
	DBG_AUDIO_SPEECH = 25,
	DBG_CONSOLE = 26,
	DBG_CONSOLE_CMDPROC = 27,
	DBG_COMMS_GENERAL = 28,
	DBG_COMMS_CONNECT = 29,
	DBG_COMMS_COMMAND = 30,
	DBG_COMMS_PACKET = 31,
	DBG_DATABASE = 32,
	DBG_FRONTEND = 33,
	DBG_FRONTEND_CPUTRADEAI = 34,
	DBG_FRONTEND_WARNING = 35,
	DBG_INTERFACE = 36,
	DBG_IORECORDING = 37,
	DBG_RENDER = 38,
	DBG_RENDER_TRACE = 39,
	DBG_RENDER_TE = 40,
	DBG_AMGENERAL = 41,
	DBG_AMDEBUG0 = 42,
	DBG_AMDEBUG1 = 43,
	DBG_AMDEBUG2 = 44,
	DBG_SIM = 45,
	DBG_SYS = 46,
	DBG_SYS_MEM = 47,
	DBG_SYS_FILE = 48,
	DBG_SYS_LOCALE = 49,
	DBG_SYS_LOCALE_CACHE = 50,
	DBG_SYS_PROFILER = 51,
	DBG_SCRIPTER = 52,
	DBG_UBI = 53,
	DBG_RCMP = 54,
	DBG_MAX = 55
};

enum SFIOLocalMessageE {
	SFIOLOCAL_BUSY = 0,
	SFIOLOCAL_FREE_ENTRY_DONE = 1,
	SFIOLOCAL_ERROR_COND = 2
};

enum SFIOTestProcessTypeE {
	MemcardLibraryProcess = 0,
	NonMemcardLibraryProcess = 1
};

enum LIBFunctionTypeE {
	LIB_FUN_NONE = 0,
	LIB_FIND_FILE = 1,
	LIB_FLUSH_FILE = 2,
	LIB_FREE_ENTRY = 3,
	LIB_FREE_SPACE = 4,
	LIB_MOUNT = 5,
	LIB_OPEN_FILE = 6,
	LIB_READ_FILE = 7,
	LIB_FILE_SEEK = 8,
	LIB_SET_ATTR = 9,
	LIB_UNMOUNT = 10,
	LIB_CREATE_SAVE = 11,
	LIB_DELETE_FILE = 12,
	LIB_CLOSE_HANDLE = 13,
	LIB_DESTROY_DIR = 14,
	LIB_WRITE = 15
};

enum SFIO_OVERLAY_STATE {
	SFIO_OVERLAY_STATE_NONE = 0,
	SFIO_OVERLAY_STATE_START_GAME = 1,
	SFIO_OVERLAY_STATE_INIT = 2,
	SFIO_OVERLAY_STATE_SAVE = 3,
	SFIO_OVERLAY_STATE_DELETE = 4
};

enum EASBSpecialMessage {
	EASB_SPECIAL_MESSAGE_NONE = 0,
	EASB_SPECIAL_CHECKING_MESSAGE = 1,
	EASB_SPECIAL_SAVING_MESSAGE = 2,
	EASB_SPECIAL_LOADING_MESSAGE = 3,
	EASB_SPECIAL_LOADING_FAILED = 4,
	EASB_SPECIAL_SAVING_COMPLETE = 5,
	EASB_SPECIAL_SAVING_FAILED_UNKNOWN = 6,
	EASB_SPECIAL_DELETE_VERIFICATION = 7,
	EASB_SPECIAL_DELETE_DOUBLE_VERIFICATION = 8,
	EASB_SPECIAL_DELETE_COMPLETE = 9,
	EASB_SPECIAL_DELETE_FAILED_NO_BIO = 10,
	EASB_SPECIAL_DELETE_FAILED_UNKNOWN = 11,
	EASB_SPECIAL_DELETE_MESSAGE = 12,
	EASB_SPECIAL_DAMAGED_CARD = 13,
	EASB_SPECIAL_FORMATTING_WARNING = 14,
	EASB_SPECIAL_FORMATTING_FAILED = 15,
	EASB_SPECIAL_FORMAT_FAILED_NO_DEVICE = 16,
	EASB_SPECIAL_NO_CARD = 17,
	EASB_SPECIAL_CARD_CHANGED = 18
};

enum SFIO_HARDWARE_OP {
	SFIO_HARDWARE_OP_NONE = 0,
	SFIO_HARDWARE_OP_FORMAT = 1,
	SFIO_HARDWARE_OP_FORMAT_FAILED_NO_CARD_ON_START = 2,
	SFIO_HARDWARE_OP_FORMAT_FAILED_DIFFERENT_CARD = 3
};

enum SFIO_INTERFACE_MODE {
	SFIO_INTERFACE_NONE = 0,
	SFIO_INTERFACE_EASB = 1,
	SFIO_INTERFACE_SHARED_LOGIN = 2
};

struct SFIOTestProcessItem { // 0xc
	/* 0x0 */ SFIOTestProcessTypeE eCurrItemType;
	/* 0x4 */ SFIOTestProcessItem *pNextItem;
	/* 0x8 */ LIBFunctionTypeE eFuncType;
};

struct _FileDiscp { // 0x50
	/* 0x00 */ bool Isused;
	/* 0x04 */ char fileName[33];
	/* 0x26 */ CardID cardID;
	/* 0x2a */ char directoryName[33];
	/* 0x4c */ FileHandle handle;
};

typedef _FileDiscp FileDscpT;

enum MC_ERROR {
	MC_OK = 0,
	MC_NO_CARD = 1,
	MC_NOT_FORMATTED = 2,
	MC_FORMATTING_FAILED = 3,
	MC_UNFORMATTING_FAILED = 4,
	MC_INSUFFICIENT_SPACE = 5,
	MC_BAD_FILENAME = 6,
	MC_TOO_MANY_FILE_HANDLES = 7,
	MC_CORRUPT_SYSTEM_AREA = 8,
	MC_WRONG_ENCODING = 9,
	MC_DAMAGED_CARD = 10,
	MC_WRONG_DEVICE = 11,
	MC_IMPROPER_SECTOR_SIZE = 12,
	MC_SYSTEM_ERROR = 13,
	MC_SYSTEM_BUSY = 14
};

struct SFIOTestProcessTypeQueue { // 0xc
private:
	/* 0x0 */ SFIOTestProcessItem *pFirstElement;
	/* 0x4 */ SFIOTestProcessItem *pLastElement;
	/* 0x8 */ int numItems;

public:
	SFIOTestProcessTypeQueue& operator=();
	SFIOTestProcessTypeQueue();
	SFIOTestProcessTypeQueue();
	SFIOTestProcessTypeQueue(SFIOTestProcessTypeQueue*, int, void);
	void Enqueue();
	SFIOTestProcessItem* Top();
	SFIOTestProcessItem* Dequeue();
};

struct SFIOSpecialMessage { // 0x1c
	/* 0x00 */ char *msgID;
	/* 0x04 */ char *button[3];
	/* 0x10 */ RESPONSE_OPTION option[3];

	SFIOSpecialMessage& operator=();
	SFIOSpecialMessage();
	SFIOSpecialMessage();
	void Clear();
};

struct SFIOInterface { // 0xac
protected:
	/* 0x00 */ bool mLoadingMessageEnabled;
	/* 0x04 */ bool mMessageDirty;
	/* 0x08 */ bool mSpecialMessagePending;
	/* 0x0c */ bool mCardChangedCheck;
	/* 0x10 */ bool mSavingOnLoading;
	/* 0x14 */ int mCheckingCounter;
	/* 0x18 */ Message mCardMessage;
	/* 0x54 */ SFIO_HARDWARE_OP mHardwareOp;
	/* 0x58 */ FileInfo *mFileInfo;
	/* 0x5c */ DirectoryInfo *mDirectoryInfo;
	/* 0x60 */ SFIOCallbacksT mSFIOCallbacksT;
public:
	/* 0xa8 */ __vtbl_ptr_type *$vf833;

	SFIOInterface& operator=();
	SFIOInterface();
	SFIOInterface();
	/* vtable[1] */ virtual SFIOInterface(SFIOInterface*, int, void);
	/* vtable[2] */ virtual void SetNonQueryMessage();
	/* vtable[3] */ virtual unsigned int GetExpectedFileSize();
	/* vtable[4] */ virtual void GetDeleteDoubleVerification();
	/* vtable[5] */ virtual void GetDeleteVerification();
	/* vtable[6] */ virtual void GetDeleteComplete();
	/* vtable[7] */ virtual void GetSavingMessage();
	/* vtable[8] */ virtual void GetLoadingMessage();
	/* vtable[9] */ virtual void GetCheckingMessage();
	/* vtable[10] */ virtual void GetSavingComplete();
	/* vtable[11] */ virtual void GetLoadingFailed();
	/* vtable[12] */ virtual void GetLoadingWarning();
	/* vtable[13] */ virtual void GetDeleteFailedNoBio();
	/* vtable[14] */ virtual void GetSavingFailedUnknown();
	/* vtable[15] */ virtual void GetDeleteFailedUnknown();
	/* vtable[16] */ virtual void GetFormattingWarning();
	/* vtable[17] */ virtual void GetFormattingFailed();
	/* vtable[18] */ virtual void GetDamagedCard();
	/* vtable[19] */ virtual void GetFileCorrupt();
	/* vtable[20] */ virtual void GetFileNeverLoaded();
	/* vtable[21] */ virtual void GetLoadedButDeleted();
	/* vtable[22] */ virtual void GetNoFileButLoaded();
	/* vtable[23] */ virtual void GetNotEnoughSpace();
	/* vtable[24] */ virtual void GetNoDeviceNoCard();
	/* vtable[25] */ virtual void GetNoDeviceNoCardDelete();
	/* vtable[26] */ virtual void GetNoDeviceCardNotFormattedSave();
	/* vtable[27] */ virtual void GetNoDeviceCardNotFormatted();
	/* vtable[28] */ virtual void GetFileOverwrite();
	/* vtable[29] */ virtual void GetWrongFileDelete();
	/* vtable[30] */ virtual void GetWrongFileSave();
	/* vtable[31] */ virtual void GetDeleteMessage();
	/* vtable[32] */ virtual void GetCardChanged();
	/* vtable[33] */ virtual void GetFormatFailedNoDevice();
	/* vtable[34] */ virtual void GetMessage();
	Message& GetMessage();
	void ClearSFIOParams();
	void SetSpecialSFIOMessage();
	FileInfo* GetFileInfo();
	DirectoryInfo* GetDirectoryInfo();
	void SetSpecialMessagePending();
	void SetMessageDirty();
	void ClearCheckingCounter();
	void IncrementCheckingCounter();
	int GetCheckingCounter();
	void DisableLoadingMessage();
	void EnableLoadingMessage();
	bool IsLoadingMessageEnabled();
	void ShowSavingOnLoading();
	void DisableSavingOnLoading();
	bool IsSavingOnLoading();
	void DisableCardChangedCheck();
	void EnableCardChangedCheck();
	bool IsCardChangedCheckEnabled();
	SFIOCallbacksT* GetSFIOcallbacks();
};

// warning: multiple differing types with the same name (enum constant not equal)
enum TaskID {
	TASK_NONE = 0,
	TASK_CHECK_CARD = 1
};

// warning: multiple differing types with the same name (enum constant not equal)
enum TaskState {
	TASKSTATE_NONE = 0,
	TASKSTATE_SHOW_NOTIFICATION = 1,
	TASKSTATE_START_CARD_OPERATION = 2,
	TASKSTATE_FINISHED = 3
};

// warning: multiple differing types with the same name (size not equal)
struct Task { // 0x10
protected:
	/* 0x0 */ TaskState mState;
	/* 0x4 */ UpdateStatus mStatus;
	/* 0x8 */ CardID &mCardID;
public:
	/* 0xc */ __vtbl_ptr_type *$vf854;

	Task& operator=();
	Task();
	Task();
	/* vtable[1] */ virtual Task(Task*, int, void);
	/* vtable[2] */ virtual void Update();
	UpdateStatus GetStatus();
};

struct SharedLoginInitData { // 0x2c
	/* 0x00 */ SLoginAccountDataT mSharedLoginAccountDataT;
	/* 0x24 */ SLoginInitParamsT mSharedLoginInitParamsT;

	SharedLoginInitData& operator=();
	SharedLoginInitData();
	SharedLoginInitData();
	static void* operator new(/* parameters unknown */);
	static void operator delete(/* parameters unknown */);
};

enum SHARED_LOGIN_STATE {
	SHARED_LOGIN_STATE_IDLE = 0,
	SHARED_LOGIN_STATE_LOAD = 1,
	SHARED_LOGIN_STATE_FORMAT_CARD = 2,
	SHARED_LOGIN_STATE_FORMAT_CARD_STEP1 = 3,
	SHARED_LOGIN_STATE_FORMAT = 4,
	SHARED_LOGIN_STATE_FORMAT_FOR_SAVING = 5,
	SHARED_LOGIN_STATE_DELETE = 6,
	SHARED_LOGIN_STATE_DELETE_FAILED = 7,
	SHARED_LOGIN_STATE_DELETE_FAILED_NO_LOGIN = 8,
	SHARED_LOGIN_STATE_DELETE_COMPLETE = 9,
	SHARED_LOGIN_STATE_SAVE = 10,
	SHARED_LOGIN_STATE_SAVE_COMPLETE = 11,
	SHARED_LOGIN_STATE_SAVE_FAILED_UNKNOWN = 12,
	SHARED_LOGIN_STATE_DELETE_CONFIRM = 13,
	SHARED_LOGIN_STATE_DELETE_DOUBLE_CONFIRM = 14,
	SHARED_LOGIN_STATE_CLOSE_FOR_DELETE = 15,
	SHARED_LOGIN_STATE_PROCESS_DELETE = 16,
	SHARED_LOGIN_STATE_RETRY = 17,
	SHARED_LOGIN_STATE_CLOSE = 18,
	SHARED_LOGIN_STATE_CLOSE_DONE = 19,
	SHARED_LOGIN_STATE_FORCE_SAVE = 20,
	SHARED_LOGIN_STATE_DONE = 21
};

struct SharedLoginInterfaceImp : /* 0x00 */ SharedLoginInterface, /* 0x20 */ SFIOInterface { // 0x100
private:
	/* 0xb0 */ SharedLoginInitData *mSharedLoginInitData;
	/* 0xb4 */ SharedLoginData mCurrentSharedLogin;
	/* 0xd8 */ SHARED_LOGIN_STATE mCurrentState;
	/* 0xdc */ SHARED_LOGIN_STATE mNextState;
	/* 0xe0 */ int mRefcount;
	/* 0xe4 */ UpdateStatus mStatus;
	/* 0xe8 */ Task *mCurrentTask;
	/* 0xec */ bool mOpFinished;
	/* 0xf0 */ bool mWaitingForReply;
	/* 0xf4 */ bool mLoginChanged;
	/* 0xf8 */ bool mForceOverwrite;
	/* 0xfc */ bool mSLoginInitialized;

public:
	SharedLoginInterfaceImp& operator=();
	SharedLoginInterfaceImp();
	SharedLoginInterfaceImp();
	/* vtable[1] */ virtual SharedLoginInterfaceImp(SharedLoginInterfaceImp*, int, void);
	static void* operator new(/* parameters unknown */);
	static void operator delete(/* parameters unknown */);
	/* vtable[1] */ virtual int AddRef();
	/* vtable[2] */ virtual int Release();
	/* vtable[10] */ virtual unsigned int GetExpectedFileSize();
	/* vtable[3] */ virtual void Startup();
	/* vtable[4] */ virtual void Shutdown();
	/* vtable[5] */ virtual void CheckCard();
	/* vtable[6] */ virtual void Load();
	/* vtable[7] */ virtual void Save();
	/* vtable[13] */ virtual UpdateStatus Update();
	/* vtable[12] */ virtual void SendUserResponse();
	/* vtable[8] */ virtual void SetSharedLoginData();
	/* vtable[9] */ virtual void GetSharedLoginData();
	/* vtable[11] */ virtual void GetMessage();
	/* vtable[2] */ virtual void SetNonQueryMessage();
private:
	void LoginInit();
	void LoginShutdown();
	void Create();
	void SetSharedLoginMessage();
	void SetSpecialMessage();
	void CheckHardwareOp();
	void SetNextState();
	void HandleError();
	void Format();
	void Delete();
	void DeleteForOverwrite();
	UpdateStatus CheckSfioOperation();
	/* vtable[5] */ virtual void GetDeleteVerification();
	/* vtable[31] */ virtual void GetDeleteMessage();
	/* vtable[4] */ virtual void GetDeleteDoubleVerification();
	/* vtable[6] */ virtual void GetDeleteComplete();
	/* vtable[7] */ virtual void GetSavingMessage();
	/* vtable[8] */ virtual void GetLoadingMessage();
	/* vtable[9] */ virtual void GetCheckingMessage();
	/* vtable[10] */ virtual void GetSavingComplete();
	/* vtable[11] */ virtual void GetLoadingFailed();
	/* vtable[12] */ virtual void GetLoadingWarning();
	/* vtable[13] */ virtual void GetDeleteFailedNoBio();
	/* vtable[14] */ virtual void GetSavingFailedUnknown();
	/* vtable[15] */ virtual void GetDeleteFailedUnknown();
	/* vtable[16] */ virtual void GetFormattingWarning();
	/* vtable[17] */ virtual void GetFormattingFailed();
	/* vtable[19] */ virtual void GetFileCorrupt();
	/* vtable[20] */ virtual void GetFileNeverLoaded();
	/* vtable[23] */ virtual void GetNotEnoughSpace();
	/* vtable[24] */ virtual void GetNoDeviceNoCard();
	/* vtable[25] */ virtual void GetNoDeviceNoCardDelete();
	/* vtable[26] */ virtual void GetNoDeviceCardNotFormattedSave();
	/* vtable[27] */ virtual void GetNoDeviceCardNotFormatted();
	/* vtable[28] */ virtual void GetFileOverwrite();
	/* vtable[32] */ virtual void GetCardChanged();
};

struct TaskCheckCard : /* 0x00 */ Task { // 0x14
private:
	/* 0x10 */ bool mCardOperationFinished;

public:
	TaskCheckCard& operator=();
	TaskCheckCard();
	TaskCheckCard();
	/* vtable[1] */ virtual TaskCheckCard(TaskCheckCard*, int, void);
	static void* operator new(/* parameters unknown */);
	static void operator delete(/* parameters unknown */);
	/* vtable[2] */ virtual void Update();
private:
	void CheckCard();
};

typedef SLoginErrorE SLoginStateProcessCBT(/* parameters unknown */);
typedef SLoginErrorE SLoginActionProcessCBT(/* parameters unknown */);
typedef SLoginErrorE SLoginStepStartCBT(/* parameters unknown */);
typedef SLoginErrorE SLoginStepProcessCBT(/* parameters unknown */);
typedef SLoginErrorE SLoginStepAbortCBT(/* parameters unknown */);
typedef void SLoginActionProcessCompleteCBT(/* parameters unknown */);
typedef void SLoginActionProcessFailCBT(/* parameters unknown */);
typedef void SLoginActionControlProcessStartCBT(/* parameters unknown */);
typedef SLoginErrorE SLoginStepProcessCompleteCBT(/* parameters unknown */);
typedef SLoginErrorE SLoginStepProcessFailCBT(/* parameters unknown */);

struct SLoginStep_t { // 0x18
	/* 0x00 */ SLoginStepE eStep;
	/* 0x04 */ void *aStepCB[3];
	/* 0x10 */ SLoginStepProcessCompleteCBT *pStepProcessCompleteCB;
	/* 0x14 */ SLoginStepProcessFailCBT *pStepProcessFailCB;
};

typedef SLoginStep_t SLoginStepT;

struct SLoginAction_t { // 0x2c
	/* 0x00 */ SLoginActionE eAction;
	/* 0x04 */ SLoginActionProcessCBT *pActionProcessCB;
	/* 0x08 */ SLoginActionProcessCompleteCBT *pActionProcessCompleteCB;
	/* 0x0c */ SLoginActionProcessFailCBT *pActionProcessFailCB;
	/* 0x10 */ SLoginActionControlProcessStartCBT *pActionControlProcessStartCB;
	/* 0x14 */ SLoginStepT Step;
};

typedef SLoginAction_t SLoginActionT;

struct SLoginState_t { // 0x34
	/* 0x00 */ SLoginStateE eState;
	/* 0x04 */ SLoginStateProcessCBT *pStateProcessCB;
	/* 0x08 */ SLoginActionT Action;
};

typedef SLoginState_t SLoginStateT;

struct SLoginStaticData_t { // 0x4
	/* 0x0 */ Bool bStartup;
	/* 0x1 */ Uint8 uPad[3];
};

typedef SLoginStaticData_t SLoginStaticDataT;

struct SLoginDynamicData_t { // 0x150
	/* 0x000 */ SFIOCallbacksT SFIOCallbacks;
	/* 0x048 */ TagFileDescriptorT FileDescriptor;
	/* 0x0ac */ SLoginAccountDataT AccountData;
	/* 0x0d0 */ SLoginAccountDataT *pLoadAccountData;
	/* 0x0d4 */ SLoginStateT State;
	/* 0x108 */ SLoginStateT StatePrevious;
	/* 0x13c */ SFIODeviceE aSFIODeviceList[2];
	/* 0x144 */ Uint32 uHeapID;
	/* 0x148 */ void *pTagFileBuffer;
	/* 0x14c */ SLoginErrorE eSLoginErrorPrevious;
};

typedef SLoginDynamicData_t SLoginDynamicDataT;

typedef enum {
	SFIO_ACTION_NONE = 0,
	SFIO_ACTION_CREATE_MULTIDEVICE = 1,
	SFIO_ACTION_DELETE_MULTIDEVICE = 2,
	SFIO_ACTION_OPEN_MULTIDEVICE = 3,
	SFIO_ACTION_CREATE_SINGLEDEVICE = 4,
	SFIO_ACTION_DELETE_SINGLEDEVICE = 5,
	SFIO_ACTION_OPEN_SINGLEDEVICE = 6,
	SFIO_ACTION_CLOSE = 7,
	SFIO_ACTION_SEEK = 8,
	SFIO_ACTION_WRITE = 9,
	SFIO_ACTION_READ = 10,
	SFIO_ACTION_RECOVER_FLUSH = 11,
	SFIO_ACTION_RECOVER_CLOSE = 12,
	SFIO_ACTION_RECOVER_UNMOUNT = 13,
	SFIO_ACTION_MAX = 14
} _SFIOActionE;

typedef enum {
	SFIO_SUBACTION_NONE = 0,
	SFIO_SUBACTION_MOUNT = 1,
	SFIO_SUBACTION_FIND = 2,
	SFIO_SUBACTION_GETFREESPACE = 3,
	SFIO_SUBACTION_GETFREEENTRY = 4,
	SFIO_SUBACTION_CREATE = 5,
	SFIO_SUBACTION_OPENICON = 6,
	SFIO_SUBACTION_WRITEICON = 7,
	SFIO_SUBACTION_FLUSHICON = 8,
	SFIO_SUBACTION_CLOSEICON = 9,
	SFIO_SUBACTION_OPENSYSTEM = 10,
	SFIO_SUBACTION_WRITESYSTEM = 11,
	SFIO_SUBACTION_FLUSHSYSTEM = 12,
	SFIO_SUBACTION_CLOSESYSTEM = 13,
	SFIO_SUBACTION_OPENDATA = 14,
	SFIO_SUBACTION_WRITEDATA = 15,
	SFIO_SUBACTION_FLUSHDATA = 16,
	SFIO_SUBACTION_CLOSEDATA = 17,
	SFIO_SUBACTION_DESTROY = 18,
	SFIO_SUBACTION_DELETESYSTEM = 19,
	SFIO_SUBACTION_DELETEICON = 20,
	SFIO_SUBACTION_DELETEDATA = 21,
	SFIO_SUBACTION_OPEN = 22,
	SFIO_SUBACTION_SETATTR = 23,
	SFIO_SUBACTION_FLUSH = 24,
	SFIO_SUBACTION_CLOSE = 25,
	SFIO_SUBACTION_UNMOUNT = 26,
	SFIO_SUBACTION_SEEK = 27,
	SFIO_SUBACTION_WRITE = 28,
	SFIO_SUBACTION_READ = 29,
	SFIO_SUBACTION_MAX = 30
} _SFIOSubActionE;

typedef SFIOErrorE (*SFIOProcessFncT)(/* parameters unknown */);
typedef SFIOErrorE (*SFIOValidateFncT)(/* parameters unknown */);

struct _SFIO_ActionData_t { // 0x74
	/* 0x00 */ _SFIOActionE eAction;
	/* 0x04 */ _SFIOSubActionE eSubAction;
	/* 0x08 */ SFIOErrorE eLastError;
	/* 0x0c */ SFIOFileDescriptorT FileDesc;
	/* 0x6c */ SFIOSortE eSort;
	/* 0x70 */ Int32 iExpected;
};

typedef _SFIO_ActionData_t SFIOActionDataT;

struct _SFIOLibraryData_t { // 0x430
	/* 0x000 */ Uint16 uDeviceMask;
	/* 0x004 */ SFIOCallbacksT Callbacks;
	/* 0x04c */ SFIOProcessFncT *pProcessFncs;
	/* 0x050 */ SFIOValidateFncT *pValidateFncs;
	/* 0x054 */ SFIOSystemDataT SysData;
	/* 0x424 */ Uint32 uSpaceRequired;
	/* 0x428 */ Uint32 uEntriesRequired;
	/* 0x42c */ Uint32 uMemID;
};

typedef _SFIOLibraryData_t SFIOLibraryDataT;

typedef enum {
	TAG_ACTION_NONE = 0,
	TAG_ACTION_CREATE = 1,
	TAG_ACTION_DELETE = 2,
	TAG_ACTION_CLOSE = 3,
	TAG_ACTION_OPEN = 4,
	TAG_ACTION_REOPEN = 5,
	TAG_ACTION_WRITE = 6,
	TAG_ACTION_SEEK = 7,
	TAG_ACTION_READ = 8,
	TAG_ACTION_MAX = 9
} _TagActionE;

typedef enum {
	TAG_SUBACTION_NONE = 0,
	TAG_SUBACTION_CREATE = 1,
	TAG_SUBACTION_DELETE = 2,
	TAG_SUBACTION_CLOSE = 3,
	TAG_SUBACTION_OPEN = 4,
	TAG_SUBACTION_SEEK = 5,
	TAG_SUBACTION_WRITEDATA = 6,
	TAG_SUBACTION_READTAG = 7,
	TAG_SUBACTION_SEEKBACK = 8,
	TAG_SUBACTION_PARSEINIT = 9,
	TAG_SUBACTION_READDATA = 10,
	TAG_SUBACTION_SKIPDATA = 11,
	TAG_SUBACTION_MAX = 12
} _TagSubActionE;

typedef Uchar TagBufferT[12];

struct _TagFileMapElement_t { // 0x10
	/* 0x0 */ TagT Tag;
	/* 0x4 */ Int32 iOffset;
	/* 0x8 */ Uint32 uDataNumBytes;
	/* 0xc */ SFIOSignatureT Checksum;
};

typedef _TagFileMapElement_t _TagFileMapElementT;

struct _TagFileMap_t { // 0x10
	/* 0x0 */ Uint32 uMaxTags;
	/* 0x4 */ Uint32 uNumTags;
	/* 0x8 */ Int32 iEOF;
	/* 0xc */ _TagFileMapElementT *pList;
};

typedef _TagFileMap_t _TagFileMapT;

struct _TagFileData_t { // 0xac
	/* 0x00 */ TagFileDescriptorT FileDesc;
	/* 0x64 */ void *pKey;
	/* 0x68 */ Uint32 uKeySize;
	/* 0x6c */ CryptType_t *pCrypt;
	/* 0x70 */ ChecksumType_t *pSignature;
	/* 0x74 */ _TagActionE eAction;
	/* 0x78 */ _TagSubActionE eSubAction;
	/* 0x7c */ void *pBuffer;
	/* 0x80 */ void *pInternalBuffer;
	/* 0x84 */ Uint32 uNumBytes;
	/* 0x88 */ SFIOSignatureT TagChecksum;
	/* 0x8c */ TagT Tag;
	/* 0x90 */ _TagFileMapT Map;
	/* 0xa0 */ Uint32 uMemID;
	/* 0xa4 */ Int32 iLastOffset;
	/* 0xa8 */ Bool bJustCreated;
};

typedef _TagFileData_t _TagFileDataT;

struct CheckSumCRC32Info_t { // 0x14
	/* 0x00 */ Uint32 *pLookupTable;
	/* 0x04 */ ChecksumResultT Result;
	/* 0x0c */ Uint32 uCRC;
	/* 0x10 */ Uint32 uMemID;
};

typedef CheckSumCRC32Info_t CheckSumCRC32InfoT;

enum cmp_type {
	CMP_SI = 0,
	CMP_DI = 1,
	CMP_SF = 2,
	CMP_DF = 3,
	CMP_MAX = 4
};

enum delay_type {
	DELAY_NONE = 0,
	DELAY_LOAD = 1,
	DELAY_HILO = 2,
	DELAY_HILO1 = 3,
	DELAY_FCMP = 4
};

enum processor_type {
	PROCESSOR_DEFAULT = 0,
	PROCESSOR_R3000 = 1,
	PROCESSOR_R3900 = 2,
	PROCESSOR_R6000 = 3,
	PROCESSOR_R4000 = 4,
	PROCESSOR_R4100 = 5,
	PROCESSOR_R4300 = 6,
	PROCESSOR_R4600 = 7,
	PROCESSOR_R4650 = 8,
	PROCESSOR_R5000 = 9,
	PROCESSOR_R5400 = 10,
	PROCESSOR_R5900 = 11,
	PROCESSOR_R8000 = 12
};

enum mips_abicalls_type {
	MIPS_ABICALLS_NO = 0,
	MIPS_ABICALLS_YES = 1
};

enum block_move_type {
	BLOCK_MOVE_NORMAL = 0,
	BLOCK_MOVE_NOT_LAST = 1,
	BLOCK_MOVE_LAST = 2
};

enum reg_class {
	NO_REGS = 0,
	M16_NA_REGS = 1,
	M16_REGS = 2,
	T_REG = 3,
	M16_T_REGS = 4,
	GR_REGS = 5,
	FP_REGS = 6,
	HI_REG = 7,
	LO_REG = 8,
	HILO_REG = 9,
	MD_REGS = 10,
	HI_AND_GR_REGS = 11,
	LO_AND_GR_REGS = 12,
	HILO_AND_GR_REGS = 13,
	HI1_REG = 14,
	LO1_REG = 15,
	HILO1_REG = 16,
	MD1_REGS = 17,
	HI1_AND_GR_REGS = 18,
	LO1_AND_GR_REGS = 19,
	HILO1_AND_GR_REGS = 20,
	HI01_REG = 21,
	LO01_REG = 22,
	HILO01_REG = 23,
	MD01_REGS = 24,
	HI01_AND_GR_REGS = 25,
	LO01_AND_GR_REGS = 26,
	HILO01_AND_GR_REGS = 27,
	ST_REGS = 28,
	ALL_REGS = 29,
	LIM_REG_CLASSES = 30
};

struct mips_frame_info { // 0x70
	/* 0x00 */ long int total_size;
	/* 0x08 */ long int var_size;
	/* 0x10 */ long int args_size;
	/* 0x18 */ long int extra_size;
	/* 0x20 */ int gp_reg_size;
	/* 0x24 */ int fp_reg_size;
	/* 0x28 */ long int mask;
	/* 0x30 */ long int fmask;
	/* 0x38 */ long int gp_save_offset;
	/* 0x40 */ long int fp_save_offset;
	/* 0x48 */ long int gp_sp_offset;
	/* 0x50 */ long int fp_sp_offset;
	/* 0x58 */ int initialized;
	/* 0x5c */ int num_gp;
	/* 0x60 */ int num_fp;
	/* 0x68 */ long int insns_len;
};

struct mips_args { // 0x3c
	/* 0x00 */ int gp_reg_found;
	/* 0x04 */ int arg_number;
	/* 0x08 */ int arg_words;
	/* 0x0c */ int fp_arg_words;
	/* 0x10 */ int last_arg_fp;
	/* 0x14 */ int fp_code;
	/* 0x18 */ int num_adjusts;
	/* 0x1c */ rtx_def *adjust[8];
};

typedef mips_args CUMULATIVE_ARGS;

enum machine_mode {
	VOIDmode = 0,
	PQImode = 1,
	QImode = 2,
	PHImode = 3,
	HImode = 4,
	PSImode = 5,
	SImode = 6,
	PDImode = 7,
	DImode = 8,
	TImode = 9,
	OImode = 10,
	QFmode = 11,
	HFmode = 12,
	TQFmode = 13,
	SFmode = 14,
	DFmode = 15,
	XFmode = 16,
	TFmode = 17,
	QCmode = 18,
	HCmode = 19,
	SCmode = 20,
	DCmode = 21,
	XCmode = 22,
	TCmode = 23,
	CQImode = 24,
	CHImode = 25,
	CSImode = 26,
	CDImode = 27,
	CTImode = 28,
	COImode = 29,
	BLKmode = 30,
	CCmode = 31,
	MAX_MACHINE_MODE = 32
};

enum mode_class {
	MODE_RANDOM = 0,
	MODE_INT = 1,
	MODE_FLOAT = 2,
	MODE_PARTIAL_INT = 3,
	MODE_CC = 4,
	MODE_COMPLEX_INT = 5,
	MODE_COMPLEX_FLOAT = 6,
	MAX_MODE_CLASS = 7
};

typedef unsigned char UQItype;
typedef int SItype;
typedef unsigned int USItype;
typedef long int DItype;
typedef long unsigned int UDItype;
typedef float SFtype;
typedef double DFtype;
typedef long int word_type;

struct DIstruct { // 0x8
	/* 0x0 */ SItype low;
	/* 0x4 */ SItype high;
};

typedef union { // 0x8
	/* 0x0 */ DIstruct s;
	/* 0x0 */ DItype ll;
} DIunion;

typedef void (*func_ptr)(/* parameters unknown */);
typedef int __gthread_mutex_t;

struct eh_context { // 0x10
	/* 0x0 */ void *handler_label;
	/* 0x4 */ void **dynamic_handler_chain;
	/* 0x8 */ void *info;
	/* 0xc */ void *table_index;
};

typedef struct { // 0xc
	/* 0x0 */ void *start_region;
	/* 0x4 */ void *end_region;
	/* 0x8 */ void *exception_handler;
} old_exception_table;

typedef struct { // 0x10
	/* 0x0 */ void *start_region;
	/* 0x4 */ void *end_region;
	/* 0x8 */ void *exception_handler;
	/* 0xc */ void *match_info;
} exception_table;

typedef struct { // 0x4
	/* 0x0 */ short int language;
	/* 0x2 */ short int version;
} exception_lang_info;

typedef struct { // 0x18
	/* 0x00 */ void *runtime_id_field;
	/* 0x04 */ exception_lang_info lang;
	/* 0x08 */ exception_table table[1];
} exception_descriptor;

typedef void* (*__eh_matcher)(/* parameters unknown */);

typedef struct { // 0x8
	/* 0x0 */ __eh_matcher match_function;
	/* 0x4 */ short int language;
	/* 0x6 */ short int version;
} __eh_info;

enum exception_source_language {
	EH_LANG_C89 = 1,
	EH_LANG_C = 2,
	EH_LANG_Ada83 = 3,
	EH_LANG_C_plus_plus = 4,
	EH_LANG_Cobol74 = 5,
	EH_LANG_Cobol85 = 6,
	EH_LANG_Fortran77 = 7,
	EH_LANG_Fortran90 = 8,
	EH_LANG_Pascal83 = 9,
	EH_LANG_Modula2 = 10,
	EH_LANG_Java = 11,
	EH_LANG_Mips_Assembler = 32769
};

struct eh_full_context { // 0x18
	/* 0x00 */ eh_context c;
	/* 0x10 */ void *top_elt[2];
};

typedef struct { // 0x2f0
	/* 0x000 */ void *cfa;
	/* 0x004 */ void *eh_ptr;
	/* 0x008 */ long int cfa_offset;
	/* 0x010 */ long int args_size;
	/* 0x018 */ long int reg_or_offset[80];
	/* 0x298 */ short unsigned int cfa_reg;
	/* 0x29a */ short unsigned int retaddr_column;
	/* 0x29c */ char saved[80];
} frame_state;

struct object { // 0x18
	/* 0x00 */ void *pc_begin;
	/* 0x04 */ void *pc_end;
	/* 0x08 */ dwarf_fde *fde_begin;
	/* 0x0c */ dwarf_fde **fde_array;
	/* 0x10 */ size_t count;
	/* 0x14 */ object *next;
};

typedef int ptr_type;
typedef short int HItype;
typedef short unsigned int UHItype;
// warning: multiple differing types with the same name (type name not equal)
typedef UDItype fractype;
// warning: multiple differing types with the same name (type name not equal)
typedef USItype halffractype;
// warning: multiple differing types with the same name (type name not equal)
typedef DFtype FLO_type;
// warning: multiple differing types with the same name (type name not equal)
typedef DItype intfrac;

typedef enum {
	CLASS_SNAN = 0,
	CLASS_QNAN = 1,
	CLASS_ZERO = 2,
	CLASS_NUMBER = 3,
	CLASS_INFINITY = 4
} fp_class_type;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x18
	/* 0x00 */ fp_class_type class;
	/* 0x04 */ unsigned int sign;
	/* 0x08 */ int normal_exp;
	/* 0x10 */ union { // 0x8
		/* 0x10 */ fractype ll;
		/* 0x10 */ halffractype l[2];
	} fraction;
} fp_number_type;

// warning: multiple differing types with the same name (size not equal)
typedef union { // 0x8
	/* 0x0 */ FLO_type value;
	/* 0x0 */ fractype value_raw;
	/* 0x0 */ halffractype words[2];
	/* 0x0 */ struct { // 0x8
		/* 0x0:0 */ fractype fraction : 52;
		/* 0x6:4 */ unsigned int exp : 11;
		/* 0x7:7 */ unsigned int sign : 1;
	} bits;
} FLO_union_type;

// warning: multiple differing types with the same name (size not equal)
struct exception { // 0x4
	/* 0x0 */ __vtbl_ptr_type *$vf26;

	exception& operator=();
	exception();
	exception();
	/* vtable[1] */ virtual exception(exception*, int, void);
	/* vtable[2] */ virtual char* what();
};

struct bad_exception : /* 0x0 */ exception { // 0x4
};

typedef void (*terminate_handler)(/* parameters unknown */);
typedef void (*unexpected_handler)(/* parameters unknown */);

struct cp_eh_info { // 0x30
	/* 0x00 */ __eh_info eh_info;
	/* 0x08 */ void *value;
	/* 0x0c */ void *type;
	/* 0x10 */ void (*cleanup)(/* parameters unknown */);
	/* 0x14 */ bool caught;
	/* 0x18 */ cp_eh_info *next;
	/* 0x20 */ long int handlers;
	/* 0x28 */ void *original_value;
};

typedef void* (*rtimetype)(/* parameters unknown */);
// warning: multiple differing types with the same name (type name not equal)
typedef USItype fractype;
// warning: multiple differing types with the same name (type name not equal)
typedef UHItype halffractype;
// warning: multiple differing types with the same name (type name not equal)
typedef SFtype FLO_type;
// warning: multiple differing types with the same name (type name not equal)
typedef SItype intfrac;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x10
	/* 0x0 */ fp_class_type class;
	/* 0x4 */ unsigned int sign;
	/* 0x8 */ int normal_exp;
	/* 0xc */ union { // 0x4
		/* 0xc */ fractype ll;
		/* 0xc */ halffractype l[2];
	} fraction;
} fp_number_type;

// warning: multiple differing types with the same name (size not equal)
typedef union { // 0x4
	/* 0x0 */ FLO_type value;
	/* 0x0 */ fractype value_raw;
	/* 0x0 */ struct { // 0x4
		/* 0x0:0 */ fractype fraction : 23;
		/* 0x2:7 */ unsigned int exp : 8;
		/* 0x3:7 */ unsigned int sign : 1;
	} bits;
} FLO_union_type;

enum dwarf_tag {
	DW_TAG_padding = 0,
	DW_TAG_array_type = 1,
	DW_TAG_class_type = 2,
	DW_TAG_entry_point = 3,
	DW_TAG_enumeration_type = 4,
	DW_TAG_formal_parameter = 5,
	DW_TAG_imported_declaration = 8,
	DW_TAG_label = 10,
	DW_TAG_lexical_block = 11,
	DW_TAG_member = 13,
	DW_TAG_pointer_type = 15,
	DW_TAG_reference_type = 16,
	DW_TAG_compile_unit = 17,
	DW_TAG_string_type = 18,
	DW_TAG_structure_type = 19,
	DW_TAG_subroutine_type = 21,
	DW_TAG_typedef = 22,
	DW_TAG_union_type = 23,
	DW_TAG_unspecified_parameters = 24,
	DW_TAG_variant = 25,
	DW_TAG_common_block = 26,
	DW_TAG_common_inclusion = 27,
	DW_TAG_inheritance = 28,
	DW_TAG_inlined_subroutine = 29,
	DW_TAG_module = 30,
	DW_TAG_ptr_to_member_type = 31,
	DW_TAG_set_type = 32,
	DW_TAG_subrange_type = 33,
	DW_TAG_with_stmt = 34,
	DW_TAG_access_declaration = 35,
	DW_TAG_base_type = 36,
	DW_TAG_catch_block = 37,
	DW_TAG_const_type = 38,
	DW_TAG_constant = 39,
	DW_TAG_enumerator = 40,
	DW_TAG_file_type = 41,
	DW_TAG_friend = 42,
	DW_TAG_namelist = 43,
	DW_TAG_namelist_item = 44,
	DW_TAG_packed_type = 45,
	DW_TAG_subprogram = 46,
	DW_TAG_template_type_param = 47,
	DW_TAG_template_value_param = 48,
	DW_TAG_thrown_type = 49,
	DW_TAG_try_block = 50,
	DW_TAG_variant_part = 51,
	DW_TAG_variable = 52,
	DW_TAG_volatile_type = 53,
	DW_TAG_MIPS_loop = 16513,
	DW_TAG_format_label = 16641,
	DW_TAG_function_template = 16642,
	DW_TAG_class_template = 16643
};

enum dwarf_form {
	DW_FORM_addr = 1,
	DW_FORM_block2 = 3,
	DW_FORM_block4 = 4,
	DW_FORM_data2 = 5,
	DW_FORM_data4 = 6,
	DW_FORM_data8 = 7,
	DW_FORM_string = 8,
	DW_FORM_block = 9,
	DW_FORM_block1 = 10,
	DW_FORM_data1 = 11,
	DW_FORM_flag = 12,
	DW_FORM_sdata = 13,
	DW_FORM_strp = 14,
	DW_FORM_udata = 15,
	DW_FORM_ref_addr = 16,
	DW_FORM_ref1 = 17,
	DW_FORM_ref2 = 18,
	DW_FORM_ref4 = 19,
	DW_FORM_ref8 = 20,
	DW_FORM_ref_udata = 21,
	DW_FORM_indirect = 22
};

enum dwarf_attribute {
	DW_AT_sibling = 1,
	DW_AT_location = 2,
	DW_AT_name = 3,
	DW_AT_ordering = 9,
	DW_AT_subscr_data = 10,
	DW_AT_byte_size = 11,
	DW_AT_bit_offset = 12,
	DW_AT_bit_size = 13,
	DW_AT_element_list = 15,
	DW_AT_stmt_list = 16,
	DW_AT_low_pc = 17,
	DW_AT_high_pc = 18,
	DW_AT_language = 19,
	DW_AT_member = 20,
	DW_AT_discr = 21,
	DW_AT_discr_value = 22,
	DW_AT_visibility = 23,
	DW_AT_import = 24,
	DW_AT_string_length = 25,
	DW_AT_common_reference = 26,
	DW_AT_comp_dir = 27,
	DW_AT_const_value = 28,
	DW_AT_containing_type = 29,
	DW_AT_default_value = 30,
	DW_AT_inline = 32,
	DW_AT_is_optional = 33,
	DW_AT_lower_bound = 34,
	DW_AT_producer = 37,
	DW_AT_prototyped = 39,
	DW_AT_return_addr = 42,
	DW_AT_start_scope = 44,
	DW_AT_stride_size = 46,
	DW_AT_upper_bound = 47,
	DW_AT_abstract_origin = 49,
	DW_AT_accessibility = 50,
	DW_AT_address_class = 51,
	DW_AT_artificial = 52,
	DW_AT_base_types = 53,
	DW_AT_calling_convention = 54,
	DW_AT_count = 55,
	DW_AT_data_member_location = 56,
	DW_AT_decl_column = 57,
	DW_AT_decl_file = 58,
	DW_AT_decl_line = 59,
	DW_AT_declaration = 60,
	DW_AT_discr_list = 61,
	DW_AT_encoding = 62,
	DW_AT_external = 63,
	DW_AT_frame_base = 64,
	DW_AT_friend = 65,
	DW_AT_identifier_case = 66,
	DW_AT_macro_info = 67,
	DW_AT_namelist_items = 68,
	DW_AT_priority = 69,
	DW_AT_segment = 70,
	DW_AT_specification = 71,
	DW_AT_static_link = 72,
	DW_AT_type = 73,
	DW_AT_use_location = 74,
	DW_AT_variable_parameter = 75,
	DW_AT_virtuality = 76,
	DW_AT_vtable_elem_location = 77,
	DW_AT_MIPS_fde = 8193,
	DW_AT_MIPS_loop_begin = 8194,
	DW_AT_MIPS_tail_loop_begin = 8195,
	DW_AT_MIPS_epilog_begin = 8196,
	DW_AT_MIPS_loop_unroll_factor = 8197,
	DW_AT_MIPS_software_pipeline_depth = 8198,
	DW_AT_MIPS_linkage_name = 8199,
	DW_AT_MIPS_stride = 8200,
	DW_AT_MIPS_abstract_name = 8201,
	DW_AT_MIPS_clone_origin = 8202,
	DW_AT_MIPS_has_inlines = 8203,
	DW_AT_sf_names = 8449,
	DW_AT_src_info = 8450,
	DW_AT_mac_info = 8451,
	DW_AT_src_coords = 8452,
	DW_AT_body_begin = 8453,
	DW_AT_body_end = 8454
};

enum dwarf_location_atom {
	DW_OP_addr = 3,
	DW_OP_deref = 6,
	DW_OP_const1u = 8,
	DW_OP_const1s = 9,
	DW_OP_const2u = 10,
	DW_OP_const2s = 11,
	DW_OP_const4u = 12,
	DW_OP_const4s = 13,
	DW_OP_const8u = 14,
	DW_OP_const8s = 15,
	DW_OP_constu = 16,
	DW_OP_consts = 17,
	DW_OP_dup = 18,
	DW_OP_drop = 19,
	DW_OP_over = 20,
	DW_OP_pick = 21,
	DW_OP_swap = 22,
	DW_OP_rot = 23,
	DW_OP_xderef = 24,
	DW_OP_abs = 25,
	DW_OP_and = 26,
	DW_OP_div = 27,
	DW_OP_minus = 28,
	DW_OP_mod = 29,
	DW_OP_mul = 30,
	DW_OP_neg = 31,
	DW_OP_not = 32,
	DW_OP_or = 33,
	DW_OP_plus = 34,
	DW_OP_plus_uconst = 35,
	DW_OP_shl = 36,
	DW_OP_shr = 37,
	DW_OP_shra = 38,
	DW_OP_xor = 39,
	DW_OP_bra = 40,
	DW_OP_eq = 41,
	DW_OP_ge = 42,
	DW_OP_gt = 43,
	DW_OP_le = 44,
	DW_OP_lt = 45,
	DW_OP_ne = 46,
	DW_OP_skip = 47,
	DW_OP_lit0 = 48,
	DW_OP_lit1 = 49,
	DW_OP_lit2 = 50,
	DW_OP_lit3 = 51,
	DW_OP_lit4 = 52,
	DW_OP_lit5 = 53,
	DW_OP_lit6 = 54,
	DW_OP_lit7 = 55,
	DW_OP_lit8 = 56,
	DW_OP_lit9 = 57,
	DW_OP_lit10 = 58,
	DW_OP_lit11 = 59,
	DW_OP_lit12 = 60,
	DW_OP_lit13 = 61,
	DW_OP_lit14 = 62,
	DW_OP_lit15 = 63,
	DW_OP_lit16 = 64,
	DW_OP_lit17 = 65,
	DW_OP_lit18 = 66,
	DW_OP_lit19 = 67,
	DW_OP_lit20 = 68,
	DW_OP_lit21 = 69,
	DW_OP_lit22 = 70,
	DW_OP_lit23 = 71,
	DW_OP_lit24 = 72,
	DW_OP_lit25 = 73,
	DW_OP_lit26 = 74,
	DW_OP_lit27 = 75,
	DW_OP_lit28 = 76,
	DW_OP_lit29 = 77,
	DW_OP_lit30 = 78,
	DW_OP_lit31 = 79,
	DW_OP_reg0 = 80,
	DW_OP_reg1 = 81,
	DW_OP_reg2 = 82,
	DW_OP_reg3 = 83,
	DW_OP_reg4 = 84,
	DW_OP_reg5 = 85,
	DW_OP_reg6 = 86,
	DW_OP_reg7 = 87,
	DW_OP_reg8 = 88,
	DW_OP_reg9 = 89,
	DW_OP_reg10 = 90,
	DW_OP_reg11 = 91,
	DW_OP_reg12 = 92,
	DW_OP_reg13 = 93,
	DW_OP_reg14 = 94,
	DW_OP_reg15 = 95,
	DW_OP_reg16 = 96,
	DW_OP_reg17 = 97,
	DW_OP_reg18 = 98,
	DW_OP_reg19 = 99,
	DW_OP_reg20 = 100,
	DW_OP_reg21 = 101,
	DW_OP_reg22 = 102,
	DW_OP_reg23 = 103,
	DW_OP_reg24 = 104,
	DW_OP_reg25 = 105,
	DW_OP_reg26 = 106,
	DW_OP_reg27 = 107,
	DW_OP_reg28 = 108,
	DW_OP_reg29 = 109,
	DW_OP_reg30 = 110,
	DW_OP_reg31 = 111,
	DW_OP_breg0 = 112,
	DW_OP_breg1 = 113,
	DW_OP_breg2 = 114,
	DW_OP_breg3 = 115,
	DW_OP_breg4 = 116,
	DW_OP_breg5 = 117,
	DW_OP_breg6 = 118,
	DW_OP_breg7 = 119,
	DW_OP_breg8 = 120,
	DW_OP_breg9 = 121,
	DW_OP_breg10 = 122,
	DW_OP_breg11 = 123,
	DW_OP_breg12 = 124,
	DW_OP_breg13 = 125,
	DW_OP_breg14 = 126,
	DW_OP_breg15 = 127,
	DW_OP_breg16 = 128,
	DW_OP_breg17 = 129,
	DW_OP_breg18 = 130,
	DW_OP_breg19 = 131,
	DW_OP_breg20 = 132,
	DW_OP_breg21 = 133,
	DW_OP_breg22 = 134,
	DW_OP_breg23 = 135,
	DW_OP_breg24 = 136,
	DW_OP_breg25 = 137,
	DW_OP_breg26 = 138,
	DW_OP_breg27 = 139,
	DW_OP_breg28 = 140,
	DW_OP_breg29 = 141,
	DW_OP_breg30 = 142,
	DW_OP_breg31 = 143,
	DW_OP_regx = 144,
	DW_OP_fbreg = 145,
	DW_OP_bregx = 146,
	DW_OP_piece = 147,
	DW_OP_deref_size = 148,
	DW_OP_xderef_size = 149,
	DW_OP_nop = 150
};

enum dwarf_type {
	DW_ATE_void = 0,
	DW_ATE_address = 1,
	DW_ATE_boolean = 2,
	DW_ATE_complex_float = 3,
	DW_ATE_float = 4,
	DW_ATE_signed = 5,
	DW_ATE_signed_char = 6,
	DW_ATE_unsigned = 7,
	DW_ATE_unsigned_char = 8
};

enum dwarf_array_dim_ordering {
	DW_ORD_row_major = 0,
	DW_ORD_col_major = 1
};

enum dwarf_access_attribute {
	DW_ACCESS_public = 1,
	DW_ACCESS_protected = 2,
	DW_ACCESS_private = 3
};

enum dwarf_visibility_attribute {
	DW_VIS_local = 1,
	DW_VIS_exported = 2,
	DW_VIS_qualified = 3
};

enum dwarf_virtuality_attribute {
	DW_VIRTUALITY_none = 0,
	DW_VIRTUALITY_virtual = 1,
	DW_VIRTUALITY_pure_virtual = 2
};

enum dwarf_id_case {
	DW_ID_case_sensitive = 0,
	DW_ID_up_case = 1,
	DW_ID_down_case = 2,
	DW_ID_case_insensitive = 3
};

enum dwarf_calling_convention {
	DW_CC_normal = 1,
	DW_CC_program = 2,
	DW_CC_nocall = 3
};

enum dwarf_inline_attribute {
	DW_INL_not_inlined = 0,
	DW_INL_inlined = 1,
	DW_INL_declared_not_inlined = 2,
	DW_INL_declared_inlined = 3
};

enum dwarf_discrim_list {
	DW_DSC_label = 0,
	DW_DSC_range = 1
};

enum dwarf_line_number_ops {
	DW_LNS_extended_op = 0,
	DW_LNS_copy = 1,
	DW_LNS_advance_pc = 2,
	DW_LNS_advance_line = 3,
	DW_LNS_set_file = 4,
	DW_LNS_set_column = 5,
	DW_LNS_negate_stmt = 6,
	DW_LNS_set_basic_block = 7,
	DW_LNS_const_add_pc = 8,
	DW_LNS_fixed_advance_pc = 9
};

enum dwarf_line_number_x_ops {
	DW_LNE_end_sequence = 1,
	DW_LNE_set_address = 2,
	DW_LNE_define_file = 3
};

enum dwarf_call_frame_info {
	DW_CFA_advance_loc = 64,
	DW_CFA_offset = 128,
	DW_CFA_restore = 192,
	DW_CFA_nop = 0,
	DW_CFA_set_loc = 1,
	DW_CFA_advance_loc1 = 2,
	DW_CFA_advance_loc2 = 3,
	DW_CFA_advance_loc4 = 4,
	DW_CFA_offset_extended = 5,
	DW_CFA_restore_extended = 6,
	DW_CFA_undefined = 7,
	DW_CFA_same_value = 8,
	DW_CFA_register = 9,
	DW_CFA_remember_state = 10,
	DW_CFA_restore_state = 11,
	DW_CFA_def_cfa = 12,
	DW_CFA_def_cfa_register = 13,
	DW_CFA_def_cfa_offset = 14,
	DW_CFA_MIPS_advance_loc8 = 29,
	DW_CFA_GNU_window_save = 45,
	DW_CFA_GNU_args_size = 46
};

enum dwarf_source_language {
	DW_LANG_C89 = 1,
	DW_LANG_C = 2,
	DW_LANG_Ada83 = 3,
	DW_LANG_C_plus_plus = 4,
	DW_LANG_Cobol74 = 5,
	DW_LANG_Cobol85 = 6,
	DW_LANG_Fortran77 = 7,
	DW_LANG_Fortran90 = 8,
	DW_LANG_Pascal83 = 9,
	DW_LANG_Modula2 = 10,
	DW_LANG_Mips_Assembler = 32769
};

enum dwarf_macinfo_record_type {
	DW_MACINFO_define = 1,
	DW_MACINFO_undef = 2,
	DW_MACINFO_start_file = 3,
	DW_MACINFO_end_file = 4,
	DW_MACINFO_vendor_ext = 255
};

typedef int sword;
typedef unsigned int uword;
typedef unsigned int uaddr;
typedef int saddr;
typedef unsigned char ubyte;

struct dwarf_cie { // 0xc
	/* 0x0 */ uword length;
	/* 0x4 */ sword CIE_id;
	/* 0x8 */ ubyte version;
	/* 0x9 */ char augmentation[0];
};

struct dwarf_fde { // 0x10
	/* 0x0 */ uword length;
	/* 0x4 */ sword CIE_delta;
	/* 0x8 */ void *pc_begin;
	/* 0xc */ uaddr pc_range;
};

typedef dwarf_fde fde;

struct cie_info { // 0x14
	/* 0x00 */ char *augmentation;
	/* 0x04 */ void *eh_ptr;
	/* 0x08 */ int code_align;
	/* 0x0c */ int data_align;
	/* 0x10 */ unsigned int ra_regno;
};

struct frame_state_internal { // 0x2f8
	/* 0x000 */ frame_state s;
	/* 0x2f0 */ frame_state_internal *saved_state;
};

union unaligned { // 0x8
	/* 0x0 */ void *p;
	/* 0x0 */ short unsigned int b2;
	/* 0x0 */ unsigned int b4;
	/* 0x0 */ long unsigned int b8;
};

typedef struct { // 0x8
	/* 0x0 */ fde **array;
	/* 0x4 */ size_t count;
} fde_vector;

typedef struct { // 0x10
	/* 0x0 */ fde_vector linear;
	/* 0x8 */ fde_vector erratic;
} fde_accumulator;

struct type_info { // 0x8
protected:
	/* 0x0 */ char *_name;
public:
	/* 0x4 */ __vtbl_ptr_type *$vf33;

private:
	type_info& operator=();
	type_info();
protected:
	type_info();
public:
	/* vtable[1] */ virtual type_info(type_info*, int, void);
	bool before();
	char* name();
	bool operator==();
	bool operator!=();
};

struct bad_cast : /* 0x0 */ exception { // 0x4
};

struct bad_typeid : /* 0x0 */ exception { // 0x4
};

struct __user_type_info : /* 0x0 */ type_info { // 0x8
	__user_type_info& operator=();
	__user_type_info();
	/* vtable[1] */ virtual __user_type_info(__user_type_info*, int, void);
	__user_type_info();
	/* vtable[2] */ virtual void* dcast();
};

struct __si_type_info : /* 0x0 */ __user_type_info { // 0xc
private:
	/* 0x8 */ __user_type_info &base;

public:
	__si_type_info& operator=();
	__si_type_info();
	/* vtable[1] */ virtual __si_type_info(__si_type_info*, int, void);
	__si_type_info();
	/* vtable[2] */ virtual void* dcast();
};

enum access {
	PUBLIC = 1,
	PROTECTED = 2,
	PRIVATE = 3
};

struct base_info { // 0x8
	/* 0x0 */ __user_type_info *base;
	/* 0x4:0 */ USItype offset : 29;
	/* 0x7:5 */ bool is_virtual : 1;
	/* 0x7 */ access access;
};

struct __class_type_info : /* 0x0 */ __user_type_info { // 0x10
	/* 0x8 */ base_info *base_list;
	/* 0xc */ size_t n_bases;

	__class_type_info& operator=();
	__class_type_info();
	/* vtable[1] */ virtual __class_type_info(__class_type_info*, int, void);
	__class_type_info();
	/* vtable[2] */ virtual void* dcast();
};

typedef void (*new_handler)(/* parameters unknown */);

struct __pointer_type_info : /* 0x0 */ type_info { // 0xc
	/* 0x8 */ type_info &type;
};

enum cv {
	NONE = 0,
	CONST = 1,
	VOLATILE = 2,
	CONSTVOL = 3
};

struct __attr_type_info : /* 0x0 */ type_info { // 0x10
	/* 0x8 */ type_info &type;
	/* 0xc */ cv attr;
};

struct __builtin_type_info : /* 0x0 */ type_info { // 0x8
};

struct __func_type_info : /* 0x0 */ type_info { // 0x8
};

struct __ptmf_type_info : /* 0x0 */ type_info { // 0x8
};

struct __ptmd_type_info : /* 0x0 */ type_info { // 0x8
};

struct __array_type_info : /* 0x0 */ type_info { // 0x8
};

struct stat { // 0x68
	/* 0x00 */ dev_t st_dev;
	/* 0x02 */ ino_t st_ino;
	/* 0x04 */ mode_t st_mode;
	/* 0x08 */ nlink_t st_nlink;
	/* 0x0a */ uid_t st_uid;
	/* 0x0c */ gid_t st_gid;
	/* 0x0e */ dev_t st_rdev;
	/* 0x10 */ off_t st_size;
	/* 0x18 */ time_t st_atime;
	/* 0x20 */ long int st_spare1;
	/* 0x28 */ time_t st_mtime;
	/* 0x30 */ long int st_spare2;
	/* 0x38 */ time_t st_ctime;
	/* 0x40 */ long int st_spare3;
	/* 0x48 */ long int st_blksize;
	/* 0x50 */ long int st_blocks;
	/* 0x58 */ long int st_spare4[2];
};

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int COUNT : 16;
	/* 0x2:0 */ unsigned int p0 : 16;
} tT_COUNT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int CLKS : 2;
	/* 0x0:2 */ unsigned int GATE : 1;
	/* 0x0:3 */ unsigned int GATS : 1;
	/* 0x0:4 */ unsigned int GATM : 2;
	/* 0x0:6 */ unsigned int ZRET : 1;
	/* 0x0:7 */ unsigned int CUE : 1;
	/* 0x1:0 */ unsigned int CMPE : 1;
	/* 0x1:1 */ unsigned int OVFE : 1;
	/* 0x1:2 */ unsigned int EQUF : 1;
	/* 0x1:3 */ unsigned int OVFF : 1;
	/* 0x1:4 */ unsigned int p0 : 20;
} tT_MODE;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int COMP : 16;
	/* 0x2:0 */ unsigned int p0 : 16;
} tT_COMP;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int HOLD : 16;
	/* 0x2:0 */ unsigned int p0 : 16;
} tT_HOLD;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int OPTION : 28;
	/* 0x3:4 */ unsigned int CODE : 4;
} tIPU_CMD_read;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x8
	/* 0x0 */ unsigned int DATA;
	/* 0x4:0 */ unsigned int p0 : 31;
	/* 0x7:7 */ unsigned int BUSY : 1;
} tIPU_CMD_write;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int IFC : 4;
	/* 0x0:4 */ unsigned int OFC : 4;
	/* 0x1:0 */ unsigned int CBP : 6;
	/* 0x1:6 */ unsigned int ECD : 1;
	/* 0x1:7 */ unsigned int SCD : 1;
	/* 0x2:0 */ unsigned int IDP : 2;
	/* 0x2:2 */ unsigned int p0 : 2;
	/* 0x2:4 */ unsigned int AS : 1;
	/* 0x2:5 */ unsigned int IVF : 1;
	/* 0x2:6 */ unsigned int QST : 1;
	/* 0x2:7 */ unsigned int MP1 : 1;
	/* 0x3:0 */ unsigned int PCT : 3;
	/* 0x3:3 */ unsigned int p1 : 3;
	/* 0x3:6 */ unsigned int RST : 1;
	/* 0x3:7 */ unsigned int BUSY : 1;
} tIPU_CTRL;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int BP : 7;
	/* 0x0:7 */ unsigned int p0 : 1;
	/* 0x1:0 */ unsigned int IFC : 4;
	/* 0x1:4 */ unsigned int p1 : 4;
	/* 0x2:0 */ unsigned int FP : 2;
	/* 0x2:2 */ unsigned int p2 : 14;
} tIPU_BP;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int RST : 1;
	/* 0x0:1 */ unsigned int p0 : 2;
	/* 0x0:3 */ unsigned int PSE : 1;
	/* 0x0:4 */ unsigned int p1 : 28;
} tGIF_CTRL;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int M3R : 1;
	/* 0x0:1 */ unsigned int p0 : 1;
	/* 0x0:2 */ unsigned int IMT : 1;
	/* 0x0:3 */ unsigned int p1 : 29;
} tGIF_MODE;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int M3R : 1;
	/* 0x0:1 */ unsigned int M3P : 1;
	/* 0x0:2 */ unsigned int IMT : 1;
	/* 0x0:3 */ unsigned int PSE : 1;
	/* 0x0:4 */ unsigned int p0 : 1;
	/* 0x0:5 */ unsigned int IP3 : 1;
	/* 0x0:6 */ unsigned int P3Q : 1;
	/* 0x0:7 */ unsigned int P2Q : 1;
	/* 0x1:0 */ unsigned int P1Q : 1;
	/* 0x1:1 */ unsigned int OPH : 1;
	/* 0x1:2 */ unsigned int APATH : 2;
	/* 0x1:4 */ unsigned int DIR : 1;
	/* 0x1:5 */ unsigned int p1 : 11;
	/* 0x3:0 */ unsigned int FQC : 5;
	/* 0x3:5 */ unsigned int p2 : 3;
} tGIF_STAT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int NLOOP : 15;
	/* 0x1:7 */ unsigned int EOP : 1;
	/* 0x2:0 */ unsigned int tag : 16;
} tGIF_TAG0;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int tag : 15;
	/* 0x1:7 */ unsigned int PRE : 1;
	/* 0x2:0 */ unsigned int PRIM : 10;
	/* 0x3:2 */ unsigned int FLG : 2;
	/* 0x3:4 */ unsigned int NREG : 4;
} tGIF_TAG1;

typedef struct { // 0x4
	/* 0x0 */ unsigned int tag;
} tGIF_TAG2;

typedef struct { // 0x4
	/* 0x0 */ unsigned int tag;
} tGIF_TAG3;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int LOOPCNT : 15;
	/* 0x1:7 */ unsigned int p0 : 1;
	/* 0x2:0 */ unsigned int REGCNT : 4;
	/* 0x2:4 */ unsigned int VUADDR : 10;
	/* 0x3:6 */ unsigned int p1 : 2;
} tGIF_CNT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int P3CNT : 15;
	/* 0x1:7 */ unsigned int p0 : 17;
} tGIF_P3CNT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int LOOPCNT : 15;
	/* 0x1:7 */ unsigned int EOP : 1;
	/* 0x2:0 */ unsigned int p0 : 16;
} tGIF_P3TAG;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int VPS : 2;
	/* 0x0:2 */ unsigned int VEW : 1;
	/* 0x0:3 */ unsigned int p0 : 1;
	/* 0x0:4 */ unsigned int p1 : 2;
	/* 0x0:6 */ unsigned int MRK : 1;
	/* 0x0:7 */ unsigned int p2 : 1;
	/* 0x1:0 */ unsigned int VSS : 1;
	/* 0x1:1 */ unsigned int VFS : 1;
	/* 0x1:2 */ unsigned int VIS : 1;
	/* 0x1:3 */ unsigned int INT : 1;
	/* 0x1:4 */ unsigned int ERO : 1;
	/* 0x1:5 */ unsigned int ER1 : 1;
	/* 0x1:6 */ unsigned int p3 : 10;
	/* 0x3:0 */ unsigned int FQC : 4;
	/* 0x3:4 */ unsigned int p4 : 1;
	/* 0x3:5 */ unsigned int p5 : 3;
} tVIF0_STAT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int RST : 1;
	/* 0x0:1 */ unsigned int FBK : 1;
	/* 0x0:2 */ unsigned int STP : 1;
	/* 0x0:3 */ unsigned int STC : 1;
	/* 0x0:4 */ unsigned int p0 : 28;
} tVIF0_FBRST;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int MII : 1;
	/* 0x0:1 */ unsigned int ME0 : 1;
	/* 0x0:2 */ unsigned int ME1 : 1;
	/* 0x0:3 */ unsigned int p0 : 29;
} tVIF0_ERR;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int MARK : 16;
	/* 0x2:0 */ unsigned int p0 : 16;
} tVIF_MARK;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int CL : 8;
	/* 0x1:0 */ unsigned int WL : 8;
	/* 0x2:0 */ unsigned int p0 : 16;
} tVIF_CYCLE;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int MOD : 2;
	/* 0x0:2 */ unsigned int p0 : 30;
} tVIF_MODE;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int num : 8;
	/* 0x1:0 */ unsigned int p0 : 24;
} tVIF0_NUM;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int m0 : 2;
	/* 0x0:2 */ unsigned int m1 : 2;
	/* 0x0:4 */ unsigned int m2 : 2;
	/* 0x0:6 */ unsigned int m3 : 2;
	/* 0x1:0 */ unsigned int m4 : 2;
	/* 0x1:2 */ unsigned int m5 : 2;
	/* 0x1:4 */ unsigned int m6 : 2;
	/* 0x1:6 */ unsigned int m7 : 2;
	/* 0x2:0 */ unsigned int m8 : 2;
	/* 0x2:2 */ unsigned int m9 : 2;
	/* 0x2:4 */ unsigned int m10 : 2;
	/* 0x2:6 */ unsigned int m11 : 2;
	/* 0x3:0 */ unsigned int m12 : 2;
	/* 0x3:2 */ unsigned int m13 : 2;
	/* 0x3:4 */ unsigned int m14 : 2;
	/* 0x3:6 */ unsigned int m15 : 2;
} tVIF_MASK;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int immediate : 16;
	/* 0x2:0 */ unsigned int num : 8;
	/* 0x3:0 */ unsigned int CMD : 8;
} tVIF_CODE;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int ITOPS : 10;
	/* 0x1:2 */ unsigned int p0 : 22;
} tVIF_ITOPS;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int ITOP : 10;
	/* 0x1:2 */ unsigned int p0 : 22;
} tVIF_ITOP;

typedef struct { // 0x4
	/* 0x0 */ unsigned int R0;
} tVIF_R0;

typedef struct { // 0x4
	/* 0x0 */ unsigned int R1;
} tVIF_R1;

typedef struct { // 0x4
	/* 0x0 */ unsigned int R2;
} tVIF_R2;

typedef struct { // 0x4
	/* 0x0 */ unsigned int R3;
} tVIF_R3;

typedef struct { // 0x4
	/* 0x0 */ unsigned int C0;
} tVIF_C0;

typedef struct { // 0x4
	/* 0x0 */ unsigned int C1;
} tVIF_C1;

typedef struct { // 0x4
	/* 0x0 */ unsigned int C2;
} tVIF_C2;

typedef struct { // 0x4
	/* 0x0 */ unsigned int C3;
} tVIF_C3;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int VPS : 2;
	/* 0x0:2 */ unsigned int VEW : 1;
	/* 0x0:3 */ unsigned int VGW : 1;
	/* 0x0:4 */ unsigned int p0 : 2;
	/* 0x0:6 */ unsigned int MRK : 1;
	/* 0x0:7 */ unsigned int DBF : 1;
	/* 0x1:0 */ unsigned int VSS : 1;
	/* 0x1:1 */ unsigned int VFS : 1;
	/* 0x1:2 */ unsigned int VIS : 1;
	/* 0x1:3 */ unsigned int INT : 1;
	/* 0x1:4 */ unsigned int ERO : 1;
	/* 0x1:5 */ unsigned int ER1 : 1;
	/* 0x1:6 */ unsigned int p1 : 9;
	/* 0x2:7 */ unsigned int FDR : 1;
	/* 0x3:0 */ unsigned int FQC : 5;
	/* 0x3:5 */ unsigned int p2 : 3;
} tVIF1_STAT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int RST : 1;
	/* 0x0:1 */ unsigned int FBK : 1;
	/* 0x0:2 */ unsigned int STP : 1;
	/* 0x0:3 */ unsigned int STC : 1;
	/* 0x0:4 */ unsigned int p0 : 28;
} tVIF1_FBRST;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int MII : 1;
	/* 0x0:1 */ unsigned int ME0 : 1;
	/* 0x0:2 */ unsigned int ME1 : 1;
	/* 0x0:3 */ unsigned int p0 : 29;
} tVIF1_ERR;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int num : 16;
	/* 0x2:0 */ unsigned int p0 : 16;
} tVIF1_NUM;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int BASE : 10;
	/* 0x1:2 */ unsigned int p0 : 22;
} tVIF1_BASE;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int OFFSET : 10;
	/* 0x1:2 */ unsigned int p0 : 22;
} tVIF1_OFST;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int TOPS : 10;
	/* 0x1:2 */ unsigned int p0 : 22;
} tVIF1_TOPS;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int TOP : 10;
	/* 0x1:2 */ unsigned int p0 : 22;
} tVIF1_TOP;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int DIR : 1;
	/* 0x0:1 */ unsigned int p0 : 1;
	/* 0x0:2 */ unsigned int MOD : 2;
	/* 0x0:4 */ unsigned int ASP : 2;
	/* 0x0:6 */ unsigned int TTE : 1;
	/* 0x0:7 */ unsigned int TIE : 1;
	/* 0x1:0 */ unsigned int STR : 1;
	/* 0x1:1 */ unsigned int p1 : 7;
	/* 0x2:0 */ unsigned int TAG : 16;
} tD_CHCR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 4;
	/* 0x0:4 */ unsigned int ADDR : 27;
	/* 0x3:7 */ unsigned int SPR : 1;
} tD_MADR;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int QWC : 16;
	/* 0x2:0 */ unsigned int p0 : 16;
} tD_QWC;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 4;
	/* 0x0:4 */ unsigned int ADDR : 27;
	/* 0x3:7 */ unsigned int SPR : 1;
} tD_TADR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 4;
	/* 0x0:4 */ unsigned int ADDR : 27;
	/* 0x3:7 */ unsigned int SPR : 1;
} tD_ASR0;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 4;
	/* 0x0:4 */ unsigned int ADDR : 27;
	/* 0x3:7 */ unsigned int SPR : 1;
} tD_ASR1;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 4;
	/* 0x0:4 */ unsigned int ADDR : 10;
	/* 0x1:6 */ unsigned int p1 : 18;
} tD_SADR;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int DMAE : 1;
	/* 0x0:1 */ unsigned int RELE : 1;
	/* 0x0:2 */ unsigned int MFD : 2;
	/* 0x0:4 */ unsigned int STS : 2;
	/* 0x0:6 */ unsigned int STD : 2;
	/* 0x1:0 */ unsigned int RCYC : 3;
	/* 0x1:3 */ unsigned int p0 : 21;
} tD_CTRL;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int CIS0 : 1;
	/* 0x0:1 */ unsigned int CIS1 : 1;
	/* 0x0:2 */ unsigned int CIS2 : 1;
	/* 0x0:3 */ unsigned int CIS3 : 1;
	/* 0x0:4 */ unsigned int CIS4 : 1;
	/* 0x0:5 */ unsigned int CIS5 : 1;
	/* 0x0:6 */ unsigned int CIS6 : 1;
	/* 0x0:7 */ unsigned int CIS7 : 1;
	/* 0x1:0 */ unsigned int CIS8 : 1;
	/* 0x1:1 */ unsigned int CIS9 : 1;
	/* 0x1:2 */ unsigned int p0 : 3;
	/* 0x1:5 */ unsigned int SIS : 1;
	/* 0x1:6 */ unsigned int MEIS : 1;
	/* 0x1:7 */ unsigned int BEIS : 1;
	/* 0x2:0 */ unsigned int CIM0 : 1;
	/* 0x2:1 */ unsigned int CIM1 : 1;
	/* 0x2:2 */ unsigned int CIM2 : 1;
	/* 0x2:3 */ unsigned int CIM3 : 1;
	/* 0x2:4 */ unsigned int CIM4 : 1;
	/* 0x2:5 */ unsigned int CIM5 : 1;
	/* 0x2:6 */ unsigned int CIM6 : 1;
	/* 0x2:7 */ unsigned int CIM7 : 1;
	/* 0x3:0 */ unsigned int CIM8 : 1;
	/* 0x3:1 */ unsigned int CIM9 : 1;
	/* 0x3:2 */ unsigned int p1 : 3;
	/* 0x3:5 */ unsigned int SIM : 1;
	/* 0x3:6 */ unsigned int MEIM : 1;
	/* 0x3:7 */ unsigned int p2 : 1;
} tD_STAT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int CPC0 : 1;
	/* 0x0:1 */ unsigned int CPC1 : 1;
	/* 0x0:2 */ unsigned int CPC2 : 1;
	/* 0x0:3 */ unsigned int CPC3 : 1;
	/* 0x0:4 */ unsigned int CPC4 : 1;
	/* 0x0:5 */ unsigned int CPC5 : 1;
	/* 0x0:6 */ unsigned int CPC6 : 1;
	/* 0x0:7 */ unsigned int CPC7 : 1;
	/* 0x1:0 */ unsigned int CPC8 : 1;
	/* 0x1:1 */ unsigned int CPC9 : 1;
	/* 0x1:2 */ unsigned int p0 : 6;
	/* 0x2:0 */ unsigned int CDE0 : 1;
	/* 0x2:1 */ unsigned int CDE1 : 1;
	/* 0x2:2 */ unsigned int CDE2 : 1;
	/* 0x2:3 */ unsigned int CDE3 : 1;
	/* 0x2:4 */ unsigned int CDE4 : 1;
	/* 0x2:5 */ unsigned int CDE5 : 1;
	/* 0x2:6 */ unsigned int CDE6 : 1;
	/* 0x2:7 */ unsigned int CDE7 : 1;
	/* 0x3:0 */ unsigned int CDE8 : 1;
	/* 0x3:1 */ unsigned int CDE9 : 1;
	/* 0x3:2 */ unsigned int p1 : 5;
	/* 0x3:7 */ unsigned int PCE : 1;
} tD_PCR;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int SQWC : 8;
	/* 0x1:0 */ unsigned int p0 : 8;
	/* 0x2:0 */ unsigned int TQWC : 8;
	/* 0x3:0 */ unsigned int p1 : 8;
} tD_SQWC;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 4;
	/* 0x0:4 */ unsigned int RMSK : 27;
	/* 0x3:7 */ unsigned int p1 : 1;
} tD_RBSR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 4;
	/* 0x0:4 */ unsigned int RMSK : 27;
	/* 0x3:7 */ unsigned int p1 : 1;
} tD_RBOR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 4;
	/* 0x0:4 */ unsigned int ADDR : 27;
	/* 0x3:7 */ unsigned int p1 : 1;
} tD_STADR;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int GS : 1;
	/* 0x0:1 */ unsigned int SBUS : 1;
	/* 0x0:2 */ unsigned int VBON : 1;
	/* 0x0:3 */ unsigned int VBOF : 1;
	/* 0x0:4 */ unsigned int VIF0 : 1;
	/* 0x0:5 */ unsigned int VIF1 : 1;
	/* 0x0:6 */ unsigned int VU0 : 1;
	/* 0x0:7 */ unsigned int VU1 : 1;
	/* 0x1:0 */ unsigned int IPU : 1;
	/* 0x1:1 */ unsigned int TIM0 : 1;
	/* 0x1:2 */ unsigned int TIM1 : 1;
	/* 0x1:3 */ unsigned int TIM2 : 1;
	/* 0x1:4 */ unsigned int TIM3 : 1;
	/* 0x1:5 */ unsigned int SFIF0 : 1;
	/* 0x1:6 */ unsigned int VU0WD : 1;
	/* 0x1:7 */ unsigned int PGPU : 1;
	/* 0x2:0 */ unsigned int p0 : 16;
} tI_STAT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int GS : 1;
	/* 0x0:1 */ unsigned int SBUS : 1;
	/* 0x0:2 */ unsigned int VBON : 1;
	/* 0x0:3 */ unsigned int VBOF : 1;
	/* 0x0:4 */ unsigned int VIF0 : 1;
	/* 0x0:5 */ unsigned int VIF1 : 1;
	/* 0x0:6 */ unsigned int VU0 : 1;
	/* 0x0:7 */ unsigned int VU1 : 1;
	/* 0x1:0 */ unsigned int IPU : 1;
	/* 0x1:1 */ unsigned int TIM0 : 1;
	/* 0x1:2 */ unsigned int TIM1 : 1;
	/* 0x1:3 */ unsigned int TIM2 : 1;
	/* 0x1:4 */ unsigned int TIM3 : 1;
	/* 0x1:5 */ unsigned int SFIF0 : 1;
	/* 0x1:6 */ unsigned int VU0WD : 1;
	/* 0x1:7 */ unsigned int PGPU : 1;
	/* 0x2:0 */ unsigned int p0 : 16;
} tI_MASK;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int UMOD : 2;
	/* 0x0:2 */ unsigned int USBL : 1;
	/* 0x0:3 */ unsigned int UPEN : 1;
	/* 0x0:4 */ unsigned int UEPS : 1;
	/* 0x0:5 */ unsigned int SCS : 2;
	/* 0x0:7 */ unsigned int p0 : 1;
	/* 0x1:0 */ unsigned int HSE : 1;
	/* 0x1:1 */ unsigned int p1 : 4;
	/* 0x1:5 */ unsigned int UODE : 1;
	/* 0x1:6 */ unsigned int TWUB : 1;
	/* 0x1:7 */ unsigned int RWUB : 1;
	/* 0x2:0 */ unsigned int p2 : 16;
} tS_LCR;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int DATA0 : 8;
	/* 0x1:0 */ unsigned int DATA1 : 8;
	/* 0x2:0 */ unsigned int DATA2 : 8;
	/* 0x3:0 */ unsigned int DATA3 : 8;
} tSB_MSCOM;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int DATA0 : 8;
	/* 0x1:0 */ unsigned int DATA1 : 8;
	/* 0x2:0 */ unsigned int DATA2 : 8;
	/* 0x3:0 */ unsigned int DATA3 : 8;
} tSB_SMCOM;

typedef struct { // 0x4
	/* 0x0 */ unsigned int FLAG;
} tSB_MSFLG;

typedef struct { // 0x4
	/* 0x0 */ unsigned int FLAG;
} tSB_SMFLG;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int MRQM : 1;
	/* 0x0:1 */ unsigned int MDE : 3;
	/* 0x0:4 */ unsigned int SRQM : 1;
	/* 0x0:5 */ unsigned int SDE : 3;
	/* 0x1:0 */ unsigned int MSOK : 1;
	/* 0x1:1 */ unsigned int STOP : 1;
	/* 0x1:2 */ unsigned int p0 : 1;
	/* 0x1:3 */ unsigned int DIR : 1;
	/* 0x1:4 */ unsigned int DMAS : 2;
	/* 0x1:6 */ unsigned int ACH : 2;
	/* 0x2:0 */ unsigned int DBSY : 1;
	/* 0x2:1 */ unsigned int LOCK : 1;
	/* 0x2:2 */ unsigned int SINT : 1;
	/* 0x2:3 */ unsigned int SRST : 1;
	/* 0x2:4 */ unsigned int MSBE : 4;
	/* 0x3:0 */ unsigned int SMBE : 4;
	/* 0x3:4 */ unsigned int FIFO : 4;
} tSB_STAT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int MRQM : 1;
	/* 0x0:1 */ unsigned int p0 : 7;
	/* 0x1:0 */ unsigned int MSOK : 1;
	/* 0x1:1 */ unsigned int STOP : 1;
	/* 0x1:2 */ unsigned int p1 : 3;
	/* 0x1:5 */ unsigned int DMAS : 1;
	/* 0x1:6 */ unsigned int p2 : 3;
	/* 0x2:1 */ unsigned int LOCK : 1;
	/* 0x2:2 */ unsigned int SINT : 1;
	/* 0x2:3 */ unsigned int SRST : 1;
	/* 0x2:4 */ unsigned int p3 : 12;
} tSB_MCNT;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int IOER : 1;
	/* 0x0:1 */ unsigned int DSER : 1;
	/* 0x0:2 */ unsigned int DMER : 1;
	/* 0x0:3 */ unsigned int DMIS : 1;
	/* 0x0:4 */ unsigned int MDIR : 1;
	/* 0x0:5 */ unsigned int FDST : 1;
	/* 0x4:0 */ unsigned int p0 : 27;
} tSB_ERR;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int TBX : 4;
	/* 0x0:4 */ unsigned int TBY : 1;
	/* 0x0:5 */ unsigned int ABR : 2;
	/* 0x0:7 */ unsigned int TPF : 2;
	/* 0x1:1 */ unsigned int DTD : 1;
	/* 0x1:2 */ unsigned int DFE : 1;
	/* 0x1:3 */ unsigned int PBW : 1;
	/* 0x1:4 */ unsigned int PBC : 1;
	/* 0x1:5 */ unsigned int p0 : 3;
	/* 0x2:0 */ unsigned int HDS2 : 1;
	/* 0x2:1 */ unsigned int HDS : 2;
	/* 0x2:3 */ unsigned int VDS : 1;
	/* 0x2:4 */ unsigned int NPB : 1;
	/* 0x2:5 */ unsigned int LBS : 1;
	/* 0x2:6 */ unsigned int IRS : 1;
	/* 0x2:7 */ unsigned int DMSK : 1;
	/* 0x3:0 */ unsigned int IRQ : 1;
	/* 0x3:1 */ unsigned int DREQ : 1;
	/* 0x3:2 */ unsigned int IDLE : 1;
	/* 0x3:3 */ unsigned int RFFL : 1;
	/* 0x3:4 */ unsigned int WFEP : 1;
	/* 0x3:5 */ unsigned int DMD : 2;
	/* 0x3:7 */ unsigned int ODE : 1;
} tPG_STAT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int PARAMETER : 4;
} tPG_PA0;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int PARAMETER : 4;
} tPG_PA1;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int PARAMETER : 4;
} tPG_PA2;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int PARAMETER : 4;
} tPG_PA3;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int DATA : 4;
} tPG_MSCOM;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int DATA : 4;
} tPG_SMCOM;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int IDMA : 1;
	/* 0x0:1 */ unsigned int IFIF : 1;
	/* 0x0:2 */ unsigned int ICOM : 1;
	/* 0x0:3 */ unsigned int DRON : 1;
	/* 0x0:4 */ unsigned int DMOD : 2;
	/* 0x0:6 */ unsigned int MSE : 1;
	/* 0x0:7 */ unsigned int SME : 1;
	/* 0x1:0 */ unsigned int VGF : 5;
	/* 0x1:5 */ unsigned int p0 : 3;
	/* 0x2:0 */ unsigned int VSF : 3;
	/* 0x2:3 */ unsigned int p1 : 1;
	/* 0x2:4 */ unsigned int PMOD : 1;
	/* 0x2:5 */ unsigned int p2 : 3;
	/* 0x3:0 */ unsigned int IDMAF : 1;
	/* 0x3:1 */ unsigned int IFIFF : 1;
	/* 0x3:2 */ unsigned int ICOMF : 1;
	/* 0x3:3 */ unsigned int p3 : 5;
} tPG_IF;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int EN : 1;
	/* 0x0:1 */ unsigned int p0 : 31;
} tPM_CNT;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int TMOUT : 16;
	/* 0x2:0 */ unsigned int p0 : 15;
	/* 0x3:7 */ unsigned int EN : 1;
} tVU0WD_CTRL;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int EN1 : 1;
	/* 0x0:1 */ unsigned int EN2 : 1;
	/* 0x0:2 */ unsigned int CRTMD : 3;
	/* 0x0:5 */ unsigned int MMOD : 1;
	/* 0x0:6 */ unsigned int AMOD : 1;
	/* 0x0:7 */ unsigned int SLBG : 1;
	/* 0x1:0 */ unsigned int ALP : 8;
	/* 0x2:0 */ unsigned int NFLD : 1;
	/* 0x2:1 */ unsigned int p0 : 15;
	/* 0x4:0 */ unsigned int EXVWINS : 10;
	/* 0x5:2 */ unsigned int EXVWINE : 10;
	/* 0x6:4 */ unsigned int EXSYNCMD : 1;
	/* 0x6:5 */ unsigned int p1 : 11;
} tGS_PMODE;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int RC : 3;
	/* 0x0:3 */ unsigned int LC : 7;
	/* 0x1:2 */ unsigned int T1248 : 2;
	/* 0x1:4 */ unsigned int SCLK : 1;
	/* 0x1:5 */ unsigned int CMOD : 2;
	/* 0x1:7 */ unsigned int EX : 1;
	/* 0x2:0 */ unsigned int PRST : 1;
	/* 0x2:1 */ unsigned int SINT : 1;
	/* 0x2:2 */ unsigned int XPCK : 1;
	/* 0x2:3 */ unsigned int PCK2 : 2;
	/* 0x2:5 */ unsigned int SPML : 4;
	/* 0x3:1 */ unsigned int GCONT : 1;
	/* 0x3:2 */ unsigned int PHS : 1;
	/* 0x3:3 */ unsigned int PVS : 1;
	/* 0x3:4 */ unsigned int PEHS : 1;
	/* 0x3:5 */ unsigned int PEVS : 1;
	/* 0x3:6 */ unsigned int CLKSEL : 2;
	/* 0x4:0 */ unsigned int NVCK : 1;
	/* 0x4:1 */ unsigned int SLCK2 : 1;
	/* 0x4:2 */ unsigned int VCKSEL : 2;
	/* 0x4:4 */ unsigned int VHP : 1;
	/* 0x4:5 */ unsigned int COFF : 1;
	/* 0x4:6 */ unsigned int p0 : 26;
} tGS_SMODE1;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int INT : 1;
	/* 0x0:1 */ unsigned int FFMD : 1;
	/* 0x0:2 */ unsigned int DPMS : 2;
	/* 0x0:4 */ unsigned int p0 : 28;
} tGS_SMODE2;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int RFSH : 6;
	/* 0x0:6 */ unsigned int p0 : 26;
} tGS_SRFSH;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int HFP : 11;
	/* 0x1:3 */ unsigned int HBP : 11;
	/* 0x2:6 */ unsigned int HSEQ : 10;
	/* 0x4:0 */ unsigned int HSVS : 11;
	/* 0x5:3 */ unsigned int HS : 10;
	/* 0x6:5 */ unsigned int p0 : 11;
} tGS_SYNCH1;

typedef struct { // 0xc
	/* 0x0:0 */ unsigned int HF : 11;
	/* 0x1:3 */ unsigned int HB : 11;
	/* 0x4:0 */ unsigned int p0 : 20;
	/* 0x8 */ unsigned int p1;
} tGS_SYNCH2;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int VFP : 10;
	/* 0x1:2 */ unsigned int VFPE : 10;
	/* 0x2:4 */ unsigned int VBP : 10;
	/* 0x3:6 */ unsigned int p0 : 2;
	/* 0x4:0 */ unsigned int VBPE : 10;
	/* 0x5:2 */ unsigned int VDP : 11;
	/* 0x6:5 */ unsigned int VS : 10;
	/* 0x7:7 */ unsigned int p1 : 1;
} tGS_SYNCV;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int FBP : 9;
	/* 0x1:1 */ unsigned int FBW : 6;
	/* 0x1:7 */ unsigned int PSM : 5;
	/* 0x2:4 */ unsigned int p0 : 12;
	/* 0x4:0 */ unsigned int DBX : 11;
	/* 0x5:3 */ unsigned int DBY : 11;
	/* 0x6:6 */ unsigned int p1 : 10;
} tGS_DISPFB1;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int DX : 12;
	/* 0x1:4 */ unsigned int DY : 11;
	/* 0x2:7 */ unsigned int MAGH : 4;
	/* 0x3:3 */ unsigned int MAGV : 2;
	/* 0x3:5 */ unsigned int p0 : 3;
	/* 0x4:0 */ unsigned int DW : 12;
	/* 0x5:4 */ unsigned int DH : 11;
	/* 0x6:7 */ unsigned int p1 : 9;
} tGS_DISPLAY1;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int FBP : 9;
	/* 0x1:1 */ unsigned int FBW : 6;
	/* 0x1:7 */ unsigned int PSM : 5;
	/* 0x2:4 */ unsigned int p0 : 12;
	/* 0x4:0 */ unsigned int DBX : 11;
	/* 0x5:3 */ unsigned int DBY : 11;
	/* 0x6:6 */ unsigned int p1 : 10;
} tGS_DISPFB2;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int DX : 12;
	/* 0x1:4 */ unsigned int DY : 11;
	/* 0x2:7 */ unsigned int MAGH : 4;
	/* 0x3:3 */ unsigned int MAGV : 2;
	/* 0x3:5 */ unsigned int p0 : 3;
	/* 0x4:0 */ unsigned int DW : 12;
	/* 0x5:4 */ unsigned int DH : 11;
	/* 0x6:7 */ unsigned int p1 : 9;
} tGS_DISPLAY2;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int EN1 : 1;
	/* 0x0:1 */ unsigned int EN2 : 1;
	/* 0x0:2 */ unsigned int CRTMD : 3;
	/* 0x0:5 */ unsigned int MMOD : 1;
	/* 0x0:6 */ unsigned int AMOD : 1;
	/* 0x0:7 */ unsigned int SLBG : 1;
	/* 0x1:0 */ unsigned int ALP : 8;
	/* 0x2:0 */ unsigned int NFLD : 1;
	/* 0x2:1 */ unsigned int p0 : 15;
	/* 0x4:0 */ unsigned int EXVWINS : 10;
	/* 0x5:2 */ unsigned int EXVWINE : 10;
	/* 0x6:4 */ unsigned int EXSYNCMD : 1;
	/* 0x6:5 */ unsigned int p1 : 11;
} tGS_EXTBUF;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int SX : 12;
	/* 0x1:4 */ unsigned int SY : 11;
	/* 0x2:7 */ unsigned int SMPH : 4;
	/* 0x3:3 */ unsigned int SMPV : 2;
	/* 0x3:5 */ unsigned int p0 : 3;
	/* 0x4:0 */ unsigned int WW : 12;
	/* 0x5:4 */ unsigned int WH : 11;
	/* 0x6:7 */ unsigned int p1 : 9;
} tGS_EXTDATA;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int WRITE : 1;
	/* 0x0:1 */ unsigned int p0 : 31;
	/* 0x4 */ unsigned int p1;
} tGS_EXTWRITE;

// warning: multiple differing types with the same name (descriptor not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int R : 8;
	/* 0x1:0 */ unsigned int G : 8;
	/* 0x2:0 */ unsigned int B : 8;
	/* 0x3 */ unsigned char p0;
	/* 0x4 */ unsigned int p1;
} tGS_BGCOLOR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int SIGNAL : 1;
	/* 0x0:1 */ unsigned int FINISH : 1;
	/* 0x0:2 */ unsigned int HSINT : 1;
	/* 0x0:3 */ unsigned int VSINT : 1;
	/* 0x0:4 */ unsigned int EDWINT : 1;
	/* 0x0:5 */ unsigned int EXHSINT : 1;
	/* 0x0:6 */ unsigned int EXVSINT : 1;
	/* 0x0:7 */ unsigned int p0 : 1;
	/* 0x1:0 */ unsigned int FLUSH : 1;
	/* 0x1:1 */ unsigned int RESET : 1;
	/* 0x1:2 */ unsigned int EXVERR : 1;
	/* 0x1:3 */ unsigned int EXFIELD : 1;
	/* 0x1:4 */ unsigned int NFIELD : 1;
	/* 0x1:5 */ unsigned int FIELD : 1;
	/* 0x1:6 */ unsigned int FIFO : 2;
	/* 0x2:0 */ unsigned int REV : 8;
	/* 0x3:0 */ unsigned int ID : 8;
	/* 0x4 */ unsigned int p1;
} tGS_CSR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int p0 : 8;
	/* 0x1:0 */ unsigned int SIGMSK : 1;
	/* 0x1:1 */ unsigned int FINISHMSK : 1;
	/* 0x1:2 */ unsigned int HSMSK : 1;
	/* 0x1:3 */ unsigned int VSMSK : 1;
	/* 0x1:4 */ unsigned int EDWMSK : 1;
	/* 0x1:5 */ unsigned int EXHSMSK : 1;
	/* 0x1:6 */ unsigned int EXVSMSK : 1;
	/* 0x1:7 */ unsigned int p1 : 17;
	/* 0x4 */ unsigned int p2;
} tGS_IMR;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int DIR : 1;
	/* 0x0:1 */ unsigned int p0 : 31;
	/* 0x4 */ unsigned int p1;
} tGS_BUSDIR;

typedef struct { // 0x8
	/* 0x0 */ unsigned int ID;
	/* 0x4 */ unsigned int p0;
} tGS_SIGID;

typedef struct { // 0x8
	/* 0x0 */ unsigned int ID;
	/* 0x4 */ unsigned int p0;
} tGS_LABELID;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int AFMODE : 1;
	/* 0x0:1 */ unsigned int p0 : 31;
	/* 0x4 */ unsigned int p1;
} tGS_SYSCNT;

typedef struct { // 0x8
	/* 0x0:0 */ unsigned int DISPLAY : 2;
	/* 0x0:2 */ unsigned int p0 : 30;
	/* 0x4:0 */ unsigned int MAGV : 16;
	/* 0x6:0 */ unsigned int MAGH : 16;
} tGS_SIMU_DISPLAY;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int OPTION : 28;
	/* 0x3:4 */ unsigned int CODE : 4;
} tIPU_CMD_write;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x8
	/* 0x0 */ unsigned int DATA;
	/* 0x4:0 */ unsigned int p0 : 31;
	/* 0x7:7 */ unsigned int BUSY : 1;
} tIPU_CMD_read;

typedef struct { // 0x8
	/* 0x0 */ unsigned int BSTOP;
	/* 0x4:0 */ unsigned int p0 : 31;
	/* 0x7:7 */ unsigned int BUSY : 1;
} tIPU_TOP;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int tag : 14;
	/* 0x1:6 */ unsigned int PRE : 1;
	/* 0x1:7 */ unsigned int PRIM : 11;
	/* 0x3:2 */ unsigned int FLG : 2;
	/* 0x3:4 */ unsigned int NREG : 4;
} tGIF_TAG1;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int VPS : 2;
	/* 0x0:2 */ unsigned int VEW : 1;
	/* 0x0:3 */ unsigned int p0 : 3;
	/* 0x0:6 */ unsigned int MRK : 1;
	/* 0x0:7 */ unsigned int p1 : 1;
	/* 0x1:0 */ unsigned int VSS : 1;
	/* 0x1:1 */ unsigned int VFS : 1;
	/* 0x1:2 */ unsigned int VIS : 1;
	/* 0x1:3 */ unsigned int INT : 1;
	/* 0x1:4 */ unsigned int ERO : 1;
	/* 0x1:5 */ unsigned int ER1 : 1;
	/* 0x1:6 */ unsigned int p2 : 10;
	/* 0x3:0 */ unsigned int FQC : 4;
	/* 0x3:4 */ unsigned int p3 : 4;
} tVIF0_STAT;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int num : 8;
	/* 0x1:0 */ unsigned int p0 : 24;
} tVIF1_NUM;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int ADDR : 31;
	/* 0x3:7 */ unsigned int SPR : 1;
} tD_MADR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int ADDR : 31;
	/* 0x3:7 */ unsigned int SPR : 1;
} tD_TADR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int ADDR : 31;
	/* 0x3:7 */ unsigned int SPR : 1;
} tD_ASR0;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int ADDR : 31;
	/* 0x3:7 */ unsigned int SPR : 1;
} tD_ASR1;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int ADDR : 14;
	/* 0x1:6 */ unsigned int p0 : 18;
} tD_SADR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int RMSK : 31;
	/* 0x3:7 */ unsigned int p0 : 1;
} tD_RBSR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int ADDR : 31;
	/* 0x3:7 */ unsigned int p0 : 1;
} tD_RBOR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x4
	/* 0x0:0 */ unsigned int ADDR : 31;
	/* 0x3:7 */ unsigned int p0 : 1;
} tD_STADR;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 16;
	/* 0x2:0 */ unsigned int CPND : 1;
	/* 0x2:1 */ unsigned int p1 : 15;
} tD_ENABLER;

typedef struct { // 0x4
	/* 0x0:0 */ unsigned int p0 : 16;
	/* 0x2:0 */ unsigned int CPND : 1;
	/* 0x2:1 */ unsigned int p1 : 15;
} tD_ENABLEW;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int EN1 : 1;
	/* 0x0:1 */ unsigned int EN2 : 1;
	/* 0x0:2 */ unsigned int CRTMD : 3;
	/* 0x0:5 */ unsigned int MMOD : 1;
	/* 0x0:6 */ unsigned int AMOD : 1;
	/* 0x0:7 */ unsigned int SLBG : 1;
	/* 0x1:0 */ unsigned int ALP : 8;
	/* 0x2:0 */ unsigned int p0 : 16;
	/* 0x4 */ unsigned int p1;
} tGS_PMODE;

// warning: multiple differing types with the same name (size not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int INT : 1;
	/* 0x0:1 */ unsigned int FFMD : 1;
	/* 0x0:2 */ unsigned int DPMS : 2;
	/* 0x0:4 */ unsigned int p0 : 28;
	/* 0x4 */ unsigned int p1;
} tGS_SMODE2;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int EXBP : 14;
	/* 0x1:6 */ unsigned int EXBW : 6;
	/* 0x2:4 */ unsigned int FBIN : 2;
	/* 0x2:6 */ unsigned int WFFMD : 1;
	/* 0x2:7 */ unsigned int EMODA : 2;
	/* 0x3:1 */ unsigned int EMODC : 2;
	/* 0x3:3 */ unsigned int p0 : 5;
	/* 0x4:0 */ unsigned int WDX : 11;
	/* 0x5:3 */ unsigned int WDY : 11;
	/* 0x6:6 */ unsigned int p1 : 10;
} tGS_EXTBUF;

// warning: multiple differing types with the same name (descriptor not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int R : 8;
	/* 0x1:0 */ unsigned int G : 8;
	/* 0x2:0 */ unsigned int B : 8;
	/* 0x3:0 */ unsigned int p0 : 8;
	/* 0x4 */ unsigned int p1;
} tGS_BGCOLOR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int SIGNAL : 1;
	/* 0x0:1 */ unsigned int FINISH : 1;
	/* 0x0:2 */ unsigned int HSINT : 1;
	/* 0x0:3 */ unsigned int VSINT : 1;
	/* 0x0:4 */ unsigned int EDWINT : 1;
	/* 0x0:5 */ unsigned int p0 : 3;
	/* 0x1:0 */ unsigned int FLUSH : 1;
	/* 0x1:1 */ unsigned int RESET : 1;
	/* 0x1:2 */ unsigned int p1 : 2;
	/* 0x1:4 */ unsigned int NFIELD : 1;
	/* 0x1:5 */ unsigned int FIELD : 1;
	/* 0x1:6 */ unsigned int FIFO : 2;
	/* 0x2:0 */ unsigned int REV : 8;
	/* 0x3:0 */ unsigned int ID : 8;
	/* 0x4 */ unsigned int p2;
} tGS_CSR;

// warning: multiple differing types with the same name (fields size not equal)
typedef struct { // 0x8
	/* 0x0:0 */ unsigned int p0 : 8;
	/* 0x1:0 */ unsigned int SIGMSK : 1;
	/* 0x1:1 */ unsigned int FINISHMSK : 1;
	/* 0x1:2 */ unsigned int HSMSK : 1;
	/* 0x1:3 */ unsigned int VSMSK : 1;
	/* 0x1:4 */ unsigned int EDWMSK : 1;
	/* 0x1:5 */ unsigned int p1 : 19;
	/* 0x4 */ unsigned int p2;
} tGS_IMR;

typedef struct { // 0x8
	/* 0x0 */ unsigned int SIGID;
	/* 0x4 */ unsigned int LBLID;
} tGS_SIGLBLID;

struct rqueue { // 0x408
	/* 0x000 */ int top;
	/* 0x004 */ int btm;
	/* 0x008 */ struct { // 0x2
		/* 0x008 */ u_char type;
		/* 0x009 */ u_char id;
	} que[512];
};

typedef struct { // 0x8
	/* 0x0 */ short unsigned int len;
	/* 0x2 */ short unsigned int rsvd;
	/* 0x4 */ short unsigned int proto;
	/* 0x6 */ unsigned char src;
	/* 0x7 */ unsigned char dest;
} sceDeci2Hdr;

typedef struct { // 0x110
	/* 0x000 */ int num;
	/* 0x004 */ int count;
	/* 0x008 */ void *rp;
	/* 0x00c */ void *wp;
	/* 0x010 */ char buf[256];
} CHQUEUE;

typedef struct { // 0x1c
	/* 0x00 */ int s;
	/* 0x04 */ u_int wlen;
	/* 0x08 */ u_int rlen;
	/* 0x0c */ u_int isSending;
	/* 0x10 */ char *wptr;
	/* 0x14 */ char *rptr;
	/* 0x18 */ CHQUEUE *rq;
} TTYINFO;

enum {
	__no_type_class = -1,
	__void_type_class = 0,
	__integer_type_class = 1,
	__char_type_class = 2,
	__enumeral_type_class = 3,
	__boolean_type_class = 4,
	__pointer_type_class = 5,
	__reference_type_class = 6,
	__offset_type_class = 7,
	__real_type_class = 8,
	__complex_type_class = 9,
	__function_type_class = 10,
	__method_type_class = 11,
	__record_type_class = 12,
	__union_type_class = 13,
	__array_type_class = 14,
	__string_type_class = 15,
	__set_type_class = 16,
	__file_type_class = 17,
	__lang_type_class = 18
};

typedef struct { // 0x10
	/* 0x0:0 */ long unsigned int NLOOP : 15;
	/* 0x1:7 */ long unsigned int EOP : 1;
	/* 0x2:0 */ long unsigned int pad16 : 16;
	/* 0x4:0 */ long unsigned int id : 14;
	/* 0x5:6 */ long unsigned int PRE : 1;
	/* 0x5:7 */ long unsigned int PRIM : 11;
	/* 0x7:2 */ long unsigned int FLG : 2;
	/* 0x7:4 */ long unsigned int NREG : 4;
	/* 0x8:0 */ long unsigned int REGS0 : 4;
	/* 0x8:4 */ long unsigned int REGS1 : 4;
	/* 0x9:0 */ long unsigned int REGS2 : 4;
	/* 0x9:4 */ long unsigned int REGS3 : 4;
	/* 0xa:0 */ long unsigned int REGS4 : 4;
	/* 0xa:4 */ long unsigned int REGS5 : 4;
	/* 0xb:0 */ long unsigned int REGS6 : 4;
	/* 0xb:4 */ long unsigned int REGS7 : 4;
	/* 0xc:0 */ long unsigned int REGS8 : 4;
	/* 0xc:4 */ long unsigned int REGS9 : 4;
	/* 0xd:0 */ long unsigned int REGS10 : 4;
	/* 0xd:4 */ long unsigned int REGS11 : 4;
	/* 0xe:0 */ long unsigned int REGS12 : 4;
	/* 0xe:4 */ long unsigned int REGS13 : 4;
	/* 0xf:0 */ long unsigned int REGS14 : 4;
	/* 0xf:4 */ long unsigned int REGS15 : 4;
} sceGifTag;

struct _sceGifPackRgbaq { // 0x10
	/* 0x0 */ u_int R;
	/* 0x4 */ u_int G;
	/* 0x8 */ u_int B;
	/* 0xc */ u_int A;
};

typedef _sceGifPackRgbaq sceGifPackRgbaq;

struct _sceGifPackAd { // 0x10
	/* 0x0 */ u_long DATA;
	/* 0x8 */ u_long ADDR;
};

typedef _sceGifPackAd sceGifPackAd;

struct _sceGifPackSt { // 0x10
	/* 0x0 */ float S;
	/* 0x4 */ float T;
	/* 0x8 */ float Q;
	/* 0xc */ u_int pad96;
};

typedef _sceGifPackSt sceGifPackSt;

struct _sceGifPackUv { // 0x10
	/* 0x0 */ int U;
	/* 0x4 */ int V;
	/* 0x8 */ long int pad64;
};

typedef _sceGifPackUv sceGifPackUv;

struct _sceGifPackXyzf { // 0x10
	/* 0x0 */ int X;
	/* 0x4 */ int Y;
	/* 0x8 */ u_int Z;
	/* 0xc:0 */ u_int F : 12;
	/* 0xd:4 */ u_int pad108 : 3;
	/* 0xd:7 */ u_int ADC : 1;
	/* 0xe:0 */ u_int pad112 : 16;
};

typedef _sceGifPackXyzf sceGifPackXyzf;

struct _sceGifPackXyz { // 0x10
	/* 0x0 */ int X;
	/* 0x4 */ int Y;
	/* 0x8 */ u_int Z;
	/* 0xc:0 */ u_int pad96 : 15;
	/* 0xd:7 */ u_int ADC : 1;
	/* 0xe:0 */ u_int pad112 : 16;
};

typedef _sceGifPackXyz sceGifPackXyz;

struct _sceGifPackFog { // 0x10
	/* 0x0 */ u_int pad[3];
	/* 0xc */ u_int F;
};

typedef _sceGifPackFog sceGifPackFog;

struct _sceGifPackNop { // 0x10
	/* 0x0 */ u_long pad[2];
};

typedef _sceGifPackNop sceGifPackNop;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int A : 2;
	/* 0x0:2 */ long unsigned int B : 2;
	/* 0x0:4 */ long unsigned int C : 2;
	/* 0x0:6 */ long unsigned int D : 2;
	/* 0x1:0 */ long unsigned int pad8 : 24;
	/* 0x4:0 */ long unsigned int FIX : 8;
	/* 0x5:0 */ long unsigned int pad40 : 24;
} sceGsAlpha;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int SBP : 14;
	/* 0x1:6 */ long unsigned int pad14 : 2;
	/* 0x2:0 */ long unsigned int SBW : 6;
	/* 0x2:6 */ long unsigned int pad22 : 2;
	/* 0x3:0 */ long unsigned int SPSM : 6;
	/* 0x3:6 */ long unsigned int pad30 : 2;
	/* 0x4:0 */ long unsigned int DBP : 14;
	/* 0x5:6 */ long unsigned int pad46 : 2;
	/* 0x6:0 */ long unsigned int DBW : 6;
	/* 0x6:6 */ long unsigned int pad54 : 2;
	/* 0x7:0 */ long unsigned int DPSM : 6;
	/* 0x7:6 */ long unsigned int pad62 : 2;
} sceGsBitbltbuf;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int WMS : 2;
	/* 0x0:2 */ long unsigned int WMT : 2;
	/* 0x0:4 */ long unsigned int MINU : 10;
	/* 0x1:6 */ long unsigned int MAXU : 10;
	/* 0x3:0 */ long unsigned int MINV : 10;
	/* 0x4:2 */ long unsigned int MAXV : 10;
	/* 0x5:4 */ long unsigned int pad44 : 20;
} sceGsClamp;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int CLAMP : 1;
	/* 0x0:1 */ long unsigned int pad01 : 63;
} sceGsColclamp;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int DIMX00 : 3;
	/* 0x0:3 */ long unsigned int pad00 : 1;
	/* 0x0:4 */ long unsigned int DIMX01 : 3;
	/* 0x0:7 */ long unsigned int pad01 : 1;
	/* 0x1:0 */ long unsigned int DIMX02 : 3;
	/* 0x1:3 */ long unsigned int pad02 : 1;
	/* 0x1:4 */ long unsigned int DIMX03 : 3;
	/* 0x1:7 */ long unsigned int pad03 : 1;
	/* 0x2:0 */ long unsigned int DIMX10 : 3;
	/* 0x2:3 */ long unsigned int pad10 : 1;
	/* 0x2:4 */ long unsigned int DIMX11 : 3;
	/* 0x2:7 */ long unsigned int pad11 : 1;
	/* 0x3:0 */ long unsigned int DIMX12 : 3;
	/* 0x3:3 */ long unsigned int pad12 : 1;
	/* 0x3:4 */ long unsigned int DIMX13 : 3;
	/* 0x3:7 */ long unsigned int pad13 : 1;
	/* 0x4:0 */ long unsigned int DIMX20 : 3;
	/* 0x4:3 */ long unsigned int pad20 : 1;
	/* 0x4:4 */ long unsigned int DIMX21 : 3;
	/* 0x4:7 */ long unsigned int pad21 : 1;
	/* 0x5:0 */ long unsigned int DIMX22 : 3;
	/* 0x5:3 */ long unsigned int pad22 : 1;
	/* 0x5:4 */ long unsigned int DIMX23 : 3;
	/* 0x5:7 */ long unsigned int pad23 : 1;
	/* 0x6:0 */ long unsigned int DIMX30 : 3;
	/* 0x6:3 */ long unsigned int pad30 : 1;
	/* 0x6:4 */ long unsigned int DIMX31 : 3;
	/* 0x6:7 */ long unsigned int pad31 : 1;
	/* 0x7:0 */ long unsigned int DIMX32 : 3;
	/* 0x7:3 */ long unsigned int pad32 : 1;
	/* 0x7:4 */ long unsigned int DIMX33 : 3;
	/* 0x7:7 */ long unsigned int pad33 : 1;
} sceGsDimx;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int DTHE : 1;
	/* 0x0:1 */ long unsigned int pad01 : 63;
} sceGsDthe;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int FBA : 1;
	/* 0x0:1 */ long unsigned int pad01 : 63;
} sceGsFba;

typedef struct { // 0x8
	/* 0x0 */ long unsigned int pad00;
} sceGsFinish;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int pad00 : 56;
	/* 0x7:0 */ long unsigned int F : 8;
} sceGsFog;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int FCR : 8;
	/* 0x1:0 */ long unsigned int FCG : 8;
	/* 0x2:0 */ long unsigned int FCB : 8;
	/* 0x3:0 */ long unsigned int pad24 : 40;
} sceGsFogcol;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int FBP : 9;
	/* 0x1:1 */ long unsigned int pad09 : 7;
	/* 0x2:0 */ long unsigned int FBW : 6;
	/* 0x2:6 */ long unsigned int pad22 : 2;
	/* 0x3:0 */ long unsigned int PSM : 6;
	/* 0x3:6 */ long unsigned int pad30 : 2;
	/* 0x4:0 */ long unsigned int FBMSK : 32;
} sceGsFrame;

typedef struct { // 0x8
	/* 0x0 */ long unsigned int WDATA;
} sceGsHwreg;

typedef struct { // 0x8
	/* 0x0 */ u_int ID;
	/* 0x4 */ u_int IDMSK;
} sceGsLabel;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int TBP1 : 14;
	/* 0x1:6 */ long unsigned int TBW1 : 6;
	/* 0x2:4 */ long unsigned int TBP2 : 14;
	/* 0x4:2 */ long unsigned int TBW2 : 6;
	/* 0x5:0 */ long unsigned int TBP3 : 14;
	/* 0x6:6 */ long unsigned int TBW3 : 6;
	/* 0x7:4 */ long unsigned int pad60 : 4;
} sceGsMiptbp1;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int TBP4 : 14;
	/* 0x1:6 */ long unsigned int TBW4 : 6;
	/* 0x2:4 */ long unsigned int TBP5 : 14;
	/* 0x4:2 */ long unsigned int TBW5 : 6;
	/* 0x5:0 */ long unsigned int TBP6 : 14;
	/* 0x6:6 */ long unsigned int TBW6 : 6;
	/* 0x7:4 */ long unsigned int pad60 : 4;
} sceGsMiptbp2;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int PABE : 1;
	/* 0x0:1 */ long unsigned int pad01 : 63;
} sceGsPabe;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int PRIM : 3;
	/* 0x0:3 */ long unsigned int IIP : 1;
	/* 0x0:4 */ long unsigned int TME : 1;
	/* 0x0:5 */ long unsigned int FGE : 1;
	/* 0x0:6 */ long unsigned int ABE : 1;
	/* 0x0:7 */ long unsigned int AA1 : 1;
	/* 0x1:0 */ long unsigned int FST : 1;
	/* 0x1:1 */ long unsigned int CTXT : 1;
	/* 0x1:2 */ long unsigned int FIX : 1;
	/* 0x1:3 */ long unsigned int pad11 : 53;
} sceGsPrim;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int pad00 : 3;
	/* 0x0:3 */ long unsigned int IIP : 1;
	/* 0x0:4 */ long unsigned int TME : 1;
	/* 0x0:5 */ long unsigned int FGE : 1;
	/* 0x0:6 */ long unsigned int ABE : 1;
	/* 0x0:7 */ long unsigned int AA1 : 1;
	/* 0x1:0 */ long unsigned int FST : 1;
	/* 0x1:1 */ long unsigned int CTXT : 1;
	/* 0x1:2 */ long unsigned int FIX : 1;
	/* 0x1:3 */ long unsigned int pad11 : 53;
} sceGsPrmode;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int AC : 1;
	/* 0x0:1 */ long unsigned int pad01 : 63;
} sceGsPrmodecont;

typedef struct { // 0x8
	/* 0x0:0 */ u_int R : 8;
	/* 0x1:0 */ u_int G : 8;
	/* 0x2:0 */ u_int B : 8;
	/* 0x3:0 */ u_int A : 8;
	/* 0x4 */ float Q;
} sceGsRgbaq;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int MSK : 2;
	/* 0x0:2 */ long unsigned int pad02 : 62;
} sceGsScanmsk;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int SCAX0 : 11;
	/* 0x1:3 */ long unsigned int pad11 : 5;
	/* 0x2:0 */ long unsigned int SCAX1 : 11;
	/* 0x3:3 */ long unsigned int pad27 : 5;
	/* 0x4:0 */ long unsigned int SCAY0 : 11;
	/* 0x5:3 */ long unsigned int pad43 : 5;
	/* 0x6:0 */ long unsigned int SCAY1 : 11;
	/* 0x7:3 */ long unsigned int pad59 : 5;
} sceGsScissor;

typedef struct { // 0x8
	/* 0x0 */ u_int ID;
	/* 0x4 */ u_int IDMSK;
} sceGsSignal;

typedef struct { // 0x8
	/* 0x0 */ float S;
	/* 0x4 */ float T;
} sceGsSt;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int ATE : 1;
	/* 0x0:1 */ long unsigned int ATST : 3;
	/* 0x0:4 */ long unsigned int AREF : 8;
	/* 0x1:4 */ long unsigned int AFAIL : 2;
	/* 0x1:6 */ long unsigned int DATE : 1;
	/* 0x1:7 */ long unsigned int DATM : 1;
	/* 0x2:0 */ long unsigned int ZTE : 1;
	/* 0x2:1 */ long unsigned int ZTST : 2;
	/* 0x2:3 */ long unsigned int pad19 : 45;
} sceGsTest;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int TBP0 : 14;
	/* 0x1:6 */ long unsigned int TBW : 6;
	/* 0x2:4 */ long unsigned int PSM : 6;
	/* 0x3:2 */ long unsigned int TW : 4;
	/* 0x3:6 */ long unsigned int TH : 4;
	/* 0x4:2 */ long unsigned int TCC : 1;
	/* 0x4:3 */ long unsigned int TFX : 2;
	/* 0x4:5 */ long unsigned int CBP : 14;
	/* 0x6:3 */ long unsigned int CPSM : 4;
	/* 0x6:7 */ long unsigned int CSM : 1;
	/* 0x7:0 */ long unsigned int CSA : 5;
	/* 0x7:5 */ long unsigned int CLD : 3;
} sceGsTex0;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int LCM : 1;
	/* 0x0:1 */ long unsigned int pad01 : 1;
	/* 0x0:2 */ long unsigned int MXL : 3;
	/* 0x0:5 */ long unsigned int MMAG : 1;
	/* 0x0:6 */ long unsigned int MMIN : 3;
	/* 0x1:1 */ long unsigned int MTBA : 1;
	/* 0x1:2 */ long unsigned int pad10 : 9;
	/* 0x2:3 */ long unsigned int L : 2;
	/* 0x2:5 */ long unsigned int pad21 : 11;
	/* 0x4:0 */ long unsigned int K : 12;
	/* 0x5:4 */ long unsigned int pad44 : 20;
} sceGsTex1;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int pad00 : 20;
	/* 0x2:4 */ long unsigned int PSM : 6;
	/* 0x3:2 */ long unsigned int pad26 : 11;
	/* 0x4:5 */ long unsigned int CBP : 14;
	/* 0x6:3 */ long unsigned int CPSM : 4;
	/* 0x6:7 */ long unsigned int CSM : 1;
	/* 0x7:0 */ long unsigned int CSA : 5;
	/* 0x7:5 */ long unsigned int CLD : 3;
} sceGsTex2;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int TA0 : 8;
	/* 0x1:0 */ long unsigned int pad08 : 7;
	/* 0x1:7 */ long unsigned int AEM : 1;
	/* 0x2:0 */ long unsigned int pad16 : 16;
	/* 0x4:0 */ long unsigned int TA1 : 8;
	/* 0x5:0 */ long unsigned int pad40 : 24;
} sceGsTexa;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int CBW : 6;
	/* 0x0:6 */ long unsigned int COU : 6;
	/* 0x1:4 */ long unsigned int COV : 10;
	/* 0x2:6 */ long unsigned int pad22 : 42;
} sceGsTexclut;

typedef struct { // 0x8
	/* 0x0 */ long unsigned int pad00;
} sceGsTexflush;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int XDR : 2;
	/* 0x0:2 */ long unsigned int pad02 : 62;
} sceGsTrxdir;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int SSAX : 11;
	/* 0x1:3 */ long unsigned int pad11 : 5;
	/* 0x2:0 */ long unsigned int SSAY : 11;
	/* 0x3:3 */ long unsigned int pad27 : 5;
	/* 0x4:0 */ long unsigned int DSAX : 11;
	/* 0x5:3 */ long unsigned int pad43 : 5;
	/* 0x6:0 */ long unsigned int DSAY : 11;
	/* 0x7:3 */ long unsigned int DIR : 2;
	/* 0x7:5 */ long unsigned int pad61 : 3;
} sceGsTrxpos;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int RRW : 12;
	/* 0x1:4 */ long unsigned int pad12 : 20;
	/* 0x4:0 */ long unsigned int RRH : 12;
	/* 0x5:4 */ long unsigned int pad44 : 20;
} sceGsTrxreg;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int U : 14;
	/* 0x1:6 */ long unsigned int pad14 : 2;
	/* 0x2:0 */ long unsigned int V : 14;
	/* 0x3:6 */ long unsigned int pad30 : 34;
} sceGsUv;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int OFX : 16;
	/* 0x2:0 */ long unsigned int pad16 : 16;
	/* 0x4:0 */ long unsigned int OFY : 16;
	/* 0x6:0 */ long unsigned int pad48 : 16;
} sceGsXyoffset;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int X : 16;
	/* 0x2:0 */ long unsigned int Y : 16;
	/* 0x4:0 */ long unsigned int Z : 32;
} sceGsXyz;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int X : 16;
	/* 0x2:0 */ long unsigned int Y : 16;
	/* 0x4:0 */ long unsigned int Z : 24;
	/* 0x7:0 */ long unsigned int F : 8;
} sceGsXyzf;

typedef struct { // 0x8
	/* 0x0:0 */ long unsigned int ZBP : 9;
	/* 0x1:1 */ long unsigned int pad09 : 15;
	/* 0x3:0 */ long unsigned int PSM : 4;
	/* 0x3:4 */ long unsigned int pad28 : 4;
	/* 0x4:0 */ long unsigned int ZMSK : 1;
	/* 0x4:1 */ long unsigned int pad33 : 31;
} sceGsZbuf;

typedef struct { // 0x10
	/* 0x0 */ unsigned int data;
	/* 0x4 */ unsigned int addr;
	/* 0x8 */ unsigned int size;
	/* 0xc */ unsigned int mode;
} sceSifDmaData;

typedef struct { // 0x10
	/* 0x0:0 */ unsigned int psize : 8;
	/* 0x1:0 */ unsigned int dsize : 24;
	/* 0x4 */ unsigned int daddr;
	/* 0x8 */ unsigned int fcode;
	/* 0xc */ unsigned int opt;
} sceSifCmdHdr;

typedef void (*sceSifCmdHandler)(/* parameters unknown */);

typedef struct { // 0xc
	/* 0x0 */ sceSifCmdHandler func;
	/* 0x4 */ void *data;
	/* 0x8 */ void *gp;
} sceSifCmdData;

typedef struct { // 0x14
	/* 0x00 */ sceSifCmdHdr chdr;
	/* 0x10 */ void *newaddr;
} sceSifCmdCSData;

typedef struct { // 0x18
	/* 0x00 */ sceSifCmdHdr chdr;
	/* 0x10 */ int rno;
	/* 0x14 */ unsigned int value;
} sceSifCmdSRData;

typedef struct { // 0x68
	/* 0x00 */ sceSifCmdHdr chdr;
	/* 0x10 */ int size;
	/* 0x14 */ int flag;
	/* 0x18 */ char arg[80];
} sceSifCmdResetData;

typedef struct { // 0x24
	/* 0x00 */ void *cpbuffer;
	/* 0x04 */ void *sbuffer;
	/* 0x08 */ void *obuffer;
	/* 0x0c */ sceSifCmdData *sysdata;
	/* 0x10 */ int ssize;
	/* 0x14 */ sceSifCmdData *userdata;
	/* 0x18 */ int usize;
	/* 0x1c */ unsigned int *sreg;
	/* 0x20 */ int evflag;
} sceSifCmdDataTable;

struct _sif_rpc_data { // 0x10
	/* 0x0 */ void *paddr;
	/* 0x4 */ unsigned int pid;
	/* 0x8 */ int tid;
	/* 0xc */ unsigned int mode;
};

typedef _sif_rpc_data sceSifRpcData;
typedef void (*sceSifEndFunc)(/* parameters unknown */);

struct _sif_client_data { // 0x28
	/* 0x00 */ _sif_rpc_data rpcd;
	/* 0x10 */ unsigned int command;
	/* 0x14 */ void *buff;
	/* 0x18 */ void *gp;
	/* 0x1c */ sceSifEndFunc func;
	/* 0x20 */ void *para;
	/* 0x24 */ _sif_serve_data *serve;
};

typedef _sif_client_data sceSifClientData;

struct _sif_receive_data { // 0x1c
	/* 0x00 */ _sif_rpc_data rpcd;
	/* 0x10 */ void *src;
	/* 0x14 */ void *dest;
	/* 0x18 */ int size;
};

typedef _sif_receive_data sceSifReceiveData;
typedef void* (*sceSifRpcFunc)(/* parameters unknown */);

struct _sif_serve_data { // 0x44
	/* 0x00 */ unsigned int command;
	/* 0x04 */ sceSifRpcFunc func;
	/* 0x08 */ void *buff;
	/* 0x0c */ int size;
	/* 0x10 */ sceSifRpcFunc cfunc;
	/* 0x14 */ void *cbuff;
	/* 0x18 */ int csize;
	/* 0x1c */ sceSifClientData *client;
	/* 0x20 */ void *paddr;
	/* 0x24 */ unsigned int fno;
	/* 0x28 */ void *receive;
	/* 0x2c */ int rsize;
	/* 0x30 */ int rmode;
	/* 0x34 */ unsigned int rid;
	/* 0x38 */ _sif_serve_data *link;
	/* 0x3c */ _sif_serve_data *next;
	/* 0x40 */ _sif_queue_data *base;
};

typedef _sif_serve_data sceSifServeData;

struct _sif_queue_data { // 0x18
	/* 0x00 */ int key;
	/* 0x04 */ int active;
	/* 0x08 */ _sif_serve_data *link;
	/* 0x0c */ _sif_serve_data *start;
	/* 0x10 */ _sif_serve_data *end;
	/* 0x14 */ _sif_queue_data *next;
};

typedef _sif_queue_data sceSifQueueData;

typedef struct { // 0x14
	/* 0x00 */ unsigned int request;
	/* 0x04 */ unsigned int opt0;
	/* 0x08 */ unsigned int opt1;
	/* 0x0c */ unsigned int opt2;
	/* 0x10 */ unsigned int opt3;
} _enddata;

typedef struct { // 0x4
	/* 0x0 */ unsigned int command;
} _binddata;

typedef struct { // 0xc
	/* 0x0 */ void *src;
	/* 0x4 */ void *dest;
	/* 0x8 */ int size;
} _rdata;

typedef struct { // 0x18
	/* 0x00 */ unsigned int fno;
	/* 0x04 */ int ssize;
	/* 0x08 */ void *receive;
	/* 0x0c */ int rsize;
	/* 0x10 */ int mode;
	/* 0x14 */ sceSifServeData *serve;
} _calldata;

typedef struct { // 0x8
	/* 0x0 */ unsigned int fno;
	/* 0x4 */ sceSifServeData *serve;
} _canceldata;

typedef struct { // 0x40
	/* 0x00 */ sceSifCmdHdr chdr;
	/* 0x10 */ unsigned int status;
	/* 0x14 */ void *paddr;
	/* 0x18 */ unsigned int pid;
	/* 0x1c */ sceSifRpcData *client;
	/* 0x20 */ union { // 0x20
		/* 0x20 */ unsigned int dummy[8];
		/* 0x20 */ _enddata end;
		/* 0x20 */ _binddata bind;
		/* 0x20 */ _calldata call;
		/* 0x20 */ _canceldata cancel;
		/* 0x20 */ _rdata rdata;
	} type;
} _sceRpcPacket;

typedef struct { // 0x2c
	/* 0x00 */ unsigned int idkey;
	/* 0x04 */ _sceRpcPacket *sys_buffer;
	/* 0x08 */ int ssize;
	/* 0x0c */ _sceRpcPacket *user_buffer;
	/* 0x10 */ int usize;
	/* 0x14 */ _sceRpcPacket *free_buffer;
	/* 0x18 */ int fsize;
	/* 0x1c */ _sceRpcPacket *free_buffer2;
	/* 0x20 */ int fsize2;
	/* 0x24 */ int findex;
	/* 0x28 */ sceSifQueueData *qdata;
} _sceRpcDataTable;

typedef struct { // 0x418
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ int flag;
	/* 0x010 */ unsigned int mode;
	/* 0x014 */ char name[1024];
	/* 0x414 */ int ee_fds;
} _sceFsOpenData;

typedef struct { // 0x14
	/* 0x00 */ int ee_semid;
	/* 0x04 */ unsigned int ee_retadr;
	/* 0x08 */ unsigned int ee_retsiz;
	/* 0x0c */ int fd;
	/* 0x10 */ int ee_fds;
} _sceFsCloseData;

typedef struct { // 0x1c
	/* 0x00 */ int ee_semid;
	/* 0x04 */ unsigned int ee_retadr;
	/* 0x08 */ unsigned int ee_retsiz;
	/* 0x0c */ int fd;
	/* 0x10 */ int offset;
	/* 0x14 */ int how;
	/* 0x18 */ int ee_fds;
} _sceFsLseekData;

typedef struct { // 0x20
	/* 0x00 */ int ee_semid;
	/* 0x04 */ unsigned int ee_retadr;
	/* 0x08 */ unsigned int ee_retsiz;
	/* 0x0c */ int fd;
	/* 0x10 */ unsigned int addr;
	/* 0x14 */ int size;
	/* 0x18 */ unsigned int intr_addr;
	/* 0x1c */ int ee_fds;
} _sceFsReadData;

typedef struct { // 0x30
	/* 0x00 */ int ee_semid;
	/* 0x04 */ unsigned int ee_retadr;
	/* 0x08 */ unsigned int ee_retsiz;
	/* 0x0c */ int fd;
	/* 0x10 */ unsigned int addr;
	/* 0x14 */ int size;
	/* 0x18 */ int psize;
	/* 0x1c */ unsigned char pdata[16];
	/* 0x2c */ int ee_fds;
} _sceFsWriteData;

typedef struct { // 0x420
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ int fd;
	/* 0x010 */ int cmd;
	/* 0x014 */ unsigned char arg[1024];
	/* 0x414 */ void *ret_argadr;
	/* 0x418 */ int ret_argsiz;
	/* 0x41c */ int arglen;
} _sceFsIoctlData;

typedef struct { // 0x40c
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ char name[1024];
} _sceFsNameData;

typedef struct { // 0x10
	/* 0x0 */ int ee_semid;
	/* 0x4 */ unsigned int ee_retadr;
	/* 0x8 */ unsigned int ee_retsiz;
	/* 0xc */ void *addr;
} _sceFsAddrData;

typedef struct { // 0x410
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ void *addr;
	/* 0x010 */ unsigned char name[1024];
} _sceFsGStatData;

typedef struct { // 0x450
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ unsigned int cbit;
	/* 0x010 */ sce_stat stat;
	/* 0x050 */ unsigned char name[1024];
} _sceFsCStatData;

typedef struct { // 0xc10
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ char path[1024];
	/* 0x40c */ char blkdevname[1024];
	/* 0x80c */ unsigned char arg[1024];
	/* 0xc0c */ int arglen;
} _sceFsFormatData;

typedef struct { // 0x80c
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ char oldpath[1024];
	/* 0x40c */ char newpath[1024];
} _sceFsRenameData;

typedef struct { // 0x414
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ int fd;
	/* 0x010 */ int flag;
	/* 0x014 */ char path[1024];
} _sceFsSyncData;

typedef struct { // 0x410
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ int flag;
	/* 0x010 */ char name[1024];
} _sceFsMkdirData;

typedef struct { // 0xc14
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ char fsdevname[1024];
	/* 0x40c */ char blkdevname[1024];
	/* 0x80c */ unsigned char arg[1024];
	/* 0xc0c */ int flag;
	/* 0xc10 */ int arglen;
} _sceFsMountData;

typedef struct { // 0x20
	/* 0x00 */ int ee_semid;
	/* 0x04 */ unsigned int ee_retadr;
	/* 0x08 */ unsigned int ee_retsiz;
	/* 0x0c */ int fd;
	/* 0x10 */ long int offset;
	/* 0x18 */ int how;
	/* 0x1c */ int ee_fds;
} _sceFsLseek64Data;

typedef struct { // 0x81c
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ char path[1024];
	/* 0x40c */ unsigned char arg_buf[1024];
	/* 0x80c */ int cmd;
	/* 0x810 */ int arglen;
	/* 0x814 */ void *ret_argadr;
	/* 0x818 */ int ret_arglen;
} _sceFsDevctlData;

typedef struct { // 0x80c
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ char existing[1024];
	/* 0x40c */ char new[1024];
} _sceFsSymlinkData;

typedef struct { // 0x414
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retadr;
	/* 0x008 */ unsigned int ee_retsiz;
	/* 0x00c */ u_int bufsize;
	/* 0x010 */ u_int bufaddr;
	/* 0x014 */ char path[1024];
} _sceFsReadlinkData;

typedef struct { // 0x90
	/* 0x00 */ int psize;
	/* 0x04 */ int ssize;
	/* 0x08 */ unsigned int paddr;
	/* 0x0c */ unsigned int saddr;
	/* 0x10 */ unsigned char pdata[64];
	/* 0x50 */ unsigned char sdata[64];
} _sceFsReadIntrData;

typedef struct { // 0x20
	/* 0x00 */ int ee_semid;
	/* 0x04 */ unsigned int ee_retmod;
	/* 0x08 */ unsigned int ee_retadr;
	/* 0x0c */ unsigned int ee_retsiz;
	/* 0x10 */ int data_top[4];
} _sceFsIntrRcvData;

typedef struct { // 0x15c
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retmod;
	/* 0x008 */ unsigned int ee_retadr;
	/* 0x00c */ unsigned int ee_retsiz;
	/* 0x010 */ int ee_ret;
	/* 0x014 */ unsigned int ee_dentadr;
	/* 0x018 */ sce_dirent dent;
} _sceFsIntrRcvDirData;

typedef struct { // 0x41c
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retmod;
	/* 0x008 */ unsigned int ee_retadr;
	/* 0x00c */ unsigned int ee_retsiz;
	/* 0x010 */ int ee_ret;
	/* 0x014 */ unsigned int ee_readladr;
	/* 0x018 */ unsigned int ee_readlsiz;
	/* 0x01c */ char linkname[1024];
} _sceFsIntrRcvReadLData;

typedef struct { // 0x41c
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retmod;
	/* 0x008 */ unsigned int ee_retadr;
	/* 0x00c */ unsigned int ee_retsiz;
	/* 0x010 */ int ee_ret;
	/* 0x014 */ unsigned int ret_argadr;
	/* 0x018 */ unsigned int ret_argsiz;
	/* 0x01c */ unsigned char arg[1024];
} _sceFsIntrRcvIoctlData;

typedef struct { // 0x41c
	/* 0x000 */ int ee_semid;
	/* 0x004 */ unsigned int ee_retmod;
	/* 0x008 */ unsigned int ee_retadr;
	/* 0x00c */ unsigned int ee_retsiz;
	/* 0x010 */ int ee_ret;
	/* 0x014 */ unsigned int ret_argadr;
	/* 0x018 */ unsigned int ret_argsiz;
	/* 0x01c */ unsigned char arg[1024];
} _sceFsIntrRcvDevctlData;

typedef struct { // 0x10
	/* 0x0 */ int i_fd;
	/* 0x4 */ unsigned int i_flag;
	/* 0x8 */ int i_errno;
	/* 0xc */ void *i_private;
} _sceFsIob;

typedef struct { // 0x8
	/* 0x0 */ void (*sceFsPoffCbfunc)(/* parameters unknown */);
	/* 0x4 */ void *sceFsPoffCbdata;
} _sceFsPoffData;

typedef struct { // 0x100
	/* 0x00 */ void *load_addr;
	/* 0x04 */ char name[252];
} _sceLoadIHData;

typedef struct { // 0x4
	/* 0x0 */ unsigned int heap_size;
} _sceAllocIHData;

typedef struct { // 0x4
	/* 0x0 */ void *heap_addr;
} _sceFreeIHData;

typedef struct { // 0xc
	/* 0x0 */ unsigned int heap_size;
	/* 0x4 */ int alloc_type;
	/* 0x8 */ void *addr;
} _sceAllocSMData;

typedef struct { // 0x200
	/* 0x000 */ unsigned int code0;
	/* 0x004 */ unsigned int code1;
	/* 0x008 */ char data0[252];
	/* 0x104 */ char data1[252];
} _sceLoadFileData;

typedef struct { // 0x8
	/* 0x0 */ int num;
	/* 0x4 */ void *func;
} SysEntry;

typedef struct { // 0x4
	/* 0x0:0 */ u_int Spdif : 1;
	/* 0x0:1 */ u_int Aspct : 2;
	/* 0x0:3 */ u_int Video : 1;
	/* 0x0:4 */ u_int oldLang : 1;
	/* 0x0:5 */ u_int Compval : 8;
	/* 0x1:5 */ u_int pad0 : 3;
	/* 0x2:0 */ u_int Language : 5;
	/* 0x2:5 */ int TimeZone : 11;
} OsdConfigParam;

struct TLBEntry { // 0x10
	/* 0x0 */ u_int pageMask;
	/* 0x4 */ u_int entryHi;
	/* 0x8 */ u_int entryLo0;
	/* 0xc */ u_int entryLo1;
};

struct TLB { // 0x1c
	/* 0x00 */ int numKernel;
	/* 0x04 */ int numFix;
	/* 0x08 */ int numUnfix;
	/* 0x0c */ int wired;
	/* 0x10 */ TLBEntry *kernelTLB;
	/* 0x14 */ TLBEntry *fixTLB;
	/* 0x18 */ TLBEntry *unfixTLB;
};

typedef struct { // 0x40
	/* 0x00 */ COUNTER *pNext;
	/* 0x04 */ COUNTER *pPrev;
	/* 0x08 */ u_int uiKey;
	/* 0x0c */ u_int uiMode;
	/* 0x10 */ u_long ulBaseTime;
	/* 0x18 */ u_long ulBaseCount;
	/* 0x20 */ u_long ulSchedule;
	/* 0x28 */ u_long (*cbHandler)(/* parameters unknown */);
	/* 0x2c */ void *gp_value;
	/* 0x30 */ void *arg;
	/* 0x34 */ u_int pad[3];
} COUNTER;

typedef struct { // 0x10
	/* 0x0 */ ALARM *pNext;
	/* 0x4 */ int counterid;
	/* 0x8 */ u_long (*cbHandler)(/* parameters unknown */);
	/* 0xc */ void *arg;
} ALARM;

typedef struct { // 0x4
	/* 0x0 */ u_char type;
	/* 0x1 */ u_char code;
	/* 0x2 */ u_short unused;
} sceDcmpHdr;

struct rtx_def;

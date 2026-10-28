//
// Information extracted with resym v0.4.0
//
// PDB file: default.pdb
// Image architecture: X86
//


struct _GUID;
struct _D3DLOCKED_RECT;
struct _D3DVERTEXATTRIBUTEFORMAT;
struct _D3DMATRIX;
struct _D3DCOPYRECTROPSTATE;
struct _D3DTILE;
struct _D3DSTREAM_INPUT;
struct _D3DVIEWPORT8;
struct _D3DSWAPDATA;
struct HWND__;
struct tagPOINT;
struct _D3DGAMMARAMP;
struct _D3DVBLANKDATA;
struct _D3DRASTER_STATUS;
struct _D3DVECTOR4;
struct _D3DMATERIAL8;
struct _D3DRECT;
struct _D3DADAPTER_IDENTIFIER8;
struct tagRECT;
struct _D3DPixelShaderDef;
struct D3DBaseTexture;
struct D3DIndexBuffer;
struct D3DSurface;
struct D3DVolumeTexture;
struct D3DVertexBuffer;
struct D3DTexture;
struct D3DFixup;
class HashString;
class Allocator;
struct OL_SObjectFileDesc;
struct TGD_TerrainInfo;
struct TGD_FoggingData;
struct BL_SBatchListElement;
struct _D3DLIGHT8;
struct _D3DSURFACE_DESC;
struct _D3DPRESENT_PARAMETERS_;
struct TBasicActorS;
struct _D3DCOPYRECTSTATE;
struct TObjectLightEnvS;
struct XG_SVertexBuffer;
struct _D3DTRIPATCH_INFO;
struct SD_SShaderObjectData_Grass_StaticRenderData;
struct SD_SContainerData_OBFData_RenderBuffer;
struct TObject3DS;
struct TX_STextureGroupCLUT;
struct XG_SIndexBuffer;
struct TGD_MaterialInfo;
struct _D3DDISPLAYMODE;
struct OL_SElement;
struct _D3DCAPS8;
struct SD_SShaderObject_Static;
struct TX_STextureGroupCLUTExtra;
struct TGD_PolygonReference;
struct _D3DDEVICE_CREATION_PARAMETERS;
struct D3DPalette;
struct D3DCubeTexture;
struct TGD_ObjectInstanceInfo;
struct TExplodingFaceChunkS;
struct SD_SShaderObjectInstanceInterface_Static;
struct _D3DFIELD_STATUS;
struct TX_STextureGroup;
struct TX_STextureGroupTextureExtra;
struct _D3DRECTPATCH_INFO;
struct SD_SContainer;
struct SD_SContainerInstanceInterface;
struct OL_SObjectHeader;
struct D3DDevice;
class ShaderTextureIterator;
struct TGD_Cell;
struct TX_STextureGroupTexture;
struct TBBoxListS;
struct OL_SObject;
struct TGD_SkyData;
struct TGD_SunData;
struct TGD_LightingData;
struct Direct3D;
struct SD_SCommonShaderObjectData_StaticData;
struct D3DPushBuffer;
struct InstanceShader_RuntimePrimitiveGroup;
struct TexIteratorItem;
struct InstanceShader_RuntimePrimitive;
struct OL_SElementHeader;

enum Lie_t : int {
  LIE_TEE_e = 0x0000,
  LIE_FAIRWAY_e = 0x0001,
  LIE_FAIRWAY_TIGHT_e = 0x0002,
  LIE_ROUGH_HIGH_e = 0x0003,
  LIE_ROUGH_e = 0x0004,
  LIE_THICK_ROUGH_e = 0x0005,
  LIE_SAND_HIGH_e = 0x0006,
  LIE_SAND_MEDIUM_e = 0x0007,
  LIE_SAND_DEEP_e = 0x0008,
  LIE_GREEN_e = 0x0009,
  LIE_FRINGE_e = 0x000a,
  LIE_CARTPATH_e = 0x000b,
  LIE_INCUP_e = 0x000c,
  LIE_WATER_e = 0x000d,
  LIE_ICE_e = 0x000e,
  LIE_SNOW_e = 0x000f,
  LIE_OUT_OF_BOUNDS_e = 0x0010,
  LIE_MISC_e = 0x0011,
  LIE_MAX_e = 0x0012,
};

enum physicsBallState_t : int {
  PHYSICS_BALLSTATE_BallDead_e = 0x0000,
  PHYSICS_BALLSTATE_BallWaiting_e = 0x0001,
  PHYSICS_BALLSTATE_BallFlying_e = 0x0002,
  PHYSICS_BALLSTATE_BallRollingAndSlipping_e = 0x0003,
  PHYSICS_BALLSTATE_BallPureRolling_e = 0x0004,
  PHYSICS_BALLSTATE_BallOutOfBounds_e = 0x0005,
  PHYSICS_BALLSTATE_BallScripted_e = 0x0006,
  PHYSICS_BALLSTATE_MAX_e = 0x0007,
};

enum LieAngle_t : int {
  LIE_ANGLE_FLAT = 0x0000,
  LIE_ANGLE_LEFT_RIGHT_e = 0x0001,
  LIE_ANGLE_RIGHT_LEFT_e = 0x0002,
  LIE_ANGLE_MAX_e = 0x0003,
};

struct PhysicsBall_t { /* Size=0xcc */
  /* 0x0000 */ public: float location[4];
  /* 0x0010 */ public: float lastLocation[4];
  /* 0x0020 */ public: float linearVelocity[4];
  /* 0x0030 */ public: float angularVelocity[4];
  /* 0x0040 */ public: float initialShotPosition[4];
  /* 0x0050 */ public: float speed;
  /* 0x0054 */ public: float altitude;
  /* 0x0058 */ public: float terrainHeight;
  /* 0x005c */ public: float sideSpinOverride;
  /* 0x0060 */ public: float forwardSpinOverride;
  /* 0x0064 */ public: float closestToCupThisShot;
  /* 0x0068 */ public: physicsBallState_t state;
  /* 0x006c */ public: Lie_t initialLie;
  /* 0x0070 */ public: Lie_t lie;
  /* 0x0074 */ public: LieAngle_t lieAngle;
  /* 0x0078 */ public: float lieModifier;
  /* 0x007c */ public: float lieReadOffset;
  /* 0x0080 */ public: int surfaceID;
  /* 0x0084 */ public: int initialSurfaceID;
  /* 0x0088 */ public: TGD_TerrainInfo* pTerrainData;
  /* 0x008c */ public: int collideCount;
  /* 0x0090 */ public: int solidCollideCount;
  /* 0x0094 */ public: TGD_MaterialInfo* pLastCollisionSurface;
  /* 0x0098 */ public: TGD_ObjectInstanceInfo* pLastCollisionObject;
  /* 0x009c */ public: TBasicActorS* pLastCollisionActor;
  /* 0x00a0 */ public: int playerID;
  /* 0x00a4 */ public: float firstSandPosition[4];
  /* 0x00b4 */ public: float firstSandVMag;
  /* 0x00b8 */ public: float lastDistFromInitShotPos;
  /* 0x00bc */ public: float timeSinceLastCheck;
  /* 0x00c0 */ public: int ballRandomBounceDelayCounter;
  /* 0x00c4 */ public: unsigned char obBounceCount;
  /* 0x00c8 */ private: unsigned int flags;

  public: bool TestFlag(unsigned int) const;
  public: void SetFlag(unsigned int);
  public: void ClearFlag(unsigned int);
  public: void SetFlagState(unsigned int, bool);
  public: void ClearAllFlags();
};

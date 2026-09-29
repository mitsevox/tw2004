// camera.h (our name): the views on screen and their cameras: the view structs, camera shots and
// sequences, a view's camera script, the camera tuning values, the golf cameras' (GoGolfCam.c)
// shared state, and the view and camera functions.

#ifndef CAMERA_H
#define CAMERA_H

#include "engine.h"

// A render camera's lens (our name; unsorted/cull.h called it CameraSub): what Camera_GetLens returns,
// the pointer at the render camera's +0x10. Only the fields read so far; its size is unknown.
typedef struct CamLens {
    s32  nType;                 // 0x00  0: a perspective camera, else flat (LLObj_Gc.c)
    f32  m4[4][4];              // 0x04  camera to world space (shadow.c fn_800B3484 hands it on as a
                                //       matrix): m4[2] is the view direction (goballfx.c fn_80093A50
                                //       takes its angle to a light), m4[3] the camera's position
                                //       (GameMode8 measures the ball's distance to it); the green
                                //       zoom-to-aim camera copies m4[0] to View.vSide
    f32  m44[4][4];             // 0x44  world to camera space (hlaudemitter.c Aud_EmiSet3DPos moves a
                                //       sound's position with it)
    f32  m84[2][4];             // 0x84  [1] the scale that
                                //       Camera_SetCameraPositionAndTargetWithOffsetAndScale puts
                                //       on the world around a point, [0] its inverse;
                                //       ViewController.c CA_vSetDefaultScalingVectors sets all
                                //       to 1.0
    f32  fFov;                  // 0xA4  the field of view (GoGolfCam.c sets DEG(60.0f) or DEG(30.0f))
    f32  fA8;                   // 0xA8  the near clip distance (RC_vUpdateRenderCtxScreenMatricesAndInfo
                                //       builds the projection from it); CA_vInitCamera starts it at 0.1
    f32  fAC;                   // 0xAC  the far clip distance; CA_vInitCamera starts it at 4096
                                //       (GoTerrain.c fn_800354B4 sets it)
    f32  fB0;                   // 0xB0  Camera_GetLensFovScale; the zoom-to-aim camera divides its
                                //       distance by it
    f32  fFlatWidth;            // 0xB4  } a flat camera's view width and height in world units:
    f32  fFlatHeight;           // 0xB8  } RC_vUpdateRenderCtxScreenMatricesAndInfo builds its
                                //       projection with Mtx_OrthoScale from them, LLObj_Gc.c
                                //       culls against them; 20 x 20 from CA_vInitCamera
                                //       (fn_80076948 sets them; GoGrass.c's top-down grass camera
                                //       sets 20 x 20 too)
} CamLens;

f32 CA_fGetCameraFieldOfView(CamLens* pLens);        // GoRenderCtx_Gc.c: the lens's field of view

// A camera shot (0xC0 bytes; TW07's CameraScript_t): a named script position the camera script
// moves to. The shots of a sequence are chained through p40.
typedef struct CamShot {
    char szName[0x20];          // 0x00
    f32  v20[4];                // 0x20  a position ([1]: the elevator camera adds the course's height)
    f32  v30[4];                // 0x30  a second position (the super zoom's target, the zoom-to-aim
                                //       camera's start)
    struct CamShot* p40;       // 0x40
    struct CamShot* p44;        // 0x44  in View.shot19C: the shot camera 13 goes back to
    f32  f48;                   // 0x48  how long the shot lasts
    f32  f4C;                   // 0x4C
    union {
        struct {
            u32 u50;            // 0x50  on the CrAP screen: bit n for CrAPGolfer.nGolferId = n up to 32
            u32 u54;            // 0x54  ... bit n - 32 above that (DynamicCam_MatchPlayer)
        } bits;
        f32 aArea[4];           // 0x50  a static camera (GoStaticCam.c): it is picked while the
                                //       golfer or the ball is inside x aArea[0]..aArea[2],
                                //       z aArea[1]..aArea[3] (StaticCam_CheckHotZone)
    } u;
    f32  fDist;                 // 0x60  how far along the way from nFromPoint to nToPoint the
                                //       camera sits (a share of the way for some tracking modes)
    f32  fSideOffset;           // 0x64  and how far to the side
    f32  fMinHeight;            // 0x68  } the camera's height range over the ground (DynamicCam;
    f32  fMaxHeight;            // 0x6C  } loading keeps fMaxHeight at least fMinHeight)
    f32  fLookSideOffset;       // 0x70  } where it looks: moved this far to the side and up
    f32  fLookUpOffset;         // 0x74  } (CameraScript_OffsetLookVector)
    f32  fFovStart;             // 0x78  } the field of view at the start and the end of the shot
    f32  fFovEnd;               // 0x7C  } (over f48; in degrees in the files)
    f32  f80;                   // 0x80
    f32  f84;                   // 0x84
    f32  fDepthOfField;         // 0x88  CamScript_RunScript hands it (plus
                                //       GameEffects_DepthOfFieldChange of it) to fn_800457B8
    f32  f8C;                   // 0x8C  } CamScript_RunScript passes both to fn_80038054 (slow motion) when
    f32  f90;                   // 0x90  } either is above 0
    f32  fShakeAmount;          // 0x94  } the shoulder shake (CameraScript_CalculateShoulderShake):
    f32  fShakeSpeed;           // 0x98  } how far and how fast; none when fShakeAmount is 0
    f32  fRoll;                 // 0x9C  the camera's roll (the script's fRoll)
    s32  nA0;                   // 0xA0  CameraScript_InterpToNewScript: 0 GameEffects_SetHalfTime on, 2
                                //       GameEffects_SetDoubleTime on (else both off); 3 calls
                                //       GolfCamera_SetCameraMatrixMode(1)
    s32  nA4;                   // 0xA4
    u8   bA8;                   // 0xA8
    u8   bA9;                   // 0xA9  another shot's p40 leads here (DynamicCam_ParseCameraViewsFE)
    u8   bShowGolfer;           // 0xAA  the golfer is drawn in this shot
    u8   nBlendKind;            // 0xAB  how the script moves into it (CamScript.nBlendKind: 0 a plain
                                //       blend, 1 fixed target, 13 share of the flight, 15 share
                                //       to the pin, ...; CamScript_RunScript)
    u8   nLookAtKind;           // 0xAC  what it looks at (CamScript_GetLookAtPoint: 0 the ball,
                                //       23 ahead of the ball's flight, ...)
    u8   nStateType;            // 0xAD  the script state it serves (DynamicCam_MatchScriptStateType)
    s8   nAE;                   // 0xAE  a static camera's record byte 0x1C (StaticCam_ParseStaticCameraActor)
    u8   nFromPoint;            // 0xAF  } the two points the camera's place is measured between
    u8   nToPoint;              // 0xB0  } (DynamicCam_GetLocation kinds; 16: the ball)
    u8   nTrackMode;            // 0xB1  how the camera follows them (DynamicCam_ProcessScript)
    u8   nHeightRef;            // 0xB2  what fMinHeight / fMaxHeight are measured from
    u8   unkB3[0xC0 - 0xB3];
} CamShot;
LAYOUT_ASSERT(CamShot, 0xC0);

// ---- the course's static and fly-by cameras (GoStaticCam.c) ----------------------------------
// A course's 'Cact' objects of type 201 are static cameras, those of type 200 fly-by cameras
// (UKernel.c's Kernel_DownloadActors hands them over); each becomes a CamShot. The fly-by cameras are
// chained into up to 10 paths, and a 'CAMC' stream object brings a spline table per path.
// The type names below are ours, from the shots' names ("Static Cam: %d", "FlyBy Cam: %d").

// A type 201 object's data (UStreamObject.pData; the same 'Cact' data as DynObjDef, 8 bytes in).
typedef struct StaticCamDef {
    u8   unk0[0x10];
    f32  aPos[3];               // 0x10  -> CamShot.v20
    s32  n1C;                   // 0x1C  -> CamShot.nAE
    s32  n20;                   // 0x20  -> CamShot.nLookAtKind (5: skipped when StaticCam_ChooseScript
                                //       is asked to)
    s32  nKinds;                // 0x24  -> CamShot.nA4: the shot kinds it serves, a bit each
    f32  fFov;                  // 0x28  in degrees -> CamShot.fFovStart
    f32  f2C;                   // 0x2C  in degrees -> CamShot.fFovEnd
    f32  f30;                   // 0x30  -> CamShot.f48
    f32  f34;                   // 0x34  -> CamShot.f4C
    f32  aArea[4];              // 0x38  -> CamShot.u.aArea
    f32  aAngle[3];             // 0x48  in degrees: which way it looks (-> CamShot.v30, turned into
                                //       the point it looks at)
    f32  f54;                   // 0x54  -> CamShot.fDepthOfField
    s32  n58;                   // 0x58  -> CamShot.nA0
} StaticCamDef;

// A type 200 object's data.
typedef struct FlyByCamDef {
    u8   unk0[0x10];
    f32  aPos[3];               // 0x10  -> CamShot.v20
    f32  fFov;                  // 0x1C  in degrees -> CamShot.fFovStart
    f32  f20;                   // 0x20  -> CamShot.f48
    s8   nId;                   // 0x24  its number on the path
    s8   nNext;                 // 0x25  the next camera's nId (-99: the path ends here)
    u8   unk26[2];
    f32  aLook[3];              // 0x28  in degrees -> CamShot.v30
    f32  f34;                   // 0x34  0..1 (clamped on load) -> CamShot.f4C
    s8   nPath;                 // 0x38  its path (0..9) -> CamShot.nA4
} FlyByCamDef;

// One key of a fly-by path's spline (0x24 bytes; a 'CAMC' object holds them byte-swapped, 0x22
// bytes each).
typedef struct FlyByKey {
    f32  af[8];                 // 0x00
    u8   b20;                   // 0x20
    u8   b21;                   // 0x21
    u8   unk22[2];
} FlyByKey;

// A fly-by path's spline (0x2E0 bytes), from the 'CAMC' object.
typedef struct FlyByPath {
    u32  u0;                    // 0x00
    u32  uPath;                 // 0x04  the path it belongs to (CamShot.nA4; StaticCam_GetFlybyTimeCurve)
    f32  fLength;               // 0x08
    u32  nKeys;                 // 0x0C
    FlyByKey aKeys[20];         // 0x10
} FlyByPath;
LAYOUT_ASSERT(FlyByPath, 0x2E0);

#define NUM_STATIC_CAMS 10
#define NUM_FLYBY_CAMS  30
#define NUM_FLYBY_PATHS 10

// GoStaticCam.c's state (0x1E68 bytes, allocated by StaticCam_Init).
typedef struct StaticCams {
    CamShot aStatic[NUM_STATIC_CAMS];      // 0x0000  "Static Cam: n"
    CamShot aFlyBy[NUM_FLYBY_CAMS];        // 0x0780  "FlyBy Cam: n"
    s32  nFlyBy;                // 0x1E00  how many of aFlyBy are loaded
    s32  nStatic;               // 0x1E04  how many of aStatic are loaded
    u8   bLinked;               // 0x1E08  the fly-by paths are chained (StaticCam_GetFlyByCam)
    f32  afPathLength[NUM_FLYBY_PATHS];    // 0x1E0C  each path's length, its shots' f4C added up
    CamShot* apPath[NUM_FLYBY_PATHS];      // 0x1E34  each path's first shot
    u32  u1E5C;                 // 0x1E5C  } the 'CAMC' object's header
    u32  nPaths;                // 0x1E60  } the number of splines in pPaths
    FlyByPath* pPaths;          // 0x1E64
} StaticCams;
LAYOUT_ASSERT(StaticCams, 0x1E68);

extern StaticCams* gpStaticCams;

// A camera sequence (DynamicCam's, 0x50 bytes): the shots a camera plan steps through, and the
// conditions it is picked on. Its shot choices (CamChoice) are in dyncam.h.
typedef struct CamSequence {
    char szName[0x20];          // 0x00  "DEF..." for a default sequence (DynamicCam_IsDefualtSeq)
    struct CamSequence* p20;   // 0x20  the sequence that follows (an index in the file)
    f32  fMinDist;              // 0x24  } the distance to the pin it is picked for
    f32  fMaxDist;              // 0x28  } (DynamicCam_MatchDistToPin)
    f32  fMinPinHeight;         // 0x2C  } the pin's height over the ball it is picked for
    f32  fMaxPinHeight;         // 0x30  } (DynamicCam_MatchHeightDiff)
    f32  f34;                   // 0x34  its weight when several fit (DynamicCam_ChooseSequence)
    f32  f38;                   // 0x38  its length
    s32  nChoices;              // 0x3C  how many shot choices pChoices holds
    u32  uCourses;              // 0x40  one bit per course it is used on
    u8   nStateType;            // 0x44  the script state it serves (DynamicCam_MatchStateType)
    u8   b45;                   // 0x45  the clubs it is for (DynamicCam_MatchSeqClub)
    u8   nShotType;             // 0x46  the shot types it is for (DynamicCam_MatchSeqShot); 6: the
                                //       ball-flight camera keeps one for shot kind 5
    u8   nPlayerType;           // 0x47  0 humans, 1 CPU players, 2 not in a replay, 3 in a replay, 4 in a
                                //       replay or a CPU player (DynamicCam_MatchPlayerType); the swing camera
                                //       starts its shot with blend 5, time 0 when it is nonzero
    u8   nModeType;             // 0x48  0 single-view play outside modes 9 and 11 and
                                //       GM_Currently_SkillZoneMode; 1 split screen or modes 9 and
                                //       11; 2 GM_Currently_SkillZoneMode (DynamicCam_ModeType)
    u8   uStartLies;            // 0x49  } camera lies, a bit each (DynamicCam_MatchLies): the
    u8   uEndLies;              // 0x4A  } ball's lie (the tee: 1), and the surface under it
    s8   n4B;                   // 0x4B  one bit per value of GM_GetCurrentHolePar
    struct CamChoice* pChoices; // 0x4C  its shot choices (dyncam.h)
} CamSequence;
LAYOUT_ASSERT(CamSequence, 0x50);

// A view's camera script (0x118 bytes at View + 0x84; TW07's CameraScriptSettings_t): the shot the
// camera plays, the one after it, and the move it makes between them. The camera script functions
// (gocamscripts.c) and DynamicCam_GetLocation take it as pScript; every caller passes
// &pView->script.
typedef struct CamScript {
    f32  vCamPos[4];            // 0x00  the current shot's camera position (camera 4 keeps the
                                //       ball here)
    f32  vNextCamPos[4];        // 0x10  the next shot's, blended to (camera 4 keeps the pin here)
    f32  aLookAt[8];            // 0x20  where the camera looks: [0..3] for the current shot,
                                //       [4..7] for the next (CamScript_GetLookAtPoint)
    f32  vFadeColor[4];         // 0x40  the screen fade's colour, its alpha [3] the most it reaches
    f32  v50[4];                // 0x50  the ball-flight camera: where the shot should land (the aim, at
                                //       the club's full distance)
    f32  v60[4];                // 0x60  CamScript_RunScript: how far the camera moved this frame (0 when
                                //       CameraScript_SnapToScript holds)
    f32  v70[4];                // 0x70  (0, 0, 0, 1) when the view is set up (CameraController_InitOneCamera)
    f32  fCamTime;              // 0x80  time on this camera
    f32  f84;                   // 0x84  fCamTime before this frame's step (CamScript_RunScript)
    f32  f88;                   // 0x88  a second clock, stepped with fCamTime
    f32  f8C;                   // 0x8C  how long the next shot lasts (its f48;
                                //       CameraController_StartScriptOfKind)
    f32  fFadeTime;             // 0x90  the fade's time so far (stepped by the frame time)
    f32  fFadeLength;           // 0x94  and its length
    f32  f98;                   // 0x98
    f32  f9C;                   // 0x9C  CamScript_GetLookAtPoint hands it to
                                //       CameraScript_CalculateShoulderShake (f98 at its end)
    f32  fA0;                   // 0xA0
    f32  fA4;                   // 0xA4
    f32  fRoll;                 // 0xA8  the camera's roll: the current shot's fRoll, blended
                                //       (CamScript_RunScript)
    CamShot* pShot;             // 0xAC  the current shot
    CamShot* pNextShot;         // 0xB0  the next one (the current shot's p40;
                                //       CameraController_StartScriptOfKind)
    CamShot* pB4;               // 0xB4  where SwitchCrAPCamera records the current camera
    CamShot* pB8;               // 0xB8  the shot before (GolfCamera_CutToGolferDoneAnimatingCam)
    s32  nBlendKind;            // 0xBC  how the script blends into the next shot (its
                                //       nBlendKind; CameraController_StartScriptOfKind)
    s32  nFade;                 // 0xC0  the screen fade (CamScript_Fade): 0 none, 1 fading up to vFadeColor
                                //       (CameraController_FadeOut), 2 fading away (FadeIn), 3 held
                                //       (CameraController_HoldFadeColor), 4 kept after 1 ends, 5 after 2 ends
                                //       (then 0)
    s32  nRequestedEvent;       // 0xC4  the camera event asked for (CameraController_StartScriptOfKind)
    s32  nC8;                   // 0xC8  the shot kind last started
    u8   bCC;                   // 0xCC
    u8   bCD;                   // 0xCD
    u8   bCE;                   // 0xCE
    u8   bFairwayFix;           // 0xCF  the camera was put back on the fairway (a point of kind
                                //       16, DynamicCam_GetLocation)
    s32  nArcDir;               // 0xD0  the way round an arced blend swings (CamUtils_vCalcArcPosition)
    f32  fD4;                   // 0xD4  the camera's speed when the script moves to the next shot
                                //       (CamScript_RunScript)
    f32  fD8;                   // 0xD8  camera 8: the ground height it follows
                                //       (DynamicCam_TrackBallVelocityTight measures the ball's height over
                                //       it)
    f32  fLagAngle;             // 0xDC  how far the look direction may trail its target
                                //       (CameraScript_KeepPointInView)
    s32  nTriggerKind;          // 0xE0  } a shot kind started once f98 passes fTriggerTime
    f32  fTriggerTime;          // 0xE4  } (DynamicCam_ChooseScriptInSequence; 25 = none)
    u8   bNoGround;             // 0xE8  no ground was found under the camera
                                //       (CamScript_SmoothTerrainHeight)
    u8   unkE9[0xEC - 0xE9];
    f32  fBallUpdates;          // 0xEC  the ball updates per frame, eased towards
                                //       GameEffects_BallUpdatesThisFrame by CamTuning.f1A8
    f32  fShakeTime;            // 0xF0  } the camera shake: time left and how far
    f32  fShakeAmount;          // 0xF4  } (CameraController_SetShakeAmount)
    f32  fBlendShare;           // 0xF8  the blend's share so far (never goes back; 1 ends blend
                                //       kinds 13 and 15)
    u8   unkFC[0x100 - 0xFC];
    f32  f100;                  // 0x100
    f32  f104;                  // 0x104
    f32  f108;                  // 0x108
    f32  f10C;                  // 0x10C
    s32  n110;                  // 0x110
    s32  n114;                  // 0x114
} CamScript;
LAYOUT_ASSERT(CamScript, 0x118);    // CameraScript_WillGolferBeOccludedInThisView copies 0x118 bytes

// A view's camera controller (CameraController_SetCameraMode, EA's name in TW06 and TW07): the
// camera mode, its shots and script. It sits at +4 in a ViewController; only the fields read so far.
typedef struct View {
    f32      v0[4];             // 0x000  what CameraController_GetCameraOrigin returns: the
                                //        camera's position (inferred)
    f32      v10[4];            // 0x010  what CameraController_GetCameraLookPoint returns: where it
                                //        looks (the pin, for camera 5)
    f32      vSide[4];          // 0x020  the camera's side vector
                                //        (CameraController_ComputeCurrentSideVector)
    f32      v30[4];            // 0x030
    f32      v40[4];            // 0x040
    f32      f50;               // 0x050
    f32      f54;               // 0x054
    f32      f58;               // 0x058
    f32      f5C;               // 0x05C
    f32      f60;               // 0x060  camera 4: the camera's height over the ball once it is in place
    f32      f64;               // 0x064
    f32      fFade;             // 0x068  how far the view has faded (the ball-in-flight state waits for 0.5)
    u8       bFade;             // 0x06C  the fade is on
    u8       unk6D[3];
    s32      nCurCamera;        // 0x070
    CamSequence* p74;           // 0x074  the camera sequence the shots are picked from
    CamSequence* p78;           // 0x078  the sequence before the post-shot cameras (p74 saved)
    CamSequence* p7C;           // 0x07C  the swing camera's sequence, kept for the replay
    CamShot* p80;               // 0x080
    CamScript script;           // 0x084  the camera script the camera functions drive
    CamShot  shot19C;           // 0x19C  a shot built by hand (the knee, steep-slope and elevator cameras)
    s32      nSavedCamera;      // 0x25C
    s32      nSpecialSwingType; // 0x260  the special swing camera picked for this shot (0 none;
                                //        GolfCamera_ChooseSpecialSwing, the game modes)
    s32      n264;              // 0x264  which of the shots 0x1D..0x21
                                //        GolfCamera_vSwitchToNextAlternateSwingCamera tries next
    u8       bSkipFancyPreshotCams; // 0x268  (GolfCamera_SetSkipFancyPreshotCams)
    u8       bShowPostShotAnims;    // 0x269  the golfer's post-shot animations are shown
    u8       bShowRemoveBall;       // 0x26A  the post-shot ball removal is shown
    u8       unk26B;
} View;
LAYOUT_ASSERT(View, 0x26C);

// One of the four views on screen (gViewControllers, 0x288 bytes each; Player.nView[], TW06's
// viewControllerID, index them): the render camera, the view's camera controller, the player it
// follows.
typedef struct ViewController {
    void*    pCamera;           // 0x000  the render camera (ViewController_GetRenderContext)
    View     view;              // 0x004  (ViewController_GetCameraControl)
    s32      nPlayer;           // 0x270  set by ViewController_SetActivePlayerNumber
    u8       bActive;           // 0x274  the view is in use (TW07's ViewController_IsActive): set by
                                //        ViewController_Init, cleared by ViewController_Delete,
                                //        either by ViewController_TurnOnViewController
    u8       bFlagOut;          // 0x275  the flagstick is out (set while every player on this view
                                //        is on the green, the fringe or holed): no pin collisions
    u8       unk276[2];
    f32      f278;              // 0x278  } the render camera's screen rectangle, saved by
    f32      f27C;              // 0x27C  } ViewController_SaveViewportRect in reverse order (f284
    f32      f280;              // 0x280  } is its first value) and put back by
    f32      f284;              // 0x284  } ViewController_RestoreViewportRect
} ViewController;
LAYOUT_ASSERT(ViewController, 0x288);

// The camera tuning values (GoGolfCam.c), allocated and set by GoCamTuningVars.c's CameraTuning_Init.
typedef struct CamTuning {
    f32  f0;                    // 0x000  the zoom-to-aim camera's base speed
    f32  f4;                    // 0x004  the zoom-to-aim camera's distance back from the target
    f32  f8;                    // 0x008  the zoom-to-aim camera slows down over this last distance
    f32  fC;                    // 0x00C  ... closer in when the ball is within this of it plus f4
    f32  f10;                   // 0x010  ... as a share of the ball's distance
    f32  f14;                   // 0x014  the zoom-to-aim camera's height over View.script.fD8
    f32  f18;                   // 0x018  the zoom-to-aim camera's creep and aim-marker lag
    f32  f1C;                   // 0x01C  both zoom-to-aim cameras' slow motion at full speed
    f32  f20;                   // 0x020  the zoom-to-aim camera's slow motion before it sets off
    f32  f24;                   // 0x024  both: the camera has arrived within this of its goal
    f32  f28;                   // 0x028  the zoom-to-aim camera's aim-marker lag at the start
    f32  f2C;                   // 0x02C  ... once there, its height's share of the move a frame
    f32  f30;                   // 0x030  ... it slows down over this last distance
    f32  f34;                   // 0x034  ... its base speed
    f32  f38;                   // 0x038  ... its distance back on a putt
    f32  f3C;                   // 0x03C  ... View.shot19C.fLookUpOffset falls by this each frame
    f32  f40;                   // 0x040  ... once there, its height moves by this a frame
    f32  f44;                   // 0x044  ... its height over the goal (for a lens of fB0 1)
    f32  f48;                   // 0x048  ... the aim marker's lag
    f32  f4C;                   // 0x04C  camera 5's height over the ball
    f32  f50;                   // 0x050  camera 5: the ball-to-pin distance of a full swing out
    f32  f54;                   // 0x054  camera 5: how long the swing out lasts
    f32  f58;                   // 0x058  how long camera 13's closing shot lasts
    f32  f5C;                   // 0x05C  how long the matrix camera's first shot lasts (twice as long for
                                //        swing camera kinds other than 1 and 6)
    f32  f60;                   // 0x060  camera 13's slow-motion step length (0: every frame)
    f32  f64;                   // 0x064
    f32  v68[4];                // 0x068  camera 13's fn_80038010 vector; [3] shrinks as the camera's time
                                //        runs
    f32  f78;                   // 0x078  ... over this many seconds
    f32  f7C;                   // 0x07C  camera 13's fallback shots' fFovStart/fFovEnd: from this value ...
    f32  f80;                   // 0x080  ... to this one
    f32  f84;                   // 0x084  how long the super zoom's first shot lasts
    f32  f88;                   // 0x088
    f32  f8C;                   // 0x08C
    f32  f90;                   // 0x090  CameraController_CheckForEvents switches to camera 2 while the ball
                                //        is below this height (and falling, not yet bounced)
    f32  f94;                   // 0x094  the elevator camera's first blend value
    f32  f98;                   // 0x098  camera 8: 1 - this is its height's share of the move a frame
    f32  f9C;                   // 0x09C  the swing camera: the least shot power for one
                                //        (GolfCamera_ChooseSpecialSwing)
    f32  fA0;                   // 0x0A0  ... above this power, the chance (percent) is fAC
    f32  fA4;                   // 0x0A4  ... above this one, fB0 (else fA8)
    f32  fA8;                   // 0x0A8
    f32  fAC;                   // 0x0AC
    f32  fB0;                   // 0x0B0
    f32  fB4;                   // 0x0B4  ... above this power with a short club, always kind 11
    f32  fB8;                   // 0x0B8  camera 15 waits this long on a ball near the green
    s32  nBeats;                // 0x0BC  the heartbeat camera's beats
    s32  nBeatFrames;           // 0x0C0
    f32  fC4;                   // 0x0C4
    f32  fC8;                   // 0x0C8
    f32  fCC;                   // 0x0CC
    f32  fD0;                   // 0x0D0  CameraScript_GetBallHeightWithMaxHeight: how fast the aim
                                //        comes down to a shot's fMaxHeight
    f32  fD4;                   // 0x0D4  CameraScript_LagBallFlight: the aim eases in over this
                                //        much of a move's time
    f32  fD8;                   // 0x0D8  CamScript_GetLookAtPoint: CameraScript_LagAimMarker's first lag
    f32  fDC;                   // 0x0DC  the green zoom-to-aim camera's aim marker
                                //        (CameraScript_LagAimMarker)
    f32  fE0;                   // 0x0E0  CamScript_SmoothTerrainHeight: CamScript.fD8's share of
                                //        the way to the ground height a frame
    f32  fE4;                   // 0x0E4  CamScript_GetLookAtPoint: the aim's lag on the ball
                                //        (CameraScript_LagTargetPoint,
                                //        CameraScript_KeepPointInView); also put in
                                //        CamScript.fLagAngle
    f32  fE8;                   // 0x0E8  ... its lag on a bone of the golfer (CameraScript_LagTargetPoint)
    f32  fEC;                   // 0x0EC  CamScript_CheckOutOfBounds: the camera time before it checks
                                //        the hole's outline
    f32  fF0;                   // 0x0F0  CameraScript_LagAimMarker: the aim closer than this (over the
                                //        lens's fB0, times the drop to it) is pushed out to it
    f32  fF4;                   // 0x0F4  CamScript_GetCameraOnFairwayPos: a spot is taken when the
                                //        dot product of its level direction to the ball with the
                                //        camera's is below this
    f32  fF8;                   // 0x0F8
    f32  fFC;                  // 0x0FC  CameraScript_IsDefaultSwingCam: the least dot product of the
                                //        camera's and the aim's level directions from the ball
    f32  f100;                  // 0x100  CamScript_UpdateFairwayCam: moves on only from this level
                                //        distance to the spot CamScript_GetNearestAIPoint picks ...
    f32  f104;                  // 0x104  ... after this long on the camera ...
    f32  f108;                  // 0x108  ... and while the ball heads away from the camera (a dot
                                //        product at most this)
    f32  f10C;                  // 0x10C  CamScript_GetCameraOnFairwayPos: the camera's height over
                                //        the ground at the spot
    f32  f110;                  // 0x110  CamScript_UpdateFairwayCam: the shot's field of view narrows
                                //        down to this ...
    f32  f114;                  // 0x114  CamScript_PutBackOnFairway: its shot's field of view
    f32  f118;                  // 0x118  ... by this a frame (CamScript_UpdateFairwayCam)
    f32  f11C;                  // 0x11C  CameraScript_CalculateShoulderShake: a shot's fShakeAmount over
                                //        this is its wobble's size
    f32  f120;                  // 0x120  CamScript_CheckObstructedCamera: a camera closer to the
                                //        pin than this (level, times the lens's fB0) ...
    f32  f124;                  // 0x124  ... and less than this above it counts as in the way
    f32  f128;                  // 0x128  CamScript_GetLookAtPoint: the least level distance for the
                                //        steep-aim limit
    f32  f12C;                  // 0x12C  CameraScript_LagAimMarker: the most the aim may sit below the camera
    f32  f130;                  // 0x130  CamScript_GuessBestPlayableHeight: the headroom a camera
                                //        needs over a ground layer
    f32  f134;                  // 0x134  CameraScript_LagTargetPoint: the look-at point eases in
                                //        slower within this share of the (field-of-view scaled)
                                //        camera distance
    f32  f138;                  // 0x138  CameraScript_LagBallFlight: the aim's most share of the
                                //        way a step ...
    f32  f13C;                  // 0x13C  ... reached at this distance from it
    f32  f140;                  // 0x140  CameraScript_LagBallFlight: its height share for a falling
                                //        ball near the ground
    f32  f144;                  // 0x144  CameraScript_LagTargetPoint: the look-at point's level
                                //        share of the way a frame
    f32  f148;                  // 0x148  ... and its height's
    f32  f14C;                  // 0x14C  } DynamicCam_TrackBallVelocityLag: how fast a following camera
    f32  f150;                  // 0x150  } closes the distance and the angle to its target, per 60th
    f32  f154;                  // 0x154  DynamicCam_TrackBallVelocityLag: while CamScript.f98 is
                                //        under this, both are scaled by ((f154 - fCamTime) / f154)
                                //        squared, over f14C (tested on f98, eased by fCamTime)
    f32  f158;                  // 0x158  CameraScript_LagBallFlight: the aim eases in over this
                                //        much of CamScript.f88
    f32  f15C;                  // 0x15C  CameraScript_InterpToNewScript puts it in CamScript.f88 (0 for
                                //        the default swing camera)
    f32  f160;                  // 0x160  CameraScript_LagBallFlight: the power of its height ease-in
    f32  f164;                  // 0x164  CamScript_GetLookAtPoint: kind 13's share of the height change
                                //        a frame
    f32  f168;                  // 0x168  the ground clearance for CamScript_KeepAboveGround
    f32  f16C;                  // 0x16C  the obstruction radius around the ball for the pre-shot routine
    f32  f170;                  // 0x170  a blend for CameraController_FadeIn / CameraController_FadeOut
    f32  f174;                  // 0x174  DynamicCam_ProcessScript: how softly a camera eases in under its
                                //        height limit
    f32  f178;                  // 0x178
    f32  v17C[4];               // 0x17C
    f32  f18C;                  // 0x18C  DynamicCam_TrackBallVelocityTight: the ball-flight camera closes in
                                //        by this share of the height above the shot's fMaxHeight ...
    f32  f190;                  // 0x190  ... and backs off by this share of the height below its fMinHeight
    f32  f194;                  // 0x194  DynamicCam_ProcessScript: how far a camera below its least height
                                //        rises a frame
    f32  f198;                  // 0x198  DynamicCam_TrackBallVelocityTight: the least ball speed it follows
                                //        the flight at
    f32  f19C;                  // 0x19C  the steepest a camera direction may tilt
                                //        (DynamicCam_ClampBallVelocity, radians)
    f32  f1A0;                  // 0x1A0
    f32  f1A4;                 // 0x1A4  DynamicCam_ChoosePreFlightSequence: the obstruction test's slope
    f32  f1A8;                  // 0x1A8  CamScript_RunScript: how fast CamScript.fBallUpdates follows the
                                //        ball's updates per frame
    f32  f1AC;                  // 0x1AC
    f32  f1B0;                  // 0x1B0
    f32  f1B4;                  // 0x1B4  } CameraController_CameraCollision (GoCamCont.c), flat directions:
    f32  f1B8;                  // 0x1B8  } the least cosine from the camera's motion (f1B4) and from its look
    f32  f1BC;                  // 0x1BC  } (f1B8) to the object; the most the camera may move in a frame
                                //        } (f1BC)
    s32  n1C0;                  // 0x1C0  nonzero enables camera 19
    s32  n1C4;                  // 0x1C4
    s32  bCheckSlope;           // 0x1C8  GolfCamera_NeedSteepSlopeCam tests the slope to the target
                                //        (GolfCamera_SteepSlopeCamCheckSlope)
    s32  bCheckTerrain;         // 0x1CC  and the ground in between (GolfCamera_SteepSlopeCamCheckCollision)
    f32  f1D0;                  // 0x1D0  the steep-slope camera: height step per try (down going
                                //        up, up going down)
    f32  f1D4;                  // 0x1D4  ... distance step back per try
    f32  f1D8;                  // 0x1D8  ... first distance back from the ball
    s32  n1DC;                  // 0x1DC  ... most tries
    f32  fSlopeUp;              // 0x1E0
    f32  fSlopeDown;            // 0x1E4
    f32  f1E8;                  // 0x1E8  the steep-slope camera's base: x offset from the ball
    f32  f1EC;                  // 0x1EC  ... height over the ball
    f32  f1F0;                  // 0x1F0  ... how far the camera may move per call
    f32  f1F4;                  // 0x1F4  ... how far the aim may move per call
    f32  fMaxPitchUp;           // 0x1F8  GolfCamera_ClampLookAngle: the steepest camera angle above
                                //        the horizontal (degrees)
    f32  fMaxPitchDown;         // 0x1FC  and below it
    f32  f200;                  // 0x200  } the camera shake CameraController_Idle starts on the swing's
    f32  f204;                  // 0x204  } events 5..14: CamScript.fShakeAmount and fShakeTime
                                //        } (CameraController_SetShakeAmount)
    f32  f208;                 // 0x208  times lbl_801D5010[view]: the alpha of GoPostFx fn_80039358's
                                //        black cover
    f32  f20C;                 // 0x20C  camera 4: the most View.f54 grows to
    f32  f210;                  // 0x210  camera 4: how fast (per second) it moves in from its start
    f32  f214;                  // 0x214  camera 4: how fast View.f54 grows and shrinks
    f32  f218;                  // 0x218  camera 4: how fast (per second) buttons 0x31/0x32 turn it round
    f32  f21C;                  // 0x21C  camera 4: the angle of its swing between the ball and the pin
    f32  f220;                  // 0x220  camera 4: its ground clearance, and its least height over the ball
    f32  f224;                  // 0x224  camera 4: (f20C - 1) times this raises the aim each frame ...
    s32  n228;                  // 0x228  ... unless this is set: then the aim is at the camera's height
    f32  f22C;                  // 0x22C  placement height kind 7 (GoDynamicCam.c
                                //        DynamicCam_AddHeightOffset): the share of the way to the
                                //        new height taken per ball step, rising ...
    f32  f230;                  // 0x230  ... and falling towards the shot's fMinHeight
    f32  f234;                  // 0x234
    f32  f238;                  // 0x238
    f32  f23C;                  // 0x23C  placement kind 8 (GoDynamicCam.c DynamicCam_TrackOffset): past this
                                //        distance the camera's offset is scaled from f240 ...
    f32  f240;                  // 0x240
    f32  f244;                  // 0x244  ... down to f248 at this distance and beyond
    f32  f248;                  // 0x248
    s32  n24C;                  // 0x24C
    f32  f250;                  // 0x250
    f32  f254;                  // 0x254
    f32  f258;                  // 0x258
} CamTuning;
LAYOUT_ASSERT(CamTuning, 0x25C);

extern CamTuning* gpCamTuning;         // EA's file list has GoCamTuningVars

// The golf cameras' shared state (0x200 bytes, allocated by GolfCamera_Init).
typedef struct GolfCamState {
    f32     fElevatorHeight[21];    // 0x000  per course, added to the elevator shot's height
    u8      bMatrixCam;         // 0x054  the matrix camera is running (freezes time)
    u8      bScriptMatrixMode;  // 0x055  the camera script's matrix mode (freezes time)
    u8      bComicCam;          // 0x056  the comic (3-screen) camera is on
    u8      b57;                // 0x057
    u8      bSuperZoomCam;      // 0x058  the super zoom is running (freezes time)
    u8      bSlowMoSwingCam;    // 0x059  the slow-motion swing camera is on
    u8      bHeartBeatCam;      // 0x05A  the heart beat camera is beating
    u8      b5B;                // 0x05B  set by the shutter camera
    u8      b5C;                // 0x05C
    u8      b5D;                // 0x05D  cleared by the swing camera
    u8      unk5E[2];
    s32     nFlyByRoute;        // 0x060  the fly-by route (StaticCam_GetFlyByCam)
    f32     fHeartBeatSlowMo;   // 0x064  camera 7's slow-motion rate while bHeartBeatCam is set
    f32     f68;                // 0x068
    CamShot shot6C;             // 0x06C  the tutorial wait's two hand-made shots (camera 18)
    CamShot shot12C;            // 0x12C
    s32     n1EC[5];            // 0x1EC  per player: the next swing camera kind (View.nSpecialSwingType) to
                                //        use, 1..11 in turn
} GolfCamState;

// The golfer shown on the menu screens (FEgolferanim.c): the create-a-player (CrAP) screen's
// state at gpCrAPState (0x1E0 bytes, allocated by FE_CharMgrInit). The golfers are kept in a ring
// of CRAP_NUM_GOLFERS slots (the code is written for more than one; the game uses one).
#define CRAP_NUM_GOLFERS 1

typedef struct CrAPGolfer {
    struct CrAPGolfer* pPrev;   // 0x00  } the ring
    struct CrAPGolfer* pNext;   // 0x04  }
    struct Character* pChar;    // 0x08  the golfer being edited (character.h)
    s32  nGolferId;             // 0x0C  the golfer's id (-1: none)
    s32  nIndex;                // 0x10  its slot number (FE_CharMgrInit); the gSession.aD2D entry
                                //       Character_RequestClothesUpdateFE sets for it
    s32  nStreamedId;           // 0x14  nGolferId once its stream was opened
                                //       (FE_StreamFunc_SkinInit), -1 none
    u8   bLoaded;               // 0x18  loaded and ready to show (set once its textures are in;
                                //       the CrAP camera script runs only then)
    u8   bFree;                 // 0x19  its character is to be freed (FE_vFreeUnusedCharacters)
    u8   unk1A[2];
    s32  nSetupKind;            // 0x1C  the screen kind (CrAPState.nScreenKind) FE_SetupCharState
                                //       set him up for; -1 none
} CrAPGolfer;

typedef struct CrAPState {
    s32  nScreenKind;           // 0x000  the menu screen showing the golfer (a menu message sets
                                //        it): 0 golfers in turn (gFEGolferCycle), 3 the CrAP
                                //        screen, 4 none; picks the shot the CrAP camera frames
                                //        (GolfCamera_ProcessFECamera)
    s32  nCamIdleState;         // 0x004  the CrAP camera's idle state (FE_SetCrAPCameraIdleState):
                                //        the camera kind GolfCamera_SwitchCrAPCamera gets, and it
                                //        picks the idle clip (FE_CrapGetIdleAnim)
    s32  nRenderState;          // 0x008  what the CrAP screen shows: 0 the golfer, 1 his clubs, 2
                                //        the ball (FE_SetCrapRenderState)
    s32  nTempRenderState;      // 0x00C  a render state for the queued animation only: it takes
                                //        over when that starts, and 0 comes back when it ends
                                //        (FE_SetTempCrapRenderState)
    char szCurAnim[0x10];       // 0x010  the animation FE_vTriggerCrAPAnimAndCamera started ("":
                                //        the idle one took over)
    char szQueuedAnim[0x10];    // 0x020  } the queued animation and its camera shot
    char szQueuedShot[0x20];    // 0x030  } (FE_QueueCrAPAnim; "": none / the idle state's)
    s32  nAnimRepeats;          // 0x050  times the current animation plays again before the idle
                                //        one (FE_SetAnimRepeatCount)
    char szQueuedBall[0x20];    // 0x054  the ball texture the queued animation puts on
                                //        (FE_QueueBallChange)
    s32  nTexSwapState;         // 0x074  the texture swap: 1 loading, 2 loaded and waiting, 0 done
                                //        (FE_SetTextureSwapState)
    u8   bDelayTexSwap;         // 0x078  a loaded swap waits for the queued animation,
                                //        fTexSwapDelay seconds into it (FE_SetDelayTextureSwap)
    u8   unk79[3];
    f32  fTexSwapDelay;         // 0x07C
    u8   bTempRenderState;      // 0x080  nTempRenderState is set
    u8   bNewTextures;          // 0x081  new textures are on the golfer (FE_SetNewTexturesFlag);
                                //        not read
    u8   b82;                   // 0x082
    u8   bDimmed;               // 0x083  the golfer is dimmed (GM_vSetGolferDimmed): alpha at most
                                //        gFEDimAlphaMax, darker lighting (FEgolferanim.c)
    u8   b84;                   // 0x084
    u8   b85;                   // 0x085
    u8   bHidden;               // 0x086  the golfer is not updated or drawn (a menu message sets
                                //        it; a change while nScreenKind is 3 calls
                                //        FE_OnGolferHiddenChanged)
    u8   b87;                   // 0x087
    u8   b88;                   // 0x088
    u8   b89;                   // 0x089
    u8   bClearCache;           // 0x08A  FE_vClearGolferCache asked for the golfers to be freed;
                                //        FE_vFreeUnusedCharacters does it once the loader is idle
    u8   unk8B;
    s32  n8C;                   // 0x08C  the golfer id whose stream was opened last (-1: none)
    u8   b90;                   // 0x090
    u8   b91;                   // 0x091
    u8   unk92[2];
    CrAPGolfer aGolfer[CRAP_NUM_GOLFERS];   // 0x094
    CrAPGolfer* pB4;            // 0x0B4  the golfer shown
    CrAPGolfer* pB8;            // 0x0B8  the golfer being loaded
    s32  nBC;                   // 0x0BC  which of pB4, its pNext or its pPrev pB8 is (0..2)
    f32  mC0[4][4];             // 0x0C0
    f32  v100[4];               // 0x100
    f32  v110[4];               // 0x110
    f32  v120[4];               // 0x120
    f32  v130[4];               // 0x130
    f32  f140;                  // 0x140
    f32  f144;                  // 0x144
    f32  f148;                  // 0x148
    f32  fAlpha;                // 0x14C  the golfer display's fade, 0..0.5 (FE_vUpdateGolferAll)
    u8   unk150[0x18C - 0x150];
    u8   b18C;                  // 0x18C
    u8   unk18D[3];
    s32  n190;                  // 0x190  counts the golfers loaded
    s32  n194;                  // 0x194  } the next golfer to show: a column and row of
    s32  n198;                  // 0x198  } gFEGolferCycle
    f32  fFacing;               // 0x19C  } the way the golfer faces and the way he turns to, in
    f32  fTargetFacing;         // 0x1A0  } radians (FE_RotateCrAPModel, FE_SetCrapRotation)
    u8   unk1A4[0x1B0 - 0x1A4];
    u8   b1B0;                  // 0x1B0
    u8   unk1B1[3];
    s32  nIdleCount;            // 0x1B4  idle clips played in a row (FE_vLoadNextCrAPAnim: after
                                //        the fifth he turns to the front; -1 then)
    s32  nClub;                 // 0x1B8  the club he holds, a gClubPartNames class (FE_SetCrapClub)
    s32  nTempClub;             // 0x1BC  the club for the queued animation only (-1: his own)
    s32  nQueuedState;          // 0x1C0  the queued animation: 0 waits for the current one's end,
                                //        1 fades out (fFadeOutTime) and starts, 2 plays, 3 fades
                                //        back in, 4 none (FE_QueueCrAPAnim, FE_vUpdateGolferAll)
    s32  nUnlockState;          // 0x1C4  } the pad cannot turn or zoom the golfer while bPadLocked;
    u8   bPadLocked;            // 0x1C8  } it unlocks when nQueuedState reaches nUnlockState (or 4)
    u8   unk1C9[3];
    f32  fFadeOutTime;          // 0x1CC  seconds of fade left before the queued animation starts
    u8   bFadeAtEnd;            // 0x1D0  the queued animation fades out at its end and back in (0:
                                //        fAlpha stays 0.5)
    u8   bClubStatesAllowed;    // 0x1D1  FE_SetClubStatesAllowed
    u8   bTexSwapDue;           // 0x1D2  a loaded texture swap is to be switched in
                                //        (FE_SetTextureSwapDue)
    u8   unk1D3;
    s32  nLastAsset;            // 0x1D4  the slider asset last shown (-1: none;
                                //        FE_SetLastCrAPAsset)
    s32  nLastCategory;         // 0x1D8  the part of the asset last put on (-1: none;
                                //        FE_SetLastCrAPCategory)
    u8   bZoom;                 // 0x1DC  the CrAP camera is zoomed in (FE_ZoomCrAPModel)
    u8   unk1DD[3];
} CrAPState;
LAYOUT_ASSERT(CrAPState, 0x1E0);

extern CrAPState* gpCrAPState;
extern struct Character* gFEGolferChars[CRAP_NUM_GOLFERS];   // per golfer slot: the character it starts
                                        // with (none); char.c's Character_FreeFEGolfers frees them

// ---- the views ------------------------------------------------------------------------------

// A render camera: TW07's render context RC_SRenderCtx (GoRenderCtx.h); only what the cleaned
// code reads here (the fuller layout is Camera in unsorted/cull.h).
typedef struct RenderCamera {
    u8   unk0[0x14];
    f32* pRect;                 // 0x14  its viewport (TW07 VM_SViewport): left, top, width, height,
                                //       fractions of the frame buffer (RC_spGetRenderCtxViewport)
} RenderCamera;

// Points at the slot holding the current render camera (lbl_80281C90): RC_spGetCurrentRenderCtx reads it,
// RC_vSetCurrentRenderCtx sets it.
extern void** lbl_80280DF0;

// GoTerrain.c: gives the current render camera the model matrix pMtx (NULL: the identity), through
// RC_vSetRenderCtxTransformationMatrix.
void   RC_vSetCurrentRenderCtxTransformationMatrix(f32 (*pMtx)[4]);
// GoRenderCtx_Gc.c: gives the camera the model matrix pMtx (NULL: the identity).
void   RC_vSetRenderCtxTransformationMatrix(void* pCamera, f32 (*pMtx)[4]);
f32*   fn_8003526C(void);               // GoTerrain.c: the current render camera's screen rectangle

ViewController* ViewController_GetCurrentViewController(void);     // the current view (gpCurViewController)
ViewController* ViewController_GetIndexedViewController(int nView);
void*  ViewController_GetRenderContext(int nView);          // the view's render camera
View*  ViewController_GetCameraControl(int nView);
void   ViewController_SetActivePlayerNumber(int nView, int nPlayer);   // the player the view follows
int    ViewController_GetActivePlayerNumber(int nView);          // the player the view follows (as set above)
u8     ViewController_IsActive(int nView);          // the view is in use
void   ViewController_TurnOnViewController(int nView, u8 bActive);   // sets ViewController.bActive
f32*   CameraController_GetCameraOrigin(View* pView);        // the camera's position (v0)
f32*   CameraController_GetCameraLookPoint(View* pView);    // where it looks (v10), or a script shot's angles
u8     CameraController_IsFlybyDone(View* pView);        // 0: the script's shot aims by angles
                                                         // (ViewController_Update)
f32*   RC_spGetRenderCtxViewport(void* pCamera);      // a render camera's screen rectangle
f32    VM_fGetViewportHeight(f32* pRect);         // the rectangle's [3]: its height
f32    VM_fGetViewportWidth(f32* pRect);         // [2]: its width
f32    VM_fGetViewportTop(f32* pRect);         // [1]: its top
f32    VM_fGetViewportLeft(f32* pRect);         // [0]: its left
void*  RC_spGetCurrentRenderCtx(void);               // the current render camera
void   RC_vUpdateRenderCtxTransformationMatrices(void* pCamera);
void   RenderState_SetViewport(void* pCamera);
void   RenderState_SetCameraMatrices(void);
int    ViewController_GetCurrentViewControllerID(void);
// Sets a viewport's rectangle (fractions of the frame buffer).
void   VM_vSetViewportRect(f32* pViewport, f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight);
// A world position on screen (0..1 across and down; pZ, if not NULL, gets a third value). Returns
// 1 when the point is in front of the camera (clip w below 0), else 0.
u8     RC_vComputeRenderCtxWorldToPrimitiveCoordinate(void* pCamera, f32* pPos, f32* pX, f32* pY, f32* pZ);
void   fn_8006A8D4(void* pCamera, f32* pX, f32* pY);

// ---- camera shots and sequences (0x8003A7C8..) ----------------------------------------------

CamShot* DynamicCam_ChooseScript(int nPlayer, int nKind, CamShot* pShot);
u8       DynamicCam_bIsSwingCamera(CamShot* pShot);    // the shot is one of kinds 1, 3, 13, 28..34 or 40..45
CamShot* DynamicCam_ChooseScriptByName(char* szName);     // the shot with this name (case ignored), or NULL
// A shot of kind nKind from the sequence, picked at random, and its blend values (each out
// pointer may be NULL).
CamShot* DynamicCam_ChooseScriptInSequence(CamSequence* pSequence, int nKind, int* pA, f32* pF1, f32* pF2,
                     int* pB, f32* pF3, int nPlayer);
CamSequence* DynamicCam_ChooseSequence(int nPlayer, int nLie, int nClass, int nKind, u8 a, f32 fDist);
CamSequence* DynamicCam_ChoosePreFlightSequence(int nPlayer, int nLie, int nKind);
// The sequence and shot named after the golfer's clip (with b, Character.p1790 first).
u8       DynamicCam_ChoosePairedSequenceOrCamera(int nPlayer, u8 b, CamSequence** ppSeq, CamShot** ppShot);
// it suits the player's club and shot
u8       DynamicCam_IsValidFlightSequence(CamSequence* pSequence, int nPlayer);
u8     CamScript_DoesScriptTrackGolfer(CamShot* pShot);     // the shot's nLookAtKind is 1..6 or 7
// The ball's position, or the script's v70 when the ball is by the pin (with bKeep v70 follows it).
void   DynamicCam_GetSmoothBallLocation(CamScript* pScript, CamShot* pShot, int nPlayer, f32* pOut, u8 bKeep);
// 0 when gSession.nGameType is 3, else Character_IsLeftHanded of the player's golfer (Player.pChar)
// as a flag; the shot is not read.
u8     CameraScript_FlipCameraForLefty(int nPlayer, CamShot* pShot);

// ---- the camera scripts (gocamscripts.c, 0x8003DCE8..) ----------------------------------------

// pCam and pSub are the view's camera position and where it looks
// (CameraController_GetCameraOrigin, CameraController_GetCameraLookPoint).
void     CamScript_RunScript(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, CamShot* pShot, u8 b,
                     f32 fTime);
void     CamScript_RunFEScript(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, CamShot* pShot, int a,
                     f32 fTime);
void     CamScript_RunFlybyCamera(int nPlayer, f32* pCam, f32* pSub, CamScript* pScript, CamShot* pShot, u8 b,
                     f32 fTime);
void     CamScript_Fade(CamScript* pScript, f32 fTime);
void     CameraScript_RecordCurrentCam(CamShot* pShot, f32* pCam, f32* pSub, int nPlayer, CamScript* pScript,
                                       u8 bView1);
void     CameraScript_InterpToNewScript(CamScript* pScript, CamShot* pShot, int nPlayer, f32* pCam, f32* pSub,
                                        int nInterpType, f32 fInterpTime, f32 fMaxSpeed,
                                        int nTimeTrigger, f32 fTimeTriggerTime);
u8       CameraScript_SnapToScript(CamScript* pScript, CamShot* pShot);
void     CameraScript_LagAimMarker(int nPlayer, f32* pSub, f32* pCam, CamShot* pShot, u8 bClose, u8 bLimit,
                                   f32 fRate, f32 fMinDist, f32 fYShare);
void     CamScript_GetCameraOnFairwayPos(CamScript* pScript, f32* pOut, f32* pCam, int nPlayer,
                     CamShot* pShot, f32* pSub, f32* pHeight);
void     CamScript_PutBackOnFairway(CamScript* pScript, f32* pCam, f32* pSub, int nPlayer, CamShot* pShot,
                                    f32* pPrev);
// how far the ball's flight has run
f32      CamScript_EstimateBallFlightPercent(int nPlayer, CamScript* pScript);
u8       CamScript_SkipLookBackCam(CamScript* pScript, CamShot* pShot, int nPlayer);
void     CameraScript_UpdateLandingEstimate(CamScript* pScript, int nPlayer);
u8       CameraScript_IsDefaultSwingCam(CamShot* pShot, int nPlayer, f32* pCam);
// Keep pNew above the ground (by fClearance); the out values are optional (NULL): pbFound any
// ground under pNew, pfGround the ground height used, pbRaised pNew was raised.
u8       CamScript_KeepAboveGround(int nPlayer, f32* pNew, f32* pOld, u8 bCheckPath, u8* pbFound,
                                   f32* pfGround, u8* pbRaised, f32 fClearance);
u8       CameraScript_WillGolferBeOccludedInThisView(int nPlayer, CamShot* pShot, CamScript* pScript);
void     CA_vSetCameraFieldOfView(CamLens* pLens, f32 fFov);   // sets the lens's field of view
u8       CamScript_DoesScriptTrackBall(CamShot* pShot);             // the shot's nLookAtKind is 0, 13..15 or
                                                                    // 23

// GoCamera.c: works out the lens's fB0 from its field of view.
void     CA_vUpdateInternalFieldOfViewData(CamLens* pLens);

// ---- camera script helpers (CamSpline.c, 0x800C7480..)-------------------------------------

// Both write a point into pOut: CamUtils_vGetPositionBetweenTwoPoints goes fDist along the
// direction from pA to pB (its y cleared unless bKeepY, normalised unless bRaw), then fSide across
// the flattened direction; CamUtils_vCalcArcPosition swings around pC from pA towards pB at share
// fT (nDir: 0 the short way round, 1 angle decreasing, else increasing).
void   CamUtils_vGetPositionBetweenTwoPoints(f32* pA, f32* pB, u8 bKeepY, u8 bRaw, f32* pOut, f32 fDist,
                                             f32 fSide);
void   CamUtils_vCalcArcPosition(f32* pA, f32* pB, f32* pC, int nDir, f32* pOut, f32 fT);
// The splined camera (CamScript_SplineCameras): the camera position on the spline through pPos0..3,
// the look angles on the one through pLook0..3 (each unwrapped to within half a turn of the one
// before), and the field of view between fFov1 and fFov2, at share fT between the middle two.
void   CamScript_SplineCamerasByPositionAndLook(f32* pPos0, f32* pPos1, f32* pPos2, f32* pPos3,
                                                f32* pLook0, f32* pLook1, f32* pLook2, f32* pLook3,
                                                f32* pCam, f32* pSub, f32* pFov, f32 fFov1, f32 fFov2,
                                                f32 fT);
// a point on the spline
void   CamScript_SplineCamerasByPosition(f32* p0, f32* p1, f32* p2, f32* p3, f32* pOut, f32 fT);
f32    CamScript_GetFlybyTimeStep(f32 fA, f32 fB, f32 fC, f32 fD, f32 fE, f32 fF);
// The value of a fly-by path's curve at time fT (CamScript_RunFlybyCamera).
f32    CamScript_fEvaluateCurve(FlyByPath* pPath, f32 fT);
// CamSpline.c: the Catmull-Rom basis matrix.
extern f32 gCatmullRomBasis[4][4];

// ---- the static and fly-by cameras (GoStaticCam.c, 0x8006449C..) ----------------------------

void     StaticCam_ProcessScript(CamShot* pShot, int nPlayer, f32* pOut);   // the shot's position
CamShot* StaticCam_ChooseScript(int nPlayer, int nKind, u8 bNotKind5, CamShot* pNot);
CamShot* StaticCam_GetFlyByCam(int nPath);        // a fly-by path's first shot (NULL past the 10th)
FlyByPath* StaticCam_GetFlybyTimeCurve(u32 uPath);      // a fly-by path's timing curve (NULL: none)
void     StaticCam_GetFlybyInformation(CamScript* pScript, int nPath, f32* pCam, f32* pSub, f32* pFov,
                     int nPlayer, f32 fShare);

// ---- the camera modes' setups (GoGolfCam.c), one per CameraController_SetCameraMode mode --------

void   GolfCamera_InitShotSetupCamera(View* pView, int nPlayer);                       // camera 0
void   GolfCamera_InitZoomToAimCamera(View* pView, int nPlayer);    // 1
void   GolfCamera_InitGreenZoomToAimCamera(View* pView, int nPlayer); // 2
void   GolfCamera_InitElevatorCamera(View* pView, int nPlayer);     // 3
void   GolfCamera_InitGreenCamera(View* pView, int nPlayer);                       // 4
void   GolfCamera_InitGreenRollCamera(View* pView, int nPlayer);                       // 5
void   GolfCamera_InitReversePuttCamera(View* pView, int nPlayer);                       // 6
void   GolfCamera_InitKneeCamera(View* pView, int nPlayer);                       // 7
void   GolfCamera_InitPlaceBallCamera(View* pView, int nPlayer);                       // 8
void   GolfCamera_InitSpeedGolfRunCamera(View* pView, int nPlayer);                       // 9
void   GolfCamera_InitFlyByCamera(View* pView, int nPlayer);                       // 10
void   GolfCamera_InitPreShotCamera(View* pView, int nPlayer);      // 11
void   GolfCamera_InitSwingCamera(View* pView, int nPlayer);        // 12
void   GolfCamera_InitReplaySwingCamera(View* pView, int nPlayer);                       // 13
void   GolfCamera_InitBallFlightCamera(View* pView, int nPlayer);   // 14
void   GolfCamera_InitPostShotCamera(View* pView, int nPlayer);     // 15
void   GolfCamera_InitInHoleCamera(View* pView, int nPlayer);       // 16
void   GolfCamera_InitScoreCardCamera(View* pView, int nPlayer);                       // 17
void   GolfCamera_InitTutorialWaitCamera(View* pView, int nPlayer); // 18
void   GolfCamera_InitSteepSlopeCamera(View* pView, int nPlayer);   // 19
void   GolfCamera_Init3ScreenCamera(View* pView, int nPlayer);                       // 20
void   GolfCamera_InitHeartBeatCamera(View* pView, int nPlayer);    // 21
void   GolfCamera_InitShutterCamera(View* pView, int nPlayer);      // 22
void   GolfCamera_InitFECamera(View* pView, int nPlayer);                       // 23
void   GolfCamera_InitGolferBoneCamera(View* pView, int nPlayer);                       // 24

// ... and their per-frame updates (GoGolfCam.c), one per mode, run by CameraController_Idle.
void   GolfCamera_ProcessShotSetupCamera(View* pView, int nPlayer);                       // camera 0
void   GolfCamera_ProcessZoomToAimCamera(View* pView, int nPlayer); // 1
void   GolfCamera_ProcessGreenZoomToAimCamera(View* pView, int nPlayer); // 2
void   GolfCamera_ProcessElevatorCamera(View* pView, int nPlayer);                       // 3
void   GolfCamera_ProcessGreenCamera(View* pView, int nPlayer);                       // 4
void   GolfCamera_ProcessGreenRollCamera(View* pView, int nPlayer);                       // 5
void   GolfCamera_ProcessReversePuttCamera(View* pView, int nPlayer);                       // 6
void   GolfCamera_ProcessKneeCamera(View* pView, int nPlayer);                       // 7
void   GolfCamera_ProcessPlaceBallCamera(View* pView, int nPlayer);                       // 8
void   GolfCamera_ProcessSpeedGolfRunCamera(View* pView, int nPlayer);                       // 9
void   GolfCamera_ProcessFlyByCamera(View* pView, int nPlayer);                       // 10
void   GolfCamera_ProcessPreShotCamera(View* pView, int nPlayer);                       // 11
void   GolfCamera_ProcessSwingCamera(View* pView, int nPlayer);                       // 12
void   GolfCamera_ProcessReplaySwingCamera(View* pView, int nPlayer);                       // 13
void   GolfCamera_ProcessBallFlightCamera(View* pView, int nPlayer); // 14
void   GolfCamera_ProcessPostShotCamera(View* pView, int nPlayer);  // 15
void   GolfCamera_ProcessInHoleCamera(View* pView, int nPlayer);    // 16
void   GolfCamera_ProcessScoreCardCamera(View* pView, int nPlayer);                       // 17
void   GolfCamera_ProcessTutorialWaitCamera(View* pView, int nPlayer);                       // 18
void   GolfCamera_ProcessSteepSlopeCamera(View* pView, int nPlayer); // 19
void   GolfCamera_Process3ScreenCamera(View* pView, int nPlayer);                       // 20
void   GolfCamera_ProcessHeartBeatCamera(View* pView, int nPlayer); // 21
void   GolfCamera_ProcessShutterCamera(View* pView, int nPlayer);                       // 22
void   GolfCamera_ProcessFECamera(View* pView, int nPlayer);                       // 23
void   GolfCamera_ProcessGolferBoneCamera(View* pView, int nPlayer);                       // 24

// ---- the camera controller (0x80062F38..) ---------------------------------------------------

void   CameraController_SetCameraMode(View* pView, int nCamera, int nPlayer, int nView);
void   CameraController_ResetCameraState(View* pView);
void   CameraController_CameraCollision(int nView, f32* pBounds);    // the view's camera is inside an
                                                                     // object's bounds
void   CameraController_FadeIn(View* pView, f32 fTime, f32* pVec);
void   CameraController_FadeOut(View* pView, f32 fTime, f32* pVec);
u8     CameraController_IsFadeDone(View* pView);             // script.nFade 3, 4 or 5: a colour fade held or
                                                             // ending
u8     CameraController_IsFadeOutDone(View* pView);             // script.nFade 4: kept after fading up
u8     CameraController_IsFadeOn(View* pView);             // script.nFade 1, 2 or 4: a colour fade running or
                                                           // held
void   CameraController_HoldFadeColor(View* pView, f32* pVec);  // script.nFade 3: hold the colour pVec over
                                                                // the view
void   CameraController_PostEvent(View* pView, int nKind, int nPlayer);
void   CameraController_ResetAimMarkerInSwingCamera(View* pView, int nPlayer);
u8     CameraController_bDontClearFrameBuffer(void);               // GolfCamera_bIs3ScreenCamOn's answer
                                                                   // (gomainloop tests it)
void   CameraController_LagSideVector(f32* pA, f32* pB, f32* pOut);   // the green zoom-to-aim camera:
                                                                      // View.vSide as pA and pOut

// ---- the golf cameras (GoGolfCam.c) ---------------------------------------------------------

void   GolfCamera_Init(void);
void   GolfCamera_DeInit(void);
void   GolfCamera_TurnOffComicCam(View* pView, int nPlayer);
u8     GolfCamera_Choose3ShotCam(View* pView, int nPlayer);
u8     GolfCamera_Choose3ScreenCam(View* pView, int nPlayer);
u8     GolfCamera_ChooseHeartBeatCam(View* pView, int nPlayer);
u8     GolfCamera_ChooseShutterCam(View* pView, int nPlayer);
int    GolfCamera_NumCompletedReplayCams(View* pView);
u8     GolfCamera_NeedSteepSlopeCam(View* pView, int nPlayer);
void   GolfCamera_vSwitchToNextAlternateSwingCamera(View* pView, int nPlayer);
void   GolfCamera_PickNextSwingReplayCam(View* pView, int nPlayer);
u8     GolfCamera_ChooseSuperSwing(View* pView, int nPlayer);
void   GolfCamera_ZoomGreenCamera(View* pView, int nPlayer);
void   GolfCamera_UnZoomGreenCamera(View* pView, int nPlayer);
void   GolfCamera_CutToGolferDoneAnimatingCam(View* pView, int nPlayer);
// Every caller passes a sixth argument (0 or 1) that the camera does not read.
void   GolfCamera_SwitchCrAPCamera(View* pView, char* szName, int nShot, u8 bBlend, u8 bForce, int n6);
u8     GolfCamera_IsPostShotCamFinalCutDone(View* pView);
void   GolfCamera_ChooseSpecialSwing(View* pView, int nPlayer);
int    GolfCamera_HowManyReplaySwings(View* pView);
f32    GolfCamera_ReplaySwingSpeed(View* pView);        // the slow-motion rate for the swing camera kind
void   GolfCamera_RestartHole(void);
u8     GolfCamera_bIs3ScreenCamOn(void);
u8     GolfCamera_bIs3ScreenFreezeOn(void);
u8     GolfCamera_IsMatrixCamActive(void);
u8     GolfCamera_IsSuperZoomCamActive(void);
u8     GolfCamera_IsSlowMoSwingCamActive(void);
u8     GolfCamera_IsFreezeTimeActive(void);
void   GolfCamera_DisableMatrixCam(void);
void   GolfCamera_DisableSuperZoomCam(void);
void   GolfCamera_DisableSlowMoSingCam(void);
void   GolfCamera_DisableHeartBeatCam(void);
u8     GolfCamera_IsThereACameraGoingToBeTimeTriggered(View* pView, int nPlayer);
u8     GolfCamera_IsPreShotCamReadyForFade(View* pView, int nPlayer, f32 fLeft);
void   GolfCamera_ForcePreShotEnding(View* pView);
void   GolfCamera_SetSkipFancyPreshotCams(View* pView, int a);
u8     GolfCamera_IsSetUpCameraDone(View* pView);
int    GolfCamera_GetSpecialSwingType(View* pView);
void   GolfCamera_SetCameraMatrixMode(int a);
u8     GolfCamera_IsScriptMatrixModeOn(void);
void   GolfCamera_SetPostShowPostShotAnimations(View* pView, int a);
u8     GolfCamera_ShowPostShotAnimations(View* pView);
void   GolfCamera_SetPostShowRemoveBall(View* pView, int a);
u8     GolfCamera_ShowPostRemoveBall(View* pView);
void   GolfCamera_AbortAllSpecialSwings(View* pView, int nPlayer);
u8     GolfCamera_IsBallFlightPaused(View* pView, int nPlayer);
u8     GolfCamera_IsZoomCamDone(View* pView, int nPlayer);

// ---- frame buffers (GoFrameBuf.c) -----------------------------------------------------------

// A frame buffer's size and scale (0x34 bytes; FB_spCreateFrameBuffer makes one). The last seven fields are
// worked out from the first six by FB_vUpdateInternalFrameBufferData.
typedef struct GoFrameBuf {
    f32  f0;                    // 0x00  0 by default
    f32  f4;                    // 0x04  0 by default
    f32  fWidth;                // 0x08  512 by default
    f32  fHeight;               // 0x0C  448 by default
    f32  f10;                   // 0x10  a horizontal scale, 1 by default
    f32  f14;                   // 0x14  a vertical scale, 1 by default
    f32  f18;                   // 0x18  fWidth * f10
    f32  f1C;                   // 0x1C  fHeight * f14
    f32  f20;                   // 0x20  fWidth / f10
    f32  f24;                   // 0x24  fHeight / f14
    f32  f28;                   // 0x28  1 / f10
    f32  f2C;                   // 0x2C  1 / f14
    f32  fAspect;               // 0x30  fHeight / fWidth
} GoFrameBuf;
LAYOUT_ASSERT(GoFrameBuf, 0x34);

void        FB_vUpdateInternalFrameBufferData(GoFrameBuf* pBuf);   // work out the derived fields
GoFrameBuf* FB_spCreateFrameBuffer(void);               // a new frame buffer with the default size
void        FB_vReleaseFrameBuffer(GoFrameBuf* pBuf);   // free it
void        FB_vSetDefaultFrameBuffer(GoFrameBuf* pBuf);   // the default size: 512 x 448, scale 1
void        FB_vSetFrameBuffer(GoFrameBuf* pBuf, f32 f0, f32 f4, f32 fWidth, f32 fHeight, f32 f10, f32 f14);
f32         fn_8001415C(GoFrameBuf* pBuf);   // GoRenderCtx_Gc.c: fHeight
f32         fn_80014164(GoFrameBuf* pBuf);   // f4
f32         fn_8001416C(GoFrameBuf* pBuf);   // fWidth
f32         fn_80014174(GoFrameBuf* pBuf);   // f0

// ---- the parts of a render camera: lens (GoCamera.c), screen rectangle (GoViewport.c) ---------

CamLens* CA_spCreateCamera(void);                     // a new lens
void     CA_vReleaseCamera(CamLens* pLens);           // free it
void     CA_vSetLookAt(CamLens* pLens, f32* pPos, f32* pTarget);   // aims the lens from pPos at pTarget
void     CA_vInitCamera(CamLens* pLens);
void     fn_80076948(CamLens* pLens, f32 fB4, f32 fB8);   // sets fB4 and fB8
void     fn_80076A0C_SetType(CamLens* pLens, s32 nType);          // sets nType
f32*     VM_spCreateViewport(void);                     // a new screen rectangle
void     VM_vReleaseViewport(f32* pRect);               // free it
void     fn_800B3438(f32* pRect, f32 x, f32 y); // shadow.c
CamLens* Camera_GetCurrentLens(void);                     // char.c

// GoRenderCtx_Gc.c: a render camera made from a lens, a frame buffer and a screen rectangle.
void*    RC_spCreateRenderCtx(CamLens* pLens, GoFrameBuf* pBuf, f32* pRect);
void     RC_vReleaseRenderCtx(void* pCamera);            // free it

#endif

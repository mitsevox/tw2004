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
    f32  mViewToWorld[4][4];    // 0x04  camera to world space (shadow.c fn_800B3484 hands it on as a
                                //       matrix): [2] is the view direction (goballfx.c
                                //       LF_fComputeAngleToLight takes its angle to a light), [3]
                                //       the camera's position (GameMode8 measures the ball's
                                //       distance to it); the green zoom-to-aim camera copies [0]
                                //       to View.vSide
    f32  mWorldToView[4][4];    // 0x44  world to camera space, the inverse of mViewToWorld
                                //       (hlaudemitterpool.c Aud_EmiSet3DPos moves a sound's
                                //       position with it)
    f32  aScalingVectors[2][4]; // 0x84  [1] the scale that
                                //       Camera_SetCameraPositionAndTargetWithOffsetAndScale puts
                                //       on the world around a point, [0] its inverse;
                                //       CA_vSetDefaultScalingVectors sets both to 1 (no scaling)
    f32  fFov;                  // 0xA4  the field of view (GoGolfCam.c sets DEG(60.0f) or DEG(30.0f))
    f32  fA8;                   // 0xA8  the near clip distance (RC_vUpdateRenderCtxScreenMatricesAndInfo
                                //       builds the projection from it); CA_vSetDefaultCamera starts it at 0.1
    f32  fAC;                   // 0xAC  the far clip distance; CA_vSetDefaultCamera starts it at 4096
                                //       (GoTerrain.c Camera_SetLensFarClip sets it)
    f32  fB0;                   // 0xB0  Camera_GetLensFovScale; the zoom-to-aim camera divides its
                                //       distance by it
    f32  fFlatWidth;            // 0xB4  } a flat camera's view width and height in world units:
    f32  fFlatHeight;           // 0xB8  } RC_vUpdateRenderCtxScreenMatricesAndInfo builds its
                                //       projection with Mtx_OrthoScale from them, LLObj_Gc.c
                                //       culls against them; 20 x 20 from CA_vSetDefaultCamera
                                //       (CA_vSetCameraFlatSize sets them; GoGrass.c's top-down grass camera
                                //       sets 20 x 20 too)
} CamLens;

f32 CA_fGetCameraFieldOfView(CamLens* pLens);        // GoRenderCtx_Gc.c: the lens's field of view

// How the camera script moves into a shot (CamShot.nInterpType, the script's nInterpType;
// CamScript_RunScript).
typedef enum CameraInterpType_e {
    CAMINTERP_SPLINE = 3,       // along the spline through the shots around it (CamScript_SplineCameras)
    CAMINTERP_CUT = 5           // no move: the camera stays on the shot, then cuts to the next
} CameraInterpType_e;

// The script state a shot serves (CamShot.nStateType, CameraSequence_t.nStateType): the golf camera
// that picks it (DynamicCam_ChooseScript, DynamicCam_ChooseSequence).
typedef enum CameraShotState_e {
    CAMSTATE_FLYBY = 0,         // a fly-by shot (CamScript_RunFlybyCamera flies it)
    CAMSTATE_PRE_SHOT = 1,      // the pre-shot camera (GolfCamera_InitPreShotCamera)
    CAMSTATE_SHOT_SETUP = 2,    // the shot set-up camera (GolfCamera_InitShotSetupCamera)
    CAMSTATE_SWING = 3,         // the swing camera (GolfCamera_InitSwingCamera)
    CAMSTATE_BALL_FLIGHT = 4,   // the ball-flight camera (GolfCamera_InitBallFlightCamera)
    CAMSTATE_ANY_SECOND_VIEW = 10,  // asked for, it takes a shot or sequence of any of states 6..9
                                    // (DynamicCam_MatchScriptStateType, DynamicCam_MatchStateType),
                                    // the shots whose field of view and blur go to the player's
                                    // second view (CamScript_IsSecondViewShot, which counts 10 too);
                                    // the golfer bone camera asks for it
                                    // (GolfCamera_InitGolferBoneCamera)
    CAMSTATE_GAMEBREAKER_FLIGHT = 11,   // a predicted GameBreaker's sequence, started while the ball flies
                                        // (GameEffects_InFlightGameBreakerTrigger)
    CAMSTATE_GAMEBREAKER_HIT = 12,      // a scripted GameBreaker's sequence, started as the ball is hit
                                        // (GameEffects_ScriptedGameBreakerBallHitTrigger)
    CAMSTATE_REPLAY_SWING = 13, // the replay swing camera's shots (GolfCamera_CreateReplayCamera; also
                                // the heartbeat and shutter cameras' when they have none of their own)
    CAMSTATE_ANY = 14,          // asked for, it takes a shot or sequence of any state (no caller asks
                                // for it)
    CAMSTATE_REACTION = 15,     // the reaction camera once the ball stops (GolfCamera_ChooseReactionCam)
    CAMSTATE_SWING_OBSTRUCTED = 28, // the swing camera when its view is blocked: an object or hazard
                                    // by the ball (DynamicCam_ChoosePreFlightSequence) or the golfer
                                    // hidden (GolfCamera_InitSwingCamera)
    CAMSTATE_ALT_SWING_1 = 29,  // } the alternate swing cameras the player steps through
    CAMSTATE_ALT_SWING_2 = 30,  // } (GolfCamera_GetAlternateSwingCamera)
    CAMSTATE_ALT_SWING_3 = 31,  // }
    CAMSTATE_ALT_SWING_4 = 32,  // }
    CAMSTATE_ALT_SWING_5 = 33,  // }
    CAMSTATE_MATRIX_SWING = 34, // the matrix camera's shot (GolfCamera_CreateMatrixCamera) and the
                                // slow-motion replay swing's (GolfCamera_CreateReplayCamera)
    CAMSTATE_STATIC = 38,       // a static camera
    CAMSTATE_COMIC_3SCREEN = 40, // } the comic camera's panels (GoComicCam.c): 40..42 for the
    CAMSTATE_COMIC_9SCREEN = 43  // } three-panel layout, 43..45 for the nine-panel one
} CameraShotState_e;

// A camera event (CameraController_PostEvent; the script's nRequestedEvent; the event a sequence's
// shot choice answers, CameraEventTrigger_t.nEvent).
typedef enum CameraEvent_e {
    CAMEVENT_ANY = 9,           // a choice for it answers every event but CAMEVENT_PRESHOT_TIMED
                                // (DynamicCam_ChooseScriptInSequence)
    CAMEVENT_TIMED_FIRST = 13,  // } the events a choice's time trigger may ask for
    CAMEVENT_TIMED_LAST = 22,   // } (CameraEventTrigger_t.nTimeTrigger); the pre-shot camera starts
                                //   with the first (GolfCamera_InitPreShotCamera)
    CAMEVENT_PRESHOT_TIMED = 23, // posted in the pre-shot when its sequence has a shot for it
                                 // (GolfCamera_IsThereACameraGoingToBeTimeTriggered)
    CAMEVENT_NONE = 25          // none: no time trigger (CameraScript_InterpToNewScript)
} CameraEvent_e;

// A point a shot places its camera by (CamShot.nFromPoint, nToPoint; DynamicCam_GetLocation) or looks
// at (CamShot.nLookAtKind; CamScript_GetLookAtPoint). Some kinds serve only one of the two: a kind the
// other has no case for leaves its point as it was. The golfer's points are his character's bones.
typedef enum CameraTracking_e {
    CAMPOINT_BALL = 0,          // the ball (DynamicCam_GetSmoothBallLocation; the look-at trails it in
                                // flight, CameraScript_LagBallFlight)
    CAMPOINT_BALL_SPOT = 1,     // where the ball lies for the shot (Player.vBall)
    CAMPOINT_GOLFER_FEET = 2,   // between the golfer's feet (bones 0x39 and 0x47)
    CAMPOINT_GOLFER_FEET_LAGGED = 3,    // look-at only: the same, trailed (CameraScript_LagTargetPoint)
    CAMPOINT_GOLFER_WAIST = 4,  // the golfer's waist (bone 1)
    CAMPOINT_GOLFER_WAIST_LAGGED = 5,   // look-at only: the same, trailed
    CAMPOINT_GOLFER_HEAD = 6,   // the golfer's head (bone 10)
    CAMPOINT_GOLFER_HEAD_LAGGED = 7,    // look-at only: the same, trailed
    CAMPOINT_GOLFER_CHEST = 8,  // camera only: the golfer's chest (bone 7)
    CAMPOINT_AIM = 9,           // the aim: Player.vTarget2 for the camera, Player.vTargetCopy (trailed,
                                // CameraScript_LagAimMarker) for the look-at
    CAMPOINT_PIN = 10,          // the pin
    CAMPOINT_TEE = 11,          // the player's tee
    CAMPOINT_LANDING = 12,      // camera only: where the shot should land (the script's vLandingEstimate)
    CAMPOINT_BALL_GROUND = 13,  // look-at only: the ground under the ball, its height eased
    CAMPOINT_BALL_LEVEL = 14,   // look-at only: the ball (DynamicCam_GetSmoothBallLocation) at the
                                // camera's height
    CAMPOINT_BALL_EXACT = 15,   // look-at only: the ball's position, without the shot's look offsets
    CAMPOINT_FAIRWAY_CAM = 16,  // camera only: the camera's own place, at the ball as the shot comes on
                                // (CameraScript_GoToNewScript, CameraScript_InterpToNewScript), moved
                                // to a spot on the fairway on its first frame (DynamicCam_GetLocation)
    CAMPOINT_GOLFER_HEAD_DIR = 17,  // } camera only: the golfer's head, chest or waist moved one unit
    CAMPOINT_GOLFER_CHEST_DIR = 18, // } along its bone's third matrix row
    CAMPOINT_GOLFER_WAIST_DIR = 19, // }
    CAMPOINT_GOLFER_AXIS_X = 20,    // } camera only: the shot's other point (the golfer's root, bone 0,
    CAMPOINT_GOLFER_AXIS_Z = 21,    // } when that is one of these two too) moved one unit along bone 0's
                                    //   first (20) or third (21) matrix row
    CAMPOINT_SHOT_TARGET = 22,  // look-at only: the shot's vLook, as CAMPOINT_SHOT_POINT
    CAMPOINT_BALL_AHEAD = 23,   // look-at only: ahead of the ball along its flight. The camera's
                                // tracking functions wait for a ball update for it as for CAMPOINT_BALL
    CAMPOINT_SHOT_POINT = 24,   // the shot's own points: vPos for the camera, vLook for the look-at
    CAMPOINT_WORLD_Z = 25       // camera only: (0, 0, 100)
} CameraTracking_e;

// How a dynamic shot's camera follows its two points (CamShot.nTrackMode; DynamicCam_ProcessScript).
typedef enum CameraDynamicTracking_e {
    CAMTRACK_PERCENT_FLAT = 0,  // DynamicCam_TrackPercent: share fDist of the way from nFromPoint to
                                // nToPoint, the way taken level
    CAMTRACK_PERCENT = 1,       // the same, the way as it is
    CAMTRACK_OFFSET_FLAT = 2,   // DynamicCam_TrackOffset: fDist along the level direction from
                                // nFromPoint to nToPoint
    CAMTRACK_OFFSET_FLAT_LAGGED = 3,    // the same, and the camera trails its moves
    CAMTRACK_OFFSET = 4,        // fDist along the direction as it is
    CAMTRACK_BALL_LAG = 5,      // DynamicCam_TrackBallVelocityLag: follows the ball's flight, closing in
                                // and turning round the ball a share a frame
    CAMTRACK_BALL_LAG_FLAT = 6, // the same, the flight taken level
    CAMTRACK_BALL_TIGHT = 7,    // DynamicCam_TrackBallVelocityTight: on the ball's flight each frame
    CAMTRACK_OFFSET_TO_PIN = 8, // as CAMTRACK_OFFSET, but once the ball is past a share of the way to
                                // the pin the camera moves only part of the way from where it was
    CAMTRACK_FIXED = 9          // DynamicCam_TrackFixed: at nFromPoint
} CameraDynamicTracking_e;

// The game speed while a shot plays (CamShot.nTimeMode), set when the script starts the shot
// (CameraScript_GoToNewScript, CameraScript_InterpToNewScript; not in split screen).
typedef enum CamShotTimeMode {
    SHOTTIME_HALF = 0,          // half speed (GameEffects_SetHalfTime)
    SHOTTIME_NORMAL = 1,
    SHOTTIME_DOUBLE = 2,        // double speed (GameEffects_SetDoubleTime)
    SHOTTIME_MATRIX = 3         // normal speed, the matrix camera mode on (GolfCamera_SetCameraMatrixMode)
} CamShotTimeMode;

// What a shot's camera height is taken from (CamShot.nHeightRef; DynamicCam_AddHeightOffset), the
// shot's fHeightOffset then added. The golfer's points are his character's bones.
typedef enum CameraHeightTracking_e {
    CAMHEIGHT_NONE = 0,         // the camera keeps the height its tracking gave it (fHeightOffset is not
                                // added)
    CAMHEIGHT_GOLFER_FEET = 1,  // between the golfer's feet (bones 0x39 and 0x47)
    CAMHEIGHT_GOLFER_WAIST = 2, // the golfer's waist (bone 1)
    CAMHEIGHT_GOLFER_HEAD = 3,  // the golfer's head (bone 10)
    CAMHEIGHT_ORIG_HEIGHT = 4,  // the camera's height before its tracking moved it this frame
    CAMHEIGHT_BALL = 5,         // the ball (DynamicCam_GetSmoothBallLocation)
    CAMHEIGHT_PIN = 6,          // the pin (left as it is without course data)
    CAMHEIGHT_BALL_EASED = 7    // the ball, the camera easing towards it at each ball update
} CameraHeightTracking_e;

// A camera shot (0xC0 bytes): a named script position the camera script moves to. The shots of a
// sequence or a fly-by route are chained through pNext.
typedef struct CamShot {
    char szName[0x20];          // 0x00
    f32  vPos[4];               // 0x20  its camera position ([1]: the elevator camera adds the
                                //       course's height)
    f32  vLook[4];              // 0x30  where it looks (the super zoom's target, the zoom-to-aim
                                //       camera's start, the point a static camera looks at; a fly-by
                                //       shot's look angles in radians)
    struct CamShot* pNext;      // 0x40  the shot that follows
    struct CamShot* pPrev;      // 0x44  the shot before it: the first point of the spline through
                                //       the shots around it (the fly-by routes chain it; the golf
                                //       cameras set it to the shot they started from, and camera 13
                                //       goes back to View.shot19C's)
    f32  fInterpTime;           // 0x48  how long the move into the shot takes (a fly-by shot's: the
                                //       flight on to the next one); the field of view goes from
                                //       fFovStart to fFovEnd over it
    f32  fLength;               // 0x4C  how long the shot is: a time for most shots (a golf camera
                                //       moves on once the script's fCamTime passes it), a fly-by
                                //       shot's distance along the spline to the next one
    union {
        struct {
            u32 uGolfersLow;    // 0x50  } in the front end, the CrAP screen's golfers it may show
            u32 uGolfersHigh;   // 0x54  } (DynamicCam_MatchPlayer): bit nGolferId of uGolfersLow
                                //         for ids up to 32, bit nGolferId - 32 of uGolfersHigh above
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
    f32  fFovEnd;               // 0x7C  } (over fInterpTime; in degrees in the files)
    f32  fHeightOffset;         // 0x80  added to the camera's height (DynamicCam_AddHeightOffset)
    f32  f84;                   // 0x84
    f32  fDepthOfField;         // 0x88  CamScript_RunScript hands it (plus
                                //       GameEffects_DepthOfFieldChange of it) to fn_800457B8
    f32  fBlur;                 // 0x8C  } CamScript_RunScript passes both to PostFx_SetBlur when either
    f32  f90;                   // 0x90  } is above 0, fBlur as the blur amount (the argument the golf
                                //       cameras pass fZoomBlur in)
    f32  fShakeAmount;          // 0x94  } the shoulder shake (CameraScript_CalculateShoulderShake):
    f32  fShakeSpeed;           // 0x98  } how far and how fast; none when fShakeAmount is 0
    f32  fRoll;                 // 0x9C  the camera's roll (the script's fRoll)
    s32  nTimeMode;             // 0xA0  the game speed while it plays (CamShotTimeMode)
    union {
        s32 nRoute;             // 0xA4  a fly-by shot: the route it is on (0..9)
        u32 uEvents;            //       a static camera: the events it serves
                                //       (StaticCam_EventType_e, a bit each)
    } pick;
    u8   bDynamic;              // 0xA8  played by the dynamic camera (DynamicCam_ProcessScript); 0: by
                                //       the static camera (StaticCam_ProcessScript)
    u8   bHasPrev;              // 0xA9  another shot's pNext leads here: not the first of its sequence
                                //       or route (DynamicCam_ChooseScript never picks it;
                                //       StaticCam_GetFlyByCam)
    u8   bShowGolfer;           // 0xAA  the golfer is drawn in this shot
    u8   nInterpType;           // 0xAB  how the script moves into it (CameraInterpType_e: 0 a plain
                                //       blend, 1 fixed target, 13 share of the flight, 15 share
                                //       to the pin, ...; CamScript_RunScript)
    u8   nLookAtKind;           // 0xAC  the point it looks at (CameraTracking_e; CamScript_GetLookAtPoint)
    u8   nStateType;            // 0xAD  the script state it serves (CameraShotState_e;
                                //       DynamicCam_MatchScriptStateType)
    s8   nUnread;               // 0xAE  a static camera's StaticCamFromTess_t.nUnread; never read
    u8   nFromPoint;            // 0xAF  } the two points the camera's place is measured between
    u8   nToPoint;              // 0xB0  } (CameraTracking_e; DynamicCam_GetLocation)
    u8   nTrackMode;            // 0xB1  how the camera follows them (CameraDynamicTracking_e)
    u8   nHeightRef;            // 0xB2  what the camera's height is taken from (CameraHeightTracking_e;
                                //       DynamicCam_AddHeightOffset)
    u8   unkB3[0xC0 - 0xB3];
} CamShot;
LAYOUT_ASSERT(CamShot, 0xC0);

// ---- the course's static and fly-by cameras (GoStaticCam.c) ----------------------------------
// A course's 'Cact' objects of type 201 are static cameras, those of type 200 fly-by cameras
// (UKernel.c's Kernel_DownloadActors hands them over); each becomes a CamShot. The fly-by cameras are
// chained into up to 10 routes, and a 'CAMC' stream object brings a timing curve per route.

// A type 201 object's data (UStreamObject.pData; the same 'Cact' data as DynObjDef, 8 bytes in).
typedef struct StaticCamFromTess_t {
    u8   unk0[0x10];
    f32  aPos[3];               // 0x10  -> CamShot.vPos
    s32  nUnread;               // 0x1C  -> CamShot.nUnread, never read
    s32  nLookAtKind;           // 0x20  -> CamShot.nLookAtKind
    u32  uEvents;               // 0x24  -> CamShot.pick.uEvents: the events it serves
                                //       (StaticCam_EventType_e, a bit each)
    f32  fFovStart;             // 0x28  } in degrees -> CamShot.fFovStart, fFovEnd
    f32  fFovEnd;               // 0x2C  }
    f32  fInterpTime;           // 0x30  -> CamShot.fInterpTime
    f32  fLength;               // 0x34  -> CamShot.fLength
    f32  aArea[4];              // 0x38  -> CamShot.u.aArea
    f32  aAngle[3];             // 0x48  in degrees: which way it looks (-> CamShot.vLook, turned into
                                //       the point it looks at)
    f32  fDepthOfField;         // 0x54  -> CamShot.fDepthOfField
    s32  nTimeMode;             // 0x58  -> CamShot.nTimeMode
} StaticCamFromTess_t;

// A type 200 object's data.
typedef struct FlybyCamFromTess_t {
    u8   unk0[0x10];
    f32  aPos[3];               // 0x10  -> CamShot.vPos
    f32  fFov;                  // 0x1C  in degrees -> CamShot.fFovStart
    f32  fInterpTime;           // 0x20  -> CamShot.fInterpTime
    s8   nId;                   // 0x24  its number, which other cameras' nNext name
    s8   nNext;                 // 0x25  the next camera's nId (FLYBY_END_HIDE_GOLFER, or a number no
                                //       camera has: the route ends here)
    u8   unk26[2];
    f32  aLook[3];              // 0x28  in degrees -> CamShot.vLook
    f32  fLength;               // 0x34  0..1 (clamped on load) -> CamShot.fLength, which
                                //       StaticCam_GetFlyByCam replaces on a route's cameras
    s8   nRoute;                // 0x38  its route (0..9) -> CamShot.pick.nRoute
} FlybyCamFromTess_t;

// A fly-by camera's nNext that ends its route and hides the golfer on every camera of the route.
#define FLYBY_END_HIDE_GOLFER -99

// A point or tangent of a fly-by's timing curve: x along the share of the fly-by's time, y the value.
typedef struct CurvePoint2D {
    f32  x;                     // 0x00
    f32  y;                     // 0x04
} CurvePoint2D;

// One key of a fly-by's timing curve, a Hermite segment from start to end (0x24 bytes; a 'CAMC'
// object holds them byte-swapped, 0x22 bytes each).
typedef struct FlyByKey {
    CurvePoint2D start;         // 0x00
    CurvePoint2D end;           // 0x08
    CurvePoint2D startTangent;  // 0x10
    CurvePoint2D endTangent;    // 0x18
    u8   uUnread20;             // 0x20  } copied from the file, never read
    u8   uUnread21;             // 0x21  }
    u8   unk22[2];
} FlyByKey;

#define CURVE_NUM_KEYS 20

// A fly-by route's timing curve (0x2E0 bytes), from the 'CAMC' object: how far along the route the
// camera is at each share of the flight's time. For a share, the key whose start.x..end.x holds it
// gives the value (CamScript_fEvaluateCurve).
typedef struct Curve {
    u32  uUnread;               // 0x00  never read
    u32  nRoute;                // 0x04  the route it times (CamShot.pick.nRoute; StaticCam_GetFlybyTimeCurve)
    f32  fDuration;             // 0x08  how long the flight along the route takes
    u32  nKeys;                 // 0x0C  how many of aKeys are used
    FlyByKey aKeys[CURVE_NUM_KEYS];        // 0x10
} Curve;
LAYOUT_ASSERT(Curve, 0x2E0);

#define STATICCAM_NUM_STATIC 10    // static cameras a course can have
#define STATICCAM_NUM_FLYBY  30    // fly-by cameras
#define STATICCAM_NUM_ROUTES 10    // fly-by routes
#define STATICCAM_CROWD_ROUTE 9    // the route the post-shot crowd fly-by takes; its cameras do not
                                   // show the golfer

// The moments a static camera can be picked for (StaticCam_ChooseScript; a bit each in
// CamShot.pick.uEvents). The ball's ones are asked for only for a CPU player or in a replay.
typedef enum StaticCam_EventType_e {
    STATICCAM_EVENT_BALL_IN_AIR = 0x02,     // the ball is flying and has hit nothing yet
    STATICCAM_EVENT_BALL_BOUNCED = 0x04,    // the ball is flying after hitting something
    STATICCAM_EVENT_BALL_ON_GROUND = 0x08,  // the ball is no longer flying
    STATICCAM_EVENT_PUTT = 0x10,            // the ball was putted
    STATICCAM_EVENT_PRE_SHOT = 0x20,        // before the shot (GolfCamera_InitPreShotCamera)
    STATICCAM_EVENT_POST_SHOT = 0x40        // after the shot (GolfCamera_InitPostShotCamera,
                                            // GolfCamera_CutToGolferDoneAnimatingCam)
} StaticCam_EventType_e;

// GoStaticCam.c's state (0x1E68 bytes, allocated by StaticCam_Init).
typedef struct StaticCamManager_t {
    CamShot aStatic[STATICCAM_NUM_STATIC];     // 0x0000  "Static Cam: n"
    CamShot aFlyBy[STATICCAM_NUM_FLYBY];       // 0x0780  "FlyBy Cam: n"
    s32  nFlyBy;                // 0x1E00  how many of aFlyBy are loaded
    s32  nStatic;               // 0x1E04  how many of aStatic are loaded
    u8   bLinked;               // 0x1E08  the fly-by routes are chained (StaticCam_GetFlyByCam)
    f32  afRouteLength[STATICCAM_NUM_ROUTES];  // 0x1E0C  each route's length, its shots' fLength
                                               //         added up
    CamShot* apRoute[STATICCAM_NUM_ROUTES];    // 0x1E34  each route's first shot
    u32  uUnread;               // 0x1E5C  the 'CAMC' object's first word, never read
    u32  nCurves;               // 0x1E60  how many curves pCurves holds
    Curve* pCurves;             // 0x1E64  the routes' timing curves
} StaticCamManager_t;
LAYOUT_ASSERT(StaticCamManager_t, 0x1E68);

extern StaticCamManager_t* gpStaticCams;

// The clubs a camera sequence is for (CameraSequence_t.nClubType; DynamicCam_MatchSeqClub). The
// woods are the drivers and the 3, 5 and 7 woods, the wedges the pitching wedge to the high lob wedge.
typedef enum CameraClubType_e {
    CAMCLUB_ANY = 0,
    CAMCLUB_WOODS = 1,
    CAMCLUB_IRONS_5_TO_9 = 2,
    CAMCLUB_IRONS_1_TO_5 = 3,
    CAMCLUB_WEDGES = 4,
    CAMCLUB_PUTTER = 5,
    CAMCLUB_WOODS_AND_IRONS = 6,
    CAMCLUB_WOODS_TO_5_IRON = 7,
    CAMCLUB_ALL_BUT_PUTTER = 8,
    CAMCLUB_5_IRON_TO_WEDGES = 9,
    CAMCLUB_IRONS_AND_WEDGES = 10
} CameraClubType_e;

// The lies a camera sequence tells apart (a bit each in CameraSequence_t.uStartLies and uEndLies):
// the ball's lies (DynamicCam_BallLieToCameraLie) and the surface classes
// (DynamicCam_MaterialToCameraLie) fall into these.
typedef enum CameraLie_e {
    CAMLIE_NONE = 0,            // surface class 0, or one past 19
    CAMLIE_TEE = 1,             // the tee (a ball on it, or surface class 1 under a ball on LIE_TEE_e)
    CAMLIE_FAIRWAY = 2,         // the fairway and the fringe
    CAMLIE_ROUGH = 3,           // the roughs, the cart path, trees and the other surface classes
    CAMLIE_SAND = 4,            // the bunkers
    CAMLIE_WATER = 5,           // water, ice and snow
    CAMLIE_GREEN = 6,           // the green and the cup
    CAMLIE_OUT_OF_BOUNDS = 7    // out of bounds, and surface class 19 (not playable)
} CameraLie_e;

// The shots a camera sequence is for (CameraSequence_t.nShotType; DynamicCam_MatchSeqShot), by the
// player's shot kind (Player.nShotKind, ShotType_t). The generic types fit only when the pick allows
// them (DynamicCam_ChooseSequence's allowGenericShotTypes). Type 9 fits no shot.
typedef enum CameraShotType_e {
    CAMSHOT_GENERIC = 0,        // generic: any shot
    CAMSHOT_GENERIC_TEE = 1,    // generic: a ball on the tee (LIE_TEE_e)
    CAMSHOT_DRIVE = 2,          // SHOT_TYPE_DRIVE_e
    CAMSHOT_CHIP = 3,           // SHOT_TYPE_CHIP_e
    CAMSHOT_PITCH = 4,          // SHOT_TYPE_PITCH_e
    CAMSHOT_PUNCH = 5,          // SHOT_TYPE_PUNCH_e
    CAMSHOT_FLOP = 6,           // SHOT_TYPE_FLOP_e (the ball-flight camera keeps one for such a shot,
                                // GolfCamera_InitBallFlightCamera)
    CAMSHOT_SHOT_KIND_6 = 7,    // } shot kinds 6 and 7
    CAMSHOT_SHOT_KIND_7 = 8,    // }
    CAMSHOT_ANY_PUTT = 10,      // SHOT_TYPE_PUTT_e
    CAMSHOT_ANY_DRIVE = 11,     // SHOT_TYPE_DRIVE_e, as CAMSHOT_DRIVE
    CAMSHOT_NOT_PUTT_OR_DRIVE = 12, // any shot kind but those two
    CAMSHOT_GENERIC_OFF_TEE = 13    // generic: a ball not on the tee
} CameraShotType_e;

// Who a camera sequence is for (CameraSequence_t.nPlayerType; DynamicCam_MatchPlayerType).
typedef enum CamSeqPlayerType {
    CAMPLAYER_HUMAN = 0,        // a human player's shot, not in a replay
    CAMPLAYER_CPU = 1,          // a CPU player's shot, not in a replay
    CAMPLAYER_LIVE = 2,         // any shot not in a replay
    CAMPLAYER_REPLAY = 3,       // any shot in a replay
    CAMPLAYER_REPLAY_OR_CPU = 4 // a shot in a replay or a CPU player's
} CamSeqPlayerType;

// The game modes and screens a camera sequence is for (CameraSequence_t.nModeType;
// DynamicCam_ModeType). The game modes are GM_GetModeType's: GM_Practice_mode and
// GM_Lessons_mode.
typedef enum CameraGameModeTypes_e {
    CAMGAMEMODE_SINGLE_VIEW = 0,    // one view, outside practice, the lessons and the skill zone
                                    // (GM_Currently_SkillZoneMode)
    CAMGAMEMODE_SPLIT_OR_TRAINING = 1,  // split screen, practice or the lessons
    CAMGAMEMODE_SKILL_ZONE = 2      // the skill zone
} CameraGameModeTypes_e;

// A camera sequence (0x50 bytes, from the 'CAMS' file; GoDynamicCam.c): the shots a camera plan
// steps through, and the conditions it is picked on. Its shot choices (CameraEventTrigger_t) are in
// dyncam.h.
typedef struct CameraSequence_t {
    char szName[0x20];          // 0x00  "DEF..." for a default sequence (DynamicCam_IsDefualtSeq)
    struct CameraSequence_t* pNext; // 0x20  the sequence that follows (an index in the file; the
                                    //       sequence itself when that one has no shot choices)
    f32  fMinDist;              // 0x24  } the distance it is picked for: the shot's
    f32  fMaxDist;              // 0x28  } (DynamicCam_ChooseSequence), the ball's to the pin for the
                                //         pre-flight cameras (DynamicCam_MatchDistToPin)
    f32  fMinPinHeight;         // 0x2C  } the pin's height over the ball it is picked for
    f32  fMaxPinHeight;         // 0x30  } (DynamicCam_MatchHeightDiff)
    f32  fWeight;               // 0x34  its weight when several fit (DynamicCam_ChooseSequence)
    f32  fLength;               // 0x38  how long the golf cameras hold a shot they build by hand while
                                //       it is the view's sequence (its fLength; GoGolfCam.c)
    s32  nChoices;              // 0x3C  how many shot choices pChoices holds
    u32  uCourses;              // 0x40  the courses it is used on, a bit each (DynamicCam_MatchCourse)
    u8   nStateType;            // 0x44  the script state it serves (CameraShotState_e;
                                //       DynamicCam_MatchStateType)
    u8   nClubType;             // 0x45  the clubs it is for (CameraClubType_e; DynamicCam_MatchSeqClub)
    u8   nShotType;             // 0x46  the shots it is for (CameraShotType_e; DynamicCam_MatchSeqShot)
    u8   nPlayerType;           // 0x47  who it is for (CamSeqPlayerType; DynamicCam_MatchPlayerType);
                                //       the swing camera cuts into its shot (CAMINTERP_CUT, time 0) when the
                                //       view's sequence before it is not CAMPLAYER_HUMAN
    u8   nModeType;             // 0x48  the game modes it is for (CameraGameModeTypes_e;
                                //       DynamicCam_ModeType)
    u8   uStartLies;            // 0x49  } camera lies (CameraLie_e), a bit each (DynamicCam_MatchLies):
    u8   uEndLies;              // 0x4A  } the ball's lie, and the surface class the shot ends on
                                //         (DynamicCam_ChooseSequence)
    s8   uPars;                 // 0x4B  the holes' pars it is used on, a bit each (DynamicCam_MatchPars)
    struct CameraEventTrigger_t* pChoices; // 0x4C  its shot choices (dyncam.h)
} CameraSequence_t;
LAYOUT_ASSERT(CameraSequence_t, 0x50);

// The colour fade over a view (CameraScriptSettings_t.nFade), run by CamScript_Fade.
typedef enum CameraFadeType_e {
    CAMFADE_NONE     = 0,
    CAMFADE_OUT      = 1,       // the colour's alpha rising from 0 (CameraController_FadeOut)
    CAMFADE_IN       = 2,       // falling to 0 (CameraController_FadeIn)
    CAMFADE_HOLD     = 3,       // the colour held until another fade starts
                                // (CameraController_HoldFadeColor)
    CAMFADE_OUT_DONE = 4,       // } a fade out or in just ended; CamScript_Fade's next step
    CAMFADE_IN_DONE  = 5        // } ends the fade (CAMFADE_NONE)
} CameraFadeType_e;

// A view's camera script (0x118 bytes at View + 0x84): the shot the camera plays, the one after
// it, and the move it makes between them. The camera script functions (gocamscripts.c) and the
// dynamic camera's (GoDynamicCam.c) take it; every caller passes &pView->script.
typedef struct CameraScriptSettings_t {
    f32  vCamPos[4];            // 0x00  the current shot's camera position (camera 4 keeps the
                                //       ball here)
    f32  vNextCamPos[4];        // 0x10  the next shot's, blended to (camera 4 keeps the pin here)
    f32  aLookAt[8];            // 0x20  where the camera looks: [0..3] for the current shot,
                                //       [4..7] for the next (CamScript_GetLookAtPoint)
    f32  vFadeColor[4];         // 0x40  the screen fade's colour, its alpha [3] the most it reaches
    f32  vLandingEstimate[4];   // 0x50  the ball-flight camera: where the shot should land (the aim, at
                                //       the club's full distance)
    f32  v60[4];                // 0x60  CamScript_RunScript: how far the camera moved this frame (0 when
                                //       CameraScript_SnapToScript holds)
    f32  vLastBallPos[4];       // 0x70  the ball's position as CamScript_RunScript last kept it
                                //       (DynamicCam_GetSmoothBallLocation with set): it stands in for
                                //       the ball by the pin, and the ball-following look-at and camera
                                //       height step from it towards the ball (CameraScript_LagBallFlight,
                                //       DynamicCam_AddHeightOffset); (0, 0, 0, 1) when the view is set
                                //       up (CameraController_InitOneCamera)
    f32  fCamTime;              // 0x80  time on this camera
    f32  fPrevCamTime;          // 0x84  fCamTime before this frame's step (CamScript_RunScript)
    f32  fCutEaseTime;          // 0x88  a second clock, stepped with fCamTime: the time since the move
                                //       out of the default swing camera (interp type 4), over which
                                //       the following cameras ease in (the tuning's fCutEaseInTime and
                                //       fCutHeightEaseTime); most other moves start it at
                                //       fCutHeightEaseTime, past the ease (CameraScript_GoToNewScript,
                                //       CameraScript_InterpToNewScript)
    f32  fInterpTime;           // 0x8C  how long the move into the next shot takes (its fInterpTime;
                                //       CameraController_StartScriptOfKind): the script moves on once
                                //       fCamTime passes it
    f32  fFadeTime;             // 0x90  the fade's time so far (stepped by the frame time)
    f32  fFadeLength;           // 0x94  and its length
    f32  fScriptTime;           // 0x98  time on the current script (0 when a new one starts; TW07's
                                //       scriptTime)
    f32  fPrevScriptTime;       // 0x9C  fScriptTime before this frame's step: CamScript_GetLookAtPoint
                                //       takes the shoulder shake at it off, and adds the one at fScriptTime
    f32  fSegmentShare;         // 0xA0  } a fly-by: the share of the current shot's spline segment and
    f32  fFlyByDist;            // 0xA4  } the distance flown so far (GoStaticCam.c)
    f32  fRoll;                 // 0xA8  the camera's roll: the current shot's fRoll, blended
                                //       (CamScript_RunScript)
    CamShot* pShot;             // 0xAC  the current shot
    CamShot* pNextShot;         // 0xB0  the next one (the current shot's pNext;
                                //       CameraController_StartScriptOfKind)
    CamShot* pRecordShot;       // 0xB4  the shot the live camera is recorded into to blend from
                                //       (CameraScript_RecordCurrentCam): the view's shot19C
                                //       (CameraController_InitOneCamera); CameraScript_InterpToNewScript
                                //       and GolfCamera_SwitchCrAPCamera record into it
    CamShot* pB8;               // 0xB8  the shot before (GolfCamera_CutToGolferDoneAnimatingCam)
    s32  nInterpType;           // 0xBC  how the script blends into the next shot (its
                                //       nInterpType; CameraController_StartScriptOfKind)
    s32  nFade;                 // 0xC0  the colour fade to vFadeColor over the view (CameraFadeType_e)
    s32  nRequestedEvent;       // 0xC4  the camera event asked for (CameraController_StartScriptOfKind)
    s32  nC8;                   // 0xC8  the shot kind last started
    u8   bBlendPending;         // 0xCC  a blend into a new shot started and has not stepped a frame yet
    u8   bCD;                   // 0xCD  set for a cut (interp type 5) to a shot looking at the ball;
                                //       nothing reads it
    u8   bCE;                   // 0xCE  set by every new script, never cleared in this build
    u8   bFairwayFix;           // 0xCF  the camera was put back on the fairway (CAMPOINT_FAIRWAY_CAM,
                                //       DynamicCam_GetLocation; CamScript_PutBackOnFairway)
    s32  nArcDir;               // 0xD0  the way round an arced blend swings, a CameraArcMode_e
                                //       (CamUtils_vCalcArcPosition)
    f32  fD4;                   // 0xD4  the camera's speed when the script moves to the next shot
                                //       (CamScript_RunScript)
    f32  fGroundHeight;         // 0xD8  the ground height the camera's heights are kept over: measured
                                //       by DynamicCam_ProcessScript as a dynamic shot starts, eased by
                                //       CamScript_SmoothTerrainHeight, set by some golf cameras
    f32  fLagAngle;             // 0xDC  how far the look direction may trail its target
                                //       (CameraScript_KeepPointInView)
    s32  nTriggerKind;          // 0xE0  } the camera event (CameraEvent_e) asked for once fScriptTime
    f32  fTriggerTime;          // 0xE4  } passes fTriggerTime: a shot choice's time trigger
                                //         (DynamicCam_ChooseScriptInSequence; CAMEVENT_NONE: none)
    u8   bNoGround;             // 0xE8  no ground was found under the camera
                                //       (CamScript_SmoothTerrainHeight)
    u8   unkE9[0xEC - 0xE9];
    f32  fBallUpdates;          // 0xEC  the ball updates per frame, eased towards
                                //       GameEffects_BallUpdatesThisFrame by the tuning's
                                //       fBallUpdatesStep
    f32  fShakeTime;            // 0xF0  } the camera shake: time left and how far
    f32  fShakeAmount;          // 0xF4  } (CameraController_SetShakeAmount)
    f32  fBlendShare;           // 0xF8  the blend's share so far (never goes back; 1 ends interp
                                //       types 13 and 15)
    u8   unkFC[0x100 - 0xFC];
    f32  fMaxBallToPin;         // 0x100  CAMTRACK_OFFSET_TO_PIN: the largest CamScript_GetBallToPinPercent
                                //        so far
    f32  fRiseHeight;           // 0x104  the height over fGroundHeight a CAMHEIGHT_BALL_EASED camera aimed
                                //        for at the ball's last update before the top of its flight
                                //        (the shot's fMinHeight on its first frame); on the way down
                                //        DynamicCam_AddHeightOffset eases the camera against it
    f32  f108;                  // 0x108
    f32  f10C;                  // 0x10C
    s32  n110;                  // 0x110
    s32  n114;                  // 0x114
} CameraScriptSettings_t;
// CameraScript_WillGolferBeOccludedInThisView copies 0x118 bytes.
LAYOUT_ASSERT(CameraScriptSettings_t, 0x118);

// A camera controller's camera mode (View.nCurCamera, set by CameraController_SetCameraMode): one
// of the golf cameras of GoGolfCam.c, numbered as CameraController_Idle switches on them.
typedef enum CameraMode_t {
    CAMMODE_GREEN_ZOOM_TO_AIM = 2,      // the zoom-to-aim camera on the green
    CAMMODE_GREEN             = 4       // the green camera: the world scaled by View.vViewScale
                                        // around View.vViewOffset
} CameraMode_t;

// A view's camera controller (CameraController_SetCameraMode, EA's name in TW06 and TW07): the
// camera mode, its shots and script. It sits at +4 in a ViewController_t; only the fields read so
// far.
typedef struct View {
    f32      vOrigin[4];        // 0x000  the camera's position (CameraController_GetCameraOrigin)
    f32      vLookPoint[4];     // 0x010  where it looks (CameraController_GetCameraLookPoint; the
                                //        pin, for camera 5); while a script shot aims by angles
                                //        (CameraController_IsFlybyDone false), the yaw, pitch and
                                //        roll instead
    f32      vSide[4];          // 0x020  the camera's side vector
                                //        (CameraController_ComputeCurrentSideVector)
    f32      v30[4];            // 0x030
    f32      v40[4];            // 0x040
    f32      vViewScale[3];     // 0x050  } the green camera's world scale and offset
    f32      vViewOffset[3];    // 0x05C  } (Camera_SetCameraPositionAndTargetWithOffsetAndScale)
    f32      fFade;             // 0x068  how far the view has faded (the ball-in-flight state waits for 0.5)
    u8       bFade;             // 0x06C  the fade is on
    u8       unk6D[3];
    s32      nCurCamera;        // 0x070  the camera mode (CameraMode_t)
    CameraSequence_t* p74;      // 0x074  the camera sequence the shots are picked from
    CameraSequence_t* p78;      // 0x078  the sequence before the post-shot cameras (p74 saved)
    CameraSequence_t* p7C;      // 0x07C  the swing camera's sequence, kept for the replay
    CamShot* p80;               // 0x080
    CameraScriptSettings_t script;           // 0x084  the camera script the camera functions drive
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

#define VIEWCONTROLLER_NUM_VIEWS 4      // gViewControllers: the views on screen at most
#define VIEWCONTROLLER_NO_PLAYER 5      // nPlayer from ViewController_Init until a player is set:
                                        // one past the five player slots

// One of the views on screen (gViewControllers, 0x288 bytes each; Player.nView[] indexes them):
// the render context it draws with, the view's camera controller, the player it follows.
typedef struct ViewController_t {
    void*    pRenderContext;    // 0x000  (ViewController_GetRenderContext)
    View     cameraController;  // 0x004  (ViewController_GetCameraControl)
    s32      nPlayer;           // 0x270  the player the view follows
                                //        (ViewController_SetActivePlayerNumber)
    u8       bActive;           // 0x274  the view is in use: set by ViewController_Init, cleared by
                                //        ViewController_Delete, either by
                                //        ViewController_TurnOnViewController
    u8       bFlagOut;          // 0x275  the flagstick is out (set while every player on this view
                                //        is on the green, the fringe or holed): no pin collisions
    u8       unk276[2];
    f32      fSavedHeight;      // 0x278  } the viewport rectangle ViewController_SaveViewportRect
    f32      fSavedWidth;       // 0x27C  } keeps and ViewController_RestoreViewportRect puts back
    f32      fSavedTop;         // 0x280  }
    f32      fSavedLeft;        // 0x284  }
} ViewController_t;
LAYOUT_ASSERT(ViewController_t, 0x288);

// ---- the camera tuning values (GoCamTuningVars.c) -------------------------------------------

// The camera tuning values: fixed numbers the cameras read (speeds, distances, blend times, angles
// in radians unless marked), allocated and set by CameraTuning_Init. Fields marked "never read" are
// set there and read by no code.
typedef struct sCameraTuningVars {
    f32  fZoomToAimSpeed;       // 0x000  the zoom-to-aim camera's base speed (times the square root
                                //        of the distance left, when that is over 1)
    f32  fZoomToAimBack;        // 0x004  ... its distance back from the target (not on a putt)
    f32  fZoomToAimRampDist;    // 0x008  ... it speeds up over this first distance (0: no ramp)
    f32  fZoomToAimNearRange;   // 0x00C  ... a target closer to the ball than fZoomToAimBack plus this
                                //        brings the camera in towards fZoomToAimNearShare of the
                                //        ball's distance to it
    f32  fZoomToAimNearShare;   // 0x010
    f32  fZoomToAimHeight;      // 0x014  ... its height over the ground at the target, at the end
    f32  fZoomToAimLag;         // 0x018  ... once there it closes 1/this of the way a frame; also the
                                //        aim marker's lag (frames) at the end of the move
    f32  fZoomBlur;             // 0x01C  both zoom-to-aim cameras' motion blur, times 1 - base speed /
                                //        speed
    f32  fZoomToAimWaitBlur;    // 0x020  the zoom-to-aim camera's motion blur before it sets off
    f32  fZoomArriveDist;       // 0x024  both: the camera has arrived within this of its goal
    f32  fZoomToAimStartLag;    // 0x028  the zoom-to-aim camera's aim-marker lag (frames) at the start
                                //        of the move, blending to fZoomToAimLag at its end
    f32  fZoomToAimHeightShare; // 0x02C  the zoom-to-aim camera, once there: its height's share of the
                                //        way a frame
    f32  fGreenZoomRampDist;    // 0x030  the green zoom-to-aim camera speeds up over this first distance
    f32  fGreenZoomSpeed;       // 0x034  ... its base speed
    f32  fZoomToAimPuttBack;    // 0x038  both zoom-to-aim cameras' distance back from the target on a putt
    f32  fGreenZoomLookDrop;    // 0x03C  the green zoom-to-aim camera: shot19C.fLookUpOffset falls by
                                //        this a frame
    f32  fGreenZoomHeightStep;  // 0x040  ... once there, its height moves by this a frame
    f32  fGreenZoomHeight;      // 0x044  ... its height over shot19C.fMinHeight (over the lens's fov
                                //        scale; more when it needs it to see the pin)
    f32  fGreenZoomLag;         // 0x048  ... once there it closes 1/this of the way a frame; the aim
                                //        marker's lag (frames)
    f32  fGreenRollHeight;      // 0x04C  the putt preview camera's height over the ghost ball
    f32  fGreenRollDist;        // 0x050  ... how far out it starts (scaled down for a ball closer
                                //        than this to the pin) ...
    f32  fGreenRollTime;        // 0x054  ... and how long it takes to close in
    f32  fReturnBlendTime;      // 0x058  the matrix and super zoom cameras: script.fInterpTime for the
                                //        shot from before, when it is queued again
    f32  fMatrixBlendTime;      // 0x05C  the matrix camera's first script.fInterpTime (twice this for
                                //        special swings other than 1 and 6)
    f32  fMatrixStepTime;       // 0x060  the matrix camera steps its script by this once every this
                                //        many seconds (0: every frame)
    f32  fMatrixBlur;           // 0x064  the matrix camera's motion blur
    f32  vReplayTint[4];        // 0x068  the swing replay camera's tint over the view (PostFx_SetFade);
                                //        its alpha [3] fades out ...
    f32  fReplayTintTime;       // 0x078  ... over this many seconds
    f32  fSuperZoomStartFov;    // 0x07C  the super zoom's field of view at the start ...
    f32  fSuperZoomEndFov;      // 0x080  ... and at the end (GolfCamera_CreateSuperZoomCamera sets it
                                //        to the view's less the letterbox's change)
    f32  fSuperZoomBlendTime;   // 0x084  ... its blend from one to the other
    f32  fSuperZoomBlur;        // 0x088  ... its motion blur
    f32  fSuperZoomShake;       // 0x08C  ... its screen shake
    f32  fBallLowEventHeight;   // 0x090  camera event 2 is posted while the falling ball is below
                                //        this height, before its first bounce
    f32  fElevatorBlendTime;    // 0x094  the elevator camera's blend
    f32  fHeightKeepShare;      // 0x098  the ball-placement and speed-golf run cameras: the share of
                                //        the height difference kept a frame
    f32  fSwingCamMinPower;     // 0x09C  the special swing camera (GolfCamera_ChooseSpecialSwing): the
                                //        least shot power for one; above it the chance (percent) is
                                //        fSwingCamLowChance ...
    f32  fSwingCamMidPower;     // 0x0A0  ... above this, fSwingCamMidChance ...
    f32  fSwingCamHighPower;    // 0x0A4  ... above this, fSwingCamHighChance
    f32  fSwingCamLowChance;    // 0x0A8
    f32  fSwingCamMidChance;    // 0x0AC
    f32  fSwingCamHighChance;   // 0x0B0
    f32  fSwingCamShortClubPower; // 0x0B4  ... above this power with a short club, always kind 11
    f32  fPostShotSandWait;     // 0x0B8  the post-shot camera waits this long on a ball in a bunker
                                //        (or for camera event 10)
    s32  nBeats;                // 0x0BC  the heartbeat camera's beats
    s32  nBeatFrames;           // 0x0C0  ... steps per beat
    f32  fBeatFadeInTime;       // 0x0C4  ... each beat's fade in
    f32  fBeatFadeOutTime;      // 0x0C8  ... fade out
    f32  fBeatHoldTime;         // 0x0CC  ... and hold before the next cut
    f32  fAboveMaxHeightShare;  // 0x0D0  CameraScript_GetBallHeightWithMaxHeight: the aim comes down
                                //        by this share of how far the ball is above a shot's fMaxHeight
    f32  fScriptEaseInTime;     // 0x0D4  the aim and the camera's height ease in over this much of a
                                //        script's time (CameraScript_LagBallFlight,
                                //        DynamicCam_AddHeightOffset)
    f32  fAimMarkerLag;         // 0x0D8  a shot looking at the aim marker: its lag (frames)
                                //        (CameraScript_LagAimMarker) ...
    f32  fAimMarkerHeightShare; // 0x0DC  ... and its height's share of the way a frame (also the
                                //        zoom-to-aim cameras')
    f32  fGroundHeightShare;    // 0x0E0  CamScript_SmoothTerrainHeight: script.fGroundHeight's share of
                                //        the way to the ground height a frame
    f32  fBallLagAngle;         // 0x0E4  the most the look may trail the ball (radians; also put in
                                //        script.fLagAngle)
    f32  fGolferLagAngle;       // 0x0E8  ... or a bone of the golfer
    f32  fOutOfBoundsDelay;     // 0x0EC  CamScript_CheckOutOfBounds: the camera time before it checks
                                //        the hole's outline
    f32  fAimMarkerMinDist;     // 0x0F0  CameraScript_LagAimMarker: an aim closer than this (over the
                                //        lens's fov scale, times the drop to it) is pushed out to it
    f32  fFairwaySpotMaxDot;    // 0x0F4  CamScript_GetCameraOnFairwayPos: a spot is taken when the dot
                                //        product of its level direction to the ball with the camera's
                                //        is below this
    f32  fUnread1;              // 0x0F8  never read
    f32  fDefaultSwingCamMinDot; // 0x0FC  CameraScript_IsDefaultSwingCam: the least dot product of the
                                //        camera's and the aim's level directions from the ball
    f32  fFairwayCamMoveDist;   // 0x100  the fairway camera (CamScript_UpdateFairwayCam) moves on only
                                //        from this level distance to the spot by the ball ...
    f32  fFairwayCamMinTime;    // 0x104  ... after this long on the camera ...
    f32  fFairwayCamMaxDot;     // 0x108  ... and while the ball heads away from the camera (a dot
                                //        product at most this)
    f32  fFairwayCamHeight;     // 0x10C  ... its height over the ground at the spot
    f32  fFairwayCamMinFov;     // 0x110  ... its field of view narrows down to this ...
    f32  fFairwayCamFov;        // 0x114  ... from this one (CamScript_PutBackOnFairway) ...
    f32  fFairwayCamFovStep;    // 0x118  ... by this a frame
    f32  fShoulderShakeScale;   // 0x11C  CameraScript_CalculateShoulderShake: a shot's fShakeAmount over
                                //        this is its wobble's size
    f32  fFlagClearDist;        // 0x120  with the flagstick in, a camera closer to the pin than this
                                //        (level, times the lens's fov scale) ...
    f32  fFlagClearHeight;      // 0x124  ... and less than this above it is in the way
                                //        (CamScript_CheckObstructedCamera) or raised
                                //        (CamScript_CheckFlagCollision)
    f32  fSteepAimMinDist;      // 0x128  the fairway fix: the aim may drop 1 in 5 to the camera at most,
                                //        fully from this level distance out
    f32  fAimMarkerMaxDrop;     // 0x12C  CameraScript_LagAimMarker: the most the aim may sit below the
                                //        camera
    f32  fPlayableHeadroom;     // 0x130  CamScript_GuessBestPlayableHeight: the room a camera needs over
                                //        a ground layer
    f32  fLookNearScale;        // 0x134  CameraScript_LagTargetPoint: the look-at point eases in slower
                                //        within this times the (fov scaled) camera distance
    f32  fBallLagMaxShare;      // 0x138  CameraScript_LagBallFlight: the aim's most share of the way a
                                //        step ...
    f32  fBallLagFullDist;      // 0x13C  ... reached at this distance from it
    f32  fBallLagLowShare;      // 0x140  ... its height share for a falling ball near the ground
    f32  fLookLevelShare;       // 0x144  CameraScript_LagTargetPoint: the look-at point's level share
                                //        of the way a frame ...
    f32  fLookHeightShare;      // 0x148  ... and its height's
    f32  fTrackCloseRate;       // 0x14C  DynamicCam_TrackBallVelocityLag: how fast a following camera
                                //        closes the distance ...
    f32  fTrackTurnRate;        // 0x150  ... and the angle to its target, per 60th of a second
    f32  fTrackEaseInTime;      // 0x154  ... while the script's time is under this, both are scaled
                                //        by ((this - camera time) / this) squared, over fTrackCloseRate
    f32  fCutEaseInTime;        // 0x158  the aim and a following camera ease in over this much of
                                //        script.fCutEaseTime ...
    f32  fCutHeightEaseTime;    // 0x15C  ... and their height over this much
                                //        (CameraScript_InterpToNewScript starts script.fCutEaseTime at
                                //        this, or at 0 from the default swing camera)
    f32  fCutHeightEasePower;   // 0x160  ... the power of that height ease-in
    f32  fGroundLookHeightShare; // 0x164  a shot looking at the ground under the ball: its height's
                                //        share of the way, each frame with ball updates
    f32  fGroundClearance;      // 0x168  the cameras' least height over the ground
    f32  fBallObstructionDist;  // 0x16C  an object or hazard this close to the ball stops the pre-shot
                                //        animation (GM_DoPreshotAnimation) and turns the swing camera's
                                //        pre-flight sequence into CAMSTATE_SWING_OBSTRUCTED's
    f32  fFadeTime;             // 0x170  the colour fades out at the end of a shot (GameMode.c,
                                //        stateFunc.c) and into the scorecard and tutorial cameras; the
                                //        golfer's animation counts as almost done within this of its end
    f32  fHeightCeilingEase;    // 0x174  DynamicCam_ProcessScript: a camera eases into its most height
                                //        over this band
    f32  fFlyByFadeTime;        // 0x178  the hole fly-by fades to vFlyByFadeColor when this much of a
                                //        group's last shot is left, and fades over this into a curve's
                                //        next shot
    f32  vFlyByFadeColor[4];    // 0x17C
    f32  fAboveMaxDistShare;    // 0x18C  the ball-flight cameras (DynamicCam_TrackBallVelocityLag and
                                //        DynamicCam_TrackBallVelocityTight): the shot's fDist is lowered
                                //        by this share of the ball's height above its fMaxHeight ...
    f32  fBelowMinDistShare;    // 0x190  ... or by this share of its height below its fMinHeight
    f32  fRiseStep;             // 0x194  DynamicCam_ProcessScript: how far a camera below its least
                                //        height rises a frame (CAMTRACK_BALL_LAG..CAMTRACK_BALL_TIGHT)
    f32  fTrackMinBallSpeed;    // 0x198  for a ball slower than this a following camera slows down
                                //        (DynamicCam_TrackBallVelocityLag) or stops
                                //        (DynamicCam_TrackBallVelocityTight)
    f32  fMaxFlightTilt;        // 0x19C  DynamicCam_ClampBallVelocity: the most the ball's flight
                                //        direction may tilt from level
    f32  fUnread2;              // 0x1A0  never read
    f32  fObstructionMaxSlope;  // 0x1A4  DynamicCam_ChoosePreFlightSequence: the obstruction test's slope
    f32  fBallUpdatesStep;      // 0x1A8  CamScript_RunScript: how fast script.fBallUpdates follows the
                                //        ball's updates per frame
    f32  fUnread3;              // 0x1AC  never read
    f32  fUnread4;              // 0x1B0  never read
    f32  fCollideMinMoveCos;    // 0x1B4  CameraController_CameraCollision, level directions: the least
                                //        cosine from the camera's motion ...
    f32  fCollideMinLookCos;    // 0x1B8  ... and from its look to the object ...
    f32  fCollideMaxMove;       // 0x1BC  ... and the most the camera may move in a frame
    s32  bSteepSlopeCam;        // 0x1C0  the elevator state may use the steep-slope camera
    s32  nUnread1;              // 0x1C4  never read
    s32  bCheckSlope;           // 0x1C8  GolfCamera_NeedSteepSlopeCam tests the slope to the target
                                //        (GolfCamera_SteepSlopeCamCheckSlope)
    s32  bCheckTerrain;         // 0x1CC  ... and the ground in between
                                //        (GolfCamera_SteepSlopeCamCheckCollision)
    f32  fSteepSlopeHeightStep; // 0x1D0  the steep-slope camera: height step per try (down going up,
                                //        up going down)
    f32  fSteepSlopeBackStep;   // 0x1D4  ... distance step back per try
    f32  fSteepSlopeBack;       // 0x1D8  ... first distance back
    s32  nSteepSlopeMaxTries;   // 0x1DC  ... most tries
    f32  fSlopeUp;              // 0x1E0  ... needed for a slope up to the target at least this steep
                                //        (rise over run) ...
    f32  fSlopeDown;            // 0x1E4  ... or down to it at least this steep
    f32  fSteepSlopeOffsetX;    // 0x1E8  ... its base: x offset from the ball ...
    f32  fSteepSlopeHeight;     // 0x1EC  ... and height over it
    f32  fSteepSlopeMaxCamMove; // 0x1F0  ... how far the camera may move per call
    f32  fSteepSlopeMaxAimMove; // 0x1F4  ... how far the aim may move per call
    f32  fMaxPitchUp;           // 0x1F8  GolfCamera_ClampLookAngle: the steepest camera angle above
                                //        the horizontal (degrees)
    f32  fMaxPitchDown;         // 0x1FC  ... and below it
    f32  fSwingShakeAmount;     // 0x200  the camera shake CameraController_Idle starts on the swing's
                                //        events 5..14 ...
    f32  fSwingShakeTime;       // 0x204  ... and how long it lasts
    f32  fScreenDarkenAlpha;    // 0x208  the alpha of the black cover fn_80039358 draws, times the
                                //        view's share (lbl_801D5010, set by the green camera)
    f32  fGreenCamMaxScale;     // 0x20C  the green camera: the most View.vViewScale[1] grows to
    f32  fGreenCamMoveInSpeed;  // 0x210  ... how fast (per second) it moves in from its start
    f32  fGreenCamScaleSpeed;   // 0x214  ... how fast View.vViewScale[1] grows and shrinks
    f32  fGreenCamOrbitSpeed;   // 0x218  ... how fast (per second) buttons 0x31/0x32 turn it round
    f32  fGreenCamArcSide;      // 0x21C  ... the side offset of its swing between the ball and the pin
    f32  fGreenCamClearance;    // 0x220  ... its ground clearance, and its least height over the ball
    f32  fGreenCamAimRaise;     // 0x224  ... (fGreenCamMaxScale - 1) times this raises the aim each
                                //        frame ...
    s32  bGreenCamLevelAim;     // 0x228  ... unless this is set: then the aim is at the camera's height
    f32  fRisingHeightShare;    // 0x22C  DynamicCam_AddHeightOffset: the share of the way to the new
                                //        height a ball step while the ball rises ...
    f32  fFallingHeightShare;   // 0x230  ... and near the shot's fMinHeight on the way down
    f32  fUnread5;              // 0x234  never read
    f32  fUnread6;              // 0x238  never read
    f32  fTrackOffsetStartShare; // 0x23C  CAMTRACK_OFFSET_TO_PIN: past this share of the way to
                                //        the pin (CamScript_GetBallToPinPercent) the camera's offset is
                                //        scaled from fTrackOffsetStartScale ...
    f32  fTrackOffsetStartScale; // 0x240
    f32  fTrackOffsetEndShare;  // 0x244  ... down to fTrackOffsetEndScale at this share and beyond
    f32  fTrackOffsetEndScale;  // 0x248
    s32  nUnread2;              // 0x24C  never read
    f32  fUnread7;              // 0x250  never read
    f32  fUnread8;              // 0x254  never read
    f32  fUnread9;              // 0x258  never read
} sCameraTuningVars;
LAYOUT_ASSERT(sCameraTuningVars, 0x25C);

extern sCameraTuningVars* gpCamTuning;

void CameraTuning_Init(void);
void CameraTuning_Close(void);

// ---- the golf cameras' shared state (GoGolfCam.c) -------------------------------------------

// The golf cameras' shared state (0x200 bytes, allocated by GolfCamera_Init).
typedef struct GolfCamState {
    f32     fElevatorHeight[21];    // 0x000  per course, added to the elevator shot's height
    u8      bMatrixCam;         // 0x054  the matrix camera is running (freezes time)
    u8      bScriptMatrixMode;  // 0x055  the camera script's matrix mode (freezes time)
    u8      bComicCam;          // 0x056  the comic (3-screen) camera is on
    u8      b57;                // 0x057  only ever cleared in this build
    u8      bSuperZoomCam;      // 0x058  the super zoom is running (freezes time)
    u8      bSlowMoSwingCam;    // 0x059  the slow-motion swing camera is on
    u8      bHeartBeatCam;      // 0x05A  the heart beat camera is beating
    u8      bShutterCam;        // 0x05B  the shutter camera was started (GolfCamera_InitShutterCamera;
                                //        the swing camera clears it; nothing reads it)
    u8      b5C;                // 0x05C  only ever cleared in this build
    u8      bGameBreakerCamPicked; // 0x05D  a shot was picked while the GameBreaker is predicted (until
                                //        then one is picked at once); the swing camera clears it
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
    CrAPGolfer* pShown;         // 0x0B4  the golfer shown
    CrAPGolfer* pLoadingGolfer; // 0x0B8  the golfer being loaded
    s32  nBC;                   // 0x0BC  which of pShown, its pNext or its pPrev pLoadingGolfer is (0..2)
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

// ---- viewports (GoViewport.c) ---------------------------------------------------------------

// The part of the frame buffer a render camera draws into (VM_spCreateViewport makes one). The
// rectangle is in fractions of the frame buffer: 0, 0, 1, 1 is all of it. The ratios are the
// screen's shape, its width and height measured against its width: 1 and 0.75 by default (a 4:3
// screen), 1 and 1 for the square shadow and grass textures. The projection divides by them
// (RC_vUpdateRenderCtxScreenMatricesAndInfo). The last seven fields are worked out from the first
// six by VM_vUpdateInternalViewportRectData.
typedef struct VM_SViewport {
    f32  fLeft;                 // 0x00  the left edge, 0 by default
    f32  fTop;                  // 0x04  the top edge, 0 by default
    f32  fWidth;                // 0x08  1 by default
    f32  fHeight;               // 0x0C  1 by default
    f32  fWidthRatio;           // 0x10  1 by default
    f32  fHeightRatio;          // 0x14  0.75 by default
    f32  fWidthTimesRatio;      // 0x18  fWidth * fWidthRatio
    f32  fHeightTimesRatio;     // 0x1C  fHeight * fHeightRatio
    f32  fWidthOverRatio;       // 0x20  fWidth / fWidthRatio
    f32  fHeightOverRatio;      // 0x24  fHeight / fHeightRatio
    f32  fOneOverWidthRatio;    // 0x28  1 / fWidthRatio
    f32  fOneOverHeightRatio;   // 0x2C  1 / fHeightRatio
    f32  fHeightOverWidth;      // 0x30  fHeight / fWidth
} VM_SViewport;
LAYOUT_ASSERT(VM_SViewport, 0x34);

void          VM_vUpdateInternalViewportRectData(VM_SViewport* pViewport);   // work out the derived fields
void          VM_vInitModule(void);     // nothing to set up
void          VM_vCloseModule(void);    // nothing to free
VM_SViewport* VM_spCreateViewport(void);                         // a new viewport, the default one
void          VM_vReleaseViewport(VM_SViewport* pViewport);      // free it

// ---- the views ------------------------------------------------------------------------------

// A render camera: TW07's render context RC_SRenderCtx (GoRenderCtx.h); only what the cleaned
// code reads here (the fuller layout is Camera in unsorted/cull.h).
typedef struct RenderCamera {
    u8   unk0[0x14];
    VM_SViewport* pRect;        // 0x14  its viewport (RC_spGetRenderCtxViewport)
} RenderCamera;

// Points at the slot holding the current render camera (gapCurrentRenderCtx):
// RC_spGetCurrentRenderCtx reads it, RC_vSetCurrentRenderCtx sets it.
extern void** gppCurrentRenderCtx;

// GoTerrain.c: gives the current render camera the model matrix pMtx (NULL: the identity), through
// RC_vSetRenderCtxTransformationMatrix.
void   RC_vSetCurrentRenderCtxTransformationMatrix(f32 (*pMtx)[4]);
// GoRenderCtx_Gc.c: gives the camera the model matrix pMtx (NULL: the identity).
void   RC_vSetRenderCtxTransformationMatrix(void* pCamera, f32 (*pMtx)[4]);
VM_SViewport* RC_spGetCurrentRenderCtxViewport(void);   // GoTerrain.c: the current render camera's viewport

// RenderAccessors.c: a viewport's rectangle and a render context's viewport (its render state
// setters are in engine.h).
f32    VM_fGetViewportHeight(VM_SViewport* pViewport);
f32    VM_fGetViewportWidth(VM_SViewport* pViewport);
f32    VM_fGetViewportTop(VM_SViewport* pViewport);
f32    VM_fGetViewportLeft(VM_SViewport* pViewport);
VM_SViewport* RC_spGetRenderCtxViewport(RenderCamera* pRenderCtx);

f32    VM_fGetViewportHeightOverWidth(VM_SViewport* pViewport);   // GoRenderCtx_Gc.c
void*  RC_spGetCurrentRenderCtx(void);               // the current render camera
void   RC_vUpdateRenderCtxTransformationMatrices(void* pCamera);
void   RenderState_SetViewport(void* pCamera);
void   RenderState_SetCameraMatrices(void);
// A world position on screen (0..1 across and down; pZ, if not NULL, gets a third value). Returns
// 1 when the point is in front of the camera (clip w below 0), else 0.
u8     RC_vComputeRenderCtxWorldToPrimitiveCoordinate(void* pCamera, f32* pPos, f32* pX, f32* pY, f32* pZ);
void   fn_8006A8D4(void* pCamera, f32* pX, f32* pY);

// ---- the dynamic cameras' shots and sequences (GoDynamicCam.c; its tables are in dyncam.h) ----

void     DynamicCam_RegisterStreamClients(void);
void     DynamicCam_UnRegisterStreamClients(void);
void     DynamicCam_RegisterStreamClientsFE(void);
void     DynamicCam_UnRegisterStreamClientsFE(void);
void     DynamicCam_Init(void);
void     DynamicCam_DeInit(void);
// lookAt and origCamPos are the view's look point and its camera position before this frame.
void     DynamicCam_ProcessScript(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                  f32* position, f32* lookAt, f32* origCamPos, f32 timeStep);
u8       DynamicCam_bIsSwingCamera(CamShot* camLink);
CamShot* DynamicCam_ChooseScript(int ePlayerID, int state, CamShot* dontPick);
CamShot* DynamicCam_ChooseScriptByName(char* camName);
// A shot for the event from the sequence's choices, picked at random, and its blend (each out
// pointer may be NULL): interp a CameraInterpType_e, timeTrigger a CameraEvent_e.
CamShot* DynamicCam_ChooseScriptInSequence(CameraSequence_t* sequence, CameraEvent_e event, int* interp,
                                           f32* time, f32* speed, int* timeTrigger, f32* timeTriggerTime,
                                           int playerID);
// startLie is the ball's lie (Lie_t), endLie a surface class (SurfaceType.nClass).
CameraSequence_t* DynamicCam_ChooseSequence(int ePlayerID, int startLie, int endLie, int state,
                                            u8 allowGenericShotTypes, f32 shotDist);
CameraSequence_t* DynamicCam_ChoosePreFlightSequence(int ePlayerID, int startLie, int state);
// The sequence or shot paired with the golfer's clip (with checkReaction, Character.pReactionClip
// first).
u8       DynamicCam_ChoosePairedSequenceOrCamera(int ePlayerID, u8 checkReaction,
                                                 CameraSequence_t** pSequence, CamShot** pLink);
// it suits the player's club and shot
u8       DynamicCam_IsValidFlightSequence(CameraSequence_t* seq, int playerID);
// The shot looks at the golfer: its nLookAtKind is the ball's spot or one of his points.
u8     CamScript_DoesScriptTrackGolfer(CamShot* script);
// The ball's position, or the settings' vLastBallPos when the ball is by the pin (with set,
// vLastBallPos follows the ball).
void   DynamicCam_GetSmoothBallLocation(CameraScriptSettings_t* settings, CamShot* script, int ePlayerID,
                                        f32* position, u8 set);
// 0 when gSession.nGameType is 3, else Character_IsLeftHanded of the player's golfer (Player.pChar)
// as a flag; the shot is not read.
u8     CameraScript_FlipCameraForLefty(int nPlayer, CamShot* pShot);

// ---- the camera scripts (gocamscripts.c, 0x8003DCE8..) ----------------------------------------

f32      CamScript_GetBallToPinPercent(int nPlayer, CameraScriptSettings_t* pScript);
// pCam and pSub are the view's camera position and where it looks
// (CameraController_GetCameraOrigin, CameraController_GetCameraLookPoint).
void     CamScript_RunScript(int nPlayer, f32* pCam, f32* pSub, CameraScriptSettings_t* pScript,
                             CamShot* pShot, u8 b, f32 fTime);
void     CamScript_RunFEScript(int nPlayer, f32* pCam, f32* pSub, CameraScriptSettings_t* pScript,
                               CamShot* pShot, int a, f32 fTime);
void     CamScript_RunFlybyCamera(int nPlayer, f32* pCam, f32* pSub, CameraScriptSettings_t* pScript,
                                  CamShot* pShot, u8 b, f32 fTime);
void     CamScript_Fade(CameraScriptSettings_t* pScript, f32 fTime);
void     CameraScript_RecordCurrentCam(CamShot* pShot, f32* pCam, f32* pSub, int nPlayer,
                                       CameraScriptSettings_t* pScript, u8 bView1);
void     CameraScript_InterpToNewScript(CameraScriptSettings_t* pScript, CamShot* pShot, int nPlayer,
                                        f32* pCam, f32* pSub, int nInterpType, f32 fInterpTime, f32 fMaxSpeed,
                                        int nTimeTrigger, f32 fTimeTriggerTime);
u8       CameraScript_SnapToScript(CameraScriptSettings_t* pScript, CamShot* pShot);
void     CameraScript_LagAimMarker(int nPlayer, f32* pSub, f32* pCam, CamShot* pShot, u8 bClose, u8 bLimit,
                                   f32 fRate, f32 fMinDist, f32 fYShare);
void     CamScript_GetCameraOnFairwayPos(CameraScriptSettings_t* pScript, f32* pOut, f32* pCam, int nPlayer,
                                         CamShot* pShot, f32* pSub, f32* pHeight);
void     CamScript_PutBackOnFairway(CameraScriptSettings_t* pScript, f32* pCam, f32* pSub, int nPlayer,
                                    CamShot* pShot, f32* pPrev);
// how far the ball's flight has run
f32      CamScript_EstimateBallFlightPercent(int nPlayer, CameraScriptSettings_t* pScript);
u8       CamScript_SkipLookBackCam(CameraScriptSettings_t* pScript, CamShot* pShot, int nPlayer);
void     CameraScript_UpdateLandingEstimate(CameraScriptSettings_t* pScript, int nPlayer);
u8       CameraScript_IsDefaultSwingCam(CamShot* pShot, int nPlayer, f32* pCam);
// Keep pNew above the ground (by fClearance); the out values are optional (NULL): pbFound any
// ground under pNew, pfGround the ground height used, pbRaised pNew was raised.
u8       CamScript_KeepAboveGround(int nPlayer, f32* pNew, f32* pOld, u8 bCheckPath, u8* pbFound,
                                   f32* pfGround, u8* pbRaised, f32 fClearance);
u8       CameraScript_WillGolferBeOccludedInThisView(int nPlayer, CamShot* pShot,
                                                     CameraScriptSettings_t* pScript);
void     CA_vSetCameraFieldOfView(CamLens* pLens, f32 fFov);   // sets the lens's field of view
u8       CamScript_DoesScriptTrackBall(CamShot* pShot);             // the shot's nLookAtKind is 0, 13..15 or
                                                                    // 23

// GoCamera.c: works out the lens's fB0 from its field of view.
void     CA_vUpdateInternalFieldOfViewData(CamLens* pLens);

// ---- the camera paths' maths (CamUtils.c) -----------------------------------------------------

// CamUtils_vCalcArcPosition's arcMode, the way the camera swings round the centre
// (CameraScriptSettings_t.nArcDir).
// The angle is measured in the flat x-z plane, from x towards z.
typedef enum CameraArcMode_e {
    CAMERA_ARC_EITHER_WAY = 0,  // no way asked for: the start angle goes a turn up when the end's is
                                // more than half a turn above it
    CAMERA_ARC_ANGLE_DOWN = 1,  // the angle decreases
    CAMERA_ARC_ANGLE_UP = 2     // the angle increases
} CameraArcMode_e;

// The splined camera: the position on the spline through pos0..3, the look angles on the one
// through look0..3 (each unwrapped to within half a turn of the one before), and the field of view
// between fov0 and fov1, at share param between the middle two.
void   CamScript_SplineCamerasByPositionAndLook(f32* pos0, f32* pos1, f32* pos2, f32* pos3, f32* look0,
                                                f32* look1, f32* look2, f32* look3, f32* finalPos,
                                                f32* finalLook, f32* finalFOV, f32 fov0, f32 fov1,
                                                f32 param);
// The point at share param between pos1 and pos2 on the Catmull-Rom spline through pos0..pos3.
void   CamScript_SplineCamerasByPosition(f32* pos0, f32* pos1, f32* pos2, f32* pos3, f32* finalPos,
                                         f32 param);
// A fly-by's time step while it speeds up (CamScript_AccelerateTime).
f32    CamScript_GetFlybyTimeStep(f32 rate0, f32 rate1, f32 time1, f32 accelParam, f32 currentTime,
                                  f32 timeStep);
// The length of the spline between pos1 and pos2.
f32    CamScript_fGetDistanceBetweenSplinePoints(f32* pos0, f32* pos1, f32* pos2, f32* pos3);
// A fly-by route's timing curve at a share of its time (-1 when out of range).
f32    CamScript_fEvaluateCurve(Curve* c, f32 time);
// Both write a point into endPt: CamUtils_vGetPositionBetweenTwoPoints goes param along the way
// from pt1 to pt2 (flattened unless use3D, a share of the way if usePercent), then sideOffset
// across it; CamUtils_vCalcArcPosition swings round centerPoint from pt1 towards pt2 at share
// param, the way arcMode (CameraArcMode_e) gives.
void   CamUtils_vGetPositionBetweenTwoPoints(f32* pt1, f32* pt2, u8 use3D, u8 usePercent, f32* endPt,
                                             f32 param, f32 sideOffset);
void   CamUtils_vCalcArcPosition(f32* pt1, f32* pt2, f32* centerPoint, CameraArcMode_e arcMode,
                                 f32* endPoint, f32 param);

// ---- the static and fly-by cameras (GoStaticCam.c) -------------------------------------------

void     StaticCam_RegisterStreamClients(void);     // take the 'CAMC' objects (the hole streamer)
void     StaticCam_UnRegisterStreamClients(void);
void     StaticCam_ParseFlybyCameraActor(UStreamObject* pCameraActor);    // 'Cact' type 200
void     StaticCam_ParseStaticCameraActor(UStreamObject* pCameraActor);   // 'Cact' type 201
void     StaticCam_Init(void);
void     StaticCam_DeInit(void);
void     StaticCam_Reset(void);                     // forget the hole's cameras
void     StaticCam_ProcessScript(CamShot* link, int ePlayerID, f32* position);   // the shot's position
CamShot* StaticCam_ChooseScript(int ePlayerID, StaticCam_EventType_e event, u8 dontShowGolfer,
                                CamShot* ignoreLink);
CamShot* StaticCam_GetFlyByCam(int iRouteNum_p);     // a route's first shot (NULL past the 10th)
Curve*   StaticCam_GetFlybyTimeCurve(u32 iRouteNum_p);   // a route's timing curve (NULL: none)
void     StaticCam_GetFlybyInformation(CameraScriptSettings_t* settings, int nRoute, f32* position,
                                       f32* lookat, f32* fov, int ePlayerID, f32 fParam);

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

void   CameraController_InitOneCamera(View* pView);             // set up a view's camera controller
void   CameraController_Idle(View* pView, int nPlayer);         // run it for a frame
void   CameraController_SetCameraMode(View* pView, int nCamera, int nPlayer, int nView);
void   CameraController_ResetCameraState(View* pView);
void   CameraController_CameraCollision(int nView, f32* pBounds);    // the view's camera is inside an
                                                                     // object's bounds
void   CameraController_FadeIn(View* pView, f32 fTime, f32* pVec);
void   CameraController_FadeOut(View* pView, f32 fTime, f32* pVec);
u8     CameraController_IsFadeDone(View* pView);             // CAMFADE_HOLD, _OUT_DONE or _IN_DONE:
                                                             // a fade held or ending
u8     CameraController_IsFadeOutDone(View* pView);             // CAMFADE_OUT_DONE: a fade out just ended
u8     CameraController_IsFadeOn(View* pView);             // CAMFADE_OUT, _IN or _OUT_DONE: a fade running or
                                                           // just faded out
void   CameraController_HoldFadeColor(View* pView, f32* pVec);  // CAMFADE_HOLD: hold the colour pVec over
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

// The screen area a render camera draws into, in screen pixels (0x34 bytes; FB_spCreateFrameBuffer
// makes one): its corner and size, and a scale on each axis. The last seven fields are worked out
// from the first six by FB_vUpdateInternalFrameBufferData. The main loop's is 512 x 448, the
// grass's and the shadow's 256 x 256, all at 0, 0 and scale 1.
typedef struct FB_SFrameBuffer {
    f32  fOffsetX;              // 0x00  the left edge, 0 by default
    f32  fOffsetY;              // 0x04  the top edge, 0 by default
    f32  fWidth;                // 0x08  512 by default
    f32  fHeight;               // 0x0C  448 by default
    f32  fWidthRatio;           // 0x10  1 by default
    f32  fHeightRatio;          // 0x14  1 by default
    f32  fWidthTimesRatio;      // 0x18  fWidth * fWidthRatio
    f32  fHeightTimesRatio;     // 0x1C  fHeight * fHeightRatio
    f32  fWidthOverRatio;       // 0x20  fWidth / fWidthRatio
    f32  fHeightOverRatio;      // 0x24  fHeight / fHeightRatio
    f32  fOneOverWidthRatio;    // 0x28  1 / fWidthRatio
    f32  fOneOverHeightRatio;   // 0x2C  1 / fHeightRatio
    f32  fHeightOverWidth;      // 0x30  fHeight / fWidth
} FB_SFrameBuffer;
LAYOUT_ASSERT(FB_SFrameBuffer, 0x34);

void             FB_vInitModule(void);     // nothing to set up
void             FB_vCloseModule(void);    // nothing to free
FB_SFrameBuffer* FB_spCreateFrameBuffer(void);  // a new frame buffer: 512 x 448 at 0, 0, scale 1
void             FB_vReleaseFrameBuffer(FB_SFrameBuffer* spFrameBuffer_p);    // free it
void             FB_vSetFrameBuffer(FB_SFrameBuffer* spFrameBuffer_p, f32 fOffsetX_p, f32 fOffsetY_p,
                                    f32 fWidth_p, f32 fHeight_p, f32 fWidthRatio_p, f32 fHeightRatio_p);
// GoRenderCtx_Gc.c: the frame buffer's corner and size.
f32              FB_fGetFrameBufferHeight(FB_SFrameBuffer* spFrameBuffer_p);
f32              FB_fGetFrameBufferOffsetY(FB_SFrameBuffer* spFrameBuffer_p);
f32              FB_fGetFrameBufferWidth(FB_SFrameBuffer* spFrameBuffer_p);
f32              FB_fGetFrameBufferOffsetX(FB_SFrameBuffer* spFrameBuffer_p);

// ---- the parts of a render camera: lens (GoCamera.c) ----------------------------------------

void     CA_vInitOnce(void);                          // once-only set-up (empty)
void     CA_vCloseOnce(void);                         // once-only close (empty)
CamLens* CA_spCreateCamera(void);                     // a new lens
void     CA_vReleaseCamera(CamLens* pLens);           // free it
void     Camera_SetCameraPositionAndTarget(CamLens* pLens, f32* pPos, f32* pTarget); // aims the lens from
                                                                                     // pPos at pTarget
// The same with the lens's side vector given.
void     Camera_SetCameraPositionAndTargetWithSideVector(CamLens* pLens, f32* pPos, f32* pTarget,
                                                         f32* pSide);
// The same, then the world scaled by pScale around pCenter.
void     Camera_SetCameraPositionAndTargetWithOffsetAndScale(CamLens* pLens, f32* pPos, f32* pTarget,
                                                             f32* pCenter, f32* pScale);
void     CA_vSetDefaultCamera(CamLens* pLens);
void     CA_vSetCameraFlatSize(CamLens* pLens, f32 fB4, f32 fB8); // sets fB4 and fB8
void     CA_vSetCameraProjectionMode(CamLens* pLens, s32 nType);  // sets nType
void     VM_vSetViewportRatios(VM_SViewport* pViewport, f32 fWidthRatio, f32 fHeightRatio); // shadow.c
CamLens* Camera_GetCurrentLens(void);                     // char.c

// GoRenderCtx_Gc.c: a render camera made from a lens, a frame buffer and a viewport.
void*    RC_spCreateRenderCtx(CamLens* pLens, FB_SFrameBuffer* pBuf, VM_SViewport* pViewport);
void     RC_vReleaseRenderCtx(void* pCamera);            // free it

// ---- the views (GoViewCont.c) ----------------------------------------------------------------

void             ViewController_ResetAll(void);                 // every view unused
void             ViewController_SetCurrentViewController(int iViewControllerID);
ViewController_t* ViewController_GetCurrentViewController(void);
ViewController_t* ViewController_GetIndexedViewController(int iViewControllerID);
int              ViewController_GetCurrentViewControllerID(void);
// Opens a view on the rectangle given in fractions of the frame buffer.
void             ViewController_Init(int iViewControllerID, f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight);
void             ViewController_Delete(int iViewControllerID);  // closes it
void             ViewController_Update(int iViewControllerID);  // runs its camera for a frame
void*            ViewController_GetRenderContext(int iViewControllerID);
View*            ViewController_GetCameraControl(int iViewControllerID);
void             ViewController_SetActivePlayerNumber(int iViewControllerID, int ePlayerID);
int              ViewController_GetActivePlayerNumber(int iViewControllerID);   // the player it follows
u8               ViewController_IsActive(int iViewControllerID);                // the view is in use
void             ViewController_TurnOnViewController(int iViewControllerID, u8 active);
void             ViewController_SaveViewportRect(int iViewControllerID);
void             ViewController_RestoreViewportRect(int iViewControllerID);
FB_SFrameBuffer* RC_spGetCurrentRenderCtxFrameBuffer(void);     // the current render context's one
void             VM_vSetViewportRect(VM_SViewport* spViewport_p, f32 fLeft_p, f32 fTop_p, f32 fWidth_p,
                                     f32 fHeight_p);            // fractions of the frame buffer
void             CA_vSetDefaultScalingVectors(CamLens* spCamera_p);             // no scaling
u8               CameraController_IsFlybyDone(View* pCameraController);
f32*             CameraController_GetCameraLookPoint(View* pCameraController);  // View.vLookPoint
f32*             CameraController_GetCameraOrigin(View* pCameraController);     // View.vOrigin

void GolfCamera_ResetSpecialCameraStates(void);

#endif

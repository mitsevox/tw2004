//
// Information extracted with resym v0.4.0
//
// PDB file: default.pdb
// Image architecture: X86
//


struct GM_Pga_StatCounts;
struct TStreamData;
struct PgaTour_WinInfo;
struct GM_PgaTour_EventSlot_t;
struct CommonData_t;
class GameModeDriverPGATour;
class GameModeDriver;

struct YearCourse_t { /* Size=0x8 */
  /* 0x0000 */ public: unsigned int course;
  /* 0x0004 */ public: int changeList;
};

struct Tournament_round_t { /* Size=0x4 */
  /* 0x0000 */ public: char pinPosition;
  /* 0x0001 */ public: char tee;
  /* 0x0002 */ public: short tigerProofLevel;
};

struct GM_PgaTour_DevPoints_t { /* Size=0x8 */
  /* 0x0000 */ public: int devPointType;
  /* 0x0004 */ public: int firstDevPoints;
};

struct GM_PgaTour_Winnings_t { /* Size=0x8 */
  /* 0x0000 */ public: int purseWinnings;
  /* 0x0004 */ public: int firstWinnings;
};

struct GM_PgaTour_SponsorshipSlot_t { /* Size=0xc */
  /* 0x0000 */ public: int gameCompletion;
  /* 0x0004 */ public: int startCashBonus;
  /* 0x0008 */ public: int cashBonusPerRound;
};

struct Tournament_events_t { /* Size=0x20 */
  /* 0x0000 */ public: short nRounds;
  /* 0x0002 */ public: short nGameMode;
  /* 0x0004 */ public: short courseListID;
  /* 0x0006 */ public: short cutRank;
  /* 0x0008 */ public: short minField;
  /* 0x000a */ public: short maxField;
  /* 0x000c */ public: Tournament_round_t round[4];
  /* 0x001c */ public: short firstRoundAIScore;
  /* 0x001e */ public: short anyRoundLowestScore;
};

struct GM_PgaTour_RankPoints_t { /* Size=0x8 */
  /* 0x0000 */ public: short majorPoints;
  /* 0x0002 */ public: short bigPoints;
  /* 0x0004 */ public: short regularPoints;
  /* 0x0006 */ public: short specialPoints;
};

struct GM_PgaTour_EntryCriteria_t { /* Size=0x18 */
  /* 0x0000 */ public: int entryStringIndex;
  /* 0x0004 */ public: int entryCriteria_1;
  /* 0x0008 */ public: int entryValue_1;
  /* 0x000c */ public: int logicalOperator;
  /* 0x0010 */ public: int entryCriteria_2;
  /* 0x0014 */ public: int entryValue_2;
};

enum GM_DriverType_t : int {
  GM_PGATour_Driver = 0x0000,
  GM_PGAGrudgeMatch_Driver = 0x0001,
  GM_Era_Driver = 0x0002,
  GM_Realtime_Driver = 0x0003,
  GM_Traditional_Driver = 0x0004,
  GM_Career_Driver = 0x0005,
  GM_Drivers_MAX = 0x0006,
};

struct GM_PgaTour_CourseListByRound_t { /* Size=0x10 */
  /* 0x0000 */ public: int course[4];
};

enum GM_HoleResult_t : int {
  kGolferLosesHole = 0x0000,
  kGolferTiesHole = 0x0001,
  kGolferWinsHole = 0x0002,
  kGolferHoleResultUnknown = 0x0003,
};

enum EVENTID_e : int {
  kEventID_BeginHole = 0x0000,
  kEventID_EndHole = 0x0001,
  kEventID_RestartHole = 0x0002,
  kEventID_BeginTurn = 0x0003,
  kEventID_EndTurn = 0x0004,
  kEventID_EndGame = 0x0005,
  kEventID_ShotSetup = 0x0006,
  kEventID_PreSwing = 0x0007,
  kEventID_Delay = 0x0008,
  kEventID_PracticeSwing = 0x0009,
  kEventID_HitBall = 0x000a,
  kEventID_SwingDone = 0x000b,
  kEventID_BallBounce = 0x000c,
  kEventID_NextClub = 0x000d,
  kEventID_PrevClub = 0x000e,
  kEventID_NextShotType = 0x000f,
  kEventID_PrevStance = 0x0010,
  kEventID_NextStance = 0x0011,
  kEventID_RotateLeft = 0x0012,
  kEventID_RotateRight = 0x0013,
  kEventID_MoveTargetForward = 0x0014,
  kEventID_MoveTargetBack = 0x0015,
  kEventID_PlaceBallRotateLeft = 0x0016,
  kEventID_PlaceBallRotateRight = 0x0017,
  kEventID_PlaceBallMoveTargetForward = 0x0018,
  kEventID_PlaceBallMoveTargetBack = 0x0019,
  kEventID_Idle = 0x001a,
  kEventID_BallMoving = 0x001b,
  kEventID_TopOfArc = 0x001c,
  kEventID_FirstBounce = 0x001d,
  kEventID_NonFirstBounce = 0x001e,
  kEventID_LastBounceForSpinna = 0x001f,
  kEventID_BallStop = 0x0020,
  kEventID_InHole = 0x0021,
  kEventID_OutOfBounds = 0x0022,
  kEventID_Collision = 0x0023,
  kEventID_CollisionObject = 0x0024,
  kEventID_CollisionTree = 0x0025,
  kEventID_CollisionPin = 0x0026,
  kEventID_CollisionActor = 0x0027,
  kEventID_BreaklineDone = 0x0028,
  kEventID_BreaklinePassedCup = 0x0029,
  kEventID_PlayerEmotionUpdated = 0x002a,
  kEventID_SpinWindowFinished = 0x002b,
  kEventID_BeganBackswing = 0x002c,
  kEventID_TappaTappaTappa = 0x002d,
  kEventID_SpinnaSpinnaSpinna = 0x002e,
  kEventID_BeganDownSwing = 0x002f,
  kEventID_StartCameraZoom = 0x0030,
  kEventID_EndCameraZoom = 0x0031,
  kEventID_StartMatrixCam = 0x0032,
  kEventID_EndMatrixCam = 0x0033,
  kEventID_3ShotSwingStarted = 0x0034,
  kEventID_SlowMotionStart = 0x0035,
  kEventID_SlowMotionEnd = 0x0036,
  kEventID_FastMotionStart = 0x0037,
  kEventID_FastMotionEnd = 0x0038,
  kEventID_StartSuperZoomCam = 0x0039,
  kEventID_EndSuperZoomCam = 0x003a,
  kEventID_SpecialSwingEnded = 0x003b,
  kEventID_SpecialSwingDone = 0x003c,
  kEventID_BallPredictionDone = 0x003d,
  kEventID_ScriptedGameBreakerStarted = 0x003e,
  kEventID_ScriptedGameBreakerEnd = 0x003f,
  kEventID_PredictedGameBreakerStarted = 0x0040,
  kEventID_PredictedGameBreakerEnd = 0x0041,
  kEventID_LeaderboardDisplay = 0x0042,
  kEventID_UnlockedNewCharacter = 0x0043,
  kEventID_UnlockedNewCourse = 0x0044,
  kEventID_SpeedgolfReady = 0x0045,
  kEventID_SpeedgolfGo = 0x0046,
  kEventID_ScoreCardDone = 0x0047,
  kEventID_AnimationSkinReset = 0x0048,
  kEventID_NewAnimationPlayed = 0x0049,
  kEventID_EstimatedBallFirstBounce = 0x004a,
  kEventID_FlyByEvent = 0x004b,
  kEventID_BallHitDelayed = 0x004c,
  kEventID_RandomBounce = 0x004d,
  kEventID_FlyByEventDone = 0x004e,
  kEventID_Player1WonGame = 0x004f,
  kEventID_ConfidenceSwing = 0x0050,
  kEventID_EndCutscene = 0x0051,
  kEventID_EndPreHit = 0x0052,
  kEventID_InitSwingState = 0x0053,
  kEventID_InitSwing_AfterMulligan = 0x0054,
  kEventID_EndEventCutscene = 0x0055,
  kEventID_EndGameCutscene = 0x0056,
  kEventID_EndTurnCutscene = 0x0057,
  kEventID_NUMEVENTS = 0x0058,
};

enum PlayerNumber_t : int {
  PLR_1_e = 0x0000,
  PLR_2_e = 0x0001,
  PLR_3_e = 0x0002,
  PLR_4_e = 0x0003,
  PLR_MAX_e = 0x0004,
};

enum UIScoreCardDispMode_t : int {
  ScoreCard_None = 0x0000,
  ScoreCard_Stroke = 0x0001,
  ScoreCard_Match = 0x0002,
  ScoreCard_Stableford = 0x0003,
  ScoreCard_Skins = 0x0004,
  ScoreCard_PGATour = 0x0005,
  ScoreCard_BestBall = 0x0006,
  ScoreCard_Skillz18 = 0x0007,
  ScoreCard_Challenge = 0x0008,
};

struct GM_Pga_HistoricalStats_t { /* Size=0x68 */
  /* 0x0000 */ public: char name[24];
  /* 0x0018 */ public: float stat[19];
  /* 0x0064 */ public: unsigned int careerWinnings;
};

struct GM_PgaTour_NameList_t { /* Size=0x8 */
  /* 0x0000 */ public: int period;
  /* 0x0004 */ public: unsigned int nameIdx;
};

struct GM_PgaTour_EventSlot_t { /* Size=0x9c */
  /* 0x0000 */ public: unsigned int nameIdx;
  /* 0x0004 */ public: int nameChangeList;
  /* 0x0008 */ public: int tournament;
  /* 0x000c */ public: int scenario;
  /* 0x0010 */ public: int isEuro;
  /* 0x0014 */ public: int isAMajor;
  /* 0x0018 */ public: int rankPointType;
  /* 0x001c */ public: int criteriaID;
  /* 0x0020 */ public: int textureID;
  /* 0x0024 */ public: char champName[16];
  /* 0x0034 */ public: int champScore;
  /* 0x0038 */ public: GM_PgaTour_Winnings_t winningsBracket;
  /* 0x0040 */ public: GM_PgaTour_DevPoints_t devPointsBracket;
  /* 0x0048 */ public: int pad[6];
  /* 0x0060 */ public: unsigned short startDate[30];
};

struct GM_PgaTour_CourseListByYear_t { /* Size=0x30 */
  /* 0x0000 */ public: int count;
  /* 0x0004 */ public: int period;
  /* 0x0008 */ public: YearCourse_t course[5];
};

struct PgaTour_WinInfo { /* Size=0xc */
  /* 0x0000 */ public: bool placed;
  /* 0x0004 */ public: int position;
  /* 0x0008 */ public: int winnings;
};

struct PGA_Master { /* Size=0x7320 */
  /* 0x0000 */ public: GM_Pga_HistoricalStats_t PgaHistoricalStatsData[174];
  /* 0x46b0 */ public: GM_PgaTour_EventSlot_t calendar[35];
  /* 0x5c04 */ public: Tournament_events_t tournaments[35];
  /* 0x6064 */ public: int anLegendCoinAwards[5];
  /* 0x6078 */ public: GM_PgaTour_SponsorshipSlot_t sponsorships[21];
  /* 0x6174 */ public: GM_PgaTour_EntryCriteria_t entryCriteria[35];
  /* 0x64bc */ public: char* entryCriteriaStrTable;
  /* 0x64c0 */ public: GM_PgaTour_CourseListByYear_t courseListByYear[35];
  /* 0x6b50 */ public: GM_PgaTour_CourseListByRound_t courseListByRound[35];
  /* 0x6d80 */ public: GM_PgaTour_NameList_t nameList[35];
  /* 0x6e98 */ public: GM_PgaTour_RankPoints_t rankPoints[144];
  /* 0x7318 */ public: char* pStrTable;
  /* 0x731c */ public: char* pStrTable2;
};

class GameModeDriver { /* Size=0x1c */
  /* 0x0004 */ protected: int m_nEvents;
  /* 0x0008 */ protected: int m_lastPlayedEventID;
  /* 0x000c */ protected: int m_selectedEventID;
  /* 0x0010 */ protected: bool m_playNowMode;
  /* 0x0014 */ private: CommonData_t* m_pCommon;
  /* 0x0018 */ private: unsigned short m_SelectedEventDate;
  /* 0x001a */ private: bool m_EndOfTournament;
  public: static const unsigned int c_nMaxTPLevels;

  public: GameModeDriver(const GameModeDriver&);
  public: GameModeDriver();
  public: virtual ~GameModeDriver();
  public: virtual bool QueryInterface(GM_DriverType_t, void*&);
  public: virtual void PreHoleLoadInit();
  public: virtual void PostHoleLoadInit();
  public: virtual bool GameFinished(bool);
  public: virtual bool GoToPlayoff(bool);
  public: virtual void StartGamePreData();
  public: virtual void EndHole();
  public: virtual void EndGame();
  public: virtual void QuitGame();
  public: virtual int NumActualPlayingGolfers() const;
  public: virtual UIScoreCardDispMode_t GetScoreDisplayMode(bool);
  public: virtual void GetCurrentScoreString(char*, int);
  public: virtual bool IsPuttForLead(PlayerNumber_t);
  public: virtual bool IsPuttForWin(PlayerNumber_t);
  public: virtual PlayerNumber_t GetCurrentLead(PlayerNumber_t);
  public: virtual PlayerNumber_t GetPotentialLead(PlayerNumber_t);
  public: virtual GM_HoleResult_t GetPotentialHoleResult(PlayerNumber_t);
  public: virtual int GetNextEvent();
  public: virtual void SelectEvent(int);
  public: virtual void StartEvent();
  public: virtual void ClearEvent();
  public: virtual bool CanPlayEvent(int);
  public: virtual void HandleEvent(PlayerNumber_t, EVENTID_e);
  public: virtual bool CheckCharNodeFlag(unsigned short);
  public: void SetEventDate(unsigned short);
  public: unsigned short GetSelectedEventDate();
  public: virtual unsigned short UI_GetCalendarInitialDate();
  public: virtual void UI_SetEventInfoText(unsigned short);
  public: virtual int UI_GetEventIconIndexOnCal(int);
  public: virtual int UI_GetEventIconIndexInDetails(unsigned short);
  public: virtual bool UI_CanPlayEventOnDate(unsigned short);
  public: int GetNumEvents();
  public: int GetSelectedEventID();
  public: int GetLastPlayedEventID();
  public: bool GetCurrentlyPlayNowMode();
  public: virtual void EndCurrentEvent();
  public: virtual int GetPgaTourTournament(int);
  public: virtual int GetAnyRoundLowScore(int);
  public: virtual float GetRankPoints(int, int);
  public: virtual const float GetHistoricStatsData(int, int);
  public: virtual const char* GetHistoricStatsGolferName(int);
  public: virtual const char* GetInitialChampName(int);
  public: virtual int GetInitialChampScore(int);
  public: virtual unsigned short GetCurrentBracket(PlayerNumber_t);
  public: virtual int GetFinalEventOfSeason();
  public: virtual const GM_PgaTour_EventSlot_t* GetEventInfo(int);
  public: virtual int GetCourses(const GM_PgaTour_EventSlot_t*, GM_CourseType_t*);
  public: virtual int GetPurseForBracket(int, int);
  public: virtual int GetFirstPrizeForBracket(int, int);
  public: virtual int GetDevPointsPurseForBracket(PlayerNumber_t);
  public: virtual int GetDevPointAssignment(PlayerNumber_t);
  public: virtual int GetDevPointsFirstPrizeForBracket(int, int);
  public: virtual unsigned short GetEndDate(int, int);
  public: virtual int GetCurrentSeason();
  public: virtual void AwardMoney(PlayerNumber_t, int);
  public: virtual bool IsSpudLeaderBoardShownCommonly();
  public: virtual bool IsSpudHudUsed();
  public: virtual void SetEndOfTournament(bool);
  public: virtual bool IsEndOfTournament();
  public: virtual bool IsCurrentEventScoreFake();
  public: virtual bool AIUseDisplayStats();
  protected: void SetCommonData(CommonData_t*);
  protected: CommonData_t* GetCommonData();
  private: void UpdateTigerProofingLevel();
  private: void ApplyTigerProofSettings();
  public: void __local_vftable_ctor_closure();
  public: virtual void* __vecDelDtor(unsigned int);
};

enum ePGA_EntryCriteria : int {
  PGA_ENTRY_CRITERIA_NONE = 0x0000,
  PGA_ENTRY_CRITERIA_SEASON_MONEY_LIST = 0x0001,
  PGA_ENTRY_CRITERIA_ALL_AROUND_RANK = 0x0002,
  PGA_ENTRY_CRITERIA_SCORING_AVERAGE = 0x0003,
  PGA_ENTRY_CRITERIA_DRIVING_DISTANCE = 0x0004,
  PGA_ENTRY_CRITERIA_PUTTING_AVERAGE = 0x0005,
  PGA_ENTRY_CRITERIA_MAJOR_WINNER = 0x0006,
  PGA_ENTRY_CRITERIA_HAS_TOUR_CARD = 0x0007,
  PGA_ENTRY_CRITERIA_PGA_TOUR_RANK = 0x0008,
  PGA_ENTRY_CRITERIA_PREVIOUS_SEASON_WINS = 0x0009,
  PGA_ENTRY_CRITERIA_PREVIOUS_YEAR_WINS = 0x000a,
  PGA_ENTRY_CRITERIA_SEASON_WINS = 0x000b,
  PGA_ENTRY_CRITERIA_PREVIOUS_SEASON_MONEY_LIST = 0x000c,
  PGA_ENTRY_CRITERIA_CAREER_MONEY_LIST = 0x000d,
  PGA_ENTRY_CRITERIA_CAREER_MAJOR_WINS = 0x000e,
  PGA_ENTRY_CRITERIA_SEASON_MAJOR_WINS = 0x000f,
  PGA_ENTRY_CRITERIA_SEASON_ALL_4_MAJORS = 0x0010,
  PGA_ENTRY_CRITERIA_QUALIFYING_EVENT_RANK = 0x0011,
};

enum SEASON_e : int {
  SEASON_SPRING_e = 0x0000,
  SEASON_FALL_e = 0x0001,
  SEASON_SUMMER_e = 0x0002,
  SEASON_WINTER_e = 0x0003,
  SEASON_MAX_e = 0x0004,
};

struct GM_Pga_StatCounts { /* Size=0x5c */
  /* 0x0000 */ public: unsigned short nEvents;
  /* 0x0002 */ public: unsigned short nRounds;
  /* 0x0004 */ public: unsigned short longestDrive;
  /* 0x0006 */ public: unsigned short nDrives;
  /* 0x0008 */ public: unsigned int totalDriveDistance;
  /* 0x000c */ public: unsigned short longestPutt;
  /* 0x000e */ public: unsigned short nFairwaysHit;
  /* 0x0010 */ public: unsigned short nFairwaysPossible;
  /* 0x0012 */ public: unsigned short nGreensHit;
  /* 0x0014 */ public: unsigned short nHoles;
  /* 0x0016 */ public: unsigned short nPutts;
  /* 0x0018 */ public: unsigned short nGIRPutts;
  /* 0x001a */ public: unsigned short nBunkerSaves;
  /* 0x001c */ public: unsigned short nBunkers;
  /* 0x001e */ public: unsigned short nWaterHitSaves;
  /* 0x0020 */ public: unsigned short nWaterHits;
  /* 0x0022 */ public: unsigned short nNonGIRPars;
  /* 0x0024 */ public: unsigned short nBirdiesAfterBogey;
  /* 0x0026 */ public: unsigned short nBogeysOrWorse;
  /* 0x0028 */ public: unsigned short nPuttsOver30Feet;
  /* 0x002a */ public: unsigned short nEagles;
  /* 0x002c */ public: unsigned short nBirdies;
  /* 0x002e */ public: unsigned short nPar3Birdies;
  /* 0x0030 */ public: unsigned short nPar3Holes;
  /* 0x0032 */ public: unsigned short nPar4Birdies;
  /* 0x0034 */ public: unsigned short nPar4Holes;
  /* 0x0036 */ public: unsigned short nPar5Birdies;
  /* 0x0038 */ public: unsigned short nPar5Holes;
  /* 0x003a */ public: unsigned short nGIRBirdies;
  /* 0x003c */ public: unsigned short nStrokes;
  /* 0x003e */ public: unsigned short nPar3Strokes;
  /* 0x0040 */ public: unsigned short nPar4Strokes;
  /* 0x0042 */ public: unsigned short nPar5Strokes;
  /* 0x0044 */ public: unsigned int seasonWinnings;
  /* 0x0048 */ public: unsigned int monthlyWinnings;
  /* 0x004c */ public: unsigned short nSeasonWins;
  /* 0x004e */ public: unsigned char playerOfYearPoints;
  /* 0x004f */ public: unsigned char nCareerStatStart;
  /* 0x0050 */ public: unsigned short nConsecutiveCuts;
  /* 0x0054 */ public: unsigned int careerWinnings;
  /* 0x0058 */ public: unsigned short nCareerWins;
  /* 0x005a */ public: unsigned short nCareerAces;
};

enum GM_ModeType_t : int {
  GM_Stroke_mode = 0x0000,
  GM_Match_mode = 0x0001,
  GM_Skins_mode = 0x0002,
  GM_OnlineTour_mode = 0x0003,
  GM_LongDriveChallenge_mode = 0x0004,
  GM_PlayNow_mode = 0x0005,
  GM_Scenario_mode = 0x0005,
  GM_ClosestToPin_mode = 0x0006,
  GM_ChippingChallenge_mode = 0x0007,
  GM_PuttingChallenge_mode = 0x0008,
  GM_Practice_mode = 0x0009,
  GM_Replay_mode = 0x000a,
  GM_Unused1_mode = 0x000b,
  GM_Crac_Editor = 0x000c,
  GM_Skillz18_mode = 0x000d,
  GM_Unused2_mode = 0x000e,
  GM_Unused3_mode = 0x000f,
  GM_Unused4_mode = 0x0010,
  GM_Unused7_mode = 0x0011,
  GM_Stableford_mode = 0x0012,
  GM_BestBall_mode = 0x0013,
  GM_FourBall_mode = 0x0014,
  GM_AlternateShot_mode = 0x0015,
  GM_Unused5_mode = 0x0016,
  GM_PgaTour_mode = 0x0017,
  GM_Realtime_mode = 0x0018,
  GM_Battle_mode = 0x0019,
  GM_Unused6_mode = 0x001a,
  GM_ThreeHoleStroke_mode = 0x001b,
  GM_ClubHouse_mode = 0x001c,
  GM_D18_Challenge = 0x001d,
  GM_GameModes_MAX = 0x001e,
  GM_SkillZone_Generic = 0x029a,
};

enum GM_CourseType_t : int {
  GM_Course_Compilation_START = 0x0010,
  GM_Course_Compilation1 = 0x0010,
  GM_Course_Compilation2 = 0x0011,
  GM_Course_Compilation3 = 0x0012,
  GM_Course_Compilation4 = 0x0013,
  GM_Course_Compilation5 = 0x0014,
  GM_Course_Compilation6 = 0x0015,
  GM_Course_Compilation7 = 0x0016,
  GM_Course_Compilation8 = 0x0017,
  GM_Course_Compilation9 = 0x0018,
  GM_Course_Compilation10 = 0x0019,
  GM_Course_Compilation_MAX = 0x001a,
  GM_Course_Dream18 = 0x0019,
  GM_Course_Random18 = 0x001a,
  GM_Course_All_MAX = 0x001a,
  GM_FORCE32BIT = 0x7fffffff,
};

class GameModeDriverPGATour : public GameModeDriver { /* Size=0x1f0 */
  /* 0x0000: fields for GameModeDriver */
  /* 0x001c */ private: bool m_UserWins;
  /* 0x0020 */ private: int m_UserWinPurse;
  /* 0x0024 */ private: PgaTour_WinInfo m_WinInfo;
  /* 0x0030 */ private: GM_Pga_StatCounts m_aGuestRoundStatCounts[3];
  /* 0x0148 */ private: unsigned long long int m_optionTimeStamp[4];
  /* 0x0168 */ private: unsigned long long int m_guestTimeStamp[4];
  /* 0x0188 */ private: GM_Pga_StatCounts m_UserRoundStatCounts;
  /* 0x01e4 */ private: int m_PlayoffHole;
  /* 0x01e8 */ private: bool m_inGrudgeMatch;
  private: static PGA_Master m_PgaData;

  public: GameModeDriverPGATour(const GameModeDriverPGATour&);
  public: GameModeDriverPGATour();
  public: virtual ~GameModeDriverPGATour();
  public: virtual void StartEvent();
  public: virtual bool QueryInterface(GM_DriverType_t, void*&);
  public: virtual unsigned short UI_GetCalendarInitialDate();
  public: virtual void UI_SetEventInfoText(unsigned short);
  public: virtual bool UI_CanPlayEventOnDate(unsigned short);
  public: virtual UIScoreCardDispMode_t GetScoreDisplayMode(bool);
  public: virtual void GetCurrentScoreString(char*, int);
  public: virtual void PostHoleLoadInit();
  public: virtual bool GameFinished(bool);
  public: virtual void StartGamePreData();
  public: virtual void EndHole();
  public: virtual void EndGame();
  public: virtual bool GoToPlayoff(bool);
  public: virtual bool IsPuttForLead(PlayerNumber_t);
  public: virtual bool IsPuttForWin(PlayerNumber_t);
  public: virtual PlayerNumber_t GetCurrentLead(PlayerNumber_t);
  public: virtual PlayerNumber_t GetPotentialLead(PlayerNumber_t);
  public: GM_CourseType_t GetCurrentCourse();
  public: GM_CourseType_t GetCourseByEventSeasonRound(const GM_PgaTour_EventSlot_t*, int, int);
  public: unsigned int GetCourseByTourneySeasonRound(int, int, int);
  public: const char* GetNameByEventSeason(int, int);
  public: bool GetEventByDate(unsigned short, int*, int*);
  public: virtual int GetNextEvent();
  public: virtual int GetFinalEventOfSeason();
  public: virtual const GM_PgaTour_EventSlot_t* GetEventInfo(int);
  public: int GetRounds(int);
  public: const GM_PgaTour_EventSlot_t* GetEventInfoByDate(unsigned short);
  public: unsigned short GetStartDate(int, int);
  public: virtual unsigned short GetEndDate(int, int);
  public: const char* GetName(int);
  public: int GetTextureID(int);
  public: virtual const char* GetInitialChampName(int);
  public: virtual int GetInitialChampScore(int);
  public: virtual int GetCourses(const GM_PgaTour_EventSlot_t*, GM_CourseType_t*);
  public: int GetSponsorshipGameCompletion(int);
  public: int GetSponsorshipStartCashBonus(int);
  public: int GetSponsorshipCashBonusPerRound(int);
  public: bool CheckCriteria(int);
  public: const char* GetEntryString(int);
  public: virtual int GetFirstPrizeForBracket(int, int);
  public: virtual int GetPurseForBracket(int, int);
  public: virtual int GetDevPointsFirstPrizeForBracket(int, int);
  public: virtual int GetDevPointsPurseForBracket(PlayerNumber_t);
  public: int GetFirstRoundAIScore(int);
  public: virtual int GetAnyRoundLowScore(int);
  public: int GetCutRank(int);
  public: int GetEventOnOrAfter(int);
  public: virtual int GetPgaTourTournament(int);
  public: int GetScenario(int);
  public: int GetMinField(int);
  public: int GetMaxField(int);
  public: unsigned int GetTournamentCourse(int, int);
  public: int GetTee(int, int);
  public: int GetPinPosition(int, int);
  public: int GetTigerProofLevel(int, int);
  public: int GetTimeOfDay(int, int);
  public: bool IsAMajor(int);
  public: int GetLegendCoinAward(int);
  public: virtual float GetRankPoints(int, int);
  public: const PgaTour_WinInfo* GetWinInfo();
  public: void CheckAdvanceTournament(PlayerNumber_t);
  public: virtual void AwardMoney(PlayerNumber_t, int);
  public: void SimCurrentTournament(PlayerNumber_t, bool);
  public: int GetSelectedEvent(int*);
  public: virtual void SelectEvent(int);
  public: void AdvanceSeason();
  public: virtual int GetCurrentSeason();
  public: int GetCurrentSeasonYear();
  public: bool OnLastSeason();
  public: int GetCurrentEventID();
  public: void GetPurseString(int, char*, unsigned int);
  public: void GetCurrentEventLeader(char*, unsigned int);
  public: int GetCurrentLeaderScore();
  public: void GetWinnerEarningsString(int, char*, unsigned int);
  public: int GetUserScore(int);
  public: void GetUserFinishString(int, char*, unsigned int);
  public: void GetChamp(int, char*, unsigned int);
  public: int GetChampScore(int);
  public: int DisplayEndOfHoleMessage(char*, unsigned int);
  public: int GetUsersCurrentEventID(PlayerNumber_t);
  public: int GetCurrentTourNum();
  public: virtual unsigned short GetCurrentBracket(PlayerNumber_t);
  public: unsigned long long int GetOptionTimeStamp(int);
  public: void SetOptionTimeStamp(int, unsigned long long int);
  public: unsigned long long int GetGuestTimeStamp(int);
  public: void SetGuestTimeStamp(int, unsigned long long int);
  public: const GM_Pga_StatCounts* GetUserRoundStatCounts();
  public: virtual const float GetHistoricStatsData(int, int);
  public: virtual const char* GetHistoricStatsGolferName(int);
  public: const unsigned int GetHistoricStatsCareerWinnings(int);
  public: bool GetInGrudgeMatch();
  public: void SetInGrudgeMatch(bool);
  public: virtual bool IsSpudLeaderBoardShownCommonly();
  public: virtual bool IsSpudHudUsed();
  private: void SetTournament(int);
  private: void PlayEndOfGameMovies();
  private: void EndTournament(PlayerNumber_t);
  private: void AdvanceEvent(PlayerNumber_t);
  private: void CommitUserOrGuestRoundStatCounts(PlayerNumber_t, int);
  private: void SetCurrentBracket(PlayerNumber_t, unsigned short);
  private: unsigned short ComputeBracket(PlayerNumber_t, int);
  private: void UpdateUserOrGuestStats(PlayerNumber_t, int);
  private: void GetTimeOfDayFromRank(char*, char*, int);
  private: SEASON_e GetSeasonFromMonth(unsigned int);
  private: int GetNEventsWon();
  private: void GetUserAndGuestNameList(char*, unsigned int, bool);
  private: void GetUserAndGuestNamesInPlayoff(char*, unsigned int);
  private: void PrepareForTeeOff();
  private: void SetupGuestAndAIPlayers();
  private: bool CriteriaSatisfied(ePGA_EntryCriteria, int);
  private: GM_ModeType_t GetGameModeForEvent(int);
  private: float ComputeFirstPrizeForBracket(int, int);
  private: float ComputePurseForBracket(int, int);
  private: float ComputeDevPointsFirstPrizeForBracket(int, int);
  public: void __local_vftable_ctor_closure();
  public: virtual void* __vecDelDtor(unsigned int);

  public: static void RegisterStreamClients();
  public: static void UnregisterStreamClients();
  public: static void Locale_PgaTourMode_LoadPGAnFromStream(TStreamData*);
  public: static void Locale_PgaTourMode_LoadPGAcFromStream(TStreamData*);
  public: static void Locale_PgaTourMode_LoadPGAeFromStream(TStreamData*);
  public: static void Locale_PgaTourMode_LoadPGAsFromStream(TStreamData*);
  public: static void Locale_PgaTourMode_LoadPGnlFromStream(TStreamData*);
  public: static void Locale_PgaTourMode_LoadPGn2FromStream(TStreamData*);
  private: static void LoadPGAcFromStream(TStreamData*);
  private: static void LoadPGAtFromStream(TStreamData*);
  private: static void LoadPGApFromStream(TStreamData*);
  private: static void LoadPGAnFromStream(TStreamData*);
  private: static void LoadPGALegendCoins(TStreamData*);
  private: static void LoadPGAeFromStream(TStreamData*);
  private: static void LoadPGAsFromStream(TStreamData*);
  private: static void LoadPGAyFromStream(TStreamData*);
  private: static void LoadPGArFromStream(TStreamData*);
  private: static void LoadPGnlFromStream(TStreamData*);
  private: static void LoadPGn2FromStream(TStreamData*);
  private: static void LoadPGrpFromStream(TStreamData*);
  private: static void LoadPGSTfromStream(TStreamData*);
};

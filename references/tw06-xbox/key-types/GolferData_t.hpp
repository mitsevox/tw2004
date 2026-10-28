//
// Information extracted with resym v0.4.0
//
// PDB file: default.pdb
// Image architecture: X86
//


class HashString;
struct GolferData_t;

union _unnamed_0x44ae { /* Size=0x10 */
  /* 0x0000 */ public: char label[16];
  /* 0x0000 */ public: long long int __align;
};

class HashString { /* Size=0x10 */
  /* 0x0000 */ private: _unnamed_0x44ae m_u;

  public: HashString(const char*);
  public: HashString(const HashString&);
  public: HashString();
  public: void Clear();
  public: void Set(const HashString&);
  public: void Set(const char*, int);
  public: void ToString(char*, int) const;
  public: bool IsEquals(const char*, bool) const;
  public: bool IsEquals(const HashString&) const;
  public: bool IsEmpty() const;
  public: int Compare(const HashString&) const;
  public: bool operator!() const;
  public: char operator[](int) const;
  private: int Length() const;

  public: static bool IsValid(const char*, HashString::StatusType*);
};

struct GolferData_t { /* Size=0x1f0 */
  /* 0x0000 */ public: unsigned char ID;
  /* 0x0001 */ public: unsigned char modelID;
  /* 0x0002 */ public: char fname[32];
  /* 0x0022 */ public: char lname[32];
  /* 0x0042 */ public: char nname[32];
  /* 0x0062 */ public: char ballID;
  /* 0x0063 */ public: char earningsRating;
  /* 0x0064 */ public: unsigned char trajectory[3];
  /* 0x0067 */ public: unsigned char characteristic;
  /* 0x0068 */ public: unsigned char severity;
  /* 0x0069 */ public: unsigned char chance;
  /* 0x006a */ public: unsigned short pad0;
  /* 0x006c */ public: char baseStats[12];
  /* 0x0078 */ public: char crapStats[12];
  /* 0x0084 */ public: char modLevel[12];
  /* 0x0090 */ public: unsigned char stance[2];
  /* 0x0092 */ public: char available;
  /* 0x0093 */ public: unsigned char pad1;
  /* 0x0094 */ public: unsigned int clubAvailable;
  /* 0x0098 */ public: unsigned int worldRanking;
  /* 0x009c */ public: unsigned int pad2;
  /* 0x00a0 */ public: HashString driver;
  /* 0x00b0 */ public: short driverTint;
  /* 0x00b2 */ public: short driverShaftTint;
  /* 0x00b4 */ public: short driverGripTint;
  /* 0x00b6 */ public: short driverpad;
  /* 0x00b8 */ public: HashString fairway;
  /* 0x00c8 */ public: short fairwayTint;
  /* 0x00ca */ public: short fairwayShaftTint;
  /* 0x00cc */ public: short fairwayGripTint;
  /* 0x00ce */ public: short fairwayPad;
  /* 0x00d0 */ public: HashString putter;
  /* 0x00e0 */ public: short putterTint;
  /* 0x00e2 */ public: short putterShaftTint;
  /* 0x00e4 */ public: short putterGripTint;
  /* 0x00e6 */ public: short putterPad;
  /* 0x00e8 */ public: HashString iron3;
  /* 0x00f8 */ public: HashString iron7;
  /* 0x0108 */ public: short ironTint;
  /* 0x010a */ public: short ironShaftTint;
  /* 0x010c */ public: short ironGripTint;
  /* 0x010e */ public: short ironPad;
  /* 0x0110 */ public: HashString wedge;
  /* 0x0120 */ public: short wedgeTint;
  /* 0x0122 */ public: short wedgeShaftTint;
  /* 0x0124 */ public: short wedgeGripTint;
  /* 0x0126 */ public: short wedgePad;
  /* 0x0128 */ public: HashString crapPresetName;
  /* 0x0138 */ public: HashString shirtNodeName;
  /* 0x0148 */ public: HashString shirt1State;
  /* 0x0158 */ public: HashString shirt2State;
  /* 0x0168 */ public: HashString shirt3State;
  /* 0x0178 */ public: HashString shirt4State;
  /* 0x0188 */ public: short shirt1Tint;
  /* 0x018a */ public: short shirt2Tint;
  /* 0x018c */ public: short shirt3Tint;
  /* 0x018e */ public: short shirt4Tint;
  /* 0x0190 */ public: HashString pantsNodeName;
  /* 0x01a0 */ public: HashString pants1State;
  /* 0x01b0 */ public: HashString pants2State;
  /* 0x01c0 */ public: HashString pants3State;
  /* 0x01d0 */ public: HashString pants4State;
  /* 0x01e0 */ public: short pants1Tint;
  /* 0x01e2 */ public: short pants2Tint;
  /* 0x01e4 */ public: short pants3Tint;
  /* 0x01e6 */ public: short pants4Tint;
  /* 0x01e8 */ public: char driverSound;
  /* 0x01e9 */ public: char fairwaySound;
  /* 0x01ea */ public: char putterSound;
  /* 0x01eb */ public: char iron3Sound;
  /* 0x01ec */ public: char iron7Sound;
  /* 0x01ed */ public: char wedgeSound;
  /* 0x01ee */ public: short pad3;

  public: GolferData_t(const GolferData_t&);
  public: GolferData_t();
};

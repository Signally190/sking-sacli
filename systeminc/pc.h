#ifndef _PC_H_ 
#define _PC_H_

#include "action.h"

#ifdef _NB_断线自动重连
typedef struct
{
	int 大区;
	int 队模;
	int 小区;
	int 人物;
	int 是否自动喊话;
	int 是否自动遇敌;
	int 人物方向;
	char 登陆人物名称[4][32];
	int 登陆延时时间;
}Landed;
#endif

#define MAXCHARACTER			2
#define CHAR_NAME_LEN			16
#define CHAR_FREENAME_LEN		32
#define MAGIC_NAME_LEN			24
#define MAGIC_MEMO_LEN			72
#define ITEM_NAME_LEN			28
#define ITEM_NAME2_LEN			16
#define ITEM_MEMO_LEN			84
#define PET_NAME_LEN			16
#define PET_FREENAME_LEN		32
#define CHAR_FMNAME_LEN			33      // 家族名称

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
#define PROFESSION_MEMO_LEN		84
#endif

#ifdef _GM_IDENTIFY		// Rog ADD GM识别
#define GM_NAME_LEN		        32
#endif

//#define CHARNAMELEN				256		// ???????

#define MAX_PET					5

#define MAX_MAGIC				9

#define MAX_PARTY				5

#define	MAX_ADR_BOOK_COUNT		4
#ifdef _EXTEND_AB
	#define	MAX_ADR_BOOK_PAGE		20//20  //10   20050214 cyg 10 add to 20
#else
	#define	MAX_ADR_BOOK_PAGE		10
#endif
#define MAX_ADR_BOOK			(MAX_ADR_BOOK_COUNT*MAX_ADR_BOOK_PAGE)

#ifdef _ITEM_EQUITSPACE
typedef enum
{
    CHAR_HEAD,               
    CHAR_BODY, 
    CHAR_ARM,
    CHAR_DECORATION1,
    CHAR_DECORATION2,
#ifdef _ITEM_EQUITSPACE
	CHAR_EQBELT,
	CHAR_EQSHIELD,
	CHAR_EQSHOES,
#endif

#ifdef _EQUIT_NEWGLOVE
	CHAR_EQGLOVE,
#endif
	
    CHAR_EQUIPPLACENUM,
#ifdef _PET_ITEM
	PET_HEAD = 0,	// 头
	PET_WING,		// 翼
	PET_TOOTH,		// 牙
	PET_PLATE,		// 身体
	PET_BACK,		// 背
	PET_CLAW,		// 爪
	PET_FOOT,		// 脚(鳍)
	PET_EQUIPNUM
#endif
}CHAR_EquipPlace;

#ifdef _PET_ITEM
typedef enum
{
// 宠物道具,共九种
	ITEM_PET_HEAD = 29,		// 头
	ITEM_PET_WING,			// 翼
	ITEM_PET_TOOTH,			// 牙
	ITEM_PET_PLATE,			// 身体护甲
	ITEM_PET_BACK,			// 背部护甲
	ITEM_PET_CLAW,			// 爪
	ITEM_PET_1_FOOT,		// 脚部,双足
	ITEM_PET_2_FOOT,		// 脚部,四足
	ITEM_PET_FIN,			// 脚部,鳍
	ITEM_CATEGORYNUM  
}ITEM_CATEGORY;
#define MAX_PET_ITEM	7
#endif


#define MAX_ITEMSTART CHAR_EQUIPPLACENUM
#define MAX_MAXHAVEITEM	15
#define MAX_ITEM (MAX_ITEMSTART+MAX_MAXHAVEITEM*3)
#else
#define MAX_ITEMSTART 5
#define MAX_ITEM				20
#endif

#define RESULT_ITEM_COUNT		3
#define RESULT_ITEM_NAME_LEN	24
//#define RESULT_CHR_EXP			4
#define RESULT_CHR_EXP			5

#define SKILL_NAME_LEN			24
#define SKILL_MEMO_LEN			72
#define MAX_SKILL				7


//#define MAX_GOLD				1000000
//#define MAX_BANKGOLD			10000000
//#define MAX_FMBANKGOLD			100000000

#define MAX_GOLD				100000000
#define MAX_BANKGOLD			1000000000
#define MAX_FMBANKGOLD			1000000000

#define MAX_PERSONAL_BANKGOLD 50000000

#ifdef _FMVER21
#define FAMILY_MAXMEMBER                100     // 家族人数
#else
#define FAMILY_MAXMEMBER                50     // 家族人数
#endif

enum
{
	PC_ETCFLAG_PARTY		= ( 1 << 0 ),
	PC_ETCFLAG_JOINT_BTL	= ( 1 << 1 ),
	PC_ETCFLAG_DUEL			= ( 1 << 2 ),
	PC_ETCFLAG_CHAT_MODE	= ( 1 << 3 ),		//队伍频道开关
	PC_ETCFLAG_MAIL			= ( 1 << 4 ),
	PC_ETCFLAG_TRADE		= ( 1 << 5 )
#ifdef _CHANNEL_MODIFY
	,PC_ETCFLAG_CHAT_TELL   = ( 1 << 6 )			//密语频道开关
	,PC_ETCFLAG_CHAT_FM		= ( 1 << 7 )			//家族频道开关
	,PC_ETCFLAG_CHAT_OCC	= ( 1 << 8 )			//职业频道开关
	,PC_ETCFLAG_CHAT_SAVE	= ( 1 << 9 )			//对话储存开关
	,PC_ETCFLAG_CHAT_CHAT	= ( 1 << 10 )			//聊天室开关
#endif
#ifdef _CHATWORLDWIDEPROTOCOL
	, PC_ETCFLAG_CHAT_WW = ( 1 << 11 )			//世界频道开关
#endif
#ifdef _NB_CHANNEL_ALLSERV
	, PC_ETCFLAG_CHAT_ALLSERV = ( 1 << 12 )			//星球频道开关
#endif

#ifdef _AUCPROTOCOL				// (不可开) Syu ADD 拍卖频道开关Protocol
	,PC_ETCFLAG_AUC			= ( 1 << 13 )
#endif
	,PC_AI_MOD = (1 << 14)
};


#define ITEM_FLAG_PET_MAIL		( 1 << 0 )
#define ITEM_FLAG_MIX			( 1 << 1 )
#define ITEM_FLAG_COOKING_MIX	( 1 << 2 )
#define ITEM_FLAG_METAL_MIX		( 1 << 3 )	//金属
#define ITEM_FLAG_JEWEL_MIX		( 1 << 4 )	//宝石
#define ITEM_FLAG_FIX_MIX		( 1 << 5 )	//修复
#ifdef _ITEM_INTENSIFY
#define ITEM_FLAG_INTENSIFY_MIX ( 1 << 6 )	//强化
#endif
#ifdef _ITEM_UPINSLAY
#define ITEM_FLAG_UPINSLAY_MIX ( 1 << 7 )	//凿孔
#endif

typedef struct
{
	int color;						// 道具颜色
	int graNo;						// 道具图
	int level;						// 装备等级
#ifdef _ITEM_PILENUMS
	int pile;						// 堆叠数量
#endif
#ifdef _ALCHEMIST //#ifdef _ITEMSET7_TXT
	char alch[4+200];
#endif
	short useFlag;					// 可否使用
	short field;					// 使用场景
	short target;					// 使用类型
	short deadTargetFlag;			// 
	short sendFlag;					// 使用对像
	char name[ITEM_NAME_LEN+1];		// 道具名字
	char name2[ITEM_NAME2_LEN+1];	// 道具名字2
	char memo[ITEM_MEMO_LEN+1];		// 道具说明
	char damage[16];				// 耐久度
#ifdef _PET_ITEM
	char type;						// 类型编号
#endif
} ITEM;

typedef struct
{
	int graNo;
	int faceGraNo;
	int id;
	int dir;
	int hp, maxHp;
	int mp, maxMp;
	int vital;
	int str, tgh, dex;
	int exp, maxExp;
	int level;
	int atk, def;
	int quick, charm, luck;
	int earth, water, fire, wind;
	int gold;
#ifdef _NEW_MANOR_LAW
	int fame;
#endif
	int titleNo;
	int dp;
	char name[CHAR_NAME_LEN+1];
	char freeName[CHAR_FREENAME_LEN+1];
	short nameColor;
#ifdef _ANGEL_SUMMON
	unsigned status;
#else
	unsigned short status;
#endif
	unsigned short etcFlag;
	short battlePetNo;
	short selectPetNo[MAX_PET];
	short mailPetNo;
#ifdef _STANDBYPET
	short standbyPet;
#endif
	int battleNo;
	short sideNo;
	short helpMode;
	ITEM item[MAX_ITEM];
	ACTION *ptAct;
	int pcNameColor;
	short transmigration;
	char familyName[CHAR_NAME_LEN+1];
	int familyleader;
	int channel;
	int quickChannel;
	int personal_bankgold;
	int ridePetNo;
	int learnride;
	unsigned int lowsride;
	char ridePetName[CHAR_FREENAME_LEN+1];
	int ridePetLevel;
	int familySprite;
	int baseGraNo;
	ITEM itempool[MAX_ITEM];
	int big4fm;
	int trade_confirm;         // 1 -> 初始值
	                           // 2 -> 慬我方按下??键
	                           // 3 -> 仅对方按下??键
	                           // 4 -> 双方皆按下??键

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	int profession_class;
	int profession_level;
//	int profession_exp;
	int profession_skill_point;	
	char profession_class_name[32];
#endif	
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	int herofloor;
#endif

#ifdef _GM_IDENTIFY		// Rog ADD GM识别
	char gm_name[GM_NAME_LEN+1];
#endif

#ifdef _FRIENDCHANNEL  // ROG ADD 好友频道
	char  chatRoomNum[4];
#endif
#ifdef _STREET_VENDOR
	int iOnStreetVendor;		// 摆摊模式
#endif
	int skywalker; // GM天行者

#ifdef _NB_文字称号
	char nb_name[64];
	int nb_namecolor;
#endif
} PC;

#ifdef _FMVER21
enum
{
	FMMEMBER_NONE   = -1,  // 未加入任何家族
    FMMEMBER_MEMBER = 1,   // 一般成员
	FMMEMBER_APPLY,        // 申请加入家族
    FMMEMBER_LEADER,       // 家族族长        
	FMMEMBER_ELDER,        // 长老
    //FMMEMBER_INVITE,     // 祭司
    //FMMEMBER_BAILEE,     // 财务长
    //FMMEMBER_VICELEADER, // 副族长
	FMMEMBER_NUMBER,
};
#endif

enum
{
	MAGIC_FIELD_ALL,
	MAGIC_FIELD_BATTLE,
	MAGIC_FIELD_MAP
};

enum
{
	MAGIC_TARGET_MYSELF,
	MAGIC_TARGET_OTHER,
	MAGIC_TARGET_ALLMYSIDE,
	MAGIC_TARGET_ALLOTHERSIDE,
	MAGIC_TARGET_ALL,
	MAGIC_TARGET_NONE,
	MAGIC_TARGET_OTHERWITHOUTMYSELF,
	MAGIC_TARGET_WITHOUTMYSELFANDPET,
	MAGIC_TARGET_WHOLEOTHERSIDE,
	#ifdef __ATTACK_MAGIC
	MAGIC_TARGET_SINGLE,				// 针对敌方某一方
	MAGIC_TARGET_ONE_ROW,				// 针对敌方某一列
	MAGIC_TARGET_ALL_ROWS,				// 针对敌方所有人
	#endif
};

enum
{
	PETSKILL_FIELD_ALL,
	PETSKILL_FIELD_BATTLE,
	PETSKILL_FIELD_MAP
};

enum
{
	PETSKILL_TARGET_MYSELF,
	PETSKILL_TARGET_OTHER,
	PETSKILL_TARGET_ALLMYSIDE,
	PETSKILL_TARGET_ALLOTHERSIDE,
	PETSKILL_TARGET_ALL,
	PETSKILL_TARGET_NONE,
	PETSKILL_TARGET_OTHERWITHOUTMYSELF,
	PETSKILL_TARGET_WITHOUTMYSELFANDPET
#ifdef _BATTLESKILL				// (不可开) Syu ADD 战斗技能介面
	,PETSKILL_TARGET_ONE_ROW
	,PETSKILL_TARGET_ONE_LINE
	,PETSKILL_TARGER_DEATH
#endif
#ifdef _SKILL_ADDBARRIER
	,PETSKILL_TARGET_ONE_ROW_ALL //选我方的单排
#endif
	, PETSKILL_TARGET_WHOLEOTHERSIDE, //我方全部 或 对方全部
};

enum
{
	ITEM_FIELD_ALL,
	ITEM_FIELD_BATTLE,
	ITEM_FIELD_MAP,
};

enum
{
	ITEM_TARGET_MYSELF,
	ITEM_TARGET_OTHER,
	ITEM_TARGET_ALLMYSIDE,
	ITEM_TARGET_ALLOTHERSIDE,
	ITEM_TARGET_ALL,
	ITEM_TARGET_NONE,
	ITEM_TARGET_OTHERWITHOUTMYSELF,
	ITEM_TARGET_WITHOUTMYSELFANDPET,
#ifdef _PET_ITEM
	ITEM_TARGET_PET
#endif
};

typedef struct
{
	int index;						// 位置
	int graNo;						// 图像
	int hp, maxHp;					// HP,最大HP
	int mp, maxMp;					// MP,最大MP
	int exp, maxExp;				// EXP,最大EXP
	int level;						// 等级
	int atk, def,quick;				// 攻击,防御,敏捷
	int ai;							// 忠诚
	int earth, water, fire, wind;	// 地水火风属性
	int maxSkill;					// 最大技能
	int trn;						// 宠物转生数
#ifdef _SHOW_FUSION
	int fusion;						// low word: 宠蛋旗标, hi word: 物种编码
#endif
#ifdef _ANGEL_SUMMON
	unsigned status;				// 状态
#else
	unsigned short status;			// 状态
#endif
	char name[CHAR_NAME_LEN+1];		// 名字
	char freeName[PET_NAME_LEN+1];	// 昵称
	short useFlag;					// 变量
	short changeNameFlag;			// 改名变量
#ifdef _PET_ITEM
	ITEM item[MAX_PET_ITEM];		// 宠物道具
#endif

#ifdef _NB_宠物绑定值
	int petlock;
#endif
    int petindex;					//宠物索引
#ifdef _NB_宠物初始值
    int initlevel;					//宠物初始等级
    int inithp;						//宠物初始血
    int initatk;					//宠物初始攻
    int initdef;					//宠物初始防
    int initquick;					//宠物初始敏
	int repetnum;					//回炉次数
#endif

	int zfhp;						//血祝福
	int zfatk;						//攻祝福
	int zfdef;						//防祝福
	int zfquick;					//敏祝福
	int zfpz1;						//血品质
	int zfpz2;						//攻品质
	int zfpz3;						//防品质
	int zfpz4;						//敏品质


	int hit_rate;						//敏品质
	int crit_rate;						//敏品质
	int avoid_rate;						//敏品质

	int DATAHP, DATASTR, DATATGH, DATADEX;
	double SDATAVIT, SDATASTR, SDATATGH, SDATADEX, SDATAALL;
	int DATARARE, DATARANK;

	int reset;
} PET;


typedef struct
{
	short useFlag;
	int mp;
	short field;
	short target;
	short deadTargetFlag;
	char name[MAGIC_NAME_LEN+1];
	char memo[MAGIC_MEMO_LEN+1];
} MAGIC;


typedef struct
{
	short useFlag;
	int id;
	int level;
	int maxHp;
	int hp;
	int mp;
	char name[CHAR_NAME_LEN+1];
	ACTION *ptAct;
} PARTY;


typedef struct
{
	short useFlag;
	short onlineFlag;
	int level;
	short transmigration;
	int dp;
	int graNo;
	char name[CHAR_NAME_LEN+1];
#ifdef _MAILSHOWPLANET				// (可开放) Syu ADD 显示名片星球
	char planetname[64];
#endif
} ADDRESS_BOOK;


typedef struct
{
	short petNo;
	short levelUp;
	int exp;
} BATTLE_RESULT_CHR;

typedef struct
{
	short useFlag;
	BATTLE_RESULT_CHR resChr[RESULT_CHR_EXP];
	char item[RESULT_ITEM_COUNT][RESULT_ITEM_NAME_LEN+1];
} BATTLE_RESULT_MSG;


typedef struct
{
	short useFlag;
	short skillId;
	short field;
	short target;
	char name[SKILL_NAME_LEN+1];
	char memo[SKILL_MEMO_LEN+1];
} PET_SKILL;

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
typedef struct
{
	short useFlag;
	short skillId;
	short target;
	short kind;
	char name[PROFESSION_MEMO_LEN+1];
	char memo[PROFESSION_MEMO_LEN+1];
	int icon;
	int costmp;
	int skill_level;
#ifdef _PRO3_ADDSKILL
	int cooltime;
#endif
} PROFESSION_SKILL;
#endif


typedef struct
{
	char name[CHAR_NAME_LEN+1];		// SJIS?????
	short level;					// ???
	int login;						// ??????

	int faceGraNo;					// ????
	int hp;							// ??
	int str;						// ???
	int def;						// ??
	int agi;						// ?
	int app;						// ?
	int attr[4];					// ?佋???????
	int dp;							// ????????
	//玩家所持和种货币数量
} CHARLISTTABLE;


extern PC pc;
extern short maxPcNo;
extern short selectPcNo;
extern short prSendMode;
extern short prSendFlag;
extern short jbSendFlag;
extern short duelSendFlag;

extern int loginDp;

extern short helpFlag;

extern short tradeFlag;
extern short tradeStatus;

//extern short channel;

extern MAGIC magic[];

extern PET pet[];

extern PARTY party[];
extern short partyModeFlag;

extern ADDRESS_BOOK addressBook[];
extern BOOL addressBookFlag;


extern BATTLE_RESULT_MSG battleResultMsg;

extern PET_SKILL petSkill[][MAX_SKILL];

extern CHARLISTTABLE chartable[];

extern char newCharacterName[];
extern int newCharacterGraNo;
extern int newCharacterFaceGraNo;
extern int newCharacterVit;
extern int newCharacterStr;
extern int newCharacterTgh;
extern int newCharacterDex;
extern int newCharacterEarth;
extern int newCharacterWater;
extern int newCharacterFire;
extern int newCharacterWind;
extern int newCharacterHomeTown;

void initPcAll( void );
void initPc( void );
void createPc( int, int, int, int );
void resetPc( void );
void setPcWarpPoint( int, int );
void setPcPoint( void );
void setPcGraNo( int, int );
void setPcId( int );
void setPcDir( int );
void setPcAction( int );
#ifdef __EMOTION
	void setPcEmotion( int emotion );
#endif
int getPcAction( void );

#ifdef _NB_文字称号
#ifdef _PRO3_ADDSKILL
    #define MAX_PROFESSION_SKILL	30
#else
    #define MAX_PROFESSION_SKILL	26
#endif
	extern PROFESSION_SKILL profession_skill[];
	void setPcParam( char *, char *, int, char *, int, int, int, int, int, int, int , int, char *,int);
#else

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
#ifdef _PRO3_ADDSKILL
    #define MAX_PROFESSION_SKILL	30
#else
    #define MAX_PROFESSION_SKILL	26
#endif
    extern PROFESSION_SKILL profession_skill[];
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	void setPcParam( char *, char *, int, char *, int, int, int, int, int, int, int , int );
#else
    void setPcParam( char *, char *, int, char *, int, int, int, int, int, int, int );
#endif
//    #endif
#else
    void setPcParam( char *, char *, int, char *, int, int, int, int );
#endif
#endif

void updataPcAct( void );
void setPcLeader( void );
void delPcLeader( void );
void setPcParty( void );
void delPcParty( void );
void setPcWatch( void );
void delPcWatch( void );
void changePcAct( int, int, int, int, int, int, int );
void setPcWalkFlag( void );
void delPcWalkFlag( void );
int checkPcWalkFlag( void );
void setPcUseMagic( void );
void delPcUseMagic( void );
void setPcFukidashi( unsigned int );
void setPcNameColor( int );

void clearPartyParam( void );
void clearPtActPartyParam( void );

int existCharacterListEntry( int );
int cmpNameCharacterList( char * );
int setCharacterList( char *, char * );
int resetCharacterList( int );

void getItem( void );
void swapItem( int, int );

BOOL lookAtAround( void );


void initAddressBook( void );
void addressBookProc( void );

int CHAR_getMaxHaveGold( void);

#endif

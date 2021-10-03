#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "../systeminc/font.h"
#include "../caryIme.h"
#include "../systeminc/anim_tbl.h"
#include "../systeminc/pc.h"
#include "../systeminc/battleMenu.h"
#include "../systeminc/battleProc.h"
#include "../systeminc/lssproto_cli.h"
#include "../systeminc/netmain.h"
#include "../systeminc/loadsprbin.h"
#include "../systeminc/savedata.h"
#include "../systeminc/t_music.h"
#include "../systeminc/menu.h"
#include "../systeminc/tool.h"
#include "../systeminc/map.h"
#include "../systeminc/field.h"
#include "../systeminc/pet_skillinfo.h"
#include "../wgs/descrypt.h"
#include "../systeminc/character.h"
#ifdef _TALK_WINDOW
#include "../systeminc/talkwindow.h"
#endif
#include "../systeminc/JL_func.h"
#include "../image.h"
#include "../PetRanking.h"


#ifdef _NEWPANEL //Syu ADD 7.0 [신 캐릭터 상태 인터페이스]

int 繁体开关 = FALSE;			//[번체자 on/off]
int 人物屏蔽开关 = FALSE;		//[캐릭터 차단 on/off]


#ifdef _NB_EXPSHOW
int 状态显示开关 = FALSE;		//[상태 표시 on/off]
#endif

#ifdef _NB_CALLGCGC				
int 原地遇敌 = FALSE;			//[원 위치(/현 위치) 적군 조우]
#endif

#ifdef _NB_宠物栏详情			//[펫 상태pet status 세부정보]
int 选中宠物 = -1;				//[선택된 펫]
int 宠物技能选中ID = -1;		//[펫 스킬 선택][펫 스킬 선택]
int 宠物状态索引 = 0;			//[펫 상태 색인(/목록)]
#endif


#ifdef _NB_人物栏详情			//[캐릭터 status세부 정보]
bool 更改昵称判断 = false;		//[닉네임 변경 판단]
#endif

#ifdef _SYUTEST3
#define MENU_STATUS_0   25
#else
#define MENU_STATUS_0   20
#endif
#else
#define MENU_STATUS_0	12
#endif
#ifdef _DROPPETWND					// [공개 가능함] Syu ADD [펫 버리기/drop 확인] 
#define MENU_PET_0		22
#else
#define MENU_PET_0		20
#endif
#define MENU_ITEM_0		40
#define MENU_MAIL_0		40
#define MENU_ALBUM_0	20
#ifdef _TRADESYSTEM2				// [공개 불가] Syu ADD [새로운 거래 시스템]
#define MENU_TRADE_0	22
#endif
#define MENU_BANK_0		20
#define MENU_BTL_RESULT_0	1
//Terry add 2003/11/19
extern bool g_bTradesystemOpen;
//end
#ifdef _READ16BITBMP
extern BOOL g_bUseAlpha;
#endif
#ifdef _NEW_COLOR_
extern BOOL g_bUseAlpha;
#endif
// ????
#define MAX_CHAT_REGISTY_STR		8		// ?????????
#define MAX_CHAT_REGISTY_STR_LEN	26 		// ??????????

#ifdef _DROPPETWND						// [공개 가능함] Syu ADD [펫 버리기/drop 확인]
bool DropPetWndflag = false;			//[펫 버리기/drop가 윈도우/창 에서 디스플레이되는지 flag]
short DropI = -1;						//[i값 임시저장]
#endif


bool ChangePetName = false;		//[펫 이름 변경]
short ChangePetI = -1;

bool ChangeMyName = false;			//[캐릭터 이름 변경]


#ifdef _NB_秀宠物					//[펫 보기/Show pet] 
static int 秀宠按钮索引[5];			//[펫 보기 버튼 색인/목록]
#endif

#ifdef _NB_道具自动叠加				//[아이템 자동 겹치기(/합성/강화)]
static int 道具叠加按钮;			//[아이템 겹치기(/합성/강화) 버튼]
BOOL ItemCanPile(int flg) {
	return (flg & 1 << 6);
}
#endif

#ifdef _NB_断线自动重连				//[연결 해제시 자동 재연결]
extern void ShowBottomLineString(int iColor, LPSTR lpstr);
extern Landed PcLanded;
#endif

#ifdef _AniCrossFrame				// Syu ADD [화면에 표현된 생물을 가로 지르는 에니메이션]
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
int UpDownflag = 0;
#endif

#define CAHT_REGISTY_STR_FILE_NAME 	"data\\chatreg.dat" 	// ???????????
static int systemWndFontNo[MENU_SYSTEM_0]; 			// ????

unsigned int  systemWndNo;						// ??????
STR_BUFFER chatRegistryStr[MAX_CHAT_REGISTY_STR];		// ??????????????
int MouseCursorFlag = TRUE;							// ???????

#ifdef _TRADETALKWND					// [공개 불가] Syu ADD [거래 대화 프레임 새롭게 추가함] 
char talkmsg[4][256];					// [4행의 임시저장 버퍼]
int talkwndx = 300, talkwndy = 350;   //[출발/시작 지점]
bool talkwndflag = false;				//[flag를 입력할 수 있는지?]
bool tradetalkwndflag = false;			//[대화창 flag를 생성할 수 있는지?]
#endif

#ifdef _MONEYINPUT //Syu ADD [금액을 수동으로 입력]
STR_BUFFER MymoneyBuffer;
bool Moneyflag = false;
#endif
STR_BUFFER TradeBuffer;
bool Tradeflag = false;    //[위치 focus flag 입력]
bool TradeBtnflag = false; //[금액 배치/위치 버튼 표시 on/off]]
						   // ???
int mapWndFontNo[MENU_MAP_0]; 	// ????
static unsigned int  mapWndNo;		// ??????
int MapWmdFlagBak;					// ???????????????

class Ranking
{
private:
	int index;
	std::string name;
public:
	int fvit, fstr, ftgh, fdex; // storage initials variable
	double basevit, basestr, basetgh, basedex, baseall, baseplus; // S Grade Data Base
	int svit, sstr, stgh, sdex; // S Grade expected capacity value
	double firstvit, firststr, firsttgh, firstdex; // S Grade State (initial capacity value)
	double upvit, upstr, uptgh, updex, upgrowth; // S Grade Growth rate
	int petgrade, petrank; // the selling price of a pet store, 등 Grade
	double K; //correction factor
	double ret, h, s, t, d;
	char* gradeh, * grades, * gradet, * graded, * gradea;
	int petprice;
	int auction_rank;
	Ranking(const PET& pet)
	{
		Calculate(pet);
	}
	void Calculate(const PET& pet)
	{

		fvit = pet.DATAHP;
		fstr = pet.DATASTR;
		ftgh = pet.DATATGH;
		fdex = pet.DATADEX;

		basevit = pet.SDATAVIT + 4.5f;
		basestr = pet.SDATASTR + 4.5f;
		basetgh = pet.SDATATGH + 4.5f;
		basedex = pet.SDATADEX + 4.5f;
		baseall = pet.SDATAALL;
		baseplus = pet.SDATAVIT + pet.SDATASTR + pet.SDATATGH + pet.SDATADEX;

		int Multiplier;

		switch (pet.DATARARE)
		{
		case 0:
			Multiplier = 1;
			petgrade = 0;
			break;
		case 1:
			Multiplier = 5;
			petgrade = 2;
			break;
		case 2:
			Multiplier = 8;
			petgrade = 1;
			break;
		default:
			Multiplier = pet.DATARARE;
			petgrade = 3;
			break;
		}
		petprice = ((((((pet.level + 1) * pet.level) / 2) - 1) * 20) + 10) * Multiplier;

		petrank = pet.DATARANK;
		if (petrank == 0) K = 4.75f;
		else if (petrank == 1) K = 4.95f;
		else if (petrank == 2) K = 5.15f;
		else if (petrank == 3) K = 5.35f;
		else if (petrank == 4) K = 5.55f;
		else if (petrank == 5) K = 5.75f;

		firstvit = (double)((basevit * 4 + basestr + basetgh + basedex) * baseall / 100);
		firststr = (double)((basevit * 0.1 + basestr + basetgh * 0.1 + basedex * 0.05) * baseall / 100);
		firsttgh = (double)((basevit * 0.1 + basestr * 0.1 + basetgh + basedex * 0.05) * baseall / 100);
		firstdex = (double)(basedex * baseall / 100);

		upvit = (basevit * 4 + basestr + basetgh + basedex) * K / 100;
		upstr = (basevit * 0.1 + basestr + basetgh * 0.1 + basedex * 0.05) * K / 100;
		uptgh = (basevit * 0.1 + basestr * 0.1 + basetgh + basedex * 0.05) * K / 100;
		updex = basedex * K / 100;

		svit = firstvit + upvit * (pet.level - 1) - 0.03;
		sstr = firststr + upstr * (pet.level - 1) - 0.03;
		stgh = firsttgh + uptgh * (pet.level - 1) - 0.03;
		sdex = firstdex + updex * (pet.level - 1) - 0.03;
		upgrowth = upstr + uptgh + updex;

		if (pet.level > 1)
		{
			h = pet.maxHp - (int)firstvit;
			h = h / (pet.level - 1);

			s = pet.atk - (int)firststr;
			s = s / (pet.level - 1);

			t = pet.def - (int)firsttgh;
			t = t / (pet.level - 1);

			d = pet.quick - (int)firstdex;
			d = d / (pet.level - 1);
			ret = s + t + d;
		}
		else
		{
			h = 0;
			s = 0;
			t = 0;
			d = 0;
			ret = 0;
		}

		if (h >= upvit) { gradeh = "S+"; }
		else if (h >= upvit - 0.06) { gradeh = "S"; }
		else if (h >= upvit - 0.12) { gradeh = "A+"; }
		else if (h >= upvit - 0.18) { gradeh = "A"; }
		else if (h >= upvit - 0.24) { gradeh = "B+"; }
		else if (h >= upvit - 0.3) { gradeh = "B"; }
		else if (h >= upvit - 0.36) { gradeh = "C+"; }
		else if (h >= upvit - 0.42) { gradeh = "C"; }
		else if (h >= upvit - 0.48) { gradeh = "D+"; }
		else if (h >= upvit - 0.54) { gradeh = "D"; }
		else if (h < upvit - 0.54) { gradeh = "F"; }

		if (s >= upstr) { grades = "S+"; }
		else if (s >= upstr - 0.01) { grades = "S"; }
		else if (s >= upstr - 0.02) { grades = "A+"; }
		else if (s >= upstr - 0.03) { grades = "A"; }
		else if (s >= upstr - 0.04) { grades = "B+"; }
		else if (s >= upstr - 0.05) { grades = "B"; }
		else if (s >= upstr - 0.06) { grades = "C+"; }
		else if (s >= upstr - 0.07) { grades = "C"; }
		else if (s >= upstr - 0.08) { grades = "D+"; }
		else if (s >= upstr - 0.09) { grades = "D"; }
		else if (s < upstr - 0.09) { grades = "F"; }

		if (t >= uptgh) { gradet = "S+"; }
		else if (t >= uptgh - 0.01) { gradet = "S"; }
		else if (t >= uptgh - 0.02) { gradet = "A+"; }
		else if (t >= uptgh - 0.03) { gradet = "A"; }
		else if (t >= uptgh - 0.04) { gradet = "B+"; }
		else if (t >= uptgh - 0.05) { gradet = "B"; }
		else if (t >= uptgh - 0.06) { gradet = "C+"; }
		else if (t >= uptgh - 0.07) { gradet = "C"; }
		else if (t >= uptgh - 0.08) { gradet = "D+"; }
		else if (t >= uptgh - 0.09) { gradet = "D"; }
		else if (t < uptgh - 0.09) { gradet = "F"; }

		if (d >= updex) { graded = "S+"; }
		else if (d >= updex - 0.01) { graded = "S"; }
		else if (d >= updex - 0.02) { graded = "A+"; }
		else if (d >= updex - 0.03) { graded = "A"; }
		else if (d >= updex - 0.04) { graded = "B+"; }
		else if (d >= updex - 0.05) { graded = "B"; }
		else if (d >= updex - 0.06) { graded = "C+"; }
		else if (d >= updex - 0.07) { graded = "C"; }
		else if (d >= updex - 0.08) { graded = "D+"; }
		else if (d >= updex - 0.09) { graded = "D"; }
		else if (d < updex - 0.09) { graded = "F"; }

		if (ret >= upstr + uptgh + updex) { gradea = "S+"; }
		else if (ret >= upstr + uptgh + updex - 0.03) { gradea = "S"; }
		else if (ret >= upstr + uptgh + updex - 0.06) { gradea = "A+"; }
		else if (ret >= upstr + uptgh + updex - 0.09) { gradea = "A"; }
		else if (ret >= upstr + uptgh + updex - 0.12) { gradea = "B+"; }
		else if (ret >= upstr + uptgh + updex - 0.15) { gradea = "B"; }
		else if (ret >= upstr + uptgh + updex - 0.18) { gradea = "C+"; }
		else if (ret >= upstr + uptgh + updex - 0.21) { gradea = "C"; }
		else if (ret >= upstr + uptgh + updex - 0.24) { gradea = "D+"; }
		else if (ret >= upstr + uptgh + updex - 0.27) { gradea = "D"; }
		else if (ret < upstr + uptgh + updex - 0.27) { gradea = "F"; }
	}
};
									// ?????
static int statusWndFontNo[MENU_STATUS_0]; 	// ????
static int statusWndBtnFlag[MENU_STATUS_0];	// ???????


static int statusWndFontNoExt[256]; 	// ????
static int statusWndBtnFlagExt[256];	// ???????


static unsigned int statusWndNo;				// ??????
STR_BUFFER shougouChange;						// ??????????
int StatusUpPoint;								// ????????????

#ifdef _TRADESYSTEM2	
static int tradeWndFontNo[MENU_TRADE_0]; 	// ????
static int tradeWndBtnFlag[MENU_TRADE_0];	// ???????
#endif
static unsigned int tradeWndNo;				// ??????

int showindex[7] = { 0,0,0,0,0,0,0 };

static char opp_sockfd[128] = "-1";
static char opp_name[128] = "";
static char trade_command[128];
static char trade_kind[128];
static int opp_showindex;
static char opp_goldmount[1024];
static char opp_itemgraph[1024];
static char opp_itemname[1024];
static char opp_itemeffect[1024];
static char opp_itemindex[1024];
static char opp_itemdamage[1024];

struct showitem {
	char name[128];
	char freename[256];
	char graph[128];
	char effect[1024];
	char color[128];
	char itemindex[128];
	char damage[128];
};
static int tradePetIndex = -1;
#ifdef _TRADESYSTEM2	// [공개 불가] Syu ADD [새로운 거래 시스템]
//[1~15는 아군 아이템]，[16~20은 아군 펫]，[21은 아군 돈/골드]
//[22~36은 상대방 아이템]，[37~41은 상대방 펫]，[42는 상대방 돈/골드]

static showitem opp_item[15];	//[거래 아이템 배열을 15개로 늘림]
struct showpet {
	char opp_petname[128];
	char opp_petfreename[128];
	char opp_petgrano[128];
	char opp_petlevel[128];
	char opp_petatk[128];
	char opp_petdef[128];
	char opp_petquick[128];
	char opp_petindex[128];
	char opp_pettrans[128];
	char opp_petshowhp[128];
	char opp_petslot[128];
	char opp_petskill1[128];
	char opp_petskill2[128];
	char opp_petskill3[128];
	char opp_petskill4[128];
	char opp_petskill5[128];
	char opp_petskill6[128];
	char opp_petskill7[128];
#ifdef _SHOW_FUSION
	char opp_fusion[64];
#endif
#ifdef _PET_ITEM
	PetItemInfo oPetItemInfo[MAX_PET_ITEM];			// [펫이 착용한 아이템]
#endif
};											//[상대방이 거래하는 펫의 능력 임시 저장]
static showpet opp_pet[5];

int itemflag[15];

static ACTION *SecondActPet;				//[두번째 윈도우/창 동물 Action]
bool MainTradeWndflag = true;				//[메인 창 디스플레이 여부 flag]
bool SecondTradeWndflag = false;			//[두번째 창에 디스플레이 되는 여부에 대한 flag]
ACTION *SecondTradeWnd;						//[두번째 창]
ACTION *TradeTalkWnd;						//[대화 창]
int ShowPetNum = 0;						//[지금 보이는 펫 번호 스캔]
static int SecondtradeWndFontNo[6];		//[두번째 창의 버튼]
int mytradelist[21] = { -1 };				//[아군 페이지(/거래) 순서 리스트]
int opptradelist[21] = { -1 };				//[상대방 페이지(/거래) 순서 리스트]
int drag1Y = 67, drag2Y = 257;			//[드래그 버튼의 초기 위치]
int tmpdrag1Y, tmpdrag2Y;					//[드래그 시작 위치]
bool dragflag1 = false, dragflag2 = false; //[버튼을 드레그하여 활성화 할지 flag]
int locknum = -1, locknum2 = -1;
#endif
static PET tradePet[2];
static char tradepetindexget[128] = "-1";
#ifdef _PET_ITEM
static bool g_bPetItemWndFlag = false;
#endif

static ACTION *pActPet3; //[거래시 메인 화면 아군 pet action]
static ACTION *pActPet4;
static ACTION *pActPet5;

struct tradelist {
	char kind;
	int data;
	char name[256];
	char damage[256];
	char freename[256];
	int level;
	int trns;
#ifdef _ITEM_PILENUMS
	int pilenum;
#endif
#ifdef _SHOW_FUSION
	int fusion;
#endif
};
#ifdef _TRADESYSTEM2	//  [공개 불가] Syu ADD [새로운 거래 시스템]
static tradelist tradeList[43];
#endif
static int mine_itemindex[2] = { -1, -1 };
static char mine_itemname[2][128];

static int bankGold;
static int bankWndFontNo[MENU_TRADE_0]; 	// ????
static int bankWndBtnFlag[MENU_TRADE_0];	// ???????
static int cashGold = 0;
static int totalGold = 0;
static int bankGoldInc = 0;
static int bankGoldCnt = 0;

#ifdef _TELLCHANNEL				//ROG ADD [귓속말 채널]
BOOL MultiTells = FALSE;
ACTION *pActMsgWnd;
int CharNum = 0;
char TellInfo[10][128];
char name[10][32];
int index[10];
char reSendMsg[STR_BUFFER_SIZE];
int  TalkMode = 0;						//[0: 일반 1: 귓속말 2 팀보이스 3부족 4직업]
char secretName[32] = { "" };				//[비밀스러운 사람/비밀 인물의 이름]
#endif

#ifdef _FRIENDCHANNEL				//ROG ADD [친구 채널]
typedef struct {
	char	roomNo[4];
	char	chiefName[CHAR_NAME_LEN + 1];
	int		chiefIndex;
	int		chiefFlag;
	int		memberNum;
	char    memberName[MAX_ROOM_NUM][CHAR_NAME_LEN + 1];
	int     memberIndex[MAX_ROOM_NUM];
	char    roomName[32];
	char    nickName[MAX_ROOM_NUM][CHAR_FREENAME_LEN + 1];
}CHATINFO;
CHATINFO chatInfo;

bool secretFlag = FALSE;				//[귓속말 flag]
bool BtnType = FALSE;					//[버튼 디스플레이 용도]
bool setRoomFlag = FALSE;
bool assentFlag = FALSE;				//[동의창 flag]
bool scrollFlag = FALSE;
char roomInfo[MAX_ROOM_NUM][64];		//[채팅방 선택]
char memInfo[64];						//member information
int  scrlBtnIndex = 0;					//[스크롤 위치]
int  BtnNo = 0;							//[버튼 번호]
int  firMemNo = 0;						// [첫번째 멤버]
int  selChar = -1;						// [비밀스러운 사람/비밀 인물의 번호]
int  closeBtn, leaveBtn, delBtn, chaBtn, outBtn, scrlHBtn, scrlLBtn; //[버튼 이미지 파일]
int  roomIndex[MAX_ROOM_NUM];
int roomNum = 0;
int memIndex = 0;						//member index
static int ChatRoomBtn[16];			//[채팅방 버튼]
STR_BUFFER chatRoomName;				//[채팅방 이름 설정] 
ACTION *pSetRoomWnd;					//[채팅방 이름 설정]
ACTION *pAssentWnd;						//同意视窗用
ACTION *pChtChanlWnd;					//聊天室视窗用
ACTION *pSelChanlWnd;					//选择聊天室视窗用
#endif

#ifdef _TIMEBAR_FUNCTION			//时间bar函式
ACTION *pTimeBarWnd;
char   timeBarTitle[32];
int    timeBarRange;
int    timeBarCurPos;
bool   timeBarFlag = FALSE;
bool   barHolder[2];                //用来切换时间条
void DrawTimeBar();
/////////TEST/////////////////
int StartTime = 0;
int timBarIdent = -1;
//////////////////////////////
#endif

// ???
static int petWndFontNo[MENU_PET_0]; 	// ????
static int petWndBtnFlag[MENU_PET_0]; // ???????
static unsigned int  petWndNo;			// ?????????
int  petStatusNo;				// ?????????????????
int  mixPetNo;					// ?????????
static ACTION *pActPet;					// ?????????????
BOOL BattlePetReceiveFlag;				// ?????????????
#ifdef _STANDBYPET
BOOL StandbyPetSendFlag = FALSE;
#endif
STR_BUFFER petNameChange;				// ????????
int BattlePetReceivePetNo = -1;			// ?????????
int SelectWazaNo;						// ????

										// ????
static int itemWndFontNo[MENU_ITEM_0]; 	// ????
static int itemWndBtnFlag[MENU_ITEM_0];	// ???????
static int itemWndBtnBag[3];	// ???????
static unsigned int  itemWndNo;
static int  jujutuNo = -1;					// ??????
static int  itemNo = -1;					// ??????????
static int  itemWndDropGold;				// ????
static int  itemWndDropGoldInc;				// ?????
static int  itemWndDropGoldCnt;				// ??????????
int  itemWndMaxBag;				// ??????????
int  itemWndBagOffset;				// ??????????

static int  tradeWndDropGold = 0;				// trade????
static int  tradeWndDropGoldInc = 0;			// trade?????
static int  tradeWndDropGoldCnt = 0;			// trade??????????
static int  tradeWndDropGoldSend = 0;
static int  tradeWndDropGoldGet = 0;
#ifdef _PET_ITEM
static int	nSelectPet;						// 打开道具栏时记录目前作用的宠物
#endif

static ACTION *pActPet2;
BOOL ItemMixRecvFlag;
static int ItemMixPetNo = -1;
static unsigned int ItemUseTime = 0;

ITEM_BUFFER ItemBuffer[MAX_ITEM];


#ifdef _PET_ITEM		
// 每一项的第一个是能装备时的底图图号,第二个是不能装时的底图图号
int nPetItemEquipBmpNumber[PET_EQUIPNUM][2] =
{
	{ 26463, 26470 },{ 26460, 26467 },{ 26458, 26465 },{ 26461, 26468 },{ 26459, 26466 },
	{ 26457, 26464 },{ 26462, 26469 }
};
#endif
// ???
#define MAIL_HISTORY_FILE_NAME 	"data\\mail.dat" 	// ??????
#ifdef _TRANS_6
#ifdef _TRANS_7
char *TransmigrationStr[8] = { "","Ⅰ","Ⅱ","Ⅲ","Ⅳ","Ⅴ","Ⅵ" ,"Ⅶ" };
#else
char *TransmigrationStr[7] = { "","Ⅰ","Ⅱ","Ⅲ","Ⅳ","Ⅴ","Ⅵ" };
#endif
#else
char *TransmigrationStr[6] = { "","Ⅱ","Ⅲ","Ⅳ","Ⅴ" };	//面板显示几转// 
#endif
static int mailWndFontNo[MENU_MAIL_0];
static int mailWndBtnFlag[MENU_MAIL_0];			// ボタン凹凸フラグ
static MAIL_WND_TYPE mailWndNo;						// 現在のメールウィンドウ番号
static MAIL_WND_TYPE mailWndNoBak;					// 前のメールウィンドウ番号
static int mailViewWndPageNo;						// 名刺一覧ウィンドウのページ番号
static int mailSendWndPageNo;						// 普通送信ウィンドウのページ番号
static int mailPetSendWndPageNo;					// ペット送信ウィンドウのページ番号
static int mailItemNo;								// ペット送信ウィンドウのアイテム番号
static int mailWndSendFlag[MAX_ADR_BOOK]; 		// 送信相手記憶フラグ
int mailHistoryWndPageNo;							// 履歴ウィンドウのページ番号
int mailHistoryWndSelectNo;							// 選択されている名刺番号
static int mailHistoryWndNowPageNo;					// 実際に表示する履歴番号
STR_BUFFER MailStr;									// 送る文章
MAIL_HISTORY MailHistory[MAX_ADR_BOOK];			// メールの履歴文字列
ACTION *pActLetter[4];							// 手紙アニメーションアクションポインタ
ACTION *pActMailItem;								// ペットメールで持っていくアイテム

													// ????
#define ALBUM_FILE_NAME 	"data\\album.dat" 	// ????????
#define ALBUM_FILE_NAME_4 	"data\\album_4.dat" 	// ????????
#define ALBUM_FILE_NAME_5 	"data\\album_5.dat" 	// ????????
#define ALBUM_FILE_NAME_6 	"data\\album_6.dat" 	// ????????
#define ALBUM_FILE_NAME_7   "data\\album_7.dat" 	// ????????
#define ALBUM_FILE_NAME_8   "data\\album_8.dat" 	// ????????
#define ALBUM_FILE_NAME_9   "data\\album_9.dat" 	// ????????
#define ALBUM_FILE_NAME_10  "data\\album_10.dat" 	// ????????
#define ALBUM_FILE_NAME_11  "data\\album_11.dat" 	// ????????
#define ALBUM_FILE_NAME_12  "data\\album_12.dat"
#define ALBUM_FILE_NAME_13	"data\\album_13.dat"
#define ALBUM_FILE_NAME_14	"data\\album_14.dat"
#define ALBUM_FILE_NAME_15  "data\\album_14.dat"
#define ALBUM_FILE_NAME_16  "data\\album_15.dat"
#define ALBUM_FILE_NAME_17  "data\\album_16.dat"
#define ALBUM_FILE_NAME_18  "data\\album_17.dat"
#define ALBUM_FILE_NAME_19  "data\\album_18.dat"
#define ALBUM_FILE_NAME_20  "data\\album_19.dat"
#define ALBUM_FILE_NAME_21  "data\\album_20.dat"
#define ALBUM_FILE_NAME_22  "data\\album_21.dat"
#define ALBUM_FILE_NAME_23  "data\\album_22.dat"
#define ALBUM_FILE_NAME_24  "data\\album_23.dat"
#define ALBUM_FILE_NAME_25  "data\\album_24.dat"
#define ALBUM_FILE_NAME_26  "data\\album_25.dat"
#define ALBUM_FILE_NAME_27  "data\\album_26.dat"
#define ALBUM_FILE_NAME_28  "data\\album_27.dat"
#define ALBUM_FILE_NAME_29  "data\\album_28.dat"
#define ALBUM_FILE_NAME_30  "data\\album_29.dat"
#define ALBUM_FILE_NAME_31  "data\\album_30.dat"
#define ALBUM_FILE_NAME_32  "data\\album_31.dat"
#define ALBUM_FILE_NAME_33  "data\\album_32.dat"
#define ALBUM_FILE_NAME_34  "data\\album_33.dat"
#define ALBUM_FILE_NAME_35  "data\\album_34.dat"
#define ALBUM_FILE_NAME_36  "data\\album_35.dat"
#define ALBUM_FILE_NAME_37  "data\\album_36.dat"
#define ALBUM_FILE_NAME_38  "data\\album_37.dat"
#define ALBUM_FILE_NAME_39  "data\\album_38.dat"
#define ALBUM_FILE_NAME_40  "data\\album_39.dat"
#define ALBUM_FILE_NAME_41  "data\\album_40.dat"
#define ALBUM_FILE_NAME_42  "data\\album_41.dat"
#define ALBUM_FILE_NAME_43  "data\\album_42.dat"
#define ALBUM_FILE_NAME_44  "data\\album_43.dat"
#define ALBUM_FILE_NAME_45  "data\\album_44.dat"
#define ALBUM_FILE_NAME_46  "data\\album_45.dat"
#define ALBUM_FILE_NAME_47  "data\\album_46.dat"
#define ALBUM_FILE_NAME_48  "data\\album_47.dat"

static int albumWndFontNo[MENU_ALBUM_0];
static int albumWndBtnFlag[MENU_ALBUM_0];
static unsigned int albumWndNo;
static int albumWndPageNo;					// ページ番号
static int albumNo;						// アルバム番号
										// アルバム用ペットデータテーブル構造体
PET_ALBUM_TBL PetAlbumTbl[] = {
#include "../systeminc/petName.h"	// ???????????
};
PET_ALBUM PetAlbum[MAX_PET_KIND];		// アルバム構造体
int AlbumIdCnt = 0;					// 今回のアルバムファイル読み込み場所カウンタ
									// ＩＤ暗号化テーブル
int IdEncryptionTbl[16] = { 48, 158, 98, 23, 134, 29, 92, 67,
70, 28, 235, 20, 189, 48, 57, 125 };

// 戦闘結果ウィンドウ
int resultWndFontNo[MENU_BTL_RESULT_0];	// 当たり判定番号
int ResultWndTimer;							// 消えるまでの時間タイマー

											// ?????
#define TASK_BAR	8
#define TASK_BAR_X 320
#define TASK_BAR_Y 468 + DISPLACEMENT_Y 
BOOL TaskBarFlag = FALSE;	// タスクバーフラグ
							// Terry add 2003/12/16 for 交易视窗开启时,显示物品说明不显示输入法
BOOL bShowItemExplain = FALSE;
// end
static int taskBarFontNo[TASK_BAR];
static int taskBarX = TASK_BAR_X, taskBarY = TASK_BAR_Y + 24;

// ??????
unsigned int MenuToggleFlag;
#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
#define CHAR_MAX_DETAIL 8
char CharDetail[CHAR_MAX_DETAIL][16];
char DetailDesc[CHAR_MAX_DETAIL][64] = {
	"지 마법 저항성　：" ,
	"수 마법 저항성　：" ,
	"화 마법 저항성　：" ,
	"풍 마법 저항성　：" ,
	"지 마법 숙련도：" ,
	"수 마법 숙련도：" ,
	"화 마법 숙련도：" ,
	"풍 마법 숙련도："
};
char DetailDesc1[CHAR_MAX_DETAIL][64] = {
	"지 정령 마법 저항성，줄일 수 있음" ,
	"수 정령 마법 저항성，줄일 수 있음" ,
	"화 정령 마법 저항성，줄일 수 있음" ,
	"풍 정령 마법 저항성，줄일 수 있음" ,
	"지 정령 마법 숙련도，사용하여 높일 수 있음" ,
	"수 정령 마법 숙련도，사용하여 높일 수 있음" ,
	"화 정령 마법 숙련도，사용하여 높일 수 있음" ,
	"풍 정령 마법 숙련도，사용하여 높일 수 있음"
};
char DetailDesc2[CHAR_MAX_DETAIL][64] = {
	"지 정령 마법의 데미지。" ,
	"수 정령 마법의 데미지。" ,
	"화 정령 마법의 데미지。" ,
	"풍 정령 마법의 데미지。" ,
	"지 정령 마법 시전 위력。" ,
	"수 정령 마법 시전 위력。" ,
	"화 정령 마법 시전 위력。" ,
	"풍 정령 마법 시전 위력。"
};
#endif

#ifdef _ALCHEPLUS
int iCharAlchePlus[25];
char sAlchePlus_list[25][16] =
{
	"돌", "나무",	"뼈", "치아",	"피부",
	"조가비", "껍질", "발톱",	"꽃", "잎",
	"선", "점토", "비늘", "독", "석화",
	"혼돈",	"만취",	"구면",	"회복",	"부활",
	"수정",	"땅", "물",	"불", "바람"
};

int iAlchePlusIcon[25] = { 26536, 26529, 26547, 26534, 26535,	/*"조가비", "껍질", "발톱", "꽃", "잎"*/
26542, 26550, 26533, 26543, 26551,	/*"선", "점토", "비늘", "독", "석화"*/
26553, 26554, 26557, 26544, 26537,	/*"혼돈", "만취", "수면", "회복", "부활"*/
26546, 26548, 26552, 26538, 26549,	/*"混乱",	"酒醉",	"睡眠",	"回复",	"复活"*/
26531, 26539, 26530, 26532, 26545 };	/*""선", "점토", "비늘", "독", "석화"*/
#endif

ACTION *pActMenuWnd;
//[여기는 아이템 창입니다]
ACTION *pActMenuWnd2;
ACTION *pActMenuWnd3;
ACTION *pActMenuWnd4;
ACTION *pActYesNoWnd;
// Bankman
ACTION *pActMenuWnd5;

ACTION *pActMenu皮肤 = NULL;
ACTION *pActMenu称号 = NULL;
ACTION *pActMenu光环 = NULL;


ACTION *pActMenu人物;		//[캐릭터]

void checkRidePet(int);
//andy_add 2002/06/24
int RIDEPET_getNOindex(int baseNo);
int RIDEPET_getPETindex(int PetNo, int learnCode);
int RIDEPET_getRIDEno(int index, int ti);


static char *monoStereoStr[] = { "       모       노       ",
"      스 테 레 오       " };

static char *mouseCursor[] = { "     회    색     ",
"     공 룡 알     " };

//#define WINDOW_CREATE_FRAME 10
int WINDOW_CREATE_FRAME = 1;	//JLWG 窗口动画FRAME

								// ????????????????
extern unsigned char crs_change_tbl[];
// ????????????????
extern unsigned char crs_change_tbl2[];
// ????
extern unsigned char crs_bound_tbl[][32];

#ifdef _STREET_VENDOR
extern short sStreetVendorBuyBtn;
#endif

#ifdef _PET_ITEM
/*	0x01:PET_HEAD	// 头
0x02:PET_WING	// 翼
0x04:PET_TOOTH	// 牙
0x08:PET_PLATE	// 身体
0x10:PET_BACK	// 背
0x20:PET_CLAW	// 爪
0x40:PET_FOOT	// 脚(鳍)
*/
// 不同种的宠物会有能装和不能装的部位
unsigned char byShowPetItemBackground[MAX_PET_SPECIES + 1] =
{
	/*	乌力	布比	拳击手	克克尔	凯比	布克	蝙蝠	乌宝宝	飞天蛙	飞龙*/
	0x5d,	0x5d,	0x7d,	0x5d,	0x5d,	0x7d,	0x5f,	0x5d,	0x5f,	0x7f,
	/*	大舌头	乌龟	鲨鱼	海主人	猪鱼龙	呼拔拔	人龙	三角龙	雷龙	暴龙*/
	0x5d,	0x5d,	0x5d,	0x5d,	0x5d,	0x7d,	0x7d,	0x5d,	0x5d,	0x7d,
	/*	水龙	猩猩	巴克	老虎	长毛象	鸟系	羊系	穿山甲	特殊系	软体系*/
	0x5d,	0x7d,	0x7d,	0x5d,	0x5d,	0x5f,	0x5d,	0x5d,	0x09,	0x1d,
	/*	小鱼	贝壳	棘皮	鱼龙	虾蟹	鲸豚	魟系	海牛系	大鲨鱼	旗鱼系*/
	0x5d,	0x79,	0x1d,	0x5d,	0x79,	0x5d,	0x5d,	0x5d,	0x5d,	0x5d,
	/*	大鱼系	鳄龙系	融合宠*/
	0x5d,	0x5d,	0x00
};
#endif

int charDetailPage = 3;


#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面
bool SkillWndflag = false;
short SkillWnd = 0;
int 职技关闭按钮句柄;
int 职技选项按钮句柄[3];
ACTION *pActSkillMenuWnd;
void SkillWndfunc2() {
#ifdef _NB_人物栏详情
	int 临时图标X, 临时图标Y, 临时起始X, 临时起始Y;
	//计算图片中心
	if (ResoMode == 分辨率::mode800) {
		临时图标X = 800 / 2;
		临时图标Y = 600 / 2;
		临时起始X = 临时图标X - 568 / 2;
		临时起始Y = 临时图标Y - 358 / 2;
	}
	else if (ResoMode == 分辨率::mode1024) {
		临时图标X = 1024 / 2;
		临时图标Y = 768 / 2;
		临时起始X = 临时图标X - 568 / 2;
		临时起始Y = 临时图标Y - 358 / 2;
	}
	else {
		临时图标X = 640 / 2;
		临时图标Y = 480 / 2;
		临时起始X = 临时图标X - 568 / 2;
		临时起始Y = 临时图标Y - 358 / 2;
	}

	int x = 0, y = 0, j = 0;
	char buf[256];
	char msg[256];
	char msg2[256];
	if (pActSkillMenuWnd == NULL) {
		pActSkillMenuWnd = MakeWindowDisp(临时起始X, 临时起始Y, 临时起始X + 568, 临时起始Y + 358, 0, -1);
	}
	else {
		if (pActSkillMenuWnd->hp > 0) {
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (HitDispNo == 职技关闭按钮句柄) {
					SkillWndflag = false;
					DeathAction(pActSkillMenuWnd);
					pActSkillMenuWnd = NULL;
					MenuToggleFlag ^= JOY_CTRL_S;
					play_se(217, 320, 240);
				}
			}

			for (int i = 0; i < 3; i++) {
				int 临时位置X = 305;
				int 临时位置Y = 20;
				int 临时间隔 = 75;
				if (MakeHitBox(临时起始X + 临时位置X + i * 临时间隔, 临时起始Y + 临时位置Y, 临时起始X + 临时位置X + 临时间隔 + i*临时间隔, 临时起始Y + 临时位置Y + 28, DISP_PRIO_IME3) == TRUE) {
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						SkillWnd = i;
						play_se(217, 320, 240);
						break;
					}
				}
			}

			sprintf(msg, "%s", pc.profession_class_name);
			StockFontBuffer(临时起始X + 74, 临时起始Y + 27, FONT_PRIO_FRONT, 1, msg, 0);
			sprintf(msg, "%d", pc.profession_skill_point);
			StockFontBuffer(临时起始X + 254, 临时起始Y + 27, FONT_PRIO_FRONT, 1, msg, 0);

			职技关闭按钮句柄 = StockDispBuffer(临时图标X + 264, 临时图标Y - 180, DISP_PRIO_IME3, IMAGE_窗口关闭X, 1);
			StockDispBuffer(临时图标X, 临时图标Y, DISP_PRIO_MENU, 52041, 1);//打印人物界面底图
			memset(msg, -1, sizeof(msg));

			extern int BattleSkill[20];
			extern int AssitSkill[20];
			extern int AdvanceSkill[20];

			sprintf(msg, " ");
			int 每行几个技能 = 8;
			switch (SkillWnd) {
			case 0:
				StockDispBuffer(临时起始X + 343, 临时起始Y + 35, DISP_PRIO_IME3, 52042, 0);
				for (int i = 0; i < 2; i++) {
					for (j = 0; j < 每行几个技能; j++) {
						if (AssitSkill[j + i * 每行几个技能] == -1)
							break;
						if (HitDispNo == StockDispBuffer(临时起始X + 71 + j * 60, 临时起始Y + 95 + i * 66, DISP_PRIO_IME3 + 1, profession_skill[AssitSkill[j + i * 每行几个技能]].icon, 2)) {
							int use_color = 0;
							if (pc.mp >= profession_skill[AssitSkill[j + i * 每行几个技能]].costmp && profession_skill[AssitSkill[j + i * 每行几个技能]].costmp != 0) {
								use_color = FONT_PAL_WHITE;
							}
							else {
								use_color = FONT_PAL_GRAY;
							}

							sprintf(msg, "%s", profession_skill[AssitSkill[j + i * 每行几个技能]].name);
							StockFontBuffer(临时起始X + 140, 临时起始Y + 220, FONT_PRIO_FRONT, use_color, msg, 0);

							sprintf(msg, "%d%%", profession_skill[AssitSkill[j + i * 每行几个技能]].skill_level);
							StockFontBuffer(临时起始X + 320, 临时起始Y + 220, FONT_PRIO_FRONT, use_color, msg, 0);

							sprintf(msg, "%d", profession_skill[AssitSkill[j + i * 每行几个技能]].costmp);
							StockFontBuffer(临时起始X + 432, 临时起始Y + 220, FONT_PRIO_FRONT, use_color, msg, 0);

							sprintf(msg, "%s", profession_skill[AssitSkill[j + i * 每行几个技能]].memo);
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
							if (mouse.onceState & MOUSE_LEFT_CRICK && profession_skill[AssitSkill[j + i * 每行几个技能]].useFlag == 0) {
								if (pc.mp >= profession_skill[AssitSkill[j + i * 每行几个技能]].costmp && profession_skill[AssitSkill[j + i * 每行几个技能]].costmp != 0) {
									lssproto_BATTLESKILL_send(sockfd, AssitSkill[j + i * 每行几个技能]);
									play_se(217, 320, 240);
								}
							}
#endif
						}
						sprintf(msg2, "%-8s", profession_skill[AssitSkill[j + i * 每行几个技能]].name);
						StockFontBuffer(临时起始X + 48 + j * 60, 临时起始Y + 118 + i * 66, FONT_PRIO_FRONT, profession_skill[BattleSkill[j + i * 每行几个技能]].useFlag + 1, msg2, 0);
					}
				}
				break;
			case 1:
				StockDispBuffer(临时起始X + 418, 临时起始Y + 35, DISP_PRIO_IME3, 52043, 0);
				for (int i = 0; i < 2; i++) {
					for (j = 0; j < 每行几个技能; j++) {
						if (BattleSkill[j + i * 每行几个技能] == -1)
							break;
						if (HitDispNo == StockDispBuffer(临时起始X + 71 + j * 60, 临时起始Y + 95 + i * 66, DISP_PRIO_IME3 + 1, profession_skill[BattleSkill[j + i * 每行几个技能]].icon, 2)) {
							int use_color = 0;
							if (pc.mp >= profession_skill[BattleSkill[j + i * 每行几个技能]].costmp && profession_skill[BattleSkill[j + i * 每行几个技能]].costmp != 0) {
								use_color = FONT_PAL_WHITE;
							}
							else {
								use_color = FONT_PAL_GRAY;
							}

							//鼠标指向
							sprintf(msg, "%s", profession_skill[BattleSkill[j + i * 每行几个技能]].name);
							StockFontBuffer(临时起始X + 140, 临时起始Y + 220, FONT_PRIO_FRONT, use_color, msg, 0);

							sprintf(msg, "%d%%", profession_skill[BattleSkill[j + i * 每行几个技能]].skill_level);//技能熟练 
							StockFontBuffer(临时起始X + 320, 临时起始Y + 220, FONT_PRIO_FRONT, use_color, msg, 0);

							sprintf(msg, "%d", profession_skill[BattleSkill[j + i * 每行几个技能]].costmp);
							StockFontBuffer(临时起始X + 432, 临时起始Y + 220, FONT_PRIO_FRONT, use_color, msg, 0);

							sprintf(msg, "%s", profession_skill[BattleSkill[j + i * 每行几个技能]].memo);
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
							if (mouse.onceState & MOUSE_LEFT_CRICK && profession_skill[BattleSkill[j + i * 每行几个技能]].useFlag == 0) {
								if (pc.mp >= profession_skill[BattleSkill[j + i *每行几个技能]].costmp && profession_skill[BattleSkill[j + i * 每行几个技能]].costmp != 0) {
									lssproto_BATTLESKILL_send(sockfd, BattleSkill[j + i * 每行几个技能]);
									play_se(217, 320, 240);
								}
							}
#endif
						}
						sprintf(msg2, "%-8s", profession_skill[BattleSkill[j + i * 每行几个技能]].name);
						StockFontBuffer(临时起始X + 48 + j * 60, 临时起始Y + 118 + i * 66, FONT_PRIO_FRONT, profession_skill[BattleSkill[j + i * 每行几个技能]].useFlag + 1, msg2, 0);
					}
				}
				break;
			case 2:
				StockDispBuffer(临时起始X + 493, 临时起始Y + 35, DISP_PRIO_IME3, 52044, 0);
				for (int i = 0; i < 2; i++) {
					for (j = 0; j < 每行几个技能; j++) {
						if (AdvanceSkill[j + i * 每行几个技能] == -1)
							break;
						if (HitDispNo == StockDispBuffer(临时起始X + 71 + j * 60, 临时起始Y + 95 + i * 66, DISP_PRIO_IME3 + 1, profession_skill[AdvanceSkill[j + i * 每行几个技能]].icon, 2)) {

							int use_color = 0;
							if (pc.mp >= profession_skill[AdvanceSkill[j + i * 每行几个技能]].costmp && profession_skill[AdvanceSkill[j + i * 每行几个技能]].costmp != 0) {
								use_color = FONT_PAL_WHITE;
							}
							else {
								use_color = FONT_PAL_GRAY;
							}

							sprintf(msg, "%s", profession_skill[AdvanceSkill[j + i * 每行几个技能]].name);
							StockFontBuffer(临时起始X + 140, 临时起始Y + 220, FONT_PRIO_FRONT, use_color, msg, 0);

							sprintf(msg, "%d%%", profession_skill[AdvanceSkill[j + i * 每行几个技能]].skill_level);
							StockFontBuffer(临时起始X + 320, 临时起始Y + 220, FONT_PRIO_FRONT, use_color, msg, 0);

							sprintf(msg, "%d", profession_skill[AdvanceSkill[j + i * 每行几个技能]].costmp);
							StockFontBuffer(临时起始X + 432, 临时起始Y + 220, FONT_PRIO_FRONT, use_color, msg, 0);

							sprintf(msg, "%s", profession_skill[AdvanceSkill[j + i * 每行几个技能]].memo);
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
							if (mouse.onceState & MOUSE_LEFT_CRICK && profession_skill[AdvanceSkill[j + i * 每行几个技能]].useFlag == 0) {
								if (pc.mp >= profession_skill[AdvanceSkill[j + i * 每行几个技能]].costmp && profession_skill[AdvanceSkill[j + i * 每行几个技能]].costmp != 0) {
									lssproto_BATTLESKILL_send(sockfd, AdvanceSkill[j + i * 每行几个技能]);
									play_se(217, 320, 240);
								}
							}
#endif
						}
						sprintf(msg2, "%-8s", profession_skill[AdvanceSkill[j + i * 每行几个技能]].name);
						StockFontBuffer(临时起始X + 48 + j * 60, 临时起始Y + 118 + i * 66, FONT_PRIO_FRONT, profession_skill[BattleSkill[j + i * 每行几个技能]].useFlag + 1, msg2, 0);
					}
				}
				break;
			}
			char *splitPoint = msg;
			sprintf(buf, "%s", msg);
			x = 临时起始X + 144;
			y = 临时起始Y + 263;
			while (1) {
				if (strlen(splitPoint) > 72) {
					strncpy(msg, splitPoint, 72);
					buf[72] = NULL;
					if (GetStrLastByte(buf) == 3) {
						buf[71] = NULL;
						splitPoint += 71;
					}
					else {
						buf[72] = NULL;
						splitPoint += 72;
					}
					StockFontBuffer(x, y, FONT_PRIO_FRONT, 10, buf, 0); y += 20;
				}
				else {
					strcpy(buf, splitPoint);
					StockFontBuffer(x, y, FONT_PRIO_FRONT, 10, buf, 0);
					break;
				}
			}
		}
	}
#else
	int x = 0, y = 0, j = 0;
	char buf[256];
	char msg[256];
	char msg2[256];
	if (pActSkillMenuWnd == NULL) {
		pActSkillMenuWnd = MakeWindowDisp(354, 0, 300, 456, 0, -1);
	}
	else {
		if (pActSkillMenuWnd->hp > 0) {
			StockDispBuffer(((WINDOW_DISP *)pActSkillMenuWnd->pYobi)->mx - 10, ((WINDOW_DISP *)pActSkillMenuWnd->pYobi)->my - 5, DISP_PRIO_MENU, CG_FIELD_SKILL_PANEL, 1);
			x = pActSkillMenuWnd->x + 2;
			y = pActSkillMenuWnd->y - 5;
			statusWndFontNo[15] = StockDispBuffer(x + 201, y + 436, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
			StockFontBuffer(x + 58, y + 36, FONT_PRIO_FRONT, 1, pc.profession_class_name, 0);
			sprintf_s(msg, "%d", pc.profession_skill_point);
			StockFontBuffer(x + 228, y + 36, FONT_PRIO_FRONT, 1, msg, 0);
			memset(msg, -1, sizeof(msg));
		}
	}
	for (int i = 0; i < 3; i++) {
#ifdef _READ16BITBMP
		if (g_bUseAlpha) {
			if (MakeHitBox(x + 254, y + 15 + i * 78, x + 284, y + 93 + i * 78, DISP_PRIO_IME3) == TRUE) {
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					SkillWnd = i;
					play_se(217, 320, 240);
					break;
				}
			}
		}
		else
#endif
			if (MakeHitBox(x + 254, y + 13 + i * 79, x + 284, y + 91 + i * 79, DISP_PRIO_IME3) == TRUE) {
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					SkillWnd = i;
					play_se(217, 320, 240);
					break;
				}
			}
	}
	if (mouse.onceState & MOUSE_LEFT_CRICK) {
		if (HitDispNo == statusWndFontNo[15]) {
			SkillWndflag = false;
			play_se(217, 320, 240);
		}
	}
	extern int BattleSkill[20];
	extern int AssitSkill[20];
	extern int AdvanceSkill[20];
	for (int i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			StockDispBuffer(x + 143 + j * 60, y + 210 + i * 57, DISP_PRIO_IME3, CG_FIELD_SKILL_TILE, 0);
		}
	}
	int 临时图标X;
	if (ResoMode == 分辨率::mode800) {
		临时图标X = 800;
	}
	else if (ResoMode == 分辨率::mode1024) {
		临时图标X = 1024;
	}
	else {
		临时图标X = 640;
	}

	sprintf_s(msg, " ");
	switch (SkillWnd) {
	case 0:
#ifdef _READ16BITBMP
		if (g_bUseAlpha) StockDispBuffer(x + 147, y + 225, DISP_PRIO_IME3, CG_FIELD_SKILL_ASSIT, 0);
		else
#endif
			StockDispBuffer(x + 143, y + 223, DISP_PRIO_IME3, CG_FIELD_SKILL_ASSIT, 0);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (AssitSkill[j + i * 4] == -1)
					break;
				if (HitDispNo == StockDispBuffer(x + 27 + j * 60, y + 102 + i * 57, DISP_PRIO_IME3 + 1, profession_skill[AssitSkill[j + i * 4]].icon, 2)) {

					int use_color = 0;
					if (pc.mp >= profession_skill[AssitSkill[j + i * 4]].costmp && profession_skill[AssitSkill[j + i * 4]].costmp != 0) {
						use_color = FONT_PAL_WHITE;
					}
					else {
						use_color = FONT_PAL_GRAY;
					}
					/*
					sprintf_s( msg , "%s  %d％  (耗费MP:%d)"
					, profession_skill[AssitSkill [ j + i * 4 ]].name
					, profession_skill[AssitSkill [ j + i * 4 ]].skill_level
					, profession_skill[AssitSkill [ j + i * 4 ]].costmp
					);
					StockFontBuffer( 临时图标X - 285, 306, FONT_PRIO_FRONT, use_color, msg, 0 );
					*/
					sprintf_s(msg, "%s", profession_skill[AssitSkill[j + i * 4]].name);
					StockFontBuffer(临时图标X - 285, 306, FONT_PRIO_FRONT, use_color, msg, 0);

					sprintf_s(msg, "%d％", profession_skill[AssitSkill[j + i * 4]].skill_level);
					StockFontBuffer(临时图标X - 200, 306, FONT_PRIO_FRONT, use_color, msg, 0);

					sprintf_s(msg, "(비용MP:%d)", profession_skill[AssitSkill[j + i * 4]].costmp);
					StockFontBuffer(临时图标X - 110, 306, FONT_PRIO_FRONT, use_color, msg, 0);

					sprintf_s(msg, "%s", profession_skill[AssitSkill[j + i * 4]].memo);
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
					if (mouse.onceState & MOUSE_LEFT_CRICK && profession_skill[AssitSkill[j + i * 4]].useFlag == 0) {
						if (pc.mp >= profession_skill[AssitSkill[j + i * 4]].costmp && profession_skill[AssitSkill[j + i * 4]].costmp != 0) {
							lssproto_BATTLESKILL_send(sockfd, AssitSkill[j + i * 4]);
							play_se(217, 320, 240);
						}
					}
#endif
				}
				sprintf_s(msg2, "%8s", profession_skill[AssitSkill[j + i * 4]].name);
				StockFontBuffer(x + j * 60, y + 110 + i * 57, FONT_PRIO_FRONT, profession_skill[AssitSkill[j + i * 4]].useFlag + 1, msg2, 0);
			}
		}
		break;
	case 1:
#ifdef _READ16BITBMP
		if (g_bUseAlpha) StockDispBuffer(x + 147, y + 225, DISP_PRIO_IME3, CG_FIELD_SKILL_BATTLE, 0);
		else
#endif
			StockDispBuffer(x + 143, y + 223, DISP_PRIO_IME3, CG_FIELD_SKILL_BATTLE, 0);
		for (int i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (BattleSkill[j + i * 4] == -1)
					break;
				if (HitDispNo == StockDispBuffer(x + 27 + j * 60, y + 102 + i * 57, DISP_PRIO_IME3 + 1, profession_skill[BattleSkill[j + i * 4]].icon, 2)) {

					int use_color = 0;
					if (pc.mp >= profession_skill[BattleSkill[j + i * 4]].costmp && profession_skill[BattleSkill[j + i * 4]].costmp != 0) {
						use_color = FONT_PAL_WHITE;
					}
					else {
						use_color = FONT_PAL_GRAY;
					}
					/*
					sprintf_s( msg , "%s  %d％  (耗费MP:%d)"
					, profession_skill[BattleSkill [ j + i * 4 ]].name
					, profession_skill[BattleSkill [ j + i * 4 ]].skill_level
					, profession_skill[BattleSkill [ j + i * 4 ]].costmp
					);
					StockFontBuffer( 临时图标X - 285, 306, FONT_PRIO_FRONT, use_color, msg, 0 );
					*/
					sprintf_s(msg, "%s", profession_skill[BattleSkill[j + i * 4]].name);
					StockFontBuffer(临时图标X - 285, 306, FONT_PRIO_FRONT, use_color, msg, 0);

					sprintf_s(msg, "%d％", profession_skill[BattleSkill[j + i * 4]].skill_level);
					StockFontBuffer(临时图标X - 200, 306, FONT_PRIO_FRONT, use_color, msg, 0);

					sprintf_s(msg, "(耗费MP:%d)", profession_skill[BattleSkill[j + i * 4]].costmp);
					StockFontBuffer(临时图标X - 110, 306, FONT_PRIO_FRONT, use_color, msg, 0);

					sprintf_s(msg, "%s", profession_skill[BattleSkill[j + i * 4]].memo);
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
					if (mouse.onceState & MOUSE_LEFT_CRICK && profession_skill[BattleSkill[j + i * 4]].useFlag == 0) {
						if (pc.mp >= profession_skill[BattleSkill[j + i * 4]].costmp && profession_skill[BattleSkill[j + i * 4]].costmp != 0) {
							lssproto_BATTLESKILL_send(sockfd, BattleSkill[j + i * 4]);
							play_se(217, 320, 240);
						}
					}
#endif
				}
				sprintf_s(msg2, "%8s", profession_skill[BattleSkill[j + i * 4]].name);
				StockFontBuffer(x + j * 60, y + 110 + i * 57, FONT_PRIO_FRONT, profession_skill[BattleSkill[j + i * 4]].useFlag + 1, msg2, 0);
			}
		}
		break;
	case 2:
#ifdef _READ16BITBMP
		if (g_bUseAlpha) StockDispBuffer(x + 147, y + 225, DISP_PRIO_IME3, CG_FIELD_SKILL_ADVSK, 0);
		else
#endif
			StockDispBuffer(x + 143, y + 223, DISP_PRIO_IME3, CG_FIELD_SKILL_ADVSK, 0);
		for (int i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (AdvanceSkill[j + i * 4] == -1)
					break;
				if (HitDispNo == StockDispBuffer(x + 27 + j * 60, y + 102 + i * 57, DISP_PRIO_IME3 + 1, profession_skill[AdvanceSkill[j + i * 4]].icon, 2)) {

					int use_color = 0;
					if (pc.mp >= profession_skill[AdvanceSkill[j + i * 4]].costmp && profession_skill[AdvanceSkill[j + i * 4]].costmp != 0) {
						use_color = FONT_PAL_WHITE;
					}
					else {
						use_color = FONT_PAL_GRAY;
					}
					/*
					sprintf_s( msg , "%s  %d％  (耗费MP:%d)"
					, profession_skill[AdvanceSkill [ j + i * 4 ]].name
					, profession_skill[AdvanceSkill [ j + i * 4 ]].skill_level
					, profession_skill[AdvanceSkill [ j + i * 4 ]].costmp
					);
					StockFontBuffer( 临时图标X - 285, 306, FONT_PRIO_FRONT, use_color, msg, 0 );
					*/
					sprintf_s(msg, "%s", profession_skill[AdvanceSkill[j + i * 4]].name);
					StockFontBuffer(临时图标X - 285, 306, FONT_PRIO_FRONT, use_color, msg, 0);

					sprintf_s(msg, "%d％", profession_skill[AdvanceSkill[j + i * 4]].skill_level);
					StockFontBuffer(临时图标X - 200, 306, FONT_PRIO_FRONT, use_color, msg, 0);

					sprintf_s(msg, "(耗费MP:%d)", profession_skill[AdvanceSkill[j + i * 4]].costmp);
					StockFontBuffer(临时图标X - 110, 306, FONT_PRIO_FRONT, use_color, msg, 0);

					sprintf_s(msg, "%s", profession_skill[AdvanceSkill[j + i * 4]].memo);
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
					if (mouse.onceState & MOUSE_LEFT_CRICK && profession_skill[AdvanceSkill[j + i * 4]].useFlag == 0) {
						if (pc.mp >= profession_skill[AdvanceSkill[j + i * 4]].costmp && profession_skill[AdvanceSkill[j + i * 4]].costmp != 0) {
							lssproto_BATTLESKILL_send(sockfd, AdvanceSkill[j + i * 4]);
							play_se(217, 320, 240);
						}
					}
#endif
				}
				sprintf_s(msg2, "%8s", profession_skill[AdvanceSkill[j + i * 4]].name);
				StockFontBuffer(x + j * 60, y + 110 + i * 57, FONT_PRIO_FRONT, profession_skill[AdvanceSkill[j + i * 4]].useFlag + 1, msg2, 0);
			}
		}
		break;
	}
	char *splitPoint = msg;
	sprintf_s(buf, "%s", msg);
	x = 临时图标X - 285;
	y = 340;
	while (1) {
		if (strlen(splitPoint) > 34) {
			strncpy_s(msg, splitPoint, 34);
			buf[34] = NULL;
			if (GetStrLastByte(buf) == 3) {
				buf[33] = NULL;
				splitPoint += 33;
			}
			else {
				buf[34] = NULL;
				splitPoint += 34;
			}
			StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, buf, 0); y += 24;
		}
		else {
			strcpy(buf, splitPoint);
			StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, buf, 0);
			break;
		}
	}

#endif
}
#endif

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
void TradeTalk(char *msg) {
	char buf[256];
	if (pActMenuWnd4 != NULL || SecondTradeWnd != NULL) {
		sprintf(buf, "%s", msg);
		//判断是否由买卖双方说出
		if (strstr(buf, pc.name) || strstr(buf, opp_name)) {
			//内容转移
			while (1) {
				if (strlen(msg) > 44) {
					strncpy(buf, msg, 44);
					buf[44] = NULL;
					if (GetStrLastByte(buf) == 3) {
						buf[43] = NULL;
						msg += 43;
					}
					else {
						buf[44] = NULL;
						msg += 44;
					}
					for (int i = 0; i < 3; i++)
						strcpy(talkmsg[i], talkmsg[i + 1]);
					sprintf(talkmsg[3], "%s", buf);
				}
				else {
					for (int i = 0; i < 3; i++)
						strcpy(talkmsg[i], talkmsg[i + 1]);
					strcpy(buf, msg);
					sprintf(talkmsg[3], "%s", buf);
					break;
				}
			}
			tradetalkwndflag = true;
		}
	}
}
#endif
void LockAndOkfunction()
{
	char buffer[1024];
	char myitembuff[1024];
	char mypetbuff[1024];
	char oppitembuff[1024];
	char opppetbuff[1024];
	// 双方皆按下확인键状态
	if (pc.trade_confirm == 4) {
		// 处理按下확인键
		if ((MainTradeWndflag == true && HitDispNo == tradeWndFontNo[0]) ||
			//andy_reEdit 2003/04/27
			(SecondTradeWndflag == true && HitDispNo == SecondtradeWndFontNo[2])) {
			// 对双方交易物品做成buffer	
			sprintf(myitembuff, "T|%s|%s|K|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|"
				, opp_sockfd, opp_name
				, tradeList[1].data, tradeList[2].data
				, tradeList[3].data, tradeList[4].data
				, tradeList[5].data, tradeList[6].data
				, tradeList[7].data, tradeList[8].data
				, tradeList[9].data, tradeList[10].data
				, tradeList[11].data, tradeList[12].data
				, tradeList[13].data, tradeList[14].data
				, tradeList[15].data);
			sprintf(mypetbuff, "P|%d|P|%d|P|%d|P|%d|P|%d|G|%d|"
				, tradeList[16].data, tradeList[17].data
				, tradeList[18].data, tradeList[19].data
				, tradeList[20].data, tradeList[21].data);
			sprintf(oppitembuff, "I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|I|%d|"
				, tradeList[22].data, tradeList[23].data
				, tradeList[24].data, tradeList[25].data
				, tradeList[26].data, tradeList[27].data
				, tradeList[28].data, tradeList[29].data
				, tradeList[30].data, tradeList[31].data
				, tradeList[32].data, tradeList[33].data
				, tradeList[34].data, tradeList[35].data
				, tradeList[36].data);
			sprintf(opppetbuff, "P|%d|P|%d|P|%d|P|%d|P|%d|G|%d|"
				, tradeList[37].data, tradeList[38].data
				, tradeList[39].data, tradeList[40].data
				, tradeList[41].data, tradeList[42].data);
			sprintf(buffer, "%s%s%s%s", myitembuff, mypetbuff, oppitembuff, opppetbuff);
			lssproto_TD_send(sockfd, buffer);
			tradeStatus = 2;
		}
	}
	// 对方按下Lock或我方要按下Lock键 
	if (pc.trade_confirm == 1 || pc.trade_confirm == 3) {
		if ((MainTradeWndflag == true && HitDispNo == tradeWndFontNo[0]) ||
			//andy_reEdit 2003/04/27
			(SecondTradeWndflag == true && HitDispNo == SecondtradeWndFontNo[2])) {
			//对方尚未Lock进入状态 2
			if (pc.trade_confirm == 1) pc.trade_confirm = 2;
			//对方已经Lock进入状态 4
			if (pc.trade_confirm == 3)	pc.trade_confirm = 4;
			sprintf(buffer, "T|%s|%s|C|confirm", opp_sockfd, opp_name);
			lssproto_TD_send(sockfd, buffer);
		}
	}
	// 处理 Lock 、 확인键 End
}
#endif

void WindowDisp(ACTION *pAct)
{
	WINDOW_DISP *pYobi = (WINDOW_DISP *)pAct->pYobi;
	int i, j;
	int x = pAct->x + 32, y = pAct->y + 24;
	switch (pAct->actNo) {

	case 0:
		StockBoxDispBuffer(pYobi->mx - pYobi->nowX,
			pYobi->my - pYobi->nowY,
			pYobi->mx + pYobi->nowX,
			pYobi->my + pYobi->nowY,
			pYobi->boxDispPrio, SYSTEM_PAL_BLACK, 0);
		pYobi->nowX += pAct->dx;
		pYobi->nowY += pAct->dy;
		pYobi->cnt++;
		if (pYobi->cnt >= WINDOW_CREATE_FRAME) {
			if (pYobi->wndType == -1) pAct->actNo = 1;
			else
				if (pYobi->wndType == -2) pAct->actNo = 3;
				else pAct->actNo = 2;
		}
		break;
	case 1:
		pAct->hp = 1;
		break;
	case 2:
		for (j = 0; j < pYobi->sizeY; j++) {
			if (j == 0) {
				for (i = 0; i < pYobi->sizeX; i++) {
					if (i == 0) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 0, pYobi->hitFlag);
					else if (i == pYobi->sizeX - 1) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 2, pYobi->hitFlag);
#ifdef _READ16BITBMP
					else if (g_bUseAlpha) {
						if (pYobi->wndType == CG_WND_G_0) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 1, pYobi->hitFlag);
						else if (pYobi->wndType == CG_WND2_G_0)StockDispBuffer(x, y + 1, DISP_PRIO_MENU, pYobi->wndType + 1, pYobi->hitFlag);
						else StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 1, pYobi->hitFlag);
					}
#endif
					else StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 1, pYobi->hitFlag);
					x += 64; // ????
				}
			}
			else
				// ?
				if (j == pYobi->sizeY - 1) {
					for (i = 0; i < pYobi->sizeX; i++) {
						// ?
						if (i == 0) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 6, pYobi->hitFlag);
						// ?
						else if (i == pYobi->sizeX - 1) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 8, pYobi->hitFlag);
						// ??
#ifdef _READ16BITBMP
						else if (g_bUseAlpha) {
							if (pYobi->wndType == CG_WND_G_0) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 7, pYobi->hitFlag);
							else if (pYobi->wndType == CG_WND2_G_0) StockDispBuffer(x, y - 1, DISP_PRIO_MENU, pYobi->wndType + 7, pYobi->hitFlag);
							else StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 7, pYobi->hitFlag);
						}
#endif
						else StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 7, pYobi->hitFlag);
						x += 64; // ????
					}
				}
				else
					// ??
					for (i = 0; i < pYobi->sizeX; i++) {
						// ?
						if (i == 0) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 3, pYobi->hitFlag);
						// ?
#ifdef _READ16BITBMP
						else if (g_bUseAlpha) {
							if (i == pYobi->sizeX - 1) {
								if (pYobi->wndType == CG_WND_G_0) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 5, pYobi->hitFlag);
								else if (pYobi->wndType == CG_WND2_G_0) StockDispBuffer(x - 1, y, DISP_PRIO_MENU, pYobi->wndType + 5, pYobi->hitFlag);
							}
							else if (i == pYobi->sizeX - 1) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 5, pYobi->hitFlag);
							else StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 4, pYobi->hitFlag);
						}
#endif
						else if (i == pYobi->sizeX - 1) StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 5, pYobi->hitFlag);
						// ??
						else StockDispBuffer(x, y, DISP_PRIO_MENU, pYobi->wndType + 4, pYobi->hitFlag);
						x += 64; // ????
					}
			x = pAct->x + 32; 	// ????
			y += 48; 		// ????
		}
		// ?????
		if (pYobi->titleNo != 0) StockDispBuffer(pYobi->titleX, pYobi->titleY, DISP_PRIO_IME3, pYobi->titleNo, pYobi->hitFlag);
		pAct->hp = 1;
		break;
	case 3:	// ??????
			// ????????
		if (mouse.onceState & MOUSE_LEFT_CRICK) {
			// ????
			if (HitDispNo == pYobi->yesDispNo) {
				pYobi->yesNoResult = TRUE;
			}
			// ?????
			if (HitDispNo == pYobi->noDispNo) {
				pYobi->yesNoResult = FALSE;
			}
		}
		// ??????????
		StockDispBuffer(pYobi->mx, pYobi->my, DISP_PRIO_YES_NO_WND, CG_COMMON_WIN_YORO, 1);
		// ?????
		pYobi->yesDispNo = StockDispBuffer(pYobi->mx, pYobi->my, DISP_PRIO_YES_NO_BTN, CG_COMMON_YES_BTN, 2);
		// ??????
		pYobi->noDispNo = StockDispBuffer(pYobi->mx, pYobi->my, DISP_PRIO_YES_NO_BTN, CG_COMMON_NO_BTN, 2);
		break;
	}
}

// ????????? **************************************************************/
//	??：	int x,y 			????
//			int sizeX,sizeY 	??????????????????????????
//			int titleNo 		?????????????????????????
//			int wndType 		?：?????????????
//			 					?：??????????????
//								?：????????????
//								?：?????????????
//								?：?????????????
//								-1：???
//****************************************************************************************/
ACTION *MakeWindowDisp(int x, int y, int sizeX, int sizeY, int titleNo, int wndType)
{
	ACTION *pAct;
	WINDOW_DISP *pYobi;
#ifdef _NEW_RESOMODE  //800 600模式
	if (x > 320) {
		x += DISPLACEMENT_X;
	}
	else if (x > 40) {
		x += DISPLACEMENT_X / 2;
	}

	if (y > 240)
		y -= DISPLACEMENT_Y / 2;
	else if (y > 40)
		y += DISPLACEMENT_Y / 2;

#endif
	// ?????????
	pAct = GetAction(PRIO_JIKI, sizeof(WINDOW_DISP));
	if (pAct == NULL) return NULL;

	// ???
	pAct->func = WindowDisp;
	// ?
	pAct->dispPrio = DISP_PRIO_MENU;
	// ?????
	pAct->atr |= ACT_ATR_HIT;
	// ???
	pAct->atr |= ACT_ATR_HIDE;

	// ???????
	pYobi = (WINDOW_DISP *)pAct->pYobi;
	// ????
	pAct->x = x;
	pAct->y = y;
	pYobi->titleX = x + (sizeX * 64) / 2;
	pYobi->titleY = pAct->y + 27;
	// ??????
	pYobi->sizeX = sizeX;
	pYobi->sizeY = sizeY;
	// ???????
	pYobi->titleNo = titleNo;
	pYobi->boxDispPrio = DISP_PRIO_MENU;
	// ??????????
	if (wndType == 0) pYobi->wndType = CG_WND_G_0;
	else if (wndType == 1) pYobi->wndType = CG_WND_G_0;
	else if (wndType == 2) pYobi->wndType = CG_WND_G_0;
	else if (wndType == 3) pYobi->wndType = CG_WND_G_0;
	else if (wndType == 4) {
		pYobi->wndType = -2;
		// ??????
		pYobi->yesDispNo = -2;
		pYobi->noDispNo = -2;
		pYobi->boxDispPrio = DISP_PRIO_YES_NO_WND;
	}
	else pYobi->wndType = -1;

	// YesNo ??????
	pYobi->yesNoResult = -1;

	// ??????
	if (wndType >= 2) pYobi->hitFlag = FALSE;
	else pYobi->hitFlag = TRUE;

	// ???
	// ????????????????
	if (wndType == -1 || wndType == 4) {
		pYobi->mx = sizeX / 2 + pAct->x;
		pYobi->my = sizeY / 2 + pAct->y;
	}
	else {	// ????????
		pYobi->mx = (sizeX * 64) / 2 + pAct->x;
		pYobi->my = (sizeY * 48) / 2 + pAct->y;
	}
	// ??
	pAct->dx = (pYobi->mx - pAct->x) / WINDOW_CREATE_FRAME;
	pAct->dy = (pYobi->my - pAct->y) / WINDOW_CREATE_FRAME;

	return pAct;
}

// ???????????? ***********************************************/
void AnimDisp(ACTION *pAct) {
	int flag = FALSE;
	int x, y, i;
	switch (pAct->actNo) {

	case ANIM_DISP_PET:
		if (pAct->hitDispNo == HitDispNo && mouse.autoState & MOUSE_LEFT_CRICK) {
			pAct->anim_ang++;
			if (pAct->anim_ang >= 8) pAct->anim_ang = 0;
			play_se(217, 320, 240);
		}
		pattern(pAct, ANM_NOMAL_SPD, ANM_LOOP);

		break;

	case ANIM_DISP_PET_SEND:
		pAct->anim_no = ANIM_WALK;
		pAct->anim_ang = 2;
		pAct->crs = crs_change_tbl2[pAct->anim_ang];
		pAct->dx++;
		if (pAct->dx % 3 == 0) {
			if (pAct->spd < 20) {
				pAct->spd++;
			}
		}
		if (pAct->dx % 5 == 0) {
			if (pAct->dy >= 2) pAct->dy--;
		}
		gemini(pAct);
		if (pActMailItem != NULL) {
			if (pAct->x <= pActMailItem->x) {
				pActMailItem->x = pAct->x;
			}
		}
		if (pAct->x <= -100) {
			DeathAction(pActMenuWnd);
			pActMenuWnd = NULL;
			DeathAction(pActPet);
			pActPet = NULL;
			DeathAction(pActMailItem);
			pActMailItem = NULL;
			mailWndNo = MAIL_WND_VIEW;
			play_se(203, 320, 240);
			GetKeyInputFocus(&MyChatBuffer);
		}
		pattern(pAct, pAct->dy, ANM_LOOP);

		break;

	case ANIM_DISP_PET_RECIEVE:
		pAct->anim_no = ANIM_WALK;
		pAct->anim_ang = 6;
		pAct->crs = crs_change_tbl2[pAct->anim_ang];
		pAct->dx++;
		if (pAct->spd > 0) {
			if (pAct->dx % 3 == 0) {
				pAct->spd--;
				if (pAct->spd <= 0) {
					pAct->actNo = ANIM_DISP_PET_MAIN;
					if (pActMailItem != NULL) {
						pActMailItem->dispPrio = DISP_PRIO_BOX3;
					}
				}
			}
		}
		if (pAct->dx % 8 == 0) {
			if (pAct->dy < 12) pAct->dy++;
		}
		gemini(pAct);
		if (pActMailItem != NULL) {
			if (pAct->x >= pActMailItem->x) {
				pActMailItem->x = pAct->x;
			}
		}
		pattern(pAct, pAct->dy, ANM_LOOP);

		break;

	case ANIM_DISP_PET_MAIN:
		if (Rnd(0, 4) >= 2) {
			pAct->actNo = ANIM_DISP_PET_WALK;
			pAct->anim_ang = Rnd(0, 7);
			pAct->crs = crs_change_tbl2[pAct->anim_ang];
			pAct->spd = Rnd(1, 12);
			pAct->dy = 13 - pAct->spd;
		}
		else pAct->actNo = Rnd(ANIM_DISP_PET_STAND, ANIM_DISP_PET_DEAD);

		pAct->anim_no_bak = -1;

		break;

	case ANIM_DISP_PET_WALK:
		pAct->anim_no = ANIM_WALK;
		gemini(pAct);
		if (pattern(pAct, pAct->dy, ANM_NO_LOOP)) {
			if (Rnd(0, 3) == 0) {
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}
			else {
				pAct->anim_no_bak = -1;
			}
		}
		if (pActPet == NULL || pActMenuWnd == NULL) {
			return;
		}
		if (pActPet->y < pActMenuWnd->y + 64) {
			pActPet->y = pActMenuWnd->y + 64;
			pAct->crs = crs_bound_tbl[0][pAct->crs];
			pAct->anim_ang = crs_change_tbl[pAct->crs];
		}
		if (pActPet->y > pActMenuWnd->y + 424 - 32) {
			pActPet->y = pActMenuWnd->y + 424 - 32;
			pAct->crs = crs_bound_tbl[2][pAct->crs];
			pAct->anim_ang = crs_change_tbl[pAct->crs];
		}
		if (pActPet->x < pActMenuWnd->x + 32) {
			pActPet->x = pActMenuWnd->x + 32;
			pAct->crs = crs_bound_tbl[3][pAct->crs];
			pAct->anim_ang = crs_change_tbl[pAct->crs];
		}
		if (pActPet->x > pActMenuWnd->x + 272 - 32) {
			pActPet->x = pActMenuWnd->x + 272 - 32;
			pAct->crs = crs_bound_tbl[1][pAct->crs];
			pAct->anim_ang = crs_change_tbl[pAct->crs];
		}
		break;

	case ANIM_DISP_PET_STAND:
		pAct->anim_no = ANIM_STAND;
		if (pattern(pAct, ANM_NOMAL_SPD, ANM_NO_LOOP)) {
			pAct->actNo = ANIM_DISP_PET_MAIN;
		}

		break;

	case ANIM_DISP_PET_ATTACK:
		pAct->anim_no = ANIM_ATTACK;
		if (pattern(pAct, ANM_NOMAL_SPD, ANM_NO_LOOP)) {
			if (Rnd(0, 1) == 0) {
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}
			else {
				pAct->anim_no_bak = -1;
			}
		}

		break;

	case ANIM_DISP_PET_GUARD:
		pAct->anim_no = ANIM_GUARD;
		if (pattern(pAct, ANM_NOMAL_SPD, ANM_NO_LOOP)) {
			if (Rnd(0, 1) == 0) {
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}
		}

		break;

	case ANIM_DISP_PET_DAMAGE:
		pAct->anim_no = ANIM_DAMAGE;
		if (pattern(pAct, ANM_NOMAL_SPD, ANM_NO_LOOP)) {
			if (Rnd(0, 1) == 0) {
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}
			else {
				pAct->anim_no_bak = -1;
			}
		}

		break;

	case ANIM_DISP_PET_DEAD:
		pAct->anim_no = ANIM_DEAD;
		if (pattern(pAct, ANM_NOMAL_SPD, ANM_NO_LOOP)) {
			if (Rnd(0, 2) == 0) {
				pAct->actNo = ANIM_DISP_PET_MAIN;
			}
		}

		break;

	case ANIM_DISP_PET_ITEM:
		pattern(pAct, ANM_NOMAL_SPD, ANM_LOOP);

		break;

	case ANIM_DISP_LETTER_MOVE:
		if (pAct->x < 245) pAct->x += 5;
		else {
			pAct->actNo = ANIM_DISP_LETTER;
			pattern(pAct, ANM_NOMAL_SPD, ANM_LOOP);
			pAct->anim_cnt += 8;
		}
		break;

	case ANIM_DISP_LETTER:
		pattern(pAct, ANM_NOMAL_SPD, ANM_LOOP);

		break;

	case ANIM_DISP_MIX_PET_INIT:
		//JLWG 料理特效1
		pAct->actNo = ANIM_DISP_MIX_PET_MOJI;
		break;

		pAct->spd = 11;//Rnd( 6, 12 );
		pAct->dy = 14 - pAct->spd;
		pAct->anim_no = ANIM_WALK;
		for (i = MAX_ITEMSTART; i < MAX_ITEM; i++) {
			if (ItemBuffer[i].mixFlag >= 1) {
				pAct->dx = i;
				break;
			}
		}
		pAct->actNo = ANIM_DISP_MIX_PET_MOVE;
		break;

	case ANIM_DISP_MIX_PET_MOVE:
		x = ItemBuffer[pAct->dx].defX;
		y = ItemBuffer[pAct->dx].defY;

		// ????
		radar(pAct, &x, &y);

		// ??
		pAct->anim_no = ANIM_WALK;

		// ?????????
		if (y >= pAct->spd / 2) {
			// ???
			pAct->crs = x;
			// ????佋????????
			if (pAct->level == FALSE) {
				pAct->anim_ang = crs_change_tbl[pAct->crs];
				// ??????????
				pAct->level = TRUE;
			}

			// ???
			gemini(pAct);
			// ???????
			pattern(pAct, pAct->dy, ANM_LOOP);
		}
		// ?????
		else {
			// ???????????
			pAct->level = FALSE;
			// ??????
			play_se(204, 320, 240);

			// ??????????????
			if (pAct->delta == 1) {
				// ???????
				pAct->actNo = ANIM_DISP_MIX_PET_MOJI;
				// ??
				pAct->anim_no = ANIM_STAND;

				// ?????????
				pAct->spd = 10;//Rnd( 6, 12 );
							   // ???????????
				pAct->dy = 14 - pAct->spd;

				// ??????
				for (i = MAX_ITEMSTART; i < MAX_ITEM; i++) {
					if (ItemBuffer[i].mixFlag >= 1) {
						ItemBuffer[i].mixFlag = 4;
					}
				}

				// ????????????
				pAct->delta = Rnd(2, 15);
				pAct->mp = 15;
				break;
			}

			// ????????
			pAct->spd = (int)((double)pAct->spd * 0.79);
			// ????????
			if (pAct->spd < 2) pAct->spd = 2;
			// ???????????
			pAct->dy = 13 - pAct->spd;

			ItemBuffer[pAct->dx].mixFlag = 3;

			// ????????
			for (i = pAct->dx + 1; i < MAX_ITEM; i++) {
				if (ItemBuffer[i].mixFlag >= 1) {
					pAct->dx = i;
					break;
				}
			}

			// ????????
			if (i >= MAX_ITEM) {
				// ????????????
				for (i = MAX_ITEMSTART; i < MAX_ITEM; i++) {
					// ???????????
					if (pc.item[i].useFlag == FALSE) {
						// ????
						pAct->dx = i;
						break;
					}
					else
						// ????????????
						if (ItemBuffer[i].mixFlag >= 1) {
							// ????
							pAct->dx = i;
							break;
						}
				}
				// ????????????
				pAct->delta = 1;
			}
		}
		break;

	case ANIM_DISP_MIX_PET_MOJI:	// ?????????????

									// ???????
		pattern(pAct, 1, ANM_LOOP);
		// ??????
		if (pAct->maxHp >= pAct->mp) pAct->maxHp = 0;
		else {
			pAct->maxHp++;
			break;
		}
		// ??????
		switch (pAct->hp) {

			// ???
		case 0:
		case 3:
			// ??
			pAct->anim_ang++;
			// ????????
			if (pAct->anim_ang >= 8) pAct->anim_ang = 0;
			// ?????
			if (pAct->hp == 0) {
				// ??????
				play_se(216, 320, 240);
			}
			// ???????
			if (pAct->hp == 3) {
				pAct->dir++;
				pAct->hp = 0;
				// ??????
				pAct->mp--;
				if (pAct->mp < 4) pAct->mp = 4;
			}
			else pAct->hp++;
			break;

			// ???
		case 1:
		case 2:
			// ??
			pAct->anim_ang--;
			// ????????
			if (pAct->anim_ang < 0) pAct->anim_ang = 7;
			pAct->hp++;
			break;
		}

		// ???????
		if (pAct->dir >= pAct->delta) {
			// ???
			if (pAct->dir == pAct->delta) {
				pAct->dir++;
				// ????
				char moji[256];
				// ????
				moji[0] = NULL;
				// ????????????
				for (i = MAX_ITEMSTART; i < MAX_ITEM; i++) {
					// ??????????
					if (ItemBuffer[i].mixFlag >= TRUE) {//ttom
						char work[256];
						// ???
						sprintf(work, "%d|", i);
						strcat(moji, work);
						// ??????
					}//ttom
				}
				// ???|????????
				moji[strlen(moji) - 1] = NULL;

				// ??????????????????
				if (bNewServer)
					lssproto_PS_send(sockfd, mixPetNo, SelectWazaNo, 0, moji);
				else
					old_lssproto_PS_send(sockfd, mixPetNo, SelectWazaNo, 0, moji);
				// ??????
				ItemMixRecvFlag = TRUE;
			}

			// ??????
			if (ItemMixRecvFlag == FALSE) {
				// ??????
				play_se(115, 320, 240);
				// ????????
				for (i = MAX_ITEMSTART; i < MAX_ITEM; i++) {
					ItemBuffer[i].mixFlag = FALSE;
				}
				// ?????????
				pAct->spd = 11;
				// ???????????
				pAct->dy = 14 - pAct->spd;

				//JLWG 料理特效2
				DeathAction(pAct);
				pActPet2 = NULL;
				ItemMixPetNo = -1;
				//		pAct->actNo = ANIM_DISP_MIX_PET_END;
			}
		}

		break;

	case ANIM_DISP_MIX_PET_END:	// ???????

								// ?佋
		x = 750;
		y = 240;

		// ????
		radar(pAct, &x, &y);

		// ??
		pAct->anim_no = ANIM_WALK;

		// ?????????
		if (y >= pAct->spd / 2) {
			// ???
			pAct->crs = x;

			// ????佋????????
			if (pAct->level == FALSE) {
				pAct->anim_ang = crs_change_tbl[pAct->crs];
				// ??????????
				pAct->level = TRUE;
			}

			// ???
			gemini(pAct);
			// ???????
			pattern(pAct, pAct->dy, ANM_LOOP);
		}
		// ?????
		else {
			// ??????
			DeathAction(pAct);
			pActPet2 = NULL;
			// ????????
			ItemMixPetNo = -1;
		}
		break;
#ifdef _AniCrossFrame	  // Syu ADD 动画层游过画面生物
	case ANIM_DISP_CROSSFRAME:
	{
		extern bool delFlag;
		extern bool flag22;
		pAct->anim_no = ANIM_WALK;
		pAct->anim_ang = 2;
		pAct->crs = crs_change_tbl2[pAct->anim_ang];
		//移动速度
		pAct->spd = 1;
		//显示层级
		pAct->dispPrio = DISP_PRIO_JIKI;
		if (pAct->dx % 5 == 0) {
			if (pAct->dy >= 2) pAct->dy--;
		}
		gemini(pAct);
		//各种方向位移
		if (((pAct->anim_chr_no > 101511) && (pAct->anim_chr_no < 101516)) ||
			(pAct->anim_chr_no == 101517) || (pAct->anim_chr_no == 101519))
		{
			pAct->x--;
			if (pAct->y == 0)
				UpDownflag = 4;
			if (UpDownflag == 0)
				UpDownflag = RAND(1, 5);
			if (UpDownflag > 3)
				pAct->y++;
			else
				pAct->y = pAct->y;
		}
		else if ((pAct->anim_chr_no == 101516) || (pAct->anim_chr_no == 101518))
		{
			pAct->x++;
			if (pAct->y == 0)
				UpDownflag = 4;
			if (UpDownflag == 0)
				UpDownflag = RAND(1, 5);
			if (UpDownflag > 3)
				pAct->y++;
			else
				pAct->y = pAct->y;
		}
		else if ((pAct->anim_chr_no == 101520) || (pAct->anim_chr_no == 101522) ||
			(pAct->anim_chr_no <= 101593 && pAct->anim_chr_no >= 101588)) //右上
		{
			pAct->x++;
			pAct->y--;
		}
		else if ((pAct->anim_chr_no == 101521) || (pAct->anim_chr_no == 101523) ||
			(pAct->anim_chr_no <= 101587 && pAct->anim_chr_no >= 101582)) //左上
		{
			pAct->x--;
			pAct->y--;
		}
		else
		{
			pAct->x++;
			pAct->y++;
		}
		//超出荧幕范围消灭
		if ((pAct->x <= -100 || pAct->y <= -100 || pAct->x >= 740 || pAct->y >= 580) && delFlag == true && flag22 == true)
		{
			flag22 = false;
			UpDownflag = 0;
		}
		pattern(pAct, pAct->dy, ANM_LOOP);
	}
	break;
#endif
	}

	// ??????????
	if (ANIM_DISP_PET_MAIN <= pAct->actNo && pAct->actNo <= ANIM_DISP_PET_DEAD) {
		// ????????????
		if (pAct->hitDispNo == HitDispNo &&
			mouse.autoState & MOUSE_LEFT_CRICK) {
			// ????
			pAct->actNo = ANIM_DISP_PET_WALK;
			// ?????
			pAct->anim_ang = Rnd(0, 7);
			// ???
			pAct->crs = crs_change_tbl2[pAct->anim_ang];
			// ?????????
			pAct->spd = 15;

			pAct->dy = 2;
			play_se(217, 320, 240); // ?????
		}
	}
}

// ??????????? ***********************************************/
ACTION *MakeAnimDisp(int x, int y, int sprNo, int mode)
{
	ACTION *pAct;

	// ?????????
	pAct = GetAction(PRIO_JIKI, NULL);
	if (pAct == NULL) return NULL;

	// ???
	pAct->func = AnimDisp;
	// ?????
	pAct->atr |= ACT_ATR_HIT;
	pAct->anim_chr_no = sprNo;
	// ??
	pAct->anim_no = ANIM_STAND;
	// ?????????( ??? )( ??????? )
	pAct->anim_ang = 1;
	/* ? */
	pAct->dispPrio = DISP_PRIO_BOX3;
	/* ??? */
	pAct->x = x;
	pAct->y = y;
	// ???
	pAct->actNo = mode;
	// ??????????
	if (pAct->actNo == ANIM_DISP_PET) {
		// ???????????
		pAct->dy = 12;
	}
	else
		if (pAct->actNo == ANIM_DISP_PET_RECIEVE) {
			pAct->dy = 1;
			pAct->spd = 30;
		}
	// ???????
	pattern(pAct, ANM_NOMAL_SPD, ANM_LOOP);

	return pAct;
}

// ??????? ****************************************************************/
BOOL SaveMailHistory(int no)
{
	FILE *fp;

	// ????????????
	if ((fp = fopen(MAIL_HISTORY_FILE_NAME, "r+b")) == NULL) {
		return FALSE;
	}

	// ?????????????????????????
	fseek(fp, sizeof(MAIL_HISTORY) * MAX_ADR_BOOK * selectPcNo, SEEK_SET);

	// ???????????????????????
	fseek(fp, sizeof(MAIL_HISTORY) * no, SEEK_CUR);

	// ???????
	if (fwrite(&MailHistory[no], sizeof(MAIL_HISTORY), 1, fp) < 1) {

		fclose(fp);// ????????
		return FALSE;
	}

	fclose(fp);

	return TRUE;
}

// ?????????? ****************************************************************/
BOOL LoadMailHistory(void)
{
	FILE *fp;
	MAIL_HISTORY work[MAX_ADR_BOOK * 2];	// ?????????

											// ???????????
	if ((fp = fopen(MAIL_HISTORY_FILE_NAME, "rb")) == NULL) {
		// ???????
		if ((fp = fopen(MAIL_HISTORY_FILE_NAME, "wb")) != NULL) {
			// ???????
			fwrite(MailHistory, sizeof(MAIL_HISTORY), MAX_ADR_BOOK, fp);
			fwrite(MailHistory, sizeof(MAIL_HISTORY), MAX_ADR_BOOK, fp);
			fclose(fp);	// ????????
		}

		return FALSE;
	}

	// ????????????
	if (fread(work, sizeof(MAIL_HISTORY), MAX_ADR_BOOK * 2, fp) < MAX_ADR_BOOK * 2) {
		// ???????
		// ???????
		if ((fp = fopen(MAIL_HISTORY_FILE_NAME, "wb")) != NULL) {
#ifdef _STONDEBUG_		
			MessageBox(hWnd, "이전 메일 자료 삭제。", "확인", MB_OK | MB_ICONSTOP);
#endif
			// ???????
			fwrite(MailHistory, sizeof(MAIL_HISTORY), MAX_ADR_BOOK, fp);
			fwrite(MailHistory, sizeof(MAIL_HISTORY), MAX_ADR_BOOK, fp);
			fclose(fp);	// ????????
		}

		return FALSE;
	}


	// ??????????????????
	fseek(fp, sizeof(MAIL_HISTORY) * MAX_ADR_BOOK * selectPcNo, SEEK_SET);

	// ??????
	if (fread(MailHistory, sizeof(MAIL_HISTORY), MAX_ADR_BOOK, fp) < MAX_ADR_BOOK) {

		fclose(fp);	// ????????
		return FALSE;
	}
	// ????????
	fclose(fp);

	return TRUE;
}

// ???????? ****************************************************************/
BOOL SaveChatRegistyStr(void)
{
	FILE *fp;

	// ????????????
	if ((fp = fopen(CAHT_REGISTY_STR_FILE_NAME, "r+b")) == NULL) {
#ifdef _STONDEBUG_		
		MessageBox(hWnd, "채팅방 로그인 텍스트 기록에 실패함！１", "확인", MB_OK | MB_ICONSTOP);
#endif
		return FALSE;
	}

	// ???????
	if (fwrite(chatRegistryStr, sizeof(STR_BUFFER), MAX_CHAT_REGISTY_STR, fp) < MAX_CHAT_REGISTY_STR) {

#ifdef _STONDEBUG_		
		MessageBox(hWnd, "채팅방 로그인 텍스트 기록에 실패함！２", "확인", MB_OK | MB_ICONSTOP);
#endif
		fclose(fp);// ????????
		return FALSE;
	}
#ifdef _TALK_WINDOW
	fwrite(&g_bTalkWindow, sizeof(BOOL), 1, fp);
#endif
	// ????????
	fclose(fp);

	return TRUE;
}

// ??????????? ****************************************************************/
BOOL LoadChatRegistyStr(void)
{
	FILE *fp;
	int i;

	// ???????????
	if ((fp = fopen(CAHT_REGISTY_STR_FILE_NAME, "rb")) == NULL) {
		// ???????
		if ((fp = fopen(CAHT_REGISTY_STR_FILE_NAME, "wb")) != NULL) {
#ifdef _STONDEBUG_		
			MessageBox(hWnd, "채팅용 로그인 텍스트 파일 만드십시오！", "확인", MB_OK);
#endif
			// ??????????????????
			for (i = 0; i < 8; i++) {
				// ??? ??
				chatRegistryStr[i].buffer[0] = NULL;
				chatRegistryStr[i].cnt = 0;
				chatRegistryStr[i].cursor = 0;
				// ????
				chatRegistryStr[i].len = MAX_CHAT_REGISTY_STR_LEN;
				// ????
				chatRegistryStr[i].lineLen = 0;
				// ?????
				chatRegistryStr[i].lineDist = 20;
				// ???
				chatRegistryStr[i].color = 0;
				// ????
				// ??????
				chatRegistryStr[i].fontPrio = FONT_PRIO_FRONT;
			}
			// ???????
			fwrite(chatRegistryStr, sizeof(STR_BUFFER), MAX_CHAT_REGISTY_STR, fp);
#ifdef _TALK_WINDOW
			fwrite(&g_bTalkWindow, sizeof(BOOL), 1, fp);
#endif
			fclose(fp);	// ????????
		}
		return FALSE;
	}

	// ??????
	if (fread(chatRegistryStr, sizeof(STR_BUFFER), MAX_CHAT_REGISTY_STR, fp) < MAX_CHAT_REGISTY_STR) {

#ifdef _STONDEBUG_		
		MessageBox(hWnd, "채팅용 로그인 텍스트 데이터를 읽어오는데 실패하였습니다.！１", "확인", MB_OK | MB_ICONSTOP);
#endif
		fclose(fp);	// ????????
		return FALSE;
	}
#ifdef _TALK_WINDOW
	fread(&g_bTalkWindow, sizeof(BOOL), 1, fp);
	if (g_bTalkWindow) TalkWindow.Create();
#endif
	// ????????
	fclose(fp);

	return TRUE;
}
#ifdef __ALBUM_47
int MAX_PET_TBL;
#endif
// ????? ****************************************************************/
BOOL SaveAlbum(int no)
{
	FILE	*fp;

#if defined(__ALBUM_47)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_48, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "앨범 데이터를 저장하는데 실패하였습니다.！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "앨범 데이터를 저장하는데 실패하였습니다.！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "앨범 데이터를 저장하는데 실패하였습니다.！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_46)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_47, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "앨범 데이터를 저장하는데 실패하였습니다.！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "앨범 데이터를 저장하는데 실패하였습니다.！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "앨범 데이터를 저장하는데 실패하였습니다.！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_45)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_46, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "앨범 데이터를 저장하는데 실패하였습니다.！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "앨범 데이터를 저장하는데 실패하였습니다.！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "앨범 데이터를 저장하는데 실패하였습니다.！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_44)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_45, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_43)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_44, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_42)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_43, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_41)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_42, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_40)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_41, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_39)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_40, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_38)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_39, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_37)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_38, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_36)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_37, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;

#elif defined(__ALBUM_35)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_36, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;
#elif defined(__ALBUM_34)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_35, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;
#elif defined(__ALBUM_33)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_34, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;
#elif defined(__ALBUM_32)
	BOOL	ret = FALSE;
	if (0 <= no && no < MAX_PET_TBL) {
		if (fp = fopen(ALBUM_FILE_NAME_33, "r+b")) {
			if (0 != fseek(fp, (16 + sizeof(PET_ALBUM)*MAX_PET_KIND)*AlbumIdCnt, SEEK_SET))
				MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
			if (0 != fseek(fp, 16 + sizeof(PET_ALBUM)*no, SEEK_CUR))
				MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
			if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) >= 1)
				ret = TRUE;
			fclose(fp);
		}
		else
			MessageBox(hWnd, "储存相簿资料失败！３", "확인", MB_OK);
	}
	return ret;
#else
	// ????????????
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL) {
#ifdef _STONDEBUG_		
		MessageBox(hWnd, "储存相簿资料失败！１", "확인", MB_OK);
#endif
		return FALSE;
	}
	// ???????
	for (int i = 0; i < AlbumIdCnt; i++) {
		// ????????????
		fseek(fp, 16, SEEK_CUR);
		fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND, SEEK_CUR);
	}
	// ???????
	fseek(fp, 16, SEEK_CUR);
	// ???????????????
	fseek(fp, sizeof(PET_ALBUM) * no, SEEK_CUR);

	// ???????
	if (fwrite(&PetAlbum[no], sizeof(PET_ALBUM), 1, fp) < 1) {

#ifdef _STONDEBUG_		
		MessageBox(hWnd, "储存相簿资料失败！２", "확인", MB_OK);
#endif
		fclose(fp);// ????????
		return FALSE;
	}
	// ????????
	fclose(fp);

	return TRUE;
#endif
}

#if defined(__ALBUM_4)

BOOL LoadAlbum_4(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_4, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_4, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			// ??????????
			// ????????????
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			//
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbum1_4(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

#elif defined(__ALBUM_47)
BOOL LoadAlbum_47(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_48, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_48, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo47(char *user, char *oldfile, DWORD num)
{

	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}


		id2[15] &= 0xf;  //JL 2017.7.12修复   id2[15]是用于存储帐号长度
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_47(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}

		id2[15] &= 0xf;  //JL 2017.7.12修复   id2[15]是用于存储帐号长度
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_46)
BOOL LoadAlbum_46(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_47, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_47, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo46(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_46(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_45)
BOOL LoadAlbum_45(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_46, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_46, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo45(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_45(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_44)
BOOL LoadAlbum_44(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_45, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_45, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo44(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_44(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_43)
BOOL LoadAlbum_43(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_44, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_44, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo43(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_43(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_42)
BOOL LoadAlbum_42(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_43, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_43, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo42(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_42(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_41)
BOOL LoadAlbum_41(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_42, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_42, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo41(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_41(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_40)
BOOL LoadAlbum_40(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_41, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_41, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo40(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_40(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_39)
BOOL LoadAlbum_39(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_40, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_40, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo39(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_39(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_38)
BOOL LoadAlbum_38(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_39, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_39, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo38(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_38(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_37)
BOOL LoadAlbum_37(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_38, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_38, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo37(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_37(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_36)
BOOL LoadAlbum_36(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_37, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_37, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo36(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				if (440 <= i && i <= 461) continue;		//修正乌力Q 问题
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_36(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_35)
BOOL LoadAlbum_35(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_36, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_36, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo35(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_35(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_34)
BOOL LoadAlbum_34(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_35, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_35, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo34(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_34(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_33)
BOOL LoadAlbum_33(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_34, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_34, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo33(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_33(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#elif defined(__ALBUM_32)
BOOL LoadAlbum_32(char *user)
{
	FILE *fp;
	BOOL ret = FALSE;
	char id[16];
	char id2[16];
	int i;
	MAX_PET_TBL = sizeof(PetAlbumTbl) / sizeof(PetAlbumTbl[0]);
	AlbumIdCnt = 0;
	memcpy(id, user, 16);
	DeleteFile("data\\album_2.dat");
	DeleteFile("data\\album_3.dat");
	DeleteFile("data\\album_10.dat");
	for (i = 0; i < MAX_PET_KIND; i++) {
		for (int j = 0; j < PET_NAME_LEN + 1; j++) {
			PetAlbum[i].name[j] = Rnd(1, 255);
			PetAlbum[i].freeName[j] = Rnd(1, 255);
		}
		PetAlbum[i].flag = 0;
		PetAlbum[i].faceGraNo = Rnd(1, 60000);
		PetAlbum[i].level = Rnd(1, 30000);
		PetAlbum[i].faceGraNo = Rnd(1, 30000);
		PetAlbum[i].maxHp = Rnd(1, 30000);
		PetAlbum[i].str = Rnd(1, 30000);
		PetAlbum[i].quick = Rnd(1, 30000);
		PetAlbum[i].def = Rnd(1, 30000);
		PetAlbum[i].earth = Rnd(1, 30000);
		PetAlbum[i].water = Rnd(1, 30000);
		PetAlbum[i].fire = Rnd(1, 30000);
		PetAlbum[i].wind = Rnd(1, 30000);
	}
	if ((fp = fopen(ALBUM_FILE_NAME_33, "r+b")) == NULL) {
		if ((fp = fopen(ALBUM_FILE_NAME_33, "wb")) != NULL) {
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
		}
		return ret;
	}
	while (1) {
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			//找不到此帐号的记录
			id[15] &= 0xf;
			id[id[15]] = Rnd(1, 100);
			for (i = 0; i < 16; i++)
				id[i] ^= IdEncryptionTbl[i];
			if (fwrite(id, sizeof(id), 1, fp) >= 1) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			}
			fclose(fp);
			return FALSE;
		}
		for (i = 0; i < 16; i++)
			id2[i] ^= IdEncryptionTbl[i];
		id2[15] &= 0xf;
		id2[id2[15]] = NULL;
		if (strcmp(id, id2) != 0) {
			//不是此帐号的记录
			if (fseek(fp, sizeof(PET_ALBUM)*MAX_PET_KIND, SEEK_CUR) != 0) {
				fseek(fp, (sizeof(PET_ALBUM)*MAX_PET_KIND + 16)*AlbumIdCnt, SEEK_SET);
				id[15] &= 0xf;
				id[id[15]] = Rnd(1, 100);
				for (i = 0; i < 16; i++)
					id[i] ^= IdEncryptionTbl[i];
				if (fwrite(id, sizeof(id), 1, fp) >= 1) {
					fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				}
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到此帐号的记录
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
				fclose(fp);
				return FALSE;
			}
			fclose(fp);
			return TRUE;
		}
		AlbumIdCnt++;
	}
	fclose(fp);
	return ret;
}

BOOL ConvertAlbumTo32(char *user, char *oldfile, DWORD num)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(oldfile, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * num, SEEK_CUR) != 0) {//下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//找到ID纪录
			if (fread(OldAlbum, sizeof(PET_ALBUM), num, fp) < num) {
				fclose(fp);
				return FALSE;
			}
			for (DWORD i = 0; i < num; i++) {
				CopyMemory(&PetAlbum[i], &OldAlbum[i], sizeof(PET_ALBUM));
				SaveAlbum(i);
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL ConvertAlbum1_32(char *user)
{
	PET_ALBUM OldAlbum[MAX_PET_KIND_1];
	char id2[16];
	int i;
	FILE *fp;
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL)
		return FALSE;
	while (1) {
		//读入ID
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			fclose(fp);
			return FALSE;
		}
		//ID解码
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		id2[id2[15]] = NULL;
		if (strcmp(user, id2) != 0) {
			//ID不符
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND_1, SEEK_CUR) != 0) {//找下一个
				fclose(fp);
				return FALSE;
			}
		}
		else {
			//读入资料
			if (fread(OldAlbum, sizeof(PET_ALBUM), MAX_PET_KIND_1, fp) < MAX_PET_KIND_1) {
				fclose(fp);
				return FALSE;
			}
			for (int i = 0; i < MAX_PET_KIND_1; i++) {
				for (int j = 0; j < MAX_PET_TBL; j++) {
					if (!OldAlbum[i].flag)
						break;
					if (OldAlbum[i].faceGraNo == PetAlbumTbl[j].faceGraNo) {
						CopyMemory(&PetAlbum[PetAlbumTbl[j].albumNo], &OldAlbum[i], sizeof(PET_ALBUM));
						SaveAlbum(PetAlbumTbl[j].albumNo);
						break;
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return TRUE;
}
#endif


// ???????? ****************************************************************/
BOOL LoadAlbum(void)
{
	char id[32];

	// ?????
	extern char szUser[];
	CopyMemory(id, szUser, 32);
	ecb_crypt("f;encor1c", id, 32, DES_DECRYPT);
	// ????
	id[15] = strlen(id);

#if defined(__ALBUM_47)
	if (LoadAlbum_47(id))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_47, MAX_PET_KIND_46))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_46, MAX_PET_KIND_45))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_45, MAX_PET_KIND_44))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_44, MAX_PET_KIND_43))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_43, MAX_PET_KIND_42))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_42, MAX_PET_KIND_41))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_41, MAX_PET_KIND_40))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_40, MAX_PET_KIND_39))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_39, MAX_PET_KIND_38))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo47(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_47(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_46)
	if (LoadAlbum_46(id))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_46, MAX_PET_KIND_45))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_45, MAX_PET_KIND_44))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_44, MAX_PET_KIND_43))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_43, MAX_PET_KIND_42))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_42, MAX_PET_KIND_41))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_41, MAX_PET_KIND_40))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_40, MAX_PET_KIND_39))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_39, MAX_PET_KIND_38))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo46(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_46(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_45)
	if (LoadAlbum_45(id))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_45, MAX_PET_KIND_44))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_44, MAX_PET_KIND_43))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_43, MAX_PET_KIND_42))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_42, MAX_PET_KIND_41))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_41, MAX_PET_KIND_40))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_40, MAX_PET_KIND_39))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_39, MAX_PET_KIND_38))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo45(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_45(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_44)
	if (LoadAlbum_44(id))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_44, MAX_PET_KIND_43))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_43, MAX_PET_KIND_42))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_42, MAX_PET_KIND_41))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_41, MAX_PET_KIND_40))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_40, MAX_PET_KIND_39))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_39, MAX_PET_KIND_38))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo44(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_44(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_43)
	if (LoadAlbum_43(id))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_43, MAX_PET_KIND_42))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_42, MAX_PET_KIND_41))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_41, MAX_PET_KIND_40))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_40, MAX_PET_KIND_39))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_39, MAX_PET_KIND_38))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo43(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_43(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_42)
	if (LoadAlbum_42(id))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_42, MAX_PET_KIND_41))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_41, MAX_PET_KIND_40))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_40, MAX_PET_KIND_39))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_39, MAX_PET_KIND_38))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo42(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_42(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_41)
	if (LoadAlbum_41(id))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_41, MAX_PET_KIND_40))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_40, MAX_PET_KIND_39))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_39, MAX_PET_KIND_38))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo41(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_41(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_40)
	if (LoadAlbum_40(id))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_40, MAX_PET_KIND_39))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_39, MAX_PET_KIND_38))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo40(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_40(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_39)
	if (LoadAlbum_39(id))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_39, MAX_PET_KIND_38))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo39(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_39(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_38)
	if (LoadAlbum_38(id))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_38, MAX_PET_KIND_37))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo38(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_38(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_37)
	if (LoadAlbum_37(id))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_37, MAX_PET_KIND_36))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo37(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_37(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_36)
	if (LoadAlbum_36(id))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_36, MAX_PET_KIND_35))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo36(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_36(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_35)
	if (LoadAlbum_35(id))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_35, MAX_PET_KIND_34))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo35(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_35(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_34)
	if (LoadAlbum_34(id))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_34, MAX_PET_KIND_33))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo34(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_34(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_33)
	if (LoadAlbum_33(id))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_33, MAX_PET_KIND_32))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo33(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_33(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_32)
	if (LoadAlbum_32(id))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_32, MAX_PET_KIND_31))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_31, MAX_PET_KIND_30))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_30, MAX_PET_KIND_29))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_29, MAX_PET_KIND_28))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_28, MAX_PET_KIND_27))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_27, MAX_PET_KIND_26))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_26, MAX_PET_KIND_25))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_25, MAX_PET_KIND_24))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_24, MAX_PET_KIND_23))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_23, MAX_PET_KIND_22))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_22, MAX_PET_KIND_21))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_21, MAX_PET_KIND_20))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_20, MAX_PET_KIND_19))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_19, MAX_PET_KIND_18))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_18, MAX_PET_KIND_17))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_17, MAX_PET_KIND_16))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_16, MAX_PET_KIND_15))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo32(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_32(id))
		return TRUE;
	return FALSE;
#elif defined(__ALBUM_15)
	if (LoadAlbum_15(id))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_15, MAX_PET_KIND_14))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_14, MAX_PET_KIND_13))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_13, MAX_PET_KIND_12))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_12, MAX_PET_KIND_11))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_10, MAX_PET_KIND_10))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_9, MAX_PET_KIND_9))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_8, MAX_PET_KIND_8))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_7, MAX_PET_KIND_7))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_6, MAX_PET_KIND_6))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_5, MAX_PET_KIND_5))
		return TRUE;
	if (ConvertAlbumTo15(id, ALBUM_FILE_NAME_4, MAX_PET_KIND_4))
		return TRUE;
	if (ConvertAlbum1_15(id))
		return TRUE;
	return FALSE;
#else
	char id2[16];
	int i;
	FILE *fp;
	// ?????????
	AlbumIdCnt = 0;
	// ???????????
	if ((fp = fopen(ALBUM_FILE_NAME, "r+b")) == NULL) {

		// ???????
		if ((fp = fopen(ALBUM_FILE_NAME, "wb")) != NULL) {
			// ????????????
			id[id[15]] = Rnd(1, 100);
			// ??????
			for (i = 0; i < 16; i++) {
				id[i] ^= IdEncryptionTbl[i];

			}
			// ?????????
			fwrite(id, sizeof(id), 1, fp);

			// ???????????
			for (i = 0; i < MAX_PET_KIND; i++) {
				int j;
				// ???
				for (j = 0; j < PET_NAME_LEN + 1; j++) {
					PetAlbum[i].name[j] = Rnd(1, 255);
					PetAlbum[i].freeName[j] = Rnd(1, 255);
				}

				PetAlbum[i].faceGraNo = Rnd(1, 60000);
				PetAlbum[i].level = Rnd(1, 30000);
				PetAlbum[i].faceGraNo = Rnd(1, 30000);
				PetAlbum[i].maxHp = Rnd(1, 30000);
				PetAlbum[i].str = Rnd(1, 30000);
				PetAlbum[i].quick = Rnd(1, 30000);
				PetAlbum[i].def = Rnd(1, 30000);

				PetAlbum[i].earth = Rnd(1, 30000);
				PetAlbum[i].water = Rnd(1, 30000);
				PetAlbum[i].fire = Rnd(1, 30000);
				PetAlbum[i].wind = Rnd(1, 30000);
			}

			// ???????
			fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			fclose(fp);	// ????????
		}

		return FALSE;
	}
	// ????????? ????? ?
	while (1) {
		// ??????
		if (fread(id2, sizeof(id2), 1, fp) < 1) {
			// ??????????
			// ????????????
			id[id[15]] = Rnd(1, 100);
			// ??????
			for (i = 0; i < 16; i++) {
				id[i] ^= IdEncryptionTbl[i];
			}
			// ?????????
			fwrite(id, sizeof(id), 1, fp);
			// ???????
			fwrite(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp);
			fclose(fp);	// ????????
			return FALSE;
		}
		// ?????????
		for (i = 0; i < 16; i++) {
			id2[i] ^= IdEncryptionTbl[i];
		}
		// ?????????
		id2[id2[15]] = NULL;
		// ??????
		if (strcmp(id, id2) != 0) {
			// ????????????????????
			if (fseek(fp, sizeof(PET_ALBUM) * MAX_PET_KIND, SEEK_CUR) != 0) {
				fclose(fp);	// ????????
				return FALSE;
			}
		}
		else {// ????
			  // ??????????
			if (fread(PetAlbum, sizeof(PET_ALBUM), MAX_PET_KIND, fp) < MAX_PET_KIND) {
				fclose(fp);	// ????????
				return FALSE;
			}
			break;
		}
		// ??????
		AlbumIdCnt++;
	}
	// ????????
	fclose(fp);

	return TRUE;
#endif
}

// ???????????? **************************************************/
void InitOhtaParam(void)
{
	LoadMailHistory();					// ??????????
	LoadChatHistoryStr();				// ????????????
										//LoadReadNameShield();
										//LoadReadSayShield();
	mailLamp = CheckMailNoReadFlag();	// ????
	StatusUpPoint = 0;					// ???????????????
	battleResultMsg.useFlag = 0;		// ????????
	MapWmdFlagBak = 0;
}

// ??????? ****************************************************************/
BOOL CheckMailNoReadFlag(void)
{
	int i, j;

	// ???
	for (i = 0; i < MAX_ADR_BOOK; i++) {
		// ?????
		for (j = 0; j < MAIL_MAX_HISTORY; j++) {
			// ???????
			if (MailHistory[i].noReadFlag[j] >= TRUE) return TRUE;
		}
	}
	return FALSE;
}

// ???????? ****************************************************************/
void CheckNewPet(int sprNo)
{
	int tblNo = sprNo - 100250; // ??????
	int albumNo;
	// ?????
#if defined(__ALBUM_47)
	if (tblNo > 1800) {					//小恶魔
		tblNo -= 1208;
	}
	else if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_46)
	if (tblNo > 1800) {					//小恶魔
		tblNo -= 1208;
	}
	else if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_45)
	if (tblNo > 1800) {					//小恶魔
		tblNo -= 1208;
	}
	else if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_44)
	if (tblNo > 1800) {					//小恶魔
		tblNo -= 1208;
	}
	else if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_43)
	if (tblNo > 1800) {					//小恶魔
		tblNo -= 1208;
	}
	else if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_42)
	if (tblNo > 1800) {					//小恶魔
		tblNo -= 1208;
	}
	else if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_41)
	if (tblNo > 1800) {					//小恶魔
		tblNo -= 1208;
	}
	else if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_40)
	if (tblNo > 1800) {					//小恶魔
		tblNo -= 1208;
	}
	else if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_39)
	if (tblNo > 1800) {					//小恶魔
		tblNo -= 1208;
	}
	else if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_38)

	if (tblNo > 1755) {					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201;
	}
	else if (tblNo > 1739) { //1710 ){			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_37)

	if (tblNo > 1710) {			//	间隔骑宠后的 飞蛇
		tblNo -= 1200;
	}
	else if (tblNo > 1686) {			//海底融合宠
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_36)

	if (tblNo > 1686) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1175;
	}
	else if (tblNo > 1641) {			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_35)

	if (tblNo > 1641) {			//鸡年兽4 甲虫2 8.0第一次整合测试
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_34)

	if (tblNo > 1639) {			//鸡年兽4
		tblNo -= 1167;
	}
	else if (tblNo > 1635) {	//麒麟
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 麒麟
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_33)

	if (tblNo > 1635) {
		tblNo -= 1148;
	}
	else if (tblNo > 1634) {	//猫女1 猫女2 麒麟
		tblNo -= 1149;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_32)

	if (tblNo > 1634) {	//猫女1 猫女2
		tblNo -= 1149;
	}
	else if (tblNo > 1628) {	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_31)

	if (tblNo > 1628) {	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}
	else if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_30)

	if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_29)

	if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_28)

	if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_27)

	if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_26)

	if (tblNo > 1616) {//布里萨尔  蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_25)
	if (tblNo > 1616) {//布里萨尔
		tblNo -= 1146;
	}
	else if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_24)
	if (tblNo > 1568) {//机人龙
		tblNo -= 1103;
	}
	else if (tblNo > 1564) {//黄色飞龙
		tblNo -= 1101;
	}
	else if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_23)
	if (tblNo > 1516) {
		tblNo -= 1055;
	}
	else if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_22)
	if (tblNo == 1516) {//修正乌力王
		tblNo = 455;
	}
	else if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_21)
	if (tblNo > 1509) {
		tblNo -= 1054;
	}
	else if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_20)
	if (tblNo == 1497) {
		tblNo = 395;
	}
	else if (tblNo > 1495) {
		tblNo -= 1044;
	}
	else if (tblNo > 1490) {
		tblNo -= 1043;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_19)
	if (tblNo > 1490) {
		tblNo -= 1044;
	}
	else if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_18)
	if (tblNo > 1485) {
		tblNo -= 1040;
	}
	else if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_17)
	if (tblNo > 1454) {
		tblNo -= 1034;
	}
	else if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}

#elif defined(__ALBUM_16)
	if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#elif defined(__ALBUM_15)
	if (tblNo == 1214) {
		tblNo = 331;
	}
	else if (tblNo == 1217) {
		tblNo = 328;
	}
	else if (tblNo > 1361) {
		tblNo -= 952;
	}
	else if (tblNo > 1356) {
		tblNo -= 999;
	}
	else if (tblNo > 1281) {
		tblNo -= 919;
	}
	else if (tblNo > 1239) {
		tblNo -= 889;
	}
	else if (tblNo > 1186) {
		tblNo -= 886;
	}
	else if (tblNo > 1173) {
		tblNo -= 877;
	}
	else if (tblNo > 1172) {
		tblNo -= 877;
	}
	else if (tblNo > 1161) {
		tblNo -= 868;
	}
	else if (tblNo > 930) {
		tblNo -= 745;
	}
	else if (tblNo == 927) {
		tblNo -= 634;
	}
	else if (tblNo > 929) {
		tblNo -= 644;
	}
	else if (tblNo > 927) {
		tblNo -= 643;
	}
	else if (tblNo > 900) {
		tblNo -= 621;
	}
	else if (tblNo > 180) {
		tblNo -= (333 + 56);
	}
#endif
	// ????????????
	if (0 <= tblNo && tblNo < MAX_PET_TBL) {
		// ???????????
		albumNo = PetAlbumTbl[tblNo].albumNo;
		// ????????
		if (albumNo != -1) {
			// ???????????
			if (PetAlbum[albumNo].flag == 0) {
				// ???????
				PetAlbum[albumNo].flag = 1;
				// ???
				strcpy(PetAlbum[albumNo].name, PetAlbumTbl[tblNo].name);
				// ?????
				SaveAlbum(albumNo);
			}
		}
	}
}

// ?????????? **********************************************************/
void CheckBattleNewPet(void)
{
	int i;

	// ?????
	for (i = 0; i < 20; i++) {
		// ??????????
		if (p_party[i] == NULL) continue;
		// ??????????
		if (p_party[i]->func == NULL) continue;
		// ?????????
		CheckNewPet(p_party[i]->anim_chr_no);
	}
}

// ?????????? *****************************************************/
void CenteringStr(char *inStr, char *outStr, int max)
{
	int len, space, amari;

	// ?????
	len = strlen(inStr);

	// ????????
	if (len >= max) {
		// ???????
		strncpy(outStr, inStr, max);
		outStr[max] = NULL;	// ??????
		return;
	}

	// ???????
	amari = (max - len) % 2;
	// ?????
	space = (max - len) / 2;

	// ?????????
	if (space != 0) {
		// ???
		sprintf(outStr, "%*c%s%*c", space, ' ', inStr, space + amari, ' ');
	}
	else {
		// ???
		sprintf(outStr, "%s ", inStr);
	}
}

// ????????????????? *************************************/
int CheckPetSkill(int skillId) {
	int i, j;
	for (j = 0; j < MAX_PET; j++) {
		if (pet[j].useFlag == TRUE) {
			for (i = 0; i < pet[j].maxSkill; i++) {
				if (petSkill[j][i].useFlag == TRUE) {
					if (petSkill[j][i].skillId == skillId) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

// ?????????? *******************************************************/
//初始化装备道具
//包括PetItem
//不包括下面的15个道具
void InitItem(int x, int y, BOOL bPetItemFlag)
{
	int i, j = 0, k = 0, q, p;
	//printf("%d.%d",x,y);
#ifdef _ITEM_EQUITSPACE
	struct _tagInitXY {
		int x;
		int y;
	}InitXY[CHAR_EQUIPPLACENUM] = {
		{ x + 84, y + 51 },{ x + 85, y + 119 },{ x + 33, y + 122 },
		{ x + 137, y + 63 },{ x + 33, y + 63 },{ x + 137, y + 122 },
		{ x + 137, y + 122 },{ x + 136, y + 130 }
#ifdef _EQUIT_NEWGLOVE
		,{ x + 137, y + 122 }
#endif
	};
#endif
	/*for (i = 0; i < MAX_ITEMSTART; i++)
	{
		ItemBuffer[i].defX = InitXY[i].x;
		ItemBuffer[i].defY = InitXY[i].y;
		ItemBuffer[i].x = ItemBuffer[0].defX;
		ItemBuffer[i].y = ItemBuffer[0].defY;
		ItemBuffer[i].bmpNo = 20000 + 0;
		ItemBuffer[i].dispPrio = DISP_PRIO_ITEM;
	}*/

		//헬멧   5칸 X + 137 Y + 61      /  9칸 X + 91 Y + 46
	ItemBuffer[0].defX = x + 91;
	ItemBuffer[0].defY = y + 46;
	ItemBuffer[0].x = ItemBuffer[0].defX;
	ItemBuffer[0].y = ItemBuffer[0].defY;
	ItemBuffer[0].bmpNo = 20000 + 0;
	ItemBuffer[0].dispPrio = DISP_PRIO_ITEM;
	//옷(/코스튬)	5칸 X + 130 Y + 123      /  9칸 X + 91 Y + 94
	ItemBuffer[1].defX = x + 91;
	ItemBuffer[1].defY = y + 94;
	ItemBuffer[1].x = ItemBuffer[1].defX;
	ItemBuffer[1].y = ItemBuffer[1].defY;
	ItemBuffer[1].bmpNo = 20200 + 1;
	ItemBuffer[1].dispPrio = DISP_PRIO_ITEM;
	//[무기]			5칸 X + 71 Y + 123      /  9칸 X + 41 Y + 94
	ItemBuffer[2].defX = x + 41;
	ItemBuffer[2].defY = y + 94;
	ItemBuffer[2].x = ItemBuffer[2].defX;
	ItemBuffer[2].y = ItemBuffer[2].defY;
	ItemBuffer[2].bmpNo = 20400 + 2;
	ItemBuffer[2].dispPrio = DISP_PRIO_ITEM;
	//왼쪽 장신구	5칸 X + 37 Y + 70      /  9칸 X + 41 Y + 46
	ItemBuffer[3].defX = x + 41;
	ItemBuffer[3].defY = y + 46;
	ItemBuffer[3].x = ItemBuffer[3].defX;
	ItemBuffer[3].y = ItemBuffer[3].defY;
	ItemBuffer[3].bmpNo = 20600 + 3;
	ItemBuffer[3].dispPrio = DISP_PRIO_ITEM;
	//오른쪽 장신구	5칸 X + 87 Y + 70      /  9칸 X + 141 Y + 46
	ItemBuffer[4].defX = x + 141;
	ItemBuffer[4].defY = y + 46;
	ItemBuffer[4].x = ItemBuffer[4].defX;
	ItemBuffer[4].y = ItemBuffer[4].defY;
	ItemBuffer[4].bmpNo = 20800 + 4;
	ItemBuffer[4].dispPrio = DISP_PRIO_ITEM;

	// CHAR_EQBELT
	ItemBuffer[5].defX = x + 91;
	ItemBuffer[5].defY = y + 142;
	ItemBuffer[5].x = ItemBuffer[5].defX;
	ItemBuffer[5].y = ItemBuffer[5].defY;
	ItemBuffer[5].bmpNo = 21000 + 5;
	ItemBuffer[5].dispPrio = DISP_PRIO_ITEM;

	// CHAR_EQSHIELD
	ItemBuffer[6].defX = x + 141;
	ItemBuffer[6].defY = y + 94;
	ItemBuffer[6].x = ItemBuffer[6].defX;
	ItemBuffer[6].y = ItemBuffer[6].defY;
	ItemBuffer[6].bmpNo = 21000 + 5;
	ItemBuffer[6].dispPrio = DISP_PRIO_ITEM;

	// CHAR_EQSHOES
	ItemBuffer[7].defX = x + 141;
	ItemBuffer[7].defY = y + 142;
	ItemBuffer[7].x = ItemBuffer[7].defX;
	ItemBuffer[7].y = ItemBuffer[7].defY;
	ItemBuffer[7].bmpNo = 21000 + 5;
	ItemBuffer[7].dispPrio = DISP_PRIO_ITEM;

	// CHAR_EQGLOVE
	ItemBuffer[8].defX = x + 41;
	ItemBuffer[8].defY = y + 142;
	ItemBuffer[8].x = ItemBuffer[8].defX;
	ItemBuffer[8].y = ItemBuffer[8].defY;
	ItemBuffer[8].bmpNo = 21000 + 5;
	ItemBuffer[8].dispPrio = DISP_PRIO_ITEM;

	for (i = MAX_ITEMSTART; i < MAX_ITEM; i++) {
		ItemBuffer[i].defX = x + 36 + j;
		ItemBuffer[i].defY = y + 64 + 48 + 48 + 48 + k;
		ItemBuffer[i].x = ItemBuffer[i].defX;
		ItemBuffer[i].y = ItemBuffer[i].defY;
		ItemBuffer[i].bmpNo = 20000 + j / 48;
		ItemBuffer[i].dispPrio = DISP_PRIO_ITEM;
		ItemBuffer[i].mixFlag = FALSE;
		j += 51;
		if (j >= 48 * 5) {
			j = 0;
			k += 48;
		}
		if (k >= 47 * 3)
		{
			k = 0;
		}
	}
}

// CoolFish: Trade 04/14
void InitItem3(int x, int y) {
	int i, j = 0, k = 0;
	for (i = MAX_ITEMSTART; i < MAX_ITEM; i++) {
		ItemBuffer[i].defX = x + 32 + j;
		ItemBuffer[i].defY = y + 56 + 48 + 48 + 48 - 160 + k;
		ItemBuffer[i].x = ItemBuffer[i].defX;
		ItemBuffer[i].y = ItemBuffer[i].defY;
		ItemBuffer[i].bmpNo = 20000 + j / 48;
		ItemBuffer[i].dispPrio = DISP_PRIO_ITEM;
		j += 51;
		if (j >= 48 * 5) {
			j = 0;
			k += 48;
		}
	}
}
// CoolFish: End 

// ******************************************************/
// MakeHitBox
// 只是为了显示一个绿色的框
// 没有别的用途
BOOL MakeHitBox(int x1, int y1, int x2, int y2, int dispPrio) {
	if (mouse.nowPoint.x <= x2 && x1 <= mouse.nowPoint.x &&
		mouse.nowPoint.y <= y2 && y1 <= mouse.nowPoint.y) {
		if (dispPrio >= 0) {
			StockBoxDispBuffer(x1, y1, x2, y2, dispPrio, BoxColor, 0);
		}
		return TRUE;
	}
	return FALSE;
}

void DeathLetterAction(void) {
	int i;

	for (i = 0; i < 4; i++) {
		DeathAction(pActLetter[i]);
		pActLetter[i] = NULL;
	}
}

void InitMailSendFlag(void) {
	int i;
	for (i = 0; i < MAX_ADR_BOOK; i++) mailWndSendFlag[i] = 0;
}

void DeathMenuAction(void) {
	if (Mammoth::Get().IsOpen())
		Mammoth::Get().Close();

	DeathAction(pActMenuWnd);
	pActMenuWnd = NULL;
#ifdef _FRIENDCHANNEL			//ROG ADD 好友频道
	chatRoomBtn = 0;
	assentFlag = FALSE;
	DeathAction(pAssentWnd);
	pAssentWnd = NULL;
	setRoomFlag = FALSE;
	DeathAction(pSetRoomWnd);
	pSetRoomWnd = NULL;
#endif

	// ??????????
	DeathAction(pActMenuWnd3);
	pActMenuWnd3 = NULL;
	// ????????
	GetKeyInputFocus(&MyChatBuffer);
	// ????
	DeathAction(pActPet);
	pActPet = NULL;
	// ????????
	DeathAction(pActMailItem);
	pActMailItem = NULL;
	// ???????
	DeathAction(pActYesNoWnd);
	pActYesNoWnd = NULL;
	// ?????????
	DeathLetterAction();

	// Robin 04/14 trade
	if (pActMenuWnd4) {
		DeathAction(pActMenuWnd4);
		pActMenuWnd4 = NULL;
	}
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
	//清除所使用Action
	if (SecondTradeWnd) {
		DeathAction(SecondTradeWnd);
		SecondTradeWnd = NULL;
	}
	if (SecondActPet) {
		DeathAction(SecondActPet);
		SecondActPet = NULL;
	}
	if (TradeTalkWnd) {
		DeathAction(TradeTalkWnd);
		TradeTalkWnd = NULL;
	}
#endif
	if (pActPet3) {
		DeathAction(pActPet3);
		pActPet3 = NULL;
	}

	if (pActPet4) {
		DeathAction(pActPet4);
		pActPet4 = NULL;
	}
	if (pActPet5) {
		DeathAction(pActPet5);
		pActPet5 = NULL;
	}
	if (pActMenuWnd5) {
		DeathAction(pActMenuWnd5);
		pActMenuWnd5 = NULL;
	}
}
// ????????????? *****************************************************/
void DeathMenuAction2(void) {
	if (Mammoth::Get().IsOpen())
		Mammoth::Get().Close();
	DeathAction(pActMenuWnd2);
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
	GetKeyInputFocus(&MyChatBuffer);
	Moneyflag = false;
#endif
	pActMenuWnd2 = NULL;
	DeathAction(pActPet2);
	pActPet2 = NULL;
	ItemMixPetNo = -1;

	// Robin 04/14 trade
	DeathAction(pActMenuWnd4);
#ifdef _FRIENDCHANNEL			//ROG ADD 好友频道
	chatRoomBtn = 0;
	assentFlag = FALSE;
	DeathAction(pAssentWnd);
	pAssentWnd = NULL;
	setRoomFlag = FALSE;
	DeathAction(pSetRoomWnd);
	pSetRoomWnd = NULL;
#endif

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
	//清除所使用Action
	DeathAction(SecondTradeWnd);
	SecondTradeWnd = NULL;
	DeathAction(SecondActPet);
	SecondActPet = NULL;
	DeathAction(TradeTalkWnd);
	TradeTalkWnd = NULL;
#endif
	//视窗消灭时还原设定
	Tradeflag = false;
	pActMenuWnd4 = NULL;
	DeathAction(pActPet3);
	pActPet3 = NULL;
	DeathAction(pActPet4);
	pActPet4 = NULL;
	DeathAction(pActPet5);
	pActPet5 = NULL;
	DeathAction(pActMenuWnd5);
	pActMenuWnd5 = NULL;

}

// ?????????? *******************************************************/
void InitMenu(void) {
	int i;
	for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
	for (i = 0; i < MENU_MAP_0; i++) mapWndFontNo[i] = -2;
	mapWndNo = 0;
	for (i = 0; i < MENU_STATUS_0; i++) statusWndFontNo[i] = -2;
	for (i = 0; i < MENU_STATUS_0; i++) statusWndBtnFlag[i] = 0;
	for (i = 0; i < MENU_TRADE_0; i++) tradeWndFontNo[i] = -2;
	for (i = 0; i < MENU_TRADE_0; i++) tradeWndBtnFlag[i] = 0;
	for (i = 0; i < MENU_PET_0; i++) petWndFontNo[i] = -2;
	petWndNo = 0;
	for (i = 0; i < MENU_ITEM_0; i++) itemWndFontNo[i] = -2;
	for (i = 0; i < MENU_ITEM_0; i++) itemWndBtnFlag[i] = 0;
	for (i = 0; i < 3; i++) itemWndBtnBag[i] = -2;
	itemWndNo = 0;
	itemWndBagOffset = 0;
	for (i = 0; i < MENU_MAIL_0; i++) mailWndFontNo[i] = -2;
	for (i = 0; i < MAX_ADR_BOOK; i++) mailWndSendFlag[i] = 0;
	mailWndNo = MAIL_WND_VIEW;
	for (i = 0; i < MENU_ALBUM_0; i++) albumWndFontNo[i] = -2;
	albumWndNo = 0;
	for (i = 0; i < TASK_BAR; i++) taskBarFontNo[i] = -2;
	for (i = 0; i < MENU_BTL_RESULT_0; i++) resultWndFontNo[i] = -2;
	MenuToggleFlag = 0;
#ifdef _TELLCHANNEL				//ROG ADD 密语频道
	MultiTells = FALSE;
#endif
	DeathMenuAction();
	DeathMenuAction2();
	mouse.itemNo = -1;
	itemNo = -1;
	jujutuNo = -1;
	ItemMixPetNo = -1;
#ifdef __AI
	void AI_CloseWnd();
	AI_CloseWnd();
#endif
}


void InitMenu2(void) {
	MenuToggleFlag = 0;
	DeathMenuAction();
	DeathMenuAction2();
	mouse.itemNo = -1;
	itemNo = -1;
	jujutuNo = -1;
	BattleResultWndFlag = FALSE;
}

// ??????????? *******************************************************/
BOOL CheckMenuFlag(void) {
	BOOL flag = FALSE;
	if (MenuToggleFlag != 0) flag = TRUE;
	if (BattleResultWndFlag >= 1) flag = TRUE;
	if (MapWmdFlagBak >= TRUE) flag = TRUE;
#ifdef _FRIENDCHANNEL
	if (assentFlag) flag = assentFlag;
	if (setRoomFlag) flag = setRoomFlag;
#endif
	return flag;
}

#ifdef __PHONEMESSAGE
void CleanSMS() {
	extern STR_BUFFER ptext;
	extern HANDLE MHandle;
	ptext.buffer[0] = '\0';
	if (MHandle) {
		TerminateThread(MHandle, 1);
		CloseHandle(MHandle);
	}
}
#endif


#ifdef _NB_断线自动重连
BOOL 自动登陆窗口 = FALSE;
ACTION *p自动登陆窗口 = NULL;
ACTION * 方向动作;
ACTION * 人物动作;
extern struct gameserver gmsv[];
extern struct gamegroup  gmgroup[];
void 自动登陆窗口回调()
{
	static int 方向窗口是否打开;
	static int btnId[15], btnState[15];
	static int x = 0, y = 0;
	static DWORD dwPressTime;
	static char msg[][8] = {
		"팀 장",
		"팀 원",
		"솔 로"
	};
	static char  chardir[][8] = {
		"아래",
		"좌하",
		"좌측",
		"좌상",
		"위",
		"우상",
		"우측",
		"우하"
	};
	if (p自动登陆窗口 == NULL) {
		方向窗口是否打开 = FALSE;
		方向动作 = NULL;
		人物动作 = NULL;
		int w = 412;
		int h = 301;
		if (ResoMode == 分辨率::mode800) {
			x = 800 / 2;
			y = 600 / 2;
		}
		else if (ResoMode == 分辨率::mode1024) {
			x = 1024 / 2;
			y = 768 / 2;
		}
		else {
			x = 640 / 2;
			y = 480 / 2;
		}
		//x = (lpDraw->xSize - w) / 2;
		//y = (lpDraw->ySize - h) / 2;
		p自动登陆窗口 = MakeWindowDisp(x, y, w, h, 52137, -1);
		for (int i = 0; i < 14; i++) {
			btnId[i] = -2;
			if (i < 10)
				btnState[i] = 0;
		}
		dwPressTime = 0;
	}
	else {
		if (p自动登陆窗口->hp >= 1) {
			if (joy_trg[0] & JOY_ESC) {
				自动登陆窗口 = FALSE;
				DeathAction(p自动登陆窗口);
				DeathAction(方向动作);
				DeathAction(人物动作);
				人物动作 = NULL;
				方向窗口是否打开 = FALSE;
				方向动作 = NULL;
				p自动登陆窗口 = NULL;
				actBtn = 0;
				return;
			}
			if (dwPressTime) {
				if (TimeGetTime() > (dwPressTime + 100)) {
					dwPressTime = 0;
					if (btnState[0] == 1) {//大区减
						btnState[0] = 0;
						play_se(217, 320, 240);
						while (TRUE) {
							PcLanded.大区--;
							if (PcLanded.大区 < 0) {
								PcLanded.大区 = 18;
							}
							if (gmgroup[PcLanded.大区].used) {
								break;
							}
						}
					}
					if (btnState[1] == 1) {//大区加
						btnState[1] = 0;
						play_se(217, 320, 240);
						while (TRUE) {
							PcLanded.大区++;
							if (PcLanded.大区 > 18) {
								PcLanded.大区 = 0;
							}
							if (gmgroup[PcLanded.大区].used) {
								break;
							}
						}
					}

					if (btnState[2] == 1) {//小区减
						btnState[2] = 0;
						play_se(217, 320, 240);
						if (PcLanded.大区 != -1)
						{
							while (TRUE) {
								PcLanded.小区--;
								if (PcLanded.小区 < 0) {
									PcLanded.小区 = gmgroup[PcLanded.大区].num - 1;
								}
								if (gmsv[gmgroup[PcLanded.大区].startindex + PcLanded.小区].used) {
									break;
								}
							}
						}
					}
					if (btnState[3] == 1) {//小区加
						btnState[3] = 0;
						play_se(217, 320, 240);
						if (PcLanded.大区 != -1)
						{
							while (TRUE) {
								PcLanded.小区++;
								if (PcLanded.小区 >= gmgroup[PcLanded.大区].num) {
									PcLanded.小区 = 0;
								}
								if (gmsv[gmgroup[PcLanded.大区].startindex + PcLanded.小区].used) {
									break;
								}
							}
						}
					}
					if (btnState[4] == 1) {//队模减
						btnState[4] = 0;
						play_se(217, 320, 240);
						PcLanded.队模--;
						if (PcLanded.队模 < 0) {
							PcLanded.队模 = 2;
						}
						if (PcLanded.队模 == 0 || PcLanded.队模 == 2) {
							PcLanded.是否自动喊话 = FALSE;
						}
						if (PcLanded.队模 == 1) {
							PcLanded.是否自动遇敌 = FALSE;
						}
					}
					if (btnState[5] == 1) {//队模加
						btnState[5] = 0;
						play_se(217, 320, 240);
						PcLanded.队模++;
						if (PcLanded.队模 > 2) {
							PcLanded.队模 = 0;
						}
						if (PcLanded.队模 == 0 || PcLanded.队模 == 2) {
							PcLanded.是否自动喊话 = FALSE;
						}
						if (PcLanded.队模 == 1) {
							PcLanded.是否自动遇敌 = FALSE;
						}
					}
					if (btnState[6] == 1) {//角色减
						btnState[6] = 0;
						play_se(217, 320, 240);
						while (TRUE) {
							PcLanded.人物--;
							if (PcLanded.人物 < 0) {
								PcLanded.人物 = 3;
							}
							if (PcLanded.登陆人物名称[PcLanded.人物][0]) break;
						}
					}
					if (btnState[7] == 1) {//角色加
						btnState[7] = 0;
						play_se(217, 320, 240);
						while (TRUE) {
							PcLanded.人物++;
							if (PcLanded.人物 > 3) {
								PcLanded.人物 = 0;
							}
							if (PcLanded.登陆人物名称[PcLanded.人物][0]) break;
						}
					}
					if (btnState[8] == 1) {//확인
						btnState[8] = 0;
						play_se(217, 320, 240);
						DeathAction(p自动登陆窗口);
						p自动登陆窗口 = NULL;
						自动登陆窗口 = FALSE;
						if (方向动作) {
							DeathAction(方向动作);
							方向动作 = NULL;
							DeathAction(人物动作);
							人物动作 = NULL;
						}
						extern short actBtn;
						actBtn = 0;
						return;
					}
					if (btnState[9] == 1) {//취소
						btnState[9] = 0;
						play_se(217, 320, 240);
						DeathAction(p自动登陆窗口);
						p自动登陆窗口 = NULL;
						自动登陆窗口 = FALSE;
						if (方向动作) {
							DeathAction(方向动作);
							方向动作 = NULL;
							DeathAction(人物动作);
							人物动作 = NULL;
						}
						extern short actBtn;
						actBtn = 0;
						return;
					}
					if (btnState[10] == 1) {//方向
						btnState[10] = 0;
						play_se(217, 320, 240);
						DeathAction(方向动作);
						方向动作 = NULL;
						DeathAction(人物动作);
						人物动作 = NULL;
						方向窗口是否打开 = FALSE;
					}
				}
			}
			else {
				for (int i = 0; i < 10; i++) {
					if (HitDispNo == btnId[i]) {
						if (i == 0 || i == 1) ShowBottomLineString(FONT_PAL_WHITE, "대 지역 설정。");
						if (i == 2 || i == 3) ShowBottomLineString(FONT_PAL_WHITE, "라인을 설정하고, 대 지역을 설정해야 함。");
						if (i == 4 || i == 5) ShowBottomLineString(FONT_PAL_WHITE, "팀장/파티장：자동으로 적을만나게끔 수정할수있음. 팀원/파티원：선전 및 자동 팀 구성가능 솔로：자동으로 적을 만나도록 설정가능 ps：AI 모드는 캐릭터를 드롭하고 다시 로그인하면 자동으로 켜집니다。");
						if (i == 6 || i == 7) ShowBottomLineString(FONT_PAL_WHITE, "로그온 캐릭터 설정。");
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							dwPressTime = TimeGetTime();
							btnState[i] = 1;
							break;
						}
					}
				}
				if (HitDispNo == btnId[14]) {
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						dwPressTime = TimeGetTime();
						btnState[10] = 1;
					}
				}
			}

			extern int 自动登陆是否开启;
			if (HitFontNo == btnId[10]) {
				ShowBottomLineString(FONT_PAL_WHITE, "로그인 후 F9키를 눌러서 켜고 끌수 있으며, 파티원 모드로 로그인후 큰소리 대화 기능을 끌 수 있습니다。");
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					if (PcLanded.大区 != -1 && PcLanded.小区 != -1 && PcLanded.人物 != -1) {
						自动登陆是否开启 = !自动登陆是否开启;
						if (!自动登陆是否开启) {
							extern int 是否重登组队, 是否重登喊话, 是否重开登组队_1;
							是否重登组队 = FALSE;
							是否重登喊话 = FALSE;
							是否重开登组队_1 = FALSE;
						}
					}
					else {
						StockChatBufferLine("대 지역 라인과 역할을 먼저 선택하 후 조작할 수 있습니다！", FONT_PAL_RED);
					}
				}
			}
			if (HitFontNo == btnId[12]) {
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					ShowBottomLineString(FONT_PAL_WHITE, "파티장과 솔로 플레이어는 이 기능을 설정할 수 있음。");
					if (PcLanded.队模 == 0 || PcLanded.队模 == 2)
						PcLanded.是否自动遇敌 = !PcLanded.是否自动遇敌;
					else {
						PcLanded.是否自动遇敌 = FALSE;
						StockChatBufferLine("적 출현 모드는 파티장과 솔로 플레이어만 켤 수 있음！", FONT_PAL_RED);
					}
				}
			}
			if (HitFontNo == btnId[13]) {
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					ShowBottomLineString(FONT_PAL_WHITE, "파티원만 이 기능을 설정할 수 있음。");
					if (PcLanded.队模 == 1)
						PcLanded.是否自动喊话 = !PcLanded.是否自动喊话;
					else {
						PcLanded.是否自动喊话 = FALSE;
						StockChatBufferLine("자동 외침은 파티원 모드에서만 활성화 할 수 있음！", FONT_PAL_RED);
					}
				}
			}
			if (HitFontNo == btnId[11]) {
				ShowBottomLineString(FONT_PAL_WHITE, "접속이끊긴 캐릭터 재접속 시 캐릭터의 방향。");
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					if (方向窗口是否打开) {
						方向窗口是否打开 = FALSE;
						if (方向动作) {
							DeathAction(方向动作);
							方向动作 = NULL;
							DeathAction(人物动作);
							人物动作 = NULL;
						}
					}
					else 方向窗口是否打开 = TRUE;
				}
			}
			int 方向偏移X = 0;
			if (方向窗口是否打开) {
				方向偏移X = -90;
				if (!方向动作) {
					方向动作 = MakeWindowDisp(x + 220 + 方向偏移X, y - 90, 192, 148, 52138, -1);
					人物动作 = MakeAnimDisp(x + 270 + 方向偏移X, y, pc.graNo, 0);
					人物动作->anim_ang = PcLanded.人物方向;
					pattern(人物动作, ANM_NOMAL_SPD, ANM_LOOP);
					btnId[14] = -2;
					btnState[10] = 0;
				}
				else if (方向动作->hp >= 1) {
					StockDispBuffer(x + 220, y, DISP_PRIO_MENU, 52138, 1);
					PcLanded.人物方向 = 人物动作->anim_ang;
					btnId[14] = StockDispBuffer(x + 120, y - 90, DISP_PRIO_IME3, 26262 + btnState[10], 2);
				}
			}
			StockDispBuffer(x + 方向偏移X, y, DISP_PRIO_MENU, 52137, 1);//打印底图
																	//大区按钮调节
			btnId[0] = StockDispBuffer(x - 81 + 方向偏移X, y + 10, DISP_PRIO_IME3, 26064 + btnState[0], 2);
			btnId[1] = StockDispBuffer(x - 100 + 方向偏移X, y + 26, DISP_PRIO_IME3, 26066 + btnState[1], 2);
			//线路按钮调节
			btnId[2] = StockDispBuffer(x + 79 + 方向偏移X, y + 10, DISP_PRIO_IME3, 26064 + btnState[2], 2);
			btnId[3] = StockDispBuffer(x + 60 + 方向偏移X, y + 26, DISP_PRIO_IME3, 26066 + btnState[3], 2);
			//队伍按钮调节
			btnId[4] = StockDispBuffer(x - 81 + 方向偏移X, y + 45, DISP_PRIO_IME3, 26064 + btnState[4], 2);
			btnId[5] = StockDispBuffer(x - 100 + 方向偏移X, y + 61, DISP_PRIO_IME3, 26066 + btnState[5], 2);
			//角色按钮调节
			btnId[6] = StockDispBuffer(x - 81 + 方向偏移X, y + 80, DISP_PRIO_IME3, 26064 + btnState[6], 2);
			btnId[7] = StockDispBuffer(x - 100 + 方向偏移X, y + 96, DISP_PRIO_IME3, 26066 + btnState[7], 2);

			btnId[8] = StockDispBuffer(x + 方向偏移X, y, DISP_PRIO_IME3, 26262 + btnState[8], 2);
			btnId[9] = StockDispBuffer(x + 方向偏移X, y, DISP_PRIO_IME3, 26264 + btnState[9], 2);

			char moji[256];
			if (PcLanded.大区 != -1) {
				StockFontBuffer(x - 110 + 方向偏移X, y - 90, FONT_PRIO_FRONT, FONT_PAL_YELLOW, gmgroup[PcLanded.大区].name, 0);
			}
			if (PcLanded.小区 != -1) {
				StockFontBuffer(x + 100 + 方向偏移X, y - 90, FONT_PRIO_FRONT, FONT_PAL_YELLOW, gmsv[gmgroup[PcLanded.大区].startindex + PcLanded.小区].name, 0);
			}
			if (PcLanded.队模 != -1) {
				StockFontBuffer(x - 110 + 方向偏移X, y - 53, FONT_PRIO_FRONT, FONT_PAL_YELLOW, msg[PcLanded.队模], 0);
			}
			if (PcLanded.人物 != -1) {
				StockFontBuffer(x - 110 + 方向偏移X, y - 16, FONT_PRIO_FRONT, FONT_PAL_YELLOW, PcLanded.登陆人物名称[PcLanded.人物], 0);
			}

			if (自动登陆是否开启)
				btnId[10] = StockFontBuffer(x - 70 + 方向偏移X, y + 21, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "켜다", 2);
			else
				btnId[10] = StockFontBuffer(x - 70 + 方向偏移X, y + 21, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "종료", 2);

			btnId[11] = StockFontBuffer(x + 100 + 方向偏移X, y - 53, FONT_PRIO_FRONT, FONT_PAL_YELLOW, chardir[PcLanded.人物方向], 2);
			if (PcLanded.是否自动遇敌)
				btnId[12] = StockFontBuffer(x + 100 + 方向偏移X, y - 16, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "켜다", 2);
			else
				btnId[12] = StockFontBuffer(x + 100 + 方向偏移X, y - 16, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "종료", 2);
			if (PcLanded.是否自动喊话)
				btnId[13] = StockFontBuffer(x + 100 + 方向偏移X, y + 21, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "켜다", 2);
			else
				btnId[13] = StockFontBuffer(x + 100 + 方向偏移X, y + 21, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "종료", 2);
		}
	}
}
#endif

#ifdef _AI_OTHER
extern int AI_Other_State;
extern void AI_OtherProc();
#endif

extern void ShowBottomLineString(int iColor, LPSTR lpstr);
void MenuProc(void) {
	int x, y, i;
	x = y = i = 0;
	char moji[256];
#ifdef __AI
	extern int AI_State;
	if (AI_State) {
		extern BOOL AI_SettingProc();
		if (AI_SettingProc()) {
			AI_State = 0;
			extern short actBtn;
			actBtn = 0;
		}
	}
#endif
#ifdef _NB_断线自动重连
	if (自动登陆窗口) {
		自动登陆窗口回调();
	}
#endif
#ifdef _AI_OTHER
	if (AI_Other_State) AI_OtherProc();
#endif

#ifdef _TELLCHANNEL				//ROG ADD 密语频道
	if (MultiTells == TRUE) {
		SelectChar();
		if (MenuToggleFlag & JOY_ESC)
			DeathTellChannel();
	}
#endif



#ifdef _TIMEBAR_FUNCTION
	if (timeBarFlag == TRUE) {
		DrawTimeBar();
	}
#endif

#ifdef __AI
	if (joy_trg[0] & JOY_ESC && checkFieldMenuFlag() == FALSE && !AI_State) {
#else
	if (joy_trg[0] & JOY_ESC && checkFieldMenuFlag() == FALSE) {
#endif
		if (GetImeString() == NULL) {
			if (MenuToggleFlag != 0 || BattleResultWndFlag >= 1 || Mammoth::Get().IsOpen()
				|| PetRanking::Instance().Check() ) {
				MenuToggleFlag = 0;
				BattleResultWndFlag = FALSE;
				mouse.itemNo = -1;
				itemNo = -1;
				jujutuNo = -1;
				if (MapWmdFlagBak != TRUE) {
					play_se(203, 320, 240);
				}
			}
			else {
				if (GetImeString() == NULL) {

					MenuToggleFlag ^= JOY_ESC;
					play_se(202, 320, 240);
					for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
					systemWndNo = 0;
					if (MenuToggleFlag & JOY_ESC) {
						MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;
						MenuToggleFlag |= JOY_ESC;
						BattleResultWndFlag = FALSE;
						play_se(202, 320, 240);
					}
					else {
						play_se(203, 320, 240);
					}
				}
			}
			DeathMenuAction();
			DeathMenuAction2();
			saveUserSetting();
		}
	}



	if (Mammoth::Get().IsOpen())
	{
		Mammoth::Get().Display();
		return;
	}



#ifdef __AI
	if (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[3] && !AI_State) {
#else
	if (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[3]) {
#endif
#ifdef _NB_新下任务栏
		TaskBarFlag = FALSE;
#endif
#ifdef _BATTLESKILL
		extern int wonflag;
		if (wonflag == 1)
			play_se(220, 320, 240);
		else {
#endif
			MenuToggleFlag ^= JOY_ESC;
			for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
			systemWndNo = 0;
			DeathMenuAction();
			saveUserSetting();
			if (MenuToggleFlag & JOY_ESC) {
				MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;
				MenuToggleFlag |= JOY_ESC;
				BattleResultWndFlag = FALSE;
				play_se(202, 320, 240);
			}
			else {
				play_se(203, 320, 240);
			}
#ifdef _BATTLESKILL
		}
#endif
	}

#ifdef __AI
	if (!AI_State && ((joy_trg[0] & JOY_CTRL_S && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[1]))) {
#else
	if ((joy_trg[0] & JOY_CTRL_S && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[1])) {
#endif
#ifdef _NB_新下任务栏
		TaskBarFlag = FALSE;
#endif
		if (ProcNo == PROC_BATTLE) {
			play_se(220, 320, 240);
		}
		else {
			MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S
			for (i = 0; i < MENU_STATUS_0; i++) statusWndFontNo[i] = -2;
			for (i = 0; i < MENU_STATUS_0; i++) statusWndBtnFlag[i] = 0;
			statusWndNo = 0;
			DeathMenuAction();
			saveUserSetting();
			if (MenuToggleFlag & JOY_CTRL_S) {
				MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;//可和道具界面和地图界面共存
				MenuToggleFlag |= JOY_CTRL_S;
				BattleResultWndFlag = FALSE;
				play_se(202, 320, 240);
			}
			else {
				play_se(203, 320, 240);
			}
		}
	}
#ifdef __AI
	if (!AI_State && joy_trg[0] & JOY_CTRL_T && GetImeString() == NULL) {
#else
	if (joy_trg[0] & JOY_CTRL_T && GetImeString() == NULL) {
#endif
		if (ProcNo == PROC_BATTLE) {
			play_se(220, 320, 240);
		}
		else {
			MenuToggleFlag ^= JOY_CTRL_T;
			for (i = 0; i < MENU_TRADE_0; i++) tradeWndFontNo[i] = -2;
			for (i = 0; i < MENU_TRADE_0; i++) tradeWndBtnFlag[i] = 0;

			tradeWndNo = 0;
			tradeInit();
			DeathMenuAction();
			DeathMenuAction2();
			saveUserSetting();
			if (MenuToggleFlag & JOY_CTRL_T) {
				MenuToggleFlag &= 0;
				MenuToggleFlag |= JOY_CTRL_T;
				BattleResultWndFlag = FALSE;
				play_se(202, 320, 240);
			}
			else {
				play_se(203, 320, 240);
			}
		}
	}
#ifdef __AI
	if (!AI_State && ((joy_trg[0] & JOY_CTRL_P && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[2]))) {
#else
	if ((joy_trg[0] & JOY_CTRL_P && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[2])) {
#endif
#ifdef _NB_新下任务栏
		TaskBarFlag = FALSE;
#endif
		if (ProcNo == PROC_BATTLE) {
			play_se(220, 320, 240);
		}
		else {
			{
				MenuToggleFlag ^= JOY_CTRL_P;	// CTRL + P
				for (i = 0; i < MENU_PET_0; i++) petWndFontNo[i] = -2;
				petWndNo = 0;
				DeathMenuAction();
				//DeathMenuAction2();
				saveUserSetting();
				if (MenuToggleFlag & JOY_CTRL_P) {
					MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;
					MenuToggleFlag |= JOY_CTRL_P;
					MenuToggleFlag &= ~JOY_CTRL_I;
					MenuToggleFlag &= ~JOY_CTRL_M;
					BattleResultWndFlag = FALSE;
					play_se(202, 320, 240);
				}
				else {
					play_se(203, 320, 240);
				}
			}
		}
	}
#ifdef __AI
	if (!AI_State && ((joy_trg[0] & JOY_CTRL_I && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[4]))) {
#else
	if ((joy_trg[0] & JOY_CTRL_I && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[4])) {
#endif
#ifdef _NB_新下任务栏
		TaskBarFlag = FALSE;
#endif
		if (ProcNo == PROC_BATTLE) {
			play_se(220, 320, 240);
		}
		else {
#ifdef _STREET_VENDOR
			if (pc.iOnStreetVendor == 1 || sStreetVendorBuyBtn == 2) StockChatBufferLine("상점이용시, 아이템사용을할수없어요!", FONT_PAL_RED);
			else
#endif

			{
				MenuToggleFlag ^= JOY_CTRL_I;	// CTRL + I
				for (i = 0; i < MENU_ITEM_0; i++) itemWndFontNo[i] = -2;
				for (i = 0; i < MENU_ITEM_0; i++) itemWndBtnFlag[i] = 0;
				for (i = 0; i < 3; i++) itemWndBtnBag[i] = -2;
				itemWndNo = 0;
				itemWndBagOffset = 0;
				mouse.itemNo = -1;
				itemWndDropGold = 0;
				if (BattleResultWndFlag >= 1) DeathMenuAction();
				DeathMenuAction2();
				saveUserSetting();
				if (MenuToggleFlag & JOY_CTRL_I) {
					MenuToggleFlag &= ~JOY_CTRL_M;
					MenuToggleFlag &= ~JOY_CTRL_T;
					MenuToggleFlag &= ~JOY_B;
					MenuToggleFlag |= JOY_CTRL_I;
					BattleResultWndFlag = FALSE;
					MapWmdFlagBak = FALSE;
					play_se(202, 320, 240);
				}
				else {
					play_se(203, 320, 240);
				}
			}
		}
	}
#ifdef __AI
	if (!AI_State && ((joy_trg[0] & JOY_CTRL_M && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[0])
		|| (MapWmdFlagBak == TRUE && BattleResultWndFlag == FALSE && EncountFlag == FALSE && ProcNo == PROC_GAME && SubProcNo == 3)
		)) {
#else
	if ((joy_trg[0] & JOY_CTRL_M && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[0])
		|| (MapWmdFlagBak == TRUE && BattleResultWndFlag == FALSE && EncountFlag == FALSE && ProcNo == PROC_GAME && SubProcNo == 3)
		) {
#endif
#ifdef _NB_新下任务栏
		TaskBarFlag = FALSE;
#endif
		if (ProcNo == PROC_BATTLE) {
			play_se(220, 320, 240);
		}
		else {

			MenuToggleFlag ^= JOY_CTRL_M;
			for (i = 0; i < MENU_PET_0; i++) petWndFontNo[i] = -2;
			mapWndNo = 0;
			if (BattleResultWndFlag >= 1) DeathMenuAction();
			DeathMenuAction2();
			if (MenuToggleFlag & JOY_CTRL_M) {
				MenuToggleFlag &= ~JOY_CTRL_I;
				MenuToggleFlag &= ~JOY_CTRL_T;
				MenuToggleFlag &= ~JOY_B;
				MenuToggleFlag |= JOY_CTRL_M;
				BattleResultWndFlag = FALSE;
				MapWmdFlagBak = FALSE;
				play_se(202, 320, 240);
			}
			else {
				play_se(203, 320, 240);
			}
		}
	}
#ifdef __AI
	if (!AI_State && ((joy_trg[0] & JOY_CTRL_E && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[5]))) {
#else
	if ((joy_trg[0] & JOY_CTRL_E && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[5])) {
#endif
#ifdef _NB_新下任务栏
		TaskBarFlag = FALSE;
#endif
#ifdef _BATTLESKILL
		extern int wonflag;
		if (wonflag == 1)
			play_se(220, 320, 240);
		else {
#endif
			MenuToggleFlag ^= JOY_CTRL_E;
			for (i = 0; i < MENU_MAIL_0; i++) mailWndFontNo[i] = -2;
			mailWndNo = MAIL_WND_VIEW;
			mailViewWndPageNo = 0;
			mailItemNo = -1;
			InitMailSendFlag();
			DeathMenuAction();
			saveUserSetting();
			if (MenuToggleFlag & JOY_CTRL_E) {
				MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;
				MenuToggleFlag |= JOY_CTRL_E;
				BattleResultWndFlag = FALSE;
				play_se(202, 320, 240);
			}
			else {
				play_se(203, 320, 240);
			}
#ifdef _BATTLESKILL
		}
#endif
		//}
	}
#ifdef __AI
	if (!AI_State && ((joy_trg[0] & JOY_CTRL_A && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[6]))) {
#else
	if ((joy_trg[0] & JOY_CTRL_A && GetImeString() == NULL)
		|| (TaskBarFlag == TRUE && mouse.onceState & MOUSE_LEFT_CRICK && HitDispNo == taskBarFontNo[6])) {
#endif
#ifdef _NB_新下任务栏
		TaskBarFlag = FALSE;
#endif
#ifdef _BATTLESKILL
		extern int wonflag;
		if (wonflag == 1)
			play_se(220, 320, 240);
		else {
#endif
			MenuToggleFlag ^= JOY_CTRL_A;
			for (i = 0; i < MENU_ALBUM_0; i++) albumWndFontNo[i] = -2;
			mapWndNo = 0;
			DeathMenuAction();
			saveUserSetting();
			if (MenuToggleFlag & JOY_CTRL_A) {
				MenuToggleFlag &= JOY_CTRL_I | JOY_CTRL_M;
				MenuToggleFlag |= JOY_CTRL_A;
				PetRanking::Instance().Init();
				PetRanking::Instance().SetCheck(1);
				BattleResultWndFlag = FALSE;
				play_se(202, 320, 240);
			}
			else {
				PetRanking::Instance().SetCheck(0);
				play_se(203, 320, 240);
			}
			//}
#ifdef _BATTLESKILL
		}
#endif
	}



#ifdef _NB_新下任务栏
	int 下任务栏X, 下任务栏Y;		//[퀘스트 수령창Y]
	if (ResoMode == 分辨率::mode800) {
		下任务栏X = 800 - 24;
		下任务栏Y = 600 - 48;
	}
	else if (ResoMode == 分辨率::mode1024) {
		下任务栏X = 1024 - 18;
		下任务栏Y = 768 - 42;
	}
	else {
		下任务栏X = 640 - 18;
		下任务栏Y = 480 - 42;
	}

	int 下任务栏按钮 = -1;
	if (ProcNo != PROC_BATTLE) {//战斗状态下不画加号
		if (TaskBarFlag == TRUE) {
			下任务栏按钮 = StockDispBuffer(下任务栏X, 下任务栏Y, DISP_PRIO_MENU, IMAGE_收纳按钮合, 2);
		}
		else {
			下任务栏按钮 = StockDispBuffer(下任务栏X, 下任务栏Y, DISP_PRIO_MENU, IMAGE_收纳按钮分, 2);
		}
		if (下任务栏按钮) {
			if (HitDispNo == 下任务栏按钮) {
				ShowBottomLineString(11, "기능퀘스트창。");
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					TaskBarFlag = !TaskBarFlag;
				}
			}
		}
	}
	StockDispBuffer(512, TASK_BAR_Y, DISP_PRIO_MENU,
		, 1);
#else
	//最下面一行的背景图片
	/*
	if (ResoMode == 分辨率::mode800){
	StockDispBuffer(TASK_BAR_X, TASK_BAR_Y, DISP_PRIO_MENU, 任务条800, 1);
	}else if (ResoMode == 分辨率::mode1024){
	StockDispBuffer(TASK_BAR_X + 192, TASK_BAR_Y, DISP_PRIO_MENU, 任务条1024, 1);
	}else{
	StockDispBuffer(TASK_BAR_X, TASK_BAR_Y, DISP_PRIO_MENU, CG_TASK_BAR_BACK, 1);
	}
	*/
	StockDispBuffer(400, TASK_BAR_Y, DISP_PRIO_MENU, 66718, 1);
	//判断滑鼠 Y座标出现功能列
	if (mouse.nowPoint.y >= 456 + DISPLACEMENT_Y) {
		//JL fix 调整窗口大小后,任务栏渐进修正改为直接一步到位修正
		if (taskBarY > TASK_BAR_Y) taskBarY = TASK_BAR_Y;
		//	if( taskBarY > TASK_BAR_Y ) taskBarY--;
		TaskBarFlag = TRUE;

	}
	else {
		if (TaskBarFlag == TRUE) {
			//JL fix 调整窗口大小后,任务栏渐进修正改为直接一步到位修正
			if (taskBarY < TASK_BAR_Y + 24) {
				taskBarY = TASK_BAR_Y + 24;
				for (i = 0; i < TASK_BAR; i++) albumWndFontNo[i] = -2;
				TaskBarFlag = FALSE;
			}
		}
	}
#endif

	if (TaskBarFlag == TRUE) {
#ifdef _NB_新下任务栏
		taskBarFontNo[0] = StockDispBuffer(下任务栏X, 下任务栏Y - 160, DISP_PRIO_IME3, IMAGE_按钮地图, 2);//地图
		taskBarFontNo[1] = StockDispBuffer(下任务栏X - 120, 下任务栏Y, DISP_PRIO_IME3, IMAGE_按钮状况, 2);//状况
		taskBarFontNo[2] = StockDispBuffer(下任务栏X - 80, 下任务栏Y, DISP_PRIO_IME3, IMAGE_按钮宠物, 2);//宠物
		taskBarFontNo[4] = StockDispBuffer(下任务栏X - 40, 下任务栏Y, DISP_PRIO_IME3, IMAGE_按钮道具, 2);//道具
		taskBarFontNo[5] = StockDispBuffer(下任务栏X, 下任务栏Y - 40, DISP_PRIO_IME3, IMAGE_按钮邮件, 2);//邮件
		taskBarFontNo[6] = StockDispBuffer(下任务栏X, 下任务栏Y - 80, DISP_PRIO_IME3, IMAGE_按钮相簿, 2);//相簿
		taskBarFontNo[3] = StockDispBuffer(下任务栏X, 下任务栏Y - 120, DISP_PRIO_IME3, IMAGE_按钮系统, 2);//系统
#else
		//JL800600 fix任务栏
		x = taskBarX, y = TASK_BAR_Y; //y = taskBarY;
		if (ResoMode == 分辨率::mode800) {
			taskBarFontNo[0] = StockDispBuffer绝对位置(112 * 0, y - 12, DISP_PRIO_IME3, 任务条MAP800 + ((MenuToggleFlag & JOY_CTRL_M) ? 1 : 0), 2);
			taskBarFontNo[1] = StockDispBuffer绝对位置(112 * 1 + 2 * 1, y - 12, DISP_PRIO_IME3, 任务条状况800 + ((MenuToggleFlag & JOY_CTRL_S) ? 1 : 0), 2);
			taskBarFontNo[2] = StockDispBuffer绝对位置(112 * 2 + 2 * 2, y - 12, DISP_PRIO_IME3, 任务条宠物800 + ((MenuToggleFlag & JOY_CTRL_P) ? 1 : 0), 2);
			taskBarFontNo[4] = StockDispBuffer绝对位置(112 * 3 + 2 * 3, y - 12, DISP_PRIO_IME3, 任务条道具800 + ((MenuToggleFlag & JOY_CTRL_I) ? 1 : 0), 2);
			taskBarFontNo[5] = StockDispBuffer绝对位置(112 * 4 + 2 * 4, y - 12, DISP_PRIO_IME3, 任务条邮件800 + ((MenuToggleFlag & JOY_CTRL_E) ? 1 : 0), 2);
			taskBarFontNo[6] = StockDispBuffer绝对位置(112 * 5 + 2 * 5, y - 12, DISP_PRIO_IME3, 任务条相册800 + ((MenuToggleFlag & JOY_CTRL_A) ? 1 : 0), 2);
			taskBarFontNo[3] = StockDispBuffer绝对位置(112 * 6 + 2 * 6, y - 12, DISP_PRIO_IME3, 任务条系统800 + ((MenuToggleFlag & JOY_ESC) ? 1 : 0), 2);
		}
		else if (ResoMode == 分辨率::mode1024) {
			taskBarFontNo[0] = StockDispBuffer绝对位置(144 * 0, y - 12, DISP_PRIO_IME3, 任务条MAP1024 + ((MenuToggleFlag & JOY_CTRL_M) ? 1 : 0), 2);
			taskBarFontNo[1] = StockDispBuffer绝对位置(144 * 1 + 2 * 1, y - 12, DISP_PRIO_IME3, 任务条状况1024 + ((MenuToggleFlag & JOY_CTRL_S) ? 1 : 0), 2);
			taskBarFontNo[2] = StockDispBuffer绝对位置(144 * 2 + 2 * 2, y - 12, DISP_PRIO_IME3, 任务条宠物1024 + ((MenuToggleFlag & JOY_CTRL_P) ? 1 : 0), 2);
			taskBarFontNo[4] = StockDispBuffer绝对位置(144 * 3 + 2 * 3, y - 12, DISP_PRIO_IME3, 任务条道具1024 + ((MenuToggleFlag & JOY_CTRL_I) ? 1 : 0), 2);
			taskBarFontNo[5] = StockDispBuffer绝对位置(144 * 4 + 2 * 4, y - 12, DISP_PRIO_IME3, 任务条邮件1024 + ((MenuToggleFlag & JOY_CTRL_E) ? 1 : 0), 2);
			taskBarFontNo[6] = StockDispBuffer绝对位置(144 * 5 + 2 * 5, y - 12, DISP_PRIO_IME3, 任务条相册1024 + ((MenuToggleFlag & JOY_CTRL_A) ? 1 : 0), 2);
			taskBarFontNo[3] = StockDispBuffer绝对位置(144 * 6 + 2 * 6, y - 12, DISP_PRIO_IME3, 任务条系统1024 + ((MenuToggleFlag & JOY_ESC) ? 1 : 0), 2);
		}
		else {
			taskBarFontNo[0] = StockDispBuffer(x, y, DISP_PRIO_IME3, CG_TASK_BAR_MAP_UP + ((MenuToggleFlag & JOY_CTRL_M) ? 1 : 0), 2);
			taskBarFontNo[1] = StockDispBuffer(x, y, DISP_PRIO_IME3, CG_TASK_BAR_STATUS_UP + ((MenuToggleFlag & JOY_CTRL_S) ? 1 : 0), 2);
			taskBarFontNo[2] = StockDispBuffer(x, y, DISP_PRIO_IME3, CG_TASK_BAR_PET_UP + ((MenuToggleFlag & JOY_CTRL_P) ? 1 : 0), 2);
			taskBarFontNo[4] = StockDispBuffer(x, y, DISP_PRIO_IME3, CG_TASK_BAR_ITEM_UP + ((MenuToggleFlag & JOY_CTRL_I) ? 1 : 0), 2);
			taskBarFontNo[5] = StockDispBuffer(x, y, DISP_PRIO_IME3, CG_TASK_BAR_MAIL_UP + ((MenuToggleFlag & JOY_CTRL_E) ? 1 : 0), 2);
			taskBarFontNo[6] = StockDispBuffer(x, y, DISP_PRIO_IME3, CG_TASK_BAR_ALBUM_UP + ((MenuToggleFlag & JOY_CTRL_A) ? 1 : 0), 2);
			taskBarFontNo[3] = StockDispBuffer(x, y, DISP_PRIO_IME3, CG_TASK_BAR_SYSTEM_UP + ((MenuToggleFlag & JOY_ESC) ? 1 : 0), 2);
		}
#endif

	}

	if (MenuToggleFlag & JOY_ESC) {
		int w = 7;
		int h = 6;
		switch (systemWndNo) {
		case 0:
			if (pActMenuWnd == NULL) {
				int 临时宽X, 临时高Y;
				if (ResoMode == 分辨率::mode800) {
					临时宽X = (800 - w * 86) / 2;
					临时高Y = (600 - h * 64) / 2;
				}
				else if (ResoMode == 分辨率::mode1024) {
					临时宽X = (1024 - w * 120) / 2;
					临时高Y = (768 - h * 84) / 2;
				}
				else {
					临时宽X = (640 - w * 64) / 2;
					临时高Y = (480 - h * 48) / 2;
				}
				pActMenuWnd = MakeWindowDisp(临时宽X, 临时高Y, w, h, CG_WND_TITLE_SYSTEM, 1);
				for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
				break;
			}
			else {
				if (pActMenuWnd->hp <= 0) break;
			}
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (HitFontNo == systemWndFontNo[0]) {
#ifdef __PHONEMESSAGE
					CleanSMS();
#endif
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 1;
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[1]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 2;
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[4]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 4;
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[3]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 3;
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[5]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 6;
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
				//原地登出
				if (HitFontNo == systemWndFontNo[6]) {
#ifdef __PHONEMESSAGE
					CleanSMS();
#endif
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 7;
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[2]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					MenuToggleFlag ^= JOY_ESC;
					play_se(203, 320, 240);
					HitFontNo = -1;
					break;
				}
#ifdef __AI
				//自动战斗设定
				if (HitFontNo == systemWndFontNo[7]) {
					extern int AI_State;
					AI_State = 1;
					extern short actBtn;
					actBtn = 1;
					closeEtcSwitch();
					closeJoinChannelWN();
					MenuToggleFlag = 0;
					DeathMenuAction2();
					DeathMenuAction();
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
#endif
#ifdef _NB_CANCEL_FANTI
				if (HitFontNo == systemWndFontNo[8]) {//简繁切换
					if (繁体开关) {
						繁体开关 = FALSE;
						StockChatBufferLine("단순화버튼！", 20);
					}
					else {
						繁体开关 = TRUE;
						StockChatBufferLine("기존버튼！", 10);
					}
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
#endif
				if (HitFontNo == systemWndFontNo[9]) {//支票签发
					lssproto_SaMenu_send(sockfd, 1, 6, 0);
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[10]) {//贴心传送
					lssproto_SaMenu_send(sockfd, 1, 8, 0);
					play_se(202, 320, 240);
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					MenuToggleFlag ^= JOY_ESC;
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[11]) {//召唤陪练
					lssproto_SaMenu_send(sockfd, 1, 11, 0);
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[12]) {//个人信息
					lssproto_SaMenu_send(sockfd, 1, 9, 0);
					play_se(202, 320, 240);
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					MenuToggleFlag ^= JOY_ESC;
					HitFontNo = -1;
					break;
				}
#ifdef _NB_CANCEL_BGM
				static BOOL stopBGMClicked = FALSE;
				if (HitFontNo == systemWndFontNo[13]) {
					if (声音开关) {
						StockChatBufferLine("BGM이 켜졌어요！", 20);
						声音开关 = FALSE;
						if (stopBGMClicked) {
							play_bgm(map_bgm_no);
						}
					}
					else {
						stopBGMClicked = TRUE;
						StockChatBufferLine("BGM이 꺼졌어요！", 10);
						play_se(202, 320, 240);
						stop_bgm();
						声音开关 = TRUE;
					}
					HitFontNo = -1;
					saveUserSetting();
					break;
				}
#endif

#ifdef _NB_SupetWalk
				if (HitFontNo == systemWndFontNo[14]) {
					extern int fastWalkFlag;
					if (fastWalkFlag == 0x75) {
						fastWalkFlag = 0x74;
						StockChatBufferLine("빠른걷기켜기！", 20);
					}
					else {
						fastWalkFlag = 0x75;
						StockChatBufferLine("빠른걷기끄기！", 10);
					}
					play_se(202, 320, 240);
					HitFontNo = -1;
					break;
				}
#endif
#ifdef	_NB_EXPSHOW
				if (HitFontNo == systemWndFontNo[15]) {
					play_se(202, 320, 240);
					状态显示开关 = !状态显示开关;
					if (状态显示开关) {
						StockChatBufferLine("상태표시열기！", 20);
					}
					else {
						StockChatBufferLine("상태표시끄기！", 10);
					}
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					MenuToggleFlag ^= JOY_ESC;
					HitFontNo = -1;
					break;
				}
#endif
#ifdef	_NB_CALLGCGC
				if (HitFontNo == systemWndFontNo[16]) {
					play_se(202, 320, 240);
					原地遇敌 = !原地遇敌;
					if (原地遇敌) {
						StockChatBufferLine("자동전투 모드가 활성화 되었습니다", 4);
						lssproto_SaMenu_send(sockfd, 1, 4, 1);
					}
					else {
						StockChatBufferLine("자동전투 모드가 종료 되었습니다.", 4);
						lssproto_SaMenu_send(sockfd, 1, 4, 0);
					}
					HitFontNo = -1;
					break;
				}
#endif
#ifdef _NB_DIY_SEND
				if (HitFontNo == systemWndFontNo[17]) {
					play_se(202, 320, 240);
					lssproto_SaMenu_send(sockfd, 1, 1, 0);
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[18]) {
					play_se(202, 320, 240);
					lssproto_SaMenu_send(sockfd, 1, 2, 0);
					HitFontNo = -1;
					break;
				}
#endif
#ifdef _NB_断线自动重连
				if (HitFontNo == systemWndFontNo[19]) {//掉线重连
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					play_se(202, 320, 240);
					closeEtcSwitch();
					closeJoinChannelWN();
					MenuToggleFlag = 0;
					DeathMenuAction2();
					DeathMenuAction();
					自动登陆窗口 = TRUE;
					extern short actBtn;
					actBtn = 1;
					break;
				}
#endif
#ifdef _CHAR_INFOSTORAGE
				if (HitFontNo == systemWndFontNo[20]) {
					play_se(202, 320, 240);
					lssproto_petshopcall_send(sockfd);
					HitFontNo = -1;
					break;
				}
				if (HitFontNo == systemWndFontNo[21]) {
					play_se(202, 320, 240);
					lssproto_poolitemcall_send(sockfd);
					HitFontNo = -1;
					break;
				}
#endif
			}
			x = pActMenuWnd->x + 28;
			y = pActMenuWnd->y + 54;
			systemWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    마을가기    ", 2);	y += 34;//y += 40;
			systemWndFontNo[6] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    로그아웃    ", 2);	y += 34;//y += 40;
			systemWndFontNo[1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    채팅설정    ", 2);	y += 34;//y += 40;
			systemWndFontNo[4] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    배경음악    ", 2);	y += 34;//y += 40;
			systemWndFontNo[3] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    효과설정    ", 2);	y += 34;//y += 40;
																										//	systemWndFontNo[ 5 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 0, 	"    커서설정    ", 2 );	y += 34;//y += 52;

#ifdef _NB_断线自动重连
																										//systemWndFontNo[ 19] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 5, 	"    掉线重连    ", 2);
#endif
																										//第二排开始
			x = pActMenuWnd->x + 160;
			y = pActMenuWnd->y + 54;
			//systemWndFontNo[5] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    커서설정    ", 2);	y += 34;//y += 52;
			systemWndFontNo[7] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 4, "    ＡＩ설정    ", 2);	y += 34;//y += 52;
#ifdef _NB_CANCEL_FANTI
			if (繁体开关) {
				//systemWndFontNo[8 ] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 6, "    切换简体    ", 2);	y += 34;//y += 40; 4
			}
			else {
				//systemWndFontNo[8 ] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 1, "    切换繁体    ", 2);	y += 34;//y += 40; 5
			}
#else
			//systemWndFontNo[8] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 7, "    위치이동    ", 0);	y += 34;//y += 40; 5
#endif
																										//systemWndFontNo[9 ] = StockFontBuffer( x, y, FONT_PRIO_FRONT, 4 ,  "    수표발행    ", 2 );	y += 34;//y += 52;
			//systemWndFontNo[10] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 4, "    위치이동    ", 2);	y += 34;//y += 52;
			//systemWndFontNo[11] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    트레이너    ", 2);	y += 34;//y += 52;
			//systemWndFontNo[12] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    개인정보    ", 2);	y += 34;//y += 52;
			systemWndFontNo[21] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 4, "    템창고  ", 2);	y += 34;//y += 52;
			systemWndFontNo[20] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 4, "    펫창고  ", 2);	y += 34;//y += 52;
																										//第三排开始
			x = pActMenuWnd->x + 300;
			y = pActMenuWnd->y + 54;
#ifdef _NB_CANCEL_BGM
			if (声音开关) {
				systemWndFontNo[13] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 5, "  음악켜기  ", 2);	y += 34;//y += 40; 2
			}
			else {
				systemWndFontNo[13] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 5, "  음악끄기  ", 2);	y += 34;//y += 40; 2
			}
#else
			systemWndFontNo[13] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 1, "    暂未开放    ", 0);	y += 34;//y += 40; 5
#endif
#ifdef	_NB_SupetWalk
			extern int fastWalkFlag;
			if (fastWalkFlag == 0x74) {
			//	systemWndFontNo[14] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 1, "  빠른걷기  ", 2);	y += 34;//y += 40; 3
			}
			else {
		//		systemWndFontNo[14] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 1, "  느린걷기  ", 2);	y += 34;//y += 40; 3
			}
#else
			systemWndFontNo[14] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 7, "    暂未开放    ", 0);	y += 34;//y += 40; 5
#endif
#ifdef	_NB_EXPSHOW
			if (状态显示开关) {
			//	systemWndFontNo[15] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "  상태끄기  ", 2);	y += 34;//y += 40;7
			}
			else {
			//	systemWndFontNo[15] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "  상태켜기  ", 2);	y += 34;//y += 40;7
			}
#else
			systemWndFontNo[15] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 7, "    暂未开放    ", 0);	y += 34;//y += 40; 5
#endif
#ifdef	_NB_CALLGCGC
			if (原地遇敌) {
				systemWndFontNo[16] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 5, "  자동끄기  ", 2); y += 34;//y += 40; 8
			}
			else {
				systemWndFontNo[16] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 5, "  자동켜기  ", 2); y += 34;//y += 40; 8
			}
#else
			systemWndFontNo[16] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 7, "    暂未开放    ", 0);	y += 34;//y += 40; 5
#endif
#ifdef _NB_DIY_SEND
			//systemWndFontNo[17] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 5, "  홈페이지  ", 2);	y += 34;//y += 40; 5
			//systemWndFontNo[18] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 5, "  디스코드  ", 2);	y += 34;//y += 52;
#else
			systemWndFontNo[17] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 7, "    暂未开放    ", 0);	y += 34;//y += 40; 5
			systemWndFontNo[18] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 7, "    暂未开放    ", 0);	y += 34;//y += 52;
#endif
			

			x = pActMenuWnd->x + 160;
			y = pActMenuWnd->y + 258;
			systemWndFontNo[2] = StockFontBuffer(x + 10, y - 10, FONT_PRIO_FRONT, 1, "　닫　　기　", 2);
			break;
		case 1://回记录点
			if (pActMenuWnd == NULL) {
				int 临时宽X, 临时高Y;
				w = 3;
				h = 3;
				if (ResoMode == 分辨率::mode800) {
					临时宽X = (800 - w * 106) / 2;
					临时高Y = (600 - h * 70) / 2;
				}
				else if (ResoMode == 分辨率::mode1024) {
					临时宽X = (1024 - w * 180) / 2;
					临时高Y = (768 - h * 96) / 2;
				}
				else {
					临时宽X = (640 - w * 64) / 2;
					临时高Y = (480 - h * 48) / 2;
				}
				pActMenuWnd = MakeWindowDisp(临时宽X, 临时高Y, w, h, CG_WND_TITLE_LOGOUT, 1);
				for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
				break;
			}
			else {
				if (pActMenuWnd->hp <= 0) break;
			}
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (HitFontNo == systemWndFontNo[0]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
#ifdef _CHAR_NEWLOGOUT
					systemWndNo = 0;
					lssproto_CharLogout_send(sockfd, 1);
#else
					GameState = GAME_LOGIN;
					ChangeProc2(PROC_CHAR_LOGOUT);
#endif
					play_se(206, 320, 240);
					break;
				}
				if (HitFontNo == systemWndFontNo[1]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 0;
					play_se(203, 320, 240);
					break;
				}
			}
			x = pActMenuWnd->x + 28;
			y = pActMenuWnd->y + 56;
			systemWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "　　 확     인　　", 2);	y += 40;
			systemWndFontNo[1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "　　 취     소　　", 2);	y += 40;
			break;
		case 2://聊天设定
			if (pActMenuWnd == NULL) {
				int 临时宽X, 临时高Y;
				w = 4;
				h = 8;
				if (ResoMode == 分辨率::mode800) {
					临时宽X = (800 - w * 100) / 2;
					临时高Y = (600 - h * 60) / 2;
				}
				else if (ResoMode == 分辨率::mode1024) {
					临时宽X = (1024 - w * 140) / 2;
					临时高Y = (768 - h * 77) / 2;
				}
				else {
					临时宽X = (640 - w * 64) / 2;
					临时高Y = (480 - h * 48) / 2;
				}
				pActMenuWnd = MakeWindowDisp(临时宽X, 临时高Y, w, h, CG_WND_TITLE_CHAT, 1);
				for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
				break;
			}
			else {
				if (pActMenuWnd->hp <= 0) break;
			}
			if (mouse.autoState & MOUSE_LEFT_CRICK) {
				if (HitFontNo == systemWndFontNo[0]) {
					NowMaxChatLine++;
					if (NowMaxChatLine > MAX_CHAT_LINE) {
						NowMaxChatLine = MAX_CHAT_LINE;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
					}
				}
				if (HitFontNo == systemWndFontNo[1]) {
					NowMaxChatLine--;
					if (NowMaxChatLine <= -1) {
						NowMaxChatLine = 0;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
					}
				}
				if (HitFontNo == systemWndFontNo[2]) {
					MyChatBuffer.color++;
					if (MyChatBuffer.color >= FONT_PAL_NUM) MyChatBuffer.color = 0;
					play_se(217, 320, 240);
				}
				if (HitFontNo == systemWndFontNo[4]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 5;
					play_se(202, 320, 240);
					break;
				}
				if (HitFontNo == systemWndFontNo[5]) {
					NowMaxVoice++;
					if (NowMaxVoice > MAX_VOICE) {
						NowMaxVoice = MAX_VOICE;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
					}
				}
				if (HitFontNo == systemWndFontNo[6]) {
					NowMaxVoice--;
					if (NowMaxVoice <= 0) {
						NowMaxVoice = 1;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
					}
				}
#ifdef _TALK_WINDOW
				if (HitFontNo == systemWndFontNo[7]) {
					g_bTalkWindow = !g_bTalkWindow;
					if (!WindowMode && g_bTalkWindow) {
						StockChatBufferLine("이기능은 전체화면모드에서 불가능합니다", FONT_PAL_RED);
						g_bTalkWindow = FALSE;
					}
					play_se(217, 320, 240);
					if (g_bTalkWindow) TalkWindow.Create();
					else TalkWindow.Visible(FALSE);
				}
#endif
				if (HitFontNo == systemWndFontNo[3]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 0;
					play_se(203, 320, 240);
					saveUserSetting();
					break;
				}
			}
			x = pActMenuWnd->x + 48 - 5;
			y = pActMenuWnd->y + 56;
			sprintf(moji, "     ◆ 채팅 표시 %3d 줄 ◆", NowMaxChatLine);
			StockFontBuffer(x - 16, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0);				y += 32;
			systemWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "        증    가      ", 2);	y += 32;
			systemWndFontNo[1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "        감    소      ", 2);	y += 32;

			sprintf(moji, "     색상 변경(%-2d)  ", MyChatBuffer.color);
			systemWndFontNo[2] = StockFontBuffer(x, y, FONT_PRIO_FRONT, MyChatBuffer.color, moji, 2);	y += 32;
			systemWndFontNo[4] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "        문자 설정      ", 2);	y += 44;

			sprintf(moji, "     ◆ 채 팅 범 위 %3d ◆", NowMaxVoice);
			StockFontBuffer(x - 16, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0);				y += 32;
			systemWndFontNo[5] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "       증    가       ", 2);	y += 32;
			systemWndFontNo[6] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "       감    소       ", 2);	y += 44;

#ifdef _TALK_WINDOW
			StockFontBuffer(x - 16, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "◆채팅창설정◆", 0); y += 32;
			systemWndFontNo[7] = StockFontBuffer(x + 20, y, FONT_PRIO_FRONT, 0, g_bTalkWindow ? "채팅창닫기" : "채팅창열기", 2); y += 32;
#endif																																						

			systemWndFontNo[3] = StockFontBuffer(x, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, "      돌 아 가 기      ", 2);	y += 40;
			break;
		case 3://音效设定
			if (pActMenuWnd == NULL) {
				int 临时宽X, 临时高Y;
				w = 4;
				h = 6;
				if (ResoMode == 分辨率::mode800) {
					临时宽X = (800 - w * 100) / 2;
					临时高Y = (600 - h * 60) / 2;
				}
				else if (ResoMode == 分辨率::mode1024) {
					临时宽X = (1024 - w * 140) / 2;
					临时高Y = (768 - h * 77) / 2;
				}
				else {
					临时宽X = (640 - w * 64) / 2;
					临时高Y = (480 - h * 48) / 2;
				}
				pActMenuWnd = MakeWindowDisp(临时宽X, 临时高Y, w, h, CG_WND_TITLE_SE, 1);
				for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
				break;
			}
			else {
				if (pActMenuWnd->hp <= 0) break;
			}
			if (mouse.autoState & MOUSE_LEFT_CRICK) {
				if (HitFontNo == systemWndFontNo[0]) {
					t_music_se_volume++;
					if (t_music_se_volume > 15) {
						t_music_se_volume = 15;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
					}
				}
				if (HitFontNo == systemWndFontNo[1]) {
					t_music_se_volume--;
					if (t_music_se_volume < 0) {
						t_music_se_volume = 0;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
					}
				}
				if (HitFontNo == systemWndFontNo[2]) {
					stereo_flg = !stereo_flg;
					play_se(217, 320, 240);
				}
			}
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (HitFontNo == systemWndFontNo[3]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 0;
					play_se(203, 320, 240);
					saveUserSetting();
					break;
				}
			}
			x = pActMenuWnd->x + 48 - 5;
			y = pActMenuWnd->y + 64;
			sprintf(moji, "  ◆  현 재 음 량%3d  ◆  ", t_music_se_volume);
			StockFontBuffer(x - 8, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0);				y += 40;
			systemWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "       증     가       ", 2);	y += 40;
			systemWndFontNo[1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "       감     소       ", 2);	y += 40;
			systemWndFontNo[2] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, monoStereoStr[stereo_flg], 2);	y += 52;
			systemWndFontNo[3] = StockFontBuffer(x, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, "      돌 아 가 기       ", 2);	y += 40;
			break;
		case 4://背景音乐
			if (pActMenuWnd == NULL) {
				int 临时宽X, 临时高Y;
				w = 4;
				h = 8;
				if (ResoMode == 分辨率::mode800) {
					临时宽X = (800 - w * 100) / 2;
					临时高Y = (600 - h * 60) / 2;
				}
				else if (ResoMode == 分辨率::mode1024) {
					临时宽X = (1024 - w * 140) / 2;
					临时高Y = (768 - h * 77) / 2;
				}
				else {
					临时宽X = (640 - w * 64) / 2;
					临时高Y = (480 - h * 48) / 2;
				}
				pActMenuWnd = MakeWindowDisp(临时宽X, 临时高Y, w, h, CG_WND_TITLE_BGM, 1);
				for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
				break;
			}
			else {
				if (pActMenuWnd->hp <= 0) break;
			}
			if (mouse.autoState & MOUSE_LEFT_CRICK) {
				if (HitFontNo == systemWndFontNo[0]) {
					t_music_bgm_volume++;
					if (t_music_bgm_volume > 15) {
						t_music_bgm_volume = 15;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
						bgm_volume_change();
					}
				}
				if (HitFontNo == systemWndFontNo[1]) {
					t_music_bgm_volume--;
					if (t_music_bgm_volume < 0) {
						t_music_bgm_volume = 0;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
						bgm_volume_change();
					}
				}
				if (HitFontNo == systemWndFontNo[3]) {
					t_music_bgm_pitch[t_music_bgm_no]++;
					set_gbm_pitch();
					if (t_music_bgm_pitch[t_music_bgm_no] > 8) {
						t_music_bgm_pitch[t_music_bgm_no] = 8;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
						set_gbm_pitch();
					}
				}
				if (HitFontNo == systemWndFontNo[4]) {
					t_music_bgm_pitch[t_music_bgm_no]--;
					if (t_music_bgm_pitch[t_music_bgm_no] < -8) {
						t_music_bgm_pitch[t_music_bgm_no] = -8;
						play_se(220, 320, 240);
					}
					else {
						play_se(217, 320, 240);
						set_gbm_pitch();
					}
				}
			}
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (HitFontNo == systemWndFontNo[2]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 0;
					play_se(203, 320, 240);
					saveUserSetting();
					break;
				}
			}
			x = pActMenuWnd->x + 48 - 5;
			y = pActMenuWnd->y + 64;
			sprintf(moji, "   ◆  배경음악 %3d  ◆", t_music_bgm_volume);
			StockFontBuffer(x - 8, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0);				y += 40;
			systemWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "       증    가       ", 2);	y += 40;
			systemWndFontNo[1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "       감    소       ", 2);	y += 48;
			if (t_music_bgm_pitch[t_music_bgm_no] == 0) {
				sprintf(moji, "◆   ◆  템포조절  0  ◆◆");
			}
			else
				sprintf(moji, "   ◆  템포조절  %+3d  ◆", t_music_bgm_pitch[t_music_bgm_no]);
			StockFontBuffer(x - 8, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0);				y += 40;
			systemWndFontNo[3] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "       증    가      ", 2);	y += 40;
			systemWndFontNo[4] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "       감    소      ", 2);	y += 52;
			systemWndFontNo[2] = StockFontBuffer(x, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, "      돌 아 가 기      ", 2);	y += 40;
			break;
		case 5://记录文字设定
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(4, 4, 272, 430, CG_WND_TITLE_CHAT, -1);
				for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
				GetKeyInputFocus(&chatRegistryStr[0]);
				break;
			}
			else {
				if (pActMenuWnd->hp <= 0) break;
			}
			int 临时宽X, 临时高Y;
			if (ResoMode == 分辨率::mode800) {
				临时宽X = 800 / 2;
				临时高Y = 600 / 2;
			}
			else if (ResoMode == 分辨率::mode1024) {
				临时宽X = 1024 / 2;
				临时高Y = 768 / 2;
			}
			else {
				临时宽X = 640 / 2;
				临时高Y = 480 / 2;
			}
			StockDispBuffer(临时宽X, 临时高Y, DISP_PRIO_MENU, CG_CHAT_REGISTY_WND, 1);
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (HitDispNo == systemWndFontNo[3]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					GetKeyInputFocus(&MyChatBuffer);
					systemWndNo = 0;
					play_se(203, 320, 240);
					saveUserSetting();
					SaveChatRegistyStr();
					break;
				}
			}
			if (ResoMode == 分辨率::mode800) {
				x = 800 / 2 - 122;
				y = 600 / 2 - 156;
			}
			else if (ResoMode == 分辨率::mode1024) {
				x = 1024 / 2 - 122;
				y = 768 / 2 - 156;
			}
			else {
				x = 640 / 2 - 122;
				y = 480 / 2 - 156;
			}
			//x = pActMenuWnd->x + 18;
			//y = pActMenuWnd->y + 58;
			for (i = 0; i < 8; i++) {
				chatRegistryStr[i].x = x + 2;
				chatRegistryStr[i].y = y;
				StockFontBuffer2(&chatRegistryStr[i]); y += 43;
				if (MakeHitBox(chatRegistryStr[i].x - 4, chatRegistryStr[i].y - 3,
					chatRegistryStr[i].x + 234 + 3, chatRegistryStr[i].y + 16 + 3, DISP_PRIO_IME3) == TRUE) {
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						GetKeyInputFocus(&chatRegistryStr[i]);
						play_se(217, 320, 240);
					}
				}
			}
			if (ResoMode == 分辨率::mode800) {
				临时宽X = 800 / 2;
				临时高Y = 600 / 2;
			}
			else if (ResoMode == 分辨率::mode1024) {
				临时宽X = 1024 / 2;
				临时高Y = 768 / 2;
			}
			else {
				临时宽X = 640 / 2;
				临时高Y = 480 / 2;
			}
			systemWndFontNo[3] = StockDispBuffer(临时宽X, 临时高Y + 194, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
			break;
		case 6://滑鼠设定
			if (pActMenuWnd == NULL) {
				int 临时宽X, 临时高Y;
				w = 3;
				h = 4;
				if (ResoMode == 分辨率::mode800) {
					临时宽X = (800 - w * 106) / 2;
					临时高Y = (600 - h * 70) / 2;
				}
				else if (ResoMode == 分辨率::mode1024) {
					临时宽X = (1024 - w * 180) / 2;
					临时高Y = (768 - h * 96) / 2;
				}
				else {
					临时宽X = (640 - w * 64) / 2;
					临时高Y = (480 - h * 48) / 2;
				}
				pActMenuWnd = MakeWindowDisp(临时宽X, 临时高Y, w, h, CG_WND_TITLE_MOUSE, 1);
				for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
				break;
			}
			else {
				if (pActMenuWnd->hp <= 0) break;
			}
			if (HitFontNo == systemWndFontNo[0]) {
				if (MouseCursorFlag == TRUE) {
					strcpy(OneLineInfoStr, "공룡알。");
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
#ifdef _TALK_WINDOW
						g_iCursorCount = ShowCursor(FALSE);
#else
						ShowCursor(FALSE);
#endif
						MouseCursorFlag = FALSE;
						play_se(217, 320, 240);
					}
				}
				else {
					strcpy(OneLineInfoStr, "뼈다귀。");
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
#ifdef _TALK_WINDOW
						g_iCursorCount = ShowCursor(TRUE);
#else
						ShowCursor(TRUE);
#endif
						MouseCursorFlag = TRUE;
						play_se(217, 320, 240);	// ?????
					}
				}
			}
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (HitFontNo == systemWndFontNo[1]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 0;
					play_se(203, 320, 240);
					saveUserSetting();
					break;
				}
			}
			x = pActMenuWnd->x + 28;
			y = pActMenuWnd->y + 56;
			StockFontBuffer(x, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, " ◆ 커 서 설 정 ◆   ", 0);	y += 40;
			systemWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, mouseCursor[MouseCursorFlag], 2);	y += 40;
			systemWndFontNo[1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, FONT_PAL_AQUA, "       종  료       ", 2);	y += 40;
			break;

		case 7://原地登出
			if (pActMenuWnd == NULL) {
				int 临时宽X, 临时高Y;
				w = 3;
				h = 3;
				if (ResoMode == 分辨率::mode800) {
					临时宽X = (800 - w * 106) / 2;
					临时高Y = (600 - h * 70) / 2;
				}
				else if (ResoMode == 分辨率::mode1024) {
					临时宽X = (1024 - w * 180) / 2;
					临时高Y = (768 - h * 96) / 2;
				}
				else {
					临时宽X = (640 - w * 64) / 2;
					临时高Y = (480 - h * 48) / 2;
				}
				pActMenuWnd = MakeWindowDisp(临时宽X, 临时高Y, w, h, CG_WND_TITLE_LOGOUT, 1);
				for (i = 0; i < MENU_SYSTEM_0; i++) systemWndFontNo[i] = -2;
				break;
			}
			else {
				if (pActMenuWnd->hp <= 0) break;
			}
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (HitFontNo == systemWndFontNo[0]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					GameState = GAME_LOGIN;
					ChangeProc2(PROC_CHAR_LOGOUT);
					play_se(206, 320, 240);
					break;
				}
				if (HitFontNo == systemWndFontNo[1]) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					systemWndNo = 0;
					play_se(203, 320, 240);
					break;
				}
			}
			x = pActMenuWnd->x + 28;
			y = pActMenuWnd->y + 56;
			systemWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    확    인     ", 2);	y += 40;
			systemWndFontNo[1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "    취    소     ", 2);	y += 40;
			break;

		}
	}

	//? ???????????? *****************************************************/
#ifdef _FRIENDCHANNEL				//ROG ADD 好友频道
	if (setRoomFlag == TRUE)
	{
		setRoomName();
	}

	if (assentFlag == TRUE)
	{
		AssentWnd();
	}
#endif

#ifdef _NB_人物栏详情	
	int 人物底图编号 = 52040;
#endif
	if (MenuToggleFlag & JOY_CTRL_S) {
		switch (statusWndNo) {

		case 4:
			void Init皮肤窗口();
			Init皮肤窗口();
			break;
		case 5:
			void Init英雄称号窗口();
			Init英雄称号窗口();
			break;
		case 6:
			void Init光环窗口();
			Init光环窗口();
			break;
		case 7:
			if (SkillWndflag == true) {
				SkillWndfunc2();
			}
			else {
				if (pActSkillMenuWnd != NULL) {
					DeathAction(pActSkillMenuWnd);
					pActSkillMenuWnd = NULL;
				}
			}
			break;
		case 0:
#ifdef _NB_人物栏详情
			int 临时图标X, 临时图标Y, 临时起始X, 临时起始Y;
			int 皮肤按钮索引, 称号按钮索引, 特效按钮索引;
			//计算图片中心
			if (ResoMode == 分辨率::mode800) {
				临时图标X = 800 / 2;
				临时图标Y = 600 / 2;
				临时起始X = 临时图标X - 568 / 2;
				临时起始Y = 临时图标Y - 358 / 2;
			}
			else if (ResoMode == 分辨率::mode1024) {
				临时图标X = 1024 / 2;
				临时图标Y = 768 / 2;
				临时起始X = 临时图标X - 568 / 2;
				临时起始Y = 临时图标Y - 358 / 2;
			}
			else {
				临时图标X = 640 / 2;
				临时图标Y = 480 / 2;
				临时起始X = 临时图标X - 568 / 2;
				临时起始Y = 临时图标Y - 358 / 2;
			}
			人物底图编号 = 52040;//低版本无职业底图

			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(临时起始X, 临时起始Y, 临时起始X + 568, 临时起始Y + 358, 0, -1);
				for (i = 0; i < MENU_STATUS_0; i++) statusWndFontNo[i] = -2;
				for (i = 0; i < MENU_STATUS_0; i++) statusWndBtnFlag[i] = 0;
				SkillWndflag = false;
				ChangeMyName = false;
			}
			else {
				if (pActMenuWnd->hp > 0) {
					if (mouse.onceState & MOUSE_LEFT_CRICK) {//鼠标点击事件函数
						if (HitDispNo == statusWndFontNo[14]) {   //职业技能
							statusWndNo = 7;
							MenuToggleFlag &= ~JOY_CTRL_I;
							MenuToggleFlag &= ~JOY_CTRL_M;
							SkillWndflag = true;
							play_se(202, 320, 240);

							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathAction(pActPet);
							pActPet = NULL;
							petWndNo = 0;
							GetKeyInputFocus(&MyChatBuffer);
							MenuToggleFlag ^= JOY_CTRL_S;
						}
					}
					if (HitDispNo == statusWndFontNo[6]) {   	// 改变称号
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面
							SkillWndflag = false;
							MenuToggleFlag &= ~JOY_CTRL_I;
							//MenuToggleFlag &= ~JOY_CTRL_M;
#endif
							if (pActMenuWnd3 == NULL) {
								pActMenuWnd3 = MakeWindowDisp(临时图标X, 临时图标Y, 临时图标X + 272, 临时图标Y + 88, 0, -1);
								play_se(202, 320, 240);
								shougouChange.buffer[0] = NULL;
								shougouChange.cnt = 0;
								shougouChange.cursor = 0;
								shougouChange.len = 12;
								shougouChange.color = 10;
								shougouChange.x = 临时图标X - 40;
								shougouChange.y = 临时图标Y - 30;
								shougouChange.fontPrio = FONT_PRIO_AFRONT;
								statusWndBtnFlag[6] = TRUE;
							}
							else {
								DeathAction(pActMenuWnd3);
								pActMenuWnd3 = NULL;
								GetKeyInputFocus(&MyChatBuffer);
								play_se(203, 320, 240);
							}
							statusWndBtnFlag[6] = TRUE;
						}
						if (mouse.state & MOUSE_LEFT_CRICK && statusWndBtnFlag[6] == TRUE) {
							statusWndBtnFlag[6] = TRUE;
						}
						else {
							statusWndBtnFlag[6] = FALSE;
						}
					}
					else {
						statusWndBtnFlag[6] = FALSE;
					}
					if (StatusUpPoint != 0) {
						for (i = 2; i < 6; i++) {
							if (HitDispNo == statusWndFontNo[i]) {
								if (mouse.onceState & MOUSE_LEFT_CRICK) {
									if (bNewServer) {
										lssproto_SKUP_send(sockfd, i - 2);
									}
									else {
										old_lssproto_SKUP_send(sockfd, i - 2);
									}
									play_se(211, 320, 240);
									statusWndBtnFlag[i] = TRUE;
								}
								if (mouse.state & MOUSE_LEFT_CRICK && statusWndBtnFlag[i] == TRUE) {
									statusWndBtnFlag[i] = TRUE;
								}
								else {
									statusWndBtnFlag[i] = FALSE;
								}
							}
							else {
								statusWndBtnFlag[i] = FALSE;
							}
						}
					}
					if (pActMenuWnd3 != NULL) {
						if (pActMenuWnd3->hp > 0) {
							GetKeyInputFocus(&shougouChange);
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								if (HitDispNo == statusWndFontNo[7]) {
									KeyboardReturn();
								}
								if (HitDispNo == statusWndFontNo[8]) {
									DeathAction(pActMenuWnd3);
									pActMenuWnd3 = NULL;
									GetKeyInputFocus(&MyChatBuffer);
									play_se(203, 320, 240);
								}
							}
							if (pActMenuWnd3 != NULL) {
								StockFontBuffer2(&shougouChange);
								statusWndFontNo[7] = StockDispBuffer(临时图标X - 50, 临时图标Y + 30, DISP_PRIO_DRAG, 52035, 2);
								statusWndFontNo[8] = StockDispBuffer(临时图标X + 50, 临时图标Y + 30, DISP_PRIO_DRAG, 52036, 2);
							}
							StockDispBuffer(临时图标X, 临时图标Y, DISP_PRIO_DRAG, 52065, 0);
						}
					}
					if (pActMenuWnd != NULL) {
						sprintf(moji, "%s", pc.name);//打印玩家名字
						StockFontBuffer(临时起始X + 150, 临时起始Y + 46, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%s", pc.freeName);//打印玩家昵称
						StockFontBuffer(临时起始X + 150, 临时起始Y + 66, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%d转", pc.transmigration);//打印玩家转生

						if (pc.familyName[0] != NULL) {
							StockFontBuffer(临时起始X + 150, 临时起始Y + 86, FONT_PRIO_FRONT, 0, pc.familyName, 0);
						}
						else {
							StockFontBuffer(临时起始X + 150, 临时起始Y + 86, FONT_PRIO_FRONT, 7, "无家族", 0);
						}
						int 临时字体颜色 = 0;
						if (pc.transmigration == 0) {
							临时字体颜色 = 0;
						}
						else if (pc.transmigration == 1) {
							临时字体颜色 = 4;
						}
						else if (pc.transmigration == 2) {
							临时字体颜色 = 5;
						}
						else if (pc.transmigration == 3) {
							临时字体颜色 = 1;
						}
						else if (pc.transmigration == 4) {
							临时字体颜色 = 6;
						}
						else if (pc.transmigration == 5) {
							临时字体颜色 = 2;
						}
						else if (pc.transmigration == 6) {
							临时字体颜色 = 8;
						}
						else if (pc.transmigration == 7) {
							临时字体颜色 = 10;
						}
						else if (pc.transmigration == 8) {
							临时字体颜色 = 10;
						}
						StockFontBuffer(临时起始X + 150, 临时起始Y + 106, FONT_PRIO_FRONT, 临时字体颜色, moji, 0);
						sprintf(moji, "%d", pc.level);//打印人物等级
						StockFontBuffer(临时起始X + 225, 临时起始Y + 106, FONT_PRIO_FRONT, 0, moji, 0);
						//打印经验值
						int 计算百分比 = (int)float(pc.exp) / float(pc.maxExp) * 100;
						char 临时字符[256];
						if (pc.maxExp == -1) {
							sprintf(临时字符, "Max");
						}
						else {
							sprintf(临时字符, "%d/%d", pc.exp, pc.maxExp);
						}
						CenteringStr(临时字符, moji, 18);//居中处理
						StockFontBuffer(临时起始X + 340, 临时起始Y + 35, FONT_PRIO_FRONT, 0, moji, 0);
						if (pc.maxExp == -1) {
							sprintf(临时字符, "Max");
						}
						else {
							sprintf(临时字符, "%d%%", 计算百分比);
						}
						CenteringStr(临时字符, moji, 4);//居中处理
						StockFontBuffer(临时起始X + 493, 临时起始Y + 35, FONT_PRIO_FRONT, 0, moji, 0);
						//打印战斗力
						int 人战斗力 = (pc.maxHp / 4) + pc.atk + pc.def + pc.quick;//战斗力
						sprintf(moji, "%d", 人战斗力);
						StockFontBuffer(临时起始X + 340, 临时起始Y + 56, FONT_PRIO_FRONT, 0, moji, 0);
						//打印DP值
						sprintf(moji, "%d", pc.dp);
						StockFontBuffer(临时起始X + 453, 临时起始Y + 56, FONT_PRIO_FRONT, 0, moji, 0);
						//打印耐久力
						计算百分比 = (int)float(pc.hp) / float(pc.maxHp) * 100;
						sprintf(moji, "%d/%d", pc.hp, pc.maxHp);
						StockFontBuffer(临时起始X + 340, 临时起始Y + 93, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%d%%", 计算百分比);
						StockFontBuffer(临时起始X + 427, 临时起始Y + 93, FONT_PRIO_FRONT, 0, moji, 0);
						//打印气力
						计算百分比 = (int)float(pc.mp) / float(pc.maxMp) * 100;
						sprintf(moji, "%d/%d", pc.mp, pc.maxMp);
						StockFontBuffer(临时起始X + 340, 临时起始Y + 113, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%d%%", 计算百分比);
						StockFontBuffer(临时起始X + 427, 临时起始Y + 113, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%d", pc.atk);//打印攻击力
						StockFontBuffer(临时起始X + 340, 临时起始Y + 134, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%d", pc.def);//打印防御力
						StockFontBuffer(临时起始X + 340, 临时起始Y + 155, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%d", pc.quick);//打印敏捷力
						StockFontBuffer(临时起始X + 340, 临时起始Y + 176, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%d", pc.charm);//打印魅力值
						StockFontBuffer(临时起始X + 340, 临时起始Y + 196, FONT_PRIO_FRONT, 0, moji, 0);

						//打印升级点数
						sprintf(moji, "%d", StatusUpPoint);
						StockFontBuffer(临时起始X + 388, 临时起始Y + 230, FONT_PRIO_FRONT, 1, moji, 0);
						if (StatusUpPoint != 0) {
							//加体按钮
							statusWndFontNo[2] = StockDispBuffer(临时起始X + 373, 临时起始Y + 257, DISP_PRIO_IME3, 52047, 2);
							//加攻按钮
							statusWndFontNo[3] = StockDispBuffer(临时起始X + 477, 临时起始Y + 257, DISP_PRIO_IME3, 52047, 2);
							//加防按钮
							statusWndFontNo[4] = StockDispBuffer(临时起始X + 373, 临时起始Y + 278, DISP_PRIO_IME3, 52047, 2);
							//加敏按钮
							statusWndFontNo[5] = StockDispBuffer(临时起始X + 477, 临时起始Y + 278, DISP_PRIO_IME3, 52047, 2);
						}

						//打印加点状况
						sprintf(moji, "%d", pc.vital);
						StockFontBuffer(临时起始X + 328, 临时起始Y + 248, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%d", pc.str);
						StockFontBuffer(临时起始X + 429, 临时起始Y + 248, FONT_PRIO_FRONT, 0, moji, 0); y += 20;

						sprintf(moji, "%d", pc.tgh);
						StockFontBuffer(临时起始X + 328, 临时起始Y + 270, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%d", pc.dex);
						StockFontBuffer(临时起始X + 429, 临时起始Y + 270, FONT_PRIO_FRONT, 0, moji, 0); y += 20;

						//打印地水火风属性
						int X偏移量 = 74;
						int Y偏移量 = 296;
						if (pc.earth > 0) {
							int 属性量 = (int)(pc.earth * 0.6);
							StockBoxDispBuffer(临时起始X + X偏移量, 临时起始Y + Y偏移量, 临时起始X + X偏移量 + 属性量, 临时起始Y + Y偏移量 + 10, DISP_PRIO_IME3, SYSTEM_PAL_GREEN, 1);
						}
						Y偏移量 += 18;
						if (pc.water > 0) {
							int 属性量 = (int)(pc.water * 0.6);
							StockBoxDispBuffer(临时起始X + X偏移量, 临时起始Y + Y偏移量, 临时起始X + X偏移量 + 属性量, 临时起始Y + Y偏移量 + 10, DISP_PRIO_IME3, SYSTEM_PAL_AQUA, 1);
						}
						Y偏移量 += 18;
						if (pc.fire > 0) {
							int 属性量 = (int)(pc.fire * 0.6);
							StockBoxDispBuffer(临时起始X + X偏移量, 临时起始Y + Y偏移量, 临时起始X + X偏移量 + 属性量, 临时起始Y + Y偏移量 + 10, DISP_PRIO_IME3, SYSTEM_PAL_RED, 1);
						}
						Y偏移量 += 18;
						if (pc.wind > 0) {
							int 属性量 = (int)(pc.wind * 0.6);
							StockBoxDispBuffer(临时起始X + X偏移量, 临时起始Y + Y偏移量, 临时起始X + X偏移量 + 属性量, 临时起始Y + Y偏移量 + 10, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW, 1);
						}
						X偏移量 = 140;
						Y偏移量 = 293;
						sprintf(moji, "(%2d)", pc.earth / 10);
						StockFontBuffer(临时起始X + X偏移量, 临时起始Y + Y偏移量, FONT_PRIO_FRONT, 5, moji, 0);
						Y偏移量 += 18;
						sprintf(moji, "(%2d)", pc.water / 10);
						StockFontBuffer(临时起始X + X偏移量, 临时起始Y + Y偏移量, FONT_PRIO_FRONT, 1, moji, 0);
						Y偏移量 += 18;
						sprintf(moji, "(%2d)", pc.fire / 10);
						StockFontBuffer(临时起始X + X偏移量, 临时起始Y + Y偏移量, FONT_PRIO_FRONT, 6, moji, 0);
						Y偏移量 += 18;
						sprintf(moji, "(%2d)", pc.wind / 10);
						StockFontBuffer(临时起始X + X偏移量, 临时起始Y + Y偏移量, FONT_PRIO_FRONT, 4, moji, 0);


						//画人物头像
						StockDispBuffer(临时起始X + 75, 临时起始Y + 84, DISP_PRIO_IME3, pc.faceGraNo, 0);
						//画人物动画
						if (pActPet == NULL) {
							pActPet = MakeAnimDisp(临时起始X + 145, 临时起始Y + 260, pc.graNo, ANIM_DISP_PET);
						}
						statusWndFontNo[6] = StockDispBuffer(临时起始X + 220, 临时起始Y + 310, DISP_PRIO_IME3, 52045, 2);
#ifdef 人物皮肤
						皮肤按钮索引 = StockDispBuffer(临时起始X + 330, 临时起始Y + 320, DISP_PRIO_IME3, IMAGE_皮肤管理, 2);
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							if (HitDispNo == 皮肤按钮索引) {
								lssproto_SaMenu_send(sockfd, 4, 1, 0);
								SkillWndflag = false;
								DeathAction(pActMenuWnd);
								pActMenuWnd = NULL;
								DeathAction(pActPet);
								pActPet = NULL;
								//statusWndNo = 3;
								MenuToggleFlag ^= JOY_CTRL_S;
								play_se(203, 320, 240);
							}
						}
#endif
#ifdef 人物称号
						称号按钮索引 = StockDispBuffer(临时起始X + 410, 临时起始Y + 320, DISP_PRIO_IME3, 52073, 2);
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							if (HitDispNo == 称号按钮索引) {
								lssproto_SaMenu_send(sockfd, 4, 2, 0);
								SkillWndflag = false;
								DeathAction(pActMenuWnd);
								pActMenuWnd = NULL;
								DeathAction(pActPet);
								pActPet = NULL;
								//statusWndNo = 3;
								MenuToggleFlag ^= JOY_CTRL_S;
								play_se(203, 320, 240);
							}
						}

#endif
#ifdef 人物光环
						特效按钮索引 = StockDispBuffer(临时起始X + 490, 临时起始Y + 320, DISP_PRIO_IME3, 52072, 2);
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							if (HitDispNo == 特效按钮索引) {
								lssproto_SaMenu_send(sockfd, 4, 3, 0);
								SkillWndflag = false;
								DeathAction(pActMenuWnd);
								pActMenuWnd = NULL;
								DeathAction(pActPet);
								pActPet = NULL;
								//statusWndNo = 3;
								MenuToggleFlag ^= JOY_CTRL_S;
								play_se(203, 320, 240);
							}
						}

#endif
						//状态明细
						statusWndFontNo[12] = StockDispBuffer(临时起始X + 220, 临时起始Y + 340, DISP_PRIO_IME3, 52046, 2);
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							if (HitDispNo == statusWndFontNo[12]) {
								SkillWndflag = false;
								DeathAction(pActMenuWnd);
								pActMenuWnd = NULL;
								DeathAction(pActPet);
								pActPet = NULL;
								statusWndNo = 3;
								//MenuToggleFlag ^= JOY_CTRL_S;
								play_se(203, 320, 240);
#ifdef _NEWREQUESTPROTOCOL// (不可开) Syu ADD 新增Protocol要求细项
								lssproto_RESIST_send(sockfd, "");
#endif
#ifdef _ALCHEPLUS
								lssproto_ALCHEPLUS_send(sockfd, "");
#endif
							}
						}

					}
					int 关闭按钮索引 = StockDispBuffer(临时图标X + 264, 临时图标Y - 180, DISP_PRIO_IME3, IMAGE_窗口关闭X, 1);//关闭按钮
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == 关闭按钮索引) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathAction(pActPet);
							pActPet = NULL;
							petWndNo = 0;
							GetKeyInputFocus(&MyChatBuffer);
							MenuToggleFlag ^= JOY_CTRL_S;
							play_se(203, 320, 240);
						}
					}
					StockDispBuffer(临时图标X, 临时图标Y, DISP_PRIO_MENU, 人物底图编号, 1);//打印人物界面底图
				}
			}


#else
			if (pActMenuWnd == NULL) {

				pActMenuWnd = MakeWindowDisp(4, 4, 308, 368, 0, -1);

				for (i = 0; i < MENU_STATUS_0; i++) statusWndFontNo[i] = -2;
				for (i = 0; i < MENU_STATUS_0; i++) statusWndBtnFlag[i] = 0;
#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面
				SkillWndflag = false;
#endif
			}
			else {
				if (pActMenuWnd->hp > 0) {
#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面
					if (SkillWndflag == true) {
						SkillWndfunc2();
					}
					else {
						if (pActSkillMenuWnd != NULL) {
							DeathAction(pActSkillMenuWnd);
							pActSkillMenuWnd = NULL;
						}
					}
					//画背景

					StockDispBuffer绝对位置(pActMenuWnd->x, pActMenuWnd->y, DISP_PRIO_MENU, IMAGE_CHAR_STATUS, 1);

#else
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_STATUS_WND, 1);
#endif
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
#ifdef 人物皮肤
						if (HitDispNo == statusWndFontNo[17])  //皮肤
						{
							lssproto_SaMenu_send(sockfd, 4, 1, 0);
							SkillWndflag = false;
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathAction(pActPet);
							pActPet = NULL;
							//statusWndNo = 3;
							MenuToggleFlag ^= JOY_CTRL_S;
							play_se(203, 320, 240);
						}
#endif
#ifdef 人物称号
						else if (HitDispNo == statusWndFontNo[18]) //英雄称号
						{
							lssproto_SaMenu_send(sockfd, 4, 2, 0);
							SkillWndflag = false;
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathAction(pActPet);
							pActPet = NULL;
							//statusWndNo = 3;
							MenuToggleFlag ^= JOY_CTRL_S;
							play_se(203, 320, 240);
						}
#endif
#ifdef 人物光环
						else if (HitDispNo == statusWndFontNo[19]) //光环
						{
							lssproto_SaMenu_send(sockfd, 4, 3, 0);
							SkillWndflag = false;
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathAction(pActPet);
							pActPet = NULL;
							//statusWndNo = 3;
							MenuToggleFlag ^= JOY_CTRL_S;
							play_se(203, 320, 240);

						}
#endif
						else if (HitDispNo == statusWndFontNo[0]) { 	// 队伍
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							statusWndNo = 1;
							play_se(202, 320, 240);
						}
						else if (HitDispNo == statusWndFontNo[1]) {  // 关闭
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							MenuToggleFlag ^= JOY_CTRL_S;
							play_se(203, 320, 240);
						}
#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面 
						else if (HitDispNo == statusWndFontNo[12])  //点击状态明细
						{
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							statusWndNo = 3;
							play_se(202, 320, 240);
#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
							lssproto_RESIST_send(sockfd, "");
#endif
#ifdef _ALCHEPLUS
							lssproto_ALCHEPLUS_send(sockfd, "");
#endif
						}
						else if (HitDispNo == statusWndFontNo[14])   //职业技能
						{
							play_se(202, 320, 240);
							SkillWndflag = true;
							MenuToggleFlag &= ~JOY_CTRL_I;
							MenuToggleFlag &= ~JOY_CTRL_M;
							if (pActMenuWnd3 != NULL) {
								DeathAction(pActMenuWnd3);
								pActMenuWnd3 = NULL;
								GetKeyInputFocus(&MyChatBuffer);
							}
						}
#endif

					}

					if (HitDispNo == statusWndFontNo[6]) {   	// 改变称号
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面
							SkillWndflag = false;
							MenuToggleFlag &= ~JOY_CTRL_I;
							//MenuToggleFlag &= ~JOY_CTRL_M;
#endif
							if (pActMenuWnd3 == NULL) {
#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面
								pActMenuWnd3 = MakeWindowDisp(304, 16 + 280 + 48, 272, 88, 0, -1);
#else
								pActMenuWnd3 = MakeWindowDisp(4, 16 + 280 + 70, 272, 88, 0, -1);
#endif
								play_se(202, 320, 240);
								shougouChange.buffer[0] = NULL;
								shougouChange.cnt = 0;
								shougouChange.cursor = 0;
								shougouChange.len = 12;
								shougouChange.color = 0;
								shougouChange.x = pActMenuWnd3->x + 38;
								shougouChange.y = pActMenuWnd3->y + 25;
								shougouChange.fontPrio = FONT_PRIO_FRONT;
								statusWndBtnFlag[6] = TRUE;
							}
							else {
								DeathAction(pActMenuWnd3);
								pActMenuWnd3 = NULL;
								GetKeyInputFocus(&MyChatBuffer);
								play_se(203, 320, 240);
							}
							statusWndBtnFlag[6] = TRUE;
						}
						if (mouse.state & MOUSE_LEFT_CRICK && statusWndBtnFlag[6] == TRUE) {
							statusWndBtnFlag[6] = TRUE;
						}
						else {
							statusWndBtnFlag[6] = FALSE;
						}
					}
					else {
						statusWndBtnFlag[6] = FALSE;
					}
					if (StatusUpPoint != 0) {
						for (i = 2; i < 6; i++) {
							if (HitDispNo == statusWndFontNo[i]) {
								if (mouse.onceState & MOUSE_LEFT_CRICK) {
									if (bNewServer)
										lssproto_SKUP_send(sockfd, i - 2);
									else
										old_lssproto_SKUP_send(sockfd, i - 2);


#ifndef _CHAR_PROFESSION			// WON ADD 人物职业
									StatusUpPoint--;
#endif

									play_se(211, 320, 240);

									statusWndBtnFlag[i] = TRUE;
								}
								if (mouse.state & MOUSE_LEFT_CRICK && statusWndBtnFlag[i] == TRUE) {
									statusWndBtnFlag[i] = TRUE;
								}
								else {
									statusWndBtnFlag[i] = FALSE;
								}
							}
							else {
								statusWndBtnFlag[i] = FALSE;
							}
						}
					}
					if (pActMenuWnd != NULL) {
#define PET_WND_ATTR_X 159
#define PET_WND_ATTR_Y 5
						int x2;
						x = pActMenuWnd->x;// + 20;
						y = pActMenuWnd->y;// + 24;
						CenteringStr(pc.name, moji, CHAR_NAME_LEN);
						StockFontBuffer(x + 82, y + 60, FONT_PRIO_FRONT, 0, moji, 0);//打印名字

						sprintf(moji, "%d (%d)", pc.level, pc.transmigration);
						StockFontBuffer(x + 235, y + 60, FONT_PRIO_FRONT, 0, moji, 0);

						StockDispBuffer绝对位置(x+ 238, y + 91, DISP_PRIO_IME3, pc.faceGraNo, 0);//打印头像

						if(pc.maxExp <= 0 )
							CenteringStr("MAX", moji, CHAR_NAME_LEN);
						else
							sprintf(moji, "%d (%.2f%%)", pc.exp, float(pc.exp) / float(pc.maxExp));

						StockFontBuffer(x + 82, y + 95, FONT_PRIO_FRONT, 0, moji, 0);//打印昵称

						sprintf(moji, "%-4d", pc.hp);
						StockFontBuffer(x + 82, y + 121, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%-4d", pc.maxHp);
						StockFontBuffer(x + 151, y + 121, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%-3d", pc.mp);
						StockFontBuffer(x + 82, y + 147, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "100");
						StockFontBuffer(x + 151, y + 147, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%-4d", pc.atk);
						StockFontBuffer(x + 82, y + 180, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%-4d", pc.def);
						StockFontBuffer(x + 178, y + 180, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%-4d", pc.quick);
						StockFontBuffer(x + 273, y + 180, FONT_PRIO_FRONT, 0, moji, 0);


						StockFontBuffer(x + 82, y + 206, FONT_PRIO_FRONT, 0, "0", 0);

						StockFontBuffer(x + 178, y + 206, FONT_PRIO_FRONT, 0, "0", 0);

						sprintf(moji, "%-4d", pc.charm);
						StockFontBuffer(x + 273, y + 206, FONT_PRIO_FRONT, 0, moji, 0);

						StockFontBuffer(x + 92, y + 232, FONT_PRIO_FRONT, 0, "0%", 0);

						StockFontBuffer(x + 178, y + 232, FONT_PRIO_FRONT, 0, "0%", 0);

						StockFontBuffer(x + 273, y + 232, FONT_PRIO_FRONT, 0, "0%", 0);


						if (pc.earth > 0) {
							x2 = (int)(pc.earth * 0.8);
							StockBoxDispBuffer(x + 91 + 0, y + 261 + 0, x + 91 + 0 + x2, y + 261 + 13, DISP_PRIO_IME4, SYSTEM_PAL_GREEN, 1);
							StockBoxDispBuffer(x + 91 + 1, y + 261 + 1, x + 91 + 1 + x2, y + 261 + 14, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0);
							StockBoxDispBuffer(x + 91 + 2, y + 261 + 2, x + 91 + 2 + x2, y + 261 + 15, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0);
						}

						if (pc.water > 0) {
							x2 = (int)(pc.water * 0.8);
							StockBoxDispBuffer(x + 232 + 0, y + 261 + 0, x + 232 + 0 + x2, y + 261 + 13, DISP_PRIO_IME4, SYSTEM_PAL_AQUA, 1);
							StockBoxDispBuffer(x + 232 + 1, y + 261 + 1, x + 232 + 1 + x2, y + 261 + 14, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0);
							StockBoxDispBuffer(x + 232 + 2, y + 261 + 2, x + 232 + 2 + x2, y + 261 + 15, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0);
						}

						if (pc.fire > 0) {
							x2 = (int)(pc.fire * 0.8);
							StockBoxDispBuffer(x + 91 + 0, y + 281 + 0, x + 91 + 0 + x2, y + 281 + 13, DISP_PRIO_IME4, SYSTEM_PAL_RED, 1);
							StockBoxDispBuffer(x + 91 + 1, y + 281 + 1, x + 91 + 1 + x2, y + 281 + 14, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0);
							StockBoxDispBuffer(x + 91 + 2, y + 281 + 2, x + 91 + 2 + x2, y + 281 + 15, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0);
						}

						if (pc.wind > 0) {
							x2 = (int)(pc.wind * 0.8);
							StockBoxDispBuffer(x + 232 + 0, y + 281 + 0, x + 232 + 0 + x2, y + 281 + 13, DISP_PRIO_IME4, SYSTEM_PAL_YELLOW, 1);
							StockBoxDispBuffer(x + 232 + 1, y + 281 + 1, x + 232 + 1 + x2, y + 281 + 14, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0);
							StockBoxDispBuffer(x + 232 + 2, y + 281 + 2, x + 232 + 2 + x2, y + 281 + 15, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0);
						}

						sprintf(moji, "%-d", pc.vital);
						StockFontBuffer(x + 106, y + 342, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%-d", pc.str);
						StockFontBuffer(x + 249, y + 342, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%-d", pc.tgh);
						StockFontBuffer(x + 106, y + 362, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%-d", pc.dex);
						StockFontBuffer(x + 249, y + 362, FONT_PRIO_FRONT, 0, moji, 0);
						if (StatusUpPoint != 0) {
							//StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx + 14 , ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my + 67 , DISP_PRIO_IME3, CG_STATUS_WND_LV_UP_POINT, 0 );//升级点
							sprintf(moji, "%-d", StatusUpPoint);
							StockFontBuffer(x + 232, y + 316, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0);

							{
								statusWndFontNo[2] = StockDispBuffer绝对位置(x + 140, y + 342, DISP_PRIO_IME3, CG_STATUS_WND_UP_BTN_UP + statusWndBtnFlag[2], 2);
								statusWndFontNo[3] = StockDispBuffer绝对位置(x + 282, y + 342, DISP_PRIO_IME3, CG_STATUS_WND_UP_BTN_UP + statusWndBtnFlag[3], 2);
								statusWndFontNo[4] = StockDispBuffer绝对位置(x + 140, y + 362, DISP_PRIO_IME3, CG_STATUS_WND_UP_BTN_UP + statusWndBtnFlag[4], 2);
								statusWndFontNo[5] = StockDispBuffer绝对位置(x + 282, y + 362, DISP_PRIO_IME3, CG_STATUS_WND_UP_BTN_UP + statusWndBtnFlag[5], 2);
							}

						}


						//打印加点点数和加点按钮
						//改变称号按钮
						

						{
							//statusWndFontNo[ 12 ] = StockDispBuffer(x + 87 , y + 4088, DISP_PRIO_IME3, CG_STATUS_DETAIL, 2 );//打印状态明细按钮
							statusWndFontNo[ 6 ] = StockDispBuffer(x + 87, y + 408, DISP_PRIO_IME3, CG_STATUS_WND_SHOUGOU_BTN_UP + statusWndBtnFlag[6], 2);
							statusWndFontNo[ 0 ] = StockDispBuffer(x + 184, y + 408, DISP_PRIO_IME3, CG_STATUS_WND_GROUP_BTN, 2);//상태창 그룹버튼
							statusWndFontNo[ 1 ] = StockDispBuffer(x + 282, y + 408, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);//상태창 닫기버
						}

					}
				}
			}
			if (pActMenuWnd3 != NULL) {
				if (pActMenuWnd3->hp > 0) {
#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd3->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd3->pYobi)->my, DISP_PRIO_MENU, CG_NAME_CHANGE_WND, 1);
#else
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd3->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd3->pYobi)->my, DISP_PRIO_MENU, CG_NAME_CHANGE_WND, 1);
#endif
					GetKeyInputFocus(&shougouChange);
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == statusWndFontNo[7]) {
							KeyboardReturn();
						}
						if (HitDispNo == statusWndFontNo[8]) {
							DeathAction(pActMenuWnd3);
							pActMenuWnd3 = NULL;
							GetKeyInputFocus(&MyChatBuffer);
							play_se(203, 320, 240);
						}
					}
					if (pActMenuWnd3 != NULL) {
						StockFontBuffer2(&shougouChange);
						x = pActMenuWnd3->x + 20;
						y = pActMenuWnd3->y + 60;
						statusWndFontNo[7] = StockDispBuffer(x + 60, y + 7, DISP_PRIO_IME3, CG_OK_BTN, 2);
						statusWndFontNo[8] = StockDispBuffer(x + 170, y + 7, DISP_PRIO_IME3, CG_CANCEL_BTN, 2);
					}
				}
			}
#endif
			break;
		case 1:
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(4, 0, 256, 456, 0, -1);
				for (i = 0; i < MENU_STATUS_0; i++) statusWndFontNo[i] = -2;
				for (i = 0; i < MENU_STATUS_0; i++) statusWndBtnFlag[i] = 0;
			}
			else {
				if (pActMenuWnd->hp > 0) {
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_STATUS_WND_GROUP_WND, 1);
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == statusWndFontNo[11]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							statusWndNo = 0;
							play_se(203, 320, 240);
						}
#ifdef _TEAM_KICKPARTY
						for (i = 0; i < 5; i++) {
							if (HitDispNo == statusWndFontNo[i + 12]) {
								lssproto_KTEAM_send(sockfd, i);
								play_se(203, 320, 240);
							}
						}
#endif
					}
					if (pActMenuWnd != NULL) {
						int selectFlag;
						x = pActMenuWnd->x + 17;
						y = pActMenuWnd->y + 25;

						if (itemNo != -1 || jujutuNo != -1) selectFlag = 2;
						else selectFlag = FALSE;

						CenteringStr(pc.name, moji, CHAR_NAME_LEN);
						statusWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, selectFlag); y += 19;
						sprintf(moji, "%4d", pc.mp);
						StockFontBuffer(x + 77, y, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%4d   %4d", pc.hp, pc.maxHp);
						StockFontBuffer(x + 142, y, FONT_PRIO_FRONT, 0, moji, 0); y += 20;
						for (i = 0; i < 5; i++) {
							if (pet[i].useFlag == TRUE) {
								if (pet[i].freeName[0] != NULL) CenteringStr(pet[i].freeName, moji, PET_NAME_LEN);
								else CenteringStr(pet[i].name, moji, PET_NAME_LEN);
								statusWndFontNo[i + 1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, selectFlag); y += 20;
								sprintf(moji, "%4d   %4d", pet[i].hp, pet[i].maxHp);
								StockFontBuffer(x + 142, y, FONT_PRIO_FRONT, 0, moji, 0); y += 20;
							}
						}
						y = 268;
						for (i = 0; i < 5; i++) {
							if (pc.id != party[i].id && party[i].useFlag == TRUE) {
								CenteringStr(party[i].name, moji, CHAR_NAME_LEN);
								statusWndFontNo[i + 6] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, selectFlag); y += 20;
#ifdef _TEAM_KICKPARTY
								//andy_add 队长踢人
								statusWndFontNo[i + 12] = StockDispBuffer(x + 187, y - 10, DISP_PRIO_IME3, CG_MAIL_WND_DELETE_BTN, 2);
#endif
								sprintf(moji, "%4d", party[i].mp);
								StockFontBuffer(x + 77, y, FONT_PRIO_FRONT, 0, moji, 0);
								sprintf(moji, "%4d %4d", party[i].hp, party[i].maxHp);
								StockFontBuffer(x + 142, y, FONT_PRIO_FRONT, 0, moji, 0); y += 20;
							}
						}
						statusWndFontNo[11] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, 441, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
					}
				}
			}
			break;
#ifdef _NEWPANEL
		case 3:
#ifdef _NB_状态明细详情
			if (ResoMode == 分辨率::mode800) {
				临时图标X = 800 / 2;
				临时图标Y = 600 / 2;
				临时起始X = 临时图标X - 568 / 2;
				临时起始Y = 临时图标Y - 358 / 2;
			}
			else if (ResoMode == 分辨率::mode1024) {
				临时图标X = 1024 / 2;
				临时图标Y = 768 / 2;
				临时起始X = 临时图标X - 568 / 2;
				临时起始Y = 临时图标Y - 358 / 2;
			}
			else {
				临时图标X = 640 / 2;
				临时图标Y = 480 / 2;
				临时起始X = 临时图标X - 568 / 2;
				临时起始Y = 临时图标Y - 358 / 2;
			}
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(临时起始X, 临时起始Y, 临时起始X + 568, 临时起始Y + 358, 0, -1);
				for (i = 0; i < MENU_STATUS_0; i++) statusWndFontNo[i] = -2;
				for (i = 0; i < MENU_STATUS_0; i++) statusWndBtnFlag[i] = 0;
			}
			else {
				if (pActMenuWnd->hp > 0) {
					statusWndFontNo[13] = StockDispBuffer(临时图标X + 264, 临时图标Y - 180, DISP_PRIO_IME3, IMAGE_窗口关闭X, 1);//返回按钮
																													//statusWndFontNo[ 13 ] = StockDispBuffer( 临时起始X+400, 临时起始Y+400, DISP_PRIO_IME3, CG_TRADE_BACK_BTN, 2 );//返回按钮
					static int statusWndFontNoExtNowCur = 2;
					for (int i = 0; i < 3; i++) {
						int 临时位置X = 305;
						int 临时位置Y = 20;
						int 临时间隔 = 75;
						if (MakeHitBox(临时起始X + 临时位置X + i * 临时间隔, 临时起始Y + 临时位置Y, 临时起始X + 临时位置X + 临时间隔 + i*临时间隔, 临时起始Y + 临时位置Y + 28, DISP_PRIO_IME3) == TRUE) {
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								statusWndFontNoExtNowCur = i;
								play_se(217, 320, 240);
								break;
							}
						}
					}
					if (statusWndFontNoExtNowCur == 0) {//鉴定一
						StockDispBuffer(临时起始X + 343, 临时起始Y + 35, DISP_PRIO_IME3, 52049, 0);
						int tm_count = 0;
						for (; tm_count < 16; tm_count++) {
							statusWndFontNoExt[tm_count] = StockDispBuffer(临时起始X + 71 + (tm_count % 8) * 60, 临时起始Y + 95 + 66 * (int)(tm_count / 8), DISP_PRIO_IME3, iAlchePlusIcon[tm_count], 2);
							StockFontBuffer(临时起始X + 55 + (tm_count % 8) * 60, 临时起始Y + 120 + 66 * (int)(tm_count / 8), FONT_PRIO_FRONT, 2, sAlchePlus_list[tm_count], 0);

							if (HitDispNo == statusWndFontNoExt[tm_count]) {
								//拼接pc的%d
								//CHAR_MAX_DETAIL
								char tm_DetailDesc[64];
								char tm_DetailDesc1[64];
								char tm_DetailDesc2[64];
								sprintf(tm_DetailDesc, "[%s]的材料鉴定熟练度：%d%%\0", sAlchePlus_list[tm_count], iCharAlchePlus[tm_count] / 1000);
								StockFontBuffer(临时起始X + 56, 临时起始Y + 218, FONT_PRIO_FRONT, 1, tm_DetailDesc, 0);
								sprintf(tm_DetailDesc1, "对于[%s]材料的鉴定熟度，可提高素材合成成功率", sAlchePlus_list[tm_count]);
								StockFontBuffer(临时起始X + 56, 临时起始Y + 250, FONT_PRIO_FRONT, 10, tm_DetailDesc1, 0);
								//sprintf(tm_DetailDesc2, "\0");
								//StockFontBuffer(临时起始X + 56, 临时起始Y + 280, FONT_PRIO_FRONT, 10, tm_DetailDesc2, 0);
							}
						}

					}
					else if (statusWndFontNoExtNowCur == 1) {//鉴定二
						StockDispBuffer(临时起始X + 418, 临时起始Y + 35, DISP_PRIO_IME3, 52050, 0);
						int tm_count = 16;
						int tm_count2 = 0;
						for (; tm_count < 25; tm_count++) {
							statusWndFontNoExt[tm_count - 16] = StockDispBuffer(临时起始X + 71 + ((tm_count - 16) % 8) * 60, 临时起始Y + 95 + 66 * (int)((tm_count - 16) / 8), DISP_PRIO_IME3, iAlchePlusIcon[tm_count], 2);
							StockFontBuffer(临时起始X + 55 + (tm_count2 % 8) * 60, 临时起始Y + 120 + 66 * (int)(tm_count2 / 8), FONT_PRIO_FRONT, 2, sAlchePlus_list[tm_count], 0);
							tm_count2++;
							if (HitDispNo == statusWndFontNoExt[tm_count - 16]) {
								char tm_DetailDesc[64];
								char tm_DetailDesc1[64];
								char tm_DetailDesc2[64];

								sprintf(tm_DetailDesc, "[%s]的材料鉴定熟练度：%d%%\0", sAlchePlus_list[tm_count], iCharAlchePlus[tm_count] / 1000);
								StockFontBuffer(临时起始X + 56, 临时起始Y + 218, FONT_PRIO_FRONT, 1, tm_DetailDesc, 0);
								sprintf(tm_DetailDesc1, "对于[%s]材料的鉴定熟度，可提高素材合成成功率", sAlchePlus_list[tm_count]);
								StockFontBuffer(临时起始X + 56, 临时起始Y + 250, FONT_PRIO_FRONT, 10, tm_DetailDesc1, 0);
							}
						}

					}
					else if (statusWndFontNoExtNowCur == 2) {//魔法
						StockDispBuffer(临时起始X + 493, 临时起始Y + 35, DISP_PRIO_IME3, 52051, 0);
						//statusWndFontNoExt[0] = StockDispBuffer(临时起始X + 71 + ((tm_count-16) % 8) * 60, 临时起始Y + 95 + 66 * (int)((tm_count-16) / 8), DISP_PRIO_IME3, iAlchePlusIcon[tm_count], 2);

						statusWndFontNoExt[0] = StockDispBuffer(临时起始X + 0 * 60 + 71, 临时起始Y + 95, DISP_PRIO_IME3, 26475 + 4, 2);
						statusWndFontNoExt[1] = StockDispBuffer(临时起始X + 1 * 60 + 71, 临时起始Y + 95, DISP_PRIO_IME3, 26475 + 0, 2);
						statusWndFontNoExt[2] = StockDispBuffer(临时起始X + 2 * 60 + 71, 临时起始Y + 95, DISP_PRIO_IME3, 26475 + 2, 2);
						statusWndFontNoExt[3] = StockDispBuffer(临时起始X + 3 * 60 + 71, 临时起始Y + 95, DISP_PRIO_IME3, 26475 + 6, 2);
						statusWndFontNoExt[4] = StockDispBuffer(临时起始X + 4 * 60 + 71, 临时起始Y + 95, DISP_PRIO_IME3, 26474 + 4, 2);
						statusWndFontNoExt[5] = StockDispBuffer(临时起始X + 5 * 60 + 71, 临时起始Y + 95, DISP_PRIO_IME3, 26474 + 0, 2);
						statusWndFontNoExt[6] = StockDispBuffer(临时起始X + 6 * 60 + 71, 临时起始Y + 95, DISP_PRIO_IME3, 26474 + 2, 2);
						statusWndFontNoExt[7] = StockDispBuffer(临时起始X + 7 * 60 + 71, 临时起始Y + 95, DISP_PRIO_IME3, 26474 + 6, 2);


						for (int ci = 0; ci < 8; ci++) {
							if (HitDispNo == statusWndFontNoExt[ci]) {
								char tm_DetailDesc[64];
								sprintf(tm_DetailDesc, "%s%s\0", DetailDesc[ci], CharDetail[ci]);

								StockFontBuffer(临时起始X + 56, 临时起始Y + 218, FONT_PRIO_FRONT, 1, tm_DetailDesc, 0);

								StockFontBuffer(临时起始X + 56, 临时起始Y + 250, FONT_PRIO_FRONT, 10, DetailDesc1[ci], 0);
								StockFontBuffer(临时起始X + 56, 临时起始Y + 270, FONT_PRIO_FRONT, 10, DetailDesc2[ci], 0);

							}
						}

					}

					//点击返回
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == statusWndFontNo[13]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							statusWndNo = 0;
							play_se(202, 320, 240);
						}
					}
					StockDispBuffer(临时图标X, 临时图标Y, DISP_PRIO_MENU, 52048, 1);//打印状态界面底图
				}
			}

#else

			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(2, 2, 302, 412, 0, -1);
				for (i = 0; i < MENU_STATUS_0; i++) statusWndFontNo[i] = -2;
				for (i = 0; i < MENU_STATUS_0; i++) statusWndBtnFlag[i] = 0;
			}
			else {
				if (pActMenuWnd->hp > 0) {
					x = pActMenuWnd->x;
					y = pActMenuWnd->y;

					//StockDispBuffer( ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->mx+8, ( ( WINDOW_DISP *)pActMenuWnd->pYobi )->my-5, DISP_PRIO_MENU, CG_SKILL_TABLE, 1 );
					//statusWndFontNo[ 13 ] = StockDispBuffer( x+139, y+423, DISP_PRIO_IME3, CG_TRADE_BACK_BTN, 2 );

					//状态明细 视窗
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_NEW_26473, 1);
					//CC add
					//这里都先用静态的处理了
					statusWndFontNo[13] = StockDispBuffer(x + 215, y + 394, DISP_PRIO_IME3, CG_TRADE_BACK_BTN, 2);
					static int statusWndFontNoExtNowCur = 2;

					if (MakeHitBox(x + 270, y + 10, x + 270 + 30, y + 10 + 80, DISP_PRIO_IME3) == TRUE) {	// 按下鉴定一
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							statusWndFontNoExtNowCur = 0;
						}
					}
					if (MakeHitBox(x + 270, y + 90, x + 270 + 30, y + 90 + 80, DISP_PRIO_IME3) == TRUE) {	// 按下鉴定二
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							statusWndFontNoExtNowCur = 1;
						}
					}
					if (MakeHitBox(x + 270, y + 170, x + 270 + 30, y + 170 + 80, DISP_PRIO_IME3) == TRUE) {	// 按下魔法状态
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							statusWndFontNoExtNowCur = 2;
						}
					}
					//CC FIX
					//20170812
					//精练数据显示
					if (statusWndFontNoExtNowCur == 0) {
						//为啥这里的坐标基本都不用改呢
						//mx+30
						//my-5
						//两个值 是测试出来的
						StockDispBuffer(((WINDOW_DISP*)pActMenuWnd->pYobi)->mx + 133, ((WINDOW_DISP*)pActMenuWnd->pYobi)->my - 156, DISP_PRIO_IME3, CG_NEW_26555, 1);
						int tm_count = 0;
						for (; tm_count < 16; tm_count++) {
							statusWndFontNoExt[tm_count] = StockDispBuffer(x + (tm_count % 4) * 60 + 48, y + 88 + 60 * (int)(tm_count / 4), DISP_PRIO_IME3, iAlchePlusIcon[tm_count], 2);
							if (HitDispNo == statusWndFontNoExt[tm_count]) {
								//拼接pc的%d
								//CHAR_MAX_DETAIL
								char tm_DetailDesc[64];
								char tm_DetailDesc1[64];
								char tm_DetailDesc2[64];
								sprintf(tm_DetailDesc, "[%s]재료 속성을 확인함       %d %% \0", sAlchePlus_list[tm_count], iCharAlchePlus[tm_count] / 1000);
								StockFontBuffer(x + 20, y + 310, FONT_PRIO_FRONT, 0, tm_DetailDesc, 0);
								sprintf(tm_DetailDesc1, "对于[%s]재료의 속성을 감정/확인하여 재료 합성의 성공율을 높일 수 있음\0", sAlchePlus_list[tm_count]);
								StockFontBuffer(x + 20, y + 330, FONT_PRIO_FRONT, 0, tm_DetailDesc1, 0);
								sprintf(tm_DetailDesc2, "재료 합성 성공율\0");
								StockFontBuffer(x + 20, y + 350, FONT_PRIO_FRONT, 0, tm_DetailDesc2, 0);
							}


						}
					}
					else if (statusWndFontNoExtNowCur == 1) {
						StockDispBuffer(((WINDOW_DISP*)pActMenuWnd->pYobi)->mx + 133, ((WINDOW_DISP*)pActMenuWnd->pYobi)->my - 78, DISP_PRIO_IME3, CG_NEW_26556, 1);
						int tm_count = 16;
						for (; tm_count < 25; tm_count++) {
							statusWndFontNoExt[tm_count - 16] = StockDispBuffer(x + ((tm_count - 16) % 4) * 60 + 48, y + 88 + 60 * (int)((tm_count - 16) / 4), DISP_PRIO_IME3, iAlchePlusIcon[tm_count], 2);
							if (HitDispNo == statusWndFontNoExt[tm_count - 16]) {
								//拼接pc的%d
								//CHAR_MAX_DETAIL
								char tm_DetailDesc[64];
								char tm_DetailDesc1[64];
								char tm_DetailDesc2[64];
								sprintf(tm_DetailDesc, "[%s]재료 속성을 확인함         %d %% \0", sAlchePlus_list[tm_count], iCharAlchePlus[tm_count] / 1000);
								StockFontBuffer(x + 20, y + 310, FONT_PRIO_FRONT, 0, tm_DetailDesc, 0);
								sprintf(tm_DetailDesc1, "재료 속성을 확인하여[%s]를 높일 수 있음\0", sAlchePlus_list[tm_count]);
								StockFontBuffer(x + 20, y + 330, FONT_PRIO_FRONT, 0, tm_DetailDesc1, 0);
								sprintf(tm_DetailDesc2, "재료합성 성공율\0");
								StockFontBuffer(x + 20, y + 350, FONT_PRIO_FRONT, 0, tm_DetailDesc2, 0);
							}
						}
					}
					else if (statusWndFontNoExtNowCur == 2) {
						statusWndFontNoExt[0] = StockDispBuffer(x + 0 * 60 + 48, y + 88, DISP_PRIO_IME3, 26479, 2);
						statusWndFontNoExt[1] = StockDispBuffer(x + 1 * 60 + 48, y + 88, DISP_PRIO_IME3, 26475, 2);
						statusWndFontNoExt[2] = StockDispBuffer(x + 2 * 60 + 48, y + 88, DISP_PRIO_IME3, 26477, 2);
						statusWndFontNoExt[3] = StockDispBuffer(x + 3 * 60 + 48, y + 88, DISP_PRIO_IME3, 26481, 2);
						statusWndFontNoExt[4] = StockDispBuffer(x + 0 * 60 + 48, y + 88, DISP_PRIO_IME3, 26478, 2);
						statusWndFontNoExt[5] = StockDispBuffer(x + 1 * 60 + 48, y + 88, DISP_PRIO_IME3, 26474, 2);
						statusWndFontNoExt[6] = StockDispBuffer(x + 2 * 60 + 48, y + 88, DISP_PRIO_IME3, 26476, 2);
						statusWndFontNoExt[7] = StockDispBuffer(x + 3 * 60 + 48, y + 88, DISP_PRIO_IME3, 26480, 2);
						StockDispBuffer(((WINDOW_DISP*)pActMenuWnd->pYobi)->mx + 133, ((WINDOW_DISP*)pActMenuWnd->pYobi)->my, DISP_PRIO_IME3, CG_NEW_26486, 1);
						for (int ci = 0; ci < 8; ci++) {
							if (HitDispNo == statusWndFontNoExt[ci]) {
								//拼接pc的%d
								//CHAR_MAX_DETAIL
								char tm_DetailDesc[64];
								sprintf(tm_DetailDesc, "%s%s\0", DetailDesc[ci], CharDetail[ci]);
								StockFontBuffer(x + 20, y + 310, FONT_PRIO_FRONT, 0, tm_DetailDesc, 0);
								StockFontBuffer(x + 20, y + 330, FONT_PRIO_FRONT, 0, DetailDesc1[ci], 0);
								StockFontBuffer(x + 20, y + 350, FONT_PRIO_FRONT, 0, DetailDesc2[ci], 0);
							}
						}
					}

					//默认显示精灵魔法
					//int coffsetX = 34;
					//int coffsetY = 73;
					//
					//for ( int ci = 0 ; ci < 4 ; ci ++ ) 
					//{



					//}
					//
					//DetailDesc



#ifdef _SYUTEST3
					for (i = 14; i < 22; i++)
						statusWndFontNo[i] = StockDispBuffer(x + 139, y + 200 + (i - 14) * 20, DISP_PRIO_IME3, CG_TRADE_BACK_BTN, 2);
#endif
					//点击返回
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == statusWndFontNo[13]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							statusWndNo = 0;
							play_se(202, 320, 240);
						}



#ifdef _SYUTEST3
						if (HitDispNo == statusWndFontNo[14])
							lssproto_CHATROOM_send(sockfd, "C|123456798");
						if (HitDispNo == statusWndFontNo[15])
							lssproto_CHATROOM_send(sockfd, "D|123456798");
						if (HitDispNo == statusWndFontNo[16])
							lssproto_CHATROOM_send(sockfd, "J|0");
						if (HitDispNo == statusWndFontNo[17])
							lssproto_CHATROOM_send(sockfd, "L|123456798");
						if (HitDispNo == statusWndFontNo[18])
							lssproto_CHATROOM_send(sockfd, "K|123456798");
						if (HitDispNo == statusWndFontNo[19])
							lssproto_CHATROOM_send(sockfd, "M|123456798");
						if (HitDispNo == statusWndFontNo[20])
							lssproto_CHATROOM_send(sockfd, "T|123456798");
						if (HitDispNo == statusWndFontNo[21])
							lssproto_CHATROOM_send(sockfd, "B|123456798");
#endif
					}
				}
			}
#endif
			break;
#endif
		}

	}
	else {
		void delAct称号();
		void delAct光环();
		void delAct皮肤();
		delAct称号();
		delAct光环();
		delAct皮肤();
	}
	//? ?????????? ********************************************************/

	if (MenuToggleFlag & JOY_CTRL_P) {
#ifdef _STREET_VENDOR
		if (pc.iOnStreetVendor == 1 || sStreetVendorBuyBtn == 2) {
			StockChatBufferLine("상점기능이용시, 페트를변경할수없어요!", FONT_PAL_RED);
			MenuToggleFlag ^= JOY_CTRL_P;
			// 为了不执行宠物视窗所以设定为 3,不作事
			petWndNo = 3;
		}
#endif
#ifdef _NB_宠物栏详情
		int 临时图标X, 临时图标Y, 临时起始X, 临时起始Y;
		//计算图片中心
		if (ResoMode == 分辨率::mode800) {
			临时图标X = 800 / 2;
			临时图标Y = 600 / 2;
			临时起始X = 临时图标X - 568 / 2;
			临时起始Y = 临时图标Y - 358 / 2;
		}
		else if (ResoMode == 分辨率::mode1024) {
			临时图标X = 1024 / 2;
			临时图标Y = 768 / 2;
			临时起始X = 临时图标X - 568 / 2;
			临时起始Y = 临时图标Y - 358 / 2;
		}
		else {
			临时图标X = 640 / 2;
			临时图标Y = 480 / 2;
			临时起始X = 临时图标X - 568 / 2;
			临时起始Y = 临时图标Y - 358 / 2;
		}
		if (pActMenuWnd == NULL) {
			pActMenuWnd = MakeWindowDisp(临时起始X, 临时起始Y, 临时起始X + 568, 临时起始Y + 358, 0, -1);
			for (i = 0; i < MENU_PET_0; i++) petWndFontNo[i] = -2;
			BattlePetReceivePetNo = -1;
			DropPetWndflag = false;
			DropI = -1;
			选中宠物 = -1;
			宠物技能选中ID = -1;
		}
		else {
			if (pActMenuWnd->hp > 0) {
				for (i = 0; i < 5; i++) {
					if (pet[i].useFlag == FALSE) {
						if (i == pc.battlePetNo)
							pc.battlePetNo = -1;
						if (i == pc.mailPetNo)
							pc.mailPetNo = -1;
						pc.selectPetNo[i] = FALSE;
					}
				}
				BattlePetStMenCnt = 0;
				for (i = 0; i < 5; i++) {
					if (pc.selectPetNo[i] == TRUE) BattlePetStMenCnt++;
				}
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					for (i = 0; i < 5; i++) {
						if (pet[i].useFlag == TRUE && BattlePetReceivePetNo != i) {
							if (HitDispNo == petWndFontNo[i]) {
#if 1
								if (i == pc.mailPetNo) {
									pc.mailPetNo = -1;
									if ((bNewServer & 0xf000000) == 0xf000000)
										lssproto_PETST_send(sockfd, i, 0);
									checkRidePet(i);
									play_se(217, 320, 240);
								}
								else
									if (i == pc.ridePetNo && pc.graNo != SPR_pet021 && pc.graNo != 100362) {//金飞
										char buf[64];
										sprintf(buf, "R|P|-1");
										if (bNewServer)
											lssproto_FM_send(sockfd, buf);
										else
											lssproto_FM_send(sockfd, buf);
										play_se(217, 320, 240);
										if ((bNewServer & 0xf000000) == 0xf000000)
											lssproto_PETST_send(sockfd, i, 0);
									}
									else
										if (i == pc.battlePetNo && BattlePetReceiveFlag == FALSE) {
											if (bNewServer)
												lssproto_KS_send(sockfd, -1);
											else
												old_lssproto_KS_send(sockfd, -1);
											BattlePetReceiveFlag = TRUE;
											BattlePetReceivePetNo = i;
											pc.selectPetNo[i] = FALSE;
											BattlePetStMenCnt--;
											if (pc.mailPetNo == -1) {
												pc.mailPetNo = i;
												if ((bNewServer & 0xf000000) == 0xf000000)
													lssproto_PETST_send(sockfd, i, 4);
											}
											else {
												checkRidePet(i);
												if ((bNewServer & 0xf000000) == 0xf000000)
													lssproto_PETST_send(sockfd, i, 0);
											}
											play_se(217, 320, 240);
										}
										else
											if (pc.selectPetNo[i] == TRUE) {
												if (pc.battlePetNo == -1 && BattlePetReceiveFlag == FALSE) {
													if (pet[i].hp > 0) {
														if (bNewServer)
															lssproto_KS_send(sockfd, i);
														else
															old_lssproto_KS_send(sockfd, i);
														BattlePetReceiveFlag = TRUE;
														BattlePetReceivePetNo = i;
														play_se(217, 320, 240);
													}
													else {
														play_se(220, 320, 240);
													}
												}
												else {
													if (pc.mailPetNo == -1) {
														pc.mailPetNo = i;
														if ((bNewServer & 0xf000000) == 0xf000000)
															lssproto_PETST_send(sockfd, i, 4);
													}
													else {
														checkRidePet(i);
														if ((bNewServer & 0xf000000) == 0xf000000)
															lssproto_PETST_send(sockfd, i, 0);
													}
													pc.selectPetNo[i] = FALSE;
													BattlePetStMenCnt--;
													play_se(217, 320, 240);
												}
											}
											else
												if (pc.selectPetNo[i] == FALSE) {
													if (BattlePetStMenCnt < 等待宠数量) {
														pc.selectPetNo[i] = TRUE;
														BattlePetStMenCnt++;
														play_se(217, 320, 240);
														if ((bNewServer & 0xf000000) == 0xf000000)
															lssproto_PETST_send(sockfd, i, 1);
													}
													else {
														if (pc.mailPetNo == -1) {
															pc.mailPetNo = i;
															play_se(217, 320, 240);
															if ((bNewServer & 0xf000000) == 0xf000000)
																lssproto_PETST_send(sockfd, i, 4);
														}
														else {
															checkRidePet(i);
														}
													}
												}
#endif
							}
							if (HitFontNo == petWndFontNo[i + 5]) {
								petStatusNo = i;
								petWndNo = 1;
								DeathAction(pActMenuWnd);
								pActMenuWnd = NULL;
								play_se(202, 320, 240);
							}

						}
					}
				}

			}
			int 宠物按钮索引;
			for (i = 0; i < 5; i++) {//遍历宠物
				if (pet[i].useFlag == TRUE) {
					sprintf(moji, "%-16s", pet[i].name);
					if (选中宠物 == i) {
						宠物按钮索引 = StockFontBuffer(临时起始X + 60, 临时起始Y + 22 + (i * 60), FONT_PRIO_FRONT, 4, moji, 2);
					}
					else {
						宠物按钮索引 = StockFontBuffer(临时起始X + 60, 临时起始Y + 22 + (i * 60), FONT_PRIO_FRONT, 0, moji, 2);
					}

					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitFontNo == 宠物按钮索引) {
							if (pActPet != NULL) {
								DeathAction(pActPet);
								pActPet = NULL;
							}
							选中宠物 = i;
							宠物技能选中ID = -1;
							petStatusNo = i;
							play_se(202, 320, 240);
						}
					}
					if (mouse.onceState & MOUSE_RIGHT_CRICK) {
						if (pet[i].useFlag == TRUE && eventWarpSendFlag == FALSE) {
							if (HitFontNo == 宠物按钮索引) {
								if (ItemMixPetNo != i && pc.ridePetNo != i) {
									DropPetWndflag = true;
									DropI = i;
								}
							}
						}
					}
					if (选中宠物 == i) {
						if (pet[i].freeName[0] != NULL) {
							sprintf(moji, "%-14s", pet[i].freeName);
							StockFontBuffer(临时起始X + 60, 临时起始Y + 38 + (i * 60), FONT_PRIO_FRONT, 4, moji, 0);
						}
						sprintf(moji, "%-3d", pet[i].level);
						StockFontBuffer(临时起始X + 50, 临时起始Y + 54 + (i * 60), FONT_PRIO_FRONT, 4, moji, 0);
						sprintf(moji, "%d/%d", pet[i].hp, pet[i].maxHp);
						StockFontBuffer(临时起始X + 97, 临时起始Y + 54 + (i * 60), FONT_PRIO_FRONT, 4, moji, 0);
					}
					else {
						if (pet[i].freeName[0] != NULL) {
							sprintf(moji, "%-16s", pet[i].freeName);
							StockFontBuffer(临时起始X + 60, 临时起始Y + 38 + (i * 60), FONT_PRIO_FRONT, 0, moji, 0);
						}
						sprintf(moji, "%-3d", pet[i].level);
						StockFontBuffer(临时起始X + 50, 临时起始Y + 54 + (i * 60), FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%d/%d", pet[i].hp, pet[i].maxHp);
						StockFontBuffer(临时起始X + 97, 临时起始Y + 54 + (i * 60), FONT_PRIO_FRONT, 0, moji, 0);
					}
				}

				int btnNo;
				if (pet[i].useFlag == TRUE) {
					btnNo = 0;// 休息
					if (pc.selectPetNo[i] == TRUE) {
						btnNo = 1;
					}
					if (i == pc.battlePetNo) {// 战斗
						btnNo = 2;
					}
					if (i == pc.mailPetNo) {// 邮件
						btnNo = 3;
					}
					if (i == pc.ridePetNo) { //骑宠		
						btnNo = 4;
					}
					petWndFontNo[i] = StockDispBuffer(临时起始X + 40, 临时起始Y + 36 + (i * 60), DISP_PRIO_IME3, 52028 + btnNo, 2);
				}
			}
			if (DropPetWndflag == true) {
				sprintf(moji, "你확인要丢弃宠物[%s]吗？", pet[DropI].name);
				StockFontBuffer(临时图标X - 110, 临时图标Y - 30, FONT_PRIO_AFRONT, 6, moji, 0);
				StockDispBuffer(临时图标X, 临时图标Y, DISP_PRIO_YES_NO_WND, 52034, 0);
				int 宠物丢弃确认按钮 = StockDispBuffer(临时图标X - 50, 临时图标Y + 30, DISP_PRIO_YES_NO_BTN, 52035, 2);
				int 宠物丢弃취소按钮 = StockDispBuffer(临时图标X + 50, 临时图标Y + 30, DISP_PRIO_YES_NO_BTN, 52036, 2);
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					if (HitDispNo == 宠物丢弃确认按钮) {
						i = DropI;
						DropI = -1;
						lssproto_DP_send(sockfd, nowGx, nowGy, i);
						if (pc.selectPetNo[i] == TRUE) {
							pc.selectPetNo[i] = FALSE;
							BattlePetStMenCnt--;
						}
						if (i == pc.battlePetNo)
							lssproto_KS_send(sockfd, -1);
						if (pc.mailPetNo == i) {
							pc.mailPetNo = -1;
						}
						play_se(217, 320, 240);
						DropPetWndflag = false;
					}
					if (HitDispNo == 宠物丢弃취소按钮) {
						DropPetWndflag = false;
						DropI = -1;
						play_se(217, 320, 240);
					}
				}
			}
			if (选中宠物 >= 0) {
				if (HitDispNo == petWndFontNo[3]) {
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (pet[选中宠物].changeNameFlag == TRUE) {
							if (pActMenuWnd3 == NULL) {
								pActMenuWnd3 = MakeWindowDisp(临时图标X, 临时图标Y, 临时图标X + 272, 临时图标Y + 88, 0, -1);
								play_se(202, 320, 240);
								petNameChange.buffer[0] = NULL;
								petNameChange.cnt = 0;
								petNameChange.cursor = 0;
								petNameChange.len = 12;
								petNameChange.color = 10;
								petNameChange.x = 临时图标X - 40;
								petNameChange.y = 临时图标Y - 30;
								petNameChange.fontPrio = FONT_PRIO_AFRONT;
								petWndBtnFlag[3] = TRUE;
							}
							else {
								DeathAction(pActMenuWnd3);
								pActMenuWnd3 = NULL;
								GetKeyInputFocus(&MyChatBuffer);
								play_se(203, 320, 240);
							}
							petWndBtnFlag[3] = TRUE;
						}
						else {
							play_se(220, 320, 240);
						}
					}
					if (mouse.state & MOUSE_LEFT_CRICK && petWndBtnFlag[3] == TRUE) {
						petWndBtnFlag[3] = TRUE;
					}
					else {
						petWndBtnFlag[3] = FALSE;
					}
				}
				else {
					petWndBtnFlag[3] = FALSE;
				}
				if (pActMenuWnd3 != NULL) {
					if (pActMenuWnd3->hp > 0) {
						GetKeyInputFocus(&petNameChange);
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							if (HitDispNo == petWndFontNo[5]) {
								KeyboardReturn();
							}
							if (HitDispNo == petWndFontNo[6]) {
								DeathAction(pActMenuWnd3);
								pActMenuWnd3 = NULL;
								GetKeyInputFocus(&MyChatBuffer);
								play_se(203, 320, 240);
							}
						}
						if (pActMenuWnd3 != NULL) {
							StockFontBuffer2(&petNameChange);
							petWndFontNo[5] = StockDispBuffer(临时图标X - 50, 临时图标Y + 30, DISP_PRIO_DRAG, 52035, 2);
							petWndFontNo[6] = StockDispBuffer(临时图标X + 50, 临时图标Y + 30, DISP_PRIO_DRAG, 52036, 2);
						}
						StockDispBuffer(临时图标X, 临时图标Y, DISP_PRIO_DRAG, 52065, 0);
					}
				}
				//修改名字
				petWndFontNo[3] = StockDispBuffer(临时起始X + 60, 临时起始Y + 348, DISP_PRIO_IME3, 52045, 2);

				//修改名字END
				if (pet[选中宠物].freeName[0] != NULL) {
					sprintf(moji, "%-s", pet[选中宠物].freeName);
					//CenteringStr( pet[选中宠物 ].freeName, moji, PET_NAME_LEN );
				}
				else {
					sprintf(moji, "%-s", pet[选中宠物].name);
					//CenteringStr( pet[ 选中宠物 ].name, moji, PET_NAME_LEN );
				}

				StockFontBuffer(临时起始X + 210, 临时起始Y + 27, FONT_PRIO_FRONT, 1, moji, 0);

				sprintf(moji, "%-8d", pet[选中宠物].exp);
				StockFontBuffer(临时起始X + 242, 临时起始Y + 55, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(moji, "%-8d", pet[选中宠物].maxExp);
				StockFontBuffer(临时起始X + 242, 临时起始Y + 77, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(moji, "%d/%d", pet[选中宠物].hp, pet[选中宠物].maxHp);
				StockFontBuffer(临时起始X + 242, 临时起始Y + 98, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(moji, "%d", pet[选中宠物].atk);
				StockFontBuffer(临时起始X + 242, 临时起始Y + 119, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(moji, "%d", pet[选中宠物].def);
				StockFontBuffer(临时起始X + 242, 临时起始Y + 140, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(moji, "%d", pet[选中宠物].quick);
				StockFontBuffer(临时起始X + 242, 临时起始Y + 160, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(moji, "%d", pet[选中宠物].ai);
				StockFontBuffer(临时起始X + 242, 临时起始Y + 180, FONT_PRIO_FRONT, 0, moji, 0);

				if (pet[选中宠物].earth > 0) {
					int 属性量 = (int)(pet[选中宠物].earth * 0.6);
					StockBoxDispBuffer(临时起始X + 328, 临时起始Y + 129, 临时起始X + 328 + 属性量, 临时起始Y + 129 + 10, DISP_PRIO_IME3, SYSTEM_PAL_GREEN, 1);
				}
				if (pet[选中宠物].water > 0) {
					int 属性量 = (int)(pet[选中宠物].water * 0.6);
					StockBoxDispBuffer(临时起始X + 328, 临时起始Y + 147, 临时起始X + 328 + 属性量, 临时起始Y + 147 + 10, DISP_PRIO_IME3, SYSTEM_PAL_AQUA, 1);
				}
				if (pet[选中宠物].fire > 0) {
					int 属性量 = (int)(pet[选中宠物].fire * 0.6);
					StockBoxDispBuffer(临时起始X + 328, 临时起始Y + 165, 临时起始X + 328 + 属性量, 临时起始Y + 165 + 10, DISP_PRIO_IME3, SYSTEM_PAL_RED, 1);
				}
				if (pet[选中宠物].wind > 0) {
					int 属性量 = (int)(pet[选中宠物].wind * 0.6);
					StockBoxDispBuffer(临时起始X + 328, 临时起始Y + 183, 临时起始X + 328 + 属性量, 临时起始Y + 183 + 10, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW, 1);
				}
				int j;
				for (j = 0; j < pet[选中宠物].maxSkill; j++) {
					if (petSkill[选中宠物][j].useFlag == TRUE) {
						int color = FONT_PAL_GRAY;
						sprintf(moji, "%-16s", petSkill[选中宠物][j].name);
						if (petSkill[选中宠物][j].field != PETSKILL_FIELD_BATTLE) {
							color = FONT_PAL_WHITE;
						}
						int 宠物技能索引;
						宠物技能索引 = StockFontBuffer(临时起始X + 410, 临时起始Y + 42 + (j * 20), FONT_PRIO_FRONT, color, moji, 2);
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							if (HitFontNo == 宠物技能索引) {
								宠物技能选中ID = j;
								play_se(217, 320, 240);
							}
						}
					}
				}
				if (pActPet == NULL) {
					if ((pet[选中宠物].graNo == 101493) || (pet[选中宠物].graNo == 101494) || (pet[选中宠物].graNo == 101495) || (pet[选中宠物].graNo == 101496)) {
						pActPet = MakeAnimDisp(临时起始X + 360, 临时起始Y + 78, pet[选中宠物].graNo, ANIM_DISP_PET);
					}
					else {
						pActPet = MakeAnimDisp(临时起始X + 360, 临时起始Y + 98, pet[选中宠物].graNo, ANIM_DISP_PET);
					}
				}
#ifdef _NB_宠物初始值
				char 临时字串[56];
				sprintf(临时字串, "%d", pet[选中宠物].initlevel);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 240, 临时起始Y + 227, FONT_PRIO_FRONT, 0, moji, 0);

				sprintf(临时字串, "%d", pet[选中宠物].inithp);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 305, 临时起始Y + 227, FONT_PRIO_FRONT, 0, moji, 0);

				sprintf(临时字串, "%d", pet[选中宠物].initatk);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 361, 临时起始Y + 227, FONT_PRIO_FRONT, 0, moji, 0);

				sprintf(临时字串, "%d", pet[选中宠物].initdef);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 421, 临时起始Y + 227, FONT_PRIO_FRONT, 0, moji, 0);

				sprintf(临时字串, "%d", pet[选中宠物].initquick);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 481, 临时起始Y + 227, FONT_PRIO_FRONT, 0, moji, 0);
				//成长率计算
				float 宠血成长, 宠攻成长, 宠防成长, 宠敏成长, 宠总成长;
				float 宠血预测, 宠攻预测, 宠防预测, 宠敏预测;
				int 宠战斗力 = (pet[选中宠物].maxHp / 4) + pet[选中宠物].atk + pet[选中宠物].def + pet[选中宠物].quick;//战斗力
				int 宠物成长等级差 = pet[选中宠物].level - pet[选中宠物].initlevel;//计算等级差
				if (宠物成长等级差 >= 0) {//如果等级差大于0才进行计算
					宠血成长 = (float)(pet[选中宠物].maxHp - pet[选中宠物].inithp) / 宠物成长等级差;
					宠攻成长 = (float)(pet[选中宠物].atk - pet[选中宠物].initatk) / 宠物成长等级差;
					宠防成长 = (float)(pet[选中宠物].def - pet[选中宠物].initdef) / 宠物成长等级差;
					宠敏成长 = (float)(pet[选中宠物].quick - pet[选中宠物].initquick) / 宠物成长等级差;

					宠总成长 = (float)(宠攻成长 + 宠防成长 + 宠敏成长);//总成长
					int 宠物预测等级差 = 140 - pet[选中宠物].initlevel;//计算等级差(预测值-初始值)
					宠血预测 = (float)(宠血成长 * 宠物预测等级差) + pet[选中宠物].inithp;
					宠攻预测 = (float)(宠攻成长 * 宠物预测等级差) + pet[选中宠物].initatk;
					宠防预测 = (float)(宠防成长 * 宠物预测等级差) + pet[选中宠物].initdef;
					宠敏预测 = (float)(宠敏成长 * 宠物预测等级差) + pet[选中宠物].initquick;
				}
				//计算结束
				//打印预测值
				sprintf(临时字串, "%d", 140);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 240, 临时起始Y + 249, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(临时字串, "%.0f", 宠血预测);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 305, 临时起始Y + 249, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(临时字串, "%.0f", 宠攻预测);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 361, 临时起始Y + 249, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(临时字串, "%.0f", 宠防预测);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 421, 临时起始Y + 249, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(临时字串, "%.0f", 宠敏预测);
				CenteringStr(临时字串, moji, 6);
				StockFontBuffer(临时起始X + 481, 临时起始Y + 249, FONT_PRIO_FRONT, 0, moji, 0);

				//打印成长率
				sprintf(临时字串, "%d", 140);
				CenteringStr(临时字串, moji, 5);
				StockFontBuffer(临时起始X + 240, 临时起始Y + 271, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(临时字串, "%.4f", 宠血成长);
				CenteringStr(临时字串, moji, 5);
				StockFontBuffer(临时起始X + 305, 临时起始Y + 271, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(临时字串, "%.4f", 宠攻成长);
				CenteringStr(临时字串, moji, 5);
				StockFontBuffer(临时起始X + 361, 临时起始Y + 271, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(临时字串, "%.4f", 宠防成长);
				CenteringStr(临时字串, moji, 5);
				StockFontBuffer(临时起始X + 421, 临时起始Y + 271, FONT_PRIO_FRONT, 0, moji, 0);
				sprintf(临时字串, "%.4f", 宠敏成长);
				CenteringStr(临时字串, moji, 5);
				StockFontBuffer(临时起始X + 481, 临时起始Y + 271, FONT_PRIO_FRONT, 0, moji, 0);

				sprintf(临时字串, "%.4f", 宠总成长);
				CenteringStr(临时字串, moji, 5);
				StockFontBuffer(临时起始X + 241, 临时起始Y + 293, FONT_PRIO_FRONT, 0, moji, 0);

				sprintf(临时字串, "%d", 宠战斗力);
				CenteringStr(临时字串, moji, 5);
				StockFontBuffer(临时起始X + 361, 临时起始Y + 293, FONT_PRIO_FRONT, 0, moji, 0);

				if (pet[选中宠物].fusion) {
					sprintf(moji, "融合%d转", pet[选中宠物].trn);
				}
				else {
					sprintf(moji, "%d转", pet[选中宠物].trn);
				}
				StockFontBuffer(临时起始X + 421, 临时起始Y + 293, FONT_PRIO_FRONT, 5, moji, 0);
#endif
				int 拍照按钮索引 = StockDispBuffer(临时起始X + 192, 临时起始Y + 32, DISP_PRIO_IME3, 52037, 1);//关闭按钮
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					if (HitDispNo == 拍照按钮索引) {
						if (pet[选中宠物].graNo != 100451 && pet[选中宠物].graNo != 100432 &&
							pet[选中宠物].graNo != 101280 && pet[选中宠物].graNo != 101281 &&
							pet[选中宠物].graNo != 100015 && pet[选中宠物].graNo != 100018 &&
							pet[选中宠物].graNo != 101279 && pet[选中宠物].graNo != 100401 &&
							pet[选中宠物].graNo != 101414 && pet[选中宠物].graNo != 101167 &&
							pet[选中宠物].graNo != 101172 && pet[选中宠物].graNo != 102011 &&
							pet[选中宠物].graNo != 102012) {	// fix 哪些宠物不能照宠照
							int tblNo = GetPettblNo(pet[选中宠物].graNo);
							int albumNo;
							if (0 <= tblNo && tblNo < MAX_PET_TBL) {
								if (tblNo == 442) albumNo = 0;
								albumNo = PetAlbumTbl[tblNo].albumNo;
								if (albumNo != -1) {
									PetAlbum[albumNo].flag = 2;
									if (pet[选中宠物].freeName[0] != NULL) {
										strcpy(PetAlbum[albumNo].freeName, pet[选中宠物].freeName);
									}
									else {
										strcpy(PetAlbum[albumNo].freeName, PetAlbumTbl[tblNo].name);
									}
									strcpy(PetAlbum[albumNo].name, PetAlbumTbl[tblNo].name);
									PetAlbum[albumNo].faceGraNo = PetAlbumTbl[tblNo].faceGraNo;
									PetAlbum[albumNo].level = pet[选中宠物].level;
									PetAlbum[albumNo].maxHp = pet[选中宠物].maxHp;
									PetAlbum[albumNo].str = pet[选中宠物].atk;
									PetAlbum[albumNo].quick = pet[选中宠物].quick;
									PetAlbum[albumNo].def = pet[选中宠物].def;
									PetAlbum[albumNo].earth = pet[选中宠物].earth;
									PetAlbum[albumNo].water = pet[选中宠物].water;
									PetAlbum[albumNo].fire = pet[选中宠物].fire;
									PetAlbum[albumNo].wind = pet[选中宠物].wind;
									play_se(201, 320, 240);
									sprintf(moji, "%s 照相完成！", PetAlbum[albumNo].freeName);
									StockChatBufferLine(moji, FONT_PAL_WHITE);
									SaveAlbum(albumNo);
								}
								else {
									play_se(220, 320, 240);
								}
							}
							else {
								play_se(220, 320, 240);
							}
						}
					}
				}
				int 秀宠按钮索引 = StockDispBuffer(临时起始X + 215, 临时起始Y + 210, DISP_PRIO_IME3, 52038, 1);//关闭按钮
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					if (HitDispNo == 秀宠按钮索引) {
						char 秀宠内容[128];
						sprintf_s(秀宠内容, "{pet=%d,str=[%s]}", pet[选中宠物].petindex, pet[选中宠物].name);
						strcat_s(MyChatBuffer.buffer, 秀宠内容);
						MyChatBuffer.cursor = strlen(MyChatBuffer.buffer);
						MyChatBuffer.cnt = strlen(MyChatBuffer.buffer);
						DeathAction(pActMenuWnd);
						pActMenuWnd = NULL;
						DeathAction(pActPet);
						pActPet = NULL;
						petWndNo = 0;
						GetKeyInputFocus(&MyChatBuffer);
						MenuToggleFlag ^= JOY_CTRL_P;
						play_se(203, 320, 240);
					}
				}
			}
			if (宠物技能选中ID >= 0) {
				char *splitPoint = petSkill[选中宠物][宠物技能选中ID].memo;
				int 临时打印Y = 临时起始Y + 334;
				while (1) {//分行打印
					if (strlen(splitPoint) > 72) {
						strncpy(moji, splitPoint, 72);
						moji[72] = NULL;
						if (GetStrLastByte(moji) == 3) {
							moji[71] = NULL;
							splitPoint += 71;
						}
						else {
							moji[72] = NULL;
							splitPoint += 72;
						}
						StockFontBuffer(临时起始X + 230, 临时打印Y, FONT_PRIO_FRONT, 11, moji, 0);
						临时打印Y += 20;
					}
					else {
						strcpy(moji, splitPoint);
						StockFontBuffer(临时起始X + 230, 临时打印Y, FONT_PRIO_FRONT, 11, moji, 0);
						break;
					}
				}
			}
			int 关闭按钮索引 = StockDispBuffer(临时图标X + 264, 临时图标Y - 180, DISP_PRIO_IME3, IMAGE_窗口关闭X, 1);//关闭按钮
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (HitDispNo == 关闭按钮索引) {
					DeathAction(pActMenuWnd);
					pActMenuWnd = NULL;
					DeathAction(pActPet);
					pActPet = NULL;
					petWndNo = 0;
					GetKeyInputFocus(&MyChatBuffer);
					MenuToggleFlag ^= JOY_CTRL_P;
					play_se(203, 320, 240);
				}
			}
			StockDispBuffer(临时图标X, 临时图标Y, DISP_PRIO_MENU, 52027, 1);//画背景图


		}
#else
		switch (petWndNo) {
		case 0:
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(4, 4, 272, 320, 0, -1);
				for (i = 0; i < MENU_PET_0; i++) petWndFontNo[i] = -2;
				BattlePetReceivePetNo = -1;
#ifdef _DROPPETWND					// (可开放) Syu ADD 丢弃宠物确认
				DropPetWndflag = false;
				DropI = -1;
#endif
			}
			else {
				if (pActMenuWnd->hp > 0) {
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_PET_WND_VIEW, 1);
					for (i = 0; i < 5; i++) {
						if (pet[i].useFlag == FALSE) {
							if (i == pc.battlePetNo)
								pc.battlePetNo = -1;
							if (i == pc.mailPetNo)
								pc.mailPetNo = -1;
							pc.selectPetNo[i] = FALSE;
						}
					}
					BattlePetStMenCnt = 0;
					for (i = 0; i < 5; i++) {
						if (pc.selectPetNo[i] == TRUE) BattlePetStMenCnt++;
					}
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						for (i = 0; i < 5; i++) {
							if (pet[i].useFlag == TRUE && BattlePetReceivePetNo != i) {
#ifdef _NB_秀宠物
								if (HitDispNo == 秀宠按钮索引[i]) {
									char 秀宠内容[128];
									sprintf_s(秀宠内容, "{pet=%d,str=[%s]}", pet[i].petindex, pet[i].name);
									strcat_s(MyChatBuffer.buffer, 秀宠内容);
									MyChatBuffer.cursor = strlen(MyChatBuffer.buffer);
									MyChatBuffer.cnt = strlen(MyChatBuffer.buffer);
									play_se(217, 320, 240);
								}
#endif
								if (HitDispNo == petWndFontNo[i]) {
#if 1
									if (i == pc.mailPetNo) {
										pc.mailPetNo = -1;
										if ((bNewServer & 0xf000000) == 0xf000000)
											lssproto_PETST_send(sockfd, i, 0);
										play_se(217, 320, 240);
									}
									else
										// when ride Pet
										if (i == pc.ridePetNo && pc.graNo != SPR_pet021&& pc.graNo != 100362) {//金飞
											char buf[64];
											sprintf(buf, "R|P|-1");
											if (bNewServer)
												lssproto_FM_send(sockfd, buf);
											else
												lssproto_FM_send(sockfd, buf);
											play_se(217, 320, 240);
											if ((bNewServer & 0xf000000) == 0xf000000)
												lssproto_PETST_send(sockfd, i, 0);
										}
										else
											if (i == pc.battlePetNo && BattlePetReceiveFlag == FALSE) {
												if (bNewServer)
													lssproto_KS_send(sockfd, -1);
												else
													old_lssproto_KS_send(sockfd, -1);

												BattlePetReceiveFlag = TRUE;
												BattlePetReceivePetNo = i;
												pc.selectPetNo[i] = FALSE;
												BattlePetStMenCnt--;
												if (pc.mailPetNo == -1) {
													pc.mailPetNo = i;
													if ((bNewServer & 0xf000000) == 0xf000000)
														lssproto_PETST_send(sockfd, i, 4);
												}
												else {
													if ((bNewServer & 0xf000000) == 0xf000000)
														lssproto_PETST_send(sockfd, i, 0);
												}
												play_se(217, 320, 240);
											}
											else
												if (pc.selectPetNo[i] == TRUE) {
													if (pc.battlePetNo == -1 && BattlePetReceiveFlag == FALSE) {
														if (pet[i].hp > 0) {
															if (bNewServer)
																lssproto_KS_send(sockfd, i);
															else
																old_lssproto_KS_send(sockfd, i);
															BattlePetReceiveFlag = TRUE;
															BattlePetReceivePetNo = i;
															play_se(217, 320, 240);
														}
														else {
															play_se(220, 320, 240);
														}
													}
													else {
														if (pc.mailPetNo == -1) {
															pc.mailPetNo = i;
															if ((bNewServer & 0xf000000) == 0xf000000)
																lssproto_PETST_send(sockfd, i, 4);
														}
														else {
															if ((bNewServer & 0xf000000) == 0xf000000)
																lssproto_PETST_send(sockfd, i, 0);
														}

														pc.selectPetNo[i] = FALSE;
														BattlePetStMenCnt--;
														play_se(217, 320, 240);
													}
												}
												else
													if (pc.selectPetNo[i] == FALSE) {
														if (BattlePetStMenCnt < 等待宠数量) {
															pc.selectPetNo[i] = TRUE;
															BattlePetStMenCnt++;
															play_se(217, 320, 240);
															if ((bNewServer & 0xf000000) == 0xf000000)
																lssproto_PETST_send(sockfd, i, 1);
														}
														else {
															if (pc.mailPetNo == -1) {
																pc.mailPetNo = i;
																play_se(217, 320, 240);
																if ((bNewServer & 0xf000000) == 0xf000000)
																	lssproto_PETST_send(sockfd, i, 4);
															}
														}
													}
#endif

								}
								if (HitFontNo == petWndFontNo[i + 5]) {
									petStatusNo = i;
									petWndNo = 1;
									DeathAction(pActMenuWnd);
									pActMenuWnd = NULL;
									play_se(202, 320, 240);
								}
								if (HitDispNo == petWndFontNo[i + 12]) {
									if (i == pc.ridePetNo && pc.graNo != SPR_pet021 && pc.graNo != 100362) {//金飞
										char buf[64];
										sprintf(buf, "R|P|-1");
										if (bNewServer)
											lssproto_FM_send(sockfd, buf);
										else
											lssproto_FM_send(sockfd, buf);
										play_se(217, 320, 240);
										if ((bNewServer & 0xf000000) == 0xf000000)
											lssproto_PETST_send(sockfd, i, 0);
									}
									else
									{
										checkRidePet(i);
									}
								}

							}
						}
						if (HitDispNo == petWndFontNo[10]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							MenuToggleFlag ^= JOY_CTRL_P;
							play_se(203, 320, 240);
							saveUserSetting();
						}
						if (HitDispNo == petWndFontNo[11]) {
							petStatusNo = 0;
							for (i = 0; i < 5; i++) {
								if (pet[i].useFlag == TRUE) {
									petStatusNo = i;
									break;
								}
							}
							petWndNo = 1;
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							play_se(202, 320, 240);
						}
#ifdef _DROPPETWND					// (可开放) Syu ADD 丢弃宠物确认
						if (HitDispNo == petWndFontNo[21]) {
							i = DropI;
							DropI = -1;
							lssproto_DP_send(sockfd, nowGx, nowGy, i);
							if (pc.selectPetNo[i] == TRUE) {
								pc.selectPetNo[i] = FALSE;
								BattlePetStMenCnt--;
							}
							if (i == pc.battlePetNo)
								lssproto_KS_send(sockfd, -1);
							if (pc.mailPetNo == i) {
								pc.mailPetNo = -1;
							}
							play_se(217, 320, 240);
							DropPetWndflag = false;
						}
						if (HitDispNo == petWndFontNo[22]) {
							DropPetWndflag = false;
							DropI = -1;
							play_se(217, 320, 240);
						}
#endif
					}
					if (mouse.onceState & MOUSE_RIGHT_CRICK) {
						for (i = 0; i < 5; i++) {
							if (pet[i].useFlag == TRUE && eventWarpSendFlag == FALSE) {
								if (HitFontNo == petWndFontNo[i + 5]) {
									if (ItemMixPetNo != i && pc.ridePetNo != i) {
#ifdef _DROPPETWND					// (可开放) Syu ADD 丢弃宠物确认
										DropPetWndflag = true;
										DropI = i;
#else
										if (bNewServer)
											lssproto_DP_send(sockfd, nowGx, nowGy, i);
										else
											old_lssproto_DP_send(sockfd, nowGx, nowGy, i);
										if (pc.selectPetNo[i] == TRUE) {
											pc.selectPetNo[i] = FALSE;
											BattlePetStMenCnt--;
										}
										if (i == pc.battlePetNo) {
											if (bNewServer)
												lssproto_KS_send(sockfd, -1);
											else
												old_lssproto_KS_send(sockfd, -1);
										}
										if (pc.mailPetNo == i) {
											pc.mailPetNo = -1;
										}
										play_se(217, 320, 240);
#endif
									}
									else {
										play_se(220, 320, 240);
									}
								}
							}
						}
					}
#ifdef _DROPPETWND					// (可开放) Syu ADD 丢弃宠物确认
					if (DropPetWndflag == true) {
						StockFontBuffer(245, 220, FONT_PRIO_AFRONT, 6, "페트를 버릴까요？", 0); y += 40;
						StockDispBuffer(320, 240, DISP_PRIO_YES_NO_WND, CG_DROPWND, 0);
						petWndFontNo[21] = StockDispBuffer(320, 240, DISP_PRIO_YES_NO_BTN, CG_COMMON_YES_BTN, 2);
						petWndFontNo[22] = StockDispBuffer(320, 240, DISP_PRIO_YES_NO_BTN, CG_COMMON_NO_BTN, 2);
					}
#endif
					if (pActMenuWnd != NULL) {

						int flag = FALSE;
						int color;
						int btnNo;
						int atrFlag = FALSE;
						int atrGraNo[4];

						x = pActMenuWnd->x + 16 + 50, y = pActMenuWnd->y + 31;
						for (i = 0; i < 5; i++) {
							if (pet[i].useFlag == TRUE) {
								color = FONT_PAL_WHITE;
								btnNo = 0;					// 休息
								if (pc.selectPetNo[i] == TRUE) { //待机
									color = FONT_PAL_AQUA;
									btnNo = 2;
								}
								// 战斗
								if (i == pc.battlePetNo) {
									color = FONT_PAL_YELLOW;
									btnNo = 1;
								}
								// 邮件
								if (i == pc.mailPetNo) {
									color = FONT_PAL_GREEN;
									btnNo = 3;
								}
								//骑宠
								if (i == pc.ridePetNo) {
									color = FONT_PAL_YELLOW;
									btnNo = 195;
								}
								if (pet[i].freeName[0] != NULL) {
									//CenteringStr( pet[ i ].freeName, moji, PET_NAME_LEN );
									sprintf(moji, "%-16s", pet[i].freeName);
								}
								else {
									//CenteringStr( pet[ i ].name, moji, PET_NAME_LEN );
									sprintf(moji, "%-16s", pet[i].name);
								}
#ifdef _NB_宠物绑定值
								/*char 绑定标志[12];
								if (pet[i].petlock == 1) {
									sprintf(绑定标志, "%s", "绑");
								}
								else {
									sprintf(绑定标志, "%s", "");
								}
								StockFontBuffer(x + 3, y, FONT_PRIO_FRONT, 6, 绑定标志, 0);*/
								petWndFontNo[i + 5] = StockFontBuffer(x + 20, y, FONT_PRIO_FRONT, color, moji, 2);
#else
								petWndFontNo[i + 5] = StockFontBuffer(x + 3, y, FONT_PRIO_FRONT, color, moji, 2);
#endif


								//petWndFontNo[ i + 5 ] = StockFontBuffer( x + 3, y, FONT_PRIO_FRONT, color, moji, 2 );
								/*char 宠物转数[64];
								if (pet[i].fusion) {
									sprintf(宠物转数, "%d", pet[i].trn - 1);
									CenteringStr(宠物转数, moji, 8);
								}
								else {
									sprintf(宠物转数, "%d", pet[i].trn);
									CenteringStr(宠物转数, moji, 8);
								}
								StockFontBuffer(pActMenuWnd->x + 12, y + 24, FONT_PRIO_FRONT, FONT_PAL_GREEN, moji, 0);*/ y += 24;
								atrFlag = FALSE;
								if (pet[i].earth > 0) {
									if (pet[i].earth > 50) atrGraNo[atrFlag] = CG_ATR_ICON_EARTH_BIG;
									else atrGraNo[atrFlag] = CG_ATR_ICON_EARTH_SML;
									atrFlag++;
								}
								if (pet[i].water > 0) {
									if (pet[i].water > 50) atrGraNo[atrFlag] = CG_ATR_ICON_WATER_BIG;
									else atrGraNo[atrFlag] = CG_ATR_ICON_WATER_SML;
									atrFlag++;
								}
								if (pet[i].fire > 0) {
									if (pet[i].fire > 50) atrGraNo[atrFlag] = CG_ATR_ICON_FIRE_BIG;
									else atrGraNo[atrFlag] = CG_ATR_ICON_FIRE_SML;
									atrFlag++;
								}
								if (pet[i].wind > 0) {
									if (pet[i].wind > 50) atrGraNo[atrFlag] = CG_ATR_ICON_WIND_BIG;
									else atrGraNo[atrFlag] = CG_ATR_ICON_WIND_SML;
									atrFlag++;
								}
								if (atrFlag > 0) StockDispBuffer(pActMenuWnd->x + 228, y - 16, DISP_PRIO_IME3, atrGraNo[0], 0);
								if (atrFlag > 1) StockDispBuffer(pActMenuWnd->x + 228 + 16, y - 16, DISP_PRIO_IME3, atrGraNo[1], 0);
								petWndFontNo[i] = StockDispBuffer(x - 27, y - 14, DISP_PRIO_IME3, CG_PET_WND_REST_BTN + btnNo, 2);
								static std::vector<int> ridelist_ =
								{ 130821,130949,130952,100872,105185,130885,130762,130904,130320,100849,130802,119023,102259,103374,100374, 100374, 100374, 105376, 105377, 105378, 105379, 105380, 105381, 105382, 105383, 105384, 105385, 105386, 105387, 105393, 105392, 105390, 105391, 105388, 105389, 100348, 105546, 105547, 105548, 105549, 105550, 105551, 105552, 105553, 105554, 105555, 105556, 105557, 105558, 105559, 105560, 105561, 105562, 110163, 100362, 105428, 105429, 105430, 105431, 105432, 105433, 105434, 105435, 105436, 105437, 105438, 105439, 105440, 105441, 105442, 105443, 105444, 105445, 100307, 105358, 105359, 105360, 105361, 105362, 105363, 105364, 105365, 105366, 105367, 105368, 105369, 105370, 105371, 105372, 105373, 105374, 105375, 100279, 105464, 105465, 105466, 105467, 105468, 105469, 105470, 105471, 105472, 105473, 105474, 105475, 105476, 105477, 105478, 105479, 105480, 105481, 100372, 101008, 101018, 101028, 101038, 101048, 101058, 101068, 101078, 101088, 101098, 101108, 101118, 101118, 101118, 101118, 101118, 101118, 101118, 100373, 101009, 101019, 101029, 101039, 101049, 101059, 101069, 101079, 101089, 101099, 101109, 101119, 105037, 105036, 105034, 105035, 105035, 105035, 105185, 105186, 105187, 105188, 105189, 105190, 105191, 105192, 105193, 105194, 105195, 105196, 105197, 105203, 105202, 105200, 105201, 105198, 105199, 105321, 105322, 105323, 105324, 105325, 105326, 105327, 105327, 105327, 105327, 105327, 105327, 105327, 105331, 105327, 105327, 105330, 105328, 105329, 100371, 105000, 105001, 105002, 105002, 105002, 105002, 105003, 105002, 105002, 105002, 105002, 105002, 105002, 105002, 105002, 105002, 105005, 105004, 105112, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105113, 105114, 102008, 105086, 105086, 105086, 105086, 105086, 105086, 105086, 105086, 105086, 105086, 105087, 105086, 105086, 105086, 105086, 105086, 105086, 105086, 105100, 105101, 105101, 105101, 105101, 105101, 105101, 105102, 105101, 105101, 105101, 105101, 105101, 105101, 105101, 105101, 105101, 105101, 105101, 102018, 105121, 105121, 105121, 105121, 105121, 105121, 105121, 105121, 105121, 105121, 105121, 105121, 105121, 105122, 105121, 105121, 105121, 105121, 102006, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 105080, 102007, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 105083, 102174, 105126, 105126, 105126, 105126, 105126, 105126, 105126, 105126, 105126, 105126, 105126, 105127, 105126, 105126, 105126, 105126, 105126, 105126, 105165, 105166, 105167, 105168, 105169, 105170, 105171, 105172, 105173, 105174, 105175, 105176, 105177, 105183, 105182, 105180, 105181, 105178, 105179, 102077, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105129, 105130, 105131, 105089, 105090, 105097, 105091, 105092, 105093, 105097, 105094, 105097, 105097, 105097, 105095, 105097, 105099, 105098, 105096, 105097, 105097, 105097, 100871, 105416, 105417, 105418, 105419, 105420, 105421, 105422, 105423, 105424, 105425, 105426, 105427, 105427, 105427, 105427, 105427, 105427, 105427, 102091, 105104, 105105, 105106, 105106, 105106, 105107, 105108, 105106, 105109, 105106, 105106, 105106, 105111, 105110, 105106, 105106, 105106, 105106, 102075, 105118, 105118, 105118, 105118, 105118, 105118, 105118, 105118, 105118, 105118, 105118, 105118, 105118, 105118, 105118, 105119, 105117, 105116, 100907, 105394, 105395, 105396, 105397, 105398, 105399, 105400, 105401, 105402, 105403, 105404, 105405, 105405, 105405, 105405, 105405, 105405, 105405, 102137, 105509, 105510, 105511, 105512, 105513, 105514, 105515, 105516, 105517, 105518, 105519, 105520, 105526, 105525, 105523, 105524, 105521, 105522, 102135, 105491, 105492, 105493, 105494, 105495, 105496, 105497, 105498, 105499, 105500, 105501, 105502, 105508, 105507, 105505, 105506, 105503, 105504, 102026, 105040, 105041, 105042, 105043, 105044, 105045, 105046, 105047, 105048, 105049, 105050, 105051, 105057, 105056, 105054, 105055, 105052, 105053, 102024, 105060, 105061, 105062, 105063, 105064, 105065, 105066, 105067, 105068, 105069, 105070, 105071, 105077, 105076, 105074, 105075, 105072, 105073, 102101, 105237, 105238, 105239, 105240, 105241, 105242, 105243, 105244, 105245, 105246, 105247, 105248, 105254, 105253, 105251, 105252, 105249, 105250, 102103, 105256, 105257, 105258, 105259, 105260, 105261, 105262, 105263, 105264, 105265, 105266, 105267, 105273, 105272, 105270, 105271, 105268, 105269, 102105, 105275, 105276, 105277, 105278, 105279, 105280, 105281, 105282, 105283, 105284, 105285, 105286, 105292, 105291, 105289, 105290, 105287, 105288, 102107, 105294, 105295, 105296, 105297, 105298, 105299, 105300, 105301, 105302, 105303, 105304, 105305, 105311, 105310, 105308, 105309, 105306, 105307, 102210, 105155, 105155, 105156, 105155, 105157, 105158, 105155, 105155, 105155, 105155, 105159, 105160, 105164, 105163, 105155, 105155, 105161, 105162, 102020, 105208, 105208, 105208, 105209, 105208, 105208, 105210, 105211, 105212, 105213, 105208, 105208, 105208, 105208, 105214, 105215, 105208, 105208, 102060, 105333, 105334, 105335, 105336, 105337, 105338, 105339, 105340, 105341, 105342, 105343, 105344, 105348, 105347, 105345, 105346, 105346, 105346, 102162, 105446, 105447, 105448, 105449, 105450, 105451, 105452, 105453, 105454, 105455, 105456, 105457, 105463, 105462, 105460, 105461, 105458, 105459, 102168, 105313, 105313, 105313, 105313, 105313, 105313, 105313, 105313, 105313, 105316, 105317, 105318, 105313, 105313, 105319, 105320, 105313, 105313, 105349, 105350, 105350, 105350, 105350, 105350, 105351, 105350, 105352, 105353, 105354, 105350, 105355, 105350, 105350, 105350, 105350, 105356, 105357, 103162, 105536, 105536, 105536, 105537, 105536, 105536, 105536, 105538, 105536, 105539, 105536, 105540, 105544, 105543, 105536, 105536, 105541, 105542, 105217, 105218, 105219, 105220, 105221, 105222, 105223, 105224, 105225, 105226, 105227, 105228, 105229, 105235, 105234, 105232, 105233, 105230, 105231, 102022, 105136, 105137, 105138, 105139, 105140, 105141, 105142, 105143, 105144, 105145, 105146, 105147, 105153, 105152, 105150, 105151, 105148, 105149, 100288, 106500, 106500, 106500, 106500, 106500, 106500, 106501, 106501, 106501, 106501, 106501, 106501, 106501, 106501, 106501, 106501, 106501, 106501, 100287, 106502, 106502, 106502, 106502, 106503, 106503, 106503, 106503, 106504, 106504, 106504, 106504, 106504, 106504, 106504, 106504, 106504, 106504, 100290, 106505, 106505, 106505, 106505, 106506, 106506, 106506, 106506, 106507, 106507, 106507, 106507, 106507, 106507, 106507, 106507, 106507, 106507, 100289, 106508, 106508, 106508, 106508, 106509, 106509, 106509, 106509, 106510, 106510, 106510, 106510, 106510, 106510, 106510, 106510, 106510, 106510, 100394, 106511, 106511, 106511, 106511, 106512, 106512, 106512, 106512, 106513, 106513, 106513, 106513, 106513, 106513, 106513, 106513, 106513, 106513, 101532, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 101979, 100283, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 101310, 100904, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 100444, 102079, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102080, 102057, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102058, 102088, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 103038, 102158, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102159, 102164, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 102165, 103042, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 103043, 102133, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102134, 102112, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102113, 102127, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102128, 102156, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102157, 102204, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102205, 102206, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102207, 102216, 103349, 103350, 103351, 103352, 103353, 103354, 103355, 103356, 103357, 103358, 103359, 103360, 103360, 103360, 103360, 103360, 103360, 103360, 102208, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102209, 102202, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 102203, 103269, 103257, 103258, 103259, 103260, 103261, 103262, 103263, 103264, 103265, 103266, 103267, 103268, 103268, 103268, 103268, 103268, 103268, 103268, 103040, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 103041, 102258, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102260, 102254, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 102255, 103054, 103055, 103056, 103057, 103058, 103059, 103060, 103061, 103062, 103063, 103064, 103065, 103066, 103066, 103066, 103066, 103066, 103066, 103066, 103067, 103068, 103069, 103070, 103071, 103072, 103073, 103074, 103075, 103076, 103077, 103078, 103079, 103079, 103079, 103079, 103079, 103079, 103079, 103080, 103081, 103082, 103083, 103084, 103085, 103086, 103087, 103088, 103089, 103090, 103091, 103092, 103092, 103092, 103092, 103092, 103092, 103092, 103282, 103270, 103271, 103272, 103273, 103274, 103275, 103276, 103277, 103278, 103279, 103280, 103281, 103281, 103281, 103281, 103281, 103281, 103281, 100274, 103100, 103101, 103102, 103103, 103104, 103105, 103106, 103107, 103108, 103109, 103110, 103111, 103111, 103111, 103111, 103111, 103111, 103111, 102199, 102199, 102248, 101424, 102081, 102249, 102250, 102199, 102248, 101424, 102081, 102249, 102250, 102250, 102250, 102250, 102250, 102250, 102250, 102072, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 103037, 102234, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 102235, 100370, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425, 100425,761000,100005, 100010, 100015, 100025, 100030, 100035, 100020, 100055, 100050, 100045, 100040, 100060, 100065, 100070, 100075, 100095, 100085, 100090, 100080, 100100, 100115, 100110, 100105, 100135, 100120, 100125, 100130, 100145, 100140, 100150, 100155, 100165, 100170, 100160, 100175, 100190, 100195, 100185, 100180, 100200, 100210, 100215, 100205, 100230, 100225, 100220, 100235, 120000, 120005, 120010, 120015, 120020, 120025, 120030, 120035, 120040, 120045, 120050, 120055, 120060, 120065, 120070, 120075, 100400, 100405, 100431, 100436,   100329, 100327, 100330, 100328,101004, 101005, 101006, 101007, 101015, 101016, 101017, 101014, 101027, 101026, 101025, 101024, 101034, 101035, 101036, 101037, 101047, 101045, 101046, 101044, 101054, 101057, 101056, 101055, 101067, 101064, 101065, 101066, 101075, 101074, 101076, 101077, 101085, 101086, 101084, 101087, 101096, 101097, 101095, 101094, 101104, 101106, 101107, 101105, 101116, 101115, 101114, 101117, 110235, 110236, 110240, 110200, 110239, 110201, 110237, 110238,110100,100000, 100352, 101001, 100005, 100352, 101002, 100010, 100352, 101003, 100015, 100352, 101000, 100700, 100352, 101000, 100705, 100352, 101010, 100020, 100396, 101011, 100025, 100396, 101012, 100030, 100396, 101013, 100035, 100396, 101010, 100710, 100396, 101010, 100715, 100396, 101020, 100040, 100351, 101021, 100045, 100351, 101022, 100050, 100351, 101023, 100055, 100351, 101020, 100720, 100351, 101020, 100725, 100351, 101030, 100060, 100353, 101031, 100065, 100353, 101032, 100070, 100353, 101033, 100075, 100353, 101030, 100730, 100353, 101030, 100735, 100353, 101040, 100080, 100396, 101041, 100085, 100396, 101042, 100090, 100396, 101043, 100095, 100396, 101040, 100740, 100396, 101040, 100745, 100396, 101050, 100100, 100353, 101051, 100105, 100353, 101052, 100110, 100353, 101053, 100115, 100353, 101050, 100750, 100353, 101050, 100755, 100353, 101060, 100120, 100354, 101061, 100125, 100354, 101062, 100130, 100354, 101063, 100135, 100354, 101060, 100760, 100354, 101060, 100765, 100354, 101070, 100140, 100354, 101071, 100145, 100354, 101072, 100150, 100354, 101073, 100155, 100354, 101070, 100770, 100354, 101070, 100775, 100354, 101080, 100160, 100352, 101081, 100165, 100352, 101082, 100170, 100352, 101083, 100175, 100352, 101080, 100780, 100352, 101080, 100785, 100352, 101090, 100180, 100351, 101091, 100185, 100351, 101092, 100190, 100351, 101093, 100195, 100351, 101090, 100790, 100351, 101090, 100795, 100351, 101100, 100200, 100353, 101101, 100205, 100353, 101102, 100210, 100353, 101103, 100215, 100353, 101100, 100800, 100353, 101100, 100805, 100353, 101110, 100220, 100396, 101111, 100225, 100396, 101112, 100230, 100396, 101113, 100235, 100396, 101110, 100810, 100396, 101110, 100815, 100396,100372, 100373, 101008, 101009, 101018, 101019, 101028, 101029, 101038, 101039, 101048, 101049, 101058, 101059, 101068, 101069, 101078, 101079, 101088, 101089, 101098, 101099, 101108, 101109, 101118, 101119, 101088, 101089, 101098, 101099, 101108, 101109, 101118, 101119, 101108, 101109, 101118, 101119 };
								for (size_t q = 0; q < ridelist_.size(); q++)
								{
									if (pet[i].graNo == ridelist_.at(q))
									{
										petWndFontNo[i + 12] = StockDispBuffer(x - 27 + 35, y - 14 + 23, DISP_PRIO_IME3, IMAGE_RIDE_BTN, 2);
										break;
									}
								}
								sprintf(moji, "%3d     %4d   %4d", pet[i].level, pet[i].hp, pet[i].maxHp);
								StockFontBuffer(x + 26 + 27, y, FONT_PRIO_FRONT, color, moji, 0); y += 27;
								flag = TRUE;
#ifdef _NB_秀宠物
								//秀宠按钮索引[i] = StockDispBuffer(x + 10, y - 20, DISP_PRIO_IME3, IMAGE_秀宠按钮, 2);//关闭按钮
#endif
							}
						}
						if (flag != TRUE) {
							petWndFontNo[10] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, pActMenuWnd->y + 299, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
							StockFontBuffer(x + 10, y, FONT_PRIO_FRONT, 0, "페트가 없습니다.", 0); y += 40;
						}
						else {
							petWndFontNo[10] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx + 56, pActMenuWnd->y + 299, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
							petWndFontNo[11] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx - 56, pActMenuWnd->y + 299, DISP_PRIO_IME3, CG_PET_WND_STATUS_BTN, 2);
						}
					}
				}
			}
			break;
		case 1:
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(4, 4, 320, 408, 0, -1);
				for (i = 0; i < MENU_PET_0; i++) petWndFontNo[i] = -2;
			}
			else {
				if (pActMenuWnd->hp > 0) {
					// 显示宠物详细窗口
					x = pActMenuWnd->x;
					y = pActMenuWnd->y;
					StockDispBuffer绝对位置(x, y, DISP_PRIO_MENU, IMAGE_PET_STATUS, 1);
					if (pActPet == NULL) {
						pActPet = MakeAnimDisp(x + 367, y + 214, pet[petStatusNo].graNo, ANIM_DISP_PET);
					}
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == petWndFontNo[0]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathAction(pActPet);
							pActPet = NULL;
							petWndNo = 0;
							DeathAction(pActMenuWnd3);
							pActMenuWnd3 = NULL;
							GetKeyInputFocus(&MyChatBuffer);
							play_se(203, 320, 240);
						}
						if (HitDispNo == petWndFontNo[4]) {
							petWndNo = 2;
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathAction(pActPet);
							pActPet = NULL;
							petWndNo = 2;
							DeathAction(pActMenuWnd3);
							pActMenuWnd3 = NULL;
							GetKeyInputFocus(&MyChatBuffer);
							play_se(202, 320, 240);
						}
					}
					if (HitDispNo == petWndFontNo[1] || joy_con[0] & JOY_A) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_A) {
							if (pActMenuWnd3 != NULL) {
								DeathAction(pActMenuWnd3);
								pActMenuWnd3 = NULL;
								GetKeyInputFocus(&MyChatBuffer);
								play_se(203, 320, 240);
							}
							while (1) {
								petStatusNo--;
								if (petStatusNo <= -1) petStatusNo = 4;
								if (pet[petStatusNo].useFlag == TRUE) break;
							}
							pActPet->anim_chr_no = pet[petStatusNo].graNo;
							play_se(217, 320, 240);
							petWndBtnFlag[1] = TRUE;
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_A) && petWndBtnFlag[1] == TRUE) {
							petWndBtnFlag[1] = TRUE;
						}
						else {
							petWndBtnFlag[1] = FALSE;
						}
					}
					else {
						petWndBtnFlag[1] = FALSE;
					}
					if (HitDispNo == petWndFontNo[2] || joy_con[0] & JOY_B) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_B) {
							if (pActMenuWnd3 != NULL) {
								DeathAction(pActMenuWnd3);
								pActMenuWnd3 = NULL;
								GetKeyInputFocus(&MyChatBuffer);
								play_se(203, 320, 240);
							}
							while (1) {
								petStatusNo++;
								if (petStatusNo >= 5) petStatusNo = 0;
								if (pet[petStatusNo].useFlag == TRUE) break;
							}

							pActPet->anim_chr_no = pet[petStatusNo].graNo;
							play_se(217, 320, 240);
							petWndBtnFlag[2] = TRUE;
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_B) && petWndBtnFlag[2] == TRUE) {
							petWndBtnFlag[2] = TRUE;
						}
						else {
							petWndBtnFlag[2] = FALSE;
						}
					}
					else {
						petWndBtnFlag[2] = FALSE;
					}
					if (HitDispNo == petWndFontNo[3]) {
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							if (pet[petStatusNo].changeNameFlag == TRUE) {
								if (pActMenuWnd3 == NULL) {
									pActMenuWnd3 = MakeWindowDisp(340, 4 + 280 + 56, 272, 88, 0, -1);
									play_se(202, 320, 240);
									petNameChange.buffer[0] = NULL;
									petNameChange.cnt = 0;
									petNameChange.cursor = 0;
									petNameChange.len = 16;
									petNameChange.color = 0;
									petNameChange.x = pActMenuWnd3->x + 22;
									petNameChange.y = pActMenuWnd3->y + 25;
									petNameChange.fontPrio = FONT_PRIO_FRONT;
								}
								else {
									DeathAction(pActMenuWnd3);
									pActMenuWnd3 = NULL;
									GetKeyInputFocus(&MyChatBuffer);
									play_se(203, 320, 240);
								}
								petWndBtnFlag[3] = TRUE;
							}
							else {
								play_se(220, 320, 240);
							}
						}
						if (mouse.state & MOUSE_LEFT_CRICK && petWndBtnFlag[3] == TRUE) {
							petWndBtnFlag[3] = TRUE;
						}
						else {
							petWndBtnFlag[3] = FALSE;
						}
					}
					else {
						petWndBtnFlag[3] = FALSE;
					}
					if (HitDispNo == petWndFontNo[7]) {
						if (pet[petStatusNo].graNo != 100451 && pet[petStatusNo].graNo != 100432 &&
							pet[petStatusNo].graNo != 101280 && pet[petStatusNo].graNo != 101281 &&
							pet[petStatusNo].graNo != 100015 && pet[petStatusNo].graNo != 100018 &&
							pet[petStatusNo].graNo != 101279 && pet[petStatusNo].graNo != 100401 &&
							pet[petStatusNo].graNo != 101414 && pet[petStatusNo].graNo != 101167 &&
							pet[petStatusNo].graNo != 101172 && pet[petStatusNo].graNo != 102011 &&
							pet[petStatusNo].graNo != 102012)	// fix 哪些宠物不能照宠照
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								int tblNo = GetPettblNo(pet[petStatusNo].graNo);
								int albumNo;
								if (0 <= tblNo && tblNo < MAX_PET_TBL) {
									if (tblNo == 442)
										albumNo = 0;

									albumNo = PetAlbumTbl[tblNo].albumNo;
									if (albumNo != -1) {
										PetAlbum[albumNo].flag = 2;
										if (pet[petStatusNo].freeName[0] != NULL) {
											strcpy(PetAlbum[albumNo].freeName, pet[petStatusNo].freeName);
										}
										else {
											strcpy(PetAlbum[albumNo].freeName, PetAlbumTbl[tblNo].name);
										}
										strcpy(PetAlbum[albumNo].name, PetAlbumTbl[tblNo].name);
										PetAlbum[albumNo].faceGraNo = PetAlbumTbl[tblNo].faceGraNo;
										PetAlbum[albumNo].level = pet[petStatusNo].level;
										PetAlbum[albumNo].maxHp = pet[petStatusNo].maxHp;
										PetAlbum[albumNo].str = pet[petStatusNo].atk;
										PetAlbum[albumNo].quick = pet[petStatusNo].quick;
										PetAlbum[albumNo].def = pet[petStatusNo].def;
										PetAlbum[albumNo].earth = pet[petStatusNo].earth;
										PetAlbum[albumNo].water = pet[petStatusNo].water;
										PetAlbum[albumNo].fire = pet[petStatusNo].fire;
										PetAlbum[albumNo].wind = pet[petStatusNo].wind;
										play_se(201, 320, 240);
										sprintf(moji, "%s 촬영！", PetAlbum[albumNo].freeName);
										StockChatBufferLine(moji, FONT_PAL_WHITE);
										petWndBtnFlag[7] = TRUE;
										SaveAlbum(albumNo);

									}
									else {
										play_se(220, 320, 240);
#ifdef _STONDEBUG_		
										sprintf(moji, "숫자가 이상해 %d", pet[petStatusNo].graNo);
										MessageBox(hWnd, moji, "확인", MB_OK | MB_ICONSTOP);
#endif
									}
								}
								else {
									play_se(220, 320, 240);
#ifdef _STONDEBUG_		
									sprintf(moji, "페트table숫자가 이상합니다 %d", tblNo);
									MessageBox(hWnd, moji, "확인", MB_OK | MB_ICONSTOP);
#endif
								}
							}
						if (mouse.state & MOUSE_LEFT_CRICK && petWndBtnFlag[7] == TRUE) {
							petWndBtnFlag[7] = TRUE;
						}
						else {
							petWndBtnFlag[7] = FALSE;
						}
					}
					else {
						petWndBtnFlag[7] = FALSE;
					}
					if (pActMenuWnd != NULL) {

#define PET_WND_ATTR_X 159
#define PET_WND_ATTR_Y 5

						int x2;
						if (pet[petStatusNo].freeName[0] != NULL) {
							//CenteringStr( pet[ petStatusNo ].name, moji, PET_NAME_LEN );
							sprintf(moji, "%s %-16s", (pet[petStatusNo].petlock ? "[LOCK]" : ""), pet[petStatusNo].freeName);
						}
						else {
							//CenteringStr( pet[ petStatusNo ].name, moji, PET_NAME_LEN );
							sprintf(moji, "%s %-16s", (pet[petStatusNo].petlock ? "[LOCK]" : ""), pet[petStatusNo].name);
						}

						StockFontBuffer(x + 93, y + 29, FONT_PRIO_FRONT, 0, moji, 0);//打印名字

						petWndFontNo[3] = StockDispBuffer绝对位置(x + 198, y + 29, DISP_PRIO_IME3, CG_NAME_CHANGE_BTN + petWndBtnFlag[3], 2);
						
						sprintf(moji, "%3d", pet[petStatusNo].level);
						StockFontBuffer(x + 71, y + 59, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%3d", 5-pet[petStatusNo].reset);
						StockFontBuffer(x + 164, y + 59, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%3d", pet[petStatusNo].ai);
						StockFontBuffer(x + 253, y + 59, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%d/%d  (%.2f%%)", pet[petStatusNo].exp, pet[petStatusNo].maxExp, (float)pet[petStatusNo].exp / pet[petStatusNo].maxExp * 100.0f);
						StockFontBuffer(x + 82, y + 83, FONT_PRIO_FRONT, 0, moji, 0);

						Ranking r(pet[petStatusNo]);
						int imoji;

						sprintf(moji, "%4d / %4d", pet[petStatusNo].hp, pet[petStatusNo].maxHp);
						StockFontBuffer(x + 82 , y + 108, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%.3f", r.h);
						StockFontBuffer(x + 194, y + 108, FONT_PRIO_FRONT, 0, moji, 0);
						imoji = pet[petStatusNo].maxHp - r.svit;
						sprintf(moji, "%s%d", imoji >= 0 ? " +" : " ", imoji);
						StockFontBuffer(x + 253, y + 108, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%4d", pet[petStatusNo].atk);
						StockFontBuffer(x + 100, y + 132, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%.3f", r.s);
						StockFontBuffer(x + 194, y + 132, FONT_PRIO_FRONT, 0, moji, 0);
						imoji = pet[petStatusNo].atk - r.sstr;
						sprintf(moji, "%s%d", imoji >= 0 ? " +" : " ", imoji);
						StockFontBuffer(x + 253, y + 132, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%4d", pet[petStatusNo].def);
						StockFontBuffer(x + 100, y + 156, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%.3f", r.t);
						StockFontBuffer(x + 194, y + 156, FONT_PRIO_FRONT, 0, moji, 0);
						imoji = pet[petStatusNo].def - r.stgh;
						sprintf(moji, "%s%d", imoji >= 0 ? " +" : " ", imoji);
						StockFontBuffer(x + 253, y + 156, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%4d", pet[petStatusNo].quick);
						StockFontBuffer(x + 100, y + 180, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%.3f", r.d);
						StockFontBuffer(x + 194, y + 180, FONT_PRIO_FRONT, 0, moji, 0);
						imoji = pet[petStatusNo].quick - r.sdex;
						sprintf(moji, "%s%d", imoji >= 0 ? " +" : " ", imoji);
						StockFontBuffer(x + 253, y + 180, FONT_PRIO_FRONT, 0, moji, 0);

						sprintf(moji, "%3d%%", pet[petStatusNo].hit_rate);
						StockFontBuffer(x + 82 , y + 204, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%3d%%", pet[petStatusNo].crit_rate);
						StockFontBuffer(x + 165, y + 204, FONT_PRIO_FRONT, 0, moji, 0);
						sprintf(moji, "%3d%%", pet[petStatusNo].avoid_rate);
						StockFontBuffer(x + 249, y + 204, FONT_PRIO_FRONT, 0, moji, 0);

						if (pet[petStatusNo].earth > 0) {
							x2 = (int)(pet[petStatusNo].earth * 0.8);
							StockBoxDispBuffer(x + 80 + 2 + 0, y + 231 + 0, x + 80 + 2 + 0 + x2, y + 231 + 8, DISP_PRIO_IME4, SYSTEM_PAL_GREEN, 1);
							StockBoxDispBuffer(x + 80 + 2 + 1, y + 231 + 1, x + 80 + 2 + 1 + x2, y + 231 + 9, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0);
							StockBoxDispBuffer(x + 80 + 2 + 2, y + 231 + 2, x + 80 + 2 + 2 + x2, y + 231 + 10, DISP_PRIO_IME3, SYSTEM_PAL_GREEN2, 0);
						}

						if (pet[petStatusNo].fire > 0) {
							x2 = (int)(pet[petStatusNo].fire * 0.8);
							StockBoxDispBuffer(x + 212 + 2 + 0, y + 231 + 0, x + 212 + 2 + 0 + x2, y + 231 + 8, DISP_PRIO_IME4, SYSTEM_PAL_RED, 1);
							StockBoxDispBuffer(x + 212 + 2 + 1, y + 231 + 1, x + 212 + 2 + 1 + x2, y + 231 + 9, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0);
							StockBoxDispBuffer(x + 212 + 2 + 2, y + 231 + 2, x + 212 + 2 + 2 + x2, y + 231 + 10, DISP_PRIO_IME3, SYSTEM_PAL_RED2, 0);
						}

						if (pet[petStatusNo].water > 0) {
							x2 = (int)(pet[petStatusNo].water * 0.8);
							StockBoxDispBuffer(x + 80 + 2 + 0, y + 255 + 0, x + 80 + 2 + 0 + x2, y + 255 + 8, DISP_PRIO_IME4, SYSTEM_PAL_AQUA, 1);
							StockBoxDispBuffer(x + 80 + 2 + 1, y + 255 + 1, x + 80 + 2 + 1 + x2, y + 255 + 9, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0);
							StockBoxDispBuffer(x + 80 + 2 + 2, y + 255 + 2, x + 80 + 2 + 2 + x2, y + 255 + 10, DISP_PRIO_IME3, SYSTEM_PAL_AQUA2, 0);
						}

						if (pet[petStatusNo].wind > 0) {
							x2 = (int)(pet[petStatusNo].wind * 0.8);
							StockBoxDispBuffer(x + 212 + 2 + 0, y + 255 + 0, x + 212 + 2 + 0 + x2, y + 255 + 8, DISP_PRIO_IME4, SYSTEM_PAL_YELLOW, 1);
							StockBoxDispBuffer(x + 212 + 2 + 1, y + 255 + 1, x + 212 + 2 + 1 + x2, y + 255 + 9, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0);
							StockBoxDispBuffer(x + 212 + 2 + 2, y + 255 + 2, x + 212 + 2 + 2 + x2, y + 255 + 10, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW2, 0);
						}

						petWndFontNo[4] = StockDispBuffer(x + 286, y + 298, DISP_PRIO_IME3, CG_PET_WND_WAZA_BTN, 2);
						petWndFontNo[0] = StockDispBuffer(x + 380, y + 298, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
						petWndFontNo[1] = StockDispBuffer(x + 71 , y + 298, DISP_PRIO_IME3, CG_PREV_BTN + petWndBtnFlag[1], 2);
						petWndFontNo[2] = StockDispBuffer(x + 116, y + 298, DISP_PRIO_IME3, CG_NEXT_BTN + petWndBtnFlag[2], 2);
					}
				}
			}

			if (pActMenuWnd3 != NULL) {
				if (pActMenuWnd3->hp > 0) {
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd3->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd3->pYobi)->my, DISP_PRIO_MENU, CG_NAME_CHANGE_WND, 1);
					GetKeyInputFocus(&petNameChange);
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == petWndFontNo[5]) {
							KeyboardReturn();
						}
						if (HitDispNo == petWndFontNo[6]) {
							DeathAction(pActMenuWnd3);
							pActMenuWnd3 = NULL;
							GetKeyInputFocus(&MyChatBuffer);
							play_se(203, 320, 240);
						}
					}
					if (pActMenuWnd3 != NULL) {
						StockFontBuffer2(&petNameChange);
						x = pActMenuWnd3->x + 20;
						y = pActMenuWnd3->y + 60;
						petWndFontNo[5] = StockDispBuffer(x + 60, y + 7, DISP_PRIO_IME3, CG_OK_BTN, 2);
						petWndFontNo[6] = StockDispBuffer(x + 170, y + 7, DISP_PRIO_IME3, CG_CANCEL_BTN, 2);
					}
				}
			}
			break;

		case 2:
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(4, 4, 272, 348, 0, -1);
				for (i = 0; i < MENU_PET_0; i++) petWndFontNo[i] = -2;
			}
			else {
				if (pActMenuWnd->hp > 0) {
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_PET_WAZA_WND, 1);
					for (i = 0; i < pet[petStatusNo].maxSkill; i++) {
						StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_IME3, CG_PET_WAZA_BAR_1 + i, 1);
					}
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == petWndFontNo[7]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							petWndNo = 1;
							play_se(203, 320, 240);
						}
					}
					if (HitDispNo == petWndFontNo[8] || joy_con[0] & JOY_A) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_A) {
							while (1) {
								petStatusNo--;
								if (petStatusNo <= -1) petStatusNo = 4;
								if (pet[petStatusNo].useFlag == TRUE) break;
							}
							for (i = 0; i < MENU_PET_0; i++) petWndFontNo[i] = -2;
							play_se(217, 320, 240);
							petWndBtnFlag[8] = TRUE;
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_A) && petWndBtnFlag[8] == TRUE) {
							petWndBtnFlag[8] = TRUE;
						}
						else {
							petWndBtnFlag[8] = FALSE;
						}
					}
					else {
						petWndBtnFlag[8] = FALSE;
					}
					if (HitDispNo == petWndFontNo[9] || joy_con[0] & JOY_B) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_B) {
							while (1) {
								petStatusNo++;
								if (petStatusNo >= 5) petStatusNo = 0;
								if (pet[petStatusNo].useFlag == TRUE) break;
							}
							for (i = 0; i < MENU_PET_0; i++) petWndFontNo[i] = -2;
							play_se(217, 320, 240);
							petWndBtnFlag[9] = TRUE;
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_B) && petWndBtnFlag[9] == TRUE) {
							petWndBtnFlag[9] = TRUE;
						}
						else {
							petWndBtnFlag[9] = FALSE;
						}
					}
					else {
						petWndBtnFlag[9] = FALSE;
					}
					if (pActMenuWnd != NULL) {
						x = pActMenuWnd->x + 32;
						y = pActMenuWnd->y + 252;
						for (i = 0; i < pet[petStatusNo].maxSkill; i++) {
							if (petSkill[petStatusNo][i].useFlag == TRUE) {
								if (HitFontNo == petWndFontNo[i]) {
									char *splitPoint = petSkill[petStatusNo][i].memo;
									while (1) {
										if (strlen(splitPoint) > 24) {
											strncpy(moji, splitPoint, 24);
											moji[24] = NULL;
											if (GetStrLastByte(moji) == 3) {
												moji[23] = NULL;
												splitPoint += 23;
											}
											else {
												moji[24] = NULL;
												splitPoint += 24;
											}
											StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 0); y += 20;
										}
										else {
											strcpy(moji, splitPoint);
											StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 0);
											break;
										}
									}
#ifdef _STONDEBUG_						
									if (mouse.onceState & MOUSE_RIGHT_CRICK) {
										if (petSkill[petStatusNo][i].field != PETSKILL_FIELD_BATTLE) {
											int j;
											int cnt = 0;
											switch (petSkill[petStatusNo][i].skillId) {

											case PETSKILL_MERGE:
												for (j = MAX_ITEMSTART; j < MAX_ITEMSTART; j++) {
													if (ItemBuffer[j].mixFlag == 1) cnt++;
												}
												break;

											case PETSKILL_MERGE2:
												for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
													if (ItemBuffer[j].mixFlag == 2) cnt++;
												}
												break;
											}

											if (cnt >= 2) {
												moji[0] = NULL;
												for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
													if (ItemBuffer[j].mixFlag >= TRUE) {
														char work[256];
														sprintf(work, "%d|", j);
														strcat(moji, work);
														ItemBuffer[j].mixFlag = FALSE;
													}
												}
												moji[strlen(moji) - 1] = NULL;
												if (bNewServer)
													lssproto_PS_send(sockfd, mixPetNo, i, 0, moji);
												else
													old_lssproto_PS_send(sockfd, mixPetNo, i, 0, moji);
												play_se(212, 320, 240);
											}
											else {
												play_se(220, 320, 240);
											}
										}
										else {
											play_se(220, 320, 240);
										}
									}
#endif									
									if (mouse.onceState & MOUSE_LEFT_CRICK) {
										if (petSkill[petStatusNo][i].field != PETSKILL_FIELD_BATTLE && pActPet2 == NULL
											/* && MenuToggleFlag & JOY_CTRL_I*/) {
											int j;
											int cnt = 0;
											switch (petSkill[petStatusNo][i].skillId) {

											case PETSKILL_MERGE:
												for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
													if (ItemBuffer[j].mixFlag == 1) cnt++;
												}
												break;

											case PETSKILL_MERGE2:
												for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
													if (ItemBuffer[j].mixFlag == 2) cnt++;
												}
												break;
											case PETSKILL_INSLAY:
												for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
													if (ItemBuffer[j].mixFlag == 10) cnt++;
												}
												break;
											case PETSKILL_FIXITEM:
											case PETSKILL_FIXITEM2:
												for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
													if (ItemBuffer[j].mixFlag == 11) cnt++;
												}
												break;
#ifdef _ITEM_INTENSIFY
											case PETSKILL_INTENSIFY:
												for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
													if (ItemBuffer[j].mixFlag == 12) cnt++;
												}
												break;
#endif

#ifdef _ALCHEMIST
											case PETSKILL_ALCHEMIST:
												for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
													if (ItemBuffer[j].mixFlag == 9) cnt++;
												}
												break;
#endif
											}
											if (cnt >= 2) {
												if (pActPet2 == NULL) {
													pActPet2 = MakeAnimDisp(750, 240, pet[petStatusNo].graNo, ANIM_DISP_MIX_PET_INIT);
													ItemMixPetNo = petStatusNo;
												}
												SelectWazaNo = i;
												mixPetNo = petStatusNo;
												play_se(212, 320, 240);
											}
											else {
												if (!(MenuToggleFlag & JOY_CTRL_I) || (MenuToggleFlag & JOY_CTRL_I && itemWndNo != 0)) {
													int j;
													MenuToggleFlag |= JOY_CTRL_I;	// CTRL + I
													for (j = 0; j < MENU_ITEM_0; j++) itemWndFontNo[j] = -2;
													for (j = 0; j < MENU_ITEM_0; j++) itemWndBtnFlag[j] = 0;
													itemWndNo = 0;
													mouse.itemNo = -1;
													itemWndDropGold = 0;
													if (BattleResultWndFlag >= 1) DeathMenuAction();
													DeathMenuAction2();
													saveUserSetting();
													MenuToggleFlag &= ~JOY_CTRL_M;
													BattleResultWndFlag = FALSE;
													MapWmdFlagBak = FALSE;
													play_se(202, 320, 240);
												}
												else
													play_se(220, 320, 240);
											}
										}
										else {
											play_se(220, 320, 240);
										}
									}
								}
							}
						}

						x = pActMenuWnd->x + 40;
						y = pActMenuWnd->y + 32;
						if (pet[petStatusNo].freeName[0] != NULL)
							CenteringStr(pet[petStatusNo].freeName, moji, PET_NAME_LEN);
						else
							CenteringStr(pet[petStatusNo].name, moji, PET_NAME_LEN);
						StockFontBuffer(x - 28, y, FONT_PRIO_FRONT, 0, moji, 0); y += 26; x += 18;
						for (i = 0; i < pet[petStatusNo].maxSkill; i++) {
							if (petSkill[petStatusNo][i].useFlag == TRUE) {
								int color = FONT_PAL_GRAY;
								sprintf(moji, "       %-22s", petSkill[petStatusNo][i].name);
								if (petSkill[petStatusNo][i].field != PETSKILL_FIELD_BATTLE) color = FONT_PAL_WHITE;
								petWndFontNo[i] = StockFontBuffer(x - 43, y, FONT_PRIO_FRONT, color, moji, 2);
							}
							y += 25;
						}
						x = pActMenuWnd->x + 20;
						y = pActMenuWnd->y + 330;
						petWndFontNo[7] = StockDispBuffer(x + 100 + 70, y, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
						petWndFontNo[8] = StockDispBuffer(x + 16 + 16 + 8, y, DISP_PRIO_IME3, CG_PREV_BTN + petWndBtnFlag[8], 2);
						petWndFontNo[9] = StockDispBuffer(x + 50 + 16 + 18, y, DISP_PRIO_IME3, CG_NEXT_BTN + petWndBtnFlag[9], 2);
					}
				}
			}
			break;
#ifdef _STREET_VENDOR
		case 3: petWndNo = 0; break;
#endif
		}

#endif
	}

	//? ??????????? *******************************************************/
	if (MenuToggleFlag & JOY_CTRL_I) {
		int 临时MAXX, 临时MAXY, 中心x;
		//计算图片中心
		if (ResoMode == 分辨率::mode800) {
			临时MAXX = 800;
			中心x = 800 / 2;
		}
		else if (ResoMode == 分辨率::mode1024) {
			临时MAXX = 1024;
			中心x = 1024 / 2;
		}
		else {
			临时MAXX = 640;
			中心x = 640 / 2;
		}

		int x1 = 0, y1 = 0;
#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面
		SkillWndflag = false;
#endif
		switch (itemWndNo) {

		case 0:
			if (pActMenuWnd2 == NULL) {
#ifndef _PET_ITEM
				pActMenuWnd2 = MakeWindowDisp(中心x - 292, 4, 292, 440, 0, -1);
				InitItem(pActMenuWnd2->x, pActMenuWnd2->y, 0);
#else


				pActMenuWnd2 = MakeWindowDisp(临时MAXX - 292, 4, 292, 440, 0, -1);
				pActMenuWnd2->x = 临时MAXX - 285;
				InitItem(pActMenuWnd2->x, -3, g_bPetItemWndFlag);



#endif
				itemNo = -1;
				ItemMixPetNo = -1;
				itemWndBagOffset = 0;
				if (MenuToggleFlag & JOY_CTRL_E && mailWndNo == MAIL_WND_ITEM) {
					MenuToggleFlag &= ~JOY_CTRL_E;
					DeathMenuAction();
				}
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
				MymoneyBuffer.buffer[0] = NULL;
				MymoneyBuffer.cnt = 0;
				MymoneyBuffer.cursor = 0;
				MymoneyBuffer.len = 10;
				MymoneyBuffer.color = 0;
				MymoneyBuffer.x = pActMenuWnd2->x + 191;
				MymoneyBuffer.y = pActMenuWnd2->y + 180;
				MymoneyBuffer.fontPrio = FONT_PRIO_FRONT;
				x1 = pActMenuWnd2->x + 173;
				y1 = pActMenuWnd2->y + 133;
#endif
				for (i = 0; i < 3; i++) itemWndBtnBag[i] = -2;
#ifdef _PET_ITEM
				nSelectPet = -1;
				for (i = 0; i < MAX_PET; i++) {
					if (pet[i].useFlag) {
						nSelectPet = i;
						break;
					}
				}
#endif
			}
			else {
				//andy_log 装备栏位修改处[장비슬롯수정]
				if (pActMenuWnd2->hp > 0) {
#ifdef _PET_ITEM
					// 소품창의 왼쪽에 있는 탭이 눌려있는지확인		[황금자루?]
					x1 = pActMenuWnd2->x - 21;
					y1 = pActMenuWnd2->y + 12;
#ifdef _NB_新道具栏界面


					StockDispBuffer(临时MAXX - 160, ((WINDOW_DISP*)pActMenuWnd2->pYobi)->my+5, DISP_PRIO_MENU, 26455, 1);


#else
					if (g_bPetItemWndFlag)
					{
						if (MakeHitBox(x1, y1, x1 + 23, y1 + 60, DISP_PRIO_IME3) == TRUE)	// 按下了人物装备[캐릭터 장비눌림]
							if (mouse.onceState & MOUSE_LEFT_CRICK)
							{
								g_bPetItemWndFlag = false;
								InitItem(pActMenuWnd2->x, pActMenuWnd2->y, g_bPetItemWndFlag);
							}
					}
					else {
						if (MakeHitBox(x1, y1 + 78, x1 + 23, y1 + 142, DISP_PRIO_IME3) == TRUE)	// 按下了宠物装备[눌러 진 애완동물장비]
							if (mouse.onceState & MOUSE_LEFT_CRICK)
							{
								g_bPetItemWndFlag = true;
								InitItem(pActMenuWnd2->x, pActMenuWnd2->y, g_bPetItemWndFlag);
							}
					}


					if (g_bPetItemWndFlag)
						StockDispBuffer(((WINDOW_DISP*)pActMenuWnd2->pYobi)->mx - 14, ((WINDOW_DISP*)pActMenuWnd2->pYobi)->my, DISP_PRIO_MENU, 26456, 1);
					else
						StockDispBuffer(((WINDOW_DISP*)pActMenuWnd2->pYobi)->mx - 14, ((WINDOW_DISP*)pActMenuWnd2->pYobi)->my, DISP_PRIO_MENU, CG_NEWITEM_WND, 1);
#endif
#else
#ifdef _ITEM_EQUITSPACE
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd2->pYobi)->my, DISP_PRIO_MENU, CG_NEWITEM_WND, 1);
#else
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd2->pYobi)->my, DISP_PRIO_MENU, CG_ITEM_WND_0, 1);

					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd2->pYobi)->my, DISP_PRIO_MENU, CG_ITEM_WND_1, 1);
#endif
#endif
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
#ifdef _PET_ITEM
					if (!g_bPetItemWndFlag)	// 캐릭터 장비 바에는 다음 항목만 표시하면 됩니다.
#endif
					{
						x1 = 临时MAXX - 102;
						y1 = 133;
						if (MakeHitBox(x1, y1, x1 + 86, y1 + 25, DISP_PRIO_IME3) == TRUE) {
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								strcpy(MymoneyBuffer.buffer, "");
								MymoneyBuffer.buffer[0] = NULL;
								MymoneyBuffer.cursor = 0;
								MymoneyBuffer.cnt = 0;
								Moneyflag = true;
								GetKeyInputFocus(&MymoneyBuffer);
								play_se(217, 320, 240);
							}
						}
					}
					if (Moneyflag == true) {
						MymoneyBuffer.x = 临时MAXX - 32 - strlen(MymoneyBuffer.buffer) * 7;
						StockFontBuffer2(&MymoneyBuffer);
						itemWndDropGold = atoi(MymoneyBuffer.buffer);
						if (atoi(MymoneyBuffer.buffer) >= pc.gold) {
							sprintf(MymoneyBuffer.buffer, "%d", pc.gold);
							itemWndDropGold = pc.gold;
						}
					}
#endif
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
#ifdef _NB_道具自动叠加
						if (HitDispNo == 道具叠加按钮) {
							char 临时提示[128];
							sprintf(临时提示, "아이템 자동 겹치기。");
							ShowBottomLineString(FONT_PAL_WHITE, 临时提示);
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								//chatStrSendForServer("/겹치기", 0);
								lssproto_SaMenu_send(sockfd,1,7,0);
							}
							play_se(203, 320, 240);
						}
#endif
						for (i = 0; i < 3; i++)
							if (HitDispNo == itemWndBtnBag[i])
							{
								itemWndBagOffset = i;
								break;
							}

						// 按下关闭钮
						if (HitDispNo == itemWndFontNo[0]) {
							DeathAction(pActMenuWnd2);
							pActMenuWnd2 = NULL;
							DeathAction(pActPet2);
							pActPet2 = NULL;
							ItemMixPetNo = -1;
							MenuToggleFlag ^= JOY_CTRL_I;
							if (MenuToggleFlag & JOY_CTRL_S) {
								MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S
								DeathMenuAction();
							}
							play_se(203, 320, 240);
							itemNo = -1;
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
							GetKeyInputFocus(&MyChatBuffer);
							Moneyflag = false;
#endif
						}

						// 按了咒术
						if (HitDispNo == itemWndFontNo[1]) {
							itemWndNo = 1;
							DeathAction(pActMenuWnd2);
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
							GetKeyInputFocus(&MyChatBuffer);
							Moneyflag = false;
#endif
							pActMenuWnd2 = NULL;
							DeathAction(pActPet2);
							pActPet2 = NULL;
							ItemMixPetNo = -1;
							play_se(202, 300, 300);
							if (!(MenuToggleFlag & JOY_CTRL_S)) {
								MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S ??
								for (i = 0; i < MENU_STATUS_0; i++) statusWndFontNo[i] = -2;
								for (i = 0; i < MENU_STATUS_0; i++) statusWndBtnFlag[i] = 0;
								statusWndNo = 1;
								DeathMenuAction();
								saveUserSetting();
								MenuToggleFlag &= JOY_CTRL_I;
								MenuToggleFlag |= JOY_CTRL_S;
							}
							else {
								if (statusWndNo != 1) {
									DeathAction(pActMenuWnd);
									pActMenuWnd = NULL;
									statusWndNo = 1;
								}
							}
						}
						if (itemNo != -1 && MenuToggleFlag & JOY_CTRL_S && statusWndNo == 1) {
							for (i = 0; i < 11; i++) {
								if (HitFontNo == statusWndFontNo[i] && eventWarpSendFlag == FALSE) {
									if (bNewServer)
										lssproto_ID_send(sockfd, nowGx, nowGy, itemNo, i);
									else
										old_lssproto_ID_send(sockfd, nowGx, nowGy, itemNo, i);
									play_se(212, 320, 240);
									itemNo = -1;

								}
							}
						}
#ifdef _PET_ITEM
						// 目前处在宠物装备视窗
						if (g_bPetItemWndFlag) {
							// 按下了左箭头
							if (HitDispNo == itemWndFontNo[5]) {
								if (nSelectPet != -1) {
									do {
										nSelectPet--;
										//CC_FIX 
										//20170807
										//不论是不是骑宠，都显示宠装
										//if (nSelectPet == pc.ridePetNo)
										//	nSelectPet--;
										if (nSelectPet < 0)
											nSelectPet = MAX_PET - 1;
									} while (!pet[nSelectPet].useFlag);
								}
							}
							// 按下了右箭头
							if (HitDispNo == itemWndFontNo[6]) {
								if (nSelectPet != -1) {
									do {
										nSelectPet++;
										//CC_FIX 
										//20170807
										//不论是不是骑宠，都显示宠装
										//if (nSelectPet == pc.ridePetNo)
										//	nSelectPet++;
										if (nSelectPet >= MAX_PET)
											nSelectPet = 0;
									} while (!pet[nSelectPet].useFlag);
								}
							}
						}
#endif
					}

					if (HitDispNo == itemWndFontNo[2]) {
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							if (itemWndDropGold > 0 && eventWarpSendFlag == FALSE) {
								itemWndBtnFlag[2] = TRUE;
								play_se(217, 320, 240);
							}
							else {
								play_se(220, 320, 240);
							}
						}
						if (mouse.onceState & MOUSE_LEFT_CRICK_UP && itemWndBtnFlag[2] == TRUE) {
							itemWndBtnFlag[2] = FALSE;
							play_se(212, 320, 240);
							if (bNewServer)
								lssproto_DG_send(sockfd, nowGx, nowGy, itemWndDropGold);
							else
								old_lssproto_DG_send(sockfd, nowGx, nowGy, itemWndDropGold);
							itemWndDropGold = 0;
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
							Moneyflag = false;
#endif

						}
					}
					else
						itemWndBtnFlag[2] = FALSE;

					if (HitDispNo == itemWndFontNo[3]) {
						if (mouse.onceState & MOUSE_LEFT_CRICK_UP && itemWndBtnFlag[3] == TRUE) {
							itemWndBtnFlag[3] = FALSE;
						}
						if (itemWndBtnFlag[3] == TRUE) {
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
							GetKeyInputFocus(&MyChatBuffer);
							Moneyflag = false;
#endif
							itemWndDropGold += itemWndDropGoldInc;
							itemWndDropGoldCnt++;
							if (itemWndDropGoldCnt >= 30) {
								itemWndDropGoldCnt = 0;
								if (itemWndDropGoldInc == 0) itemWndDropGoldInc = 1;
								else {
									itemWndDropGoldInc *= 5;
									if (itemWndDropGoldInc > 10000) itemWndDropGoldInc = 10000;
								}
							}
							if (itemWndDropGold >= pc.gold) {
								itemWndDropGold = pc.gold;
								play_se(220, 320, 240);
							}
						}
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							itemWndDropGold++;
							if (itemWndDropGold >= pc.gold) {
								itemWndDropGold = pc.gold;
								play_se(220, 320, 240);
							}
							else {
								itemWndDropGoldInc = 0;
								itemWndDropGoldCnt = 0;
								itemWndBtnFlag[3] = TRUE;
								play_se(217, 320, 240);
							}
						}

					}
					else
						itemWndBtnFlag[3] = FALSE;

					if (HitDispNo == itemWndFontNo[4]) {
						if (mouse.onceState & MOUSE_LEFT_CRICK_UP && itemWndBtnFlag[4] == TRUE) {
							itemWndBtnFlag[4] = FALSE;
						}
						if (itemWndBtnFlag[4] == TRUE) {
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
							GetKeyInputFocus(&MyChatBuffer);
							Moneyflag = false;
#endif
							itemWndDropGold -= itemWndDropGoldInc;
							itemWndDropGoldCnt++;
							if (itemWndDropGoldCnt >= 30) {
								itemWndDropGoldCnt = 0;
								if (itemWndDropGoldInc == 0) itemWndDropGoldInc = 1;
								else {
									itemWndDropGoldInc *= 5;
									if (itemWndDropGoldInc > 10000) itemWndDropGoldInc = 10000;
								}
							}
							if (itemWndDropGold < 0) {
								itemWndDropGold = 0;
								play_se(220, 320, 240);
							}
						}
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							itemWndDropGold--;
							if (itemWndDropGold <= 0) {
								itemWndDropGold = 0;
								play_se(220, 320, 240);
							}
							else {
								itemWndDropGoldInc = 0;
								itemWndDropGoldCnt = 0;
								itemWndBtnFlag[4] = TRUE;
								play_se(217, 320, 240);

							}
						}
					}
					else
						itemWndBtnFlag[4] = FALSE;

					if (mouse.itemNo != -1)
						for (i = 0; i < 3; i++)
							if (HitDispNo == itemWndBtnBag[i])
							{
								if (i <= itemWndMaxBag)
								{
									itemWndBagOffset = i;
									break;
								}
							}

					for (i = 0; i < 3; i++)
						itemWndBtnBag[i] = -2;

					if (pActMenuWnd2 != NULL) {
						x = pActMenuWnd2->x + 16;
						y = pActMenuWnd2->y + 351 + 7;
						if (itemNo != -1) {
							StockBoxDispBuffer(ItemBuffer[itemNo].defX - 24, ItemBuffer[itemNo].defY - 24,
								ItemBuffer[itemNo].defX + 26, ItemBuffer[itemNo].defY + 23,
								DISP_PRIO_IME4, SYSTEM_PAL_AQUA, 0);
						}

						for (i = 0; i < 3; i++)
						{
							if (i == itemWndBagOffset)
								itemWndBtnBag[i] = StockDispBuffer绝对位置(492, 160 + i * 54, DISP_PRIO_IME4, 66715 + i, 2);
							else if (i <= itemWndMaxBag)
								itemWndBtnBag[i] = StockDispBuffer绝对位置(499, 160 + i * 54, DISP_PRIO_IME4, 55110 + i, 2);
							else
								StockDispBuffer绝对位置(499, 160 + i * 54, DISP_PRIO_IME4, 55107 + i, TRUE);
						}
						int _Start = MAX_ITEMSTART + itemWndBagOffset * MAX_MAXHAVEITEM;

						for (i = MAX_ITEM - 1; i >= 0; i--) {
							if (!(_Start <= i && i < _Start + MAX_MAXHAVEITEM 
										|| 0 <= i && i < MAX_ITEMSTART)) continue;

#ifdef _PET_ITEM
							// 当显示宠物道具栏时,宠物身上的装备只显示七个,多的就跳过不处理
							if (g_bPetItemWndFlag && (i >= PET_EQUIPNUM && i < MAX_ITEMSTART))
								continue;
#endif

							if (!g_bPetItemWndFlag && i >= CHAR_EQUIPPLACENUM  && i < MAX_ITEMSTART) { // 아이템 장비칸
								continue;
							}

#ifdef _NB_新道具栏界面
							if (MakeHitBox(ItemBuffer[i].defX - 24, ItemBuffer[i].defY - 24,
								ItemBuffer[i].defX + 26, ItemBuffer[i].defY + 23, DISP_PRIO_IME3) == TRUE) {
#else
							if (MakeHitBox(ItemBuffer[i].defX - 24, ItemBuffer[i].defY - 24,
								ItemBuffer[i].defX + 26, ItemBuffer[i].defY + 23, DISP_PRIO_IME3) == TRUE) {
#endif
#ifdef _PET_ITEM
								// 处理显示宠物装备[페트장비자리 필요없는기능]
								if (g_bPetItemWndFlag && (i >= PET_HEAD && i < PET_EQUIPNUM) && nSelectPet > -1) {
									if (pet[nSelectPet].item[i].useFlag == TRUE && (ItemBuffer[i].mixFlag <= 2 || ItemBuffer[i].mixFlag == 10)) {
										char *splitPoint = pet[nSelectPet].item[i].memo;
										char damage_msg[256];
										int color = pet[nSelectPet].item[i].color;
#ifdef _NB_道具栏详情
										char 道具类型说明[][56] = { "爪","斧头","棍棒","枪","弓","盾牌","头盔/帽子","衣服/铠甲","手环","乐器","项链",
											"未知","未知","耳环","鼻环","护身符","普通物品","回旋标","投掷斧头","投掷石",
											"一般物品","金属","宝石","货物","腰带","盾","鞋子","手套","未知","未知","炼金",
											"宠物头套","宠物牙套","宠物爪套","宠物胸甲","宠物背甲","宠物翅膀","宠物脚"
										};
										char 详情临时文本[256];
										StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 250, FONT_PRIO_FRONT, color, pet[nSelectPet].item[i].name, 0);
										sprintf(详情临时文本, "耐 久 度：%s", pet[nSelectPet].item[i].damage);
										StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 270, FONT_PRIO_FRONT, 0, 详情临时文本, 0);
#ifdef _NB_管理模式
										sprintf(详情临时文本, "道具类型：%s(%d)", 道具类型说明[pet[nSelectPet].item[i].type], pet[nSelectPet].item[i].type);
#else
										sprintf(详情临时文本, "道具类型：%s", 道具类型说明[pet[nSelectPet].item[i].type]);
#endif
										StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 290, FONT_PRIO_FRONT, 0, 详情临时文本, 0);

										sprintf(详情临时文本, "使用等级：%d", pet[nSelectPet].item[i].level);
										StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 310, FONT_PRIO_FRONT, color, 详情临时文本, 0);

										int 分割临时行数 = 0;
										while (1) {
											if (strlen(splitPoint) > 18) {
												strncpy(moji, splitPoint, 18);
												moji[18] = NULL;
												if (GetStrLastByte(moji) == 3) {
													moji[17] = NULL;
													splitPoint += 17;
												}
												else {
													moji[18] = NULL;
													splitPoint += 18;
												}
												StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 330 + 分割临时行数 * 20, FONT_PRIO_FRONT, 1, moji, 0);
												分割临时行数++;
											}
											else {
												strcpy(moji, splitPoint);
												StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 330 + 分割临时行数 * 20, FONT_PRIO_FRONT, 1, moji, 0);
												break;
											}
										}
										splitPoint = pet[nSelectPet].item[i].memo;
										StockDispBuffer(pActMenuWnd2->x - 100, pActMenuWnd2->y + 214, ItemBuffer[i].dispPrio, pet[nSelectPet].item[i].graNo, 0);
										StockDispBuffer(pActMenuWnd2->x - 100, pActMenuWnd2->y + 300, ItemBuffer[i].dispPrio, IMAGE_道具详情框, 0);


#endif
										StockFontBuffer(pActMenuWnd2->x + 16, pActMenuWnd2->y + 323, FONT_PRIO_FRONT, color, pet[nSelectPet].item[i].name, 0);
										// 显示物品耐久度[페트장비 내구도 필요없는기능]
										sprintf(damage_msg, "耐久度(%s)", pet[nSelectPet].item[i].damage);
										StockFontBuffer(pActMenuWnd2->x + 150, pActMenuWnd2->y + 323, FONT_PRIO_FRONT, color, damage_msg, 0);
										int 重定义Y = pActMenuWnd2->y + 346;
										while (1) {
											if (strlen(splitPoint) > 28) {
												strncpy(moji, splitPoint, 28);
												moji[28] = NULL;
												if (GetStrLastByte(moji) == 3) {
													moji[27] = NULL;
													splitPoint += 27;
												}
												else {
													moji[28] = NULL;
													splitPoint += 28;
												}
												StockFontBuffer(x, 重定义Y, FONT_PRIO_FRONT, 0, moji, 0);
												重定义Y += 18;
											}
											else {
												strcpy(moji, splitPoint);
												StockFontBuffer(x, 重定义Y, FONT_PRIO_FRONT, 0, moji, 0);
												break;
											}
										}
									}
								}
								else
									// 人物装备
#endif
								{
#ifdef _ALCHEMIST // 显示Item说明 (奇怪的Bug)
									if (pc.item[i].useFlag == TRUE) {
#else
									if (pc.item[i].useFlag == TRUE && (ItemBuffer[i].mixFlag <= 2 || ItemBuffer[i].mixFlag == 10)) {
#endif
										char *splitPoint = pc.item[i].memo;
										int color = pc.item[i].color;
										if (pc.transmigration == 0 && pc.level < pc.item[i].level) color = FONT_PAL_RED;
#ifdef _NB_道具栏详情
										char 道具类型说明[][56] = { "발톱","도끼","곤봉","총","활","토템","헬멧/모자","옷/갑옷","팔찌","악기","목걸이",
											"알수없음","알수없음","귀걸이","코걸이","부적","공통아이템","스핀","스로잉","스톤",
											"일반품목","금속","보석","화물","벨트","방패","신발","장갑","알수없음","알수없음","합성아이템",
											"페트모자","페트교정기","애완둥물발톱덮개","페트흉갑","페트등딱지","페트날개	","페트발"
										};
										/*char 详情临时文本[256];		//아이템설명창
										StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 250, FONT_PRIO_FRONT, color, pc.item[i].name, 0);
										/*sprintf( 详情临时文本, "내 구 성：%s", pc.item[ i ].damage );
										StockFontBuffer( pActMenuWnd2->x - 170, pActMenuWnd2->y + 270, FONT_PRIO_FRONT, 0, 详情临时文本, 0 );*/
#ifdef _NB_管理模式
										//										sprintf( 详情临时文本, "종    류：%s(%d)", 道具类型说明[pc.item[ i ].type], pc.item[ i ].type);
#else
										sprintf(详情临时文本, "항목형식：%s", 道具类型说明[pc.item[i].type]);
#endif
										/*StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 290, FONT_PRIO_FRONT, 0, 详情临时文本, 0);

										sprintf(详情临时文本, "착용레벨：%d", pc.item[i].level);
										StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 310, FONT_PRIO_FRONT, color, 详情临时文本, 0);

										int 分割临时行数 = 0;
										while (1) {
											if (strlen(splitPoint) > 18) {
												strncpy(moji, splitPoint, 18);
												moji[18] = NULL;
												if (GetStrLastByte(moji) == 3) {
													moji[17] = NULL;
													splitPoint += 17;
												}
												else {
													moji[18] = NULL;
													splitPoint += 18;
												}
												StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 330 + 分割临时行数 * 20, FONT_PRIO_FRONT, 1, moji, 0);
												分割临时行数++;
											}
											else {
												strcpy(moji, splitPoint);
												StockFontBuffer(pActMenuWnd2->x - 170, pActMenuWnd2->y + 330 + 分割临时行数 * 20, FONT_PRIO_FRONT, 1, moji, 0);
												break;
											}
										}
										splitPoint = pc.item[i].memo;
										StockDispBuffer(pActMenuWnd2->x - 100, pActMenuWnd2->y + 214, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
										StockDispBuffer(pActMenuWnd2->x - 100, pActMenuWnd2->y + 300, ItemBuffer[i].dispPrio, IMAGE_道具详情框, 0);
										*/

#endif
										StockFontBuffer(pActMenuWnd2->x + 16, pActMenuWnd2->y + 337, FONT_PRIO_FRONT, color, pc.item[i].name, 0);
										// 显示物品耐久度 아이템내구성표시
										char damage_msg[256];
										sprintf(damage_msg, "내구(%s)", pc.item[i].damage);
										StockFontBuffer(pActMenuWnd2->x + 150, pActMenuWnd2->y + 337, FONT_PRIO_FRONT, color, damage_msg, 0);
										int 重定义Y = pActMenuWnd2->y + 360;
										while (1) {
											if (strlen(splitPoint) > 28) {
												strncpy(moji, splitPoint, 28);
												moji[28] = NULL;
												if (GetStrLastByte(moji) == 3) {
													moji[27] = NULL;
													splitPoint += 27;
												}
												else {
													moji[28] = NULL;
													splitPoint += 28;
												}
												StockFontBuffer(x, 重定义Y, FONT_PRIO_FRONT, 0, moji, 0);
												重定义Y += 18;
											}
											else {
												strcpy(moji, splitPoint);
												StockFontBuffer(x, 重定义Y, FONT_PRIO_FRONT, 0, moji, 0);
												break;
											}
										}
									}
								}
								if (pActPet2 == NULL) {
									if (mouse.onceState & MOUSE_LEFT_CRICK && mouse.itemNo == -1) {
										ItemBuffer[i].mixFlag = FALSE;
#ifdef _PET_ITEM
										if (g_bPetItemWndFlag && (i >= PET_HEAD && i < PET_EQUIPNUM) && nSelectPet > -1) {
											if (pet[nSelectPet].item[i].useFlag == TRUE) {
												ItemBuffer[i].dragFlag = TRUE;
												mouse.itemNo = i;
												itemNo = -1;
											}
										}
										else
#endif
											if (pc.item[i].useFlag == TRUE) {
												ItemBuffer[i].dragFlag = TRUE;
												mouse.itemNo = i;
												itemNo = -1;
											}
									}
									if (mouse.onceState & MOUSE_LEFT_CRICK_UP) {
										if (mouse.itemNo != -1) {
											if (mouse.itemNo != i && ItemBuffer[i].mixFlag == 0) {
#ifdef _PET_ITEM
												if (g_bPetItemWndFlag) {
													if (nSelectPet > -1 && !(mouse.itemNo >= CHAR_EQUIPPLACENUM && i >= CHAR_EQUIPPLACENUM)) {
														if (i < CHAR_EQUIPPLACENUM && nSelectPet == pc.ridePetNo)	// 若是要装上去,检查是不是骑宠
															StockChatBufferLine("骑宠不可装装备！", FONT_PAL_YELLOW);
														else
															lssproto_PetItemEquip_send(sockfd, nowGx, nowGy, nSelectPet, mouse.itemNo, i);
													}
													else {
														if (bNewServer)
															lssproto_MI_send(sockfd, mouse.itemNo, i);
														else
															old_lssproto_MI_send(sockfd, mouse.itemNo, i);
													}
												}
												else
#endif
												{
													if (bNewServer)
														lssproto_MI_send(sockfd, mouse.itemNo, i);
													else
														old_lssproto_MI_send(sockfd, mouse.itemNo, i);
												}
												play_se(217, 320, 240);
											}
											ItemBuffer[mouse.itemNo].dragFlag = FALSE;
											mouse.itemNo = -1;
										}
									}
									if (mouse.onceState & MOUSE_LEFT_DBL_CRICK && ItemUseTime < timeGetTime() - 500) {
										ItemUseTime = timeGetTime();
#ifdef _PET_ITEM
										// 若目前是在人物装备栏且想装备宠物装备时,自动切到宠物装备栏
										if (!g_bPetItemWndFlag) {
											if (pc.item[i].useFlag && pc.item[i].type >= ITEM_PET_HEAD && pc.item[i].type < ITEM_CATEGORYNUM) {
												g_bPetItemWndFlag = true;
												InitItem(pActMenuWnd2->x, pActMenuWnd2->y, g_bPetItemWndFlag);
												break;
											}
										}
										// 若目前是在宠物装备栏且想装备人物装备时,自动切到人物装备栏
										else {
											if (pc.item[i].useFlag && pc.item[i].type < ITEM_PET_HEAD && i >= MAX_ITEMSTART) {
												g_bPetItemWndFlag = false;
												InitItem(pActMenuWnd2->x, pActMenuWnd2->y, g_bPetItemWndFlag);
												break;
											}
										}
#endif
										if (pc.item[i].useFlag != TRUE || pc.item[i].field == ITEM_FIELD_BATTLE || pc.transmigration == 0 && pc.level < pc.item[i].level) {
											play_se(220, 320, 240);
										}
										else {
											mouse.itemNo = -1;
											switch (pc.item[i].target) {
											case ITEM_TARGET_MYSELF:
												if (eventWarpSendFlag == FALSE) {
													if (bNewServer)
														lssproto_ID_send(sockfd, nowGx, nowGy, i, 0);
													else
														old_lssproto_ID_send(sockfd, nowGx, nowGy, i, 0);
													play_se(212, 320, 240);
												}
												break;
											case ITEM_TARGET_OTHER:
												itemNo = i;
												play_se(217, 320, 240);
												if (!(MenuToggleFlag & JOY_CTRL_S)) {
													int j;
													MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S
													for (j = 0; j < MENU_STATUS_0; j++) statusWndFontNo[j] = -2;
													for (j = 0; j < MENU_STATUS_0; j++) statusWndBtnFlag[j] = 0;
													statusWndNo = 1;
													DeathMenuAction();
													saveUserSetting();
													play_se(202, 320, 240);
													MenuToggleFlag &= JOY_CTRL_I;
													MenuToggleFlag |= JOY_CTRL_S;
												}
												else {
													if (statusWndNo != 1) {
														DeathAction(pActMenuWnd);
														pActMenuWnd = NULL;
														statusWndNo = 1;
														play_se(202, 320, 240);
													}
												}
												break;
#ifdef _PET_ITEM
											case ITEM_TARGET_PET:
												if (eventWarpSendFlag == FALSE) {
													// 若是在已装备的装备上连点二下则不动作
													if (i >= PET_HEAD && i < PET_EQUIPNUM)
														break;
													if (pc.ridePetNo != -1 && nSelectPet == pc.ridePetNo)	// 检查是不是骑宠
														StockChatBufferLine("탑승펫장비를 착용할수없어요！", FONT_PAL_YELLOW);
													else
														lssproto_PetItemEquip_send(sockfd, nowGx, nowGy, nSelectPet, i, -1);
													play_se(212, 320, 240);
												}
												break;
#endif
											}
										}
									}
									//#ifdef _STONDEBUG_		
									if (mouse.onceState & MOUSE_RIGHT_CRICK  && mouse.itemNo == -1) {
										int j;
										int cnt = 0;
										if (pc.item[i].useFlag == TRUE && i >= MAX_ITEMSTART) {
											int j;
											int flag = 0;
											if (itemNo != -1) itemNo = -1;
											for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
												if (ItemBuffer[j].mixFlag >= TRUE) {
													flag = ItemBuffer[j].mixFlag;
													break;
												}
											}
											if (ItemBuffer[i].mixFlag == FALSE) {
												if (flag == 0) {
													if (pc.item[i].sendFlag & ITEM_FLAG_COOKING_MIX) {
														if (CheckPetSkill(PETSKILL_MERGE2) == TRUE) {
															ItemBuffer[i].mixFlag = 2;
															play_se(217, 320, 240);
														}
														else {
															play_se(220, 320, 240);
														}
													}
													//andy_add
													else if (pc.item[i].sendFlag & ITEM_FLAG_METAL_MIX ||
														pc.item[i].sendFlag & ITEM_FLAG_JEWEL_MIX) {
														if (CheckPetSkill(PETSKILL_INSLAY)) {
															ItemBuffer[i].mixFlag = 10;
															play_se(217, 320, 240);
														}
														else {
															play_se(220, 320, 240);
														}
													}
													else if (pc.item[i].sendFlag & ITEM_FLAG_FIX_MIX) {
														if (CheckPetSkill(PETSKILL_FIXITEM) || CheckPetSkill(PETSKILL_FIXITEM2)) {
															ItemBuffer[i].mixFlag = 11;
															play_se(217, 320, 240);
														}
														else {
															play_se(220, 320, 240);
														}
													}
#ifdef _ITEM_INTENSIFY
													else if (pc.item[i].sendFlag & ITEM_FLAG_INTENSIFY_MIX) {
														if (CheckPetSkill(PETSKILL_INTENSIFY)) {
															ItemBuffer[i].mixFlag = 12;
															play_se(217, 320, 240);
														}
														else {
															play_se(220, 320, 240);
														}
													}
#endif

													else {
#ifdef _ALCHEMIST // 第一个是否精炼物
														if (CheckPetSkill(PETSKILL_ALCHEMIST) == TRUE &&
															pc.item[i].sendFlag & ITEM_FLAG_MIX &&
															strcmp(pc.item[i].alch, "杂") != NULL) {
															ItemBuffer[i].mixFlag = 9;
														}
														else
#endif
															if (CheckPetSkill(PETSKILL_MERGE) == TRUE &&
																pc.item[i].sendFlag & ITEM_FLAG_MIX) {
																ItemBuffer[i].mixFlag = 1;
																play_se(217, 320, 240);
															}
															else {
																play_se(220, 320, 240);
															}
													}
												}
												else {
													if (pc.item[i].sendFlag & ITEM_FLAG_COOKING_MIX) {
														if (flag == 2) {
															ItemBuffer[i].mixFlag = 2;
															play_se(217, 320, 240);
														}
														else {
															play_se(220, 320, 240);
														}
													}

													else {
														if (flag == 1) {
															ItemBuffer[i].mixFlag = 1;
															play_se(217, 320, 240);
														}
#ifdef _ALCHEMIST // 第一个之后的精炼物
														else if (flag == 9) {
															if (strcmp(pc.item[i].alch, pc.item[j].alch) == FALSE) {
																ItemBuffer[i].mixFlag = 9;
															}
															play_se(217, 320, 240);
														}
#endif
														else if (flag == 10) {	//andy_add inslay
															ItemBuffer[i].mixFlag = 10;
															play_se(217, 320, 240);
														}
														else if (flag == 11) {	//andy_add fixitem
															ItemBuffer[i].mixFlag = 11;
															play_se(217, 320, 240);
														}
#ifdef _ITEM_INTENSIFY
														else if (flag == 12) {
															ItemBuffer[i].mixFlag = 12;
															play_se(217, 320, 240);
														}
#endif
														else {
															play_se(220, 320, 240);
														}

													}
												}
											}
											else
											{
#ifdef _ALCHEMIST // 취소一个合成物时...
												if (ItemBuffer[i].mixFlag == 9) {
													int k;
													ItemBuffer[i].mixFlag = 1;
													for (k = MAX_ITEMSTART; k < MAX_ITEM; k++) {
														if (ItemBuffer[k].mixFlag >= TRUE && (k != i)) {
															ItemBuffer[i].mixFlag = FALSE;
															break;
														}
													}
												}
												else {
													ItemBuffer[i].mixFlag = FALSE;
												}
#else
												ItemBuffer[i].mixFlag = FALSE;
#endif
												play_se(217, 320, 240);
											}
										}
										else
											play_se(220, 320, 240);

										for (j = MAX_ITEMSTART; j < MAX_ITEM; j++) {
											if (ItemBuffer[j].mixFlag >= 1) cnt++;
										}
										if (cnt >= 2) {
											if (!(MenuToggleFlag & JOY_CTRL_P)) {
												MenuToggleFlag ^= JOY_CTRL_P;	// CTRL + S
												for (j = 0; j < MENU_PET_0; j++) petWndFontNo[j] = -2;
												for (j = 0; j < MENU_PET_0; j++) petWndBtnFlag[j] = 0;
												petWndNo = 0;
												DeathMenuAction();
												saveUserSetting();
												play_se(202, 320, 240);
												MenuToggleFlag &= JOY_CTRL_I;	// 
												MenuToggleFlag |= JOY_CTRL_P;	// 
											}
										}
									}
								}
							}
#ifdef _PET_ITEM
							if (g_bPetItemWndFlag && nSelectPet > -1 && (i >= PET_HEAD && i < PET_EQUIPNUM) && pet[nSelectPet].useFlag == TRUE)
								StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, ItemBuffer[i].dispPrio, pet[nSelectPet].item[i].graNo, 0);
							else
#endif
								if (pc.item[i].useFlag == TRUE) {
#ifdef _PET_ITEM
									if (!(g_bPetItemWndFlag && i < CHAR_EQUIPPLACENUM))
#endif
									{
#if 0 //#ifdef _ALCHEMIST
										if (ItemBuffer[i].mixFlag <= 9)
#else
										if (ItemBuffer[i].mixFlag <= 2)
#endif
										{
											StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
#ifdef _ITEM_PILENUMS
											if (pc.item[i].pile > 1)
											{
												char pile[256];
												sprintf(pile, "%d", pc.item[i].pile);
												StockFontBuffer(ItemBuffer[i].defX + 10, ItemBuffer[i].defY, FONT_PRIO_FRONT, FONT_PAL_WHITE, pile, 0);
											}
#endif
										}
#ifdef _ALCHEMIST
										else if (ItemBuffer[i].mixFlag == 9) {
											StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
										}
#endif
										else if (ItemBuffer[i].mixFlag == 10) { //andy_add inslay
											StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
										}
										else if (ItemBuffer[i].mixFlag == 11) { //andy_add fixitem
											StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
										}
#ifdef _ITEM_INTENSIFY
										else if (ItemBuffer[i].mixFlag == 12) { //andy_add fixitem
											StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
										}
#endif

										else if (ItemBuffer[i].mixFlag == 3) {
											StockDispBuffer(pActPet2->x, pActPet2->y - 16, DISP_PRIO_DRAG, pc.item[i].graNo, 0);
										}
										else if (ItemBuffer[i].mixFlag == 4) {
											StockDispBuffer(pActPet2->x, pActPet2->y, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
										}

									}
								}
							// 显示合成文字
							if (ItemBuffer[i].mixFlag >= TRUE) {
								if (pc.item[i].useFlag == TRUE) {
									// 普通合成的时候
									if (ItemBuffer[i].mixFlag == 1) {
										StockFontBuffer(ItemBuffer[i].defX - 17, ItemBuffer[i].defY, FONT_PRIO_FRONT, 0, "합성", 0);
									}
									else
										// 料理合成的时候
										if (ItemBuffer[i].mixFlag == 2) {
											StockFontBuffer(ItemBuffer[i].defX - 17, ItemBuffer[i].defY, FONT_PRIO_FRONT, 0, "요리", 0);
										}
#ifdef _ALCHEMIST
									if (ItemBuffer[i].mixFlag == 9) {
										StockFontBuffer(ItemBuffer[i].defX - 17, ItemBuffer[i].defY, FONT_PRIO_FRONT, 0, "정련/정제", 0);
									}
#endif
									if (ItemBuffer[i].mixFlag == 10) {// PETSKILL_INSLAY ANDY_ADD
										StockFontBuffer(ItemBuffer[i].defX - 17, ItemBuffer[i].defY, FONT_PRIO_FRONT, 0, "정교", 0);
									}
									if (ItemBuffer[i].mixFlag == 11) {// PETSKILL_FIXITEM ANDY_ADD
										StockFontBuffer(ItemBuffer[i].defX - 17, ItemBuffer[i].defY, FONT_PRIO_FRONT, 0, "수리", 0);
									}
#ifdef _ITEM_INTENSIFY
									if (ItemBuffer[i].mixFlag == 12) {
										StockFontBuffer(ItemBuffer[i].defX - 17, ItemBuffer[i].defY, FONT_PRIO_FRONT, 0, "강화", 0);
									}
#endif
								}
								else {
									ItemBuffer[i].mixFlag = TRUE;
								}
							}
						}
						if (mouse.onceState & MOUSE_LEFT_CRICK_UP) {
							if (mouse.itemNo != -1) {
								if (mouse.nowPoint.x <= pActMenuWnd2->x && mouse.itemNo != -1 && eventWarpSendFlag == FALSE) {
#ifdef _PET_ITEM
									if (g_bPetItemWndFlag)
										lssproto_PetItemEquip_send(sockfd, nowGx, nowGy, nSelectPet, mouse.itemNo, -2);
									else
#endif
										if (bNewServer) {
											lssproto_DI_send(sockfd, nowGx, nowGy, mouse.itemNo);
										}
										else
											old_lssproto_DI_send(sockfd, nowGx, nowGy, mouse.itemNo);

								}
								ItemBuffer[mouse.itemNo].dragFlag = FALSE;
								mouse.itemNo = -1;
								play_se(217, 320, 240);
							}
						}
						if (mouse.itemNo != -1) {
							if (mouse.onceState & MOUSE_RIGHT_CRICK) {
								ItemBuffer[mouse.itemNo].dragFlag = FALSE;
								mouse.itemNo = -1;
								play_se(217, 320, 240);
							}
#ifdef _PET_ITEM
							if (g_bPetItemWndFlag && nSelectPet > -1 && (mouse.itemNo >= PET_HEAD && mouse.itemNo < PET_EQUIPNUM) && pet[nSelectPet].useFlag == TRUE)
								StockDispBuffer(mouse.nowPoint.x, mouse.nowPoint.y, DISP_PRIO_DRAG, pet[nSelectPet].item[mouse.itemNo].graNo, 0);
							else
#endif
								StockDispBuffer(mouse.nowPoint.x, mouse.nowPoint.y, DISP_PRIO_DRAG, pc.item[mouse.itemNo].graNo, 0);
						}
						if (pActMenuWnd2 != NULL) {
#ifdef _NB_道具自动叠加		//[아이템 자동 겹치기(/합성/강화)]
														道具叠加按钮 = StockDispBuffer(临时MAXX - 300, 380, DISP_PRIO_IME3, IMAGE_叠加按钮, 2);
							itemWndFontNo[0] = StockDispBuffer(临时MAXX - 150, 428, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
#else
							itemWndFontNo[0] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, pActMenuWnd2->y + 422, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
#endif
#ifdef _PET_ITEM
							if (g_bPetItemWndFlag) {
								itemWndFontNo[1] = -2;
								// 显示左右箭头 //临时MAXX
#ifdef _NB_新道具栏界面
								itemWndFontNo[5] = StockDispBuffer(临时MAXX - 90, 150, DISP_PRIO_IME3, 26047, 2);
								itemWndFontNo[6] = StockDispBuffer(临时MAXX - 42, 150, DISP_PRIO_IME3, 26048, 2);
#else
								itemWndFontNo[5] = StockDispBuffer(pActMenuWnd2->x + 188, pActMenuWnd2->y + 142, DISP_PRIO_IME3, 26047, 2);
								itemWndFontNo[6] = StockDispBuffer(pActMenuWnd2->x + 236, pActMenuWnd2->y + 142, DISP_PRIO_IME3, 26048, 2);
#endif	

								if (nSelectPet > -1 && pet[nSelectPet].useFlag) {// [펫 정보 보기]
									char szTemp[16];
									WORD wSpecies = HIWORD(pet[nSelectPet].fusion);
									if (wSpecies < 0 || wSpecies >= MAX_PET_SPECIES)
										wSpecies = MAX_PET_SPECIES;
									// 显示宠物装备栏的底图[펫 장비 슬롯 기본 이미지를 표시함] 
									for (i = 0; i < PET_EQUIPNUM; i++) {
										StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, DISP_PRIO_IME3, nPetItemEquipBmpNumber[i][0], 0);
										//NB취소可否装备底图判断
										/*
										if (byShowPetItemBackground[wSpecies] & (1 << i)){
										StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, DISP_PRIO_IME3, nPetItemEquipBmpNumber[i][0], 0);
										}else{
										StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, DISP_PRIO_IME3, nPetItemEquipBmpNumber[i][1], 0);
										}*/
									}

									// 显示宠物名称[펫 이름 표시]
									StockFontBuffer(临时MAXX - 128, 17, FONT_PRIO_FRONT, 1, pet[nSelectPet].name, 0);
									if (strlen(pet[nSelectPet].freeName) > 0) {
										StockFontBuffer(临时MAXX - 128, 35, FONT_PRIO_FRONT, 8, pet[nSelectPet].freeName, 0);
									}

									// 显示宠物的属性[펫 속성 표시]
									itoa(pet[nSelectPet].maxHp, szTemp, 10);
									StockFontBuffer(临时MAXX - 56, 60, FONT_PRIO_FRONT, FONT_PAL_WHITE, szTemp, 0);
									itoa(pet[nSelectPet].atk, szTemp, 10);
									StockFontBuffer(临时MAXX - 56, 80, FONT_PRIO_FRONT, FONT_PAL_WHITE, szTemp, 0);
									itoa(pet[nSelectPet].def, szTemp, 10);
									StockFontBuffer(临时MAXX - 56, 100, FONT_PRIO_FRONT, FONT_PAL_WHITE, szTemp, 0);
									itoa(pet[nSelectPet].quick, szTemp, 10);
									StockFontBuffer(临时MAXX - 56, 120, FONT_PRIO_FRONT, FONT_PAL_WHITE, szTemp, 0);
								}
								else {
									nSelectPet = -1;
									for (i = 0; i < MAX_PET; i++) {
										// [이런 펫이 있음]
										if (pet[i].useFlag) {
											nSelectPet = i;
											break;
										}
									}
								}
							}
							else
#endif
							{
								//打印咒术按钮[주술 버튼 인쇄/표시]
								itemWndFontNo[1] = StockDispBuffer(临时MAXX - 68, 225, DISP_PRIO_IME3, CG_ITEM_WND_JUJUTU_BTN, 2);
								sprintf(moji, "%10d", pc.gold);
								StockFontBuffer(临时MAXX - 92, 90, FONT_PRIO_FRONT, 0, moji, 0);

								sprintf(moji, "%10d", itemWndDropGold);
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
								if (Moneyflag == false)
									StockFontBuffer(临时MAXX - 2, 138, FONT_PRIO_FRONT, 0, moji, 0);
#else
								StockFontBuffer(pActMenuWnd2->x + 192, pActMenuWnd2->y + 138, FONT_PRIO_FRONT, 0, moji, 0);
#endif
								itemWndFontNo[2] = StockDispBuffer(临时MAXX - 136, 224, DISP_PRIO_IME3, CG_ITEM_WND_GOLD_DROP_BTN_UP + itemWndBtnFlag[2], 2);
								itemWndFontNo[3] = StockDispBuffer(临时MAXX - 136, 224, DISP_PRIO_IME3, CG_ITEM_WND_GOLD_INC_BTN_UP + itemWndBtnFlag[3], 2);
								itemWndFontNo[4] = StockDispBuffer(临时MAXX - 136, 224, DISP_PRIO_IME3, CG_ITEM_WND_GOLD_DEC_BTN_UP + itemWndBtnFlag[4], 2);
							}
						}
					}
				}
			}
			break;
		case 1:
			if (pActMenuWnd2 == NULL) {
				pActMenuWnd2 = MakeWindowDisp(364, 4, 272, 280, 0, -1);
				for (i = 0; i < MENU_ITEM_0; i++) itemWndFontNo[i] = -2;
				for (i = 0; i < MENU_ITEM_0; i++) itemWndBtnFlag[i] = 0;
				jujutuNo = -1;
			}
			else {
				if (pActMenuWnd2->hp > 0) {
					x = pActMenuWnd2->x + 74;
					y = pActMenuWnd2->y + 208;
#ifdef _READ16BITBMP
					if (g_bUseAlpha) StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd2->pYobi)->my + 7, DISP_PRIO_MENU, CG_JUJUTU_WND, 1);
					else
#endif
						StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd2->pYobi)->my + 10, DISP_PRIO_MENU, CG_JUJUTU_WND, 1);
					for (i = 0; i < 5; i++) {
						if (HitFontNo == itemWndFontNo[i]) {
							char *splitPoint = magic[i].memo;
							sprintf(moji, "%2d/%2d", magic[i].mp, pc.mp);
							StockFontBuffer(pActMenuWnd2->x + 74 + 81, pActMenuWnd2->y + 183, FONT_PRIO_FRONT, 0, moji, 0);
							while (1) {
								if (strlen(splitPoint) > 22) {
									strncpy(moji, splitPoint, 22);
									moji[22] = NULL;
									if (GetStrLastByte(moji) == 3) {
										moji[21] = NULL;
										splitPoint += 21;
									}
									else {
										moji[22] = NULL;
										splitPoint += 22;
									}
									StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 0); y += 24;
								}
								else {
									strcpy(moji, splitPoint);
									StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 0);
									break;
								}
							}
#if 0
							if (magic[i].memo[0] != NULL) {
								strncpy(moji, magic[i].memo, 22);
								moji[22] = NULL;
								StockFontBuffer(pActMenuWnd2->x + 74, pActMenuWnd2->y + 204, FONT_PRIO_FRONT, 0, moji, 0);

								if (magic[i].memo[22] != NULL) {
									strncpy(moji, magic[i].memo + 22, 22);
									moji[22] = NULL;
									StockFontBuffer(pActMenuWnd2->x + 74, pActMenuWnd2->y + 224, FONT_PRIO_FRONT, 0, moji, 0);
								}
							}
#endif
							StockDispBuffer(pActMenuWnd2->x + 37, pActMenuWnd2->y + 220, DISP_PRIO_ITEM, pc.item[i].graNo, 0);
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								if (magic[i].field == MAGIC_FIELD_BATTLE || magic[i].mp > pc.mp) {
									play_se(220, 320, 240);
								}
								else {
									switch (magic[i].target) {

									case MAGIC_TARGET_MYSELF:
										if (eventWarpSendFlag == FALSE) {
											if (bNewServer)
												lssproto_MU_send(sockfd, nowGx, nowGy, i, 0);
											else
												old_lssproto_MU_send(sockfd, nowGx, nowGy, i, 0);
											play_se(100, 320, 240);
										}
										break;

									case MAGIC_TARGET_OTHER:
										jujutuNo = i;
										play_se(217, 320, 240);
										//itemNo = -1;
										break;
#if 0
									case MAGIC_TARGET_ALLMYSIDE:
										if (bNewServer)
											lssproto_MU_send(sockfd, nowGx, nowGy, i, 0);
										else
											old_lssproto_MU_send(sockfd, nowGx, nowGy, i, 0);
										play_se(100, 320, 240);
										break;

									case MAGIC_TARGET_ALLOTHERSIDE:
										if (bNewServer)
											lssproto_MU_send(sockfd, nowGx, nowGy, i, 0);
										else
											old_lssproto_MU_send(sockfd, nowGx, nowGy, i, 0);
										play_se(100, 320, 240);
										break;

									case MAGIC_TARGET_ALL:
										if (bNewServer)
											lssproto_MU_send(sockfd, nowGx, nowGy, i, 0);
										else
											old_lssproto_MU_send(sockfd, nowGx, nowGy, i, 0);
										play_se(100, 320, 240);
										break;
#endif									
									}
								}
							}
						}
					}
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == itemWndFontNo[12]) {
							DeathAction(pActMenuWnd2);
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
							GetKeyInputFocus(&MyChatBuffer);
							Moneyflag = false;
#endif
							pActMenuWnd2 = NULL;
							itemWndNo = 0;
							play_se(203, 320, 240);
							jujutuNo = -1;
							if (MenuToggleFlag & JOY_CTRL_S) {
								MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S
								DeathMenuAction();
							}
						}
						if (jujutuNo != -1 && MenuToggleFlag & JOY_CTRL_S && statusWndNo == 1) {
							for (i = 0; i < 11; i++) {
								if (HitFontNo == statusWndFontNo[i] && eventWarpSendFlag == FALSE) {
									if (bNewServer)
										lssproto_MU_send(sockfd, nowGx, nowGy, jujutuNo, i);
									else
										old_lssproto_MU_send(sockfd, nowGx, nowGy, jujutuNo, i);
									play_se(100, 320, 240);
									if (magic[jujutuNo].mp > pc.mp - magic[jujutuNo].mp) jujutuNo = -1;;

								}
							}
						}

					}
					if (pActMenuWnd2 != NULL) {
						int flag = FALSE;
						int col;
						x = pActMenuWnd2->x + 30;
						y = pActMenuWnd2->y + 35;
						for (i = 0; i < 5; i++) {
							if (magic[i].useFlag == TRUE) {
								col = FONT_PAL_WHITE;
								if (jujutuNo == i) col = FONT_PAL_AQUA;
								if (pc.familySprite == 0) {
									if ((magic[i].mp * 80 / 100) > pc.mp) col = FONT_PAL_RED;
								}
								else {
									if (magic[i].mp > pc.mp) col = FONT_PAL_RED;
								}
								if (magic[i].field == MAGIC_FIELD_BATTLE) col = FONT_PAL_GRAY;
								CenteringStr(magic[i].name, moji, MAGIC_NAME_LEN);
								itemWndFontNo[i] = StockFontBuffer(x, y, FONT_PRIO_FRONT, col, moji, 2); y += 28;
								flag = TRUE;
							}
						}
						if (flag == FALSE) StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "주술이 없습니다.", 0);
						itemWndFontNo[12] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, pActMenuWnd2->y + 262 + 10, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
					}
				}
			}
			break;

		case 2:
			if (pActMenuWnd2 == NULL) {
				pActMenuWnd2 = MakeWindowDisp(368, 4, 192, 304, 0, -1);
				for (i = 0; i < MENU_ITEM_0; i++) itemWndFontNo[i] = -2;
				for (i = 0; i < MENU_ITEM_0; i++) itemWndBtnFlag[i] = 0;
			}
			else {
				if (pActMenuWnd2->hp > 0) {
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd2->pYobi)->my, DISP_PRIO_MENU, CG_ITEM_WND_SELECT_WND, 1);
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						for (i = 0; i < 11; i++) {
							if (HitFontNo == itemWndFontNo[i] && eventWarpSendFlag == FALSE) {
								if (jujutuNo != -1) {
									if (bNewServer)
										lssproto_MU_send(sockfd, nowGx, nowGy, jujutuNo, i);
									else
										old_lssproto_MU_send(sockfd, nowGx, nowGy, jujutuNo, i);
									play_se(100, 320, 240);
									if (magic[jujutuNo].mp > pc.mp - magic[jujutuNo].mp) {
										DeathAction(pActMenuWnd2);
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
										GetKeyInputFocus(&MyChatBuffer);
										Moneyflag = false;
#endif
										pActMenuWnd2 = NULL;
										itemWndNo = 1;
										play_se(203, 320, 240);
									}
								}
								else {
									if (bNewServer)
										lssproto_ID_send(sockfd, nowGx, nowGy, itemNo, i);
									else
										old_lssproto_ID_send(sockfd, nowGx, nowGy, itemNo, i);
									play_se(212, 320, 240);
									DeathAction(pActMenuWnd2);
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
									GetKeyInputFocus(&MyChatBuffer);
									Moneyflag = false;
#endif
									pActMenuWnd2 = NULL;
									itemWndNo = 0;
									if (MenuToggleFlag & JOY_CTRL_S) {
										MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S
										DeathMenuAction();
									}
									play_se(203, 320, 240);
								}
							}
						}
						if (HitDispNo == itemWndFontNo[11]) {
							DeathAction(pActMenuWnd2);
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
							GetKeyInputFocus(&MyChatBuffer);
							Moneyflag = false;
#endif
							pActMenuWnd2 = NULL;
							if (jujutuNo != -1) itemWndNo = 1;
							else {
								itemWndNo = 0;
								if (MenuToggleFlag & JOY_CTRL_S) {
									MenuToggleFlag ^= JOY_CTRL_S;	// CTRL + S
									DeathMenuAction();
								}
							}
							play_se(203, 320, 240);
						}
					}
					if (pActMenuWnd2 != NULL) {
						x = pActMenuWnd2->x + 25;
						y = pActMenuWnd2->y + 30;
						sprintf(moji, "%-16s", pc.name);
						itemWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 2); y += 23;
						for (i = 0; i < 5; i++) {
							if (pet[i].useFlag == TRUE) {
								if (pet[i].freeName[0] != NULL)
									sprintf(moji, "%-16s", pet[i].freeName);
								else
									sprintf(moji, "%-16s", pet[i].name);
								itemWndFontNo[i + 1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 2); y += 23;
							}
						}
						y = 183;
						for (i = 0; i < 5; i++) {
							if (pc.id != party[i].id && party[i].useFlag == TRUE) {
								sprintf(moji, "%-16s", party[i].name);
								itemWndFontNo[i + 6] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 2); y += 23;
							}
						}
						itemWndFontNo[11] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, pActMenuWnd2->y + 287, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
					}
				}
			}
			break;
		}
	}

	//? ?????????? *******************************************************/

	if (MenuToggleFlag & JOY_CTRL_M) {
#ifdef _NEWPANEL //Syu ADD 7.0 新人物状态介面
		SkillWndflag = false;
#endif
		switch (mapWndNo) {

		case 0:
			if (pActMenuWnd2 == NULL) {
				pActMenuWnd2 = MakeWindowDisp(388, 4, 248, 240, 0, -1);
				for (i = 0; i < MENU_MAP_0; i++) mapWndFontNo[i] = -2;
			}
			else {
				if (pActMenuWnd2->hp > 0) {
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd2->pYobi)->my, DISP_PRIO_MENU, CG_MAP_WND, 0);
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == mapWndFontNo[0]) {
							DeathAction(pActMenuWnd2);
#ifdef _MONEYINPUT //Syu ADD 手动输入金钱量
							GetKeyInputFocus(&MyChatBuffer);
							Moneyflag = false;
#endif
							pActMenuWnd2 = NULL;
							MenuToggleFlag ^= JOY_CTRL_M;
							play_se(203, 320, 240);
						}
					}
					if (pActMenuWnd2 != NULL) {
						x = pActMenuWnd2->x + 61;
						y = pActMenuWnd2->y + 186;
						StockFontBuffer(pActMenuWnd2->x + 22, pActMenuWnd2->y + 31, FONT_PRIO_FRONT, 0, nowFloorName, 0);
						sprintf(moji, "동 %3d", nowGx);
						StockFontBuffer(x, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0);
						sprintf(moji, "남 %3d", nowGy);
						StockFontBuffer(x + 73, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, moji, 0); y += 36;
						mapWndFontNo[0] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd2->pYobi)->mx, y, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
					}
				}
			}
			break;
		}
	}


	//? ?????????? *******************************************************/

	if (MenuToggleFlag & JOY_CTRL_E) {
		// ????????
		switch (mailWndNo) {

		case MAIL_WND_VIEW:	// ???????

							// ?????????
			if (pActMenuWnd == NULL) {
				// ?????????
				pActMenuWnd = MakeWindowDisp(4, 4, 272, 440, 0, -1);
				// ??????
				for (i = 0; i < MENU_MAIL_0; i++) mailWndFontNo[i] = -2;
				// ????????????????????????
				for (i = 0; i < MAX_ADR_BOOK; i++) {
					int j;
					// ????????
					for (j = 0; j < MAIL_MAX_HISTORY; j++) {
						// ????
						if (MailHistory[i].noReadFlag[j] >= TRUE) {
							// ????????
							mailViewWndPageNo = i / MAX_ADR_BOOK_COUNT;
							i = MAX_ADR_BOOK;
							break;
						}
					}
				}
			}
			else {
				if (pActMenuWnd->hp > 0) {
					static int nowDelNo;
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_MAIL_WND, 1);
					if (mouse.onceState & MOUSE_LEFT_CRICK && pActYesNoWnd == NULL) {
						int nowNo = mailViewWndPageNo * MAX_ADR_BOOK_COUNT;
						for (i = 0; i < 12; i += 3, nowNo++) {
#ifdef _EXTEND_AB
							if (addressBook[nowNo].useFlag == TRUE
								|| addressBook[nowNo].useFlag == 2
								|| addressBook[nowNo].useFlag == 3) {
#else
							if (addressBook[nowNo].useFlag == TRUE) {
#endif	
								if (HitFontNo == mailWndFontNo[i]) {
									mailHistoryWndSelectNo = nowNo;
									DeathAction(pActMenuWnd);
									pActMenuWnd = NULL;
									DeathLetterAction();
									mailWndNo = MAIL_WND_HISTORY;
									play_se(202, 320, 240);
								}
								if (HitDispNo == mailWndFontNo[i + 1]) {
									if (pActYesNoWnd == NULL) {
										play_se(202, 320, 240);
										pActYesNoWnd = MakeWindowDisp(pActMenuWnd->x + 42, i * 32 + 54, 176, 56, 0, 4);
										nowDelNo = nowNo;
									}
								}
								if (HitDispNo == mailWndFontNo[i + 2]) {
									DeathAction(pActMenuWnd);
									pActMenuWnd = NULL;
									DeathLetterAction();
									InitMailSendFlag();
									mailWndSendFlag[nowNo] = TRUE;
									mailWndNoBak = mailWndNo;
									if (ProcNo == PROC_BATTLE) {
										mailWndNo = MAIL_WND_SEND;
									}
									else {
										if (pc.mailPetNo != -1) {
											mailWndNo = MAIL_WND_SELECT;
										}
										else {
											mailWndNo = MAIL_WND_SEND;
										}
									}
									play_se(202, 320, 240);
								}
							}
						}
						if (HitDispNo == mailWndFontNo[19]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathLetterAction();
							MenuToggleFlag ^= JOY_CTRL_E;
							play_se(203, 320, 240);
						}
					}
					if ((HitDispNo == mailWndFontNo[26] || joy_con[0] & JOY_A) && pActYesNoWnd == NULL) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_A) {
							mailViewWndPageNo--;
							if (mailViewWndPageNo < 0) mailViewWndPageNo = MAX_ADR_BOOK_PAGE - 1;
							play_se(217, 320, 240);
							mailWndBtnFlag[26] = TRUE;
							DeathLetterAction();
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_A) && mailWndBtnFlag[26] == TRUE) {
							mailWndBtnFlag[26] = TRUE;
						}
						else {
							mailWndBtnFlag[26] = FALSE;
						}
					}
					else {
						mailWndBtnFlag[26] = FALSE;
					}
					if ((HitDispNo == mailWndFontNo[27] || joy_con[0] & JOY_B) && pActYesNoWnd == NULL) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_B) {
							mailViewWndPageNo++;
							if (mailViewWndPageNo >= MAX_ADR_BOOK_PAGE) mailViewWndPageNo = 0;
							play_se(217, 320, 240);
							mailWndBtnFlag[27] = TRUE;
							DeathLetterAction();
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_B) && mailWndBtnFlag[27] == TRUE) {
							mailWndBtnFlag[27] = TRUE;
						}
						else {
							mailWndBtnFlag[27] = FALSE;
						}
					}
					else {
						mailWndBtnFlag[27] = FALSE;
					}
					if (pActMenuWnd != NULL) {
						int nowNo = mailViewWndPageNo * MAX_ADR_BOOK_COUNT;
						x = pActMenuWnd->x;
						y = pActMenuWnd->y;
						int graNo[7] = { CG_MAIL_WND_OFF_LINE_BTN,
							CG_MAIL_WND_ON_LINE_SUN_BTN,
							CG_MAIL_WND_ON_LINE_MOON_BTN,
							CG_MAIL_WND_ON_LINE_MERCURY_BTN,
							CG_MAIL_WND_ON_LINE_VINUS_BTN,
							CG_MAIL_WND_ON_LINE_EARTH_BTN,
							CG_MAIL_WND_ON_LINE_MARS_BTN
						};

						for (i = 0; i < 12; i += 3, nowNo++) {
#ifdef _EXTEND_AB
							if (addressBook[nowNo].useFlag == TRUE
								|| addressBook[nowNo].useFlag == 2
								|| addressBook[nowNo].useFlag == 3) {
#else
							if (addressBook[nowNo].useFlag == TRUE) {
#endif
								int j = 0;
								for (j = 0; j < MAIL_MAX_HISTORY; j++) {
									if (pActLetter[i / 3] == NULL) {
										if (MailHistory[nowNo].noReadFlag[j] >= TRUE) {
											pActLetter[i / 3] = MakeAnimDisp(x - 20, y + 38, SPR_mail, ANIM_DISP_LETTER_MOVE);
										}
									}
								}
#ifdef _EXTEND_AB
								if (addressBook[nowNo].useFlag == 2)
									StockFontBuffer(x + 15, y + 85, FONT_PRIO_FRONT, 5, "정령의 사자", 0);
								if (addressBook[nowNo].useFlag == 3)
									StockFontBuffer(x + 15, y + 85, FONT_PRIO_FRONT, 5, "정령의 용자", 0);
#endif
								StockDispBuffer(x + 44, y + 68, DISP_PRIO_IME3, addressBook[nowNo].graNo, 0);
								CenteringStr(addressBook[nowNo].name, moji, CHAR_NAME_LEN);
								mailWndFontNo[i] = StockFontBuffer(x + 80, y + 30, FONT_PRIO_FRONT, 0, moji, 2);
								sprintf(moji, "%3d", addressBook[nowNo].level);
								StockFontBuffer(x + 104, y + 60, FONT_PRIO_FRONT, 0, moji, 0);
#ifdef _MAILSHOWPLANET				// (可开放) Syu ADD 显示名片星球
								sprintf(moji, "%8s", addressBook[nowNo].planetname);
								StockFontBuffer(x + 198, y + 30, FONT_PRIO_FRONT, 5, moji, 0);
#endif
								//
#ifdef _TRANS_6
#ifdef _TRANS_7
								if (addressBook[nowNo].transmigration < 0 || addressBook[nowNo].transmigration >= 8) addressBook[nowNo].transmigration = 0;
#else
								if (addressBook[nowNo].transmigration < 0 || addressBook[nowNo].transmigration >= 7) addressBook[nowNo].transmigration = 0;
#endif
#else								
								if (addressBook[nowNo].transmigration < 0 || addressBook[nowNo].transmigration >= 6) addressBook[nowNo].transmigration = 0;
#endif
								StockFontBuffer(x + 152, y + 59, FONT_PRIO_FRONT, FONT_PAL_AQUA, TransmigrationStr[addressBook[nowNo].transmigration], 0);
								sprintf(moji, "%8d", addressBook[nowNo].dp);
								StockFontBuffer(x + 103, y + 88, FONT_PRIO_FRONT, 0, moji, 0);
								StockDispBuffer(x + 225, y + 60, DISP_PRIO_IME3, graNo[(addressBook[nowNo].onlineFlag == 0) ? 0 : 1], 0);
								mailWndFontNo[i + 2] = StockDispBuffer(x + 225, y + 80, DISP_PRIO_IME3, CG_MAIL_WND_MAIL_BTN, 2);
								mailWndFontNo[i + 1] = StockDispBuffer(x + 225, y + 100, DISP_PRIO_IME3, CG_MAIL_WND_DELETE_BTN, 2);

							}
#ifdef _EXTEND_AB
							else {
								if (nowNo == MAX_ADR_BOOK - 1)
								{
									StockFontBuffer(x + 100, y + 30, FONT_PRIO_FRONT, FONT_PAL_RED, "정령 소환 용도", 0);
								}
							}
#endif
							y += 96;
						}
						mailWndFontNo[26] = StockDispBuffer(pActMenuWnd->x + 25 + 28 - 8, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_PREV_BTN + mailWndBtnFlag[26], 2);
						sprintf(moji, "%2d", mailViewWndPageNo + 1);
						StockFontBuffer(pActMenuWnd->x + 46 + 28 - 8, pActMenuWnd->y + 413, FONT_PRIO_FRONT, 0, moji, 0);
						mailWndFontNo[27] = StockDispBuffer(pActMenuWnd->x + 75 + 28, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_NEXT_BTN + mailWndBtnFlag[27], 2);
						mailWndFontNo[19] = StockDispBuffer(pActMenuWnd->x + 220 - 28, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
					}
					if (pActYesNoWnd != NULL) {
						if (((WINDOW_DISP *)pActYesNoWnd->pYobi)->yesNoResult == TRUE) {
							if (bNewServer)
								lssproto_DAB_send(sockfd, nowDelNo);
							else
								old_lssproto_DAB_send(sockfd, nowDelNo);
							play_se(217, 320, 240);
							DeathAction(pActYesNoWnd);
							pActYesNoWnd = NULL;
							play_se(203, 320, 240);
						}
						else if (((WINDOW_DISP *)pActYesNoWnd->pYobi)->yesNoResult == FALSE) {
							DeathAction(pActYesNoWnd);
							pActYesNoWnd = NULL;
							play_se(203, 320, 240);
						}
					}
				}

			}
			break;
		case MAIL_WND_SELECT:
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(40, 196, 3, 3, NULL, 0);
				for (i = 0; i < MENU_MAIL_0; i++) mailWndFontNo[i] = -2;
				for (i = 0; i < 5; i++) {
					if (pet[i].useFlag == FALSE) {
						if (i == pc.mailPetNo) {
							pc.mailPetNo = -1;
							saveUserSetting();
						}
					}
				}
			}
			else {
				if (pActMenuWnd->hp > 0) {
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitFontNo == mailWndFontNo[0]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							mailWndNo = MAIL_WND_SEND;
							play_se(202, 320, 240);
						}
						if (HitFontNo == mailWndFontNo[1]) {

#ifdef _STREET_VENDOR
							if (pc.iOnStreetVendor == 1) {
								play_se(220, 320, 240);
								StockChatBufferLine("거래중인 페트는 페트메일기능을 사용할 수 없어요!", FONT_PAL_RED);
							}
							else
#endif

								if (pc.mailPetNo != -1) {
									DeathAction(pActMenuWnd);
									pActMenuWnd = NULL;
									mailWndNo = MAIL_WND_PET_SEND;
									play_se(202, 320, 240);
								}
								else {
									play_se(220, 320, 240);
								}
						}
						if (HitDispNo == mailWndFontNo[2]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							mailWndNo = MAIL_WND_VIEW;
							play_se(203, 320, 240);
						}
					}
					if (pActMenuWnd != NULL) {
						int color = FONT_PAL_WHITE;
						if (pc.mailPetNo == -1)
							color = FONT_PAL_RED;
						x = pActMenuWnd->x + 28;
						y = pActMenuWnd->y + 32;
						mailWndFontNo[0] = StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, "  일 반 메 일   ", 2);	y += 32;
						mailWndFontNo[1] = StockFontBuffer(x, y, FONT_PRIO_FRONT, color, "  페 트 메 일   ", 2);	y += 48;
						mailWndFontNo[2] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, y, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
					}
				}
			}
			break;

		case MAIL_WND_SEND:
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(4, 4, 272, 304, 0, -1);
				for (i = 0; i < MENU_MAIL_0; i++) mailWndFontNo[i] = -2;
				mailPetSendWndPageNo = mailViewWndPageNo;
				MailStr.len = 140;
				MailStr.lineLen = 28;
				MailStr.lineDist = 20;
				MailStr.color = 0;
				MailStr.x = pActMenuWnd->x + 12;
				MailStr.y = pActMenuWnd->y + 280 - 136;
				MailStr.fontPrio = FONT_PRIO_FRONT;
			}
			else {
				if (pActMenuWnd->hp > 0) {
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_MAIL_WND_SEND_WND, 1);
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						int nowNo = mailPetSendWndPageNo * MAX_ADR_BOOK_COUNT;
						for (i = 0; i < MAX_ADR_BOOK_COUNT; i++) {
							if (HitFontNo == mailWndFontNo[i]) {
#ifdef _EXTEND_AB
								if (addressBook[nowNo + i].useFlag == TRUE
									|| addressBook[nowNo + i].useFlag == 2
									|| addressBook[nowNo + i].useFlag == 3) {
#else
								if (addressBook[nowNo + i].useFlag == TRUE) {
#endif
									if (mailWndSendFlag[nowNo + i] == FALSE) mailWndSendFlag[nowNo + i] = TRUE;
									else mailWndSendFlag[nowNo + i] = FALSE;
									play_se(217, 320, 240);
								}
							}
						}
						if (HitDispNo == mailWndFontNo[19]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							mailWndNo = mailWndNoBak;
							play_se(203, 320, 240);
							GetKeyInputFocus(&MyChatBuffer);
						}
					}
					if (HitDispNo == mailWndFontNo[18]) {
						if (mouse.onceState & MOUSE_LEFT_CRICK && ItemUseTime < timeGetTime() - 5000) {
							int flag = FALSE;
							ItemUseTime = timeGetTime();
							for (i = 0; i < MAX_ADR_BOOK; i++) {
								if (mailWndSendFlag[i] == TRUE) flag = TRUE;
							}
							if (MailStr.buffer[0] != NULL && flag == TRUE) {
								for (i = 0; i < MAX_ADR_BOOK; i++) {
									if (mailWndSendFlag[i] == TRUE) {
										char moji2[256];
										strcpy(moji2, MailStr.buffer);
										makeEscapeString(moji2, moji, sizeof(moji));
										if (bNewServer)
											lssproto_MSG_send(sockfd, i, moji, FONT_PAL_WHITE);
										else
											old_lssproto_MSG_send(sockfd, i, moji, FONT_PAL_WHITE);

									}
								}
								play_se(101, 320, 240);
								mailWndBtnFlag[18] = TRUE;
							}
							else {
								play_se(220, 320, 240);
							}
						}
						if (mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[18] == TRUE) {
							mailWndBtnFlag[18] = TRUE;
						}
						else {
							mailWndBtnFlag[18] = FALSE;
						}
					}
					else {
						mailWndBtnFlag[18] = FALSE;
					}
					if (HitDispNo == mailWndFontNo[16] || joy_con[0] & JOY_A) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_A) {
							mailPetSendWndPageNo--;
							if (mailPetSendWndPageNo < 0) mailPetSendWndPageNo = MAX_ADR_BOOK_PAGE - 1;
							play_se(217, 320, 240);
							mailWndBtnFlag[16] = TRUE;
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_A) && mailWndBtnFlag[16] == TRUE) {
							mailWndBtnFlag[16] = TRUE;
						}
						else {
							mailWndBtnFlag[16] = FALSE;
						}
					}
					else {
						mailWndBtnFlag[16] = FALSE;
					}
					if (HitDispNo == mailWndFontNo[17] || joy_con[0] & JOY_B) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_B) {
							mailPetSendWndPageNo++;
							if (mailPetSendWndPageNo >= MAX_ADR_BOOK_PAGE) mailPetSendWndPageNo = 0;
							play_se(217, 320, 240);
							mailWndBtnFlag[17] = TRUE;
						}
						// ?????????
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_B) && mailWndBtnFlag[17] == TRUE) {
							// ????
							mailWndBtnFlag[17] = TRUE;
						}
						else {
							// ???
							mailWndBtnFlag[17] = FALSE;
						}
					}
					else {
						// ???
						mailWndBtnFlag[17] = FALSE;
					}

					// ??????
					if (HitDispNo == mailWndFontNo[14]) {
						// ????????
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							// ??? ??
							MailStr.buffer[0] = NULL;
							MailStr.cnt = 0;
							MailStr.cursor = 0;
							// ?????
							play_se(217, 320, 240);
							mailWndBtnFlag[14] = TRUE;
						}
						// ?????????
						if (mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[14] == TRUE) {
							// ????
							mailWndBtnFlag[14] = TRUE;
						}
						else {
							// ???
							mailWndBtnFlag[14] = FALSE;
						}
					}
					else {
						// ???
						mailWndBtnFlag[14] = FALSE;
					}
					// ??????
					if (HitDispNo == mailWndFontNo[15]) {
						// ????????
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							// ????????
							InitMailSendFlag();
							// ?????
							play_se(217, 320, 240);
							mailWndBtnFlag[15] = TRUE;
						}
						// ?????????
						if (mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[15] == TRUE) {
							// ????
							mailWndBtnFlag[15] = TRUE;
						}
						else {
							// ???
							mailWndBtnFlag[15] = FALSE;
						}
					}
					else {
						// ???
						mailWndBtnFlag[15] = FALSE;
					}

					// ????????
					if (pActMenuWnd != NULL) {
						int nowNo = mailPetSendWndPageNo * MAX_ADR_BOOK_COUNT;
						int color;

						// ?
						x = pActMenuWnd->x;
						y = pActMenuWnd->y;
						// ??????
						GetKeyInputFocus(&MailStr);
						// ???
						StockFontBuffer2(&MailStr);

						for (i = 0; i < MAX_ADR_BOOK_COUNT; i++) {
							// ????????????????
#ifdef _EXTEND_AB
							if (addressBook[nowNo + i].useFlag == TRUE
								|| addressBook[nowNo + i].useFlag == 2
								|| addressBook[nowNo + i].useFlag == 3) {
#else
							if (addressBook[nowNo + i].useFlag == TRUE) {
#endif
								color = FONT_PAL_GRAY;	// ?????
														// ???????
								if (addressBook[nowNo + i].onlineFlag > 0) {
									// ?
									color = FONT_PAL_WHITE;
								}
								// ??????
								if (mailWndSendFlag[nowNo + i] == TRUE) {
									// ?
									color = FONT_PAL_AQUA;
								}
								// 
								CenteringStr(addressBook[nowNo + i].name, moji, CHAR_NAME_LEN);
								mailWndFontNo[i] = StockFontBuffer(x + 111, y + 177 - 136, FONT_PRIO_FRONT, color, moji, 2);

							}
							y += 23;
						}
						// ??????
						mailWndFontNo[15] = StockDispBuffer(pActMenuWnd->x + 51, pActMenuWnd->y + 211 - 136, DISP_PRIO_IME3, CG_MAIL_WND_CLEAR_BTN_UP + mailWndBtnFlag[15], 2);
						// ???
						mailWndFontNo[16] = StockDispBuffer(pActMenuWnd->x + 26 - 8 + 4, pActMenuWnd->y + 241 - 132, DISP_PRIO_IME3, CG_PREV_BTN + mailWndBtnFlag[16], 2);
						// ????
						sprintf(moji, "%2d", mailPetSendWndPageNo + 1);
						StockFontBuffer(pActMenuWnd->x + 47 - 8 + 4, pActMenuWnd->y + 233 - 132, FONT_PRIO_FRONT, 0, moji, 0);
						// ????
						mailWndFontNo[17] = StockDispBuffer(pActMenuWnd->x + 76 + 4, pActMenuWnd->y + 241 - 132, DISP_PRIO_IME3, CG_NEXT_BTN + mailWndBtnFlag[17], 2);
						// ??????
						mailWndFontNo[14] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx + 60, pActMenuWnd->y + 390 - 136, DISP_PRIO_IME3, CG_MAIL_WND_CLEAR_BTN_UP + mailWndBtnFlag[14], 2);
						// ????
						mailWndFontNo[18] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx - 60, pActMenuWnd->y + 421 - 136, DISP_PRIO_IME3, CG_SEND_BTN + mailWndBtnFlag[18] * 75, 2);
						// ??
						mailWndFontNo[19] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx + 60, pActMenuWnd->y + 421 - 136, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
					}
				}
			}
			break;

		case MAIL_WND_PET_SEND:
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(4, 4, 272, 440, 0, -1);
				for (i = 0; i < MENU_MAIL_0; i++) mailWndFontNo[i] = -2;
				mailPetSendWndPageNo = mailViewWndPageNo;
				MailStr.len = MAIL_STR_LEN;
				MailStr.lineLen = 28;
				MailStr.lineDist = 20;
				MailStr.color = 0;
				MailStr.x = pActMenuWnd->x + 12;
				MailStr.y = pActMenuWnd->y + 280;
				MailStr.fontPrio = FONT_PRIO_FRONT;
			}
			else {
				if (pActMenuWnd->hp > 0) {
					if (pActPet == NULL) {
						pActPet = MakeAnimDisp(pActMenuWnd->x + 190, pActMenuWnd->y + 120, pet[pc.mailPetNo].graNo, ANIM_DISP_PET);
					}
					if (pActMailItem == NULL && mailItemNo != -1) {
						pActMailItem = MakeAnimDisp(pActMenuWnd->x + 56, pActMenuWnd->y + 96, pc.item[mailItemNo].graNo, ANIM_DISP_PET_ITEM);
						pActMailItem->atr |= ACT_ATR_INFO;
						pActMailItem->atr |= ACT_ATR_TYPE_ITEM;
						pActMailItem->dispPrio = DISP_PRIO_DRAG;
						strcpy(pActMailItem->name, pc.item[mailItemNo].name);
					}
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_MAIL_WND_PET_SEND_WND, 1);
					// ????????
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						int nowNo = mailPetSendWndPageNo * MAX_ADR_BOOK_COUNT;
						// ??????
						for (i = 0; i < MAX_ADR_BOOK_COUNT; i++) {
							// ??????????????????????
							if (HitFontNo == mailWndFontNo[i]) {
								// ????????
#ifdef _EXTEND_AB
								if (addressBook[nowNo + i].useFlag == TRUE
									|| addressBook[nowNo + i].useFlag == 2
									|| addressBook[nowNo + i].useFlag == 3) {
#else								????????
								if (addressBook[nowNo + i].useFlag == TRUE) {
#endif
									int j, flag = FALSE;
									// ??????????
									for (j = 0; j < MAX_ADR_BOOK; j++) {
										if (mailWndSendFlag[j] == TRUE) flag = TRUE;
									}

									// ???????
									if (flag == FALSE) {
										mailWndSendFlag[nowNo + i] = TRUE;
										// ?????
										play_se(217, 320, 240);
									}
									else { // ????????
										   // ????????????????
										if (mailWndSendFlag[nowNo + i] == TRUE) {
											mailWndSendFlag[nowNo + i] = FALSE;
											// ?????
											play_se(217, 320, 240);
										}
										else { // ?????????????????
											   // ???
											play_se(220, 320, 240);
										}
									}
								}
							}
						}
						// ???
						if (HitDispNo == mailWndFontNo[19]) {
							// ??????
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							// ????
							DeathAction(pActPet);
							pActPet = NULL;
							// ?????
							DeathAction(pActMailItem);
							pActMailItem = NULL;
							// ????????
							mailWndNo = MAIL_WND_VIEW;
							// ????????
							play_se(203, 320, 240);
							// ????????
							GetKeyInputFocus(&MyChatBuffer);
						}
						// ???????
						if (HitDispNo == mailWndFontNo[15]) {
							// ??????
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							// ????
							DeathAction(pActPet);
							pActPet = NULL;
							// ?????
							DeathAction(pActMailItem);
							pActMailItem = NULL;
							// ?????????
							mailWndNo = MAIL_WND_ITEM;
							// ????????
							play_se(202, 320, 240);
							// ????????
							GetKeyInputFocus(&MyChatBuffer);
						}
					}
					// ????
					if (HitDispNo == mailWndFontNo[18]) {

						// ????????
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							int flag = FALSE;
							// ??????????
							for (i = 0; i < MAX_ADR_BOOK; i++) {
								if (mailWndSendFlag[i] == TRUE) flag = TRUE;
							}
							// ??????
							if (MailStr.buffer[0] != NULL && flag == TRUE
								&& pActPet != NULL
								&& pActPet->actNo == ANIM_DISP_PET
								&& pc.mailPetNo != ItemMixPetNo) {
								for (i = 0; i < MAX_ADR_BOOK; i++) {
									// ????????
									if (mailWndSendFlag[i] == TRUE) {
										// Terry add fix can send mail to offline character 2004/2/5
										if (addressBook[i].onlineFlag == 0) StockChatBufferLine("친구가 접속해있지 않아요！", FONT_PAL_RED);
										else
											// end
										{
											char moji2[256];
											// ????????
											strcpy(moji2, MailStr.buffer);
											// ???????
											makeEscapeString(moji2, moji, sizeof(moji));
											// ???????
											if (bNewServer)
												lssproto_PMSG_send(sockfd, i, pc.mailPetNo, mailItemNo, moji2, FONT_PAL_WHITE);
											else
												old_lssproto_PMSG_send(sockfd, i, pc.mailPetNo, mailItemNo, moji2, FONT_PAL_WHITE);
											pc.mailPetNo = -1;
											// ??????????
											ItemBuffer[mailItemNo].mixFlag = 0;
											// ????????
											saveUserSetting();
											// ????????????????
											if (pActPet != NULL) {
												// ??????
												pActPet->actNo = 1;
											}
										}
									}
								}
								play_se(101, 320, 240);
								mailWndBtnFlag[18] = TRUE;
							}
							else {
								// ???
								play_se(220, 320, 240);
							}
						}
						// ?????????
						if (mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[18] == TRUE) {
							// ????
							mailWndBtnFlag[18] = TRUE;
						}
						else {
							// ???
							mailWndBtnFlag[18] = FALSE;
						}
					}
					else {
						// ???
						mailWndBtnFlag[18] = FALSE;
					}

					// ???
					if (HitDispNo == mailWndFontNo[16] || joy_con[0] & JOY_A) {
						// ????????
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_A) {
							mailPetSendWndPageNo--; // ?????
							if (mailPetSendWndPageNo < 0) mailPetSendWndPageNo = MAX_ADR_BOOK_PAGE - 1;
							// ?????
							play_se(217, 320, 240);
							mailWndBtnFlag[16] = TRUE;
						}
						// ?????????
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_A) && mailWndBtnFlag[16] == TRUE) {
							// ????
							mailWndBtnFlag[16] = TRUE;
						}
						else {
							mailWndBtnFlag[16] = FALSE;
						}
					}
					else {
						mailWndBtnFlag[16] = FALSE;
					}
					if (HitDispNo == mailWndFontNo[17] || joy_con[0] & JOY_B) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_B) {
							mailPetSendWndPageNo++;
							if (mailPetSendWndPageNo >= MAX_ADR_BOOK_PAGE) mailPetSendWndPageNo = 0;
							play_se(217, 320, 240);
							mailWndBtnFlag[17] = TRUE;
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_B) && mailWndBtnFlag[17] == TRUE) {
							mailWndBtnFlag[17] = TRUE;
						}
						else {
							mailWndBtnFlag[17] = FALSE;
						}
					}
					else {
						mailWndBtnFlag[17] = FALSE;
					}
					if (HitDispNo == mailWndFontNo[14]) {
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							MailStr.buffer[0] = NULL;
							MailStr.cnt = 0;
							MailStr.cursor = 0;
							play_se(217, 320, 240);
							mailWndBtnFlag[14] = TRUE;
						}
						if (mouse.state & MOUSE_LEFT_CRICK && mailWndBtnFlag[14] == TRUE) {
							mailWndBtnFlag[14] = TRUE;
						}
						else {
							mailWndBtnFlag[14] = FALSE;
						}
					}
					else {
						mailWndBtnFlag[14] = FALSE;
					}
					if (pActMenuWnd != NULL) {
						int nowNo = mailPetSendWndPageNo * MAX_ADR_BOOK_COUNT;
						int color;
						x = pActMenuWnd->x;
						y = pActMenuWnd->y;
						GetKeyInputFocus(&MailStr);

						if (pet[pc.mailPetNo].freeName[0] == NULL) {
							CenteringStr(pet[pc.mailPetNo].name, moji, PET_NAME_LEN);
						}
						else {
							CenteringStr(pet[pc.mailPetNo].freeName, moji, PET_NAME_LEN);
						}
						StockFontBuffer(x + 120, y + 14, FONT_PRIO_FRONT, 0, moji, 0);
						StockFontBuffer2(&MailStr);
						for (i = 0; i < MAX_ADR_BOOK_COUNT; i++) {
#ifdef _EXTEND_AB
							if (addressBook[nowNo + i].useFlag == TRUE
								|| addressBook[nowNo + i].useFlag == 2
								|| addressBook[nowNo + i].useFlag == 3) {
#else
							if (addressBook[nowNo + i].useFlag == TRUE) {
#endif
								color = FONT_PAL_GRAY;
								if (addressBook[nowNo + i].onlineFlag > 0) {
									color = FONT_PAL_WHITE;
								}
								if (mailWndSendFlag[nowNo + i] == TRUE) {
									color = FONT_PAL_AQUA;
								}
								CenteringStr(addressBook[nowNo + i].name, moji, CHAR_NAME_LEN);
								mailWndFontNo[i] = StockFontBuffer(x + 111, y + 177, FONT_PRIO_FRONT, color, moji, 2);
							}
							y += 23;
						}
						mailWndFontNo[15] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_IME3, CG_MAIL_WND_ITEM_BTN, 2);
						mailWndFontNo[16] = StockDispBuffer(pActMenuWnd->x + 25 - 8 + 4, pActMenuWnd->y + 241, DISP_PRIO_IME3, CG_PREV_BTN + mailWndBtnFlag[16], 2);
						sprintf(moji, "%2d", mailPetSendWndPageNo + 1);
						StockFontBuffer(pActMenuWnd->x + 46 - 8 + 4, pActMenuWnd->y + 233, FONT_PRIO_FRONT, 0, moji, 0);
						mailWndFontNo[17] = StockDispBuffer(pActMenuWnd->x + 75 + 4, pActMenuWnd->y + 241, DISP_PRIO_IME3, CG_NEXT_BTN + mailWndBtnFlag[17], 2);
						mailWndFontNo[14] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx + 60, pActMenuWnd->y + 390, DISP_PRIO_IME3, CG_MAIL_WND_CLEAR_BTN_UP + mailWndBtnFlag[14], 2);
						mailWndFontNo[18] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx - 60, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_SEND_BTN + mailWndBtnFlag[18] * 75, 2);
						mailWndFontNo[19] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx + 60, pActMenuWnd->y + 421, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
					}
				}
			}
			break;
		case MAIL_WND_ITEM:
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(4, 24, 271, 281, 0, -1);
				for (i = 0; i < MENU_MAIL_0; i++) mailWndFontNo[i] = -2;
				mailSendWndPageNo = 0;
				InitItem(pActMenuWnd->x, pActMenuWnd->y - 1, FALSE);
				if (MenuToggleFlag & JOY_CTRL_I && itemWndNo == 0) {
					MenuToggleFlag &= ~JOY_CTRL_I;
					DeathMenuAction2();
				}
			}
			else {
				if (pActMenuWnd->hp > 0) {
#ifdef _READ16BITBMP
					if (g_bUseAlpha) StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_ITEM_WND_1, 1);
#endif
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my - 80, DISP_PRIO_MENU, CG_ITEM_WND_1, 1);
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my - 80, DISP_PRIO_MENU, CG_BTL_ITEM_WND_TITLE, 1);
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == mailWndFontNo[19]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							mailWndNo = MAIL_WND_PET_SEND;
							play_se(203, 320, 240);
						}
					}
					if (pActMenuWnd != NULL) {
						x = pActMenuWnd->x + 16;
						y = pActMenuWnd->y + 191 + 6;
						for (i = MAX_ITEM - 1; i >= MAX_ITEMSTART; i--) {
							if (MakeHitBox(ItemBuffer[i].defX - 24, ItemBuffer[i].defY - 24 - 160,
								ItemBuffer[i].defX + 25, ItemBuffer[i].defY + 23 - 160, DISP_PRIO_IME3) == TRUE) {
								if (pc.item[i].useFlag == TRUE) {
									char *splitPoint = pc.item[i].memo;
									int color = pc.item[i].color;
									if (pc.level < pc.item[i].level) color = FONT_PAL_RED;
									StockFontBuffer(pActMenuWnd->x + 16, pActMenuWnd->y + 332 - 160 - 1, FONT_PRIO_FRONT, color, pc.item[i].name, 0);
									while (1) {
										if (strlen(splitPoint) > 28) {
											strncpy(moji, splitPoint, 28);
											moji[28] = NULL;
											if (GetStrLastByte(moji) == 3) {
												moji[27] = NULL;
												splitPoint += 27;
											}
											else {
												moji[28] = NULL;
												splitPoint += 28;
											}
											StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 0); y += 24;
										}
										else {
											strcpy(moji, splitPoint);
											StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 0);
											break;
										}
									}
								}
								if (mouse.onceState & MOUSE_LEFT_DBL_CRICK) {
									if (pc.item[i].useFlag == TRUE && pc.item[i].sendFlag & ITEM_FLAG_PET_MAIL) {
										mailItemNo = i;
										DeathAction(pActMenuWnd);
										pActMenuWnd = NULL;
										mailWndNo = MAIL_WND_PET_SEND;
										play_se(203, 320, 240);
										GetKeyInputFocus(&MyChatBuffer);
									}
									else {
										play_se(220, 320, 240);
									}
								}
							}
							if (pc.item[i].useFlag == TRUE) {
								StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY - 160, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
							}
						}
						if (pActMenuWnd != NULL) {
							mailWndFontNo[19] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, pActMenuWnd->y + 262, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
						}
					}
				}
			}
			break;
		case MAIL_WND_HISTORY:
			if (pActMenuWnd == NULL) {
				pActMenuWnd = MakeWindowDisp(4, 4, 272, 424, 0, -1);
				for (i = 0; i < MENU_MAIL_0; i++) mailWndFontNo[i] = -2;
				for (mailHistoryWndPageNo = MAIL_MAX_HISTORY - 1; mailHistoryWndPageNo > 0; mailHistoryWndPageNo--) {
					if (mailHistoryWndPageNo < 0) mailHistoryWndPageNo = MAIL_MAX_HISTORY - 1;
					mailHistoryWndNowPageNo = (MailHistory[mailHistoryWndSelectNo].newHistoryNo + mailHistoryWndPageNo) % MAIL_MAX_HISTORY;
					if (MailHistory[mailHistoryWndSelectNo].noReadFlag[mailHistoryWndNowPageNo] >= TRUE) break;
				}
#if 0
				if (pActPet == NULL) {
					pActPet = MakeAnimDisp(pActMenuWnd->x - 204, pActMenuWnd->y + 160, pet[pc.mailPetNo].graNo, ANIM_DISP_PET_RECIEVE);
					pActPet->atr |= ACT_ATR_INFO;
					pActPet->atr |= ACT_ATR_TYPE_PET;
					pActPet->level = 32;
					strcpy(pActPet->name, "测试宠物");
				}
				if (pActMailItem == NULL && MailHistory[mailHistoryWndSelectNo].itemGraNo[mailHistoryWndNowPageNo] != -1) {
					pActMailItem = MakeAnimDisp(pActMenuWnd->x - 204, pActMenuWnd->y + 128, pc.item[5].graNo, ANIM_DISP_PET_ITEM);
					pActMailItem->atr |= ACT_ATR_INFO;
					pActMailItem->atr |= ACT_ATR_TYPE_ITEM;
					pActMailItem->dispPrio = DISP_PRIO_DRAG;
					strcpy(pActMailItem->name, "道具完成");
				}
#endif
			}
			else {
				if (pActMenuWnd->hp > 0) {
					static int nowDelNo;
					StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd->pYobi)->my, DISP_PRIO_MENU, CG_MAIL_WND_HISTORY_WND, 1);
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == mailWndFontNo[18]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathLetterAction();
							DeathAction(pActPet);
							pActPet = NULL;
							DeathAction(pActMailItem);
							pActMailItem = NULL;
							mailWndNoBak = mailWndNo;
							InitMailSendFlag();
							mailWndSendFlag[mailHistoryWndSelectNo] = TRUE;
							if (ProcNo == PROC_BATTLE) {
								mailWndNo = MAIL_WND_SEND;
							}
							else {
								if (pc.mailPetNo != -1) {
									mailWndNo = MAIL_WND_SELECT;
								}
								else {
									mailWndNo = MAIL_WND_SEND;
								}
							}
							play_se(202, 320, 240);
						}
						if (HitDispNo == mailWndFontNo[19]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							DeathLetterAction();
							DeathAction(pActPet);
							pActPet = NULL;
							DeathAction(pActMailItem);
							pActMailItem = NULL;
							mailWndNo = MAIL_WND_VIEW;
							play_se(203, 320, 240);
						}
					}
					if (HitDispNo == mailWndFontNo[16] || joy_con[0] & JOY_A) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_A) {
							mailHistoryWndPageNo--;
							if (mailHistoryWndPageNo < 0) mailHistoryWndPageNo = MAIL_MAX_HISTORY - 1;
							DeathLetterAction();
							DeathAction(pActPet);
							pActPet = NULL;
							DeathAction(pActMailItem);
							pActMailItem = NULL;
							play_se(217, 320, 240);
							mailWndBtnFlag[16] = TRUE;
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_A) && mailWndBtnFlag[16] == TRUE) {
							mailWndBtnFlag[16] = TRUE;
						}
						else {
							mailWndBtnFlag[16] = FALSE;
						}
					}
					else {
						mailWndBtnFlag[16] = FALSE;
					}
					if (HitDispNo == mailWndFontNo[17] || joy_con[0] & JOY_B) {
						if (mouse.autoState & MOUSE_LEFT_CRICK || joy_auto[0] & JOY_B) {
							mailHistoryWndPageNo++;
							if (mailHistoryWndPageNo >= MAIL_MAX_HISTORY) mailHistoryWndPageNo = 0;
							DeathLetterAction();
							DeathAction(pActPet);
							pActPet = NULL;
							DeathAction(pActMailItem);
							pActMailItem = NULL;
							play_se(217, 320, 240);
							mailWndBtnFlag[17] = TRUE;
						}
						if ((mouse.state & MOUSE_LEFT_CRICK || joy_con[0] & JOY_B) && mailWndBtnFlag[17] == TRUE) {
							mailWndBtnFlag[17] = TRUE;
						}
						else {
							mailWndBtnFlag[17] = FALSE;
						}
					}
					else {
						mailWndBtnFlag[17] = FALSE;
					}
					if (pActMenuWnd != NULL) {
						x = pActMenuWnd->x;
						y = pActMenuWnd->y;
						char *splitPoint;
						mailHistoryWndNowPageNo = (MailHistory[mailHistoryWndSelectNo].newHistoryNo + mailHistoryWndPageNo) % MAIL_MAX_HISTORY;
						if (MailHistory[mailHistoryWndSelectNo].noReadFlag[mailHistoryWndNowPageNo] >= TRUE) {
							if (MailHistory[mailHistoryWndSelectNo].noReadFlag[mailHistoryWndNowPageNo] > TRUE) {
								if (pActPet == NULL) {
									pActPet = MakeAnimDisp(pActMenuWnd->x - 204, pActMenuWnd->y + 160, MailHistory[mailHistoryWndSelectNo].noReadFlag[mailHistoryWndNowPageNo], ANIM_DISP_PET_RECIEVE);
									pActPet->atr |= ACT_ATR_INFO;
									pActPet->atr |= ACT_ATR_TYPE_PET;
									pActPet->level = MailHistory[mailHistoryWndSelectNo].petLevel[mailHistoryWndNowPageNo];
									strcpy(pActPet->name, MailHistory[mailHistoryWndSelectNo].petName[mailHistoryWndNowPageNo]);
								}
								if (pActMailItem == NULL && MailHistory[mailHistoryWndSelectNo].itemGraNo[mailHistoryWndNowPageNo] != -1) {
									pActMailItem = MakeAnimDisp(pActMenuWnd->x - 204, pActMenuWnd->y + 128, MailHistory[mailHistoryWndSelectNo].itemGraNo[mailHistoryWndNowPageNo], ANIM_DISP_PET_ITEM);
									pActMailItem->atr |= ACT_ATR_INFO;
									pActMailItem->atr |= ACT_ATR_TYPE_ITEM;
									pActMailItem->dispPrio = DISP_PRIO_DRAG;
									strcpy(pActMailItem->name, "아이템 완성");
								}
							}
							if (pActLetter[0] == NULL) {
								pActLetter[0] = MakeAnimDisp(x + 28, y + 244, SPR_mail, ANIM_DISP_LETTER);
							}
							MailHistory[mailHistoryWndSelectNo].noReadFlag[mailHistoryWndNowPageNo] = FALSE;
							SaveMailHistory(mailHistoryWndSelectNo);
							mailLamp = CheckMailNoReadFlag();

						}
						splitPoint = MailHistory[mailHistoryWndSelectNo].str[mailHistoryWndNowPageNo];
						CenteringStr(addressBook[mailHistoryWndSelectNo].name, moji, CHAR_NAME_LEN);
						StockFontBuffer(x + 64, y + 34, FONT_PRIO_FRONT, 0, moji, 0);
						StockFontBuffer(x + 148, y + 236, FONT_PRIO_FRONT, 0, MailHistory[mailHistoryWndSelectNo].dateStr[mailHistoryWndNowPageNo], 0);
						while (1) {
							if (strlen(splitPoint) > 28) {
								strncpy(moji, splitPoint, 28);
								moji[28] = NULL;
								if (GetStrLastByte(moji) == 3) {
									moji[27] = NULL;
									splitPoint += 27;
								}
								else {
									moji[28] = NULL;
									splitPoint += 28;
								}
								StockFontBuffer(x + 12, y + 260, FONT_PRIO_FRONT, 0, moji, 0); y += 20;
							}
							else {
								strcpy(moji, splitPoint);
								StockFontBuffer(x + 12, y + 260, FONT_PRIO_FRONT, 0, moji, 0);
								break;
							}
						}
						mailWndFontNo[16] = StockDispBuffer(pActMenuWnd->x + 25 + 16 + 8, pActMenuWnd->y + 405, DISP_PRIO_IME3, CG_PREV_BTN + mailWndBtnFlag[16], 2);
						sprintf(moji, "%2d", mailHistoryWndPageNo + 1);
						StockFontBuffer(pActMenuWnd->x + 46 + 16 + 9, pActMenuWnd->y + 397, FONT_PRIO_FRONT, 0, moji, 0);
						mailWndFontNo[17] = StockDispBuffer(pActMenuWnd->x + 75 + 16 + 9 + 8, pActMenuWnd->y + 405, DISP_PRIO_IME3, CG_NEXT_BTN + mailWndBtnFlag[17], 2);
						mailWndFontNo[18] = StockDispBuffer(pActMenuWnd->x + 196, pActMenuWnd->y + 371, DISP_PRIO_IME3, CG_MAIL_WND_MAIL_BTN, 2);
						mailWndFontNo[19] = StockDispBuffer(pActMenuWnd->x + 220 - 16 - 8, pActMenuWnd->y + 405, DISP_PRIO_IME3, CG_RETURN_BTN, 2);
					}
				}
			}
			break;


		}
	}

	if (MenuToggleFlag & JOY_CTRL_A) {

		if (PetRanking::Instance().Check())
		{
			PetRanking::Instance().Proc();
		}
	}
	else
	{
		if (PetRanking::Instance().Check())
		{
			PetRanking::Instance().SetCheck(0);
		}
	}

	//? ??????? ***********************************************************/
	if (BattleResultWndFlag >= 1 && ProcNo == PROC_GAME && SubProcNo == 3) {
		if (--ResultWndTimer <= 0) {
			DeathAction(pActMenuWnd);
			pActMenuWnd = NULL;
			BattleResultWndFlag = FALSE;
			if (MapWmdFlagBak != TRUE) {
				play_se(203, 320, 240);
			}
		}

		switch (BattleResultWndFlag) {
		case 1:
			if (pActMenuWnd == NULL) {
				int flag = 0;
				pActMenuWnd = MakeWindowDisp(320 - 160, 240 - 120, 5, 6, CG_WND_TITLE_RESULT, 2);
				for (i = 0; i < MENU_BTL_RESULT_0; i++) resultWndFontNo[i] = -2;
				for (i = 0; i < 4; i++) {
					if (battleResultMsg.resChr[i].levelUp == TRUE) flag = TRUE;
				}
				if (flag == TRUE) {
					play_se(211, 320, 240);
				}
				else {
					play_se(202, 320, 240);
				}
			}
			else {
				if (pActMenuWnd->hp > 0) {
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == resultWndFontNo[0]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							BattleResultWndFlag = FALSE;
							if (MapWmdFlagBak != TRUE) {
								play_se(203, 320, 240);
							}
						}
					}
					if (pActMenuWnd != NULL) {
						int color;
						int flag, j;
						x = pActMenuWnd->x + 12;
						y = pActMenuWnd->y + 44;
						CenteringStr(pc.name, moji, CHAR_NAME_LEN);
						StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 0);
						color = FONT_PAL_GRAY;
						if (battleResultMsg.resChr[0].levelUp == TRUE) {
							StockFontBuffer(x + 152, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "LvUp!", 0);
						}
						sprintf(moji, "Exp %+5d", battleResultMsg.resChr[0].exp);
						StockFontBuffer(x + 148 + 58, y, FONT_PRIO_FRONT, 0, moji, 0);
						y += 20;
						for (i = 0; i < 5; i++) {
							if (pet[i].useFlag == TRUE && (pc.selectPetNo[i] == TRUE || pc.ridePetNo == i)) {
								if (pet[i].freeName[0] != NULL)
									CenteringStr(pet[i].freeName, moji, PET_NAME_LEN);
								else
									CenteringStr(pet[i].name, moji, PET_NAME_LEN);
								StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 0);
								color = FONT_PAL_GRAY;
								flag = FALSE;
								for (j = 1; j < RESULT_CHR_EXP; j++) {
									if (battleResultMsg.resChr[j].petNo == i) {
										if (battleResultMsg.resChr[j].levelUp == TRUE) {
											StockFontBuffer(x + 152, y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "LvUp!", 0);
										}
										sprintf(moji, "Exp %+5d", battleResultMsg.resChr[j].exp);
										StockFontBuffer(x + 148 + 58, y, FONT_PRIO_FRONT, 0, moji, 0);
										flag = TRUE;
									}
								}
								if (flag == FALSE) {
									sprintf(moji, "Exp %+4d", battleResultMsg.resChr[j].exp);
									StockFontBuffer(x + 148 + 58, y, FONT_PRIO_FRONT, 0, "Exp    +0", 0);
								}
								y += 20;
							}
						}
						flag = FALSE;
						y = pActMenuWnd->y + 134;
						StockFontBuffer(x, y + 30, FONT_PRIO_FRONT, 0, "  아이템 ：", 0);
						char itemName[128];
						memset(itemName, 0, 128);
						for (i = 0; i < 3; i++) {
							if (battleResultMsg.item[i][0] != NULL) {
								StockFontBuffer(x + 85, y + 30, FONT_PRIO_FRONT, 0, battleResultMsg.item[i], 0);
								y += 22;
								flag = TRUE;
							}
						}
						if (flag == FALSE) {
							StockFontBuffer(x + 85, y + 30, FONT_PRIO_FRONT, 0, "아이템을 얻지 못하였습니다。", 0);
						}
						resultWndFontNo[0] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, pActMenuWnd->y + 216 + 30, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);

					}
				}
			}
			break;

		case 2:
			if (pActMenuWnd == NULL) {
				int flag = 0;
				pActMenuWnd = MakeWindowDisp(320 - 96, 240 - 72, 3, 3, CG_WND_TITLE_RESULT, 2);
				for (i = 0; i < MENU_BTL_RESULT_0; i++) resultWndFontNo[i] = -2;
				play_se(202, 320, 240);
			}
			else {
				if (pActMenuWnd->hp > 0) {
					if (mouse.onceState & MOUSE_LEFT_CRICK) {
						if (HitDispNo == resultWndFontNo[0]) {
							DeathAction(pActMenuWnd);
							pActMenuWnd = NULL;
							BattleResultWndFlag = FALSE;
							if (MapWmdFlagBak != TRUE) {
								play_se(203, 320, 240);
							}
						}
					}
					if (pActMenuWnd != NULL) {
						int color = FONT_PAL_WHITE;
						x = pActMenuWnd->x + 17;
						y = pActMenuWnd->y + 48;
						sprintf(moji, " Ｄ  Ｐ  %+8d", battleResultMsg.resChr[0].exp);
						StockFontBuffer(x, y, FONT_PRIO_FRONT, color, moji, 0);
						y += 28;
						sprintf(moji, " 합  계  %8d", battleResultMsg.resChr[1].exp);
						StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, moji, 0);
						resultWndFontNo[0] = StockDispBuffer(((WINDOW_DISP *)pActMenuWnd->pYobi)->mx, pActMenuWnd->y + 120, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
					}
				}
			}
			break;
		}
	}

	//以下开始为交易视窗部分
	if (MenuToggleFlag & JOY_CTRL_T)
	{
		char buffer[1024];
#ifdef _TRADESYSTEM2	// Syu ADD 新交易系统
		char buffer2[1024];
		int j;
#endif
#ifdef _PET_ITEM
		static BOOL	bViewPetEquip = FALSE;
#endif
		switch (tradeWndNo) {
		case 0:
			//视窗为产生时初始化
			if (pActMenuWnd4 == NULL) {
				DeathMenuAction();
				DeathMenuAction2();
				//手动输入金额buff初始化
				TradeBuffer.buffer[0] = NULL;
				TradeBuffer.cnt = 0;
				TradeBuffer.cursor = 0;
				TradeBuffer.len = 8;
				TradeBuffer.color = 0;
				TradeBuffer.x = 600;
				TradeBuffer.y = 120;
				TradeBuffer.fontPrio = FONT_PRIO_FRONT;
				x = (lpDraw->xSize - 620) / 2;
				y = (lpDraw->ySize - 456) / 2;
				pActMenuWnd4 = MakeWindowDisp(x, y, 620, 456, 0, -1);

#ifdef _CHANGETRADERULE		   // (不可开) Syu ADD 交易规则修订
				TradeBtnflag = false;
#endif
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
				//状态初始化
				locknum = -1;
				locknum2 = -1;
				MainTradeWndflag = true;
				SecondTradeWndflag = false;




				//andy_add 20030610
				for (i = 0; i < MENU_TRADE_0; i++) tradeWndFontNo[i] = -2;
				for (i = 0; i < 6; i++) SecondtradeWndFontNo[i] = -2;

				for (i = 0; i < 43; i++)
					tradeList[i].data = -1;
#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
				//视窗开启清空内容
				tradetalkwndflag = false;
				for (i = 0; i < 4; i++)
					sprintf(talkmsg[i], "");
#endif
				drag1Y = 67;
				drag2Y = 257;
				x = (lpDraw->xSize - 620) / 2;
				y = (lpDraw->ySize - 456) / 2;
				SecondTradeWnd = MakeWindowDisp(x, y, 620, 456, 0, -1);
				for (i = 0; i <= 42; i++) {
					tradeList[i].data = -1;
					tradeList[i].kind = 'S';
				}
				for (i = 0; i < 21; i++) {
					mytradelist[i] = -1;
					opptradelist[i] = -1;
				}

				for (i = 0; i < 15; i++) {
#ifdef _ITEM_PILENUMS
					itemflag[i] = pc.item[i + MAX_ITEMSTART].pile;
#else
					itemflag[i] = 0;
#endif
					strcpy(opp_item[i].damage, "");
					strcpy(opp_item[i].effect, "");
					strcpy(opp_item[i].name, "");
				}
#endif

				InitItem3(325, 230);
				g_bTradesystemOpen = true;
			}
			else {

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
				// 主视窗内容
				if (MainTradeWndflag == true) {
					SecondTradeWndflag = false;
#endif
					if (pActMenuWnd4->hp > 0) {
						//产生交易主视窗
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
					//回主视窗时清除检视视窗PetAction
						if (SecondActPet != NULL) {
							DeathAction(SecondActPet);
							SecondActPet = NULL;
						}
						StockDispBuffer(((WINDOW_DISP *)pActMenuWnd4->pYobi)->mx, ((WINDOW_DISP *)pActMenuWnd4->pYobi)->my - 20, DISP_PRIO_MENU, CG_TRADE_WND, 1);
#endif
						// 以下为滑鼠左键被按下时 Start
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							// 处理취소键 Start
							//处理按下취소键
							if (HitDispNo == tradeWndFontNo[1]) {
								//关闭交易视窗
								MenuToggleFlag ^= JOY_CTRL_T;
								//视窗关闭音效
								play_se(203, 320, 240);
								sprintf(buffer, "W|%s|%s", opp_sockfd, opp_name);
								//送出취소讯息给Server通知对方
								lssproto_TD_send(sockfd, buffer);
								tradeStatus = 0;
								tradeInit();
								pc.trade_confirm = 1;
							}
							// 处理취소键 End						


							// 处理 Lock 、 확인键 Start
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
						//主视窗与检视视窗共用Lock跟확인function
							LockAndOkfunction();
							// 处理 Lock 、 확인键 End

							//主副视窗切换钮
							if (HitDispNo == tradeWndFontNo[21]) {
								MainTradeWndflag = false;
								SecondTradeWndflag = true;
								//andy_add 20030610
								for (i = 0; i < MENU_TRADE_0; i++) tradeWndFontNo[i] = -2;
								for (i = 0; i < 6; i++) SecondtradeWndFontNo[i] = -2;

							}
							//对方物品向上卷动钮
							if (HitDispNo == tradeWndFontNo[18]) {
								if (drag1Y >= 67)
									drag1Y -= 4;
								if (drag1Y < 67)
									drag1Y = 67;
							}
							//对方物品向下卷动钮
							if (HitDispNo == tradeWndFontNo[19]) {
								if (drag1Y <= 151)
									drag1Y += 4;
								if (drag1Y > 151)
									drag1Y = 151;
							}
							//我方物品向上卷动钮
							if (HitDispNo == tradeWndFontNo[15]) {
								if (drag2Y >= 257)
									drag2Y -= 4;
								if (drag2Y < 257)
									drag2Y = 257;
							}
							//我方物品向下卷动钮
							if (HitDispNo == tradeWndFontNo[16]) {
								if (drag2Y <= 341)
									drag2Y += 4;
								if (drag2Y > 341)
									drag2Y = 341;
							}
							//我方物品拖曳启动
							if (HitDispNo == tradeWndFontNo[20]) {
								if (!dragflag1) {
									tmpdrag1Y = drag1Y;
									dragflag1 = true;
								}
							}

							//对方物品拖曳启动
							if (HitDispNo == tradeWndFontNo[17]) {
								if (!dragflag2) {
									tmpdrag2Y = drag2Y;
									dragflag2 = true;
								}

							}
#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
							//拖曳钮
							if (HitDispNo == tradeWndFontNo[14])
								talkwndflag = true;
							//关闭钮
#endif
#endif
						}
						// 以上为滑鼠左键被按下时 End


#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
					//我方拖曳钮界线限制
						if (dragflag1 == true &&
							(tmpdrag1Y - mouse.crickLeftDownPoint.y + mouse.nowPoint.y) <= 151 &&
							(tmpdrag1Y - mouse.crickLeftDownPoint.y + mouse.nowPoint.y) >= 67) {
							drag1Y = tmpdrag1Y - mouse.crickLeftDownPoint.y + mouse.nowPoint.y;
						}
						//对方拖曳钮界线限制
						if (dragflag2 == true &&
							(tmpdrag2Y - mouse.crickLeftDownPoint.y + mouse.nowPoint.y) <= 341 &&
							(tmpdrag2Y - mouse.crickLeftDownPoint.y + mouse.nowPoint.y) >= 257) {
							drag2Y = tmpdrag2Y - mouse.crickLeftDownPoint.y + mouse.nowPoint.y;
						}
#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
						//对话方框拖曳位移
						if (talkwndflag == true) {
							talkwndx = mouse.nowPoint.x;
							talkwndy = mouse.nowPoint.y;
						}
						if (mouse.onceState & MOUSE_RIGHT_CRICK)
							tradetalkwndflag = false;
#endif
#endif
						// 处理宠物选取左键 Start
						// 处理宠物选取左键
						if (HitDispNo == tradeWndFontNo[2]) {
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								tradeWndBtnFlag[2] = TRUE;
								int i = 0;
								while (1) {
									tradePetIndex++;
									if (tradePetIndex >= 5) tradePetIndex = 0;
									if (pet[tradePetIndex].useFlag != NULL
										&& pc.ridePetNo != tradePetIndex)
										break;
									if ((i++) > 5) {
										tradePetIndex = -1;
										break;
									}
								}
								if (pActPet3 != NULL) {
									DeathAction(pActPet3);
									pActPet3 = NULL;
								}

							}
						}
						// 处理宠物选取左键 End

						// 处理宠物选取右键 Start
						// 处理宠物选取右键
						if (HitDispNo == tradeWndFontNo[3]) {
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								tradeWndBtnFlag[3] = TRUE;
								int i = 0;
								while (1) {
									tradePetIndex--;
									if (tradePetIndex < 0) tradePetIndex = 4;
									if (pet[tradePetIndex].useFlag != NULL
										&& pc.ridePetNo != tradePetIndex)
										break;
									if ((i++) > 5) {
										tradePetIndex = -1;
										break;
									}
								}
								if (pActPet3 != NULL) {
									DeathAction(pActPet3);
									pActPet3 = NULL;
								}

							}
						}

						// 处理宠物选取右键 End

						// 处理金额增加键 Start
						if (HitDispNo == tradeWndFontNo[4]) {
							if (mouse.onceState & MOUSE_LEFT_CRICK_UP && tradeWndBtnFlag[4] == TRUE)
								tradeWndBtnFlag[4] = FALSE;
							if (tradeWndBtnFlag[4] == TRUE) {
								// 按下增加时将Focus还给ChatBuffer
								GetKeyInputFocus(&MyChatBuffer);
								// 视窗消灭时还原设定
								Tradeflag = false;
								tradeWndDropGold += tradeWndDropGoldInc;
								tradeWndDropGoldCnt++;
								if (tradeWndDropGoldCnt >= 30) {
									tradeWndDropGoldCnt = 0;
									if (tradeWndDropGoldInc == 0)
										tradeWndDropGoldInc = 1;
									else {
										tradeWndDropGoldInc *= 5;
										if (tradeWndDropGoldInc > 10000)
											tradeWndDropGoldInc = 10000;
									}
								}
								//金额上限确认
								if (tradeWndDropGold >= pc.gold) {
									tradeWndDropGold = pc.gold;
									play_se(220, 320, 240);
								}
							}
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								tradeWndDropGold++;
								if (tradeWndDropGold >= pc.gold) {
									tradeWndDropGold = pc.gold;
									play_se(220, 320, 240);
								}
								else {
									tradeWndDropGoldInc = 0;
									tradeWndDropGoldCnt = 0;
									tradeWndBtnFlag[4] = TRUE;
									play_se(217, 320, 240);
								}
							}
						}
						else
							tradeWndBtnFlag[4] = FALSE;
						// 处理金额增加键 End

						// 处理金额减少键 Start
						if (HitDispNo == tradeWndFontNo[5]) {
							if (mouse.onceState & MOUSE_LEFT_CRICK_UP && tradeWndBtnFlag[5] == TRUE)
								tradeWndBtnFlag[5] = FALSE;
							if (tradeWndBtnFlag[5] == TRUE) {
								// 按下减少时将Focus还给ChatBuffer
								GetKeyInputFocus(&MyChatBuffer);
								// 视窗消灭时还原设定
								Tradeflag = false;
								tradeWndDropGold -= tradeWndDropGoldInc;
								tradeWndDropGoldCnt++;
								if (tradeWndDropGoldCnt >= 30) {
									tradeWndDropGoldCnt = 0;
									if (tradeWndDropGoldInc == 0) {
										tradeWndDropGoldInc = 1;
									}
									else {
										tradeWndDropGoldInc *= 5;
										if (tradeWndDropGoldInc > 10000)
											tradeWndDropGoldInc = 10000;
									}
								}
								if (tradeWndDropGold < 0) {
									tradeWndDropGold = 0;
									play_se(220, 320, 240);
								}
							}
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								tradeWndDropGold--;
								if (tradeWndDropGold <= 0) {
									tradeWndDropGold = 0;
									play_se(220, 320, 240);
								}
								else {
									tradeWndDropGoldInc = 0;
									tradeWndDropGoldCnt = 0;
									tradeWndBtnFlag[5] = TRUE;
									play_se(217, 320, 240);
								}
							}
						}
						else
							tradeWndBtnFlag[5] = FALSE;
						// 处理金额减少键 End

						// 处理金额放置键 Start
						if (HitDispNo == tradeWndFontNo[6]) {
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								// 按下放置时将Focus还给ChatBuffer
								GetKeyInputFocus(&MyChatBuffer);
								// 视窗消灭时还原设定
								Tradeflag = false;
								if (tradeWndDropGold > 0 && eventWarpSendFlag == FALSE) {
									tradeWndBtnFlag[6] = TRUE;
									play_se(217, 320, 240);
								}
								else
									play_se(220, 320, 240);
							}
							if (mouse.onceState & MOUSE_LEFT_CRICK_UP && tradeWndBtnFlag[6] == TRUE) {
								if (tradeStatus != 1)	return;
								if (pc.trade_confirm == 2 || pc.trade_confirm == 4) return;
								tradeWndBtnFlag[6] = FALSE;
								int TradeGoldIndex = 0;

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
								if ((tradeList[21].data == -1)) {
									tradeList[21].data = tradeWndDropGold;
									tradeList[21].kind = 'G';
									TradeGoldIndex = 3;
								}
#endif

								if (TradeGoldIndex != 0) {
#ifdef _CHANGETRADERULE		   // (不可开) Syu ADD 交易规则修订
									TradeBtnflag = true;
#endif
									play_se(212, 320, 240);
									tradeWndDropGoldSend = tradeWndDropGold;
									sprintf(buffer, "T|%s|%s|G|%d|%d", opp_sockfd, opp_name, TradeGoldIndex, tradeWndDropGoldSend);
									lssproto_TD_send(sockfd, buffer);
									tradeWndDropGold = 0;
								}
							}
						}
						else
							tradeWndBtnFlag[6] = FALSE;
						// 处理金额放置键 End

						// 处理宠物放置键 Start		
						if (HitDispNo == tradeWndFontNo[7]) {
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
							tradeWndBtnFlag[7] = TRUE;
#endif
							if (mouse.onceState & MOUSE_LEFT_CRICK_UP) {
								if (tradeStatus != 1)	return;
								if (pc.trade_confirm == 2 || pc.trade_confirm == 4) return;

								if (pet[tradePetIndex].useFlag &&  pc.ridePetNo != tradePetIndex) {
									if (pet[tradePetIndex].freeName[0] != NULL)
										strcpy(tradePet[0].name, pet[tradePetIndex].freeName);
									else
										strcpy(tradePet[0].name, pet[tradePetIndex].name);
									tradePet[0].level = pet[tradePetIndex].level;
									tradePet[0].atk = pet[tradePetIndex].atk;
									tradePet[0].def = pet[tradePetIndex].def;
									tradePet[0].quick = pet[tradePetIndex].quick;
									tradePet[0].graNo = pet[tradePetIndex].graNo;
									tradePet[0].trn = pet[tradePetIndex].trn;
									tradePet[0].maxHp = pet[tradePetIndex].maxHp;

									tradePet[0].index = tradePetIndex;
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
									pet[tradePetIndex].useFlag = NULL;
									if (pActPet3)
									{
										DeathAction(pActPet3);
										pActPet3 = NULL;
									}
#endif


									DeathAction(pActPet4);
									pActPet4 = NULL;
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
									tradeList[tradePetIndex + 16].data = tradePetIndex;
									tradeList[tradePetIndex + 16].kind = 'P';
									strcpy(tradeList[tradePetIndex + 16].name, pet[tradePetIndex].name);
									tradeList[tradePetIndex + 16].level = pet[tradePetIndex].level;
									tradeList[tradePetIndex + 16].trns = pet[tradePetIndex].trn;
#ifdef _SHOW_FUSION
									tradeList[tradePetIndex + 16].fusion = pet[tradePetIndex].fusion;
#endif									

									if (pet[tradePetIndex].freeName[0] != NULL)
										strcpy(tradeList[tradePetIndex + 16].freename, pet[tradePetIndex].freeName);
									else
										strcpy(tradeList[tradePetIndex + 16].freename, "");// change fix 防止未改名的宠物显示出上一个丢掉宠物的改名bug
#endif

									if (tradeStatus) {
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
										//增加送出欲交易宠物的技能、原名、更改名
										sprintf(buffer2, "%s|%s|%s|%s|%s|%s|%s|%s|%s",
											petSkill[tradePetIndex][0].name,
											petSkill[tradePetIndex][1].name,
											petSkill[tradePetIndex][2].name,
											petSkill[tradePetIndex][3].name,
											petSkill[tradePetIndex][4].name,
											petSkill[tradePetIndex][5].name,
											petSkill[tradePetIndex][6].name,
											pet[tradePetIndex].name,
											pet[tradePetIndex].freeName
										);
										sprintf(buffer, "T|%s|%s|P|3|%d|%s", opp_sockfd, opp_name, tradePetIndex, buffer2);
#endif
										lssproto_TD_send(sockfd, buffer);
									}
								}
							}
						}
						// 处理宠物放置键 End

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
					//滑鼠左键放掉취소所有拖曳旗标
						if (mouse.onceState & MOUSE_LEFT_CRICK_UP) {
							dragflag1 = false;
							dragflag2 = false;
#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
							talkwndflag = false;
#endif
						}
#endif
						//未被按下的钮全部还原
						for (i = 2; i <= 8; i++) {
							if (mouse.state & MOUSE_LEFT_CRICK && tradeWndBtnFlag[i] == TRUE)
								tradeWndBtnFlag[i] = TRUE;
							else
								tradeWndBtnFlag[i] = FALSE;
						}
						// 产生主视窗各零件
						if (pActMenuWnd4 != NULL) {
							// 取得视窗基准X , Y座标
							x = pActMenuWnd4->x;
							y = pActMenuWnd4->y;
							// 个人金额最大值显示

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
						//对话框内容
							if (tradetalkwndflag != false) {
								for (i = 3; i >= 0; i--) {
									sprintf(moji, "%s", talkmsg[i]);
									StockFontBuffer(talkwndx - 154, talkwndy - 26 + i * 20, FONT_PRIO_AFRONT, FONT_PAL_YELLOW, moji, 0);
								}
								tradeWndFontNo[14] = StockDispBuffer(talkwndx, talkwndy, DISP_PRIO_BOX3, CG_TRADE_TALK_WND, 2);
							}
#endif						
							int ShowPoint;
							int j = 0;
							//我方要显示的起始位置
							ShowPoint = (drag2Y - 257) / 4;
							//建立我方List清单
							for (i = 0; i < 21; i++) {
								if (tradeList[i + 1].data != -1) {
									mytradelist[j] = i + 1;
									j++;
								}
							}
							j = 0;
							//建立对方List清单
							for (i = 0; i < 21; i++) {
								if (tradeList[i + 22].data != -1) {
									opptradelist[j] = i + 22;
									j++;
								}
							}
							j = 0;
							//显示交易清单
							for (i = ShowPoint; i < ShowPoint + 5; i++) {
								if (mytradelist[i] == -1) break;
								if (i > 21) break;
								//说明框用的HitBox暂时不做
								//								if( MakeHitBox( x + 10 , y + 226 + j * 29, x + 280  , y + 244 + j * 29 , DISP_PRIO_IME3 ) == TRUE ){
								//								}
								sprintf(moji, "%c", tradeList[mytradelist[i]].kind);
								//显示道具
								if (strcmp(moji, "I") == 0) {
									sprintf(moji, "%s", tradeList[mytradelist[i]].name);
									StockFontBuffer(x + 12, y + 228 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);

#ifdef _ITEM_PILENUMS
									sprintf(moji, "x%d", tradeList[mytradelist[i]].pilenum);
									StockFontBuffer(x + 180, y + 228 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);
#endif
									sprintf(moji, "%s", tradeList[mytradelist[i]].damage);
									StockFontBuffer(x + 220, y + 228 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);
									j++;
								}
								//显示宠物   페트환생
								else if (strcmp(moji, "P") == 0)
								{
									int colors = 0;
									int mylist = mytradelist[i];
									sprintf(moji, "%s", tradeList[mylist].name);
									StockFontBuffer(x + 12, y + 228 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);

									if (strcmp(tradeList[mylist].freename, tradeList[mylist].name))
									{
										sprintf(moji, "[%s]", tradeList[mylist].freename);
										colors = FONT_PAL_RED;
										StockFontBuffer(x + 102, y + 228 + j * 29, FONT_PRIO_FRONT, colors, moji, 0);
									}
#ifdef _PET_2TRANS
									sprintf(moji, "Lv:%d%s", tradeList[mylist].level, "");
									if (tradeList[mylist].trns == 1)
										sprintf(moji, "Lv:%d%s", tradeList[mylist].level, "1 단계");
									else if (tradeList[mylist].trns == 2)
										sprintf(moji, "Lv:%d%s", tradeList[mylist].level, "2 단계");
#ifdef _SHOW_FUSION
									if (LOWORD(tradeList[mylist].fusion) == 1)
										sprintf(moji, "Lv:%d%s", tradeList[mylist].level, "융 햡");
#endif
#else
									sprintf(moji, "Lv:%d%s", tradeList[mylist].level, (tradeList[mylist].trns == 0) ? "" : "转");
#endif
									StockFontBuffer(x + 220, y + 228 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);

									j++;
								}
								//显示金钱
								else if (strcmp(moji, "G") == 0)
								{
									StockFontBuffer(x + 12, y + 228 + j * 29, FONT_PRIO_FRONT, 0, "스 톤", 0);
									sprintf(moji, "%d", tradeList[mytradelist[i]].data);
									StockFontBuffer(x + 102, y + 228 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);
									sprintf(moji, "%s", "Gold");
									StockFontBuffer(x + 220, y + 228 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);
									j++;
								}
								if (j == 5)
									break;
							}
							j = 0;
							//我方要显示的起始位置
							ShowPoint = (drag1Y - 67) / 4;
							//显示交易清单
							for (i = ShowPoint + 22; i < ShowPoint + 27; i++)
							{
								if (opptradelist[i - 22] == -1) break;
								if (i > 42) break;
								//说明框用的HitBox暂时不做
								//								if( MakeHitBox( x + 10 , y + 37 + j * 29, x + 280  , y + 55 + j * 29 , DISP_PRIO_IME3 ) == TRUE ){
								//								}
								sprintf(moji, "%c", tradeList[opptradelist[i - 22]].kind);
								//显示道具
								if (strcmp(moji, "I") == 0)
								{
									int colors = 0;
									int opptradlist = opptradelist[i - 22];
									// Terry modify for 都用原名 2004/6/9
									sprintf(moji, "%s", tradeList[opptradlist].name);
									//									sprintf( moji , "%s" , tradeList[ opptradlist].freename ) ; 
									//									if( strcmp( tradeList[opptradlist].name, tradeList[opptradlist].freename ) ){
									//										sprintf( moji , "[%s]" , tradeList[ opptradlist].freename ) ; 
									//										colors = 6;
									//									}
									StockFontBuffer(x + 12, y + 40 + j * 29, FONT_PRIO_FRONT, colors, moji, 0);
#ifdef _ITEM_PILENUMS
									sprintf(moji, "x%d", tradeList[opptradelist[i - 22]].pilenum);
									StockFontBuffer(x + 180, y + 40 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);
#endif

									sprintf(moji, "%s", tradeList[opptradelist[i - 22]].damage);
									StockFontBuffer(x + 220, y + 40 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);
									j++;
								}
								else if (strcmp(moji, "P") == 0)
								{//显示宠物
									int colors = 0; //FONT_PAL_RED
									int opplist = opptradelist[i - 22];
									sprintf(moji, "%s", tradeList[opplist].name);
									StockFontBuffer(x + 12, y + 40 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);
#ifdef _SHOW_FUSION									
									// change fix 只为了颢示-----> []
									if (strcmp(tradeList[opplist].freename, tradeList[opplist].name) == 0)
									{
										sprintf(moji, "[]");
										colors = FONT_PAL_RED;
										StockFontBuffer(x + 102, y + 40 + j * 29, FONT_PRIO_FRONT, colors, moji, 0);
									}
#endif
									if (strcmp(tradeList[opplist].freename, tradeList[opplist].name))
									{
										sprintf(moji, "[%s]", tradeList[opplist].freename);
										colors = FONT_PAL_RED;
										StockFontBuffer(x + 102, y + 40 + j * 29, FONT_PRIO_FRONT, colors, moji, 0);
									}
#ifdef _PET_2TRANS
									sprintf(moji, "Lv:%d%s", tradeList[opplist].level, (tradeList[opplist].trns == 0) ? "" : "");
									if (tradeList[opplist].trns == 1)
										sprintf(moji, "Lv:%d%s", tradeList[opplist].level, "1 단계");
									else if (tradeList[opplist].trns == 2)
										sprintf(moji, "Lv:%d%s", tradeList[opplist].level, "2 단계");
#ifdef _SHOW_FUSION
									if (LOWORD(tradeList[opplist].fusion) == 1)
										sprintf(moji, "Lv:%d%s", tradeList[opplist].level, "융합");
#endif
#else
									sprintf(moji, "Lv:%d%s", tradeList[opplist].level, (tradeList[opplist].trns == 0) ? "" : "转");
#endif
									StockFontBuffer(x + 220, y + 40 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);
									j++;
								}
								//显示金钱
								else if (strcmp(moji, "G") == 0)
								{
									StockFontBuffer(x + 12, y + 40 + j * 29, FONT_PRIO_FRONT, 0, "스톤", 0);
									sprintf(moji, "%d", tradeList[opptradelist[i - 22]].data);
									StockFontBuffer(x + 102, y + 40 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);
									sprintf(moji, "%s", "Gold");
									StockFontBuffer(x + 220, y + 40 + j * 29, FONT_PRIO_FRONT, 0, moji, 0);

									j++;
								}
								if (j == 5)
									break;
							}
#endif
							sprintf(moji, "%7d", pc.gold);
							StockFontBuffer(x + 550, y + 65, FONT_PRIO_FRONT, 0, moji, 0);
							// Focus不在手动输入时显示原数值
							if (Tradeflag == false)
							{
								sprintf(moji, "%7d", tradeWndDropGold);
								StockFontBuffer(x + 550, y + 120, FONT_PRIO_FRONT, 0, moji, 0);
							}
							if (pet[tradePetIndex].useFlag
								&& pc.ridePetNo != tradePetIndex)
							{
								if (pActPet3 == NULL)
								{
#ifdef _LIZARDPOSITION			   // (可开放) Syu ADD 修正龙蜥显示位置过低
									if ((pet[tradePetIndex].graNo == 101493) || (pet[tradePetIndex].graNo == 101494) ||
										(pet[tradePetIndex].graNo == 101495) || (pet[tradePetIndex].graNo == 101496))
									{
										pActPet3 = MakeAnimDisp(x + 400, y + 145, pet[tradePetIndex].graNo, ANIM_DISP_PET);
									}
									else
										pActPet3 = MakeAnimDisp(x + 400, y + 145, pet[tradePetIndex].graNo, ANIM_DISP_PET);
#else 
									pActPet3 = MakeAnimDisp(400, /*175*/145, pet[tradePetIndex].graNo, ANIM_DISP_PET);
#endif
								}

								if (pet[tradePetIndex].freeName[0] != NULL)
									CenteringStr(pet[tradePetIndex].freeName, moji, PET_NAME_LEN);
								else
									CenteringStr(pet[tradePetIndex].name, moji, PET_NAME_LEN);

								//有转生时颜色显示蓝色
#ifdef _TRADESYSTEM2			// (不可开) Syu ADD 新交易系统
							//新系统位置偏移修正
								if (pet[tradePetIndex].trn == 1)
									StockFontBuffer(x + 330, y + 33, FONT_PRIO_FRONT, 1, moji, 0);
								else
									StockFontBuffer(x + 330, y + 33, FONT_PRIO_FRONT, 0, moji, 0);

								sprintf(moji, "%3d", pet[tradePetIndex].level);
								StockFontBuffer(x + 485, y + 105, FONT_PRIO_FRONT, 0, moji, 0);
								sprintf(moji, "%3d", pet[tradePetIndex].atk);
								StockFontBuffer(x + 485, y + 129, FONT_PRIO_FRONT, 0, moji, 0);
								sprintf(moji, "%3d", pet[tradePetIndex].def);
								StockFontBuffer(x + 485, y + 153, FONT_PRIO_FRONT, 0, moji, 0);
								sprintf(moji, "%3d", pet[tradePetIndex].quick);
								StockFontBuffer(x + 485, y + 177, FONT_PRIO_FRONT, 0, moji, 0);
								sprintf(moji, "%3d", pet[tradePetIndex].maxHp);
								StockFontBuffer(x + 485, y + 201, FONT_PRIO_FRONT, 0, moji, 0);
#endif
							}


#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
							// 以交易进行到的状态결정Button样式
							if (pc.trade_confirm == 1 || pc.trade_confirm == 3)
								tradeWndFontNo[0] = StockDispBuffer(x + 59, y + 390, DISP_PRIO_IME3, CG_TRADE_LOCK_BTN, 2);
							if (pc.trade_confirm == 4)
								tradeWndFontNo[0] = StockDispBuffer(x + 59, y + 390, DISP_PRIO_IME3, CG_TRADE_OK_BTN, 2);
							//偏移
							tradeWndFontNo[1] = StockDispBuffer(x + 240, y + 390, DISP_PRIO_IME3, CG_TRADE_CANCEL_BTN, 2);
#endif

							tradeWndFontNo[2] = StockDispBuffer(x + 452 + 20, y + 63 + 8, DISP_PRIO_IME3, CG_TRADE_LEFT_BTN_UP + tradeWndBtnFlag[2], 2);
							tradeWndFontNo[3] = StockDispBuffer(x + 486 + 20, y + 63 + 8, DISP_PRIO_IME3, CG_TRADE_RIGHT_BTN_UP + tradeWndBtnFlag[3], 2);
							tradeWndFontNo[4] = StockDispBuffer(x + 554 - 94, y + 93 + 106, DISP_PRIO_IME3, CG_TRADE_UP_BTN_UP + tradeWndBtnFlag[4], 2);
							tradeWndFontNo[5] = StockDispBuffer(x + 560 - 94, y + 93 + 106, DISP_PRIO_IME3, CG_TRADE_DOWN_BTN_UP + tradeWndBtnFlag[5], 2);
#ifdef _CHANGETRADERULE		   // (不可开) Syu ADD 交易规则修订
							if (TradeBtnflag == false)
								tradeWndFontNo[6] = StockDispBuffer(x + 562 - 62 + 25, y + 148 + 108 + 8, DISP_PRIO_IME3, CG_TRADE_PUT_BTN_UP + tradeWndBtnFlag[6], 2);
#else
							tradeWndFontNo[6] = StockDispBuffer(x + 562 - 62 + 25, y + 148 + 108 + 8, DISP_PRIO_IME3, CG_TRADE_PUT_BTN_UP + tradeWndBtnFlag[6], 2);
#endif
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
							//偏移
							tradeWndFontNo[7] = StockDispBuffer(x + 365 - 62 + 25, y + 190 + 108 + 8, DISP_PRIO_IME3, CG_TRADE_PUT_BTN_UP + tradeWndBtnFlag[7], 2);
#endif
#ifndef _CHANGETRADERULE		   // (不可开) Syu ADD 交易规则修订
							tradeWndFontNo[8] = StockDispBuffer(x + 55 + 25, y + 190 + 18, DISP_PRIO_IME3, CG_MAIL_WND_CLEAR_BTN_UP + tradeWndBtnFlag[8], 2);
#endif

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
							//偏移
							//交易显示双方名称
							StockFontBuffer(x + 5, y + 193, FONT_PRIO_FRONT, FONT_PAL_PURPLE, pc.name, 0);
							StockFontBuffer(x + 5, y + 6, FONT_PRIO_FRONT, FONT_PAL_PURPLE, opp_name, 0);
							//检视视窗钮
							tradeWndFontNo[21] = StockDispBuffer(x + 150, y + 390, DISP_PRIO_IME3, CG_TRADE_VIEW_BTN, 2);
							//我方卷轴拖曳钮
							tradeWndFontNo[18] = StockDispBuffer(x + 302, y + 42, DISP_PRIO_IME3, CG_TRADE_SCROLL_UP, 2);
							//我方卷轴上移钮
							tradeWndFontNo[19] = StockDispBuffer(x + 302, y + 176, DISP_PRIO_IME3, CG_TRADE_SCROLL_DOWN, 2);
							//我方卷轴下移钮
							//							tradeWndFontNo[ 20 ] = StockDispBuffer( x+302, y+drag1Y, DISP_PRIO_IME3, CG_TRADE_SCROLL_BTN, 2 );
							//对方卷轴拖曳钮
							tradeWndFontNo[15] = StockDispBuffer(x + 302, y + 232, DISP_PRIO_IME3, CG_TRADE_SCROLL_UP, 2);
							//对方卷轴上移钮
							tradeWndFontNo[16] = StockDispBuffer(x + 302, y + 366, DISP_PRIO_IME3, CG_TRADE_SCROLL_DOWN, 2);
							//对方卷轴下移钮
							//							tradeWndFontNo[ 17 ] = StockDispBuffer( x+302, y+drag2Y, DISP_PRIO_IME3, CG_TRADE_SCROLL_BTN, 2 );
#endif

						// 假设游标在金额位置时
							if (MakeHitBox(x + 530, y + 115, x + +530 + 86, y + 115 + 25, DISP_PRIO_IME3) == TRUE)
							{
								// 按下滑鼠取得focus及初始化
								if (mouse.onceState & MOUSE_LEFT_CRICK)
								{
									strcpy(TradeBuffer.buffer, "");
									TradeBuffer.buffer[0] = NULL;
									TradeBuffer.cursor = 0;
									TradeBuffer.cnt = 0;
									Tradeflag = true;
									GetKeyInputFocus(&TradeBuffer);
									play_se(217, 320, 240);
								}
							}
							// focus在手动输入时显示buffer
							if (Tradeflag == true)
							{
								TradeBuffer.x = pActMenuWnd4->x + 600 - strlen(TradeBuffer.buffer) * 7;
								TradeBuffer.y = pActMenuWnd4->y + 120;
								StockFontBuffer2(&TradeBuffer);
								tradeWndDropGold = atoi(TradeBuffer.buffer);
								if (atoi(TradeBuffer.buffer) >= pc.gold)
								{
									sprintf(TradeBuffer.buffer, "%d", pc.gold);
									tradeWndDropGold = pc.gold;
								}
							}

							if (pc.trade_confirm == 2)
							{
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
								//偏移
								StockFontBuffer(x + 220, y + 194, FONT_PRIO_FRONT, FONT_PAL_RED, "거래금액", 0);
							}
							if (pc.trade_confirm == 3)
							{
								StockFontBuffer(x + 210, y + 194, FONT_PRIO_FRONT, FONT_PAL_RED, "상대방 거래금액", 0);
							}
							if (pc.trade_confirm == 4)
							{
								if (tradeStatus == 2)
									StockFontBuffer(x + 220, y + 194, FONT_PRIO_FRONT, FONT_PAL_RED, "거래확인", 0);
								else
									StockFontBuffer(x + 210, y + 194, FONT_PRIO_FRONT, FONT_PAL_RED, "최종 거래확인", 0);
							}
#endif
						}

						for (i = MAX_ITEM - 1; i >= MAX_ITEMSTART; i--)
						{
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
							//已选取交易的盖杖印章
#ifdef _ITEM_PILENUMS
							if (itemflag[i - MAX_ITEMSTART] <= 0) {
#else
							if (itemflag[i - MAX_ITEMSTART] == 1) {
#endif
								//原本为显示原道具并盖上章
								//StockDispBuffer( ItemBuffer[ i ].defX, ItemBuffer[ i ].defY, ItemBuffer[ i ].dispPrio, CG_TRADE_SEAL , 0 );
								//修正为道具隐藏，直接跳过不做后续显示
								continue;

							}
#endif

							if (MakeHitBox(x + ItemBuffer[i].defX - 35, y + ItemBuffer[i].defY - 22,
								x + ItemBuffer[i].defX + 15, y + ItemBuffer[i].defY + 28, DISP_PRIO_IME3) == TRUE)
							{
#ifdef _TELLCHANNEL
								// Terry fix 2003/12/16 for 交易视窗开启时,显示物品说明不显示输入法
								//TaskBarFlag = TRUE;这行不要
								bShowItemExplain = TRUE; // 新增这行
														 // end
#endif
													 // 该栏位有道具的情况
								if (pc.item[i].useFlag == TRUE && ItemBuffer[i].mixFlag <= 2)
								{
									char *splitPoint = pc.item[i].memo;
									int color = pc.item[i].color;
									// 交易物品能否使用Check
									if (pc.transmigration == 0 && pc.level < pc.item[i].level)
										color = FONT_PAL_RED;
									// 道具名称显示
									//JL800600
									StockFontBuffer(0, 460 + DISPLACEMENT_Y, FONT_PRIO_FRONT, color, pc.item[i].name, 0); //아이템창 이름
																														  // 道具说明
																														  //JL800600
									StockFontBuffer(160, 460 + DISPLACEMENT_Y, FONT_PRIO_FRONT, 0, splitPoint, 0); //아이템창 설명
								}
								// 在道具上点两下的情况
								if (mouse.onceState & MOUSE_LEFT_DBL_CRICK)
								{
									if (tradeStatus != 1)	return;
									if (pc.trade_confirm == 2 || pc.trade_confirm == 4) return;
									// 合成旗标关闭
									ItemBuffer[i].mixFlag = FALSE;
									// 确认该位置有道具
									if (pc.item[i].useFlag == TRUE) {
										ItemBuffer[i].dragFlag = TRUE;
										mouse.itemNo = i;
										// 道具选择初期化
										itemNo = -1;
									}
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
									if (mouse.itemNo != -1)
									{

										char buf[1024];
										tradeList[i - 4].kind = 'I';
										tradeList[i - 4].data = i;
										strcpy(tradeList[i - 4].name, pc.item[i].name);
										strcpy(tradeList[i - 4].damage, pc.item[i].damage);
#ifdef _ITEM_PILENUMS
										itemflag[i - MAX_ITEMSTART]--;
#else
										itemflag[i - MAX_ITEMSTART] = 1;
#endif
#ifdef _ITEM_PILENUMS
										tradeList[i - 4].pilenum = pc.item[i].pile - itemflag[i - MAX_ITEMSTART];
#endif
										sprintf(buf, "T|%s|%s|I|1|%d", opp_sockfd, opp_name, i);
										lssproto_TD_send(sockfd, buf);

									}
#endif
									mouse.itemNo = -1;
								}
							}
							if (pc.item[i].useFlag == TRUE)
							{
								// 非合成物品的情况
								if (ItemBuffer[i].mixFlag <= 2) {
									char buf[256];
									// 道具显示
									StockDispBuffer(x + ItemBuffer[i].defX - 8, y + ItemBuffer[i].defY, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);


#ifdef _ITEM_PILENUMS
									sprintf(buf, "%d", itemflag[i - MAX_ITEMSTART]);
									StockFontBuffer(x + ItemBuffer[i].defX - 8, y + ItemBuffer[i].defY, FONT_PRIO_FRONT, FONT_PAL_RED, buf, 0);
#endif

								}
								else if (ItemBuffer[i].mixFlag == 10)
									StockDispBuffer(x + ItemBuffer[i].defX - 8, y + ItemBuffer[i].defY, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
								else if (ItemBuffer[i].mixFlag == 11)
									StockDispBuffer(x + ItemBuffer[i].defX - 8, y + ItemBuffer[i].defY, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);
								else if (ItemBuffer[i].mixFlag == 3)
									StockDispBuffer(pActPet2->x, pActPet2->y - 16, DISP_PRIO_DRAG, pc.item[i].graNo, 0);
								else if (ItemBuffer[i].mixFlag == 4)
									StockDispBuffer(pActPet2->x, pActPet2->y, ItemBuffer[i].dispPrio, pc.item[i].graNo, 0);

							}

						}
					}
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
				}
				// 检视视窗内容
				else  if (SecondTradeWndflag == true)
				{
					MainTradeWndflag = false;
					if (pActMenuWnd4 != NULL && SecondTradeWnd != NULL)
					{
						x = SecondTradeWnd->x;
						y = SecondTradeWnd->y;
#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
						//对话框内容
						if (tradetalkwndflag != false) {
							for (i = 3; i >= 0; i--) {
								sprintf(moji, "%s", talkmsg[i]);
								StockFontBuffer(talkwndx - 154, talkwndy - 26 + i * 20, FONT_PRIO_AFRONT, FONT_PAL_YELLOW, moji, 0);
							}
							SecondtradeWndFontNo[4] = StockDispBuffer(talkwndx, talkwndy, DISP_PRIO_BOX3, CG_TRADE_TALK_WND, 2);
						}
#endif						
						SecondtradeWndFontNo[0] = StockDispBuffer(x + 254, y + 380, DISP_PRIO_IME3, CG_TRADE_CANCEL_BTN, 2);
						SecondtradeWndFontNo[1] = StockDispBuffer(x + 159, y + 380, DISP_PRIO_IME3, CG_TRADE_BACK_BTN, 2);
						//交易进行状态
						if (pc.trade_confirm == 1 || pc.trade_confirm == 3)
							SecondtradeWndFontNo[2] = StockDispBuffer(x + 63, y + 380, DISP_PRIO_IME3, CG_TRADE_LOCK_BTN, 2);
						if (pc.trade_confirm == 4)
							SecondtradeWndFontNo[2] = StockDispBuffer(x + 63, y + 380, DISP_PRIO_IME3, CG_TRADE_OK_BTN, 2);

						sprintf(moji, "%7d", tradeWndDropGoldGet);

#ifdef _PET_ITEM
						// 显示宠物装备资料
						if (locknum != -1 && tradeList[locknum + 37].data != -1)
						{
							if (bViewPetEquip)
								SecondtradeWndFontNo[5] = StockDispBuffer(x + 259, y + 310, DISP_PRIO_IME3, 26472, 2);
							else
								SecondtradeWndFontNo[5] = StockDispBuffer(x + 259, y + 310, DISP_PRIO_IME3, 26471, 2);
						}
#endif

						//显示金钱
						if (tradeWndDropGoldGet > 0)
							StockFontBuffer(x + 95, y + 336, FONT_PRIO_FRONT, 0, moji, 0);
						for (i = 0; i < 5; i++)
						{
							if (MakeHitBox(x + 320, y + 12 + i * 42, x + 600, y + 51 + i * 42, DISP_PRIO_IME3) == TRUE)
							{
#ifdef _TRADELOCKBTN				// (不可开) Syu ADD 增加锁定键
								if (mouse.onceState & MOUSE_LEFT_CRICK)
								{
#ifdef _PET_ITEM
									bViewPetEquip = FALSE;
									HitDispNo = 0;
#endif
									locknum = i;
									locknum2 = -1;
								}
								if (locknum == -1)
								{
									locknum = i;
									locknum2 = -1;
								}
							}
							if (locknum != -1)
							{
								if (tradeList[locknum + 37].data != -1)
								{
#ifdef _PET_ITEM
									if (bViewPetEquip)
									{
										int		iY = 251, iColor;
										char	*splitPoint;

										// 显示宠物装备栏的底图及装备
										for (int iCount = 0; iCount < PET_EQUIPNUM; iCount++)
										{
											StockDispBuffer(ItemBuffer[iCount].defX, ItemBuffer[iCount].defY, DISP_PRIO_IME3, nPetItemEquipBmpNumber[iCount][0], 0);
											if (opp_pet[locknum].oPetItemInfo[iCount].bmpNo > 0)
												StockDispBuffer(ItemBuffer[iCount].defX, ItemBuffer[iCount].defY, DISP_PRIO_ITEM, opp_pet[locknum].oPetItemInfo[iCount].bmpNo, 0);
										}
										for (int iCount = 0; iCount < PET_EQUIPNUM; iCount++)
										{
											// 显示装备内容
											if (MakeHitBox(ItemBuffer[iCount].defX - 26, ItemBuffer[iCount].defY - 26,
												ItemBuffer[iCount].defX + 26, ItemBuffer[iCount].defY + 23, DISP_PRIO_IME3) == TRUE)
											{
												if (opp_pet[locknum].oPetItemInfo[iCount].bmpNo > 0)
												{
													iColor = opp_pet[locknum].oPetItemInfo[iCount].color;
													splitPoint = opp_pet[locknum].oPetItemInfo[iCount].memo;
													// 装备名称
													StockFontBuffer(x + 25, y + 195, FONT_PRIO_FRONT, iColor, opp_pet[locknum].oPetItemInfo[iCount].name, 0);
													// 耐久度
													sprintf(moji, "인내력(%s)", opp_pet[locknum].oPetItemInfo[iCount].damage);
													StockFontBuffer(x + 25, y + 215, FONT_PRIO_FRONT, iColor, moji, 0);

													// 显示道具叙述
													while (1)
													{
														if (strlen(splitPoint) > 28)
														{
															_snprintf(moji, 28, "%s", splitPoint);
															moji[28] = NULL;
															if (GetStrLastByte(moji) == 3)
															{
																moji[27] = NULL;
																splitPoint += 27;
															}
															else
															{
																moji[28] = NULL;
																splitPoint += 28;
															}
															StockFontBuffer(x + 25, iY, FONT_PRIO_FRONT, 0, moji, 0);
															iY += 24;
														}
														else
														{
															strcpy(moji, splitPoint);
															StockFontBuffer(x + 25, iY, FONT_PRIO_FRONT, 0, moji, 0);
															break;
														}
													}
												}
											}
										}
									}
									else
#endif
									{
										//产生宠物Action
										if (SecondActPet == NULL) {
											ShowPetNum = atoi(opp_pet[locknum].opp_petgrano);
											SecondActPet = MakeAnimDisp(x + 215, y + 130, ShowPetNum, ANIM_DISP_PET);
										}
										//已存在显示的宠物
										else if (SecondActPet != NULL && ShowPetNum != atoi(opp_pet[locknum].opp_petgrano)) {
											DeathAction(SecondActPet);
											SecondActPet = NULL;
											ShowPetNum = atoi(opp_pet[locknum].opp_petgrano);
											SecondActPet = MakeAnimDisp(x + 215, y + 130, atoi(opp_pet[locknum].opp_petgrano), ANIM_DISP_PET);
										}

										//opp_petfreename
										//显示数值、技能 ( 左方 )
										//StockDispBuffer( x + 95 , y + 40 , DISP_PRIO_IME3, CG_TRADE_LINE , 0 );
										sprintf(moji, "%s", opp_pet[locknum].opp_petname);
										StockFontBuffer(x + 55, y + 30, FONT_PRIO_FRONT, 0, moji, 0);//打印宠物名字

										if (strcmp(opp_pet[locknum].opp_petfreename, opp_pet[locknum].opp_petname)) {
											sprintf(moji, "[%s]", opp_pet[locknum].opp_petfreename);
											StockFontBuffer(x + 155, y + 30, FONT_PRIO_FRONT, 6, moji, 0);//打印宠物昵称
										}

#ifdef _PET_2TRANS
										sprintf(moji, "%d회", atoi(opp_pet[locknum].opp_pettrans));
#ifdef _SHOW_FUSION
										if (LOWORD(atoi(opp_pet[locknum].opp_fusion)) == 1) {
											sprintf(moji, "융합%d회", atoi(opp_pet[locknum].opp_pettrans));
										}
#endif
#else
										sprintf(moji, "%s", (atoi(opp_pet[locknum].opp_pettrans) == 0) ? "" : "转");
#endif
										StockFontBuffer(x + 90, y + 52, FONT_PRIO_FRONT, 2, moji, 0);


										StockDispBuffer(x + 55, y + 60, DISP_PRIO_IME3, CG_TRADE_LV_LINE, 0);
										sprintf(moji, "%3d", atoi(opp_pet[locknum].opp_petlevel));
										StockFontBuffer(x + 55, y + 52, FONT_PRIO_FRONT, 0, moji, 0);//打印宠物等级

										StockDispBuffer(x + 55, y + 82, DISP_PRIO_IME3, CG_TRADE_HP_LINE, 0);
										sprintf(moji, "%3d", atoi(opp_pet[locknum].opp_petshowhp));
										StockFontBuffer(x + 55, y + 74, FONT_PRIO_FRONT, 0, moji, 0);

										StockDispBuffer(x + 55, y + 104, DISP_PRIO_IME3, CG_TRADE_ATK_LINE, 0);
										sprintf(moji, "%3d", atoi(opp_pet[locknum].opp_petatk));
										StockFontBuffer(x + 55, y + 96, FONT_PRIO_FRONT, 0, moji, 0);

										//StockDispBuffer(x + 55, y + 126, DISP_PRIO_IME3, CG_TRADE_DEF_LINE, 0);
										sprintf(moji, "%3d", atoi(opp_pet[locknum].opp_petdef));
										StockFontBuffer(x + 55, y + 118, FONT_PRIO_FRONT, 0, moji, 0);

										StockDispBuffer(x + 55, y + 148, DISP_PRIO_IME3, CG_TRADE_DEX_LINE, 0);
										sprintf(moji, "%3d", atoi(opp_pet[locknum].opp_petquick));
										StockFontBuffer(x + 55, y + 140, FONT_PRIO_FRONT, 0, moji, 0);
										for (j = 0; j < atoi(opp_pet[locknum].opp_petslot); j++) {
											if (j == 0) {
												StockDispBuffer(x + 92, y + 170 + j * 22, DISP_PRIO_IME3, CG_TRADE_SK1_LINE + j, 0);
											}
											else {
												StockDispBuffer(x + 100, y + 170 + j * 22, DISP_PRIO_IME3, CG_TRADE_SK1_LINE + j, 0);
											}
										}
										sprintf(moji, "%s", opp_pet[locknum].opp_petskill1);
										if (strcmp(moji, "(null)") != 0)
											StockFontBuffer(x + 65, y + 162, FONT_PRIO_FRONT, 0, moji, 0);
										sprintf(moji, "%s", opp_pet[locknum].opp_petskill2);
										if (strcmp(moji, "(null)") != 0)
											StockFontBuffer(x + 65, y + 184, FONT_PRIO_FRONT, 0, moji, 0);
										sprintf(moji, "%s", opp_pet[locknum].opp_petskill3);
										if (strcmp(moji, "(null)") != 0)
											StockFontBuffer(x + 65, y + 206, FONT_PRIO_FRONT, 0, moji, 0);
										sprintf(moji, "%s", opp_pet[locknum].opp_petskill4);
										if (strcmp(moji, "(null)") != 0)
											StockFontBuffer(x + 65, y + 228, FONT_PRIO_FRONT, 0, moji, 0);
										sprintf(moji, "%s", opp_pet[locknum].opp_petskill5);
										if (strcmp(moji, "(null)") != 0)
											StockFontBuffer(x + 65, y + 250, FONT_PRIO_FRONT, 0, moji, 0);
										sprintf(moji, "%s", opp_pet[locknum].opp_petskill6);
										if (strcmp(moji, "(null)") != 0)
											StockFontBuffer(x + 65, y + 272, FONT_PRIO_FRONT, 0, moji, 0);
										sprintf(moji, "%s", opp_pet[locknum].opp_petskill7);
										if (strcmp(moji, "(null)") != 0)
											StockFontBuffer(x + 65, y + 294, FONT_PRIO_FRONT, 0, moji, 0);
									}
								}
								else if (SecondActPet != NULL) {
									DeathAction(SecondActPet);
									SecondActPet = NULL;
								}
#endif
							}
							//显示数值、技能 ( 右方 )
							if (tradeList[i + 37].data != -1) {
								sprintf(moji, "%s", opp_pet[i].opp_petname);
								StockFontBuffer(x + 322, y + 13 + i * 42, FONT_PRIO_FRONT, atoi(opp_pet[i].opp_pettrans), moji, 0);

								if (strcmp(opp_pet[i].opp_petname, opp_pet[i].opp_petfreename)) {
									sprintf(moji, "[%s]", opp_pet[i].opp_petfreename);
									StockFontBuffer(x + 442, y + 13 + i * 42, FONT_PRIO_FRONT, 6, moji, 0);
								}

#ifdef _PET_2TRANS
								sprintf(moji, "%s", "");
								if (atoi(opp_pet[i].opp_pettrans) == 1)
									sprintf(moji, "%s", "1 단계");
								else if (atoi(opp_pet[i].opp_pettrans) == 2)
									sprintf(moji, "%s", "2 단계");
#ifdef _SHOW_FUSION
								if (LOWORD(atoi(opp_pet[i].opp_fusion)) == 1)
									sprintf(moji, "%s", "융합");
#endif
#else
								sprintf(moji, "%s", (atoi(opp_pet[i].opp_pettrans) == 0) ? "" : "转");
#endif
								StockFontBuffer(x + 590, y + 13 + i * 42, FONT_PRIO_FRONT, 2, moji, 0);

								sprintf(moji, "%4d", atoi(opp_pet[i].opp_petshowhp));
								StockFontBuffer(x + 504, y + 33 + i * 42, FONT_PRIO_FRONT, atoi(opp_pet[i].opp_pettrans), moji, 0);
								sprintf(moji, "%3d", atoi(opp_pet[i].opp_petlevel));
								StockFontBuffer(x + 558, y + 33 + i * 42, FONT_PRIO_FRONT, atoi(opp_pet[i].opp_pettrans), moji, 0);
								sprintf(moji, "%3d", atoi(opp_pet[i].opp_petatk));
								StockFontBuffer(x + 345, y + 33 + i * 42, FONT_PRIO_FRONT, atoi(opp_pet[i].opp_pettrans), moji, 0);
								sprintf(moji, "%3d", atoi(opp_pet[i].opp_petdef));
								StockFontBuffer(x + 398, y + 33 + i * 42, FONT_PRIO_FRONT, atoi(opp_pet[i].opp_pettrans), moji, 0);
								sprintf(moji, "%3d", atoi(opp_pet[i].opp_petquick));
								StockFontBuffer(x + 451, y + 33 + i * 42, FONT_PRIO_FRONT, atoi(opp_pet[i].opp_pettrans), moji, 0);
							}
						}
						//显示道具
						for (i = 0; i < 3; i++)
						{
							for (j = 0; j < 5; j++)
							{
								if (MakeHitBox(x + 368 + 51 * j - 35, y + 265 + 47 * i - 23,
									x + 368 + 51 * j + 18, y + 265 + 47 * i + 23, DISP_PRIO_IME3) == TRUE)
								{
#ifdef _TRADELOCKBTN				// (不可开) Syu ADD 增加锁定键
									if (mouse.onceState & MOUSE_LEFT_CRICK) {
										locknum2 = i * 5 + j;
										locknum = -1;
									}
									if (locknum2 == -1) {
										locknum2 = i * 5 + j;
										locknum = -1;
									}
									if (SecondActPet != NULL) {
										DeathAction(SecondActPet);
										SecondActPet = NULL;
									}
#ifdef _PET_ITEM
									bViewPetEquip = FALSE;
#endif
								}
								if (locknum2 != -1)
								{
									x = SecondTradeWnd->x;
									y = SecondTradeWnd->y;
									if (tradeList[locknum2 + 22].data != -1)
									{
										//说明过长换行
										char *splitPoint = opp_item[locknum2].effect;
										while (1)
										{
											if (y > 40)
												break;
											if (strlen(splitPoint) > 28)
											{
												strncpy(moji, splitPoint, 28);
												moji[28] = NULL;
												if (GetStrLastByte(moji) == 3)
												{
													moji[27] = NULL;
													splitPoint += 27;
												}
												else
												{
													moji[28] = NULL;
													splitPoint += 28;
												}
												StockFontBuffer(x + 50, y + 160, FONT_PRIO_FRONT, 0, moji, 0); y += 40;
											}
											else
											{
												strcpy(moji, splitPoint);
												StockFontBuffer(x + 50, y + 160, FONT_PRIO_FRONT, 0, moji, 0);
												break;
											}
										}
										StockDispBuffer(x + 80, y + 90, DISP_PRIO_IME3, atoi(opp_item[locknum2].graph), 0);

										sprintf(moji, "%s ", opp_item[locknum2].name);
										StockFontBuffer(x + 50, y + 120, FONT_PRIO_FRONT, 0, moji, 0);
										if (strcmp(opp_item[locknum2].name, opp_item[locknum2].freename))
										{
											sprintf(moji, "[%s]", opp_item[locknum2].freename);
											StockFontBuffer(x + 50, y + 140, FONT_PRIO_FRONT, 6, moji, 0);
										}
#ifdef _ITEM_PILENUMS
										sprintf(moji, "x%d", tradeList[locknum2 + 22].pilenum);
#else
										sprintf(moji, "%s", opp_item[locknum2].name);
#endif
										StockFontBuffer(x + 140, y + 80, FONT_PRIO_FRONT, 0, moji, 0);


										if (strcmp(opp_item[locknum2].damage, "") != 0)
											sprintf(moji, "인내력：%s", opp_item[locknum2].damage);
										StockFontBuffer(x + 50, y + 240, FONT_PRIO_FRONT, 0, moji, 0);

									}
#endif
								}
								if (tradeList[i * 5 + j + 22].data != -1)
								{
									StockDispBuffer(x + 368 + 47 * j, y + 265 + 47 * i, DISP_PRIO_IME3, atoi(opp_item[i * 5 + j].graph), 0);
#ifdef _ITEM_PILENUMS
									if (tradeList[i * 5 + j + 22].pilenum > 1)
									{
										sprintf(moji, "%d", tradeList[i * 5 + j + 22].pilenum);
										StockFontBuffer(x + 370 + 51 * j + 10, y + 265 + 47 * i + 2, FONT_PRIO_FRONT, 0, moji, 0);
									}
#endif

								}
							}
						}
					}
					if (SecondTradeWnd != NULL) {
						if (SecondTradeWnd->hp > 0) {
							//产生交易第二视窗
							if (pActPet3 != NULL) {
								DeathAction(pActPet3);
								pActPet3 = NULL;
							}
							StockDispBuffer(((WINDOW_DISP *)SecondTradeWnd->pYobi)->mx, ((WINDOW_DISP *)SecondTradeWnd->pYobi)->my - 25, DISP_PRIO_MENU, CG_TRADE_VIEWWND, 1);
							if (mouse.onceState & MOUSE_LEFT_CRICK) {
								if (HitDispNo == SecondtradeWndFontNo[0]) {
									//关闭交易视窗
									MenuToggleFlag ^= JOY_CTRL_T;
									//视窗关闭音效
									play_se(203, 320, 240);
									sprintf(buffer, "W|%s|%s", opp_sockfd, opp_name);
									//送出취소讯息给Server通知对方
									lssproto_TD_send(sockfd, buffer);
									tradeStatus = 0;
									tradeInit();
									pc.trade_confirm = 1;
								}
								else if (HitDispNo == SecondtradeWndFontNo[1])
								{
									MainTradeWndflag = true;
									SecondTradeWndflag = false;
									//andy_add 20030610
									for (i = 0; i < MENU_TRADE_0; i++) tradeWndFontNo[i] = -2;
									for (i = 0; i < 6; i++) SecondtradeWndFontNo[i] = -2;
#ifdef _PET_ITEM
									InitItem3(325, 230);
									bViewPetEquip = FALSE;
#endif
								}
#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
								else if (HitDispNo == SecondtradeWndFontNo[4])
									talkwndflag = true;
#endif
#ifdef _PET_ITEM
								else if (HitDispNo == SecondtradeWndFontNo[5])
								{
									bViewPetEquip = !bViewPetEquip;
									InitItem(SecondTradeWnd->x, SecondTradeWnd->y + 20, bViewPetEquip);
								}
#endif
								else
									LockAndOkfunction();
							}
#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
							if (talkwndflag == true)
							{
								talkwndx = mouse.nowPoint.x;
								talkwndy = mouse.nowPoint.y;
							}
							if (mouse.onceState & MOUSE_LEFT_CRICK_UP)
								talkwndflag = false;
							if (mouse.onceState & MOUSE_RIGHT_CRICK)
								tradetalkwndflag = false;
#endif
						}
					}
				}

#endif
			}
			break;
		}
	}
	else if (tradeStatus != 0)
	{
		char buffer[1024] = "";
		tradeStatus = 0;
		sprintf(buffer, "W|%s|%s", opp_sockfd, opp_name);
		lssproto_TD_send(sockfd, buffer);
		tradeInit();
		pc.trade_confirm = 1;
	}

	// 交易视窗部分到此结束


	// show Bank Window
	if (MenuToggleFlag & JOY_B) {

		char buffer[1024];
		int x, y, w, h;

		if (checkPcWalkFlag() == 1) closeBankman();


		if (joy_trg[0] & JOY_ESC
			|| actBtn == 1
			|| menuBtn == 1
			|| disconnectServerFlag == TRUE
			)
		{
			closeBankman();
			return;
		}

		// ?????????
		if (pActMenuWnd5 == NULL) {
			w = 270; h = 160;
			x = (lpDraw->xSize - w) / 2;
			y = (lpDraw->ySize - h) / 2;

			pActMenuWnd5 = MakeWindowDisp(x, y, w, h, NULL, -1);
			play_se(202, 320, 240);	// ????????									

		}
		else {
			// ??????????????
			if (pActMenuWnd5->hp > 0) {

				if (mouse.onceState & MOUSE_LEFT_CRICK) {

					// ???
					if (HitDispNo == bankWndFontNo[0]) {
						closeBankman();
						return;
					}

					if (HitDispNo == bankWndFontNo[1]) {
						sprintf(buffer, "B|G|%d", pc.gold - cashGold);
						if (bNewServer)
							lssproto_FM_send(sockfd, buffer);
						else
							old_lssproto_FM_send(sockfd, buffer);
						closeBankman();
						return;
					}

				}

				// Add Gold
				if (HitDispNo == bankWndFontNo[2]) {

					if (mouse.onceState & MOUSE_LEFT_CRICK_UP && bankWndBtnFlag[2] == TRUE) {

						bankWndBtnFlag[2] = FALSE;
					}

					if ((cashGold < CHAR_getMaxHaveGold()) && (totalGold - cashGold > 0)) {

						if (bankWndBtnFlag[2] == TRUE) {
							cashGold += bankGoldInc;
							bankGoldCnt++;
							if (bankGoldCnt >= 30) {
								bankGoldCnt = 0;
								if (bankGoldInc == 0) {
									bankGoldInc = 1;
								}
								else {
									bankGoldInc *= 5;
									if (bankGoldInc > 10000) {
										bankGoldInc = 10000;
									}
								}
							}
							if (cashGold >= CHAR_getMaxHaveGold()) {
								cashGold = CHAR_getMaxHaveGold();
								play_se(220, 320, 240);
							}
							if (cashGold >= totalGold) {
								cashGold = totalGold;
								play_se(220, 320, 240);
							}

						}
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							cashGold++;
							if (cashGold >= CHAR_getMaxHaveGold()) {
								cashGold = CHAR_getMaxHaveGold();
								play_se(220, 320, 240);
							}
							else {
								bankGoldInc = 0;
								bankGoldCnt = 0;
								bankWndBtnFlag[2] = TRUE;
								play_se(217, 320, 240);
							}

						}
					}
				}
				else {
					bankWndBtnFlag[2] = FALSE;
				}
				if (HitDispNo == bankWndFontNo[3]) {
					if (mouse.onceState & MOUSE_LEFT_CRICK_UP && bankWndBtnFlag[3] == TRUE) {
						bankWndBtnFlag[3] = FALSE;
						//play_se( 212, 320, 240 );
					}
					if ((cashGold > 0) && (totalGold - cashGold < MAX_BANKGOLD)) {
						if (bankWndBtnFlag[3] == TRUE) {
							cashGold -= bankGoldInc;
							bankGoldCnt++;
							if (bankGoldCnt >= 30) {
								bankGoldCnt = 0;
								if (bankGoldInc == 0) {
									bankGoldInc = 1;
								}
								else {
									bankGoldInc *= 5;
									if (bankGoldInc > 10000) {
										bankGoldInc = 10000;
									}
								}
							}
							if (cashGold <= 0) {
								cashGold = 0;
								play_se(220, 320, 240);
							}
							if (totalGold - cashGold >= MAX_BANKGOLD) {
								cashGold = totalGold - MAX_BANKGOLD;
								play_se(220, 320, 240);
							}

						}
						if (mouse.onceState & MOUSE_LEFT_CRICK) {
							cashGold--;
							if (cashGold <= 0) {
								cashGold = 0;
								play_se(220, 320, 240);
							}
							else if (totalGold - cashGold >= MAX_BANKGOLD) {
								cashGold = totalGold - MAX_BANKGOLD;
								play_se(220, 320, 240);
							}
							else {
								bankGoldInc = 0;
								bankGoldCnt = 0;
								bankWndBtnFlag[3] = TRUE;
								play_se(217, 320, 240);

							}

						}
					}
					else {
						bankGoldInc = 0;
						bankGoldCnt = 0;
					}
				}
				else {
					bankWndBtnFlag[3] = FALSE;
				}
				for (int i = 2; i <= 8; i++) {
					if (mouse.state & MOUSE_LEFT_CRICK && tradeWndBtnFlag[i] == TRUE) {
						tradeWndBtnFlag[i] = TRUE;
					}
					else {
						tradeWndBtnFlag[i] = FALSE;
					}
				}
				if (pActMenuWnd5 != NULL) {
					x = pActMenuWnd5->x;
					y = pActMenuWnd5->y;

					int 屏幕X, 屏幕Y;
					if (ResoMode == 分辨率::mode800) {
						屏幕X = 800 / 2;
						屏幕Y = 600 / 2;
					}
					else if (ResoMode == 分辨率::mode1024) {
						屏幕X = 1024 / 2;
						屏幕Y = 768 / 2;
					}
					else {
						屏幕X = 640 / 2;
						屏幕Y = 480 / 2;
					}
#ifdef NEW个人银行图档偏移		//[개인 은행 이미지 offset]
					bankWndFontNo[0] = StockDispBuffer(屏幕X + 60, 屏幕Y + 60, DISP_PRIO_IME3, CG_TRADE_CANCEL_BTN, 2);
					bankWndFontNo[1] = StockDispBuffer(屏幕X - 65, 屏幕Y + 60, DISP_PRIO_IME3, CG_TRADE_OK_BTN, 2);
#else
					bankWndFontNo[0] = StockDispBuffer(x + 200, y + 133, DISP_PRIO_IME3, CG_TRADE_CANCEL_BTN, 2);
					bankWndFontNo[1] = StockDispBuffer(x + 75, y + 133, DISP_PRIO_IME3, CG_TRADE_OK_BTN, 2);
#endif
					bankWndFontNo[2] = StockDispBuffer(屏幕X + 20, 屏幕Y - 5, DISP_PRIO_IME3, CG_UP_BTN + bankWndBtnFlag[2], 2);
					bankWndFontNo[3] = StockDispBuffer(屏幕X + 80, 屏幕Y - 5, DISP_PRIO_IME3, CG_DOWN_BTN + bankWndBtnFlag[3], 2);

#ifdef NEW个人银行图档偏移		//[개인 은행 이미지 offset]
					sprintf(moji, "%d", cashGold);
					StockFontBuffer(屏幕X + 40, 屏幕Y - 37, FONT_PRIO_FRONT, 0, moji, 0);
					sprintf(moji, "%d", totalGold - cashGold);
					StockFontBuffer(屏幕X + 40, 屏幕Y + 11, FONT_PRIO_FRONT, 0, moji, 0);
#else
					sprintf(moji, "%7d", cashGold);
					StockFontBuffer(x + 180, y + 40, FONT_PRIO_FRONT, 0, moji, 0);
					sprintf(moji, "%7d", totalGold - cashGold);
					StockFontBuffer(x + 180, y + 86, FONT_PRIO_FRONT, 0, moji, 0);
#endif

					//JL 800600 fix
					StockDispBuffer(320 + DISPLACEMENT_X / 2, 240 + DISPLACEMENT_Y / 2, DISP_PRIO_IME3, 26287, 1);
				}
			}
		}
	}

#ifdef _STANDBYPET
	if (StandbyPetSendFlag == FALSE) {
		int s_pet = 0;
		int i;

		for (i = 0; i < MAX_PET; i++) {
			if (pc.selectPetNo[i])
				s_pet |= (1 << i);
		}
		if (s_pet != pc.standbyPet)
			send_StandBy_Pet();
	}
#endif


}

#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
void lssproto_RESIST_recv(int fd, char *data)
{
	for (int i = 0; i < CHAR_MAX_DETAIL; i++)
		getStringToken(data, '|', i + 1, sizeof(CharDetail[i]) - 1, CharDetail[i]);
}
#endif

#ifdef _ALCHEPLUS
void lssproto_ALCHEPLUS_recv(int fd, char *data)
{
	char token[64];
	int i;
	for (i = 0; i < 25; i++) {
		getStringToken(data, '|', i + 1, sizeof(token) - 1, token);
		iCharAlchePlus[i] = atoi(token);
	}
}
#endif

#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
void lssproto_BATTLESKILL_recv(int fd, char *data) {
	setCharMind(pc.ptAct, atoi(data));
}
#endif

void lssproto_TD_recv(int fd, char *data)
{
	char Head[2] = "";
	char buf_sockfd[128] = "";
	char buf_name[128] = "";
	char buf[128] = "";
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
	char opp_index[128];
	int  index;
	char realname[256];
	char freename[256];
#endif
	getStringToken(data, '|', 1, sizeof(char), Head);
	// 交易开启资料初始化	
	if (strcmp(Head, "C") == 0) {
		if ( /*(tradeFlag==0) ||*/ (tradeStatus != 0))
			return;

		strcpy(opp_sockfd, "");
		strcpy(opp_name, "");
		strcpy(trade_command, "");

		getStringToken(data, '|', 2, sizeof(opp_sockfd) - 1, opp_sockfd);
		getStringToken(data, '|', 3, sizeof(opp_name) - 1, opp_name);
		getStringToken(data, '|', 4, sizeof(trade_command) - 1, trade_command);

		if (strcmp(trade_command, "0") == 0) {
			return;
		}
		else if (strcmp(trade_command, "1") == 0) {
			tradeStatus = 1;
			MenuToggleFlag = JOY_CTRL_T;
			// shan trade(DoubleCheck)
			pc.trade_confirm = 1;
		}
	}
	//处理物品交易资讯传递
	else if (strcmp(Head, "T") == 0) {

		if (tradeStatus == 0)	return;
		char buf_showindex[128];

		//andy_add mttrade
		getStringToken(data, '|', 4, sizeof(trade_kind) - 1, trade_kind);
		if (strcmp(trade_kind, "S") == 0) {
			char buf1[256];
			int objno = -1, showno = -1;
			if (pActMenuWnd4 == NULL) {
				DeathMenuAction();
				DeathMenuAction2();
				pActMenuWnd4 = MakeWindowDisp(10, 0, 620, 456, 0, -1);
				InitItem3(325, 230);
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
				if (SecondTradeWnd == NULL)
					SecondTradeWnd = MakeWindowDisp(10, 0, 620, 456, 0, -1);
#endif
			}
			getStringToken(data, '|', 6, sizeof(buf1) - 1, buf1);
			objno = atoi(buf1);
			getStringToken(data, '|', 7, sizeof(buf1) - 1, buf1);
			showno = atoi(buf1);
			getStringToken(data, '|', 5, sizeof(buf1) - 1, buf1);

			ItemBuffer[objno].mixFlag = FALSE;
			if (pc.item[objno].useFlag == TRUE) {
				ItemBuffer[objno].dragFlag = TRUE;
				mouse.itemNo = objno;
			}

			if (!strcmp(buf1, "I")) {	//I
			}
			else {	//P
				tradePetIndex = objno;
				tradePet[0].index = objno;

				if (pet[objno].useFlag &&  pc.ridePetNo != objno) {
					if (pet[objno].freeName[0] != NULL)
						strcpy(tradePet[0].name, pet[objno].freeName);
					else
						strcpy(tradePet[0].name, pet[objno].name);
					tradePet[0].level = pet[objno].level;
					tradePet[0].atk = pet[objno].atk;
					tradePet[0].def = pet[objno].def;
					tradePet[0].quick = pet[objno].quick;
					tradePet[0].graNo = pet[objno].graNo;

					showindex[3] = 3;
					DeathAction(pActPet4);
					pActPet4 = NULL;
				}
			}

			mouse.itemNo = -1;

			return;
		}

		getStringToken(data, '|', 2, sizeof(buf_sockfd) - 1, buf_sockfd);
		getStringToken(data, '|', 3, sizeof(buf_name) - 1, buf_name);
		getStringToken(data, '|', 4, sizeof(trade_kind) - 1, trade_kind);
		getStringToken(data, '|', 5, sizeof(buf_showindex) - 1, buf_showindex);
		opp_showindex = atoi(buf_showindex);

		if ((strcmp(buf_sockfd, opp_sockfd) != 0) || (strcmp(buf_name, opp_name) != 0))
			return;

		if (strcmp(trade_kind, "G") == 0) {

			getStringToken(data, '|', 6, sizeof(opp_goldmount) - 1, opp_goldmount);
			int mount = atoi(opp_goldmount);
#ifdef _CHANGETRADERULE		   // (不可开) Syu ADD 交易规则修订
			if (tradeWndDropGoldGet != 0) {
				MenuToggleFlag ^= JOY_CTRL_T;
				play_se(203, 320, 240);
				sprintf(buf, "W|%s|%s", opp_sockfd, opp_name);
				if (bNewServer)
					lssproto_TD_send(sockfd, buf);
				else
					old_lssproto_TD_send(sockfd, buf);
				sprintf(buf, "%s비정상적인 방식으로 거래 금액을 수정하면 시스템이 거래창을 강제로 닫습니다！", opp_name);
				StockChatBufferLine(buf, FONT_PAL_RED);
				return;
			}
#endif

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
			//andy_reEdit
			if (mount != -1) {
				tradeList[42].kind = 'G';
				tradeList[42].data = mount;
				tradeWndDropGoldGet = mount;
			}
			else {
				tradeList[42].data = 0;
				tradeWndDropGoldGet = 0;
			}
			/*
			if( opp_showindex == 3 ) {
			if( mount != -1 ) {
			tradeList[42].kind = 'G' ;
			tradeList[42].data = mount ;
			tradeWndDropGoldGet = mount;
			}else {
			tradeList[42].data = 0;
			tradeWndDropGoldGet = 0;
			}
			}else
			*/

#else
			if (opp_showindex == 1) {
				if (mount != -1) {
					showindex[4] = 2;
					tradeWndDropGoldGet = mount;
				}
				else {
					showindex[4] = 0;
					tradeWndDropGoldGet = 0;
				}
			}
			else if (opp_showindex == 2) {
				if (mount != -1) {
					showindex[5] = 2;
					tradeWndDropGoldGet = mount;
				}
				else {
					showindex[5] = 0;
					tradeWndDropGoldGet = 0;
				}
			}
			else return;
#endif

		}

		if (strcmp(trade_kind, "I") == 0) {
			char pilenum[256], item_freename[256], itemup[64];

			getStringToken(data, '|', 6, sizeof(opp_itemgraph) - 1, opp_itemgraph);

			getStringToken(data, '|', 7, sizeof(opp_itemname) - 1, opp_itemname);
			getStringToken(data, '|', 8, sizeof(item_freename) - 1, item_freename);

			getStringToken(data, '|', 9, sizeof(opp_itemeffect) - 1, opp_itemeffect);
			getStringToken(data, '|', 10, sizeof(opp_itemindex) - 1, opp_itemindex);
			getStringToken(data, '|', 11, sizeof(opp_itemdamage) - 1, opp_itemdamage);// 显示物品耐久度

#ifdef _ITEM_PILENUMS
			getStringToken(data, '|', 12, sizeof(pilenum) - 1, pilenum);//pilenum
#endif

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
			if (strcmp(opp_itemgraph, "-1") == 0)
				return;
			strcpy(opp_item[atoi(opp_itemindex) - MAX_ITEMSTART].name, makeStringFromEscaped(opp_itemname));
			strcpy(opp_item[atoi(opp_itemindex) - MAX_ITEMSTART].freename, makeStringFromEscaped(item_freename));
			strcpy(opp_item[atoi(opp_itemindex) - MAX_ITEMSTART].graph, opp_itemgraph);
			strcpy(opp_item[atoi(opp_itemindex) - MAX_ITEMSTART].effect, makeStringFromEscaped(opp_itemeffect));
			strcpy(opp_item[atoi(opp_itemindex) - MAX_ITEMSTART].itemindex, opp_itemindex);
			strcpy(opp_item[atoi(opp_itemindex) - MAX_ITEMSTART].damage, makeStringFromEscaped(opp_itemdamage));

			tradeList[atoi(opp_itemindex) - MAX_ITEMSTART + 22].kind = 'I';
			tradeList[atoi(opp_itemindex) - MAX_ITEMSTART + 22].data = atoi(opp_itemindex);
			strcpy(tradeList[atoi(opp_itemindex) - MAX_ITEMSTART + 22].name, makeStringFromEscaped(opp_itemname));

			strcpy(tradeList[atoi(opp_itemindex) - MAX_ITEMSTART + 22].freename, makeStringFromEscaped(item_freename));

			strcpy(tradeList[atoi(opp_itemindex) - MAX_ITEMSTART + 22].damage, makeStringFromEscaped(opp_itemdamage));
#ifdef _ITEM_PILENUMS
			tradeList[atoi(opp_itemindex) - MAX_ITEMSTART + 22].pilenum = atoi(pilenum);
#endif
#endif  //_TRADESYSTEM2
		}

		if (strcmp(trade_kind, "P") == 0) {
#ifdef _PET_ITEM
			int		iItemNo;
			char	szData[256];
#endif

#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
			getStringToken(data, '|', 12, sizeof(opp_index) - 1, opp_index);
			index = atoi(opp_index);
			getStringToken(data, '|', 6, sizeof(opp_pet[index].opp_petgrano) - 1, opp_pet[index].opp_petgrano);
			getStringToken(data, '|', 7, sizeof(opp_pet[index].opp_petname) - 1, opp_pet[index].opp_petname);
			getStringToken(data, '|', 8, sizeof(opp_pet[index].opp_petlevel) - 1, opp_pet[index].opp_petlevel);
			getStringToken(data, '|', 9, sizeof(opp_pet[index].opp_petatk) - 1, opp_pet[index].opp_petatk);
			getStringToken(data, '|', 10, sizeof(opp_pet[index].opp_petdef) - 1, opp_pet[index].opp_petdef);
			getStringToken(data, '|', 11, sizeof(opp_pet[index].opp_petquick) - 1, opp_pet[index].opp_petquick);
			getStringToken(data, '|', 12, sizeof(opp_pet[index].opp_petindex) - 1, opp_pet[index].opp_petindex);
			getStringToken(data, '|', 13, sizeof(opp_pet[index].opp_pettrans) - 1, opp_pet[index].opp_pettrans);
			getStringToken(data, '|', 14, sizeof(opp_pet[index].opp_petshowhp) - 1, opp_pet[index].opp_petshowhp);
			getStringToken(data, '|', 15, sizeof(opp_pet[index].opp_petslot) - 1, opp_pet[index].opp_petslot);
			getStringToken(data, '|', 16, sizeof(opp_pet[index].opp_petskill1) - 1, opp_pet[index].opp_petskill1);
			getStringToken(data, '|', 17, sizeof(opp_pet[index].opp_petskill2) - 1, opp_pet[index].opp_petskill2);
			getStringToken(data, '|', 18, sizeof(opp_pet[index].opp_petskill3) - 1, opp_pet[index].opp_petskill3);
			getStringToken(data, '|', 19, sizeof(opp_pet[index].opp_petskill4) - 1, opp_pet[index].opp_petskill4);
			getStringToken(data, '|', 20, sizeof(opp_pet[index].opp_petskill5) - 1, opp_pet[index].opp_petskill5);
			getStringToken(data, '|', 21, sizeof(opp_pet[index].opp_petskill6) - 1, opp_pet[index].opp_petskill6);
			getStringToken(data, '|', 22, sizeof(opp_pet[index].opp_petskill7) - 1, opp_pet[index].opp_petskill7);
			getStringToken(data, '|', 23, sizeof(realname), realname);
			getStringToken(data, '|', 24, sizeof(freename), freename);
#ifdef _SHOW_FUSION
			getStringToken(data, '|', 25, sizeof(opp_pet[index].opp_fusion) - 1, opp_pet[index].opp_fusion);
#endif
			strcpy(opp_pet[index].opp_petname, realname);
			strcpy(opp_pet[index].opp_petfreename, freename);
#endif

#ifdef _PET_ITEM
			for (int i = 0;; i++) {
				if (getStringToken(data, '|', 26 + i * 6, sizeof(szData), szData))
					break;
				iItemNo = atoi(szData);
				getStringToken(data, '|', 27 + i * 6, sizeof(opp_pet[index].oPetItemInfo[iItemNo].name), opp_pet[index].oPetItemInfo[iItemNo].name);
				getStringToken(data, '|', 28 + i * 6, sizeof(opp_pet[index].oPetItemInfo[iItemNo].memo), opp_pet[index].oPetItemInfo[iItemNo].memo);
				getStringToken(data, '|', 29 + i * 6, sizeof(opp_pet[index].oPetItemInfo[iItemNo].damage), opp_pet[index].oPetItemInfo[iItemNo].damage);
				getStringToken(data, '|', 30 + i * 6, sizeof(szData), szData);
				opp_pet[index].oPetItemInfo[iItemNo].color = atoi(szData);
				getStringToken(data, '|', 31 + i * 6, sizeof(szData), szData);
				opp_pet[index].oPetItemInfo[iItemNo].bmpNo = atoi(szData);
			}
#endif


#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
			tradeList[atoi(opp_pet[index].opp_petindex) + 37].data = atoi(opp_pet[index].opp_petindex);
			tradeList[atoi(opp_pet[index].opp_petindex) + 37].kind = 'P';
			strcpy(tradeList[atoi(opp_pet[index].opp_petindex) + 37].freename, freename);
			strcpy(tradeList[atoi(opp_pet[index].opp_petindex) + 37].name, realname);
			tradeList[atoi(opp_pet[index].opp_petindex) + 37].level = atoi(opp_pet[index].opp_petlevel);
			tradeList[atoi(opp_pet[index].opp_petindex) + 37].trns = atoi(opp_pet[index].opp_pettrans);
#ifdef _SHOW_FUSION
			tradeList[atoi(opp_pet[index].opp_petindex) + 37].fusion = atoi(opp_pet[index].opp_fusion);
#endif
#endif

			if (opp_showindex == 3) {
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
				if (strcmp(opp_pet[atoi(opp_index)].opp_petgrano, "-1") == 0) {
#endif
					showindex[6] = 0;
				}
				else {
					showindex[6] = 3;
				}
				DeathAction(pActPet5);
				pActPet5 = NULL;
			}
		}

		// shan trade(DoubleCheck) begin
		if (strcmp(trade_kind, "C") == 0) {
			if (pc.trade_confirm == 1) pc.trade_confirm = 3;
			if (pc.trade_confirm == 2)	pc.trade_confirm = 4;
		}
		// end

		if (strcmp(trade_kind, "A") == 0)
			tradeStatus = 2;

	}
	else if (strcmp(Head, "W") == 0) {//취소交易
		getStringToken(data, '|', 2, sizeof(buf_sockfd) - 1, buf_sockfd);
		getStringToken(data, '|', 3, sizeof(buf_name) - 1, buf_name);
		if ((strcmp(buf_sockfd, opp_sockfd) == 0) && (strcmp(buf_name, opp_name) == 0)) {
			tradeStatus = 0;
			MenuToggleFlag ^= JOY_CTRL_T;
			tradeInit();
			// shan trade(DoubleCheck)
			pc.trade_confirm = 1;
			play_se(203, 320, 240);

		}
	}
}

void tradeInit(void)
{
#ifdef _TRADESYSTEM2	// (不可开) Syu ADD 新交易系统
	//初始化时清空Action
	DeathAction(SecondTradeWnd);
	SecondTradeWnd = NULL;
	DeathAction(SecondActPet);
	SecondActPet = NULL;
	DeathAction(TradeTalkWnd);
	TradeTalkWnd = NULL;
	//Terry add 2003/11/25
	g_bTradesystemOpen = false;
	//end
#endif
	DeathAction(pActMenuWnd4);
	// 交易视窗初始化时将focus还给chat
	GetKeyInputFocus(&MyChatBuffer);
	Tradeflag = false;
	pActMenuWnd4 = NULL;
	DeathAction(pActPet3);
	pActPet3 = NULL;
	DeathAction(pActPet4);
	pActPet4 = NULL;
	DeathAction(pActPet5);
	pActPet5 = NULL;
	for (int i = 0; i <= 6; i++)
		showindex[i] = 0;
	strcpy(opp_sockfd, "-1");
	strcpy(opp_name, "");
	tradePetIndex = 0;
	strcpy(tradepetindexget, "-1");
	mine_itemindex[0] = -1;
	mine_itemindex[1] = -1;


	tradeWndDropGoldSend = 0;
	tradeWndDropGoldGet = 0;
	tradeWndDropGold = 0;
	strcpy(opp_item[0].itemindex, "-1");
	strcpy(opp_item[1].itemindex, "-1");
}

void BankmanInit(char *data)
{
	char token1[1024];
	getStringToken(data, '|', 3, sizeof(token1) - 1, token1);

	bankGold = atoi(token1);
	cashGold = pc.gold;
	totalGold = bankGold + cashGold;

	MenuToggleFlag = JOY_B;
	DeathAction(pActMenuWnd5);
	pActMenuWnd5 = NULL;
}

void closeBankman(void) {

	MenuToggleFlag ^= JOY_B;
	play_se(203, 320, 240);
	DeathAction(pActMenuWnd5);
	pActMenuWnd5 = NULL;

}

void checkRidePet(int pindex)
{
	int j;
#ifdef _PET_ITEM
	BOOL	bHavePetItem = FALSE;
#endif

	if (!bNewServer)
		return;
#ifdef _PET_ITEM
	// 宠身上有装备不可骑
	for (j = 0; j < MAX_PET_ITEM; ++j) {
		if (pet[pindex].item[j].useFlag) {	// 身上有装装备
			bHavePetItem = TRUE;
			break;
		}
	}
#endif
	if (pc.ridePetNo < 0
		&& pc.learnride >= pet[pindex].level //Change fix 这里被注解掉了 20050801打开
		//&& (pc.level + 5) > pet[pindex].level
		// 페트 탑승 레벨 부분
		// 캐릭터 레벨 + (( 캐릭터 환생 * 5 )+ 5 )
		// 0환 : 5
		// 1환 : 10
		// 2환 : 15
		// 3환 : 20
		// 4환 : 25
		// 5환 : 30
		// 6환 : 35
		&& (pc.level + ((pc.transmigration * 5) + 5)) >= pet[pindex].level
		&& pet[pindex].ai >= 100
		&& pc.graNo != SPR_pet021
		&& pc.graNo != 100362 //金飞
#ifdef _PETSKILL_BECOMEPIG // 乌力化中不可骑
		&& pc.graNo != 100250
#endif

#ifdef _PET_2TRANS // 二转宠不可骑
		&& pet[pindex].trn <= 1
#endif
		)
	{
#ifdef _PET_ITEM
		pc.ridePetNo = -1;
		if (bHavePetItem) {	// 有装备不可骑
			StockChatBufferLine("펫장비가 장착되있으면 탑승할수없어요！", FONT_PAL_YELLOW);
			pc.selectPetNo[pindex] = 0;
			return;
		}
#endif
#ifdef _NB_可骑乘任何宠物
		{
			char buf[64];
			sprintf(buf, "R|P|%d", pindex);
			if (bNewServer)
				lssproto_FM_send(sockfd, buf);
			else
				old_lssproto_FM_send(sockfd, buf);
			pc.ridePetNo = pindex;
		}
#else
		for (j = 0; j < sizeof(ridePetTable) / sizeof(tagRidePetTable); j++) {
			int baseimageNo = pc.graNo - (pc.graNo % 5);
			int leaderimageNo = 100700 + ((baseimageNo - 100000) / 20) * 10 + (pc.familySprite) * 5;
			//andy_edit
			if (((ridePetTable[j].charNo == pc.graNo) || (ridePetTable[j].charNo == pc.baseGraNo)) && ridePetTable[j].petNo == pet[pindex].graNo) {
				char buf[64];
				sprintf(buf, "R|P|%d", pindex);
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
				pc.ridePetNo = pindex;
				return;
			}
#ifdef _LEADERRIDE
			if (ridePetTable[j].charNo == leaderimageNo && ridePetTable[j].petNo == pet[pindex].graNo &&
				pc.big4fm != 0 && pc.familyleader != FMMEMBER_APPLY && pc.familyleader != FMMEMBER_NONE) {
				char buf[64];
				sprintf(buf, "R|P|%d", pindex);
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
				pc.ridePetNo = pindex;
				return;
			}
#endif
		}
#endif
		{
#ifdef _PET_ITEM
			if (bHavePetItem) {	// 有装备不可骑
				StockChatBufferLine("펫장비가 장착되있으면 탈수없어요！", FONT_PAL_YELLOW);
				pc.selectPetNo[pindex] = 0;
				return;
			}
#endif
			//andy_add 新骑宠
			int ti = -1, index;
#ifdef _NB_可骑乘任何宠物
			{
				if ((index = RIDEPET_getNOindex(pc.baseGraNo)) >= 0) {
					char buf[64];
					sprintf(buf, "R|P|%d", pindex);
					if (bNewServer)
						lssproto_FM_send(sockfd, buf);
					else
						old_lssproto_FM_send(sockfd, buf);
					pc.ridePetNo = pindex;
					return;
				}
			}
#else
			unsigned int LRCode = 1 << 30;
			if ((ti = RIDEPET_getPETindex(pet[pindex].graNo, pc.lowsride)) < 0) {
			}
			if ((index = RIDEPET_getNOindex(pc.baseGraNo)) >= 0) {
				char buf[64];
				if (RIDEPET_getRIDEno(index, ti) >= 0) {

					sprintf(buf, "R|P|%d", pindex);
					if (bNewServer)
						lssproto_FM_send(sockfd, buf);
					else
						old_lssproto_FM_send(sockfd, buf);
					pc.ridePetNo = pindex;
					return;
				}
			}
#endif
		}
	}
}
//andy_add 2002/06/24
int RIDEPET_getNOindex(int baseNo)
{
	int i;
	for (i = 0; i < sizeof(RPlistMode) / sizeof(tagRidePetList); i++) {
		if (RPlistMode[i].charNo == baseNo)
			return RPlistMode[i].Noindex;
	}
	return -1;
}
//andy_add 2002/06/24
int RIDEPET_getPETindex(int PetNo, int learnCode)
{
	int i;
	for (i = 0; i < sizeof(RideCodeMode) / sizeof(tagRideCodeMode); i++) {
		if (RideCodeMode[i].petNo == PetNo &&
			(RideCodeMode[i].learnCode & learnCode))
			return i;
	}
	return -1;
}

//andy_add 2002/06/24
int RIDEPET_getRIDEno(int index, int ti)
{
	if (index < 0 || index >= sizeof(RideNoList) / sizeof(tagRideNoList))
		return -1;
	if (ti < 0 || ti >= MAXNOINDEX)
		return -1;
	return RideNoList[index].RideNo[ti];
}

#ifdef _TELLCHANNEL				//ROG ADD 密语频道
void InitSelectChar(char *msg, BOOL endFlag)
{
	int turn, level;
	char name1[32];
	if (MultiTells == TRUE)
		DeathTellChannel();

	if (!endFlag) {
#ifndef _CHANNEL_MODIFY
		index[CharNum] = getIntegerToken(msg, '|', 2);
		turn = getIntegerToken(msg, '|', 3);
		level = getIntegerToken(msg, '|', 4);
		getStringToken(msg, '|', 5, sizeof(name[CharNum]) - 1, name[CharNum]);
		getStringToken(msg, '|', 6, sizeof(name1) - 1, name1);
#else
		index[CharNum] = getIntegerToken(msg, '|', 3);
		turn = getIntegerToken(msg, '|', 4);
		level = getIntegerToken(msg, '|', 5);
		getStringToken(msg, '|', 6, sizeof(name[CharNum]) - 1, name[CharNum]);
		getStringToken(msg, '|', 7, sizeof(name1) - 1, name1);
#endif
		sprintf(TellInfo[CharNum], " %15s %15s  %5d  %5d   ", name[CharNum], name1, turn, level);
		CharNum++;
	}
	else {
#ifndef _CHANNEL_MODIFY
		getStringToken(msg, '|', 2, sizeof(reSendMsg) - 1, reSendMsg);
#else
		getStringToken(msg, '|', 3, sizeof(reSendMsg) - 1, reSendMsg);
#endif
		MultiTells = TRUE;
	}
	//test//////////////
#ifdef _TIMEBAR_FUNCTION
	StartTime = timeGetTime();
	timBarIdent = SetTimeBar("aaaaaa", 50);
#endif
	//////////////////
}

void SelectChar(void)
{
	char tmpMsg[1024];
	int x, y;
	if (pActMsgWnd == NULL) {
		pActMsgWnd = MakeWindowDisp(80, 200, 6, CharNum / 2 + 1, 2, 0);
	}
	else if (pActMsgWnd->hp > 0) {
		x = pActMsgWnd->x;
		y = pActMsgWnd->y;
		char title[] = { "           이 름           별 명  환 생  레 벨 " };
		StockFontBuffer(x + 10, y + 15, FONT_PRIO_FRONT, 4, title, 0);
		for (int i = 0; i < CharNum; i++) {
			StockFontBuffer(x + 10, y + 40 + i * 20, FONT_PRIO_FRONT, 5, TellInfo[i], 0);
			if (MakeHitBox(x + 10, y + 37 + i * 20, x + 360, y + 56 + i * 20, DISP_PRIO_BOX2)) {
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					sprintf(tmpMsg, "%s %s /T%d  ", name[i], reSendMsg, index[i]);
					TalkMode = 1;
					chatStrSendForServer(tmpMsg, 0);
					DeathTellChannel();
				}
			}
		}
	}

	//test//////////////
#ifdef _TIMEBAR_FUNCTION
	int Now = timeGetTime();
	Now -= StartTime;
	int rsut = SetTimeBarPos(timBarIdent, Now / 1000);
	if (rsut == 1)
		StartTime += 50000;
	else if (rsut == -1) {
		DeathAction(pTimeBarWnd);
		pTimeBarWnd = NULL;
	}

#endif
	////////////////////
}

void DeathTellChannel(void)
{
	DeathAction(pActMsgWnd);
	pActMsgWnd = NULL;
	MultiTells = FALSE;
	CharNum = 0;
}

#endif

#ifdef _FRIENDCHANNEL				//ROG ADD 好友频道

void initSetRoomName()
{
	DeathMenuAction();
	DeathMenuAction2();
	DeathAction(pSetRoomWnd);
	pSetRoomWnd = NULL;
	GetKeyInputFocus(&MyChatBuffer);
	setRoomFlag = TRUE;
	SelRoomBtn = 0;
}

void setRoomName(void)
{
	static int setRoomBtn[2];				//设定聊天室名称用

	if (pSetRoomWnd == NULL) {
		pSetRoomWnd = MakeWindowDisp(270, 0, 3, 2, NULL, 0);
		play_se(202, 320, 240);
		chatRoomName.buffer[0] = NULL;
		chatRoomName.cnt = 0;
		chatRoomName.cursor = 0;
		chatRoomName.len = 16;
		chatRoomName.color = 0;
		chatRoomName.x = pSetRoomWnd->x + 22;
		chatRoomName.y = pSetRoomWnd->y + 35;
		chatRoomName.fontPrio = FONT_PRIO_FRONT;
	}
	else if (pSetRoomWnd->hp > 0) {
		int x = pSetRoomWnd->x;
		int y = pSetRoomWnd->y;

		setRoomBtn[0] = StockDispBuffer(pSetRoomWnd->x + 53, pSetRoomWnd->y + 70, DISP_PRIO_IME3, CG_OK_BTN, 2);
		setRoomBtn[1] = StockDispBuffer(pSetRoomWnd->x + 140, pSetRoomWnd->y + 70, DISP_PRIO_IME3, CG_CANCEL_BTN, 2);

		char title[] = { "채널 이름을 입력하시기 바랍니다" };
		StockFontBuffer(x + 22, y + 10, FONT_PRIO_FRONT, 4, title, 0);
		GetKeyInputFocus(&chatRoomName);
		StockFontBuffer2(&chatRoomName);

		if (mouse.onceState & MOUSE_LEFT_CRICK) {
			if (HitDispNo == setRoomBtn[0])
				KeyboardReturn();
			else if (HitDispNo == setRoomBtn[1]) {
				GetKeyInputFocus(&MyChatBuffer);
#ifdef _CHATROOMPROTOCOL
				lssproto_CHATROOM_send(sockfd, "B|");
#endif
				DeathAction(pSetRoomWnd);
				pSetRoomWnd = NULL;
				setRoomFlag = FALSE;
			}
		}
	}
}

void InitSelChatRoom(char *msg)
{
	char tempRoomNum[32], RoomName[32];
	int memberNum = 0;
	char chiefName[128] = { "" };
	char temp[16], *temp1;
	int i = 2, k = 0;
	roomNum = 0;

	while (getStringToken(msg, '|', i, sizeof(tempRoomNum) - 1, tempRoomNum) != 1) {
		if (tempRoomNum[0] == 'r') {
			temp1 = tempRoomNum;
			temp1++;
			roomNum = atoi(temp1);

			getStringToken(msg, '|', i + 1, sizeof(RoomName) - 1, RoomName);
			getStringToken(msg, '|', i + 3, sizeof(chiefName) - 1, chiefName);
			getStringToken(msg, '|', i + 4, sizeof(temp) - 1, temp);

			if ((temp1 = strtok(temp, "p")) != NULL)
				memberNum = atoi(temp1);

			roomIndex[k] = roomNum;
			sprintf(roomInfo[k], "  %16s  %16s   %2d ", RoomName, chiefName, memberNum);
			k++;
			roomNum++;
			i += 5;
		}
		else
			break;
	}
	SelRoomBtn = 1;
}

#ifdef _CHATROOMPROTOCOL
void SelectChatRoom(void)
{
	char tmpMsg[128] = { "" };

	if (pSelChanlWnd == NULL) {
		pSelChanlWnd = MakeWindowDisp(120, 100, 7, roomNum / 2 + 2, 2, 0);
	}
	else if (pSelChanlWnd->hp > 0) {
		int x = pSelChanlWnd->x;
		int y = pSelChanlWnd->y;
		char title[] = { "          체널이름          파티장이름  인원 수 " };
		StockFontBuffer(x + 10, y + 15, FONT_PRIO_FRONT, 4, title, 0);
		int i;
		for (i = 0; i < roomNum; i++) {
			StockFontBuffer(x + 10, y + 40 + i * 20, FONT_PRIO_FRONT, 5, roomInfo[i], 0);
			if (MakeHitBox(x + 10, y + 37 + i * 20, x + 320, y + 58 + i * 20, DISP_PRIO_BOX2)) {
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					sprintf(tmpMsg, "J|%d", roomIndex[i]);
					lssproto_CHATROOM_send(sockfd, tmpMsg);
					SelRoomBtn = 0;         //关闭选择频道视窗
				}
			}
		}

		StockFontBuffer(x + 30, y + 50 + i * 20, FONT_PRIO_FRONT, 5, "  새로운 채팅방 만들기", 0);
		if (MakeHitBox(x + 30, y + 47 + i * 20, x + 340, y + 68 + i * 20, DISP_PRIO_BOX2)) {
			if (mouse.onceState & MOUSE_LEFT_CRICK)
				initSetRoomName();
		}
	}
}
#endif

void SwapOrder(int pos1, int pos2)
{
	char    tempName[CHAR_NAME_LEN + 1];
	int     tempIndex;
	char    tempNick[CHAR_FREENAME_LEN + 1];

	strcpy(tempName, chatInfo.memberName[pos1]);
	strcpy(tempNick, chatInfo.nickName[pos1]);
	tempIndex = chatInfo.memberIndex[pos1];

	strcpy(chatInfo.memberName[pos1], chatInfo.memberName[pos2]);
	strcpy(chatInfo.nickName[pos1], chatInfo.nickName[pos2]);
	chatInfo.memberIndex[pos1] = chatInfo.memberIndex[pos2];

	strcpy(chatInfo.memberName[pos2], tempName);
	strcpy(chatInfo.nickName[pos2], tempNick);
	chatInfo.memberIndex[pos2] = tempIndex;
}

void InitRoomInfo()
{
	for (int i = 0; i < 50; i++) {
		chatInfo.memberIndex[i] = 0;
		strcpy(chatInfo.memberName[i], "");
		strcpy(chatInfo.nickName[i], "");
	}
	chatInfo.chiefFlag = 0;
	chatInfo.chiefIndex = 0;
	chatInfo.memberNum = 0;
	firMemNo = 0;
}

void InitCreateChatRoom(char *msg)		//初始化聊天室视窗
{
	char temp[64], *temp1;
	chatInfo.chiefFlag = 0;
	InitRoomInfo();					    //初始化参数
	chatRoomBtn = 1;
	getStringToken(msg, '|', 2, sizeof(temp) - 1, temp);
	if ((temp1 = strtok(temp, "r")) != NULL) {
		strcpy(chatInfo.roomNo, temp1);
		strcpy(pc.chatRoomNum, chatInfo.roomNo);

		getStringToken(msg, '|', 3, sizeof(chatInfo.roomName) - 1, chatInfo.roomName);

		chatInfo.chiefIndex = getIntegerToken(msg, '|', 4);

		getStringToken(msg, '|', 5, sizeof(temp) - 1, temp);
		strcpy(chatInfo.chiefName, temp);

		getStringToken(msg, '|', 6, sizeof(temp) - 1, temp);

		if ((temp1 = strtok(temp, "p")) != NULL)
			chatInfo.memberNum = atoi(temp1);
		int i;
		for (i = 0; i < chatInfo.memberNum; i++) {
			getStringToken(msg, '|', 7 + i * 3, sizeof(temp) - 1, temp);
			if ((temp1 = strchr(temp, 'I')) == NULL)
				break;
			chatInfo.memberIndex[i] = atoi(temp1 + 1);
			getStringToken(msg, '|', 8 + i * 3, sizeof(chatInfo.memberName[i]) - 1
				, chatInfo.memberName[i]);
			getStringToken(msg, '|', 9 + i * 3, sizeof(chatInfo.nickName[i]) - 1
				, chatInfo.nickName[i]);
			if (i > 0 && chatInfo.memberIndex[i] == chatInfo.chiefIndex)
				SwapOrder(i, 0);                 //室长排序
			secretFlag = FALSE;
			selChar = -1;
		}
		chatInfo.chiefFlag = getIntegerToken(msg, '|', 9 + i * 3 - 2);
	}
#ifdef _CHANNEL_MODIFY
	pc.etcFlag |= PC_ETCFLAG_CHAT_CHAT;
#endif
}

#ifdef _CHATROOMPROTOCOL
void ChatRoomWnd(void)			//聊天室视窗
{
	char tmpMsg[STR_BUFFER_SIZE];
	int nameColor;
	int scrlLenth = 183;

	if (pChtChanlWnd == NULL) {
		pChtChanlWnd = MakeWindowDisp(350, 60, 540, 456, 0, -1);
	}
	else if (pChtChanlWnd->hp > 0) {
		int	x = pChtChanlWnd->x + 2;
		int y = pChtChanlWnd->y - 5;

		int scrTop = y + 69;           //Scroll高度

		StockDispBuffer(((WINDOW_DISP *)pChtChanlWnd->pYobi)->mx - 120, ((WINDOW_DISP *)pChtChanlWnd->pYobi)->my - 28, DISP_PRIO_MENU, CG_FIELD_CHATROOM_PANEL, 1);
		char title[32] = { "" };

		sprintf(title, "%s", chatInfo.roomName);

		StockFontBuffer(x + 140 - strlen(title) * 4, y + 50, FONT_PRIO_FRONT, 4, title, 0);

		if (!BtnType) {
			closeBtn = CG_FIELD_CLOSE_BTN_UP;
			leaveBtn = CG_FIELD_LEAVE_BTN_UP;
			scrlHBtn = CG_FIELD_SCROLL_HUP;
			scrlLBtn = CG_FIELD_SCROLL_LUP;

			if (chatInfo.chiefFlag == 1) {									//队长专有按钮
				delBtn = CG_FIELD_DELETE_BTN_UP;
				if (secretFlag && chatInfo.chiefIndex != chatInfo.memberIndex[selChar]) {
					outBtn = CG_FIELD_OUTMEMBER_BTN_UP;
					chaBtn = CG_FIELD_CHANGECHIEF_BTN_UP;
				}
				else {
					outBtn = CG_FIELD_OUTMEMBER_BTN_DISABLE;
					chaBtn = CG_FIELD_CHANGECHIEF_BTN_DISABLE;
				}
			}
			else {
				outBtn = CG_FIELD_OUTMEMBER_BTN_DISABLE;
				chaBtn = CG_FIELD_CHANGECHIEF_BTN_DISABLE;
				delBtn = CG_FIELD_DELETE_BTN_DISABLE;
			}
		}
		ChatRoomBtn[0] = StockDispBuffer(x + 70, y + 335, DISP_PRIO_IME3, closeBtn, 2);
		ChatRoomBtn[1] = StockDispBuffer(x + 230, y + 335, DISP_PRIO_IME3, leaveBtn, 2);
		ChatRoomBtn[2] = StockDispBuffer(x + 150, y + 305, DISP_PRIO_IME3, outBtn, 2);
		ChatRoomBtn[3] = StockDispBuffer(x + 230, y + 305, DISP_PRIO_IME3, chaBtn, 2);
		ChatRoomBtn[4] = StockDispBuffer(x + 70, y + 305, DISP_PRIO_IME3, delBtn, 2);
		ChatRoomBtn[5] = StockDispBuffer(x + 267, scrTop + scrlBtnIndex, DISP_PRIO_IME3, CG_FIELD_CHANNEL_SCROLL, 2);
		ChatRoomBtn[6] = StockDispBuffer(x + 267, scrTop - 20, DISP_PRIO_IME3, scrlHBtn, 2);
		ChatRoomBtn[7] = StockDispBuffer(x + 267, y + 274, DISP_PRIO_IME3, scrlLBtn, 2);

		for (int i = 0; i < 10; i++) {
			if (chatInfo.memberNum - 1 < i + firMemNo)
				break;
			//人名变色
			if (MakeHitBox(x + 20, y + 77 + i * 20, x + 250, y + 96 + i * 20, DISP_PRIO_BOX2)) {
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					if (strcmp(chatInfo.memberName[i + firMemNo], pc.name) != 0 || strcmp(chatInfo.nickName[i + firMemNo], pc.freeName) != 0) {
						if (secretFlag == TRUE && selChar == i) {
							strcpy(secretName, "");
							pNowStrBuffer->buffer[0] = NULL;
							pNowStrBuffer->cursor = 0;
							pNowStrBuffer->cnt = 0;
							StrToNowStrBuffer(secretName);
							secretFlag = FALSE;
							selChar = -1;
							TalkMode = 0;
						}
						else {
							sprintf(secretName, "%s ", chatInfo.memberName[i + firMemNo]);
							pNowStrBuffer->buffer[0] = NULL;
							pNowStrBuffer->cursor = 0;
							pNowStrBuffer->cnt = 0;
							StrToNowStrBuffer(secretName);
							secretFlag = TRUE;
							TalkMode = 1;
							selChar = i + firMemNo;
						}
					}
				}
			}

			if (secretFlag && selChar == i + firMemNo)
				nameColor = 4;
			else
				nameColor = 5;

			StockFontBuffer(x + 30, y + 80 + i * 20, FONT_PRIO_FRONT, nameColor, chatInfo.memberName[i + firMemNo], 0);
			StockFontBuffer(x + 160, y + 80 + i * 20, FONT_PRIO_FRONT, nameColor, chatInfo.nickName[i + firMemNo], 0);

		}

		for (int i = 0; i < 8; i++) {
			if (i == 2 && chatInfo.chiefFlag == 0)		//不是队长则跳过三个按钮
				i = 5;
			else if (i == 2 && chatInfo.chiefFlag == 1 && selChar == -1)			//是队长未选人
				i = 4;

			if (HitDispNo == ChatRoomBtn[i]) {
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					switch (i) {
					case 0:
						closeBtn = CG_FIELD_CLOSE_BTN_DOWN;
						break;
					case 1:
						leaveBtn = CG_FIELD_LEAVE_BTN_DOWN;
						break;
					case 2:
						outBtn = CG_FIELD_OUTMEMBER_BTN_DOWN;
						break;
					case 3:
						chaBtn = CG_FIELD_CHANGECHIEF_BTN_DOWN;
						break;
					case 4:
						delBtn = CG_FIELD_DELETE_BTN_DOWN;
						break;
					case 5:
						scrollFlag = TRUE;
						break;
					case 6:
						if (firMemNo > 0)
						{
							firMemNo--;
							scrlBtnIndex = (scrlLenth / (chatInfo.memberNum - 10))  * firMemNo;
							scrlHBtn = CG_FIELD_SCROLL_HDOWN;
						}
						break;
					case 7:
						if (chatInfo.memberNum > firMemNo + 10 && chatInfo.memberNum > 10)
						{
							firMemNo++;
							scrlBtnIndex = (scrlLenth / (chatInfo.memberNum - 10))  * firMemNo;
							scrlLBtn = CG_FIELD_SCROLL_LDOWN;
						}
						break;
					}
					BtnNo = i;
					BtnType = TRUE;

				}
				else if (mouse.onceState & MOUSE_LEFT_CRICK_UP) {
					switch (i) {
					case 0:						//关闭视窗
						DeathMenuAction();
						break;

					case 1:						//离开频道
						if (chatInfo.chiefFlag == 1)
							StockChatBufferLine("채팅 방장은 채팅방에서 나갈 수 없습니다. 나가고 싶을 경우 다른사람에게 방장을 넘기시기 바랍니다！", FONT_PAL_RED);
						else {
							lssproto_CHATROOM_send(sockfd, "L|");
							strcpy(pc.chatRoomNum, "");
							DeathMenuAction();
							StockChatBufferLine("당신은 이미 채팅방을 떠났습니다.", FONT_PAL_BLUE);
							TalkMode = 0;
#ifdef _CHANNEL_MODIFY
							pc.etcFlag &= ~PC_ETCFLAG_CHAT_CHAT;
#endif
						}
						break;

					case 2:						//踢人
						sprintf(tmpMsg, "K|%d", chatInfo.memberIndex[selChar]);
						lssproto_CHATROOM_send(sockfd, tmpMsg);
						break;

					case 3:					//换队长
						sprintf(tmpMsg, "M|%d", chatInfo.memberIndex[selChar]);
						lssproto_CHATROOM_send(sockfd, tmpMsg);
						TalkMode = 0;
						break;

					case 4:					//删除频道
						lssproto_CHATROOM_send(sockfd, "D|");
						strcpy(pc.chatRoomNum, "");
						TalkMode = 0;
#ifdef _CHANNEL_MODIFY
						pc.etcFlag &= ~PC_ETCFLAG_CHAT_CHAT;
#endif
						break;
					}
					secretFlag = FALSE;
					selChar = -1;

				}
			}
		}

		if (mouse.onceState & MOUSE_LEFT_CRICK_UP || HitDispNo != ChatRoomBtn[BtnNo])
			BtnType = FALSE;

		if (mouse.onceState & MOUSE_LEFT_CRICK_UP || mouse.nowPoint.x < x)
			scrollFlag = FALSE;

		if (scrollFlag) {
			if (mouse.nowPoint.y > scrTop - 2 && mouse.nowPoint.y < (scrTop + scrlLenth + 4)) {
				if (chatInfo.memberNum > 10) {
					firMemNo = (mouse.nowPoint.y - scrTop) / (scrlLenth / (chatInfo.memberNum - 10));
					scrlBtnIndex = mouse.nowPoint.y - scrTop;
				}
				else
					scrlBtnIndex = 0;
			}
		}
	}
}
#endif

void initAssentWnd(char *data)			//要求加入视窗
{
	assentFlag = TRUE;
	char temp[64];
	getStringToken(data, '|', 2, sizeof(temp) - 1, temp);
	sprintf(memInfo, "%s 가입신청", temp);
	memIndex = getIntegerToken(data, '|', 3);
}

#ifdef _CHATROOMPROTOCOL
void AssentWnd(void)                    //要求加入视窗
{
	int i = 0;
	char tmpMsg[128] = { "" };

	int AssentBtn[2];
	int x, y;
	if (pAssentWnd == NULL)
		pAssentWnd = MakeWindowDisp(270, 0, 3, 2, 2, 0);
	else if (pAssentWnd->hp > 0) {
		x = pAssentWnd->x;
		y = pAssentWnd->y;

		StockFontBuffer(x + 20, y + 30, FONT_PRIO_FRONT, 5, memInfo, 0);
		AssentBtn[0] = StockDispBuffer(x + 90, y + 60, DISP_PRIO_IME3, CG_FIELD_AGREE_BTN, 2);
		AssentBtn[1] = StockDispBuffer(x + 100, y + 60, DISP_PRIO_IME3, CG_FIELD_DISAGREE_BTN, 2);

		for (i = 0; i < 2; i++) {
			if (HitDispNo == AssentBtn[i]) {
				if (mouse.onceState & MOUSE_LEFT_CRICK) {
					if (i == 0) {
						sprintf(tmpMsg, "A|%d|1", memIndex);
						lssproto_CHATROOM_send(sockfd, tmpMsg);
					}
					else {
						sprintf(tmpMsg, "A|%d|0", memIndex);
						lssproto_CHATROOM_send(sockfd, tmpMsg);
					}
					DeathAction(pAssentWnd);
					pAssentWnd = NULL;
					assentFlag = FALSE;
				}
			}
		}
	}
}
#endif

#ifdef _CHANNEL_MODIFY
void SaveChatData(char *msg, char KindOfChannel, bool bCloseFile);
#endif

void InitRecvMsg(char *data)
{
	char temp[STR_BUFFER_SIZE];
	char msg[STR_BUFFER_SIZE];
	getStringToken(data, '|', 2, sizeof(temp) - 1, temp);
#ifndef _CHANNEL_MODIFY
	sprintf(msg, "[频道]%s", temp);
#else
	sprintf(msg, "[대화]%s", temp);
	TradeTalk(msg);
	SaveChatData(msg, 'R', false);
#endif
	StockChatBufferLine(msg, 2);
}

#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
void lssproto_CHATROOM_recv(int fd, char *data)
{
	char type[3] = { "" };
	int i = 0;
	getStringToken(data, '|', 1, sizeof(type) - 1, type);
	switch (type[0]) {
	case 'B':
		InitSelChatRoom(data);
		break;
	case 'D':						//Delete
		chatRoomBtn = 0;
		strcpy(pc.chatRoomNum, "");
		StockChatBufferLine("채팅방이 이미 삭제되었어요！", FONT_PAL_RED);
#ifdef _CHANNEL_MODIFY
		pc.etcFlag &= ~PC_ETCFLAG_CHAT_CHAT;
#endif
		break;
	case 'T':
		InitRecvMsg(data);    //处理讯息
		break;
	case 'K'://剔除
		chatRoomBtn = 0;
		strcpy(pc.chatRoomNum, "");
		StockChatBufferLine("채팅방에서 강퇴되었어요！", FONT_PAL_RED);
#ifdef _CHANNEL_MODIFY
		pc.etcFlag &= ~PC_ETCFLAG_CHAT_CHAT;
#endif
		break;
	case 'J'://加入申请
		if (assentFlag == FALSE)
		{
			DeathAction(pAssentWnd);
			pAssentWnd = NULL;
			initAssentWnd(data);
		}
		break;
	case 'R':			//更新聊天室资讯
		InitCreateChatRoom(data);
		break;
	}
}
#endif

#endif

#ifdef _TIMEBAR_FUNCTION
/***********************************************************
参数:
title :  TimeBar 标头  范围32bit
range :  TimeBar 范围
回传值:	 TimeBar 识别用.用以设定正确的值

barHolder[timBarIdent] : 可用以判断现在是否为此函式的拥有者
***********************************************************/
int SetTimeBar(char *title, int range)
{
	int timBarIdent = 0;
	if (strlen(title) > 32)
		return -1;
	if (timeBarFlag = TRUE) {
		DeathAction(pTimeBarWnd);
		pTimeBarWnd = NULL;
		if (barHolder[0]) {
			barHolder[0] = FALSE;
			barHolder[1] = TRUE;
			timBarIdent = 1;
		}
		else {
			barHolder[1] = FALSE;
			barHolder[0] = TRUE;
			timBarIdent = 0;
		}
	}
	else {
		barHolder[0] = TRUE;
		timBarIdent = 0;
	}
	strcpy(timeBarTitle, title);
	timeBarRange = range;
	timeBarFlag = TRUE;

	return timBarIdent;
}
/*******************************************
return -1: 注意!!代表无法设定现在的位址,可能被其他程式占用
return  1: 已经完成 100 %
*******************************************/
int SetTimeBarPos(int timBarIdent, int CurPos)
{
	if (!barHolder[timBarIdent])//|| CurPos > timeBarRange)
		return -1;
	timeBarCurPos = 20 * CurPos / timeBarRange;  //20为bar的单位总数.
	if (timeBarCurPos > 20) {
		timeBarCurPos = 20;
		return 1;				//已经到达100 %了
	}
	return 0;
}

void DrawTimeBar()
{
	if (pTimeBarWnd == NULL) {
		pTimeBarWnd = MakeWindowDisp(270, 0, 3, 2, 2, 0);
	}
	else if (pTimeBarWnd->hp > 0) {
		int x = pTimeBarWnd->x;
		int y = pTimeBarWnd->y;
		char percent[4] = { "" };
		sprintf(percent, "%d", (timeBarCurPos * 5));
		StockFontBuffer(x + 20, y + 30, FONT_PRIO_FRONT, 5, timeBarTitle, 0);
		StockFontBuffer(x + 50, y + 50, FONT_PRIO_FRONT, 5, percent, 0);
		for (int i = 0; i < timeBarCurPos; i++)
			StockDispBuffer(x + i * 10, y + 60, DISP_PRIO_IME3, CG_TIMEBAR_UNIT, 2);
	}
}
#endif

#ifdef _STANDBYPET
void send_StandBy_Pet(void)
{
	int	standbypet = 0;
	int i, cnt = 0;
	for (i = 0; i < MAX_PET; i++) {
		if (pc.selectPetNo[i] == TRUE) {
			cnt++;
			if (cnt > 等待宠数量)
				break;
			standbypet |= (1 << i);
		}
	}
	lssproto_SPET_send(sockfd, standbypet);
	StandbyPetSendFlag = TRUE;
}
#endif

void lssproto_CHAREFFECT_recv(int fd, char *data) {
	if (data[0] == '1')
		/*setCharFamily(pc.ptAct, atoi(data + 2))*/;
	else if (data[0] == '2')
		setCharMind(pc.ptAct, atoi(data + 2));
	else if (data[0] == '3')
		/*setCharmFamily(pc.ptAct, atoi(data + 2))*/;
#ifdef 人物称号
	else if (data[0] == '4') {
		setCharmTitle(pc.ptAct, atoi(data + 2));
	}
#endif
#ifdef 人物光环
	else if (data[0] == '5')
		setCharmManor(pc.ptAct, atoi(data + 2));
#endif
}


void lssproto_BATTLEBUTTON_recv(int fd, int help, int escp) 
{
	NoHelpFlag = help;
	NoEscFlag = escp;
}

/* =========================================

注意!!
这个档案已经超过1万5千多行了，造成VC编辑器效率低落，
请尽量不要在后面增加新Code了。

~Robin~

所以请加到menu2.cpp  by  Change
========================================= */

/*#ifdef _ON_OFF_ITEM
static int exp_up_btn = -1;

void EXP_ON_OFF_ITEM()
{
if (mouse.autoState & MOUSE_LEFT_CRICK && HitDispNo == exp_up_btn)
{
lssproto_Expup_send(sockfd);
play_se(217, 320, 240);
}
//exp_up_btn = StockDispBuffer(340, 20, DISP_PRIO_IME3, 56040, 2);
//ShowBottomLineString(FONT_PAL_WHITE, "경험치 구슬을 사용하거나 사용을 종료합니다.");
}
#endif

#ifdef _ON_OFF_NOENEMY
static int noenemy_btn = -1;

void NOENEMY_OF_OFF_ITEM()
{
if (mouse.autoState & MOUSE_LEFT_CRICK && HitDispNo == noenemy_btn)
{
lssproto_Noenemy_send(sockfd);
play_se(217, 320, 240);
}
//noenemy_btn = StockDispBuffer(450, 20, DISP_PRIO_IME3, 56039, 2);
//ShowBottomLineString(FONT_PAL_WHITE, "벽청의 구슬을 사용하거나 사용을 종료합니다.");
}
#endif

#ifdef _CLICK_PARTYAUTO
static int partyauto_btn = -1;

void CLICK_PARTYAUTO()
{
if (mouse.autoState & MOUSE_LEFT_CRICK && HitDispNo == partyauto_btn)
{
lssproto_Partyauto_send(sockfd);
play_se(217, 320, 240);
}
//partyauto_btn = StockDispBuffer(632, 19, DISP_PRIO_IME3, 60011, 2);
//ShowBottomLineString(FONT_PAL_WHITE, "해당지역에 있는 파티를 검색합니다.");
}
#endif*/

Mammoth::Mammoth()
{
	m_OpenFlag = FALSE;
}

Mammoth& Mammoth::Get()
{
	static Mammoth i;
	return i;
}

void Mammoth::Display()
{
	if (joy_trg[0] & JOY_ESC)
	{
		Close();
		return;
	}
	MouseControl();

	static int LastTime = 0;
	static int Cursor = 0;
	static char buffer[25];

	//int StockDispBuffer(int x, int y, UCHAR dispPrio, int bmpNo, BOOL hitFlag)
	//BG
	StockDispBuffer(X, Y, DISP_PRIO_MENU, 60000, 1);

	if (TimeGetTime() > LastTime)
	{
		Cursor++;
		LastTime = TimeGetTime() + 500;
	}

	StockDispBuffer(X + 105, Y - 150, DISP_PRIO_IME3, 60002 + (Cursor % 2), 2);
	Buttons.at(BUTTONS::EXIT) = StockDispBuffer(X + 234, Y - 150, DISP_PRIO_IME3, 60004 + (Cursor % 2), 2);

	//TEXT
	sprintf(buffer, "%d", m_CouponCount);
	StockFontBuffer(X - 25, Y - 158, FONT_PRIO_FRONT, FONT_PAL_WHITE, buffer, 0);

	DisplayRewardList();
	switch (m_Status)
	{
	case _STATUS::STARTING:
	{
		if (m_Fail == _FAIL::NO_COUPON)
		{
			StockDispBuffer(X, Y, DISP_PRIO_IME3, 60014, 1);
		}
		else if (m_Fail == _FAIL::BAG_FULL)
		{
			StockDispBuffer(X, Y, DISP_PRIO_IME3, 60010, 1);
		}
		else
		{
			Buttons.at(BUTTONS::START) = StockDispBuffer(X, Y + 100, DISP_PRIO_IME3, 60008, 2);
		}
		break;
	}
	case _STATUS::ANIMATING:
	{
		//ANIMATE
		if (pattern(m_Animation, ANM_NOMAL_SPD, ANM_NO_LOOP))
		{
			KillAction();
			m_Status = _STATUS::FINISHED;
		}
		//ITEM
		break;
	}
	case _STATUS::FINISHED:
	{
		//
		StockDispBuffer(X, Y, DISP_PRIO_IME3, 60001, 1);
		StockDispBuffer(X, Y-10, DISP_PRIO_IME4, pc.item[m_ItemReceiveIndex].graNo, 2);
		extern int getTextLength( char *);
		int dx = getTextLength(pc.item[m_ItemReceiveIndex].name) / 2;
		StockFontBuffer(X - dx, Y + 50, FONT_PRIO_FRONT, FONT_PAL_WHITE, pc.item[m_ItemReceiveIndex].name, 0);
		//
		Buttons.at(BUTTONS::GET) = StockDispBuffer(X, Y + 85, DISP_PRIO_IME4, 60006, 2);
		break;
	}
	default:
		break;
	}
}

void Mammoth::Init()
{
	m_ItemReceiveIndex = -1;
	m_Status = _STATUS::STARTING;
	m_Fail = _FAIL::NONE;
	Buttons.fill(-2);

	m_CouponCount = GetCouponCount();
	if (m_CouponCount == 0)
		m_Fail = _FAIL::NO_COUPON;
	else if (IsBagFull())
		m_Fail = _FAIL::BAG_FULL;
	
}

void Mammoth::Animation()
{
	
}

void Mammoth::InitAction()
{
	m_Animation = GetAction(PRIO_CHR, 0);
	m_Animation->anim_chr_no = 120284;
	m_Animation->dispPrio = DISP_PRIO_IME3;
	m_Animation->x = X - 1;
	m_Animation->y = Y + 12;
	m_Animation->anim_ang = 0;
}

void Mammoth::KillAction()
{
	if (m_Animation != nullptr)
		DeathAction(m_Animation);
}

void Mammoth::Open(char * data)
{
	if (m_OpenFlag == FALSE)
	{
		RewardList.clear();

		std::string str(data);
		auto vstr = PetData::Instance().Split(str, "|");

		for (std::string v : vstr)
		{
			RewardList.push_back(std::stoi(v));
		}


		if (MenuToggleFlag != 0 || BattleResultWndFlag >= 1) {
			MenuToggleFlag = 0;
			BattleResultWndFlag = FALSE;
			mouse.itemNo = -1;
			itemNo = -1;
			jujutuNo = -1;
			if (MapWmdFlagBak != TRUE) {
				play_se(203, 320, 240);
			}
		}
		DeathMenuAction();
		DeathMenuAction2();
		m_OpenFlag = TRUE;
		Init();
	}
}

void Mammoth::Close()
{
	KillAction();
	m_OpenFlag = FALSE;
}

void Mammoth::MouseControl()
{
	if (mouse.onceState & MOUSE_LEFT_CRICK)
	{
		if (HitDispNo == Buttons.at(BUTTONS::START))
		{
			int CouponIndex = GetCouponIndex();

			if (CouponIndex == -1) goto GO_FAIL;

			if (IsBagFull()) goto GO_FAIL;




			m_Status = _STATUS::ANIMATING;
			InitAction();
			//Send Packet
			lssproto_MAMMOTH_START_send(sockfd, CouponIndex);
		}
		else if (HitDispNo == Buttons.at(BUTTONS::EXIT))
		{
			Close();
		}
		else if (HitDispNo == Buttons.at(BUTTONS::GET))
		{
			Init();
		}
		else if (HitDispNo == Buttons.at(BUTTONS::CLOSE))
		{
			m_Fail = _FAIL::NONE;
		}
	}

GO_FAIL:
	Buttons.fill(-2);
}

int Mammoth::GetCouponIndex()
{
	ITEM* item;
	for (size_t i = MAX_ITEMSTART; i < MAX_ITEM; i++)
	{
		item = &pc.item[i];
		if (item->useFlag == FALSE) continue;
		if (item->graNo != COUPON_IMG) continue;
		if (strcmp(item->name, COUPON_NAME) != 0) continue;
		if (strcmp(item->memo, COUPON_DESC) != 0) continue;

		return i;
	}
	return -1;
}

void Mammoth::DisplayRewardList()
{
	static int LastTIme = 0;
	static int i = 0;


	StockDispBuffer(X-225, Y-160, DISP_PRIO_IME4, RewardList.at(i), 1);

	if (TimeGetTime() > LastTIme)
	{
		++i;
		LastTIme = TimeGetTime() + 500;
	}

	if (i >= RewardList.size())
		i = 0;
}

int Mammoth::GetCouponCount()
{
	int cnt = 0;
	ITEM* item;
	for (size_t i = MAX_ITEMSTART; i < MAX_ITEM; i++)
	{
		item = &pc.item[i];
		if (item->useFlag == FALSE) continue;
		if (item->graNo != COUPON_IMG) continue;
		if (strcmp(item->name, COUPON_NAME) != 0) continue;
		if (strcmp(item->memo, COUPON_DESC) != 0) continue;

		cnt++;
	}
	return cnt;
}

int Mammoth::IsBagFull()
{
	for (size_t i = MAX_ITEMSTART; i < MAX_ITEMSTART + MAX_MAXHAVEITEM + MAX_MAXHAVEITEM * itemWndMaxBag; i++)
	{
		if (pc.item[i].useFlag == FALSE)
			return FALSE;
	}
	return TRUE;
}

void Mammoth::Receive(int prizeindex)
{
	m_ItemReceiveIndex = prizeindex;
}

BOOL Mammoth::IsOpen()
{
	return m_OpenFlag;
}
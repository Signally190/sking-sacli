#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<direct.h>
#include<math.h>

#include "../systeminc/version.h"
#include"../systeminc/system.h"
#include"../systeminc/loadrealbin.h"
#include"../systeminc/loadsprbin.h"
#include"../systeminc/anim_tbl.h"
#include"../systeminc/login.h"
#include"../systeminc/menu.h"
#include"../systeminc/map.h"
#include"../systeminc/lssproto_cli.h"
#include"../systeminc/pc.h"
#include"../systeminc/netmain.h"
#include"../systeminc/handletime.h"
#include"../systeminc/character.h"
#include "../caryIme.h"
#include "../systeminc/ime_sa.h"
#include "../systeminc/t_music.h"
#include "../systeminc/netproc.h"
#include "../winlua.h"
#include "../systeminc/tool.h"
#include "../image.h"
#include "../systeminc/VMProtectSDK.h"

extern int ProcNo2;
#ifdef _PET_ITEM		
extern int nPetItemEquipBmpNumber[PET_EQUIPNUM][2];
#endif

#if 0

#include"netproc.h"
#include"character.h"
#include "battleProc.h"

#endif

#include "../systeminc/field.h"
#include "../PetRanking.h"

#ifdef _NB_管理模式				//[관리모형]
bool GM管理开关 = false;		//[관리스위치]
#endif

#ifdef _NB_顶部详情				//[정상부의 상세한 상황]
int 战骑信息开关;				//{워키 정보 스위치]
int 功能列表开关;				//[기능목록스위치]
int 左菜单开关;					//[좌측메뉴스위치]
int 顶部新按钮索引[10];			//[맨 위 새 버튼 색인]

extern DWORD dwPingTime, dwPingState;

extern int framesToShow;
extern int skipFramesToShow;

#endif



// ???????
short drawFieldButtonFlag = 1;
int fieldBtnHitId[FIELD_FUNC_END];	// ?????????
short drawTimeAnimeFlag = 0;

extern BOOL bNewServer;
extern BOOL g_bUseAlpha;

#ifdef _WATERANIMATION //Syu ADD [눈물바다 애니메이션 층]
#define RAND(x,y) ((x - 1) + 1 + (int)((double)(y - (x - 1)) * rand() / (RAND_MAX + 1.0)))
bool in817flag = false;
#endif

#ifdef _TEACHER_SYSTEM
void TeacherSystemWndfunc(int flag, char* data);
#endif

BOOL BattlingFlag = FALSE;  //[선회 시스템 및 과업 일지 사용]



#ifdef _JOBDAILY
//BOOL	 JobDailyFlag = FALSE;
JOBDAILY jobdaily[MAXMISSION];
BOOL	 JobdailyGetFlag = FALSE;
int	 JobdailyGetMax = FALSE;
//int      MaxPage = 0;
//int		 PageIndex = 0;
#endif

#ifdef _CLOUD_EFFECT				// (不可开) ROG ADD[구름 효과]
void CloudDraw();
#endif



#ifdef _SURFACE_ANIM        //ROG ADD 动态地上物显示[동적 지상물 디스플레이]
int ani_num = 0;
BOOL breadAniFlag = FALSE;
ACTION* SPACT[MAX_ANIM];
int ProduceXY[MAX_ANIM][7] = { -1 }; //0: spr编号,[번호, 1: x좌표, 2: y좌표, 3: 출현시간 4: 우선권 5: 완료여부 6:ACTION]
#else
#ifdef _SPECIALSPACEANIM	// Syu ADD 特殊场景动画配置[특수 장면 애니메이션 배치]
static ACTION* SPACT[MAX_ANIM];
#ifdef _SPECIALSPACEANIM_FIX
#ifdef _MOON_FAIRYLAND       // ROG ADD 月之仙境[달나라]
// 0: 지도상 x 좌표 1: 지도상 y좌표 2: 지도번호 3:grano 4: 이 ProduceXY 사용여부 5: 이 ProduceXY는 SPACT 위치 6: 우선순위 표시
int ProduceXY[TOTAL_ANIM][7] = {
	{  48 , 27 , 1200 , 101290 , 0 , -1 , 30},
	{  36 , 28 , 8255 , 101730 , 0 , -1 , 0 },
	{  33 , 30 , 8255 , 101739 , 0 , -1 , 0 },
	{  25 , 38 , 8255 , 101733 , 0 , -1 , 0 },
	{  34 , 31 , 8255 , 101731 , 0 , -1 , 20}
};
#else
int ProduceXY[TOTAL_ANIM][7] = {
	{  48 , 27 , 1200 , 101290 , 0 , -1 , 30}
};
#endif
#else
int ProduceXY[TOTAL_ANIM][6] = {
	{  48 , 27 , 1200 , 101290 , 0 , -1 }
};
#endif
#endif
#endif

#ifdef _AniCrossFrame	  //[애니메이션 층에서 화상을 헤엄쳐가]
static ACTION* WaterAct2;	   //[헤엄쳐 건너는 화면 애니메이션]
bool delFlag = false;		   //[헤엄쳐 건너본 화면 애니메이션 삭제 여부 플래그]
bool flag22 = true;			   //[헤엄쳐 건너던 만화 같은 건 잊었네]
bool Timeflag2 = false;		   //[헤엄쳐 건너면 애니메이션 크로노그래프 플래그]
int TimeTick2 = 0;			   //[헤엄쳐 건너보는 애니메이션 크로노그래프]
int TimeTickMax2 = 0;		   //[그래픽애니메이션 타이머 상한선 헤엄쳐 건너기]
#endif

#ifdef _AniRandom   //[랜덤 환경 애니메이션]
static ACTION* WaterAct[50];   //[환경 애니메이션]
bool Timeflag[50] = { false };   //[환경 애니메이션 깃발]
int TimeTick[50] = { 0 };		   //[환경 애니메이션 타이머]
int TimeTickMax[50] = { 0 };	   //[환경애니메이션 타이머 상한선]
bool AniFlag = false;		   //[환경 애니메이션 퇴치 플래그]
#endif

#ifdef _AniImmobile	 // Syu ADD [특정 애니메이션을 지정하여 발생시키다]
static ACTION* WaterAct3[30];  //[스폿 애니메이션]
bool Updownflag = false;	   //[스폿 애니메이션 위아래로 떠 있는 플래그] 
int XYposition[560][7] =       //[스폿 애니메이션 어레이 생성] 
							   //[{ X 좌표, Y좌표, 그림번호, 존재여부 플래그, 배치된 Action번호]
							   //[플로팅 위아래 플래그(1회동, 2회동 안 함), 플로팅 타이머]
{
	{ 252 , 255 , 101511 , 0 , -1 , 1 , 0},
	{ 233 , 268 , 101510 , 0 , -1 , 1 , 0},
	{ 227 , 257 , 101516 , 0 , -1 , 1 , 0},
	{ 209 , 254 , 101511 , 0 , -1 , 1 , 0},
	{ 308 , 133 , 101511 , 0 , -1 , 1 , 0},
	{ 305 , 101 , 101518 , 0 , -1 , 1 , 0},
	{ 316 , 121 , 101510 , 0 , -1 , 1 , 0},
	{ 303 , 104 , 101511 , 0 , -1 , 1 , 0},
	{ 287 , 122 , 101510 , 0 , -1 , 1 , 0},
	{ 253 ,  53 , 101510 , 0 , -1 , 1 , 0},
	{ 109 ,  98 , 101516 , 0 , -1 , 1 , 0},
	{ 144 ,  89 , 101511 , 0 , -1 , 1 , 0},
	{ 114 ,  86 , 101510 , 0 , -1 , 1 , 0},
	{ 101 ,  84 , 101517 , 0 , -1 , 1 , 0},
	{  94 ,  93 , 101510 , 0 , -1 , 1 , 0},
	{ 371 , 108 , 101511 , 0 , -1 , 1 , 0},
	{ 183 , 120 , 101510 , 0 , -1 , 1 , 0},
	{ 210 , 187 , 101510 , 0 , -1 , 1 , 0},
	{ 382 , 187 , 101519 , 0 , -1 , 1 , 0},
	{ 225 , 178 , 101510 , 0 , -1 , 1 , 0},
	{ 355 , 206 , 101511 , 0 , -1 , 1 , 0},
	{ 364 , 217 , 101517 , 0 , -1 , 1 , 0},
	{ 239 , 150 , 101510 , 0 , -1 , 1 , 0},
	{ 343 , 217 , 101511 , 0 , -1 , 1 , 0},
	{ 245 , 139 , 101510 , 0 , -1 , 1 , 0},
	{ 334 , 242 , 101516 , 0 , -1 , 1 , 0},
	{ 324 , 218 , 101517 , 0 , -1 , 1 , 0},
	{ 260 , 124 , 101518 , 0 , -1 , 1 , 0},
	{ 301 , 246 , 101510 , 0 , -1 , 1 , 0},
	{ 288 , 265 , 101510 , 0 , -1 , 1 , 0},
	{ 278 , 267 , 101511 , 0 , -1 , 1 , 0},
	{ 226 ,  23 , 101510 , 0 , -1 , 1 , 0},
	{ 344 ,  43 , 101511 , 0 , -1 , 1 , 0},
	{ 368 , 110 , 101516 , 0 , -1 , 1 , 0},
	{ 360 , 145 , 101510 , 0 , -1 , 1 , 0},
	{ 347 , 232 , 101510 , 0 , -1 , 1 , 0},
	{ 327 , 230 , 101519 , 0 , -1 , 1 , 0},
	{ 147 ,  23 , 101518 , 0 , -1 , 1 , 0},
	{ 147 ,  42 , 101517 , 0 , -1 , 1 , 0},
	{ 280 ,  38 , 101510 , 0 , -1 , 1 , 0},
	{ 158 ,  44 , 101510 , 0 , -1 , 1 , 0},
	{ 145 ,  68 , 101510 , 0 , -1 , 1 , 0},
	{ 189 ,  66 , 101510 , 0 , -1 , 1 , 0},
	{ 204 ,  55 , 101511 , 0 , -1 , 1 , 0},
	{ 199 ,  55 , 101510 , 0 , -1 , 1 , 0},
	{ 235 ,  52 , 101511 , 0 , -1 , 1 , 0},
	{ 253 ,  34 , 101510 , 0 , -1 , 1 , 0},
	{ 288 ,  51 , 101510 , 0 , -1 , 1 , 0},
	{ 298 ,  62 , 101511 , 0 , -1 , 1 , 0},
	{ 299 ,  72 , 101510 , 0 , -1 , 1 , 0},
	{ 256 ,  94 , 101511 , 0 , -1 , 1 , 0},
	{ 284 , 106 , 101516 , 0 , -1 , 1 , 0},
	{ 344 ,  42 , 101518 , 0 , -1 , 1 , 0},
	{ 329 ,  72 , 101519 , 0 , -1 , 1 , 0},
	{ 351 ,  82 , 101516 , 0 , -1 , 1 , 0},
	{ 379 ,  82 , 101517 , 0 , -1 , 1 , 0},
	{ 164 , 264 , 101519 , 0 , -1 , 1 , 0},
	{ 155 , 263 , 101510 , 0 , -1 , 1 , 0},
	{ 124 , 267 , 101511 , 0 , -1 , 1 , 0},
	{ 125 , 266 , 101510 , 0 , -1 , 1 , 0},
	{ 112 , 290 , 101511 , 0 , -1 , 1 , 0},
	{ 102 , 287 , 101510 , 0 , -1 , 1 , 0},
	{  95 , 308 , 101511 , 0 , -1 , 1 , 0},
	{  65 , 335 , 101511 , 0 , -1 , 1 , 0},
	{  50 , 335 , 101516 , 0 , -1 , 1 , 0},
	{  48 , 366 , 101510 , 0 , -1 , 1 , 0},
	{  72 , 354 , 101517 , 0 , -1 , 1 , 0},
	{  79 , 376 , 101511 , 0 , -1 , 1 , 0},
	{ 112 , 372 , 101518 , 0 , -1 , 1 , 0},
	{ 129 , 411 , 101510 , 0 , -1 , 1 , 0},
	{ 134 , 421 , 101519 , 0 , -1 , 1 , 0},
	{ 169 , 416 , 101510 , 0 , -1 , 1 , 0},
	{ 201 , 418 , 101511 , 0 , -1 , 1 , 0},
	{ 196 , 376 , 101516 , 0 , -1 , 1 , 0},
	{ 206 , 366 , 101510 , 0 , -1 , 1 , 0},
	{ 202 , 342 , 101517 , 0 , -1 , 1 , 0},
	{ 172 , 334 , 101511 , 0 , -1 , 1 , 0},
	{ 192 , 323 , 101510 , 0 , -1 , 1 , 0},
	{ 155 , 337 , 101510 , 0 , -1 , 1 , 0},
	{ 132 ,  45 , 101511 , 0 , -1 , 1 , 0},
	{  99 , 489 , 101510 , 0 , -1 , 1 , 0},
	{  99 , 466 , 101511 , 0 , -1 , 1 , 0},
	{ 114 , 435 , 101510 , 0 , -1 , 1 , 0},
	{ 119 , 465 , 101516 , 0 , -1 , 1 , 0},
	{ 142 , 435 , 101510 , 0 , -1 , 1 , 0},
	{ 166 , 427 , 101517 , 0 , -1 , 1 , 0},
	{ 159 , 383 , 101518 , 0 , -1 , 1 , 0},
	{  47 , 156 , 101510 , 0 , -1 , 1 , 0},
	{  47 , 132 , 101519 , 0 , -1 , 1 , 0},
	{  36 , 123 , 101510 , 0 , -1 , 1 , 0},
	{  61 , 162 , 101517 , 0 , -1 , 1 , 0},
	{  79 , 145 , 101510 , 0 , -1 , 1 , 0},
	{  93 , 157 , 101510 , 0 , -1 , 1 , 0},
	{ 107 , 154 , 101511 , 0 , -1 , 1 , 0},
	{ 118 , 165 , 101510 , 0 , -1 , 1 , 0},
	{ 133 , 164 , 101519 , 0 , -1 , 1 , 0},
	{ 138 , 176 , 101510 , 0 , -1 , 1 , 0},
	{ 146 , 205 , 101510 , 0 , -1 , 1 , 0},
	{ 162 , 211 , 101516 , 0 , -1 , 1 , 0},
	{ 170 , 202 , 101510 , 0 , -1 , 1 , 0},

	//[대홍해초 우 101595					대홍해초 좌 101597]
	{130,181, 101595 , 0 , -1 , 2 , 0},{275,255, 101597 , 0 , -1 , 2 , 0},
	{124,161, 101595 , 0 , -1 , 2 , 0},{261,265, 101597 , 0 , -1 , 2 , 0},
	{243,90 , 101595 , 0 , -1 , 2 , 0},{226,250, 101597 , 0 , -1 , 2 , 0},
	{269,73 , 101595 , 0 , -1 , 2 , 0},{212,249, 101597 , 0 , -1 , 2 , 0},
	{292,55 , 101595 , 0 , -1 , 2 , 0},{182,250, 101597 , 0 , -1 , 2 , 0},
	{307,108, 101595 , 0 , -1 , 2 , 0},{169,260, 101597 , 0 , -1 , 2 , 0},
	{307,102, 101595 , 0 , -1 , 2 , 0},{147,262, 101597 , 0 , -1 , 2 , 0},
	{183,95 , 101595 , 0 , -1 , 2 , 0},{121,281, 101597 , 0 , -1 , 2 , 0},
	{156,56 , 101595 , 0 , -1 , 2 , 0},{109,289, 101597 , 0 , -1 , 2 , 0},
	{137,183, 101595 , 0 , -1 , 2 , 0},{103,297, 101597 , 0 , -1 , 2 , 0},
	{133,167, 101595 , 0 , -1 , 2 , 0},{46,320 , 101597 , 0 , -1 , 2 , 0},
	{130,163, 101595 , 0 , -1 , 2 , 0},{63,338 , 101597 , 0 , -1 , 2 , 0},
	{112,164, 101595 , 0 , -1 , 2 , 0},{50,360 , 101597 , 0 , -1 , 2 , 0},
	{82,151 , 101595 , 0 , -1 , 2 , 0},{92,365 , 101597 , 0 , -1 , 2 , 0},
	{66,154 , 101595 , 0 , -1 , 2 , 0},{122,380, 101597 , 0 , -1 , 2 , 0},
	{47,132 , 101595 , 0 , -1 , 2 , 0},{151,409, 101597 , 0 , -1 , 2 , 0},
	{43,130 , 101595 , 0 , -1 , 2 , 0},{162,416, 101597 , 0 , -1 , 2 , 0},
	{52,140 , 101595 , 0 , -1 , 2 , 0},{209,369, 101597 , 0 , -1 , 2 , 0},
	{76,155 , 101595 , 0 , -1 , 2 , 0},{192,338, 101597 , 0 , -1 , 2 , 0},
	{120,169, 101595 , 0 , -1 , 2 , 0},{165,315, 101597 , 0 , -1 , 2 , 0},
	{148,220, 101595 , 0 , -1 , 2 , 0},{166,343, 101597 , 0 , -1 , 2 , 0},
	{225,175, 101595 , 0 , -1 , 2 , 0},{199,403, 101597 , 0 , -1 , 2 , 0},
	{244,144, 101595 , 0 , -1 , 2 , 0},{171,408, 101597 , 0 , -1 , 2 , 0},
	{229,103, 101595 , 0 , -1 , 2 , 0},{145,406, 101597 , 0 , -1 , 2 , 0},
	{238,110, 101595 , 0 , -1 , 2 , 0},{164,461, 101597 , 0 , -1 , 2 , 0},
	{99,88  , 101595 , 0 , -1 , 2 , 0},{188,508, 101597 , 0 , -1 , 2 , 0},
	{134,80 , 101595 , 0 , -1 , 2 , 0},{158,457, 101597 , 0 , -1 , 2 , 0},
	{207,46 , 101595 , 0 , -1 , 2 , 0},{148,487, 101597 , 0 , -1 , 2 , 0},
	{315,43 , 101595 , 0 , -1 , 2 , 0},{103,460, 101597 , 0 , -1 , 2 , 0},
	{337,36 , 101595 , 0 , -1 , 2 , 0},{111,446, 101597 , 0 , -1 , 2 , 0},
	{353,75 , 101595 , 0 , -1 , 2 , 0},{126,443, 101597 , 0 , -1 , 2 , 0},
	{359,90 , 101595 , 0 , -1 , 2 , 0},{132,409, 101597 , 0 , -1 , 2 , 0},
	{359,100, 101595 , 0 , -1 , 2 , 0},{110,382, 101597 , 0 , -1 , 2 , 0},
	{355,100, 101595 , 0 , -1 , 2 , 0},{378,186, 101597 , 0 , -1 , 2 , 0},
	{351,152, 101595 , 0 , -1 , 2 , 0},{370,209, 101597 , 0 , -1 , 2 , 0},
	{360,157, 101595 , 0 , -1 , 2 , 0},{349,225, 101597 , 0 , -1 , 2 , 0},
	{370,170, 101595 , 0 , -1 , 2 , 0},{341,234, 101597 , 0 , -1 , 2 , 0},
	{300,254, 101595 , 0 , -1 , 2 , 0},{311,239, 101597 , 0 , -1 , 2 , 0},
	//대록해초오른쪽 101594					대록해초왼쪽 101596]
	{129,185, 101594 , 0 , -1 , 2 , 0},{367,204, 101596 , 0 , -1 , 2 , 0},
	{133,160, 101594 , 0 , -1 , 2 , 0},{340,221, 101596 , 0 , -1 , 2 , 0},
	{248,87 , 101594 , 0 , -1 , 2 , 0},{330,227, 101596 , 0 , -1 , 2 , 0},
	{262,36 , 101594 , 0 , -1 , 2 , 0},{316,250, 101596 , 0 , -1 , 2 , 0},
	{256,83 , 101594 , 0 , -1 , 2 , 0},{291,250, 101596 , 0 , -1 , 2 , 0},
	{313,115, 101594 , 0 , -1 , 2 , 0},{276,261, 101596 , 0 , -1 , 2 , 0},
	{303,110, 101594 , 0 , -1 , 2 , 0},{252,261, 101596 , 0 , -1 , 2 , 0},
	{173,83 , 101594 , 0 , -1 , 2 , 0},{222,249, 101596 , 0 , -1 , 2 , 0},
	{142,87 , 101594 , 0 , -1 , 2 , 0},{209,249, 101596 , 0 , -1 , 2 , 0},
	{252,126, 101594 , 0 , -1 , 2 , 0},{179,248, 101596 , 0 , -1 , 2 , 0},
	{221,179, 101594 , 0 , -1 , 2 , 0},{167,256, 101596 , 0 , -1 , 2 , 0},
	{199,182, 101594 , 0 , -1 , 2 , 0},{142,261, 101596 , 0 , -1 , 2 , 0},
	{96,149 , 101594 , 0 , -1 , 2 , 0},{113,278, 101596 , 0 , -1 , 2 , 0},
	{85,141 , 101594 , 0 , -1 , 2 , 0},{103,288, 101596 , 0 , -1 , 2 , 0},
	{61,157 , 101594 , 0 , -1 , 2 , 0},{84,307 , 101596 , 0 , -1 , 2 , 0},
	{39,135 , 101594 , 0 , -1 , 2 , 0},{46,324 , 101596 , 0 , -1 , 2 , 0},
	{40,131 , 101594 , 0 , -1 , 2 , 0},{63,343 , 101596 , 0 , -1 , 2 , 0},
	{55,148 , 101594 , 0 , -1 , 2 , 0},{53,363 , 101596 , 0 , -1 , 2 , 0},
	{92,147 , 101594 , 0 , -1 , 2 , 0},{100,369, 101596 , 0 , -1 , 2 , 0},
	{129,176, 101594 , 0 , -1 , 2 , 0},{135,390, 101596 , 0 , -1 , 2 , 0},
	{151,208, 101594 , 0 , -1 , 2 , 0},{153,407, 101596 , 0 , -1 , 2 , 0},
	{229,166, 101594 , 0 , -1 , 2 , 0},{169,419, 101596 , 0 , -1 , 2 , 0},
	{254,132, 101594 , 0 , -1 , 2 , 0},{212,370, 101596 , 0 , -1 , 2 , 0},
	{309,109, 101594 , 0 , -1 , 2 , 0},{196,338, 101596 , 0 , -1 , 2 , 0},
	{236,76 , 101594 , 0 , -1 , 2 , 0},{161,321, 101596 , 0 , -1 , 2 , 0},
	{111,89 , 101594 , 0 , -1 , 2 , 0},{184,335, 101596 , 0 , -1 , 2 , 0},
	{137,76 , 101594 , 0 , -1 , 2 , 0},{182,410, 101596 , 0 , -1 , 2 , 0},
	{269,57 , 101594 , 0 , -1 , 2 , 0},{179,425, 101596 , 0 , -1 , 2 , 0},
	{308,40 , 101594 , 0 , -1 , 2 , 0},{151,399, 101596 , 0 , -1 , 2 , 0},
	{345,60 , 101594 , 0 , -1 , 2 , 0},{167,487, 101596 , 0 , -1 , 2 , 0},
	{346,79 , 101594 , 0 , -1 , 2 , 0},{187,516, 101596 , 0 , -1 , 2 , 0},
	{359,98 , 101594 , 0 , -1 , 2 , 0},{106,488, 101596 , 0 , -1 , 2 , 0},
	{373,110, 101594 , 0 , -1 , 2 , 0},{143,487, 101596 , 0 , -1 , 2 , 0},
	{373,106, 101594 , 0 , -1 , 2 , 0},{106,455, 101596 , 0 , -1 , 2 , 0},
	{356,152, 101594 , 0 , -1 , 2 , 0},{104,458, 101596 , 0 , -1 , 2 , 0},
	{365,154, 101594 , 0 , -1 , 2 , 0},{146,428, 101596 , 0 , -1 , 2 , 0},
	{360,169, 101594 , 0 , -1 , 2 , 0},{128,405, 101596 , 0 , -1 , 2 , 0},
	{373,186, 101594 , 0 , -1 , 2 , 0},{92,381 , 101596 , 0 , -1 , 2 , 0},
	//[중청해초우 101598						 중록해초좌 101600]
	{148,199, 101598 , 0 , -1 , 2 , 0},{356,218, 101600 , 0 , -1 , 2 , 0},
	{120,158, 101598 , 0 , -1 , 2 , 0},{337,222, 101600 , 0 , -1 , 2 , 0},
	{235,78 , 101598 , 0 , -1 , 2 , 0},{319,233, 101600 , 0 , -1 , 2 , 0},
	{286,49 , 101598 , 0 , -1 , 2 , 0},{305,242, 101600 , 0 , -1 , 2 , 0},
	{372,121, 101598 , 0 , -1 , 2 , 0},{281,260, 101600 , 0 , -1 , 2 , 0},
	{346,62 , 101598 , 0 , -1 , 2 , 0},{271,262, 101600 , 0 , -1 , 2 , 0},
	{325,80 , 101598 , 0 , -1 , 2 , 0},{243,246, 101600 , 0 , -1 , 2 , 0},
	{156,52 , 101598 , 0 , -1 , 2 , 0},{223,250, 101600 , 0 , -1 , 2 , 0},
	{159,74 , 101598 , 0 , -1 , 2 , 0},{202,247, 101600 , 0 , -1 , 2 , 0},
	{240,153, 101598 , 0 , -1 , 2 , 0},{175,257, 101600 , 0 , -1 , 2 , 0},
	{204,176, 101598 , 0 , -1 , 2 , 0},{163,256, 101600 , 0 , -1 , 2 , 0},
	{189,189, 101598 , 0 , -1 , 2 , 0},{135,263, 101600 , 0 , -1 , 2 , 0},
	{88,142 , 101598 , 0 , -1 , 2 , 0},{113,288, 101600 , 0 , -1 , 2 , 0},
	{76,147 , 101598 , 0 , -1 , 2 , 0},{99,291 , 101600 , 0 , -1 , 2 , 0},
	{58,140 , 101598 , 0 , -1 , 2 , 0},{76,318 , 101600 , 0 , -1 , 2 , 0},
	{39,119 , 101598 , 0 , -1 , 2 , 0},{69,325 , 101600 , 0 , -1 , 2 , 0},
	{27,125 , 101598 , 0 , -1 , 2 , 0},{74,354 , 101600 , 0 , -1 , 2 , 0},
	{60,158 , 101598 , 0 , -1 , 2 , 0},{77,357 , 101600 , 0 , -1 , 2 , 0},
	{100,157, 101598 , 0 , -1 , 2 , 0},{100,375, 101600 , 0 , -1 , 2 , 0},
	{141,192, 101598 , 0 , -1 , 2 , 0},{126,403, 101600 , 0 , -1 , 2 , 0},
	{189,196, 101598 , 0 , -1 , 2 , 0},{160,399, 101600 , 0 , -1 , 2 , 0},
	{244,160, 101598 , 0 , -1 , 2 , 0},{176,433, 101600 , 0 , -1 , 2 , 0},
	{258,116, 101598 , 0 , -1 , 2 , 0},{205,364, 101600 , 0 , -1 , 2 , 0},
	{305,113, 101598 , 0 , -1 , 2 , 0},{191,330, 101600 , 0 , -1 , 2 , 0},
	{169,103, 101598 , 0 , -1 , 2 , 0},{150,329, 101600 , 0 , -1 , 2 , 0},
	{131,84 , 101598 , 0 , -1 , 2 , 0},{186,344, 101600 , 0 , -1 , 2 , 0},
	{196,97 , 101598 , 0 , -1 , 2 , 0},{183,413, 101600 , 0 , -1 , 2 , 0},
	{285,49 , 101598 , 0 , -1 , 2 , 0},{175,434, 101600 , 0 , -1 , 2 , 0},
	{330,24 , 101598 , 0 , -1 , 2 , 0},{162,462, 101600 , 0 , -1 , 2 , 0},
	{355,70 , 101598 , 0 , -1 , 2 , 0},{165,500, 101600 , 0 , -1 , 2 , 0},
	{328,60 , 101598 , 0 , -1 , 2 , 0},{180,510, 101600 , 0 , -1 , 2 , 0},
	{368,104, 101598 , 0 , -1 , 2 , 0},{137,489, 101600 , 0 , -1 , 2 , 0},
	{371,118, 101598 , 0 , -1 , 2 , 0},{130,491, 101600 , 0 , -1 , 2 , 0},
	{359,138, 101598 , 0 , -1 , 2 , 0},{114,443, 101600 , 0 , -1 , 2 , 0},
	{359,170, 101598 , 0 , -1 , 2 , 0},{99,470 , 101600 , 0 , -1 , 2 , 0},
	{349,131, 101598 , 0 , -1 , 2 , 0},{132,414, 101600 , 0 , -1 , 2 , 0},
	{372,177, 101598 , 0 , -1 , 2 , 0},{137,392, 101600 , 0 , -1 , 2 , 0},
	{381,203, 101598 , 0 , -1 , 2 , 0},{79,374 , 101600 , 0 , -1 , 2 , 0},
	//[청해초 우 101599						중청해초 좌 101601]
	{141,202, 101599 , 0 , -1 , 2 , 0},{358,210, 101601 , 0 , -1 , 2 , 0},
	{125,162, 101599 , 0 , -1 , 2 , 0},{344,230, 101601 , 0 , -1 , 2 , 0},
	{243,94 , 101599 , 0 , -1 , 2 , 0},{326,229, 101601 , 0 , -1 , 2 , 0},
	{284,46 , 101599 , 0 , -1 , 2 , 0},{318,248, 101601 , 0 , -1 , 2 , 0},
	{317,126, 101599 , 0 , -1 , 2 , 0},{288,252, 101601 , 0 , -1 , 2 , 0},
	{354,75 , 101599 , 0 , -1 , 2 , 0},{274,255, 101601 , 0 , -1 , 2 , 0},
	{315,116, 101599 , 0 , -1 , 2 , 0},{243,263, 101601 , 0 , -1 , 2 , 0},
	{155,73 , 101599 , 0 , -1 , 2 , 0},{224,253, 101601 , 0 , -1 , 2 , 0},
	{161,78 , 101599 , 0 , -1 , 2 , 0},{208,252, 101601 , 0 , -1 , 2 , 0},
	{250,139, 101599 , 0 , -1 , 2 , 0},{178,254, 101601 , 0 , -1 , 2 , 0},
	{206,197, 101599 , 0 , -1 , 2 , 0},{160,260, 101601 , 0 , -1 , 2 , 0},
	{195,192, 101599 , 0 , -1 , 2 , 0},{135,264, 101601 , 0 , -1 , 2 , 0},
	{94,155 , 101599 , 0 , -1 , 2 , 0},{112,281, 101601 , 0 , -1 , 2 , 0},
	{77,153 , 101599 , 0 , -1 , 2 , 0},{103,296, 101601 , 0 , -1 , 2 , 0},
	{55,146 , 101599 , 0 , -1 , 2 , 0},{79,310 , 101601 , 0 , -1 , 2 , 0},
	{45,122 , 101599 , 0 , -1 , 2 , 0},{66,328 , 101601 , 0 , -1 , 2 , 0},
	{39,120 , 101599 , 0 , -1 , 2 , 0},{68,351 , 101601 , 0 , -1 , 2 , 0},
	{48,157 , 101599 , 0 , -1 , 2 , 0},{74,353 , 101601 , 0 , -1 , 2 , 0},
	{99,151 , 101599 , 0 , -1 , 2 , 0},{107,374, 101601 , 0 , -1 , 2 , 0},
	{136,182, 101599 , 0 , -1 , 2 , 0},{138,393, 101601 , 0 , -1 , 2 , 0},
	{173,203, 101599 , 0 , -1 , 2 , 0},{163,399, 101601 , 0 , -1 , 2 , 0},
	{223,160, 101599 , 0 , -1 , 2 , 0},{171,411, 101601 , 0 , -1 , 2 , 0},
	{254,121, 101599 , 0 , -1 , 2 , 0},{214,383, 101601 , 0 , -1 , 2 , 0},
	{315,118, 101599 , 0 , -1 , 2 , 0},{192,330, 101601 , 0 , -1 , 2 , 0},
	{234,69 , 101599 , 0 , -1 , 2 , 0},{155,322, 101601 , 0 , -1 , 2 , 0},
	{123,88 , 101599 , 0 , -1 , 2 , 0},{183,336, 101601 , 0 , -1 , 2 , 0},
	{164,80 , 101599 , 0 , -1 , 2 , 0},{182,421, 101601 , 0 , -1 , 2 , 0},
	{284,45 , 101599 , 0 , -1 , 2 , 0},{175,430, 101601 , 0 , -1 , 2 , 0},
	{333,30 , 101599 , 0 , -1 , 2 , 0},{150,415, 101601 , 0 , -1 , 2 , 0},
	{352,62 , 101599 , 0 , -1 , 2 , 0},{169,498, 101601 , 0 , -1 , 2 , 0},
	{335,83 , 101599 , 0 , -1 , 2 , 0},{188,517, 101601 , 0 , -1 , 2 , 0},
	{363,100, 101599 , 0 , -1 , 2 , 0},{118,492, 101601 , 0 , -1 , 2 , 0},
	{370,103, 101599 , 0 , -1 , 2 , 0},{136,490, 101601 , 0 , -1 , 2 , 0},
	{338,91 , 101599 , 0 , -1 , 2 , 0},{113,446, 101601 , 0 , -1 , 2 , 0},
	{359,153, 101599 , 0 , -1 , 2 , 0},{110,464, 101601 , 0 , -1 , 2 , 0},
	{363,162, 101599 , 0 , -1 , 2 , 0},{134,424, 101601 , 0 , -1 , 2 , 0},
	{373,171, 101599 , 0 , -1 , 2 , 0},{127,403, 101601 , 0 , -1 , 2 , 0},
	{385,192, 101599 , 0 , -1 , 2 , 0},{85,377 , 101601 , 0 , -1 , 2 , 0},
	//[소록해초 우 101603					소록해초 좌 101605]
	{136,173, 101603 , 0 , -1 , 2 , 0},{349,213, 101605 , 0 , -1 , 2 , 0},
	{118,158, 101603 , 0 , -1 , 2 , 0},{339,225, 101605 , 0 , -1 , 2 , 0},
	{227,75 , 101603 , 0 , -1 , 2 , 0},{322,244, 101605 , 0 , -1 , 2 , 0},
	{286,57 , 101603 , 0 , -1 , 2 , 0},{299,247, 101605 , 0 , -1 , 2 , 0},
	{353,90 , 101603 , 0 , -1 , 2 , 0},{281,258, 101605 , 0 , -1 , 2 , 0},
	{322,70 , 101603 , 0 , -1 , 2 , 0},{272,256, 101605 , 0 , -1 , 2 , 0},
	{352,40 , 101603 , 0 , -1 , 2 , 0},{230,249, 101605 , 0 , -1 , 2 , 0},
	{155,52 , 101603 , 0 , -1 , 2 , 0},{216,248, 101605 , 0 , -1 , 2 , 0},
	{152,71 , 101603 , 0 , -1 , 2 , 0},{190,243, 101605 , 0 , -1 , 2 , 0},
	{234,168, 101603 , 0 , -1 , 2 , 0},{172,255, 101605 , 0 , -1 , 2 , 0},
	{199,190, 101603 , 0 , -1 , 2 , 0},{150,258, 101605 , 0 , -1 , 2 , 0},
	{175,203, 101603 , 0 , -1 , 2 , 0},{117,271, 101605 , 0 , -1 , 2 , 0},
	{85,141 , 101603 , 0 , -1 , 2 , 0},{109,284, 101605 , 0 , -1 , 2 , 0},
	{72,151 , 101603 , 0 , -1 , 2 , 0},{100,301, 101605 , 0 , -1 , 2 , 0},
	{44,139 , 101603 , 0 , -1 , 2 , 0},{75,315 , 101605 , 0 , -1 , 2 , 0},
	{45,122 , 101603 , 0 , -1 , 2 , 0},{74,311 , 101605 , 0 , -1 , 2 , 0},
	{29,116 , 101603 , 0 , -1 , 2 , 0},{61,367 , 101605 , 0 , -1 , 2 , 0},
	{72,151 , 101603 , 0 , -1 , 2 , 0},{48,346 , 101605 , 0 , -1 , 2 , 0},
	{115,157, 101603 , 0 , -1 , 2 , 0},{111,376, 101605 , 0 , -1 , 2 , 0},
	{154,202, 101603 , 0 , -1 , 2 , 0},{142,397, 101605 , 0 , -1 , 2 , 0},
	{212,183, 101603 , 0 , -1 , 2 , 0},{162,398, 101605 , 0 , -1 , 2 , 0},
	{244,157, 101603 , 0 , -1 , 2 , 0},{188,385, 101605 , 0 , -1 , 2 , 0},
	{256,105, 101603 , 0 , -1 , 2 , 0},{199,363, 101605 , 0 , -1 , 2 , 0},
	{281,113, 101603 , 0 , -1 , 2 , 0},{188,322, 101605 , 0 , -1 , 2 , 0},
	{154,73 , 101603 , 0 , -1 , 2 , 0},{161,330, 101605 , 0 , -1 , 2 , 0},
	{148,73 , 101603 , 0 , -1 , 2 , 0},{185,389, 101605 , 0 , -1 , 2 , 0},
	{252,41 , 101603 , 0 , -1 , 2 , 0},{173,430, 101605 , 0 , -1 , 2 , 0},
	{296,59 , 101603 , 0 , -1 , 2 , 0},{134,436, 101605 , 0 , -1 , 2 , 0},
	{312,67 , 101603 , 0 , -1 , 2 , 0},{189,470, 101605 , 0 , -1 , 2 , 0},
	{352,62 , 101603 , 0 , -1 , 2 , 0},{184,506, 101605 , 0 , -1 , 2 , 0},
	{333,86 , 101603 , 0 , -1 , 2 , 0},{164,499, 101605 , 0 , -1 , 2 , 0},
	{371,113, 101603 , 0 , -1 , 2 , 0},{156,486, 101605 , 0 , -1 , 2 , 0},
	{397,131, 101603 , 0 , -1 , 2 , 0},{110,491, 101605 , 0 , -1 , 2 , 0},
	{353,132, 101603 , 0 , -1 , 2 , 0},{117,438, 101605 , 0 , -1 , 2 , 0},
	{363,164, 101603 , 0 , -1 , 2 , 0},{99,480 , 101605 , 0 , -1 , 2 , 0},
	{365,165, 101603 , 0 , -1 , 2 , 0},{142,398, 101605 , 0 , -1 , 2 , 0},
	{373,184, 101603 , 0 , -1 , 2 , 0},{105,385, 101605 , 0 , -1 , 2 , 0},
	{372,185, 101603 , 0 , -1 , 2 , 0},{71,371 , 101605 , 0 , -1 , 2 , 0},
	//[소홍해초 우 101602				 소홍해초 우 101604]

	{154,200, 101602 , 0 , -1 , 2 , 0},{349,211, 101604 , 0 , -1 , 2 , 0},
	{119,157, 101602 , 0 , -1 , 2 , 0},{345,231, 101604 , 0 , -1 , 2 , 0},
	{239,82 , 101602 , 0 , -1 , 2 , 0},{326,243, 101604 , 0 , -1 , 2 , 0},
	{290,59 , 101602 , 0 , -1 , 2 , 0},{312,248, 101604 , 0 , -1 , 2 , 0},
	{358,99 , 101602 , 0 , -1 , 2 , 0},{281,253, 101604 , 0 , -1 , 2 , 0},
	{328,69 , 101602 , 0 , -1 , 2 , 0},{265,257, 101604 , 0 , -1 , 2 , 0},
	{258,37 , 101602 , 0 , -1 , 2 , 0},{235,247, 101604 , 0 , -1 , 2 , 0},
	{153,51 , 101602 , 0 , -1 , 2 , 0},{219,248, 101604 , 0 , -1 , 2 , 0},
	{153,72 , 101602 , 0 , -1 , 2 , 0},{197,244, 101604 , 0 , -1 , 2 , 0},
	{132,187, 101602 , 0 , -1 , 2 , 0},{174,252, 101604 , 0 , -1 , 2 , 0},
	{131,179, 101602 , 0 , -1 , 2 , 0},{161,257, 101604 , 0 , -1 , 2 , 0},
	{120,169, 101602 , 0 , -1 , 2 , 0},{131,271, 101604 , 0 , -1 , 2 , 0},
	{101,151, 101602 , 0 , -1 , 2 , 0},{108,285, 101604 , 0 , -1 , 2 , 0},
	{71,158 , 101602 , 0 , -1 , 2 , 0},{97,294 , 101604 , 0 , -1 , 2 , 0},
	{58,139 , 101602 , 0 , -1 , 2 , 0},{75,311 , 101604 , 0 , -1 , 2 , 0},
	{29,128 , 101602 , 0 , -1 , 2 , 0},{71,311 , 101604 , 0 , -1 , 2 , 0},
	{20,119 , 101602 , 0 , -1 , 2 , 0},{77,356 , 101604 , 0 , -1 , 2 , 0},
	{67,156 , 101602 , 0 , -1 , 2 , 0},{47,349 , 101604 , 0 , -1 , 2 , 0},
	{108,157, 101602 , 0 , -1 , 2 , 0},{87,372 , 101604 , 0 , -1 , 2 , 0},
	{149,200, 101602 , 0 , -1 , 2 , 0},{128,406, 101604 , 0 , -1 , 2 , 0},
	{187,190, 101602 , 0 , -1 , 2 , 0},{141,396, 101604 , 0 , -1 , 2 , 0},
	{238,157, 101602 , 0 , -1 , 2 , 0},{176,434, 101604 , 0 , -1 , 2 , 0},
	{270,120, 101602 , 0 , -1 , 2 , 0},{200,358, 101604 , 0 , -1 , 2 , 0},
	{271,120, 101602 , 0 , -1 , 2 , 0},{187,326, 101604 , 0 , -1 , 2 , 0},
	{155,57 , 101602 , 0 , -1 , 2 , 0},{151,334, 101604 , 0 , -1 , 2 , 0},
	{140,88 , 101602 , 0 , -1 , 2 , 0},{182,404, 101604 , 0 , -1 , 2 , 0},
	{240,70 , 101602 , 0 , -1 , 2 , 0},{165,419, 101604 , 0 , -1 , 2 , 0},
	{287,51 , 101602 , 0 , -1 , 2 , 0},{134,438, 101604 , 0 , -1 , 2 , 0},
	{302,57 , 101602 , 0 , -1 , 2 , 0},{182,471, 101604 , 0 , -1 , 2 , 0},
	{355,64 , 101602 , 0 , -1 , 2 , 0},{174,503, 101604 , 0 , -1 , 2 , 0},
	{342,93 , 101602 , 0 , -1 , 2 , 0},{186,505, 101604 , 0 , -1 , 2 , 0},
	{369,107, 101602 , 0 , -1 , 2 , 0},{148,487, 101604 , 0 , -1 , 2 , 0},
	{372,123, 101602 , 0 , -1 , 2 , 0},{120,491, 101604 , 0 , -1 , 2 , 0},
	{355,138, 101602 , 0 , -1 , 2 , 0},{117,438, 101604 , 0 , -1 , 2 , 0},
	{366,161, 101602 , 0 , -1 , 2 , 0},{98,475 , 101604 , 0 , -1 , 2 , 0},
	{363,163, 101602 , 0 , -1 , 2 , 0},{132,411, 101604 , 0 , -1 , 2 , 0},
	{369,181, 101602 , 0 , -1 , 2 , 0},{110,391, 101604 , 0 , -1 , 2 , 0},
	{374,189, 101602 , 0 , -1 , 2 , 0},{81,373 , 101604 , 0 , -1 , 2 , 0},
};
#endif

#ifdef _STREET_VENDOR
int iOldGX, iOldGY;
extern ITEM_BUFFER ItemBuffer[MAX_ITEM];
ACTION* pActStreetVendorWnd = NULL;
ACTION* pActStreetVendorBuyWnd = NULL;
short sStreetVendorBtn = 0;
short sStreetVendorBuyBtn = 0;
short sStreetVendorBtnFocus = 0;
int sStreetVendorBtnGraNo[] =
{
	IMAGE_Shop_Off,
	IMAGE_Shop_On
};

//[수입가 윈도우]
extern void CheckNumber(char* buf, int num);
static ACTION* pActSellPriceWnd = NULL;
int SellPriceWndfunc()
{
	static STR_BUFFER str_bufferMoney;		// [판매가격 입력란, 최대 천만원이다.]
	int x, y, iConfirm;

	if (pActSellPriceWnd == NULL)
	{
		pActSellPriceWnd = MakeWindowDisp((DEF_APPSIZEX >> 1) - (106 >> 1) + 10, 350,
			106, 80, CG_FIELD_SV_SELL_PRICE_PANEL, -1);
		//[판매가격문자초기화]
		ZeroMemory(str_bufferMoney.buffer, sizeof(str_bufferMoney.buffer));
		str_bufferMoney.cnt = 0;
		str_bufferMoney.cursor = 0;
		str_bufferMoney.color = 0;
		str_bufferMoney.len = 8;
		str_bufferMoney.lineLen = 8;
		str_bufferMoney.lineDist = 0;
		str_bufferMoney.x = pActSellPriceWnd->x + 10;
		str_bufferMoney.y = pActSellPriceWnd->y + 28;
		str_bufferMoney.fontPrio = FONT_PRIO_FRONT;
		GetKeyInputFocus(&str_bufferMoney);
	}
	else
	{
		if (pActSellPriceWnd->hp > 0)
		{
			x = pActSellPriceWnd->x;
			y = pActSellPriceWnd->y;
			StockDispBuffer(((WINDOW_DISP*)pActSellPriceWnd->pYobi)->mx,
				((WINDOW_DISP*)pActSellPriceWnd->pYobi)->my,
				DISP_PRIO_MENU, CG_FIELD_SV_SELL_PRICE_PANEL, 1);
			iConfirm = StockDispBuffer(x + 55, y + 67, DISP_PRIO_IME3, 26289, 2);

			//[확인을 누르다]
			if ((mouse.onceState & MOUSE_LEFT_CRICK) && HitDispNo == iConfirm)
			{
				DeathAction(pActSellPriceWnd);
				pActSellPriceWnd = NULL;
				GetKeyInputFocus(&MyChatBuffer);
				return atoi(str_bufferMoney.buffer);
			}
			//[마우스는 판매가격 구내에 있다]
			if (MakeHitBox(str_bufferMoney.x, str_bufferMoney.y, str_bufferMoney.x + 88, str_bufferMoney.y + 22, DISP_PRIO_BOX2) == TRUE)
				GetKeyInputFocus(&str_bufferMoney);
			CheckNumber(str_bufferMoney.buffer, 10000000);
			// [표시가격]
			StockFontBuffer2(&str_bufferMoney);
		}
	}

	return -1;
}

// [노점 기능 윈도우]

extern void CheckSpace(char* buf);
ACTION* pShowPet = NULL;
Show_Sell_Item ShowSellItem[MAX_SELL_ITEM];
STR_BUFFER str_bufferVendorName;		//[가게 이름]

Show_Sell_Item* GetEmptyShowSellItem(Show_Sell_Item* ShowSellItem)
{
	for (int i = 0; i < MAX_SELL_ITEM; i++)
	{
		if (ShowSellItem[i].usage == false)
			return ShowSellItem + i;
	}
	return NULL;
}

void StreetVendorWndfunc(bool bReset, char* data)
{
	int i, j, x, y, itemx, itemy, selId = 0;
	char temp[8], szMsg[128];
	static int iBtn[7], iSellItemBtn[MAX_SELL_ITEM], iItemOnSell[15], iShowPet, iPetOnSell[MAX_PET], iGetSellPrice, iListStart;
	static ITEM* pItem = NULL;
	static bool bChangePet = true, bRunSellPrice = false;
	static Show_Sell_Item* pShowSellItem = NULL;

	if (pActStreetVendorWnd == NULL)
	{
		//JL800600
		int tempX, tempY;
		if (DISPLACEMENT_X == 0)
		{
			tempX = (DEF_APPSIZEX >> 1) - (622 >> 1);
			tempY = 20;
		}
		else
		{
			tempX = 41;
			tempY = 41;
		}

		pActStreetVendorWnd = MakeWindowDisp(tempX, tempY, 622, 413, CG_FIELD_SV_SELL_PANEL, -1);


		memset(iBtn, -1, sizeof(iBtn));
		memset(iItemOnSell, -1, sizeof(iItemOnSell));
		memset(iPetOnSell, -1, sizeof(iPetOnSell));
		for (i = 0; i < MAX_SELL_ITEM; i++)
		{
			ShowSellItem[i].name[0] = '\0';
			ShowSellItem[i].freeName[0] = '\0';
			ShowSellItem[i].needGetPrice = false;
			ShowSellItem[i].pile = 0;
			ShowSellItem[i].price = 0;
			ShowSellItem[i].usage = false;
			ShowSellItem[i].kind = -1;
			ShowSellItem[i].index = -1;
		}
		if (bReset)
		{
			//[점호 문자 초기화]
			ZeroMemory(str_bufferVendorName.buffer, sizeof(str_bufferVendorName.buffer));
			str_bufferVendorName.cnt = 0;
			str_bufferVendorName.cursor = 0;
			str_bufferVendorName.color = 0;
			str_bufferVendorName.len = 20;
			str_bufferVendorName.lineLen = 20;
			str_bufferVendorName.lineDist = 0;
			str_bufferVendorName.x = pActStreetVendorWnd->x + 25;
			str_bufferVendorName.y = pActStreetVendorWnd->y + 346;
			str_bufferVendorName.fontPrio = FONT_PRIO_FRONT;
		}
		pItem = pc.item;
		iShowPet = 0;
		iListStart = 0;
		bChangePet = true;
		bRunSellPrice = false;
		if (pShowPet != NULL)
		{
			DeathAction(pShowPet);
			pShowPet = NULL;
		}
		pShowSellItem = NULL;
		if (data != NULL)
		{
			int count = 0, kind, index, price;

			getStringToken(data, '|', 2, sizeof(szMsg) - 1, szMsg);
			count = atoi(szMsg);
			for (i = 0; i < count; i++)
			{
				getStringToken(data, '|', 3 + i * 3, sizeof(szMsg) - 1, szMsg);
				kind = atoi(szMsg);
				getStringToken(data, '|', 4 + i * 3, sizeof(szMsg) - 1, szMsg);
				index = atoi(szMsg);
				getStringToken(data, '|', 5 + i * 3, sizeof(szMsg) - 1, szMsg);
				price = atoi(szMsg);
				pShowSellItem = GetEmptyShowSellItem(ShowSellItem);
				if (pShowSellItem != NULL)
				{
					// 소품
					if (kind == 0 && pItem[index].useFlag)
					{
						pShowSellItem->kind = kind;
						pShowSellItem->index = index;
						pShowSellItem->price = price;
						sprintf(pShowSellItem->name, "%s", pItem[index].name);
						pShowSellItem->pile = pItem[index].pile;
						pShowSellItem->usage = true;
						iItemOnSell[index - 9] = index;
					}
					// 페트
					else if (kind == 1 && pet[index].useFlag)
					{
						pShowSellItem->kind = kind;
						pShowSellItem->index = index;
						pShowSellItem->price = price;
						sprintf(pShowSellItem->name, "%s", pet[index].name);
						sprintf(pShowSellItem->freeName, "%s", pet[index].freeName);
						pShowSellItem->pile = 1;
						pShowSellItem->usage = true;
						iPetOnSell[index] = index;
					}
				}
			}
		}
	}
	else
	{
		if (pActStreetVendorWnd->hp > 0)
		{
			if (bRunSellPrice)
			{
				iGetSellPrice = SellPriceWndfunc();
				if (iGetSellPrice != -1)
				{
					for (i = 0; i < MAX_SELL_ITEM; i++)
					{
						if (ShowSellItem[i].needGetPrice)
						{
							ShowSellItem[i].price = iGetSellPrice;
							ShowSellItem[i].needGetPrice = false;
						}
					}
					bRunSellPrice = false;
				}
			}
			x = pActStreetVendorWnd->x;
			y = pActStreetVendorWnd->y;
			selId = focusFontId(iSellItemBtn, sizeof(iSellItemBtn) / sizeof(int));

			// [button이 나타나는 위치 표시 및 설정]
			// [확정하다]
			iBtn[0] = StockDispBuffer(x + 80, y + 388, DISP_PRIO_IME3, 26289, 2);
			// [취소]
			iBtn[1] = StockDispBuffer(x + 230, y + 388, DISP_PRIO_IME3, 26291, 2);
			// [상권 화살표]
			iBtn[2] = StockDispBuffer(x + 305, y + 44, DISP_PRIO_IME3, 26331, 2);
			// [하권 화살표]
			iBtn[3] = StockDispBuffer(x + 305, y + 304, DISP_PRIO_IME3, 26332, 2);
			// [방치 버튼]
			iBtn[4] = StockDispBuffer(x + 335, y + 300, DISP_PRIO_IME3, 26062, 2);
			// [왼쪽 화살표]
			iBtn[5] = StockDispBuffer(x + 520, y + 60, DISP_PRIO_IME3, 26047, 2);
			// [오른쪽 화살표]
			iBtn[6] = StockDispBuffer(x + 570, y + 60, DISP_PRIO_IME3, 26048, 2);

			// [왼쪽에 있는 물건 목록 표시]
			for (i = iListStart, j = 0; i < iListStart + 10; i++, j++)
			{
				if (ShowSellItem[i].usage)
				{
					if (ShowSellItem[i].kind == 0)
					{
						if (ShowSellItem[i].price == 0)
							sprintf(szMsg, "%-20sx%3d%13s", ShowSellItem[i].name, ShowSellItem[i].pile, "X");
						else
							sprintf(szMsg, "%-20sx%3d%13d", ShowSellItem[i].name, ShowSellItem[i].pile, ShowSellItem[i].price);
					}
					else
					{
						if (ShowSellItem[i].price == 0)
							sprintf(szMsg, "%-17s%-12s%8s", ShowSellItem[i].name, ShowSellItem[i].freeName, "议价");
						else
							sprintf(szMsg, "%-17s%-12s%8d", ShowSellItem[i].name, ShowSellItem[i].freeName, ShowSellItem[i].price);
					}
					iSellItemBtn[j] = StockFontBuffer(x + 20, y + 32 + 28 * j, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 2);
				}
				else
					iSellItemBtn[j] = StockFontBuffer(x + 20, y + 32 + 28 * j, FONT_PRIO_FRONT, FONT_PAL_WHITE, "", 0);
			}

			for (i = 0; i < 7; i++)
			{
				if (mouse.onceState & MOUSE_LEFT_CRICK)
				{
					if (HitDispNo == iBtn[i])
					{
						play_se(217, 320, 240);
						switch (i)
						{
							// [확정하다]
						case 0:
							if (!bRunSellPrice)
							{
								int count = 0;
								char szMsg[512], szTemp[512], szTemp1[16];

								memset(szMsg, 0, sizeof(szMsg));
								memset(szTemp, 0, sizeof(szTemp));
								//[팔려고 하는 물건의 색인을 server에게 전하다]
								for (j = 0; j < MAX_SELL_ITEM; j++)
								{
									if (ShowSellItem[j].usage)
									{
										sprintf(szTemp1, "%d|%d|%d|", ShowSellItem[j].kind, ShowSellItem[j].index, ShowSellItem[j].price);
										strcat(szTemp, szTemp1);
										count++;	// [총수통계]
									}
								}
								if (count != 0)
								{
									if (str_bufferVendorName.cnt == 0)
									{
										sprintf(str_bufferVendorName.buffer, "어서오십시오!!");
										str_bufferVendorName.cnt = 10;
									}
									sprintf(szMsg, "S|%d|%s%s", count, szTemp, str_bufferVendorName.buffer);
									lssproto_STREET_VENDOR_send(sockfd, szMsg);
									// [노점을 벌여 놓고 말에서 내려야 한다]
									if (pc.ridePetNo > -1)
									{
										char buf[64];

										sprintf(buf, "R|P|-1");
										if (bNewServer)
											lssproto_FM_send(sockfd, buf);
										else
											lssproto_FM_send(sockfd, buf);
										if ((bNewServer & 0xf000000) == 0xf000000)
											lssproto_PETST_send(sockfd, pc.ridePetNo, 0);
									}
								}
								else
								{
									lssproto_STREET_VENDOR_send(sockfd, "E|");
									pc.iOnStreetVendor = 0;
								}
							}
							else
								break;
							// [취소]
						case 1:
							if (i == 1)
							{
								lssproto_STREET_VENDOR_send(sockfd, "E|");
								pc.iOnStreetVendor = 0;
							}
							sStreetVendorBtn = 0;
							actBtn = 0;
							if (pShowPet != NULL)
							{
								DeathAction(pShowPet);
								pShowPet = NULL;
							}
							if (pActSellPriceWnd != NULL)
								DeathAction(pActSellPriceWnd);
							pActSellPriceWnd = NULL;
							GetKeyInputFocus(&MyChatBuffer);
							break;
							// [상권]
						case 2:
							iListStart--;
							if (iListStart < 0)
								iListStart = 0;
							break;
							// [하권]
						case 3:
							iListStart++;
							if (iListStart > 10)
								iListStart = 10;
							break;
							// [방치]
						case 4:
							if (!bRunSellPrice)
							{
								pShowSellItem = GetEmptyShowSellItem(ShowSellItem);
								// [그리고 팔 수 있는 공간도 있어]
								if (pShowSellItem != NULL && iPetOnSell[iShowPet] == -1 && pShowPet != NULL)
								{
									iPetOnSell[iShowPet] = iShowPet;
									bRunSellPrice = true;
									pShowSellItem->usage = true;
									pShowSellItem->needGetPrice = true;
									sprintf(pShowSellItem->name, "%s", pet[iShowPet].name);
									sprintf(pShowSellItem->freeName, "%s", pet[iShowPet].freeName);
									pShowSellItem->pile = 1;
									pShowSellItem->kind = 1;
									pShowSellItem->index = iShowPet;
								}
							}
							break;
							// [왼쪽 화살표]
						case 5:
							if (!bRunSellPrice)
							{
								iShowPet = (iShowPet + 1 > 4 ? 0 : iShowPet + 1);
								bChangePet = true;
							}
							break;
							// [오른쪽 화살표]
						case 6:
							if (!bRunSellPrice)
							{
								iShowPet = (iShowPet - 1 < 0 ? 4 : iShowPet - 1);
								bChangePet = true;
							}
							break;
						}
						break;
					}
				}
			}
			if (!bRunSellPrice)
			{
				for (i = 0; i < 10; i++)
				{
					// [가격수정]
					if (mouse.onceState & MOUSE_LEFT_CRICK)
					{
						if (selId == iSellItemBtn[i])
						{
							ShowSellItem[i + iListStart].needGetPrice = true;
							bRunSellPrice = true;
							break;
						}
					}
					// [취소불매]
					else if (mouse.onceState & MOUSE_RIGHT_CRICK)
					{
						if (selId == iSellItemBtn[i])
						{
							if (ShowSellItem[i + iListStart].kind == 0)
								iItemOnSell[ShowSellItem[i + iListStart].index - 9] = -1;
							else
								iPetOnSell[ShowSellItem[i + iListStart].index] = -1;
							ShowSellItem[i + iListStart].name[0] = '\0';
							ShowSellItem[i + iListStart].freeName[0] = '\0';
							ShowSellItem[i + iListStart].needGetPrice = false;
							ShowSellItem[i + iListStart].pile = 0;
							ShowSellItem[i + iListStart].price = 0;
							ShowSellItem[i + iListStart].usage = false;
							ShowSellItem[i + iListStart].kind = -1;
							ShowSellItem[i + iListStart].index = -1;
							break;
						}
					}
				}
			}
			for (j = 0; j < 3; j++)
			{
				for (i = 0; i < 5; i++)
				{
					itemx = x + 323 + i * 51;
					itemy = y + 247 + j * 48;
					// [플레이어의 몸통을 보여주는 소품]
					if (pItem[j * 5 + i + 9].useFlag)
					{
						if (!bRunSellPrice)
							StockDispBuffer(itemx + 25, itemy + 25, DISP_PRIO_ITEM, pItem[j * 5 + i + 9].graNo, 0);
						// [가격 입력 윈도우가 열리면 disp_prio menu로 설정해야 가격 윈도우에 else를 덮어쓰지 않아.]
						else
							StockDispBuffer(itemx + 25, itemy + 25, DISP_PRIO_MENU, pItem[j * 5 + i + 9].graNo, 0);
					}
					if (iItemOnSell[j * 5 + i] != -1)
					{
						// [가격 입력 윈도우가 열리면 셀이 표시되지 않아]
						if (!bRunSellPrice)
							StockFontBuffer(itemx + 25, itemy + 25, FONT_PRIO_FRONT, FONT_PAL_RED, "SELL", 0);
					}
					if (!bRunSellPrice)
					{
						if (MakeHitBox(itemx, itemy, itemx + 50, itemy + 47, DISP_PRIO_IME3) == TRUE)
						{
							// [이 도구가 있어]
							if (pItem[j * 5 + i + 9].useFlag &&
								// [도구가 선택되었는지 여부를 판단하다]
								(mouse.onceState & MOUSE_LEFT_DBL_CRICK) &&
								//  [아직 팔지]
								(iItemOnSell[j * 5 + i] == -1)
								)
							{
								pShowSellItem = GetEmptyShowSellItem(ShowSellItem);
								// [그리고 팔 수 있는 공간도 있어]
								if (pShowSellItem != NULL)
								{
									iItemOnSell[j * 5 + i] = j * 5 + i + 9; //[기록상으로는 그 도구가 팔릴 것 같다]
									play_se(217, 320, 240);
									bRunSellPrice = true;
									pShowSellItem->usage = true;
									pShowSellItem->needGetPrice = true;
									sprintf(pShowSellItem->name, "%s", pItem[j * 5 + i + 9].name);
									pShowSellItem->pile = pItem[j * 5 + i + 9].pile;
									pShowSellItem->kind = 0;
									pShowSellItem->index = j * 5 + i + 9;
								}
							}
						}
					}
				}
			}
			// 페트보기
			for (i = 0; i < MAX_PET; i++) {
				if (bChangePet && !bRunSellPrice) {
					// [이렇게 총애를 받는 것은 기승 및 설정에 있어서 총애를 받는 것이 아니어]
					if (pet[iShowPet].useFlag == 1 && (pc.ridePetNo != iShowPet) && (pc.mailPetNo != iShowPet)) {
						if (pShowPet != NULL) {
							DeathAction(pShowPet);
							pShowPet = NULL;
						}
						pShowPet = MakeAnimDisp(x + 396, y + 170, pet[iShowPet].graNo, 0);
						bChangePet = false;
						break;
					}
					else {
						if (HitDispNo == iBtn[5])
							iShowPet = (iShowPet + 1 > 4 ? 0 : iShowPet + 1);
						else if (HitDispNo == iBtn[6])
							iShowPet = (iShowPet - 1 < 0 ? 4 : iShowPet - 1);
						else
							iShowPet = (iShowPet + 1 > 4 ? 0 : iShowPet + 1);
					}
				}
				if (pShowPet != NULL) {
					// 페트정보보기
					if (iPetOnSell[iShowPet] != -1)
						StockFontBuffer(x + 440, y + 195, FONT_PRIO_FRONT, FONT_PAL_RED, "SELL", 0);
					// 페트이름표시
					if (strlen(pet[iShowPet].freeName) > 0)
						StockFontBuffer(x + 336, y + 28, FONT_PRIO_FRONT, 0, pet[iShowPet].freeName, 0);
					else
						StockFontBuffer(x + 336, y + 28, FONT_PRIO_FRONT, 0, pet[iShowPet].name, 0);
					// 페트속성보기
					itoa(pet[iShowPet].level, temp, 10);
					StockFontBuffer(x + 542, y + 92, FONT_PRIO_FRONT, FONT_PAL_WHITE, temp, 0);
					itoa(pet[iShowPet].atk, temp, 10);
					StockFontBuffer(x + 542, y + 92 + 25, FONT_PRIO_FRONT, FONT_PAL_WHITE, temp, 0);
					itoa(pet[iShowPet].def, temp, 10);
					StockFontBuffer(x + 542, y + 92 + 50, FONT_PRIO_FRONT, FONT_PAL_WHITE, temp, 0);
					itoa(pet[iShowPet].quick, temp, 10);
					StockFontBuffer(x + 542, y + 92 + 73, FONT_PRIO_FRONT, FONT_PAL_WHITE, temp, 0);
					itoa(pet[iShowPet].maxHp, temp, 10);
					StockFontBuffer(x + 542, y + 92 + 97, FONT_PRIO_FRONT, FONT_PAL_WHITE, temp, 0);
				}
			}
			// [몸에 총애가 없다.]
			if (i == MAX_PET && bChangePet) {
				bChangePet = false;
				if (pShowPet != NULL) {
					DeathAction(pShowPet);
					pShowPet = NULL;
				}
			}
			// [마우스는 상점명구 내에 있다]
			if (MakeHitBox(str_bufferVendorName.x, str_bufferVendorName.y, str_bufferVendorName.x + 250,
				str_bufferVendorName.y + 24, DISP_PRIO_BOX2) == TRUE)
				GetKeyInputFocus(&str_bufferVendorName);
			CheckSpace(str_bufferVendorName.buffer);
			// 상점이름표시
			StockFontBuffer2(&str_bufferVendorName);
			StockDispBuffer(((WINDOW_DISP*)pActStreetVendorWnd->pYobi)->mx,
				((WINDOW_DISP*)pActStreetVendorWnd->pYobi)->my,
				DISP_PRIO_MENU, CG_FIELD_SV_SELL_PANEL, 1);
		}
	}
}

// [노점을 벌여 놓고 인터페이스를 구매하다]
Show_Vendor_Item ShowVendorItem[MAX_SELL_ITEM];

void StreetVendorBuyWndfunc(char* data)
{
	int i, j, x, y, selId = 0;
	const int	iMaxBtnNum = 5;
	static int	iBtn[iMaxBtnNum], iBuyItemBtn[MAX_SELL_ITEM], iListStart, iTotalBuy[2]; //iTotalBuy[0]:买入道具数量 iTotalBuy[1]:宠物买入数量 [페트 구매수량]
	static int	iEmptyNum[2], iCurrentShow, iTotalBuyMoney; // iEmptyNum[0]:身上道具栏空位数量 iEmptyNum[1]:宠物栏空位数量 [펫 칸 공석 수]
	static int	iMakePetAni = -1;
#ifdef _PET_ITEM
	static BOOL	bViewPetItem = FALSE;
#endif
	char szMsg[1024];

	if (pActStreetVendorBuyWnd == NULL)
	{
		pActStreetVendorBuyWnd = MakeWindowDisp((DEF_APPSIZEX >> 1) - (617 >> 1), 0, 617, 405, CG_FIELD_SV_BUY_PANEL, -1);
		memset(ShowVendorItem, 0, sizeof(ShowVendorItem));
		memset(iBuyItemBtn, 0, sizeof(iBuyItemBtn));
		iTotalBuy[0] = iTotalBuy[1] = iListStart = iEmptyNum[0] = iEmptyNum[1] = iCurrentShow = iTotalBuyMoney = 0;
		iMakePetAni = -1;
		for (i = MAX_ITEMSTART; i < MAX_ITEM; i++)
		{
			if (pc.item[i].useFlag == FALSE)
				iEmptyNum[0]++;
		}
		for (i = 0; i < MAX_PET; i++)
		{
			if (pet[i].useFlag == 0)
				iEmptyNum[1]++;
		}
		if (pShowPet != NULL)
		{
			DeathAction(pShowPet);
			pShowPet = NULL;
		}
#ifdef _PET_ITEM
		InitItem(pActStreetVendorBuyWnd->x + 310, pActStreetVendorBuyWnd->y + 20, TRUE);
#endif
	}
	else
	{
		if (pActStreetVendorBuyWnd->hp > 0)
		{
			x = pActStreetVendorBuyWnd->x;
			y = pActStreetVendorBuyWnd->y;
			selId = focusFontId(iBuyItemBtn, sizeof(iBuyItemBtn) / sizeof(int));
			// [button이 나타나는 위치 표시 및 설정]
			// [확정하다]
			iBtn[0] = StockDispBuffer(x + 410, y + 378, DISP_PRIO_IME3, 26289, 2);
			// [취소]
			iBtn[1] = StockDispBuffer(x + 535, y + 378, DISP_PRIO_IME3, 26291, 2);
			// [상권 화살표]
			iBtn[2] = StockDispBuffer(x + 300, y + 34, DISP_PRIO_IME3, 26331, 2);
			// [하권 화살표]
			iBtn[3] = StockDispBuffer(x + 300, y + 374, DISP_PRIO_IME3, 26332, 2);
#ifdef _PET_ITEM
			if (bViewPetItem)
				iBtn[4] = StockDispBuffer(x + 560, y + 318, DISP_PRIO_IME3, 26472, 2);
			else
				iBtn[4] = StockDispBuffer(x + 560, y + 318, DISP_PRIO_IME3, 26471, 2);
#endif
			for (i = 0; i < iMaxBtnNum; i++)
			{
				if (mouse.onceState & MOUSE_LEFT_CRICK)
				{
					if (HitDispNo == iBtn[i])
					{
						play_se(217, 320, 240);
						switch (i)
						{
							// [확정하다]
						case 0:
						{
							int count = 0;
							char szMsg[128], szTemp[128], szTemp1[8];

							memset(szMsg, 0, sizeof(szMsg));
							memset(szTemp, 0, sizeof(szTemp));
							// [사려는 물건의 색인을 server에게 전하다.]
							for (j = 0; j < MAX_SELL_ITEM; j++)
							{
								if (ShowVendorItem[j].bBuy)
								{
									sprintf(szTemp1, "%d|", ShowVendorItem[j].index);
									strcat(szTemp, szTemp1);
									count++;	//[총수통계]
								}
							}
							if (count != 0)
							{
								sprintf(szMsg, "B|%d|%s", count, szTemp);
								lssproto_STREET_VENDOR_send(sockfd, szMsg);
							}
							else
								lssproto_STREET_VENDOR_send(sockfd, "N|");
						}
						// [취소]
						case 1:
							if (i == 1)
								lssproto_STREET_VENDOR_send(sockfd, "N|");
							sStreetVendorBuyBtn = 0;
							actBtn = 0;
							if (pShowPet != NULL)
							{
								DeathAction(pShowPet);
								pShowPet = NULL;
							}
							if (pActSellPriceWnd != NULL)
								DeathAction(pActSellPriceWnd);
							pActSellPriceWnd = NULL;
							break;
							// [상권]
						case 2:
							iListStart--;
							if (iListStart < 0)
								iListStart = 0;
							break;
							// [하권]
						case 3:
							iListStart++;
							if (iListStart > 13)
								iListStart = 13;
							break;
#ifdef _PET_ITEM
						case 4:
							bViewPetItem = !bViewPetItem;
							break;
#endif
						}
						break;
					}
				}
			}

			// [왼쪽에 있는 물건 목록 표시]
			for (i = iListStart, j = 0; i < iListStart + 13; i++, j++)
			{
				if (ShowVendorItem[i].usage)
				{
					if (ShowVendorItem[i].kind == 0)
					{
						if (ShowVendorItem[i].price == 0)
							sprintf(szMsg, "%-20sx%3d%13s", ShowVendorItem[i].name, ShowVendorItem[i].pile, "협상 가격");
						else
							sprintf(szMsg, "%-20sx%3d%13d", ShowVendorItem[i].name, ShowVendorItem[i].pile, ShowVendorItem[i].price);
					}
					else
					{
						if (ShowVendorItem[i].price == 0)
							sprintf(szMsg, "%-17s%-12s%8s", ShowVendorItem[i].name, ShowVendorItem[i].freeName, "협상 가격");
						else
							sprintf(szMsg, "%-17s%-12s%8d", ShowVendorItem[i].name, ShowVendorItem[i].freeName, ShowVendorItem[i].price);
					}
					iBuyItemBtn[j] = StockFontBuffer(x + 20, y + 22 + 28 * j, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 2);
					if (ShowVendorItem[i].bBuy)
						StockBoxDispBuffer(x + 18, y + 19 + 28 * j, x + 280, y + 39 + 28 * j, DISP_PRIO_BOX2, SYSTEM_PAL_RED, 0);
				}
				else
					iBuyItemBtn[j] = StockFontBuffer(x + 20, y + 22 + 28 * j, FONT_PRIO_FRONT, FONT_PAL_WHITE, "", 0);
			}

			// [프로젝트 상세 자료 표시]
			if (ShowVendorItem[iCurrentShow].bGetData) {
				// [소품 상세 자료 표시]
				if (ShowVendorItem[iCurrentShow].kind == 0) {
					int		iy = y + 198, color = ShowVendorItem[iCurrentShow].color;
					char* splitPoint = ShowVendorItem[iCurrentShow].memo;
					// [애완동물 제거도]
					if (pShowPet != NULL) {
						DeathAction(pShowPet);
						pShowPet = NULL;
					}
					// [디스플레이 도구]
					StockDispBuffer(x + 415, y + 100, DISP_PRIO_ITEM, ShowVendorItem[iCurrentShow].bmpNo, 0);
					// [도구의 밑바닥을 나타내는 석판]
					StockDispBuffer(x + 415, y + 100, DISP_PRIO_IME3, 26269, 0);
					// [디스플레이 도구 이름]
					StockFontBuffer(x + 325, y + 150, FONT_PRIO_FRONT, color, ShowVendorItem[iCurrentShow].name, 0);


					// [도구 내구도 표시]
					sprintf(szMsg, "내구력(%s)", ShowVendorItem[iCurrentShow].damage);
					StockFontBuffer(x + 325, y + 174, FONT_PRIO_FRONT, color, szMsg, 0);

					// [디스플레이 도구 서술]
					while (1) {
						if (strlen(splitPoint) > 28) {
							_snprintf(szMsg, 28, "%s", splitPoint);
							szMsg[28] = NULL;
							if (GetStrLastByte(szMsg) == 3) {
								szMsg[27] = NULL;
								splitPoint += 27;
							}
							else {
								szMsg[28] = NULL;
								splitPoint += 28;
							}
							StockFontBuffer(x + 325, iy, FONT_PRIO_FRONT, 0, szMsg, 0);
							iy += 24;
						}
						else {
							strcpy(szMsg, splitPoint);
							StockFontBuffer(x + 325, iy, FONT_PRIO_FRONT, 0, szMsg, 0);
							break;
						}
					}
				}
				else {// [페트 상세 자료 표시]
					if (pShowPet != NULL) {
#ifdef _PET_ITEM
						// [페트의 몸통 도구를 표시]
						if (bViewPetItem) {
							int		nY = 251, nColor;
							char* splitPoint;

							// [페트 장비 표시줄의 밑그림 및 장비]
							for (i = 0; i < PET_EQUIPNUM; i++) {
								StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, DISP_PRIO_IME3, nPetItemEquipBmpNumber[i][0], 0);
								if (ShowVendorItem[iCurrentShow].oPetItemInfo[i].bmpNo > 0)
									StockDispBuffer(ItemBuffer[i].defX, ItemBuffer[i].defY, DISP_PRIO_ITEM, ShowVendorItem[iCurrentShow].oPetItemInfo[i].bmpNo, 0);
							}
							for (i = 0; i < PET_EQUIPNUM; i++) {
								// 디스플레이 장비 내용]
								if (MakeHitBox(ItemBuffer[i].defX - 26, ItemBuffer[i].defY - 26, ItemBuffer[i].defX + 26, ItemBuffer[i].defY + 23, DISP_PRIO_IME3) == TRUE) {
									if (ShowVendorItem[iCurrentShow].oPetItemInfo[i].bmpNo > 0) {
										nColor = ShowVendorItem[iCurrentShow].oPetItemInfo[i].color;
										splitPoint = ShowVendorItem[iCurrentShow].oPetItemInfo[i].memo;
										// [장비명칭]
										StockFontBuffer(x + 325, y + 195, FONT_PRIO_FRONT, nColor, ShowVendorItem[iCurrentShow].oPetItemInfo[i].name, 0);
										// [내구도]
										sprintf(szMsg, "내구력(%s)", ShowVendorItem[iCurrentShow].oPetItemInfo[i].damage);
										StockFontBuffer(x + 325, y + 215, FONT_PRIO_FRONT, nColor, szMsg, 0);
										// [디스플레이 도구 서술]
										while (1)
										{
											if (strlen(splitPoint) > 28)
											{
												_snprintf(szMsg, 28, "%s", splitPoint);
												szMsg[28] = NULL;
												if (GetStrLastByte(szMsg) == 3)
												{
													szMsg[27] = NULL;
													splitPoint += 27;
												}
												else
												{
													szMsg[28] = NULL;
													splitPoint += 28;
												}
												StockFontBuffer(x + 325, nY, FONT_PRIO_FRONT, 0, szMsg, 0);
												nY += 24;
											}
											else
											{
												strcpy(szMsg, splitPoint);
												StockFontBuffer(x + 325, nY, FONT_PRIO_FRONT, 0, szMsg, 0);
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
							// [페트 이름 표시]
							StockFontBuffer(x + 325, y + 35, FONT_PRIO_FRONT, FONT_PAL_WHITE, ShowVendorItem[iCurrentShow].name, 0);
							// [페트 재생 수를 표시하다]
							sprintf(szMsg, "%d 회전", ShowVendorItem[iCurrentShow].trans);
							StockFontBuffer(x + 570, y + 35, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
							// [페트 개폐를 나타내는 이름]
							StockFontBuffer(x + 325, y + 55, FONT_PRIO_FRONT, FONT_PAL_RED, ShowVendorItem[iCurrentShow].freeName, 0);
							// [페트 등급, 내구력, 공격, 방, 민 네 가지 속성을 표시한다.]
							sprintf(szMsg, "Lv：%d", ShowVendorItem[iCurrentShow].level);
							StockFontBuffer(x + 325, y + 80, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
							sprintf(szMsg, "내：%d", ShowVendorItem[iCurrentShow].maxhp);
							StockFontBuffer(x + 325, y + 100, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
							sprintf(szMsg, "공：%d", ShowVendorItem[iCurrentShow].attack);
							StockFontBuffer(x + 325, y + 120, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
							sprintf(szMsg, "방：%d", ShowVendorItem[iCurrentShow].defence);
							StockFontBuffer(x + 325, y + 140, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
							sprintf(szMsg, "순：%d", ShowVendorItem[iCurrentShow].dex);
							StockFontBuffer(x + 325, y + 160, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
							sprintf(szMsg, "지：%d", ShowVendorItem[iCurrentShow].earth / 10);
							StockFontBuffer(x + 390, y + 80, FONT_PRIO_FRONT, FONT_PAL_GREEN, szMsg, 0);
							sprintf(szMsg, "수：%d", ShowVendorItem[iCurrentShow].water / 10);
							StockFontBuffer(x + 390, y + 100, FONT_PRIO_FRONT, FONT_PAL_BLUE, szMsg, 0);
							sprintf(szMsg, "화：%d", ShowVendorItem[iCurrentShow].fire / 10);
							StockFontBuffer(x + 390, y + 120, FONT_PRIO_FRONT, FONT_PAL_RED, szMsg, 0);
							sprintf(szMsg, "풍：%d", ShowVendorItem[iCurrentShow].wind / 10);
							StockFontBuffer(x + 390, y + 140, FONT_PRIO_FRONT, FONT_PAL_YELLOW, szMsg, 0);
							sprintf(szMsg, "충：%d", ShowVendorItem[iCurrentShow].fidelity);
							StockFontBuffer(x + 390, y + 160, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
							// [총애를 과시하다]
							for (i = 0; i < ShowVendorItem[iCurrentShow].maxSkill; i++)
							{
								sprintf(szMsg, "기술 %d：%s", i + 1, ShowVendorItem[iCurrentShow].skillname[i]);
								StockFontBuffer(x + 325, y + 190 + i * 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
							}
						}
					}
				}
			}

			// [좌측 항목을 처리하다]
			int kind;

			for (i = 0; i < 13; i++)
			{
				// [매수를 선정하다]
				if (mouse.onceState & MOUSE_LEFT_CRICK)
				{
					if (selId == iBuyItemBtn[i])
					{
						kind = ShowVendorItem[i + iListStart].kind;

						// [이미 선택했다면 취소한다]
						if (ShowVendorItem[i + iListStart].bBuy)
						{
							ShowVendorItem[i + iListStart].bBuy = false;
							iTotalBuy[kind]--;
							iTotalBuyMoney -= ShowVendorItem[i + iListStart].price;
						}
						// [가격결정 품목인지 여부]
						else if (ShowVendorItem[i + iListStart].price > 0)
						{
							// [몸에 아직 빈 자리가 있다]
							if (iEmptyNum[kind] > iTotalBuy[kind])
							{
								// [몸에 돈이 충분하다]
								if ((iTotalBuyMoney + ShowVendorItem[i + iListStart].price) <= pc.gold)
								{
									// [캐릭터가 페트보다 등급이 높은지 확인한다] 페트등급
									//if (kind == 1 && (ShowVendorItem[i + iListStart].level > (pc.level + 5) && pc.transmigration < 1))
									//{
									//	StockChatBufferLine("당신은 이 페트를 사용할수없어！", FONT_PAL_RED);
									//	break;
									//}
									ShowVendorItem[i + iListStart].bBuy = true;
									iTotalBuy[kind]++;
									iTotalBuyMoney += ShowVendorItem[i + iListStart].price;
								}
								else
									StockChatBufferLine("스톤이 부족해！", FONT_PAL_RED);
							}
							else
							{
								if (kind == 0)
									StockChatBufferLine("바디 아이템 섹션이 부족해！", FONT_PAL_RED);
								else if (kind == 1)
									StockChatBufferLine("바디 아이템 섹션이 부족해！", FONT_PAL_RED);
							}
						}
						else
							StockChatBufferLine("협정 가격 물품은 일일이 고를 수 없다！", FONT_PAL_RED);
						break;
					}
				}
				// 看项目内容
				else if (mouse.onceState & MOUSE_RIGHT_CRICK)
				{
					if (selId == iBuyItemBtn[i])
					{
						if (!ShowVendorItem[i + iListStart].bGetData)
						{
							sprintf(szMsg, "D|%d|", ShowVendorItem[i + iListStart].index);
							lssproto_STREET_VENDOR_send(sockfd, szMsg);
						}
						else if (ShowVendorItem[i + iListStart].kind == 1)
						{
							if (iCurrentShow != i + iListStart)
							{
								if (pShowPet != NULL)
								{
									DeathAction(pShowPet);
									pShowPet = NULL;
								}
								iMakePetAni = i + iListStart;
							}
						}
						iCurrentShow = i + iListStart;
#ifdef _PET_ITEM
						bViewPetItem = FALSE;
#endif
						break;
					}
				}
			}

			// [몸에 지니고 있는 돈을 과시하다]
			sprintf(szMsg, "%d", pc.gold);
			StockFontBuffer(x + 380, y + 336, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
			// 선택한 품목의 총가격을 표시합니다.
			sprintf(szMsg, "%d", iTotalBuyMoney);
			StockFontBuffer(x + 525, y + 336, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
			if (iMakePetAni > -1)
			{
				pShowPet = MakeAnimDisp(x + 520, y + 230, ShowVendorItem[iMakePetAni].bmpNo, 0);
				iMakePetAni = -1;
			}
			StockDispBuffer(((WINDOW_DISP*)pActStreetVendorBuyWnd->pYobi)->mx,
				((WINDOW_DISP*)pActStreetVendorBuyWnd->pYobi)->my,
				DISP_PRIO_MENU, CG_FIELD_SV_BUY_PANEL, 1);
		}
	}
	if (data != NULL)
	{
		int count, index;
#ifdef _PET_ITEM
		int nPetItemPlace;
#endif
		getStringToken(data, '|', 1, sizeof(szMsg) - 1, szMsg);
		// [판매물 기본 자료]
		if (szMsg[0] == 'B')
		{
			getStringToken(data, '|', 2, sizeof(szMsg) - 1, szMsg);
			count = atoi(szMsg);
			for (i = 0; i < count; i++)
			{
				getStringToken(data, '|', 3 + i * 5, sizeof(szMsg) - 1, szMsg);
				ShowVendorItem[i].kind = atoi(szMsg);
				getStringToken(data, '|', 4 + i * 5, sizeof(szMsg) - 1, szMsg);
				ShowVendorItem[i].price = atoi(szMsg);
				getStringToken(data, '|', 5 + i * 5, sizeof(szMsg) - 1, szMsg);
				strncpy(ShowVendorItem[i].name, szMsg, sizeof(ShowVendorItem[i].name));
				getStringToken(data, '|', 6 + i * 5, sizeof(szMsg) - 1, szMsg);
				if (ShowVendorItem[i].kind == 0)
					ShowVendorItem[i].pile = atoi(szMsg);
				else
					strncpy(ShowVendorItem[i].freeName, szMsg, sizeof(ShowVendorItem[i].freeName));
				getStringToken(data, '|', 7 + i * 5, sizeof(szMsg) - 1, szMsg);
				ShowVendorItem[i].index = atoi(szMsg);
				ShowVendorItem[i].usage = true;
			}
		}
		// [판매물 상세 자료]
		else if (szMsg[0] == 'D')
		{
			getStringToken(data, '|', 2, sizeof(szMsg) - 1, szMsg);
			index = atoi(szMsg);
			for (i = 0; i < MAX_SELL_ITEM; i++)
			{
				if (ShowVendorItem[i].usage && ShowVendorItem[i].index == index)
				{
					index = i;
					break;
				}
			}
			if (index >= 0 && index < MAX_SELL_ITEM)
			{
				if (ShowVendorItem[index].kind == 0)
				{
					getStringToken(data, '|', 3, sizeof(szMsg) - 1, szMsg);
					strncpy(ShowVendorItem[index].memo, szMsg, sizeof(ShowVendorItem[index].memo));
					getStringToken(data, '|', 4, sizeof(szMsg) - 1, szMsg);
					strncpy(ShowVendorItem[index].damage, szMsg, sizeof(ShowVendorItem[index].damage));
					getStringToken(data, '|', 5, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].color = atoi(szMsg);
					getStringToken(data, '|', 6, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].bmpNo = atoi(szMsg);
				}
				else
				{
					int size = sizeof(ShowVendorItem[index].skillname[0]);

					getStringToken(data, '|', 3, sizeof(szMsg) - 1, szMsg);
					count = atoi(szMsg);
					for (i = 0; i < count; i++)
					{
						getStringToken(data, '|', 4 + i, sizeof(szMsg) - 1, szMsg);
						strncpy(ShowVendorItem[index].skillname[i], szMsg, size);
					}
					ShowVendorItem[index].maxSkill = count;
					getStringToken(data, '|', 4 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].level = atoi(szMsg);
					getStringToken(data, '|', 5 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].maxhp = atoi(szMsg);
					getStringToken(data, '|', 6 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].attack = atoi(szMsg);
					getStringToken(data, '|', 7 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].defence = atoi(szMsg);
					getStringToken(data, '|', 8 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].dex = atoi(szMsg);
					getStringToken(data, '|', 9 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].earth = atoi(szMsg);
					getStringToken(data, '|', 10 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].water = atoi(szMsg);
					getStringToken(data, '|', 11 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].fire = atoi(szMsg);
					getStringToken(data, '|', 12 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].wind = atoi(szMsg);
					getStringToken(data, '|', 13 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].fidelity = atoi(szMsg);
					getStringToken(data, '|', 14 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].trans = atoi(szMsg);
					getStringToken(data, '|', 15 + i, sizeof(szMsg) - 1, szMsg);
					ShowVendorItem[index].bmpNo = atoi(szMsg);
					if (pShowPet != NULL)
					{
						DeathAction(pShowPet);
						pShowPet = NULL;
					}
					iMakePetAni = index;
#ifdef _PET_ITEM
					for (int j = 0; j < MAX_PET_ITEM; j++)
					{
						// [장비위치]
						if (getStringToken(data, '|', 16 + i + j * 6, sizeof(szMsg) - 1, szMsg))
							continue;
						nPetItemPlace = atoi(szMsg);
						// 이름
						getStringToken(data, '|', 17 + i + j * 6,
							sizeof(ShowVendorItem[index].oPetItemInfo[nPetItemPlace].name) - 1,
							ShowVendorItem[index].oPetItemInfo[nPetItemPlace].name);
						// 기술
						getStringToken(data, '|', 18 + i + j * 6,
							sizeof(ShowVendorItem[index].oPetItemInfo[nPetItemPlace].memo) - 1,
							ShowVendorItem[index].oPetItemInfo[nPetItemPlace].memo);
						// 내구력
						getStringToken(data, '|', 19 + i + j * 6,
							sizeof(ShowVendorItem[index].oPetItemInfo[nPetItemPlace].damage) - 1,
							ShowVendorItem[index].oPetItemInfo[nPetItemPlace].damage);
						// 텍스트색상
						getStringToken(data, '|', 20 + i + j * 6, sizeof(szMsg) - 1, szMsg);
						ShowVendorItem[index].oPetItemInfo[nPetItemPlace].color = atoi(szMsg);
						// 도면번호
						getStringToken(data, '|', 21 + i + j * 6, sizeof(szMsg) - 1, szMsg);
						ShowVendorItem[index].oPetItemInfo[nPetItemPlace].bmpNo = atoi(szMsg);
					}
#endif
				}
				ShowVendorItem[index].bGetData = true;
			}
		}
	}
}

#else
#ifdef _TABLEOFSKILL				// (不可开) Syu ADD 7.0 [새로운 인물 기능 리스트]
int SkillWnd = 0;
ACTION* pActSkillWnd;
short skillBtn = 0;
short skillBtnFocus = 0;
int skillBtnGraNo[] =
{
	CG_FIELD_SKILL_BTN_OFF,
	CG_FIELD_SKILL_BTN_ON
};
// 职业技能视窗
void SkillWndfunc()
{
	int x, y, j;
	char msg[256];
	char buf[256];
	char msg2[256];
	if (pActSkillWnd == NULL)
	{
		pActSkillWnd = MakeWindowDisp(354, 0, 300, 456, 0, -1);
#ifdef _NEW_RESOMODE  //800 600模式
		x = pActSkillWnd->x;
		y = pActSkillWnd->y;
#endif
	}
	else
	{
		if (pActSkillWnd->hp > 0)
		{
			StockDispBuffer(((WINDOW_DISP*)pActSkillWnd->pYobi)->mx - 10, ((WINDOW_DISP*)pActSkillWnd->pYobi)->my - 5, DISP_PRIO_MENU, CG_FIELD_SKILL_PANEL, 1);
			x = pActSkillWnd->x + 2;
			y = pActSkillWnd->y - 5;
			fieldBtnHitId[14] = StockDispBuffer(x + 201, y + 442, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
			StockFontBuffer(x + 58, y + 40, FONT_PRIO_FRONT, 1, pc.profession_class_name, 2);
			sprintf(msg, "%d", pc.profession_skill_point);
			StockFontBuffer(x + 228, y + 40, FONT_PRIO_FRONT, 1, msg, 0);
			memset(msg, -1, sizeof(msg));
		}
	}
	for (int i = 0; i < 3; i++)
	{
#ifdef _READ16BITBMP
		if (g_bUseAlpha)
		{
			if (MakeHitBox(x + 258, y + 15 + i * 78, x + 288, y + 93 + i * 78, DISP_PRIO_IME3) == TRUE)
			{
				if (mouse.onceState & MOUSE_LEFT_CRICK)
				{
					SkillWnd = i;
					play_se(217, 320, 240);
					break;
				}
			}
		}
		else
#endif
			if (MakeHitBox(x + 250, y + 18 + i * 79, x + 280, y + 96 + i * 79, DISP_PRIO_IME3) == TRUE)
			{
				if (mouse.onceState & MOUSE_LEFT_CRICK)
				{
					SkillWnd = i;
					play_se(217, 320, 240);
					break;
				}
			}
	}
	if (mouse.onceState & MOUSE_LEFT_CRICK)
	{
		if (HitDispNo == fieldBtnHitId[14])
		{
			actBtn = 0;
			skillBtn = 0;
			play_se(217, 320, 240);
		}
	}
	extern int BattleSkill[20];
	extern int AssitSkill[20];
	extern int AdvanceSkill[20];
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			StockDispBuffer(x + 143 + j * 60, y + 210 + i * 57, DISP_PRIO_IME3, CG_FIELD_SKILL_TILE, 0);
	}
	switch (SkillWnd)
	{
	case 0:
#ifdef _READ16BITBMP
		if (g_bUseAlpha)
			StockDispBuffer(x + 147, y + 225, DISP_PRIO_IME3, CG_FIELD_SKILL_ASSIT, 0);
		else
#endif
			StockDispBuffer(x + 138, y + 228, DISP_PRIO_IME3, CG_FIELD_SKILL_ASSIT, 0);
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (AssitSkill[j + i * 4] == -1)
					break;
				if (HitDispNo == StockDispBuffer(x + 29 + j * 60, y + 102 + i * 57, DISP_PRIO_IME3 + 1, profession_skill[AssitSkill[j + i * 4]].icon, 2))
				{
					int use_color = 0;

					if (pc.mp >= profession_skill[AssitSkill[j + i * 4]].costmp && profession_skill[AssitSkill[j + i * 4]].costmp != 0)
						use_color = FONT_PAL_WHITE;
					else
						use_color = FONT_PAL_GRAY;
					sprintf(msg, "%s", profession_skill[AssitSkill[j + i * 4]].name);
					StockFontBuffer(355, 310, FONT_PRIO_FRONT, use_color, msg, 0);
					sprintf(msg, "%d％", profession_skill[AssitSkill[j + i * 4]].skill_level);
					StockFontBuffer(440, 310, FONT_PRIO_FRONT, use_color, msg, 0);
					sprintf(msg, "(耗费MP:%d)", profession_skill[AssitSkill[j + i * 4]].costmp);
					StockFontBuffer(500, 310, FONT_PRIO_FRONT, use_color, msg, 0);
					sprintf(msg, "%s", profession_skill[AssitSkill[j + i * 4]].memo);
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
					if (mouse.onceState & MOUSE_LEFT_CRICK && profession_skill[AssitSkill[j + i * 4]].useFlag == 0)
					{
						if (pc.mp >= profession_skill[AssitSkill[j + i * 4]].costmp && profession_skill[AssitSkill[j + i * 4]].costmp != 0)
						{
							lssproto_BATTLESKILL_send(sockfd, AssitSkill[j + i * 4]);
							play_se(217, 320, 240);
						}
					}
#endif
				}
				sprintf(msg2, "%8s", profession_skill[AssitSkill[j + i * 4]].name);
				StockFontBuffer(x + j * 60, y + 110 + i * 57, FONT_PRIO_FRONT, profession_skill[AssitSkill[j + i * 4]].useFlag + 1, msg2, 0);
			}
		}
		break;
	case 1:
#ifdef _READ16BITBMP
		if (g_bUseAlpha)
			StockDispBuffer(x + 147, y + 225, DISP_PRIO_IME3, CG_FIELD_SKILL_BATTLE, 0);
		else
#endif
			StockDispBuffer(x + 138, y + 228, DISP_PRIO_IME3, CG_FIELD_SKILL_BATTLE, 0);
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (BattleSkill[j + i * 4] == -1)
					break;
				if (HitDispNo == StockDispBuffer(x + 29 + j * 60, y + 102 + i * 57, DISP_PRIO_IME3 + 1, profession_skill[BattleSkill[j + i * 4]].icon, 2))
				{
					int use_color = 0;

					if (pc.mp >= profession_skill[BattleSkill[j + i * 4]].costmp && profession_skill[BattleSkill[j + i * 4]].costmp != 0)
						use_color = FONT_PAL_WHITE;
					else
						use_color = FONT_PAL_GRAY;

					sprintf(msg, "%s", profession_skill[BattleSkill[j + i * 4]].name);
					StockFontBuffer(355, 310, FONT_PRIO_FRONT, use_color, msg, 0);
					sprintf(msg, "%d％", profession_skill[BattleSkill[j + i * 4]].skill_level);
					StockFontBuffer(440, 310, FONT_PRIO_FRONT, use_color, msg, 0);
					sprintf(msg, "(耗费MP:%d)", profession_skill[BattleSkill[j + i * 4]].costmp);
					StockFontBuffer(500, 310, FONT_PRIO_FRONT, use_color, msg, 0);
					sprintf(msg, "%s", profession_skill[BattleSkill[j + i * 4]].memo);
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
					if (mouse.onceState & MOUSE_LEFT_CRICK && profession_skill[BattleSkill[j + i * 4]].useFlag == 0)
					{
						if (pc.mp >= profession_skill[BattleSkill[j + i * 4]].costmp && profession_skill[BattleSkill[j + i * 4]].costmp != 0)
						{
							lssproto_BATTLESKILL_send(sockfd, BattleSkill[j + i * 4]);
							play_se(217, 320, 240);
						}
					}
#endif
				}
				sprintf(msg2, "%8s", profession_skill[BattleSkill[j + i * 4]].name);
				StockFontBuffer(x + j * 60, y + 110 + i * 57, FONT_PRIO_FRONT, profession_skill[BattleSkill[j + i * 4]].useFlag + 1, msg2, 0);
			}
		}
		break;
	case 2:
#ifdef _READ16BITBMP
		if (g_bUseAlpha)
			StockDispBuffer(x + 147, y + 225, DISP_PRIO_IME3, CG_FIELD_SKILL_ADVSK, 0);
		else
#endif
			StockDispBuffer(x + 138, y + 228, DISP_PRIO_IME3, CG_FIELD_SKILL_ADVSK, 0);
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (AdvanceSkill[j + i * 4] == -1)
					break;
				if (HitDispNo == StockDispBuffer(x + 29 + j * 60, y + 102 + i * 57, DISP_PRIO_IME3 + 1, profession_skill[AdvanceSkill[j + i * 4]].icon, 2))
				{
					int use_color = 0;

					if (pc.mp >= profession_skill[AdvanceSkill[j + i * 4]].costmp && profession_skill[AdvanceSkill[j + i * 4]].costmp != 0)
						use_color = FONT_PAL_WHITE;
					else
						use_color = FONT_PAL_GRAY;

					sprintf(msg, "%s", profession_skill[AdvanceSkill[j + i * 4]].name);
					StockFontBuffer(355, 310, FONT_PRIO_FRONT, use_color, msg, 0);
					sprintf(msg, "%d％", profession_skill[AdvanceSkill[j + i * 4]].skill_level);
					StockFontBuffer(440, 310, FONT_PRIO_FRONT, use_color, msg, 0);
					sprintf(msg, "(耗费MP:%d)", profession_skill[AdvanceSkill[j + i * 4]].costmp);
					StockFontBuffer(500, 310, FONT_PRIO_FRONT, use_color, msg, 0);
					sprintf(msg, "%s", profession_skill[AdvanceSkill[j + i * 4]].memo);
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
					if (mouse.onceState & MOUSE_LEFT_CRICK && profession_skill[AdvanceSkill[j + i * 4]].useFlag == 0)
					{
						if (pc.mp >= profession_skill[AdvanceSkill[j + i * 4]].costmp && profession_skill[AdvanceSkill[j + i * 4]].costmp != 0)
						{
							lssproto_BATTLESKILL_send(sockfd, AdvanceSkill[j + i * 4]);
							play_se(217, 320, 240);
						}
					}
#endif
				}
				sprintf(msg2, "%8s", profession_skill[AdvanceSkill[j + i * 4]].name);
				StockFontBuffer(x + j * 60, y + 110 + i * 57, FONT_PRIO_FRONT, profession_skill[AdvanceSkill[j + i * 4]].useFlag + 1, msg2, 0);
			}
		}
		break;
	}
	char* splitPoint = msg;

	sprintf(buf, "%s", msg);
	x = 355;
	y = 340;
	while (1)
	{
		if (strlen(splitPoint) > 34)
		{
			strncpy(msg, splitPoint, 34);
			buf[34] = NULL;
			if (GetStrLastByte(buf) == 3)
			{
				buf[33] = NULL;
				splitPoint += 33;
			}
			else
			{
				buf[34] = NULL;
				splitPoint += 34;
			}
			StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, buf, 0);
			y += 24;
		}
		else
		{
			strcpy(buf, splitPoint);
			StockFontBuffer(x, y, FONT_PRIO_FRONT, 0, buf, 0);
			break;
		}
	}
}
#endif
#endif

#ifdef _FRIENDCHANNEL			//ROG ADD 好友频道
ACTION* pChatRoomWnd;
short  chatRoomBtn = 0;
short  SelRoomBtn = 0;
short chatRoomBtnFocus = 0;
int   chatRoomBtnGraNo[] =
{
	CG_FIELD_CHATROOM_BTN_OFF,
	CG_FIELD_CHATROOM_BTN_ON
};
#endif

// ???????
short menuBtn = 0;
short menuBtnFocus = 0;
int menuBtnGraNo[] =
{
	CG_FIELD_MENU_BTN_OFF,
	CG_FIELD_MENU_BTN_ON
};

// ???
short cardBtn = 0;
short cardBtnFocus = 0;
int cardBtnGraNo[] =
{
	CG_FIELD_CARD_BTN_OFF,
	CG_FIELD_CARD_BTN_ON
};

// Robin 04/12 Trade Button
short tradeBtn = 0;
short tradeBtnFocus = 0;
//short tradeBtnEnableFlag = 1;
int tradeBtnGraNo[] =
{
	CG_FIELD_TRADE_BTN_OFF,
	CG_FIELD_TRADE_BTN_ON
};

#ifdef _ANGEL_SUMMON
extern short HelpProcNo;
extern short jumpHelpPage;
extern short jumpHelpSeg;

int angelFlag = FALSE;
int angelRecvType;
int angelRecvButton;
char angelMsg[1024 * 10] = "";

short angelLampDrawFlag = 0;
unsigned int angelLampFlashTime = 0;

short angelBtn = 0;
short angelBtnFocus = 0;
int angelBtnGraNo[] =
{
	35273, // CG_FIELD_ANGEL_BTN_OFF
	35272  // CG_FIELD_ANGEL_BTN_ON
};
#endif

// Robin 06/01 Channel Button
short channelBtn = 0;
short channelBtnFocus = 0;
short channelWNFlag = 1;
int channelBtnGraNo[] =
{
	CG_FIELD_CHANNEL_BTN_OFF,
	CG_FIELD_CHANNEL_BTN_ON
};

// ?????
short partyBtn = 0;
short partyBtnFocus = 0;
short partyBtnEnableFlag = 1;
int partyBtnGraNo[] =
{
	CG_FIELD_GROUP_BTN_OFF,
	CG_FIELD_GROUP_BTN_ON
};

// ??????
short mailLamp = 0;
short mailLampDrawFlag = 0;

#ifdef __PHONEMESSAGE
// LeiBoy 2002 Jan.26 --- Cell Phone's Messages Button -- BEGIN
short msgBtn = 0;
short msgBtnFocus = 0;
int   msgBtnGraNo[] =
{
	CG_FIELD_MSG_BTN_OFF,
	CG_FIELD_MSG_BTN_ON
};
// LeiBoy 2002 Jan.26 --- Cell Phone's Messages Button -- END
#endif

int helpBtnGraNo[] =
{
	IMAGE_Guide_Off,
	IMAGE_Guide_On
};
short helpBtn = 0;
short helpBtnFocus = 0;
unsigned int mailLampFlashTime = 0;
// ???
short joinBattleBtn = 0;
short joinBattleBtnFocus = 0;
short joinBattleBtnEnableFlag = 1;
short joinBattleHelpMsgFlag = 1;
int joinBattleBtnGraNo[] =
{
	CG_FIELD_JOIN_BTL_BTN_OFF,
	CG_FIELD_JOIN_BTL_BTN_ON
};
// ???
short duelBtn = 0;
short duelBtnFocus = 0;
int duelBtnGraNo[] =
{
	CG_FIELD_DUEL_BTN_OFF,
	CG_FIELD_DUEL_BTN_ON
};

// ????????
short actBtn = 0;
short actBtnForcus = 0;
int actBtnUpTime;
int nowActMode;
int actBtnGraNo[] =
{
	CG_FIELD_ACT_BTN_OFF,
	CG_FIELD_ACT_BTN_ON
};
short charActionAnimeChangeProcNo;
short charActionAnimeChangeWindowFlag;
// ????ＯＫ???????
short etcSwitchProcNo;
short etcSwitchChangeFlag = 0;
// ?????
int amPmAnimeTime;
int amPmAnimeX;
int amPmAnimeGraNoIndex0, amPmAnimeGraNoIndex1;
int amPmAnimeGraNo[] =
{
	CG_FIELD_AM_PM_01,
	CG_FIELD_AM_PM_02,
	CG_FIELD_AM_PM_03,
	CG_FIELD_AM_PM_00
};

short ridelistBtn = 0;
short ridelistBtnFocus = 0;
int ridelistBtnGraNo[] =
{
	IMAGE_Ride_Off,
	IMAGE_Ride_On
};

short vipshopBtn = 0;
short vipshopBtnFocus = 0;
int	vipshopBtnGraNo[] =
{
	IMAGE_VipShop_off,
	IMAGE_VipShop_on
};

#ifdef _TEACHER_SYSTEM
short sTeacherSystemBtn = 0;
short sTeacherSystemBtnFocus = 0;
int iTeacherSystemBtnGraNo[] = {
	CG_FIELD_TS_BTN_OFF,
	CG_FIELD_TS_BTN_ON
};
#endif

#ifdef 顶部称号按钮
short s称号SystemBtn = 0;
short s称号SystemBtnFocus = 0;
int i称号SystemBtnGraNo[] = {
	CG_FIELD_TS_BTN_OFF,
	CG_FIELD_TS_BTN_ON
};
#endif

// ????????????????
short etcSendFlag = 0;
// ??????
short nowFieldMenuOpenFlag = 0;		// ??????????????????
// ???????????????????????
unsigned int fieldBtnPushTime = 0;
// ??????????
unsigned int fieldInfoTime = 0;
// ??????
void initCharActionAnimeChange(void);
int charActionAnimeChange(void);
void initEtcSwitch(void);
int  etcSwitch(void);
// Robin 06/02
void initJoinChannelWN(void);
void  closeJoinChannelWN(void);
void  joinChannelWN(void);
void initJoinChannel2WN(char* data);
void  closeJoinChannel2WN(void);
void  joinChannel2WN(void);
void  leaderFuncWN(void);
//static short joinChannelProc = 0;
static short joinChannel2Proc = 0;
struct tm* serverAliveTime;
time_t serverAliveLongTime;

// ?????????????
void initFieldProc(void)
{
	int i;

	drawFieldButtonFlag = 1;
	menuBtn = 0;
	cardBtn = 0;
	partyBtn = 0;
	tradeBtn = 0;
	// initPcAll()??????????????????
	//	mailLamp = 0;
#ifdef __PHONEMESSAGE
	msgBtn = 0;  //LeiBoy 2002 Jan.26
#endif
#ifdef _STREET_VENDOR
	sStreetVendorBtn = 0;
	sStreetVendorBuyBtn = 0;
#else
#ifdef _TABLEOFSKILL				// (不可开) Syu ADD 7.0 新增人物技能表单
	skillBtn = 0;
#endif
#endif
#ifdef _FRIENDCHANNEL      //ROG ADD 好友频道
	chatRoomBtn = 0;
	SelRoomBtn = 0;
	TalkMode = 0;
#endif
	mailLampDrawFlag = 0;
	mailLampFlashTime = 0;
	joinBattleBtn = 0;
	duelBtn = 0;
	actBtn = 0;
	actBtnForcus = 0;
	nowActMode = 0;
	drawTimeAnimeFlag = 1;

	for (i = 0; i < FIELD_FUNC_END; i++)
		fieldBtnHitId[i] = -2;
	etcSendFlag = 0;
	etcSwitchChangeFlag = 0;
	fieldInfoTime = 0;
	partyBtnEnableFlag = 1;
	joinBattleBtnEnableFlag = 1;
	joinBattleHelpMsgFlag = 1;
	//tradeBtnEnableFlag = 1;
}

// ????????????
void resetFieldProc(void)
{
	actBtn = 0;
	menuBtn = 0;
	fieldInfoTime = 0;
#ifdef __PHONEMESSAGE
	msgBtn = 0;
#endif
	channelWNFlag = 0;
#ifdef _STREET_VENDOR
	sStreetVendorBtn = 0;
	sStreetVendorBuyBtn = 0;
#else
#ifdef _TABLEOFSKILL
	skillBtn = 0;
#endif
#endif
	helpBtn = 0;
}

void fieldProc(void)
{
	int selId;
	int pushId;
	int focusId;
	BOOL walkFlag = FALSE;
	rand2();
	actionShortCutKeyProc();
	if (etcSwitchChangeFlag && eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0) {
		if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime()) {
			if (bNewServer)
				lssproto_FS_send(sockfd, pc.etcFlag);
			else
				old_lssproto_FS_send(sockfd, pc.etcFlag);
			fieldBtnPushTime = TimeGetTime();
			etcSwitchChangeFlag = 0;
		}
	}
	if (CheckMenuFlag() || windowTypeWN > -1 || actBtn == 1 || menuBtn == 1 || channelWNFlag == 1)
		drawFieldButtonFlag = 0;
	else
		drawFieldButtonFlag = 1;

	nowFieldMenuOpenFlag = 0;
	if (windowTypeWN != -1)
		nowFieldMenuOpenFlag = 1;
	if (actBtn == 1)
		nowFieldMenuOpenFlag = 1;
	if (menuBtn == 1)
		nowFieldMenuOpenFlag = 1;
	amPmAnimeTime = (SaTime.hour + 832) % 1024;
	amPmAnimeGraNoIndex0 = amPmAnimeTime / 256;
	amPmAnimeGraNoIndex1 = (amPmAnimeGraNoIndex0 + 1) % 4;
	amPmAnimeX = (amPmAnimeTime % 256) / 4;

	if (nowVx != 0 || nowVy != 0)
		walkFlag = TRUE;

	if (etcSendFlag)
	{
		if (prSendFlag == 0 && jbSendFlag == 0 && duelSendFlag == 0)
			etcSendFlag = 0;
		else
			etcSendFlag = 1;
	}
#if 1
	selId = selGraId(fieldBtnHitId, sizeof(fieldBtnHitId) / sizeof(int));
	pushId = pushGraId(fieldBtnHitId, sizeof(fieldBtnHitId) / sizeof(int));
	focusId = focusGraId(fieldBtnHitId, sizeof(fieldBtnHitId) / sizeof(int));
	if (etcSendFlag != 0 || (walkFlag != FALSE && selId != FIELD_FUNC_PARTY))
		selId = -1;
#else
	if (etcSendFlag == 0 && walkFlag == FALSE)
	{
		selId = selGraId(fieldBtnHitId, sizeof(fieldBtnHitId) / sizeof(int));
		pushId = pushGraId(fieldBtnHitId, sizeof(fieldBtnHitId) / sizeof(int));
		focusId = focusGraId(fieldBtnHitId, sizeof(fieldBtnHitId) / sizeof(int));
	}
	else
		selId = -1;
#endif
	if (/* nowFloor == 32021
		||	*/nowFloor == 31706 || nowFloor == 10204 || (10601 <= nowFloor && nowFloor <= 10605)
		|| nowFloor == 10919 || nowFloor == 10920 || nowFloor == 20711 || nowFloor == 20712
		|| nowFloor == 1008 || nowFloor == 1021 || nowFloor == 3008 || nowFloor == 3021
#ifdef _LOCKPARTYFLOOR				// (可开放) Syu ADD 增加不可组队的地图编号
		|| (nowFloor <= 8213 && nowFloor >= 8200)
#endif
		|| (nowFloor >= 30017 && nowFloor <= 30021) || (nowFloor == 7025)	// 森林捉宠活动
		)
		partyBtnEnableFlag = 0;
	else
		partyBtnEnableFlag = 1;

	if (nowFloor < 10000 && (nowFloor / 1000 > 0) && (nowFloor % 1000 == 7) || nowFloor == 130)
	{
		joinBattleHelpMsgFlag = 0;
		joinBattleBtnEnableFlag = 1;
	}
	else
		if (nowFloor == 1008 || nowFloor == 1021 || nowFloor == 3008 || nowFloor == 3021
			|| nowFloor == 10919 || nowFloor == 10920 || nowFloor == 20711 || nowFloor == 20712
			|| nowFloor == 10204)
		{
			joinBattleHelpMsgFlag = 2;
			joinBattleBtnEnableFlag = 0;
		}
		else
		{
			joinBattleHelpMsgFlag = 1;	// 
			joinBattleBtnEnableFlag = 1;
		}
	if (selId == FIELD_FUNC_MENU || (joy_trg[1] & JOY_CTRL_Q)
#ifdef __PHONEMESSAGE
		&& msgBtn == 0
#endif
#ifdef _STREET_VENDOR
		&& sStreetVendorBtn == 0
		&& sStreetVendorBuyBtn == 0
#endif

		&& helpBtn == 0
		)
	{
		if (menuBtn == 0) {
			closeCharActionAnimeChange();
			closeJoinChannelWN();
			if (CheckMenuFlag())
				InitMenu2();

			initEtcSwitch();
			menuBtn = 1;
			play_se(202, 320, 240);
		}
		else
			closeEtcSwitch();
	}
	if (menuBtn == 1 && etcSwitch())
		menuBtn = 0;
	if (focusId == FIELD_FUNC_MENU)
		menuBtnFocus = 1;
	else
		menuBtnFocus = 0;
	if (pushId == FIELD_FUNC_WARP_) {
		if (selId == FIELD_FUNC_WARP_) {
			{
				play_se(202, 320, 240);
				lssproto_SaMenu_send(sockfd, 3, 7, 1);
			}
		}
	}
	if (pushId == FIELD_FUNC_VIPSHOP) {
		if (selId == FIELD_FUNC_VIPSHOP) {
			{
				play_se(202, 320, 240);
				lssproto_SaMenu_send(sockfd, 3, 9, 1);
			}
		}
	}
#ifdef _STREET_VENDOR
	if (pushId == FIELD_FUNC_STREET_VENDOR) {
		if (selId == FIELD_FUNC_STREET_VENDOR) {
			{

				{
					play_se(202, 320, 240);
					lssproto_STREET_VENDOR_send(sockfd, "O|");
					sStreetVendorBtn = 0;
				}
			}
		}
	}
	if (sStreetVendorBtn == 1 || sStreetVendorBtn == 3)
	{
		actBtn = 1;
		closeEtcSwitch();
		closeJoinChannelWN();
		if (CheckMenuFlag())
			InitMenu2();
		sStreetVendorBtn = 2;
		iOldGX = nowGx;
		iOldGY = nowGy;
		// 交易及组队及PK选项关闭
		pc.etcFlag &= ~PC_ETCFLAG_TRADE;
		pc.etcFlag &= ~PC_ETCFLAG_PARTY;
		pc.etcFlag &= ~PC_ETCFLAG_DUEL;
		if (bNewServer)
			lssproto_FS_send(sockfd, pc.etcFlag);
		else
			old_lssproto_FS_send(sockfd, pc.etcFlag);
	}
	else
		if (sStreetVendorBtn == 2)
		{
			StreetVendorWndfunc(true, NULL);
			if (((joy_trg[0] & JOY_ESC) && GetImeString() == NULL) || CheckMenuFlag())
			{
				play_se(203, 320, 240);
				sStreetVendorBtn = 0;
				actBtn = 0;
			}
		}
		else
		{
			if (pActStreetVendorWnd)
			{
				DeathAction(pActStreetVendorWnd);
				pActStreetVendorWnd = NULL;
				if (pShowPet != NULL)
				{
					DeathAction(pShowPet);
					pShowPet = NULL;
				}
			}
			if (pActSellPriceWnd)
			{
				DeathAction(pActSellPriceWnd);
				pActSellPriceWnd = NULL;
			}
		}
	if (sStreetVendorBuyBtn == 1)
	{
		actBtn = 1;
		closeEtcSwitch();
		closeJoinChannelWN();
		if (CheckMenuFlag())
			InitMenu2();
		sStreetVendorBuyBtn = 2;
		iOldGX = nowGx;
		iOldGY = nowGy;
	}
	else
		if (sStreetVendorBuyBtn == 2)
		{
			StreetVendorBuyWndfunc(NULL);
			if (((joy_trg[0] & JOY_ESC) && GetImeString() == NULL) || CheckMenuFlag())
			{
				play_se(203, 320, 240);
				sStreetVendorBuyBtn = 0;
				actBtn = 0;
			}
		}
		else
		{
			if (pActStreetVendorBuyWnd)
			{
				DeathAction(pActStreetVendorBuyWnd);
				pActStreetVendorBuyWnd = NULL;
				actBtn = 0;
				if (pShowPet != NULL)
				{
					DeathAction(pShowPet);
					pShowPet = NULL;
				}
				lssproto_STREET_VENDOR_send(sockfd, "N|");
			}
		}
	// 在摆摊或是向摆摊的人购买时走动则취소
	if ((pc.iOnStreetVendor == 1 || sStreetVendorBuyBtn == 2) && (iOldGX != nowGx || iOldGY != nowGy))
	{
		if (pc.iOnStreetVendor == 1)
			lssproto_STREET_VENDOR_send(sockfd, "E|");
		if (sStreetVendorBuyBtn == 2)
			lssproto_STREET_VENDOR_send(sockfd, "N|");
		pc.iOnStreetVendor = 0;
		sStreetVendorBtn = 0;
		sStreetVendorBuyBtn = 0;
		actBtn = 0;
		closeCharActionAnimeChange();
	}

	if (focusId == FIELD_FUNC_STREET_VENDOR)
		sStreetVendorBtnFocus = 1;
	else
		sStreetVendorBtnFocus = 0;
#else
#ifndef _BACK_VERSION
#ifdef _TABLEOFSKILL
	if (pushId == FIELD_FUNC_SKILL)
	{
		if (selId == FIELD_FUNC_SKILL)
		{
			closeEtcSwitch();
			closeJoinChannelWN();
			if (CheckMenuFlag())
				InitMenu2();
			play_se(202, 320, 240);
			actBtn = 1;
			skillBtn = 1;
		}
	}
	if (skillBtn == 1)
	{
		//主要技能视窗函式
		SkillWndfunc();
		if (((joy_trg[0] & JOY_ESC) && GetImeString() == NULL) || CheckMenuFlag())
		{
			play_se(203, 320, 240);
			skillBtn = 0;
			actBtn = 0;
		}
	}
	else
	{
		if (pActSkillWnd)
		{
			DeathAction(pActSkillWnd);
			pActSkillWnd = NULL;
		}
	}
	if (focusId == FIELD_FUNC_SKILL)
		skillBtnFocus = 1;
	else
		skillBtnFocus = 0;
#endif //_TABLEOFSKILL
#endif //_BACK_VERSION
#endif //_STREET_VENDOR
	if (pushId == FIELD_FUNC_CARD) {
		cardBtn = 1;

		if (selId == FIELD_FUNC_CARD) {
			int dx, dy;
			int flag;
			getRouteData(pc.dir, &dx, &dy);
			flag = checkCharObjPoint(nowGx + dx, nowGy + dy, CHAROBJ_TYPE_USER_NPC);
			if (partyModeFlag == 0 && flag == TRUE && eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0) {
				if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime()) {
					if (bNewServer)
						lssproto_AAB_send(sockfd, nowGx, nowGy);
					else
						old_lssproto_AAB_send(sockfd, nowGx, nowGy);
					fieldBtnPushTime = TimeGetTime();
				}
			}
			play_se(217, 320, 240);	// ?????
		}
	}
	else
		cardBtn = 0;
	if (focusId == FIELD_FUNC_CARD)
		cardBtnFocus = 1;		// ???????????????
	else
		cardBtnFocus = 0;

#ifdef _FRIENDCHANNEL			//ROG ADD 好友频道
	if (pushId == FIELD_FUNC_CHATROOM) {
		if (selId == FIELD_FUNC_CHATROOM) {
			closeEtcSwitch();
			closeJoinChannelWN();
			if (CheckMenuFlag())
				InitMenu2();

			if (strcmp(pc.chatRoomNum, "") == 0) {
				if (SelRoomBtn == 1 || pSelChanlWnd != NULL)
					SelRoomBtn = 0;
				else
				{
					/*
					// WON ADD	防止传送太频繁
					{
					unsigned int now_time = (unsigned int)time(NULL);
					static unsigned int old_time = 0;

					  chatRoomBtn = 1;

						if( now_time > old_time ){

						  old_time = now_time + 10;

							lssproto_CHATROOM_send( sockfd,"B|");
							}
							}

							  old_time = now_time + 10;
						*/
#ifdef _CHATROOMPROTOCOL
					lssproto_CHATROOM_send(sockfd, "B|");
#endif
				}
			}
			else {
				if (chatRoomBtn == 1 || pChtChanlWnd != NULL)
					chatRoomBtn = 0;
				else
					chatRoomBtn = 1;
			}
			if ((joy_trg[0] & JOY_ESC) || CheckMenuFlag())
			{
				play_se(203, 320, 240);
				chatRoomBtn = 0;
			}
		}
	}

	if (SelRoomBtn == 1)
	{
		SelectChatRoom();
		if (((joy_trg[0] & JOY_ESC) && GetImeString() == NULL) || CheckMenuFlag())
		{
			play_se(203, 320, 240);
			SelRoomBtn = 0;
		}
	}
	else if (chatRoomBtn == 1)
	{
		ChatRoomWnd();
		if (((joy_trg[0] & JOY_ESC) && GetImeString() == NULL) || CheckMenuFlag())
		{
			play_se(203, 320, 240);
			chatRoomBtn = 0;
		}
	}
	else
	{
		if (pChtChanlWnd)
		{
			DeathAction(pChtChanlWnd);
			pChtChanlWnd = NULL;
		}
		if (pSelChanlWnd)
		{
			DeathAction(pSelChanlWnd);
			pSelChanlWnd = NULL;
		}
	}

	if (focusId == FIELD_FUNC_CHATROOM)
		chatRoomBtnFocus = 1;
	else
		chatRoomBtnFocus = 0;
#endif

	// Robin 04/11 Trade
	if (pushId == FIELD_FUNC_TRADE)
	{
		tradeBtn = 1;

		if (selId == FIELD_FUNC_TRADE)
		{
			int dx, dy;
			int flag;

			// ???????????
			getRouteData(pc.dir, &dx, &dy);
			flag = checkCharObjPoint(nowGx + dx, nowGy + dy, CHAROBJ_TYPE_USER_NPC);
			// ??????????????????
			if (partyModeFlag == 0 && flag == TRUE && eventWarpSendFlag == 0 && eventEnemySendFlag == 0
				&& sendEnFlag == 0 && tradeFlag == 0
#ifdef _STREET_VENDOR
				&& (pc.iOnStreetVendor == 0 || sStreetVendorBuyBtn == 2)
#endif

				)
			{
				if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime())// ??
				{
					if (bNewServer)
						lssproto_TD_send(sockfd, "D|D");
					else
						old_lssproto_TD_send(sockfd, "D|D");
					fieldBtnPushTime = TimeGetTime();
				}
			}
#ifdef _STREET_VENDOR
			if (pc.iOnStreetVendor == 1)
				StockChatBufferLine("노점상에서 교환을 해서는 안 된다", FONT_PAL_RED);
#endif
			play_se(217, 320, 240);	// ?????
		}
	}
	else
		tradeBtn = 0;
	if (focusId == FIELD_FUNC_TRADE)
		tradeBtnFocus = 1;		// ???????????????
	else
		tradeBtnFocus = 0;
#ifdef _ANGEL_SUMMON
	if (pushId == FIELD_FUNC_ANGEL)
	{
		angelBtn = 1;

		if (selId == FIELD_FUNC_ANGEL)
		{
			if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime())// ??
			{
				//lssproto_WN_send( sockfd, nowGx, nowGy, WINDOW_MESSAGETYPE_ANGELMESSAGE, , WINDOW_BUTTONTYPE_YES, msg ) ;
				if (angelFlag)
					openServerWindow(WINDOW_MESSAGETYPE_MESSAGE, angelRecvButton, angelRecvType, -1, angelMsg);
				else
				{
					if (helpBtn == 0)
					{
						// ????ＯＫ???????????????????????
						// ?????????????
						closeEtcSwitch();
						closeJoinChannelWN();
						// ???????????
						if (CheckMenuFlag())
							InitMenu2();
						HelpProcNo = 0;
						helpBtn = 1;
						actBtn = 1;
						jumpHelpPage = 52;
						jumpHelpSeg = 29;
						play_se(202, 320, 240);	// ????????
					}
					else
						HelpProcNo = 1000;
				}
				fieldBtnPushTime = TimeGetTime();
			}
			play_se(217, 320, 240);	// ?????
		}
	}
	else
		angelBtn = 0;
	if (focusId == FIELD_FUNC_ANGEL)
		angelBtnFocus = 1;		// ???????????????
	else
		angelBtnFocus = 0;

	if (angelFlag) {
		if (angelLampFlashTime == 0)
		{
			angelLampDrawFlag = 1;
			angelLampFlashTime = TimeGetTime();
		}
		else if (angelLampFlashTime + FIELD_MAIL_LAMP_FLASH_TIME < TimeGetTime())
		{
			angelLampDrawFlag++;
			angelLampDrawFlag &= 1;
			angelLampFlashTime = TimeGetTime();
		}
	}
	else
	{
		angelLampDrawFlag = 0;
		angelLampFlashTime = 0;
	}
#endif

#ifdef 顶部称号按钮	//[톱 호칭 버튼]

	static ACTION* pAct称号Wnd = NULL;

	if (pushId == FIELD_FUNC_称号 && selId == FIELD_FUNC_称号)
	{
		if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime())  //[클릭 간격은 500ms이어야 해]
		{
			play_se(202, 320, 240);
			//	lssproto_TEACHER_SYSTEM_send(sockfd, "P|");
			fieldBtnPushTime = TimeGetTime();

			//[창구를 만들다]
			pAct称号Wnd = MakeWindowDisp(0, 0, 0, 0, 0, -1);

			s称号SystemBtn = 0;
		}
	}

	if (pAct称号Wnd && pAct称号Wnd->hp > 0)
	{
		//[창을 그려내다]
		StockDispBuffer绝对位置(180, 70, DISP_PRIO_MENU, 称号界面, 1);
	}

	if (focusId == FIELD_FUNC_称号)  //focus了，[밑을 나타내는 알림 문자]
		s称号SystemBtnFocus = 1;
	else
		s称号SystemBtnFocus = 0;

#endif


#ifdef _TEACHER_SYSTEM
	if (pushId == FIELD_FUNC_TEACHER && selId == FIELD_FUNC_TEACHER)
	{
		if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime())
		{
			play_se(202, 320, 240);
			lssproto_TEACHER_SYSTEM_send(sockfd, "P|");
			fieldBtnPushTime = TimeGetTime();
			sTeacherSystemBtn = 0;
		}
	}
	if (sTeacherSystemBtn == 1)
	{
		if (helpBtn == 0)
		{
			closeEtcSwitch();
			closeJoinChannelWN();
			if (CheckMenuFlag())
				InitMenu2();
			HelpProcNo = 0;
			helpBtn = 1;
			actBtn = 1;
			jumpHelpPage = 54 + 1;
			jumpHelpSeg = 30;
			play_se(202, 320, 240);
		}
		else
			HelpProcNo = 1000;
		sTeacherSystemBtn = 0;
	}
	else if (sTeacherSystemBtn == 2)
		TeacherSystemWndfunc(0, NULL);
	else if (sTeacherSystemBtn == 3)
		TeacherSystemWndfunc(1, NULL);
	else if (sTeacherSystemBtn == 4)
		TeacherSystemWndfunc(2, NULL);
	else if (sTeacherSystemBtn == 5)
		TeacherSystemWndfunc(3, NULL);
	if (focusId == FIELD_FUNC_TEACHER)
		sTeacherSystemBtnFocus = 1;
	else
		sTeacherSystemBtnFocus = 0;
#endif
	// Robin 06/01 Channel
	if (selId == FIELD_FUNC_CHANNEL)
	{
#ifdef _FRIENDCHANNEL
		closeJoinChannelWN();
#endif
		if (pc.familyName[0] == NULL)
		{
			char buf[64];

			lssproto_TK_recv(sockfd, 0, "P|부족에 가입되지 않아 정보를 확인할 수 없어요.", 6);
			sprintf(buf, "S|P", selId);
			if (bNewServer)
				lssproto_FM_send(sockfd, buf);
			else
				old_lssproto_FM_send(sockfd, buf);
			return;
		}
		if (channelWNFlag == 0)
		{
			if (CheckMenuFlag())
				InitMenu2();
			channelWNFlag = 1;
			initJoinChannelWN();
			play_se(202, 320, 240);
		}
		else
			closeJoinChannelWN();
	}
	if (focusId == FIELD_FUNC_CHANNEL)
		channelBtnFocus = 1;
	else
		channelBtnFocus = 0;

	if (channelWNFlag == 1)
		joinChannelWN();

#ifndef _CHANNEL_MODIFY
	if (bNewServer && joy_trg[1] & JOY_CTRL_C)
	{
		char buf[64];

		if (pc.channel == -1)
		{
			if ((pc.quickChannel != -1) && (pc.familyName[0] != NULL))
			{
				sprintf(buf, "C|J|%d", pc.quickChannel);
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
			}
		}
		else
		{
			if (bNewServer)
				lssproto_FM_send(sockfd, "C|J|-1");
			else
				old_lssproto_FM_send(sockfd, "C|J|-1");
		}
	}
#endif
	if (pushId == FIELD_FUNC_PARTY) {
		partyBtn = 1;
		if (partyBtnEnableFlag == 0) {
			partyBtn = 0;
			play_se(220, 320, 240);
		}
		else if (selId == FIELD_FUNC_PARTY) {
			if (partyModeFlag == 0) {
				int dx, dy;
				int flag;
				getRouteData(pc.dir, &dx, &dy);
				flag = checkCharObjPointNotStatus(nowGx + dx, nowGy + dy, (CHAROBJ_TYPE_USER_NPC | CHAROBJ_TYPE_PARTY_OK), (1 << 31));
				if (flag == 1 && eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0
#ifdef _STREET_VENDOR
					&& (pc.iOnStreetVendor == 0 || sStreetVendorBuyBtn == 2)
#endif

					)
				{
					if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime())// ??
					{
						// ?????????????????????????
						if (bNewServer)
							lssproto_PR_send(sockfd, nowGx, nowGy, 1);
						else
							old_lssproto_PR_send(sockfd, nowGx, nowGy, 1);
						prSendMode = 1;
						prSendFlag = 1;
						etcSendFlag = 1;
						fieldBtnPushTime = TimeGetTime();
					}
				}
#ifdef _STREET_VENDOR
				if (pc.iOnStreetVendor == 1)
					StockChatBufferLine("이곳에서 파티를 구성할 수 없습니다", FONT_PAL_RED);
#endif
			}
			else {
				if (eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0) {
					if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime()) {
						if (bNewServer)
							lssproto_PR_send(sockfd, nowGx, nowGy, 0);
						else
							old_lssproto_PR_send(sockfd, nowGx, nowGy, 0);
						prSendMode = 0;
						prSendFlag = 1;
						etcSendFlag = 1;
						fieldBtnPushTime = TimeGetTime();
#ifdef _CHANNEL_MODIFY
						TalkMode = 0;
#endif
					}
				}
			}
			play_se(217, 320, 240);
		}
	}
	else
		partyBtn = 0;

	if (focusId == FIELD_FUNC_PARTY)
		partyBtnFocus = 1;
	else
		partyBtnFocus = 0;
#ifdef __PHONEMESSAGE
	// LeiBoy 2002 Feb. 2 --- Cell Phone's Messages Button -- BEGIN
	if (focusId == FIELD_FUNC_MSG)
		msgBtnFocus = 1;
	else
		msgBtnFocus = 0;
	if (pushId == FIELD_FUNC_MSG)
	{
		extern int MsgID;

		if (selId == FIELD_FUNC_MSG)
		{
			if (msgBtn == 0)
			{
				/*closeEtcSwitch();
				closeJoinChannelWN();
				if (CheckMenuFlag())
					InitMenu2();
				MsgID = 0;
				msgBtn = 1;
				play_se(202, 320, 240);
				actBtn = 1;*/

				::ShellExecute(hWnd, NULL, "http://www.waei.com/", NULL, NULL, SW_SHOW);
			}
		}
	}

	//if (msgBtn == 1)
	//{
	//	extern int MsgProc();

	//	int msgret = MsgProc();

	//	if (msgret)
	//	{
	//		msgBtn = 0;
	//		actBtn = 0;
	//		GetKeyInputFocus(&MyChatBuffer);
	//	}
	//}
	// LeiBoy 2002 Feb. 2 --- Cell Phone's Messages Button -- END
#endif
	helpBtnFocus = 0;
	if (focusId == FIELD_FUNC_HELP)
		helpBtnFocus = 1;
	if (selId == FIELD_FUNC_HELP) {
		extern short HelpProcNo;
		if (helpBtn == 0) {
			closeEtcSwitch();
			closeJoinChannelWN();
			if (CheckMenuFlag())
				InitMenu2();
			lssproto_SaMenu_send(sockfd, 3, 8, 1);
			//HelpProcNo = 0;
			//helpBtn = 1;
			//actBtn = 1;
			play_se(202, 320, 240);	// ????????
		}
		else
			HelpProcNo = 1000;
	}
	if (helpBtn == 1) {
		extern int HelpProc();
		if (HelpProc()) {
			helpBtn = 0;
			actBtn = 0;
		}
	}
	if (pushId == FIELD_FUNC_JOIN_BATTLE) {
		joinBattleBtn = 1;
		if (joinBattleBtnEnableFlag == 0) {
			joinBattleBtn = 0;
			play_se(220, 320, 240);
		}
		else if (selId == FIELD_FUNC_JOIN_BATTLE) {
			int dx, dy;
			int flag;
			getRouteData(pc.dir, &dx, &dy);
			flag = checkCharObjPointStatus(nowGx + dx, nowGy + dy, CHAROBJ_TYPE_USER_NPC, CHR_STATUS_BATTLE | CHR_STATUS_WATCH);
			if ((partyModeFlag == 0 || (partyModeFlag == 1 && (pc.status & CHR_STATUS_LEADER) != 0))
				&& flag == 1 && eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0) {
				if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime()) {
					if (bNewServer) {
						if (nowFloor == 9000)
							lssproto_LB_send(sockfd, nowGx, nowGy);
						else
							lssproto_JB_send(sockfd, nowGx, nowGy);
					}
					else
						old_lssproto_JB_send(sockfd, nowGx, nowGy);
					jbSendFlag = 1;
					etcSendFlag = 1;
					fieldBtnPushTime = TimeGetTime();
				}
			}
			play_se(217, 320, 240);
		}
	}
	else
		joinBattleBtn = 0;
	if (focusId == FIELD_FUNC_JOIN_BATTLE)
		joinBattleBtnFocus = 1;
	else
		joinBattleBtnFocus = 0;

	if (pushId == FIELD_FUNC_DUEL) {
		duelBtn = 1;
		if (selId == FIELD_FUNC_DUEL) {
			int dx, dy;
			int flag;
			getRouteData(pc.dir, &dx, &dy);
			flag = checkCharObjPoint(nowGx + dx, nowGy + dy, CHAROBJ_TYPE_USER_NPC);
			if ((partyModeFlag == 0 || (partyModeFlag == 1 && (pc.status & CHR_STATUS_LEADER) != 0))
				&& flag == TRUE && eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0) {
				if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime()) {
					if (bNewServer)
						lssproto_DU_send(sockfd, nowGx, nowGy);
					else
						old_lssproto_DU_send(sockfd, nowGx, nowGy);
					duelSendFlag = 1;
					etcSendFlag = 1;
					fieldBtnPushTime = TimeGetTime();
				}
			}
			play_se(217, 320, 240);
		}
	}
	else
		duelBtn = 0;
	if (focusId == FIELD_FUNC_DUEL)
		duelBtnFocus = 1;
	else
		duelBtnFocus = 0;
	if (selId == FIELD_FUNC_ACT || (joy_trg[1] & JOY_CTRL_W)) {
		if (actBtn == 0) {
			closeEtcSwitch();
			closeJoinChannelWN();
			if (CheckMenuFlag())
				InitMenu2();
			initCharActionAnimeChange();
			actBtn = 1;
			play_se(202, 320, 240);
		}
		else
			closeCharActionAnimeChange();
	}
	if (actBtn == 1 && charActionAnimeChange())
		actBtn = 0;
	if (focusId == FIELD_FUNC_ACT)
		actBtnForcus = 1;
	else
		actBtnForcus = 0;

	if (mailLamp) {
		if (mailLampFlashTime == 0) {
			mailLampDrawFlag = 1;
			mailLampFlashTime = TimeGetTime();
		}
		else if (mailLampFlashTime + FIELD_MAIL_LAMP_FLASH_TIME < TimeGetTime()) {
			mailLampDrawFlag++;
			mailLampDrawFlag &= 1;
			mailLampFlashTime = TimeGetTime();
		}
	}
	else {
		mailLampDrawFlag = 0;
		mailLampFlashTime = 0;
	}
}

void fieldProc2(void) {
	if ((joy_trg[1] & JOY_CTRL_Q)) {
		if (menuBtn == 0) {
			closeCharActionAnimeChange();
			closeJoinChannelWN();
			if (CheckMenuFlag())
				InitMenu2();
			initEtcSwitch();
			menuBtn = 1;
			play_se(202, 320, 240);
		}
		else
			closeEtcSwitch();
	}
	if (menuBtn == 1 && etcSwitch())
		menuBtn = 0;
}

#ifdef _AniCrossFrame	  // Syu ADD 动画层游过画面生物
void crossAniRelease() {
	if (WaterAct2 != NULL) {
		DeathAction(WaterAct2);
		WaterAct2 = NULL;
		Timeflag2 = false;
	}
}
#endif

#ifdef _AniImmobile	 // Syu ADD 定点产生特定动画
void ImmobileAniRelease()
{
	Updownflag = false;
	for (int i = 0; i < 560; i++)
	{
		if (XYposition[i][3] == 1)
		{
			if (WaterAct3[XYposition[i][4]] != NULL)
			{
				DeathAction(WaterAct3[XYposition[i][4]]);
				WaterAct3[XYposition[i][4]] = NULL;
				XYposition[i][3] = 0;
				XYposition[i][4] = -1;
				//				XYposition[i][5] = 1  ; //新增不漂浮的旗标2，故不做
				XYposition[i][6] = 0;
			}
		}
	}
}
#endif

#ifdef _AniRandom   // Syu ADD 随机产生环境动画
void AniRandomRelease()
{
	for (int i = 0; i < 50; i++)
	{
		if (WaterAct[i] != NULL)
		{
			DeathAction(WaterAct[i]);
			WaterAct[i] = NULL;
		}
	}
}
#endif


// ??????????????
void drawField(void)
{
	int leftUpPanelX = 0;		// ????????
	int leftUpPanelY = 0;
	int rightUpPanelX = 504 + DISPLACEMENT_X;	// ????????
	int rightUpPanelY = 0;
	int i;
#ifdef _WATERANIMATION //Syu ADD 泪之海动画层
	int Createflag = 0;
	int RandAnimate = 0;
#endif
#ifdef _AniRandom   // Syu ADD 随机产生环境动画
	int x1, y1;
#endif
#ifdef _AniCrossFrame	  // Syu ADD 动画层游过画面生物
	int x2, y2;
#endif

#if 0
#ifdef  _STONDEBUG__MSG

	char msg[256];
	// ?????Echo??????????
	//  ???????????????????????
	sprintf(msg, "%s Server Alive -> %02d/%02d/%02d %02d:%02d:%02d",
		selectServerName2[selectServerIndex],
		(serverAliveTime->tm_year % 100), serverAliveTime->tm_mon + 1, serverAliveTime->tm_mday,
		serverAliveTime->tm_hour, serverAliveTime->tm_min, serverAliveTime->tm_sec);
	StockFontBuffer(108, 8, FONT_PRIO_FRONT, 0, msg, 0);

#endif
#endif

#ifdef _SPECIALSPACEANIM	// Syu ADD 特殊场景动画配置
	SpecAnim(nowFloor);
#endif
#ifdef _AniCrossFrame	  // Syu ADD 动画层游过画面生物
	if ((nowFloor == 817) || (nowFloor == 8007) || (nowFloor == 8101) || (nowFloor == 8100) ||
		(nowFloor == 8027) || (nowFloor == 8028) || (nowFloor == 8029) || (nowFloor == 8015) || (nowFloor == 8113) || (nowFloor == 8114))
	{
		//游过画面动画 Start
		if (flag22 == false)
		{
			if (WaterAct2 != NULL && Timeflag2 == false)
			{
				DeathAction(WaterAct2);
				WaterAct2 = NULL;
				Timeflag2 = true;
				//结束后下一次出现的等待时间
				TimeTickMax2 = RAND(500, 1500);
			}
		}
		if (Timeflag2 == true)
		{
			TimeTick2++;
			if (TimeTick2 > TimeTickMax2)
			{
				Timeflag2 = false;
				TimeTick2 = 0;
			}
		}
		if (WaterAct2 == NULL || delFlag == false)
		{
			delFlag = true;
			flag22 = true;
			x2 = RAND(0, 1);
			RandAnimate = RAND(0, 2);
			if (RandAnimate > 1)
				RandAnimate = RAND(101512, 101515);
			//有右下、左下、右上、左上四个方向
			else
			{
				RandAnimate = RAND(0, 4);
				if (RandAnimate > 2)
					RandAnimate = RAND(101582, 101593);
				else
					RandAnimate = RAND(101516, 101523);
				if (RandAnimate == 101516 || RandAnimate == 101518)
					x2 = RAND(2, 3);
				else if (RandAnimate == 101521 || RandAnimate == 101523 ||
					(RandAnimate <= 101587 && RandAnimate >= 101582))
					x2 = RAND(4, 5); //左上
				else if (RandAnimate == 101522 || RandAnimate == 101520 ||
					(RandAnimate <= 101593 && RandAnimate >= 101588))
					x2 = RAND(6, 7); //右上
			}
			//결정进入画面的位置
			if (x2 == 0)  // 左下
			{
				x2 = RAND(50, 640);
				y2 = 0;
				WaterAct2 = MakeAnimDisp(x2, y2, RandAnimate, 18);
			}
			else if (x2 == 1)
			{
				x2 = 640;
				y2 = RAND(50, 430);
				WaterAct2 = MakeAnimDisp(x2, y2, RandAnimate, 18);
			}
			else if (x2 == 2) // 右下 
			{
				x2 = RAND(0, 590);
				y2 = 0;
				WaterAct2 = MakeAnimDisp(x2, y2, RandAnimate, 18);
			}
			else if (x2 == 3)
			{
				x2 = 0;
				y2 = RAND(50, 430);
				WaterAct2 = MakeAnimDisp(x2, y2, RandAnimate, 18);
			}
			else if (x2 == 4) // 左上
			{
				x2 = RAND(50, 640);
				y2 = 480;
				WaterAct2 = MakeAnimDisp(x2, y2, RandAnimate, 18);
			}
			else if (x2 == 5)
			{
				x2 = 640;
				y2 = RAND(50, 430);
				WaterAct2 = MakeAnimDisp(x2, y2, RandAnimate, 18);
			}
			else if (x2 == 6) // 右上
			{
				x2 = 0;
				y2 = RAND(50, 430);
				WaterAct2 = MakeAnimDisp(x2, y2, RandAnimate, 18);
			}
			else if (x2 == 7)
			{
				x2 = RAND(0, 590);
				y2 = 480;
				WaterAct2 = MakeAnimDisp(x2, y2, RandAnimate, 18);
			}
		}
	}
	if ((nowFloor != 817) && (nowFloor != 8007) && (nowFloor != 8100) && (nowFloor != 8101) &&
		(nowFloor != 8027) && (nowFloor != 8028) && (nowFloor != 8029) && (nowFloor != 8015) && (nowFloor != 8113) && (nowFloor != 8114))
	{
		delFlag = false;
		flag22 = true;
		Timeflag2 = false;
	}
#endif

#ifdef _AniImmobile	 // Syu ADD 定点产生特定动画
	if (nowFloor == 817)
	{
		in817flag = true;
		for (i = 0; i < 560; i++)
		{
			//判断定点动画产生点是否在人物 32 * 32范围内
			if ((nowGx > XYposition[i][0] - 16) && (nowGx < XYposition[i][0] + 16) &&
				(nowGy > XYposition[i][1] - 16) && (nowGy < XYposition[i][1] + 16))
			{
				//判断是否已产生
				if (XYposition[i][3] == 0)
				{
					//配置ACTION
					for (int j = 0; j < 30; j++)
					{
						if (WaterAct3[j] == NULL)
						{
							if (XYposition[i][5] == 2)
								WaterAct3[j] = createWaterAnimation(XYposition[i][2], XYposition[i][0], XYposition[i][1], 0);
							else
								WaterAct3[j] = createWaterAnimation(XYposition[i][2], XYposition[i][0], XYposition[i][1], 30);
							XYposition[i][3] = 1;
							XYposition[i][4] = j;
							break;
						}
					}
				}
				else
				{
					//做定点动画的漂浮
					XYposition[i][6]++;
					//时间固定
					if (XYposition[i][6] > 65)
					{
						XYposition[i][6] = 0;
						//(XYposition[i][5]== 1 ) ? 0 : 1;
						if (XYposition[i][5] == 1)
							XYposition[i][5] = 0;
						else if (XYposition[i][5] == 0)
							XYposition[i][5] = 1;
					}
					if (XYposition[i][5] == 1)
					{
						WaterAct3[XYposition[i][4]]->mx = WaterAct3[XYposition[i][4]]->mx + float(0.5);
						WaterAct3[XYposition[i][4]]->my = WaterAct3[XYposition[i][4]]->my - float(0.5);
					}
					else if (XYposition[i][5] == 0)
					{
						WaterAct3[XYposition[i][4]]->mx = WaterAct3[XYposition[i][4]]->mx - float(0.5);
						WaterAct3[XYposition[i][4]]->my = WaterAct3[XYposition[i][4]]->my + float(0.5);
					}
				}
			}
			// 合并
			if (XYposition[i][3] == 1)
			{
				if ((nowGx < XYposition[i][0] - 16) || (nowGx > XYposition[i][0] + 16) ||
					(nowGy < XYposition[i][1] - 16) || (nowGy > XYposition[i][1] + 16))
				{
					if (WaterAct3[XYposition[i][4]] != NULL)
					{
						DeathAction(WaterAct3[XYposition[i][4]]);
						WaterAct3[XYposition[i][4]] = NULL;
						XYposition[i][3] = 0;
					}
				}
			}
		}
	}
	else {
		if (in817flag == true) {
			for (i = 0; i < 560; i++) {
				if (XYposition[i][3] == 1) {
					if (WaterAct3[XYposition[i][4]] != NULL) {
						DeathAction(WaterAct3[XYposition[i][4]]);
						WaterAct3[XYposition[i][4]] = NULL;
						XYposition[i][3] = 0;
					}
				}
			}
			in817flag = false;
		}
	}
#endif

#ifdef _AniRandom   // Syu ADD 随机产生环境动画
	if ((nowFloor == 817) || (nowFloor == 8007) || (nowFloor == 8100) || (nowFloor == 8101) ||
		(nowFloor == 8029) || (nowFloor == 8028) || (nowFloor == 8027) || (nowFloor == 8015) || (nowFloor == 8113) || (nowFloor == 8114))  //判断地图
	{
		AniFlag = true;
		for (i = 0; i < 50; i++)
		{
			if (Timeflag[i] == false)
			{
				//随机결정产生与否，不论与否都产生等待时间
				Createflag = RAND(0, 3);
				if (Createflag > 0)
				{
					if (WaterAct[i] == NULL)
					{
						//产生范围在人物 30 * 30 范围中
						x1 = RAND(nowGx - 15, nowGx + 15);
						y1 = RAND(nowGy - 15, nowGy + 15);
						RandAnimate = RAND(0, 6);
						if (RandAnimate > 2)
							RandAnimate = RAND(101502, 101509);
						else
							RandAnimate = RAND(101524, 101529);
						WaterAct[i] = createWaterAnimation(RandAnimate, x1, y1, 103);
						Timeflag[i] = true;
						TimeTickMax[i] = RAND(100, 400);
					}
				}
				else
				{
					Timeflag[i] = true;
					TimeTickMax[i] = RAND(300, 600);
				}
			}
		}
		//活着的环境动画计时
		for (i = 0; i < 50; i++)
		{
			if (Timeflag[i] == true)
				TimeTick[i]++;
			if (TimeTick[i] > TimeTickMax[i])
			{
				TimeTick[i] = -100;
				Timeflag[i] = false;
				DeathAction(WaterAct[i]);
				WaterAct[i] = NULL;
			}
		}
	}
	//消灭动画
	else if (((nowFloor != 817) && (nowFloor != 8007) && (nowFloor != 8100) && (nowFloor != 8101) && (nowFloor != 8113) && (nowFloor != 8114) &&
		(nowFloor != 8027) && (nowFloor != 8028) && (nowFloor != 8029) && (nowFloor != 8015)) && (AniFlag == true))
	{
		AniFlag = false;
		for (i = 0; i < 50; i++)
		{
			if (WaterAct[i] != NULL)
			{
				DeathAction(WaterAct[i]);
				WaterAct[i] = NULL;
			}
		}
	}
#endif
	int 临时图标X, 临时图标Y;					//[임시 아이콘X, 임시 아이콘Y]
	if (ResoMode == 分辨率::mode800) {			//[해상도]
		临时图标X = 785;						//[임시 아이콘]
		临时图标Y = 505;						//[임시 아이콘]
	}
	else if (ResoMode == 分辨率::mode1024) {	//[해상도]
		临时图标X = 1009;						//[임시 아이콘]
		临时图标Y = 673;						//[임시 아이콘]
	}
	else {
		临时图标X = 625;						//[임시 아이콘]
		临时图标Y = 385;						//[임시 아이콘]
	}
	if (drawFieldButtonFlag) {
#ifdef _NB_SA_LIAOTIAN
		static int 聊天状态1 = 0;						//[채팅 상태 1]
		static int 聊天状态2 = 0;						//[채팅 상태1]
		static int 聊天状态3 = 0;						//[채팅상태 2]
		char 聊天提示[128];							//[채팅 힌트]
		int 聊天临时;								//[채팅 시간]
		/*聊天临时 = StockDispBuffer(临时图标X, 临时图标Y, DISP_PRIO_IME3, 52001, 2);			//[임시 아이콘X, 임시 아이콘]
		if( HitDispNo == 聊天临时){
			sprintf(聊天提示,"聊天信息上拉,当前聊天信息位置%d！",NowChatLine);				//[채팅 힌트, " 채팅 메시지 올리기, 현재 채팅 메시지 위
			ShowBottomLineString(FONT_PAL_WHITE, 聊天提示);				//[채팅 힌트]
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				if(NowChatLine > NowMaxChatLine){
					NowChatLine--;
					if (NowChatLine <= 0){
						NowChatLine = 1;
					}
				}
				play_se(203, 320, 240);
			}
		}*/

		聊天临时 = StockDispBuffer(临时图标X, 临时图标Y + 25, DISP_PRIO_IME3, 52002, 2);
		if (HitDispNo == 聊天临时) {
			sprintf(聊天提示, "채팅 정보 드롭 다운, 현재 채팅 정보 위치%d！", NowChatLine);
			ShowBottomLineString(FONT_PAL_WHITE, 聊天提示);
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				if (*ChatBuffer[NowChatLine].buffer) {
					NowChatLine++;
					if (NowChatLine > MAX_CHAT_LINE)
						NowChatLine = MAX_CHAT_LINE;
				}
				play_se(203, 320, 240);
			}
		}

		聊天临时 = StockDispBuffer(临时图标X, 临时图标Y + 50, DISP_PRIO_IME3, 52003, 2);
		if (HitDispNo == 聊天临时) {
			sprintf(聊天提示, "채팅 정보 드롭 다운, 현재 채팅 정보 위치%d！", NowChatLine);
			ShowBottomLineString(FONT_PAL_WHITE, 聊天提示);
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				extern int NowChatLine_Bak;
				NowChatLine = NowChatLine_Bak;
				play_se(203, 320, 240);
			}
		}
#endif

		/*#ifdef _NB_管理模式				//[#ifdef _NB_관리모드]
		VMProtectBegin("GmMake");
				int GM管理项[6];
				if (GM管理开关){
					GM管理项[0] = StockDispBuffer(临时图标X-145, leftUpPanelY+20, DISP_PRIO_MENU, IMAGE_收纳按钮合, 2);
				}else{
					GM管理项[0] = StockDispBuffer(临时图标X-145, leftUpPanelY+20, DISP_PRIO_MENU, IMAGE_收纳按钮分, 2);
				}
				if( HitDispNo == GM管理项[0]){
					ShowBottomLineString(11, "비밀기능！");
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						GM管理开关 = !GM管理开关;
					}
					HitDispNo = -1;
				}
				if (GM管理开关){
					GM管理项[1] = StockFontBuffer( leftUpPanelX + 120, leftUpPanelY +80, FONT_PRIO_FRONT, 1, "클라이언트새로고침LUA",2);
					GM管理项[2] = StockFontBuffer( leftUpPanelX + 120, leftUpPanelY +100, FONT_PRIO_FRONT, 1, "모두삭제GM권위，자신을다음과같이설정GM",2);
					GM管理项[3] = StockFontBuffer( leftUpPanelX + 120, leftUpPanelY +120, FONT_PRIO_FRONT, 1, "삭제하지않기GM권위，자신을다음과같이설정GM",2);
					GM管理项[4] = StockFontBuffer( leftUpPanelX + 120, leftUpPanelY +140, FONT_PRIO_FRONT, 1, "대면정보얻기",2);
					GM管理项[5] = StockFontBuffer( leftUpPanelX + 120, leftUpPanelY +160, FONT_PRIO_FRONT, 1, "얼굴가져오기NPC통로",2);
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						if( HitFontNo == GM管理项[1]){
							ReLoadStoneAgeLUA(_LUA_PATCH_);
							StockChatBufferLine("클라이언트재읽기LUA수행！", 4);
							GM管理开关 = false;
							HitFontNo = -1;
						}
						if( HitFontNo == GM管理项[2]){
							lssproto_Gmset_send(sockfd,1,"New Version");
							StockChatBufferLine("모두삭제GM권위，자신을다음과같이설정GM！", 4);
							GM管理开关 = false;
							HitFontNo = -1;
						}
						if( HitFontNo == GM管理项[3]){
							lssproto_Gmset_send(sockfd,2,"New Version");
							StockChatBufferLine("삭제하지않기GM권위，자신을다음과같이설정GM！", 4);
							GM管理开关 = false;
							HitFontNo = -1;
						}
						if( HitFontNo == GM管理项[4]){
							lssproto_Gmset_send(sockfd,3,"");
							GM管理开关 = false;
							HitFontNo = -1;
						}
						if( HitFontNo == GM管理项[5]){
							lssproto_Gmset_send(sockfd,4,"");
							GM管理开关 = false;
							HitFontNo = -1;DISP_PRIO_IME3
						}
					}
					StockDispBuffer(leftUpPanelX+300, leftUpPanelY+200, DISP_PRIO_MENU, 26258, 1);
				}
		VMProtectEnd();
		#endif*/

#ifdef _NB_顶部详情
		char moji[128];
		char 顶部信息提示[128];
		//	if (左菜单开关){
			//	顶部新按钮索引[0] = StockDispBuffer(leftUpPanelX+20, leftUpPanelY+20, DISP_PRIO_MENU, IMAGE_收纳按钮合, 2);
		StockDispBuffer(leftUpPanelX + 148, leftUpPanelY + 27, DISP_PRIO_MENU, IMAGE_LEFTMENU, 0);
		/*	}else{
				顶部新按钮索引[0] = StockDispBuffer(leftUpPanelX+20, leftUpPanelY+20, DISP_PRIO_MENU, IMAGE_收纳按钮分, 2);
			}
			if (顶部新按钮索引[0]){
				if( HitDispNo == 顶部新按钮索引[0]){
					ShowBottomLineString(11, "기능옵션。");
					//sprintf(顶部信息提示,"功能选项！");
					//NewShowBottomLineString(11, 顶部信息提示);
					if( mouse.onceState & MOUSE_LEFT_CRICK ){
						左菜单开关 = !左菜单开关;
					}
					if (!左菜单开关){
						for (i = 0; i < FIELD_FUNC_END; i++){
							fieldBtnHitId[i] = -2;
						}
					}
					HitDispNo = -1;
				}
			}
			if (左菜单开关){ */
			/*char mapname[128];
			sprintf(mapname, "[%s]동:%d 남:%d", nowFloorName , nowGx, nowGy);
			StockFontBuffer(leftUpPanelX + 380, leftUpPanelY + 580, FONT_PRIO_FRONT, 4, mapname, 0);*/

		fieldBtnHitId[FIELD_FUNC_MENU] = StockDispBuffer(leftUpPanelX + 52, leftUpPanelY + 27, DISP_PRIO_IME3, menuBtnGraNo[menuBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_MENU]) {
			ShowBottomLineString(0, "메뉴를 설정합니다.");
		}
		fieldBtnHitId[FIELD_FUNC_CARD] = StockDispBuffer(leftUpPanelX + 52, leftUpPanelY + 27, DISP_PRIO_IME3, cardBtnGraNo[cardBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_CARD]) {
			ShowBottomLineString(0, "상대방과 명함을 교환합니다.");
		}
		fieldBtnHitId[FIELD_FUNC_PARTY] = StockDispBuffer(leftUpPanelX + 52, leftUpPanelY + 27, DISP_PRIO_IME3, partyBtnGraNo[partyBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_PARTY]) {
			if (partyBtnEnableFlag == 0) {
				ShowBottomLineString(6, "파티에 참여할 수 없어요.");
			}
			else {
				ShowBottomLineString(0, "상대방의 파티에 참여합니다.");
			}
		}
		fieldBtnHitId[FIELD_FUNC_TRADE] = StockDispBuffer(leftUpPanelX + 114, leftUpPanelY + 17, DISP_PRIO_IME3, tradeBtnGraNo[tradeBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_TRADE]) {
			ShowBottomLineString(0, "상대방에게 교환을 신청합니다.");
		}
		
		/*fieldBtnHitId[FIELD_FUNC_STREET_VENDOR] = StockDispBuffer(leftUpPanelX + 175, leftUpPanelY + 18, DISP_PRIO_IME3, sStreetVendorBtnGraNo[pc.iOnStreetVendor], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_STREET_VENDOR]) {
			ShowBottomLineString(0, "개인상점을 개설합니다.");
		}*/
		fieldBtnHitId[FIELD_FUNC_WARP_] = StockDispBuffer(leftUpPanelX + 172, leftUpPanelY + 50, DISP_PRIO_IME3, 66750, 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_WARP_]) {
			ShowBottomLineString(0, "편리하게 위치를 이동할수있는 워프도우미입니다");
		}
		fieldBtnHitId[FIELD_FUNC_VIPSHOP] = StockDispBuffer(leftUpPanelX + 175, leftUpPanelY + 18, DISP_PRIO_IME3, sStreetVendorBtnGraNo[pc.iOnStreetVendor], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_VIPSHOP]) {
			ShowBottomLineString(0, "CHAOS 서버 VIP상점입니다");
		}

		/*fieldBtnHitId[FIELD_FUNC_HELP] = StockDispBuffer(leftUpPanelX + 230, leftUpPanelY + 33, DISP_PRIO_IME3, helpBtnGraNo[helpBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_HELP]) {
			ShowBottomLineString(5, "퀘스트 정보를 확인합니다.");
		}*/

		/*fieldBtnHitId[FIELD_FUNC_CHANNEL] = StockDispBuffer(leftUpPanelX + 115, leftUpPanelY + 27, DISP_PRIO_IME3, channelBtnGraNo[channelBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_CHANNEL]) {
			ShowBottomLineString(0, "부족에 대한 정보를 확인합니다.");
		}*/

		StockDispBuffer(rightUpPanelX + 55, rightUpPanelY + 31, DISP_PRIO_MENU, CG_FIELD_MENU_RIGHT, 0);
		StockDispBuffer(rightUpPanelX + 106 - amPmAnimeX, rightUpPanelY + 19, DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex0], 0);
		StockDispBuffer(rightUpPanelX + 106 + 64 - amPmAnimeX, rightUpPanelY + 19, DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex1], 0);

		fieldBtnHitId[FIELD_FUNC_DUEL] = StockDispBuffer(rightUpPanelX + 38, rightUpPanelY + 31, DISP_PRIO_IME3, duelBtnGraNo[duelBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_DUEL]) {
			ShowBottomLineString(0, "상대방에게 대전을 신청합니다.");
		}
		fieldBtnHitId[FIELD_FUNC_JOIN_BATTLE] = StockDispBuffer(rightUpPanelX + 38, rightUpPanelY + 31, DISP_PRIO_IME3, joinBattleBtnGraNo[joinBattleBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_JOIN_BATTLE]) {
			if (joinBattleHelpMsgFlag == 1) {
				ShowBottomLineString(0, "전투에 참여합니다.");
			}
			else if (joinBattleHelpMsgFlag == 2) {
				ShowBottomLineString(6, "전투에 참여할 수 없어요.");
			}
			else {
				ShowBottomLineString(0, "전투를 관전합니다.");
			}
		}
		fieldBtnHitId[FIELD_FUNC_HELP] = StockDispBuffer(rightUpPanelX + 115, rightUpPanelY + 32, DISP_PRIO_IME3, helpBtnGraNo[helpBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_HELP]) {
			ShowBottomLineString(5, "퀘스트 정보를 확인합니다.");
		}

		fieldBtnHitId[FIELD_FUNC_ACT] = StockDispBuffer(rightUpPanelX + 68, rightUpPanelY + 30, DISP_PRIO_IME3, actBtnGraNo[actBtn], 2);
		if (HitDispNo == fieldBtnHitId[FIELD_FUNC_ACT]) {
			ShowBottomLineString(0, "캐릭터 액션 정보를 확인합니다.");
		}

		/*顶部新按钮索引[1] = StockDispBuffer(leftUpPanelX+20, leftUpPanelY + 220, DISP_PRIO_IME3,IMAGE_按钮观战, 2);
		if( HitDispNo == 顶部新按钮索引[1]){
			ShowBottomLineString(11, "관전。");
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				lssproto_SaMenu_send(sockfd,3, 4, 0);
				play_se(217, 320, 240);
			}
		}*/

		/*顶部新按钮索引[2] = StockDispBuffer(rightUpPanelX-400, rightUpPanelY + 19, DISP_PRIO_IME3, 60012, 2);
		if( HitDispNo == 顶部新按钮索引[2]){
			ShowBottomLineString(4, "지원 포인트 샵을 열람합니다.");
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				lssproto_SaMenu_send(sockfd,3, 2, 0);
				play_se(217, 320, 240);
			}
		}*/

		顶部新按钮索引[3] = StockDispBuffer(leftUpPanelX + 143, leftUpPanelY + 18, DISP_PRIO_IME3, ridelistBtnGraNo[ridelistBtn], 2);
		if (HitDispNo == 顶部新按钮索引[3]) {
			ShowBottomLineString(0, "캐릭터의 탑승 페트를 확인합니다.");
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				lssproto_SaMenu_send(sockfd, 3, 1, 0);
				play_se(217, 320, 240);
			}
		}
		顶部新按钮索引[4] = StockDispBuffer(leftUpPanelX + 632, leftUpPanelY + 18, DISP_PRIO_IME3, 60011, 2);
		if (HitDispNo == 顶部新按钮索引[4]) {
			ShowBottomLineString(0, "해당지역에 있는 파티를 검색합니다.");
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				lssproto_Partyauto_send(sockfd);
				play_se(217, 320, 240);
			}
		}
		//顶部新按钮索引[5] = StockDispBuffer(leftUpPanelX + 340, leftUpPanelY + 20, DISP_PRIO_IME3, 56040, 2);
		/*if (HitDispNo == 顶部新按钮索引[5]) {
			ShowBottomLineString(0, "경험치 구슬을 사용하거나 사용을 종료합니다.");
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				lssproto_Expup_send(sockfd);
				play_se(217, 320, 240);
			}
		}*/
		/*顶部新按钮索引[6] = StockDispBuffer(leftUpPanelX + 450, leftUpPanelY + 20, DISP_PRIO_IME3, 56039, 2);
		if (HitDispNo == 顶部新按钮索引[6]) {
			ShowBottomLineString(0, "벽청의 소라를 사용하거나 사용을 종료합니다.");
			if (mouse.onceState & MOUSE_LEFT_CRICK) {
				lssproto_Noenemy_send(sockfd);
				play_se(217, 320, 240);
			}
		}*/

		/*顶部新按钮索引[4] = StockDispBuffer(leftUpPanelX+340, leftUpPanelY+20, DISP_PRIO_IME3, IMAGE_按钮信箱, 2);//地图
		if( HitDispNo == 顶部新按钮索引[4]){
			ShowBottomLineString(11, "우편。");
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				lssproto_SaMenu_send(sockfd,3, 5, 0);
				play_se(217, 320, 240);
			}
		}*/
#ifdef _NB_红包系统
		/*顶部新按钮索引[5] = StockDispBuffer(leftUpPanelX+380, leftUpPanelY+20, DISP_PRIO_IME3, IMAGE_按钮红包, 2);//红包
		if( HitDispNo == 顶部新按钮索引[5]){
			ShowBottomLineString(11, "빨간봉투。");
			if( mouse.onceState & MOUSE_LEFT_CRICK ){
				lssproto_SaMenu_send(sockfd,3, 6, 0);
				play_se(217, 320, 240);
			}
		}*/
#endif
		/*	}else{
				extern int 状态显示开关;
				if (状态显示开关){
					sprintf( moji,"[캐릭터]%-s Lv:%d",pc.name, pc.level);
					StockFontBuffer( leftUpPanelX + 6, leftUpPanelY +60, FONT_PRIO_FRONT, 1, moji, 0 );//打印等级

					int 血量百分比 = (int)float(pc.hp)/float(pc.maxHp)*100;
					sprintf( moji,"HP:%d/%d(%d%%)",pc.hp,pc.maxHp,血量百分比);
					StockFontBuffer( leftUpPanelX + 6, leftUpPanelY + 76, FONT_PRIO_FRONT, 4, moji, 0 );
					//StockBoxDispBuffer( leftUpPanelX + 114, leftUpPanelY+27,leftUpPanelX+114 + 血量百分比, leftUpPanelY + 27 + 12, DISP_PRIO_IME3, SYSTEM_PAL_RED, 1 );

					int 气力百分比 = (int)float(pc.mp)/float(pc.maxMp)*100;
					sprintf( moji,"MP:%d/%d(%d%%)",pc.mp,pc.maxMp,气力百分比);
					StockFontBuffer( leftUpPanelX + 6, leftUpPanelY + 92, FONT_PRIO_FRONT, 4, moji, 0 );
					//StockBoxDispBuffer( leftUpPanelX + 114, leftUpPanelY+45,leftUpPanelX+114 + 气力百分比, leftUpPanelY + 45 + 12, DISP_PRIO_IME3, SYSTEM_PAL_AQUA, 1 );

					int 经验百分比 = (int)float(pc.exp)/float(pc.maxExp)*100;
					if (pc.maxExp == -1){
						sprintf( moji,"Exp:Max");
					}else{
						sprintf( moji,"Exp:%d%%", 经验百分比);
					}

					StockFontBuffer( leftUpPanelX + 6, leftUpPanelY + 108, FONT_PRIO_FRONT, 4, moji, 0 );
					//StockBoxDispBuffer( leftUpPanelX + 114, leftUpPanelY+63,leftUpPanelX+114 + 经验百分比, leftUpPanelY + 63 + 12, DISP_PRIO_IME3, SYSTEM_PAL_YELLOW, 1 );

					if (pc.battlePetNo >= 0){
						sprintf( moji,"[페트]%s Lv:%d",pet[pc.battlePetNo].name,pet[pc.battlePetNo].level);//打印战宠名字
						StockFontBuffer( leftUpPanelX+6, leftUpPanelY + 124, FONT_PRIO_FRONT, 1, moji, 0 );

						int 临时百分比 = (int)float(pet[pc.battlePetNo].hp)/float(pet[pc.battlePetNo].maxHp)*100;
						sprintf( moji,"HP:%d%/%d(%d%%)", pet[pc.battlePetNo].hp,pet[pc.battlePetNo].maxHp,临时百分比);
						StockFontBuffer( leftUpPanelX+6, leftUpPanelY + 140, FONT_PRIO_FRONT, 4, moji, 0 );
						if (pet[pc.battlePetNo].maxExp == -1){
							sprintf( moji,"Exp:Max");
						}else{
							临时百分比 = (int)float(pet[pc.battlePetNo].exp)/float(pet[pc.battlePetNo].maxExp)*100;
							sprintf( moji,"Exp:%d%%", 临时百分比);
						}
						StockFontBuffer( leftUpPanelX+6, leftUpPanelY + 156, FONT_PRIO_FRONT, 4, moji, 0 );
					}
					//打印骑宠信息
					if (pc.ridePetNo >= 0){
						sprintf( moji,"[탑승]%s Lv:%d",pet[pc.ridePetNo].name,pet[pc.ridePetNo].level);
						StockFontBuffer( leftUpPanelX+6, leftUpPanelY + 172, FONT_PRIO_FRONT, 1, moji, 0 );

						int 临时百分比 = (int)float(pet[pc.ridePetNo].hp)/float(pet[pc.ridePetNo].maxHp)*100;
						sprintf( moji,"HP:%d%/%d(%d%%)", pet[pc.ridePetNo].hp,pet[pc.ridePetNo].maxHp,临时百分比);
						StockFontBuffer( leftUpPanelX+6, leftUpPanelY + 188, FONT_PRIO_FRONT, 4, moji, 0 );

						if (pet[pc.ridePetNo].maxExp == -1){
							sprintf( moji,"Exp:Max");
						}else{
							临时百分比 = (int)float(pet[pc.ridePetNo].exp)/float(pet[pc.ridePetNo].maxExp)*100;
							sprintf( moji,"Exp:%d%%", 临时百分比);
						}
						StockFontBuffer( leftUpPanelX+6, leftUpPanelY + 204, FONT_PRIO_FRONT, 4, moji, 0 );
					}
				}


			}*/

			//오른쪽의 기능메뉴
		if (ResoMode == 分辨率::mode800) {
			临时图标X = 784;
			临时图标Y = 60;
		}
		else if (ResoMode == 分辨率::mode1024) {
			临时图标X = 1008;
			临时图标Y = 60;
		}
		else {
			临时图标X = 624;
			临时图标Y = 60;
		}
		//显示PING值
		char tmp[64];
		/*
		if( dwPingState & 0x80000000){
			wsprintf( tmp, "PING:%d", dwPingTime);
			dwPingState++;
			if( (dwPingState&0xff) > 40)
				dwPingState = 0;
		}else{
			wsprintf( tmp, "PING:%d", dwPingTime);
		}
		if (dwPingTime > 125){
			StockFontBuffer( 临时图标X-100, 临时图标Y -58, FONT_PRIO_FRONT, 4, tmp, 0 );
		}else if (dwPingTime > 180){
			StockFontBuffer( 临时图标X-100, 临时图标Y -58, FONT_PRIO_FRONT, 6, tmp, 0 );
		}else{
			StockFontBuffer( 临时图标X-100, 临时图标Y -58, FONT_PRIO_FRONT, 5, tmp, 0 );
		}*/
		//,nowGx,nowGy
		//wsprintf( tmp, "동:%d", nowGx);
		//StockFontBuffer(临时图标X-95, 临时图标Y-54, FONT_PRIO_FRONT, 1, tmp, 0);
		//wsprintf( tmp, "남:%d", nowGy);
		//StockFontBuffer(临时图标X-95, 临时图标Y-40, FONT_PRIO_FRONT, 1, tmp, 0);
		//wsprintf(tmp, "Time:%d", SaTime.hour * 24 / 1200);
		//StockFontBuffer( 临时图标X-100, 临时图标Y -40, FONT_PRIO_FRONT, 4, tmp, 0 );


		//시간표기
		//StockDispBuffer(临时图标X-54, 临时图标Y-38, DISP_PRIO_MENU, 52108, 0);
		//if (drawTimeAnimeFlag){지도에관계없이시간표시]
			//StockDispBuffer(rightUpPanelX + 106 - amPmAnimeX, rightUpPanelY + 20, DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex0], 0);
			//StockDispBuffer(rightUpPanelX + 106 + 64 - amPmAnimeX, rightUpPanelY + 20, DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex1], 0);

		//}else{
			//StockDispBuffer(rightUpPanelX + 68, rightUpPanelY + 31, DISP_PRIO_MENU, CG_FIELD_MENU_RIGHT_BACK, 0);
		//}

#else
#ifdef __PHONEMESSAGE
		// LeiBoy 2002 Jan.26 --- Cell Phone's Messages Button -- BEGIN
#ifdef _TABLEOFSKILL
#ifdef _ANGEL_SUMMON
#ifdef _TEACHER_SYSTEM
		StockDispBuffer(leftUpPanelX + 148, leftUpPanelY + 25, DISP_PRIO_MENU, CG_FIELD_TS_LEFT_MENU, 0);
#else
		StockDispBuffer(leftUpPanelX + 150, leftUpPanelY + 27, DISP_PRIO_MENU, CG_FIELD_MENU_LEFT_75, 0);
#endif
		if (angelLampDrawFlag)
			StockDispBuffer(leftUpPanelX + 64, leftUpPanelY + 42, DISP_PRIO_IME3, CG_FIELD_ANGEL_LAMP, 0);
#else
#ifdef _FRIENDCHANNEL
		StockDispBuffer(leftUpPanelX + 141, leftUpPanelY + 27, DISP_PRIO_MENU, CG_FIELD_CHANNEL_MENU, 0);
#else
		StockDispBuffer(leftUpPanelX + 141, leftUpPanelY + 27, DISP_PRIO_MENU, CG_FIELD_SKILL_MENU, 0);
#endif
#endif
#else
		StockDispBuffer(leftUpPanelX + 100, leftUpPanelY + 26, DISP_PRIO_MENU, CG_FIELD_MENU_LEFT, 0);
#endif
		// LeiBoy 2002 Jan.26 --- Cell Phone's Messages Button -- END
#else

		//#ifdef __FAMILY_UI_
		if (bNewServer)
			StockDispBuffer(leftUpPanelX + 140, leftUpPanelY + 27, DISP_PRIO_MENU, CG_FIELD_MENU_LEFT_NEW, 0);
		else
			//#else
			StockDispBuffer(leftUpPanelX + 26 + 52, leftUpPanelY + 28, DISP_PRIO_MENU, CG_FIELD_MENU_LEFT, 0);
		//#endif
#endif


		// 게임설정버튼
		fieldBtnHitId[FIELD_FUNC_MENU] = StockDispBuffer(leftUpPanelX + 52, leftUpPanelY + 25, DISP_PRIO_IME3, menuBtnGraNo[menuBtn], 2);
		if (menuBtnFocus)
			ShowBottomLineString(FONT_PAL_WHITE, "게임설정。");
		// 명함교환버튼
		fieldBtnHitId[FIELD_FUNC_CARD] = StockDispBuffer(leftUpPanelX + 52, leftUpPanelY + 25, DISP_PRIO_IME3, cardBtnGraNo[cardBtn], 2);
		if (cardBtnFocus)
			ShowBottomLineString(FONT_PAL_WHITE, "명함교환。");

		// 파티참여버튼
		fieldBtnHitId[FIELD_FUNC_PARTY] = StockDispBuffer(leftUpPanelX + 52, leftUpPanelY + 25, DISP_PRIO_IME3, partyBtnGraNo[partyBtn], 2);
		if (partyBtnFocus) {
			if (partyBtnEnableFlag == 0)
				ShowBottomLineString(FONT_PAL_WHITE, "파티합류불가。");
			else
				ShowBottomLineString(FONT_PAL_WHITE, "파티합류。");
		}
		// 거래하기버튼
		fieldBtnHitId[FIELD_FUNC_TRADE] = StockDispBuffer(leftUpPanelX + 104 + 10, leftUpPanelY + 25 - 10, DISP_PRIO_IME3, tradeBtnGraNo[tradeBtn], 2);
		if (tradeBtnFocus)
			ShowBottomLineString(FONT_PAL_WHITE, "거래。");

		// 가족기능버튼
		if (bNewServer) {
#ifndef _CHANNEL_MODIFY
			if (pc.channel != -1)
				channelBtn = 1;
			else
				channelBtn = 0;
#else
			if (pc.channel == 5)
				channelBtn = 1;
			else
				channelBtn = 0;
#endif
			fieldBtnHitId[FIELD_FUNC_CHANNEL] = StockDispBuffer(leftUpPanelX + 115, leftUpPanelY + 25, DISP_PRIO_IME3, channelBtnGraNo[channelBtn], 2);
			if (channelBtnFocus)
				ShowBottomLineString(FONT_PAL_WHITE, "가족기능。");
		}


#ifdef _STREET_VENDOR
		// 스톨기능버튼
		fieldBtnHitId[FIELD_FUNC_STREET_VENDOR] = StockDispBuffer(leftUpPanelX + 176, leftUpPanelY + 16, DISP_PRIO_IME3, sStreetVendorBtnGraNo[pc.iOnStreetVendor], 2);
		if (sStreetVendorBtnFocus)
			ShowBottomLineString(FONT_PAL_WHITE, "노점기능。");
#else
#ifdef _TABLEOFSKILL
		fieldBtnHitId[FIELD_FUNC_SKILL] = StockDispBuffer(leftUpPanelX + 108, leftUpPanelY + 26, DISP_PRIO_IME3, skillBtnGraNo[skillBtn], 2);
		if (skillBtnFocus)
			ShowBottomLineString(FONT_PAL_WHITE, "직업기능。");
#endif
#endif
#ifdef __PHONEMESSAGE
		// LeiBoy 2002 Jan.26 --- Cell Phone's Messages Button -- BEGIN
#ifdef _TABLEOFSKILL
		fieldBtnHitId[FIELD_FUNC_MSG] = StockDispBuffer(leftUpPanelX + 132, leftUpPanelY + 28, DISP_PRIO_IME3, msgBtnGraNo[msgBtn], 2);
#else
		fieldBtnHitId[FIELD_FUNC_MSG] = StockDispBuffer(leftUpPanelX + 101, leftUpPanelY + 28, DISP_PRIO_IME3, msgBtnGraNo[msgBtn], 2);
#endif
#endif

		if (mailLampDrawFlag)
			StockDispBuffer(leftUpPanelX + 52, leftUpPanelY + 26, DISP_PRIO_IME3, CG_FIELD_MAIL_LAMP, 0);

		StockDispBuffer(rightUpPanelX + 24, rightUpPanelY + 30, DISP_PRIO_MENU, CG_FIELD_MENU_RIGHT, 0);
		fieldBtnHitId[FIELD_FUNC_JOIN_BATTLE] = StockDispBuffer(rightUpPanelX + 38, rightUpPanelY + 30, DISP_PRIO_IME3, joinBattleBtnGraNo[joinBattleBtn], 2);
		if (joinBattleBtnFocus) {
			if (joinBattleHelpMsgFlag == 1)
				ShowBottomLineString(FONT_PAL_WHITE, "전투참여。");
			else if (joinBattleHelpMsgFlag == 2)
				ShowBottomLineString(FONT_PAL_WHITE, "전투참여불가。");
			else
				ShowBottomLineString(FONT_PAL_WHITE, "관전。");
		}

		fieldBtnHitId[FIELD_FUNC_DUEL] = StockDispBuffer(rightUpPanelX + 38, rightUpPanelY + 30, DISP_PRIO_IME3, duelBtnGraNo[duelBtn], 2);
		if (duelBtnFocus)
			ShowBottomLineString(FONT_PAL_WHITE, "듀얼신청。");

		fieldBtnHitId[FIELD_FUNC_HELP] = StockDispBuffer(rightUpPanelX + 115, rightUpPanelY + 31, DISP_PRIO_IME3, helpBtnGraNo[helpBtn], 2);
		if (helpBtnFocus)
			ShowBottomLineString(FONT_PAL_WHITE, "게임가이드。");

		fieldBtnHitId[FIELD_FUNC_ACT] = StockDispBuffer(rightUpPanelX + 68, rightUpPanelY + 30, DISP_PRIO_IME3, actBtnGraNo[actBtn], 2);
		if (actBtnForcus)
			ShowBottomLineString(FONT_PAL_WHITE, "캐릭터액션。");

		if (drawTimeAnimeFlag) {
			StockDispBuffer(rightUpPanelX + 106 - amPmAnimeX, rightUpPanelY + 19, DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex0], 0);
			StockDispBuffer(rightUpPanelX + 106 + 64 - amPmAnimeX, rightUpPanelY + 19, DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex1], 0);
		}
		else
			// ???????????????????????????
			StockDispBuffer(rightUpPanelX + 68, rightUpPanelY + 30, DISP_PRIO_MENU, CG_FIELD_MENU_RIGHT_BACK, 0);

#ifdef _ANGEL_SUMMON
		/*
		fieldBtnHitId[FIELD_FUNC_ANGEL] = StockDispBuffer(leftUpPanelX + 207, leftUpPanelY + 19, DISP_PRIO_IME3, angelBtnGraNo[angelBtn], 2);
		if (angelBtnFocus)
			ShowBottomLineString(FONT_PAL_WHITE, "精灵的召唤。");
		*/
#endif
#ifdef _TEACHER_SYSTEM//原为235
		/*
		fieldBtnHitId[FIELD_FUNC_TEACHER] = StockDispBuffer(leftUpPanelX + 238, leftUpPanelY + 20, DISP_PRIO_IME3, iTeacherSystemBtnGraNo[sTeacherSystemBtn > 0 ? 1:0], 2);
		if (sTeacherSystemBtnFocus)
			ShowBottomLineString(FONT_PAL_WHITE, "导师系统。");
		*/
#endif


#endif
	}
	else {
		for (i = 0; i < FIELD_FUNC_END; i++)
			fieldBtnHitId[i] = -2;
	}

}

BOOL checkFieldMenuFlag(void) {
	return nowFieldMenuOpenFlag;
}

void initCharActionAnimeChange(void)
{
	charActionAnimeChangeProcNo = 0;
}

// ???????
void closeCharActionAnimeChange(void)
{
	charActionAnimeChangeProcNo = 1000;
}

// ????????
//  ??: 0 ... ???
//          !0 ... ????
int charActionAnimeChange(void)
{
	static ACTION* ptActMenuWin = NULL;
	static int x, y, w, h;
	static int btnId[13];
	int id = 0;
	int focusId = 0;
	char* msg[] =
	{
		" 앉　　다 ",
		" 일어나다 ",
		" 손흔들다 ",
		" 걷　　다 ",
		" 인사하다 ",
		" 쓰러지다 ",
		" 기뻐하다 ",
		" 공격하다 ",
		" 화를내다 ",
		" 방어하다 ",
		" 슬퍼하다 ",
		"데　미　지",
		"던　지　다"
#ifdef __EMOTION
		," 잠자는행동 "
		," 콜드액션 "
#endif
	};
	char* msg2[] =
	{
		"제자리에 앉는 액션을 취합니다. (Ctrl+1)",
		"일어서는 액션을 취합니다. (Ctrl+7)",
		"손을 흔드는 액션을 취합니다. (Ctrl+2)",
		"걷는 액션을 취합니다. (Ctrl+8)",
		"인사하는 액션을 취합니다. (Ctrl+3)",
		"쓰러지는 액션을 취합니다. (Ctrl+9)",
		"기뻐하는 액션을 취합니다. (Ctrl+4)",
		"공격하는 액션을 취합니다. (Ctrl+0)",
		"화를 내는 액션을 취합니다. (Ctrl+5)",
		"방어하는 액션을 취합니다. (Ctrl+-)",
		"슬퍼하는 액션을 취합니다. (Ctrl+6)",
		"타격을 입는 액션을 취합니다. (Ctrl+=)",
		"던지는 액션을 취합니다. (Ctrl+\\)"
#ifdef __EMOTION
		,"잠자는행동。"
		,"콜드액션。"
#endif
	};
	int i, j;
	int x1, y1, x2, y2;
	int chgTbl[] =
	{
	   5,	//  ?  ? 
	   3,	//  ?  ? 
	   6,	// ??
	   4,	//    ? 
	  11,	// ????
	   2,	//  ?? 
	   7,	//  ?  ? 
	   0,	//  ?  ? 
	   8,	//    ? 
	  10,	//  ??? 
	   9,	//  ?? 
	   1,	// ????
	  12	// ???
	};
#ifdef __EMOTION
	int emotion[] = { SPR_sleep, SPR_stone };
#endif

	if (charActionAnimeChangeProcNo == 0)
	{
		for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
			btnId[i] = -2;

		// ??????
		w = 3;
		h = 6;
		x = 440;
		y = 16;
		ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
#ifdef _NEW_RESOMODE  //800 600模式
		x = ptActMenuWin->x;
		y = ptActMenuWin->y;
#endif
		charActionAnimeChangeProcNo++;
	}

	if (ptActMenuWin != NULL)
	{
		id = -1;
		focusId = -1;
		if (ptActMenuWin->hp >= 1)
		{
			// ????
			id = selGraId(btnId, sizeof(btnId) / sizeof(int));
			focusId = focusGraId(btnId, sizeof(btnId) / sizeof(int));

#ifdef __EMOTION
			for (i = 0; i < 7; i++)
#else
			for (i = 0; i < 6; i++)
#endif
			{
				for (j = 0; j < 2; j++)
				{
					x1 = x + 84 * j + 18;
					y1 = y + i * 30 + 21;
					x2 = x1 + 73;
					y2 = y1 + 22;
					if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
					{
						if (mouse.onceState & MOUSE_LEFT_CRICK)
							id = i * 2 + j + 1;
						focusId = i * 2 + j + 1;
					}
				}
			}

#ifdef __EMOTION
			i = 7;
#else
			i = 6;
#endif
			j = 0;
			x1 = x + 42 + 18;
			y1 = y + i * 30 + 21;
			x2 = x1 + 73;
			y2 = y1 + 22;
			if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
			{
				if (mouse.onceState & MOUSE_LEFT_CRICK)
					id = i * 2 + j + 1;
				focusId = i * 2 + j + 1;
			}
		}

		// ?????????????????????
		if (CheckMenuFlag())
			id = 100;
		else if ((joy_trg[0] & JOY_ESC) && GetImeString() == NULL)	// ESC????????
		{
			id = 100;
			play_se(203, 320, 240);	// ????????
		}
		else if (charActionAnimeChangeProcNo == 1000)	// ???????????
			id = 100;

		// ?????
		if (id == 0)
			play_se(203, 320, 240);	// ????????
		else if (1 <= id && id <= 13)
		{
			id--;
			if (eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0)
			{
				if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime())// ??
				{
					if (bNewServer)
						lssproto_AC_send(sockfd, nowGx, nowGy, chgTbl[id]);
					else
						old_lssproto_AC_send(sockfd, nowGx, nowGy, chgTbl[id]);
					setPcAction(chgTbl[id]);
					fieldBtnPushTime = TimeGetTime();
				}
			}
			// ?????????????????????????
			id = -1;
			play_se(217, 320, 240);	// ?????
		}
#ifdef __EMOTION
		if (14 <= id && id <= 15)
		{
			id -= 14;
			setPcEmotion(emotion[id]);
			id = -1;
			play_se(217, 320, 240);	// ?????
		}
#endif
		if (id >= 0)
		{
			DeathAction(ptActMenuWin);
			ptActMenuWin = NULL;
			return 1;
		}
		if (ptActMenuWin->hp >= 1)
		{
			btnId[0] = StockDispBuffer(x + 96, y + 258, DISP_PRIO_MENU, CG_CLOSE_BTN, 2);
			if (focusId == 0)
				ShowBottomLineString(FONT_PAL_WHITE, "작업창닫기");
#ifdef __EMOTION
			for (i = 0; i < 7; i++)
#else
			for (i = 0; i < 6; i++)
#endif
			{
				for (j = 0; j < 2; j++)
				{
					StockFontBuffer(x + 84 * j + 20, y + i * 30 + 24, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i * 2 + j], 0);
					if ((i * 2 + j) == (focusId - 1))
						ShowBottomLineString(FONT_PAL_WHITE, msg2[i * 2 + j]);
				}
			}
#ifdef __EMOTION
			i = 7;
#else
			i = 6;
#endif
			j = 0;
			StockFontBuffer(x + 42 + 20, y + i * 30 + 24, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i * 2 + j], 0);
			if ((i * 2 + j) == (focusId - 1))
				ShowBottomLineString(FONT_PAL_WHITE, msg2[i * 2 + j]);
		}
	}

	return 0;
}

// ????ＯＫ?????????
// ???
void initEtcSwitch(void)
{
	etcSwitchProcNo = 0;
}

// ???????
void closeEtcSwitch(void)
{
	etcSwitchProcNo = 1000;
}

// ????ＯＫ??????
//  ??: 0 ... ???
//          !0 ... ????
#ifdef _CHANNEL_MODIFY
void SaveChatData(char* msg, char KindOfChannel, bool bCloseFile);
#endif

short onlinegmProcNo;

#ifdef _NB_SetResoMode
void JL_RESOMODE(int mode);
void SetResoMode(int Mode);
void ChangeWindowMode(void);
void RecoverDirectDraw(void);
#endif

int etcSwitch(void) {
	static ACTION* ptActMenuWin = NULL;
#ifdef _CHANNEL_MODIFY
	static ACTION* pActChannelWin = NULL;
	static int iChannelbtnId[4];
	FILE* pf = NULL;
#endif
	static int x, y, w, h;
#ifdef _AUCPROTOCOL				// (不可开) Syu ADD 拍卖频道开关Protocol
	static int btnId[6];
#else
	static int btnId[5];
#endif
	int id = 0;
	int focusId = 0;
	char* msg[] =
	{
		"파    티：",
		"듀    얼：",
		"명함교환：",
#ifndef _CHANNEL_MODIFY
		"채팅하기：",
#else
		"채    널    버    튼",
#endif
		"거    래：",
#ifdef _AUCPROTOCOL				// (不可开) Syu ADD 拍卖频道开关Protocol
		"경　　매：",
#endif
#ifdef _NB_SetResoMode
		//"해 상 도：",//5
#endif
	};

	char* msg2[] =
	{
		" Ｎ  Ｏ",
		" ＹＥＳ",
		//" Other"
	};
#ifndef _CHANNEL_MODIFY
	char* msg3[] =
	{
		" 전직원",
		" 5  팀"
	};
#endif
	char* msg4[] =
	{
		"다른사람팀합류여부설정。",
		"다른사람도전받을지설정。",
		"명함교환허용여부설정。",
#ifndef _CHANNEL_MODIFY
		"채팅내용을들읏수있는범위설정。",
#else
		"각채널설정또는해제。",
#endif
		"거래수락여부설정。"
#ifdef _AUCPROTOCOL				// (不可开) Syu ADD 拍卖频道开关Protocol
		,
		"경매채널메시지수신여부。"
#endif
#ifdef _NB_SetResoMode
		,"게임인터페이스해상도설정。"//5
#endif

	};

	int mask[] =
	{
		PC_ETCFLAG_PARTY,
		PC_ETCFLAG_DUEL,
		PC_ETCFLAG_MAIL,
#ifndef _CHANNEL_MODIFY
		PC_ETCFLAG_CHAT_MODE,
#else
		0,
#endif
		PC_ETCFLAG_TRADE
#ifdef _AUCPROTOCOL				// (不可开) Syu ADD 拍卖频道开关Protocol
		,PC_ETCFLAG_AUC
#endif

	};
#ifdef _CHANNEL_MODIFY
	char* pszChannel[] = {
		"귓속말",
		"가족말",
		"경매채널",
		"대화저장"
	};
	int iChannelSwitch[] = {
		PC_ETCFLAG_CHAT_TELL,
		PC_ETCFLAG_CHAT_FM,
		PC_ETCFLAG_CHAT_OCC,
		PC_ETCFLAG_CHAT_SAVE
	};
#endif

	/*#ifdef _JOBDAILY
		char *msg5[] = {
			"종  료",
			"이전페이지",
			"다음페이지"
		};
	#endif*/

	int i;
	int x1, y1, x2, y2;

	if (etcSwitchProcNo == 0)
	{
		for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
			btnId[i] = -2;

		// ??????
		w = 3;
		h = 5;
		x = 5;
		//x = 440;
		//y = 72;
		y = 5;
		ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1); //ptActMenuWin = MakeWindowDisp(x, y, w, h+1, NULL, 1);
#ifdef _NEW_RESOMODE  //800 600模式
		x = ptActMenuWin->x;
		y = ptActMenuWin->y;
#endif
		etcSwitchProcNo++;
	}
#ifdef _CHANNEL_MODIFY
	// [채널 윈도우 스위치]
	if (etcSwitchProcNo == 100)
	{
		w = 3;
		h = 4;
		x = 16;
		y = 16;
		pActChannelWin = MakeWindowDisp(x, y, w, h, NULL, 1);
		etcSwitchProcNo++;
	}

	if (pActChannelWin != NULL)
	{
		id = -1;
		focusId = -1;
		if (pActChannelWin->hp >= 1)
		{
			id = selGraId(iChannelbtnId, sizeof(iChannelbtnId) / sizeof(int));
			focusId = focusGraId(iChannelbtnId, sizeof(iChannelbtnId) / sizeof(int));
			for (i = 0; i < sizeof(pszChannel) / sizeof(char*); i++)
			{
				x1 = x + 18;
				y1 = y + i * 30 + 23;
				x2 = x1 + 157;
				y2 = y1 + 22;
				if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
				{
					if (mouse.onceState & MOUSE_LEFT_CRICK)
						id = i + 1;
					focusId = i + 1;
				}
			}

			if (CheckMenuFlag())
				id = 100;
			else if (((joy_trg[0] & JOY_ESC) && GetImeString() == NULL))
			{
				id = 100;
				play_se(203, 320, 240);
			}
			else if (etcSwitchProcNo == 1000)
				id = 100;

			if (id == 0)
				play_se(203, 320, 240);
			else if (1 <= id && id <= sizeof(msg) / sizeof(char*))
			{
				id--;
				if (pc.etcFlag & iChannelSwitch[id])
				{
					pc.etcFlag &= ~iChannelSwitch[id];
					TalkMode = 0;
				}
				else
					pc.etcFlag |= iChannelSwitch[id];
				switch (id)
				{
				case 1:
					if (pc.familyleader <= 0)
					{
						StockChatBufferLine("가족에가입하지않았습니다", FONT_PAL_RED);
						pc.etcFlag &= ~iChannelSwitch[id];
					}
					break;
					//2.5版去除职业相关功能

				case 2:
					if (pc.profession_class == 0)
					{
						StockChatBufferLine("고용되지않았습니다", FONT_PAL_RED);
						pc.etcFlag &= ~iChannelSwitch[id];
					}
					break;
				case 3:
					if (!(pc.etcFlag & iChannelSwitch[id]))
						SaveChatData(NULL, 0, true);
					break;
				}
				if (eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0)
				{
					if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime())
					{
						if (bNewServer)
							lssproto_FS_send(sockfd, pc.etcFlag);
						else
							old_lssproto_FS_send(sockfd, pc.etcFlag);

						fieldBtnPushTime = TimeGetTime();
					}
					else
						etcSwitchChangeFlag = 1;
				}
				id = -1;
				play_se(217, 320, 240);
			}
			if (id >= 0) {
				DeathAction(pActChannelWin);
				pActChannelWin = NULL;
				if ((pf = fopen("data\\channel.dat", "w+")) == NULL)
					StockChatBufferLine("채널 설정 파일 읽기 실패", FONT_PAL_RED);
				else {
					char buf[3];

					buf[0] = (pc.etcFlag & PC_ETCFLAG_CHAT_TELL) ? 1 : 0;
					buf[1] = (pc.etcFlag & PC_ETCFLAG_CHAT_SAVE) ? 1 : 0;
					buf[2] = (pc.etcFlag & PC_ETCFLAG_CHAT_OCC) ? 1 : 0;
					fwrite(buf, 1, sizeof(buf), pf);
					fclose(pf);
				}
				return 1;
			}

			iChannelbtnId[0] = StockDispBuffer(x + 96, y + 160, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
			if (focusId == 0)
				ShowBottomLineString(FONT_PAL_WHITE, "메뉴 설정창을 닫습니다.");
			for (i = 0; i < sizeof(pszChannel) / sizeof(char*); i++) {
				StockFontBuffer(x + 20, y + i * 30 + 26, FONT_PRIO_FRONT, FONT_PAL_WHITE, pszChannel[i], 0);
				StockFontBuffer(x + 105, y + i * 30 + 26, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg2[((pc.etcFlag & iChannelSwitch[i]) ? 1 : 0)], 0);
			}
		}
	}
#endif




	if (ptActMenuWin != NULL)
	{
		id = -1;
		focusId = -1;
		if (ptActMenuWin->hp >= 1)
		{
			// ????
			id = selGraId(btnId, sizeof(btnId) / sizeof(int));
			focusId = focusGraId(btnId, sizeof(btnId) / sizeof(int));

			for (i = 0; i < sizeof(msg) / sizeof(char*); i++)
			{
				x1 = x + 18;
				y1 = y + i * 30 + 23;
				x2 = x1 + 157;
				y2 = y1 + 22;
				if (MakeHitBox(x1, y1, x2, y2, DISP_PRIO_BOX2))
				{
					if (mouse.onceState & MOUSE_LEFT_CRICK)
						id = i + 1;
					focusId = i + 1;
				}
			}
		}
		if (CheckMenuFlag())
			id = 100;
		else if (((joy_trg[0] & JOY_ESC) && GetImeString() == NULL)) {
			id = 100;
			play_se(203, 320, 240);
		}
		else if (etcSwitchProcNo == 1000)
			id = 100;
		if (id == 0)
			play_se(203, 320, 240);
		else
#ifdef _CHANNEL_MODIFY
			if (id == 4)
			{
				DeathAction(ptActMenuWin);
				ptActMenuWin = NULL;
				etcSwitchProcNo = 100;
				return 0;
			}
			else
#endif
#ifdef _NB_SetResoMode
				if (id == 6) {
					if (ProcNo == PROC_BATTLE) {
						return 0;
					}
					if (ResoMode == 0) {
						ResoMode = 3;
					}
					else if (ResoMode == 3) {
						ResoMode = 4;
					}
					else {
						ResoMode = 0;
					}
					JL_RESOMODE(ResoMode);
					SetResoMode(ResoMode);
					ChangeWindowMode();
					RecoverDirectDraw();
					SendMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
					SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
					return 0;
				}
				else
#endif
					if (1 <= id && id <= sizeof(msg) / sizeof(char*)) {
						id--;
						if (pc.etcFlag & mask[id])
							pc.etcFlag &= ~mask[id];
						else {
#ifdef _STREET_VENDOR
							if (pc.iOnStreetVendor == 1 || sStreetVendorBuyBtn == 2)
							{
								if (mask[id] == PC_ETCFLAG_TRADE || mask[id] == PC_ETCFLAG_PARTY || mask[id] == PC_ETCFLAG_DUEL)
									StockChatBufferLine("노점에서거래열수없음、파티및전투기능", FONT_PAL_RED);
								else
									pc.etcFlag |= mask[id];
							}
							else
								pc.etcFlag |= mask[id];
#else
							pc.etcFlag |= mask[id];
#endif
						}
						if (eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0)
						{
							if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime())// ??
							{
								if (bNewServer)
									lssproto_FS_send(sockfd, pc.etcFlag);
								else
									old_lssproto_FS_send(sockfd, pc.etcFlag);
								fieldBtnPushTime = TimeGetTime();
							}
							else etcSwitchChangeFlag = 1;
						}
						id = -1;
						play_se(217, 320, 240);
					}

		if (id >= 0) {
			DeathAction(ptActMenuWin);
			ptActMenuWin = NULL;
			return 1;
		}

		if (ptActMenuWin->hp >= 1) {
			btnId[0] = StockDispBuffer(x + 96, y + 210, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
			if (focusId == 0)
				ShowBottomLineString(FONT_PAL_WHITE, "메뉴창닫기。");
			for (i = 0; i < sizeof(msg) / sizeof(char*); i++) {
#ifdef _NB_SetResoMode
				char* ResoModeMsg[] = { " 640 x 480"
										," 320 x 280"
										," 640 x 480"
										," 800 x 600"
										,"1024 x 768" };
#endif
				if (i == 5) {
					char buf[64];
					sprintf(buf, "%s%s", msg[i], ResoModeMsg[ResoMode]);
					StockFontBuffer(x + 20, y + i * 30 + 26, FONT_PRIO_FRONT, 1, buf, 0);
				}
				else {
					StockFontBuffer(x + 20, y + i * 30 + 26, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i], 0);
				}
				if (i != 3 && i != 5 && i != 6)    //[주요 선택서를 수정할 때는 반드시 주의해야 한다]
					StockFontBuffer(x + 105, y + i * 30 + 26, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg2[((pc.etcFlag & mask[i]) ? 1 : 0)], 0);
#ifndef _CHANNEL_MODIFY
				else
					StockFontBuffer(x + 105, y + i * 30 + 26, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg3[((pc.etcFlag & mask[i]) ? 1 : 0)], 0);
#endif
				if (i == focusId - 1)
					ShowBottomLineString(FONT_PAL_WHITE, msg4[i]);
			}
		}
	}

	return 0;
}

int disconnectServer(void)
{
	static ACTION* ptActMenuWin = NULL;
	static int x, y, w, h;
	static int btnId[1];
	int id = 0;
	char* msg[] =
	{
		"서버와의 연결이 종료되었습니다.",
		"돌아가시겠습니까？"
	};
	int i;
	int ret = 0;
	int xx, yy;

	if (ptActMenuWin == NULL)
	{
		if (CheckMenuFlag()) {
			InitMenu2();
		}
		closeEtcSwitch();
		closeCharActionAnimeChange();
		closeJoinChannelWN();

		for (i = 0; i < sizeof(btnId) / sizeof(int); i++) {
			btnId[i] = -2;
		}
		w = 5;
		h = 3;
		x = (640 - w * 64) / 2;
		y = (456 - h * 48) / 2;
		ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 1);
#ifdef _NEW_RESOMODE  //800 600模式
		x = ptActMenuWin->x;
		y = ptActMenuWin->y;
#endif
	}
#ifdef _NB_断线自动重连
	extern int 自动登陆是否开启;
#endif
	if (ptActMenuWin != NULL) {
		id = -1;
		if (ptActMenuWin->hp >= 1) {
			//JL add 断线自动点OK
#ifdef _NB_断线自动重连
			id = selFontId(btnId, sizeof(btnId) / sizeof(int));
#else
			int JL_断线自动点OK();

			id = JL_断线自动点OK();
			if (id == -1) {
				id = selFontId(btnId, sizeof(btnId) / sizeof(int));
			}

#endif
			if (id >= 0
#ifdef _NB_断线自动重连
				|| 自动登陆是否开启
#endif
				) {
				DeathAction(ptActMenuWin);
				ptActMenuWin = NULL;
				ret = 1;
			}

			yy = (h * 48) / 4;
			for (i = 0; i < sizeof(msg) / sizeof(char*); i++) {
				xx = (w * 64 - strlen(msg[i]) / 2 * 17) / 2;
				StockFontBuffer(x + xx, y + (i + 1) * yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i], 0);
			}

			xx = (w * 64 - strlen("확인") / 2 * 17) / 2;
			btnId[0] = StockFontBuffer(x + xx, y + 3 * yy, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "확인", 2);
		}
	}

	return ret;
}

void drawFieldInfoWin(void)
{
	static ACTION* ptActMenuWin = NULL;
	static int x, y, w, h;
	int xx, yy;

	if (fieldInfoTime == 0) {
		if (ptActMenuWin != NULL) {
			DeathAction(ptActMenuWin);
			ptActMenuWin = NULL;
		}
		return;
	}

	if (MapWmdFlagBak)
		return;

	if (ptActMenuWin == NULL) {
		if (CheckMenuFlag()) {
			if (MenuToggleFlag & JOY_CTRL_M)
				MapWmdFlagBak = TRUE;
			InitMenu2();
		}
		closeEtcSwitch();
		closeCharActionAnimeChange();
		closeJoinChannelWN();

		// ??????
		w = 4;
		h = 2;
		x = (640 - w * 64) / 2;
		y = (456 - h * 48) / 2;
		ptActMenuWin = MakeWindowDisp(x, y, w, h, NULL, 3);
#ifdef _NEW_RESOMODE  //800 600模式
		x = ptActMenuWin->x;
		y = ptActMenuWin->y;
#endif
		play_se(202, 320, 240);	// ????????
	}

	if (ptActMenuWin != NULL) {
		if (fieldInfoTime + 2000 <= TimeGetTime() || CheckMenuFlag() || windowTypeWN > -1 || actBtn == 1 || menuBtn == 1) {
			DeathAction(ptActMenuWin);
			ptActMenuWin = NULL;
			fieldInfoTime = 0;
			return;
		}
		if (ptActMenuWin->hp >= 1) {
			xx = (w * 64 - strlen(nowFloorName) / 2 * 17) / 2;
			yy = (h * 48 - 16) / 2;
			StockFontBuffer(x + xx, y + yy, FONT_PRIO_FRONT, FONT_PAL_WHITE, nowFloorName, 0);
		}
	}

}

// ???????????????????
void actionShortCutKeyProc(void)
{
	unsigned int key[] =
	{
		JOY_CTRL_0,				// 0
		JOY_CTRL_CIRCUMFLEX,	// 1
		JOY_CTRL_9,				// 2
		JOY_CTRL_7,				// 3
		JOY_CTRL_8,				// 4
		JOY_CTRL_1,				// 5
		JOY_CTRL_2,				// 6
		JOY_CTRL_4,				// 7
		JOY_CTRL_5,				// 8
		JOY_CTRL_6,				// 9
		JOY_CTRL_MINUS,			// 10
		JOY_CTRL_3,				// 11
		JOY_CTRL_YEN			// 12
	};
	int i;
	BOOL pushFlag = FALSE;

	// ??????????????
	if (moveRouteCnt != 0 || nowVx != 0 || nowVy != 0)
		return;

	for (i = 0; i < sizeof(key) / sizeof(unsigned int); i++)
	{
		if (joy_trg[1] & key[i])
		{
			pushFlag = TRUE;
			break;
		}
	}

	if (!pushFlag)
		return;

	if (eventWarpSendFlag == 0 && eventEnemySendFlag == 0 && sendEnFlag == 0)
	{
		if (fieldBtnPushTime + FIELD_BTN_PUSH_WAIT < TimeGetTime())// ??
		{
			if (bNewServer)
				lssproto_AC_send(sockfd, nowGx, nowGy, i);
			else
				old_lssproto_AC_send(sockfd, nowGx, nowGy, i);
			setPcAction(i);
			fieldBtnPushTime = TimeGetTime();
		}
	}
}

static ACTION* ptActChannelWin = NULL;
static ACTION* ptActChannel2Win = NULL;
static ACTION* ptActLeaderFuncWin = NULL;

#ifndef _FM_MODIFY
char channelName[9][20] =
{
	" 부족프로필  ",
	"부족채널 １",
	"부족채널 ２",
	"부족채널 ３",
	"부족채널 ４",
	" 족장방송  ",
	" 채널나가기  ",
	" 족장기능  ",
	"  추   방  "
};
#else
char channelName[5][20] =
{
	" 부족프로필 ",
	" 족장기능 ",
	"부족게시판",
	" 채 널 종 료 ",
	""
};
#endif

void initJoinChannelWN(void)
{
	channelWNFlag = 1;
	//joinChannelProc = 1000;
}

void closeJoinChannelWN(void)
{
	channelWNFlag = 0;
	//joinChannelProc = 0;
	DeathAction(ptActChannelWin);
	ptActChannelWin = NULL;
	joinChannel2Proc = 0;
	DeathAction(ptActChannel2Win);
	ptActChannel2Win = NULL;
	DeathAction(ptActLeaderFuncWin);
	ptActLeaderFuncWin = NULL;
#ifdef _FRIENDCHANNEL
	chatRoomBtn = 0;
	SelRoomBtn = 0;
#ifdef _STREET_VENDOR
	if (sStreetVendorBtn != 3)
	{
		DeathAction(pActStreetVendorWnd);
		pActStreetVendorWnd = NULL;
	}
#else
#ifdef _TABLEOFSKILL
	DeathAction(pActSkillWnd);
	pActSkillWnd = NULL;
#endif
#endif
#endif
}

void joinChannelWN(void)
{
	//static ACTION *ptActMenuWin = NULL;
	int x, y, w, h, i;
#ifndef _FM_MODIFY
	static int btnId[9];
#else
	static int btnId[5];
#endif
	static int graId[1];
	char buf[1024], buf2[64];
	int selId = 0;
	int selgraId;

	x = 20;
	y = 30;
	w = 2;
#ifndef _FM_MODIFY
	h = 8;
#else
	h = 5;
#endif


	if (ptActChannelWin == NULL)
	{
		ptActChannelWin = MakeWindowDisp(x, y, w, h, NULL, 1);
#ifdef _NEW_RESOMODE  //800 600模式
		x = ptActChannelWin->x;
		y = ptActChannelWin->y;
#endif
		for (i = 0; i < sizeof(btnId) / sizeof(int); i++)
			btnId[i] = -2;
	}
	else
	{
		if (ptActChannelWin->hp >= 1)
		{
			// ESC????????
			if (((joy_trg[0] & JOY_ESC) && GetImeString() == NULL))
			{
				closeJoinChannelWN();
				play_se(203, 320, 240);	// ????????
			}
			if (CheckMenuFlag())
				closeJoinChannelWN();

			selId = focusFontId(btnId, sizeof(btnId) / sizeof(int));
			selgraId = focusGraId(graId, sizeof(graId) / sizeof(int));

			if (mouse.onceState & MOUSE_LEFT_CRICK)
			{
				if (selgraId == 0)
				{
					closeJoinChannelWN();
					play_se(203, 320, 240);
					return;
				}
#ifndef _FM_MODIFY
				switch (selId)
				{
				case 0:
					sprintf(buf, "S|D2", selId);
					break;
				case 1:
				case 2:
				case 3:
					sprintf(buf, "C|L|%d", selId);
					break;
				case 5:
#ifdef _FMVER21
					if (pc.familyleader != FMMEMBER_LEADER)
						return;
#else
					if (pc.familyleader != 1)
						return;
#endif
					sprintf(buf, "C|J|5");
					closeJoinChannelWN();
					play_se(203, 320, 240);
					break;
				case 6:
					sprintf(buf, "C|J|-1");
					closeJoinChannelWN();
					play_se(203, 320, 240);
					break;
				case 7:
					DeathAction(ptActChannel2Win);
					ptActChannel2Win = NULL;
					joinChannel2Proc = 2000;
					play_se(203, 320, 240);
					return;
				case 8:
					sprintf(buf, "S|SELF");
					closeJoinChannelWN();
					play_se(203, 320, 240);
					break;
				}
#else
				switch (selId)
				{
				case 0:
					sprintf(buf, "S|D2", selId);
					break;
				case 1:
					DeathAction(ptActChannel2Win);
					ptActChannel2Win = NULL;
					joinChannel2Proc = 2000;
					play_se(203, 320, 240);
					return;
				case 2:
					sprintf(buf, "D");
					closeJoinChannelWN();
					play_se(203, 320, 240);
					break;
				case 3:
					sprintf(buf, "C|J|-1");
					if (bNewServer)
						lssproto_FM_send(sockfd, buf);
					else
						old_lssproto_FM_send(sockfd, buf);
					sprintf(buf, "S|SELF");
					closeJoinChannelWN();
					play_se(203, 320, 240);
					break;
				}
#endif
				if (selId != -1)
				{
					if (bNewServer)
						lssproto_FM_send(sockfd, buf);
					else
						old_lssproto_FM_send(sockfd, buf);
					return;
				}
			}

			strcpy(buf2, "");
			for (i = 0; i < (int)(16 - strlen(pc.familyName)) / 2; i++)
				strcat(buf2, " ");
			strcat(buf2, pc.familyName);

#ifndef _FM_MODIFY
			btnId[8] = StockFontBuffer(x + 7, y + 30, FONT_PRIO_FRONT, FONT_PAL_GREEN, buf2, 0);
#else
			btnId[4] = StockFontBuffer(x + 7, y + 30, FONT_PRIO_FRONT, FONT_PAL_GREEN, buf2, 0);
			btnId[2] = StockFontBuffer(x + 25, y + 65 + 36 * 2, FONT_PRIO_FRONT, FONT_PAL_WHITE, channelName[2], 2);
#endif
			btnId[0] = StockFontBuffer(x + 25, y + 65, FONT_PRIO_FRONT, FONT_PAL_WHITE, channelName[0], 2);

#ifndef _FM_MODIFY
			for (i = 1; i < 4; i++)
			{
				if (pc.channel == i)
					btnId[i] = StockFontBuffer(x + 25, y + 65 + 36 * i, FONT_PRIO_FRONT, FONT_PAL_YELLOW, channelName[i], 2);
				else
					btnId[i] = StockFontBuffer(x + 25, y + 65 + 36 * i, FONT_PRIO_FRONT, FONT_PAL_WHITE, channelName[i], 2);
			}
			btnId[6] = StockFontBuffer(x + 25, y + 65 + 36 * 4, FONT_PRIO_FRONT, FONT_PAL_WHITE, channelName[6], 2);
#endif
#ifdef _FMVER21
			// shan add		
			if (pc.familyleader == FMMEMBER_LEADER || pc.familyleader == FMMEMBER_ELDER)
#else
			if (pc.familyleader == 1)
#endif
#ifndef _FM_MODIFY
				btnId[7] = StockFontBuffer(x + 25, y + 65 + 36 * 5, FONT_PRIO_FRONT, FONT_PAL_WHITE, channelName[7], 2);
			else
				btnId[7] = StockFontBuffer(x + 25, y + 65 + 36 * 5, FONT_PRIO_FRONT, FONT_PAL_GRAY, channelName[7], 0);
			graId[0] = StockDispBuffer(x + 65, y + 13 + 320, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
#else
			{
				btnId[1] = StockFontBuffer(x + 25, y + 65 + 36, FONT_PRIO_FRONT, FONT_PAL_WHITE, channelName[1], 2);
				btnId[3] = StockFontBuffer(x + 25, y + 65 + 36 * 3, FONT_PRIO_FRONT, FONT_PAL_WHITE, channelName[3], 2);
			}
			else
			{
				btnId[1] = StockFontBuffer(x + 25, y + 65 + 36, FONT_PRIO_FRONT, FONT_PAL_GRAY, channelName[1], 0);
				btnId[3] = StockFontBuffer(x + 25, y + 65 + 36 * 3, FONT_PRIO_FRONT, FONT_PAL_GRAY, channelName[3], 0);
			}
			graId[0] = StockDispBuffer(x + 65, y + 13 + 200, DISP_PRIO_IME3, CG_CLOSE_BTN, 2);
#endif

#ifndef _FM_MODIFY
			switch (selId)
			{
			case 0:
				ShowBottomLineString(FONT_PAL_WHITE, "부족세부정보보기。");
				break;
			case 1:
			case 2:
			case 3:
				if (pc.channel == selId)
					sprintf(buf, "현재채널에있습니다。");
				else
					sprintf(buf, "현재채팅 채널보기。");
				ShowBottomLineString(FONT_PAL_WHITE, buf);
				break;
			case 5:
				ShowBottomLineString(FONT_PAL_WHITE, "부족장이 부족원 전체에게 방송。");
				break;
			case 6:
				ShowBottomLineString(FONT_PAL_WHITE, "현재채널종료。");
				break;
			case 7:
				ShowBottomLineString(FONT_PAL_WHITE, "축복사의특별기능。");
				break;
			default:
				if (selgraId == 0)
					ShowBottomLineString(FONT_PAL_WHITE, "채팅채널메뉴에서나가기。");
				break;
			}
#else
			switch (selId)
			{
			case 0:
				ShowBottomLineString(FONT_PAL_WHITE, "가족세부정보보기。");
				break;
			case 1:
				ShowBottomLineString(FONT_PAL_WHITE, "축복사의특별한기능。");
				break;
			case 2:
				ShowBottomLineString(FONT_PAL_WHITE, "부족게시판기능사용。");
				break;
			case 3:
				ShowBottomLineString(FONT_PAL_WHITE, "족장방송기능끄기。");
				break;
			default:
				if (selgraId == 0)
					ShowBottomLineString(FONT_PAL_WHITE, "채팅채널메뉴에서나가기。");
				break;
			}
#endif
			if (joinChannel2Proc == 1000)
				joinChannel2WN();
			else if (joinChannel2Proc == 2000)
				leaderFuncWN();
		}
	}
}

struct TchannelData {
	int index;
	int num;
	int	join[FAMILY_MAXMEMBER];
	int joinNum;
	char name[FAMILY_MAXMEMBER][20];
};
static TchannelData channelData;

char pageStr[2][10] =
{
	"이전",
	"다음",
};
static int pageNum;

void initJoinChannel2WN(char* data)
{
	int i;
	char buf[256];

	channelData.joinNum = 0;
	getStringToken(data, '|', 3, sizeof(buf) - 1, buf);
	channelData.index = atoi(buf);
	getStringToken(data, '|', 4, sizeof(buf) - 1, buf);
	channelData.num = atoi(buf);

	if (channelData.num > 0)
	{
		for (i = 0; i < channelData.num && i < FAMILY_MAXMEMBER; i++)
		{
			getStringToken(data, '|', 4 + (i * 2) + 1, sizeof(buf) - 1, buf);
			channelData.join[i] = atoi(buf);
			if (channelData.join[i] == 1)
				channelData.joinNum++;
			getStringToken(data, '|', 4 + (i * 2) + 2, sizeof(buf) - 1, buf);
			makeStringFromEscaped(buf);
			strcpy(channelData.name[i], buf);
		}
	}
	pageNum = 0;
	play_se(202, 320, 240);	// ????????
	joinChannel2Proc = 1000;

}

void closeJoinChannel2WN(void)
{
	joinChannel2Proc = 0;
	DeathAction(ptActChannel2Win);
	ptActChannel2Win = NULL;
}

void joinChannel2WN(void)
{
	//static ACTION *ptActMenuWin = NULL;
	int x, y, w, h, i;
	static int fontBtnId[4];
	char buf[1024];
	int selId;

	x = 160;
	y = 30;
	w = 7;
	h = 8;

	if (ptActChannel2Win == NULL)
	{
		DeathAction(ptActLeaderFuncWin);
		ptActLeaderFuncWin = NULL;
		ptActChannel2Win = MakeWindowDisp(x, y, w, h, NULL, 1);
#ifdef _NEW_RESOMODE  //800 600模式
		x = ptActChannel2Win->x;
		y = ptActChannel2Win->y;
#endif
	}
	else if (ptActChannel2Win->hp >= 1)
	{
		selId = focusFontId(fontBtnId, sizeof(fontBtnId) / sizeof(int));

		switch (selId)
		{
		case 0:
			ShowBottomLineString(FONT_PAL_WHITE, "이채널에가입。");
			break;
		case 1:
			ShowBottomLineString(FONT_PAL_WHITE, "이창닫기。");
			break;
		}

		if (mouse.onceState & MOUSE_LEFT_CRICK)
		{
			switch (selId)
			{
			case 0:
				sprintf(buf, "C|J|%d", channelData.index);
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
				closeJoinChannelWN();
				play_se(203, 320, 240);	// ????????
				break;
			case 1:
				if (pc.channel == channelData.index)
				{
					sprintf(buf, "C|J|-1");
					if (bNewServer)
						lssproto_FM_send(sockfd, buf);
					else
						old_lssproto_FM_send(sockfd, buf);
				}
				closeJoinChannel2WN();
				play_se(203, 320, 240);	// ????????
				break;
			case 2:
				if (pageNum > 0)
					pageNum--;
				break;
			case 3:
				if (pageNum < 2)
					pageNum++;
				break;
			}
		}

		fontBtnId[0] = StockFontBuffer(x + 240, y + 340, FONT_PRIO_FRONT, FONT_PAL_YELLOW, " 참  가 ", 2);
		fontBtnId[1] = StockFontBuffer(x + 340, y + 340, FONT_PRIO_FRONT, FONT_PAL_YELLOW, " 탈  퇴 ", 2);

		switch (pageNum)
		{
		case 0:
			if (channelData.num > 30)
				fontBtnId[3] = StockFontBuffer(x + 120, y + 340, FONT_PRIO_FRONT, FONT_PAL_YELLOW, pageStr[1], 2);
			break;
		case 1:
			if (channelData.num > 60)
				fontBtnId[3] = StockFontBuffer(x + 120, y + 340, FONT_PRIO_FRONT, FONT_PAL_YELLOW, pageStr[1], 2);
			fontBtnId[2] = StockFontBuffer(x + 50, y + 340, FONT_PRIO_FRONT, FONT_PAL_YELLOW, pageStr[0], 2);
			break;
		case 2:
			fontBtnId[2] = StockFontBuffer(x + 50, y + 340, FONT_PRIO_FRONT, FONT_PAL_YELLOW, pageStr[0], 2);
			break;
		}
		StockFontBuffer(x + 25, y + 25, FONT_PRIO_FRONT, FONT_PAL_WHITE, channelName[channelData.index], 0);

		StockFontBuffer(x + 25, y + 50, FONT_PRIO_FRONT, FONT_PAL_WHITE, "[ 채 널 회 원 ]", 0);

		sprintf(buf, "현재 %d 이부족의 가족。", channelData.num);
		StockFontBuffer(x + 200, y + 25, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);
		sprintf(buf, "있다 %d 이채널의사람들。", channelData.joinNum);
		StockFontBuffer(x + 280, y + 50, FONT_PRIO_FRONT, FONT_PAL_WHITE, buf, 0);

		for (i = 0; i < channelData.num; i++)
		{
			int color;

			if (channelData.join[i] == 1)
				color = FONT_PAL_WHITE;
			else
				color = FONT_PAL_GRAY;

			if (pageNum == 0 && i < 30)
			{
				if (i < 10)
					StockFontBuffer(x + 25, y + 80 + i * 25, FONT_PRIO_FRONT, color, channelData.name[i], 0);
				else if (i < 20)
					StockFontBuffer(x + 25 + 140, y + 80 + (i - 10) * 25, FONT_PRIO_FRONT, color, channelData.name[i], 0);
				else if (i < 30)
					StockFontBuffer(x + 25 + 280, y + 80 + (i - 20) * 25, FONT_PRIO_FRONT, color, channelData.name[i], 0);
			}
			else if (pageNum == 1 && i >= 30 && i < 60)
			{
				if (i < 40)
					StockFontBuffer(x + 25, y + 80 + (i - 30) * 25, FONT_PRIO_FRONT, color, channelData.name[i], 0);
				else if (i < 50)
					StockFontBuffer(x + 25 + 140, y + 80 + (i - 40) * 25, FONT_PRIO_FRONT, color, channelData.name[i], 0);
				else if (i < 60)
					StockFontBuffer(x + 25 + 280, y + 80 + (i - 50) * 25, FONT_PRIO_FRONT, color, channelData.name[i], 0);
			}
			else if (pageNum == 2 && i >= 60)
			{
				if (i < 70)
					StockFontBuffer(x + 25, y + 80 + (i - 60) * 25, FONT_PRIO_FRONT, color, channelData.name[i], 0);
				else if (i < 80)
					StockFontBuffer(x + 25 + 140, y + 80 + (i - 70) * 25, FONT_PRIO_FRONT, color, channelData.name[i], 0);
				else if (i < 90)
					StockFontBuffer(x + 25 + 280, y + 80 + (i - 80) * 25, FONT_PRIO_FRONT, color, channelData.name[i], 0);
			}
		}
	}
}


void leaderFuncWN(void)
{
	//static ACTION *ptActMenuWin = NULL;
	int x, y, w, h;
	static int btnId[9];
	static int graId[1];
	char buf[1024];
	int selId;
	int selgraId;

	x = 160;
	y = 30;
	w = 2;
	h = 7;

	if (ptActLeaderFuncWin == NULL)
	{
		ptActLeaderFuncWin = MakeWindowDisp(x, y, w, h, NULL, 1);
#ifdef _NEW_RESOMODE  //800 600模式
		x = ptActLeaderFuncWin->x;
		y = ptActLeaderFuncWin->y;
#endif
	}
	else if (ptActLeaderFuncWin->hp >= 1)
	{
		// ESC????????
		if (((joy_trg[0] & JOY_ESC) && GetImeString() == NULL))
		{
			closeJoinChannelWN();
			play_se(203, 320, 240);	// ????????
		}
		if (CheckMenuFlag())
			closeJoinChannelWN();

		selId = focusFontId(btnId, sizeof(btnId) / sizeof(int));
		selgraId = focusGraId(graId, sizeof(graId) / sizeof(int));

		if (mouse.onceState & MOUSE_LEFT_CRICK)
		{
			if (selId == 1)
			{
#ifdef _FMVER21
				if (pc.familyleader != FMMEMBER_LEADER)
#else
				if (pc.familyleader != 1)
#endif
					return;
				sprintf(buf, "C|J|5");
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
				closeJoinChannelWN();
				play_se(203, 320, 240);	// ????????
				return;
			}
			if (selId == 2)
			{
				sprintf(buf, "L|F|1");
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
				closeJoinChannelWN();
				play_se(203, 320, 240);	// ????????
				return;
			}

			if (selId == 3)
			{
				sprintf(buf, "L|L|1");
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
				closeJoinChannelWN();
				play_se(203, 320, 240);	// ????????
				return;
			}
			if (selId == 4)
			{
				sprintf(buf, "L|L|2");
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
				closeJoinChannelWN();
				play_se(203, 320, 240);	// ????????
				return;
			}

			if (selId == 5)
			{
				sprintf(buf, "L|CHANGE|L");
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
				closeJoinChannelWN();
				play_se(203, 320, 240);	// ????????
				return;
			}
#ifdef	_ADD_FAMILY_TAX		// WON ADD 增加庄园税收
			if (selId == 6)
			{
				sprintf(buf, "L|FMTAX|W");
				if (bNewServer)
					lssproto_FM_send(sockfd, buf);
				else
					old_lssproto_FM_send(sockfd, buf);
				closeJoinChannelWN();
				play_se(203, 320, 240);	// ????????
				return;
			}
#endif
		}
		int 临时补X = 0;
		if (ResoMode == 分辨率::mode800) {
			临时补X = 80;
		}
		else if (ResoMode == 分辨率::mode1024) {
			临时补X = 190;
		}
#ifdef _FMVER21
		if (pc.familyleader == FMMEMBER_LEADER)
#else
		if (pc.familyleader == 1)
#endif
		{
			if (pc.channel == 5)
				btnId[1] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 0, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "  족장방송  ", 2);
			else
				btnId[1] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 0, FONT_PRIO_FRONT, FONT_PAL_WHITE, "  족장방송  ", 2);

			btnId[2] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 1, FONT_PRIO_FRONT, FONT_PAL_WHITE, "부족구성원관리", 2);

			btnId[3] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 2, FONT_PRIO_FRONT, FONT_PAL_WHITE, " 부족초대 ", 2);
			btnId[4] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 3, FONT_PRIO_FRONT, FONT_PAL_WHITE, " 연구동의 ", 2);
			btnId[5] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 4, FONT_PRIO_FRONT, FONT_PAL_WHITE, "  족장권한부여  ", 2);
#ifdef	_ADD_FAMILY_TAX		// WON ADD 增加庄园税收
			btnId[6] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 5, FONT_PRIO_FRONT, FONT_PAL_WHITE, "유산세율조정", 2);
#endif
		}
		// shan begin
#ifdef _FMVER21
		if (pc.familyleader == FMMEMBER_ELDER)
		{
			btnId[1] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 0, FONT_PRIO_FRONT, FONT_PAL_GRAY, "  족장방송  ", 0);
			btnId[2] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 1, FONT_PRIO_FRONT, FONT_PAL_WHITE, "부족구성원관리", 2);

			btnId[3] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 2, FONT_PRIO_FRONT, FONT_PAL_WHITE, " 부족초대 ", 2);
			btnId[4] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 3, FONT_PRIO_FRONT, FONT_PAL_WHITE, " 연구동의 ", 2);
			btnId[5] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 4, FONT_PRIO_FRONT, FONT_PAL_GRAY, "  족장권한부여  ", 0);

#ifdef	_ADD_FAMILY_TAX		// WON ADD 增加庄园税收
			btnId[6] = StockFontBuffer(x + 临时补X + 22, y + 65 + 36 * 5, FONT_PRIO_FRONT, FONT_PAL_GRAY, "  유산세율조정  ", 0);
#endif
		}
#endif
		// shan end
		if (selId == 1)
			ShowBottomLineString(FONT_PAL_WHITE, "족장은 부족원전체에게 공지합니다。");
		else if (selId == 2)
			ShowBottomLineString(FONT_PAL_WHITE, "부족구성원목록。");
		else if (selId == 3)
			ShowBottomLineString(FONT_PAL_WHITE, "부족방문초청장작성。");
		else if (selId == 4)
			ShowBottomLineString(FONT_PAL_WHITE, "탑승배우기위한동의서작성。");
#ifdef	_ADD_FAMILY_TAX		// WON ADD 增加庄园税收
		else if (selId == 6)
			ShowBottomLineString(FONT_PAL_WHITE, "부동산항목에대한세금조정。");
#endif
	}

}

#ifdef _SURFACE_ANIM        //ROG ADD 动态地上物显示
int ReadAniFile(int floor)
{
	HANDLE hFile;
	DWORD readbyte;
	char fileName[32];
	ani_num = 0;
	int col = 0, row = 0;
	breadAniFlag = TRUE;  //读取档案旗标
	sprintf(fileName, "map\\%d.ani", floor);
	for (int i = 0; i < MAX_ANIM; i++)
	{
		ProduceXY[i][5] = -2;
		ProduceXY[i][6] = -1;
	}
	if ((hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
		return -1;

	while (ReadFile(hFile, &ProduceXY[row], sizeof(int) * 5, &readbyte, NULL) && readbyte)
	{
		ProduceXY[row][5] = 0;
		row++;
	}
	ani_num = row; //该地图总动画数
	CloseHandle(hFile);

	return 1;
}
void SpecAnim(int floor)
{
	int j = 0;

	if (breadAniFlag && ProduceXY[0][5] == -2)
		return;
	else if (breadAniFlag == FALSE && ReadAniFile(floor) == -1)
		return;

	for (int i = 0; i < ani_num; i++)
	{
		switch (SaTimeZoneNo)
		{
		case LS_NIGHT:
			if (ProduceXY[i][3] == 1 && ProduceXY[i][5] == 1)
			{
				if (SPACT[ProduceXY[i][6]] != NULL)
				{
					DeathAction(SPACT[ProduceXY[i][6]]);
					SPACT[ProduceXY[i][6]] = NULL;
					ProduceXY[i][6] = -1;
				}
				ProduceXY[i][5] = -1;
			}
			else if (ProduceXY[i][3] == 2 && ProduceXY[i][5] == -1)
				ProduceXY[i][5] = 0;
			break;
		case LS_MORNING:
		case LS_NOON:
		case LS_EVENING:
			if (ProduceXY[i][3] == 2 && ProduceXY[i][5] == 1)
			{
				if (SPACT[ProduceXY[i][6]] != NULL)
				{
					DeathAction(SPACT[ProduceXY[i][6]]);
					SPACT[ProduceXY[i][6]] = NULL;
					ProduceXY[i][6] = -1;
				}
				ProduceXY[i][5] = -1;
			}
			else if (ProduceXY[i][3] == 1 && ProduceXY[i][5] == -1)
				ProduceXY[i][5] = 0;
			break;
		}

		if (ProduceXY[i][5] == 0)
		{
			for (j = 0; j < MAX_ANIM; j++)
			{
				if (SPACT[j] == NULL)
				{
					float mx, my;
					//SPACT[j] = createWaterAnimation( ProduceXY[i][0] , ProduceXY[i][1] , ProduceXY[i][2] , ProduceXY[i][4] );
					SPACT[j] = GetAction(DISP_PRIO_BOX3, sizeof(CHAREXTRA));
					if (SPACT[j] == NULL)
						break;
					SPACT[j]->anim_chr_no = ProduceXY[i][0];
					SPACT[j]->anim_no = ANIM_STAND;
					SPACT[j]->anim_ang = 1;
					SPACT[j]->dispPrio = ProduceXY[i][4];
					SPACT[j]->atr |= ACT_ATR_HIT;
					SPACT[j]->nextGx = ProduceXY[i][1];
					SPACT[j]->nextGy = ProduceXY[i][2];
					SPACT[j]->bufCount = 0;
					SPACT[j]->gx = ProduceXY[i][1];
					SPACT[j]->gy = ProduceXY[i][2];
					SPACT[j]->mx = (float)ProduceXY[i][1] * GRID_SIZE;
					SPACT[j]->my = (float)ProduceXY[i][2] * GRID_SIZE;
					SPACT[j]->vx = 0;
					SPACT[j]->vy = 0;
					camMapToGamen(SPACT[j]->mx, SPACT[j]->my, &mx, &my);
					SPACT[j]->x = (int)(mx + .5);
					SPACT[j]->y = (int)(my + .5);
					extern void waterAniProc(ACTION * ptAct);
					SPACT[j]->func = waterAniProc;
					//pattern(SPACT[j], ANM_NOMAL_SPD, ANM_LOOP);
					ProduceXY[i][5] = 1;
					ProduceXY[i][6] = j;
					break;
				}
			}
		}
	}
}

void ReleaseSpecAnim()
{
	breadAniFlag = FALSE;
	for (int i = 0; i < ani_num; i++)
	{
		if (ProduceXY[i][5] == 1)
		{
			if (SPACT[ProduceXY[i][6]] != NULL)
			{
				DeathAction(SPACT[ProduceXY[i][6]]);
				SPACT[ProduceXY[i][6]] = NULL;
				ProduceXY[i][6] = -1;
				ProduceXY[i][5] = 0;
			}
		}
	}
}

#else

#ifdef _SPECIALSPACEANIM	// Syu ADD 特殊场景动画配置
void SpecAnim(int floor)
{
	int j = 0;
	for (int i = 0; i < TOTAL_ANIM; i++)
	{
		if (floor == ProduceXY[i][2] && ProduceXY[i][4] == 0)
		{
			for (j = 0; j < MAX_ANIM; j++)
			{
				if (SPACT[j] == NULL)
				{
#ifdef _SPECIALSPACEANIM_FIX
					SPACT[j] = createWaterAnimation(ProduceXY[i][3], ProduceXY[i][0], ProduceXY[i][1], ProduceXY[i][6]);
#else
					SPACT[j] = createWaterAnimation(ProduceXY[i][3], ProduceXY[i][0], ProduceXY[i][1], 30);
#endif
					ProduceXY[i][4] = 1;
					ProduceXY[i][5] = j;
					break;
				}
			}
		}
		else if (floor != ProduceXY[i][2] && ProduceXY[i][4] == 1)
		{
			if (SPACT[ProduceXY[i][5]] != NULL)
			{
				DeathAction(SPACT[ProduceXY[i][5]]);
				SPACT[ProduceXY[i][5]] = NULL;
				ProduceXY[i][5] = -1;
				ProduceXY[i][4] = 0;
			}
		}
	}
}

void ReleaseSpecAnim()
{
	for (int i = 0; i < TOTAL_ANIM; i++)
	{
		if (ProduceXY[i][4] == 1)
		{
			if (SPACT[ProduceXY[i][5]] != NULL)
			{
				DeathAction(SPACT[ProduceXY[i][5]]);
				SPACT[ProduceXY[i][5]] = NULL;
				ProduceXY[i][5] = -1;
				ProduceXY[i][4] = 0;
			}
		}
	}
}
#endif
#endif

#ifdef _CLOUD_EFFECT				// (不可开) ROG ADD云层效果
void CloudDraw()
{
	static DWORD SlowDraw;
	static int	dif, dif1;
	DWORD now;

	if ((now = TimeGetTime()) > SlowDraw)
	{
		SlowDraw = now + 10;
		dif++;
		if (dif > 1000)
			dif = 0;
		dif1 += 2;
		if (dif1 > 1000)
			dif1 = 0;
	}
	StockDispBuffer(0 + dif - 200, 0 + dif, 20, 40500, 1);
	StockDispBuffer(0 + dif1 - 360, 20 + dif1, 20, 40501, 1);
	StockDispBuffer(0 + dif - 50, 10 + dif - 480, 20, 40502, 1);
	StockDispBuffer(0 + dif1 - 100, 50 + dif - 400, 20, 40503, 1);
}
#endif

#ifdef _TEACHER_SYSTEM
void TeacherSystemWndfunc(int flag, char* data)
{
	int i;
	static int winX, winY, winW, winH, iOnline = 0, iGetNum = 0;
	static int iFontId[7], iIndex[5];
	static ACTION* pActTeacherSystemWnd = NULL;
	static char szBuf[6][32];
	static BOOL bOpen = FALSE;
	char szMsg[128];

	if (pActTeacherSystemWnd == NULL)
	{
		memset(iFontId, -1, sizeof(iFontId));
		memset(iIndex, -1, sizeof(iIndex));
		memset(szBuf, 0, sizeof(szBuf));
		switch (flag)
		{
			// 只有一个人
		case 0:
			winW = 5;
			winH = 2;
			// get name
			getStringToken(data, '|', 2, sizeof(szBuf[0]) - 1, szBuf[0]); // 存导师的名字
			getStringToken(data, '|', 3, sizeof(szBuf[6]) - 1, szBuf[6]); // 取出导师 index
			iIndex[0] = atoi(szBuf[6]);
			break;
			// 一人以上在同一格
		case 1:
			winW = 5;
			winH = 5;
			getStringToken(data, '|', 2, sizeof(szBuf[6]) - 1, szBuf[6]);
			iGetNum = atoi(szBuf[6]);
			// get name
			for (i = 0; i < iGetNum; i++)
			{
				getStringToken(data, '|', 3 + i * 2, sizeof(szBuf[i]) - 1, szBuf[i]); // 存导师的名字
				getStringToken(data, '|', 4 + i * 2, sizeof(szBuf[6]) - 1, szBuf[6]); // 取出导师 index
				iIndex[i] = atoi(szBuf[6]);
			}
			break;
			// 显示导师资料
		case 2:
			winW = 5;
			winH = 3;
			getStringToken(data, '|', 2, sizeof(szBuf[0]) - 1, szBuf[0]);	// 存导师的名字
			getStringToken(data, '|', 3, sizeof(szBuf[6]) - 1, szBuf[6]); // 取出在不在线状态
			iOnline = atoi(szBuf[6]);
			getStringToken(data, '|', 4, sizeof(szBuf[1]) - 1, szBuf[1]); // 存星系 ip
			break;
		case 3:
			winW = 5;
			winH = 2;
			break;
		}
		iOldGX = nowGx;
		iOldGY = nowGy;
		winX = (640 - winW * 64) / 2;
		winY = (456 - winH * 48) / 2;
		pActTeacherSystemWnd = MakeWindowDisp(winX, winY, winW, winH, NULL, 1);
	}
	else
	{
		// close window
		if (CheckMenuFlag() || (joy_trg[0] & JOY_ESC) || actBtn == 1 || menuBtn == 1 ||
			disconnectServerFlag == TRUE || (iOldGX != nowGx || iOldGY != nowGy))
		{
			DeathAction(pActTeacherSystemWnd);
			pActTeacherSystemWnd = NULL;
			sTeacherSystemBtn = 0;
			bOpen = FALSE;
			return;
		}
		if (pActTeacherSystemWnd->hp >= 1)
		{
			if (!bOpen)
				bOpen = TRUE;
			// 只有一个人询问是否要对方当你的导师
			if (flag == 0)
			{
				sprintf(szMsg, "당신은식별할수있습니까 %s", szBuf[0]);
				StockFontBuffer(winX + 20, winY + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
				StockFontBuffer(winX + 20, winY + 36, FONT_PRIO_FRONT, FONT_PAL_WHITE, "멘토가되어보세요？", 0);
				iFontId[0] = StockFontBuffer(winX + 64, winY + winH * 30, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "확  인", 2);
				iFontId[1] = StockFontBuffer(winX + (winW + 1) * 32, winY + winH * 30, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "취  소", 2);
				if (mouse.onceState & MOUSE_LEFT_CRICK)
				{
					for (i = 0; i < 2; i++)
					{
						if (HitFontNo == iFontId[i])
						{
							play_se(217, 320, 240);
							// 확인
							if (i == 0)
							{
								sprintf(szMsg, "O|%d", iIndex[0]);
								lssproto_TEACHER_SYSTEM_send(sockfd, szMsg);
							}
							DeathAction(pActTeacherSystemWnd);
							pActTeacherSystemWnd = NULL;
							sTeacherSystemBtn = 0;
							bOpen = FALSE;
							return;
						}
					}
				}
			}
			// 一人以上在同一格询问是否要对方当你的导师
			else if (flag == 1)
			{
				sprintf(szMsg, "멘토로 누굴 선택합니까？");
				StockFontBuffer(winX + 20, winY + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
				iFontId[0] = StockFontBuffer(winX + winW / 2 * 64, winY + (winH - 1) * 48, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "취  소", 2);
				for (i = 0; i < iGetNum; i++)
					iFontId[i + 1] = StockFontBuffer(winX + 20, winY + 46 + i * 24, FONT_PRIO_FRONT, FONT_PAL_WHITE, szBuf[i], 2);
				if (mouse.onceState & MOUSE_LEFT_CRICK)
				{
					for (i = 0; i < 7; i++)
					{
						if (HitFontNo == iFontId[i])
						{
							play_se(217, 320, 240);
							// 选好了导师
							if (i != 0)
							{
								sprintf(szMsg, "O|%d", iIndex[i - 1]);
								lssproto_TEACHER_SYSTEM_send(sockfd, szMsg);
							}
							DeathAction(pActTeacherSystemWnd);
							pActTeacherSystemWnd = NULL;
							sTeacherSystemBtn = 0;
							bOpen = FALSE;
							return;
						}
					}
				}
			}
			// 显示导师资料
			else if (flag == 2)
			{
				sprintf(szMsg, "멘토상태");
				StockFontBuffer(winX + 130, winY + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
				sprintf(szMsg, "멘토이름：%s", szBuf[0]);
				StockFontBuffer(winX + 20, winY + 36, FONT_PRIO_FRONT, FONT_PAL_WHITE, szMsg, 0);
				StockFontBuffer(winX + 20, winY + 52, FONT_PRIO_FRONT, FONT_PAL_WHITE, "상태：", 0);
				StockFontBuffer(winX + 66, winY + 52, FONT_PRIO_FRONT,
					iOnline == 1 ? FONT_PAL_YELLOW : FONT_PAL_RED, iOnline == 1 ? "Online" : "Offline", 0);
				StockFontBuffer(winX + 148, winY + 52, FONT_PRIO_FRONT, FONT_PAL_WHITE, "은하：", 0);
				if (iOnline == 1)
				{
					for (i = 0; i < MAX_GMSV; i++)
					{
						if (gmsv[i].used == '1')
						{
							if (strcmp(szBuf[1], gmsv[i].ipaddr) == 0)
							{
								StockFontBuffer(winX + 190, winY + 52, FONT_PRIO_FRONT, FONT_PAL_GREEN, gmsv[i].name, 0);
								break;
							}
						}
					}
				}
				else
					StockFontBuffer(winX + 190, winY + 52, FONT_PRIO_FRONT, FONT_PAL_WHITE, "아니", 0);
				iFontId[0] = StockFontBuffer(winX + 64, winY + winH * 30, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "종  료", 2);
				iFontId[1] = StockFontBuffer(winX + (winW + 1) * 32, winY + winH * 30, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "멘토의 실격", 2);
				if (mouse.onceState & MOUSE_LEFT_CRICK)
				{
					for (i = 0; i < 2; i++)
					{
						if (HitFontNo == iFontId[i])
						{
							play_se(217, 320, 240);
							sTeacherSystemBtn = 0;
							// 취소导师资格
							if (i != 0)
								sTeacherSystemBtn = 5;
							DeathAction(pActTeacherSystemWnd);
							pActTeacherSystemWnd = NULL;
							bOpen = FALSE;
							return;
						}
					}
				}
			}
			// 确认是否真的要취소导师
			else if (flag == 3)
			{
				StockFontBuffer(winX + 20, winY + 20, FONT_PRIO_FRONT, FONT_PAL_WHITE, "멘토를취소하면매력가치가감소합니다，", 0);
				StockFontBuffer(winX + 20, winY + 40, FONT_PRIO_FRONT, FONT_PAL_WHITE, "멘토를취소하시겠습니까？", 0);
				iFontId[0] = StockFontBuffer(winX + 64, winY + winH * 30, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "확  인", 2);
				iFontId[1] = StockFontBuffer(winX + (winW + 1) * 32, winY + winH * 30, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "취  소", 2);
				if (mouse.onceState & MOUSE_LEFT_CRICK)
				{
					for (i = 0; i < 2; i++)
					{
						if (HitFontNo == iFontId[i])
						{
							play_se(217, 320, 240);
							// 취소导师资格
							if (i == 0)
								lssproto_TEACHER_SYSTEM_send(sockfd, "C|");
							DeathAction(pActTeacherSystemWnd);
							pActTeacherSystemWnd = NULL;
							sTeacherSystemBtn = 0;
							bOpen = FALSE;
							return;
						}
					}
				}
			}
		}
		else
		{
			if (bOpen)
			{
				bOpen = FALSE;
				DeathAction(pActTeacherSystemWnd);
				pActTeacherSystemWnd = NULL;
				sTeacherSystemBtn = 0;
			}
		}
	}
}
#endif

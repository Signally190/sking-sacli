﻿#include "../systeminc/version.h"
#include"../systeminc/system.h"
#include"../systeminc/action.h"
#include"../systeminc/pc.h"
#include"../systeminc/character.h"
#include"../systeminc/map.h"
#include"../systeminc/tool.h"
#include"../systeminc/anim_tbl.h"
#include"../systeminc/loadsprbin.h"
#include"../systeminc/netmain.h"
#include"../systeminc/lssproto_cli.h"
#include"../systeminc/savedata.h"
#include"../systeminc/menu.h"
#include"../systeminc/field.h"
#include"../systeminc/login.h"
#include"../systeminc/netproc.h"

PC pc;
short maxPcNo;
short selectPcNo;
short prSendMode = 0;
short prSendFlag = 0;

short jbSendFlag;
short duelSendFlag;

int loginDp;

short helpFlag = 0;

short tradeFlag = 0;
short tradeStatus = 0;

PET pet[MAX_PET];

#ifdef _NB_断线自动重连
extern Landed PcLanded;
#endif

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
PROFESSION_SKILL profession_skill[MAX_PROFESSION_SKILL];
#endif

MAGIC magic[MAX_MAGIC];


PARTY party[MAX_PARTY];
short partyModeFlag = 0;


CHARLISTTABLE chartable[MAXCHARACTER];

ADDRESS_BOOK addressBook[MAX_ADR_BOOK];

BATTLE_RESULT_MSG battleResultMsg;

PET_SKILL petSkill[MAX_PET][MAX_SKILL];


char newCharacterName[CHAR_NAME_LEN+1];
int newCharacterGraNo;
int newCharacterFaceGraNo;
int newCharacterVit;
int newCharacterStr;
int newCharacterTgh;
int newCharacterDex;
int newCharacterEarth;
int newCharacterWater;
int newCharacterFire;
int newCharacterWind;
int newCharacterHomeTown;

template <typename T>
inline void swap(T &T1, T &T2)
{
	T TTemp;

	TTemp = T1;
	T1 = T2;
	T2 = TTemp;
}

void swap(char *a, char *b)
{
	char tmp[256];

	if(strlen(a) > 255 || strlen(b) > 255)
		return;

	strcpy(tmp, a);
	strcpy(a, b);
	strcpy(b, tmp);
}

//void swapInteger(int *, int *);
//void swapShort(short *, short *);
//void swapString(char *, char *);

void initPcAll(void)
{
	int i;

	memset(&pc, 0, sizeof(pc));
	memset(&pet, 0, sizeof(pet));
	memset(&magic, 0, sizeof(magic));
	memset(&party, 0, sizeof(party));
	memset(&petSkill, 0, sizeof(petSkill));

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业	
	memset(&profession_skill, 0, sizeof(profession_skill));
#endif

	pc.mailPetNo = -1;

	getUserSetting(selectPcNo);
		
	partyModeFlag = 0;
	sPetStatFlag = 1;

	for(i = 0; i < MAX_PARTY; i++)
	{
		party[i].useFlag = 0;
		party[i].id = 0;
		party[i].ptAct = NULL;
	}


#if 1
	pc.faceGraNo = newCharacterFaceGraNo;
	pc.dp = loginDp;
#endif

	// shan 2002/01/18	
	{
		FILE *fp;
		char filename[255];
		char vTemp[32];		

		sprintf(filename, "map\\bgm%d.dat", sCharSide);
		if((fp = fopen(filename, "rb+"))!=NULL)
		{
			fread(&vTemp, sizeof(char), 2, fp);
			fclose (fp);
			fMapBgm = atoi(vTemp);
		}		
	}
#ifdef _STREET_VENDOR
	pc.iOnStreetVendor = 0;
#endif
}


void initPc(void)
{
	int walk = 0, height = 0;
	int i;

#ifndef _DEBUG
	createPc(pc.graNo, nowGx, nowGy, pc.dir);
#else
	if(offlineFlag)
	{
		createPc(SPR_pet004, nowGx, nowGy, pc.dir);
	}
	else
	{
		createPc(pc.graNo, nowGx, nowGy, pc.dir);
	}
#endif

	if((pc.status & CHR_STATUS_W))
	{
		walk = 1;
	}
	if((pc.status & CHR_STATUS_H))
	{
		height = 1;
	}
#ifdef _NB_文字称号
	setPcParam(pc.name, pc.freeName, pc.level, pc.ridePetName, pc.ridePetLevel, pc.nameColor, walk, height, pc.profession_class, pc.profession_level, pc.profession_skill_point , pc.herofloor,pc.nb_name,pc.nb_namecolor);
#else
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
//    #ifdef _GM_IDENTIFY		// Rog ADD GM识别
//	setPcParam(pc.name, pc.freeName, pc.level, pc.ridePetName, pc.ridePetLevel, pc.nameColor, walk, height, pc.profession_class, pc.profession_level, pc.profession_exp, pc.profession_skill_point, pc.ptAct->gm_name);
//	setPcParam(pc.name, pc.freeName, pc.level, pc.ridePetName, pc.ridePetLevel, pc.nameColor, walk, height, pc.profession_class, pc.profession_level, pc.profession_skill_point, pc.ptAct->gm_name);    
//	#else	
//	setPcParam(pc.name, pc.freeName, pc.level, pc.ridePetName, pc.ridePetLevel, pc.nameColor, walk, height, pc.profession_class, pc.profession_level, pc.profession_exp, pc.profession_skill_point);
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	setPcParam(pc.name, pc.freeName, pc.level, pc.ridePetName, pc.ridePetLevel, pc.nameColor, walk, height, pc.profession_class, pc.profession_level, pc.profession_skill_point , pc.herofloor);
#else
	setPcParam(pc.name, pc.freeName, pc.level, pc.ridePetName, pc.ridePetLevel, pc.nameColor, walk, height, pc.profession_class, pc.profession_level, pc.profession_skill_point);
#endif
//	#endif
#else
	setPcParam(pc.name, pc.freeName, pc.level, pc.ridePetName, pc.ridePetLevel, pc.nameColor, walk, height);
#endif
#endif

#ifdef _FRIENDCHANNEL
//	strcpy(pc.chatRoomNum,"");
#endif

	if(pc.ptAct != NULL)
	{
		pc.ptAct->hp = pc.hp;
		pc.ptAct->maxHp = pc.maxHp;

		// ??????????????????
		for(i = 0; i < MAX_PARTY; i++)
		{
			if(party[i].useFlag != 0 && party[i].id == pc.id)
			{
				party[i].ptAct = pc.ptAct;
				break;
			}
		}
	}


	prSendMode = 0;
	prSendFlag = 0;
	jbSendFlag = 0;
	duelSendFlag = 0;
	helpFlag = 0;

}


// PC????
void createPc(int graNo, int gx, int gy, int dir)
{
	pc.graNo = graNo;
	pc.dir = dir;
	if(pc.ptAct == NULL)
	{
		pc.ptAct = createCharAction(graNo, gx, gy, dir);
		if(pc.ptAct != NULL)
		{
			pc.ptAct->atr |= ACT_ATR_TYPE_PC;
		}
	}
	else
	{
		setPcGraNo(graNo, dir);
		setPcWarpPoint(gx, gy);
		setPcPoint();
	}
}


// PC???????
void resetPc(void)
{
	int i;

	try
	{
		if (pc.ptAct != NULL)
		{
#ifdef _ITEM_FIREWORK
			if (((CHAREXTRA*)pc.ptAct->pYobi)->pActFirework[0])
			{
				DeathAction(((CHAREXTRA*)pc.ptAct->pYobi)->pActFirework[0]);
				((CHAREXTRA*)pc.ptAct->pYobi)->pActFirework[0] = NULL;
			}
			if (((CHAREXTRA*)pc.ptAct->pYobi)->pActFirework[1])
			{
				DeathAction(((CHAREXTRA*)pc.ptAct->pYobi)->pActFirework[1]);
				((CHAREXTRA*)pc.ptAct->pYobi)->pActFirework[1] = NULL;
			}
#endif
#ifdef ITEM_AURA_
			if (((CHAREXTRA*)pc.ptAct->pYobi)->ptActAura1)
			{
				DeathAction(((CHAREXTRA*)pc.ptAct->pYobi)->ptActAura1);
				((CHAREXTRA*)pc.ptAct->pYobi)->ptActAura1 = nullptr;
			}
			if (((CHAREXTRA*)pc.ptAct->pYobi)->ptActAura2)
			{
				DeathAction(((CHAREXTRA*)pc.ptAct->pYobi)->ptActAura2);
				((CHAREXTRA*)pc.ptAct->pYobi)->ptActAura2 = nullptr;
			}
#endif

			DeathAction(pc.ptAct);
			pc.ptAct = NULL;
		}

		delPcLeader();
	}
	catch (...)
	{
		pc.ptAct = NULL;
		return;
	}



	// ????????????

}


// PC??????????
void setPcGraNo(int graNo, int dir)
{
	pc.graNo = graNo;
	pc.dir = dir;

	if(pc.ptAct == NULL)
		return;

	pc.ptAct->anim_chr_no = graNo;
	pc.ptAct->anim_ang = dir;
}


// PC???ID?
void setPcId(int id)
{
	pc.id = id;
}


// PC??????
void setPcWarpPoint(int gx, int gy)
{
//	if(pc.ptAct == NULL)
//		return;

	setWarpMap(gx, gy);
}


// PC???????????
void setPcPoint(void)
{
	if(pc.ptAct == NULL)
		return;

	pc.ptAct->mx = nowX;
	pc.ptAct->my = nowY;
	pc.ptAct->gx = nowGx;
	pc.ptAct->gy = nowGy;
	pc.ptAct->vx = nowVx;
	pc.ptAct->vy = nowVy;
	pc.ptAct->nextGx = nextGx;
	pc.ptAct->nextGy = nextGy;
}


//JL 注释 设置人物方向
void setPcDir(int dir)
{
	pc.dir = dir;

	if(pc.ptAct == NULL)
		return;

	pc.ptAct->anim_ang = dir;
}


// ???????
void setPcAction(int act)
{
	if(pc.ptAct == NULL)
		return;

	pc.ptAct->anim_no = act;
	if(pc.ptAct->anim_no != ANIM_WALK)
	{
		pc.ptAct->anim_no_bak = -1;
	}
}

#ifdef __EMOTION
void setPcEmotion(int emotion)
{
	CHAREXTRA *ext;
	if(pc.ptAct == NULL)
		return;
	ext = (CHAREXTRA *)pc.ptAct->pYobi;
	if(ext->ptActEmotion){
		DeathAction(ext->ptActEmotion);
		ext->ptActEmotion = NULL;
	}
	ext->ptActEmotion = 
		createCommmonEffectAction(emotion, pc.ptAct->gx, pc.ptAct->gy, 0, 0, DISP_PRIO_CHAR);
}
#endif

// ????????
int getPcAction(void)
{
	if(pc.ptAct == NULL)
		return -1;

	return pc.ptAct->anim_no;
}

#ifdef _CHANNEL_MODIFY
extern int TalkMode;
#endif

#ifdef _NB_文字称号
void setPcParam(char *name, char *freeName, int level, char *petname, int petlevel, int nameColor, int walk, int height, int profession_class, int profession_level, int profession_skill_point , int herofloor,char *newname,int newnamecolor)
#else
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
//    #ifdef _GM_IDENTIFY		// Rog ADD GM识别
//  void setPcParam(char *name, char *freeName, int level, char *petname, int petlevel, int nameColor, int walk, int height, int profession_class, int profession_level, int profession_exp, int profession_skill_point , char *gm_name)
//    void setPcParam(char *name, char *freeName, int level, char *petname, int petlevel, int nameColor, int walk, int height, int profession_class, int profession_level, int profession_skill_point , char *gm_name)    
//	#else
//	void setPcParam(char *name, char *freeName, int level, char *petname, int petlevel, int nameColor, int walk, int height, int profession_class, int profession_level, int profession_exp, int profession_skill_point)
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	void setPcParam(char *name, char *freeName, int level, char *petname, int petlevel, int nameColor, int walk, int height, int profession_class, int profession_level, int profession_skill_point , int herofloor)
#else
	void setPcParam(char *name, char *freeName, int level, char *petname, int petlevel, int nameColor, int walk, int height, int profession_class, int profession_level, int profession_skill_point)
#endif
// 	#endif
#else
    void setPcParam(char *name, char *freeName, int level, char *petname, int petlevel, int nameColor, int walk, int height)
#endif
#endif
{
	int nameLen;
	int freeNameLen;
	int petnameLen;
#ifdef _GM_IDENTIFY		// Rog ADD GM识别
	int gmnameLen;
#endif
	nameLen = strlen(name);
	if(nameLen <= CHAR_NAME_LEN)
	{
		strcpy(pc.name, name);
	}

	freeNameLen = strlen(freeName);
	if(freeNameLen <= CHAR_FREENAME_LEN)
	{
		strcpy(pc.freeName, freeName);
	}

	pc.level = level;

	petnameLen = strlen(petname);
	if(petnameLen <= CHAR_FREENAME_LEN)
	{
		strcpy(pc.ridePetName, petname);
	}

	pc.ridePetLevel = petlevel;

	pc.nameColor = nameColor;
	if(walk != 0)		// ?????
	{
		pc.status |= CHR_STATUS_W;
	}
	if(height != 0)	// ?????
	{
		pc.status |= CHR_STATUS_H;
	}

	if(pc.ptAct == NULL)
		return;

	if(nameLen <= CHAR_NAME_LEN)
	{
		strcpy(pc.ptAct->name, name);
	}
	if(freeNameLen <= CHAR_FREENAME_LEN)
	{
		strcpy(pc.ptAct->freeName, freeName);
	}
	pc.ptAct->level = level;
	
	if(petnameLen <= CHAR_FREENAME_LEN)
	{
		strcpy(pc.ptAct->petName, petname);
	}
	pc.ptAct->petLevel = petlevel;

	pc.ptAct->itemNameColor = nameColor;

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	pc.profession_class	= profession_class;
	pc.ptAct->profession_class = profession_class;
	pc.profession_level = profession_level;
//	pc.profession_exp = profession_exp;
	pc.profession_skill_point = profession_skill_point;
#endif
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	pc.herofloor = herofloor;
#endif
#ifdef _GM_IDENTIFY		// Rog ADD GM识别
	gmnameLen = strlen(gm_name);
	if(gmnameLen <= 33){
		strcpy(pc.ptAct->gm_name, gm_name);
	}
#endif

#ifdef _CHANNEL_MODIFY
	if(pc.profession_class == 0){
		pc.etcFlag &= ~PC_ETCFLAG_CHAT_OCC;
		//TalkMode = 0;
	}
#endif
#ifdef _NB_文字称号
	if( strlen( newname ) <= CHAR_NAME_LEN )
		strcpy(pc.ptAct->nb_name, newname);
	pc.nb_namecolor = newnamecolor;
#endif
}

extern int nServerGroup;
// PC???????????????
void updataPcAct(void)
{
	if(pc.ptAct == NULL)
		return;

	if(strlen(pc.name) <= CHAR_NAME_LEN)
	{
		strcpy(pc.ptAct->name, pc.name);
	}
	if(strlen(pc.freeName) <= CHAR_FREENAME_LEN)
	{
		strcpy(pc.ptAct->freeName, pc.freeName);
	}
	pc.ptAct->level = pc.level;
	pc.ptAct->hp    = pc.hp;
	pc.ptAct->maxHp = pc.maxHp;


}


// PC???????????
void setPcLeader(void)
{
	pc.status |= CHR_STATUS_LEADER;
}


// PC????????????????
void delPcLeader(void)
{
	pc.status &= (~CHR_STATUS_LEADER);
}


// PC???????????????
void setPcParty(void)
{
	pc.status |= CHR_STATUS_PARTY;
}


// PC?????????????????
void delPcParty(void)
{
	pc.status &= (~CHR_STATUS_PARTY);
}


// PC???????????
void setPcWatch(void)
{
	pc.status |= CHR_STATUS_WATCH;
}


// PC???????????????
void delPcWatch(void)
{
	pc.status &= (~CHR_STATUS_WATCH);
}


// ????????????
//???????????????????
void setPcWalkFlag(void)
{
	if(pc.ptAct == NULL)
		return;

	pc.ptAct->walkFlag = 1;
}

// ????????????????
//???????????????????
void delPcWalkFlag(void)
{
	if(pc.ptAct == NULL)
		return;

	pc.ptAct->walkFlag = 0;
}


// ?????
int checkPcWalkFlag(void)
{
	if(pc.ptAct == NULL)
		return 0;

	return (int)pc.ptAct->walkFlag;
}


// ????????
void setPcUseMagic(void)
{
	pc.status |= CHR_STATUS_USE_MAGIC;
}


// ????????
void delPcUseMagic(void)
{
	pc.status &= (~CHR_STATUS_USE_MAGIC);
}


// PC??????
void setPcFukidashi(unsigned int offTime)
{
	CHAREXTRA *ext;

	if(pc.ptAct == NULL)
		return;

	pc.status |= CHR_STATUS_FUKIDASHI;
	ext = (CHAREXTRA *)pc.ptAct->pYobi;

	//JL fix 可能出现0xdddddddd
	if ((int)ext == 0xdddddddd)
	{
		return;
	}
	ext->drawFukidashiTime = offTime + TimeGetTime();
}


// ?????????????
void setPcNameColor(int color)
{
	pc.pcNameColor = color;

	if(pc.ptAct == NULL)
		return;

	pc.ptAct->charNameColor = color;
}

#ifdef _ANGEL_SUMMON
void setPcAngel(void)
{
	int find = FALSE;
	for(int i =0; i <CHAR_EQUIPPLACENUM; i++) {
		if(pc.item[i].graNo == 35268)
			find = TRUE;
	}
	if(find)
		pc.status |= CHR_STATUS_ANGEL;
}

void delPcAngel(void)
{
	pc.status &= (~CHR_STATUS_ANGEL);
}
#endif


// PC????? /////////////////////////////////////////////////////////
void changePcAct(int x, int y, int dir, int action,int effectno, int effectparam1, int effectparam2){
	switch(action){
		case 0:
#if 0
#if 1
			setPcWarpPoint(x, y);
			setPcDir(dir);
#else
			if(partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0){
				setPcWarpPoint(x, y);
				setPcDir(dir);
			}
#endif
#endif
			setPcAction(ANIM_STAND);
			break;
#ifdef _SHOWFAMILYBADGE_
		case 43:
			if (effectno > 0) {
				if (pc.familyName[0])
					setCharmManor(pc.ptAct, effectno);
			}
			else
				delCharmManor(pc.ptAct);
			break;
#endif
#ifdef FAMILY_MANOR_
		case 44:
			if (effectno >= 0) {
				if (pc.familyName[0])
					setCharmFamily(pc.ptAct, effectno);
			}
			else
				delCharmFamily(pc.ptAct);
			break;
#endif
#ifdef 人物光环
		case 46:
			if (effectno >= 0) {
				setCharmManor(pc.ptAct, effectno);
			}
			else
				delCharmManor(pc.ptAct);
			break;
#endif
#ifdef 人物称号
		case 45:
			if (effectno >= 0) {
				setCharmTitle(pc.ptAct, effectno);
			}else
				delCharmTitle(pc.ptAct);
			break;
#endif
		// ???
		case 31:
#if 1
			setPcWarpPoint(x, y);
			setPcDir(dir);
#else
			// ??????????????????
			if(partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0)
			{
				setPcWarpPoint(x, y);
				setPcDir(dir);
			}
#endif
			setPcAction(ANIM_STAND);
			break;

		// ??
		case 30:
			setPcDir(dir);
			break;

		// ?
		case 1:
			setPcAction(ANIM_WALK);
			break;

		// ??
		case 2:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_ATTACK);
			break;

		// ???
		case 3:
			setPcDir(dir);
			setPcAction(ANIM_THROW);
			break;

		// ????
		case 4:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_DAMAGE);
			break;

		// ?
		case 5:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_DEAD);
			break;

		// ??
		case 6:
			break;

		// ?????
		case 7:
			break;

		// ?????
		case 8:		/* UsedMagic */
			break;

		// ??
		case 10:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_DEAD);
			break;

		// ??
		case 11:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_SIT);
			break;

		// ??
		case 12:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_HAND);
			break;

		// ??
		case 13:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_HAPPY);
			break;

		// ?
		case 14:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_ANGRY);
			break;

		// ??
		case 15:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_SAD);
			break;

		// ???
		case 16:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_GUARD);
			break;

		// ??????
		case 17:
			setPcDir(dir);
			setPcAction(ANIM_WALK);
			break;

		// ????
		case 18:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_NOD);
			break;

		// ?????????
		case 19:
			//setPcWarpPoint(x, y);
			setPcDir(dir);
			setPcAction(ANIM_STAND);
			break;

		// 
		case 20:
			setPcWarpPoint(x, y);
			setPcDir(dir);
			break;

		// ????
		case 21:
			setPcWarpPoint(x, y);
			setPcDir(dir);
			if(effectno == 1)
			{
				// ???????
				setPcLeader();
#if 0
				if(pc.ptAct != NULL)
				{
					pc.ptAct->partyGx = -1;
					pc.ptAct->partyGy = -1;
				}
#endif
			}
			else
			{
				// ????????
				delPcLeader();
			}
			break;

		// ?
		case 22:
			setPcWarpPoint(x, y);
			setPcDir(dir);
			if(effectno == 1)
			{
				setPcWatch();
			}
			else
			{
				delPcWatch();
			}
			break;

		// ????????????
		case 23:
			setPcNameColor(effectno);
			break;
#ifdef _STREET_VENDOR
		case 41:
			{
				CHAREXTRA *ext;
				if(pc.ptAct == NULL) break;
				ext = (CHAREXTRA *)pc.ptAct->pYobi;
				if(ext->ptStreetVendor){
					DeathAction(ext->ptStreetVendor);
					ext->ptStreetVendor = NULL;
				}
				ext->ptStreetVendor = GetAction(PRIO_CHR,0);
			}
			break;
		case 42:
			{
				CHAREXTRA *ext;
				if(pc.ptAct == NULL) break;
				ext = (CHAREXTRA *)pc.ptAct->pYobi;
				if(ext->ptStreetVendor){
					DeathAction(ext->ptStreetVendor);
					ext->ptStreetVendor = NULL;
				}
			}
			break;
#endif

#ifdef _ANGEL_SUMMON
		case 34:
			setPcWarpPoint(x, y);
			setPcDir(dir);
			if(effectno == 1)
			{
				setPcAngel();
			}
			else
			{
				delPcAngel();
			}
			break;
#endif
#ifdef _ITEM_FIREWORK
		case 51:
			{
				CHAREXTRA *ext;

				if (pc.ptAct == NULL)
					break;
				ext = (CHAREXTRA *)pc.ptAct->pYobi;
				if (ext->pActFirework[0])
				{
					DeathAction(ext->pActFirework[0]);
					ext->pActFirework[0] = NULL;
				}
				if (ext->pActFirework[1])
				{
					DeathAction(ext->pActFirework[1]);
					ext->pActFirework[1] = NULL;
				}
				ext->pActFirework[0] = GetAction(PRIO_CHR, 0);
				ext->pActFirework[0]->damage		= effectno;	// 借用 damage 来储存 type (effectno = type)
				ext->pActFirework[0]->anim_chr_no	= effectparam1;
				ext->pActFirework[0]->anim_no		= ANIM_STAND;
				ext->pActFirework[0]->anim_ang		= 1;
				ext->pActFirework[0]->dispPrio		= DISP_PRIO_RESERVE;
				if (effectno == 3)
				{
					ext->pActFirework[0]->gx		= pc.ptAct->gx + 3;
					ext->pActFirework[0]->gy		= pc.ptAct->gy - 3;
					ext->pActFirework[0]->bufGx[0]	= pc.ptAct->gx;
					ext->pActFirework[0]->bufGy[0]	= pc.ptAct->gy;
				}
				else
				{
					ext->pActFirework[0]->gx		= pc.ptAct->gx;
					ext->pActFirework[0]->gy		= pc.ptAct->gy;
				}
				ext->pActFirework[0]->actNo			= 0;
			}
			break;
#endif
#ifdef ITEM_AURA_
		case 60:
		{
			CHAREXTRA* ext;

			if (pc.ptAct == nullptr)
				break;
			ext = (CHAREXTRA*)pc.ptAct->pYobi;

			if (ext == nullptr) break;

			if (ext->ptActAura1 == nullptr)
			{
				ext->ptActAura1 = GetAction(PRIO_CHR, 0);
			}

			if (ext->ptActAura1 != nullptr)
			{
				if (effectno == 0)
				{
					DeathAction(ext->ptActAura1);
					ext->ptActAura1 = nullptr;
				}
				else
				{
					ext->ptActAura1->anim_chr_no = effectno;
					ext->ptActAura1->dispPrio = DISP_PRIO_CHAR - 1;
					ext->ptActAura1->anim_ang = 0;
					ext->ptActAura1->anim_no = 0;
				}
			}
			break;
		}
		case 61:
		{
			CHAREXTRA* ext;

			if (pc.ptAct == nullptr)
				break;
			ext = (CHAREXTRA*)pc.ptAct->pYobi;

			if (ext == nullptr) break;

			if (ext->ptActAura2 == nullptr)
			{
				ext->ptActAura2 = GetAction(PRIO_CHR, 0);
			}

			if (ext->ptActAura2 != nullptr)
			{
				if (effectno == 0)
				{
					DeathAction(ext->ptActAura2);
					ext->ptActAura2 = nullptr;
				}
				else
				{
					ext->ptActAura2->anim_chr_no = effectno;
					ext->ptActAura2->dispPrio = DISP_PRIO_CHAR;
					ext->ptActAura2->anim_ang = 0;
					ext->ptActAura2->anim_no = 0;
				}
			}
			break;
		}
#endif

	}
}


// ?????? /////////////////////////////////////////////////////////
void clearPartyParam(void)
{
	int i;

	for(i = 0; i < MAX_PARTY; i++)
	{
		// ???????????????????????
		if(party[i].useFlag != 0)
		{
			if( party[i].id == pc.id)
			{
				// ??????????????
				if(party[i].ptAct != NULL)
				{
					party[i].ptAct->bufCount = 0;
				}
				delPcParty();
			}
			else
			{
				// ???????????????????????
				if((pc.status & CHR_STATUS_LEADER) != 0
				 && party[i].ptAct != NULL)
				{
					party[i].ptAct->bufCount = 0;
				}
				delCharParty(party[i].ptAct);
			}
		}
		party[i].useFlag = 0;
		party[i].id = 0;
		party[i].ptAct = NULL;
	}

	delPcLeader();
}


// ???ptAct?NULL?????
void clearPtActPartyParam(void)
{
	int i;

	for(i = 0; i < MAX_PARTY; i++)
	{
		party[i].ptAct = NULL;
	}
}





// ??????????????????? /////////////////////////////
int existCharacterListEntry(int index)
{
	if(index < 0 || index >= MAXCHARACTER)
		return -1;

	if(chartable[index].name[0] != '\0')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


// ????????????????
int cmpNameCharacterList(char *name)
{
	int i;

	for(i = 0; i < MAXCHARACTER; i++)
	{
		if(strcmp(name, chartable[i].name) == 0)
			return 1;
	}

	return 0;
}


// ??????????????????????????????????
int setCharacterList(char *name, char *opt)
{
	int index;

	makeStringFromEscaped(name);
	makeStringFromEscaped(opt);

	index = getIntegerToken(opt, '|', 1);

	if(index < 0 || index >= MAXCHARACTER)
		return -1;
#ifdef _NB_断线自动重连
	memset(PcLanded.登陆人物名称[index],0,32);
#endif
	memset(&chartable[index], 0, sizeof(CHARLISTTABLE));

	if(strlen(name) <= CHAR_NAME_LEN)
	{
		strcpy(chartable[index].name, name);
	}
	else
	{
		strcpy(chartable[index].name, "???");
	}
	chartable[index].faceGraNo	= getIntegerToken(opt, '|',  2);
	chartable[index].level		= getIntegerToken(opt, '|',  3);
	chartable[index].hp			= getIntegerToken(opt, '|',  4);
	chartable[index].str		= getIntegerToken(opt, '|',  5);
	chartable[index].def		= getIntegerToken(opt, '|',  6);
	chartable[index].agi		= getIntegerToken(opt, '|',  7);
	chartable[index].app		= getIntegerToken(opt, '|',  8);
	chartable[index].dp			= getIntegerToken(opt, '|',  9);
	chartable[index].attr[0]	= getIntegerToken(opt, '|', 10)/10;
	chartable[index].attr[1]	= getIntegerToken(opt, '|', 11)/10;
	chartable[index].attr[2]	= getIntegerToken(opt, '|', 12)/10;
	chartable[index].attr[3]	= getIntegerToken(opt, '|', 13)/10;
	chartable[index].login		= getIntegerToken(opt, '|', 14);
#ifdef _NB_断线自动重连
	strcpy(PcLanded.登陆人物名称[index],name);
#endif
	return 0;
}


// ????????????
//   int index : ???????0  ~ 7
int resetCharacterList(int index)
{
	if(index < 0 || index >= MAXCHARACTER)
		return -1;

	memset(&chartable[index], 0, sizeof(CHARLISTTABLE));

	return 0;
}


// ?????? /////////////////////////////////////////////////////////
void getItem(void)
{
	float tmpDir;
	float tmpX, tmpY;
	int dir;
	static unsigned int piSendTime = 0;

	// ????????????????????????
	if(windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMENU
	 || windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMAIN
	 || windowTypeWN == WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN)
		return;

	// ???????????????????
	if(ABS(nowGx - mouseMapGx) > 1
	 || ABS(nowGy - mouseMapGy) > 1)
		return;

	// ?????????????????
	if(!checkCharObjPoint(mouseMapGx, mouseMapGy,
		CHAROBJ_TYPE_NPC|CHAROBJ_TYPE_ITEM|CHAROBJ_TYPE_MONEY))
//		CHAROBJ_TYPE_ITEM|CHAROBJ_TYPE_MONEY))
		return;

	// ???????????
	tmpX = (float)(mouseMapGx - nowGx);
	tmpY = (float)(mouseMapGy - nowGy);
	tmpDir = Atan(tmpX, tmpY) + 22.5F - 45.0F*3;
	AdjustDir(&tmpDir);
	dir = (int)(tmpDir/45);

	// ??
	if(piSendTime+FIELD_BTN_PUSH_WAIT < TimeGetTime())
	{
		// ??????
		if(bNewServer) {
			lssproto_PI_send(sockfd, nowGx, nowGy, dir);
		}
		else
			old_lssproto_PI_send(sockfd, nowGx, nowGy, dir);
		piSendTime = TimeGetTime();
	}
}

#ifdef __TALK_TO_NPC

BOOL TalkToNPC(void)
{
	float tmpDir;
	float tmpX, tmpY;
	int dir;
	static unsigned int talkSendTime = 0;
	if(windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMENU
	 || windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMAIN
	 || windowTypeWN == WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN)
		return FALSE;

	if(nowVx != 0 || nowVy != 0)
		return FALSE;
	if(ABS(nowGx - mouseMapGx) > 2
	 || ABS(nowGy - mouseMapGy) > 2)
		return FALSE;
	if(!checkCharObjPoint(mouseMapGx, mouseMapGy, CHAROBJ_TYPE_NPC))
		return FALSE;
	tmpX = (float)(mouseMapGx - nowGx);
	tmpY = (float)(mouseMapGy - nowGy);
	tmpDir = Atan(tmpX, tmpY) + 22.5F;
	AdjustDir(&tmpDir);
	dir = (int)(tmpDir/45);
	if(talkSendTime+FIELD_BTN_PUSH_WAIT < TimeGetTime()){
		if(pc.ptAct->anim_ang != dir){
			char dir2[2];
			setPcDir(dir);

			dir2[0] = cnvServDir(dir, 1);
			dir2[1] = '\0';

			walkSendForServer(nowGx, nowGy, dir2);
		}
		lookAtAround();
		//CC
		//这里不应该被注销
		//是点npc说hi.

		//
		char dest[1024], m[1024];
		extern STR_BUFFER chatRegistryStr[];

		if(chatRegistryStr[7].cnt > 0)
			makeEscapeString(chatRegistryStr[7].buffer, dest, sizeof(dest));
		else
			makeEscapeString("hi", dest, sizeof(dest));
		sprintf(m, "P|%s", dest);
		if(bNewServer)
			lssproto_TK_send(sockfd, nowGx, nowGy, m, MyChatBuffer.color, NowMaxVoice);
		else
			old_lssproto_TK_send(sockfd, nowGx, nowGy, m, MyChatBuffer.color, NowMaxVoice);
		
		talkSendTime = TimeGetTime();
	}
	return TRUE;
}

#endif

// ??????? /////////////////////////////////////////////////////
void swapItem(int from, int to)
{
	if(from < 0 || to < 0)
		return;
//	swapShort(&pc.item[from].useFlag, &pc.item[to].useFlag);
	swap(pc.item[from].useFlag, pc.item[to].useFlag);
//	swapString(pc.item[from].name, pc.item[to].name);
	swap(pc.item[from].name, pc.item[to].name);
//	swapString(pc.item[from].name2, pc.item[to].name2);
	swap(pc.item[from].name2, pc.item[to].name2);
//	swapString(pc.item[from].memo, pc.item[to].memo);
	swap(pc.item[from].memo, pc.item[to].memo);
//	swapInteger(&pc.item[from].color, &pc.item[to].color);
	swap(pc.item[from].color, pc.item[to].color);
//	swapInteger(&pc.item[from].graNo, &pc.item[to].graNo);
	swap(pc.item[from].graNo, pc.item[to].graNo);
//	swapInteger(&pc.item[from].level, &pc.item[to].level);
	swap(pc.item[from].level, pc.item[to].level);
//	swapShort(&pc.item[from].field, &pc.item[to].field);
	swap(pc.item[from].field, pc.item[to].field);
//	swapShort(&pc.item[from].target, &pc.item[to].target);
	swap(pc.item[from].target, pc.item[to].target);
//	swapShort(&pc.item[from].deadTargetFlag, &pc.item[to].deadTargetFlag);
	swap(pc.item[from].deadTargetFlag, pc.item[to].deadTargetFlag);
//	swapShort(&pc.item[from].sendFlag, &pc.item[to].sendFlag);
	swap(pc.item[from].sendFlag, pc.item[to].sendFlag);
//	swapString(pc.item[from].damage, pc.item[to].damage);
	swap(pc.item[from].damage, pc.item[to].damage);
#ifdef _ITEM_PILENUMS
//	swapInteger(&pc.item[from].pile, &pc.item[to].pile);
	swap(pc.item[from].pile, pc.item[to].pile);
#endif
#ifdef _PET_ITEM
	swap(pc.item[from].type, pc.item[to].type);
#endif

}

// ????
/*void swapInteger(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}*/


/*void swapShort(short *a, short *b)
{
	short tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}*/


// ??????????255???????
/*void swapString(char *a, char *b)
{
	char tmp[256];

	if(strlen(a) > 255 || strlen(b) > 255)
		return;

	strcpy(tmp, a);
	strcpy(a, b);
	strcpy(b, tmp);
}
*/

// ?????????? /////////////////////////////////////////////////
BOOL lookAtAround(void)
{
	float tmpDir;
	float tmpX, tmpY;
	int dir;
	static unsigned int lSendTime = 0;

	// ????????????????????????
	if(windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMENU
	 || windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMAIN
	 || windowTypeWN == WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN)
		return FALSE;

	// ???????????????????
	if(ABS(nowGx - mouseMapGx) > 2
	 || ABS(nowGy - mouseMapGy) > 2)
		return FALSE;

	// ?????????????????
	if(!checkCharObjPoint(mouseMapGx, mouseMapGy, CHAROBJ_TYPE_LOOKAT))
		return FALSE;

	// ???????????
	tmpX = (float)(mouseMapGx - nowGx);
	tmpY = (float)(mouseMapGy - nowGy);
	if(tmpX == 0 && tmpY == 0)	// ??????????????
		return FALSE;

	tmpDir = Atan(tmpX, tmpY) + 22.5F - 45.0F*3;
	AdjustDir(&tmpDir);
	dir = (int)(tmpDir/45);

	// ??
	if(lSendTime+FIELD_BTN_PUSH_WAIT < TimeGetTime())
	{
		// ??????
		if(bNewServer)
			lssproto_L_send(sockfd, dir);
		else
			old_lssproto_L_send(sockfd, dir);
		lSendTime = TimeGetTime();
	}

	return TRUE;
}





// ????????? /////////////////////////////////////////////////////
BOOL addressBookFlag = FALSE;
ACTION *ptActAddressBookWin = NULL;
int addressBookX = 16;
int addressBookY = 16;
int addressBookPage = 0;
ACTION *ptActAddressBookChar[MAX_ADR_BOOK_COUNT];

// ????????
int addressBookSelectStrItem[1+3*MAX_ADR_BOOK_COUNT];
// 0 ... ??
// n*3+1 ... ?
// n*3+2 ... 
// n*3+3 ... ??


BOOL addressBookSendMsgFlag = FALSE;
ACTION *ptActAddressBookSendMsgWin = NULL;


void addressBookWindow1(void);


// ???????????
void initAddressBook(void)
{
	int i;

	addressBookFlag = FALSE;
	ptActAddressBookWin = NULL;
	memset(&ptActAddressBookChar, 0, sizeof(ptActAddressBookChar));
	addressBookPage = 0;
	for(i = 0; i < sizeof(addressBookSelectStrItem)/sizeof(int); i++)
	{
		addressBookSelectStrItem[i] = -2;
	}

	addressBookSendMsgFlag = FALSE;
	ptActAddressBookSendMsgWin = NULL;
}


// ?????????
void addressBookProc(void)
{
	int i;
	int no;

	// ??????????
	if(addressBookFlag
	 && ptActAddressBookWin == NULL)
	{
		// ??????????????
		ptActAddressBookWin = MakeWindowDisp(addressBookX, addressBookY, 4, 8, NULL, 1);
		// ??????????
		for(i = 0; i < MAX_ADR_BOOK_COUNT; i++)
		{
			no = addressBookPage*MAX_ADR_BOOK_COUNT+i;

			ptActAddressBookChar[i] = GetAction(PRIO_CHR, 0);
			if(ptActAddressBookChar[i] != NULL)
			{
				ptActAddressBookChar[i]->anim_chr_no = addressBook[no].graNo;
				ptActAddressBookChar[i]->atr = ACT_ATR_HIDE;
				ptActAddressBookChar[i]->anim_no = ANIM_STAND;
				ptActAddressBookChar[i]->dispPrio = DISP_PRIO_ITEM;
				ptActAddressBookChar[i]->x = addressBookX + 40;
				ptActAddressBookChar[i]->y = addressBookY + i * 110 + 80;
				pattern(ptActAddressBookChar[i], ANM_NOMAL_SPD, ANM_LOOP);
			}
		}
	}
	// ??????????
	else
	if(!addressBookFlag
	 && ptActAddressBookWin != NULL)
	{
		DeathAction(ptActAddressBookWin);
		ptActAddressBookWin = NULL;
		for(i = 0; i < MAX_ADR_BOOK_COUNT; i++)
		{
			if(ptActAddressBookChar[i] != NULL)
			{
				DeathAction(ptActAddressBookChar[i]);
				ptActAddressBookChar[i] = NULL;
			}
		}
	}
	// ???????
	else
	if(addressBookFlag
	 && ptActAddressBookWin != NULL)
	{
		addressBookWindow1();
	}
}


// ???????
void addressBookWindow1(void)
{
	int mode = 0;
	int i;
	int no;
	char msg[256];

	// ?????????????
	// ???????????????????
	if(mouse.onceState & MOUSE_LEFT_CRICK)
	{
		for(i = 0; i < sizeof(addressBookSelectStrItem)/sizeof(int); i++)
		{
			if(addressBookSelectStrItem[i] == HitFontNo)
			{
				mode = i + 1;
				break;
			}
		}
	}

	// ??????????????
	if(mode == 1)
	{
		addressBookFlag = FALSE;
		return;
	}
	// ?????????????????????
	else
	if((mode-2) < MAX_ADR_BOOK_COUNT*3
	 && ((mode-2) % 3) == 2)
	{
		if(bNewServer)
			lssproto_DAB_send(sockfd, addressBookPage*MAX_ADR_BOOK_COUNT+((mode-2)/3));
		else
			old_lssproto_DAB_send(sockfd, addressBookPage*MAX_ADR_BOOK_COUNT+((mode-2)/3));
	}

	// ?????????????????
	if(ptActAddressBookWin->hp >= 1)
	{
		for(i = 0; i < MAX_ADR_BOOK_COUNT; i++)
		{
			no = addressBookPage*MAX_ADR_BOOK_COUNT+i;

			if(ptActAddressBookChar[i] != NULL)
			{
				// ????????????????
				if(strlen(addressBook[no].name) > 0)
				{
					// ???????????
					// ???
					ptActAddressBookChar[i]->atr &= (~ACT_ATR_HIDE);
					ptActAddressBookChar[i]->anim_chr_no = addressBook[no].graNo;
					// 
					sprintf(msg, "NAME. %s", addressBook[no].name);
					StockFontBuffer(addressBookX + 68, addressBookY + i * 110 + 28,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0);
				
					// ???
					sprintf(msg, "LV. %d", addressBook[no].level);
					StockFontBuffer(addressBookX + 68, addressBookY + i * 110 + 46,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0);
					// ?????
					if(addressBook[i].onlineFlag != 0)
					{
						StockFontBuffer(addressBookX + 130, addressBookY + i * 110 + 46,
							FONT_PRIO_FRONT, FONT_PAL_WHITE, "线上", 0);
					}
					else
					{
						StockFontBuffer(addressBookX + 130, addressBookY + i * 110 + 46,
							FONT_PRIO_FRONT, FONT_PAL_GRAY, "断线", 0);
					}
					// ????
					addressBookSelectStrItem[i*3+1] =
						StockFontBuffer(addressBookX + 68, addressBookY + i * 110 + 64,
							FONT_PRIO_FRONT, FONT_PAL_YELLOW, "送信", 2);
					// ???
					addressBookSelectStrItem[i*3+2] =
						StockFontBuffer(addressBookX + 112, addressBookY + i * 110 + 64,
							FONT_PRIO_FRONT, FONT_PAL_YELLOW, "收信", 2);
					// ?????
					addressBookSelectStrItem[i*3+3] =
						StockFontBuffer(addressBookX + 156, addressBookY + i * 110 + 64,
							FONT_PRIO_FRONT, FONT_PAL_YELLOW, "删除", 2);
				}
				else
				{
					ptActAddressBookChar[i]->atr |= ACT_ATR_HIDE;
	
					ptActAddressBookChar[i]->anim_chr_no = 0;
				}
			}
		}

		addressBookSelectStrItem[0] =
			StockFontBuffer(addressBookX + 56, addressBookY + 352,
				FONT_PRIO_FRONT, FONT_PAL_YELLOW, "关闭", 2);
	}
}

int CHAR_getMaxHaveGold()
{
	int MaxGold;
#ifdef _FIX_MAXGOLD				// WON ADD 增加人物金钱上限
	int trans = pc.transmigration;
	switch(trans){
		case 0:
			MaxGold = 1000000;
			break;
		case 1:
			MaxGold = 2000000;
			break;
		case 2:
			MaxGold = 5000000;
			break;
		case 3:
			MaxGold = 10000000;
			break;
		case 4:
			MaxGold = 50000000;
			break;
		default:
			MaxGold = 100000000;
			break;
	}
#else
	MaxGold = MAX_GOLD;
#endif
	return MaxGold;
}

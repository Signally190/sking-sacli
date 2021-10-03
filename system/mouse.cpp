/************************/
/*	mouse.c				*/
/************************/
#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "../systeminc/loadrealbin.h"
#include "../systeminc/anim_tbl.h"
#include "../systeminc/pc.h"
#include "../systeminc/menu.h"
#include "../systeminc/battleMenu.h"
#include "../systeminc/ime_sa.h"
#include "../systeminc/map.h"
#include "../systeminc/login.h"

#ifdef _SASHIELD
#include "../systeminc/VMProtectSDK.h"
#endif

// ??????????
#define MOUSE_HIT_SIZE_X 48
#define MOUSE_HIT_SIZE_Y 48

// ??????????????
#define MOUSE_AUTO_REPEATE_TIME 100

// Robin

// ????
MOUSE mouse;
int	HitFontNo;
int HitDispNo;
char OneLineInfoStr[256];

int oneLineInfoFlag = 1;
int BoxColor;
extern int transmigrationEffectFlag;

void MouseInit(void)
{
	memset(&mouse, 0, sizeof(MOUSE));
	mouse.itemNo = -1;


}

void MouseNowPoint(int x, int y)
{
#ifdef _SASHIELD
	VMProtectBeginVirtualization("SAS_MMOVE");
	if (ResoMode == 1) {
		mouse.nowPoint.x = x * 2;
		mouse.nowPoint.y = y * 2;
	}
	else {
		mouse.nowPoint.x = x;
		mouse.nowPoint.y = y;
	}
	extern void RecordMouseMoveline(int x, int y);
	RecordMouseMoveline(x, y);
	VMProtectEnd();
#else
	if (ResoMode == 1) {
		if (x > 640)x = 640;
		if (y > 480)y = 480;
	}
	else {
		if (x > lpDraw->xSize)x = lpDraw->xSize;
		if (y > lpDraw->ySize)y = lpDraw->ySize;
	}
	// ?????
	//???????????????
	if (ResoMode == 1) {
		mouse.nowPoint.x = x * 2;
		mouse.nowPoint.y = y * 2;
	}
	else {
		mouse.nowPoint.x = x;
		mouse.nowPoint.y = y;
	}
#endif
}

// ????????????????????????? //////////////////////
void MouseCrickLeftDownPoint(int x, int y)
{
#ifdef _SASHIELD
	VMProtectBeginVirtualization("SAS_MCLK_D");
	mouse.state |= MOUSE_LEFT_CRICK;
	mouse.onceState |= MOUSE_LEFT_CRICK;
	mouse.autoState |= MOUSE_LEFT_CRICK;
	// ?????
	mouse.crickLeftDownPoint.x = x;
	mouse.crickLeftDownPoint.y = y;
	// ???????????????????
	mouse.beforeLeftPushTime = TimeGetTime();
	mouse.leftPushTime = 0;

	extern void SAS_CheckMouseClick(int x, int y);
	SAS_CheckMouseClick(x, y);
	VMProtectEnd();
#else
	// ????
	mouse.state |= MOUSE_LEFT_CRICK;
	mouse.onceState |= MOUSE_LEFT_CRICK;
	mouse.autoState |= MOUSE_LEFT_CRICK;
	// ?????
	mouse.crickLeftDownPoint.x = x;
	mouse.crickLeftDownPoint.y = y;
	// ???????????????????
	mouse.beforeLeftPushTime = TimeGetTime();
	mouse.leftPushTime = 0;
#endif
}

// ???????????????????????? ////////////////////////
void MouseCrickLeftUpPoint(int x, int y)
{
	// ????
	mouse.state &= (~MOUSE_LEFT_CRICK);
	mouse.onceState |= MOUSE_LEFT_CRICK_UP;
	mouse.autoState &= (~MOUSE_LEFT_CRICK);
	// ?????
	mouse.crickLeftUpPoint.x = x;
	mouse.crickLeftUpPoint.y = y;
	// ????????????????
	mouse.beforeLeftPushTime = 0;
	mouse.leftPushTime = 0;
}

// ??????????????????????? //////////////////////////
void MouseDblCrickLeftUpPoint(int x, int y)
{
	// ????
	//mouse.onceState |= ( MOUSE_LEFT_DBL_CRICK | MOUSE_LEFT_CRICK );
	mouse.state |= MOUSE_LEFT_CRICK;
	mouse.onceState |= MOUSE_LEFT_DBL_CRICK;
	mouse.onceState |= MOUSE_LEFT_CRICK;
	mouse.autoState |= MOUSE_LEFT_CRICK;
	// ?????
	mouse.crickLeftDownPoint.x = x;
	mouse.crickLeftDownPoint.y = y;
	// ???????????????????
	mouse.beforeLeftPushTime = TimeGetTime();
	mouse.leftPushTime = 0;
}

// ???????????????????????? ////////////////////////
void MouseCrickRightDownPoint(int x, int y)
{
	// ????
	mouse.state |= MOUSE_RIGHT_CRICK;
	mouse.onceState |= MOUSE_RIGHT_CRICK;
	mouse.autoState |= MOUSE_RIGHT_CRICK;
	// ?????
	mouse.crickRightDownPoint.x = x;
	mouse.crickRightDownPoint.y = y;
	// ???????????????????
	mouse.beforeRightPushTime = TimeGetTime();
	mouse.rightPushTime = 0;
}

// ???????????????????????? ////////////////////////
void MouseCrickRightUpPoint(int x, int y)
{
	// ????
	mouse.state &= (~MOUSE_RIGHT_CRICK);
	mouse.onceState |= MOUSE_RIGHT_CRICK_UP;
	mouse.autoState &= (~MOUSE_RIGHT_CRICK);
	// ?????
	mouse.crickRightUpPoint.x = x;
	mouse.crickRightUpPoint.y = y;
	// ????????????????
	mouse.beforeRightPushTime = 0;
	mouse.rightPushTime = 0;
}

// ??????????????????????? //////////////////////////
void MouseDblCrickRightUpPoint(int x, int y)
{
	// ????
	mouse.state |= MOUSE_RIGHT_CRICK;
	mouse.onceState |= MOUSE_RIGHT_DBL_CRICK;
	mouse.onceState |= MOUSE_RIGHT_CRICK;
	mouse.autoState |= MOUSE_RIGHT_CRICK;
	// ?????
	mouse.crickRightDownPoint.x = x;
	mouse.crickRightDownPoint.y = y;
	// ???????????????????
	mouse.beforeRightPushTime = TimeGetTime();
	mouse.rightPushTime = 0;
}

// ?????  ///////////////////////////////////////////////////////////////
void MouseProc(void)
{
	static UINT leftPushTimeBak;
	static UINT rightPushTimeBak;

	// ????
	mouse.onceState = MOUSE_NO_CRICK;
	// ???????????????????
	if (mouse.beforeLeftPushTime > 0)
	{
		mouse.leftPushTime = TimeGetTime() - mouse.beforeLeftPushTime;
	}
	// ???????????????????
	if (mouse.beforeRightPushTime > 0)
	{
		mouse.rightPushTime = TimeGetTime() - mouse.beforeRightPushTime;
	}
	// ?????????
	// ????????????
	// ?????????
	if (mouse.leftPushTime > 500) {
		// ???????
		if (leftPushTimeBak == 0) {
			mouse.autoState |= MOUSE_LEFT_CRICK; // ?????
			leftPushTimeBak = mouse.leftPushTime;	// ?????
		}
		else
			// ?????????
			if (mouse.leftPushTime - leftPushTimeBak >= MOUSE_AUTO_REPEATE_TIME) {
				mouse.autoState |= MOUSE_LEFT_CRICK; // ?????
				leftPushTimeBak = mouse.leftPushTime;	// ?????
			}
			else {	// ?????????
				mouse.autoState &= (~MOUSE_LEFT_CRICK);	// ??????
			}
	}
	else {
		mouse.autoState &= (~MOUSE_LEFT_CRICK);	// ??????
		leftPushTimeBak = 0;
	}

	// ????????????
	// ?????????
	if (mouse.rightPushTime > 500) {
		// ???????
		if (rightPushTimeBak == 0) {
			mouse.autoState |= MOUSE_RIGHT_CRICK; // ?????
			rightPushTimeBak = mouse.rightPushTime;	// ?????
		}
		else
			// ?????????
			if (mouse.rightPushTime - rightPushTimeBak >= MOUSE_AUTO_REPEATE_TIME) {
				mouse.autoState |= MOUSE_RIGHT_CRICK; // ?????
				rightPushTimeBak = mouse.rightPushTime;	// ?????
			}
			else {	// ?????????
				mouse.autoState &= (~MOUSE_RIGHT_CRICK);	// ??????
			}
	}
	else {
		mouse.autoState &= (~MOUSE_RIGHT_CRICK);	// ??????
		rightPushTimeBak = 0;
	}



	// ????????????
	//if( mouse.rightPushTime > 0 ){
	//}else mouse.state &= (~MOUSE_RIGHT_AUTO_CRICK);
	// ????????????
	//if( !WindowMode ){
	//if( 0 <= mouse.nowPoint.x && mouse.nowPoint.x < 640 &&
	//	0 <= mouse.nowPoint.y && mouse.nowPoint.y < 480 ){

	// ????????
	if (MouseCursorFlag == FALSE) {
		if (mouse.flag == FALSE) {
			StockDispBuffer(mouse.nowPoint.x + 16, mouse.nowPoint.y + 16, DISP_PRIO_MOUSE, CG_MOUSE_CURSOR, 0);
#ifdef _SHOW_MOUSE_POS
			char buf[20];
			wsprintf(buf, "%d,%d", mouse.nowPoint.x, mouse.nowPoint.y);
			StockFontBuffer(mouse.nowPoint.x, mouse.nowPoint.y - 15, FONT_PRIO_FRONT, 4, buf, 0);
#endif // _SHOW_MOUSE_POS

		}
	}

}
// ???????? *********************************************************/
void CheckGroupSelect(int no)
{
	int i;
	DISP_INFO 	*pDispInfo;
	DISP_SORT 	*pDispSort;
	/* ?????????????????????? */
	for (i = 0; i < DispBuffer.DispCnt; i++) {

		pDispInfo = DispBuffer.DispInfo + i;
		pDispSort = DispBuffer.DispSort + i;

		// ??????????
		if (pDispInfo->hitFlag == no) {
			// ??????????
			if (pDispSort->dispPrio >= DISP_PRIO_MENU) {
				// ????????????????
#ifndef __CARYTEST
				StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
					pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
					pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
					DISP_PRIO_BOX2, BoxColor, 0);
#else
				StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
					pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
					pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
					DISP_PRIO_BOX2, BoxColor, 0);
#endif
			}
			else {
				// ????????????????
#ifndef __CARYTEST
				StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
					pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
					pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
					DISP_PRIO_BOX, BoxColor, 0);
				//pDispSort->dispPrio, 250, 0 );
#else
				StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
					pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
					pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
					DISP_PRIO_BOX, BoxColor, 0);
#endif
			}
		}
	}
}

#if 1
// ????????????
UCHAR BoxColorTbl[] = {
	//255, 255, 255, 255, 255,
	//8,8,8,8,8,
	250,250,250,250,250,
	250,250,250,250,250,
	250,250,250,250,250,
	250,250,250,250,250,
	250,250,250,250,250,
	250,250,250,250,250,
	250,250,250,250,250,
	250,250,250,250,250,
	250,250,250,250,250,
	15,15,15,15,15,
	2,2,2,2,2,
	15,15,15,15,15,
	//250,250,250,250,250, 
	//8,8,8,8,8,
};
#else
UCHAR BoxColorTbl[] = {
	255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,
	7,7,7,7,7,7,7,7,7,7,
	248,248,248,248,248,248,248,248,248,248,
	0,0,0,0,0,0,0,0,0,0,
	248,248,248,248,248,248,248,248,248,248,
	7,7,7,7,7,7,7,7,7,7,
};
#endif

/* ??????????? **************************************************/
void HitMouseCursor(void)
{
	int i;					// ???????
	int strWidth;			// ???????
	int hitFlag = FALSE;	// ??????
	static int cnt = 0;		// ?????
	int itemNameColor = FONT_PAL_WHITE;	// ?????

	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt - 1;
	DISP_INFO 	*pDispInfo;

	// ??????
	if (!transmigrationEffectFlag)
		oneLineInfoFlag = 1;
	else
		oneLineInfoFlag = 0;

	if (cnt >= sizeof(BoxColorTbl) - 1) cnt = 0;
	else cnt++;

	// ?????
	BoxColor = BoxColorTbl[cnt];
#ifdef _ITEM_PATH
	ITEMPATHFLAG = FALSE;
#endif
	for (i = 0; i < FontCnt; i++) {
		// jl 文字不接受点击则跳过
		if (FontBuffer[i].hitFlag == 0) continue;
		// jl 取文字宽度
		strWidth = GetStrWidth(FontBuffer[i].str);
		// jl 检测鼠标在文字矩形内
		if (mouse.nowPoint.x <= FontBuffer[i].x + strWidth + 2 && FontBuffer[i].x - 2 <= mouse.nowPoint.x &&
			mouse.nowPoint.y <= FontBuffer[i].y + FONT_SIZE + 2 && FontBuffer[i].y - 2 <= mouse.nowPoint.y) {
			// jl 赋值点击的文字编号，鼠标在上面就赋值，不需要点击
			HitFontNo = i;
			// 如果hitFlag为2则画一个边框
			if (FontBuffer[i].hitFlag == 2) {
				StockBoxDispBuffer(FontBuffer[i].x - 3,
#ifdef _NEWFONT_
					FontBuffer[i].y - 2,
#else
					FontBuffer[i].y - 4,
#endif
					FontBuffer[i].x + strWidth + 2,
#ifdef _NEWFONT_
					FontBuffer[i].y + FONT_SIZE + 6,
#else
					FontBuffer[i].y + FONT_SIZE + 4,
#endif

					DISP_PRIO_BOX2, BoxColor, 0);

				SortDispBuffer();
			}
			//jl 点击的是文字，则把图片点击赋值-1
			HitDispNo = -1;
			mouse.level = DISP_PRIO_MENU;
			// jl状态栏显示提示文字
			if (TaskBarFlag == FALSE && oneLineInfoFlag) {
				ShowBottomLineString(itemNameColor, OneLineInfoStr);
				//the third StockFontBuffer( 8, 460, FONT_PRIO_FRONT, itemNameColor, OneLineInfoStr, 0 );
			}
			OneLineInfoStr[0] = NULL;
			return;		// jl 拿到HitDispNo就返回
		}
	}
	// jl 没有文字点击则赋值-1
	HitFontNo = -1;
	for (i = DispBuffer.DispCnt - 1; i >= 0; i--, pDispSort--) {
		pDispInfo = DispBuffer.DispInfo + pDispSort->no;
		if (pDispInfo->hitFlag == 0) continue;
		//if( SpriteInfo[ pDispInfo->bmpNo ].lpSurfaceInfo == NULL ) continue;
		if (LoadBmp(pDispInfo->bmpNo) == FALSE) continue;
		if (pDispInfo->pAct == NULL) {
#ifndef __CARYTEST
			if (mouse.nowPoint.x <= pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width &&
				pDispInfo->x <= mouse.nowPoint.x &&
				mouse.nowPoint.y <= pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height &&
				pDispInfo->y <= mouse.nowPoint.y)
#else
			if (mouse.nowPoint.x <= pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width &&
				pDispInfo->x <= mouse.nowPoint.x &&
				mouse.nowPoint.y <= pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height &&
				pDispInfo->y <= mouse.nowPoint.y)
#endif				
				hitFlag = TRUE;
		}else{
#ifndef __CARYTEST
			if (mouse.nowPoint.x <= pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width * 0.5 + MOUSE_HIT_SIZE_X * 0.5 &&
				pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width * 0.5 - MOUSE_HIT_SIZE_X * 0.5 <= mouse.nowPoint.x &&
				mouse.nowPoint.y <= pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height &&
				pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height - MOUSE_HIT_SIZE_Y <= mouse.nowPoint.y)
#else
			if (mouse.nowPoint.x <= pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width * 0.5 + MOUSE_HIT_SIZE_X * 0.5 &&
				pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width * 0.5 - MOUSE_HIT_SIZE_X * 0.5 <= mouse.nowPoint.x &&
				mouse.nowPoint.y <= pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height &&
				pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height - MOUSE_HIT_SIZE_Y <= mouse.nowPoint.y)
#endif				
				hitFlag = TRUE;
		}

		if (hitFlag == TRUE) {

#ifdef __ATTACK_MAGIC

			int cnt;

			for (cnt = 0; cnt < BATTLKPKPLYAERNUM; cnt++)
			{
				if (p_party[cnt] == pDispInfo->pAct)
					break;
			}

			// 左上到右下编号为6 , 7 , 8 , 9
			// 当仅能选择单排且目前所选择的在我方的位置
			//printf("\n BattleMyNo=%d  pDispInfo->hitFlag=%d",BattleMyNo,pDispInfo->hitFlag);
			if (BattleMyNo >= 10 && (6 == pDispInfo->hitFlag || 7 == pDispInfo->hitFlag))
			{
				hitFlag = FALSE;
				continue;
			}
#ifdef _SKILL_85_LSFIX
			// CAX修正攻击一排时候出错
			if (BattleMyNo < 10 && (10 == pDispInfo->hitFlag || 9 == pDispInfo->hitFlag))
#else
			// 当仅能选择单排且目前所选择的在我方的位置
			if (BattleMyNo < 10 && (8 == pDispInfo->hitFlag || 9 == pDispInfo->hitFlag))
#endif
			{
				//printf("\n BattleMyNo=%d  pDispInfo->hitFlag=%d",BattleMyNo,pDispInfo->hitFlag);
				hitFlag = FALSE;
				continue;
			}
#ifdef _SKILL_ADDBARRIER
			// 只能选我方
			if (BattleMyNo < 10 && (11 == pDispInfo->hitFlag || 12 == pDispInfo->hitFlag)){
				//printf("\n BattleMyNo=%d  pDispInfo->hitFlag=%d",BattleMyNo,pDispInfo->hitFlag);
				hitFlag = FALSE;
				continue;
			}
			if (BattleMyNo >= 10 && (12 == pDispInfo->hitFlag || 13 == pDispInfo->hitFlag)){
				hitFlag = FALSE;
				continue;
			}
#endif
#endif
			//			StockFontBuffer( pDispInfo->x, pDispInfo->y, FONT_PRIO_FRONT, 0, "Hit", 1 );
			HitDispNo = pDispSort->no;
			if (pDispInfo->hitFlag >= 2) {
				if (pDispSort->dispPrio >= DISP_PRIO_YES_NO_WND) {
#ifndef __CARYTEST
					StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
						pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
						pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
						//DISP_PRIO_BOX2, 250, 0 );
						DISP_PRIO_BOX3, BoxColor, 0);
#else
					StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
						pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
						pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
						DISP_PRIO_BOX3, BoxColor, 0);
#endif
				}
				else
					if (pDispSort->dispPrio >= DISP_PRIO_MENU) {
#ifndef __CARYTEST
						StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
							pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
							pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
							//DISP_PRIO_BOX2, 250, 0 );
							DISP_PRIO_BOX2, BoxColor, 0);
#else
						StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
							pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
							pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
							DISP_PRIO_BOX2, BoxColor, 0);
#endif
					}else {
#ifndef __CARYTEST
						StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
							pDispInfo->x + SpriteInfo[pDispInfo->bmpNo].width + 2,
							pDispInfo->y + SpriteInfo[pDispInfo->bmpNo].height + 2,
							//DISP_PRIO_BOX, 250, 0 );
							DISP_PRIO_BOX, BoxColor, 0);
						//pDispSort->dispPrio, 250, 0 );
#else
						StockBoxDispBuffer(pDispInfo->x - 2, pDispInfo->y - 2,
							pDispInfo->x + g_lpRealAdrn[pDispInfo->bmpNo].width + 2,
							pDispInfo->y + g_lpRealAdrn[pDispInfo->bmpNo].height + 2,
							DISP_PRIO_BOX, BoxColor, 0);
#endif
					}
					if (pDispInfo->hitFlag >= 3) CheckGroupSelect(pDispInfo->hitFlag);
					SortDispBuffer();
			}
			if (TaskBarFlag == FALSE){
				if (pDispInfo->pAct != NULL){
					if (pDispInfo->pAct->atr & ACT_ATR_INFO){
						if (ProcNo == PROC_GAME){
#ifdef _MOUSE_SHOW_INFO_FOR_HEAD
							extern short nameOverTheHeadFlag;
							if (!nameOverTheHeadFlag){//F11开启时，不显示
								char msg[256];
								sprintf( msg, "%s", pDispInfo->pAct->name);
								int left = GetStrWidth(msg) / 2;
								StockFontBuffer( pDispInfo->pAct->x-left, pDispInfo->pAct->y-10, FONT_PRIO_FRONT, 0, msg, 0 );
								//StockFontBuffer(pDispInfo->x + 20 - left, pDispInfo->y - 10, FONT_PRIO_FRONT, itemNameColor, pDispInfo->pAct->name, 0);

							}
#endif
#ifdef _STONEAGE_NG
							if (DllInterface.MouseShowName != NULL) {
								if (DllOptionCheck.MouseShowName == TRUE) {
									DllInterface.MouseShowName(pDispInfo);
								}
							}
#endif
							if (pDispInfo->pAct->atr & ACT_ATR_TYPE_PC){
								itemNameColor = pDispInfo->pAct->itemNameColor;
								if (pDispInfo->pAct->freeName[0] != NULL) {
									//sprintf_s( OneLineInfoStr,"%s [%s] Lv：%d  耐：%d／%d  气：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp );
									if (pDispInfo->pAct->petName[0] != NULL){
										if (pc.familyName[0] != NULL){
											sprintf_s(OneLineInfoStr, "{%s} %s [%s]    레벨：%d  체력：%d／%d 기력：%d [탑승]%s 레벨：%d",
												pc.familyName, pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp,
												pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
										}else{
											sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d  체력：%d／%d 기력：%d [탑승]%s 레벨：%d",
												pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp,
												pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
										}
									}else{
										if (pc.familyName[0] != NULL)
											sprintf_s(OneLineInfoStr, "{%s} %s [%s]    레벨：%d  체력：%d／%d 기력：%d",
												pc.familyName, pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp);
										else
											sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d  체력：%d／%d 기력：%d",
												pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp);
									}
								}else {
									//sprintf_s( OneLineInfoStr,"%s  Lv：%d  耐：%d／%d  气：%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp );
									if (pDispInfo->pAct->petName[0] != NULL)
										if (pc.familyName[0] != NULL)
											sprintf_s(OneLineInfoStr, "{%s} %s    레벨：%d  체력：%d／%d 기력：%d [탑승]%s 레벨：%d",
												pc.familyName, pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp, pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
										else
											sprintf_s(OneLineInfoStr, "%s    레벨：%d  체력：%d／%d 기력：%d [탑승]%s 레벨：%d",
												pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp, pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
									else
										if (pc.familyName[0] != NULL)
											sprintf_s(OneLineInfoStr, "{%s} %s    레벨：%d   체력：%d／%d 기력：%d",
												pc.familyName, pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp);
										else
											sprintf_s(OneLineInfoStr, "%s    레벨：%d   체력：%d／%d 기력：%d",
												pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp);
								}
							}else
								if (pDispInfo->pAct->atr & ACT_ATR_TYPE_OTHER_PC) {
#ifdef _MOUSE_DBL_CLICK
									if (mouseDblRightOn) {
										openServerWindow(WINDOW_MESSAGETYPE_MOUSEGETNAME, 0, 0, 0, pDispInfo->pAct->name);
										return;
									}
#endif
									itemNameColor = pDispInfo->pAct->itemNameColor;
									if (pDispInfo->pAct->freeName[0] != NULL) {
										//sprintf_s( OneLineInfoStr,"%s [%s]  Lv：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level );
										if (pDispInfo->pAct->petName[0] != NULL)
											if (pDispInfo->pAct->fmname[0] != NULL)
												sprintf_s(OneLineInfoStr, "{%s} %s [%s]    레벨：%d [탑승]%s 레벨：%d", pDispInfo->pAct->fmname, pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
											else
												sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d [탑승]%s 레벨：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
										else
											if (pDispInfo->pAct->fmname[0] != NULL)
												sprintf_s(OneLineInfoStr, "{%s} %s [%s]    레벨：%d", pDispInfo->pAct->fmname, pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level);
											else
												sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level);
									}else{
										//sprintf_s( OneLineInfoStr,"%s  Lv：%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
										if (pDispInfo->pAct->petName[0] != NULL)
											if (pDispInfo->pAct->fmname[0] != NULL)
												sprintf_s(OneLineInfoStr, "{%s} %s    레벨：%d [탑승]%s 레벨：%d", pDispInfo->pAct->fmname, pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
											else
												sprintf_s(OneLineInfoStr, "%s    레벨：%d [탑승]%s 레벨：%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
										else
											if (pDispInfo->pAct->fmname[0] != NULL) {
												sprintf_s(OneLineInfoStr, "{%s} %s    레벨：%d", pDispInfo->pAct->fmname, pDispInfo->pAct->name, pDispInfo->pAct->level);
											}
											else {
												sprintf_s(OneLineInfoStr, "%s    레벨：%d", pDispInfo->pAct->name, pDispInfo->pAct->level);
											}
									}
								}else
									if (pDispInfo->pAct->atr & ACT_ATR_TYPE_PET) {//对像单纯是宠物
										if (pDispInfo->pAct->freeName[0] != NULL) {
											sprintf_s(OneLineInfoStr, "%s    레벨：%d", pDispInfo->pAct->freeName, pDispInfo->pAct->level);
										}
										else {
											sprintf_s(OneLineInfoStr, "%s    레벨：%d", pDispInfo->pAct->name, pDispInfo->pAct->level);
										}
									}else
										if (pDispInfo->pAct->atr & ACT_ATR_TYPE_ITEM) {
											itemNameColor = pDispInfo->pAct->itemNameColor;
											sprintf_s(OneLineInfoStr, "%s", pDispInfo->pAct->name);
#ifdef _ITEM_PATH
											ITEMPATHFLAG = TRUE;
#endif
										}else
											if (pDispInfo->pAct->atr & ACT_ATR_TYPE_GOLD) {
												sprintf_s(OneLineInfoStr, "%s", pDispInfo->pAct->name);
											}else
												if (pDispInfo->pAct->atr & ACT_ATR_TYPE_OTHER) {
													sprintf_s(OneLineInfoStr, "%s", pDispInfo->pAct->name);
												}
						}else
							if (ProcNo == PROC_BATTLE) {
								if (pDispInfo->pAct->atr & ACT_ATR_TYPE_PET) {
									if (pDispInfo->pAct->freeName[0] != NULL) {
										sprintf_s(OneLineInfoStr, "%s    레벨：%d", pDispInfo->pAct->freeName, pDispInfo->pAct->level);
									}else {
										sprintf_s(OneLineInfoStr, "%s    레벨：%d", pDispInfo->pAct->name, pDispInfo->pAct->level);
									}
								}else
									if (pDispInfo->pAct->atr & ACT_ATR_TYPE_ITEM) {
										//itemNameColor = pDispInfo->pAct->itemNameColor;
										sprintf_s(OneLineInfoStr, "%s", pDispInfo->pAct->name);
									}else
										if (BattleMyNo < BATTLKPKPLYAERNUM) {
											if (((ATR_EQU *)pDispInfo->pAct->pYobi)->place_no == BattleMyNo) {
												if (pDispInfo->pAct->freeName[0] != NULL) {
													if (pDispInfo->pAct->onRide == 1)
														sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d 체력：%d／%d 기력：%d [탑승]%s 레벨：%d 체력：%d／%d",
															pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pDispInfo->pAct->mp,
															pDispInfo->pAct->petName, pDispInfo->pAct->petLevel, pDispInfo->pAct->petHp, pDispInfo->pAct->petMaxHp);
													else
														sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d 체력：%d／%d 기력：%d",
															pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pDispInfo->pAct->mp);
												}
												else {
													//sprintf_s( OneLineInfoStr,"%s  Lv：%d  耐：%d／%d  气：%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, BattleMyMp );
													//sprintf_s( OneLineInfoStr,"%s  Lv：%d  耐：%d／%d  气：%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pDispInfo->pAct->mp );
													if (pDispInfo->pAct->onRide == 1)
														sprintf_s(OneLineInfoStr, "%s    레벨：%d  체력：%d／%d 기력：%d [탑승]%s 레벨：%d 체력：%d／%d",
															pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pDispInfo->pAct->mp,
															pDispInfo->pAct->petName, pDispInfo->pAct->petLevel, pDispInfo->pAct->petHp, pDispInfo->pAct->petMaxHp);
													else
														sprintf_s(OneLineInfoStr, "%s    레벨：%d  체력：%d／%d 기력：%d",
															pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pDispInfo->pAct->mp);
												}
											}else
												if (((ATR_EQU *)p_party[BattleMyNo]->pYobi)->group_flg == ((ATR_EQU *)pDispInfo->pAct->pYobi)->group_flg) {
													//sprintf_s( OneLineInfoStr,"%s  Lv：%d  耐：%4d/%4d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
													if (pDispInfo->pAct->freeName[0] != NULL) {
														//sprintf_s( OneLineInfoStr,"%s [%s]  Lv：%d  耐：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
														if (pDispInfo->pAct->onRide == 1)
															sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d 체력：%d／%d [탑승]%s Lv：%d 체력：%d／%d",
																pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp,
																pDispInfo->pAct->petName, pDispInfo->pAct->petLevel, pDispInfo->pAct->petHp, pDispInfo->pAct->petMaxHp);
														else
															sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d 체력：%d／%d",
																pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp);
													}else {
														//sprintf_s( OneLineInfoStr,"%s  Lv：%d  耐：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
														if (pDispInfo->pAct->onRide == 1)
															sprintf_s(OneLineInfoStr, "%s    레벨：%d 체력：%d／%d [탑승]%s 레벨：%d 체력：%d／%d",
																pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp,
																pDispInfo->pAct->petName, pDispInfo->pAct->petLevel, pDispInfo->pAct->petHp, pDispInfo->pAct->petMaxHp);
														else
															sprintf_s(OneLineInfoStr, "%s    레벨：%d 체력：%d／%d",
																pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp);
													}
												}else{
													if (pDispInfo->pAct->freeName[0] != NULL) {
														//sprintf_s( OneLineInfoStr,"%s [%s]  Lv：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level );
														if (pDispInfo->pAct->onRide == 1)
#ifdef _CAX_LOOK_HPMP
															sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d 체력：%d [탑승]%s 레벨：%d 체력：%d",
																pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp,
																pDispInfo->pAct->petName, pDispInfo->pAct->petLevel, pDispInfo->pAct->petHp);
#else
															sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d [탑승]%s 레벨：%d",
																pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level,
																pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
#endif
														else
#ifndef _CAX_LOOK_HPMP
															sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d 체력：%d／%d",
																pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp);
#else
															sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d",
																pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level);
#endif
													}else{
														//sprintf_s( OneLineInfoStr,"%s  Lv：%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
														if (pDispInfo->pAct->onRide == 1)
#ifdef _CAX_LOOK_HPMP
															sprintf_s(OneLineInfoStr, "%s    레벨：%d 체력：%d [탑승]%s 레벨：%d 체력：%d",
																pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp,
																pDispInfo->pAct->petName, pDispInfo->pAct->petLevel, pDispInfo->pAct->petHp);
#else
															sprintf_s(OneLineInfoStr, "%s    레벨：%d [탑승]%s 레벨：%d",
																pDispInfo->pAct->name, pDispInfo->pAct->level,
																pDispInfo->pAct->petName, pDispInfo->pAct->petLevel);
#endif
														else
#ifdef _CAX_LOOK_HPMP
															sprintf_s(OneLineInfoStr, "%s    레벨：%d 체력：%d／%d",
																pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp);
#else
															sprintf_s(OneLineInfoStr, "%s    레벨：%d", pDispInfo->pAct->name, pDispInfo->pAct->level);
#endif
													}
												}
										}else{
#ifdef _CAX_LOOK_HPMP
											if (pDispInfo->pAct->freeName[0] != NULL) {
												//sprintf_s( OneLineInfoStr,"%s [%s]  Lv：%d  耐：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
												//sprintf_s( OneLineInfoStr,"%s [%s]  Lv：%d  耐：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
												//sprintf_s( OneLineInfoStr,"%s [%s]  Lv：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level );
												sprintf(OneLineInfoStr, "%s [%s]    레벨：%d 체력：%d [탑승]%s 레벨：%d 耐：%d",
													pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp,
													pDispInfo->pAct->petName, pDispInfo->pAct->petLevel, pDispInfo->pAct->petHp);
											}else{
												//sprintf_s( OneLineInfoStr,"%s  Lv：%d  耐：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
												//sprintf_s( OneLineInfoStr,"%s  Lv：%d  耐：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
												//sprintf_s( OneLineInfoStr,"%s  Lv：%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
												if (pDispInfo->pAct->petLevel == 0) {
													sprintf_s(OneLineInfoStr, "%s    레벨：%d 체력：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp);
												}
												else {
													sprintf(OneLineInfoStr, "%s [%s]    레벨：%d 체력：%d [탑승]%s 레벨：%d 체력：%d",
														pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp,
														pDispInfo->pAct->petName, pDispInfo->pAct->petLevel, pDispInfo->pAct->petHp);
												}
											}
#else
											if (pDispInfo->pAct->freeName[0] != NULL) {
												//sprintf_s( OneLineInfoStr,"%s [%s]  Lv：%d  耐：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
												sprintf_s(OneLineInfoStr, "%s [%s]    레벨：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level);
											}else{
												//sprintf_s( OneLineInfoStr,"%s  Lv：%d  耐：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
												sprintf_s(OneLineInfoStr, "%s    레벨：%d", pDispInfo->pAct->name, pDispInfo->pAct->level);
											}
#endif
										}
							}
					}
				}
				if (oneLineInfoFlag)
					ShowBottomLineString(itemNameColor, OneLineInfoStr);
			}
			OneLineInfoStr[0] = NULL;
			mouse.level = pDispSort->dispPrio;
			return;
		}
	}
	mouse.level = DISP_PRIO_TILE;
	HitDispNo = -1;
	if (TaskBarFlag == FALSE) {
		ShowBottomLineString(itemNameColor, OneLineInfoStr);
	}
	OneLineInfoStr[0] = NULL;

}



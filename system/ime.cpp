/************************/
/*	ime.cpp				*/
/************************/
#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "winnls32.h"
#include "../systeminc/font.h"
#include "../caryIme.h"
#include "../systeminc/menu.h"
#include "../systeminc/map.h"
#include "../systeminc/battleMenu.h"
#ifdef _TALK_WINDOW
#include "../systeminc/talkwindow.h"
#endif


DWORD dwInfo;
void ShowBottomLineString(int iColor, LPSTR lpstr)
{
	LPSTR lpstr1 = GetImeString();
	dwInfo = 0;
	if (!lpstr1 && lpstr[0] != 0) {
		dwInfo = 1;
		StockFontBuffer(8, 460 + DISPLACEMENT_Y, FONT_PRIO_FRONT, iColor, lpstr, 0);
	}
}

int strstr3(char* str, char* strSearch) {
	char* strTemp;
	char* strSearchTemp;
	int num = 0;
	while (*str != '\0') {
		strTemp = str;
		strSearchTemp = strSearch;
		while ((*strTemp == *strSearchTemp) && (*strTemp != '\0') && (*strSearchTemp != '\0')) {
			strTemp++;
			strSearchTemp++;
		}
		if (*strSearchTemp == '\0') {
			num++;
		}
		unsigned char code = *str;
		//判断是否为中文，是则地址加2，否则加1
		if (code >= 0x80)
			str += 2;
		else
			str++;
	}
	return num;
}


void NewShowBottomLineString(int iColor, LPSTR lpstr)
{
	int 屏幕X, 屏幕Y;
	int 显示X, 显示Y;
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
	LPSTR lpstr1 = GetImeString();
	dwInfo = 0;
	if (!lpstr1 && lpstr[0] != 0) {
		dwInfo = 1;
		if (mouse.nowPoint.x > 屏幕X) {
			显示X = mouse.nowPoint.x - 40 - strlen(lpstr);
		}
		else {
			显示X = mouse.nowPoint.x + 40;
		}
		if (mouse.nowPoint.y > 屏幕Y) {
			显示Y = mouse.nowPoint.y - 40;
		}
		else {
			显示Y = mouse.nowPoint.y + 40;
		}
		StockFontBuffer(显示X, 显示Y, FONT_PRIO_FRONT, iColor, lpstr, 0);
	}
}

// Terry add 2003/12/16 for 交易视窗开启时,显示物品说明不显示输入法
extern BOOL bShowItemExplain;
#include <Windows.h>
#pragma comment(lib, "Imm32.lib")

// end
//	输入法的处理
void ImeProc() {
	int 屏幕X, 屏幕Y;
	if (ResoMode == 分辨率::mode800) {
		屏幕X = 800;
		屏幕Y = 600;
	}
	else if (ResoMode == 分辨率::mode1024) {
		屏幕X = 1024;
		屏幕Y = 768;
	}
	else {
		屏幕X = 640;
		屏幕Y = 480;
	}
	// 显示输入法的名称
	// Terry fix 2003/12/16 for 交易视窗开启时,显示物品说明不显示输入法
	//if( TaskBarFlag == FALSE ){ //这行改成下一行
	//if(bShowItemExplain == FALSE){
	if (TaskBarFlag == FALSE && bShowItemExplain == FALSE) {
		LPSTR lpstr, lpstr1;
		//去掉输入法的显示
		/*
		LPSTR lpstr=GetImeString();
		if(lpstr){
			StockFontBuffer(临时图标X-60,临时图标Y-60,FONT_PRIO_FRONT,FONT_PAL_WHITE,lpstr,0);
#ifdef _TALK_WINDOW
			if(g_bTalkWindow) TalkWindow.Update();
#endif
		}
		*/
		lpstr1 = GetImeDescString();
		if (1 != dwInfo) {
			//cary 秀出ping的时间
			int FontColor = 0;
			char* 频道显示名[] = {
				"일반채팅",
				"귓 속 말",
				"파티채팅",
				"부족채팅",
				//#ifdef _CHAR_PROFESSION
								"경력채널",
								//#endif
								//#ifdef _CHATROOMPROTOCOL
												"채널방채널",
												//#endif
												//#ifdef _CHATWORLDWIDEPROTOCOL
																"월  드",
																//#endif
																//#ifdef _NB_CHANNEL_ALLSERV
																				"사용안함",
																				//#endif
			};

			lpstr1 = 频道显示名[TalkMode];
			StockFontBuffer(10, 屏幕Y - 20, FONT_PRIO_FRONT, FontColor, lpstr1, 0);

#ifdef _SHOW_mouse
			char buf[20];
			wsprintfA(buf, "Mouse:%d,%d", mouse.nowPoint.x, mouse.nowPoint.y);
			StockFontBuffer(150, 460 + DISPLACEMENT_Y, FONT_PRIO_FRONT, FontColor, buf, 0);
#endif
#ifdef _NB_SHOW_MAPNAME
			char mapname[128];
#ifdef _NB_管理模式
			sprintf(mapname, "[%s]동:%d 남:%d", nowFloorName , nowGx, nowGy);
			StockFontBuffer(屏幕X / 2 - 130, 屏幕Y - 20, FONT_PRIO_FRONT, 4, mapname, 0);
			//sprintf(mapname, "[%s(%d)]동:%d 남:%d Mouse:%d.%d", nowFloorName, nowFloor, nowGx, nowGy, mouse.nowPoint.x, mouse.nowPoint.y);
			//StockFontBuffer(屏幕X / 2 - 130, 屏幕Y - 20, FONT_PRIO_FRONT, 4, mapname, 0);
#else
			sprintf(mapname, "[%s(%d)]동:%d 남:%d", nowFloorName, nowFloor, nowGx, nowGy);
			StockFontBuffer(屏幕X / 2 - 150, 屏幕Y - 20, FONT_PRIO_FRONT, 4, mapname, 0);
#endif
#endif
			lpstr = "수동모드";
			switch (AI) {
			case AI_ATTACK:
				lpstr = "강제공격";
				break;
			case AI_GUARD:
				lpstr = "필수방어";
				break;
			case AI_SELECT:
				lpstr = "ＡＩ모드";
				break;
			}
			StockFontBuffer(540 + DISPLACEMENT_X, 460 + DISPLACEMENT_Y, FONT_PRIO_FRONT, FontColor, lpstr, 0);
		}
	}
	// Terry add 2003/12/16 for 交易视窗开启时,显示物品说明不显示输入法
	bShowItemExplain = FALSE;
	// end
}


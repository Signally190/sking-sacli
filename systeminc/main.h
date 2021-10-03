// main.cpp ???????

#ifndef _MAIN_H_
#define _MAIN_H_

#include "version.h"

//---------------------------------------------------------------------------//
// ?? ：????define)?                                                //
//---------------------------------------------------------------------------//
#define DEF_APPNAME		"ＳｔｏｎｅＡｇｅ"				//???????
#ifdef _NEW_RESOMODE
//#define DEF_APPSIZEX	800 	 						//???WINDOWS???
//#define DEF_APPSIZEY	600								//???WINDOWS??
//#define DEF_APPSIZEX	1024	 						//???WINDOWS???
//#define DEF_APPSIZEY	768								//???WINDOWS??
#else
#define DEF_APPSIZEX	640							//???WINDOWS???
#define DEF_APPSIZEY	480								//???WINDOWS??
#endif
//#define DEF_APPSIZEX	320								//???WINDOWS???
//#define DEF_APPSIZEY	240								//???WINDOWS??

extern int DEF_APPSIZEX;
extern int DEF_APPSIZEY;
extern int SCREEN_WIDTH_CENTER;
extern int SCREEN_HEIGHT_CENTER;

//#define SCREEN_WIDTH_CENTER			DEF_APPSIZEX/2
//#define SCREEN_HEIGHT_CENTER		DEF_APPSIZEY/2

//---------------------------------------------------------------------------//
// ?? ：?????????                                               //
//---------------------------------------------------------------------------//

int PASCAL WinMain( HINSTANCE 		hInstance, 	HINSTANCE 		hPrevInstance, 	LPSTR lpCmdLine, 	int nCmdShow );	// ?????

LRESULT CALLBACK PASCAL WindMsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );				//??????????
BOOL SystemTask( void );																		// ??????????

// ??????
extern HWND hWnd;		// ?????????
extern BOOL WindowMode;		// ????????
extern int ResoMode;
extern int LowResoCmdFlag;		// ??????????
extern int	CmdShow;	// WinMain??????????????
extern LPSTR CmdLine; 	// WinMain?????????????????????
extern HANDLE hMutex;	// ?????????????????

extern char realBinName[];
extern char adrnBinName[];
extern char sprBinName[];
extern char sprAdrnBinName[];
#ifdef _READ16BITBMP
extern char realtrueBinName[];
extern char adrntrueBinName[];
#endif
//add jeffrey
extern struct gameserver gmsv[];
extern struct gamegroup  gmgroup[];
//end


extern BOOL offlineFlag;
extern char DebugKey0[ 256 ];		
extern char DebugKey1[ 256 ];		
extern char DebugKey2[ 256 ];		


#endif


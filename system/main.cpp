/**** SYSTEM INCLUDE ****/
#include <chrono>
#include <ctime>
#include <thread>
#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include <winnls32.h>
#include <ime.h>
#include <imm.h>
#include <tlhelp32.h>
#include "../caryIme.h"
#include "../resource.h"
#include "../systeminc/netmain.h"
#include "../systeminc/battleMap.h"
#include "../systeminc/battleProc.h"
#include "../systeminc/netmain.h"
#include "../systeminc/login.h"
#include "../systeminc/map.h"
#include "../systeminc/pc.h"
#ifdef _REMAKE_20
#include "../MMOGprotect.h"
#endif
#ifdef _TALK_WINDOW
#include "../systeminc/talkwindow.h"
#endif

#ifdef _STONDEBUG_
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


//JL
#include "../systeminc/JL_func.h"
#include "..\winlua.h"
HINSTANCE StoneageNg=NULL;




#include "../systeminc/CCA_ServerListCtrl.h"
// ?
// [9/14/2010 %WXY%]-------------------------BEGIN----------------------
//#define REALBIN_DIR		"data\\real.bin" 		// REAL.BIN?? 
//#define ADRNBIN_DIR		"data\\adrn.bin" 		// ADRN.BIN??
//#define SPRBIN_DIR		"data\\spr.bin" 		// SPR.BIN??
//#define SPRADRNBIN_DIR	"data\\spradrn.bin" 	// SPRADRN.BIN??
//#ifdef _READ16BITBMP
//#define REALTRUEBIN_DIR "data/realtrue.bin"
//#define ADRNTRUEBIN_DIR "data/adrntrue.bin"
//#endif
//゜挂眳ゴ腔测钨 党蜊珨狟醴翘腔恅璃靡秪峈祥⑤餍

#define REALBIN_DIR		"data\\real.bin" 		// REAL.BIN?? 
#define ADRNBIN_DIR		"data\\adrn.bin" 		// ADRN.BIN??
#define SPRBIN_DIR		"data\\spr.bin" 		// SPR.BIN??
#define SPRADRNBIN_DIR	"data\\spradrn.bin" 	// SPRADRN.BIN??
#ifdef _READ16BITBMP
#define REALTRUEBIN_DIR "data/realtrue_13.bin"
#define ADRNTRUEBIN_DIR "data/adrntrue_5.bin"
#endif
//-------------------------------------------END------------------------
//#define PATCHER_RUN
#define PARAM_ARGS "HASH___________@@@@@@@@"

#ifdef _NB_断线自动重连
Landed PcLanded;
int 是否重登 = FALSE;
int 是否重登战斗了 = FALSE;
int 是否重登AI模式 = FALSE;
int 是否重登组队 = FALSE;
int 是否重登喊话 = FALSE;
int 是否重登人物方向 = FALSE;
int 是否重开登组队 = FALSE;
int 自动登陆是否开启 = 0;
#endif
//---------------------------------------------------------------------------//
// 概要 ：グローバル変数定義														//
//---------------------------------------------------------------------------//
HINSTANCE	hInst;			// インスタンスハンドル定義
HWND hWnd = 0;					// 窗口句柄
int	CmdShow;				// WinMain関数の引数をグローバルにする
LPSTR CmdLine;				// WinMain関数のコマンドライン引数をグローバルにする
HANDLE hMutex;				// 同時起動チェックオブジェクトのハンドルix n
HANDLE hCheckMutex = NULL;	// 给更新程式检查用的

//BOOL WindowMode = TRUE;		// ウィンドウモード
BOOL WindowMode = FALSE;	// フルスクリーンモード
							//	TRUE  : WindowMode
							//	FALSE : FullScreen
BOOL NoDelay = TRUE;		// TCP_NODELAY を使うかどうか。add by ringo
//#define _NEW_RESOMODE

#ifdef _NEW_RESOMODE
//解析度模式( 0 : 640 * 480 , 3 : 1024 * 768 , 4 : 800 * 600 ) 
//int ResoMode = 3;
#else
int ResoMode = 0;			
#endif
int LowResoCmdFlag = 0;		// 解像度変更コマンドフラグ

RECT 	g_clientRect;
POINT g_clientPoint;
RECT 	g_moveRect;


BOOL offlineFlag = FALSE;	// オフラインフラグ
char DebugKey0[ 256 ];		// デバックキー０
char DebugKey1[ 256 ];		// デバックキー１
char DebugKey2[ 256 ];		// デバックキー２
int g_iMallocCount = 0;


char realBinName[256];
char adrnBinName[256];
char sprBinName[256];
char sprAdrnBinName[256];
#ifdef _READ16BITBMP
char realtrueBinName[256];
char adrntrueBinName[256];
extern BOOL g_bUseAlpha;
#endif
#ifdef _TALK_WINDOW
extern int MouseCursorFlag;
#endif

void GetSettings();
/* コマンドライン解析 */
void AnalyzeCmdLine( void );
// ウィンドウモード切り替え関数宣言
void ChangeWindowMode( void );
// 解像度モード設定処理
void SetResoMode(int Mode);

// ?????????
WNDCLASS wndclass;

HANDLE	hProcessSnap = NULL, hParentProcess = NULL;
DWORD	dwPID;
PROCESSENTRY32 pe32;

#ifdef _TIME_GET_TIME
extern LARGE_INTEGER tickCount;
extern LARGE_INTEGER CurrentTick;
#endif

#ifdef _TALK_WINDOW
CTalkWindow TalkWindow;
#endif

extern int 编码;
extern char* GB2312ToBIG5(const char* szGBString);
int MessageBoxNew(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	if (编码 == 950) {
		char 繁体[1024] = { 0 };
		char 繁体1[1024] = { 0 };
		LCMapString(0x804, 0x4000000, lpText, strlen(lpText), 繁体, 1024);
		LCMapString(0x804, 0x4000000, lpCaption, strlen(lpCaption), 繁体1, 1024);
		return MessageBox(hWnd, GB2312ToBIG5((const char *)繁体), GB2312ToBIG5((const char *)繁体1), uType);
	}
	else {
		return MessageBox(hWnd, lpText, lpCaption, uType);
	}
}


void CreateCompatibleDEVMODE(DEVMODE* pdm, DWORD BitsPerPixel, DWORD Width, DWORD Height, DWORD Frequency)
{
	ZeroMemory(pdm, sizeof(DEVMODE));
	pdm->dmSize = sizeof(DEVMODE);

	if (BitsPerPixel)
	{
		pdm->dmBitsPerPel = BitsPerPixel;
		pdm->dmFields |= DM_BITSPERPEL;
	}

	if (Width)
	{
		pdm->dmPelsWidth = Width;
		pdm->dmFields |= DM_PELSWIDTH;
	}

	if (Height)
	{
		pdm->dmPelsHeight = Height;
		pdm->dmFields |= DM_PELSHEIGHT;
	}

	if (Frequency)
	{
		pdm->dmDisplayFrequency = Frequency;
		pdm->dmFields |= DM_DISPLAYFREQUENCY;
	}
}

void detectSpeedHack()
{
	int count = 0;
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	while (1) {
		auto new_end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = new_end - end;
		std::time_t end_time = std::chrono::system_clock::to_time_t(new_end);
		end = new_end;
		if (float(elapsed_seconds.count()) > 2) {
			Sleep(2000);
			exit(0);
		}
		Sleep(1000);
	}
}

int PASCAL WinMain( HINSTANCE hInstance ,HINSTANCE hPrevInstance ,LPSTR lpCmdLine ,int nCmdShow )
{
	std::thread t1(detectSpeedHack);
#ifdef PATCHER_RUN
	if (strcmp(PARAM_ARGS, lpCmdLine) != 0)
	{
		MessageBox(NULL, "RUN PATCHER", "ERROR", MB_OK);
		return 0;
	}
#endif
	 //加载动态的列表
//	 写入serverMax();
	//LoadStoneAgeLUA(_LUA_PATCH_);
#ifdef _NB_CMD_MODE
	extern void InitConsoleWindow(void)  ;
	InitConsoleWindow();
#endif
#ifndef SWITCH_MODE
	extern int displayBpp;
	HDC hDcDest = GetDC(hWnd);
	displayBpp = GetDeviceCaps(hDcDest, BITSPIXEL);
	ReleaseDC(hWnd, hDcDest);
	if (displayBpp != 32) {
		DEVMODE dm;
		CreateCompatibleDEVMODE(&dm, 32, GetDeviceCaps(hDcDest, HORZRES), GetDeviceCaps(hDcDest, VERTRES), GetDeviceCaps(hDcDest, VREFRESH));
		ChangeDisplaySettings(&dm, 0);
		return FALSE;
	}
#endif

	hInst = hInstance;		// パラメーターでもらったインスタンスハンドルをグローバールに代入
	CmdShow = nCmdShow;		// WinMain関数の引数をグローバルにする
	CmdLine = lpCmdLine;	// WinMain関数のコマンドライン引数をグローバルにする

#ifndef _NB_客户端直接启动
	if( !strstr( CmdLine, "updated" ) ){
		MessageBox( NULL, "此执行档不能起动！\n，请执行“启动石器时代”\n或执行“StoneAge.exe”！", "확인", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
#endif

	编码 = 949;// GetACP();

	// 建立一个核心物件,让更新程式可以判断是否有石器正在执行
	hCheckMutex = CreateMutex(NULL, FALSE, "CheckForUpdate");

#ifdef _STONDEBUG_
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
#ifdef __NEW_CLIENT
#ifdef __KOREA
	//ADD jeffrey
	char ip[16];
	int startindex = 0;

	ZeroMemory( gmsv, sizeof(gameserver)*MAX_GMSV);
	ZeroMemory( gmgroup, sizeof(gamegroup)*MAX_GMGROUP);
	char *temp = strstr(CmdLine , "&");
	strtok(temp , ";");
	char *tmp = strtok(NULL , ";");
	extern int nGroup;
	nGroup = atoi(tmp);                          //群组数
	for(int z = 0 ; z < nGroup ; z++)
	{
		char *universes = strtok(NULL , ";");        //星系名称
		strcpy( gmgroup[z].name, universes);
		char *numtmp = strtok(NULL , ";");           //星球数
		int num = atoi(numtmp);
		gmgroup[z].num = num;
		gmgroup[z].startindex = startindex;
		startindex += num;
		gmgroup[z].used = 1;
		char *szIP = strtok(NULL , ";");             //星系IP
		for(int v = gmgroup[z].startindex ; v < startindex ; v++)
		{
			char *name = strtok(NULL , ";");         //星球名称
			strcpy( gmsv[v].name, name);
			char *lastip = strtok(NULL , ";");       //星球ip
			sprintf(ip , "%s.%s" , szIP , lastip);
			strcpy( gmsv[v].ipaddr, ip);
			char *port = strtok(NULL , ";");      //port
			strcpy( gmsv[v].port, port);
			gmsv[v].used = '1';
		}
	}
	//END
#endif
	dwPID = GetCurrentProcessId();
	if( INVALID_HANDLE_VALUE == (hProcessSnap=CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL)))
		return 0;
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if( Process32First( hProcessSnap, &pe32)){ 
			do{
				if( pe32.th32ProcessID == dwPID){
					dwPID = pe32.th32ParentProcessID;
					break;
				}
			}while( Process32Next( hProcessSnap, &pe32));
		}
	}
#endif

    if( !hPrevInstance ){
		wndclass.style = CS_BYTEALIGNCLIENT | CS_DBLCLKS;	// クラススタイル
		wndclass.lpfnWndProc = WindMsgProc;		// ウインドウプロシージャーアドレス定義
		wndclass.cbClsExtra = 0;				// クラス毎の補足定義なし
		wndclass.cbWndExtra = 0;				// ウインドウ毎の補足データ無し
		wndclass.hInstance = hInstance;			// インスタンスハンドル定義
	    wndclass.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(SA_ICON2));
		wndclass.hCursor = LoadCursor(hInstance ,MAKEINTRESOURCE(SA_MOUSE));
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);// ウィンドウの背景色
		wndclass.lpszMenuName= NULL;			// メニューなし
		wndclass.lpszClassName = DEF_APPNAME;	// タイトルバー文字設定
		if( !RegisterClass(&wndclass) ){
			MessageBox( NULL, "初始化视窗失败！\n，请执行重新安装或洽询客服人员！",
				"확인", MB_OK | MB_ICONSTOP );
			return FALSE;						// 作成に失敗したら終了する
		}
	}
#ifdef _TIME_GET_TIME
	QueryPerformanceFrequency(&tickCount);
	tickCount.QuadPart = tickCount.QuadPart/1000;
#endif
	// 読み込みファイル名の設定
	strcpy( realBinName, REALBIN_DIR );			// REAL.BINの場所
	strcpy( adrnBinName, ADRNBIN_DIR );			// ADRN.BINの場所
	strcpy( sprBinName, SPRBIN_DIR );			// SPR.BINの場所
	strcpy( sprAdrnBinName, SPRADRNBIN_DIR );	// SPRADRN.BINの場所
#ifdef _READ16BITBMP
	strcpy(realtrueBinName,REALTRUEBIN_DIR);
	strcpy(adrntrueBinName,ADRNTRUEBIN_DIR);
#endif
	//コマンドライン解析 
	AnalyzeCmdLine();
	memset(lpCmdLine, 0, strlen(lpCmdLine));
	// 解像度モード設定

	SetResoMode( ResoMode );
	// ウィンドウの作成
	ChangeWindowMode();
#ifdef _TALK_WINDOW
	TalkWindow.Init(hWnd,hInstance);
	if(g_bTalkWindow) TalkWindow.Create();
#endif
#ifdef _NB_断线自动重连
	PcLanded.大区 = PcLanded.人物 = PcLanded.小区 = PcLanded.队模 = -1;
	PcLanded.是否自动喊话 = PcLanded.是否自动遇敌 = PcLanded.人物方向 = PcLanded.登陆延时时间 = FALSE;
	memset(PcLanded.登陆人物名称, 0, 4 * 32);
#endif
	void SetTestServer();
//	SetTestServer();
	GameMain();

#ifdef __NEW_CLIENT
	CloseHandle( hProcessSnap);
#endif

#ifdef SWITCH_MODE
	extern DEVMODE g_OriginalMode;
	if( g_OriginalMode.dmSize != 0){
		DEVMODE	DevMode;
		DevMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &DevMode);
		if( DevMode.dmPelsWidth!=g_OriginalMode.dmPelsWidth || DevMode.dmPelsHeight!=g_OriginalMode.dmPelsHeight || DevMode.dmBitsPerPel!=g_OriginalMode.dmBitsPerPel){
			g_OriginalMode.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
			ChangeDisplaySettingsEx( NULL, &g_OriginalMode, NULL, CDS_UPDATEREGISTRY, NULL);
		}
	}
#endif
#ifdef _REMAKE_20
#ifndef _STONDEBUG_
	QuitingInputScript();
#endif
	QuitRestore();
#endif
#ifdef _STONDEBUG_
	if(g_iMallocCount != 0){
		char MSG[256];
		sprintf(MSG,"g_iMallocCount = %d",g_iMallocCount);
		MessageBox( NULL,MSG,"확인", MB_OK | MB_ICONSTOP );
	}
#endif
	ReleaseMutex(hCheckMutex);

	return FALSE;
}

#ifdef _STONDEBUG_

// デバックキー解析 ***********************************************************/
void DebugKey( char *str )
{
	char *work;
	int i = 0;
	
	// コマンドラインオプションに key0: があるとき
	if( ( work = strstr( str, "KEY0:" ) ) )
	{
		work += 5;	// 文字列のあるところまでポインタ進める
		// : がくるまでループ
		while( *work != ':' ){
			DebugKey0[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}
	DebugKey0[ i ] = NULL;	// 終端記号
	i = 0;
	// コマンドラインオプションに key1: があるとき
	if( ( work = strstr( str, "KEY1:" ) ) )
	{
		work += 5;	// 文字列のあるところまでポインタ進める
		// : がくるまでループ
		while( *work != ':' ){
			DebugKey1[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}	
	DebugKey1[ i ] = NULL;	// 終端記号
	i = 0;
	// コマンドラインオプションに key1: があるとき
	if( ( work = strstr( str, "KEY2:" ) ) )
	{
		work += 5;	// 文字列のあるところまでポインタ進める
		// : がくるまでループ
		while( *work != ':' ){
			DebugKey2[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}	
	DebugKey2[ i ] = NULL;	// 終端記号
}

#endif
/* コマンドライン解析 *********************************************************/
extern BOOL bNewServer;
void AnalyzeCmdLine( void )
{
	char *addr;

//	MessageBox(hWnd, CmdLine, "Error", MB_OK);
	// 読み込みファイル名の変更
	if( addr = strstr( CmdLine, "realbin:" ) )
		sprintf( realBinName, "data\\real_%d.bin", atoi(addr+strlen("realbin:")) );
	if( addr = strstr( CmdLine, "adrnbin:" ) )
		sprintf( adrnBinName, "data\\adrn_%d.bin", atoi(addr+strlen("adrnbin:")) );
	if( addr = strstr( CmdLine, "sprbin:" ) )
		sprintf( sprBinName, "data\\spr_%d.bin", atoi(addr+strlen("sprbin:")) );
	if( addr = strstr( CmdLine, "spradrnbin:" ) )
		sprintf( sprAdrnBinName, "data\\spradrn_%d.bin", atoi(addr+strlen("spradrnbin:")) );

#ifdef _READ16BITBMP
	if (addr = strstr(CmdLine, "realtrue:"))
		sprintf_s(realtrueBinName, "data\\realtrue_%d.bin", atoi(addr + strlen("realtrue:")));
	if (addr = strstr(CmdLine, "adrntrue:"))
		sprintf_s(adrntrueBinName, "data\\adrntrue_%d.bin", atoi(addr + strlen("adrntrue:")));
	if (strstr(CmdLine, "usealpha")) g_bUseAlpha = TRUE;
#endif

//#ifdef CC_HOST
//	if(strstr(CmdLine,"usealpha")) g_bUseAlpha = TRUE;
//#else
//	if(strstr(CmdLine,"usealpha")) g_bUseAlpha = TRUE;
//#endif
	WindowMode = TRUE;
	if( strstr( CmdLine, "nodelay" ) )
		NoDelay = FALSE;

	ProcNo = PROC_ID_PASSWORD;

}

void ChangeWindowMode( void )
{
	RECT clientRect;		// クライアントの表示領域
	DWORD windowStyle;		// ウィンドウスタイル
	ChangeMode(WindowMode);
	// ウィンドウモードの時
	if( WindowMode ){
		// ウィンドウスタイル
		windowStyle = WS_MINIMIZEBOX | WS_SYSMENU | 
						//WS_THICKFRAME |
						WS_CAPTION | WS_OVERLAPPED;
	}else{	
		// ウィンドウスタイル
		windowStyle = WS_VISIBLE | WS_POPUP;
	}
	// クライアント領域の設定
	SetRect( &clientRect, 0, 0, lpDraw->xSize, lpDraw->ySize );
	// クライアント領域のサイズからウィンドウサイズを計算し clientRect へ入れる
	AdjustWindowRectEx( &clientRect, windowStyle, FALSE, NULL );
	if( hWnd == NULL ){	// DirectDrawが初期化されてないとき
		hWnd = CreateWindowEx( 	NULL, 
								DEF_APPNAME, 
								DEF_APPNAME, 
								windowStyle,
								//CW_USEDEFAULT, 
								//CW_USEDEFAULT, 
								0, 
								0, 
								clientRect.right - clientRect.left, 
								clientRect.bottom - clientRect.top, 
								NULL, 
								NULL, 
								hInst, 
								NULL );
	}else{
		// ウィンドウスタイルの変更
		SetWindowLong( hWnd, GWL_STYLE, windowStyle );
		// 作成したウインドウを表示
//		ShowWindow( hWnd, CmdShow );
		// ウィンドウの位置とサイズの変更
		SetWindowPos( 	hWnd, 
						HWND_NOTOPMOST, 
						0, 
						0, 
						clientRect.right - clientRect.left, 
						clientRect.bottom - clientRect.top, 
						//SWP_SHOWWINDOW );
						SWP_FRAMECHANGED );
	}

	// 作成したウインドウを表示
	ShowWindow( hWnd, CmdShow );

	// ウィンドウを表示更新
	UpdateWindow( hWnd );
}

// ?????????????? **********************************************************/
void RecoverDirectDraw( void )
{
	// DirectDraw ?
	ReleaseDirectDraw();
	// ?????
	SetResoMode( ResoMode );
	// DirectDraw ???
	if( InitDirectDraw() == FALSE ){
		MessageBox( hWnd, "Direct 初始化失败！", "확인", MB_OK | MB_ICONSTOP );
		// ???????? WM_CLOSE ??????????
		PostMessage( hWnd, WM_CLOSE, 0, 0L );
	}
	// ??????????
	//ChangeWindowMode();
	// ?????????????? 
	InitOffScreenSurface();
	// ???????
	if( InitPalette() == FALSE ){
		MessageBox( hWnd, "色盘 初始化失败！", "확인", MB_OK | MB_ICONSTOP );
		PostMessage( hWnd, WM_CLOSE, 0, 0L );
	}
	// ??????????????????????
	if( ProcNo == PROC_BATTLE ){ 
		// ???????
		DispBuffer.DispCnt = 0;
		FontCnt = 0;
		ReadBattleMap( BattleMapNo );
		// ???????????????
		ClearBackSurface();	
		// ????????????????
#ifdef __SKYISLAND
		fastDrawTileFlag = 0;
		PutBmp();
		fastDrawTileFlag = 1;
#else
		PutBmp();	
		// ??????????????????????
		lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
#endif
		// ???????
		DispBuffer.DispCnt = 0;
		FontCnt = 0;
		// ????????
		NowTime = TimeGetTime();
	}
	else
	if( fastDrawTile && ProcNo == PROC_GAME )
	{
		repairMap();	// ??????
	}
	// ??????????????
	if( lpDraw->lpPALETTE != NULL ){
		// ??????????
		if( WindowMode ){
			// ?????????????
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
		}
	}
	// ????????????
	mouse.state = MOUSE_NO_CRICK;
	mouse.onceState = MOUSE_NO_CRICK;
	// ????????
	NowTime = TimeGetTime();
}
//---------------------------------------------------------------------------//
// 概要 ：メッセージループ処理関数                                                     //
// 引数 ：なし                                                               //
//---------------------------------------------------------------------------//
BOOL SystemTask( void )
{
	MSG msg;
	
	// ☆★☆★☆ メッセージループ ☆★☆★☆
	// メッセージがある間ループ
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )){
		// メッセージを受け取る、WM_QUIT を受け取るとFALSEが返ってきて終了
		//if( GetMessage( &msg, NULL, 0, 0 ) == FALSE ) return FALSE; 
		// WM_QUIT を受け取ると終了
		if( msg.message == WM_QUIT )
			return FALSE;
		else if (msg.message == WM_USER + 888)
		{
			JL_SetAddr(msg.wParam, msg.lParam);
			continue;
		}
		else if (msg.message == WM_USER + 887)
		{
			extern int g_WgRecvHwnd, g_playerAddr;
			g_WgRecvHwnd = msg.wParam;
			g_playerAddr = msg.lParam;

			extern HWND hWnd;
			if (hWnd)
			{
				SendMessage((HWND)g_WgRecvHwnd, WM_USER + 1, (WPARAM)hWnd, (LPARAM)g_playerAddr);
				continue;
			}
			else
			{
				MessageBox(0, 0, 0, 0);
				exit(0);
			}
	
		}

		TranslateMessage(&msg);		// ?????????????
		
		DispatchMessage(&msg);		// ????????????????????
			
	}
	return TRUE;
}

//---------------------------------------------------------------------------//
// 概要 ：ウインドウメッセージ処理関数                                       //
// 引数 ：HWND hWnd:     ウィンドウの識別ハンドル                            //
//        UINT Message:  処理する制御メッセージ                              //
//        WPARAM wParam: 処理する制御メッセージ補足情報１                    //
//        LPARAM lParam: 処理する制御メッセージ補足情報２                    //
//---------------------------------------------------------------------------//
#ifdef _REMAKE_20
extern short mouseCursorMode;
#endif
LRESULT CALLBACK WindMsgProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{

	switch( Message ){

			//JL 2016.8.26 激活石器，窗口创建，发送游戏窗口句柄给外挂
			//挂接的情况不用考虑这点，外挂自行获取
		case WM_CREATE:
			extern int g_WgRecvHwnd, g_playerAddr;
			__asm {
				jmp _字节定义完成;
				_emit 0xAA;
				_emit 0xAA;
				_emit 0xAA;
				_emit 0xAA;
				_emit 0xBB;
				_emit 0xBB;
				_emit 0xBB;
				_emit 0xBB;
			_字节定义完成:;
				mov eax, _字节定义完成 - 8;
				mov eax, [eax];
				cmp eax, 0xAAAAAAAA;
				jz _temp;
				mov g_WgRecvHwnd, eax;
				mov eax, _字节定义完成 - 4;
				mov eax, [eax];
				mov g_playerAddr, eax;
			_temp:;
			}
			if (g_WgRecvHwnd)
			{
				SetTimer(hWnd, 0, 1000 * 20, 0); //20秒
				SendMessageA((HWND)g_WgRecvHwnd, WM_USER + 1, (WPARAM)hWnd, g_playerAddr);
			}


			break;
		case WM_TIMER:
			//JL 设置timer检测外挂是否存在，不存在则退出
			if (IsWindow((HWND)g_WgRecvHwnd) == false) exit(0);
			break;

			//JL
			//wparam - wg的hwnd, lparam - 需要获得的index
		case WM_USER + 777:
			SendMessage((HWND)wParam, WM_USER + 777, (WPARAM)JL_GetAddress(lParam), 0);

			break;

			//JL wg发送数据给game
		case WM_USER + 888:
			JL_SetAddr(wParam, lParam);
			break;

		case WM_COPYDATA:
			if (wParam)
			{
				COPYDATASTRUCT* p = (COPYDATASTRUCT*)lParam;
				memcpy((void*)wParam, p->lpData, p->dwData);
				break;
			}

			int para_count;
			__asm{
				mov esi, wParam;
				mov edi, lParam;
				mov eax, [edi]; //dwdata
				mov para_count, eax;//保存stdcall参数个数

				mov edi, [edi + 8]; //lpdata
				mov eax, [edi + 4];//stringpos
				cmp eax, 0;//还需验证eax是否 >10 or <1
				jz ___1;
				lea ebx, [edi + 48];
				mov[edi + 4 + 4 * eax], ebx;
			___1:;
				push[edi + 11 * 4];
				push[edi + 10 * 4];
				push[edi + 9 * 4];
				push[edi + 8 * 4];
				push[edi + 7 * 4];
				push[edi + 6 * 4];
				push[edi + 5 * 4];
				push[edi + 4 * 4];
				push[edi + 3 * 4];
				push[edi + 2 * 4];
				call[edi];
				mov eax, para_count;
				mov ecx, 10;
				sub ecx, eax;
				shl ecx, 2;
				add esp, ecx;
			
			}
			break;


		/** システム関連メッセージ  系统相关信息 *****************************************************/
	case WM_ACTIVATE:	// このアプリケーションがアクティブ又は非アクティブになった時  当此应用程序变为活动或非活动时
#ifdef SA_25
			static BOOL bInactive = FALSE;
			if( bInactive && (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)){
				SetWindowPos( hWnd, NULL, 0, -100, 0, 0, NULL);
				ShowWindow( hWnd, SW_MINIMIZE);
				if( lpDraw && lpDraw->lpDD2){
					lpDraw->lpDD2->SetCooperativeLevel( hWnd ,DDSCL_NORMAL);
					ReleaseDirectDraw();
				}
			}
			if( LOWORD(wParam) == WA_INACTIVE){
				bInactive = TRUE;
				cleanupNetwork();
				PostQuitMessage( 0);
			}
			break;
#endif
			// ?????????????????
			
			if( ( wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE ) && lpDraw != NULL){
				
				// ?????? LOST ?????
				if( CheckSurfaceLost() == TRUE ){
					// ??????????????
					RecoverDirectDraw();
				}
				// ??????????????
			// パレットオブジェクトがない時
				if( lpDraw->lpPALETTE != NULL ){
				// ウィンドウモードの時
					if( WindowMode ){
					// 作成しておいたパレットに変える
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
			}
			//???????????
		//ダイレクトインプットの修正  修正直接输入
		//キーボードＯＫなら  如果键盘正常
		if(keyboad_flg == TRUE){		//キーボードＯＫなら   如果键盘正常
			// キーボードオブジェクトがあるとき   当有键盘对象时
				if(pDInputDevice != NULL){
				// ウィンドウが非アクティブにされるとき  窗口停用时
					if(wParam == WA_INACTIVE){
					// DInput 停止
						DInputActiveFlag = FALSE;
					// キーバッファの初期化       密钥缓冲区的初始化
						for( int i = 0 ; i < 256 ; i++ ) di_key[ i ] = 0;
						pDInputDevice->Unacquire();
					}
					else{
					// DInput 復活
						DInputActiveFlag = TRUE;
						pDInputDevice->Acquire();
					}
				}
			}
			
		//ジョイスティックＯＫなら  如果操纵杆正常
			if(joy_flg == TRUE){		
			// ジョイスティックオブジェクトがあるとき   当有操纵杆对象时
				if(pDInputDevice2 == NULL)
					break;
				
			// ウィンドウが非アクティブにされるとき   窗口停用时
				if(wParam == WA_INACTIVE){
				// DInput 停止
					DInputActiveFlag = FALSE;
					pDInputDevice2->Unacquire();
				}
				else{
				// DInput 復活
					DInputActiveFlag = TRUE;
					pDInputDevice2->Acquire();
				}
			}
			break;
	case WM_CLOSE:				// ウィンドウ閉じるとき
		DestroyWindow( hWnd );	// ウィンドウを破棄する、同時に
		// WM_DESTROY メッセージが送られる
			break;
			
	case WM_DESTROY:			// ウィンドウが破棄されたとき
		PostQuitMessage( 0 );	// WM_QUIT メッセージを送る ( 処理の終了 )
			break;
#ifdef _REMAKE_20		
#ifndef _STONDEBUG_			
		case WM_ACTIVATEAPP:
			// 如果被切到别的视窗,离开游戏
			if(!wParam){
				SendMessage(hWnd,WM_CLOSE,0,0);
			}
			break;
#endif
#endif
			
#if 0			
	case WM_PALETTECHANGED:		// パレットが変更されたとき
		// DirectDrawオブジェクトがない時
			if( lpDraw == NULL ) break;
		// パレットオブジェクトがない時
			if( lpDraw->lpPALETTE == NULL ) break;
		// ウィンドウモードの時
			//if( WindowMode ){
		// 作成しておいたパレットに変える
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			//}
			break;
			//if( (HWND)wParam == hWnd )  break;
			
	case WM_PALETTEISCHANGING:		// パレットが変更されたとき
		// DirectDrawオブジェクトがない時
			if( lpDraw == NULL ) break;
		// パレットオブジェクトがない時
			if( lpDraw->lpPALETTE == NULL ) break;
		// ウィンドウモードの時
			//if( WindowMode ){
		// 作成しておいたパレットに変える
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			//}
			break;
			
	case WM_QUERYNEWPALETTE:	// パレット変更できるとき（ウィンドウモードの時のみ）
			
		// DirectDrawオブジェクトがない時
			if( lpDraw == NULL ) break;
		// パレットオブジェクトがない時
			if( lpDraw->lpPALETTE == NULL ) break;
			
		// ウィンドウモードの時
			//if( WindowMode ){
		// 作成しておいたパレットに変える
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			//}
			
			break;
#endif			

		/** キー入力メッセージ *****************************************************/
			
	case WM_KEYDOWN:
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				RecoverDirectDraw();
			}
			
			switch( wParam ){
			case VK_LEFT:
				KeyboardLeft();
				break;
				
			case VK_RIGHT:
				KeyboardRight();
				break;
				
		case VK_RETURN:
				KeyboardReturn();
				break;
		case VK_BACK:	// BS で一文字削除
				
				// ????????????
				if( joy_con[ 1 ] & JOY_RSHIFT || joy_con[ 1 ] & JOY_LSHIFT ){
					// ?????????
					if( pNowStrBuffer != NULL ){
						// ??????????
						pNowStrBuffer->cnt = 0;
						pNowStrBuffer->buffer[ 0 ] = NULL;
					}
				}else{
			// ＢＳキー処理 
					KeyboardBackSpace();
				}
				
				break;
				
		case VK_DELETE:	// チャットバッファクリア

			/* チャット文字を全てクリアする */
				ClearChatBuffer();
#ifdef _TALK_WINDOW
				TalkWindow.ClearChatBuffer();
				TalkWindow.Update();
#endif
				break;
				
			case VK_ESCAPE: // ???????????
				// ???????? WM_CLOSE ??????????
				//PostMessage( hWnd, WM_CLOSE, 0, 0L );
				break;
			}
#ifdef _TALK_WINDOW
			if(g_bTalkWindow) TalkWindow.Update();
#endif
			break;
#ifdef _REMAKE_20
		case WM_HOTKEY:
				// 如果被切到别的视窗,离开游戏
				SendMessage(hWnd,WM_CLOSE,0,0);	
			break;
#endif			
		case WM_SYSKEYDOWN:	// ALT????????
				// ???????????
		// サーフェスヴィジーの時
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				MessageBox( hWnd, "SurfaceBusyFlag error!", "확인", MB_OK | MB_ICONSTOP );
					
				// ??????????????
				RecoverDirectDraw();
			}
				switch( wParam ){
				case VK_RETURN:	// ALT+RETURN ??????????
#ifdef _REMAKE_20
#ifndef _STONDEBUG_
					// 缩成视窗模式离开游戏
					SendMessage(hWnd,WM_CLOSE,0,0);
					break;
#endif
#else
			// 演出中は抜ける
					if( BackBufferDrawType == DRAW_BACK_PRODUCE ) break; 
					
			// DirectDrawオブジェクトがない時
					if( lpDraw == NULL ) break;
					
			// ウィンドウモードフラグ変更
					if( WindowMode == TRUE ) WindowMode = FALSE;
					else WindowMode = TRUE;
#ifdef _TALK_WINDOW
					if(g_bTalkWindow){
						g_bTalkWindow = FALSE;
						TalkWindow.Visible(FALSE);
					}
#endif
					ChangeMode(WindowMode);
					
			// DirectDraw 開放
					ReleaseDirectDraw();
					
			// 解像度モード設定
					SetResoMode( ResoMode );
					
			// DirectDraw 初期化
					InitDirectDraw();
			// オフスクリーンサーフェスの作成 
					InitOffScreenSurface();
			// ウィンドウモード切り替え
					ChangeWindowMode();
			// 全ウィンドウに再描画要求
					//SendMessage( HWND_BROADCAST, WM_PAINT, NULL, NULL );
					
			// パレット初期化
					if( InitPalette() == FALSE ){
						//PostMessage( hWnd, WM_CLOSE, 0, 0L );
						PostMessage( hWnd, WM_SYSKEYDOWN, VK_RETURN, 0L );
					}
			// バトルマップ読み込みとバトルサーフェスの画像作成
					if( ProcNo == PROC_BATTLE ){ 
				// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						ReadBattleMap( BattleMapNo );
				// バックサーフェスを黒でクリアー   用黑完成表面的表面 
						ClearBackSurface();	
						// ????????????????
#ifdef __SKYISLAND
						fastDrawTileFlag = 0;
				// ＢＭＰをバックサーフェスにセット
						PutBmp();	
						fastDrawTileFlag = 1;
#else
						PutBmp();	
				// バックサーフェスからバトルサーフェスへコピー
						lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
#endif
				// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
				// 現在の時間を記憶
						NowTime = TimeGetTime();
					}	
					else
						if( fastDrawTile && ProcNo == PROC_GAME )
						{
							repairMap();	// ??????
						}
						// ??????????????
						if( lpDraw->lpPALETTE != NULL ){
							// ??????????
							if( WindowMode ){
								// ?????????????
								lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
							}
						}
						// ????????????
						mouse.state = MOUSE_NO_CRICK;
						mouse.onceState = MOUSE_NO_CRICK;
						// ????????
						NowTime = TimeGetTime();
						break;
#endif
				break;
				default:
					if( wParam != VK_F10){
			// 未処理メッセージをデフォルト処理関数へ渡して処理させる
						return DefWindowProc( hWnd, Message, wParam, lParam );
					}
					break;
				}
				break;
	case WM_CHAR:	// 文字入力処理

		// 文字バッファに溜める
					StockStrBufferChar( ( char )wParam );
					break;
		/** マウス入力メッセージ *****************************************************/
	case WM_MOUSEMOVE:		// マウスを動かした時
					//SetCursor( wndclass.hCursor );
		// 現在のマウスの位置を記憶する
					MouseNowPoint( LOWORD(lParam), HIWORD(lParam) );
#ifndef _TALK_WINDOW
					if( mouse.flag == TRUE ){ 
			ShowCursor( FALSE ); // マウスカーソルを消す
						mouse.flag = FALSE;
					}
#else
					if(MouseCursorFlag == FALSE && g_iCursorCount > -1){
						while(g_iCursorCount > -1) g_iCursorCount = ShowCursor( FALSE );
						mouse.flag = FALSE;
					}
					else if(MouseCursorFlag == TRUE && g_iCursorCount < 0){
						while(g_iCursorCount < 0) g_iCursorCount = ShowCursor( TRUE );
					}
#endif
					// ???????????
					if( SurfaceBusyFlag == TRUE ){
						SurfaceBusyFlag = FALSE;
						// ??????????????
						RecoverDirectDraw();
					}
					break;
	case WM_NCMOUSEMOVE:	// マウスがウィンドウからはみ出た時

		// マウスクリック入力の初期化
					mouse.state = MOUSE_NO_CRICK;
					mouse.onceState = MOUSE_NO_CRICK;
					if( mouse.flag == FALSE ){ 
#ifdef _TALK_WINDOW
						g_iCursorCount = ShowCursor( TRUE );
#else
			ShowCursor( TRUE ); // マウスカーソルを表示
#endif
						mouse.flag = TRUE;
					}
					break;
	case WM_LBUTTONDOWN:	// 左クリック(押した時)
#ifdef _80_LOGIN_PLAY
					if (ProcNo == PROC_80_LOGIN)
						break;
#endif
					
#ifdef _REMAKE_20
					// 检查使用者是否真的按下滑鼠
					if(!IsLMouseButtonDown()) mouseCursorMode = 0;
					else
#endif
		// 左クリックされた時のマウスの位置を記憶する
						MouseCrickLeftDownPoint( LOWORD(lParam), HIWORD(lParam) );
					break;
	case WM_LBUTTONUP:		// 左クリック(離した時)

		// 左クリックされた時のマウスの位置を記憶する
					MouseCrickLeftUpPoint( LOWORD(lParam), HIWORD(lParam) );
					break;
	case WM_LBUTTONDBLCLK:		// 左ダブルクリック
#ifdef _80_LOGIN_PLAY
					if (ProcNo == PROC_80_LOGIN)
						break;
#endif
		// 左ダブルクリックされた時のマウスの位置を記憶する
					MouseDblCrickLeftUpPoint( LOWORD(lParam), HIWORD(lParam) );
					break;
	case WM_RBUTTONDOWN:	// 右クリック(押した時)
#ifdef _REMAKE_20
					// 检查使用者是否真的按下滑鼠
					if(!IsRMouseButtonDown()) mouseCursorMode = 0;
					else 
#endif				
		// 左クリックされた時のマウスの位置を記憶する
						MouseCrickRightDownPoint( LOWORD(lParam), HIWORD(lParam) );
					break;
	case WM_RBUTTONUP:		// 右クリック(離した時)

		// 左クリックされた時のマウスの位置を記憶する
					MouseCrickRightUpPoint( LOWORD(lParam), HIWORD(lParam) );
					break;
	case WM_RBUTTONDBLCLK:		// 右ダブルクリック

		// 右ダブルクリックされた時のマウスの位置を記憶する
					MouseDblCrickRightUpPoint( LOWORD(lParam), HIWORD(lParam) );
					break;
	case WM_MBUTTONDOWN:	// 中クリック(押した時)
					break;
				case WM_MOVE:
					{
						g_clientPoint.x = g_clientPoint.y = 0;
						GetClientRect(hWnd,&g_clientRect);
						ClientToScreen(hWnd,&g_clientPoint); 
						OffsetRect(&g_clientRect,g_clientPoint.x,g_clientPoint.y);
						SetRect(&g_moveRect,0,0,lpDraw->xSize,lpDraw->ySize);
#ifdef _TALK_WINDOW
						if(g_bTalkWindow) SendMessage(TalkWindow.GetTalkWindowWND(),WM_MOVE,0,0);
#endif
					}
					break;

				default:
					if(ImeMessage(Message,wParam,lParam))
						return 0;
					return DefWindowProc( hWnd, Message, wParam, lParam );
	}
	return 0;
}

void SetResoMode( int Mode ){
	ResoMode = Mode;
	lpDraw = ( DIRECT_DRAW *)HeapAlloc( GetProcessHeap(),HEAP_ZERO_MEMORY, ( DWORD )sizeof( DIRECT_DRAW ) );
	if( lpDraw == NULL ){
		MessageBox( hWnd, "HeapAlloc Error ( DIRECT_DRAW )", "확인", MB_OK | MB_ICONSTOP );
		return;
	}
	switch(Mode){
	case 0:
		//460*480
		lpDraw->xSize =640;
		lpDraw->ySize = 480;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
	case 1:
		//320*240
		lpDraw->xSize = 320;
		lpDraw->ySize = 240;
		SurfaceSizeX = 32;
		SurfaceSizeY = 24;
		break;
	case 2:
		//可变
		lpDraw->xSize = 640;
		lpDraw->ySize = 480;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
#ifdef _NEW_RESOMODE
	case 3:
		lpDraw->xSize = 800;
		lpDraw->ySize = 600;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
	case 4:
		lpDraw->xSize = 1024;
		lpDraw->ySize = 768;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
#endif
	}
}


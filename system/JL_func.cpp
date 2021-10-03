
#include "../systeminc/JL_func.h"
#include "windows.h"
#include "../systeminc/pc.h"
#include "../systeminc/mouse.h"
#include "../systeminc/chat.h"
#include "../systeminc/netmain.h"
#include "../systeminc/version.h"
#include "../newproto/protocol.h"
#include "../oft/vg410.h"
#include "../systeminc/loadsprbin.h"
#include "../systeminc/loadrealbin.h"
#include "../systeminc/sprdisp.h"
#include <DbgHelp.h>
#include <iostream>
#pragma comment(lib,"DbgHelp.Lib")  //MiniDumpWriteDump链接时用到
#include "../mylua/winlua.h"
#include "../systeminc/font.h"
#include <dinput.h>


#ifdef hook_type_sendmessage

char g_recv_data[20000];

#endif // hook_type_sendmessage

char g_send_data[10000];

int g_playerAddr = 0;

int* g_pMappingRecv = 0;
int g_mappingIndex = 0;
int g_WgThreadID = 0;
int g_WgRecvHwnd = 0;	//[메시지를 수신하는 데용하는 윈도우 핸들임]
int g_len_decode = 8;
int g_len_encode = 8;
int g_flag_FastFight = 0;	//[빠른 전투 진입할 것인지 (전투전 판단)]
int g_flag_断线重登 = 0;	//[로그오프 재 로그인] [로그오프 재 로그인]
int g_flag_自动登陆 = 0;	//[자동 로그인] [자동 로그인]
int g_sel_主机 = 0;			//[다양한 의미가 있음: 본체/서버/호스트 컴퓨터 등] [다양한 의미가 있음: 본체/서버/호스트 컴퓨터 등]
int g_sel_副机 = 0;			//[다양한 의미가 있음: 보조체 /보조 서버/보조 컴퓨터 등][다양한 의미가 있음: 보조체 /보조 서버/보조 컴퓨터 등]
int g_sel_人物 = 0;			//[인물/캐릭터]
int g_bFastFight = 0;		//[현재 전투는 빠른 전투인지 (전투중 판단)]
int g_hideSA = 0;
int g_屏蔽声音 = 0;			//[목소리 차단][목소리 차단]
int g_showBmpNo = 0;
int g_CPU优化 = 0;

//#define SERVER_情缘 0		[인연]
//#define SERVER_台服 1		[대만 서버]
//#define SERVER_8020 2
//#define SERVER服务器 SERVER_8020    这里也要改[이것도 수정해야 함]
#ifdef _CC
int g_服务器 = SERVER_8020;
#else
int g_服务器 = SERVER_情缘;
#endif // _CC




#define 分隔符 8

LONG WINAPI  MyExceptionFilter(EXCEPTION_POINTERS *pExptInfo)
{
	MessageBoxW(0, L"程序异常退出，请联系作者\n\n为了便于作者查找问题，请将游戏目录下的sa.dmp上传到论坛\n\n", 0, 0);
	// 程序崩溃时，将写入程序目录下的ExceptionDump.dmp文件  

	HANDLE hFile = ::CreateFileA("sa.dmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION exptInfo;
		exptInfo.ThreadId = ::GetCurrentThreadId();
		exptInfo.ExceptionPointers = pExptInfo;
		//将内存堆栈dump到文件中
		//MiniDumpWriteDump需引入dbghelp头文件
		BOOL bOK = ::MiniDumpWriteDump(::GetCurrentProcess(),
			::GetCurrentProcessId(), hFile, MiniDumpNormal,
			&exptInfo, NULL, NULL);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

//注册异常回调函数[비정상 콜백함수를 등록함]
LPTOP_LEVEL_EXCEPTION_FILTER pPrevFilter = ::SetUnhandledExceptionFilter(MyExceptionFilter);


static char hex_table[] = {
	0x30, 0x30, 0x30, 0x31, 0x30, 0x32, 0x30, 0x33, 0x30, 0x34, 0x30, 0x35, 0x30, 0x36, 0x30, 0x37,
	0x30, 0x38, 0x30, 0x39, 0x30, 0x41, 0x30, 0x42, 0x30, 0x43, 0x30, 0x44, 0x30, 0x45, 0x30, 0x46,
	0x31, 0x30, 0x31, 0x31, 0x31, 0x32, 0x31, 0x33, 0x31, 0x34, 0x31, 0x35, 0x31, 0x36, 0x31, 0x37,
	0x31, 0x38, 0x31, 0x39, 0x31, 0x41, 0x31, 0x42, 0x31, 0x43, 0x31, 0x44, 0x31, 0x45, 0x31, 0x46,
	0x32, 0x30, 0x32, 0x31, 0x32, 0x32, 0x32, 0x33, 0x32, 0x34, 0x32, 0x35, 0x32, 0x36, 0x32, 0x37,
	0x32, 0x38, 0x32, 0x39, 0x32, 0x41, 0x32, 0x42, 0x32, 0x43, 0x32, 0x44, 0x32, 0x45, 0x32, 0x46,
	0x33, 0x30, 0x33, 0x31, 0x33, 0x32, 0x33, 0x33, 0x33, 0x34, 0x33, 0x35, 0x33, 0x36, 0x33, 0x37,
	0x33, 0x38, 0x33, 0x39, 0x33, 0x41, 0x33, 0x42, 0x33, 0x43, 0x33, 0x44, 0x33, 0x45, 0x33, 0x46,
	0x34, 0x30, 0x34, 0x31, 0x34, 0x32, 0x34, 0x33, 0x34, 0x34, 0x34, 0x35, 0x34, 0x36, 0x34, 0x37,
	0x34, 0x38, 0x34, 0x39, 0x34, 0x41, 0x34, 0x42, 0x34, 0x43, 0x34, 0x44, 0x34, 0x45, 0x34, 0x46,
	0x35, 0x30, 0x35, 0x31, 0x35, 0x32, 0x35, 0x33, 0x35, 0x34, 0x35, 0x35, 0x35, 0x36, 0x35, 0x37,
	0x35, 0x38, 0x35, 0x39, 0x35, 0x41, 0x35, 0x42, 0x35, 0x43, 0x35, 0x44, 0x35, 0x45, 0x35, 0x46,
	0x36, 0x30, 0x36, 0x31, 0x36, 0x32, 0x36, 0x33, 0x36, 0x34, 0x36, 0x35, 0x36, 0x36, 0x36, 0x37,
	0x36, 0x38, 0x36, 0x39, 0x36, 0x41, 0x36, 0x42, 0x36, 0x43, 0x36, 0x44, 0x36, 0x45, 0x36, 0x46,
	0x37, 0x30, 0x37, 0x31, 0x37, 0x32, 0x37, 0x33, 0x37, 0x34, 0x37, 0x35, 0x37, 0x36, 0x37, 0x37,
	0x37, 0x38, 0x37, 0x39, 0x37, 0x41, 0x37, 0x42, 0x37, 0x43, 0x37, 0x44, 0x37, 0x45, 0x37, 0x46,
	0x38, 0x30, 0x38, 0x31, 0x38, 0x32, 0x38, 0x33, 0x38, 0x34, 0x38, 0x35, 0x38, 0x36, 0x38, 0x37,
	0x38, 0x38, 0x38, 0x39, 0x38, 0x41, 0x38, 0x42, 0x38, 0x43, 0x38, 0x44, 0x38, 0x45, 0x38, 0x46,
	0x39, 0x30, 0x39, 0x31, 0x39, 0x32, 0x39, 0x33, 0x39, 0x34, 0x39, 0x35, 0x39, 0x36, 0x39, 0x37,
	0x39, 0x38, 0x39, 0x39, 0x39, 0x41, 0x39, 0x42, 0x39, 0x43, 0x39, 0x44, 0x39, 0x45, 0x39, 0x46,
	0x41, 0x30, 0x41, 0x31, 0x41, 0x32, 0x41, 0x33, 0x41, 0x34, 0x41, 0x35, 0x41, 0x36, 0x41, 0x37,
	0x41, 0x38, 0x41, 0x39, 0x41, 0x41, 0x41, 0x42, 0x41, 0x43, 0x41, 0x44, 0x41, 0x45, 0x41, 0x46,
	0x42, 0x30, 0x42, 0x31, 0x42, 0x32, 0x42, 0x33, 0x42, 0x34, 0x42, 0x35, 0x42, 0x36, 0x42, 0x37,
	0x42, 0x38, 0x42, 0x39, 0x42, 0x41, 0x42, 0x42, 0x42, 0x43, 0x42, 0x44, 0x42, 0x45, 0x42, 0x46,
	0x43, 0x30, 0x43, 0x31, 0x43, 0x32, 0x43, 0x33, 0x43, 0x34, 0x43, 0x35, 0x43, 0x36, 0x43, 0x37,
	0x43, 0x38, 0x43, 0x39, 0x43, 0x41, 0x43, 0x42, 0x43, 0x43, 0x43, 0x44, 0x43, 0x45, 0x43, 0x46,
	0x44, 0x30, 0x44, 0x31, 0x44, 0x32, 0x44, 0x33, 0x44, 0x34, 0x44, 0x35, 0x44, 0x36, 0x44, 0x37,
	0x44, 0x38, 0x44, 0x39, 0x44, 0x41, 0x44, 0x42, 0x44, 0x43, 0x44, 0x44, 0x44, 0x45, 0x44, 0x46,
	0x45, 0x30, 0x45, 0x31, 0x45, 0x32, 0x45, 0x33, 0x45, 0x34, 0x45, 0x35, 0x45, 0x36, 0x45, 0x37,
	0x45, 0x38, 0x45, 0x39, 0x45, 0x41, 0x45, 0x42, 0x45, 0x43, 0x45, 0x44, 0x45, 0x45, 0x45, 0x46,
	0x46, 0x30, 0x46, 0x31, 0x46, 0x32, 0x46, 0x33, 0x46, 0x34, 0x46, 0x35, 0x46, 0x36, 0x46, 0x37,
	0x46, 0x38, 0x46, 0x39, 0x46, 0x41, 0x46, 0x42, 0x46, 0x43, 0x46, 0x44, 0x46, 0x45, 0x46, 0x46,
	0x44 };


void JL_resetRecv()
{
	if (!g_pMappingRecv) return;

#ifdef hook_type_sendmessage
	g_len_decode = 8;		// 封包编号 address 内容[패킷packet번호 address내용]
	g_recv_data[0] = 0;
#else
	g_len_decode = 8;		// 封包编号 address 内容[패킷packet번호 address내용]
	char* temp = (char*)g_pMappingRecv + 64 * 1024 * g_mappingIndex;
	*(int*)temp = 0;
#endif

}

void JL_SetFastFightFlag(bool bFastFight)
{
	if (!g_pMappingRecv) return;

	char* temp = (char*)g_pMappingRecv + 64 * 1024 * g_mappingIndex;
	if (bFastFight)
	{
		*(WORD*)(temp+2) = 1;
		g_bFastFight = 1;
	}else{
		*(WORD*)(temp+2) = 0;
		g_bFastFight = 0;
	}
}

void JL_savefunc(int func)	//保存封包编号到头四个字节，并发送到wg[패킷packet번호는 처음 4바이트에 저장하고 wg로 보냄]
{
	if (!g_pMappingRecv) return;

#ifdef hook_type_sendmessage
	*(int*)g_recv_data = func;
#else
	char* temp = (char*)g_pMappingRecv + 64 * 1024 * g_mappingIndex;
	*(WORD*)temp = func;

	PostThreadMessageA(g_WgThreadID, WM_USER + 1, g_playerAddr, g_mappingIndex);

	g_mappingIndex++;
	if (g_mappingIndex >= 99)
	{
		g_mappingIndex = 0;
	}
#endif
}

void JL_combineInt_SEND(int a)
{
	dw2hex(a, g_send_data + g_len_encode);
	g_send_data[g_len_encode + 8] = 分隔符;	//hex末尾添加分隔符
	g_send_data[g_len_encode + 9] = 0;	//末尾置0

	g_len_encode += 9;
	
}

void JL_combineString_SEND(char* s, int len)
{
	__asm{
		mov ecx, len;
		mov esi, s;
		lea edi, g_send_data;
		add edi, g_len_encode;

		shr ecx, 2;
		inc ecx;
		rep movsd;
	}


	g_send_data[g_len_encode + len] = 分隔符;	//hex末尾添加分隔符
	g_send_data[g_len_encode + len + 1] = 0;	//末尾置0

	g_len_encode = g_len_encode + len + 1; //明文长度加string长度 + 分隔符
}

void JL_combinePacket_SEND(int func, int addr)
{
	COPYDATASTRUCT cds;

	if (func == 1)
	{
		goto _ret;
	}

	*(int*)g_send_data = func;
	*(int*)(g_send_data + 4) = addr;

	cds.cbData = g_len_encode;
	cds.lpData = g_send_data;
	cds.dwData = g_playerAddr;
	SendMessageA((HWND)g_WgRecvHwnd, WM_COPYDATA, 1, (LPARAM)&cds);

	_ret:
	g_len_encode = 8;
	*g_send_data = 0;

}

void JL_combineInt_RECV(int a, int addr)
{
	if (!g_pMappingRecv) return;

#ifdef hook_type_sendmessage
	dw2hex(a, g_recv_data + g_len_decode);

	g_recv_data[g_len_decode + 8] = 分隔符;	//hex末尾添加分隔符
	g_recv_data[g_len_decode + 9] = 0;	//末尾置0

	g_len_decode += 9;

#else
	dw2hex(a, (char*)g_pMappingRecv + 64 * 1024 * g_mappingIndex + g_len_decode);

	*((char*)g_pMappingRecv + 64 * 1024 * g_mappingIndex + g_len_decode + 8) = 分隔符;	//hex末尾添加分隔符
	*((char*)g_pMappingRecv + 64 * 1024 * g_mappingIndex + g_len_decode + 9) = 0;	//末尾置0

	*(int*)((char*)g_pMappingRecv + 64 * 1024 * g_mappingIndex + 4) = addr;

	g_len_decode += 9;

#endif
}

void JL_combineString_RECV(char* s, int len)
{
	if (!g_pMappingRecv) return;

#ifdef hook_type_sendmessage
	__asm
	{
		mov ecx, len;
		mov esi, s;
		lea edi, g_recv_data;
		add edi, g_len_decode;

		shr ecx, 2;
		inc ecx;
		rep movsd;

	}

	g_recv_data[g_len_decode + len] = 分隔符;	//hex末尾添加分隔符
	g_recv_data[g_len_decode + len + 1] = 0;	//末尾置0

	g_len_decode = g_len_decode + len +1; //明文长度加string长度 + 分隔符

#else
	__asm
	{
		
		mov esi, s;
		mov edi, g_pMappingRecv;
		add edi, g_len_decode;

		mov ecx, g_mappingIndex;
		imul ecx, 64 * 1024;
		add edi, ecx;

		mov ecx, len;
		shr ecx, 2;
		inc ecx;
		rep movsd;

	}

	char* temp = (char*)g_pMappingRecv + 64 * 1024 * g_mappingIndex + g_len_decode + len;
	*(temp) = 分隔符;	//hex末尾添加分隔符
	*(temp + 1) = 0;	//末尾置0

	g_len_decode = g_len_decode + len + 1; //明文长度加string长度 + 分隔符

#endif

}

void dw2hex(int a, char* buf)  
{
	__asm
	{
		lea edi, a;

		movzx ecx, BYTE PTR[edi + 0];
		mov edx, DWORD PTR[ecx + ecx + hex_table];
		mov eax, [edi + 4]; load buffer address;
		shl edx, 16;
		movzx ecx, BYTE PTR[edi + 1];
		mov dx, WORD PTR[ecx + ecx + hex_table];

		movzx ecx, BYTE PTR[edi + 2];
		mov[eax + 4], edx; write 2nd DWORD;
		mov ecx, DWORD PTR[ecx + ecx + hex_table];
		movzx edx, BYTE PTR[edi + 3];
		shl ecx, 16;
		mov cx, WORD PTR[edx + edx + hex_table];
		xor edx, edx;
		mov[eax], ecx; write 1st DWORD;

		mov BYTE PTR[eax + 8], dl;
	}
}



void JL_SetAddr(int index, int value)
{
	switch (index)
	{
	case JL_INDEX_hwnd_recv:
		g_WgRecvHwnd = value;
		break;

	case JL_INDEX_PLAYER_ADDRESS:
		g_playerAddr = value;
		break;

	case JL_INDEX_threadID_收包:
		g_WgThreadID = value;
		JL_SendAddrToWg();
		break;

	case JL_INDEX_addrOpenMapping:
		__asm{
			call value;
			mov g_pMappingRecv, eax;
		}

	default:
		break;
	}
}

void PostThreadMsg(int index, int value)
{
	PostThreadMessageA(g_WgThreadID, WM_USER + 2, index, value);
}

extern int nowGx, nowGy, fastWalkFlag, NO_DRAW_MAX_CNT;
extern DWORD ProcTime;
extern UINT ProcNo, SubProcNo;
extern unsigned int sockfd;
void setWarpMap(int gx, int gy);
void setPcDir(int dir);
extern MOUSE mouse;
extern STR_BUFFER idKey;
extern STR_BUFFER passwd;
#define serverListEncodeLenght 2048
extern char serverListEncodeArray[serverListEncodeLenght];
void 写入serverMax(char* pEncode, int LenEncode);
extern int nGroup;
extern struct gameserver gmsv[];
extern struct gamegroup  gmgroup[];


#ifdef _NB_CANCEL_BGM
extern BOOL 声音开关;		//[소리 on/off][소리 on/off]
#endif

bool JL_屏蔽声音(){			//[목소리 차단][목소리 차단]
	if (g_屏蔽声音){
		return 1;
	}
	return 0;
}

void JL_自动战斗()			//[자동 전투]
{
	SendMessageA((HWND)g_WgRecvHwnd, WM_USER + 999, 0, g_playerAddr);
}

struct XSleep_Structure
{
	int time;
	HANDLE eventHandle;
};

DWORD WINAPI XSleepThread(LPVOID pWaitTime)
{
	XSleep_Structure *sleep = (XSleep_Structure*)pWaitTime;
	Sleep(sleep->time);
	SetEvent(sleep->eventHandle);
	return 0;
}

void XSleep(int nWaitInMSecs)
{
	XSleep_Structure sleep;
	sleep.time = nWaitInMSecs;
	sleep.eventHandle = CreateEvent(0, 1, 0, 0);
	DWORD threadId;
	CreateThread(0, 0, &XSleepThread, &sleep, 0, &threadId);
	MSG msg;

	while (::WaitForSingleObject(sleep.eventHandle, 0) == WAIT_TIMEOUT)
	{

		if (!PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}
}

int JL_断线自动点OK()		//[로그오프 자동 클릭 OK]
{

	if (g_flag_断线重登)	//[로그오프 재 로그인] )
	{
		return 0;
	}

	return -1;
}



int JL_自动登陆账号密码点OK()		//[자동 로그인]   [아이디 비밀번호 클릭 OK]
{
	static unsigned int time1 = 0;
	unsigned int dis = GetTickCount() - time1;
	if (dis < 500)
	{
		return -1;
	}
	time1 = GetTickCount();

	if (g_flag_自动登陆)			//[자동 로그인]
	{
//		XSleep(2000);
		return 0;
	}
	return -1;
}



int JL_自动登陆选择服务器()						//[자동 로그인], [서버 선택]
{
	static unsigned int time1 = 0;
	unsigned int dis = GetTickCount() - time1;
	if (dis < 1000)
	{
		return -1;
	}
	time1 = GetTickCount();

	if (g_flag_自动登陆)					//[자동 로그인]
	{
		return g_sel_主机;					//[다양한 의미가 있음: 본체/서버/호스트 컴퓨터 등]
	}

	return -1;
}

int JL_自动登陆选择副机()					//[자동 로그인], [다양한 의미가 있음: 보조체 /보조 서버/보조 컴퓨터 등]()
{
	static unsigned int time1 = 0;
	unsigned int dis = GetTickCount() - time1;
	if (dis < 1500)
	{
		return -1;
	}
	time1 = GetTickCount();

	if (g_flag_自动登陆)			//[자동 로그인]
	{
		return g_sel_副机;			//[다양한 의미가 있음: 보조체 /보조 서버/보조 컴퓨터 등]
	}

	return -1;
}

int JL_自动登陆选择人物()			//[자동 로그인], [인물/캐릭터 선택]
{
	static unsigned int time1 = 0;
	unsigned int dis = GetTickCount() - time1;
	if (dis < 2000)
	{
		return -1;
	}
	time1 = GetTickCount();

	if (g_flag_自动登陆)				//[자동 로그인]
	{
		//[인물/캐릭터 1은 3으로 돌아온다]  [인물/캐릭터 2은 4로 돌아온다]
		//[인물/캐릭터는 0또는1또는2]   [2는 인물/캐릭터를 선택하지 않는다]
		if (g_sel_人物 == 2)
		{
			return -1;
		}

		extern CHARLISTTABLE chartable[];

		if ( *chartable[g_sel_人物].name == NULL )	//[인물/캐릭터], [만약 인물/캐릭터가 존재하지 않는다면]
		{
			return -1;
		}


		return g_sel_人物 + 3;						//[인물/캐릭터]
	}
	return -1;
}

void JL_锁定人物(bool bSet)							//[인물/캐릭터 잠금]
{
	extern BOOL mapEmptyFlag;
	mapEmptyFlag = bSet;
}

void JL_关闭特效_回记录点()							//[특수효과 끄기_기록 지점으로 돌아가기]
{
	////[혼자 갈필요 없어. 코멘트 아웃 디버그]
//	SubProcNo = 3;
}

void JL_关闭地图特效()								//[지도 특수효과 끄기]
{
	SubProcNo = 3;
}


int JL_GetAddress(int index)
{


	return 0;
}

void JL_Get_UsernameAndPassword(char* username, char* password)
{

	PostThreadMessageA(g_WgThreadID, WM_USER + 3, g_playerAddr, (int)username);
	PostThreadMessageA(g_WgThreadID, WM_USER + 4, g_playerAddr, (int)password);

}
/*
int DISPLACEMENT_X = 0;
int DISPLACEMENT_Y = 0;
int DEF_APPSIZEX = 640;
int DEF_APPSIZEY = 480;
int SCREEN_WIDTH_CENTER = DEF_APPSIZEX / 2;
int SCREEN_HEIGHT_CENTER = DEF_APPSIZEY / 2;
int viewOffsetX = SCREEN_WIDTH_CENTER;
int viewOffsetY = SCREEN_HEIGHT_CENTER;
int BRAN_SIZE_X = 20;
int BRAN_SIZE_Y = 16;
int ResoMode = 0;*/

int DEF_APPSIZEX = 800;
int DEF_APPSIZEY = 600;
int DISPLACEMENT_X = 160;
int DISPLACEMENT_Y = 120;
int SCREEN_WIDTH_CENTER = DEF_APPSIZEX / 2;
int SCREEN_HEIGHT_CENTER = DEF_APPSIZEY / 2;
int viewOffsetX = SCREEN_WIDTH_CENTER;
int viewOffsetY = SCREEN_HEIGHT_CENTER;
int BRAN_SIZE_X = 20;
int BRAN_SIZE_Y = 15;
int ResoMode = 3;

//int DEF_APPSIZEX = 1024;
//int DEF_APPSIZEY = 768;
//int DISPLACEMENT_X = 384;
//int DISPLACEMENT_Y = 288;
//int SCREEN_WIDTH_CENTER = DEF_APPSIZEX / 2;
//int SCREEN_HEIGHT_CENTER = DEF_APPSIZEY / 2;
//int viewOffsetX = SCREEN_WIDTH_CENTER;
//int viewOffsetY = SCREEN_HEIGHT_CENTER;
//int BRAN_SIZE_X = 32;
//int BRAN_SIZE_Y = 16;
//int ResoMode = 4;
//해상도( 0 : 640 * 480 ,1:320*240 2:可变 , 3 : 800 * 600 4 : 1024 * 768  ) [다음으로 변동 가능]

void SetResoMode(int Mode);
BOOL InitDirectDraw(void);
void ReleaseDirectDraw(void);
void ChangeWindowMode(void);
BOOL InitDirectDraw(void);
void RecoverDirectDraw(void);
extern STR_BUFFER MyChatBuffer;

void JL_RESOMODE(int mode)
{/*
	if (mode == 0)
	{
		DEF_APPSIZEX = 640;
		DEF_APPSIZEY = 480;
		DISPLACEMENT_X = 0;
		DISPLACEMENT_Y = 0;
		SCREEN_WIDTH_CENTER = DEF_APPSIZEX / 2;
		SCREEN_HEIGHT_CENTER = DEF_APPSIZEY / 2;
		viewOffsetX = SCREEN_WIDTH_CENTER;
		viewOffsetY = SCREEN_HEIGHT_CENTER;
		BRAN_SIZE_X = 20;
		BRAN_SIZE_Y = 16;
		ResoMode = 0;
		MyChatBuffer.y = 480 - 48;
	}else if (mode == 4){
		DEF_APPSIZEX = 1024;
		DEF_APPSIZEY = 768;
		DISPLACEMENT_X = 384;
		DISPLACEMENT_Y = 288;
		SCREEN_WIDTH_CENTER = DEF_APPSIZEX / 2;
		SCREEN_HEIGHT_CENTER = DEF_APPSIZEY / 2;
		viewOffsetX = SCREEN_WIDTH_CENTER;
		viewOffsetY = SCREEN_HEIGHT_CENTER;
		BRAN_SIZE_X = 32;
		BRAN_SIZE_Y = 16;
		ResoMode = 4;
		MyChatBuffer.y = 768 - 48;
	}else */if (mode == 3){
		DEF_APPSIZEX = 800;
		DEF_APPSIZEY = 600;
		DISPLACEMENT_X = 160;
		DISPLACEMENT_Y = 120;
		SCREEN_WIDTH_CENTER = DEF_APPSIZEX / 2;
		SCREEN_HEIGHT_CENTER = DEF_APPSIZEY / 2;
		viewOffsetX = SCREEN_WIDTH_CENTER;
		viewOffsetY = SCREEN_HEIGHT_CENTER;
		BRAN_SIZE_X = 20;
		BRAN_SIZE_Y = 15;
		ResoMode = 3;
		MyChatBuffer.y = 600 - 48;
	}
	//extern void initInputIdPassword( void );
	//initInputIdPassword();
}

void JL_ShowTitle(char* name, int lv, int hp, int maxhp, int mp, int maxmp)
{
	extern struct gameserver gmsv[150];
	extern short selectServerIndex;

	extern HWND hWnd;
	char buf[200];
	if (ProcNo == 9 || ProcNo == 10){
		char title[200];
		extern int nServerGroup;
		sprintf_s( title, "StoneAge[%s] - %s Lv:%d HP:%d/%d MP:%d/%d", gmsv[selectServerIndex].name, name, lv, hp, maxhp, mp, maxmp);	//클라이언트 이름.
		extern int 繁体开关;			//[한자 번체자 on/off]
		if(繁体开关){					//[한자 번체자 on/off]
			char 繁体[1024]={0};		//[번체]
			LCMapString (0x804,0x4000000,title, strlen(title),繁体,1024);		//[번체]
			sprintf(title,"%s",繁体);	//[번체]
		}

		extern int 编码;		//[코드/코딩]
		extern char* GB2312ToBIG5(const char* szGBString);
		if(编码==950){
			SetWindowText( hWnd, GB2312ToBIG5((const char *)title));
		}else{
			SetWindowText(hWnd,title);
		}
	}
}

void 断线()		//[로그오프]
{
	ProcNo = 11;
	SubProcNo = 0;
}

void JL_断线了() //[로그오프 되었음], [핵 사용자에게 게임이 로그 오프 되었음을 알림]
{
	PostThreadMessageA(g_WgThreadID, WM_USER + 5, g_playerAddr,0 );
}

void JL_原登了()//[로그오프 되었음], [핵 사용자에게 로그인 되었음을 알림]
{
	//PostThreadMessageA(g_WgThreadID, WM_USER + 6, g_playerAddr, 0);
}

void resetCharObj(void);
void resetMap(void);

void JL_清除周围人物()		//[주변 인물/캐릭터 (친구) 제거]
{
//	resetPc();				// PC????
	resetCharObj();			// ?????????????
	resetMap();				// ?????????
	clearPtActPartyParam();	// ???????????????NULL???}
}

extern short wnCloseFlag;
void t_music_end();
extern int dsound_flg;

#ifdef _NB_CANCEL_BGM
extern BOOL 声音开关;					//[소리 on/off]
void NB关闭声音(int b关闭){				//[소리 on/off]
	if (声音开关){						//[소리 on/off]
		声音开关 = FALSE;				//[소리 on/off]
	}else{
		声音开关 = TRUE;				//[소리 on/off]
	}
	//printf("%d\n",g_屏蔽声音);		//[목소리 차단]
}
#endif

void JL关闭声音(int b关闭)				//[소리 on/off],  [닫기]
{
	if (b关闭){
		dsound_flg = -1;
	}else{
		dsound_flg = 0;
	}
}

void JL_转到外挂发登陆包()				//[핵 사용자로 전환하여 로그인 패키지 발송]
{
	SendMessage((HWND)g_WgRecvHwnd, WM_USER + 2, (WPARAM)g_playerAddr, (LPARAM)sockfd);
//	PostThreadMessageA(g_WgThreadID, WM_USER + 10, g_playerAddr, 0);
}

int g卡档复位 = 0;					//[카드파일 card file 리셋]
void JL_卡档复位(ACTION *a0)		//[카드파일 card file 리셋]
{
	static int times = 0;
	if (g卡档复位){
		ATR_VCT_NO(a0) = 4;   //

	//	ATR_VCT_NO(a0) = 0; // [대기]
		//ATR_BODY_WORK(0, a0) = NULL;	// [정리]
		//ATR_BODY_CNT(a0) = 1;
		//((ATR_EQU *)a0->pYobi)->vct_no = 0; //[강제 리셋 실행 가능함]
		times++;
		if (times >= 20){
			times = 0;
			g卡档复位 = 0;
		}
	}

}

BOOL readMap(int floor, int x1, int y1, int x2, int y2, unsigned short *tile, unsigned short *parts, unsigned short *event);
void readHitMap(int, int, int, int, unsigned short *, unsigned short *, unsigned short *, unsigned short *);
void openServerWindow(int windowtype, int buttontype, int index, int id, char *data);
void lssproto_EN_recv(int fd, int result, int field);//[7번 패키지];
void lssproto_B_recv(int fd, char* command);//[15번 패키지]


#ifdef _Jason_修复外挂原登问题		//[핵 사용자 로그인 문제 수정][핵 사용자 로그인 문제 수정]
extern int windowTypeWN;
extern bool LUA_WINDOW_OPEN;
void Test();
#endif

//[필요한 주소 wg에게 전송]
void JL_SendAddrToWg(){
	PostThreadMsg(JL_INDEX_x, (int)&nowGx);
	PostThreadMsg(JL_INDEX_y, (int)&nowGy);
	PostThreadMsg(JL_INDEX_dir, (int)&pc.dir);
	PostThreadMsg(JL_INDEX_print, (int)&StockChatBufferLineExt);
	PostThreadMsg(JL_INDEX_图档移动, (int)&setWarpMap);
	PostThreadMsg(JL_INDEX_图档转动, (int)&setPcDir);
	PostThreadMsg(JL_INDEX_快速走路, (int)&fastWalkFlag);
	PostThreadMsg(JL_INDEX_GameFlag1, (int)&ProcNo);
	PostThreadMsg(JL_INDEX_GameFlag2, (int)&SubProcNo);
	PostThreadMsg(JL_INDEX_mouseX, (int)&mouse.nowPoint.x);
	PostThreadMsg(JL_INDEX_mouseY, (int)&mouse.nowPoint.y);
	PostThreadMsg(JL_INDEX_socket, (int)&sockfd);
	PostThreadMsg(JL_INDEX_加速1, (int)&NO_DRAW_MAX_CNT);		//[가속1]
//	PostThreadMsg(JL_INDEX_加速2, (int)&ProcTime);				//[가속2]
	extern DWORD SystemTime;
	PostThreadMsg(JL_INDEX_加速2, (int)&SystemTime);			//[가속2]
	PostThreadMsg(JL_INDEX_快速战斗, (int)&g_flag_FastFight);	//[쾌속 전투/신속 전투]
	PostThreadMsg(JL_INDEX_自动登陆, (int)&g_flag_自动登陆);	//[자동 로그인]
	PostThreadMsg(JL_INDEX_主机, (int)&g_sel_主机);				//[다양한 의미가 있음: 본체/서버/호스트 컴퓨터 등]
	PostThreadMsg(JL_INDEX_副机, (int)&g_sel_副机);				//[다양한 의미가 있음 : 보조체 / 보조 서버 / 보조 컴퓨터 등]
	PostThreadMsg(JL_INDEX_人物, (int)&g_sel_人物);				//[인물/캐릭터],  [인물/캐릭터])
	PostThreadMsg(JL_INDEX_USERNAME, (int)&idKey);
	PostThreadMsg(JL_INDEX_PASSWORD, (int)&passwd);	
	PostThreadMsg(JL_INDEX_断线重登, (int)&g_flag_断线重登);	//[로그오프 재 로그인]
	PostThreadMsg(JL_INDEX_隐藏石器, (int)&g_hideSA);
	PostThreadMsg(JL_INDEX_登录MAX, (int)写入serverMax);
	PostThreadMsg(JL_INDEX_SHOWTITLE, (int)JL_ShowTitle);
	PostThreadMsg(JL_INDEX_CLOSEDLG, (int)&wnCloseFlag);
	PostThreadMsg(JL_INDEX_RESOMODE, (int)JL_RESOMODE);
	PostThreadMsg(JL_INDEX_SetResoMode, (int)SetResoMode);
	PostThreadMsg(JL_INDEX_ChangeWindowMode, (int)ChangeWindowMode);
	PostThreadMsg(JL_INDEX_RecoverDirectDraw, (int)RecoverDirectDraw);
	PostThreadMsg(JL_INDEX_清除周围人物, (int)JL_清除周围人物);	//[주변 인물/캐릭터 (친구) 제거, [주변 인물/캐릭터 (친구) 
	PostThreadMsg(JL_INDEX_music_end, (int)t_music_end);
	PostThreadMsg(JL_INDEX_卡档复位, (int)&g卡档复位);			//[카드파일 card file 리셋], 
	PostThreadMsg(JL_INDEX_ReadMap, (int)&readMap);				//[로그오프]
	PostThreadMsg(JL_INDEX_ReadHitMap, (int)&readHitMap);
	PostThreadMsg(JL_INDEX_MakeWindow, (int)&openServerWindow);
	PostThreadMsg(JL_INDEX_7号包, (int)&lssproto_EN_recv);		//[7번 패키지]
	PostThreadMsg(JL_INDEX_15号包, (int)&lssproto_B_recv);		//[15번 패키지]
	PostThreadMsg(JL_INDEX_LuaLoadMem, (int)&JL_LuaLoadMem);
	PostThreadMsg(JL_INDEX_showBmpNo, (int)&g_showBmpNo);
	PostThreadMsg(JL_INDEX_CPU优化, (int)&g_CPU优化);			//[CPU 최적화]
	PostThreadMsg(JL_INDEX_断线, (int)断线);					//[로그오프]

	
#ifdef _Jason_修复外挂原登问题					//[핵 사용자 로그인 문제 수정]
	PostThreadMsg(JL_INDEX_windowTypeWN, (int)&windowTypeWN);
	PostThreadMsg(JL_INDEX_LUA_WINDOW_OPEN, (int)&LUA_WINDOW_OPEN);
	PostThreadMsg(JL_INDEX_TestFunc, (int)Test);
#endif
	Sleep(200);
}

#ifdef _Jason_修复外挂原登问题					//[핵 사용자 로그인 문제 수정]
void Test(){
	extern ACTION *pActMenuWnd;
	DeathAction(pActMenuWnd);
	pActMenuWnd = NULL;
}
#endif

// 大五码转GBK码：
void __fastcall BIG52GBK(char *szBuf){
	if (!strcmp(szBuf, ""))
		return;
	int nStrLen = strlen(szBuf);
	wchar_t *pws = new wchar_t[nStrLen + 1];
	__try
	{
		int nReturn = MultiByteToWideChar(950, 0, szBuf, nStrLen, pws, nStrLen + 1);
		BOOL bValue = false;
		nReturn = WideCharToMultiByte(936, 0, pws, nReturn, szBuf, nStrLen + 1, "?", &bValue);
		szBuf[nReturn] = 0;
	}
	__finally
	{
		delete[] pws;
	}
}
// GBK转大五码
void __fastcall GBK2BIG5(char *szBuf)
{
	if (!strcmp(szBuf, ""))
		return;
	int nStrLen = strlen(szBuf);
	wchar_t *pws = new wchar_t[nStrLen + 1];
	__try
	{
		MultiByteToWideChar(936, 0, szBuf, nStrLen, pws, nStrLen + 1);
		BOOL bValue = false;
		WideCharToMultiByte(950, 0, pws, nStrLen, szBuf, nStrLen + 1, "?", &bValue);
		szBuf[nStrLen] = 0;
	}
	__finally
	{
		delete[] pws;
	}
}
// GB2312码转GBK码
void __fastcall GB2GBK(char *szBuf)
{
	if (!strcmp(szBuf, ""))
		return;
	int nStrLen = strlen(szBuf);
	WORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
	int nReturn = LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nStrLen, NULL, 0);
	if (!nReturn)
		return;
	char *pcBuf = new char[nReturn + 1];
	__try
	{
		wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
		LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1);
		strncpy(szBuf, pcBuf, nReturn);
	}
	__finally
	{
		delete[] pcBuf;
	}
}
//test
// GBK码转GB2312码
void __fastcall GBK2GB(char *szBuf)
{
	if (!strcmp(szBuf, ""))
		return;
	int nStrLen = strlen(szBuf);
	WORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_BIG5);
	int nReturn = LCMapString(wLCID, LCMAP_SIMPLIFIED_CHINESE, szBuf, nStrLen, NULL, 0);
	if (!nReturn)
		return;
	char *pcBuf = new char[nReturn + 1];
	__try
	{
		wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_BIG5);
		LCMapString(wLCID, LCMAP_SIMPLIFIED_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1);
		strncpy(szBuf, pcBuf, nReturn);
	}
	__finally
	{
		delete[]pcBuf;
	}
}



int i = 0;
int JL_LuaLoadMem(char* pSrc){
	i++;
	extern MY_Lua MYLua;
	MY_Lua *salua = &MYLua;
	while (salua->next != NULL) {
		salua = salua->next;
	}
	char buf[10];
	sprintf(buf, "%d", i);
	salua->luapath = buf;
	salua->next = (tagMYLua *)new MY_Lua;
	memset(salua->next, 0, sizeof(MY_Lua));
	if (salua->next == NULL)return EXIT_FAILURE;

	salua->lua = lua_open();
	if (salua->lua == NULL) {
		return FALSE;
	}
	lua_gc(salua->lua, LUA_GCSTOP, 0);
	luaL_openlibs(salua->lua);
	luaAB_openlibs(salua->lua);
	lua_gc(salua->lua, LUA_GCRESTART, 0);

	int status = luaL_dostring(salua->lua, (const char *)pSrc);
	lua_getglobal(salua->lua, "main");
	if (lua_isfunction(salua->lua, -1)) {
		docall(salua->lua, 0, 1);
	}
	return TRUE;

#if 0

	extern MY_Lua MYLua;
	MY_Lua *salua = &MYLua;
	while (salua->next != NULL) {
		salua = salua->next;
	}
	salua->luapath = "mem";
	salua->next = (tagMYLua *)new MY_Lua;
	memset(salua->next, 0, sizeof(MY_Lua));
	if (salua->next == NULL)return EXIT_FAILURE;

	salua->lua = lua_open();
	if (salua->lua == NULL) {
		return FALSE;
	}
	lua_gc(salua->lua, LUA_GCSTOP, 0);
	luaL_openlibs(salua->lua);
	luaAB_openlibs(salua->lua);
	lua_gc(salua->lua, LUA_GCRESTART, 0);

	int status = luaL_dostring(salua->lua, (const char *)pSrc);
	lua_getglobal(salua->lua, "main");
	if (lua_isfunction(salua->lua, -1)) {
		docall(salua->lua, 0, 1);
	}
	return TRUE;
#endif
}


int util_mkint(char *buffer, int value);
extern unsigned int sockfd;	// SOCKET ? unsigned int?
void util_SendMesg(int fd, int func, char *buffer);

void JL_SHOP(int index)
{
	char buffer[1000];
	int iChecksum = 0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, index);
	util_mkint(buffer, iChecksum);  
}



void EncryptTEA(unsigned int *firstChunk, unsigned int *secondChunk, unsigned int* key, unsigned int delta)
{
	unsigned int y = *firstChunk;
	unsigned int z = *secondChunk;
	unsigned int sum = 0;


	for (int i = 0; i < 8; i++)//[8라운드 연산 (아래의 복호화 핵심 함수와 라운드 수 일치해야 함)]
	{
		sum += delta;
		y += ((z << 4) + key[0]) ^ (z + sum) ^ ((z >> 5) + key[1]);
		z += ((y << 4) + key[2]) ^ (y + sum) ^ ((y >> 5) + key[3]);
	}

	*firstChunk = y;
	*secondChunk = z;
}

void DecryptTEA(unsigned int *firstChunk, unsigned int *secondChunk, unsigned int* key, unsigned int delta)
{
	unsigned int  sum = 0;
	unsigned int  y = *firstChunk;
	unsigned int  z = *secondChunk;


	sum = delta << 3; //[buffer: 암호화 할 입력 데이터 버퍼의 경우 함수의 메타 데이터 버퍼를 직접 암호화 합니다. size: 버퍼 길이; key는 cipher code임]

	for (int i = 0; i < 8; i++) //8轮运算
	{
		z -= (y << 4) + key[2] ^ y + sum ^ (y >> 5) + key[3];
		y -= (z << 4) + key[0] ^ z + sum ^ (z >> 5) + key[1];
		sum -= delta;
	}

	*firstChunk = y;
	*secondChunk = z;
}

//buffer：输入的待加密数据buffer，在函数中直接对元数据buffer进行加密；size：buffer长度；key是密钥；
int EncryptBuffer(char* buffer, int size, unsigned int* key, unsigned int delta)
{
	char *p = buffer;

	int len = (size % 8 ? size / 8 + 1 : size / 8) * 8;

	int leftSize = len;

	while (p < buffer + len &&
		leftSize >= sizeof(unsigned int) * 2)
	{
		EncryptTEA((unsigned int *)p, (unsigned int *)(p + sizeof(unsigned int)), key, delta);
		p += sizeof(unsigned int) * 2;

		leftSize -= sizeof(unsigned int) * 2;
	}
	return len;
}

//buffer：输入的待解密数据buffer，在函数中直接对元数据buffer进行解密；size：buffer长度；key是密钥；
void DecryptBuffer(char* buffer, int size, unsigned int* key, unsigned int delta)
{
	char *p = buffer;

	int leftSize = size;

	while (p < buffer + size &&
		leftSize >= sizeof(unsigned int) * 2)
	{
		DecryptTEA((unsigned int *)p, (unsigned int *)(p + sizeof(unsigned int)), key, delta);
		p += sizeof(unsigned int) * 2;

		leftSize -= sizeof(unsigned int) * 2;
	}
}


HFONT JL_FONT()			//글꼴 글씨체
{
	HFONT font;
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	// [글꼴 크기]
	extern HWND hWnd;
	HDC hdc = GetDC(hWnd);
	logFont.lfHeight = -MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	logFont.lfWidth = 0;    // [글꼴 너비(기본값)]
	logFont.lfWeight = 0;    // [글꼴 두께 (기본값, FW_BOLD는 두껍게)]
	logFont.lfItalic = 0;    // [이탤릭체]
	logFont.lfUnderline = 0; // [밑줄]
	logFont.lfStrikeOut = 0; // [삭제선]
	logFont.lfCharSet = HANGUL_CHARSET;   // [글꼴집(기본값)]
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = FIXED_PITCH | FF_ROMAN;

										  // [글꼴 이름]
	sprintf(logFont.lfFaceName, TEXT("%s"), TEXT("굴림체"));
	font = CreateFontIndirect(&logFont);
	return font;
}



void 屏幕文字点击(){
	extern int ChatLineSmoothY;
	int i, j, k = 0;
	j = NowChatLine - 1;
	if (j < 0)
		j = MAX_CHAT_LINE - 1;
	if (ChatLineSmoothY > 0)
		k = NowMaxChatLine + 1;
	else
		k = NowMaxChatLine;

	if (k > MAX_CHAT_LINE)
		k = MAX_CHAT_LINE;

	for (int x = 0; x <= j; x++){
		for (i = 0; i < k; i++) {
			if (*ChatBuffer[x].buffer != NULL) {
				CHAT_BUFFER * temp = &ChatBuffer[x];
				while (temp) {
					if( mouse.onceState & MOUSE_RIGHT_CRICK ){
						if (temp->fontID == HitFontNo){ //[이미지가 있는데 마우스를 녹색 상자위에 올려 놓았을 경우]
							if (temp->image){
								lssproto_SaMenu_send(sockfd,5,1,temp->image);
								mouse.onceState = ~MOUSE_RIGHT_CRICK;
								//MessageBox(0, "클릭 성공", 0, 0);
								return;
							}
						}
					}
					temp = temp->NextChatBuffer;
				}
			}
		}
	}
}



char* 说话分行(char* str,int curLen, int maxLen)
{

	if (strlen(str) < maxLen) return 0;

	char outText[512];
	char outText2[512];
	*outText2 = 0;

	char* pNextStr = 0;

	int lenTotal = curLen;

	//查找 { 
	char *temp1 = sunday(str, "{");
	if (temp1 != NULL)
	{
		lenTotal = lenTotal + temp1 - str;
		char *temp2 = sunday(temp1, "}");
		if (temp2 == 0) goto aa; //[특수하지 않은 어구/비특수 어구]

		//[아직도 str=이 있는지 판단할 필요가 있음]

		pNextStr = temp2 + 1;
		
		int len括号长度 = temp2 - temp1;
		memcpy(outText, temp1+1, len括号长度);
		outText[len括号长度] = 0;

		void getStrStr(char* strIn, char* strOut);
		getStrStr(outText, outText2);
		int lenStr = lstrlen(outText2);
		int tempLen = lenTotal + lenStr;
		if (tempLen > maxLen) 
		{
			//需要分行了，分行的位置为{  [행 구분이 필요함, 행을 나누는 위치는 다음과 같음]
			return temp1;
		}

		//如果没有下一个了，则不需要分行 [만약 다음이 없다면 행을 나눌 필요가 없음]
		if (*pNextStr == 0) return 0;

		//继续处理下一个 [계속 다음을 처리함/연산을 수행함]
		return 说话分行(pNextStr, tempLen, maxLen);

	}
	else
	{
	aa:;
		int len = strlen(str);
		if (curLen + len > maxLen)
		{
			char* p = str + maxLen - curLen;
			return p;
		}
	}

	return 0;

}

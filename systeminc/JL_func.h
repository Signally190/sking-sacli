#pragma once
#include "../systeminc/version.h"

#define JL_INDEX_PLAYER_ADDRESS		1	//WG player的地址
#define JL_INDEX_threadID_收包		2
#define JL_INDEX_addrOpenMapping	3
#define JL_INDEX_hwnd_recv			4	//wg用于接收消息的句柄

#define JL_INDEX_x	5
#define JL_INDEX_y	6
#define JL_INDEX_dir	7
#define JL_INDEX_print	8
#define JL_INDEX_图档移动 9
#define JL_INDEX_图档转动 10
#define JL_INDEX_快速走路 11
#define JL_INDEX_GameFlag1 12
#define JL_INDEX_GameFlag2 13
#define JL_INDEX_mouseX 14
#define JL_INDEX_mouseY 15
#define JL_INDEX_socket 16
#define JL_INDEX_加速1 17
#define JL_INDEX_加速2 18
#define JL_INDEX_快速战斗 19
#define JL_INDEX_自动登陆 20
#define JL_INDEX_主机 21
#define JL_INDEX_副机 22
#define JL_INDEX_人物 23
#define JL_INDEX_USERNAME 24
#define JL_INDEX_PASSWORD 25
#define JL_INDEX_断线重登 26
#define JL_INDEX_隐藏石器 27
#define JL_INDEX_登录MAX 28
#define JL_INDEX_SHOWTITLE 29
#define JL_INDEX_CLOSEDLG 30
#define JL_INDEX_RESOMODE 31

#define JL_INDEX_SetResoMode 32
#define JL_INDEX_ChangeWindowMode 33
#define JL_INDEX_RecoverDirectDraw 34
#define JL_INDEX_清除周围人物 35
#define JL_INDEX_music_end 36
#define JL_INDEX_卡档复位 37
#define JL_INDEX_ReadMap 38
#define JL_INDEX_ReadHitMap 39
#define JL_INDEX_MakeWindow 40
#define JL_INDEX_7号包 41
#define JL_INDEX_15号包 42
#define JL_INDEX_关闭声音 43
#define JL_INDEX_WindMsgProc 44
#define JL_INDEX_LuaLoadMem 45
#define JL_INDEX_发送登陆包 46
#define JL_INDEX_showBmpNo 47
#define JL_INDEX_CPU优化 48
#define JL_INDEX_断线 49

#ifdef _Jason_修复外挂原登问题
#define JL_INDEX_windowTypeWN 50
#define JL_INDEX_LUA_WINDOW_OPEN 51
#define JL_INDEX_TestFunc 52
#endif

#define SERVER_情缘 0
#define SERVER_台服 1
#define SERVER_8020 2
#ifdef _CC
#define SERVER服务器 SERVER_8020
#else	
#define SERVER服务器 SERVER_情缘
#endif // _CC

enum 分辨率
{
	mode640 = 0,
	mode800 = 3,
	mode1024 = 4,
};

#define int台服发包 0x12AC21

void DecryptBuffer(char* buffer, int size, unsigned int* key, unsigned int delta);
int EncryptBuffer(char* buffer, int size, unsigned int* key, unsigned int delta);
void JL_SHOP(int index);
void JL_转到外挂发登陆包();
void JL_SendAddrToWg();
int JL_GetAddress(int index);
void JL_SetAddr(int index, int value);
void JL_savefunc(int func);
void JL_resetRecv();
void JL_combineInt_RECV(int a, int addr);
void JL_combineString_RECV(char* s, int len);
void JL_combineInt_SEND(int a);
void JL_combineString_SEND(char* s, int len);
void JL_combinePacket_SEND(int func, int addr);
void JL_自动战斗();
void dw2hex(int a, char* buf);
void PostThreadMsg(int index, int value);
void JL_SetFastFightFlag(bool bFastFight);
int JL_自动登陆选择服务器();
int JL_自动登陆选择副机();
int JL_自动登陆选择人物();
int JL_自动登陆账号密码点OK();
void JL_锁定人物(bool bSet);
void JL_关闭特效_回记录点();
int JL_LuaLoadMem(char* pSrc);

void __fastcall GBK2GB(char *szBuf);
void __fastcall GB2GBK(char *szBuf);
void __fastcall GBK2BIG5(char *szBuf);
void __fastcall BIG52GBK(char *szBuf);

int split_damage(char* src, char delim, int* pArraySplitAddr, unsigned int maxCount);


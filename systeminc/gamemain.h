// gamemain.cpp

#ifndef _GAMEMAIN_H_
#define _GAMEMAIN_H_

enum{
	GAME_LOGIN,					// ????
	GAME_FIELD,					// ????????
	GAME_FIELD_TO_ENCOUNT,		// ??????????????????
	GAME_ENCOUNT_TO_BATTLE,		// ?????????
	GAME_BATTLE				// ?????????
};


extern int	GameState;
extern	DWORD NowTime;
//extern double NowTime;

BOOL GameMain( void );

BOOL InitGame( void );

void EndGame( void );
int getMAC(char * mac);
int getCpuId(char * cpuid);
bool GetSysInfo(char* out);

#ifdef _STONDEBUG_		
void InfoDisp( void );
#endif

void DisplayFrameRate( void );

#endif

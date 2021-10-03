#include <stdio.h>
#include <winsock.h>
#include <time.h>

#include "../systeminc/version.h"
#include"../systeminc/system.h"
#include "../systeminc/netmain.h"
#include "../systeminc/netproc.h"
#include "../systeminc/lssproto_cli.h"
#include "../systeminc/savedata.h"
#include "../systeminc/process.h"
#include "../systeminc/pc.h"
#include "../systeminc/tool.h"
#include "../systeminc/map.h"
#include "../systeminc/character.h"
#include "../systeminc/action.h"
#include "../systeminc/battlemenu.h"
#include "../systeminc/battleProc.h"
#include "../systeminc/battleMap.h"
#include "../systeminc/menu.h"
#include "../systeminc/anim_tbl.h"
#include "../systeminc/login.h"
#include "../systeminc/handletime.h"
#include "../systeminc/field.h"
#include "../systeminc/t_music.h"
#include "../systeminc/battleMenu.h"
#include "../wgs/message.h"
#include "../wgs/descrypt.h"
#include "./test.h"
#include "../NewProto/autil.h"
#ifdef _REMAKE_20
#include "../MMOGprotect.h"
#endif
#include "../systeminc/VMProtectSDK.h"
#include "../systeminc/JL_func.h"

#ifdef _TALK_WINDOW
#include "../systeminc/TalkWindow.h"
#endif
//#include <openssl/md5.h>

#ifdef _NB_断线自动重连
#include "../systeminc/pc.h"
extern Landed PcLanded;
#endif
//JL fix 鬼知道是什么,干掉没问题
//#pragma comment(lib, "libeay32.lib")
//#pragma comment(lib, "ssleay32.lib")


#ifdef  _STONDEBUG_
extern int g_iMallocCount;
#endif
#define NETPROC_NOTSEND     0           // ??????????
#define NETPROC_SENDING     1           // ??????
#define NETPROC_RECEIVED    2           // ???
#define SET_SENDTIME(time)	time = TimeGetTime()
#define CHECK_TIMEOUT(time, timeout) \
	if ((TimeGetTime() - (time)) > (timeout)) \
	return -1;
#define SETSENDING  netproc_sending = NETPROC_SENDING; start_time = TimeGetTime();
#define SETTIMEOUT(msg, state) \
	if ((TimeGetTime() - start_time) > TIMEOUT)	\
	return;
#define SETTIMEOUT2(msg) \
	if ((TimeGetTime() - start_time) > TIMEOUT)	\
{											\
	sprintf( netprocErrmsg, msg );			\
	return -1;								\
}
#define SETNEEDCHOOSE(mode)	\
	if (server_choosed == 0) \
	return;


extern char szUser[], szPassword[];

short selectServerIndex = -1;
short clientLoginStatus = 0;
short charListStatus = 0;
#ifdef _PK2007
short pkListStatus = 0;
#endif
short charDelStatus = 0;
short newCharStatus = 0;
short charLoginStatus = 0;
short charLogoutStatus = 0;

#ifdef _MAILSHOWPLANET				// (可开放) Syu ADD 显示名片星球
extern struct gameserver gmsv[];
#endif


#ifdef _NEW_WGS_MSG				// WON ADD WGS的新视窗
int ERROR_MESSAGE = 0;
#endif

// GAMESTATE_CHOOSESERVER??????????????
char gamestate_chooseserver_name[128];
// GAMESTATE_LOGIN??????????????
char gamestate_login_charname[128];
// GAMESTATE_DELETECHAR??????????????
char gamestate_deletechar_charname[128];
char netprocErrmsg[1024];
int netproc_sending = NETPROC_NOTSEND; 
DWORD start_time = 0;
BOOL bNewServer;
char c_temp[1024];
int connectServerCounter = 0;

#ifdef _SKILLSORT
int AdvanceSkill[20] = {-1};
int AssitSkill[20] = {-1};
int BattleSkill[20] = {-1};
void SortSkill()
{
	int count1 = 0, count2 = 0, count3 = 0;

	for (int i = 0; i < 20; i++)
	{
		AdvanceSkill[i] = -1;
		AssitSkill[i] = -1;		
		BattleSkill[i] = -1;
	}
	for (int i = 0; i < 26; i++)
	{
		switch (profession_skill[i].kind)
		{
		case 1: // 战斗技能
			BattleSkill[count1] = i;
			count1++;
			break;
		case 2: // 辅助
			AssitSkill[count2] = i;
			count2++;
			break;
		case 3: // 进阶
			AdvanceSkill[count3] = i;
			count3++;
			break;
		}
	}
}
#endif

#ifdef _CHANNEL_MODIFY
// 储存对话内容
void SaveChatData(char *msg,char KindOfChannel,bool bCloseFile);
#endif
void initConnectServer(void)
{
	connectServerCounter = 0;
}

void LogToRecvdata2(char *data)
{
	//printf("收到：\t%s\n", data);
	char lssproto_readlogfilename[256] = "recvdata.txt";

	if (lssproto_readlogfilename[0] != '\0')
	{
		FILE *rfp;
		rfp = fopen(lssproto_readlogfilename, "a+");
		if (rfp)
		{
			fprintf(rfp, "收到：\t%s\n", data);
			fclose(rfp);
		}
	}
}

extern int	iEncrypt;
int ConnectWGS()
{
	if (connectServerCounter == 0)
	{
		HANDLE hFileMapping = NULL;
		LPSTR lpstr = NULL;

		cleanupNetwork();
		if (initNet())
		{
			unsigned long flg = 1;
			struct sockaddr_in sin;
			struct hostent *h;

			iWGS = 0;
			iEncrypt = E_INIT;
			start_time = TimeGetTime();
			if (INVALID_SOCKET == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
			{
#ifdef _NB_断线自动重连
				PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
				sprintf(netprocErrmsg, NET_ERRMSG_SOCKETERROR);
				return -3;
			}
			ioctlsocket(sockfd, FIONBIO, &flg);
			sin.sin_family = AF_INET;
			sin.sin_port = htons(wWGS);
			sin.sin_addr.s_addr = inet_addr(szWGS);
			if (sin.sin_addr.s_addr == -1)
			{
				if (!(h = gethostbyname(szWGS)))
				{
#ifdef _NB_断线自动重连
					PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
					sprintf(netprocErrmsg, NET_ERRMSG_NOTGETADDR, "WGS");
					closesocket(sockfd);
					dwServer = NULL;
					return -4;
				}
				memcpy((void*)&sin.sin_addr.s_addr, h->h_addr, sizeof(struct in_addr));
			}
			int ret = connect(sockfd, (struct sockaddr*)&sin, sizeof(sin));
			if (ret == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					closesocket(sockfd);
					dwServer = NULL;
#ifdef _NB_断线自动重连
					PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
					sprintf(netprocErrmsg, NET_ERRMSG_NOTCONNECT_S);
					return -5;
				}
			}
			dwServer = WGS;
			iWGS = 1;
			connectServerCounter = 1;
		}
		else
		{
#ifdef _NB_断线自动重连
			PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
			strcpy(netprocErrmsg, NET_ERRMSG_SOCKLIBERROR);
			return -1;
		}
	}else if (connectServerCounter >= 1 && connectServerCounter <= 70){
		fd_set rfds, wfds, efds;
		struct timeval tm;

		if (++connectServerCounter == 70)
			connectServerCounter = 69;
		tm.tv_sec = 0;
		tm.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		FD_SET(sockfd, &rfds);
		FD_SET(sockfd, &wfds);
		FD_SET(sockfd, &efds);
		int a = select(2, &rfds, &wfds, &efds, &tm);
		if (FD_ISSET(sockfd, &wfds))
		{
			connectServerCounter = 71;
			server_choosed = 100;
		}
#ifdef _OMIT_WGS
		testtest();
		connectServerCounter = 143;   // 暂定
		iWGS = 7;
#endif
		if (FD_ISSET(sockfd, &efds))
		{
#ifdef _NB_断线自动重连
			PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
			sprintf(netprocErrmsg, NET_ERRMSG_NOTCONNECT);
			closesocket(sockfd);
			dwServer = NULL;
			return -6;
		}
	}
	else if (connectServerCounter == 71)
	{
		if (iWGS == 2)
		{
			unsigned long flg = 1;
			struct sockaddr_in sin;
			struct hostent *h;

			closesocket(sockfd);
			dwServer = NULL;
			if (INVALID_SOCKET == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
			{
#ifdef _NB_断线自动重连
				PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
				sprintf(netprocErrmsg, NET_ERRMSG_SOCKETERROR);
				return -3;
			}
			ioctlsocket(sockfd, FIONBIO, &flg);
			sin.sin_family = AF_INET;
			sin.sin_port = htons(wCS);
			sin.sin_addr.s_addr = inet_addr(szCSIP);
			if (sin.sin_addr.s_addr == -1)
			{
				if (!(h = gethostbyname(szCSIP)))
				{
#ifdef _NB_断线自动重连
					PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
					sprintf(netprocErrmsg, NET_ERRMSG_NOTGETADDR, "CS");
					closesocket(sockfd);
					dwServer = NULL;
					return -4;
				}
				memcpy((void*)&sin.sin_addr.s_addr, h->h_addr, sizeof(struct in_addr));
			}
			int ret = connect(sockfd, (struct sockaddr*)&sin, sizeof(sin));
			if (ret == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					closesocket(sockfd);
					dwServer = NULL;
#ifdef _NB_断线自动重连
					PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
					sprintf(netprocErrmsg, NET_ERRMSG_NOTCONNECT_S);
					return -5;
				}
			}
			dwServer = CS;
			iWGS = 3;
			connectServerCounter = 72;
		}
	}
	else if (connectServerCounter >= 72 && connectServerCounter <= 140)
	{
		fd_set rfds, wfds, efds;
		struct timeval tm;

		if (++connectServerCounter == 140)
			connectServerCounter = 139;
		tm.tv_sec = 0;
		tm.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		FD_SET(sockfd, &rfds);
		FD_SET(sockfd, &wfds);
		FD_SET(sockfd, &efds);
		int a = select(2, &rfds, &wfds, &efds, &tm);
		if (FD_ISSET(sockfd, &wfds))
		{
			connectServerCounter = 141;
			server_choosed = 100;
		}
		if (FD_ISSET(sockfd, &efds))
		{
#ifdef _NB_断线自动重连
			PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
			sprintf(netprocErrmsg, NET_ERRMSG_NOTCONNECT);
			closesocket(sockfd);
			dwServer = NULL;
			return -6;
		}
	}
	else if (connectServerCounter == 141)
	{
		if (iWGS == 4)
		{
			DoSKey();
			connectServerCounter = 142;
			iWGS = 5;
		}
	}
	else if (connectServerCounter == 142)
	{
		if (iWGS == 6)
		{
			DoHellow();
			connectServerCounter = 143;
			iWGS = 7;
		}
	}
	else if (connectServerCounter == 143)
	{
		if (iWGS == 8)
		{
			cleanupNetwork();
			disconnectServerFlag = TRUE;
			oldDisconnectServerFlag = TRUE;
			closesocket(sockfd);
			dwServer = GS;
			return 1;
		}
	}
	if (iWGS == 9)
		return -7;
	SETTIMEOUT2(NET_ERRMSG_CONNECTTIMEOUT);
	return 0;
}

#ifdef _NB_登录错误信息提示
char 登录错误提示[512];
#endif
struct sockaddr_in sin_server;
int connectServer(void)
{
	if (!init_net)
		return 0;
	static int count = 0;

	if (connectServerCounter == 0){
		char hostname[128];
		short pt;
		unsigned long flg = 1;
		struct hostent *h;

		lstrcpy(PersonalKey, _DEFAULT_PKEY);
		//lstrcpy(PersonalKey, "forever");
		start_time = TimeGetTime();
		if (getServerInfo(selectServerIndex, hostname, &pt) < 0)
		{
			count = 0;
#ifdef _NB_断线自动重连
			PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
			sprintf(netprocErrmsg, NET_ERRMSG_BADNAME);
			return -2;
		}
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == INVALID_SOCKET)
		{
			count = 0;
#ifdef _NB_断线自动重连
			PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
			sprintf(netprocErrmsg, NET_ERRMSG_SOCKETERROR);
			return -3;
		}
		ioctlsocket(sockfd, FIONBIO, &flg);

		extern BOOL NoDelay;
		if (NoDelay)
		{
			int flag = 1;

			if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int)) != 0)
			{
				count = 0;
				return -100;
			}
		}
		ZeroMemory(&sin_server, sizeof(sin_server));
		sin_server.sin_family = AF_INET;
		sin_server.sin_port = htons(pt);
		sin_server.sin_addr.s_addr = inet_addr(hostname);     /* accept only dot notaion  */
		if (sin_server.sin_addr.s_addr == -1)
		{
			h = gethostbyname(hostname);
			if (h == NULL)
			{
				count = 0;
#ifdef _NB_断线自动重连
				PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
				sprintf(netprocErrmsg, NET_ERRMSG_NOTGETADDR, hostname);
				closesocket(sockfd); 
				dwServer = NULL;
				return -4;
			}
			memcpy((void*)&sin_server.sin_addr.s_addr, h->h_addr, sizeof(struct in_addr));
		}
		// Non blocking Connect
		int ret = connect(sockfd, (struct sockaddr*)&sin_server, sizeof(sin_server));
		if (ret == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				count = 0;
				closesocket(sockfd);
				dwServer = NULL;
#ifdef _NB_断线自动重连
				PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
				sprintf(netprocErrmsg, NET_ERRMSG_NOTCONNECT_S);
				return -5;
			}
			connectServerCounter = 1;
			c_temp[0] = 0;
			bNewServer = FALSE;
			/*sprintf(netprocErrmsg, NET_ERRMSG_NOTCONNECT);
			closesocket(sockfd);
			dwServer = NULL;
			return -6; */
		}
	}else if (connectServerCounter >= 1 && connectServerCounter <= 70){
		fd_set rfds, wfds, efds;
		struct timeval tm;

		connectServerCounter++;
		if (connectServerCounter == 70)
			connectServerCounter = 69;

		tm.tv_sec = 0;
		tm.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		FD_SET(sockfd, &rfds);
		FD_SET(sockfd, &wfds);
		FD_SET(sockfd, &efds);
		int a = select(sockfd, &rfds, &wfds, &efds, &tm);
		if (a > 0){
			if (FD_ISSET(sockfd, &rfds)){
				// Nuke start
				int i_len = recv(sockfd, c_temp, sizeof(c_temp) - 1, 0);
				if (i_len > 0){
					
					if (c_temp[0] == 'E'){
#ifdef _NB_断线自动重连
						PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
						count = 0;
						sprintf(netprocErrmsg, c_temp + 1);
						closesocket(sockfd);
						dwServer = NULL;
						return -6;
					}else if (c_temp[0] == _SA_VERSION){
						bNewServer = 0xf000000 | 1;
						if (FD_ISSET(sockfd, &wfds)){
							connectServerCounter = 71;					
							server_choosed = 1;
						}
					}else{
#ifdef _NB_断线自动重连
						PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
						count = 0;
						sprintf(netprocErrmsg, NET_ERRMSG_VERSIONERROR);
						closesocket(sockfd);
						dwServer = NULL;
						return -8;
					}
				}else{
#ifdef _NB_断线自动重连
					PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
					count = 0;
					sprintf(netprocErrmsg, NET_ERRMSG_NOTCONNECT);
					closesocket(sockfd);
					dwServer = NULL;
					return -6; 
				}
			}
			if (FD_ISSET(sockfd, &efds)){
#ifdef _NB_断线自动重连
				PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
				count = 0;
				sprintf(netprocErrmsg, NET_ERRMSG_NOTCONNECT);
				closesocket(sockfd);
				dwServer = NULL;
				return -6; 
			}
		}else{
			count++;
			if(count >= 1500){
#ifdef _NB_断线自动重连
				PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
				count = 0;
				sprintf(netprocErrmsg, NET_ERRMSG_NOTCONNECT);
				closesocket(sockfd);
				dwServer = NULL;
				return -6; 
			}
		}
	}else if (connectServerCounter >= 71 && connectServerCounter <= 80){
		count = 0;
		if (connectServerCounter == 71){
VMProtectBegin("login");
			char userId[32], userPassword[32];

			clientLoginStatus = 0;
			CopyMemory(userId, szUser, 32);
			ecb_crypt("f;encor1c", userId, 32, DES_DECRYPT);
			CopyMemory(userPassword, szPassword, 32);
			ecb_crypt("f;encor1c", userPassword, 32, DES_DECRYPT);

			if (bNewServer)
				lssproto_ClientLogin_send(sockfd, userId, userPassword);
			else
				old_lssproto_ClientLogin_send(sockfd, userId, userPassword);

			//cary 2001 9 6
			if ((bNewServer & 0xf000000) == 0xf000000){
				lstrcpy(PersonalKey, userId);
				lstrcat(PersonalKey, _RUNNING_KEY);
			}else{
				lstrcpy(PersonalKey, userId);
				lstrcat(PersonalKey, "19761101");
			}
			netproc_sending = NETPROC_SENDING;
VMProtectEnd();
		}
		if (netproc_sending == NETPROC_RECEIVED){
			if (clientLoginStatus)
				connectServerCounter = 81;
			else{
#ifdef _NB_断线自动重连
				PcLanded.登陆延时时间 = TimeGetTime() + 2000;
#endif
				count = 0;
				netproc_sending = NETPROC_NOTSEND;
#ifdef _NB_登录错误信息提示
				sprintf(netprocErrmsg, 登录错误提示);
#else
				sprintf(netprocErrmsg, NET_ERRMSG_LOGINFAIL);
#endif
				closesocket(sockfd);
				dwServer = NULL;
				return -7;
			}
		}
		connectServerCounter++;
		if (connectServerCounter == 81)
			connectServerCounter = 80;
	}
	else if (connectServerCounter >= 81 && connectServerCounter <= 98)
		connectServerCounter ++;
	else if (connectServerCounter == 99){
		netproc_sending = NETPROC_NOTSEND;
		return 1;
	}
	SETTIMEOUT2(NET_ERRMSG_CONNECTTIMEOUT);
	return 0;
}

void lssproto_ClientLogin_recv(int fd, char *result){
	if (netproc_sending == NETPROC_SENDING){
		netproc_sending = NETPROC_RECEIVED;	
		if (strcmp(result, OKSTR) == 0){
			clientLoginStatus = 1;
			time(&serverAliveLongTime);
			serverAliveTime = localtime(&serverAliveLongTime);

		}else if(strcmp (result, CANCLE) == 0){
			cleanupNetwork();
			PaletteChange(DEF_PAL, 0);
			ChangeProc( PROC_ID_PASSWORD );
			SubProcNo = 5;
			DeathAllAction();
		}else{
#ifdef _NB_登录错误信息提示
			sprintf(登录错误提示, "%s", result);
#endif
		}
	}
}

void charListStart(void){
	int i;

	for (i = 0; i < MAXCHARACTER; i++)
		resetCharacterList(i);

	charListStatus = 0;
	if (bNewServer)
		lssproto_CharList_send(sockfd);
	else
		old_lssproto_CharList_send(sockfd);
	SETSENDING;
}

int charListProc(void){
	if (netproc_sending == NETPROC_RECEIVED){
		netproc_sending = NETPROC_NOTSEND;
		if (charListStatus == 1)
			return 1;
		else if (charListStatus == 2)
			return -3;
#ifdef _CHANGEGALAXY
		else if (charListStatus == 3)
			return -12;
#endif
#ifdef _ERROR301
		else if (charListStatus == 4)
			return -13;
#endif
		else
			return -2;
	}
	SETTIMEOUT2(NET_ERRMSG_CHARLISTTIMEOUT);
	return 0;
}

void lssproto_CharList_recv(int fd, char *result, char *data){

	if(strcmp(result,"failed")==0) {
#ifdef _NB_断线自动重连
		PcLanded.登陆延时时间 = TimeGetTime()+2000;
#endif
	}
	if (netproc_sending == NETPROC_SENDING){
		char nm[1024], opt[1024];
		int i;
		netproc_sending = NETPROC_RECEIVED;
		if (strcmp(result, SUCCESSFULSTR) != 0){
			if (strcmp(data, "OUTOFSERVICE") == 0)
				charListStatus = 2;
#ifdef _CHANGEGALAXY
			if (strcmp(data, "CHANGE_GALAXY") == 0)
				charListStatus = 3;
#endif
#ifdef _ERROR301
			if (strcmp(data, "301") == 0)
				charListStatus = 4;
#endif
			return;
		}
		charListStatus = 1;
		for (i = 0; i < MAXCHARACTER; i++){
			strcpy(nm, "");
			strcpy(opt, "");
			getStringToken(data, '|', i * 2 + 1, sizeof(nm) - 1 , nm);
			getStringToken(data, '|', i * 2 + 2, sizeof(opt) - 1, opt);
			setCharacterList(nm, opt);
		}
	}
}

#ifdef _PK2007
void pkListStart(void){
	pkListStatus = 0;
	lssproto_pkList_send(sockfd);
	SETSENDING;
}
int pkListProc(void){
	if (netproc_sending == NETPROC_RECEIVED){
		netproc_sending = NETPROC_NOTSEND;
		if (pkListStatus == 1)
			return 1;
		else if (pkListStatus == 2)
			return 2;
	}
	SETTIMEOUT2(NET_ERRMSG_CHARLISTTIMEOUT);
	return 0;
}
void lssproto_pkList_recv(int fd, int count, char *data){
	if (netproc_sending == NETPROC_SENDING){
		char nm[1024], opt[1024];
		int i;
		netproc_sending = NETPROC_RECEIVED;
		if (count == 0)
			pkListStatus = 2;	
		else
			pkListStatus = 1;
	}
}
#endif


void charLoginStart(void){
	charLoginStatus = 0;
#ifdef _NEW_WGS_MSG				// WON ADD WGS的新视窗
	ERROR_MESSAGE = 0;
#endif
	if (bNewServer)
		lssproto_CharLogin_send(sockfd, gamestate_login_charname);
	else
		old_lssproto_CharLogin_send(sockfd, gamestate_login_charname);
	SETSENDING;
}

int charLoginProc(void){
	SendCount = 30;
	tradeStatus = 0;
	if (!server_choosed)
		return 0;
	if (netproc_sending == NETPROC_RECEIVED){
		netproc_sending = NETPROC_NOTSEND;
		if (charLoginStatus == 1)
			return 1;
		else
#ifdef _NEW_WGS_MSG				// WON ADD WGS的新视窗
			if (ERROR_MESSAGE != 0)
				return ERROR_MESSAGE;
			else
#endif
				return -2;
	}
	SETTIMEOUT2(NET_ERRMSG_LOGINTIMEOUT);
	return 0;
}

#ifdef __NEW_CLIENT
HANDLE hPing = NULL;
DWORD dwPingID;
DWORD WINAPI PingFunc(LPVOID param);
#endif

void lssproto_CharLogin_recv(int fd, char* result, char* data){
	if (netproc_sending == NETPROC_SENDING){
		netproc_sending = NETPROC_RECEIVED;
#ifdef __NEW_CLIENT
		if (strcmp(result, SUCCESSFULSTR) == 0 && !hPing)
#else
		if (strcmp(result, SUCCESSFULSTR) == 0)
#endif
		{
			charLoginStatus = 1;
#ifdef __NEW_CLIENT
			hPing = CreateThread(NULL, 0, PingFunc, &sin_server.sin_addr, 0, &dwPingID);
#endif
		}

#ifdef _NEW_WGS_MSG				// WON ADD WGS的新视窗
		if (strcmp(result, "failed" ) == 0 && !hPing)
			ERROR_MESSAGE = atoi(data);
#endif

#ifdef _ANGEL_SUMMON
		angelFlag = FALSE;
		angelMsg[0] = NULL;
#endif
	}
}

void charLogoutStart(void)
{
	charLogoutStatus = 0;
	if (bNewServer)
		lssproto_CharLogout_send(sockfd, 1);
	else
		old_lssproto_CharLogout_send(sockfd);
	SETSENDING;
}

int charLogoutProc(void)
{
	if (netproc_sending == NETPROC_RECEIVED)
	{
		netproc_sending = NETPROC_NOTSEND;
		if (charLogoutStatus == 1)
			return 1;
		else
			return -2;
    }
	SETTIMEOUT2(NET_ERRMSG_LOGOUTTIMEOUT);
	return 0;
}

void lssproto_CharLogout_recv(int fd, char *result, char *data)
{
	if (netproc_sending == NETPROC_SENDING)
	{
		netproc_sending = NETPROC_RECEIVED;
		if (strcmp(result, SUCCESSFULSTR) == 0)
			charLogoutStatus = 1;
	}
}

#define S_DELIM '|'

/*================================
C warp 用
D 修正时间
X 骑宠
P 人物状态
F 家族状态
M HP,MP,EXP
K 宠物状态
E nowEncountPercentage
J 魔法
N 队伍资讯
I 道具
W 宠物技能
S 职业技能
G 职业技能冷却时间
================================*/
void lssproto_S_recv(int fd, char *data){
	if (logOutFlag)
		return;
	switch (data[0]){
		case 'C':
		{
			int fl, maxx, maxy, gx, gy;

			floorChangeFlag = TRUE;
			if (!loginFlag && ProcNo == PROC_GAME){
				if (!warpEffectFlag){				
					SubProcNo = 200;
					//JLWG 回记录点特效去除
					JL_关闭特效_回记录点();
					warpEffectProc();
					if (MenuToggleFlag & JOY_CTRL_M)
						MapWmdFlagBak = TRUE;
				}
				resetPc();
				warpEffectFlag = FALSE;
				warpEffectStart = TRUE;

			}
			data++;
			fl   = getIntegerToken(data, S_DELIM, 1);
			maxx = getIntegerToken(data, S_DELIM, 2);
			maxy = getIntegerToken(data, S_DELIM, 3);
			gx   = getIntegerToken(data, S_DELIM, 4);
			gy   = getIntegerToken(data, S_DELIM, 5);
			setMap(fl, gx, gy);
			createMap(fl, maxx, maxy);
			nowFloorGxSize = maxx;
			nowFloorGySize = maxy;
			resetCharObj();
			mapEmptyFlag = FALSE;
			nowEncountPercentage = minEncountPercentage;
			nowEncountExtra = 0;
			resetMap();
			transmigrationEffectFlag = 0;
			//JL fix 2017.9.22 切地图时,将组队的act都清0,这样打精灵王以及过地图就不崩了
			clearPtActPartyParam();
            break;
		}
		case 'D':
			data++;
			setPcId(getIntegerToken(data, S_DELIM, 1));
			serverTime = getIntegerToken(data, S_DELIM, 2);
			FirstTime = TimeGetTime();
			RealTimeToSATime(&SaTime);
			SaTimeZoneNo = getLSTime(&SaTime);
			PaletteChange(SaTimeZoneNo, 0);
			break;
		//andy_add
		case 'X':
			pc.lowsride = getIntegerToken(data, S_DELIM, 2);
			break;
		case 'P':
			{
				char name[256], freeName[256];
				int i, kubun;
				unsigned int mask;

				data++;
				kubun = getInteger62Token(data, S_DELIM, 1);
				if (!bNewServer)
					pc.ridePetNo = -1;
				if (kubun == 1)
				{
					pc.hp		= getIntegerToken(data, S_DELIM, 2);		// 0x00000002
					pc.maxHp	= getIntegerToken(data, S_DELIM, 3);		// 0x00000004
					pc.mp		= getIntegerToken(data, S_DELIM, 4);		// 0x00000008
					pc.maxMp	= getIntegerToken(data, S_DELIM, 5);		// 0x00000010
					pc.vital	= getIntegerToken(data, S_DELIM, 6);		// 0x00000020
					pc.str		= getIntegerToken(data, S_DELIM, 7);		// 0x00000040
					pc.tgh		= getIntegerToken(data, S_DELIM, 8);		// 0x00000080
					pc.dex		= getIntegerToken(data, S_DELIM, 9);		// 0x00000100
					pc.exp		= getIntegerToken(data, S_DELIM, 10);		// 0x00000200
					pc.maxExp	= getIntegerToken(data, S_DELIM, 11);		// 0x00000400
					pc.level	= getIntegerToken(data, S_DELIM, 12);		// 0x00000800
					pc.atk		= getIntegerToken(data, S_DELIM, 13);		// 0x00001000
					pc.def		= getIntegerToken(data, S_DELIM, 14);		// 0x00002000
					pc.quick	= getIntegerToken(data, S_DELIM, 15);		// 0x00004000
					pc.charm	= getIntegerToken(data, S_DELIM, 16);		// 0x00008000
					pc.luck		= getIntegerToken(data, S_DELIM, 17);		// 0x00010000
					pc.earth	= getIntegerToken(data, S_DELIM, 18);		// 0x00020000
					pc.water	= getIntegerToken(data, S_DELIM, 19);		// 0x00040000
					pc.fire		= getIntegerToken(data, S_DELIM, 20);		// 0x00080000
					pc.wind		= getIntegerToken(data, S_DELIM, 21);		// 0x00100000
					pc.gold		= getIntegerToken(data, S_DELIM, 22);		// 0x00200000
					pc.titleNo	= getIntegerToken(data, S_DELIM, 23);		// 0x00400000
					pc.dp		= getIntegerToken(data, S_DELIM, 24);		// 0x00800000
					pc.transmigration = getIntegerToken(data, S_DELIM, 25);// 0x01000000
					pc.ridePetNo = getIntegerToken(data, S_DELIM, 26);	// 0x02000000
					pc.learnride = getIntegerToken(data, S_DELIM, 27);	// 0x04000000
					pc.baseGraNo = getIntegerToken(data, S_DELIM, 28);	// 0x08000000
#ifdef _NEW_RIDEPETS
					pc.lowsride = getIntegerToken(data, S_DELIM, 29);		// 0x08000000
#endif
					getStringToken(data, S_DELIM, 30, sizeof(name) - 1, name);
					makeStringFromEscaped(name);
					if (strlen(name) <= CHAR_NAME_LEN)
						strcpy(pc.name, name);
					getStringToken(data, S_DELIM, 31, sizeof(freeName) - 1, freeName);
					makeStringFromEscaped(freeName);
					if (strlen(freeName) <= CHAR_FREENAME_LEN)
						strcpy(pc.freeName, freeName);
				}
				else
				{
					mask = 2;
					i = 2;
					for (; mask > 0; mask <<= 1)
					{
						if (kubun & mask)
						{
							if (mask == 0x00000002) // ( 1 << 1 )
							{
								pc.hp = getIntegerToken(data, S_DELIM, i);// 0x00000002
								i++;
							}
							else if (mask == 0x00000004) // ( 1 << 2 )
							{
								pc.maxHp = getIntegerToken(data, S_DELIM, i);// 0x00000004
								i++;
							}
							else if (mask == 0x00000008)
							{
								pc.mp = getIntegerToken(data, S_DELIM, i);// 0x00000008
								i++;
							}
							else if (mask == 0x00000010)
							{
								pc.maxMp = getIntegerToken(data, S_DELIM, i);// 0x00000010
								i++;
							}
							else if (mask == 0x00000020)
							{
								pc.vital = getIntegerToken(data, S_DELIM, i);// 0x00000020
								i++;
							}
							else if (mask == 0x00000040)
							{
								pc.str = getIntegerToken(data, S_DELIM, i);// 0x00000040
								i++;
							}
							else if (mask == 0x00000080)
							{
								pc.tgh = getIntegerToken(data, S_DELIM, i);// 0x00000080
								i++;
							}
							else if (mask == 0x00000100)
							{
								pc.dex = getIntegerToken(data, S_DELIM, i);// 0x00000100
								i++;
							}
							else if (mask == 0x00000200)
							{
								pc.exp = getIntegerToken(data, S_DELIM, i);// 0x00000200
								i++;
							}
							else if (mask == 0x00000400)
							{
								pc.maxExp = getIntegerToken(data, S_DELIM, i);// 0x00000400
								i++;
							}
							else if (mask == 0x00000800)
							{
								pc.level = getIntegerToken(data, S_DELIM, i);// 0x00000800
								i++;
							}
							else if (mask == 0x00001000)
							{
								pc.atk = getIntegerToken(data, S_DELIM, i);// 0x00001000
								i++;
							}
							else if (mask == 0x00002000)
							{
								pc.def = getIntegerToken(data, S_DELIM, i);// 0x00002000
								i++;
							}
							else if (mask == 0x00004000)
							{
								pc.quick = getIntegerToken(data, S_DELIM, i);// 0x00004000
								i++;
							}
							else if (mask == 0x00008000)
							{
								pc.charm = getIntegerToken(data, S_DELIM, i);// 0x00008000
								i++;
							}
							else if (mask == 0x00010000)
							{
								pc.luck = getIntegerToken(data, S_DELIM, i);// 0x00010000
								i++;
							}
							else if (mask == 0x00020000)
							{
								pc.earth = getIntegerToken(data, S_DELIM, i);// 0x00020000
								i++;
							}
							else if (mask == 0x00040000)
							{
								pc.water = getIntegerToken(data, S_DELIM, i);// 0x00040000
								i++;
							}
							else if (mask == 0x00080000)
							{
								pc.fire = getIntegerToken(data, S_DELIM, i);// 0x00080000
								i++;
							}
							else if (mask == 0x00100000)
							{
								pc.wind = getIntegerToken(data, S_DELIM, i);// 0x00100000
								i++;
							}
							else if (mask == 0x00200000)
							{
								pc.gold = getIntegerToken(data, S_DELIM, i);// 0x00200000
								i++;
							}
							else if (mask == 0x00400000)
							{
								pc.titleNo = getIntegerToken(data, S_DELIM, i);// 0x00400000
								i++;
							}
							else if (mask == 0x00800000)
							{
								pc.dp = getIntegerToken(data, S_DELIM, i);// 0x00800000
								i++;
							}
							else if (mask == 0x01000000)
							{
								pc.transmigration = getIntegerToken(data, S_DELIM, i);// 0x01000000
								i++;
							}
							else if (mask == 0x02000000)
							{
								getStringToken(data, S_DELIM, i, sizeof(name) - 1, name);// 0x01000000
								makeStringFromEscaped(name);
								if (strlen(name) <= CHAR_NAME_LEN)
									strcpy(pc.name, name);
								i++;
							}
							else if (mask == 0x04000000)
							{
								getStringToken(data, S_DELIM, i, sizeof(freeName) - 1, freeName);// 0x02000000
								makeStringFromEscaped(freeName);
								if (strlen(freeName) <= CHAR_FREENAME_LEN)
									strcpy(pc.freeName, freeName);
								i++;
							}
							else if (mask == 0x08000000) // ( 1 << 27 )
							{
								pc.ridePetNo = getIntegerToken(data, S_DELIM, i);// 0x08000000
								i++;
							}
							else if (mask == 0x10000000) // ( 1 << 28 )
							{
								pc.learnride = getIntegerToken(data, S_DELIM, i);// 0x10000000
								i++;
							}
							else if (mask == 0x20000000) // ( 1 << 29 )
							{
								pc.baseGraNo = getIntegerToken(data, S_DELIM, i);// 0x20000000
								i++;
							}
							else if (mask == 0x40000000) // ( 1 << 30 )
							{
								pc.skywalker = getIntegerToken(data, S_DELIM, i);// 0x40000000
								i++;
							}
							//else if (mask == 0x80000000 ){ // ( 1 << 31 )
								//	pc.debugmode = getIntegerToken(data, S_DELIM, i);// 0x80000000
								//	i++;
							//}
						}
					}
				}
				updataPcAct();
				if ((pc.status & CHR_STATUS_LEADER) != 0 && party[0].useFlag != 0)
				{
					party[0].level = pc.level;
					party[0].maxHp = pc.maxHp;
					party[0].hp = pc.hp;
					strcpy(party[0].name, pc.name);
				}
			}
			if (!bNewServer)
				pc.ridePetNo = -1;
			if ((bNewServer & 0xf000000) == 0xf000000 && sPetStatFlag == 1)
				saveUserSetting();
			break;
		case 'F':
			char familyName[256];

			data++;
			getStringToken(data, S_DELIM, 1, sizeof(familyName) - 1, familyName);
			makeStringFromEscaped(familyName);
			if (strlen(familyName) <= CHAR_NAME_LEN)
				strcpy(pc.familyName, familyName);
			pc.familyleader = getIntegerToken(data, S_DELIM, 2);
			pc.channel = getIntegerToken(data, S_DELIM, 3);
			pc.familySprite = getIntegerToken(data, S_DELIM, 4);
			pc.big4fm =	getIntegerToken(data, S_DELIM, 5);
#ifdef _CHANNEL_MODIFY
			if (pc.familyleader == FMMEMBER_NONE)
			{
				pc.etcFlag &= ~PC_ETCFLAG_CHAT_FM;
				TalkMode = 0;
			}
#endif
			break;
		// HP,MP,EXP
		case 'M':
			data++;
			pc.hp  = getIntegerToken(data, '|', 1);
			pc.mp  = getIntegerToken(data, '|', 2);
			pc.exp = getIntegerToken(data, '|', 3);
			updataPcAct();
			if ((pc.status & CHR_STATUS_LEADER) != 0 && party[0].useFlag != 0)
				party[0].hp = pc.hp;
			break;
		case 'K':
			{
				char name[256], freeName[256];
				int no, kubun, i;
				unsigned int mask;

				no = data[1] - '0';
				data += 3;
				kubun = getInteger62Token(data, S_DELIM, 1);
				if (kubun == 0){
					if (pet[no].useFlag){
						if (no == pc.battlePetNo)
							pc.battlePetNo = -1;
						if (no == pc.mailPetNo)
							pc.mailPetNo = -1;
						pc.selectPetNo[no] = FALSE;
					}
					pet[no].useFlag = 0;
					break;
				}
				pet[no].useFlag = 1;
				if (kubun == 1){
					pet[no].graNo	= getIntegerToken(data, S_DELIM, 2);		// 0x00000002
					pet[no].hp		= getIntegerToken(data, S_DELIM, 3);		// 0x00000004
					pet[no].maxHp	= getIntegerToken(data, S_DELIM, 4);		// 0x00000008
					pet[no].mp		= getIntegerToken(data, S_DELIM, 5);		// 0x00000010
					pet[no].maxMp	= getIntegerToken(data, S_DELIM, 6);		// 0x00000020
					pet[no].exp		= getIntegerToken(data, S_DELIM, 7);		// 0x00000040
					pet[no].maxExp	= getIntegerToken(data, S_DELIM, 8);		// 0x00000080
					pet[no].level	= getIntegerToken(data, S_DELIM, 9);		// 0x00000100
					pet[no].atk		= getIntegerToken(data, S_DELIM, 10);		// 0x00000200
					pet[no].def		= getIntegerToken(data, S_DELIM, 11);		// 0x00000400
					pet[no].quick	= getIntegerToken(data, S_DELIM, 12);		// 0x00000800
					pet[no].ai		= getIntegerToken(data, S_DELIM, 13);		// 0x00001000
					pet[no].earth	= getIntegerToken(data, S_DELIM, 14);		// 0x00002000
					pet[no].water	= getIntegerToken(data, S_DELIM, 15);		// 0x00004000
					pet[no].fire	= getIntegerToken(data, S_DELIM, 16);		// 0x00008000
					pet[no].wind	= getIntegerToken(data, S_DELIM, 17);		// 0x00010000
					pet[no].maxSkill= getIntegerToken(data, S_DELIM, 18);		// 0x00020000
					pet[no].changeNameFlag = getIntegerToken(data, S_DELIM, 19);// 0x00040000
					pet[no].trn = getIntegerToken(data , S_DELIM, 20);
#ifdef _SHOW_FUSION
					pet[no].fusion = getIntegerToken(data , S_DELIM, 21);
					getStringToken(data, S_DELIM, 22, sizeof(name) - 1, name);// 0x00080000
					makeStringFromEscaped(name);
					if (strlen(name) <= PET_NAME_LEN)
						strcpy(pet[no].name, name);
					getStringToken(data, S_DELIM, 23, sizeof(freeName) - 1, freeName);// 0x00100000
					makeStringFromEscaped(freeName);
					if (strlen(freeName) <= PET_NAME_LEN)
						strcpy(pet[no].freeName, freeName);
#else
					getStringToken(data, S_DELIM, 21, sizeof(name) - 1, name);// 0x00080000
					makeStringFromEscaped(name);
					if (strlen(name) <= PET_NAME_LEN)
						strcpy(pet[no].name, name);
					getStringToken(data, S_DELIM, 22, sizeof(freeName) - 1, freeName);// 0x00100000
					makeStringFromEscaped(freeName);
					if (strlen(freeName) <= PET_NAME_LEN)
						strcpy(pet[no].freeName, freeName);
#endif
#ifdef _NB_宠物绑定值
					char 绑定标志[2];
					getStringToken(data, S_DELIM, 24, sizeof(绑定标志) - 1, 绑定标志);// 0x00100000
					makeStringFromEscaped(绑定标志);
					char *temp2 = sunday(绑定标志, "*");
					if (temp2){
						pet[no].petlock = 1;
					}else{
						pet[no].petlock = 0;
					}

#endif
					pet[no].petindex = getIntegerToken(data , S_DELIM, 25);//索引
					//printf("data[1]:%s\n",data);
#ifdef _NB_宠物初始值
					pet[no].initlevel = getIntegerToken(data , S_DELIM, 26);//
					pet[no].inithp = getIntegerToken(data , S_DELIM, 27);
					pet[no].initatk = getIntegerToken(data , S_DELIM, 28);
					pet[no].initdef = getIntegerToken(data , S_DELIM, 29);
					pet[no].initquick = getIntegerToken(data , S_DELIM, 30);
					pet[no].repetnum = getIntegerToken(data , S_DELIM, 39);
					//printf("data[%d]:%d %d %d %d\n",no,pet[no].inithp,pet[no].initatk,pet[no].initdef,pet[no].initquick);
#endif
					pet[no].hit_rate = getIntegerToken(data, S_DELIM, 40);
					pet[no].crit_rate = getIntegerToken(data, S_DELIM, 41);
					pet[no].avoid_rate = getIntegerToken(data, S_DELIM, 42);

					pet[no].DATAHP   = getIntegerToken(data, S_DELIM, 43);
					pet[no].DATASTR  = getIntegerToken(data, S_DELIM, 44);
					pet[no].DATATGH  = getIntegerToken(data, S_DELIM, 45);
					pet[no].DATADEX  = getIntegerToken(data, S_DELIM, 46);
					pet[no].SDATAVIT = getIntegerToken(data, S_DELIM, 47);
					pet[no].SDATASTR = getIntegerToken(data, S_DELIM, 48);
					pet[no].SDATATGH = getIntegerToken(data, S_DELIM, 49);
					pet[no].SDATADEX = getIntegerToken(data, S_DELIM, 50);
					pet[no].SDATAALL = getIntegerToken(data, S_DELIM, 51);
					pet[no].DATARARE = getIntegerToken(data, S_DELIM, 52);
					pet[no].DATARANK = getIntegerToken(data, S_DELIM, 53);

					pet[no].reset = getIntegerToken(data, S_DELIM, 54);

					printf("data[1]:%s\n",data);
				}else{
					mask = 2;
					i = 2;
					for (; mask > 0; mask <<= 1){
						if (kubun & mask){
							if (mask == 0x00000002){
								pet[no].graNo = getIntegerToken(data, S_DELIM, i);// 0x00000002
								i++;
							}else if (mask == 0x00000004){
								pet[no].hp = getIntegerToken(data, S_DELIM, i);// 0x00000004
								i++;
							}else if (mask == 0x00000008){
								pet[no].maxHp = getIntegerToken(data, S_DELIM, i);// 0x00000008
								i++;
							}else if (mask == 0x00000010){
								pet[no].mp = getIntegerToken(data, S_DELIM, i);// 0x00000010
								i++;
							}else if (mask == 0x00000020){
								pet[no].maxMp = getIntegerToken(data, S_DELIM, i);// 0x00000020
								i++;
							}else if (mask == 0x00000040){
								pet[no].exp = getIntegerToken(data, S_DELIM, i);// 0x00000040
								i++;
							}else if (mask == 0x00000080){
								pet[no].maxExp = getIntegerToken(data, S_DELIM, i);// 0x00000080
								i++;
							}else if (mask == 0x00000100){
								pet[no].level = getIntegerToken(data, S_DELIM, i);// 0x00000100
								i++;
							}else if (mask == 0x00000200){
								pet[no].atk = getIntegerToken(data, S_DELIM, i);// 0x00000200
								i++;
							}else if (mask == 0x00000400){
								pet[no].def = getIntegerToken(data, S_DELIM, i);// 0x00000400
								i++;
							}else if (mask == 0x00000800){
								pet[no].quick = getIntegerToken(data, S_DELIM, i);// 0x00000800
								i++;
							}else if (mask == 0x00001000 ){
								pet[no].ai = getIntegerToken(data, S_DELIM, i);// 0x00001000
								i++;
							}else if (mask == 0x00002000){
								pet[no].earth = getIntegerToken(data, S_DELIM, i);// 0x00002000
								i++;
							}else if (mask == 0x00004000 ){
								pet[no].water = getIntegerToken(data, S_DELIM, i);// 0x00004000
								i++;
							}else if (mask == 0x00008000){
								pet[no].fire = getIntegerToken(data, S_DELIM, i);// 0x00008000
								i++;
							}else if (mask == 0x00010000){
								pet[no].wind = getIntegerToken(data, S_DELIM, i);// 0x00010000
								i++;
							}else if (mask == 0x00020000){
								pet[no].maxSkill = getIntegerToken(data, S_DELIM, i);// 0x00020000
								i++;
							}else if (mask == 0x00040000){
								pet[no].changeNameFlag = getIntegerToken(data, S_DELIM, i);// 0x00040000
								i++;
							}else if (mask == 0x00080000){
								getStringToken(data, S_DELIM, i, sizeof(name) - 1, name);// 0x00080000
								makeStringFromEscaped(name);
								if (strlen(name) <= PET_NAME_LEN)
									strcpy(pet[no].name, name);
								i++;
							}else if (mask == 0x00100000){
								getStringToken(data, S_DELIM, i, sizeof(freeName) - 1, freeName);// 0x00100000
								makeStringFromEscaped(freeName);
								if (strlen(freeName) <= PET_NAME_LEN)
									strcpy(pet[no].freeName, freeName);
								i++;
							}
							//printf("petdata[2]:%s\n",data);
						}
					}
				}

			}
			break; 
		case 'E':
			{
				data++;
				minEncountPercentage = getIntegerToken(data, S_DELIM, 1);
				maxEncountPercentage = getIntegerToken(data, S_DELIM, 2);
				nowEncountPercentage = minEncountPercentage;
			}
			break;
		case 'J':
			{
				char name[256], memo[256];
				int no;

				no = data[1] - '0';
				data += 3;
				magic[no].useFlag = getIntegerToken(data, S_DELIM, 1);
				if (magic[no].useFlag == 0)
					break;
				magic[no].mp		= getIntegerToken(data, S_DELIM, 2);
				magic[no].field		= getIntegerToken(data, S_DELIM, 3);
				magic[no].target	= getIntegerToken(data, S_DELIM, 4);
				if (magic[no].target >= 100)
				{
					magic[no].target %= 100;
					magic[no].deadTargetFlag = 1;
				}
				else
					magic[no].deadTargetFlag = 0;
				getStringToken(data, S_DELIM, 5, sizeof(name) - 1, name);
				makeStringFromEscaped(name);
				if (strlen(name) <= sizeof(magic[no].name) - 1)
					strcpy(magic[no].name, name);
				getStringToken(data, S_DELIM, 6, sizeof(memo) - 1, memo);
				makeStringFromEscaped(memo);
				if (strlen(memo) <= sizeof(magic[no].memo) - 1)
					strcpy(magic[no].memo, memo);
			}
			break;
		case 'N':
			{
				ACTION *ptAct;
				char name[256];
				int no, kubun, i, checkPartyCount, gx, gy, no2;
				unsigned int mask;
				
				no = data[1] - '0';
				data += 3;
				kubun = getInteger62Token(data, S_DELIM, 1);
				if (kubun == 0)
				{
					if (party[no].useFlag != 0 && party[no].id != pc.id)
					{
						ptAct = getCharObjAct(party[no].id);
						if (ptAct != NULL)
							delCharParty(ptAct);
					}
					gx = -1;
					gy = -1;
					if (party[no].ptAct != NULL)
					{
						gx = party[no].ptAct->nextGx;
						gy = party[no].ptAct->nextGy;
					}
					party[no].useFlag = 0;
					party[no].ptAct	= NULL;
					checkPartyCount = 0;
					no2 = -1;
					for (i = 0; i < MAX_PARTY; i++)
					{
						if (party[i].useFlag != 0)
						{
							checkPartyCount++;
							if (no2 == -1 && i > no)
								no2 = i;
						}
					}
					if (checkPartyCount <= 1)
					{
						partyModeFlag = 0;
						clearPartyParam();
#ifdef _CHANNEL_MODIFY
						pc.etcFlag &= ~PC_ETCFLAG_CHAT_MODE;
						if (TalkMode == 2)
							TalkMode = 0;
#endif
					}
					else
					{
						if (no2 >= 0 || gx >= 0 || gy >= 0)
							goFrontPartyCharacter(no2, gx, gy);
					}
					break;
				}
				partyModeFlag = 1;
				prSendFlag = 0;
				party[no].useFlag = 1;
				
				if (kubun == 1)
				{
					party[no].id		= getIntegerToken(data, S_DELIM, 2);	// 0x00000002
					party[no].level		= getIntegerToken(data, S_DELIM, 3);	// 0x00000004
					party[no].maxHp		= getIntegerToken(data, S_DELIM, 4);	// 0x00000008
					party[no].hp		= getIntegerToken(data, S_DELIM, 5);	// 0x00000010
					party[no].mp		= getIntegerToken(data, S_DELIM, 6);	// 0x00000020
					getStringToken(data, S_DELIM, 7, sizeof(name) - 1, name);	// 0x00000040
					makeStringFromEscaped(name);
					if (strlen(name) <= sizeof(party[no].name) - 1)
						strcpy(party[no].name, name);
					else
						strcpy(party[no].name, "???");
				}
				else
				{
					mask = 2;
					i = 2;
					for (; mask > 0; mask <<= 1)
					{
						if (kubun & mask)
						{
							if (mask == 0x00000002)
							{
								party[no].id = getIntegerToken(data, S_DELIM, i);// 0x00000002
								i++;
							}
							else if (mask == 0x00000004)
							{
								party[no].level = getIntegerToken(data, S_DELIM, i);// 0x00000004
								i++;
							}
							else if (mask == 0x00000008)
							{
								party[no].maxHp = getIntegerToken(data, S_DELIM, i);// 0x00000008
								i++;
							}
							else if (mask == 0x00000010)
							{
								party[no].hp = getIntegerToken(data, S_DELIM, i);// 0x00000010
								i++;
							}
							else if (mask == 0x00000020)
							{
								party[no].mp = getIntegerToken(data, S_DELIM, i);// 0x00000020
								i++;
							}
							else if (mask == 0x00000040)
							{
								getStringToken(data, S_DELIM, i, sizeof(name) - 1, name);// 0x00000040
								makeStringFromEscaped(name);
								if (strlen(name) <= sizeof(party[no].name) - 1)
									strcpy(party[no].name, name);
								else
									strcpy(party[no].name, "???");
								i++;
							}
						}
					}
				}
				if (party[no].id != pc.id)
				{
					ptAct = getCharObjAct(party[no].id);
					if (ptAct != NULL)
					{
						party[no].ptAct = ptAct;
						setCharParty(ptAct);
						// NPC???????
						if (no == 0)
							setCharLeader(ptAct);
					}
					else
						party[no].ptAct = NULL;
				}
				else
				{
					party[no].ptAct = pc.ptAct;
					setPcParty();
					// PC???????
					if (no == 0)
						setPcLeader();
				}
			}
			break;
		case 'I':
			{
				int i, no;
				char temp[256];
				
				data++;
				for (i = 0; i < MAX_ITEM; i++)
				{
#ifdef _PET_ITEM

					//CC Fix
					extern int g_服务器;
					if (g_服务器 == SERVER_台服)
					{
						no = i * 16;
					}
					else
					{
						no = i * 14;
					}
					
					//old
					//no = i * 13;
#else
#ifdef _ITEM_PILENUMS
#ifdef _ALCHEMIST //#ifdef _ITEMSET7_TXT
					no = i * 12;
#else

					no = i * 11;

#endif//_ALCHEMIST
#else

					no = i * 10;

#endif//_ITEM_PILENUMS
#endif//_PET_ITEM
					getStringToken(data, '|', no + 1, sizeof(temp) - 1 , temp);
					makeStringFromEscaped(temp);
					if (strlen(temp) == 0 )
					{
						pc.item[i].useFlag = 0;
						continue;
					}
					pc.item[i].useFlag = 1;
					if (strlen(temp) <= ITEM_NAME_LEN)
						strcpy(pc.item[i].name, temp);
					getStringToken(data, '|', no + 2, sizeof(temp) - 1, temp);
					makeStringFromEscaped(temp);
					if (strlen(temp) <= ITEM_NAME2_LEN)
						strcpy(pc.item[i].name2, temp);
					pc.item[i].color = getIntegerToken(data, '|', no + 3);
					if (pc.item[i].color < 0)
						pc.item[i].color = 0;
					getStringToken(data, '|', no + 4, sizeof(temp) - 1, temp);
					makeStringFromEscaped(temp);
					if (strlen(temp) <= ITEM_MEMO_LEN)
						strcpy(pc.item[i].memo, temp);
					pc.item[i].graNo = getIntegerToken(data, '|', no + 5);
					pc.item[i].field = getIntegerToken(data, '|', no + 6);
					pc.item[i].target = getIntegerToken(data, '|', no + 7);
					if (pc.item[i].target >= 100)
					{
						pc.item[i].target %= 100;
						pc.item[i].deadTargetFlag = 1;
					}
					else
						pc.item[i].deadTargetFlag = 0;
					pc.item[i].level = getIntegerToken(data, '|', no + 8);
					pc.item[i].sendFlag = getIntegerToken(data, '|', no + 9);

					// 显示物品耐久度
					getStringToken(data, '|', no + 10, sizeof(temp) - 1, temp);
					makeStringFromEscaped(temp);
					if (strlen(temp) <= 16)
						strcpy(pc.item[i].damage, temp);
#ifdef _ITEM_PILENUMS
					getStringToken(data, '|', no + 11, sizeof(temp) - 1, temp);
					makeStringFromEscaped(temp);
					pc.item[i].pile = atoi(temp);
#endif
#ifdef _ALCHEMIST //_ITEMSET7_TXT
					getStringToken(data, '|', no + 12, sizeof(temp) - 1, temp);
					makeStringFromEscaped(temp);
					strcpy(pc.item[i].alch, temp);
#endif
#ifdef _PET_ITEM
					pc.item[i].type = getIntegerToken(data, '|', no + 13);
#endif

				}
			}
			break;
		//接收到的宠物技能
		case 'W':
			{
				int i, no, no2;
				char temp[256];

				no = data[1] - '0';
				data += 3;
				for (i = 0; i < MAX_SKILL; i++)
					petSkill[no][i].useFlag = 0;
				for (i = 0; i < MAX_SKILL; i++)
				{
					no2 = i * 5;
					getStringToken(data, '|', no2 + 4, sizeof(temp) - 1, temp);
					makeStringFromEscaped(temp);
					if (strlen(temp) == 0)
						continue;
					petSkill[no][i].useFlag = 1;
					if (strlen(temp) <= SKILL_NAME_LEN)
						strcpy(petSkill[no][i].name, temp);
					else
						strcpy(petSkill[no][i].name, "??? name ???");
					petSkill[no][i].skillId = getIntegerToken(data, '|', no2 + 1);
					petSkill[no][i].field = getIntegerToken(data, '|', no2 + 2);
					petSkill[no][i].target = getIntegerToken(data, '|', no2 + 3);
					getStringToken(data, '|', no2 + 5, sizeof(temp) - 1, temp);
					makeStringFromEscaped(temp);
					if (strlen(temp) <= SKILL_MEMO_LEN)
						strcpy(petSkill[no][i].memo, temp);
					else
						strcpy(petSkill[no][i].memo, "??? memo ???");
				}
			}
		break;
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
		case 'S':
			{
				char name[CHAR_NAME_LEN + 1];
				char memo[PROFESSION_MEMO_LEN + 1];
				int i, count = 0;
				
				data++;
				for (i = 0; i < MAX_PROFESSION_SKILL; i++)
				{
					profession_skill[i].useFlag = 0;
					profession_skill[i].kind = 0;
				}
				for (i = 0; i < MAX_PROFESSION_SKILL; i++)
				{
					count = i * 9;
					profession_skill[i].useFlag  =  getIntegerToken(data, S_DELIM, 1 + count);
					profession_skill[i].skillId  =  getIntegerToken(data, S_DELIM, 2 + count);
					profession_skill[i].target   =  getIntegerToken(data, S_DELIM, 3 + count);
					profession_skill[i].kind	 =  getIntegerToken(data, S_DELIM, 4 + count);
					profession_skill[i].icon	 =  getIntegerToken(data, S_DELIM, 5 + count);
					profession_skill[i].costmp	 =  getIntegerToken(data, S_DELIM, 6 + count);
					profession_skill[i].skill_level = getIntegerToken(data, S_DELIM, 7 + count);
					memset(name, 0, sizeof(name));
					getStringToken(data, S_DELIM, 8 + count, sizeof(name) - 1, name);
					makeStringFromEscaped(name);
					if (strlen(name) <= CHAR_NAME_LEN)
						strcpy(profession_skill[i].name, name);
					memset(memo, 0, sizeof(memo));
					getStringToken(data, S_DELIM, 9 + count, sizeof(memo) - 1, memo);
					makeStringFromEscaped(memo);
					if (strlen(memo) <= PROFESSION_MEMO_LEN)
						strcpy(profession_skill[i].memo, memo);
				}
#ifdef _SKILLSORT
				SortSkill();
#endif
			}
		break;
#endif
#ifdef _PET_ITEM
		case 'B':
			{
				int i, no, nPetIndex;
				char szData[256];

				nPetIndex = data[1] - '0';
				data += 2;
				for (i = 0; i < MAX_PET_ITEM; i++)
				{
				//	no = i * 13;
					no = i * 14;	//修复不显示宠物胸甲宠装
					getStringToken(data, '|', no + 1, sizeof(szData) - 1, szData);
					makeStringFromEscaped(szData);
					if (strlen(szData) == 0)	// 没道具
					{
						memset(&pet[nPetIndex].item[i], 0, sizeof(pet[nPetIndex].item[i]));
						continue;
					}
					pet[nPetIndex].item[i].useFlag = 1;
					if (strlen(szData) <= ITEM_NAME_LEN)
						strcpy(pet[nPetIndex].item[i].name, szData);
					getStringToken(data, '|', no + 2, sizeof(szData) - 1, szData);
					makeStringFromEscaped(szData);
					if (strlen(szData) <= ITEM_NAME2_LEN)
						strcpy(pet[nPetIndex].item[i].name2, szData);
					pet[nPetIndex].item[i].color		= getIntegerToken(data, '|', no + 3);
					if (pet[nPetIndex].item[i].color < 0)
						pet[nPetIndex].item[i].color			= 0;
					getStringToken(data, '|', no + 4, sizeof(szData) - 1, szData);
					makeStringFromEscaped(szData);
					if (strlen(szData) <= ITEM_MEMO_LEN)
						strcpy(pet[nPetIndex].item[i].memo, szData);
					pet[nPetIndex].item[i].graNo				= getIntegerToken(data, '|', no + 5);
					pet[nPetIndex].item[i].field				= getIntegerToken(data, '|', no + 6);
					pet[nPetIndex].item[i].target				= getIntegerToken(data, '|', no + 7);
					if (pet[nPetIndex].item[i].target >= 100)
					{
						pet[nPetIndex].item[i].target			%= 100;
						pet[nPetIndex].item[i].deadTargetFlag	= 1;
					}
					else
						pet[nPetIndex].item[i].deadTargetFlag	= 0;
					pet[nPetIndex].item[i].level				= getIntegerToken(data, '|', no + 8);
					pet[nPetIndex].item[i].sendFlag				= getIntegerToken(data, '|', no + 9);
					
					// 显示物品耐久度
					getStringToken(data, '|', no + 10, sizeof(szData) - 1, szData);
					makeStringFromEscaped(szData);
					if (strlen(szData) <= 16)
						strcpy(pet[nPetIndex].item[i].damage, szData);
					pet[nPetIndex].item[i].pile					= getIntegerToken(data, '|', no + 11);
	#ifdef _ALCHEMIST //_ITEMSET7_TXT
					getStringToken(data, '|', no + 12, sizeof(szData) - 1, szData);
					makeStringFromEscaped(szData);
					strcpy(pet[nPetIndex].item[i].alch, szData);
	#endif
					pet[nPetIndex].item[i].type					= getIntegerToken(data, '|', no + 13);
					//可拿给宠物装备的道具,就不会是拼图了,以下就免了
					//if( i == JigsawIdx )
					//	SetJigsaw( pc.item[i].graNo, pc.item[i].jigsaw );
				}
			}
			break;
#endif
			case 'V':
			{
				extern int itemWndMaxBag;
				//V|1
				itemWndMaxBag = getIntegerToken(data, '|', 2);
			}
			break;
    }
} 

void lssproto_MC_recv( int fd, int fl, int x1, int y1, int x2, int y2, int tileSum, int partsSum, int eventSum, char *data)
{
	char showString[512], floorName[32];

	if (logOutFlag)
		return;

#ifdef DEBUGPUSH
    char msg[800];
    sprintf(msg, "佋???????????????FL%d %d,%d-%d,%d (%ud/%ud)", fl, x1, y1, x2, y2, tileSum, partsSum);
    PUSH(msg);
#endif
	getStringToken(data, '|', 1, sizeof(showString) - 1, showString);
	makeStringFromEscaped(showString);
	if (nowFloor == fl)
	{
		char strPal[32];

		getStringToken(showString, '|', 1, sizeof(floorName) - 1, floorName);
		if (strlen(floorName) <= FLOOR_NAME_LEN)
			strcpy(nowFloorName, floorName);
		else
			strcpy(nowFloorName, "???");
		palNo = -2;
		getStringToken(showString, '|', 2, sizeof(strPal) - 1, strPal);
		if (strlen(strPal) == 0)
		{
			if (TimeZonePalChangeFlag == FALSE || loginFlag)
			{
				palNo = -1;
				palTime = 0;
				drawTimeAnimeFlag = 1;
			}
		}
		else
		{
			int pal;

			pal = atoi(strPal);
			if (pal >= 0)
			{
				if (TimeZonePalChangeFlag == TRUE || loginFlag)
				{
					palNo = pal;
					palTime = 0;
					drawTimeAnimeFlag = 0;
				}
			}
			else
			{
				if (TimeZonePalChangeFlag == FALSE || loginFlag)
				{
					palNo = -1;
					palTime = 0;
					drawTimeAnimeFlag = 1;
				}
			}
		}
	}

	//JL fix 2017.9.25 组队情况下，队员不检查地图，防止很卡
	for (int i = 1; i < MAX_PARTY; i++) 
	{
		if (party[i].useFlag != 0)
		{
			if (party[i].id == pc.id)
			{
				if (nowFloor == fl)
				{
					floorChangeFlag = FALSE;
					if (warpEffectStart)
						warpEffectOk = TRUE;
				}
				loginFlag = FALSE;
				return;
			}
		}
	}



	if (mapCheckSum(fl, x1, y1, x2, y2, tileSum, partsSum, eventSum))
	{
		if (nowFloor == fl)
		{
			floorChangeFlag = FALSE;
			if (warpEffectStart)
				warpEffectOk = TRUE;
		}
		loginFlag = FALSE;
	}
}

void lssproto_M_recv(int fd, int fl, int x1, int y1, int x2, int y2, char* data)
{
	char showString[512], floorName[32], tilestring[18192], partsstring[18192], eventstring[18192], tmp[100];
	unsigned short tile[2048] , parts[2048], event[2048];
	int i, flag;

	if (logOutFlag)
		return;

#ifdef DEBUGPUSH
    char msg[800];
    sprintf(msg , "??????佋????????FL%d %d,%d-%d,%d", fl, x1, y1, x2, y2);
    PUSH(msg);
#endif
	getStringToken(data, '|', 1, sizeof(showString) - 1, showString);
	makeStringFromEscaped(showString);
	if (nowFloor == fl)
	{
		char strPal[32];

		getStringToken(showString, '|', 1, sizeof(floorName) - 1, floorName);
		if (strlen(floorName) <= FLOOR_NAME_LEN)
			strcpy(nowFloorName, floorName);
		else
			strcpy(nowFloorName, "???");
		palNo = -2;
		getStringToken(showString, '|', 2, sizeof(strPal) - 1, strPal);
		if (strlen(strPal) == 0)
		{
			if (TimeZonePalChangeFlag == FALSE || loginFlag)
			{
				palNo = -1;
				palTime = 0;
				drawTimeAnimeFlag = 1;
			}
		}
		else
		{
			int pal;

			pal = atoi(strPal);
			if (pal >= 0)
			{
				if (TimeZonePalChangeFlag == TRUE || loginFlag)
				{
					palNo = pal;
					palTime = 0;
					drawTimeAnimeFlag = 0;
				}
			}
			else
			{
				if (TimeZonePalChangeFlag == FALSE || loginFlag)
				{
					palNo = -1;
					palTime = 0;
					drawTimeAnimeFlag = 1;
				}
			}
		}
	}


	//JL fix 2017.9.25  修复组队跑的时候客户端卡的问题，因为会频繁收到此包，如果是队员则把后面的地图操作??掉
	for (int i = 1; i < MAX_PARTY; i++)  //检测是否为队员
	{
		if (party[i].useFlag != 0)
		{
			if (party[i].id == pc.id)
			{
				if (mapEmptyFlag || floorChangeFlag)
				{
					if (nowFloor == fl)
					{
						redrawMap();
						floorChangeFlag = FALSE;
						if (warpEffectStart)
							warpEffectOk = TRUE;
					}
				}
				loginFlag = FALSE;
				return;
			}
		}
	}

	getStringToken(data, '|', 2, sizeof(tilestring) - 1, tilestring);	
	getStringToken(data, '|', 3, sizeof(partsstring) - 1, partsstring);
	getStringToken(data, '|', 4, sizeof(eventstring) - 1, eventstring);
	for (i = 0; ; i++)
	{
		flag = getStringToken(tilestring, ',', i + 1, sizeof(tmp) - 1, tmp);
		tile[i] = a62toi(tmp);
		getStringToken(partsstring, ',', i + 1, sizeof(tmp) - 1, tmp);
		parts[i] = a62toi(tmp);
		getStringToken(eventstring, ',', i + 1, sizeof(tmp) - 1, tmp);
		event[i] = a62toi(tmp);
		if (flag == 1)
			break;
	}
	writeMap(fl, x1, y1, x2, y2, tile, parts, event);
	if (mapEmptyFlag || floorChangeFlag)
	{
		if (nowFloor == fl)
		{
			redrawMap();
			floorChangeFlag = FALSE;
			if (warpEffectStart)
				warpEffectOk = TRUE;
		}
	}
	loginFlag = FALSE;
}

/*===========================
1 OBJTYPE_CHARA
2 OBJTYPE_ITEM
3 OBJTYPE_GOLD
4 NPC&other player
===========================*/
void lssproto_C_recv(int fd, char *data)
{
	int i, j, id, x, y, dir, graNo, level, nameColor, walkable, height, classNo, money, charType, charNameColor;
	char bigtoken[2048], smalltoken[2048], name[2048], freeName[2048], info[1024], fmname[2048], petname[1024];
	int petlevel;
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	int profession_class, profession_level, profession_skill_point;
#endif
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	int herofloor;
#endif
#ifdef _NPC_PICTURE
	char picture;
#endif
#ifdef _NB_文字称号
	char nb_name[64];
	int nb_namecolor;
#endif
#ifdef _NB_任务指引
	int noticeNo;
#endif
	ACTION *ptAct;

	if (logOutFlag)
		return;
	if (encountNowFlag)
		return;
	for (i = 0; ; i++)
	{
		getStringToken(data, ',', i + 1, sizeof(bigtoken) - 1, bigtoken);
		if (strlen(bigtoken) == 0)
			break;
#ifdef _OBJSEND_C
		getStringToken(bigtoken, '|', 1, sizeof(smalltoken) - 1, smalltoken);
		if (strlen(smalltoken) == 0)
			return;
		switch (atoi(smalltoken))
		{
		case 1://OBJTYPE_CHARA
			//printf("bigtoken %s\n",bigtoken);
			charType = getIntegerToken(bigtoken, '|', 2);
			getStringToken(bigtoken, '|', 3, sizeof(smalltoken) - 1, smalltoken);
			id = a62toi(smalltoken);
			getStringToken(bigtoken, '|', 4, sizeof(smalltoken) - 1, smalltoken);
			x = atoi(smalltoken);
			getStringToken(bigtoken, '|', 5, sizeof(smalltoken) - 1, smalltoken);
			y = atoi(smalltoken);
			getStringToken(bigtoken, '|', 6, sizeof(smalltoken) - 1, smalltoken);
			dir = (atoi(smalltoken) + 3) % 8;
			getStringToken(bigtoken, '|', 7, sizeof(smalltoken) - 1, smalltoken);
			graNo = atoi(smalltoken);
			getStringToken(bigtoken, '|', 8, sizeof(smalltoken) - 1,smalltoken);
			level = atoi(smalltoken);
			nameColor = getIntegerToken(bigtoken, '|', 9);
			getStringToken(bigtoken, '|', 10, sizeof(name) - 1, name);
			makeStringFromEscaped(name);
			getStringToken(bigtoken, '|', 11, sizeof(freeName) - 1, freeName);
			makeStringFromEscaped(freeName);
			getStringToken(bigtoken, '|', 12, sizeof(smalltoken) - 1, smalltoken);
			walkable = atoi(smalltoken);
			getStringToken(bigtoken, '|', 13, sizeof(smalltoken) - 1, smalltoken);
			height = atoi(smalltoken);
			charNameColor = getIntegerToken(bigtoken, '|', 14);
			getStringToken(bigtoken , '|', 15, sizeof(fmname) - 1, fmname);
			makeStringFromEscaped(fmname);
			getStringToken(bigtoken , '|', 16, sizeof(petname) - 1, petname);
			makeStringFromEscaped(petname);
			getStringToken(bigtoken, '|', 17, sizeof(smalltoken) - 1, smalltoken);
			petlevel = atoi(smalltoken);

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
			getStringToken(bigtoken, '|', 18, sizeof(smalltoken) - 1, smalltoken);
			profession_class = atoi(smalltoken);
			getStringToken(bigtoken, '|', 19, sizeof(smalltoken) - 1, smalltoken);
			profession_level = atoi(smalltoken);
//			getStringToken(bigtoken, '|', 20, sizeof(smalltoken) - 1, smalltoken);
//			profession_exp = atoi(smalltoken);
			getStringToken(bigtoken, '|', 20, sizeof(smalltoken) - 1, smalltoken);
			profession_skill_point = atoi(smalltoken);
#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
			getStringToken(bigtoken, '|', 21, sizeof(smalltoken) - 1, smalltoken);
			herofloor = atoi(smalltoken);
#endif
#ifdef _NPC_PICTURE
			getStringToken(bigtoken, '|', 22, sizeof(smalltoken) - 1, smalltoken);
			picture = atoi(smalltoken);
#endif
#ifdef _GM_IDENTIFY		// Rog ADD GM识别
			getStringToken(bigtoken , '|', 23 , sizeof( gm_name ) - 1, gm_name );
			makeStringFromEscaped( gm_name );
#endif
#endif
#ifdef _NB_任务指引
			getStringToken(bigtoken, '|', 23, sizeof(smalltoken) - 1, smalltoken);
			noticeNo = atoi(smalltoken);
			//printf("name %s noticeNo %d\n",name,noticeNo);
#endif
#ifdef _NB_文字称号
			getStringToken(bigtoken , '|', 24 , sizeof( nb_name ) - 1, nb_name);
			makeStringFromEscaped(nb_name);
			getStringToken(bigtoken, '|', 25, sizeof(smalltoken) - 1, smalltoken);
			nb_namecolor = atoi(smalltoken);
#endif

			if (charNameColor < 0)
				charNameColor = 0;
			if (pc.id == id){
				if (pc.ptAct == NULL){
					createPc(graNo, x, y, dir);
					updataPcAct();
				}
				else
					setPcGraNo(graNo, pc.dir);
				updateMapArea();

#ifdef _NB_文字称号
				setPcParam(name, freeName, level, petname, petlevel, nameColor, walkable, height, profession_class, profession_level, profession_skill_point,herofloor,nb_name,nb_namecolor);
#else
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
//    #ifdef _GM_IDENTIFY		// Rog ADD GM识别
//				setPcParam(name, freeName, level, petname, petlevel, nameColor, walkable, height, profession_class, profession_level, profession_exp, profession_skill_point , gm_name);
//				setPcParam(name, freeName, level, petname, petlevel, nameColor, walkable, height, profession_class, profession_level, profession_skill_point , gm_name);
//    #else
//				setPcParam(name, freeName, level, petname, petlevel, nameColor, walkable, height, profession_class, profession_level, profession_exp, profession_skill_point);
#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
				setPcParam(name, freeName, level, petname, petlevel, nameColor, walkable, height, profession_class, profession_level, profession_skill_point , herofloor);
#else
				setPcParam(name, freeName, level, petname, petlevel, nameColor, walkable, height, profession_class, profession_level, profession_skill_point);
#endif
//    #endif
#else
				setPcParam(name, freeName, level, petname, petlevel, nameColor, walkable, height);
#endif

#endif
				//JL 设置id
				pc.ptAct->objIndex = id;

				setPcNameColor(charNameColor);
				if ((pc.status & CHR_STATUS_LEADER) != 0 && party[0].useFlag != 0)
				{
					party[0].level = pc.level;
					strcpy(party[0].name, pc.name);
				}
				for (j = 0; j < MAX_PARTY; j++)
				{
					if (party[j].useFlag != 0 && party[j].id == id)
					{
						party[j].ptAct = pc.ptAct;
						setPcParty();
						if (j == 0)
							setPcLeader();
						break;
					}
				}
			}else{
				setNpcCharObj(id, graNo, x, y, dir, fmname, name, freeName,
					level, petname, petlevel, nameColor, walkable, height, charType, profession_class
#ifdef _NPC_PICTURE
					, picture
#endif
#ifdef _NB_文字称号
					,nb_name,nb_namecolor
#endif
#ifdef _NB_任务指引
					,noticeNo
#endif
					);
				ptAct = getCharObjAct(id);
#ifdef _NB_任务指引
				if(charType==13 && noticeNo>0){
					setNpcNotice( ptAct, noticeNo);
				}
#endif
				if (ptAct != NULL)
				{
					//JL 设置id
					ptAct->objIndex = id;
					for (j = 0; j < MAX_PARTY; j++)
					{
						if (party[j].useFlag != 0 && party[j].id == id)
						{
							party[j].ptAct = ptAct;
							setCharParty(ptAct);
							if (j == 0)
								setCharLeader(ptAct);
							break;
						}
					}
					setCharNameColor(ptAct, charNameColor);
				}
			}
			break;
		case 2://OBJTYPE_ITEM
				getStringToken(bigtoken, '|', 2, sizeof(smalltoken) - 1, smalltoken);
				id = a62toi(smalltoken);
				getStringToken(bigtoken, '|', 3, sizeof(smalltoken) - 1, smalltoken);
				x = atoi(smalltoken);
				getStringToken(bigtoken, '|', 4, sizeof(smalltoken) - 1, smalltoken);
				y = atoi(smalltoken);
				getStringToken(bigtoken, '|', 5, sizeof(smalltoken) - 1, smalltoken);
				graNo = atoi(smalltoken);
				classNo = getIntegerToken(bigtoken, '|', 6);
				getStringToken(bigtoken, '|', 7, sizeof(info) - 1, info);
				makeStringFromEscaped(info);
				setItemCharObj(id, graNo, x, y, 0, classNo, info);
			break;
		case 3://OBJTYPE_GOLD
				getStringToken(bigtoken, '|', 2, sizeof(smalltoken) - 1, smalltoken);
				id = a62toi(smalltoken);
				getStringToken(bigtoken, '|', 3, sizeof(smalltoken) - 1, smalltoken);
				x = atoi(smalltoken);
				getStringToken(bigtoken, '|', 4, sizeof(smalltoken) - 1, smalltoken);
				y = atoi(smalltoken);
				getStringToken(bigtoken, '|', 5, sizeof(smalltoken) - 1, smalltoken);
				money = atoi(smalltoken);
				sprintf(info, "%d Stone", money);
				if (money > 10000)
					setMoneyCharObj(id, 24050, x, y, 0, money, info);
				else if (money > 1000)
					setMoneyCharObj(id, 24051, x, y, 0, money, info);
				else
					setMoneyCharObj(id, 24052, x, y, 0, money, info);
			break;
		case 4:
			getStringToken(bigtoken, '|', 2, sizeof(smalltoken) - 1, smalltoken);
			id = a62toi(smalltoken);
			getStringToken(bigtoken, '|', 3, sizeof(name) - 1, name);
			makeStringFromEscaped(name);
			getStringToken(bigtoken, '|', 4, sizeof(smalltoken) - 1, smalltoken);
			dir = (atoi(smalltoken) + 3) % 8;
			getStringToken(bigtoken, '|', 5, sizeof(smalltoken) - 1, smalltoken);
			graNo = atoi(smalltoken);
			getStringToken(bigtoken, '|', 6, sizeof(smalltoken) - 1, smalltoken);
			x = atoi(smalltoken);
			getStringToken(bigtoken, '|', 7, sizeof(smalltoken) - 1, smalltoken);
			y = atoi(smalltoken);
			setNpcCharObj( id, graNo, x, y, dir, "", name, "",
				level, petname, petlevel, nameColor, 0, height, 2, 0
#ifdef _NPC_PICTURE
			,0 
#endif
#ifdef _NB_文字称号
			,"",0
#endif
#ifdef _NB_任务指引
			,0
#endif
			);
			ptAct = getCharObjAct( id );
			break;
		}
#else
		getStringToken(bigtoken , '|' , 11 , sizeof(smalltoken) - 1, smalltoken);
		if ( strlen(smalltoken) > 0 ){
			// NPC?
			charType = getIntegerToken(bigtoken, '|', 1 );
			getStringToken(bigtoken, '|', 2, sizeof(smalltoken) - 1, smalltoken);
			id = a62toi(smalltoken);
			getStringToken(bigtoken, '|', 3, sizeof(smalltoken) - 1, smalltoken);
			x = atoi(smalltoken);
			getStringToken(bigtoken, '|', 4, sizeof(smalltoken) - 1, smalltoken);
			y = atoi(smalltoken);
			getStringToken(bigtoken, '|', 5, sizeof(smalltoken) - 1, smalltoken);
			dir = (atoi(smalltoken) + 3) % 8;
			getStringToken(bigtoken, '|', 6, sizeof(smalltoken) - 1, smalltoken);
			graNo = atoi(smalltoken);
			getStringToken(bigtoken, '|', 7, sizeof(smalltoken) - 1,smalltoken);
			level = atoi(smalltoken);
			nameColor = getIntegerToken(bigtoken, '|', 8 );
			getStringToken(bigtoken , '|' , 9 , sizeof(name) - 1, name );
			makeStringFromEscaped(name);
			getStringToken(bigtoken , '|' , 10 , sizeof( freeName ) - 1, freeName );
			makeStringFromEscaped( freeName );
			getStringToken(bigtoken, '|', 11, sizeof(smalltoken) - 1, smalltoken);
			walkable = atoi(smalltoken);
			getStringToken(bigtoken, '|', 12, sizeof(smalltoken) - 1, smalltoken);
			height = atoi(smalltoken);
			charNameColor = getIntegerToken(bigtoken, '|', 13 );
			getStringToken(bigtoken , '|' , 14, sizeof( fmname ) - 1, fmname );
			makeStringFromEscaped( fmname );
			getStringToken(bigtoken , '|' , 15 , sizeof( petname ) - 1, petname );
			makeStringFromEscaped( petname );
			getStringToken(bigtoken, '|', 16, sizeof(smalltoken) - 1, smalltoken);
			petlevel = atoi(smalltoken);
			if ( charNameColor < 0 )
				charNameColor = 0;

			if ( pc.id == id ){
				if ( pc.ptAct == NULL ){
					createPc( graNo, x, y, dir );
					updataPcAct();
				}else{
					setPcGraNo( graNo, pc.dir );
				}
				updateMapArea();
				setPcParam( name, freeName, level, petname, petlevel, nameColor, walkable, height );			
				setPcNameColor( charNameColor );
				if ( (pc.status & CHR_STATUS_LEADER) != 0
				 && party[0].useFlag != 0 ){
					party[0].level = pc.level;
					strcpy( party[0].name, pc.name );
				}
				for ( j = 0; j < MAX_PARTY; j++ ){
					if ( party[j].useFlag != 0 && party[j].id == id ){
						party[j].ptAct = pc.ptAct;
						setPcParty();
						if ( j == 0 ){
							setPcLeader();
						}
						break;
					}
				}
			}else{
				setNpcCharObj( id, graNo, x, y, dir, fmname, name, freeName,
					level, petname, petlevel, nameColor, walkable, height, charType
#ifdef _NPC_PICTURE
					, 0 
#endif
#ifdef _NB_文字称号
					,"",0
#endif
#ifdef _NB_任务指引	
					,0
#endif
					);
				ptAct = getCharObjAct( id );
				if ( ptAct != NULL ){
					for ( j = 0; j < MAX_PARTY; j++ ){
						if ( party[j].useFlag != 0 && party[j].id == id ){
							party[j].ptAct = ptAct;
							setCharParty( ptAct );
							if ( j == 0 ){
								setCharLeader( ptAct );
							}
							break;
						}
					}
					setCharNameColor( ptAct, charNameColor );
				}
			}
		}else{
			getStringToken(bigtoken, '|', 6, sizeof(smalltoken) - 1, smalltoken);
			if ( strlen(smalltoken) > 0 ){
				getStringToken(bigtoken, '|', 1, sizeof(smalltoken) - 1, smalltoken);
				id = a62toi(smalltoken);
				getStringToken(bigtoken, '|', 2, sizeof(smalltoken) - 1, smalltoken);
				x = atoi(smalltoken);
				getStringToken(bigtoken, '|', 3, sizeof(smalltoken) - 1, smalltoken);
				y = atoi(smalltoken);
				getStringToken(bigtoken, '|', 4, sizeof(smalltoken) - 1, smalltoken);
				graNo = atoi(smalltoken);
				classNo = getIntegerToken(bigtoken, '|', 5 );
				getStringToken(bigtoken, '|', 6, sizeof( info ) - 1, info );
				makeStringFromEscaped( info );
				setItemCharObj( id, graNo, x, y, 0, classNo, info );
			}else{
				getStringToken(bigtoken, '|', 4, sizeof(smalltoken) - 1, smalltoken);
				if ( strlen(smalltoken) > 0 ){
					getStringToken(bigtoken, '|', 1, sizeof(smalltoken) - 1, smalltoken);
					id = a62toi(smalltoken);
					getStringToken(bigtoken, '|', 2, sizeof(smalltoken) - 1, smalltoken);
					x = atoi(smalltoken);
					getStringToken(bigtoken, '|', 3, sizeof(smalltoken) - 1, smalltoken);
					y = atoi(smalltoken);
					getStringToken(bigtoken, '|', 4, sizeof(smalltoken) - 1, smalltoken);
					money = atoi(smalltoken);
					sprintf( info, "%d Stone", money );
					if ( money > 10000 ){
						setMoneyCharObj( id, 24050, x, y, 0, money, info );
					}else
					if ( money > 1000 ){
						setMoneyCharObj( id, 24051, x, y, 0, money, info );
					}else{
						setMoneyCharObj( id, 24052, x, y, 0, money, info );
					}
				}
			}
		}
#endif
	}

}


void lssproto_CA_recv( int fd, char *data ) 
{
	char bigtoken[2048];
	char smalltoken[2048];
	int alreadytellC[1024];
	int tellCindex = 0;
	int tellflag;
	int i, j;
	int charindex;
	int x;
	int y;
	int act;
	int dir;
	int effectno = 0, effectparam1 = 0, effectparam2 = 0;
#ifdef _STREET_VENDOR
	char szStreetVendorTitle[21];
#endif
	ACTION *ptAct;
	if ( logOutFlag )
		return;
	if ( encountNowFlag )
	{
		return;
	}

	for ( i = 0; ; i++ )
	{
		getStringToken(data, ',', i+1, sizeof(bigtoken ) - 1, bigtoken );
		if ( strlen(bigtoken ) == 0 )
			break;
		getStringToken(bigtoken, '|', 1, sizeof(smalltoken) - 1, smalltoken);
		charindex = a62toi(smalltoken);
		getStringToken(bigtoken, '|', 2, sizeof(smalltoken) - 1, smalltoken);
		x = atoi(smalltoken);
		getStringToken(bigtoken, '|', 3, sizeof(smalltoken) - 1, smalltoken);
		y = atoi(smalltoken);
		getStringToken(bigtoken, '|', 4, sizeof(smalltoken) - 1, smalltoken);
		act = atoi(smalltoken);
		getStringToken(bigtoken, '|', 5, sizeof(smalltoken) - 1, smalltoken);
		dir = (atoi(smalltoken)+3)%8;
		getStringToken(bigtoken, '|', 6, sizeof(smalltoken) - 1, smalltoken);
#ifdef _STREET_VENDOR
		if (act == 41) strncpy(szStreetVendorTitle,smalltoken,sizeof(szStreetVendorTitle));
		else
#endif
		{
		effectno = atoi(smalltoken);
		effectparam1 = getIntegerToken(bigtoken, '|', 7 );
		effectparam2 = getIntegerToken(bigtoken, '|', 8 );
		}


		if ( pc.id == charindex ){
			if ( pc.ptAct == NULL
			 || (pc.ptAct != NULL && pc.ptAct->anim_chr_no == 0) )
			{
// JL 2016.8.25 禁止查询
#if 0
				if (bNewServer)
					lssproto_C_send(sockfd, charindex );
				else
					old_lssproto_C_send(sockfd, charindex );
#endif
			}
			else
			{
#ifdef _STREET_VENDOR
				if (act == 41){
					if (pc.iOnStreetVendor == 1){
						memset(pc.ptAct->szStreetVendorTitle,0,sizeof(pc.ptAct->szStreetVendorTitle));
						strncpy(pc.ptAct->szStreetVendorTitle,szStreetVendorTitle,sizeof(szStreetVendorTitle));
						changePcAct( x, y, dir, act, effectno, effectparam1, effectparam2 );
#ifdef _STREET_VENDOR_CHANGE_ICON
						if (bNewServer)
							lssproto_AC_send(sockfd,nowGx,nowGy,5);
						else
							old_lssproto_AC_send(sockfd,nowGx,nowGy,5);
						setPcAction(5);
#endif
					}
				}
				else
#endif
				changePcAct( x, y, dir, act, effectno, effectparam1, effectparam2 );
			}
			continue;
		}

		ptAct = getCharObjAct( charindex );
		if ( ptAct == NULL ){
			tellflag = 0;
			for ( j = 0; j < tellCindex; j++ ){
				if ( alreadytellC[j] == charindex ){
					tellflag = 1;
					break;
				}
			}
			if ( tellflag == 0 && tellCindex < sizeof(alreadytellC) ){
				alreadytellC[tellCindex] = charindex;
				tellCindex++;
// JL 2016.8.25 禁止查询
#if 0
				if (bNewServer)
					lssproto_C_send(sockfd, charindex);
				else
					old_lssproto_C_send(sockfd, charindex);
#endif
			}
		}else{
			ptAct->objIndex = charindex;
#ifdef _STREET_VENDOR
			if (act == 41){
				memset(ptAct->szStreetVendorTitle,0,sizeof(ptAct->szStreetVendorTitle));
				strncpy(ptAct->szStreetVendorTitle,szStreetVendorTitle,sizeof(szStreetVendorTitle));
			}
#endif
			changeCharAct( ptAct, x, y, dir, act, effectno, effectparam1, effectparam2 );
		}
	}
}

void lssproto_CD_recv( int fd, char *data ){
	int i, j;
	int id;
	if ( logOutFlag )
		return;
	for ( i = 1; ; i++ ){
//		id = getInteger62Token(data, '|', i );
		id = getInteger62Token(data, ',', i );
		if ( id == -1 )
			break;

		delCharObj( id );

		for ( j = 0; j < MAX_PARTY; j++ ){
			if ( party[j].useFlag != 0 && party[j].id == id ){
				party[j].ptAct = NULL;
				break;
			}
		}
	}
}




// ? ///////////////////////////////////////////////////////////////////
void walkSendForServer( int x, int y, char *direction ){
#ifdef _MIND_ICON

	//JL fix 2017.8.13 切换地图的时候崩溃(可能进了战斗)，此时pc.ptAct = NULL
	if (pc.ptAct == NULL) return;
	if ( (pc.ptAct->sMindIcon == SPR_asleep) ||
		(pc.ptAct->sMindIcon == SPR_cafe)   ||
		(pc.ptAct->sMindIcon == SPR_accent) ){
			CHAREXTRA *ext;
			ext = (CHAREXTRA *)pc.ptAct->pYobi;

			DeathAction( ext->ptMindIcon);
			ext->ptMindIcon = NULL;
			pc.ptAct->sMindIcon = 0;
			
			lssproto_MA_send(sockfd, nowGx, nowGy, 0);
	}
#endif
#ifdef _REMAKE_20
	if (!ChangedLibrary()) 
		RestoreLibrary();
#endif
	if (bNewServer)
		lssproto_W2_send(sockfd , x, y, direction );
	else
		old_lssproto_W_send(sockfd , x, y, direction );
}


void noChecksumWalkSendForServer( int x, int y, char *direction )
{
//#ifdef __NEW_PROTOCOL
	if (bNewServer)
		lssproto_W2_send(sockfd , x, y, direction );
	else
//#else
		old_lssproto_w_send(sockfd , x, y, direction );
//#endif
}


// ???????
void lssproto_W_recv( int fd, int id, int x, int y )
{
	// ?????????????????
	// ??????????????????????
}

#ifdef _SETTICK_COUNT
float *fTalkTick=NULL;
#endif
void chatStrSendForServer( char *str, int color )
{
	char dest[1024], m[1024];
	int x, y;
#ifdef _FRIENDCHANNEL
	char tmpMsg[128];
#endif
#ifdef _TELLCHANNEL
	char tmp1[128] ;
#endif

#ifdef _CHAR_MANOR_DEBUG
	if (strstr(str, ".光环")) {
		int 光环ID = 0;
		光环ID = getIntegerToken(str, ' ', 2);
		setCharmManor(pc.ptAct, 光环ID);
		return;
	}
#endif

#ifdef _人物称号_DEBUG
	if (strstr(str, ".称号")) {
		int 称号ID = 0;
		称号ID = getIntegerToken(str, ' ', 2);
		setCharmTitle(pc.ptAct, 称号ID);
		return;
	}
#endif


#ifdef _SETTICK_COUNT
	if ( fTalkTick == NULL || (GetTickCount() - *fTalkTick) > 60000 ){
		FREE( fTalkTick);
#ifdef  _STONDEBUG_
		g_iMallocCount--;
#endif
		fTalkTick = (float *)MALLOC( sizeof(float));
#ifdef  _STONDEBUG_
		g_iMallocCount++;
#endif
		*fTalkTick = (float)0.00;
	}
	if ( (GetTickCount() - *fTalkTick) > 2500 ){
		*fTalkTick = (float)GetTickCount();
	}else{
		return;
	}
#endif
	x = nowGx;
	y = nowGy;
	makeEscapeString( str, dest, sizeof(dest) );

#ifdef _TELLCHANNEL
	getStringToken( str , ' ', 1, sizeof( tmp1 ) -1, tmp1 );
		char *found;
		char tellName[32]={""};

	switch (TalkMode){
	case 0:

#ifdef _CHANNEL_MODIFY
	case 2:
#endif
		sprintf( m, "P|%s", dest );
		break;
	case 1:
		if ( strcmp ( tmp1 , str ) == 0 || strlen(tmp1) > 16) {
			StockChatBufferLine(  "指令使用格式不正确！" , FONT_PAL_RED);
			strcpy(secretName,"");
			return ; 
		}
		if ( found = strstr( str, " " )){
			strncpy(tellName, str, strlen(str) - strlen(found));
		}
		sprintf( secretName, "%s ",tellName);
		sprintf( m, "P|/tell %s", dest );
		break;
#ifndef _CHANNEL_MODIFY
	#ifdef _FRIENDCHANNEL
		case 2:
			sprintf(tmpMsg,"T|%s : %s",pc.name,dest);
			lssproto_CHATROOM_send (sockfd ,tmpMsg);
			return;
	#endif
#else
		case 3:
			sprintf(m,"P|/FM%s",dest);
			break;
		case 4:
			sprintf(m,"P|/OCC%s：%s",pc.name,dest);
			break;
#ifdef _CHATROOMPROTOCOL
		case 5:
			sprintf(tmpMsg,"T|%s：%s",pc.name,dest);
			lssproto_CHATROOM_send (sockfd ,tmpMsg);
			return;
#endif
#ifdef _CHATWORLDWIDEPROTOCOL
		case 6:
			sprintf(m, "P|/WD %s", dest);
			break;
#endif
#ifdef _NB_CHANNEL_ALLSERV
		case 7:
			sprintf(m, "P|/XQ %s", dest);
			break;
#endif
#endif
	}
#else
	sprintf( m, "P|%s", dest );
#endif
	if (bNewServer)
		lssproto_TK_send(sockfd, x, y, m, color, NowMaxVoice );
	else
		old_lssproto_TK_send(sockfd, x, y, m, color, NowMaxVoice );
}




// ???? ///////////////////////////////////////////////////////////
void lssproto_TK_recv( int fd, int index, char *message, int color )
{
	char id[2];
	char msg[2024];
	ACTION *ptAct;
	int fontsize =0;

	int type;

	// ????????????????????
	if ( logOutFlag )
		return;

	getStringToken( message, '|', 1, sizeof( id  ) - 1, id );

	if ( id[0] == 'P' ){
#ifndef _CHANNEL_MODIFY
		getStringToken( message, '|', 2, sizeof( msg ) - 1, msg );
		makeStringFromEscaped( msg );
	#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
		TradeTalk( msg ) ; 
	#endif
#endif

#ifdef _CHANNEL_MODIFY
		char szToken[4];

		if (getStringToken(message,'|',2,sizeof(szToken) - 1,szToken) == 0){
			getStringToken(message,'|',3,sizeof(msg) - 1,msg);
			makeStringFromEscaped(msg);
#ifdef _FONT_SIZE
			char token[10];
			if ( getStringToken(message,'|',4,sizeof(token) - 1,token) == 1) {
				fontsize = atoi( token);
				if ( fontsize < 0 )	fontsize = 0;

				//CC fix
				//适配世界频道
				if (fontsize == 99)	fontsize = 0;
			}
			else {
				fontsize = 0;
			}
#endif

			type = TYPE_TALK::附近;

			if (strlen(szToken) > 1){
				if (strcmp(szToken,"TK") == 0)	InitSelectChar(message,0);
				else if (strcmp(szToken,"TE") == 0) InitSelectChar(message,1);
				return;
			}
			else{
				switch (szToken[0]){
				
				case 'M':
					type = TYPE_TALK::私聊;  // 密语频道
					break;
				
				case 'F':
					type = TYPE_TALK::家族;  // 家族频道
					break;
			
				case 'T':
					type = TYPE_TALK::队伍;  // 队伍频道
					break;	 

				case 'O':
					type = TYPE_TALK::系统;  // 职业频道
					break;
				}
				SaveChatData(msg,szToken[0],false);
			}
		}
		else getStringToken(message,'|',2,sizeof(msg) - 1,msg);
#ifdef _TALK_WINDOW
		if (!g_bTalkWindow)
#endif
		TradeTalk(msg);
		if (strcmp(msg,"成立聊天室扣除２００石币") == 0)	pc.gold -= 200;
#ifdef _FONT_SIZE

		if (index == -1)type = TYPE_TALK::系统;


		StockChatBufferLineExt( msg, color, fontsize, type);
#else
		StockChatBufferLine(msg,color);
#endif
#else
	#ifdef _TELLCHANNEL		// (不可开) ROG ADD 密语频道
		char tellName[128] = { "" };
		char tmpMsg[STR_BUFFER_SIZE + 32];
		char TK[4];

		if ( getStringToken( msg, '|', 1, sizeof( TK ) - 1, TK ) == 0){
			if (strcmp(TK,"TK") == 0)	InitSelectChar( msg, 0);
			else if (strcmp(TK,"TE") == 0) InitSelectChar( msg, 1);
		}
		else{
			char temp[] = "告诉你：";
			char *found;

			if (strcmp( msg, "成立聊天室扣除２００石币") == 0)	pc.gold -= 200;

			if ( found = strstr( msg, temp )){
				strncpy(tellName, msg, strlen(msg) - strlen(found));
				color = 5;
				sprintf(tmpMsg,"[%s]%s",tellName, found);
				StockChatBufferLine( tmpMsg, color );
				sprintf(msg,"");
				sprintf(secretName,"%s ",tellName);
			}else StockChatBufferLine( msg, color );
		}
	#else
#ifdef _FONT_SIZE
		StockChatBufferLineExt( msg, color, fontsize );
#else
		StockChatBufferLine( msg, color );
#endif
#endif
#endif
		if ( index >= 0 ){
			if ( pc.ptAct != NULL && pc.id == index ){
				// JL 注释 设置说话时气泡保留的时间
				setPcFukidashi( 1000 );  
			}
			else
			{
				ptAct = getCharObjAct( index );
				if ( ptAct != NULL )
				{
					// 1000??
					setCharFukidashi( ptAct, 1000 );
				}
			}
		}
	}

/*#if 0
	if (index == my_id_in_server){
		if (pinfo.ptaction != NULL){
			createPlayerCommonEffect( pinfo.ptaction, SPR_fukidasi, 0, 1, -5 );
		}
	}else{
		int goind = searchGameObjectIndex( index );
//		if ( goind >= 0 ||gameobj[goind].ptaction!=NULL){
		if ( goind >= 0 && gameobj[goind].ptaction!=NULL ){
			createPlayerCommonEffect( gameobj[goind].ptaction, SPR_fukidasi, 0, 1, -5 );
		}
	}

#ifdef DEBUGPUSH
	if (color<249)PUSHVAR("??????????%d?'%s'",color, message); 
#endif
	char id[2];
    char msg[2024];
	getStringToken( message , '|' , 1 , sizeof( id ) - 1, id);
	if (id[0] == 'P'){
		getStringToken( message , '|' , 2 , sizeof( msg ) - 1, msg);
		makeStringFromEscaped(msg);
		char cutmes[1024];
		int i, j;
		for (i=0, j=0;;i++,j++){
			cutmes[j] = msg[i];
			if (msg[i] == '\0'){
				chatinputPushString(cutmes, color);
				break;
			}
			if ( IsDBCSTrailByte( msg, &msg[i+1] ) ){
				continue;
			}
			cutmes[j+1]='\0';
			if (GetStringWidth(cutmes)>600){
				j = -1;
				chatinputPushString(cutmes, color);
			}
		}
	}else if (id[0] == 'D'){
		int i;
		selectwindowid = getIntegerToken(message, '|', 2);
		void setDengonbanMessageNumber(int i);//netaction.cpp?
		setDengonbanMessageNumber(getIntegerToken(message, '|', 3));
		for (i=0;i<5;i++){
			getStringToken( message , '|' , i+4 , sizeof( selectstringline[i] ) - 1, selectstringline[i]);
			makeStringFromEscaped(selectstringline[i]);
		}
		cantclosewindowflag = 1;
		OpenNPCWindow(NPC_BOARD_WINDOW);
	}else if (id[0] == 'W'){
		int i;
		char token[1024];
		getStringToken( message , '|' , 2 , sizeof( token ) - 1, token);
		selectwindowid = atoi(token);
		char mode[10];
		getStringToken( message , '|' , 3 , sizeof( mode ) - 1, mode);
		char caption[255];
		int flag;
		flag = getStringToken( message , '|' , 4 , sizeof( caption ) - 1, caption);
		makeStringFromEscaped(caption);
		strcpy(selectstringline[0],caption);
		for (i=1;;i++){
			selectstringlineindex[i] = getIntegerToken(message, '|', 3+i*2);
			flag = getStringToken( message , '|' , 4+i*2 , sizeof( caption ) - 1, caption);
			makeStringFromEscaped(caption);
			strcpy(selectstringline[i],caption);
			if (flag==1)break;
		}
		selectstringlinenumber=i;
		cantclosewindowflag = 1;
		if (mode[0] == 'G')OpenNPCWindow(NPC_KATARIBE_WINDOW);
		else if (mode[0] == 'N')OpenNPCWindow(NPC_WINDOW);
		else PUSHVAR("????????????????????????", msg);
	}else if (id[0] == 'M'){
		int i = 0, j = 0, k = 0;
		if (strlen(message)>1024){
#ifdef DEBUGPUSH
			PUSH("??????????????????");
#endif
			return;
		}
		char token[1024];
		getStringToken( message , '|' , 2 , sizeof( token ) - 1, token);
		selectwindowid = atoi(token);
		char mode[10];
		getStringToken( message , '|' , 3 , sizeof( mode ) - 1, mode);
		char caption[1024], onelinestring[1024];
		getStringToken( message , '|' , 4 , sizeof( caption ) - 1, caption);
		makeStringFromEscaped(caption);
		for (i=0;;i++){
			if (caption[i] == '\n'){
				onelinestring[k] = '\0';
				memcpy(selectstringline[j], onelinestring, k+1);
				j++;
				k = 0;
			}else if (caption[i] == '\0'){
				onelinestring[k] = '\0';
				memcpy(selectstringline[j], onelinestring, k+1);
				j++;
				break;
			}else{
				onelinestring[k] = caption[i];
				k++;
			}
		}
		selectstringlinenumber=j;
		if (mode[0] == 'G')OpenNPCWindow(NPC_G_MESSAGE_WINDOW);
		else if (mode[0] == 'N')OpenNPCWindow(NPC_MESSAGE_WINDOW);
		else PUSHVAR("????????????????????????", msg);
	}else if (id[0] == 'S'){
		int i;
#if 1
		// ?????????????????
		for ( i = 0; i < MAXSHOPITEM; i++ )
		{
			shopitem[i].plice = 0;
			shopitem[i].stock = 0;
			shopitem[i].itm.name[0] = '\0';
			shopitem[i].itm.name2[0] = '\0';
			shopitem[i].itm.memo[0] = '\0';
			shopitem[i].itm.stage = 0;
			shopitem[i].itm.imgno = 0;
		}
#endif
		selectwindowid = getIntegerToken( message , '|' , 2 );	
		for (i=0;i<7;i++){
			getStringToken( message , '|' , 3+i , sizeof( shopmessage[i] ) - 1, shopmessage[i] );
			makeStringFromEscaped(shopmessage[i]);
		}
		shopgold = getIntegerToken( message , '|' , 10 );
		shopstock = getIntegerToken(message, '|', 11 );
		for (i=0;i<shopstock;i++){
			getStringToken( message , '|' , 12+i*5, sizeof(shopitem[i].itm.name) - 1, shopitem[i].itm.name);
			makeStringFromEscaped(shopitem[i].itm.name);
			if (strlen(shopitem[i].itm.name)==0)break;
			shopitem[i].plice = getIntegerToken( message , '|' , 13+i*5);
			shopitem[i].itm.stage = getIntegerToken( message , '|' , 14+i*5);
			if (shopitem[i].itm.stage<0)shopitem[i].itm.stage=0;
			shopitem[i].itm.imgno = getIntegerToken( message , '|' , 15+i*5);
			getStringToken( message , '|' , 16+i*5, sizeof(shopitem[i].itm.memo) - 1, shopitem[i].itm.memo);
			makeStringFromEscaped(shopitem[i].itm.memo);
			shopitem[i].stock = 1;
		}
		for (i=0;i<40;i++){
			charitemplice[i] = getIntegerToken(message, '|', 12+shopstock*5+i);
		}
		void setShoppage(int i);//layout.cpp
		setShoppage (0);
		OpenNPCWindow(NPC_SHOP_START_WINDOW);
		cantclosewindowflag = 1;
		PUSHVAR( "??????? '%s'??????????" , msg );
	}else if (id[0] == 'L'){//???
		getStringToken( message , '|' , 2 , sizeof( selectstringline[0] ) - 1, selectstringline[0] );
			makeStringFromEscaped(selectstringline[0]);
		getStringToken( message , '|' , 3 , sizeof( selectstringline[1] ) - 1, selectstringline[1] );
			makeStringFromEscaped(selectstringline[1]);
		selectstringlineindex[1] = SYSTEM_CHARLOST;
		selectstringlinenumber=1;
		cantclosewindowflag = 1;
		OpenSystemWindow();
	}else if (id[0] == 'K'){
		void setSystemMessage(int i, char *mes, int index);//layout.cpp
		int i;
		char token[1024];
		getStringToken( message , '|' , 2 , sizeof( token ) - 1, token);
		selectwindowid = atoi(token);
		char caption[255];
		int flag;
		int index;
		flag = getStringToken( message , '|' , 3 , sizeof( caption ) - 1, caption);
		makeStringFromEscaped(caption);
		strcpy(selectstringline[0],caption);
		setSystemMessage( 0, caption, SYSTEM_SYSTEM);
		for (i=1;;i++){
			flag = getStringToken( message , '|' , i+3 , sizeof( caption ) - 1, caption);
			makeStringFromEscaped(caption);
			strcpy(selectstringline[i],caption);
			if (strcmp(selectstringline[i],"?????")==0){
				index = SYSTEM_LOGOUT;
			}else{
				index = SYSTEM_SERVERPUSH_1+i-1;
			}
			selectstringlineindex[i]=index;
			setSystemMessage( i, caption, index);
			if (flag==1)break;
		}
		selectstringlinenumber=i;
		deadflag = 1;
		OpenSystemWindow();
//		OpenNPCWindow(NPC_WINDOW);
		PUSH("TK?????????");
	}else if (id[0] == 'R'){
		deadflag = 0;
	}else{
		PUSH("TK????????????");
	}

#endif
	*/
}




// ???? ///////////////////////////////////////////////////////////
// ????????????????
void createNewCharStart(void)
{
	newCharStatus = 0;

	// ??????????????????
	if (bNewServer)
		lssproto_CreateNewChar_send(sockfd, selectPcNo, newCharacterName,
			newCharacterGraNo, newCharacterFaceGraNo,
			newCharacterVit, newCharacterStr, newCharacterTgh, newCharacterDex,
			newCharacterEarth, newCharacterWater, newCharacterFire, newCharacterWind,
			newCharacterHomeTown );
	else
		old_lssproto_CreateNewChar_send(sockfd, selectPcNo, newCharacterName,
			newCharacterGraNo, newCharacterFaceGraNo,
			newCharacterVit, newCharacterStr, newCharacterTgh, newCharacterDex,
			newCharacterEarth, newCharacterWater, newCharacterFire, newCharacterWind,
			newCharacterHomeTown );

	SETSENDING;
}


// ???????
//   ??： 0 ... ? / 1 ... ?? / -1 ... ?????? / -2 ... ?????????
int createNewCharProc(void)
{
	if ( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if ( newCharStatus == 1 )
		{
			return 1;
		}
		else
		{
			return -2;
		}
    }

	SETTIMEOUT2( NET_ERRMSG_CREATECHARTIMEOUT );

	return 0;
}


// ??????
void lssproto_CreateNewChar_recv( int fd, char *result, char *data ) 
{
	if ( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;

		if ( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			newCharStatus = 1;
		}
    }
}








// ????? /////////////////////////////////////////////////////////////
// ???????
void delCharStart(void)
{
	charDelStatus = 0;

	if (bNewServer)
		lssproto_CharDelete_send(sockfd, gamestate_deletechar_charname );
	else
		old_lssproto_CharDelete_send(sockfd, gamestate_deletechar_charname );

	SETSENDING;
}


// ??????
//   ??： 0 ... ??? / 1 ... ???? / -1 ... ?????? / -2 ... ?????????
int delCharProc(void)
{
	if ( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if ( charDelStatus )
		{
			return 1;
		}
		else
		{
			return -2;
		}
	}

	SETTIMEOUT2( NET_ERRMSG_DELETECHARTIMEOUT );

	return 0;
}


// ???????
void lssproto_CharDelete_recv( int fd, char *result, char *data )
{
	if ( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;
		if ( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			charDelStatus = 1;
    	}
    }
}

// ????? /////////////////////////////////////////////////////////
void lssproto_PR_recv( int fd, int request, int result )
{
	// ????????????????????
	if ( logOutFlag )
		return;

	if ( request == 1 && result == 1 )
	{
		// ??????????????
		setPcParty();
	}
	else
	if ( request == 0 && result == 1 )
	{
		// ??????????
		//delPcParty();
		//delPcLeader();
		partyModeFlag = 0;
		clearPartyParam();
#ifdef _CHANNEL_MODIFY
		pc.etcFlag &= ~PC_ETCFLAG_CHAT_MODE;
		if (TalkMode == 2) TalkMode = 0;
#endif

		char dir = (pc.dir+5) % 8;
		if (bNewServer)
			lssproto_SP_send(sockfd, nextGx, nextGy, dir );
		else
			old_lssproto_SP_send(sockfd, nextGx, nextGy, dir );
	}
	prSendFlag = 0;
}



// EV?????????? /////////////////////////////////////////
void lssproto_EV_recv( int fd, int seqno, int result )
{


	// ????????????????????
	if ( logOutFlag )
		return;

	if ( eventWarpSendId == seqno )
	{
		eventWarpSendFlag = 0;
		if ( result == 0 )
		{
			// ?????????????
			redrawMap();
			floorChangeFlag = FALSE;
			// ???????????
			warpEffectStart = TRUE;

		}
	}
	else
	if ( eventEnemySendId == seqno )
	{
		if ( result == 0 )
		{
			eventEnemySendFlag = 0;
		}
		//else
		//{
			// ??????process.cpp???
		//}
	}
}


// ??OK???OK????? ＯＫ? ////////////////////////
void lssproto_FS_recv( int fd, int flg )
{
	// ????????????????????
	if ( logOutFlag )
		return;

	pc.etcFlag = (unsigned short)flg;
	pc.etcFlag &= ~PC_ETCFLAG_JOINT_BTL;	// ?????????
}



// ????????? ///////////////////////////////////////////////
void lssproto_AB_recv( int fd, char *data )
{
	int i;
	int no;
	int nameLen;
	char name[256];
	int flag;
	int useFlag;
#ifdef _MAILSHOWPLANET				// (可开放) Syu ADD 显示名片星球
	char planetid[8];
	int j ;
#endif
	// ????????????????????
	if ( logOutFlag )
		return;

	for ( i = 0; i < MAX_ADR_BOOK; i++ )
	{
		//no = i * 6; //the second
        //CC_FIX 客户端名片有故障，每一个名片应该有8个说明符号，7个说明有异常 20170806
		//no = i * 7;
		no = i * 8;
		useFlag = getIntegerToken(data, '|', no+1 );
		if ( useFlag == -1 )
		{
			useFlag = 0;
		}
		if ( useFlag <= 0 )
		{
#if 0
			if ( addressBook[i].useFlag == 1 )
#else
			if ( MailHistory[i].dateStr[MAIL_MAX_HISTORY-1][0] != '\0' )
#endif
			{
				memset( &MailHistory[i], 0, sizeof( MailHistory[0] ) ) ;
				SaveMailHistory( i );
				// ????
				mailLamp = CheckMailNoReadFlag();
				// ?????
				DeathLetterAction();
			}
			addressBook[i].useFlag = 0;
			addressBook[i].name[0] = '\0';
			continue;
		}

#ifdef _EXTEND_AB
		if ( i == MAX_ADR_BOOK-1 )
			addressBook[i].useFlag = useFlag;
		else
			addressBook[i].useFlag = 1;
#else
		addressBook[i].useFlag = 1;
#endif

		flag = getStringToken(data, '|', no+2, sizeof(name) - 1 , name );

		if ( flag == 1 )
			break;

		makeStringFromEscaped(name);
		nameLen = strlen(name);
		if ( 0 < nameLen && nameLen <= CHAR_NAME_LEN )
		{
			strcpy( addressBook[i].name, name );
		}
		addressBook[i].level = getIntegerToken(data, '|', no+3 );
		addressBook[i].dp = getIntegerToken(data, '|', no+4 );
		addressBook[i].onlineFlag = (short)getIntegerToken(data, '|', no+5 );
		addressBook[i].graNo = getIntegerToken(data, '|', no+6 );
		addressBook[i].transmigration = getIntegerToken(data, '|', no+7 );
#ifdef 修复邮件服务器显示
		if (addressBook[i].onlineFlag)
		{
			getStringToken(data, '|', no + 8, 12, addressBook[i].planetname);
		}
#endif // 修复邮件服务器显示



//#ifdef _MAILSHOWPLANET				// (可开放) Syu ADD 显示名片星球
//		for ( j = 0 ; j < MAX_GMSV ; j ++ ) {
//			if ( gmsv[j].used == '1' ) {
//				getStringToken( gmsv[j].ipaddr, '.', 4, sizeof( planetid ) -1, planetid );
//				if ( addressBook[i].onlineFlag == atoi(planetid) ) {
//					sprintf ( addressBook[i].planetname , "%s" , gmsv[j].name ) ; 
//					break;
//				}
//			}
//		}
//#endif


	}
}


// ???????????////////////////////////////////////////
void lssproto_ABI_recv( int fd, int num, char* data )
{
	char name[256];
	int nameLen;
	int useFlag;
#ifdef _MAILSHOWPLANET				// (可开放) Syu ADD 显示名片星球
	char planetid[8];
	int j ;
#endif

	// ????????????????????
	if ( logOutFlag )
		return;

	useFlag = getIntegerToken(data, '|', 1 );
	if ( useFlag == -1 )
	{
		useFlag = 0;
	}
	if ( useFlag == 0 )
	{
#if 0
		if ( addressBook[num].useFlag == 1 )
#else
		if ( MailHistory[num].dateStr[MAIL_MAX_HISTORY-1][0] != '\0' )
#endif
		{
			memset( &MailHistory[num], 0, sizeof( MailHistory[0] ) ) ;
			SaveMailHistory( num );
			// ????
			mailLamp = CheckMailNoReadFlag();
			// ?????
			DeathLetterAction();
		}
		addressBook[num].useFlag = useFlag;
		addressBook[num].name[0] = '\0';
		return;
	}

#ifdef _EXTEND_AB
	if ( num == MAX_ADR_BOOK-1 )
		addressBook[num].useFlag = useFlag;
	else
		addressBook[num].useFlag = 1;
#else
	addressBook[num].useFlag = useFlag;
#endif

	getStringToken(data, '|', 2, sizeof(name) - 1 , name );
	makeStringFromEscaped(name);
	nameLen = strlen(name);
	if ( 0 < nameLen && nameLen <= CHAR_NAME_LEN )
	{
		strcpy( addressBook[num].name, name );
	}
	addressBook[num].level = getIntegerToken(data, '|', 3 );
	addressBook[num].dp = getIntegerToken(data, '|', 4 );
	addressBook[num].onlineFlag = (short)getIntegerToken(data, '|', 5 );
	addressBook[num].graNo = getIntegerToken(data, '|', 6 );
	addressBook[num].transmigration = getIntegerToken(data, '|', 7 );
#ifdef 修复邮件服务器显示				// (可开放) Syu ADD 显示名片星球
	if ( addressBook[num].onlineFlag == 0 ) 
		sprintf( addressBook[num].planetname , " ");
	else
		getStringToken(data, '|',  8, 12, addressBook[num].planetname);
	//for ( j = 0 ; j < MAX_GMSV ; j ++ ) {
	//	if ( gmsv[j].used == '1' ) {
	//		getStringToken( gmsv[j].ipaddr, '.', 4, sizeof( planetid ) -1, planetid );
	//		if ( addressBook[num].onlineFlag == atoi(planetid) ) {
	//			sprintf ( addressBook[num].planetname , "%s" , gmsv[j].name ) ; 
	//			break;
	//		}
	//	}
	//}
#endif


}


// JL 战斗结果显示 获得经验和道具 /////////////////////////////////////////////////
void lssproto_RS_recv( int fd, char *data ){
	int i;
	char token[2048];
	char item[2048];

	//JLWG 是快速战斗则不处理此包
	extern int g_bFastFight;
	if (g_bFastFight){
		return;
	}

	if ( logOutFlag )
		return;

	battleResultMsg.useFlag = 1;
	//cary ?? 栏位 数
	int cols = RESULT_CHR_EXP;
	getStringToken(data, ',', RESULT_CHR_EXP+1, sizeof( token ) - 1, token );
	if ( token[0] == 0){
		cols = RESULT_CHR_EXP - 1;
		battleResultMsg.resChr[RESULT_CHR_EXP-1].petNo = -1;
		battleResultMsg.resChr[RESULT_CHR_EXP-1].levelUp = -1;
		battleResultMsg.resChr[RESULT_CHR_EXP-1].exp = -1;
	}
	//end cary
	for ( i = 0; i < cols; i++ ){
		getStringToken(data, ',', i+1, sizeof( token ) - 1, token );
		battleResultMsg.resChr[i].petNo = getIntegerToken( token, '|', 1 );
		battleResultMsg.resChr[i].levelUp = getIntegerToken( token, '|', 2 );
		battleResultMsg.resChr[i].exp = getInteger62Token( token, '|', 3 );
	}

	getStringToken(data, ',', i+1, sizeof( token ) - 1, token );
	getStringToken( token, '|', 1, sizeof( item ) - 1, item );
	makeStringFromEscaped( item );
	if ( strlen( item ) <= RESULT_ITEM_NAME_LEN ){
		strcpy( battleResultMsg.item[0], item );
	}
	getStringToken( token, '|', 2, sizeof( item ) - 1, item );
	makeStringFromEscaped( item );
	if ( strlen( item ) <= RESULT_ITEM_NAME_LEN ){
		strcpy( battleResultMsg.item[1], item );
	}
	getStringToken( token, '|', 3, sizeof( item ) - 1, item );
	makeStringFromEscaped( item );
	if ( strlen( item ) <= RESULT_ITEM_NAME_LEN ){
		strcpy( battleResultMsg.item[2], item );
	}
}


// ??????????? /////////////////////////////////////////////////
void lssproto_RD_recv( int fd, char *data )
{
	// ????????????????????
	if ( logOutFlag )
		return;

	battleResultMsg.useFlag = 2;

	battleResultMsg.resChr[0].exp = getInteger62Token(data, '|', 1 );
	battleResultMsg.resChr[1].exp = getInteger62Token(data, '|', 2 );
}


// ?????? ///////////////////////////////////////////////////
void lssproto_SI_recv( int fd, int from, int to )
{
	// ????????????????????
	if ( logOutFlag )
		return;

	swapItem( from, to );
}


// ???? ///////////////////////////////////////////////////////
void lssproto_I_recv( int fd, char *data )
{
	int i, j;
	int no;
	char name[256];
	char name2[256];
	char memo[256];

	// ????????????????????
	if ( logOutFlag )
		return;

	for ( j = 0; ; j++ ){

#ifdef _PET_ITEM
		//no = j * 14;
		//CC FIX 这里修复成15？？？
		no = j * 15;
#endif//_PET_ITEM

		i = getIntegerToken(data, '|', no+1 );
		if ( getStringToken(data, '|', no+2, sizeof(name) - 1 , name ) == 1 )
			break;
		makeStringFromEscaped(name);
		if ( strlen(name) == 0 ){
			pc.item[i].useFlag = 0;
			continue;
		}

		pc.item[i].useFlag = 1;
		if ( strlen(name) <= ITEM_NAME_LEN ){
			strcpy( pc.item[i].name, name );
		}
		getStringToken(data, '|', no+3, sizeof( name2 ) - 1, name2 );
		makeStringFromEscaped( name2 );
		if ( strlen( name2 ) <= ITEM_NAME2_LEN ){
			strcpy( pc.item[i].name2, name2 );
		}
		pc.item[i].color = getIntegerToken(data, '|', no+4 );
		if ( pc.item[i].color < 0 )
			pc.item[i].color = 0;
		getStringToken(data, '|', no+5, sizeof( memo ) - 1, memo );
		makeStringFromEscaped( memo );
		if ( strlen( memo ) <= ITEM_MEMO_LEN ){
			strcpy( pc.item[i].memo, memo );
		}
		pc.item[i].graNo = getIntegerToken(data, '|', no+6 );
		pc.item[i].field = getIntegerToken(data, '|', no+7 );
		pc.item[i].target = getIntegerToken(data, '|', no+8 );
		if ( pc.item[i].target >= 100 ){
			pc.item[i].target %= 100;
			pc.item[i].deadTargetFlag = 1;
		}else{
			pc.item[i].deadTargetFlag = 0;
		}
		pc.item[i].level = getIntegerToken(data, '|', no+9 );
		pc.item[i].sendFlag = getIntegerToken(data, '|', no+10 );

		{
			// 显示物品耐久度
			char damage[256];
			getStringToken(data, '|', no+11, sizeof( damage ) - 1, damage );
			makeStringFromEscaped( damage );
			if ( strlen( damage ) <= 16 ){	
				strcpy( pc.item[i].damage, damage );
			}
		} 

#ifdef _ITEM_PILENUMS
		{
			char pile[256];
			getStringToken(data, '|', no+12, sizeof( pile ) - 1, pile );
			makeStringFromEscaped( pile );
			pc.item[i].pile = atoi( pile);
		}
#endif

#ifdef _ALCHEMIST //_ITEMSET7_TXT
		{
			char alch[256];
			getStringToken(data, '|', no+13, sizeof( alch ) - 1, alch );
			makeStringFromEscaped( alch );
			strcpy( pc.item[i].alch, alch );
		}
#endif
#ifdef _PET_ITEM
		{
			char type[8];
			getStringToken(data, '|', no + 14, sizeof(type) - 1, type);
			makeStringFromEscaped(type);
			pc.item[i].type = atoi(type);
		}
#endif


	}
}

void lssproto_WN_recv( int fd,int windowtype,int buttontype,int seqno,int objindex,char* data )
{
	// ????????????????????
	if ( logOutFlag )
		return;

	if( strstr(data,"否则家族在七天之后会消失唷！") ){	
		if( TimeGetTime() - MsgCooltime > 300000 )
			MsgCooltime = TimeGetTime();  
		else
			return;
	}
	openServerWindow( windowtype, buttontype, seqno, objindex, data );
}


// ??????? ///////////////////////////////////////////////////////
void lssproto_PME_recv( int fd, int objindex,
	int graphicsno, int x, int y, int dir, int flg, int no, char *cdata )
{
	// ????????????????????
	if ( logOutFlag )
		return;

	// ?????
	if ( encountNowFlag )
		return;

	// ??
	if ( flg == 0 )
	{
		switch ( no )
		{
			case 0:
				createPetAction( graphicsno, x, y, dir, 0, dir, -1 );
				break;
			case 1:
				createPetAction( graphicsno, x, y, dir, 2, 0, -1 );
				break;
		}
	}
	else
	// ?
	{
		char smalltoken[2048];
		int id;
		int x;
		int y;
		int dir;
		int graNo;
		int level;
		int nameColor;
		char name[2048];
		char freeName[2048];
		int walkable;
		int height;
		int charType;
		int ps=1;
#ifdef _OBJSEND_C
		ps = 2;
#endif
		charType = getIntegerToken( cdata, '|', ps++);
		getStringToken( cdata, '|', ps++, sizeof(smalltoken) - 1, smalltoken);
		id = a62toi(smalltoken);
		getStringToken( cdata, '|', ps++, sizeof(smalltoken) - 1, smalltoken);
		x = atoi(smalltoken);
		getStringToken( cdata, '|', ps++, sizeof(smalltoken) - 1, smalltoken);
		y = atoi(smalltoken);
		getStringToken( cdata, '|', ps++, sizeof(smalltoken) - 1, smalltoken);
		dir = (atoi(smalltoken)+3)%8;
		getStringToken( cdata, '|', ps++, sizeof(smalltoken) - 1, smalltoken);
		graNo = atoi(smalltoken);
		getStringToken( cdata, '|', ps++, sizeof(smalltoken) - 1,smalltoken);
		level = atoi(smalltoken);
		nameColor = getIntegerToken( cdata, '|', ps++ );
		getStringToken( cdata, '|' , ps++ , sizeof(name) - 1, name );
		makeStringFromEscaped(name);
		getStringToken( cdata, '|' , ps++ , sizeof( freeName ) - 1, freeName );
		makeStringFromEscaped( freeName );
		getStringToken( cdata, '|', ps++, sizeof(smalltoken) - 1, smalltoken);
		walkable = atoi(smalltoken);
		getStringToken( cdata, '|', ps++, sizeof(smalltoken) - 1, smalltoken);
		height = atoi(smalltoken);

		// ?????????????
		if ( setReturnPetObj( id, graNo, x, y, dir, name, freeName,
			level, nameColor, walkable, height, charType ) )
		{
			switch ( no )
			{
				case 0:
					createPetAction( graphicsno, x, y, dir, 1, 0, objindex );
					break;

				case 1:
					createPetAction( graphicsno, x, y, dir, 3, 0, objindex );
					break;
			}
		}
	}
}

char *pCommand = NULL;
DWORD dwDiceTimer;
// ?????????? /////////////////////////////////////////////

//处理特效
void lssproto_EF_recv( int fd, int effect, int level, char *option )
{
	// ??????????
	if ( effect == 0 )
	{
		mapEffectRainLevel = 0;
		mapEffectSnowLevel = 0;
		mapEffectKamiFubukiLevel = 0;
#ifdef _HALLOWEEN_EFFECT
		mapEffectHalloween = 0;
		initMapEffect(FALSE);
#endif
		return;
	}
	// ???????
	if ( effect & 1 )
	{
		mapEffectRainLevel = level;
	}
	// ??????
	if ( effect & 2 )
	{
		mapEffectSnowLevel = level;
	}
	// ????
	if ( effect & 4 )
	{
		mapEffectKamiFubukiLevel = level;
	}
#ifdef _HALLOWEEN_EFFECT
	if (effect & 8) mapEffectHalloween = level;
#endif
// Terry add 2002/01/14
#ifdef __EDEN_DICE
	// 骰子
	if (effect == 10) 
	{
		pCommand = (char*)MALLOC(strlen(option)+1);
#ifdef  _STONDEBUG_
		g_iMallocCount++;
#endif
		if (pCommand != NULL) 
		{
			strcpy(pCommand,option);
			bMapEffectDice = TRUE;
			dwDiceTimer = TimeGetTime();
		}
	}
#endif
// Terry end
}


// ? ///////////////////////////////////////////////////////////////////
void lssproto_IS_recv( int fd, char* cdata )
{
}

void lssproto_EN_recv( int fd,int result,int field )	//JL 7号包，进入战斗 
{

	if ( logOutFlag )
		return;
	if ( result > 0 ){
		EncountFlag = TRUE;
		if ( result == 4)
			vsLookFlag = 1;
		

		//JLWG 2016.9.16 快速战斗处理
		extern int g_flag_FastFight;
		if (vsLookFlag == 0 && g_flag_FastFight == 1)	//如果是快速战斗的处理
		{
			EncountFlag = FALSE;
			pc.status |= CHR_STATUS_BATTLE;
			JL_SetFastFightFlag(true);
			JL_锁定人物(true);

			return;
		}else{
			JL_SetFastFightFlag(false);
		}


		if ( result == 6 || result == 2 )
			eventEnemyFlag = 1;
		else
			eventEnemyFlag = 0;

		if ( field < 0 || BATTLE_MAP_FILES <= field )
			BattleMapNo = 0;
		else
			BattleMapNo = field;

		if (result == 2)
		{
			NoCastFlag = false;  //JL fix 2018/8/28 决斗时强制复位沉默，可以下指令
			DuelFlag = TRUE;
		}
		else
			DuelFlag = FALSE;

		//if ( result == 2 || result == 5 )
		//	NoHelpFlag = TRUE;
		//else
		//	NoHelpFlag = FALSE;

		BattleStatusReadPointer = BattleStatusWritePointer =0;
		BattleCmdReadPointer = BattleCmdWritePointer =0;

	}else{
		sendEnFlag = 0;
		duelSendFlag = 0;
		jbSendFlag = 0;
	}
}


void lssproto_HL_recv( int fd, int flg )
{
	helpFlag = flg;
}

#ifdef 战斗光环
char gStr光环[100];
#endif 

void lssproto_B_recv( int fd,char* command )
{
	//JLWG 是快速战斗则不处理此包
	extern int g_bFastFight;
	if (g_bFastFight)
	{
		return;
	}
	//printf("\n%s",command);
	if ( *( command + 1 ) == 'C'){
		strcpy( BattleStatusBak[ BattleStatusWritePointer ], command );
		BattleStatusWritePointer = ( BattleStatusWritePointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
	}else if (*(command + 1) == 'P') {
		sscanf(command + 3, "%X|%X|%X", &BattleMyNo, &BattleBpFlag, &BattleMyMp);
		//JL fix 遇敌偷袭
		if (BattleBpFlag == 26)
		{
			BattleBpFlag = 0;
		}
	}else if ( *( command + 1 ) == 'A'){
		sscanf( command + 3, "%X|%X", &BattleAnimFlag, &BattleSvTurnNo );

		if ( BattleTurnReceiveFlag == TRUE ){ 
			BattleCliTurnNo = BattleSvTurnNo;
			BattleTurnReceiveFlag = FALSE;
		}
	}else if ( *( command + 1 ) == 'U')
		BattleEscFlag = TRUE;
#ifdef 战斗光环
	else if (*(command + 1) == '&'){
		printf("\n%s",command);
		if (*(command + 2) == '|'  && *(command + 3) && *(command + 4)){
			lstrcpyA(gStr光环, command + 3);
		}else{
			return;
		}
	
	}
#endif
	else {
		strcpy( BattleCmdBak[ BattleCmdWritePointer ], command );
		BattleCmdWritePointer = ( BattleCmdWritePointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
	}
	
#ifdef  _STONDEBUG__MSG
//	StockChatBufferLine( command, FONT_PAL_RED );
#endif

}

#ifdef _PETS_SELECTCON
void lssproto_PETST_recv( int fd,int petarray,int result )
{
	if ( petarray < 0 || petarray >= 5 ) return;
	pc.selectPetNo[ petarray] = result;
	BattlePetStMenCnt--;
	if ( BattlePetStMenCnt < 0 ) BattlePetStMenCnt = 0;
	if ( BattlePetStMenCnt > 等待宠数量) BattlePetStMenCnt = 等待宠数量;
	if ( pc.battlePetNo == petarray )
		pc.battlePetNo = -1;
}

#endif

void lssproto_KS_recv( int fd,int petarray,int result )
{
	int cnt = 0;
	int i;

	BattlePetReceiveFlag = FALSE;
	BattlePetReceivePetNo = -1;
	if ( result == TRUE ){ 
		battlePetNoBak = -2;
		if ( petarray != -1 ){ 
			pc.selectPetNo[ petarray ] = TRUE;
			if ( pc.mailPetNo == petarray ) pc.mailPetNo = -1;
			for ( i = 0 ; i < 5 ; i++ ){
				if ( pc.selectPetNo[ i ] == TRUE && i != petarray ) cnt++;
				if ( cnt >= 等待宠数量){
					pc.selectPetNo[ i ] = FALSE;
					cnt--;
				}
			}
		}
		pc.battlePetNo = petarray;
	}
#ifdef _AFTER_TRADE_PETWAIT_
	else{
		if (tradeStatus==2){
			pc.selectPetNo[petarray] = 0;
			if (petarray==pc.battlePetNo)
				pc.battlePetNo = -1;
		}		
	}
#endif
}

#ifdef _STANDBYPET
void lssproto_SPET_recv( int fd, int standbypet, int result )
{
	int cnt = 0;
	int i;

	StandbyPetSendFlag = FALSE;

	if ( result == TRUE ) { 
		pc.standbyPet = standbypet;
		for ( i =0; i <MAX_PET; i++) {
			if ( standbypet & ( 1 << i) )
				pc.selectPetNo[ i ] = TRUE;
			else
				pc.selectPetNo[ i ] = FALSE;
		}
	}

}
#endif


void lssproto_SKUP_recv( int fd,int point )
{
	// ????????????
	StatusUpPoint = point;
}

// ????????????????
void lssproto_MSG_recv( int fd,int aindex,char* text ,int color)
{
	char moji[ 256 ];
	int noReadFlag;
	
	// ???????????
	mailLamp = TRUE;
	
	// ??????
	MailHistory[ aindex ].newHistoryNo--;
	// ????????
	if ( MailHistory[ aindex ].newHistoryNo <= -1 ) MailHistory[ aindex ].newHistoryNo = MAIL_MAX_HISTORY - 1;
	
	// ????
	getStringToken( text, '|', 1, sizeof( MailHistory[ aindex ].dateStr[ MailHistory[ aindex ].newHistoryNo ] ) - 1, 
					MailHistory[ aindex ].dateStr[ MailHistory[ aindex ].newHistoryNo ] );
	// ???
	getStringToken( text, '|', 2, sizeof( MailHistory[ aindex ].str[ MailHistory[ aindex ].newHistoryNo ] ) - 1, 
					MailHistory[ aindex ].str[ MailHistory[ aindex ].newHistoryNo ] );
	// ????????
	makeStringFromEscaped( MailHistory[ aindex ].str[ MailHistory[ aindex ].newHistoryNo ] );
	
	// ?????????????
	noReadFlag = getIntegerToken( text, '|', 3 );
	// ????????
	if ( noReadFlag != -1 ){
		// ?????????????
		MailHistory[ aindex ].noReadFlag[ MailHistory[ aindex ].newHistoryNo ] = noReadFlag;
		// ??????????
		MailHistory[ aindex ].petLevel[ MailHistory[ aindex ].newHistoryNo ] = getIntegerToken( text, '|', 4 );
		// ???????
		getStringToken( text, '|', 5, sizeof( MailHistory[ aindex ].petName[ MailHistory[ aindex ].newHistoryNo ] ), 
						MailHistory[ aindex ].petName[ MailHistory[ aindex ].newHistoryNo ] );
		// ?????????????
		makeStringFromEscaped( MailHistory[ aindex ].petName[ MailHistory[ aindex ].newHistoryNo ] );
		// ???????????????
		MailHistory[ aindex ].itemGraNo[ MailHistory[ aindex ].newHistoryNo ] = getIntegerToken( text, '|', 6 );
		// ????????????
		sprintf( moji,"收到%s送来的宠物邮件！", addressBook[ aindex ].name );
	}
	// ??????
	else{	
		MailHistory[ aindex ].noReadFlag[ MailHistory[ aindex ].newHistoryNo ] = TRUE;
		// ????????????
		sprintf( moji,"收到%s送来的邮件！", addressBook[ aindex ].name );
	}
	
	// ??????????????????
	StockChatBufferLine( moji, FONT_PAL_WHITE );
	
	// ????????
	if ( mailHistoryWndSelectNo == aindex ){
		// ??????????????
		mailHistoryWndPageNo++;
		// ????????
		if ( mailHistoryWndPageNo >= MAIL_MAX_HISTORY ) mailHistoryWndPageNo = 0;
		// ?????????
	//	DeathLetterAction();
	}
	// ?
	play_se( 101, 320, 240 );
	// ???????
	SaveMailHistory( aindex );
}


// ???????????
void lssproto_PS_recv( int fd,int result,int havepetindex,int havepetskill,int toindex )
{
	char moji[ 256 ];
	
	// ???????
	ItemMixRecvFlag = FALSE;
	
	// ???
	if ( result == 0 ){
		//???????
		sprintf( moji,"失败！");
		// ??????????????????
		StockChatBufferLine( moji, FONT_PAL_WHITE );
	}
	
}


// ?????????
void lssproto_SE_recv( int fd, int x, int y, int senumber, int sw )
{
	// ????????????????????
	if ( logOutFlag )
		return;

	if ( sw )
	{
		play_se( senumber, x, y );
	}
	else
	{
		// ????????????
	}
}

// ??????????
void lssproto_XYD_recv( int fd, int x, int y, int dir )
{
	updateMapArea();
	setPcWarpPoint( x, y );
	setPcPoint();
	dir = (dir+3) % 8;
	setPcDir( dir );

	//JLWG 2016.9.16 收到2号包，战斗结束，去掉头顶战斗vs图案
	pc.status &= (~CHR_STATUS_BATTLE);

	vsLookFlag = 0;
	JL_锁定人物(false);

	//JL fix 2017.9.18  把eo放到这里发,因为原eo需要SubProcNo=8时发,(battleProc.cpp line:1011)
	//而如果画面速度过快,可能会不经过SubProcNo=8,导致不发eo,卡画面
	//暂时先不修,会出现战斗后画面被清
	//if (bNewServer)
	//	lssproto_EO_send(sockfd, 0);
	//else
	//	old_lssproto_EO_send(sockfd, 0);
}

void lssproto_WO_recv( int fd, int effect )
{
	return;
	// ???????
	if ( effect == 0 )
	{
		// ????????
		transmigrationEffectFlag = 1;
		// ?????????????
		transEffectPaletteStatus = 1;
		// ????????
		palNo = 15;
		palTime = 300;
	}
}

// ?????? /////////////////////////////////////////////////////////
void lssproto_Echo_recv( int fd, char *test )
{
#if 1
#ifdef  _STONDEBUG__MSG

	// ?????
	time( &serverAliveLongTime );
	serverAliveTime = localtime( &serverAliveLongTime );

#endif
#endif
}


// Robin 2001/04/06
void lssproto_NU_recv( int fd, int AddCount)
{
	//JL 2016.11.13  90号包，走路步数
	return;


	if ( AddCount < 0 )
		SendCount += AddCount;
	else
		SendCount += AddCount;
}


// ?????
void lssproto_PlayerNumGet_recv( int fd, int logincount, int player )
{
}


void lssproto_ProcGet_recv( int fd, char* data )
{
}




/*
   ???????????

*/
void lssproto_R_recv( int fd, char* data ) 
{
}


void lssproto_D_recv( int fd,int category,int dx,int dy,char* data ) 
{
}

/*
	???????????????????????
*/
void lssproto_ACI_recv( int fd, char* data )
{

}

extern void initFamilyList( char * );
extern void initFamilyDetail( char * );
extern void initJoinChannelWN( char * );
extern void initJoinChannel2WN( char * );
extern void initFamilyTaxWN( char * );
extern void initFamilyLeaderChange( char * );

void lssproto_FM_recv( int fd, char* data )
{
	char FMType1[512];
	char FMType2[512];
	char FMType3[512];
	
	getStringToken(data, '|', 1, sizeof( FMType1 ) - 1, FMType1 );
	//makeStringFromEscaped( FMType1 );
	getStringToken(data, '|', 2, sizeof( FMType2 ) - 1, FMType2 );
	//makeStringFromEscaped( FMType2 );

	if ( strcmp(FMType1,"S") ==0 )
	{
		if ( strcmp(FMType2,"F") ==0) // 家族列表
		{
			initFamilyList(data );

		}
		if ( strcmp(FMType2,"D") ==0) // 家族详细
		{
			initFamilyDetail(data );

		}

	}
	else if ( strcmp(FMType1,"C") ==0 )
	{
		if ( strcmp(FMType2,"J") ==0) // 加入频道
		{
			getStringToken(data, '|', 3, sizeof( FMType3 ) - 1, FMType3 );
			pc.channel = atoi( FMType3 );
			if ( pc.channel != -1 )
				pc.quickChannel = pc.channel;
		}
		if ( strcmp(FMType2,"L") ==0) // 频道列表
		{
			initJoinChannel2WN(data );

		}
	}
	else if ( strcmp(FMType1,"B") ==0 )
	{

		//MenuToggleFlag = JOY_CTRL_B;
		if ( strcmp(FMType2,"G") ==0)
		{
			//getStringToken(data, '|', 3, sizeof( FMType3 ) - 1, FMType3 );
			BankmanInit(data );
		}
		if ( strcmp(FMType2,"I") ==0)
		{
			//getStringToken(data, '|', 3, sizeof( FMType3 ) - 1, FMType3 );
			//ItemmanInit(data );
			//initItemman(data );
		}
		if ( strcmp(FMType2,"T") ==0)
		{
			initFamilyTaxWN(data );
		}


	}
	else if ( strcmp(FMType1,"R") ==0 )
	{
		if ( strcmp(FMType2,"P") ==0) // ride Pet
		{
			//initFamilyList(data );
			getStringToken(data, '|', 3, sizeof( FMType3 ) - 1, FMType3 );
			pc.ridePetNo = atoi( FMType3 );

		}

	}
	else if ( strcmp(FMType1,"L") ==0 )	// 族长功能
	{
		if ( strcmp(FMType2,"CHANGE") ==0)
		{
			initFamilyLeaderChange(data );
		}
	}

}


DWORD dwPingTime;
DWORD dwPingState;

#ifdef __NEW_CLIENT

#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0
#define ICMP_MIN 8 // minimum 8 byte icmp packet (just header)

/* The IP header */
typedef struct iphdr {
	unsigned int h_len:4;          // length of the header
	unsigned int version:4;        // Version of IP
	unsigned char tos;             // Type of service
	unsigned short total_len;      // total length of the packet
	unsigned short ident;          // unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char  ttl; 
	unsigned char proto;           // protocol (TCP, UDP etc)
	unsigned short checksum;       // IP checksum

	unsigned int sourceIP;
	unsigned int destIP;

}IpHeader;

typedef struct _ihdr {
  BYTE i_type;
  BYTE i_code; /* type sub code */
  USHORT i_cksum;
  USHORT i_id;
  USHORT i_seq;
  /* This is not the std header, but we reserve space for time */
  ULONG timestamp;
}IcmpHeader;

#define STATUS_FAILED 0xFFFF
#define DEF_PACKET_SIZE 32
#define MAX_PACKET 1024

USHORT checksum(USHORT *buffer, int size)
{
	unsigned long cksum=0;
	while( size > 1){
		cksum += *buffer++;
		size -=sizeof(USHORT);
	}
	if ( size)
		cksum += *(UCHAR*)buffer;
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);
	return (USHORT)(~cksum);
}

int iWrote, iRead;
SOCKET sockRaw = INVALID_SOCKET;
DWORD WINAPI PingFunc(LPVOID param)
{
	struct sockaddr_in from;
	struct sockaddr_in dest;
	int datasize;
	int fromlen = sizeof(from);
	int timeout = 1000;
	IcmpHeader *icmp_hdr;

	char icmp_data[MAX_PACKET];
	char recvbuf[MAX_PACKET];
	USHORT seq_no = 0;

	ZeroMemory( &dest, sizeof(dest));
	memcpy( &(dest.sin_addr), (void*)param, 4);
	dest.sin_family = AF_INET;
	if ( sockRaw != INVALID_SOCKET){
		closesocket( sockRaw);
		sockRaw = INVALID_SOCKET;
	}
	sockRaw = socket( AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if ( sockRaw != INVALID_SOCKET){
		iRead = setsockopt( sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
		if ( iRead != SOCKET_ERROR){
			timeout = 1000;
			iRead = setsockopt( sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
			if ( iRead != SOCKET_ERROR){
				datasize = DEF_PACKET_SIZE + sizeof(IcmpHeader);  

				icmp_hdr = (IcmpHeader*)icmp_data;
				icmp_hdr->i_type = ICMP_ECHO;
				icmp_hdr->i_code = 0;
				icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
				icmp_hdr->i_cksum = 0;
				icmp_hdr->i_seq = 0;
				memset( icmp_data+sizeof(IcmpHeader), 'E', datasize - sizeof(IcmpHeader));
				while( 1){
			//		dwPingState = 0;
					((IcmpHeader*)icmp_data)->i_cksum = 0;
					((IcmpHeader*)icmp_data)->timestamp = GetTickCount();
					
					((IcmpHeader*)icmp_data)->i_seq = seq_no;
					((IcmpHeader*)icmp_data)->i_cksum = checksum( (USHORT*)icmp_data, datasize);
					
					iWrote = sendto( sockRaw, icmp_data, datasize, 0, (struct sockaddr*)&dest, sizeof(struct sockaddr));
					if ( iWrote == SOCKET_ERROR){
						if ( WSAGetLastError() == WSAETIMEDOUT){
							dwPingTime = -1;
							continue;
						}
						dwPingState = 0x40000000 | WSAGetLastError();
						continue;
					}
					if ( iWrote < datasize)
						dwPingState = 0x04000000;//len error
					while( 1){
						iRead = recvfrom( sockRaw, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&from, &fromlen);
						if ( iRead == SOCKET_ERROR){
							if ( WSAGetLastError() == WSAETIMEDOUT){
								dwPingTime = -1;
								break;
							}
							dwPingState = 0x40000000 | WSAGetLastError();
							continue;
						}
						IpHeader *iphdr;
						unsigned short iphdrlen;
						iphdr = (IpHeader *)recvbuf;
						iphdrlen = iphdr->h_len * 4 ; // number of 32-bit words *4 = bytes
						if ( iRead < iphdrlen + ICMP_MIN)
							dwPingState = 0x08000000;//len error
						icmp_hdr = (IcmpHeader*)(recvbuf + iphdrlen);
						if ( icmp_hdr->i_type != ICMP_ECHOREPLY){
							dwPingState = 0x10000000;//non-echo type recvd
							continue;
						}
						if ( icmp_hdr->i_seq != seq_no)
							continue;
						if ( icmp_hdr->i_id != (USHORT)GetCurrentProcessId()){
							dwPingState = 0x20000000;//someone else's packet!
							continue;
						}
						dwPingTime = GetTickCount()-icmp_hdr->timestamp;
						break;
					}
					seq_no++;
					dwPingState = 0x80000000;
					Sleep( 3000);
				}
			}
		}
	}
	return 0;
}

#endif
#ifdef _ITEM_CRACKER 
extern bool m_bt;
void lssproto_IC_recv(int fd, int x, int y)
{
	m_bt = true; setCharMind( pc.ptAct, SPR_cracker);	
	m_bt = false;
}
#endif

#ifdef _CHECK_GAMESPEED
static int delaytimes=0;
void lssproto_CS_recv( int fd, int deltimes)
{
	delaytimes = deltimes;
}
int lssproto_getdelaytimes()
{
	if ( delaytimes < 0 ) delaytimes = 0; 
	return delaytimes;
}
void lssproto_setdelaytimes( int delays)
{
	delaytimes = delays; 
}
#endif

#ifdef _MAGIC_NOCAST//沉默
void lssproto_NC_recv( int fd, int flg )
{  
	if (flg==1 )
		NoCastFlag=TRUE;
    else 
		NoCastFlag=FALSE;
}
#endif
#ifdef _CHANNEL_MODIFY
// 储存对话内容
void SaveChatData(char *msg,char KindOfChannel,bool bCloseFile){
	static FILE *pFile[5];
	static char szFileName[256];
	static struct tm *nowTime;
	static time_t longTime;
	static unsigned short Channel[] = {
		PC_ETCFLAG_CHAT_MODE, //队伍频道开关
		PC_ETCFLAG_CHAT_TELL,	//密语频道开关
		PC_ETCFLAG_CHAT_FM,		//家族频道开关
		PC_ETCFLAG_CHAT_OCC,	//职业频道开关
		PC_ETCFLAG_CHAT_CHAT	//聊天室开关
	};
	char ChannelType[] = {'T','M','F','O','R'};

	//JL 重写保存chat聊天记录
	if ((pc.etcFlag & PC_ETCFLAG_CHAT_SAVE) && !bCloseFile) 
	{
		time(&longTime);
		nowTime = localtime(&longTime);

		if (pFile[0] == NULL) 
		{
			sprintf(szFileName, "chat\\%02d%02d%02d.TXT", (nowTime->tm_year % 100), nowTime->tm_mon + 1, nowTime->tm_mday);
			pFile[0] = fopen(szFileName, "a");
			int len = lstrlen(msg);
			fwrite(msg, 1, len, pFile[0]);
			fwrite("\n", 1, 1, pFile[0]);
			fflush(pFile[0]);
			__asm nop;
		}
		else 
		{
			int len = lstrlen(msg);
			fwrite(msg, 1, len, pFile[0]);
			fwrite("\n", 1, 1, pFile[0]);
			fflush(pFile[0]);
			__asm nop;
		}

	}
	else bCloseFile = 1;

	if (bCloseFile) {
		if (pFile[0] != NULL)
		{
			fclose(pFile[0]);
			pFile[0] = 0;
		}
	}

	// 储存对话内容选项开启
	//if ((pc.etcFlag & PC_ETCFLAG_CHAT_SAVE) && !bCloseFile){
	//	time(&longTime);
	//	nowTime = localtime(&longTime);
	//	for (int i=0;i<5;i++){
	//		if (pc.etcFlag & Channel[i]){
	//			if (pFile[i] == NULL){
	//				sprintf(szFileName,"chat\\%c%02d%02d%02d.TXT",ChannelType[i],(nowTime->tm_year % 100),nowTime->tm_mon+1,nowTime->tm_mday);
	//				if ((pFile[i] = fopen(szFileName,"a")) == NULL) continue;
	//				int n =fwrite(msg, lstrlen(msg), lstrlen(msg), pFile[i]);
	//				__asm nop;
	//			}
	//		}
	//		else{
	//			if (pFile[i] != NULL) fclose(pFile[i]);
	//		}
	//	}
	//	for (int i=0;i<5;i++){
	//		if (KindOfChannel == ChannelType[i]){
	//			if (pFile[i] != NULL) fprintf(pFile[i],"[%02d:%02d:%02d]%s\n",nowTime->tm_hour,nowTime->tm_min,nowTime->tm_sec,msg);
	//		}
	//	}
	//}
	//else bCloseFile = 1;

	//if (bCloseFile){
	//	for (int i=0;i<5;i++)
	//		if (pFile[i] != NULL) fclose(pFile[i]);
	//}
}
#endif

#ifdef _STREET_VENDOR
extern short sStreetVendorBtn;
extern short sStreetVendorBuyBtn;
extern void StreetVendorWndfunc(bool bReset,char *data);
extern void StreetVendorBuyWndfunc(char *data);

void lssproto_STREET_VENDOR_recv(int fd,char *data)
{
	char szMessage[32];

	getStringToken(data,'|',1,sizeof(szMessage) - 1,szMessage);
	switch (szMessage[0]){
		// 开新摆摊介面
		case 'O':
			sStreetVendorBtn = 1;
			pc.iOnStreetVendor = 1;
			break;
		// 设定摆摊内容
		case 'S':
			sStreetVendorBtn = 3;
			StreetVendorWndfunc(false,data);
			break;
		// server送来的卖方贩卖内容
		case 'B':
			sStreetVendorBuyBtn = 1;
			StreetVendorBuyWndfunc(data);
			break;
		// server 送来关闭视窗
		case 'C':
			sStreetVendorBuyBtn = 0;
			break;
		// server 送来的单笔贩卖物详细资料
		case 'D':
			StreetVendorBuyWndfunc(data);
			break;
	}
}
#endif

#ifdef _JOBDAILY
extern JOBDAILY jobdaily[MAXMISSION];
extern BOOL JobdailyGetFlag;  //是否有接收到资料
extern int JobdailyGetMax;  //是否有接收到资料
void lssproto_JOBDAILY_recv(int fd,char *data)
{
	//解读资料
	int  i=1,j=1;
	char getdata[250];
	char perdata[200];

	//StockChatBufferLine(data,FONT_PAL_RED); 

	memset(jobdaily,0,sizeof(jobdaily));
	getdata[0] = '\0';
	perdata[0] = '\0';
	while(getStringToken(data,'#',i,sizeof(getdata) - 1,getdata)!=1){
		while(getStringToken(getdata,'|',j,sizeof(perdata) - 1,perdata)!=1){
			switch (j){
			case 1: jobdaily[i-1].JobId = atoi(perdata);break;
			case 2: strcpy(jobdaily[i-1].explain,perdata); break;
			case 3: strcpy(jobdaily[i-1].state,perdata); break;
			default: StockChatBufferLine("每笔资料内参数有错误",FONT_PAL_RED); break;
			}
			perdata[0] = '\0';
			j++;
		}
		getdata[0] = '\0';
		j=1;
		i++;
	}
	if (i>1){ 
		JobdailyGetFlag = TRUE;
		JobdailyGetMax= i-2;
	}else{
		JobdailyGetFlag = FALSE;
		JobdailyGetMax = -1;
	}
}
	
#endif

#ifdef _TEACHER_SYSTEM
extern short sTeacherSystemBtn;
extern void TeacherSystemWndfunc(int flag,char *data);
void lssproto_TEACHER_SYSTEM_recv(int fd,char *data)
{
	char szMessage[16];

	getStringToken(data,'|',1,sizeof(szMessage) - 1,szMessage);
	switch (szMessage[0]){
		// 显示说明
		case 'M':sTeacherSystemBtn = 1;break;
		// 询问是否要对方当你的导师
		case 'C':
			sTeacherSystemBtn = 2;
			TeacherSystemWndfunc(0,data);
			break;
		// 超过一人,询问要找谁当导师
		case 'A':
			sTeacherSystemBtn = 3;
			TeacherSystemWndfunc(1,data);
			break;
		// 显示导师资料
		case 'V':
			sTeacherSystemBtn = 4;
			TeacherSystemWndfunc(2,data);
			break;

	}	
}
#endif

#ifdef _ADD_STATUS_2
void lssproto_S2_recv(int fd,char *data)
{
	char szMessage[16];

	getStringToken(data,'|',1,sizeof(szMessage) - 1,szMessage);

#ifdef _NEW_MANOR_LAW
	if (strcmp(szMessage,"FAME") == 0){
		getStringToken(data,'|',2,sizeof(szMessage) - 1,szMessage);
		pc.fame = atoi(szMessage);
	}
#endif


}
#endif

#ifdef _ITEM_FIREWORK
void lssproto_Firework_recv(int fd, int nCharaindex, int nType, int nActionNum)
{
	ACTION	*pAct;

	if (pc.id == nCharaindex)
		changePcAct(0, 0, 0, 51, nType, nActionNum, 0);
	else
	{
		pAct = getCharObjAct(nCharaindex);
		changeCharAct(pAct, 0, 0, 0, 51, nType, nActionNum, 0);
	
	}
}
#endif


#ifdef _NB_公告系统
void lssproto_DENGON_recv(char *data, int colors, int nums){
	extern int 公告数量;
	extern char 公告内容[512];
	extern int 公告颜色;
	extern int 公告时间;
	公告时间=0;
	sprintf(公告内容, "%s", data);
	公告颜色 = colors;
	公告数量 = nums;
}
#endif

void lssproto_MAMMOTH_OPEN_recv( char* data)
{
	Mammoth::Get().Open(data);
}
/************************/
/*	chat.c				*/
/************************/
#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "../systeminc/font.h"
#include "../caryIme.h"
#include "../systeminc/netproc.h"
#include "../systeminc/battleProc.h"
#include "../systeminc/savedata.h"
#include "time.h"
#include "../systeminc/menu.h"
#include "../systeminc/t_music.h"
#include "../systeminc/t_music.h"
#include "../systeminc/tool.h"
#include "../systeminc/lssproto_cli.h"
#include "../systeminc/netmain.h"
#include "../systeminc/chat.h"
#include "../NewProto/protocol.h"
#ifdef _TALK_WINDOW
#include "../systeminc/talkwindow.h"
#endif
#include "../systeminc/EncryptClient.h"
#include "../image.h"

extern INPUT_HISTORY InputHistory ;
STR_BUFFER SubBuffer; 



// ?????????
CHAT_BUFFER ChatBuffer[ MAX_CHAT_LINE ];

// ?????????
STR_BUFFER MyChatBuffer;

// ???????????????????
STR_BUFFER *pNowStrBuffer = NULL;

// ??????????
int NowChatLine = 0;
// ????
int NowMaxChatLine = DEF_CHAT_LINE;
// ?????
int NowMaxVoice = DEF_VOICE;
// ????????
int CursorFlashCnt = 0;
// ??????????
int ChatLineSmoothY = 0 ;

#define CAHT_HISTORY_STR_FILE_NAME 	"data\\chathis.dat" 	// ??????????????
// ???????????
CHAT_HISTORY ChatHistory;

#define MAX_SHIELD_SIZE 5000
typedef CEncryptClient<0x55, 0x168, 0xC9, 0x3C, 0x6B, 0x49, 0x81, 0x65>	ENCRYPTCONF;
char SayShieldList[MAX_SHIELD_SIZE][24];
char NameShieldList[MAX_SHIELD_SIZE][24];

// ??????? 
void KeyboardTab( void );

// ??????
FILE *chatLogFile = NULL;
char chatLogFileName[] = "CHAT_000000.TXT";
void openChatLogFile( void );

/*
#ifdef _TELLCHANNEL				// (不可开) ROG ADD 密语频道
char ReTellName[] = "";
#endif 
*/

void InitChat(void)
{
	memset(ChatBuffer, 0, sizeof(CHAT_BUFFER) * MAX_CHAT_LINE);
	memset(&MyChatBuffer, 0, sizeof(STR_BUFFER));
	MyChatBuffer.len = _FONTDATALEN_ - 10;
#ifndef _CHANNEL_MODIFY
	MyChatBuffer.fontPrio = FONT_PRIO_BACK;
#else
	MyChatBuffer.fontPrio = FONT_PRIO_CHATBUFFER;
#endif
#ifdef 聊天表情
	MyChatBuffer.x = 24 + 26;	//이모티콘 버튼 옆 채팅 시작지점
#else
	MyChatBuffer.x = 8;
#endif
	MyChatBuffer.y = 434 + DISPLACEMENT_Y;
	getUserChatOption();
	//JL fix
//	openChatLogFile();
#ifdef _CHANNEL_MODIFY
	FILE *pf;
	char buf;

	if ((pf = fopen("data\\channel.dat","r")) == NULL){
		pc.etcFlag |= PC_ETCFLAG_CHAT_TELL;
		if ((pf = fopen("data\\channel.dat", "w+")) == NULL)
			return;
		else{
			char buf[3];

			buf[0] = (pc.etcFlag & PC_ETCFLAG_CHAT_TELL) ? 1:0;
			buf[1] = (pc.etcFlag & PC_ETCFLAG_CHAT_SAVE) ? 1:0;
			buf[2] = (pc.etcFlag & PC_ETCFLAG_CHAT_OCC) ? 1:0;
			fwrite(buf, 1, sizeof(buf), pf);
			fclose(pf);
		}
	}else{
		fread(&buf,1,sizeof(char),pf);
		if (buf)
			pc.etcFlag |= PC_ETCFLAG_CHAT_TELL;
		fread(&buf,1,sizeof(char),pf);
		if (buf)
			pc.etcFlag |= PC_ETCFLAG_CHAT_SAVE;
		fread(&buf,1,sizeof(char),pf);
		if (buf)
			pc.etcFlag |= PC_ETCFLAG_CHAT_OCC;
		fclose(pf);
	}

	if (pc.familyleader <= 0) pc.etcFlag &= ~PC_ETCFLAG_CHAT_FM;
	else pc.etcFlag |= PC_ETCFLAG_CHAT_FM;

	if (pc.profession_class == 0) pc.etcFlag &= ~PC_ETCFLAG_CHAT_OCC;

	//else pc.etcFlag |= PC_ETCFLAG_CHAT_OCC;

	if (bNewServer) lssproto_FS_send(sockfd,pc.etcFlag);
	else old_lssproto_FS_send(sockfd,pc.etcFlag);
#endif
}

void openChatLogFile( void )
{
	if( chatLogFile == NULL ){
#ifdef  _STONDEBUG_
		chatLogFile = fopen( chatLogFileName, "wt" );
#else
		struct tm *nowTime;
		time_t longTime;
		time( &longTime );
		nowTime = localtime( &longTime );
		sprintf( chatLogFileName, "CHAT_%02d%02d%02d.TXT",
			(nowTime->tm_year % 100), nowTime->tm_mon+1, nowTime->tm_mday );

		chatLogFile = fopen( chatLogFileName, "a" );
		if( chatLogFile ){
			fprintf( chatLogFile, "----- Login: %02d/%02d/%02d %02d:%02d:%02d -----\n",
				(nowTime->tm_year % 100), nowTime->tm_mon+1, nowTime->tm_mday,
				nowTime->tm_hour, nowTime->tm_min, nowTime->tm_sec );
		}
#endif
	}
}

BOOL SaveChatHistoryStr( int no )
{
	FILE *fp; 
	// ????????????
	if( ( fp = fopen( CAHT_HISTORY_STR_FILE_NAME, "r+b" ) ) == NULL )
		return FALSE;
	// ?????????????????????
	fseek( fp, sizeof( ChatHistory.str[ 0 ] ) * no, SEEK_SET );
	// ?????????
	if( fwrite( &ChatHistory.str[ no ], sizeof( ChatHistory.str[ 0 ] ), 1, fp ) < 1 ){
	
		fclose( fp );// ????????
		return FALSE;
	}
	
	// ????????????????????
	fseek( fp, sizeof( ChatHistory.str[ 0 ] ) * MAX_CHAT_HISTORY, SEEK_SET );
	// ??????
	if( fwrite( &no, sizeof( int ), 1, fp ) < 1 ){
	
		fclose( fp );// ????????
		return FALSE;
	}
	
	// ????????
	fclose( fp );
	
	return TRUE;
}

// ???????????? ****************************************************************/
BOOL LoadChatHistoryStr( void )
{
	FILE *fp;

	// ??????????????
	ChatHistory.nowNo = -1;

	// ???????????
	if( ( fp = fopen( CAHT_HISTORY_STR_FILE_NAME, "rb" ) ) == NULL ){
		// ???????
		if( ( fp = fopen( CAHT_HISTORY_STR_FILE_NAME, "wb" ) ) != NULL ){
			// ???????
			fwrite( &ChatHistory, sizeof( CHAT_HISTORY ) - sizeof( int ), 1, fp );
			fclose( fp );	// ????????
		}
		
		// ?????????????
		ChatHistory.nowNo = MAX_CHAT_HISTORY - 1;
		
		return FALSE;
	}
	
	// ??????
	if( fread( &ChatHistory, sizeof( CHAT_HISTORY ) - sizeof( int ), 1, fp ) < 1 ){
		
		fclose( fp );	// ????????
		return FALSE;
	}
	// ????????
	fclose( fp );
	
	return TRUE;
}

/* ???????????? ****************************************************/
//加载屏蔽字
void* EncryptFileName( LPCTSTR pszResFile,unsigned int &nSize )
{
	ENCRYPTCONF	myConfFileCncryp;
	if( NULL != pszResFile ) ; //  [Chancy2008-4-16 21:51]

	char szFile[MAX_PATH] = "";
	char szResFileTemp[MAX_PATH] = "" ;
	lstrcpyn( szResFileTemp , pszResFile , sizeof( szResFileTemp ) ) ;

	char* pszReadName = strlwr(szResFileTemp); //  [Chancy2008-4-16 21:52]

	if ( !pszReadName )
		return NULL;

	sprintf(szFile,"data/%s.bin",pszReadName);

	FILE* fp = fopen(szFile, "rb");

	if ( !fp )
	{
		return false;
	}

	(myConfFileCncryp).Init();

	fseek(fp,0,SEEK_END);//move file pointer to file end
	unsigned int unFileSize =  ftell( fp );
	nSize = ftell( fp );
	fseek( fp,0,SEEK_SET );// RESTORE

	void* pBuffer = new char[ unFileSize + 1 ]; // apply 1 char memory for '\0' because char pointer

	if( !pBuffer )
		return false;
	memset( pBuffer,0,sizeof( char ) * ( unFileSize + 1 ) );
	fread( pBuffer,unFileSize,1,fp );
	(myConfFileCncryp).Encrypt((unsigned char *)pBuffer,unFileSize);
	fclose(fp);

	return pBuffer;
}

BOOL MoveFilePointInt( const char * pBuffer ,unsigned int nSize , unsigned int &nRead, BOOL &bRead ,int nResult) // 移动文件指针 [11/27/2007]
{
	unsigned int nReadCount = nRead;	// 记录上次指针的位置 [11/28/2007]
	const char* pFind = strstr( pBuffer + nRead,"\n");//find \n
	if( pFind )
	{
		nRead = ( pFind - pBuffer + 1 );	// 得到当前指针的位置 [11/28/2007]  //得到相对与文件开始位置的偏移位置// 最先的注释不准确 [杨文鸽 2007-12-14]
	}
	if ((2 == nRead - nReadCount && nResult !=0)|| nResult == 0)	// /	判断读到真实数据（2个字节表示“\r\n”）  [11/28/2007]
	{
		bRead = FALSE;
	}
	else
	{
		bRead = TRUE;
	}
	if( nRead > nSize )
		return false;
	return true;
}

BOOL LoadReadSayShield( void )
{
	char szName[]="ShieldWorld";
	unsigned int nSize;
	char* pBuffer =	(char*)EncryptFileName((char*)szName,nSize);
	if( !pBuffer )
		return FALSE;

	unsigned int nRead = 0;		//已经读了的
	BOOL bRead = FALSE;
	int i = 0;
	static char szWord[20] = "";
	while(nRead < nSize)
	{
			int nResult = sscanf(pBuffer + nRead , "%s", &szWord);
			if ( !MoveFilePointInt(pBuffer,nSize,nRead,bRead,nResult))
				break;
			if ( bRead == FALSE )
				continue;
			if ( 1 == nResult )
			{	
				if(i >= MAX_SHIELD_SIZE)
				{
					break;
				}
				strcpy(SayShieldList[i], szWord );
				i++;
			}
			else
				break;
	}
	delete[] pBuffer;
	pBuffer = NULL;
	return TRUE;
}

BOOL LoadReadNameShield( void )
{
	char szName[]="ShieldName";
	unsigned int nSize;
	char* pBuffer =	(char*)EncryptFileName((char*)szName,nSize);
	if( !pBuffer )
		return FALSE;

	unsigned int nRead = 0;		//已经读了的
	BOOL bRead = FALSE;
	int i = 0;
	static char szName_[20] = "";

	while(nRead < nSize)
	{
		
		int nResult = sscanf(pBuffer + nRead , "%s", &szName_);
		if ( !MoveFilePointInt(pBuffer,nSize,nRead,bRead,nResult))
			break;
		if ( bRead == FALSE )
			continue;
		if ( 1 == nResult )
		{	
			if(i >= MAX_SHIELD_SIZE)
			{
				break;
			}
			strcpy(NameShieldList[i], szName_ );
			i++;
		}
		else
			break;
	}
	delete[] pBuffer;
	pBuffer = NULL;
	return TRUE;

}
bool CheckSay( const char* strSay, const char szReplace )
{
	if ( NULL==strSay ) return true;
	if ( strlen(strSay)==0 ) return true;

	for ( size_t i=0;i<MAX_SHIELD_SIZE;i++ )
	{
		if ( const char *szRe = strstr( strSay,SayShieldList[i] ) )
		{
			//	g_objGameMsg.AddMsg( SayShieldList[i].c_str() );             // 测试时显示出来 
			//memset( (void*)szRe,szReplace,strlen(SayShieldList[i]) );
			return false;
		}
	}
	return true;
}

bool CheckName( const char* strSay )
{
	if ( NULL==strSay ) return true;
	if ( strlen(strSay)==0 ) return true;

	for ( size_t i=0;i<MAX_SHIELD_SIZE;i++ )
	{
		if ( const char *szRe = strstr( strSay,NameShieldList[i] ) )
		{
			//	g_objGameMsg.AddMsg( SayShieldList[i].c_str() );             // 测试时显示出来 
			//	memset( (void*)szRe,szReplace,NameShieldList[i].length() );
			//word = NameShieldList[i];
			return false;
		}
	}
	return true;
}

#ifdef _NB_CANCEL_FANTI
extern int 编码;
extern int 繁体开关;
#endif
void StrToNowStrBuffer( char *str ){
#ifdef _NB_CANCEL_FANTI
	if(编码 == 950 ){
		extern char* BIG5ToGB2312(const char* szBIG5String);
		char *newstr;
		newstr=BIG5ToGB2312((const char *)str);
		int strLen,i;
		strLen=strlen(newstr);
		if(strLen>86)
			strLen=86;
		for(i=0;i<strLen;i++){
			if(IsDBCSLeadByte(newstr[i])){
				StockStrBufferDBChar(newstr+i);
				i++;
			}else
				StockStrBufferChar(newstr[i]);
		}
	}else{
		int strLen,i;
		strLen=strlen(str);
		if(strLen>86)
			strLen=86;
		for(i=0;i<strLen;i++){
			if(IsDBCSLeadByte(str[i])){
				StockStrBufferDBChar(str+i);
				i++;
			}else
				StockStrBufferChar(str[i]);
		}
	}
#else
	int strLen,i;
	strLen=strlen(str);
	/*if(strLen>=256)
		strLen=255;*/
	if(strLen>86)
		strLen=86;
	for(i=0;i<strLen;i++){
		if(IsDBCSLeadByte(str[i])){
			StockStrBufferDBChar(str+i);
			i++;
		}else
			StockStrBufferChar(str[i]);
	}
#endif
}

#ifdef _NB_CANCEL_FANTI
void StrToNowStrBuffer1( char *str ){
	if(繁体开关){
		int strLen,i;
		char 繁体[1024]={0};
		LCMapString (0x804,0x4000000,str, strlen(str),繁体,1024);
		strLen=strlen(繁体);
		if(strLen>86)
			strLen=86;
		for(i=0;i<strLen;i++){
			if(IsDBCSLeadByte(繁体[i])){
				StockStrBufferDBChar(繁体+i);
				i++;
			}else
				StockStrBufferChar(繁体[i]);
		}
	}else{
		int strLen,i;
		strLen=strlen(str);
		if(strLen>86)
			strLen=86;
		for(i=0;i<strLen;i++){
			if(IsDBCSLeadByte(str[i])){
				StockStrBufferDBChar(str+i);
				i++;
			}else
				StockStrBufferChar(str[i]);
		}
	}
}
#endif

int StrToNowStrBuffer2( char *str )
{
	int strLen,i;
	strLen=strlen(str);
	if(strLen>70){
		if(IsDBCSLeadByte(str[68]))
			strLen = 69;
		else
			strLen = 70;
	}
	for(i=0;i<strLen;i++){
		if(IsDBCSLeadByte(str[i])){
			StockStrBufferDBChar(str+i);
			i++;
		}else
			StockStrBufferChar(str[i]);
	}
	return i;
}

/* ???????? ************************************************************/
void ChatProc( void )
{
	// ???????????
	//if( joy_trg[ 1 ] & JOY_RETURN ) KeyboardReturn();
	
	// ??????????
	if (joy_trg[1] & JOY_TAB)
	{
		KeyboardTab();
	}
	
	// ??????
	//if( joy_trg[ 0 ] & JOY_CTRL_C ){
		// ???????????
	//	SetClipboad();
	//}
	// ??????
	if( joy_trg[ 0 ] & JOY_CTRL_V ){
		// ??????????????????
		GetClipboad();
	}


	
#ifdef _TELLCHANNEL				//ROG ADD 密语频道
	if( joy_trg[ 1 ] & JOY_CTRL_R )
	{
		pNowStrBuffer->buffer[ 0 ] = NULL;
		pNowStrBuffer->cursor=0;
		pNowStrBuffer->cnt = 0;
		StrToNowStrBuffer(secretName);
		TalkMode = 1;
	}
#else
#ifdef _FRIENDCHANNEL
	StrToNowStrBuffer( secretName );
#endif
#endif	
	
	// ??????????????????
	if( pNowStrBuffer == &MyChatBuffer || pNowStrBuffer == &MailStr ){
	
		// ???????????????????????
		if(	joy_trg[ 1 ] & JOY_F1 ) StrToNowStrBuffer( chatRegistryStr[ 0 ].buffer );
		if( joy_trg[ 1 ] & JOY_F2 ) StrToNowStrBuffer( chatRegistryStr[ 1 ].buffer );
		if( joy_trg[ 1 ] & JOY_F3 ) StrToNowStrBuffer( chatRegistryStr[ 2 ].buffer );
		if( joy_trg[ 1 ] & JOY_F4 ) StrToNowStrBuffer( chatRegistryStr[ 3 ].buffer );
	
		if( joy_trg[ 1 ] & JOY_F5 ) StrToNowStrBuffer( chatRegistryStr[ 4 ].buffer );
		if( joy_trg[ 1 ] & JOY_F6 ) StrToNowStrBuffer( chatRegistryStr[ 5 ].buffer );
		if( joy_trg[ 1 ] & JOY_F7 ) StrToNowStrBuffer( chatRegistryStr[ 6 ].buffer );
		if( joy_trg[ 1 ] & JOY_F8 ) StrToNowStrBuffer( chatRegistryStr[ 7 ].buffer );
	}

	// ??????????????????????????
	if( pNowStrBuffer == &MyChatBuffer && GetImeString() == NULL ){
		// ???????
		if( joy_auto[ 0 ] & JOY_UP ){
			int bak = ChatHistory.nowNo;	// ??????
			// ??????????????
			if( ChatHistory.nowNo == -1 ) ChatHistory.nowNo = ChatHistory.newNo;
			else ChatHistory.nowNo--;
			// ????????
			if( ChatHistory.nowNo < 0 ) ChatHistory.nowNo = MAX_CHAT_HISTORY - 1;
			// ????????????( ??????????????????????????? ?
			if( ChatHistory.str[ ChatHistory.nowNo ][ 0 ] != 0 && ( ChatHistory.nowNo != ChatHistory.newNo || bak == -1 ) ){
				// ??????????
				pNowStrBuffer->cnt = 0;
				pNowStrBuffer->buffer[ 0 ] = NULL;
				pNowStrBuffer->cursor=0;
				// ????????????????
				StrToNowStrBuffer( ChatHistory.str[ ChatHistory.nowNo ] );
			}else{
				// ??????????
				ChatHistory.nowNo = bak;
			}
#ifdef _TALK_WINDOW
			if(g_bTalkWindow) TalkWindow.Update();
#endif
		}else
		// ????????
		if( joy_auto[ 0 ] & JOY_DOWN ){
			// ??????????????
			if( ChatHistory.nowNo != -1 ){
				// ????????????
				if( ChatHistory.nowNo == ChatHistory.newNo ){ 
					ChatHistory.nowNo = -1;
					// ??????????
					pNowStrBuffer->cnt = 0;
					pNowStrBuffer->buffer[ 0 ] = NULL;
					pNowStrBuffer->cursor=0;
				}else{
					ChatHistory.nowNo++;
					// ????????
					if( ChatHistory.nowNo >= MAX_CHAT_HISTORY ) ChatHistory.nowNo = 0;
					// ??????????
					pNowStrBuffer->cnt = 0;
					pNowStrBuffer->buffer[ 0 ] = NULL;
					pNowStrBuffer->cursor=0;
					// ????????????????
					StrToNowStrBuffer( ChatHistory.str[ ChatHistory.nowNo ] );
				}
#ifdef _TALK_WINDOW
				if(g_bTalkWindow) TalkWindow.Update();
#endif
			}
		}
	}
/*#ifdef _ON_OFF_ITEM
	EXP_ON_OFF_ITEM();
#endif
#ifdef _ON_OFF_NOENEMY
	NOENEMY_OF_OFF_ITEM();
#endif
#ifdef _CLICK_PARTYAUTO
	CLICK_PARTYAUTO();
#endif*/
}

// ??????????? **************************************************/
void FlashKeyboardCursor( void )
{
	if(pNowStrBuffer==NULL) return;
	if(CursorFlashCnt >= 20){
		StockFontBuffer(pNowStrBuffer->imeX, pNowStrBuffer->imeY , pNowStrBuffer->fontPrio, pNowStrBuffer->color, "_" , 0 );
	}
	CursorFlashCnt++;
	if( CursorFlashCnt >= 40 ) CursorFlashCnt = 0;
}

// ?????? ***************************************************************/
void KeyboardBackSpace( void )
{
	int byte,cursor;
	if(pNowStrBuffer==NULL) return;

	if((cursor=pNowStrBuffer->cursor) > 0){
		char *lpstr=pNowStrBuffer->buffer;
		char *lpstr1=lpstr+cursor;
		byte=lpstr1-GetCharPrev(lpstr,lpstr1);
		for(;cursor<=pNowStrBuffer->cnt;cursor++){
			lpstr[cursor-byte]=lpstr[cursor];
		}
		pNowStrBuffer->cnt-=byte;
		pNowStrBuffer->cursor-=byte;
		CursorFlashCnt=20;
	}
}	

// ??????? ***************************************************************/
void KeyboardTab( void )
{
	int i,flag = 0;
	
	// ????????
	if( pNowStrBuffer == NULL ) return;
	
	// ?????? *******************************
	for( i = 0 ; i < MAX_CHAT_REGISTY_STR ; i++ ){
		if( pNowStrBuffer == &chatRegistryStr[ i ] ){ 
			flag = TRUE;
			break;
		}
	}
	// ???? *******************************
	if( flag == TRUE ){
		// ???????????
		if( joy_con[ 1 ] & JOY_RSHIFT || joy_con[ 1 ] & JOY_LSHIFT ){
			i--;
			// ????????
			if( i < 0 ) i = MAX_CHAT_REGISTY_STR - 1;
		}else{
			i++;
			// ????????
			if( i >= MAX_CHAT_REGISTY_STR ) i = 0;
		}
		// ???????
		GetKeyInputFocus( &chatRegistryStr[ i ] );
	}
	
	// ???????????????
	CursorFlashCnt = 20;
	static DWORD dwChannelChangeTime = TimeGetTime();
	if(dwChannelChangeTime + 250 < TimeGetTime()){
		dwChannelChangeTime = TimeGetTime();


#ifndef _CHATWORLDWIDEPROTOCOL
		TalkMode = (TalkMode + 1) % 6;
#else
#ifdef _NB_CHANNEL_ALLSERV
		TalkMode = (TalkMode + 1) % 8;
#else
		TalkMode = (TalkMode + 1) % 7;
#endif
#endif
		switch(TalkMode){
		case 0:
			strcpy(secretName,"");
			// 已在队伍频道切回一般频道
			if(pc.etcFlag & PC_ETCFLAG_CHAT_MODE){
				pc.etcFlag &= ~PC_ETCFLAG_CHAT_MODE;
				if(bNewServer) lssproto_FS_send(sockfd,pc.etcFlag);
				else old_lssproto_FS_send(sockfd,pc.etcFlag);
			}
			break;
		case 1:
			// 密语频道关闭,跳到下一个频道
			if(!(pc.etcFlag & PC_ETCFLAG_CHAT_TELL)) TalkMode++;
			else break;
		case 2:
			// 无队伍跳到下一个频道
			if(partyModeFlag == 0) TalkMode++;
			else{
				pc.etcFlag |= PC_ETCFLAG_CHAT_MODE;
				if(bNewServer) lssproto_FS_send(sockfd,pc.etcFlag);
				else old_lssproto_FS_send(sockfd,pc.etcFlag);
				break;
			}
		case 3:
			// 若队频有开,关掉队频
			if(pc.etcFlag & PC_ETCFLAG_CHAT_MODE){
				pc.etcFlag &= ~PC_ETCFLAG_CHAT_MODE;
				if(bNewServer) lssproto_FS_send(sockfd,pc.etcFlag);
				else old_lssproto_FS_send(sockfd,pc.etcFlag);
			}
			// 家族频道关闭,跳到下一个频道
			if(!(pc.etcFlag & PC_ETCFLAG_CHAT_FM)) TalkMode++;
			else break;
		case 4:
			// 职业频道关闭,跳到下一个频道
			if(!(pc.etcFlag & PC_ETCFLAG_CHAT_OCC))
				TalkMode++;
			else break;
#ifdef _CHATROOMPROTOCOL
		case 5:
			// 聊天室频道
			if(!(pc.etcFlag & PC_ETCFLAG_CHAT_CHAT)){
				secretFlag = FALSE;
				selChar = -1;
#ifdef _CHATWORLDWIDEPROTOCOL
				TalkMode++;
#else
				TalkMode = 0;	
#endif
				strcpy(secretName,"");
			}
			break;
#endif
#ifdef _CHATWORLDWIDEPROTOCOL
		//CC 世界频道
		case 6:
#ifdef _NB_CHANNEL_ALLSERV
				TalkMode++;
#else
				TalkMode = 0;	
#endif
			break;
#endif
#ifdef _NB_CHANNEL_ALLSERV
		case 7:
			//TalkMode = 0;
			break;
		}
#ifdef _TALK_WINDOW
		if(g_bTalkWindow) TalkWindow.Update();
#endif
	}
#else
	#ifdef _FRIENDCHANNEL
		TalkMode = (TalkMode + 1) % 3;
		if(TalkMode == 2 ){
			secretFlag = FALSE;
			selChar = -1;
			pNowStrBuffer->buffer[ 0 ] = NULL;
			pNowStrBuffer->cursor=0;
			pNowStrBuffer->cnt = 0;
			StrToNowStrBuffer("");
			if(strcmp(pc.chatRoomNum, "") == 0)	TalkMode = 0;
		}else	strcpy(secretName,"");
	#else
		#ifdef _TELLCHANNEL
			TalkMode = (TalkMode + 1) % 2;
			pNowStrBuffer->buffer[ 0 ] = NULL;
			pNowStrBuffer->cursor=0;
			pNowStrBuffer->cnt = 0;
			StrToNowStrBuffer("");
		#endif
	#endif
#endif
}	

void KeyboardLeft()
{
	int byte,cursor;
	if(pNowStrBuffer==NULL) return;
	if((cursor=pNowStrBuffer->cursor) > 0){
		char *lpstr=pNowStrBuffer->buffer;
		char *lpstr1=lpstr+cursor;
		byte=lpstr1-GetCharPrev(lpstr,lpstr1);
		pNowStrBuffer->cursor-=byte;
		CursorFlashCnt=20;
	}
}

void KeyboardRight()
{
	int byte=1,cursor;
	if(pNowStrBuffer==NULL) return;
	if((cursor=pNowStrBuffer->cursor) < (pNowStrBuffer->cnt)){
		char *lpstr=pNowStrBuffer->buffer;
		lpstr+=cursor;
		if(*lpstr && IsDBCSLeadByte(*lpstr))
			byte=2;
		pNowStrBuffer->cursor+=byte;
	}
	CursorFlashCnt=20;
}

#include <tlhelp32.h>
/* ???????? ************************************************************/
void KeyboardReturn( void )
{
	//ttom
	static bool first_keydown=true;
	if(!first_keydown) {
		//cary 十七
		static DWORD PreTime=TimeGetTime(),CurTime;
		if(((CurTime=TimeGetTime())-PreTime)<500)
		return;
		PreTime=CurTime;
	}

	first_keydown=false;
	//end
	//ttom
	char bakNo;
	// ?????????????????
	if( GetImeString() != NULL )
		return;
	// ??????? *******************************
	if( pNowStrBuffer == &MyChatBuffer ){
		if( pNowStrBuffer->cnt == 0 )
			return;
		pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = '\0';
#ifdef  _STONDEBUG_
		// ????????
		if( strstr( pNowStrBuffer->buffer, "[battlein]" ) )
			EncountFlag = TRUE;
		if( strstr( pNowStrBuffer->buffer, "[battleout]" ) ){
			ChangeProc( PROC_GAME, 1 );
			DeathAllAction();	// ????????
		}
	
		if( strstr( pNowStrBuffer->buffer, "[cary encountoff]" ) ){
			EncountOffFlag = TRUE;
			pNowStrBuffer->cnt = 0;
			pNowStrBuffer->cursor=0;
			*( pNowStrBuffer->buffer )= '\0';
			return;
		}
		if( strstr( pNowStrBuffer->buffer, "[cary encounton]" ) ){
			EncountOffFlag = FALSE;
			pNowStrBuffer->cnt = 0;
			pNowStrBuffer->cursor=0;
			*( pNowStrBuffer->buffer )= '\0';
			return;
		}
#endif
		// ??????
		bakNo = ChatHistory.newNo;
		// ????????
		ChatHistory.newNo++;
		// ????????
		if( ChatHistory.newNo >= MAX_CHAT_HISTORY )
			ChatHistory.newNo = 0;
//cary
		BOOL bSave=TRUE;
		/*
		BOOL bH=FALSE;
		for(int i=0;i<pNowStrBuffer->cnt;i++){
			if(!bH){
				if('['==pNowStrBuffer->buffer[i])
					bH=TRUE;
			}else{
			}
		}
		*/
		/*
		for(int i=0;i<pNowStrBuffer->cnt;i++){
			if('['==pNowStrBuffer->buffer[i]){
				if(strstr( pNowStrBuffer->buffer+i, "debug on]" ) != 0){
					bSave=FALSE;
					break;
				}
			}
		}
		*/
		if(('['==pNowStrBuffer->buffer[0])&&(']'==pNowStrBuffer->buffer[pNowStrBuffer->cnt-1]))
			bSave=FALSE;
		if(!bSave){
			ChatHistory.newNo = bakNo;
		}else{
//end
			// ????????
			if( strcmp( pNowStrBuffer->buffer, ChatHistory.str[ bakNo ] ) != 0 ){
				// ?????????
				strcpy( ChatHistory.str[ ChatHistory.newNo ], pNowStrBuffer->buffer );
				// ?????????
				SaveChatHistoryStr( ChatHistory.newNo );
			}else{
				// ??????????
				ChatHistory.newNo = bakNo;
			}
//cary
		}
//end
		// ?????????
		ChatHistory.nowNo = -1;
#ifdef  _STONDEBUG_
		if( offlineFlag == FALSE )
#endif

		/*if(!CheckSay(pNowStrBuffer->buffer))
		{
			#ifdef _FONT_SIZE
						StockChatBufferLineExt( "您输入内容有不允许字符", MyChatBuffer.color, 0 );
			#else
						StockChatBufferLine( "输入内容有不允许字符", MyChatBuffer.color );
			#endif

			return;
		}*/
		chatStrSendForServer( pNowStrBuffer->buffer, MyChatBuffer.color );
		pNowStrBuffer->cnt = 0;
		pNowStrBuffer->cursor=0;
		*( pNowStrBuffer->buffer )= '\0';
	}else
	
	// ???? **********************************
	if( pNowStrBuffer == &shougouChange ){
		// ??????
		DeathAction( pActMenuWnd3 );
		pActMenuWnd3 = NULL;
		// ????????
		GetKeyInputFocus( &MyChatBuffer );
		// ????????
		play_se( 203, 320, 240 );
		// ??
		if( bNewServer)
			lssproto_FT_send( sockfd, shougouChange.buffer ) ; /* ../doc/lssproto.html line 1792 */
		else
			old_lssproto_FT_send( sockfd, shougouChange.buffer ) ; /* ../doc/lssproto.html line 1792 */
	}else
	
	// ??? **********************************
	if( pNowStrBuffer == &petNameChange ){
		// ??????
		DeathAction( pActMenuWnd3 );
		pActMenuWnd3 = NULL;
		// ????????
		GetKeyInputFocus( &MyChatBuffer );
		// ????????
		play_se( 203, 320, 240 );
		// ??
		if( bNewServer)
			lssproto_KN_send( sockfd, petStatusNo, petNameChange.buffer ) ; /* ../doc/lssproto.html line 1792 */
		else
			old_lssproto_KN_send( sockfd, petStatusNo, petNameChange.buffer ) ; /* ../doc/lssproto.html line 1792 */
	}else
	
	// ????? **********************************
	if( pNowStrBuffer == &MailStr
#ifdef __EDEN_AUCTION
		|| pNowStrBuffer == &AuctionStr){
#else
		){
#endif

		char *buffer=pNowStrBuffer->buffer;
		//尚可放入新行时
		if(pNowStrBuffer->cnt < pNowStrBuffer->len-pNowStrBuffer->lineLen){
			//放入一行全部为space的新行
			if((pNowStrBuffer->cursor)%pNowStrBuffer->lineLen == 0)
				StockStrBufferChar(' ');
			while((pNowStrBuffer->cursor)%pNowStrBuffer->lineLen)
				StockStrBufferChar(' ');
		}
	}else
	
	// ????? **********************************
	if( pNowStrBuffer == &MailStr ){
		// ????????????
		if( MailStr.cnt < MailStr.len - MailStr.lineLen ){
			// ???
			if( MailStr.cnt % MailStr.lineLen == 0 )
				StockStrBufferChar( ' ' );
			// ???????????
			while( MailStr.cnt % MailStr.lineLen )
				StockStrBufferChar( ' ' );
		}
	}

	// ??????? ****************************
#ifdef _FRIENDCHANNEL       //聊天室命名
	char temp[STR_BUFFER_SIZE];
	if( pNowStrBuffer == &chatRoomName ){
		GetKeyInputFocus( &MyChatBuffer );
		play_se( 203, 320, 240 );
		sprintf(temp,"C|%s",chatRoomName.buffer);
		lssproto_CHATROOM_send ( sockfd , temp ) ; 
	 	DeathAction(pSetRoomWnd);
		pSetRoomWnd = NULL;
		setRoomFlag = FALSE;
	}
#endif

#ifdef _TELLCHANNEL
	if(TalkMode == 1){               //聊天密语显示人名
		pNowStrBuffer->buffer[ 0 ] = NULL;
		pNowStrBuffer->cursor=0;
		pNowStrBuffer->cnt = 0;
		StrToNowStrBuffer(secretName);
	}
#endif

	extern STR_BUFFER *idPasswordStr;
	extern short idKeyReturn;
	if( pNowStrBuffer == idPasswordStr )
	{
		idKeyReturn = 1;
	}
	
	// ???????????????
	CursorFlashCnt = 20;
	
}
extern STR_BUFFER idKey;
extern STR_BUFFER passwd;
extern STR_BUFFER selCharName;
/*	将单一字元储放至目前的输入String buffer
parameter:	c:	字元					*/
void StockStrBufferChar(char c)
{
	if(BYTE(c)>0x1f){
		int cnt,cursor;
		if(pNowStrBuffer==NULL || (cnt=pNowStrBuffer->cnt) >= pNowStrBuffer->len)
			return;
		char *buffer=pNowStrBuffer->buffer;
 		if(pNowStrBuffer==&idKey || pNowStrBuffer==&passwd){
			/*if(!(('0'<=c && c<='9') || ('A'<=c && c<='Z') || ('a'<=c && c<='z')))
				return;*/
		}else if(pNowStrBuffer==&selCharName){
			if(' '==c || '　'==c|| ','==c || '|'==c || '\\'==c)
				return;
		}else if(pNowStrBuffer==&SubBuffer){
			if('&'==c || '|'==c )
				return;
		}
		for(cursor=pNowStrBuffer->cursor++;cursor<=cnt;cnt--)
			buffer[cnt+1]=buffer[cnt];
		buffer[cursor++]=c;
		pNowStrBuffer->cnt++;
		CursorFlashCnt=20;
#ifdef _TALK_WINDOW
		if(g_bTalkWindow) TalkWindow.Update();
#endif
	}
}

/*	将双位元字元储放至目前的输入String buffer
parameter:	lpc:	双位元的字元			*/
void StockStrBufferDBChar(char *lpc)
{
	int cnt,cursor;
	if(pNowStrBuffer==NULL || (cnt=pNowStrBuffer->cnt) >= pNowStrBuffer->len-1)
		return;
	char *buffer=pNowStrBuffer->buffer;
	if(pNowStrBuffer==&idKey || pNowStrBuffer==&passwd)
		return;
	else{
		for(cursor=pNowStrBuffer->cursor;cursor<=cnt;cnt--)
			buffer[cnt+2]=buffer[cnt];
		buffer[cursor++]=*lpc++;
		buffer[cursor++]=*lpc;
		pNowStrBuffer->cnt+=2;
		pNowStrBuffer->cursor+=2;
		CursorFlashCnt=20;
	}
}


#ifdef _NB_SA_LIAOTIAN
int NowChatLine_Bak = 0;
#endif

#ifdef _FONT_SIZE
void StockChatBufferLine(char *str, unsigned char color,int id)
{
	StockChatBufferLineExt(str, color, 0,id);
}
void StockChatBufferLineExt(char *str_, unsigned char color, int fontsize,int id)
#else
void StockChatBufferLine(char *str_, unsigned char color)
#endif
{
	char *str = str_;
	extern int 编码;
	extern int 繁体开关;
	if (繁体开关) {
		char 繁体[1024] = { 0 };
		LCMapString(0x804, 0x4000000, str_, strlen(str_), 繁体, 1024);
		str = 繁体;
	}else {
		str = str_;
	}
#ifdef _NB_SA_LIAOTIAN
	if (NowChatLine_Bak != NowChatLine) NowChatLine = NowChatLine_Bak;
#endif
#ifdef 聊天表情
	char strtemp[1024];
	delFontBuffer(&ChatBuffer[NowChatLine]);
	int splitPoint = 0;
	char splitStr[STR_BUFFER_SIZE + 1];
	//648*480  90
	//800*600  112
	//1024*768 144
	unsigned int MyChatBufferLen = _FONTDATALEN_;

	switch (ResoMode)
	{
	case 分辨率::mode1024:
		MyChatBufferLen = 144;
		break;
	case  分辨率::mode800:
		MyChatBufferLen = 112;
		break;
	default:
		MyChatBufferLen = 86; //640*480 原来可能87?
		break;
	}

	//TODO 分割{}重新计算分割行
	char* 说话分行(char* str, int curLen, int maxLen);
	char* pTemp = 说话分行(str,0, MyChatBufferLen);
	if (pTemp){
		splitPoint = pTemp - str;
		strncpy_s(splitStr, str, splitPoint);
		if (GetStrLastByte(splitStr) == 3){
			splitPoint -= 1;
		}
		strncpy_s(strtemp, str, splitPoint);
		strtemp[splitPoint] = 0;
		NewStockFontBuffer(&ChatBuffer[NowChatLine], 0, color, strtemp, fontsize, id);
		__asm nop;
	}else{
		NewStockFontBuffer(&ChatBuffer[NowChatLine], 0, color, str, fontsize, id);
	}

	//if (strlen(str) > MyChatBufferLen) {
	//	strncpy_s(splitStr, str, MyChatBufferLen);
	//	*(splitStr + MyChatBufferLen) = NULL;
	//	if (GetStrLastByte(splitStr) == 3) {
	//		splitPoint = MyChatBufferLen - 1;
	//		*(splitStr + MyChatBufferLen - 1) = NULL;
	//	}
	//	else
	//		splitPoint = MyChatBufferLen;
	//	strncpy_s(strtemp, str, splitPoint);
	//	strtemp[splitPoint] = 0;
	//	NewStockFontBuffer(&ChatBuffer[NowChatLine], 0, color, strtemp, fontsize,id);
	//}
	//else NewStockFontBuffer(&ChatBuffer[NowChatLine], 0, color, str, fontsize,id);
#ifdef _NB_SA_LIAOTIAN
	NowChatLine_Bak++;
#endif
	NowChatLine++;
	if (NowChatLine >= MAX_CHAT_LINE) {
		NowChatLine = 0;
#ifdef _NB_SA_LIAOTIAN
		NowChatLine_Bak = 0;
#endif
	}
	if (chatLogFile) {
		fprintf(chatLogFile, "%s\n", str);
}
	ChatLineSmoothY = 20;
	if (splitPoint != 0) {
#ifdef _FONT_SIZE
		StockChatBufferLineExt(str + splitPoint, color, fontsize,id);
#else
		StockChatBufferLine(str + splitPoint, color);
#endif
	}
#else

	int splitPoint = 0;
	char splitStr[STR_BUFFER_SIZE + 1];
#ifdef _NEWFONT_
	unsigned int MyChatBufferLen = 87;
#else
	unsigned int MyChatBufferLen = 112;
#endif
#ifdef _FONT_SIZE
	if (fontsize > 0) {
		MyChatBufferLen = (int)(MyChatBufferLen*((float)FONT_SIZE / (float)fontsize));
	}
#endif

	if (strlen(str) > MyChatBufferLen) {
		strncpy_s(splitStr, str, MyChatBufferLen);
		*(splitStr + MyChatBufferLen) = NULL;
		if (GetStrLastByte(splitStr) == 3) {
			splitPoint = MyChatBufferLen - 1;
			*(splitStr + MyChatBufferLen - 1) = NULL;
		}
		else
			splitPoint = MyChatBufferLen;
		strncpy_s(ChatBuffer[NowChatLine].buffer, str, splitPoint);

		*(ChatBuffer[NowChatLine].buffer + splitPoint) = NULL;
#ifdef _TALK_WINDOW
		TalkWindow.AddString(ChatBuffer[NowChatLine].buffer, color);
#endif	
	}
	else {
		strcpy(ChatBuffer[NowChatLine].buffer, str);
#ifdef _TALK_WINDOW
		TalkWindow.AddString(str, color);
#endif	
	}
	if (chatLogFile) {
		fprintf(chatLogFile, "%s\n", ChatBuffer[NowChatLine].buffer);
	}
	ChatBuffer[NowChatLine].color = color;
#ifdef _FONT_SIZE
	ChatBuffer[NowChatLine].fontsize = fontsize;
#endif
	NowChatLine++;
	if (NowChatLine >= MAX_CHAT_LINE) NowChatLine = 0;
	if (splitPoint != 0) {
#ifdef _FONT_SIZE
		StockChatBufferLineExt(str + splitPoint, color, fontsize);
#else
		StockChatBufferLine(str + splitPoint, color);
#endif
	}

	ChatLineSmoothY = 20;
	//ChatLineSmoothY = 20 - ChatBuffer[ NowChatLine-1 ].fontsize;

#ifdef _TALK_WINDOW
	if (g_bTalkWindow) TalkWindow.Update();
#endif
#endif
}

void ClearChatBuffer( void ){
	int i;
	for( i = 0 ; i < MAX_CHAT_LINE ; i++ ){
		ChatBuffer[ i ].buffer[ 0 ] = '\0';
	}
#ifdef _TALK_WINDOW
	NowChatLine = 0;
#endif
#ifdef _NB_SA_LIAOTIAN
	NowChatLine_Bak=0;
#endif
}


void ChatBufferToFontBuffer( void )
{
#ifdef _TALK_WINDOW
	if(g_bTalkWindow) return;
#endif
	#define CHAT_ZOUBUN_Y 22   //jl 说话显示列间距,原来为20，因加入表情，稍微加大点
	int i, j, k = 0;
	int x = 8;// ?
	int y = 400;
//	int y = DEF_APPSIZEY - 80;
	j = NowChatLine - 1;
	if( j < 0 )
		j = MAX_CHAT_LINE - 1;
	if( ChatLineSmoothY > 0 )
		k = NowMaxChatLine + 1;
	else
		k = NowMaxChatLine;
	if( k > MAX_CHAT_LINE )
		k = MAX_CHAT_LINE;
	for( i = 0 ; i < k; i++ ){
		if( *ChatBuffer[ j ].buffer != NULL ){
#ifdef _FONT_SIZE
#ifdef 聊天表情
			CHAT_BUFFER * temp = &ChatBuffer[j];
			//printf("\n temp=%s",temp);
			while (temp) {
				if (temp->x>760) break;
				if (temp->image){
					temp->fontID = StockFontBufferExt(/*40+*/temp->x + x, y + ChatLineSmoothY + DISPLACEMENT_Y, FONT_PRIO_BACK, temp->color, (char *)temp->buffer, 2, temp->fontsize);
				}else if (temp->BmpNo) {
					StockDispBuffer(/*40+*/temp->x + x + 14, y + ChatLineSmoothY + DISPLACEMENT_Y + 7, DISP_PRIO_IME1, temp->BmpNo, 0);
				}else {
					StockFontBufferExt(/*40+*/temp->x + x, y + ChatLineSmoothY + DISPLACEMENT_Y, FONT_PRIO_BACK, temp->color, (char *)temp->buffer, 0, temp->fontsize);
				}
				temp = temp->NextChatBuffer;
			}
			//printf("\n temp=%s",temp);
#else
			StockFontBufferExt(x, y + ChatLineSmoothY + DISPLACEMENT_Y - (int)((ChatBuffer[j].fontsize / 2)*1.4), FONT_PRIO_BACK, ChatBuffer[j].color, (char *)ChatBuffer[j].buffer, 0, ChatBuffer[j].fontsize);
#endif
			//StockFontBufferExt( temp->x+x, y + ChatLineSmoothY + DISPLACEMENT_Y, FONT_PRIO_BACK, temp->color, ( char *)temp->buffer, 0, 0);

#else
			StockFontBuffer(    x, y + ChatLineSmoothY + DISPLACEMENT_Y, FONT_PRIO_BACK, ChatBuffer[ j ].color, ( char *)ChatBuffer[ j ].buffer, 0 );
#endif
		}
		y -= CHAT_ZOUBUN_Y;
#ifdef _FONT_SIZE
		y -= (int)((ChatBuffer[j].fontsize/2)*1.4);
#endif
		j--;
		if( j < 0 )
			j = MAX_CHAT_LINE - 1;
	}
	if( ChatLineSmoothY > 0 )
		ChatLineSmoothY--;
#ifdef _NB_LABEL_DISP
	int 频道图标Y = 480 - 41;
	if (ResoMode == 分辨率::mode800){
		频道图标Y = 600 - 41;
	}else if (ResoMode == 分辨率::mode640){
		频道图标Y = 480 - 41;
	}else if (ResoMode == 分辨率::mode1024){
		频道图标Y = 768 - 41;
	}
	int g_szChannelDisp[] = { 55400, 55401, 56003, 56004
#ifdef _CHAR_PROFESSION//职业频道
								,56005
#endif
#ifdef _CHATROOMPROTOCOL//聊天室
								,56006
#endif
#ifdef _CHATWORLDWIDEPROTOCOL//世界频道
								,56007
#endif
#ifdef _NB_CHANNEL_ALLSERV//星球频道
								,56008
#endif
								};
	StockDispBuffer(25, 频道图标Y, DISP_PRIO_IME3, g_szChannelDisp[TalkMode], 1);
#endif

#ifdef 聊天表情
	extern void ShowBottomLineString(int iColor, LPSTR lpstr);
	extern int focusGraId(int *id, int cnt);
	extern int pushGraId(int *id, int cnt);
	extern int selGraId(int *id, int cnt);
	int 表情按钮ID = -1;
	static int 表情图片索引 = 0;
	static int 表情窗口状态 = FALSE;
	static int 表情当前页 = 0;
	static int 表情总页 = (EXPRESSION_NOID_NUM) % 48 ? (EXPRESSION_NOID_NUM) / 48 + 1 : (EXPRESSION_NOID_NUM) / 48;
	//30 549 
	int 表情x = 47;
	int 表情y = 549;
	if (ResoMode == 分辨率::mode800){
		表情y = 600 - 51;
	}else if (ResoMode == 分辨率::mode640){
		表情y = 480 - 51;
	}else if (ResoMode == 分辨率::mode1024){
		表情y = 768 - 51;
	}
	if(表情窗口状态){
		StockDispBuffer绝对位置(表情x, 表情y, DISP_PRIO_IME3, CG_FIELD_CHAT_BTN_OFF, 1);
	}else{
		StockDispBuffer绝对位置(表情x, 表情y, DISP_PRIO_IME3, CG_FIELD_CHAT_BTN_ON, 1);
	}
	if (MakeHitBox(表情x, 表情y, 表情x + 20, 表情y + 20, DISP_PRIO_IME3) == TRUE){
		ShowBottomLineString(FONT_PAL_WHITE, "상대방에게 전송할 이모티콘을 선택합니다.");
		if (mouse.onceState & MOUSE_LEFT_CRICK) {
			表情当前页 = 1;
			表情图片索引 = 1;
			if (表情窗口状态) 表情窗口状态 = FALSE;
			else 表情窗口状态 = TRUE;
			play_se(203, 320, 240);
		}
		else 表情图片索引 = 0;
	}
	if (表情窗口状态) {
		if (joy_trg[0] & JOY_ESC) {
			MenuToggleFlag |= JOY_ESC;
			play_se(203, 320, 240);
			表情窗口状态 = FALSE;
		}
		int 方向按钮ID[2];
		StockDispBuffer绝对位置(2, 表情y - 204, DISP_PRIO_IME3, 55103, 1);
		方向按钮ID[0] = StockDispBuffer绝对位置(77, 表情y - 26, DISP_PRIO_IME4, 55104, 2);
		方向按钮ID[1] = StockDispBuffer绝对位置(140, 表情y - 26, DISP_PRIO_IME4, 55105, 2);
		int 按下ID = selGraId(方向按钮ID, 2);
		if (按下ID == 0) {//按方向左
			if (表情当前页>1) 表情当前页--;
		}
		else if (按下ID == 1) {//按方向右
			if (表情当前页 < 表情总页) 表情当前页++;
		}
		int start = (表情当前页 - 1) * 48;
		int end = (表情当前页 * 48 < EXPRESSION_NOID_NUM ? 表情当前页 * 48 : EXPRESSION_NOID_NUM);
		int x, y;
		for (y = 0; y<6; y++) {
			for (x = 0; x<8; x++) {
				表情按钮ID = StockDispBuffer绝对位置(7 + x * 29, 表情y -200 + y * 29, DISP_PRIO_IME4, EXPRESSION_NOID_START + start, 2);
				if (selGraId(&表情按钮ID, 1) != -1) {
					表情窗口状态 = FALSE;
					play_se(203, 320, 240);
					char 表情内容[128];
					sprintf_s(表情内容, "{#%d}", start + 1);
					strcat_s(MyChatBuffer.buffer, 表情内容);
					MyChatBuffer.cursor = strlen(MyChatBuffer.buffer);
					MyChatBuffer.cnt = strlen(MyChatBuffer.buffer);
				}
				start++;
				if (EXPRESSION_NOID_NUM < start) break;
			}
			if (EXPRESSION_NOID_NUM < start) break;
		}
	}
#endif

	StockFontBuffer2( &MyChatBuffer );
}

/*******************************************************************************/
/* ?????????????
/* ??  	?：?
/* 			?：?
/* 			??：?
/*******************************************************************************/
int GetStrLastByte( char *str )
{
	int byte = 0;
	
	// ??????????
	while( !( *str == '\0' ) ){
	//		( *str == ( char )0x81 && *( str + 1 ) == ( char )0x51 ) ) ){
	
		// ????
		if(IsDBCSLeadByte(*str)){
		
			// ???????????
			if( *( str + 1 ) == NULL ) return 3; // ??
			
			str += 2; 	// ??????
			byte = 2;	// ???
			
		}else{	// ????
		
			str ++; 	// ??????
			byte = 1;	// ???
		}
	}
	return byte;
}

/*******************************************************************************/
/* ??????????????
/* ??  	????
/*******************************************************************************/
#ifdef HITFLAG_FONT
int GetStrWidth( char *str, int iSize )
{
	int width = 0;
	// ??????????
	while(!( *str == '\0' ) ){
		if(IsDBCSLeadByte(*str)){
			str += 2;
			width += iSize; // 全形的size
		}else{
			str ++;
			width += iSize>>1; // 半形的size
		}
	}
	return width;
}
#else
int GetStrWidth( char *str )
{
	int width = 0;
	// ??????????
	while(!( *str == '\0' ) ){
		if(IsDBCSLeadByte(*str)){
			str += 2;
			width += FONT_SIZE; // 全形的size
		}else{
			str ++;
			width += FONT_SIZE>>1; // 半形的size
		}
	}
	return width;
}
#endif

/*******************************************************************************/
/* ??????
/*******************************************************************************/
void GetKeyInputFocus( STR_BUFFER *pStrBuffer )
{
	
	// ??????
	//if( pNowStrBuffer != NULL ){
		// ??????????
	//	if(	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] != '\0' ){
			// ?????
	//		pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = '\0';
	//	}
	//}
	// ????????????
	//*ImeBuffer = '\0';
	// ???????????
	pNowStrBuffer = pStrBuffer;
}

// ?????????????????? ********************************************/
void GetClipboad( void )
{
	HGLOBAL hMem;
	LPTSTR lpMem;

	/* ???????????? */
	OpenClipboard( hWnd );
	/* ????????????????????? */
	hMem = GetClipboardData( CF_TEXT );
	/* ?????????? */
	if( hMem == NULL ){
		CloseClipboard();
		return;
	}
	/* ????????????? */
	lpMem = (LPTSTR)GlobalLock( hMem );
	/* ????????????????? */
//	SetWindowText( hwStrE, lpMem );
//	lstrcpy( chat_input_buf, lpMem );

	// ????????????
	StrToNowStrBuffer( lpMem );
	
	/* ?????????? */
	GlobalUnlock( hMem );
	CloseClipboard();
}

// ??????????? ******************************************************/
void SetClipboad( void )
{
	HGLOBAL hMem;
	LPTSTR lpMem;

	// ????????
	if( pNowStrBuffer == NULL ) return;
	// ???????
	if( pNowStrBuffer->buffer[ 0 ] == NULL ) return;
	
	/* ?????????? */
	hMem = GlobalAlloc( GHND, 512 );
	/* ???????????????????? */
	lpMem = ( LPTSTR )GlobalLock( hMem );
//	lstrcpy( lpMem, chat_input_buf );
	/* ????????????????? */
	lstrcpy( lpMem, pNowStrBuffer->buffer );
	/* ??????????????? */
	GlobalUnlock( hMem );
	/* ???????????? */
	OpenClipboard( hWnd );
	/* ??????????? */
	EmptyClipboard();
	/* ???????????????????????? */
	SetClipboardData( CF_TEXT, hMem );
	/* ???????????? */
	CloseClipboard();
	/* ?????????? */
	//GlobalFree( hMem );
}



/************************/
/*	chat.h				*/
/************************/
#pragma once
#ifndef _CHAT_H_
#define _CHAT_H_
//cary 256 -> 260
#define STR_BUFFER_SIZE 	260 		// ???????????
#ifdef _NEW_RESOMODE
#ifdef _NB_SA_LIAOTIAN
	#define MAX_CHAT_LINE 		200 		// 聊天行数限制
#else
	#define MAX_CHAT_LINE 		30 			// 聊天行数限制
#endif
#else
	#define MAX_CHAT_LINE 		20 			// 聊天行数限制
#endif
#define DEF_CHAT_LINE 		20 			// ?????????
#define MAX_CHAT_HISTORY 	64			// ???????????
#define DEF_VOICE 			3			// ????????
#define MAX_VOICE 			5			// ???????

#define NORMAL_TYPE		0
#define BLIND_TYPE		1
#define NUMBER_TYPE		2


typedef struct{
	char 	buffer[ STR_BUFFER_SIZE ]; // ??????????
	unsigned char 	len;		// ???
	unsigned char 	lineLen;	// ??????
	unsigned char 	lineDist;	// ?????????????
	unsigned char 	cnt;		// ??????
	unsigned char 	color;		// ??
	unsigned char	cursor;		//游标的位置
	int	x, y;					// ????
	int	imeX, imeY;				// ????????
	int	fontPrio;				// ??
	BOOL filterFlag;			// ???????????????
	int hitFontNo;				// ??????????????
	
}STR_BUFFER;

typedef struct _CHAT_BUFFER {
	char 	buffer[STR_BUFFER_SIZE + 1]; // ??????????
	unsigned char 	color;
	int id;
#ifdef 聊天表情
	int BmpNo;
	int x;
	_CHAT_BUFFER * NextChatBuffer;
	//pos
	int map;
	short posx;
	short posy;

	//pet
	char 称号[20];
	short reborn, lv,hp, gong, fang, min;
	int image;

	int fontID;
#endif
#ifdef _FONT_SIZE
	int fontsize;
#endif
}CHAT_BUFFER;

// ???????????
typedef struct{
	char str[ MAX_CHAT_HISTORY ][ STR_BUFFER_SIZE + 1 ];	// ????????????
	int newNo;												// ????????????
	int nowNo;												// ?????????????
}CHAT_HISTORY;

// onlinegm问题输入纪录   
typedef struct{
	char str[115][STR_BUFFER_SIZE + 1 ];
	int newNo;	  							
	int nowNo;
	int addNo;  
	int lockNo;
	BOOL color[115];
}INPUT_HISTORY;

// ???????????????????????
extern STR_BUFFER *pNowStrBuffer;

// ?????????
extern CHAT_BUFFER ChatBuffer[];
// ?????????
extern STR_BUFFER MyChatBuffer;

// ??????????
extern int NowChatLine;

// ??????????
extern int NowMaxChatLine;

// ?????
extern int NowMaxVoice;

/*	将单一字元储放至目前的输入String buffer
parameter:	c:	字元					*/
void StockStrBufferChar(char c);

/*	将双位元字元储放至目前的输入String buffer
parameter:	lpc:	双位元的字元			*/
void StockStrBufferDBChar(char *lpc);

enum TYPE_TALK
{
	系统,
	附近,
	家族,
	私聊,
	队伍,
	世界,
};

/* ?????????????????????? */
void ChatBufferToFontBuffer( void );
void ChatProc( void );
void StockChatBufferLine( char *str, unsigned char color,int id = TYPE_TALK::系统);
#ifdef _FONT_SIZE
void StockChatBufferLineExt( char *str, unsigned char color, int fontsize ,int id = TYPE_TALK::系统);
#endif
void ClearChatBuffer( void );
int GetStrLastByte( char *str );
#ifdef HITFLAG_FONT
int GetStrWidth( char *str, int iSize = 14 );
#else
int GetStrWidth( char *str );
#endif
void GetKeyInputFocus( STR_BUFFER *pStrBuffer );

/* ????????? **********************************************************/
void InitChat( void );

/* ???????? ************************************************************/
void ChatProc( void );

// ??????????? **************************************************/
void FlashKeyboardCursor( void );

void KeyboardLeft();
void KeyboardRight();

/* ???????? ************************************************************/
void KeyboardReturn( void );

// ?????? ***************************************************************/
void KeyboardBackSpace( void );

// ????????? ****************************************************************/
BOOL SaveChatHistoryStr( int no );
// ???????????? ****************************************************************/
BOOL LoadChatHistoryStr( void );

BOOL LoadReadSayShield( void );
BOOL LoadReadNameShield( void );

bool CheckSay( const char* strSay, const char szReplace = '*' );
bool CheckName( const char* strSay/*, char* word*/ );
// ?????????????????? ********************************************/
void GetClipboad( void );
// ??????????????????????? ***********************************/
void SetClipboad( void );
int GetCharByte( char c );

#ifdef _FRIENDCHANNEL			//ROG ADD 好友频道
void StrToNowStrBuffer( char *str );
#endif




#endif

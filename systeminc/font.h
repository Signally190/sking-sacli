﻿/************************/
/*	font.h				*/
/************************/
#ifndef _FONT_H_
#define _FONT_H_

// ????????
#define FONT_SIZE 14
#define FONT_SIZE1 14
#define FONT_SIZE2 19
#define FONT_ZENKAU_WIDTH FONT_SIZE + 1
// ???????????
#define	FONT_HANKAKU_WIDTH	FONT_SIZE / 2 + 1

// ???????????
extern int FontZenkauWidth;
// ???????????
extern int FontHankakuWidth;

enum{
	FONT_PRIO_BACK,
	FONT_PRIO_FRONT

#ifdef _TRADETALKWND				// (不可开) Syu ADD 交易新增对话框架
	, FONT_PRIO_AFRONT
#endif
#ifdef _CHANNEL_MODIFY
	,FONT_PRIO_CHATBUFFER
#endif
};

typedef struct{
	short x, y;
	char color;
	char str[ 256 ];
	UCHAR	fontPrio;
	BOOL hitFlag;
#ifdef _FONT_SIZE
	int size;
#endif
}FONT_BUFFER;

extern FONT_BUFFER FontBuffer[];

extern int FontCnt;
void FontPrint( char *moji, int x, int y, int dispPrio );

#ifdef 聊天表情
void NewStockFontBuffer(CHAT_BUFFER *chatbuffer, int x, unsigned char color, char *str, int size,int id = -1);
void delFontBuffer(CHAT_BUFFER *chatbuffer);
#endif

// ???????? ///////////////////////////////////////////////////////
void FontPrintDec( char *moji, int x, int y, int dispPrio );

/* ????????????? *********************************************/
int StockFontBuffer( int x, int y, char fontPrio, int color, char *str, BOOL hitFlag );
#ifdef _FONT_SIZE
int StockFontBufferExt( int x, int y, char fontPrio, int color, char *str, BOOL hitFlag, int size );
HFONT CreateNewFont( int size);
#endif
/* ?????????????????********************************/
void StockFontBuffer2(STR_BUFFER *strBuffer);

/* ONLINEGM ADD */
int StockFontBuffer3( STR_BUFFER *strBuffer );

#endif

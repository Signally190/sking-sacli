#ifndef _LOADREALBIN_H_
#define _LOADREALBIN_H_

void initAutoMapColor( char * );
void makeAutoMapColor( void );
int writeAutoMapColor( char *, char * );
int readAutoMapColor( char *, char * );

#ifdef _HI16_REEDITIMAGE
void initRealbinFPoint();
#endif


#ifdef _READ16BITBMP
int InitRealTruebinFileOpen(char *szRealTrueBinFileName,char *szAdrnTruebinFileName);
#endif
void cleanupRealbin(void);

extern unsigned char autoMapColorTbl[];

#include <stdio.h>
typedef unsigned char MOJI;
typedef unsigned char U1;
typedef          char S1;
typedef unsigned short U2;
typedef          short S2;
typedef unsigned long U4;
typedef          long S4;
typedef float  F4;
typedef double F8;
#ifdef _READ16BITBMP
BOOL Read16BMP(int BmpNo,unsigned char **BmpData,int *width,int *height,BYTE **AlphaData,BOOL *useAlpha);
#endif
int realGetImage(int graphicNo, unsigned char **bmpdata, int *width, int *height);
int realGetPos(U4 GraphicNo , S2 *x , S2 *y);
int realGetWH(U4 GraphicNo , S2 *w , S2 *h);
int realGetHitPoints(U4 GraphicNo , S2 *HitX , S2 *HitY);
int realGetHitFlag(U4 GraphicNo , S2 *Hit);
BOOL realGetPrioType(U4 GraphicNo , S2 *prioType);
int realGetHeightFlag(U4 GraphicNo , S2 *Height);
int realGetNo( U4 CharAction , U4 *GraphicNo );
int realGetSoundEffect(U4 GraphicNo);
int realGetWalkSoundEffect(U4 GraphicNo);
int realGetBitmapNo( int num );
typedef struct {
	unsigned char atari_x,atari_y;	//??
	unsigned short hit;				// 可否行走 //???
	short height;				//?????
	short broken;				//????
	short indamage;				//HP????
	short outdamage;			//????
	short inpoison;				//
	short innumb;				//???
	short inquiet;				//?
	short instone;				//?
	short indark;				//??
	short inconfuse;			//??
	short outpoison;			//
	short outnumb;				//??
	short outquiet;				//?
	short outstone;				//?
	short outdark;				//??
	short outconfuse;			//??
	short effect1;				//?????1???????????????
	short effect2;				//?????2?????????????
	unsigned short damy_a;
	unsigned short damy_b;
	unsigned short damy_c;
	unsigned int bmpnumber;		//???
} MAP_ATTR;

struct ADRNBIN{
	unsigned long	bitmapno;
	unsigned long	adder;
	unsigned long	size;
	int	xoffset;
	int	yoffset;
	unsigned int width;
	unsigned int height;

	MAP_ATTR attr;

};

#ifdef _READ16BITBMP
struct AddressBin_s{
	unsigned int bitmapno;			// 图号
	unsigned int adder;					// 资料在图素档里的起始位址
	unsigned int palSize;				// 色盘资料大小
	unsigned int size;					// 一笔资料的大小
	unsigned int alpha_size;		// alpha_size不为0表示在图素资料后连着是alpha资料,alpha_size就是alpha资料的大小
															// 如果alpha_size为0表示没有带alpha资料
	int	xoffset;
	int	yoffset;
	unsigned int width;					// 图宽
	unsigned int height;				// 图高
	unsigned int staturated;		// 是否用饱和效果
};
#endif

// ????????
static DWORD FontPal[]={
	RGB(0xff ,0xff, 0xff ), // 0:?
		RGB(0x00 ,0xff, 0xff ), // 1:?
		RGB(0xff ,0x00, 0xff ), // 2:?
		RGB(0x00 ,0x00, 0xff ), // 3:
		RGB(0xff ,0xff, 0x00 ), // 4:?
		RGB(0x00 ,0xff, 0x00 ), // 5:?
		RGB(0xff ,0x00, 0x00 ), // 6:
		RGB(0xa0 ,0xa0, 0xa4 ), // 7:?
		RGB(0xa6 ,0xca, 0xf0 ), // 8:?
		RGB(0xc0 ,0xdc, 0xc0 ),  // 9:??
		RGB(0xFF ,0x8C, 0x00) , // 10:??
		RGB(0xFF ,0x7f, 0x24) ,  // 11:??
		RGB(0 ,0, 0)  // 12:??
};

/*
BOOL InitRealbinFileOpen( char *realbinfilename, char *addrbinfilename);	//初始化realbin的资料并配置相关的记忆体
//void SetCurFrame();									//累加CurFrame的counter，请每个frame执行一次
void CacheRLU( int howlong);							//释放太久没使用的图档记忆体
BOOL realGetNo( DWORD CharAction, DWORD *GraphicNo);	//从char action转成图档编号
DWORD realGetNo2( DWORD bnum);							//从bnum转成图档编号
BOOL realIsValid( DWORD CharAction);					//检查图号是否有效
DWORD realGetBitmapNo( DWORD num);						//从图档编号转回bnum
BOOL realGetPos( DWORD GraphicNo, short *x, short *y);	//取回GraphicNo的x和y offset
BOOL realGetWH( DWORD GraphicNo, short *w, short *h);	//取回GraphicNo的宽和高
BOOL realGetHitPoints( DWORD GraphicNo, short *HitX, short *HitY);	//取回GraphicNo的实际宽和高
BOOL realGetPrioType( DWORD GraphicNo , short *prioType);	//取回GraphicNo的优先权种类
BOOL realGetHitFlag( DWORD GraphicNo, short *Hit);			//取回GraphicNo的Hit
BOOL realGetHeightFlag( DWORD GraphicNo, short *Height);	//取回GraphicNo是否有高度
int realGetSoundEffect( DWORD GraphicNo);					//取回GraphicNo的环境音
int realGetWalkSoundEffect( DWORD GraphicNo);				//取回GraphicNo的脚步声
LPBYTE ReadRealBin( DWORD GraphicNo);
//画出realbin
BOOL DrawScaleImage( DWORD GraphicNo, int screenx, int screeny, int screenw, int screenh, LPWORD dest,
					int destw, int desth, int pitch, LPWORD pal);
*/
#endif /*_LOADREALBIN_H_*/

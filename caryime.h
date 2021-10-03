/******************************************************************************
	caryIme.h:	cary's Ime的DllMain。
												(c) 许百胜Cary Hsu,	2000.7.10
******************************************************************************/
#ifndef __CARYIME
#define __CARYIME

typedef void(*RESULTCALLBACK)(LPSTR);

/*	IME的初始化
parameter:	hwnd:			所使用的HWND
			ResultCallback:	中文输入法的结果回报
return:		TRUE:			严重的错误，请结束程式，否则无法使用中文输入法*/
BOOL InitIme(HWND hwnd,RESULTCALLBACK ResultCallback);

/*	IME结束*/
void EndIme();

/*	改变显示的模式，window mode或fullscreen mode
parameter:	bWindowMode:	使用的视窗模式		*/
void ChangeMode(BOOL bWindowMode);

/*	处理IME的message
parameter:	Message:	视窗讯息
			wParam,lParam:视窗讯息的参数
return:	TRUE:	已经有处理IME的message
		FALSE:	还有其也的message需要处理	*/
BOOL ImeMessage(UINT Message,WPARAM wParam,LPARAM lParam);

/*	clear IME buffer	*/
void ClearIme();

/*	IME的ON,OFF状态确认
return:	ON:	TRUE
		OFF:FALSE		*/
BOOL ImeOpenState();

/*	取得输入法的字串，可能candiate或compsition	
return:	NULL:	没有字串，请自行处理
		非NULL:	有输入法的字串				*/
LPSTR GetImeString();

/*	取得输入法的描述字串
return:	输入法的描述字串	*/
LPSTR GetImeDescString();

///////////////////////////////////////////////////////////////////////////////

/*	取得lpstr的上一个字的位址
parameter:	lpstrHead:	字串的头
			lpstr:		要取回上一个字的字串
return:		上一个字的位址					*/
char *GetCharPrev(char *lpstrHead,char *lpstr);

#endif

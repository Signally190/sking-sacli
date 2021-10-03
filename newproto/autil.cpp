// Arminius' protocol utilities ver 0.1
//
// Any questions and bugs, mailto: arminius@mail.hwaei.com.tw

// -------------------------------------------------------------------
// The following definitions is to define game-dependent codes.
// Before compiling, remove the "//".
#define __STONEAGE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "autil.h"
#include "../systeminc/chat.h"
#include "../systeminc/font.h"

#ifdef __STONEAGE
#include "../systeminc/version.h"
#include "../systeminc/lssproto_util.h"
//#include "../systeminc/common.h"
#endif

//JL
#include "../systeminc/JL_func.h"

char **MesgSlice;
int SliceCount;

char PersonalKey[32];

#ifdef  _STONDEBUG_
extern int g_iMallocCount;
#endif

// -------------------------------------------------------------------
// Initialize utilities
//
void util_Init(void){
  int i;
  MesgSlice = (char **) MALLOC(sizeof(char *) * SLICE_MAX);
#ifdef  _STONDEBUG_
	g_iMallocCount++;
#endif
  for (i=0; i<SLICE_MAX; i++){
    MesgSlice[i] = (char *) MALLOC(SLICE_SIZE);
#ifdef  _STONDEBUG_
		g_iMallocCount++;
#endif
	}
  SliceCount = 0;
}

// -------------------------------------------------------------------
// Split up a message into slices by spearator.  Store those slices
// into a global buffer "char **MesgSlice"
//
// arg: source=message string;  separator=message separator (1 byte)
// ret: (none)
void util_SplitMessage(char *source, char *separator){
  if (source && separator) {	// NULL input is invalid.
    char *ptr;
    char *head = source;
    
    while ((ptr = (char *) strstr(head, separator)) && (SliceCount<=SLICE_MAX)) {
      ptr[0] = '\0';
      if (strlen(head)<SLICE_SIZE) {	// discard slices too large
        strcpy(MesgSlice[SliceCount], head);
        SliceCount++;
      }
      head = ptr+1;
    }
    strcpy(source, head);	// remove splited slices
  }
}

// -------------------------------------------------------------------
// Encode the message
//
// arg: dst=output  src=input
// ret: (none)
void util_EncodeMessage(char *dst, char *src){
  int rn = rand()%99;
  int t1, t2;
  char t3[65500], tz[65500];
  util_swapint(&t1, &rn, "2413");
  t2 = t1 ^ 0xffffffff;
  util_256to64(tz, (char *) &t2, sizeof(int), DEFAULTTABLE);
  
  util_shlstring(t3, src, rn);
  strcat(tz, t3);
  util_xorstring(dst, tz);
}

// -------------------------------------------------------------------
void util_DecodeMessage(char *dst, char *src){
#define INTCODESIZE	(sizeof(int)*8+5)/6
  int rn;
  int *t1, t2;
  char t3[4096], t4[4096];
  char tz[65500];

  if( src[strlen(src)-1] == '\n')
	src[strlen(src)-1] = 0;
  util_xorstring(tz, src);

  rn=INTCODESIZE;
  strncpy(t4, tz, INTCODESIZE);
  t4[INTCODESIZE] = '\0';
  util_64to256(t3, t4, DEFAULTTABLE);
  t1 = (int *) t3;
  t2 = *t1 ^ 0xffffffff;
  util_swapint(&rn, &t2, "3142");
  util_shrstring(dst, tz + INTCODESIZE, rn);
}

// -------------------------------------------------------------------
int util_GetFunctionFromSlice(int *func, int *fieldcount){
  char t1[16384];
  int i;
  strcpy(t1, MesgSlice[1]);
  *func = atoi(t1) - 23;
  for (i=0; i<SLICE_MAX; i++)
    if (strcmp(MesgSlice[i], DEFAULTFUNCEND)==0) {
      *fieldcount=i-2;
      return 1;
    }

  return 0;
}

void util_DiscardMessage(void){
	SliceCount=0;
}


void util_SendMesg(int fd, int func, char *buffer){
	char t1[60000], t2[60000];
  int addr = 0;
  __asm
  {
	  push dword ptr [ebp + 4];
	  pop addr;

  }
  JL_combinePacket_SEND(func, addr);
  sprintf(t1, "&;%d%s;#;", func + 13, buffer);
#if 0
	if( lssproto_writelogfilename[0] != '\0' ){
		FILE *wfp = fopen( lssproto_writelogfilename , "a+" );
		if(wfp){
			fprintf( wfp , "送出：\t%s\n", t1 );
			fclose(wfp);
		}
	}
#endif
  util_EncodeMessage(t2, t1);
#ifdef __STONEAGE
  lssproto_Send(fd, t2);
#endif
}

// -------------------------------------------------------------------
int util_256to64(char *dst, char *src, int len, char *table){
  unsigned int dw,dwcounter;
  int i;

  if (!dst || !src || !table) return 0;
  dw=0;
  dwcounter=0;
  for (i=0; i<len; i++) {
    dw = ( ((unsigned int)src[i] & 0xff) << ((i%3)<<1) ) | dw;
    dst[ dwcounter++ ] = table[ dw & 0x3f ];
    dw = ( dw >> 6 );
    if (i%3==2) {
      dst[ dwcounter++ ] = table[ dw & 0x3f ];
      dw = 0;
    }
  }
  if (dw) dst[ dwcounter++ ] = table[ dw ];
  dst[ dwcounter ] = '\0';
  return dwcounter;
}

// -------------------------------------------------------------------
int util_64to256(char *dst, char *src, char *table){
  unsigned int dw,dwcounter;
  unsigned int i, j;
  char *ptr = NULL;

  dw=0;
  dwcounter=0;
  if (!dst || !src || !table) return 0;
  char c;
  for (i=0; i<strlen(src); i++) {
	  c = src[i];
	  for( j = 0; j < strlen(table); j++){
		  if( table[j] == c){
			  ptr = table+j;
			  break;
		  }
	  }
    if (!ptr) return 0;
    if (i%4) {
      dw = ((unsigned int)(ptr-table) & 0x3f) << ((4-(i%4))<<1) | dw;
      dst[ dwcounter++ ] = dw & 0xff;
      dw = dw >> 8;
    } else {
      dw = (unsigned int)(ptr-table) & 0x3f;
    }
  }
  if (dw) dst[ dwcounter++ ] = dw & 0xff;
  dst[ dwcounter ] = '\0';
  return dwcounter;
}

// -------------------------------------------------------------------
int util_256to64_shr(char *dst, char *src, int len, char *table, char *key){
  unsigned int dw,dwcounter,j;
  int i;

  if (!dst || !src || !table || !key) return 0;
  if (strlen(key)<1) return 0;	// key can't be empty.
  dw=0;
  dwcounter=0;
  j=0;
  for (i=0; i<len; i++) {
    dw = ( ((unsigned int)src[i] & 0xff) << ((i%3)<<1) ) | dw;
    dst[ dwcounter++ ] = table[ ((dw & 0x3f) + key[j]) % 64 ];	// check!
    j++;  if (!key[j]) j=0;
    dw = ( dw >> 6 );
    if (i%3==2) {
      dst[ dwcounter++ ] = table[ ((dw & 0x3f) + key[j]) % 64 ];// check!
      j++;  if (!key[j]) j=0;
      dw = 0;
    }
  }
  if (dw) dst[ dwcounter++ ] = table[ (dw + key[j]) % 64 ];	// check!
  dst[ dwcounter ] = '\0';
  return dwcounter;
}

// -------------------------------------------------------------------
int util_shl_64to256(char *dst, char *src, char *table, char *key){
  unsigned int dw,dwcounter,i,j,k;
  char *ptr = NULL;

  if (!key || (strlen(key)<1)) return 0;	// must have key

  dw=0;
  dwcounter=0;
  j=0;
  if (!dst || !src || !table) return 0;
  char c;
  for (i=0; i<strlen(src); i++) {
	  c = src[i];
	  for( k = 0; k < strlen(table); k++){
		  if( table[k] == c){
			  ptr = table+k;
			  break;
		  }
	  }
    if (!ptr) return 0;
    if (i%4) {
      // check!
      dw = ((((unsigned int)(ptr-table) & 0x3f) + 64 - key[j]) % 64)
           << ((4-(i%4))<<1) | dw;
      j++;  if (!key[j]) j=0;
      dst[ dwcounter++ ] = dw & 0xff;
      dw = dw >> 8;
    } else {
      // check!
      dw = (((unsigned int)(ptr-table) & 0x3f) + 64 - key[j]) % 64;
      j++;  if (!key[j]) j=0;
    }
  }
  if (dw) dst[ dwcounter++ ] = dw & 0xff;
  dst[ dwcounter ] = '\0';
  return dwcounter;
}

// -------------------------------------------------------------------
int util_256to64_shl(char *dst, char *src, int len, char *table, char *key)
{
  unsigned int dw,dwcounter;
  int i, j;

  if (!dst || !src || !table || !key) return 0;
  if (strlen(key)<1) return 0;	// key can't be empty.
  dw=0;
  dwcounter=0;
  j=0;
  for (i=0; i<len; i++) {
    dw = ( ((unsigned int)src[i] & 0xff) << ((i%3)<<1) ) | dw;
    dst[ dwcounter++ ] = table[ ((dw & 0x3f) + 64 - key[j]) % 64 ];	// check!
    j++;  if (!key[j]) j=0;
    dw = ( dw >> 6 );
    if (i%3==2) {
      dst[ dwcounter++ ] = table[ ((dw & 0x3f) + 64 - key[j]) % 64 ];	// check!
      j++;  if (!key[j]) j=0;
      dw = 0;
    }
  }
  if (dw) dst[ dwcounter++ ] = table[ (dw + 64 - key[j]) % 64 ];	// check!
  dst[ dwcounter ] = '\0';
  return dwcounter;
}

// -------------------------------------------------------------------
int util_shr_64to256(char *dst, char *src, char *table, char *key){
  unsigned int dw,dwcounter,i,j,k;
  char *ptr = NULL;

  if (!key || (strlen(key)<1)) return 0;	// must have key

  dw=0;
  dwcounter=0;
  j=0;
  if (!dst || !src || !table) return 0;
  char c;
  for (i=0; i<strlen(src); i++) {
	  c = src[i];
	  for( k = 0; k < strlen(table); k++){
		  if( table[k] == c){
			  ptr = table+k;
			  break;
		  }
	  }
    if (!ptr) return 0;
    if (i%4) {
      dw = ((((unsigned int)(ptr-table) & 0x3f) + key[j]) % 64)
           << ((4-(i%4))<<1) | dw;
      j++;  if (!key[j]) j=0;
      dst[ dwcounter++ ] = dw & 0xff;
      dw = dw >> 8;
    } else {
      dw = (((unsigned int)(ptr-table) & 0x3f) + key[j]) % 64;
      j++;  if (!key[j]) j=0;
    }
  }
  if (dw) dst[ dwcounter++ ] = dw & 0xff;
  dst[ dwcounter ] = '\0';
  return dwcounter;
}

// -------------------------------------------------------------------
void util_swapint(int *dst, int *src, char *rule){
  char *ptr, *qtr;
  int i;

  ptr = (char *) src;
  qtr = (char *) dst;
  for (i=0; i<4; i++) qtr[rule[i]-'1']=ptr[i];
}

// -------------------------------------------------------------------
void util_xorstring(char *dst, char *src){
  unsigned int i;
  
  for (i=0; i<strlen(src); i++) dst[i]=src[i]^255;
  dst[i]='\0';
}

// -------------------------------------------------------------------
void util_shrstring(char *dst, char *src, int offs)
{
  char *ptr;
  int len = strlen(src);
  if (!dst || !src || (strlen(src)<1)) return;
  
  offs = strlen(src) - (offs % strlen(src));
  ptr = src+offs;
  strcpy(dst, ptr);
  strncat(dst, src, offs);
  dst[strlen(src)]='\0';
}

// -------------------------------------------------------------------
// Shift the string left.
//
void util_shlstring(char *dst, char *src, int offs)
{
  char *ptr;
  if (!dst || !src || (strlen(src)<1)) return;
  
  offs = offs % strlen(src);
  ptr = src+offs;
  strcpy(dst, ptr);
  strncat(dst, src, offs);
  dst[strlen(src)]='\0';
}

// -------------------------------------------------------------------
int util_deint(int sliceno, int *value)
{
  int *t1, t2;
  char t3[4096];
  int addr;
  __asm
  {
	  push dword ptr[ebp + 4];
	  pop addr;
  }
  //printf("动态密钥1：%s\n",PersonalKey);
  util_shl_64to256(t3, MesgSlice[sliceno], DEFAULTTABLE, PersonalKey);
  t1 = (int *) t3;
  t2 = *t1 ^ 0xffffffff;
  util_swapint(value, &t2, "2413");

  //JL 2016.8.23
  //hook 整数解码
  JL_combineInt_RECV(*value, addr);



  return *value;
}

// -------------------------------------------------------------------
int util_mkint(char *buffer, int value){
  int t1, t2;
  char t3[4096];
  util_swapint(&t1, &value, "3142");
  t2 = t1 ^ 0xffffffff;

  //printf("动态密钥2：%s\n",PersonalKey);
  util_256to64_shr(t3, (char *) &t2, sizeof(int), DEFAULTTABLE, PersonalKey);
  strcat(buffer, ";");	// It's important to append a SEPARATOR between fields
  strcat(buffer, t3);

  //JL 2016.8.25
  JL_combineInt_SEND(value);

  return value;
}


// -------------------------------------------------------------------
// Convert a message slice into string.  Return a checksum.
//
// arg: sliceno=slice index in MesgSlice    value=result
// ret: checksum, this value must match the one generated by util_mkstring
int util_destring(int sliceno, char *value)
{

	//printf("动态密钥3：%s\n",PersonalKey);
	util_shr_64to256(value, MesgSlice[sliceno], DEFAULTTABLE, PersonalKey);
	extern int g_服务器;
	if (g_服务器 == SERVER_台服){
		BIG52GBK(value);
		GBK2GB(value);
	}


	//JL 2016.8.23
	//hook string解码
	int len = strlen(value);
	JL_combineString_RECV(value, len);

	return len; 
}

// -------------------------------------------------------------------
// Convert a string into buffer (a string).  Return a checksum.
//
// arg: buffer=output   value=data to pack
// ret: checksum, this value must match the one generated by util_destring
int util_mkstring(char *buffer, char *value)
{
  char t1[SLICE_SIZE];
  //printf("动态密钥4：%s\n",PersonalKey);
  util_256to64_shl(t1, value, strlen(value), DEFAULTTABLE, PersonalKey);
  strcat(buffer, ";");	// It's important to append a SEPARATOR between fields
  strcat(buffer, t1);

  //JL 2016.8.25
  //hook string编码
  int len = strlen(value);
  JL_combineString_SEND(value, len);

  return len ;
}

void util_Release(void)
{
  int i;
  
	if(MesgSlice){
		for(i=0;i<SLICE_MAX;i++){
			if(MesgSlice[i]){
				FREE(MesgSlice[i]);
#ifdef  _STONDEBUG_
				g_iMallocCount--;
#endif
			}
		}
		FREE(MesgSlice);
#ifdef  _STONDEBUG_
		g_iMallocCount--;
#endif
	}
}

int strcmptail(char *s1, char *s2)
{
	int i;
	int len1 = strlen(s1);
	int len2 = strlen(s2);

	for (i = 0;; i++) {
		int ind1 = len1 - 1 - i;
		int ind2 = len2 - 1 - i;
		if (ind1 < 0 || ind2 < 0) return 0;
		if (s1[ind1] != s2[ind2]) return 1;
	}
}

#define IS_2BYTEWORD( _a_ ) ( (char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF) )

BOOL getStringFromIndexWithDelim_body(char* src, char* delim, int index, char* buf, int buflen)
{
	int i;
	int length = 0;
	int addlen = 0;
	int oneByteMode = 0;

	if (strlen(delim) == 1) {
		oneByteMode = 1;
	}
	for (i = 0; i < index; i++) {
		char* last;
		src += addlen;

		if (oneByteMode) {

			char * ScanOneByte(char *src, char delim);
			last = ScanOneByte(src, delim[0]);
		}
		else {
			last = strstr(src, delim);
		}
		if (last == NULL) {
			void strcpysafe(char* dest, size_t n, const char* src);
			strcpysafe(buf, buflen, src);
			if (i == index - 1) {
				if (buf[0] == 0) return FALSE;
				return TRUE;
			}
			buf[0] = 0;
			return FALSE;
		}
		length = last - src;
		addlen = length + strlen(delim);
	}
	void strncpysafe(char* dest, const size_t n,
		const char* src, const int length);
	strncpysafe(buf, buflen, src, length);
	if (buf[0] == 0) return FALSE;
	return TRUE;
}

void ltrim(char *str) {
	char *ptr;
	for (ptr = str; *ptr == 32; ptr++);

}

void rtrim(char *str) {
	int i;
	for (i = (int)strlen(str) - 1; str[i] == 32 && i >= 0; str[i--] = 0);
}

#ifdef _FONT_STYLE_

WM_STR wmstr[25];
extern int getTextLength(char * str);
void getstrstyle(char *str, int index, int pos, int flg, WM_STR wm[])
{
	char *stemp, *etemp;
	if (flg)
		stemp = str;
	else
		stemp = sunday(str, "[style ");
	if (stemp) {
		if (stemp != str) {
			wm[index].flg = TRUE;
			wm[index].style[pos].size = FONT_SIZE1;
			if (pos == 0)
				wm[index].style[pos].x = 0;
			else
				wm[index].style[pos].x = getTextLength(wm[index].style[pos - 1].str) + wm[index].style[pos - 1].x;
			wm[index].style[pos].color = 0;
			int len = stemp - str;
			memcpy(wm[index].style[pos].str, str, len);
			wm[index].style[pos].str[len] = NULL;
			pos++;
			getstrstyle(stemp, index, pos, TRUE, wm);
		}
		else {
			wm[index].flg = TRUE;
			stemp = stemp + 7;
			etemp = sunday(stemp, "]") + 1;
			char *scolor = sunday(stemp, "c=");
			if (scolor) {
				scolor = scolor + 2;
				char strnum[3];
				int i = 0;
				for (i; i<2 && scolor[i] != ' ' && scolor[i] != ']'; i++) {
					strnum[i] = scolor[i];
				}
				strnum[i] = 0;
				wm[index].style[pos].color = atoi(strnum);
			}
			else wm[index].style[pos].color = 0;

			char *ssize = sunday(stemp, "s=");
			if (ssize) {
				ssize = ssize + 2;
				char strsize[3];
				int i = 0;
				for (i; i<2 && ssize[i] != ' ' && ssize[i] != ']'; i++) {
					strsize[i] = ssize[i];
				}
				strsize[i] = 0;
				wm[index].style[pos].size = atoi(ssize);
			}
			else wm[index].style[pos].size = FONT_SIZE1;

			stemp = sunday(str, "[/style]");

			int len = stemp - etemp;
			memcpy(wm[index].style[pos].str, etemp, len);
			wm[index].style[pos].str[len] = NULL;
			if (pos == 0)
				wm[index].style[pos].x = 0;
			else
				wm[index].style[pos].x = getTextLength(wm[index].style[pos - 1].str) + wm[index].style[pos - 1].x;
			pos++;
			stemp = stemp + 8;
			getstrstyle(stemp, index, pos, FALSE, wm);
		}
	}
	else {
		sprintf(wm[index].style[pos].str, "%s", str);
		if (!wm[index].flg) {
			wm[index].flg = TRUE;
			wm[index].style[pos].x = 0;
			wm[index].style[pos].color = 0;
			wm[index].style[pos].size = FONT_SIZE1;
		}
		else {
			wm[index].style[pos].size = FONT_SIZE1;
			wm[index].style[pos].x = getTextLength(wm[index].style[pos - 1].str) + wm[index].style[pos - 1].x;
			wm[index].style[pos].color = 0;
		}
	}
}

void PutWinText(int x, int y, char fontPrio, int color, char *str, BOOL hitFlag, int index)
{
	int i = 0;
	for (; i<30; i++) {
		if (*wmstr[index].style[i].str)
			StockFontBufferExt(x + wmstr[index].style[i].x, y, fontPrio, wmstr[index].style[i].color,
				wmstr[index].style[i].str, hitFlag, wmstr[index].style[i].size);
	}
}
#ifdef _CHARTITLE_STR_
void PutTitleText(int x, int y, char fontPrio, TITLE_STR str, BOOL hitFlag)
{
	int i = 0;
	for (; i<10; i++) {
		if (*str.style[i].str)
			StockFontBufferExt(x + str.style[i].x, y, fontPrio, str.style[i].color,
				str.style[i].str, hitFlag, str.style[i].size);
	}
}

extern int getTextLength(char * str);
void getTitlestyle(char *str, int pos, int flg, TITLE_STR* wm)
{
	char *stemp, *etemp;
	if (flg)
		stemp = str;
	else
		stemp = sunday(str, "[style ");
	if (stemp) {
		if (stemp != str) {
			wm->flg = TRUE;
			wm->style[pos].size = FONT_SIZE1;
			if (pos == 0)
				wm->style[pos].x = 0;
			else
				wm->style[pos].x = getTextLength(wm->style[pos - 1].str) + wm->style[pos - 1].x;
			wm->style[pos].color = 0;
			int len = stemp - str;
			memcpy(wm->style[pos].str, str, len);
			wm->style[pos].str[len] = NULL;
			pos++;
			getTitlestyle(stemp, pos, TRUE, wm);
		}
		else {
			wm->flg = TRUE;
			stemp = stemp + 7;
			etemp = sunday(stemp, "]") + 1;
			char *scolor = sunday(stemp, "c=");
			if (scolor) {
				scolor = scolor + 2;
				char strnum[3];
				int i = 0;
				for (i; i<2 && scolor[i] != ' ' && scolor[i] != ']'; i++) {
					strnum[i] = scolor[i];
				}
				strnum[i] = 0;
				wm->style[pos].color = atoi(strnum);
			}
			else wm->style[pos].color = 0;

			char *ssize = sunday(stemp, "s=");
			if (ssize) {
				ssize = ssize + 2;
				char strsize[3];
				int i = 0;
				for (i; i<2 && ssize[i] != ' ' && ssize[i] != ']'; i++) {
					strsize[i] = ssize[i];
				}
				strsize[i] = 0;
				wm->style[pos].size = atoi(ssize);
			}
			else wm->style[pos].size = FONT_SIZE1;

			stemp = sunday(str, "[/style]");

			int len = stemp - etemp;
			memcpy(wm->style[pos].str, etemp, len);
			wm->style[pos].str[len] = NULL;
			if (pos == 0)
				wm->style[pos].x = 0;
			else
				wm->style[pos].x = getTextLength(wm->style[pos - 1].str) + wm->style[pos - 1].x;
			pos++;
			stemp = stemp + 8;
			getTitlestyle(stemp, pos, FALSE, wm);
		}
	}
	else {
		sprintf(wm->style[pos].str, "%s", str);
		if (!wm->flg) {
			wm->flg = TRUE;
			wm->style[pos].x = 0;
			wm->style[pos].color = 0;
			wm->style[pos].size = FONT_SIZE1;
			wm->len = getTextLength(wm->style[pos].str);
		}
		else {
			wm->style[pos].size = FONT_SIZE1;
			wm->style[pos].x = getTextLength(wm->style[pos - 1].str) + wm->style[pos - 1].x;
			wm->style[pos].color = 0;
			wm->len = getTextLength(wm->style[pos].str) + wm->style[pos].x;
		}
	}
}

void getCharTitleSplit(char *str, TITLE_STR* title)
{
	memset(title, 0, sizeof(TITLE_STR));
	if (str == NULL) return;
	int i = 0;
	for (i; i<25; i++) {
		if (*str) {
			getTitlestyle(str, 0, FALSE, title);
		}
	}
}
#endif


void getStrSplitNew(char str[][256])
{
	memset(wmstr, 0, sizeof(WM_STR) * 25);
	int i = 0;

	for (i; i<25; i++) {
		if (str[i][0]) {
			getstrstyle(str[i], i, 0, FALSE, wmstr);
		}
	}
}


#endif




﻿#include "../systeminc/version.h"
#ifdef _PTTERN_SEPARATION_BIN
#include <windows.h> 
#include "../systeminc/system.h"
#include "../systeminc/loadrealbin.h"
#include "../systeminc/loadsprbin.h"
#include "../systeminc/unpack.h"
#include "../systeminc/sprmgr.h"
#include "../systeminc/anim_tbl.h"
#include "PtternSeparationBin.h"
#include "../systeminc/VMProtectSDK.h"
#include "../wgs/tea.h"

extern ADRNBIN adrnbuff[MAX_GRAPHICS];
extern unsigned long bitmapnumbertable[MAX_GRAPHICS];
extern SPRITEDATA SpriteData[mxSPRITE];

int getfilesize(FILE* fp)
{
	if (fp == NULL) return 0;
	int size;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return size;
}

//cary 2001 07 10
extern FILE *Realbinfp[MAX_GRAPHICS];
int MaxAdrnID = 0;
void InitPteernSeparationBin(LPCTSTR lpszPath) 
{

	TCHAR szFind[MAX_PATH]; 
	lstrcpy(szFind, lpszPath); 
	lstrcat(szFind, "\\"); 
	lstrcat(szFind, "*.*"); // 找所有文件 
	WIN32_FIND_DATA wfd; 
	HANDLE hFind = FindFirstFile(szFind, &wfd); 
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败 
		return; 
	int index = 2;
	do 
	{ 
		if (wfd.cFileName[0] == '.') 
			continue; // 过滤这两个目录 
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){ 
			TCHAR szFile[MAX_PATH]; 
			TCHAR szFindAdrnName[MAX_PATH]; 
			TCHAR szFindRealName[MAX_PATH]; 
			TCHAR szFindSprAdrnName[MAX_PATH]; 
			TCHAR szFindSprName[MAX_PATH]; 

			char FilePath[200]; 
			GetCurrentDirectory(200,FilePath); 
			wsprintf(szFile, "%s//%s", FilePath, lpszPath); 
			wsprintf(szFile, "%s//%s", lpszPath, wfd.cFileName); 

#ifdef 分离补丁加密
			wsprintf(szFindAdrnName, "%s//adrn.sq", szFile); 
			wsprintf(szFindRealName, "%s//real.sq", szFile); 
#else
			wsprintf(szFindAdrnName, "%s//adrn.bin", szFile);
			wsprintf(szFindRealName, "%s//real.bin", szFile);
#endif

			if(AdrnInit(szFindAdrnName, szFindRealName, index)==TRUE){
#ifdef 分离补丁加密
				wsprintf(szFindSprAdrnName, "%s//spradrn.sq", szFile); 
				wsprintf(szFindSprName, "%s//spr.sq", szFile); 
#else
				wsprintf(szFindSprAdrnName, "%s//spradrn.bin", szFile); 
				wsprintf(szFindSprName, "%s//spr.bin", szFile); 
#endif
				extern BOOL InitSprBinFileOpen( char *SPR_BIN, char *SPRADRN_BIN );
				InitSprBinFileOpen(szFindSprName,szFindSprAdrnName);
				index++;
				if(index>100)break;
			}
			InitPteernSeparationBin(szFile); // 如果找到的是目录，则进入此目录进行递归
		}else{ 

		} 
	} while (FindNextFile(hFind, &wfd)); 
	FindClose(hFind); // 关闭查找句柄 

} 
int nextMaxAdrnID=0;
BOOL AdrnInit(LPCTSTR strAdrnFileName, LPCTSTR strRealFileName, int index)
{
	ADRNBIN tmpadrnbuff;
	FILE *Realfp, *Addrbinfp;
	nextMaxAdrnID = MaxAdrnID;
	int maxid = MaxAdrnID;
	if (fopen_s(&Addrbinfp, strAdrnFileName, "rb")) {
		return FALSE;
	}
	if (fopen_s(&Realfp, strRealFileName, "rb")) {
		return FALSE;
	}
#ifdef 分离补丁加密
VMProtectBegin("bin");
	int size = getfilesize(Addrbinfp);
	char * sadrnbin = (char*)malloc(size);
	fread(sadrnbin, 1, size, Addrbinfp);
	TEA解密((long*)sadrnbin, (size / 4), (long *)VMProtectDecryptStringA(_分离补丁密钥));
VMProtectEnd();
	int i = 0;
	for (i; i < size / sizeof(ADRNBIN); i++) {
		memcpy(&tmpadrnbuff, sadrnbin + i * sizeof(ADRNBIN), sizeof(ADRNBIN));
		if (adrnbuff[tmpadrnbuff.bitmapno + maxid].bitmapno)
		{
			__asm nop;
		}
		adrnbuff[tmpadrnbuff.bitmapno + maxid] = tmpadrnbuff;
		Realbinfp[tmpadrnbuff.bitmapno + maxid] = Realfp;
		if ((ULONG)MaxAdrnID < tmpadrnbuff.bitmapno + maxid) {
			MaxAdrnID = tmpadrnbuff.bitmapno + maxid;
		}
		if (tmpadrnbuff.attr.bmpnumber != 0) {
			bitmapnumbertable[tmpadrnbuff.attr.bmpnumber] = tmpadrnbuff.bitmapno + maxid;
		}
	}
	fclose(Addrbinfp);
	free(sadrnbin);
#else
	while (!feof(Addrbinfp)) {
		fread(&tmpadrnbuff, sizeof(tmpadrnbuff), 1, Addrbinfp);
		adrnbuff[tmpadrnbuff.bitmapno + maxid] = tmpadrnbuff;
		Realbinfp[tmpadrnbuff.bitmapno + maxid] = Realfp;
		if ((ULONG)MaxAdrnID < tmpadrnbuff.bitmapno + maxid) {
			MaxAdrnID = tmpadrnbuff.bitmapno + maxid;
		}
		if (tmpadrnbuff.attr.bmpnumber != 0) {
			bitmapnumbertable[tmpadrnbuff.attr.bmpnumber] = tmpadrnbuff.bitmapno + maxid;
		}
	}
	fclose(Addrbinfp);
#endif
	return TRUE;
}

#endif


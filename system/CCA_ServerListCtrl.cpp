/**** SYSTEM INCLUDE ****/

#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include <winnls32.h>
#include <ime.h>
#include <imm.h>
#include <tlhelp32.h>
#include "../caryIme.h"
#include "../resource.h"
#include "../systeminc/netmain.h"
#include "../systeminc/battleMap.h"
#include "../systeminc/battleProc.h"
#include "../systeminc/netmain.h"
#include "../systeminc/login.h"
#include "../systeminc/map.h"
#ifdef _REMAKE_20
#include "../MMOGprotect.h"
#endif
#ifdef _TALK_WINDOW
#include "../systeminc/talkwindow.h"
#endif

#ifdef  _STONDEBUG_
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif



//JL
#include "../systeminc/JL_func.h"


#include "../systeminc/netmain.h"
extern struct gameserver gmsv[];
extern struct gamegroup gmgroup[];
extern int nGroup;



#define getStringFromIndexWithDelim( src, delim, index, buf, buflen ) getStringFromIndexWithDelim_body( src, delim, index, buf, buflen, __FILE__, __LINE__ )
#define IS_2BYTEWORD( _a_ ) ( (char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF) )
char* strncpy2(char* dest, const char* src, size_t n)
{
	if (n > 0){
		char*   d = dest;
		const char*   s = src;
		int i;
		for (i = 0; i<n; i++){
			if (*(s + i) == 0){
				/*  戊疋□仄云歹匀凶日 NULL   侬毛  木月   */
				*(d + i) = '\0';
				return dest;
			}
			if (*(s + i) & 0x80){
				*(d + i) = *(s + i);
				i++;
				if (i >= n){
					*(d + i - 1) = '\0';
					break;
				}
				*(d + i) = *(s + i);
			}
			else
				*(d + i) = *(s + i);
		}
	}
	return dest;
}

void strncpysafe(char* dest, const size_t n,
	const char* src, const int length)
{
	/*
	* src 井日 dest 卞 length 戊疋□允月
	* strcpy, strncpy 匹反 dest 方曰 戊疋□允月汹互
	*   五中凛卞裟少午,丢乒伉陆失弁本旦互粟月.
	* 仇及楮醒匹反｝strlen( src ) 午 length 及凝今中幻丹
	* (  端卞戊疋□允月汹) 午 dest 及扔奶术毛  屯化｝
	* strcpysafe 午  元仪毛允月［
	*/

	int Short;
	Short = min(strlen(src), length);

	/* NULL  侬 毛哔  仄凶  胜 */
	if (n < Short + 1){
		/*
		* 田永白央互箫曰卅中及匹 n - 1(NULL  侬)
		* 匹 strncpy 毛裟少
		*/
		strncpy2(dest, src, n - 1);
		dest[n - 1] = '\0';

	}
	else if (n <= 0){
		return;
	}
	else{
		/*
		* 田永白央反蜗坌卞丐月及匹 Short 匹strncpy毛裟少
		* 卅云 src 卞反 Short 及赢今  卞 NULL 互卅中及匹｝
		* dest 卞反 馨笛仄化云仁［
		*/

		strncpy2(dest, src, Short);
		dest[Short] = '\0';

	}
}

void strcpysafe(char* dest, size_t n, const char* src)
{
	/*
	* src 井日 dest 尺戊疋□允月.
	* strcpy, strncpy 匹反 dest 方曰 戊疋□允月汹互
	*   五中凛卞裟少午,丢乒伉陆失弁本旦互粟月.
	* 仇木毛  什啃卞, strncpy 互丐月互 strlen( src ) 互 n 方曰
	*   五中凛卞反, dest 及    互 NULL   侬午反卅日卅中.
	*
	* 仄凶互匀化 dest 及  五今方曰 src 及幻丹互赢中凛卞反
	* n-1 匹 strncpy 毛允月. 凝今中凛反公及引引戊疋□允月
	*
	* n 互  及凛反云井仄仁卅月及匹  及凛反 窒手仄卅中［
	*
	*/
	// Nuke +1 (08/25): Danger if src=0
	if (!src) {
		*dest = '\0';
		return;
	}
	if (n <= 0)        /* 窒手仄卅中   */
		return;

	/*  仇及凛鳔匹｝ n >= 1 动晓互瑁烂  */
	/*  NULL  侬毛哔  仄化  胜允月  */
	else if (n < strlen(src) + 1){
		/*
		* 田永白央互箫曰卅中及匹 n - 1(NULL  侬)
		* 匹 strncpy 毛裟少
		*/
		strncpy2(dest, src, n - 1);
		dest[n - 1] = '\0';
	}
	else
		strcpy(dest, src);

}

char * ScanOneByte(char *src, char delim)
{
	// Nuke
	if (!src) return NULL;

	//   侬  互卅仁卅月引匹腹绸
	for (; src[0] != '\0'; src++){
		if (IS_2BYTEWORD(src[0])){
			// 蝈剩分［公及桦宁反ㄠ田奶玄芴坌卞褡引六月［
			// 凶分仄ㄠ田奶玄仄井卅中桦宁反公丹仄卅中
			if (src[1] != 0){
				src++;
			}
			continue;
		}
		//   剩分匀凶［仇仇匹覆擂及  侬午  胜
		if (src[0] == delim){
			return src;
		}
	}
	// 伙□皿  仃凶日苇勾井日卅井匀凶［
	return NULL;
}

BOOL getStringFromIndexWithDelim_body(char* src, char* delim, int index,
	char* buf, int buflen,
	char *file, int line)
{//ttom this function all change,copy from the second
	int i;          /* 伙□皿  醒 */
	int length = 0;  /* 潸曰请仄凶  侬  及赢今 */
	int addlen = 0;   /* 箫今木月赢今 */
	int oneByteMode = 0; /* ㄠ田奶玄乒□玉井＂ */

	if (strlen(delim) == 1){ // 腹绸互ㄠ田奶玄卅日ㄠ田奶玄乒□玉卞允月
		oneByteMode = 1;// 公及端ㄡ田奶玄  侬反民尼永弁仄卅中
	}
	for (i = 0; i < index; i++){
		char* last;
		src += addlen;/* 心勾井匀凶赢今毛箫允 */

		if (oneByteMode){
			// ㄠ田奶玄乒□玉分匀凶日仇切日匹腹绸
			last = ScanOneByte(src, delim[0]);
		}
		else{
			last = strstr(src, delim);  /* 苇尥仃月 */
		}
		if (last == NULL){
			/*
			* 心勾井日卅井匀凶及匹允屯化戊疋□仄化 return［
			*/
			strcpysafe(buf, buflen, src);

			if (i == index - 1)
				/*切斤丹升心勾井匀凶*/
				return TRUE;

			/*心勾井日卅井匀凶*/
			return FALSE;
		}

		/*
		* 心勾井匀凶赭午  赓及匏  及犒毛菲户月
		* 勾引曰嗉濠日木化中月  侬  及赢今
		*/
		length = last - src;

		/*
		* 戚及伙□皿及啃卞心勾井匀凶赢今午 delim 及赢今毛箫仄化云仁
		*/
		addlen = length + strlen(delim);
	}
	strncpysafe(buf, buflen, src, length);

	return TRUE;
}


int split_damage(char* src, char delim, int* pArraySplitAddr, unsigned int maxCount)
{

	unsigned int count = 1;	//最少能分割1个成员(当分隔符不存在的时候，返回自身)
	*pArraySplitAddr = (int)src;
	pArraySplitAddr++;
	int i = 0;
	while (*(src + i)){	//当非0的时候继续循环
		if (*(src + i) == delim){	//如果找到了分隔符
			if (count >= maxCount){
				return count;
			}
			*(src + i) = 0;	//将分隔符置0
			*pArraySplitAddr = (int)(src + i + 1);	//将分隔符的下一个字节地址储存
			pArraySplitAddr++;
			count++;
		}
		i++;
	}

	return count;
}

void 解析登陆列表明文(char* src, char* dest)
{
	int array_split[5000];
	int count = split_damage(src, ';', array_split, 5000);

	int index = 1;
	int count_大服数量 = atoi((char*)array_split[index]);
	index++;

	*dest = 0;

	for (int i = 0; i < count_大服数量; i++)
	{
		index++; //跳过0   0;本地;3;192.168.0;本地;110:9065;雨水;146:9070;立春;134:9075;;
		char* 主机名 = (char*)array_split[index];
		index++;
		int count_小服数量 = atoi((char*)array_split[index]);
		index++;

		// 主机名|副机|IP|副机|IP\n主机名|副机|IP
		lstrcatA(dest, 主机名);



		char* IP头 = (char*)array_split[index];
		index++;

		for (int j = 0; j < count_小服数量; j++)
		{
			char* 副机名 = (char*)array_split[index];
			index++;
			char* IP尾 = (char*)array_split[index];
			index++;

			if (index > count)
			{
				return;
			}

			char buf2[100];
			wsprintfA(buf2, "|%s|%s.%s", 副机名, IP头, IP尾);
			lstrcatA(dest, buf2);

		}
		index++;

		lstrcatA(dest, "\n");

	}

}

void 解密MAX内联代码(char *pMax, int file_size)
{
	*(int *)pMax = 0x1234;
	*(int *)(pMax + 4) = 0x5678;
	*(int *)(pMax + 8) = 0x9ABC;
	*(int *)(pMax + 12) = 0xDEF0;
	__asm{
		pushad

			mov ecx, [file_size]; 文件大小->ecx
			mov edi, ecx
			and ecx, 0x80000007
			jns sa_8018_0048C866
			dec ecx
			or ecx, 0xFFFFFFF8
			inc ecx

		sa_8018_0048C866 :

		mov eax, edi
			je sa_8018_0048C878
			sub eax, ecx
			cdq
			and edx, 0x7
			add eax, edx
			sar eax, 0x3
			inc eax
			jmp sa_8018_0048C881

		sa_8018_0048C878 :

		cdq
			and edx, 0x7
			add eax, edx
			sar eax, 0x3

		sa_8018_0048C881 :

						 test eax, eax; 根据文件大小计算出来的eax
						 jle sa_8018_0048C89F
						 mov esi, pMax
						 add esi, 16
						 mov edi, eax

					 sa_8018_0048C88B :

		mov ecx, pMax
			push esi
			push ecx
			call 解密max; <= Jump / Call Address Not Resolved; 解密max文件
			add esp, 0x8
			add esi, 0x8
			dec edi
			jnz sa_8018_0048C88B

		sa_8018_0048C89F :
		jmp ooookkkk

		解密max : ; <= Procedure Start

				mov eax, dword ptr ss : [esp + 0x8]
				mov ecx, dword ptr ss : [esp + 0x4]
				push - 0x20
				push eax
				push ecx
				call max加密解密; <= Jump / Call Address Not Resolved
				add esp, 0xC
				retn; <= Procedure End



			max加密解密 : ; <= Procedure Start

					  mov edx, dword ptr ss : [esp + 0xC]
					  push ebx
					  push ebp
					  push esi
					  push edi
					  mov edi, dword ptr ss : [esp + 0x18]
					  xor esi, esi
					  mov ecx, dword ptr ds : [edi]
					  mov eax, dword ptr ds : [edi + 0x4]
					  test edx, edx
					  jle sa_8018_0048C622
					  imul edx, edx, 0x9E3779B9
					  test edx, edx
					  mov dword ptr ss : [esp + 0x18], edx
					  je sa_8018_0048C675
					  mov edx, dword ptr ss : [esp + 0x14]

				  sa_8018_0048C5CD :

								   mov ebx, eax
								   mov ebp, eax
								   sar ebx, 0x5
								   shl ebp, 0x4
								   xor ebx, ebp
								   mov ebp, esi
								   and ebp, 0x3
								   add ebx, dword ptr ds : [edx + ebp * 4]
								   mov ebp, esi
								   xor ebp, eax
								   sub esi, 0x61C88647
								   add ebp, ecx
								   lea ecx, dword ptr ds : [ebx + ebp]
								   mov ebx, ecx
								   mov ebp, ecx
								   sar ebx, 0x5
								   shl ebp, 0x4
								   xor ebx, ebp
								   mov ebp, esi
								   sar ebp, 0xB
								   and ebp, 0x3
								   add ebx, dword ptr ds : [edx + ebp * 4]
								   mov ebp, esi
								   xor ebp, ecx
								   add ebp, eax
								   lea eax, dword ptr ds : [ebx + ebp]
								   mov ebx, dword ptr ss : [esp + 0x18]
								   cmp esi, ebx
								   jnz sa_8018_0048C5CD
								   mov dword ptr ds : [edi], ecx
								   mov dword ptr ds : [edi + 0x4], eax
								   pop edi
								   pop esi
								   pop ebp
								   pop ebx
								   retn

							   sa_8018_0048C622 :

		imul edx, edx, 0x61C88647
			test edx, edx
			je sa_8018_0048C675
			mov esi, dword ptr ss : [esp + 0x14]

		sa_8018_0048C630 :

						 mov ebx, ecx
						 mov ebp, ecx
						 sar ebx, 0x5
						 shl ebp, 0x4
						 xor ebx, ebp
						 mov ebp, edx
						 sar ebp, 0xB
						 and ebp, 0x3
						 add ebx, dword ptr ds : [esi + ebp * 4]
						 mov ebp, edx
						 xor ebp, ecx
						 add edx, 0x61C88647
						 add ebx, ebp
						 sub eax, ebx
						 mov ebx, eax
						 mov ebp, eax
						 sar ebx, 0x5
						 shl ebp, 0x4
						 xor ebx, ebp
						 mov ebp, edx
						 and ebp, 0x3
						 add ebx, dword ptr ds : [esi + ebp * 4]
						 mov ebp, edx
						 xor ebp, eax
						 add ebx, ebp
						 sub ecx, ebx
						 test edx, edx
						 jnz sa_8018_0048C630

					 sa_8018_0048C675 :

		mov dword ptr ds : [edi], ecx
			mov dword ptr ds : [edi + 0x4], eax
			pop edi
			pop esi
			pop ebp
			pop ebx
			retn; <= Procedure End



		ooookkkk :
		popad
	}

}

BOOL serverListDecodeComplete = false;
#define serverListEncodeLenght 2048
//
//数据写入serverListEncodeArray，加密数据来着
//
char serverListEncodeArray[serverListEncodeLenght] = { 0 };
char serverListDecodeArray[serverListEncodeLenght];
int serverListEncodeRealLenght = -1;


/*
extern struct gameserver gmsv[];
extern struct gamegroup gmgroup[];
*/
void 写入serverMax(char* pEncode, int LenEncode)
{
	//测试
	//sprintf(serverListDecodeArray, "%s\0", "情缘|电信1|218.92.147.50:9065\nCC|CC|amoijing.gicp.net:9065\n本地|本地|192.168.0.110:9065\n本地|本地2|192.168.0.110:9065\n本地|本地3|192.168.0.110:9065\n");
	memset(serverListDecodeArray, 0, serverListEncodeLenght);

	if (*pEncode == '\0')
	{
		sprintf(serverListDecodeArray, "%s\0", "运行|立秋|222.186.51.178:9065\n运行|测试一线|amoijing.gicp.net:9065\n运行|本地一线|192.168.1.25:9065\n");
	}

	if (serverListDecodeComplete)
	{
		//如果解码完成什么都不做
		return;
	}
	else
	{
		//计算加密的encodeArray长度
		serverListEncodeRealLenght = LenEncode;
		//解码

		if (*pEncode != '\0')
		{
			解密MAX内联代码(pEncode, serverListEncodeRealLenght);
			解析登陆列表明文((char*)(pEncode + 16), serverListDecodeArray);
		}
		else
		{
			//如果没有写入serverListEncodeArray
			//那么就用默认的数据
		}
		//解码完成，写入列表数据到原有的逻辑
		//情缘电信|立秋-电信一线|222.186.51.178:9065|立春-电信二线|222.186.51.178:9066|雨水-电信三线|222.186.51.178:9067|谷雨-电信四线|222.186.51.178:9068\n
		//情缘网通 | 秋分 - 网通一线 | 153.37.74.177:9065 | 白露 - 网通二线 | 153.37.74.177 : 9066 | 惊蛰 - 网通三线 | 153.37.74.177 : 9067 | 夏至 - 网通四线 | 153.37.74.177 : 9068\n
		//	CC | CC | amoijing.gicp.net : 9065\n
		//	本地 | 本地 | 192.168.0.110 : 9065\n

		int tmcount = 0; 
		char *strpos = NULL;
		char *tmstrposSearch = &serverListDecodeArray[0];
		//统计 \n的列表数量 

		char* array1[100]; //  \n分割
		char* array2[5];   //  |分割一次
		char* array3[50];  //  |分割
		char* array4[5];   //  :分割
		int n1 = split_damage(serverListDecodeArray, '\n', (int*)array1, 100);
		for (int i = 0; i < n1; i++)
		{
			int n2 = split_damage(array1[i], '|', (int*)array2, 2);
			if (n2 != 2) goto _ret;

			int n3 = split_damage(array2[1], '|', (int*)array3, 50);
			for (int j = 0; j < n3; )
			{
				gmsv[tmcount].used = '1';
				lstrcpy(gmsv[tmcount].name, array3[j]);
				int n4 = split_damage(array3[j + 1], ':', (int*)array4, 3);
				if (n4 != 2)  goto _ret;
				lstrcpy(gmsv[tmcount].ipaddr, array4[0]);
				lstrcpy(gmsv[tmcount].port, array4[1]);
				j = j + 2;
				tmcount++;
			}
		}
		_ret:
		gmgroup->num = (char)tmcount;
	//	for (; ;)
	//	{
	//		strpos = strstr(tmstrposSearch, "\n");
	//		if (strpos != NULL)
	//		{

	//			//修改 gmsv信息
	//			char    splitBuf[512];
	//			getStringFromIndexWithDelim(serverListDecodeArray, "\n", tmcount +1, splitBuf, sizeof(splitBuf));
	//			gmsv[tmcount].used = '1';
	//			getStringFromIndexWithDelim(splitBuf, "|", 2, gmsv[tmcount].name, sizeof(gmsv[tmcount].name));
	//			char    serverBuf[512];
	//			getStringFromIndexWithDelim(splitBuf, "|", 3, serverBuf, sizeof(serverBuf));
	//			getStringFromIndexWithDelim(serverBuf, ":", 1, gmsv[tmcount].ipaddr, sizeof(gmsv[tmcount].ipaddr));
	//			getStringFromIndexWithDelim(serverBuf, ":", 2, gmsv[tmcount].port, sizeof(gmsv[tmcount].port));

	//			tmcount++; 
	//			tmstrposSearch = strpos + 1;


	//		}
	//		else
	//		{
	//			break;
	//		}
	//	}
	//	//已统计出具体的列表数量

	//	//修改group 信息
	//	gmgroup->num = (char)tmcount;
	//	//写入完成
	//	serverListDecodeComplete = true;
	}
	return;
}

void 测试解码serverMax()
{
	FILE *pf;
	if ((pf = fopen("server.max", "r")) == NULL)
	{
	}
	else
	{
		fseek(pf, 0, SEEK_END); //定位到文件末 
		int nFileLen = ftell(pf); //文件长度 
		fseek(pf, 0, SEEK_SET); //定位到文件末 

		char * pMax = (char *)malloc(nFileLen);
		char * pMax1 = (char *)malloc(2048);
		memset(pMax, 0, nFileLen);
		memset(pMax1, 0, 2048);
		fread(pMax, sizeof(char), nFileLen, pf);
		解密MAX内联代码(pMax, nFileLen);
		解析登陆列表明文((char*)(pMax + 16), pMax1);
		//情缘|电信1|218.92.147.50:9065\nCC|CC|amoijing.gicp.net:9065\n本地|本地|192.168.0.110:9065\n
		fclose(pf);
		free(pMax);
		free(pMax1);
	}
}

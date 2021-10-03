#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "../image.h"

#define ASCII(a) a-'A'+10
#define ASCII_DEC(a) a-'0'+ 35

#define FONT_BUFFER_SIZE 1024

FONT_BUFFER FontBuffer[FONT_BUFFER_SIZE];
int FontCnt = 0;
int FontZenkauWidth;
int	FontHankakuWidth;
int MessageBoxNew(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
#ifdef _SUNDAY_STR_SEARCH
char* sunday(char* str, char* subStr){
	const int maxSize = 256;
	int next[maxSize];
	int strLen = strlen(str);
	int subLen = strlen(subStr);
	int i, j, pos;
	for (i = 0; i<maxSize; i++){
		next[i] = subLen + 1;
	}
	for (i = 0; i<subLen; i++){
		next[(unsigned char)subStr[i]] = subLen - i;//计算子串中的字符到字符串结尾的\0之间的距离
	}
	pos = 0;
	while (pos <= (strLen - subLen)){
		i = pos;
		for (j = 0; j<subLen; j++, i++){
			if (str[i] != subStr[j]){
				pos += next[(unsigned char)str[pos + subLen]];//向后移动
				break;
			}

		}
		if (j == subLen){//找到字串，返回
			return str + pos;
		}
	}
	return NULL;
}
#endif


#ifdef _FONT_SIZE
int StockFontBufferExt(int x, int y, char fontPrio, int color, char *str, BOOL hitFlag, int size){
	if (FontCnt >= FONT_BUFFER_SIZE) return -2;
	FontBuffer[FontCnt].x = x;
	FontBuffer[FontCnt].y = y;
	FontBuffer[FontCnt].fontPrio = fontPrio;
	FontBuffer[FontCnt].color = color;
	FontBuffer[FontCnt].hitFlag = hitFlag;
	extern int 编码;
	extern int 繁体开关;
	if (繁体开关) {
		char 繁体[1024] = { 0 };
		LCMapString(0x804, 0x4000000, str, strlen(str), 繁体, 1024);
		if (编码 == 950) {
			extern char* GB2312ToBIG5(const char* szBIG5String);
			strcpy(FontBuffer[FontCnt].str, GB2312ToBIG5((const char *)繁体));
		}
		else
			strcpy(FontBuffer[FontCnt].str, 繁体);
	}else {
		if (编码 == 950) {
			extern char* GB2312ToBIG5(const char* szBIG5String);
			strcpy(FontBuffer[FontCnt].str, GB2312ToBIG5((const char *)str));
		}
		else
			strcpy(FontBuffer[FontCnt].str, str);
	}
	FontBuffer[FontCnt].size = size;
	return FontCnt++;
}

int StockFontBuffer(int x, int y, char fontPrio, int color, char *str, BOOL hitFlag){
	return StockFontBufferExt(x, y, fontPrio, color, str, hitFlag, 0);
}
#else

int StockFontBuffer(int x, int y, char fontPrio, int color, char *str, BOOL hitFlag){
	if (FontCnt >= FONT_BUFFER_SIZE) return -2;
	FontBuffer[FontCnt].x = x;
	FontBuffer[FontCnt].y = y;
	FontBuffer[FontCnt].fontPrio = fontPrio;
	FontBuffer[FontCnt].color = color;
	FontBuffer[FontCnt].hitFlag = hitFlag;
	extern int 编码;
	extern int 繁体开关;
	if (繁体开关) {
		char 繁体[1024] = { 0 };
		LCMapString(0x804, 0x4000000, str, strlen(str), 繁体, 1024);
		strcpy(FontBuffer[FontCnt].str, 繁体);
	}
	else {
		strcpy(FontBuffer[FontCnt].str, str);
	}
	return FontCnt++;
}
#endif

HDC FontSizeHdc = NULL;
HDC hScrDC = NULL;
void CreatFontHdc(){
	hScrDC = CreateDC("FontHdc", NULL, NULL, NULL);
	FontSizeHdc = CreateCompatibleDC(hScrDC);
	if (FontSizeHdc) {
#ifdef _NEWFONT_
		extern int 编码;
		char strfame[128];
		if (编码 == 950) {
			sprintf(strfame, "Microsoft JhengHei");
		}
		else {
			sprintf(strfame, "Microsoft JhengHei");
		}
		HFONT font = CreateFont(FONT_SIZE1, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 1,
			0, 0, 0, 17, TEXT("굴림체"));
#else
		extern int 编码;
		char strfame[128];
		HFONT font;
		if (编码 == 950) {
			sprintf(strfame, "Microsoft JhengHei");
			font = CreateFont(FONT_SIZE2, 0, 0, 0, FW_NORMAL, HANGUL_CHARSET, FALSE, FALSE, 1,
				0, 0, 0, 17, TEXT("굴림체"));
		}
		else {
#ifdef 加载新字体
			HFONT JL_FONT();
			font = JL_FONT();
#else
			//font = CreateFont(FONT_SIZE1, 8, 0, 0, 200, FALSE, FALSE, FALSE, 134,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, (LPCTSTR)"楷体");
			font = CreateFont(FONT_SIZE1, 0, 0, 0, 400, FALSE, FALSE, FALSE, HANGUL_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_ROMAN, TEXT("굴림체"));
#endif
		}


#endif
		SelectObject(FontSizeHdc, font);
	}else {
		MessageBoxNew(NULL, "创建HDC错误！", "ＳｔｏｎｅＡｇｅ", NULL);
		exit(0);
	}
}
int getTextLength(char * str){
	SIZE fontsize;
	GetTextExtentPoint32(FontSizeHdc, (LPCSTR)str, strlen(str), (LPSIZE)&fontsize);
	return fontsize.cx;
}


#ifdef 聊天表情
void delFontBuffer(CHAT_BUFFER *chatbuffer){

	CHAT_BUFFER *pNextBuffer = chatbuffer->NextChatBuffer;
	while (pNextBuffer){//如果链中还存在结点
		CHAT_BUFFER* pTemp;
		pTemp = pNextBuffer;
		pNextBuffer = pNextBuffer->NextChatBuffer;
		free(pTemp);
		pTemp = NULL;
	}
	chatbuffer->NextChatBuffer = NULL;
}

int getStrColor(char* str){
	int color = -1;
	char *temp1 = sunday(str, "color=");
	if (temp1){
		color = atoi(temp1 + 6);
	}

	return color;
}

void getStrStr(char* strIn, char* strOut){
	char *temp1 = sunday(strIn, "str=");
	char* temp2 = sunday(temp1, "}");
	int a = temp2 - temp1 - 4;
	memcpy(strOut, temp1+4,a);
	strOut[a] = 0;
}

bool getStrPos(char* str, CHAT_BUFFER *chatbuffer){
	//printf("\n%s",str);
	char *temp1 = sunday(str, "pos=");
	int len = lstrlen(str);
	char *pbuf = new char[len + 1];
	memset(pbuf, 0, len + 1);
	memcpy(pbuf, str, len);

	char* array[5];
	int count = split_damage(pbuf + 4, ',', (int*)array, 3);
	if (count != 3){
		delete[] pbuf;
		return false;
	}
	chatbuffer->map = atoi(array[0]+1);  //跳过=符号
	chatbuffer->posx = atoi(array[1]);
	chatbuffer->posy = atoi(array[2]);
	delete[] pbuf;
	return true;
}

int getStrPetindex(char* str){
	int index = -1;
	char *temp1 = sunday(str, "pet=");
	if (temp1){
		index = atoi(temp1 + 4);
	}
	return index;
}

bool getStrPet(char* str, CHAT_BUFFER *chatbuffer){
	//称号|100872|0|140|1735|452|202|258|,str=佩露夏}
	char *temp1 = sunday(str, "pet=");
	int len = lstrlen(str);
	char *pbuf = new char[len + 1];
	memset(pbuf, 0, len + 1);
	memcpy(pbuf, str, len);

	char* array[10];
	int count = split_damage(pbuf + 5, '|', (int*)array, 8);
	if (count != 8){
		delete[] pbuf;
		return false;
	}
	chatbuffer->image = atoi(array[1]);
	chatbuffer->reborn = atoi(array[2]);
	chatbuffer->lv = atoi(array[3]);
	chatbuffer->hp = atoi(array[4]);
	chatbuffer->gong = atoi(array[5]);
	chatbuffer->fang = atoi(array[6]);
	chatbuffer->min = atoi(array[7]);

	return true;

}

bool getStrExtraData(char* strIn, char* strOut){
	return false;
}

//TODO 文字扩展
void MyNewStockFontBuffer(CHAT_BUFFER *chatbuffer, int x, unsigned char color, char *str, int size, int id){
	//  1、之前的表情 #1 变为{ #1 }
	//	3、{ c = 2,pet = 고르고르 | 称号 | 攻 | 防...,str = 卖빨간 호랑이 } 表示：c为color，str为显示文字，pet为鼠标移动上去，显示相关的图档及附加信息
	//	3、{ c = 2,item = 小的枪 | 称号 | 攻 | 防...,str = 小的枪 } 表示：c为color，str为显示文字，item为鼠标移动上去，显示相关的图档及附加信息
	//	4、{ c = 2,pos = map,x,y,str = xx坐标 } 表示：c为color，str为显示文字，pos为鼠标点击可寻路

	if (!str[0])return;
	memset(chatbuffer, 0, sizeof(CHAT_BUFFER));
	chatbuffer->fontsize = size;
	chatbuffer->id = id;
	char outText[512];
	//查找 { 
	char *temp1 = sunday(str, "{");
	if (temp1){
		if (temp1 == str){
			char *temp2 = sunday(str, "}");
			if (temp2){
				int strl = temp2 - str;
				memcpy(outText, str + 1, strl );   //outText为#之前的拷贝
				outText[strl-1] = 0x0;
				char* pStr = outText;
				if (*pStr == '#'){
					//检测表情
					int cnt_int = 0;
					int i = 1;
					for (i; i<4; i++) {
						if (pStr[i] >= '0'&& pStr[i] <= '9') {
							cnt_int *= 10;
							cnt_int += pStr[i] - '0';
						}else{
							break;
						}
					}

					if (cnt_int>0 && cnt_int <= EXPRESSION_NOID_NUM + 1) {
						chatbuffer->x = x;
						chatbuffer->BmpNo = EXPRESSION_NOID_START + cnt_int - 1;
						pStr += i;
						x += 26;   //表情的宽度
						chatbuffer->NextChatBuffer = (CHAT_BUFFER *)calloc(1, sizeof(CHAT_BUFFER));
						MyNewStockFontBuffer(chatbuffer->NextChatBuffer, x , color, temp1 + strl + 1, size, id);  //将后续文字继续遍历输出
					}else{
						//如果不是表情，则把{#xxx}输出
						memcpy(outText, str, strl+1);   
						outText[strl+1] = 0x0;
						SIZE fontsize;
						GetTextExtentPoint32(FontSizeHdc, (LPCSTR)outText, strl+1, (LPSIZE)&fontsize);
						chatbuffer->color = color;
						chatbuffer->x = x;
						strcpy(chatbuffer->buffer, outText);
						chatbuffer->NextChatBuffer = (CHAT_BUFFER *)calloc(1, sizeof(CHAT_BUFFER));
						MyNewStockFontBuffer(chatbuffer->NextChatBuffer, x + fontsize.cx, color, temp1+ strl+1, size, id);  //将后续文字继续遍历输出
					}
				}else{
					//如果不是表情的#
					int c = getStrColor(outText);
					if (c == -1)c = color;
					char* pTextStr = sunday(str, "str=");
					char* pTextPos = sunday(str, "pos=");
#ifdef _NB_秀宠物
					char* pTextPet = sunday(str, "pet=");
#endif
					char* pTextItem = sunday(str, "item=");

					if (!pTextPos && !pTextPet && !pTextItem && pTextStr){
						//如果只有str，则变色输出
						getStrStr(str, outText);
						SIZE fontsize;
						GetTextExtentPoint32(FontSizeHdc, (LPCSTR)outText, lstrlen(outText), (LPSIZE)&fontsize);
						chatbuffer->color = c;
						chatbuffer->x = x;
						strcpy(chatbuffer->buffer, outText);
						chatbuffer->NextChatBuffer = (CHAT_BUFFER *)calloc(1, sizeof(CHAT_BUFFER));
						MyNewStockFontBuffer(chatbuffer->NextChatBuffer, x + fontsize.cx, color, temp1 + strl + 1, size, id);  //将后续文字继续遍历输出
						return;
					}

					if (!pTextPos && !pTextPet && !pTextItem || pTextStr == NULL){
						//如果找不到对应文本，则视为普通的文本，照常输出
						memcpy(outText, str, strl + 1);   
						outText[strl + 1] = 0x0;
						SIZE fontsize;
						GetTextExtentPoint32(FontSizeHdc, (LPCSTR)outText, strl + 1, (LPSIZE)&fontsize);
						chatbuffer->color = c;
						chatbuffer->x = x;
						strcpy(chatbuffer->buffer, outText);
						chatbuffer->NextChatBuffer = (CHAT_BUFFER *)calloc(1, sizeof(CHAT_BUFFER));
						MyNewStockFontBuffer(chatbuffer->NextChatBuffer, x + fontsize.cx, color, temp1 + strl + 1, size, id);  //将后续文字继续遍历输出
						return;
					}

					if (pTextPos){
						//获取坐标
						if (getStrPos(str, chatbuffer)){
							getStrStr(str, outText);
							SIZE fontsize;
							GetTextExtentPoint32(FontSizeHdc, (LPCSTR)outText, lstrlen(outText), (LPSIZE)&fontsize);
							chatbuffer->color = c;
							chatbuffer->x = x;
							strcpy(chatbuffer->buffer, outText);
							chatbuffer->NextChatBuffer = (CHAT_BUFFER *)calloc(1, sizeof(CHAT_BUFFER));
							MyNewStockFontBuffer(chatbuffer->NextChatBuffer, x + fontsize.cx, color, temp1 + strl + 1, size, id);  //将后续文字继续遍历输出
							return;
						}
#ifdef _NB_秀宠物
					}else if (pTextPet){//获取宠物
						int 宠物索引 = getStrPetindex(str);
						getStrStr(str, outText);
						chatbuffer->image = 宠物索引;
						SIZE fontsize;
						GetTextExtentPoint32(FontSizeHdc, (LPCSTR)outText, lstrlen(outText), (LPSIZE)&fontsize);
						chatbuffer->color = 11;
						chatbuffer->x = x;
						strcpy(chatbuffer->buffer, outText);
						chatbuffer->NextChatBuffer = (CHAT_BUFFER *)calloc(1, sizeof(CHAT_BUFFER));
						MyNewStockFontBuffer(chatbuffer->NextChatBuffer, x + fontsize.cx, color, temp1 + strl + 1, size, id);  //将后续文字继续遍历输出
						return;
#endif
					}else if (pTextItem){//获取道具
					}
					__asm nop;
				}

			
			}
			__asm nop;
		}
		if (temp1 != str){//如果不相等，则{之前的文字要先输出
			int strl = temp1 - str;
			memcpy(outText, str, strl);   //outText为#之前的拷贝
			outText[strl] = 0x0;
			SIZE fontsize;
			GetTextExtentPoint32(FontSizeHdc, (LPCSTR)outText, strl, (LPSIZE)&fontsize);
			chatbuffer->color = color;
			chatbuffer->x = x;
			strcpy(chatbuffer->buffer, outText);
			chatbuffer->NextChatBuffer = (CHAT_BUFFER *)calloc(1, sizeof(CHAT_BUFFER));
			MyNewStockFontBuffer(chatbuffer->NextChatBuffer, x + fontsize.cx, color, temp1, size, id);  //将后续文字继续遍历输出
			return;
		}
	}else{
		//如果没有{则正常输出该段文字
		chatbuffer->color = color;
		chatbuffer->x = x;
		strcpy(chatbuffer->buffer, str);
	}


}


void NewStockFontBuffer(CHAT_BUFFER *chatbuffer, int x, unsigned char color, char *str, int size,int id){
	MyNewStockFontBuffer(chatbuffer, x, color, str, size, id);
	return;
	if (!str[0]) {
		return;
	}
	memset(chatbuffer, 0, sizeof(CHAT_BUFFER));
	chatbuffer->fontsize = size;
	chatbuffer->id = id;
	char outText[512];
	char *temp = sunday(str, "#");
	if (temp) {
		if (temp != str) {
			int strl = temp - str;
			memcpy(outText, str, strl);   //outText为#之前的拷贝
			outText[strl] = 0x0;
			SIZE fontsize;
			GetTextExtentPoint32(FontSizeHdc, (LPCSTR)outText, strl, (LPSIZE)&fontsize);
			chatbuffer->color = color;
			chatbuffer->x = x;
			strcpy(chatbuffer->buffer, outText);
			chatbuffer->NextChatBuffer = (CHAT_BUFFER *)calloc(1, sizeof(CHAT_BUFFER));
			NewStockFontBuffer(chatbuffer->NextChatBuffer, x + fontsize.cx, color, temp, size,id);
		}else{
			int cnt_int = 0;
			int i = 1;
			for (i; i<4; i++) {
				if (temp[i] >= '0'&& temp[i] <= '9') {
					cnt_int *= 10;
					cnt_int += temp[i] - '0';
				}
				else {
					break;
				}
			}
			if (cnt_int>0 && cnt_int <= EXPRESSION_NOID_NUM + 1) {
				chatbuffer->x = x;
				chatbuffer->BmpNo = EXPRESSION_NOID_START + cnt_int - 1;
				temp += i;
				x += 26;   //表情的宽度
			}else{
				memcpy(outText, temp, i);
				outText[i] = 0x0;
				SIZE fontsize;
				GetTextExtentPoint32(FontSizeHdc, (LPCSTR)outText, i, (LPSIZE)&fontsize);
				chatbuffer->color = color;
				chatbuffer->x = x;
				strcpy(chatbuffer->buffer, outText);
				x += fontsize.cx;
				temp += i;
			}
			chatbuffer->NextChatBuffer = (CHAT_BUFFER *)calloc(1, sizeof(CHAT_BUFFER));
			NewStockFontBuffer(chatbuffer->NextChatBuffer, x, color, temp, size,id);
		}
	}else{
		chatbuffer->color = color;
		chatbuffer->x = x;
		strcpy(chatbuffer->buffer, str);
	}
}
#endif

void StockFontBuffer2(STR_BUFFER *strBuffer){
	int lineDist = 0; //行距
	int splitPoint = 0;
	int bakSplitPoint, cursor;
	BOOL SetCursor = FALSE;
	char splitStr[256];
	if (FontCnt >= FONT_BUFFER_SIZE) {
		strBuffer->hitFontNo = -2;
		return;
	}
	//需要分行时
	if (strBuffer->lineLen != 0) {
		cursor = strBuffer->cursor;
		while (strlen(strBuffer->buffer + splitPoint) >= strBuffer->lineLen) {
			bakSplitPoint = splitPoint;
			//copy一行的字串
			strncpy_s(splitStr, strBuffer->buffer + splitPoint, strBuffer->lineLen);
			*(splitStr + strBuffer->lineLen) = NULL;
			if (GetStrLastByte(splitStr) == 3) {
				//分割到中DBCS时，退回一个byte
				splitPoint = strBuffer->lineLen - 1 + splitPoint;
				*(splitStr + strBuffer->lineLen - 1) = NULL;
			}
			else
				splitPoint = strBuffer->lineLen + splitPoint;
			StockFontBuffer(strBuffer->x, strBuffer->y + lineDist, strBuffer->fontPrio, 0, splitStr, 0);
			if (cursor >= bakSplitPoint && cursor<splitPoint) {
				strBuffer->imeX = strBuffer->x + (cursor - bakSplitPoint)*(FONT_SIZE >> 1);
				strBuffer->imeY = strBuffer->y + lineDist;
				SetCursor = TRUE;
			}
			lineDist += strBuffer->lineDist;
		}
		if (!SetCursor) {
			strBuffer->imeX = strBuffer->x + (strBuffer->cursor - splitPoint)*(FONT_SIZE >> 1);
			strBuffer->imeY = strBuffer->y + lineDist;
		}
		StockFontBuffer(strBuffer->x, strBuffer->y + lineDist, strBuffer->fontPrio, 0, strBuffer->buffer + splitPoint, 0);
	}else {
		FontBuffer[FontCnt].x = strBuffer->x;
		FontBuffer[FontCnt].y = strBuffer->y;
		FontBuffer[FontCnt].fontPrio = strBuffer->fontPrio;
		FontBuffer[FontCnt].color = strBuffer->color;
		FontBuffer[FontCnt].hitFlag = 0;
#ifdef _FONT_SIZE
		FontBuffer[FontCnt].size = 0;
#endif
		//定义Hook_type用来处理星号产生
#ifdef _SAHOOK //Syu ADD Hook程式
		if (strBuffer->filterFlag == HOOK_TYPE) {
			extern int HOOK_PASSWD_NUM;
			for (int i = 0; i < HOOK_PASSWD_NUM; i++)
				FontBuffer[FontCnt].str[i] = '*';
			FontBuffer[FontCnt].str[i] = NULL;
			strBuffer->cursor = HOOK_PASSWD_NUM;
		}
		else if (strBuffer->filterFlag == BLIND_TYPE) {
#else
		if (strBuffer->filterFlag == BLIND_TYPE) {
#endif
			int i;
			for (i = 0; i < strBuffer->cnt; i++)
				FontBuffer[FontCnt].str[i] = '*';
			FontBuffer[FontCnt].str[i] = NULL;
		}else {
			extern int 编码;
			if (编码 == 950) {
				extern char* GB2312ToBIG5(const char* szBIG5String);
				strcpy(FontBuffer[FontCnt].str, GB2312ToBIG5((const char *)strBuffer->buffer));
			}
			else
				strcpy(FontBuffer[FontCnt].str, strBuffer->buffer);
		}
#ifdef _NEWFONT_
		char strtemp[512];
		char *ptempstr;
		if (strBuffer->filterFlag == BLIND_TYPE) {
			ptempstr = FontBuffer[FontCnt].str;
		}
		else {
			ptempstr = strBuffer->buffer;
		}
		if (strBuffer->cursor == 0) {
			strBuffer->imeX = strBuffer->x;
		}
		else {
			memcpy(strtemp, ptempstr, strBuffer->cursor);
			strtemp[strBuffer->cursor] = 0;
			strBuffer->imeX = strBuffer->x + GetStrWidth(strtemp);

		}
#else
		strBuffer->imeX = strBuffer->x + strBuffer->cursor*(FONT_SIZE >> 1);
#endif
		strBuffer->imeY = strBuffer->y;
		strBuffer->hitFontNo = FontCnt++;
		}
	}
//ONLINEGM USE
int StockFontBuffer3(STR_BUFFER *strBuffer){
	int lineDist = 0; //行距
	int splitPoint = 0;
	int bakSplitPoint, cursor;
	BOOL SetCursor = FALSE;
	char splitStr[256];

	cursor = strBuffer->cursor;
	//需要分行时
	while (strlen(strBuffer->buffer + splitPoint) >= (unsigned)strBuffer->lineLen - 1) {
		bakSplitPoint = splitPoint;
		strcpy(splitStr, strBuffer->buffer + splitPoint);  //一个字
		*(splitStr + strBuffer->lineLen) = NULL;  //指向最后
		splitPoint = strBuffer->lineLen + splitPoint;
		StockFontBuffer(strBuffer->x, strBuffer->y, strBuffer->fontPrio, 0, splitStr, 0);

		if (cursor >= bakSplitPoint && cursor<splitPoint) {
			strBuffer->imeX = strBuffer->x + (cursor - bakSplitPoint)*(FONT_SIZE >> 1);
			strBuffer->imeY = strBuffer->y + lineDist;
			SetCursor = TRUE;
		}
		lineDist += strBuffer->lineDist;
		return 1;
	}
	if (!SetCursor) {  // 游标不需换行
		strBuffer->imeX = strBuffer->x + (strBuffer->cursor - splitPoint)*(FONT_SIZE >> 1);
		strBuffer->imeY = strBuffer->y + lineDist;
	}
	StockFontBuffer(strBuffer->x, strBuffer->y + lineDist, strBuffer->fontPrio, 0, strBuffer->buffer + splitPoint, 0);
	return 0;
}


void StockFontBufferFamily(STR_BUFFER *strBuffer){
	int lineDist = 0; //行距
	int splitPoint = 0;
	int bakSplitPoint, cursor;
	BOOL SetCursor = FALSE;
	char splitStr[256];
	if (FontCnt >= FONT_BUFFER_SIZE) {
		strBuffer->hitFontNo = -2;
		return;
	}
	int 行字数;
	if (strBuffer->lineLen != 0) {
		cursor = strBuffer->cursor;
		if (lineDist) 行字数 = strBuffer->lineLen;
		else 行字数 = 38;
		while (strlen(strBuffer->buffer + splitPoint) >= 行字数) {
			bakSplitPoint = splitPoint;
			//copy一行的字串
			strncpy_s(splitStr, strBuffer->buffer + splitPoint, 行字数);
			*(splitStr + 行字数) = NULL;
			if (GetStrLastByte(splitStr) == 3) {
				//分割到中DBCS时，退回一个byte
				splitPoint = 行字数 - 1 + splitPoint;
				*(splitStr + 行字数 - 1) = NULL;
			}else
				splitPoint = 行字数 + splitPoint;
			if (lineDist)
				StockFontBuffer(strBuffer->x - 60, strBuffer->y + lineDist, strBuffer->fontPrio, 0, splitStr, 0);
			else
				StockFontBuffer(strBuffer->x, strBuffer->y + lineDist, strBuffer->fontPrio, 0, splitStr, 0);
			if (cursor >= bakSplitPoint && cursor<splitPoint) {
				if (lineDist)
					strBuffer->imeX = strBuffer->x - 60 + (cursor - bakSplitPoint)*(FONT_SIZE >> 1);
				else
					strBuffer->imeX = strBuffer->x + (cursor - bakSplitPoint)*(FONT_SIZE >> 1);
				strBuffer->imeY = strBuffer->y + lineDist;
				SetCursor = TRUE;
			}
			lineDist += strBuffer->lineDist;
			if (lineDist) 行字数 = strBuffer->lineLen;
			else 行字数 = 38;
		}
		if (!SetCursor) {
			if (lineDist)
				strBuffer->imeX = strBuffer->x - 60 + (strBuffer->cursor - splitPoint)*(FONT_SIZE >> 1);
			else
				strBuffer->imeX = strBuffer->x + (strBuffer->cursor - splitPoint)*(FONT_SIZE >> 1);
			strBuffer->imeY = strBuffer->y + lineDist;
		}
		if (lineDist)
			StockFontBuffer(strBuffer->x - 60, strBuffer->y + lineDist, strBuffer->fontPrio, 0, strBuffer->buffer + splitPoint, 0);
		else
			StockFontBuffer(strBuffer->x, strBuffer->y + lineDist, strBuffer->fontPrio, 0, strBuffer->buffer + splitPoint, 0);
	}else {
		FontBuffer[FontCnt].x = strBuffer->x;
		FontBuffer[FontCnt].y = strBuffer->y;
		FontBuffer[FontCnt].fontPrio = strBuffer->fontPrio;
		FontBuffer[FontCnt].color = strBuffer->color;
		FontBuffer[FontCnt].hitFlag = 0;
#ifdef _FONT_SIZE
		FontBuffer[FontCnt].size = 0;
#endif
		//定义Hook_type用来处理星号产生
#ifdef _SAHOOK //Syu ADD Hook程式
		if (strBuffer->filterFlag == HOOK_TYPE) {
			extern int HOOK_PASSWD_NUM;
			for (int i = 0; i < HOOK_PASSWD_NUM; i++)
				FontBuffer[FontCnt].str[i] = '*';
			FontBuffer[FontCnt].str[i] = NULL;
			strBuffer->cursor = HOOK_PASSWD_NUM;
		}
		else if (strBuffer->filterFlag == BLIND_TYPE) {
#else
		if (strBuffer->filterFlag == BLIND_TYPE) {
#endif
			int i;
			for (i = 0; i < strBuffer->cnt; i++)
				FontBuffer[FontCnt].str[i] = '*';
			FontBuffer[FontCnt].str[i] = NULL;
		}
		else {
			extern int 编码;
			if (编码 == 950) {
				extern char* GB2312ToBIG5(const char* szBIG5String);
				strcpy(FontBuffer[FontCnt].str, GB2312ToBIG5((const char *)strBuffer->buffer));
			}
			else
				strcpy(FontBuffer[FontCnt].str, strBuffer->buffer);
		}
		strBuffer->imeX = strBuffer->x - 60 + strBuffer->cursor*(FONT_SIZE >> 1);
		strBuffer->imeY = strBuffer->y;
		strBuffer->hitFontNo = FontCnt++;
		}
	}

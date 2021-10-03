#include <windows.h>
#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "../systeminc/action.h"
#include "../systeminc/menu.h"
#include "../systeminc/sprdisp.h"
#include "../systeminc/loadrealbin.h"
#include "../image.h"
#include "../systeminc/font.h"
#include "../systeminc/login.h"
#include "../mylua/winlua.h"
#include "../systeminc/光环称号皮肤.h"



int count_光环 = 1;		//光环的总数量 ， 通过lua改写
BOOL realGetWH(U4 GraphicNo, S2 *w, S2 *h);



光环STRUCT data光环[MAX_COUNT_光环] = {	//称号数据，通过lua改下
	{ 103340, "103340", 1 },

	//光环2
	//103340开始
	//103344结束
};

static int button[5];
static int nPage = 0;
static int nPageTotal = 0;
static int curSel = -1;
ACTION *pAct光环 = NULL;
ACTION *pAct光环人物 = NULL;



//CHENHAORIGHT'103285|203284|103284|1534153113'
void lssproto_查询光环_recv(char* data){
	char* array[5];
	int count = split_damage(data, '\'', (int*)array, 4);
	if (count != 3)return;

	char* array2[2001];
	count = split_damage(array[1], '|', (int*)array2, 2000);

	光环STRUCT* pData = NULL;
	int max = 0;

	if (lstrcmpiA(array[0], "CHENHAORIGHT") == 0){	//称号
		extern 光环STRUCT data称号[];
		pData = &data称号[0];
		max = MAX_COUNT_称号;
	}else if (lstrcmpiA(array[0], "GUANGHUANRIGHT") == 0){//光环
		extern 光环STRUCT data光环[];
		pData = &data光环[0];
		max = MAX_COUNT_光环;
	}else if (lstrcmpiA(array[0], "PIFURIGHT") == 0){//皮肤
		extern 光环STRUCT data皮肤[];
		pData = &data皮肤[0];
		max = MAX_COUNT_皮肤;
	}
	//先将所有数据的时间清0
	for (int i = 0; i < max; i++){
		pData[i].time = 0;
	}

	for (int i = 0; i < count/2; i++){
		int image = atoi(array2[2 * i]);
		int time = atoi(array2[2 * i + 1]);
		for (int j = 0; j < max; j++){
			if (pData[j].bmpNO == image){
				pData[j].time = time;
			}
		}
	}

}

void delAct光环(){
	if (pAct光环人物){
		DeathAction(pAct光环人物);
		pAct光环人物 = NULL;
	}
	if (pAct光环){
		DeathAction(pAct光环);
		pAct光环 = NULL;
	}
}

void Init光环窗口(){
	int BmpNo;
	static short width = 0, height = 0;
	if (width == 0){
		realGetNo(IMAGE_光环图档, (U4 *)&BmpNo);
		realGetWH(BmpNo, &width, &height);			//获取图档尺寸
	}

	//根据分辨率自适应左上角x,y
	extern int DEF_APPSIZEX, DEF_APPSIZEY;
	int x = (DEF_APPSIZEX - width) / 2;
	int y = (DEF_APPSIZEY - height) / 2;

	//画图
	StockDispBuffer绝对位置(x, y, DISP_PRIO_MENU, IMAGE_光环图档, 1);

	//左右箭头
	button[0] = StockDispBuffer(x + 50, y + 310, DISP_PRIO_IME4, 55104, 2);
	button[1] = StockDispBuffer(x + 110, y + 310, DISP_PRIO_IME4, 55105, 2);

	//显示光环
	StockFontBuffer(x + 50, y + 340, FONT_PRIO_FRONT, FONT_PAL_AQUA, "设置光环", 0);
	if (MakeHitBox(x + 50 - 5, y + 340 - 5, x + 50 + 60, y + 340 + 20, DISP_PRIO_IME4) == TRUE){
		if (mouse.onceState & MOUSE_LEFT_CRICK){
			if (data光环[curSel].time == 0){
				StockChatBufferLineExt("你没有权限使用这个光环！", FONT_PAL_RED, 0);
				return;
			}

			void LSSPROTO_203_SEND(int fd, char* data);
			extern unsigned int sockfd;
			char buf[50];
			sprintf(buf, "SETCURRENTGUANGHUAN'%d'", data光环[curSel].bmpNO);
			LSSPROTO_203_SEND(sockfd, buf);
			return;
		}
	}

	//??光环
	StockFontBuffer(x + 200, y + 340, FONT_PRIO_FRONT, FONT_PAL_AQUA, "??光环", 0);
	if (MakeHitBox(x + 200 - 5, y + 340 - 5, x + 200 + 60, y + 340 + 20, DISP_PRIO_IME4) == TRUE){
		if (mouse.onceState & MOUSE_LEFT_CRICK){
			void LSSPROTO_203_SEND(int fd, char* data);
			extern unsigned int sockfd;
			LSSPROTO_203_SEND(sockfd, "CANCELCURRENTGUANGHUAN'");
			return;
		}
	}

	//关闭
	StockFontBuffer(x + 330, y + 340, FONT_PRIO_FRONT, FONT_PAL_AQUA, "  关闭  ", 0);
	if (MakeHitBox(x + 330 - 5, y + 340 - 5, x + 330 + 60, y + 340 + 20, DISP_PRIO_IME4) == TRUE){
		if (mouse.onceState & MOUSE_LEFT_CRICK){
			MenuToggleFlag &= ~JOY_CTRL_S;
			return;
		}
	}

	//取称号个数
	int count = count_光环;

	//总页面数
	nPageTotal = (count + 13 - 1) / 13;

	//显示 页面/总页面
	char buf[20];
	sprintf(buf, "%d/%d", nPage + 1, nPageTotal);
	StockFontBuffer(x + 65, y + 295, FONT_PRIO_FRONT, FONT_PAL_AQUA, buf, 0);

	//取当前页显示的数量
	int count_this_page = count - nPage * 13;
	count_this_page = (count_this_page >= 13) ? 13 : count_this_page % 13;

	for (int i = nPage * 13, j = 0; i < nPage * 13 + count_this_page; i++, j++){
		char buf[30];
		//文字居中
		CenteringStr(data光环[i].name, buf, 19);
		//设定文字左上角位置
		int x1 = x + 13;
		int y1 = y + 30 + j * 20;

		//输出文字
		if (data光环[i].time != 0)
			if (i == curSel){
				StockFontBuffer(x1, y1, FONT_PRIO_FRONT, FONT_PAL_RED, buf, 0);	//红色
			}else{
				StockFontBuffer(x1, y1, FONT_PRIO_FRONT, FONT_PAL_GREEN, buf, 0);	//绿色
			}
		else
			StockFontBuffer(x1, y1, FONT_PRIO_FRONT, FONT_PAL_GRAY, buf, 0);	//灰色

																				//画绿框框并判断点击
		if (MakeHitBox(x1, y1 - 2, x1 + 130, y1 + 17, DISP_PRIO_IME4) == TRUE){
			if (mouse.onceState & MOUSE_LEFT_CRICK){
				//保存点击的编号
				curSel = i;
				//删除原有动画
				delAct光环();

				pAct光环人物 = MakeAnimDisp(x + 280, y + 184, pc.graNo, 0);

				short x2, y2;
				if (获取动画尺寸(pAct光环人物, &x2, &y2)){
					pAct光环 = MakeAnimDisp(x + 280, y + 184, data光环[i].bmpNO, 0);
				}
				return;
			}
		}
	}

	if (mouse.onceState & MOUSE_LEFT_CRICK){
		if (HitDispNo == button[0]){ //左箭头
			nPage--;
			if (nPage < 0)nPage = 0;
		}else if (HitDispNo == button[1]){ //右箭头
			nPage++;
			if (nPage >= nPageTotal- 1) nPage = nPageTotal -1 ;
		}
		__asm nop;
	}

}

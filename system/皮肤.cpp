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




int count_皮肤 = 1;		//皮肤的总数量 ， 通过lua改写
BOOL realGetWH(U4 GraphicNo, S2 *w, S2 *h);


光环STRUCT data皮肤[MAX_COUNT_皮肤] = {	//皮肤数据，通过lua改下
	{ 100000, "100000", 1 },
};

static int button[5];
static int nPage = 0;
static int nPageTotal = 0;
static int curSel = -1;
ACTION *pAct皮肤 = NULL;
void delAct皮肤(){
	if (pActMenu皮肤){
		DeathAction(pActMenu皮肤);
		pActMenu皮肤 = NULL;
	}

}

void Init皮肤窗口(){
	int BmpNo;
	static short width = 0, height = 0;
	if (width == 0){
		realGetNo(IMAGE_皮肤图档, (U4 *)&BmpNo);
		realGetWH(BmpNo, &width, &height);			//获取图档尺寸
	}

	//根据分辨率自适应左上角x,y
	extern int DEF_APPSIZEX, DEF_APPSIZEY;
	int x = (DEF_APPSIZEX - width) / 2;
	int y = (DEF_APPSIZEY - height) / 2;

	//画图
	StockDispBuffer绝对位置(x, y, DISP_PRIO_MENU, IMAGE_皮肤图档, 1);

	//左右箭头
	button[0] = StockDispBuffer(x + 50, y + 310, DISP_PRIO_IME4, 55104, 2);
	button[1] = StockDispBuffer(x + 110, y + 310, DISP_PRIO_IME4, 55105, 2);

	//显示皮肤
	StockFontBuffer(x + 50, y + 340, FONT_PRIO_FRONT, FONT_PAL_GREEN, "设置皮肤", 0);
	if (MakeHitBox(x + 50 - 5, y + 340 - 5, x + 50 + 60, y + 340 + 20, DISP_PRIO_IME4) == TRUE){
		if (mouse.onceState & MOUSE_LEFT_CRICK){
			if (data皮肤[curSel].time == 0){
				StockChatBufferLineExt("你没有权限使用这个皮肤！", FONT_PAL_RED, 0);
				return;
			}
			void LSSPROTO_203_SEND(int fd, char* data);
			extern unsigned int sockfd;
			char buf[50];
			sprintf(buf, "SETCURRENTPIFU'%d'", data皮肤[curSel].bmpNO);
			LSSPROTO_203_SEND(sockfd, buf);
			return;
		}
	}

	//??皮肤
	StockFontBuffer(x + 200, y + 340, FONT_PRIO_FRONT, FONT_PAL_GREEN, "??皮肤", 0);
	if (MakeHitBox(x + 200 - 5, y + 340 - 5, x + 200 + 60, y + 340 + 20, DISP_PRIO_IME4) == TRUE){
		if (mouse.onceState & MOUSE_LEFT_CRICK){
			void LSSPROTO_203_SEND(int fd, char* data);
			extern unsigned int sockfd;
			LSSPROTO_203_SEND(sockfd, "CANCELCURRENTPIFU'");
			return;
		}
	}

	//关闭
	StockFontBuffer(x + 330, y + 340, FONT_PRIO_FRONT, FONT_PAL_GREEN, "  关闭  ", 0);
	if (MakeHitBox(x + 330 - 5, y + 340 - 5, x + 330 + 60, y + 340 + 20, DISP_PRIO_IME4) == TRUE){
		if (mouse.onceState & MOUSE_LEFT_CRICK){
			MenuToggleFlag &= ~JOY_CTRL_S;
			return;
		}
	}

	//取皮肤个数
	int count = count_皮肤;
	
	//总页面数
	nPageTotal = (count + 13 - 1) / 13; 

	//显示 页面/总页面
	char buf[20];
	sprintf(buf, "%d/%d", nPage + 1, nPageTotal);
	StockFontBuffer(x + 65, y + 295, FONT_PRIO_FRONT, FONT_PAL_GREEN, buf, 0);


	//取当前页显示的数量
	int count_this_page = count - nPage * 13;
	count_this_page = (count_this_page >= 13) ? 13 : count_this_page % 13;

	for (int i = nPage * 13, j = 0; i < nPage * 13 + count_this_page; i++, j++)
	{
		char buf[30];

		//文字居中
		CenteringStr(data皮肤[i].name, buf, 19);

		//设定文字左上角位置
		int x1 = x + 13;
		int y1 = y + 30 + j * 20;

		//输出文字
		if (data皮肤[i].time != 0)
			if (i == curSel)
			{
				StockFontBuffer(x1, y1, FONT_PRIO_FRONT, FONT_PAL_RED, buf, 0);	//红色
			}
			else
			{
				StockFontBuffer(x1, y1, FONT_PRIO_FRONT, FONT_PAL_GREEN, buf, 0);	//绿色
			}
		else
			StockFontBuffer(x1, y1, FONT_PRIO_FRONT, FONT_PAL_GRAY, buf, 0);	//灰色

																				//画绿框框并判断点击
		if (MakeHitBox(x1, y1 - 2, x1 + 130, y1 + 17, DISP_PRIO_IME4) == TRUE)
		{
			if (mouse.onceState & MOUSE_LEFT_CRICK)
			{

				//保存点击的编号
				curSel = i;

				//删除原有动画
				delAct皮肤();

				pActMenu皮肤 = MakeAnimDisp(x + 280, y + 184, data皮肤[i].bmpNO, 0);

				return;
			}
		}
	}

	if (mouse.onceState & MOUSE_LEFT_CRICK)
	{

		if (HitDispNo == button[0]) //左箭头
		{
			nPage--;
			if (nPage < 0)nPage = 0;
		}
		else if (HitDispNo == button[1]) //右箭头
		{
			nPage++;
			if (nPage >= nPageTotal - 1) nPage = nPageTotal - 1;
		}
		__asm nop;
	}

}

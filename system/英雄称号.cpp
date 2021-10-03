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




int count_英雄称号 = 1;		//称号的总数量 ， 通过lua改写
BOOL realGetWH(U4 GraphicNo, S2 *w, S2 *h);



光环STRUCT data称号[MAX_COUNT_称号] = {	//称号数据，通过lua改下
{ 103283, "一马当先", 1},


};

static int button[5];
static int nPage = 0;
static int nPageTotal = 0;
static int curSel = -1;
ACTION *pAct称号 = NULL;
ACTION *pAct人物 = NULL;


void delAct称号()
{
	if (pAct人物)
	{
		DeathAction(pAct人物);
		pAct人物 = NULL;
	}
	if (pAct称号)
	{
		DeathAction(pAct称号);
		pAct称号 = NULL;
	}
}

void Init英雄称号窗口()
{
	int BmpNo;
	static short width = 0, height = 0;
	if (width == 0)
	{
		realGetNo(IMAGE_称号图档, (U4 *)&BmpNo);
		realGetWH(BmpNo, &width, &height);			//获取图档尺寸
	}

	//根据分辨率自适应左上角x,y
	extern int DEF_APPSIZEX, DEF_APPSIZEY;
	int x = (DEF_APPSIZEX - width) / 2;
	int y = (DEF_APPSIZEY - height) / 2;



	//画图
	StockDispBuffer绝对位置(x, y, DISP_PRIO_MENU, IMAGE_称号图档, 1);

	//左右箭头
	button[0] = StockDispBuffer(x + 50, y + 310, DISP_PRIO_IME4, 55104, 2);
	button[1] = StockDispBuffer(x + 110, y + 310, DISP_PRIO_IME4, 55105, 2);

	//显示称号
	StockFontBuffer(x + 50, y + 340, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "设置称号", 0);	
	if (MakeHitBox(x + 50-5, y + 340-5, x + 50+60, y + 340+20, DISP_PRIO_IME4) == TRUE)
	{
		if (mouse.onceState & MOUSE_LEFT_CRICK)
		{
			if (data称号[curSel].time == 0)
			{
				StockChatBufferLineExt("你没有权限使用这个称号！", FONT_PAL_RED, 0);
				return;
			}

			void LSSPROTO_203_SEND(int fd, char* data);
			extern unsigned int sockfd;
			char buf[50];
			sprintf(buf, "SETCURRENTCHENHAO'%d'", data称号[curSel].bmpNO);
			LSSPROTO_203_SEND(sockfd, buf);
			return;
		}
	}

	//??称号
	StockFontBuffer(x + 200, y + 340, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "??称号", 0);	
	if (MakeHitBox(x + 200 - 5, y + 340 - 5, x + 200 + 60, y + 340 + 20, DISP_PRIO_IME4) == TRUE)
	{
		if (mouse.onceState & MOUSE_LEFT_CRICK)
		{
			void LSSPROTO_203_SEND(int fd, char* data);
			extern unsigned int sockfd;
			LSSPROTO_203_SEND(sockfd, "CANCELCURRENTCHENHAO'");
			return;
		}
	}

	//关闭
	StockFontBuffer(x + 330, y + 340, FONT_PRIO_FRONT, FONT_PAL_YELLOW, "  关闭  ", 0);
	if (MakeHitBox(x + 330 - 5, y + 340 - 5, x + 330 + 60, y + 340 + 20, DISP_PRIO_IME4) == TRUE)
	{
		if (mouse.onceState & MOUSE_LEFT_CRICK)
		{
			MenuToggleFlag &= ~JOY_CTRL_S;
			return;
		}
	}

	//取称号个数
	int count = count_英雄称号;

	//总页面数
	nPageTotal = (count + 13 - 1) / 13;

	//显示 页面/总页面
	char buf[20];
	sprintf(buf, "%d/%d", nPage + 1, nPageTotal);
	StockFontBuffer(x + 65, y + 295, FONT_PRIO_FRONT, FONT_PAL_YELLOW, buf, 0);


	//取当前页显示的数量
	int count_this_page = count - nPage * 13;
	count_this_page = (count_this_page >= 13) ? 13 : count_this_page % 13;
	
	for (int i = nPage * 13, j = 0; i < nPage * 13 + count_this_page; i++, j++)
	{
		char buf[30];

		//文字居中
		CenteringStr(data称号[i].name, buf, 19);

		//设定文字左上角位置
		int x1 = x + 13;
		int y1 = y + 30 + j * 20;

		//输出文字
		if (data称号[i].time != 0)
		{
			if (i == curSel)
			{
				StockFontBuffer(x1, y1, FONT_PRIO_FRONT, FONT_PAL_RED, buf, 0);	//红色
			}
			else
			{
				StockFontBuffer(x1, y1, FONT_PRIO_FRONT, FONT_PAL_GREEN, buf, 0);	//绿色
			}
			
		}
			
		else
			StockFontBuffer(x1, y1, FONT_PRIO_FRONT, FONT_PAL_GRAY, buf, 0);	//灰色

		//画绿框框并判断点击
		if (MakeHitBox(x1,y1 -2,x1 + 130, y1+ 17, DISP_PRIO_IME4) == TRUE)
		{
			if (mouse.onceState & MOUSE_LEFT_CRICK)
			{
				//保存点击的编号
				curSel = i;

				//删除原有动画
				delAct称号();

				pAct人物 = MakeAnimDisp(x + 280, y + 184, pc.graNo, 0);

				short x2, y2;
				if (获取动画尺寸(pAct人物, &x2, &y2)) 
				{
					pAct称号 = MakeAnimDisp(x + 280, y + 184 - y2/2 - 45, data称号[i].bmpNO, 0);
				}
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



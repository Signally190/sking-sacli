/************************/
/*	action.c			*/
/************************/
#include "../systeminc/version.h"
#include "../systeminc/system.h"


ACTION *pActTop;
ACTION *pActBtm;

#ifdef  _STONDEBUG_		
int ActCnt;
extern int g_iMallocCount;
#endif

void InitAction( void )
{
	/* ?????????? */
	pActTop = ( ACTION * )CALLOC( 1, sizeof( ACTION ) );
	pActBtm = ( ACTION * )CALLOC( 1, sizeof( ACTION ) );
#ifdef  _STONDEBUG_
	g_iMallocCount += 2;
#endif

	pActTop->pPrev = NULL;	 	/* ?????? */
	pActTop->pNext = pActBtm;	/* ???????? */
	pActTop->func  = NULL;		/* ????? */
	pActTop->prio  = PRIO_TOP;	/* ???? */
	pActTop->bmpNo = -1;		/* ??????? */
	
	pActBtm->pPrev = pActTop;	/* ????? */
	pActBtm->pNext = NULL;		/* ??????? */
	pActBtm->func  = NULL;		/* ????? */
	pActBtm->prio  = PRIO_BTM;	/* ???? */
	pActBtm->bmpNo = -1;		/* ??????? */
}

/* ?????????? *****************************************************/
ACTION *GetAction( UCHAR prio, UINT yobiSize )
{
	ACTION *pAct;
	ACTION *pActLoop;
	/* ?????????? */
	pAct = ( ACTION * )CALLOC( 1, sizeof( ACTION ) );
#ifdef  _STONDEBUG_
	g_iMallocCount++;
#endif
	if( pAct == NULL ){
		MessageBox( hWnd, "沒有足夠的硬盤空間！", "GetAction Erorr", MB_OK );
		return NULL;
	}
	/* ???? */
	if( yobiSize > 0 ){
		/* ?????? */
		pAct->pYobi = CALLOC( 1, yobiSize );
#ifdef  _STONDEBUG_
		g_iMallocCount++;
#endif
		// ???
		if( pAct->pYobi == NULL ){
			/* ???????????? */
			FREE( pAct );
			//JL 尝试fix,作用未知  2017/9/7
			pAct = NULL;

#ifdef  _STONDEBUG_
			g_iMallocCount--;
#endif
			MessageBox( hWnd, "沒有足夠的硬盤空間！", "GetYobi Erorr", MB_OK );
			return NULL;
		}
	}
	/* ??? */
	pAct->func  = NULL;			/* ????? */
	pAct->prio  = prio;			/* ??? */
	pAct->bmpNo = -1;			/* ??????? */
	pAct->hitDispNo = -2;		/* ?????????????? */
	/* ????? */
	for( pActLoop = pActTop->pNext ; pActLoop != pActBtm->pNext ; pActLoop = pActLoop->pNext ){
		/* ???? */
		if( pActLoop->prio > prio ){
			/* ???? */
			/* ????????? */
			pAct->pPrev = pActLoop->pPrev;	/* ???? */
			pAct->pNext = pActLoop;			/* ????? */
			/* ????? */
			pActLoop->pPrev->pNext = pAct;	/* ???? */
			pActLoop->pPrev = pAct;			/* ????? */
#ifdef  _STONDEBUG_
			/* ?????????? */
   			ActCnt++;
#endif
			break;
		}
	}
	return pAct;
}

/* ????????? *********************************************************/
// 执行所有的动作，当deathFlag设定为TRUE时，会造成此动作的释放

void RunAction( void )
{
	ACTION *pActLoop = pActTop->pNext; 	/* ???????? */
	ACTION *pActLoopBak;  /* ????????? */
	
	static int a = 0;

	while(1){
		extern int g_CPU优化;
		if (g_CPU优化)
		{
			a++;
			if (a > 1000000)a == 0;
			if (a % 12 == 0)
			{
				Sleep(1);
			}
		}


		/* ?????????? */	
		if( pActLoop == pActBtm ) break;
		/* ????? */
		if( pActLoop->deathFlag == FALSE ){
			/* ?????????? */
			if( pActLoop->func != NULL ){
				pActLoop->func( pActLoop );
			}
			/* ?????????? */
			pActLoop = pActLoop->pNext;
		}else{	/* ?????? */
			/* ????? */
			pActLoop->pPrev->pNext = pActLoop->pNext;
			pActLoop->pNext->pPrev = pActLoop->pPrev;
			/* ?????? */
			pActLoopBak = pActLoop->pNext;
			/* ?????? */
			ClearAction( pActLoop );
			/* ???????? */
			pActLoop = pActLoopBak;
		}
	}
}

/* ???????????? ***********************************************/
extern ACTION *ptActMenuWin;
extern short helpBtn;
extern short actBtn;
void DeathAction( ACTION *pAct )
{

	if (pAct == NULL) return;
	if (ptActMenuWin == pAct){
		helpBtn = 0;
		actBtn = 0;
	}

	if ((int)pAct < 10000)
	{
		pAct = 0;
		return;
	}

	if (pAct == NULL /*|| (int)pAct < 10000 */) return;

	////JL fix 检测pAct是否为有效地址
	try
	{
		pAct->deathFlag = TRUE;

	}
	catch (...)
	{
		pAct = 0;
	}

}

void RemoveAction(ACTION *pAct)
{
	ACTION* pre = pAct->pPrev;
	ACTION* next= pAct->pNext;

	if (pre)
	{
		pre->pNext = next;
	}
	next->pPrev = pre;

	ClearAction(pAct);

}

/* ???????????? ***********************************************/
void ClearAction( ACTION *pAct )
{
	// ???????
	if( pAct->pYobi != NULL ){
		// ????
		FREE( pAct->pYobi );
		pAct->pYobi = NULL;
#ifdef  _STONDEBUG_
		g_iMallocCount--;
#endif
	}
	// ????????
	FREE( pAct );
	pAct = NULL;
#ifdef  _STONDEBUG_
	g_iMallocCount--;
#endif
#ifdef  _STONDEBUG_		
	/* ?????????? */
	ActCnt--;
#endif		

}
#include <tlhelp32.h>

/* ?????? ***********************************************************/
void DeathAllAction( void )
{

	ACTION *pActLoop = pActTop->pNext;
	/* ???????? */
	while( pActLoop != pActBtm ){
		/* ??????? */
		pActLoop->deathFlag = TRUE;
		/* ??????? */
		pActLoop = pActLoop->pNext;
	}
}

/* ???????? *********************************************************/
void EndAction( void )
{
	// ??????
	DeathAllAction();
	//?????????????
	RunAction();
	/* ??????????? */
	FREE( pActTop );
	//JL尝试fix 2017.9.7
	pActTop = NULL;

	FREE( pActBtm );
	//JL尝试fix 2017.9.7
	pActBtm = NULL;

#ifdef  _STONDEBUG_
	g_iMallocCount -= 2;
#endif
}



int GetPettblNo(int graNo){
	int tblNo = graNo - 100250;
#if defined(__ALBUM_47)
	if( tblNo > 1800 ){					//小恶魔
		tblNo -= 1208;
	}else if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_46)
	if( tblNo > 1800 ){					//小恶魔
		tblNo -= 1208;
	}else if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_45)
	if( tblNo > 1800 ){					//小恶魔
		tblNo -= 1208;
	}else if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_44)
	if( tblNo > 1800 ){					//小恶魔
		tblNo -= 1208;
	}else if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_43)
	if( tblNo > 1800 ){					//小恶魔
		tblNo -= 1208;
	}else if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_42)
	if( tblNo > 1800 ){					//小恶魔
		tblNo -= 1208;
	}else if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_41)
	if( tblNo > 1800 ){					//小恶魔
		tblNo -= 1208;
	}else if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_40)
	if( tblNo > 1800 ){					//小恶魔
		tblNo -= 1208;
	}else if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_39)
	if( tblNo > 1800 ){					//小恶魔
		tblNo -= 1208;
	}else if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_38)
	if( tblNo > 1755 ){					//间隔南瓜魔王后的 狐猴
		tblNo -= 1201 ;
	}else if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_37)
	if (tblNo > 1739 ){			//飞蛇
		tblNo -= 1200;
	}else if (tblNo > 1686 ){			//8.0宠物
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_36)
	if (tblNo > 1686 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1175;
	}else if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_35)
	if (tblNo > 1641 ){			//鸡年兽4 甲虫2	8.0第一次整合测试
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}

#elif defined(__ALBUM_34)
	if (tblNo > 1639 ){			//鸡年兽4
		tblNo -= 1167;
	}else if (tblNo > 1635 ){   //麒麟
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_33)
	if (tblNo > 1635 ){
		tblNo -= 1148;
	}else if ( tblNo > 1634 ){	//猫女1 猫女2 麒麟
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_32)
	if ( tblNo > 1634 ){	//猫女1 猫女2
		tblNo -= 1149;
	}else if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_31)
	if ( tblNo > 1628 ){	//火蚁1 火蚁2 牛人1 牛人2
		tblNo -= 1147;
	}else if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_30)
	if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼 布伊酷
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_29)
	if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔 白狼
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_28)
	if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2 灰人熔
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_27)
	if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王 狮人1 狮人2
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}

#elif defined(__ALBUM_26)
	if( tblNo > 1615 ){//布里萨尔 蜜蜂1 蜜蜂2 蝴蝶1 蝴蝶2 暗黑乌力王
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_25)
	if( tblNo > 1615 ){//布里萨尔
		tblNo -= 1145;
	}else if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_24)
	if( tblNo > 1568 ){//机人龙
		tblNo -= 1103;
	}else if( tblNo > 1564 ){//黄色飞龙
		tblNo -= 1101;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_23)
	if( tblNo > 1518 ){
		tblNo -= 1057;
	}else if( tblNo > 1516 ){
		tblNo -= 1055;
	}else if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_22)
	if ( tblNo == 1516 ){//修正乌力王
		tblNo = 455 ;
	}else if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}				
#elif defined(__ALBUM_21)
	if ( tblNo > 1509 ){
		tblNo -= 1054;
	}else if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}						
#elif defined(__ALBUM_20)
	if ( tblNo == 1497 ){
		tblNo = 395;
	}else if ( tblNo > 1495 ){
		tblNo -= 1044 ;
	}else if ( tblNo > 1490 ){
		tblNo -= 1043 ;
	}else if ( tblNo > 1485 ){
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}						
#elif defined(__ALBUM_19)
	if ( tblNo > 1485 ) {
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_18)
	if ( tblNo > 1490 ){
		tblNo -= 1044;
	}else if ( tblNo > 1485 ) {
		tblNo -= 1040 ; 
	}else if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_17)
	if ( tblNo > 1454 ) {
		tblNo -= 1034 ; 
	}else if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_16)
	if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#elif defined(__ALBUM_15)
	if ( tblNo == 1214 ) {
		tblNo = 331 ;
	}else if ( tblNo == 1217 ) {
		tblNo = 328 ; 
	}else if ( tblNo > 1361){
		tblNo -= 952;
	}else if( tblNo > 1356){
		tblNo -= 999;
	}else if( tblNo > 1281){
		tblNo -= 919;
	}else if( tblNo > 1239){
		tblNo -= 889;
	}else if( tblNo > 1186 ){
		tblNo -= 886;
	}else if( tblNo > 1173 ){
		tblNo -= 877;
	}else if( tblNo > 1172){
		tblNo -= 877;
	}else if( tblNo > 1161){
		tblNo -= 868;
	}else if( tblNo > 930){
		tblNo -= 745;
	}else if( tblNo == 927){
		tblNo -= 634;
	}else if( tblNo > 929){
		tblNo -= 644;
	}else if( tblNo > 927){
		tblNo -= 643;
	}else if( tblNo > 900){
		tblNo -= 621;
	}else if( tblNo > 180){
		tblNo -= (333+56);
	}
#endif
	return tblNo;
}

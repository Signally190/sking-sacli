#include <stdio.h>
#include <stdlib.h>

/* WIN32_LEAN_AND_MEAN?define???WINDOWS.H???
???????????????????????
????????????????? */
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <Mmsystem.h>
#include <math.h>

#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "../systeminc/action.h"
#include "../systeminc/map.h"
#include "../systeminc/pattern.h"
#include "../systeminc/loadrealbin.h"
#include "../systeminc/loadsprbin.h"
#include "../systeminc/sprdisp.h"
#include "../systeminc/math2.h"

#include "../systeminc/character.h"
#include "../systeminc/pc.h"
#include "../systeminc/menu.h"
#include "../systeminc/main.h"
#include "../systeminc/anim_tbl.h"
#include "../systeminc/chat.h"

extern BOOL g_bUseAlpha;

// ?????????
enum
{
	CHAROBJ_USE_FREE,
	CHAROBJ_USE_STAY,
	CHAROBJ_USE_VIEW
};
// Robin
//#define MAX_CHAROBJ	1000
#define MAX_CHAROBJ	1500
CHAROBJ charObj[MAX_CHAROBJ];
int maxCharObj;		// ??
int tailCharObj;	// ????????
int freeCharObj;	// ???????
int searchCharObj;	// ???

#ifdef _AniCharBubble	   // Syu ADD 动画层人物吐出气泡
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
bool popflag = false;		//人物产生气泡与否旗标
bool waittimeflag = false;  //气泡结束后等待时间旗标
int waittime = 0 ;			//气泡结束后等待时间
static ACTION *popAct;		//气泡动画
#endif

// ???????????????
int charIconOffsetY[12] =
{
	-71, -88, -84, -88, -96, -102, -88, -88, -88, -85, -85, -88
};

// ???????
int battleIconTbl[][2] =
{
	{  CG_VS_MARK_1A,  CG_VS_MARK_1B },
	{  CG_VS_MARK_2A,  CG_VS_MARK_2B },
	{  CG_VS_MARK_3A,  CG_VS_MARK_3B },
	{  CG_VS_MARK_4A,  CG_VS_MARK_4B },
	{  CG_VS_MARK_5A,  CG_VS_MARK_5B },
	{  CG_VS_MARK_6A,  CG_VS_MARK_6B },
	{  CG_VS_MARK_7A,  CG_VS_MARK_7B },
	{  CG_VS_MARK_8A,  CG_VS_MARK_8B },
	{  CG_VS_MARK_9A,  CG_VS_MARK_9B },
	{ CG_VS_MARK_10A, CG_VS_MARK_10B }
};


/*
	{23, 9,21, 9,22, 8, 5,-1},	//0 玩家
	{22,14,20,12,21,11, 4, 4},//1 玩家
	{17,15,17,13,18,12,-1, 5},	//2 玩家
	{13,11,15,11,16,10,-5, 1},	//3 玩家
	{18,10,16, 8,17, 7,-4,-4},	//4 玩家
	{19, 5,19, 7,20, 6, 1,-5},	//5 玩家
	{18,18,18,10,19, 9, 0, 0},	//6 庄家	
*/

short nameOverTheHeadFlag = 1;

void charProc( ACTION * );

// ????????????????
void charProc( ACTION *ptAct )
{
#ifdef _SHOW_ALL_ID
	char *lssproto_cnv10to62(int a, char *out, int outlen);
	char c62[20];
	char buf[20];
	lssproto_cnv10to62(ptAct->objIndex, c62, 20);
	wsprintfA(buf, "%s(%d)", c62, ptAct->objIndex);
	StockFontBuffer(ptAct->x-5, ptAct->y+10, 0, 4, buf, 0);
#endif // _SHOW_ALL_ID



	float mx, my;
	int animLoop;
	int no;
	CHAREXTRA *ext;
	int pFlag = 0;
	// ???
	// PC???????????
	// PC??????????????????
	if( pc.ptAct != ptAct ){
		ext = (CHAREXTRA *)ptAct->pYobi;
		no = ext->charObjTblId;
		// ???????????????????
		if( (charObj[no].status & CHR_STATUS_PARTY) == 0 ){
			charMove( ptAct );
			pFlag = 0;
		}else
			pFlag = 1;
		if( ptAct->vx == 0 && ptAct->vy == 0 && charObj[no].stockDir != -1
		 && charObj[no].stockDirX == ptAct->gx && charObj[no].stockDirY == ptAct->gy ){
			ptAct->anim_ang = charObj[no].stockDir;
			charObj[no].stockDir = -1;
			charObj[no].stockDirX = 0;
			charObj[no].stockDirY = 0;
		}
	}
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	
	ptAct->x = (int)(mx+.5);
	ptAct->y = (int)(my+.5);

#ifdef _NPC_PICTURE
#endif
	drawCharStatus( ptAct );
	// ?????????
	if( ptAct->anim_no == ANIM_HAND
	 || ptAct->anim_no == ANIM_HAPPY
	 || ptAct->anim_no == ANIM_ANGRY
	 || ptAct->anim_no == ANIM_SAD
	 || ptAct->anim_no == ANIM_WALK
	 || ptAct->anim_no == ANIM_STAND
	 || ptAct->anim_no == ANIM_NOD )
		animLoop = ANM_LOOP;
	else
		animLoop = ANM_NO_LOOP;
	pattern( ptAct, ANM_NOMAL_SPD, animLoop );
	if( pFlag ){
		if( nowSpdRate >= 1.2F )
			pattern( ptAct, ANM_NOMAL_SPD, animLoop );
		if( nowSpdRate >= 1.6F )
			pattern( ptAct, ANM_NOMAL_SPD, animLoop );
	}else{
		if( ptAct->bufCount >= 2 )
			pattern( ptAct, ANM_NOMAL_SPD, animLoop );
		if( ptAct->bufCount >= 4 )
			pattern( ptAct, ANM_NOMAL_SPD, animLoop );
	}
	if( pc.ptAct != ptAct ){
		if( charObj[no].newFoundFlag ){
			S2 xx, yy, ww, hh;
			// ???????
			realGetPos( ptAct->bmpNo, &xx, &yy );
			realGetWH( ptAct->bmpNo, &ww, &hh );
			xx += ptAct->x;
			yy += ptAct->y;
			if( 0 <= xx && xx+ww <= DEF_APPSIZEX
			 && 0 <= yy && yy+hh <= DEF_APPSIZEY ){
				CheckNewPet( ptAct->anim_chr_no );
				charObj[no].newFoundFlag = 0;
			}
		}
	}
	if( 20000 <= ptAct->anim_chr_no && ptAct->anim_chr_no <= 24999 ){
		// ????
		if( pc.ptAct != ptAct ){
			// ??????????
			if( ptAct->gx < nowGx-16 || nowGx+16 < ptAct->gx
			 || ptAct->gy < nowGy-16 || nowGy+16 < ptAct->gy ){
				delCharObj( charObj[no].id );
				return;
			}
		}
		// ??????????
		if( !itemOverlapCheck( ptAct->bmpNo, ptAct->gx, ptAct->gy ) ){
			// ??????????????
			setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my );
			ptAct->atr &= (~ACT_ATR_HIDE);
		}else
			ptAct->atr |= ACT_ATR_HIDE;
	}else{
		// ????????
		// ??????????????
		setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my );
	}
}
//水世界Action套用Char的Action修改
#ifdef _WATERANIMATION //Syu ADD 泪之海动画层
void waterAniProc( ACTION *ptAct )
{
	float mx, my;
	int animLoop;
	int no;
	CHAREXTRA *ext;
	int pFlag = 0;
	// ???
	// PC???????????
	// PC??????????????????
	if( pc.ptAct != ptAct ){
		ext = (CHAREXTRA *)ptAct->pYobi;
		no = ext->charObjTblId;
		// ???????????????????
		if( (charObj[no].status & CHR_STATUS_PARTY) == 0 ){
			charMove( ptAct );
			pFlag = 0;
		}else
			pFlag = 1;
		if( ptAct->vx == 0 && ptAct->vy == 0 && charObj[no].stockDir != -1
		 && charObj[no].stockDirX == ptAct->gx && charObj[no].stockDirY == ptAct->gy ){
			ptAct->anim_ang = charObj[no].stockDir;
			charObj[no].stockDir = -1;
			charObj[no].stockDirX = 0;
			charObj[no].stockDirY = 0;
		}
	}
	// ??
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)(mx+.5);
	ptAct->y = (int)(my+.5);
	// ??????????
	//  不??掉人物状态会被复制到其他ACTION
	//	drawCharStatus( ptAct );
	// ?????????
	if( ptAct->anim_no == ANIM_HAND
	 || ptAct->anim_no == ANIM_HAPPY
	 || ptAct->anim_no == ANIM_ANGRY
	 || ptAct->anim_no == ANIM_SAD
	 || ptAct->anim_no == ANIM_WALK
	 || ptAct->anim_no == ANIM_STAND
	 || ptAct->anim_no == ANIM_NOD )
		animLoop = ANM_LOOP;
	else
		animLoop = ANM_NO_LOOP;
	pattern( ptAct, ANM_NOMAL_SPD, animLoop );
	if( pFlag ){
		if( nowSpdRate >= 1.2F )
			pattern( ptAct, ANM_NOMAL_SPD, animLoop );
		if( nowSpdRate >= 1.6F )
			pattern( ptAct, ANM_NOMAL_SPD, animLoop );
	}else{
		if( ptAct->bufCount >= 2 )
			pattern( ptAct, ANM_NOMAL_SPD, animLoop );
		if( ptAct->bufCount >= 4 )
			pattern( ptAct, ANM_NOMAL_SPD, animLoop );
	}
	if( pc.ptAct != ptAct ){
		if( charObj[no].newFoundFlag ){
			S2 xx, yy, ww, hh;
			// ???????
			realGetPos( ptAct->bmpNo, &xx, &yy );
			realGetWH( ptAct->bmpNo, &ww, &hh );
			xx += ptAct->x;
			yy += ptAct->y;
			if( 0 <= xx && xx+ww <= DEF_APPSIZEX
			 && 0 <= yy && yy+hh <= DEF_APPSIZEY ){
				CheckNewPet( ptAct->anim_chr_no );
				charObj[no].newFoundFlag = 0;
			}
		}
	}
	if( 20000 <= ptAct->anim_chr_no && ptAct->anim_chr_no <= 24999 ){
		// ????
		if( pc.ptAct != ptAct ){
			// ??????????
			if( ptAct->gx < nowGx-16 || nowGx+16 < ptAct->gx
			 || ptAct->gy < nowGy-16 || nowGy+16 < ptAct->gy ){
				delCharObj( charObj[no].id );
				return;
			}
		}
		// ??????????
		if( !itemOverlapCheck( ptAct->bmpNo, ptAct->gx, ptAct->gy ) ){
			// ??????????????
#ifdef _SPECIALSPACEANIM_FIX  //ROG ADD 修改图层
			setPartsPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my,ptAct->dispPrio);
#else
			setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my );
#endif
			ptAct->atr &= (~ACT_ATR_HIDE);
		}else
			ptAct->atr |= ACT_ATR_HIDE;
	}else{
		// ????????
		// ??????????????
#ifdef _SPECIALSPACEANIM_FIX  //ROG ADD 修改图层
		setPartsPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my,ptAct->dispPrio);
#else
		setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my );
#endif
	}
}
/* graNo	 Spr图号
   gx		 产生的x座标
   gy	  	 产生的y座标
   dispprio  图案的覆盖顺序
*/

ACTION *createWaterAnimation( int graNo, int gx, int gy, int dispprio )
{

	ACTION *ptAct;
	float mx, my;
	ptAct = GetAction( DISP_PRIO_BOX3, sizeof( CHAREXTRA ) );
//	ptAct = GetAction( PRIO_JIKI, NULL );
	if( ptAct == NULL )	return NULL;
	ptAct->func = waterAniProc;
	ptAct->anim_chr_no = graNo;
	ptAct->anim_no = ANIM_STAND;
	ptAct->anim_ang = 1;
	ptAct->dispPrio = dispprio;
	ptAct->atr |= ACT_ATR_HIT;
	ptAct->nextGx = gx;	
	ptAct->nextGy = gy;
	ptAct->bufCount = 0;
	ptAct->gx = gx;
	ptAct->gy = gy;
	ptAct->mx = (float)gx * GRID_SIZE;	
	ptAct->my = (float)gy * GRID_SIZE;
	ptAct->vx = 0;			
	ptAct->vy = 0;
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)(mx+.5);
	ptAct->y = (int)(my+.5);
	pattern( ptAct, ANM_NOMAL_SPD, ANM_NO_LOOP );
	return ptAct;
}

#endif
// ????????????
ACTION *createCharAction( int graNo, int gx, int gy, int dir )
{
	ACTION *ptAct;
	float mx, my;
	/* ?????????? */
	ptAct = GetAction( PRIO_CHR, sizeof( CHAREXTRA ) );
	if( ptAct == NULL )
		return NULL;
	// ???
	ptAct->func = charProc;
	// ????????
	ptAct->anim_chr_no = graNo;
	// ??
	ptAct->anim_no = ANIM_STAND;
	// ?????????( ??? )( ??????? )
	ptAct->anim_ang = dir;
	// ?
//	ptAct->dispPrio =	DISP_PRIO_BOX3;
	ptAct->dispPrio = DISP_PRIO_CHAR;
	// 1????????
	ptAct->atr = ACT_ATR_INFO |	ACT_ATR_HIT | ACT_ATR_HIDE2;
	// ???
	ptAct->nextGx = gx;					// ???????????
	ptAct->nextGy = gy;
	ptAct->bufCount = 0;
	ptAct->gx = gx;						// ???????????佋?
	ptAct->gy = gy;
	ptAct->mx = (float)gx * GRID_SIZE;	// ????
	ptAct->my = (float)gy * GRID_SIZE;
	ptAct->vx = 0;						// ??
	ptAct->vy = 0;
	// ??
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)(mx+.5);
	ptAct->y = (int)(my+.5);
//#ifdef 人物光环
//#ifdef 人物称号

	ptAct->称号图档编号 = 0;
	ptAct->光环图档编号 = 0;
	return ptAct;
}

// ??????????????
void stockCharMovePoint( ACTION *ptAct, int nextGx, int nextGy )
{
	if( ptAct == NULL )
		return;
	// ??????????????
	if( ptAct->bufCount < sizeof( ptAct->bufGx )/sizeof( int ) ){
		ptAct->bufGx[ptAct->bufCount] = nextGx;
		ptAct->bufGy[ptAct->bufCount] = nextGy;
		ptAct->bufCount++;
	}else{
	// ???????????????
		ptAct->bufCount = 0;
		setCharWarpPoint( ptAct, nextGx, nextGy );
	}
}

// ??????????????????????
//?????????????????????????
void correctCharMovePoint( ACTION *ptAct, int nextGx, int nextGy )
{
	int dx, dy;
	int nGx[2], nGy[2], nCnt = 0;
	int i;
	int preCnt;
	int nextGx2, nextGy2;
	if( ptAct == NULL )
		return;
	// ???????
	// ??????????????next????
	if( ptAct->bufCount <= 0 ){
		nextGx2 = ptAct->nextGx;
		nextGy2 = ptAct->nextGy;
	}else{
		preCnt = ptAct->bufCount - 1;
		nextGx2 = ptAct->bufGx[preCnt];
		nextGy2 = ptAct->bufGy[preCnt];
	}
	dx = nextGx - nextGx2;
	dy = nextGy - nextGy2;
	if( ABS( dx ) == 2 && ABS( dy ) == 2 ){
		nGx[nCnt] = nextGx2+dx/2;
		nGy[nCnt] = nextGy2+dy/2;
		nCnt++;
	}else if( ABS( dx ) == 2 ){
		nGx[nCnt] = nextGx2+dx/2;
		nGy[nCnt] = nextGy;
		nCnt++;
	}else if( ABS( dy ) == 2 ){
		nGx[nCnt] = nextGx;
		nGy[nCnt] = nextGy2+dy/2;
		nCnt++;
	}
	nGx[nCnt] = nextGx;
	nGy[nCnt] = nextGy;
	nCnt++;
	// ??????????????
	if( ptAct->bufCount+nCnt <= sizeof( ptAct->bufGx )/sizeof( int ) ){
		for( i = 0; i < nCnt; i++ ){
			ptAct->bufGx[ptAct->bufCount] = nGx[i];
			ptAct->bufGy[ptAct->bufCount] = nGy[i];
			ptAct->bufCount++;
		}
	}else{
	// ???????????????
		ptAct->bufCount = 0;
		nCnt--;
		setCharWarpPoint( ptAct, nGx[nCnt], nGy[nCnt] );
	}
}

// ?????
void setCharMovePoint( ACTION *ptAct, int nextGx, int nextGy )
{
	float dir1;
	int dir;
	float dx, dy;
	float len;
	float rate = 1.0F;
	if( ptAct == NULL )
		return;
	if( ptAct->bufCount > 5 )
		rate = 2.0F;
	else if( ptAct->bufCount >= 4 )
		rate = 1.6F;
	else if( ptAct->bufCount >= 2 )
		rate = 1.2F;
	dx = nextGx*GRID_SIZE-ptAct->mx;
	dy = nextGy*GRID_SIZE-ptAct->my;
	len = (float)sqrt( (double)(dx*dx+dy*dy) );
	if( len > 0 ){
		dx /= len;
		dy /= len;
	}else{
		dx = 0;
		dy = 0;
	}
	ptAct->vx = dx * MOVE_SPEED * rate;
	ptAct->vy = dy * MOVE_SPEED * rate;
	ptAct->nextGx = nextGx;
	ptAct->nextGy = nextGy;
	if( dx != 0 || dy != 0 ){
		dir1 = Atan( dx, dy ) + 22.5F;
		AdjustDir( &dir1 );
		dir = (int)(dir1/45);
		ptAct->anim_ang = dir;
		ptAct->walkFlag = 1;
	}
}

void _setCharMovePoint( ACTION *ptAct, int nextGx, int nextGy )
{
	float dir1;
	int dir;
	float dx, dy;
	float len;
	if( ptAct == NULL )
		return;
	dx = nextGx*GRID_SIZE-ptAct->mx;
	dy = nextGy*GRID_SIZE-ptAct->my;
	len = (float)sqrt( (double)(dx*dx+dy*dy) );
	if( len > 0 ){
		dx /= len;
		dy /= len;
	}else{
		dx = 0;
		dy = 0;
	}
	ptAct->vx = dx * MOVE_SPEED;
	ptAct->vy = dy * MOVE_SPEED;
	ptAct->nextGx = nextGx;
	ptAct->nextGy = nextGy;
	if( dx != 0 || dy != 0 ){
		dir1 = Atan( dx, dy ) + 22.5F;
		AdjustDir( &dir1 );
		dir = (int)(dir1/45);
		ptAct->anim_ang = dir;
		ptAct->walkFlag = 1;
	}
}

// ?????????????
void shiftBufCount( ACTION *ptAct )
{
	int i;
	if( ptAct == NULL )
		return;
	if( ptAct->bufCount > 0 )
		ptAct->bufCount--;
	for( i = 0; i < ptAct->bufCount; i++ ){
		ptAct->bufGx[i] = ptAct->bufGx[i+1];
		ptAct->bufGy[i] = ptAct->bufGy[i+1];
	}
}

// ??????
void charMove( ACTION *ptAct )
{
	float mx, my;
	if( ptAct == NULL )
		return;
	mx = (float)ptAct->nextGx*GRID_SIZE;
	my = (float)ptAct->nextGy*GRID_SIZE;
	// ????????????????
	if( mx == ptAct->mx && my == ptAct->my ){
		// ?????????????
		if( ptAct->bufCount > 0 ){
			setCharMovePoint( ptAct, ptAct->bufGx[0], ptAct->bufGy[0] );
			shiftBufCount( ptAct );
		}
	}
	mx = (float)ptAct->nextGx*GRID_SIZE;
	my = (float)ptAct->nextGy*GRID_SIZE;
	// ????
	if( ptAct->vx != 0 || ptAct->vy != 0 ){
		// ?佋?????????????
		if( pointLen2( ptAct->mx, ptAct->my, mx, my )
			<= ptAct->vx*ptAct->vx+ptAct->vy*ptAct->vy ){
			ptAct->mx = mx;
			ptAct->my = my;
			ptAct->vx = 0;
			ptAct->vy = 0;
		}else{
		// ?
			ptAct->mx += ptAct->vx;
			ptAct->my += ptAct->vy;
		}
		// ????
		ptAct->anim_no = ANIM_WALK;
	}else{
		// ??????
		if( ptAct->walkFlag != 0 )
			ptAct->anim_no = ANIM_STAND;
		ptAct->walkFlag = 0;
	}
	ptAct->gx = (int)(ptAct->mx/GRID_SIZE);
	ptAct->gy = (int)(ptAct->my/GRID_SIZE);
}

// ?????????????
void charMove2( ACTION *ptAct )
{
	float mx, my;
	if( ptAct == NULL )
		return;
	mx = (float)ptAct->nextGx*GRID_SIZE;
	my = (float)ptAct->nextGy*GRID_SIZE;
	// ????
	if( ptAct->vx != 0 || ptAct->vy != 0 ){
		// ?佋?????????????
		if( pointLen2( ptAct->mx, ptAct->my, mx, my )
			<= ptAct->vx*ptAct->vx+ptAct->vy*ptAct->vy ){
			ptAct->mx = mx;
			ptAct->my = my;
			ptAct->vx = 0;
			ptAct->vy = 0;
		}else{
		// ?
			ptAct->mx += ptAct->vx;
			ptAct->my += ptAct->vy;
		}
		// ????
		ptAct->anim_no = ANIM_WALK;
	}else{
		// ??????
		if( ptAct->walkFlag != 0 )
			ptAct->anim_no = ANIM_STAND;
		ptAct->walkFlag = 0;
	}
	ptAct->gx = (int)(ptAct->mx/GRID_SIZE);
	ptAct->gy = (int)(ptAct->my/GRID_SIZE);
}

// ?????????????
void _charMove( ACTION *ptAct )
{
	float mx, my;
	float vx, vy;
	if( ptAct == NULL )
		return;
	mx = (float)ptAct->nextGx*GRID_SIZE;
	my = (float)ptAct->nextGy*GRID_SIZE;
	// ????
	if( ptAct->vx != 0 || ptAct->vy != 0 ){
		vx = ptAct->vx * nowSpdRate;
		vy = ptAct->vy * nowSpdRate;
		// ?佋?????????????
		if( pointLen2( ptAct->mx, ptAct->my, mx, my )
			<= vx*vx+vy*vy ){
			ptAct->mx = mx;
			ptAct->my = my;
			ptAct->vx = 0;
			ptAct->vy = 0;
		}else{
		// ?
			ptAct->mx += vx;
			ptAct->my += vy;
		}
		// ????
		ptAct->anim_no = ANIM_WALK;
	}else{
		// ??????
		if( ptAct->walkFlag != 0 )
			ptAct->anim_no = ANIM_STAND;
		ptAct->walkFlag = 0;
	}
	ptAct->gx = (int)(ptAct->mx/GRID_SIZE);
	ptAct->gy = (int)(ptAct->my/GRID_SIZE);
}

// ???????
void setCharWarpPoint( ACTION *ptAct, int gx, int gy )
{
	if( ptAct == NULL )
		return;
	ptAct->gx = gx;
	ptAct->gy = gy;
	ptAct->nextGx = gx;
	ptAct->nextGy = gy;
	ptAct->mx = (float)gx*GRID_SIZE;
	ptAct->my = (float)gy*GRID_SIZE;
	ptAct->vx = 0;
	ptAct->vy = 0;
}

// ?????????
//
//   status : ?????????????
//   smsg   : ???????
//
//  smsg???status???
//
void setCharStatus( unsigned short *status, char *smsg )
{
	// 
	if( strstr( smsg, "P" ) )
		*status |= CHR_STATUS_P;
	else
		*status &= (~CHR_STATUS_P);
	// ??
	if( strstr( smsg, "N" ) )
		*status |= CHR_STATUS_N;
	else
		*status &= (~CHR_STATUS_N);
	// ?
	if( strstr( smsg, "Q" ) )
		*status |= CHR_STATUS_Q;
	else
		*status &= (~CHR_STATUS_Q);
	// ?
	if( strstr( smsg, "S" ) )
		*status |= CHR_STATUS_S;
	else
		*status &= (~CHR_STATUS_S);
	// ??
	if( strstr( smsg, "D" ) )
		*status |= CHR_STATUS_D;
	else
		*status &= (~CHR_STATUS_D);
	// ??
	if( strstr( smsg, "C" ) )
		*status |= CHR_STATUS_C;
	else
		*status &= (~CHR_STATUS_C);
}

// ???????????
void setCharLeader( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_LEADER;
}

// ????????????????
void delCharLeader( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~CHR_STATUS_LEADER);
}

// ???????????????
void setCharParty( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_PARTY;
}

// ?????????????????
void delCharParty( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~CHR_STATUS_PARTY);
}

// ????????
void setCharWatch( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_WATCH;
}

// ????????????
void delCharWatch( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~CHR_STATUS_WATCH);
}
#ifdef _MIND_ICON
void setCharMind( ACTION *ptAct, int MindNo)
{
	CHAREXTRA *ext;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	ptAct->sMindIcon = MindNo;
	if(ext->ptMindIcon){
		DeathAction( ext->ptMindIcon);
		ext->ptMindIcon = NULL;
	}
	ext->ptMindIcon = createCommmonEffectAction( MindNo, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR);
}
void delCharMind( ACTION *ptAct)
{
	CHAREXTRA *ext;	
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;	
}
#endif
// ?????
void setCharBattle( ACTION *ptAct, int battleNo, short sideNo, short helpMode )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_BATTLE;
	charObj[no].battleNo = battleNo;
	charObj[no].sideNo = sideNo;
	charObj[no].helpMode = helpMode;
	if( helpMode )
		charObj[no].status |= CHR_STATUS_HELP;
	else
		charObj[no].status &= ~CHR_STATUS_HELP;
}

// ?????
void delCharBattle( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~(CHR_STATUS_BATTLE | CHR_STATUS_HELP));
}

// 交易中
//void setCharBattle( ACTION *ptAct, int battleNo, short sideNo, short helpMode )
void setCharTrade( ACTION *ptAct, int battleNo )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_TRADE;
	//charObj[no].battleNo = battleNo;
}

// ?????
void delCharTrade( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~CHR_STATUS_TRADE|CHR_STATUS_LEADER);
}

#ifdef _ANGEL_SUMMON
void setCharAngel( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_ANGEL;
}

void delCharAngel( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= ~CHR_STATUS_ANGEL;
}
#endif

// ???????????
void setCharUseMagic( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_USE_MAGIC;
}

// ???????????????????
// ???????
void delCharUseMagic( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~CHR_STATUS_USE_MAGIC);
}

// ?????????
void setCharFukidashi( ACTION *ptAct, unsigned int offTime )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;

	//JL fix 可能出现0xdddddddd
	if ((int)ext == 0xdddddddd)
	{
		return;
	}

	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_FUKIDASHI;
	ext->drawFukidashiTime = offTime + TimeGetTime();
}

// ??????
void drawCharStatus( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
#ifdef _ANGEL_SUMMON
	unsigned status;
#else
	unsigned short status;
#endif
	int battleNo;
	int sideNo;
	int helpMode;
	char msg[256];
#ifdef _AniCharBubble	   // Syu ADD 动画层人物吐出气泡
	int RandAnimate;
#endif
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	if( pc.ptAct != NULL && pc.ptAct == ptAct ){
		// ???????
		status = pc.status;
		battleNo = 0;
		sideNo = 0;
		helpMode = 0;
	}else{
		status = charObj[no].status;
		battleNo = charObj[no].battleNo;
		sideNo = charObj[no].sideNo;
		helpMode = charObj[no].helpMode;
	}

#ifdef 人物光环
	if (ext->ptmManorIcon) {
		{
			int 人物屏蔽开关 = 0;
			if (人物屏蔽开关 && ProcNo == PROC_GAME && pc.ptAct != ptAct) {
				if (ATR_PAT_NO(ext->ptmManorIcon)) {
					ATR_PAT_BAK_NO(ext->ptmManorIcon) = ATR_PAT_NO(ext->ptmManorIcon);
					ATR_PAT_NO(ext->ptmManorIcon) = 0;
				}

			}
			else {
				if (!ATR_PAT_NO(ext->ptmManorIcon)) {
					if (ATR_PAT_BAK_NO(ext->ptmManorIcon)) {
						ATR_PAT_NO(ext->ptmManorIcon) = ATR_PAT_BAK_NO(ext->ptmManorIcon);
					}
				}
				short x1, y1;
				if (获取动画尺寸(ptAct, &x1, &y1)) {
					int y = SpriteInfo[ptAct->bmpNo].height;
					ext->ptmManorIcon->x = ptAct->x;
					ext->ptmManorIcon->y = ptAct->y - y - 20;
					ext->ptmManorIcon->mx = ptAct->mx;
					ext->ptmManorIcon->my = ptAct->my;
					pattern(ext->ptmManorIcon, ANM_NOMAL_SPD, ANM_LOOP);
#ifdef _CAX_STOP_TEXIAO
					extern int stoptxkk;
					if (stoptxkk == 0) {
#endif
						StockDispBuffer2(ext->ptmManorIcon->x, ext->ptmManorIcon->y, ext->ptmManorIcon->dispPrio, ext->ptmManorIcon->bmpNo, 0);
#ifdef _CAX_STOP_TEXIAO
					}
#endif
				}
			}
		}
	}
#endif

#ifdef 人物称号
	if (ext->ptTitleIcon) {//这里删除掉就可以走动显示
		{
			int 人物屏蔽开关 = 0;
			if (人物屏蔽开关 && ProcNo == PROC_GAME && pc.ptAct != ptAct) {
				if (ATR_PAT_NO(ext->ptTitleIcon)) {
					ATR_PAT_BAK_NO(ext->ptTitleIcon) = ATR_PAT_NO(ext->ptTitleIcon);
					ATR_PAT_NO(ext->ptTitleIcon) = 0;
				}
			}else{
				if (!ATR_PAT_NO(ext->ptTitleIcon)) {
					if (ATR_PAT_BAK_NO(ext->ptTitleIcon)) {
						ATR_PAT_NO(ext->ptTitleIcon) = ATR_PAT_BAK_NO(ext->ptTitleIcon);
					}
				}
				short x1, y1, x2, y2;
				y2 = x2 = 0;
				if (获取动画尺寸(ptAct, &x1, &y1)) {
					ext->ptTitleIcon->x = ptAct->x;
					ext->ptTitleIcon->y = ptAct->y;
					ext->ptTitleIcon->mx = ptAct->mx;
					ext->ptTitleIcon->my = ptAct->my;
					pattern(ext->ptTitleIcon, ANM_NOMAL_SPD, ANM_LOOP);
					//if (ext->ptFamilyIcon) {  //家族徽章
					//	x2 = 10;
					//}
#ifdef _CAX_STOP_TEXIAO
					extern int stoptxkk;
					if (stoptxkk == 0) {
#endif
						setCharPrio(ext->ptTitleIcon->bmpNo,
							ext->ptTitleIcon->x + x2, ext->ptTitleIcon->y, 0, -(y1 / 2 +45),
							ext->ptTitleIcon->mx, ext->ptTitleIcon->my
#ifdef _SFUMATO
							, ptAct->sfumato
#endif
						);


					}
#ifdef _CAX_STOP_TEXIAO
				}
#endif
			}
		}
	}
#endif


	if( (status & CHR_STATUS_BATTLE) != 0 ){
		int no;
		U4 bmpNo;
		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;
		realGetNo( battleIconTbl[(battleNo%10)][(sideNo%1)], &bmpNo );
		setCharPrio( bmpNo, ptAct->x, ptAct->y, 0, charIconOffsetY[no], ptAct->mx, ptAct->my );
	}
	// Trade Mark
	if( (status & CHR_STATUS_TRADE) != 0 ){
		int no;
		U4 bmpNo;
		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;
		realGetNo( CG_TRADE_MARK, &bmpNo );		
		setCharPrio( bmpNo, ptAct->x, ptAct->y, 0, charIconOffsetY[no], ptAct->mx, ptAct->my );
	}

#ifdef _ANGEL_SUMMON
	if( (status & CHR_STATUS_ANGEL) != 0  && ext->ptActAngelMark == NULL ) {
		//if( g_bUseAlpha )
		//	ext->ptActAngelMark =createCommmonEffectAction( 101812, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR );  // JL fix图档可能有错误
		//else
			ext->ptActAngelMark =createCommmonEffectAction( 101865, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR );

	}else if( (status & CHR_STATUS_ANGEL) == 0 && ext->ptActAngelMark != NULL ) {
		// ??????????????
		DeathAction( ext->ptActAngelMark );
		ext->ptActAngelMark = NULL;
	}else if( (status & CHR_STATUS_ANGEL) != 0 && ext->ptActAngelMark != NULL ) {
		int no;
		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;	// ?????????
		// ???????????????
		ext->ptActAngelMark->x  = ptAct->x;
		ext->ptActAngelMark->y  = ptAct->y + ptAct->anim_y + 140;
		ext->ptActAngelMark->mx = ptAct->mx;
		ext->ptActAngelMark->my = ptAct->my;
		// ?????????
		pattern( ext->ptActAngelMark, ANM_NOMAL_SPD, ANM_LOOP );
		// ??????????????
		setCharPrio( ext->ptActAngelMark->bmpNo,
			ext->ptActAngelMark->x, ext->ptActAngelMark->y, 0, charIconOffsetY[no],
				ext->ptActAngelMark->mx, ext->ptActAngelMark->my );
	}
#endif
	if( (status & CHR_STATUS_WATCH) != 0 ){
		int no;
		U4 bmpNo;
		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;
		realGetNo( CG_ICON_WATCHING, &bmpNo );
		setCharPrio( bmpNo, ptAct->x, ptAct->y, 0, charIconOffsetY[no], ptAct->mx, ptAct->my );
	}
	if( (status & CHR_STATUS_HELP) != 0 ){
		int no;
		U4 bmpNo;
		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;
		realGetNo( CG_SPEECH_HELP, &bmpNo );
		setCharPrio( bmpNo, ptAct->x, ptAct->y, 0, charIconOffsetY[no]-20, ptAct->mx, ptAct->my );
	}
	if( (status & CHR_STATUS_LEADER) != 0
	 && ext->ptActLeaderMark == NULL
	 && ((status & CHR_STATUS_BATTLE) == 0 || (status & CHR_STATUS_WATCH) == 0) ){
		ext->ptActLeaderMark =
			createCommmonEffectAction( SPR_leader, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR );
	}else if( ((status & CHR_STATUS_LEADER) == 0 || (status & CHR_STATUS_BATTLE) != 0)
	 && ext->ptActLeaderMark != NULL ){
		DeathAction( ext->ptActLeaderMark );
		ext->ptActLeaderMark = NULL;
	}else if( (status & CHR_STATUS_LEADER) != 0 && ext->ptActLeaderMark != NULL ){
		int no;
		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;
		ext->ptActLeaderMark->x  = ptAct->x;
		ext->ptActLeaderMark->y  = ptAct->y;
		ext->ptActLeaderMark->mx = ptAct->mx;
		ext->ptActLeaderMark->my = ptAct->my;
		pattern( ext->ptActLeaderMark, ANM_NOMAL_SPD, ANM_LOOP );
		setCharPrio( ext->ptActLeaderMark->bmpNo,
			ext->ptActLeaderMark->x, ext->ptActLeaderMark->y, 0, charIconOffsetY[no],
				ext->ptActLeaderMark->mx, ext->ptActLeaderMark->my );
	}
	if( (status & CHR_STATUS_USE_MAGIC) != 0 && ext->ptActMagicEffect == NULL ){
		ext->ptActMagicEffect =
			createCommmonEffectAction( SPR_effect01, ptAct->gx, ptAct->gy+1, 0, 0, DISP_PRIO_CHAR );
		if( pc.ptAct != NULL && pc.ptAct == ptAct )
			delPcUseMagic();
		else
			delCharUseMagic( ptAct );
	}else if( ext->ptActMagicEffect != NULL ){
		ext->ptActMagicEffect->x  = ptAct->x;
		ext->ptActMagicEffect->y  = ptAct->y;
		ext->ptActMagicEffect->mx = ptAct->mx;
		ext->ptActMagicEffect->my = ptAct->my;
		if( pattern( ext->ptActMagicEffect, ANM_NOMAL_SPD, ANM_NO_LOOP ) == 0 ){
			setCharPrio( ext->ptActMagicEffect->bmpNo,
				ext->ptActMagicEffect->x, ext->ptActMagicEffect->y+1, 0, 0,
				ext->ptActMagicEffect->mx, ext->ptActMagicEffect->my );
		}else{
			DeathAction( ext->ptActMagicEffect );
			ext->ptActMagicEffect = NULL;
		}
	}
	if( (status & CHR_STATUS_FUKIDASHI) != 0 ){
		if( ext->drawFukidashiTime > TimeGetTime() ){
			int no;
			U4 bmpNo;
			realGetNo( CG_ICON_FUKIDASI, &bmpNo );
			if( SPR_001em <= ptAct->anim_chr_no && ptAct->anim_chr_no <= SPR_114bw ){
				no = (ptAct->anim_chr_no - SPR_001em) / 20;
				no %= 12;
				setCharPrio( bmpNo, ptAct->x, ptAct->y, 20, charIconOffsetY[no], ptAct->mx, ptAct->my );
			}else{
				setCharPrio( bmpNo, ptAct->x, ptAct->y, 20, -84, ptAct->mx, ptAct->my );
			}
		}else
			status &= (~CHR_STATUS_FUKIDASHI);
	}
#ifdef _NB_文字称号
	extern void PutTitleText(int x,int y,char fontPrio,TITLE_STR str,BOOL hitFlag);
	CHAREXTRA *ext1 = (CHAREXTRA *)ptAct->pYobi;
	if(ext1){
		if(!ext1->ptStreetVendor){
			if(strlen(ptAct->nb_name)>0){
				short x1,y1;
				if(获取动画尺寸(ptAct,&x1,&y1)){
					StockFontBuffer( ptAct->x-GetStrWidth( ptAct->nb_name )/2, ptAct->y - (y1/2+45), FONT_PRIO_BACK, ptAct->nb_namecolor, ptAct->nb_name, 0 );
				}
			}	
		}
	}
#endif
	if( nameOverTheHeadFlag
	 && ptAct != pc.ptAct
	 && (ptAct->atr & (ACT_ATR_TYPE_PC | ACT_ATR_TYPE_OTHER_PC | ACT_ATR_TYPE_PET | ACT_ATR_TYPE_OTHER))
	 && ptAct->anim_chr_no != 999 ){
		if( (ptAct->atr & ACT_ATR_TYPE_PET) && strlen( ptAct->freeName ) > 0 )
			sprintf( msg, "%s", ptAct->freeName );
		else{
				sprintf( msg, "%s", ptAct->fmname);
				StockFontBuffer( ptAct->x-GetStrWidth( msg )/2, ptAct->y-23, FONT_PRIO_BACK, ptAct->charNameColor, msg, 0 );

#ifdef _F11_SHOW_ID
				char *lssproto_cnv10to62(int a, char *out, int outlen);
				char c62[20];
				lssproto_cnv10to62(ptAct->objIndex, c62, 20);
				sprintf(msg, "%s %s(%d)", ptAct->name, c62, ptAct->objIndex);
#else
				sprintf(msg, "%s", ptAct->name);
#endif // _F11_SHOW_ID
		}
		StockFontBuffer( ptAct->x-GetStrWidth( msg )/2, ptAct->y-10, FONT_PRIO_BACK, ptAct->charNameColor, msg, 0 );
	}

#ifdef _GM_MARK				   // WON ADD GM 识别
	{
		int graNo = ptAct->anim_chr_no ;

		//三种ＧＭ造形
		if( (graNo == 100420) || (graNo == 100425) || (graNo == 100445) ){
			static count1=1, count2=0;
			char msg[256];

			if(count1 >10 ) count1=1;
			if(count2 >15 ){				
				count1++;
				count2=0;
			}

			count2++;
			sprintf( msg , "ＧＭ");
			StockFontBuffer( ptAct->x-GetStrWidth( msg )/2, ptAct->y-110, FONT_PRIO_BACK, count1, msg, 1);	
		}
	}
#endif

#ifdef _GM_IDENTIFY		// Rog ADD GM识别
		int i;
		sprintf( msg , ptAct->gm_name);
        for(i=2;i>0;i--)
	        StockFontBuffer( ptAct->x-GetStrWidth( msg )/2, ptAct->y-110-i, FONT_PRIO_BACK, i , msg, 1);
#endif

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业

//	if( ptAct->profession_class != 0 ){
	{
		char msg[10];

		switch(pc.profession_class){
		case 0:		sprintf( msg , "");	break;
		case 1:		sprintf( msg , "白狼勇士");	break;
		case 2:		sprintf( msg , "闇灵法师");	break;
		case 3:		sprintf( msg , "追猎者");	break;
		}
		sprintf( pc.profession_class_name , "%s" , msg ) ; 
		//StockFontBuffer( ptAct->x-GetStrWidth( msg )/2, ptAct->y-110, FONT_PRIO_BACK, FONT_PAL_YELLOW, msg, 1);
		//StockFontBuffer( ptAct->x-GetStrWidth( msg )/2, ptAct->y-110, FONT_PRIO_BACK, FONT_PAL_YELLOW, msg, 1);
		//StockDispBuffer( ptAct->x-GetStrWidth( msg )/2 + 30, ptAct->y-90 , DISP_PRIO_IME3, CG_WAR_ICON_SMALL + pc.profession_class - 1, 0 );
						
	}

#endif

#ifdef _AniCharBubble	   // Syu ADD 动画层人物吐出气泡
	if ( ( (nowFloor == 817) || (nowFloor == 8007) || (nowFloor == 8101) || (nowFloor == 8100) ||
		(nowFloor == 8027) || (nowFloor == 8028) || (nowFloor == 8029) || nowFloor == 8113 || nowFloor == 8114 ||
		(nowFloor == 8015) ) && (popflag == false) && (waittimeflag == false))
	{
		RandAnimate = RAND ( 101508 , 101509 );  //随机选取左气泡或右气泡
		if ( popAct == NULL )  
		{
			//做左、右气泡产生时需要的偏移值
			if( RandAnimate == 101508 )  
				popAct = createWaterAnimation( RandAnimate , nowGx + 2 , nowGy - 1 , 30 );
			if( RandAnimate == 101509 )
				popAct = createWaterAnimation( RandAnimate , nowGx + 1 , nowGy - 2 , 30 );
			popflag = true;
		}
	}
	if ( waittimeflag == false && popAct != NULL)
	if ( popAct->anim_cnt == 6 ) //拨放到最后一张
	{
		if ( popAct != NULL)
		{
			DeathAction( popAct );
			popAct = NULL;
		}
		popflag = false;
		waittimeflag = true;
	}
	if ( waittimeflag == true ) //计时等待下一次产生时间
	{
		waittime ++ ;
		if (waittime > 350 ) 
		{
			waittime = 0;
			waittimeflag = false;
		}
	}
	if ( ( (nowFloor != 817) && (nowFloor != 8007) && (nowFloor != 8101) && (nowFloor != 8100) && (nowFloor != 8113) && (nowFloor != 8114) &&
		(nowFloor != 8027) && (nowFloor != 8028) && (nowFloor != 8029) && (nowFloor != 8015)) && (popflag == true))
	{
		popflag = false;
		waittimeflag = false;  
		if ( popAct != NULL)
		{
			DeathAction ( popAct ) ;
			popAct = NULL;
		}
	}

#endif
#ifdef __EMOTION
	if( ext->ptActEmotion){
		if( ext->ptActEmotion->mx != ptAct->mx ||
			ext->ptActEmotion->my != ptAct->my ||
			ext->ptActEmotion->x != ptAct->x ||
			ext->ptActEmotion->y != ptAct->y ){
			DeathAction( ext->ptActEmotion);
			ext->ptActEmotion = NULL;
		}else{
			int no;
			no = (ptAct->anim_chr_no - SPR_001em) / 20;
			no %= 12;	// ?????????
			// ?????????
			pattern( ext->ptActEmotion, ANM_NOMAL_SPD, ANM_LOOP );
			// ??????????????
			setCharPrio( ext->ptActEmotion->bmpNo,
				ext->ptActEmotion->x, ext->ptActEmotion->y, 0, charIconOffsetY[no],
					ext->ptActEmotion->mx, ext->ptActEmotion->my );
		}
	}
#endif
#ifdef _STREET_VENDOR
	if(ext->ptStreetVendor){
		ext->ptStreetVendor->x  = ptAct->x;
		ext->ptStreetVendor->y  = ptAct->y;
		ext->ptStreetVendor->mx = ptAct->mx;
		ext->ptStreetVendor->my = ptAct->my;
#ifdef _STREET_VENDOR_CHANGE_ICON
		StockDispBuffer(ext->ptStreetVendor->x,ext->ptStreetVendor->y + 35,DISP_PRIO_TILE+1,35343,0);
		StockDispBuffer(ext->ptStreetVendor->x - 10,ext->ptStreetVendor->y + 10,DISP_PRIO_CHAR-1,35344,0);
		StockDispBuffer(ext->ptStreetVendor->x,ext->ptStreetVendor->y - 15,DISP_PRIO_CHAR+1,35345,0);
#else
	#ifdef _READ16BITBMP
		StockDispBuffer(ext->ptStreetVendor->x,ext->ptStreetVendor->y - 80,DISP_PRIO_CHAR,OLD_GRAPHICS_START+58,10);
	#endif
#endif
			StockFontBuffer(ext->ptStreetVendor->x - 65,ext->ptStreetVendor->y - 92,
										FONT_PRIO_BACK,MyChatBuffer.color,ptAct->szStreetVendorTitle,0);
	}
#endif
#ifdef _MIND_ICON
	if( ext->ptMindIcon){		
		if( ((ptAct->sMindIcon != SPR_asleep) &&
		     (ptAct->sMindIcon != SPR_cafe) &&
		     (ptAct->sMindIcon != SPR_accent) ) &&
			 pattern( ext->ptMindIcon, ANM_NOMAL_SPD, ANM_NO_LOOP)
		    ){
			DeathAction( ext->ptMindIcon);
			ext->ptMindIcon = NULL;
		}else{
			int no;
			no = (ptAct->anim_chr_no - SPR_001em) / 20;
			no %= 12;
			
			ext->ptMindIcon->x  = ptAct->x;
			ext->ptMindIcon->y  = ptAct->y;
			ext->ptMindIcon->mx = ptAct->mx;
			ext->ptMindIcon->my = ptAct->my;
			
			pattern( ext->ptMindIcon, ANM_NOMAL_SPD, ANM_LOOP );
			
			setCharPrio( ext->ptMindIcon->bmpNo,
				ext->ptMindIcon->x, ext->ptMindIcon->y, 0, charIconOffsetY[no],
				ext->ptMindIcon->mx, ext->ptMindIcon->my );			
		}
	}
#endif

#ifdef _NB_任务指引
	if( ext->ptNoticeIcon){//这里删除掉就可以走动显示
		if(!ATR_PAT_NO(ext->ptNoticeIcon) ){
			if(ATR_PAT_BAK_NO(ext->ptNoticeIcon)){
				ATR_PAT_NO(ext->ptNoticeIcon) = ATR_PAT_BAK_NO(ext->ptNoticeIcon);
			}
		}
		short x1,y1,x2,y2;
		y2=x2=0;
		if(获取动画尺寸(ptAct,&x1,&y1)){
			ext->ptNoticeIcon->x  = ptAct->x;
			ext->ptNoticeIcon->y  = ptAct->y;
			ext->ptNoticeIcon->mx = ptAct->mx;
			ext->ptNoticeIcon->my = ptAct->my;
			pattern( ext->ptNoticeIcon, ANM_NOMAL_SPD, ANM_LOOP );
			setCharPrio( ext->ptNoticeIcon->bmpNo,
				ext->ptNoticeIcon->x-x2, ext->ptNoticeIcon->y, 0,-(y1/2+55),
				ext->ptNoticeIcon->mx, ext->ptNoticeIcon->my
#ifdef _SFUMATO
				, ptAct->sfumato
#endif
				);
		}
	}
#endif

#ifdef _ITEM_FIREWORK
	if (ext->pActFirework[0])	// 有要放烟火
	{
		float	fX, fY;

		camMapToGamen((float)ext->pActFirework[0]->gx * GRID_SIZE, (float)ext->pActFirework[0]->gy * GRID_SIZE, &fX, &fY);
		ext->pActFirework[0]->x	= (int)fX;
		ext->pActFirework[0]->y	= (int)fY;
		// 如果第一个烟火放完了,再来以type来??接下来要怎么放
		if (pattern(ext->pActFirework[0], ANM_NOMAL_SPD, ANM_NO_LOOP))
		{
			if (ext->pActFirework[0]->damage == 0)	// 一个放中间二个左右
			{
				int nGX, nGY;
				
				nGX = ext->pActFirework[0]->gx;
				nGY = ext->pActFirework[0]->gy;
				ext->pActFirework[1] = GetAction(PRIO_CHR, 0);
				ext->pActFirework[1]->damage			= 1;
				ext->pActFirework[1]->anim_chr_no		= ext->pActFirework[0]->anim_chr_no;
				ext->pActFirework[1]->anim_no			= ANIM_STAND;
				ext->pActFirework[1]->anim_ang			= 1;
				ext->pActFirework[1]->actNo				= 0;
				ext->pActFirework[1]->dispPrio			= DISP_PRIO_RESERVE;
				ext->pActFirework[0]->gx				= nGX - 2;
				ext->pActFirework[0]->gy				= nGY - 2;
				ext->pActFirework[0]->anim_frame_cnt	= 0;
				ext->pActFirework[0]->anim_cnt			= 0;
				ext->pActFirework[1]->gx				= nGX + 2;
				ext->pActFirework[1]->gy				= nGY + 2;
				ext->pActFirework[0]->damage			= -1;
			}
			else if (ext->pActFirework[0]->damage == 1)	// 中间三次
			{
				ext->pActFirework[1] = GetAction(PRIO_CHR, 0);
				ext->pActFirework[1]->anim_chr_no	= ext->pActFirework[0]->anim_chr_no;
				ext->pActFirework[1]->anim_no		= ANIM_STAND;
				ext->pActFirework[1]->anim_ang		= 1;
				ext->pActFirework[1]->actNo			= 0;
				ext->pActFirework[1]->dispPrio		= DISP_PRIO_RESERVE;
				ext->pActFirework[1]->level			= 1;					// level 拿来当计数用
				ext->pActFirework[1]->gx			= ext->pActFirework[0]->gx;
				ext->pActFirework[1]->gy			= ext->pActFirework[0]->gy;
				ext->pActFirework[0]->damage		= -2;
			}
			else if (ext->pActFirework[0]->damage == 2 || ext->pActFirework[0]->damage == -1)
				ext->pActFirework[0]->damage = -2;
			else if (ext->pActFirework[0]->damage == 3)	// 绕人物四周一圈, 共三次
			{
				ext->pActFirework[1] = GetAction(PRIO_CHR, 0);
				ext->pActFirework[1]->anim_chr_no	= ext->pActFirework[0]->anim_chr_no;
				ext->pActFirework[1]->anim_no		= ANIM_STAND;
				ext->pActFirework[1]->anim_ang		= 1;
				ext->pActFirework[1]->actNo			= 0;
				ext->pActFirework[1]->dispPrio		= DISP_PRIO_RESERVE;
				ext->pActFirework[1]->level			= 10;					// level 拿来当计数用
				ext->pActFirework[1]->damage		= ext->pActFirework[0]->damage;
				ext->pActFirework[1]->gx			= ext->pActFirework[0]->bufGx[0] - 2;
				ext->pActFirework[1]->gy			= ext->pActFirework[0]->bufGy[0] - 2;
				ext->pActFirework[1]->bufGx[0]		= ext->pActFirework[0]->bufGx[0];
				ext->pActFirework[1]->bufGy[0]		= ext->pActFirework[0]->bufGy[0];
				ext->pActFirework[0]->damage		= -2;
			}
		}
		if (ext->pActFirework[0]->damage == -2 || warpEffectStart)
		{
			DeathAction(ext->pActFirework[0]);
			ext->pActFirework[0] = NULL;
		}
	}
	if (ext->pActFirework[1])
	{
		float	fX, fY;

		camMapToGamen((float)ext->pActFirework[1]->gx * GRID_SIZE, (float)ext->pActFirework[1]->gy * GRID_SIZE, &fX, &fY);
		ext->pActFirework[1]->x	= (int)fX;
		ext->pActFirework[1]->y	= (int)fY;
		if (pattern(ext->pActFirework[1], ANM_NOMAL_SPD, ANM_NO_LOOP))
		{
			if (--ext->pActFirework[1]->level >= 0)	// 计算还要跑几次
			{
				ext->pActFirework[1]->anim_frame_cnt	= 0;
				ext->pActFirework[1]->anim_cnt			= 0;
				if (ext->pActFirework[1]->damage == 3)
				{
					int nXY[4][2] = { {3,-3}, {2,2}, {-3,3}, {-2,-2}};
					int nDir = ext->pActFirework[1]->level % 4;

					ext->pActFirework[1]->gx	= ext->pActFirework[1]->bufGx[0] + nXY[nDir][0];
					ext->pActFirework[1]->gy	= ext->pActFirework[1]->bufGy[0] + nXY[nDir][1];
				}
			}
			else
			{
				DeathAction(ext->pActFirework[1]);
				ext->pActFirework[1] = NULL;
			}
		}
		if (warpEffectStart)
		{
			DeathAction(ext->pActFirework[1]);
			ext->pActFirework[1] = NULL;
		}
	}
#endif

#ifdef ITEM_AURA_
	if (ext->ptActAura1)
	{
		ext->ptActAura1->x = ptAct->x;
		ext->ptActAura1->y = ptAct->y;
		ext->ptActAura1->mx = ptAct->mx;
		ext->ptActAura1->my = ptAct->my;
		pattern(ext->ptActAura1, ANM_NOMAL_SPD, ANM_LOOP);
	}
	if (ext->ptActAura2)
	{
		ext->ptActAura2->x = ptAct->x;
		ext->ptActAura2->y = ptAct->y;
		ext->ptActAura2->mx = ptAct->mx;
		ext->ptActAura2->my = ptAct->my;
		pattern(ext->ptActAura2, ANM_NOMAL_SPD, ANM_LOOP);
	}
#endif


#ifdef _NPC_PICTURE
	if( ((ptAct->picture>>24) & 0x000000ff) == 2 && ext->ptActPicture == NULL ) {
		if( g_bUseAlpha )
			ext->ptActPicture =
				createCommmonEffectAction( 101812, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR );
		else
			ext->ptActPicture =
				createCommmonEffectAction( 101865, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR );
	}
	else if( ((ptAct->picture>>24) & 0x000000ff) != 2 && ext->ptActPicture != NULL ) {
		// ??????????????
		DeathAction( ext->ptActAngelMark );
		ext->ptActAngelMark = NULL;
	}
	else if( ((ptAct->picture>>24) & 0x000000ff) == 2 && ext->ptActPicture != NULL ) {
		int pictable[9]={100388,100841,101178,100854,101570,100353,101759,101424,101489};
		if( ptAct->anim_chr_no == pictable[((ptAct->picture&0x00ff0000)>>16)&0x000000ff] ){
			int no;
			no = (ptAct->anim_chr_no - SPR_001em) / 20;
			no %= 12;	// ?????????
			// ???????????????
			ext->ptActPicture->x  = ptAct->x;
			ext->ptActPicture->y  = ptAct->y + ptAct->anim_y + 140;
			ext->ptActPicture->mx = ptAct->mx;
			ext->ptActPicture->my = ptAct->my;
			// ?????????
			pattern( ext->ptActPicture, ANM_NOMAL_SPD, ANM_LOOP );
			// ??????????????
			setCharPrio( ext->ptActPicture->bmpNo, ext->ptActPicture->x, ext->ptActPicture->y, 0, charIconOffsetY[no], ext->ptActPicture->mx, ext->ptActPicture->my );
		}
	}
	/*if( ((ptAct->picture>>24) & 0x000000ff) == 2 ){
		int pictable[9]={100388,100841,101178,100854,101570,100353,101759,101424,101489};
		if( ptAct->anim_chr_no == pictable[((ptAct->picture&0x00ff0000)>>16)&0x000000ff] ){
			int no;
			U4 bmpNo;
			no = (ptAct->anim_chr_no - SPR_001em) / 20;
			no %= 12;	// ?????????
			realGetNo( CG_TRADE_MARK, &bmpNo );
			// ??????????????
			setCharPrio( bmpNo, ptAct->x, ptAct->y, 0, charIconOffsetY[no], ptAct->mx, ptAct->my );
		}
	}
	*/
#endif

}
#ifdef _AniCharBubble	   // Syu ADD 动画层人物吐出气泡
void CharbubbleRelease()
{
	if ( popAct != NULL )
	{
		DeathAction(popAct);
		popAct = NULL;
	}
	popflag = false;
	waittimeflag = false;  
}
#endif
// ?????????????
void setCharNameColor( ACTION *ptAct, int color )
{
	CHAREXTRA *ext;
	int no;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	ptAct->charNameColor = color;
	charObj[no].charNameColor = color;
}

// ???????? /////////////////////////////////////////////////
ACTION *createCommmonEffectAction( int graNo, int gx, int gy, int anim, int dir, int prio )
{
	ACTION *ptAct;
	float mx, my;
	/* ?????????? */
	ptAct = GetAction( PRIO_CHR, 0 );
	if( ptAct == NULL )
		return NULL;
	// ????????
	ptAct->anim_chr_no = graNo;
	// ??
	ptAct->anim_no = anim;
	// ?????????( ??? )( ??????? )
	ptAct->anim_ang = dir;
	// ?
	ptAct->dispPrio = prio;
	// 1????????
	ptAct->atr = ACT_ATR_HIDE2;
	// ???
	ptAct->nextGx = gx;					// ???????????
	ptAct->nextGy = gy;
	ptAct->gx = gx;						// ???????????佋?
	ptAct->gy = gy;
	ptAct->mx = (float)gx * GRID_SIZE;	// ????
	ptAct->my = (float)gy * GRID_SIZE;
	ptAct->vx = 0;						// ??
	ptAct->vy = 0;
	// ??
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)(mx+.5);
	ptAct->y = (int)(my+.5);
	return ptAct;
}

// ???????????????? /////////////////////////////////
// ????????????????????????
void commmonEffectNoLoop( ACTION *ptAct )
{
	float mx, my;
	// ??
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)(mx+.5);
	ptAct->y = (int)(my+.5);
	// ?????????
	if( pattern( ptAct, ANM_NOMAL_SPD, ANM_NO_LOOP ) == 0 )
		// ??????????????
		setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y+1, 0, 0, ptAct->mx, ptAct->my );
	else{
		// ???????????????????
		DeathAction( ptAct );
		ptAct = NULL;
	}
}

ACTION *createCommmonEffectNoLoop( int graNo, int gx, int gy, int anim, int dir, int prio )
{
	ACTION *ptAct;
	float mx, my;
	/* ?????????? */
	ptAct = GetAction( PRIO_CHR, 0 );
	if( ptAct == NULL )
		return NULL;
	// ???
	ptAct->func = commmonEffectNoLoop;
	// ????????
	ptAct->anim_chr_no = graNo;
	// ??
	ptAct->anim_no = anim;
	// ?????????( ??? )( ??????? )
	ptAct->anim_ang = dir;
	// ?
	ptAct->dispPrio = prio;
	// 1????????
	ptAct->atr = ACT_ATR_HIDE2;
	// ???
	ptAct->nextGx = gx;					// ???????????
	ptAct->nextGy = gy;
	ptAct->gx = gx;						// ???????????佋?
	ptAct->gy = gy;
	ptAct->mx = (float)gx * GRID_SIZE;	// ????
	ptAct->my = (float)gy * GRID_SIZE;
	ptAct->vx = 0;						// ??
	ptAct->vy = 0;
	// ??
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)(mx+.5);
	ptAct->y = (int)(my+.5);
	return ptAct;
}

// ????? /////////////////////////////////////////////////////////
void changeCharAct( ACTION *ptAct, int x, int y, int dir, int action,int effectno, int effectparam1, int effectparam2 )
{
	CHAREXTRA *ext;
	int no;
	int i;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	if ((int)ext == 0xdddddddd)
	{
		return;
	}
	no = ext->charObjTblId;
	switch( action ){
		case 0:
			ptAct->bufCount = 0;
			ptAct->anim_no = ANIM_STAND;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
#ifdef _SHOWFAMILYBADGE_
		case 43:
			if (effectno > 0) {
				setCharmTitle(ptAct, effectno);
			}
			else
				delCharmTitle(ptAct);
			break;
#endif
#ifdef FAMILY_MANOR_
		case 44:
			if (effectno >= 0) {
				if (pc.familyName[0])
					setCharmFamily(ptAct, effectno);
			}
			else
				delCharmFamily(ptAct);
			break;
#endif
#ifdef 人物称号
		case 45:
			if (effectno >= 0) {
				setCharmTitle(ptAct, effectno);
			}
			else
				delCharmTitle(ptAct);
			break;
#endif
#ifdef 人物光环
		case 46:
			if (effectno > 0) {
				setCharmManor(ptAct, effectno);
			}
			else
				delCharmManor(ptAct);
			break;
#endif

		case 31:
			setCharWarpPoint( ptAct, x, y );
			ptAct->bufCount = 0;
			ptAct->anim_no = ANIM_STAND;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			charObj[no].stockDir = -1;
			break;
		case 30:
			charObj[no].stockDir = dir;
			charObj[no].stockDirX = x;
			charObj[no].stockDirY = y;
			if( (x == ptAct->nextGx && y == ptAct->nextGy) || ABS( ABS( x - ptAct->nextGx ) - ABS( y - ptAct->nextGy ) ) > 1 )
				break;
		case 1:
			if( (charObj[no].status & CHR_STATUS_PARTY) != 0 && (charObj[no].status & CHR_STATUS_LEADER) != 0 ){
				if( ptAct->bufCount < sizeof( ptAct->bufGx )/sizeof( int ) )
					correctCharMovePoint( ptAct, x, y );
				else{
					stockCharMovePoint( ptAct, x, y );
					charObj[no].stockDir = -1;
					for( i = 1; i < MAX_PARTY; i++ ){
						if( party[i].useFlag != 0 ){
							if( party[i].id != pc.id ){
								//JL fix 2018/08/28 点pk时这里崩端,加个ptAct不为0的验证
								if (party[i].ptAct){
									party[i].ptAct->bufCount = 0;
									setCharWarpPoint(party[i].ptAct, x, y);
								}
							}else{
								if( pc.ptAct != NULL )
									pc.ptAct->bufCount = 0;
								setPcWarpPoint( x, y );
							}
						}
					}
				}
			}else
				stockCharMovePoint( ptAct, x, y );
			ptAct->anim_no = ANIM_WALK;
			break;
		case 2:
			ptAct->anim_no = ANIM_ATTACK;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 3:
			ptAct->anim_no = ANIM_THROW;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 4:
			ptAct->anim_no = ANIM_DAMAGE;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 5:
			ptAct->anim_no = ANIM_DEAD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:		/* UsedMagic */
			break;
		case 10:
			ptAct->anim_no = ANIM_DEAD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 11:
			ptAct->anim_no = ANIM_SIT;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 12:
			ptAct->anim_no = ANIM_HAND;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 13:
			ptAct->anim_no = ANIM_HAPPY;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 14:
			ptAct->anim_no = ANIM_ANGRY;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 15:
			ptAct->anim_no = ANIM_SAD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 16:
			ptAct->anim_no = ANIM_GUARD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 17:
			ptAct->anim_no = ANIM_WALK;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 18:
			ptAct->anim_no = ANIM_NOD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 19:
			ptAct->anim_no = ANIM_STAND;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;
		case 20:
			if( effectno >= 0 ){
				setCharBattle( ptAct, effectno, effectparam1, effectparam2 );
				ptAct->anim_no = ANIM_STAND;
				ptAct->anim_no_bak = -1;
			}else
				delCharBattle( ptAct );
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_ang = dir;
			break;
		case 21:
			if( effectno == 1 )
				setCharLeader( ptAct );
			else
				delCharLeader( ptAct );
			ptAct->anim_ang = dir;
			break;
		case 22:
			if( effectno == 1 )
				setCharWatch( ptAct );
			else
				delCharWatch( ptAct );
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_ang = dir;
			break;
		case 23:
			setCharNameColor( ptAct, effectno );
			break;

		// 交易中
		case 32:
			if( effectno >= 0 ){
				setCharTrade( ptAct, effectno );
				ptAct->anim_no = ANIM_STAND;
				ptAct->anim_no_bak = -1;
			}else
				delCharTrade( ptAct );
			//setCharWarpPoint( ptAct, x, y );
			ptAct->anim_ang = dir;
			break;

#ifdef _ANGEL_SUMMON
		case 34:
			if( effectno > 0 ){
				setCharAngel( ptAct );
				// ?????????
				ptAct->anim_no = ANIM_STAND;
				ptAct->anim_no_bak = -1;
			}else
				delCharAngel( ptAct );
			//setCharWarpPoint( ptAct, x, y );
			ptAct->anim_ang = dir;
			break;
#endif

#ifdef _STREET_VENDOR
		case 41:
			if(ext->ptStreetVendor){
				DeathAction(ext->ptStreetVendor);
				ext->ptStreetVendor = NULL;
			}
			ext->ptStreetVendor = GetAction(PRIO_CHR,0);
			break;
		case 42:
			if(ext->ptStreetVendor){
				DeathAction(ext->ptStreetVendor);
				ext->ptStreetVendor = NULL;
			}
			break;
#endif
#ifdef _MIND_ICON
		case 40:
			if(effectno >= 0)
				setCharMind( ptAct, effectno);				
			else
				delCharMind( ptAct);
			break;
#endif
#ifdef _ITEM_CRACKER
		case 50:
			if(effectno >= 0)
				setCharMind( ptAct, effectno);				
			else
				delCharMind( ptAct);
			break;
#endif
#ifdef _ITEM_FIREWORK
		case 51:
			if (ext->pActFirework[0])
			{
				DeathAction(ext->pActFirework[0]);
				ext->pActFirework[0] = NULL;
			}
			if (ext->pActFirework[1])
			{
				DeathAction(ext->pActFirework[1]);
				ext->pActFirework[1] = NULL;
			}
			ext->pActFirework[0] = GetAction(PRIO_CHR, 0);
			ext->pActFirework[0]->damage		= effectno;	// 借用 damage 来储存 type (effectno = type)
			ext->pActFirework[0]->anim_chr_no	= effectparam1;
			ext->pActFirework[0]->anim_no		= ANIM_STAND;
			ext->pActFirework[0]->anim_ang		= 1;
			ext->pActFirework[0]->dispPrio		= DISP_PRIO_RESERVE;
			if (effectno == 3)
			{
				ext->pActFirework[0]->gx		= ptAct->gx + 3;
				ext->pActFirework[0]->gy		= ptAct->gy - 3;
				ext->pActFirework[0]->bufGx[0]	= pc.ptAct->gx;
				ext->pActFirework[0]->bufGy[0]	= pc.ptAct->gy;
			}
			else
			{
				ext->pActFirework[0]->gx		= ptAct->gx;
				ext->pActFirework[0]->gy		= ptAct->gy;
			}
			ext->pActFirework[0]->actNo			= 0;
			break;
#endif
#ifdef ITEM_AURA_
		case 60:
			if (ext->ptActAura1 == nullptr )
			{
				ext->ptActAura1 = GetAction(PRIO_CHR, 0);
			}

			if (ext->ptActAura1 != nullptr)
			{
				if (effectno == 0)
				{
					DeathAction(ext->ptActAura1);
					ext->ptActAura1 = nullptr;
				}
				else
				{
					ext->ptActAura1->anim_chr_no = effectno;
					ext->ptActAura1->dispPrio = DISP_PRIO_CHAR - 1;
					ext->ptActAura1->anim_ang = 0;
					ext->ptActAura1->anim_no = 0;
				}
			}
			break;
		case 61:
			if (ext->ptActAura2 == nullptr)
			{
				ext->ptActAura2 = GetAction(PRIO_CHR, 0);
			}

			if (ext->ptActAura2 != nullptr)
			{
				if (effectno == 0)
				{
					DeathAction(ext->ptActAura2);
					ext->ptActAura2 = nullptr;
				}
				else
				{
					ext->ptActAura2->anim_chr_no = effectno;
					ext->ptActAura2->dispPrio = DISP_PRIO_CHAR;
					ext->ptActAura2->anim_ang = 0;
					ext->ptActAura2->anim_no = 0;
				}
			}
			break;
#endif


	}//ITEM_AURA_
}

// ID??
// ??： 0? ... ID???? / -1 ... ???
int searchCharObjId( int id )
{
	int i;
	int no = -1;
	for( i = searchCharObj; i < tailCharObj; i++ ){
		if( charObj[i].use != CHAROBJ_USE_FREE && charObj[i].id == id ){
			no = i;
			searchCharObj = i;
			break;
		}
	}
	if( no < 0 && searchCharObj > 0 ){
		for( i = 0; i < searchCharObj; i++ ){
			if( charObj[i].use != CHAROBJ_USE_FREE && charObj[i].id == id ){
				no = i;
				searchCharObj = i;
				break;
			}
		}
	}
	return no;
}

// ???
// ??：TRUE ... ???????
BOOL checkCharObjPoint( int gx, int gy, short type )
{
	int i;
	int no = -1;
	for( i = 0; i < tailCharObj; i++ ){
		if( charObj[i].use != CHAROBJ_USE_FREE && charObj[i].ptAct != NULL ){
			if( charObj[i].ptAct->gx == gx && charObj[i].ptAct->gy == gy && (charObj[i].type & type) != 0 )
				return TRUE;
		}
	}
	return FALSE;
}

// ?????????
// ??：1 ... ??????????????
//         0 ... ????
#ifdef _ANGEL_SUMMON
int checkCharObjPointStatus( int gx, int gy, short type, unsigned status )
#else
int checkCharObjPointStatus( int gx, int gy, short type, unsigned short status )
#endif
{
	int i;
	int no = -1;
	for( i = 0; i < tailCharObj; i++ ){
		if( charObj[i].use != CHAROBJ_USE_FREE && charObj[i].ptAct != NULL ){
			if( charObj[i].ptAct->gx == gx && charObj[i].ptAct->gy == gy && (charObj[i].type & type) != 0 ){
				if( (charObj[i].status & status ) != 0 )
					return 1;
			}
		}
	}
	return 0;
}

// ?????????
// ??：1 ... ????????????????
//         0 ... ????
#ifdef _ANGEL_SUMMON
int checkCharObjPointNotStatus( int gx, int gy, short type, unsigned status )
#else
int checkCharObjPointNotStatus( int gx, int gy, short type, unsigned short status )
#endif
{
	int i;
	int no = -1;
	for( i = 0; i < tailCharObj; i++ ){
		if( charObj[i].use != CHAROBJ_USE_FREE && charObj[i].ptAct != NULL ){
			if( charObj[i].ptAct->gx == gx && charObj[i].ptAct->gy == gy && (charObj[i].type & type) != 0 ){
				if( (charObj[i].status & status ) == 0 )
					return 1;
			}
		}
	}
	return 0;
}

void setNpcCharObj( int id, int graNo, int gx, int gy, int dir, char *fmname,
	                        char *name, char *freeName, int level, char *petname, int petlevel,
	                        int nameColor, int walk, int height, int charType, int profession_class
#ifdef _NPC_PICTURE					
							,int picture
#endif
#ifdef _NB_文字称号
							,char *newname,int newnamecolor
#endif
#ifdef _NB_任务指引
							,int noticeNo
#endif
					)
{
	int no;
	BOOL existFlag = FALSE;
	CHAREXTRA *ext;
#ifdef _ANGEL_SUMMON
	unsigned angelMode =0;
#endif

	int 原有光环 = 0;
	int 原有称号 = 0;


	//TODO 要修光环消失
	if( (no = searchCharObjId( id )) >= 0 ) {
#ifdef _ANGEL_SUMMON
		angelMode = charObj[no].status & CHR_STATUS_ANGEL;
#endif

		////JL fix 不删id的内存
		//existFlag = true;
		//goto __out1;
		if (charObj[no].ptAct != NULL)
		{
			原有光环 = charObj[no].ptAct->光环图档编号;
			原有称号 = charObj[no].ptAct->称号图档编号;
		}

		delCharObj( id);
	}
__out1:;

	if( !existFlag ){
		no = getCharObjBuf();
		if( no < 0 )
			return;
		charObj[no].ptAct = createCharAction( graNo, gx, gy, dir );
		if( charObj[no].ptAct == NULL )
			return;

		charObj[no].use = CHAROBJ_USE_VIEW;
		ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
		ext->charObjTblId = no;
		charObj[no].type      = getAtrCharObjType( charType );
		charObj[no].id        = id;
		charObj[no].stockDir  = -1;
	}
	charObj[no].graNo     = graNo;
	charObj[no].nameColor = nameColor;
	charObj[no].level     = level;
	charObj[no].gx = gx;
	charObj[no].gy = gy;
	charObj[no].dir = dir;
	if( walk != 0 )
		charObj[no].status |= CHR_STATUS_W;
	if( height != 0 )
		charObj[no].status |= CHR_STATUS_H;
	if( strlen( name ) <= CHAR_NAME_LEN )
		strcpy( charObj[no].name, name );		
	// shan add code
    if( strlen( fmname ) <= CHAR_FMNAME_LEN )
	    strcpy( charObj[no].fmname, fmname );	
	if( charType != CHAR_TYPEPET ){
		if( strlen( freeName ) <= CHAR_FREENAME_LEN )
			strcpy( charObj[no].freeName, freeName );		
	}else{
		if( strlen( freeName ) <= PET_FREENAME_LEN )
			strcpy( charObj[no].freeName, freeName );        		    
	}
	// Robin 0730
    if( strlen( petname ) <= CHAR_FREENAME_LEN )
	    strcpy( charObj[no].petName, petname );	
	charObj[no].petLevel = petlevel;

	charObj[no].charType = getAtrCharType( charType );
	charObj[no].newFoundFlag = 1;

#ifdef _ANGEL_SUMMON
	charObj[no].status |= angelMode;
#endif

	if( charObj[no].ptAct == NULL )
		return;
//#define 战斗光环
//#define 人物光环   
	if (原有光环){
		setCharmManor(charObj[no].ptAct, 原有光环);
	}
	if (原有称号){
		setCharmTitle(charObj[no].ptAct, 原有称号);
	}


	charObj[no].ptAct->anim_chr_no = graNo;
	charObj[no].ptAct->level = level;
	charObj[no].ptAct->atr |= charObj[no].charType;
	if( strlen( name ) <= CHAR_NAME_LEN )
		strcpy( charObj[no].ptAct->name, name );
	// shan add
	if( charType == CHAR_TYPEPLAYER){
		if( strlen( fmname ) <= CHAR_FMNAME_LEN )
			strcpy( charObj[no].ptAct->fmname, fmname );
	}
	else{
		strcpy( charObj[no].ptAct->fmname, " " );
	}

	if( charType != CHAR_TYPEPET ){
		if( strlen( freeName ) <= CHAR_FREENAME_LEN )
			strcpy( charObj[no].ptAct->freeName, freeName );
	}else{
		if( strlen( freeName ) <= PET_FREENAME_LEN )
			strcpy( charObj[no].ptAct->freeName, freeName );
	}
	// Robin 0730
	if( strlen( petname ) <= CHAR_FREENAME_LEN )
		strcpy( charObj[no].ptAct->petName, petname );
	charObj[no].ptAct->petLevel = petlevel;

	charObj[no].ptAct->itemNameColor = nameColor;

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	charObj[no].ptAct->profession_class = profession_class;
#endif
#ifdef _NPC_PICTURE
	charObj[no].ptAct->picture = picture;
	charObj[no].ptAct->picturetemp = ((((picture)&0x00ff0000)>>16)&0x000000ff);//(picture&0x0000ffff);
	//charObj[no].ptAct->anim_no = ((((picture)&0x0000ff00)>>8 )&0x000000ff);
#endif
#ifdef _GM_IDENTIFY		// Rog ADD GM识别
    if( strlen( gm_name ) <= GM_NAME_LEN )
	    strcpy( charObj[no].ptAct->gm_name, gm_name );	
#endif
#ifdef _NB_文字称号
	if( strlen( newname ) <= CHAR_NAME_LEN )
	    strcpy( charObj[no].ptAct->nb_name, newname );

	charObj[no].ptAct->nb_namecolor = newnamecolor;
#endif
#ifdef _NB_任务指引
	charObj[no].ptAct->noticeNo = noticeNo;
#endif
}



BOOL setReturnPetObj( int id, int graNo, int gx, int gy, int dir, 
char *name, char *freeName, int level, int nameColor, int walk, int height, int charType )
{
	int no;
	// ???????????
	if( (no = searchCharObjId( id )) >= 0 )
		// ?????
		return FALSE;
	// ????????????
	no = getCharObjBuf();
	if( no < 0 )
		return FALSE;	// ????????????
	charObj[no].ptAct = NULL;
	charObj[no].use = CHAROBJ_USE_VIEW;
	charObj[no].type      = getAtrCharObjType( charType );
	charObj[no].id        = id;
	charObj[no].graNo     = graNo;
	charObj[no].nameColor = nameColor;
	charObj[no].level     = level;
	charObj[no].gx = gx;
	charObj[no].gy = gy;
	charObj[no].dir = dir;
	if( walk != 0 )
		charObj[no].status |= CHR_STATUS_W;
	if( height != 0 )
		charObj[no].status |= CHR_STATUS_H;
	if( strlen( name ) <= CHAR_NAME_LEN )
		strcpy( charObj[no].name, name );
	if( strlen( freeName ) <= PET_FREENAME_LEN )
		strcpy( charObj[no].freeName, freeName );
	charObj[no].charType = getAtrCharType( charType );
	return TRUE;
}

//
//   ?????
//
void setItemCharObj( int id, int graNo, int gx, int gy, int dir, int classNo, char *info )
{
	int no;
	BOOL existFlag = FALSE;
	CHAREXTRA *ext;
	// ???????????
	if( (no = searchCharObjId( id )) >= 0 )
		existFlag = TRUE;
	// ????????????
	if( !existFlag ){
		no = getCharObjBuf();
		if( no < 0 )
			return;	// ????????????
		charObj[no].ptAct = createCharAction( graNo, gx, gy, dir );
		if( charObj[no].ptAct == NULL )
			// ?????????????????????
			return;
		charObj[no].use = CHAROBJ_USE_VIEW;
		ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
		ext->charObjTblId = no;
		charObj[no].type      = CHAROBJ_TYPE_ITEM;
		charObj[no].id        = id;
		charObj[no].stockDir  = -1;
	}
	charObj[no].graNo   = graNo;
	charObj[no].classNo = classNo;
	charObj[no].gx = gx;
	charObj[no].gy = gy;
	charObj[no].dir = dir;
	if( strlen( info ) <= 60 )
		strcpy( charObj[no].info, info );
	charObj[no].charType = ACT_ATR_TYPE_ITEM;
	charObj[no].newFoundFlag = 0;	// ?????????
	// ??????????????
	if( charObj[no].ptAct == NULL )
		return;
	charObj[no].ptAct->anim_chr_no = graNo;
	// 1??????????name???
	if( strlen( info ) <= ITEM_NAME_LEN )
		strcpy( charObj[no].ptAct->name, info );
	else
		strcpy( charObj[no].ptAct->name, "???" );
	charObj[no].ptAct->atr |= ACT_ATR_TYPE_ITEM;
	charObj[no].ptAct->itemNameColor = classNo;
}

//
//   ???
//
void setMoneyCharObj( int id, int graNo, int gx, int gy, int dir, int money, char *info )
{
	int no;
	BOOL existFlag = FALSE;
	CHAREXTRA *ext;
	// ???????????
	if( (no = searchCharObjId( id )) >= 0 )
		existFlag = TRUE;
	// ????????????
	if( !existFlag ){
		no = getCharObjBuf();
		if( no < 0 )
			return;	// ????????????
		charObj[no].ptAct = createCharAction( graNo, gx, gy, dir );
		if( charObj[no].ptAct == NULL )
			// ?????????????????????
			return;
		charObj[no].use = CHAROBJ_USE_VIEW;
		ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
		ext->charObjTblId = no;
		charObj[no].type      = CHAROBJ_TYPE_MONEY;
		charObj[no].id        = id;
		charObj[no].stockDir  = -1;
	}
	charObj[no].graNo = graNo;
	charObj[no].money = money;
	charObj[no].gx = gx;
	charObj[no].gy = gy;
	charObj[no].dir = dir;
	if( strlen( info ) <= 60 )
		strcpy( charObj[no].info, info );
	charObj[no].charType = ACT_ATR_TYPE_GOLD;
	charObj[no].newFoundFlag = 0;	// ?????????
	// ??????????????
	if( charObj[no].ptAct == NULL )
		return;
	charObj[no].ptAct->anim_chr_no = graNo;
	// 1??????????name???
	if( strlen( info ) <= CHAR_NAME_LEN )
		strcpy( charObj[no].ptAct->name, info );
	else
		strcpy( charObj[no].ptAct->name, "???" );
	charObj[no].ptAct->atr |= ACT_ATR_TYPE_GOLD;
}

// ??????????
//  ??：charObj????
//          -1 ... ???????
int getCharObjBuf( void )
{
	int i;
	int ret;
	// ????????????
	if( maxCharObj >= MAX_CHAROBJ )
		return -1;
	ret = freeCharObj;
	// ??????
	maxCharObj++;
	// ?????
	if( freeCharObj+1 > tailCharObj )
		tailCharObj = freeCharObj+1;
	// ???????????
	for( i = freeCharObj+1; i < MAX_CHAROBJ; i++ ){
		if( charObj[i].use == CHAROBJ_USE_FREE ){
			freeCharObj = i;
			break;
		}
	}
	if( freeCharObj > 0 && i >= MAX_CHAROBJ ){
		for( i = 0; i < freeCharObj; i++ ){
			if( charObj[i].use == CHAROBJ_USE_FREE ){
				freeCharObj = i;
				break;
			}
		}
	}
	return ret;
}

// ?????????????????
//  ?  ?：type ... ?????????????
//  ??：??????atr???
int getAtrCharType( int type )
{
	switch( type ){
		// ???
		case CHAR_TYPEPLAYER:
		case CHAR_TYPEBUS:
			return ACT_ATR_TYPE_OTHER_PC;
		// ???
		case CHAR_TYPEPET:
			return ACT_ATR_TYPE_PET;
		// ??
		default:
			return ACT_ATR_TYPE_OTHER;
	}
	return 0;
}

// ??????????????????
//  ?  ?：type ... ?????????????
//  ??：charObj?type???
int getAtrCharObjType( int type )
{
	int ret;
	switch( type ){
		case CHAR_TYPEPLAYER:
			ret = CHAROBJ_TYPE_USER_NPC;
			break;
		case CHAR_TYPEDENGON:
		case CHAR_TYPEDUELRANKING:
		case CHAR_TYPEMSG:
			ret = CHAROBJ_TYPE_NPC | CHAROBJ_TYPE_LOOKAT;
			break;
		case CHAR_TYPEBUS:
			ret = CHAROBJ_TYPE_NPC | CHAROBJ_TYPE_PARTY_OK;
			break;
		default:
			ret = CHAROBJ_TYPE_NPC;
			break;
	}
	return ret;
}

// ID??????????????
// ?????NULL
ACTION *getCharObjAct( int id )
{
	int no;
	if( (no = searchCharObjId( id )) < 0 )
		return NULL;
	if( charObj[no].ptAct == NULL )
		return NULL;
	return charObj[no].ptAct;
}


// ????????????
void delCharObj( int id )
{
	int i;
	int no;
	CHAREXTRA *ext;

	if( pc.id == id ){
		resetPc();
		return;
	}
	no = searchCharObjId( id );
	// ID???????
	if( no >= 0 ){
		if( charObj[no].ptAct != NULL ){
			ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
			if ((int)ext == 0xdddddddd)
			{
				charObj[no].ptAct = NULL;
				return;
			}
			if( ext != NULL ){
				if( ext->ptActLeaderMark != NULL ){
					DeathAction( ext->ptActLeaderMark );
					ext->ptActLeaderMark = NULL;
				}
				if( ext->ptActMagicEffect != NULL ){
					DeathAction( ext->ptActMagicEffect );
					ext->ptActMagicEffect = NULL;
				}
#ifdef __EMOTION
				if( ext->ptActEmotion != NULL ){
					DeathAction( ext->ptActEmotion );
					ext->ptActEmotion = NULL;
				}
#endif
#ifdef _STREET_VENDOR
				if(ext->ptStreetVendor != NULL){
					DeathAction(ext->ptStreetVendor);
					ext->ptStreetVendor = NULL;
				}
#endif
#ifdef _MIND_ICON
				if( ext->ptMindIcon != NULL ){
					DeathAction( ext->ptMindIcon );
					ext->ptMindIcon = NULL;
				}
#endif
#ifdef 人物光环
				if (ext->ptmManorIcon != NULL) {
					DeathAction(ext->ptmManorIcon);
					ext->ptmManorIcon = NULL;
				}
#endif
#ifdef 人物称号
				if (ext->ptTitleIcon != NULL) {
					DeathAction(ext->ptTitleIcon);
					ext->ptTitleIcon = NULL;
				}
#endif
#ifdef _NB_任务指引
				if( ext->ptNoticeIcon != NULL ){
					DeathAction( ext->ptNoticeIcon );
					ext->ptNoticeIcon = NULL;
				}
#endif
#ifdef _ITEM_FIREWORK
				if (ext->pActFirework[0] != NULL)
					DeathAction(ext->pActFirework[0]);
				if (ext->pActFirework[1] != NULL)
					DeathAction(ext->pActFirework[1]);
				ext->pActFirework[0] = NULL;
				ext->pActFirework[1] = NULL;
#endif
#ifdef ITEM_AURA_
				if (ext->ptActAura1 != nullptr)
					DeathAction(ext->ptActAura1);
				if (ext->ptActAura2 != nullptr)
					DeathAction(ext->ptActAura2);
				ext->ptActAura1 = nullptr;
				ext->ptActAura2 = nullptr;
#endif
			}
			DeathAction( charObj[no].ptAct );
			charObj[no].ptAct = NULL;
		}
		charObj[no].use = CHAROBJ_USE_FREE;
		charObj[no].status = 0;
		charObj[no].stockDir = -1;
		// ???
		if( maxCharObj > 0 )
			maxCharObj--;
		// ????????????????????
		if( freeCharObj > no )
			freeCharObj = no;
		// ??????????
		for( i = tailCharObj; i > 0; i-- ){
			if( charObj[i-1].use != CHAROBJ_USE_FREE ){
				tailCharObj = i;
				break;
			}
		}
	}
}


// ????????????
void initCharObj( void )
{
	int i;

	maxCharObj = 0;
	tailCharObj = 0;
	freeCharObj = 0;
	for( i = 0; i < MAX_CHAROBJ; i++ )
	{
		charObj[i].use = CHAROBJ_USE_FREE;
		charObj[i].ptAct = NULL;
		charObj[i].status = 0;
		charObj[i].id = 0;
		charObj[i].stockDir = -1;
		charObj[i].name[0] = '\0';
	}

	searchCharObj = 0;
}


// ?????????????
void resetCharObj( void )
{
	int i;
	CHAREXTRA *ext;

	maxCharObj = 0;
	tailCharObj = 0;
	freeCharObj = 0;
	for( i = 0; i < MAX_CHAROBJ; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE )
		{
			if( charObj[i].ptAct != NULL )
			{
				ext = (CHAREXTRA *)charObj[i].ptAct->pYobi;
				if ((int)ext==0xdddddddd)
				{
					charObj[i].ptAct = NULL;
					return;
				}

				if( ext != NULL )
				{
					if( ext->ptActLeaderMark != NULL )
					{
						DeathAction( ext->ptActLeaderMark );
						ext->ptActLeaderMark = NULL;
					}
					if( ext->ptActMagicEffect != NULL )
					{
						DeathAction( ext->ptActMagicEffect );
						ext->ptActMagicEffect = NULL;
					}
#ifdef __EMOTION
					if( ext->ptActEmotion != NULL ){
						DeathAction( ext->ptActEmotion );
						ext->ptActEmotion = NULL;
					}
#endif
#ifdef _STREET_VENDOR
					if(ext->ptStreetVendor != NULL){
						DeathAction(ext->ptStreetVendor);
						ext->ptStreetVendor = NULL;
					}
#endif
#ifdef _MIND_ICON
    				if( ext->ptMindIcon != NULL ){
	    				DeathAction( ext->ptMindIcon );
		   			    ext->ptMindIcon = NULL;
					}
#endif
#ifdef 人物光环
					if (ext->ptmManorIcon != NULL) {
						DeathAction(ext->ptmManorIcon);
						ext->ptmManorIcon = NULL;
					}
#endif
#ifdef 人物称号
					if (ext->ptTitleIcon != NULL) {
						DeathAction(ext->ptTitleIcon);
						ext->ptTitleIcon = NULL;
					}
#endif
#ifdef _ITEM_FIREWORK
					if (ext->pActFirework[0] != NULL)
						DeathAction(ext->pActFirework[0]);
					if (ext->pActFirework[1] != NULL)
						DeathAction(ext->pActFirework[1]);
					ext->pActFirework[0] = NULL;
					ext->pActFirework[1] = NULL;

#endif
#ifdef ITEM_AURA_
					if (ext->ptActAura1 != nullptr)
						DeathAction(ext->ptActAura1);
					if (ext->ptActAura2 != nullptr)
						DeathAction(ext->ptActAura2);
					ext->ptActAura1 = nullptr;
					ext->ptActAura2 = nullptr;

#endif
				}
				DeathAction( charObj[i].ptAct );
				charObj[i].ptAct = NULL;
			}
			charObj[i].use = CHAROBJ_USE_FREE;
			charObj[i].status = 0;
			charObj[i].id = 0;
			charObj[i].stockDir = -1;
		}
	}

	searchCharObj = 0;
}


// ???????????????????????
void clearPtActCharObj( void )
{
	int i;

	for( i = 0; i < MAX_CHAROBJ; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE )
		{
			charObj[i].ptAct = NULL;
			charObj[i].stockDir = -1;
		}
	}
}


// ??????????????????????????
void restorePtActCharObjAll( void )
{
	int i;
	CHAREXTRA *ext;

	for( i = 0; i < MAX_CHAROBJ; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE
		 && charObj[i].ptAct == NULL )
		{
			charObj[i].ptAct =
				createCharAction( charObj[i].graNo, charObj[i].gx, charObj[i].gy, charObj[i].dir );
			if( charObj[i].ptAct == NULL )
			{
				continue;
			}

			charObj[i].use = CHAROBJ_USE_VIEW;

			ext = (CHAREXTRA *)charObj[i].ptAct->pYobi;
			ext->charObjTblId = i;

			// NPC????????
			// ?PC????????
			// ?
			if( charObj[i].type == CHAROBJ_TYPE_NPC
			 || charObj[i].type == CHAROBJ_TYPE_USER_NPC )
			{
				charObj[i].ptAct->level = charObj[i].level;
				strcpy( charObj[i].ptAct->name, charObj[i].name );
				charObj[i].ptAct->atr |= charObj[i].charType;
			}
			else
			// Item????????
			if( charObj[i].type == CHAROBJ_TYPE_ITEM )
			{
				// 1??????????name???
				if( strlen( charObj[i].info ) <= ITEM_NAME_LEN )
				{
					strcpy( charObj[i].ptAct->name, charObj[i].info );
				}
				else
				{
					strcpy( charObj[i].ptAct->name, "???" );
				}
				charObj[i].ptAct->atr |= charObj[i].charType;
			}
			else
			// ??????????
			if( charObj[i].type == CHAROBJ_TYPE_MONEY )
			{
				// 1??????????name???
				if( strlen( charObj[i].info ) <= CHAR_NAME_LEN )
				{
					strcpy( charObj[i].ptAct->name, charObj[i].info );
				}
				else
				{
					strcpy( charObj[i].ptAct->name, "???" );
				}
				charObj[i].ptAct->atr |= charObj[i].charType;
			}
		}
	}
}


// ?????????????????????????
void restorePtActCharObj( int id )
{
	int no;
	CHAREXTRA *ext;

	if( id < 0 )
		return;

	// ???????????
	if( (no = searchCharObjId( id )) < 0 )
	{
		// ??????
		return;
	}

	if( charObj[no].use != CHAROBJ_USE_FREE
	 && charObj[no].ptAct == NULL )
	{
		charObj[no].ptAct =
			createCharAction( charObj[no].graNo, charObj[no].gx, charObj[no].gy, charObj[no].dir );
		if( charObj[no].ptAct == NULL )
		{
			// ???????????????
			return;
		}

		charObj[no].use = CHAROBJ_USE_VIEW;

		ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
		ext->charObjTblId = no;

		charObj[no].ptAct->level = charObj[no].level;
		strcpy( charObj[no].ptAct->name, charObj[no].name );
		charObj[no].ptAct->atr |= charObj[no].charType;
	}
}


// ?????
void setMovePointCharObj( int id, int nextGx, int nextGy )
{
	int no;

	no = searchCharObjId( id );
	if( no >= 0 )
	{
		stockCharMovePoint( charObj[no].ptAct, nextGx, nextGy );
	}
}




///////////////////////////////////////////////////////////////////////////
// ???????????????????????????

typedef struct
{
	unsigned int bmpNo;
	int gx;
	int gy;
} ITEM_OVERLAP_CHECK;

#define MAX_ITEM_OVERLAP	100
ITEM_OVERLAP_CHECK itemOverlapTbl[MAX_ITEM_OVERLAP];
int itemOverlapCheckCnt;

void initItemOverlapCheck( void )
{
	itemOverlapCheckCnt = 0;
}


// ??： TRUE  ... ????
//          FALSE ... ??
BOOL itemOverlapCheck( unsigned int bmpNo, int gx, int gy )
{
	int i;
	BOOL flag = FALSE;
	for( i = 0; i < itemOverlapCheckCnt; i++ ){
		if( itemOverlapTbl[i].bmpNo == bmpNo
		 && itemOverlapTbl[i].gx == gx
		 && itemOverlapTbl[i].gy == gy ){
			flag = TRUE;
			break;
		}
	}
	if( !flag ){
		if( itemOverlapCheckCnt < MAX_ITEM_OVERLAP ){
			itemOverlapTbl[itemOverlapCheckCnt].bmpNo = bmpNo;
			itemOverlapTbl[itemOverlapCheckCnt].gx    = gx;
			itemOverlapTbl[itemOverlapCheckCnt].gy    = gy;
			itemOverlapCheckCnt++;
		}
	}
	return flag;
}






///////////////////////////////////////////////////////////////////////////
// ????????


// ??????????????
void limitCantClientDir( short *dir )
{
	// dir??????????????
	if( ((*dir) % 2) == 0 )
	{
		(*dir)++;
	}
}

// dir? 0 ?? 7 ???????
void ajustClientDir( short *dir )
{
	if( *dir < 0 )
	{
		do
		{
			(*dir) += 8;
		} while( *dir < 0 );
	}
	else
	if( *dir > 7 )
	{
		do
		{
			(*dir) -= 8;
		} while( *dir > 7 );
	}
}


void getPetRoute( ACTION *ptAct )
{
	short dir;
	int i;
	int dx, dy;
	int gx, gy;
	PETEXTRA *pe;

	if( ptAct->bufCount > 0 )
		return;

	// ????????????
	if( nowGx != oldGx || nowGy != oldGy )
		return;

	pe = (PETEXTRA *)ptAct->pYobi;

	dir = pe->moveDir;

	for( i = 0; i < 4; i++ )
	{
		getRouteData( dir, &dx, &dy );
		gx = ptAct->gx+dx;
		gy = ptAct->gy+dy;
		if( (gx != pe->preGx || gy != pe->preGy)
		 && checkHitMap( gx, gy ) == FALSE )
		{
			break;
		}
		dir += (2*(i+1));
		ajustClientDir( &dir );
	}

	if( i >= 4 )
	{
		pe->preGx = -1;
		pe->preGy = -1;
		pe->moveDir -= 2;
		ajustClientDir( &pe->moveDir );
		return;
	}

	if( pe->dirCnt >= 2 )
	{
		pe->dirCnt = 0;
		pe->moveDir -= 6;
		ajustClientDir( &pe->moveDir );
	}

	if( dir != pe->preDir )
	{
		pe->dirCnt++;
	}
	else
	{
		pe->dirCnt = 0;
	}

	pe->preDir = dir;

	ptAct->bufGx[ptAct->bufCount] = gx;
	ptAct->bufGy[ptAct->bufCount] = gy;
	ptAct->bufCount++;

	pe->preGx = ptAct->gx;
	pe->preGy = ptAct->gy;

	return;
}


// ???
BOOL petMoveProc( ACTION *ptAct )
{
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;

	// ??????????
	// ???8??????
	if( ABS( ptAct->gx - nowGx ) >= 13
	 || ABS( ptAct->gy - nowGy ) >= 13
	 || (pe->createTime+8000 < TimeGetTime() && ptAct->vx == 0 && ptAct->vy == 0) )
	{
		restorePtActCharObj( pe->id );
		DeathAction( ptAct );
		ptAct = NULL;
		return FALSE;
	}

	if( pe->ptAct == NULL && pe->createTime+7200 < TimeGetTime() )
	{
		pe->ptAct = createCommmonEffectNoLoop( SPR_difence, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}
	if( pe->ptAct2 == NULL && pe->createTime+7500 < TimeGetTime() )
	{
		pe->ptAct2 = createCommmonEffectNoLoop( SPR_difence, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}


	if( ptAct->vx == 0 && ptAct->vy == 0 )
	{
		// ????????????
		getPetRoute( ptAct );
	}
	charMove( ptAct );

	if( pe->ptAct != NULL )
	{
		pe->ptAct->mx = ptAct->mx;
		pe->ptAct->my = ptAct->my;
	}
	if( pe->ptAct2 != NULL )
	{
		pe->ptAct2->mx = ptAct->mx;
		pe->ptAct2->my = ptAct->my;
	}

	return TRUE;
}


// ????
BOOL uprisePetProc( ACTION *ptAct )
{
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;

	if( pe->ptAct == NULL && pe->createTime < TimeGetTime() )
	{
		pe->ptAct = createCommmonEffectNoLoop( SPR_difence, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}
	if( pe->ptAct2 == NULL && pe->createTime+500 < TimeGetTime() )
	{
		pe->ptAct2 = createCommmonEffectNoLoop( SPR_difence, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}

	// ???????????
	if( pe->createTime+1200 < TimeGetTime() )
	{
		restorePtActCharObj( pe->id );
		DeathAction( ptAct );
		ptAct = NULL;
	}

	return FALSE;
}


// ??????????
BOOL petCircleOutProc( ACTION *ptAct )
{
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;
	float mx, my;
	float dx, dy;
	float angle;
	int i;

	if( pe->ptAct == NULL )
	{
		pe->ptAct = (ACTION *)1;
		pe->angle = 0.0F;
		pe->r = 0;
		createCommmonEffectNoLoop( SPR_hoshi, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}

	if( pe->r > 800 )
	{
		restorePtActCharObj( pe->id );
		DeathAction( ptAct );
		ptAct = NULL;
		return FALSE;
	}

	if( pe->ptAct != NULL && pe->createTime+1000 < TimeGetTime() )
	{
		pe->r += 4;
		pe->angle += 6.0F;
	}

	angle = pe->angle;
	for( i = 0; i < 3; i++ )
	{
		dx = (float)pe->r * CosT( angle );
		dy = (float)pe->r * SinT( angle );

		// ??
		camMapToGamen( ptAct->mx+dx, ptAct->my+dy, &mx, &my );
		ptAct->x = (int)(mx+.5);
		ptAct->y = (int)(my+.5);

		// ?????????
		pattern( ptAct, ANM_NOMAL_SPD, ANM_LOOP );

		// ??????????????
		setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx+dx, ptAct->my+dy );
		angle += 120;
		AdjustDir( &angle );
	}

	return FALSE;
}


// ?????????
BOOL petCircleInProc( ACTION *ptAct )
{
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;
	float mx, my;
	float dx, dy;
	float angle;
	int i;

	if( pe->ptAct == NULL )
	{
		pe->ptAct  = (ACTION *)1;
		pe->ptAct2 = (ACTION *)NULL;
		pe->angle = 0.0F;
		pe->r = 800;
	}
	else
	{
		if( pe->r <= 0 )
		{
			restorePtActCharObj( pe->id );
			DeathAction( ptAct );
			ptAct = NULL;
			return TRUE;
		}
	}

	if( pe->ptAct != NULL )
	{
		if( pe->ptAct2 == NULL && pe->createTime+2300 < TimeGetTime() )
		{
			pe->ptAct2 = createCommmonEffectNoLoop( SPR_hoshi, ptAct->gx, ptAct->gy,
				0, 0, ptAct->dispPrio );
		}

		pe->r -= 4;
		pe->angle += 6.0F;

		angle = pe->angle;
		for( i = 0; i < 3; i++ )
		{
			dx = (float)pe->r * CosT( angle );
			dy = (float)pe->r * SinT( angle );

			// ??
			camMapToGamen( ptAct->mx+dx, ptAct->my+dy, &mx, &my );
			ptAct->x = (int)(mx+.5);
			ptAct->y = (int)(my+.5);

			// ?????????
			pattern( ptAct, ANM_NOMAL_SPD, ANM_LOOP );

			// ??????????????
			setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx+dx, ptAct->my+dy );
			angle += 120;
			AdjustDir( &angle );
		}
	}

	return FALSE;
}


// ??????????????
void petProc( ACTION *ptAct )
{
	float mx, my;
	int animLoop;
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;

	switch( pe->mode )
	{
		// ?????：?????
		case 0:
			if( !petMoveProc( ptAct ) )
			{
				return;
			}
			break;

		// ?????：
		// ???????????：?
		case 1:
			if( !uprisePetProc( ptAct ) )
			{
				return;
			}
			break;

		// ?????：?????
		case 2:
			if( !petCircleOutProc( ptAct ) )
			{
				return;
			}
			break;

		// ????：??????
		case 3:
			if( !petCircleInProc( ptAct ) )
			{
				return;
			}
			break;
	}

	// ??
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)(mx+.5);
	ptAct->y = (int)(my+.5);

	// ?????????
	if( ptAct->anim_no == ANIM_HAND
	 || ptAct->anim_no == ANIM_HAPPY
	 || ptAct->anim_no == ANIM_ANGRY
	 || ptAct->anim_no == ANIM_SAD
	 || ptAct->anim_no == ANIM_WALK
	 || ptAct->anim_no == ANIM_STAND
	 || ptAct->anim_no == ANIM_NOD )
	{
		animLoop = ANM_LOOP;
	}
	else
	{
		animLoop = ANM_NO_LOOP;
	}
	pattern( ptAct, ANM_NOMAL_SPD, animLoop );

	// ??????????????
	setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my );
}



// ??????????
ACTION *createPetAction( int graNo, int gx, int gy, int dir, int mode, int moveDir, int id )
{
	ACTION *ptAct;
	float mx, my;
	PETEXTRA *pe;

	/* ?????????? */
	ptAct = GetAction( PRIO_CHR, sizeof( PETEXTRA ) );
	if( ptAct == NULL )
		return NULL;

	// ???
	ptAct->func = petProc;
	// ????????
	ptAct->anim_chr_no = graNo;
	// ??
	ptAct->anim_no = ANIM_STAND;
	// ?????????( ??? )( ??????? )
	ptAct->anim_ang = dir;
	// ?
	ptAct->dispPrio = DISP_PRIO_CHAR;
	// 1????????
	ptAct->atr = ACT_ATR_INFO |	ACT_ATR_HIT | ACT_ATR_HIDE2;
	// ???
	ptAct->nextGx = gx;					// ???????????
	ptAct->nextGy = gy;
	ptAct->bufCount = 0;
	ptAct->gx = gx;						// ???????????佋?
	ptAct->gy = gy;
	ptAct->mx = (float)gx * GRID_SIZE;	// ????
	ptAct->my = (float)gy * GRID_SIZE;
	ptAct->vx = 0;						// ??
	ptAct->vy = 0;

	// ??
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)(mx+.5);
	ptAct->y = (int)(my+.5);

	pe = (PETEXTRA *)ptAct->pYobi;
	pe->mode = mode;
	pe->moveDir = moveDir;
	limitCantClientDir( &pe->moveDir );
	pe->preDir = pe->moveDir;
	pe->dirCnt = 0;
	pe->preGx = gx;
	pe->preGy = gy;
	pe->walkCnt = 0;
	pe->createTime = TimeGetTime();
	pe->ptAct  = NULL;
	pe->ptAct2 = NULL;
	pe->angle = 0.0F;
	pe->r = 0;

	pe->id = id;

	return ptAct;
}




#ifdef 人物光环
void setCharmManor(ACTION *ptAct, int MindNo)
{
	//无光环图像时不显示
	if (MindNo >= SPRSTART)
	{
		if (SpriteData[MindNo - SPRSTART].animSize == 0) {
			return;
		}
	}

	CHAREXTRA *ext;
	if (ptAct == NULL)
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	ptAct->光环图档编号 = MindNo;
	

	if (ext->ptmManorIcon) {
		DeathAction(ext->ptmManorIcon);
		ext->ptmManorIcon = NULL;
	}
	ext->ptmManorIcon = createCommmonEffectAction(MindNo, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR);
}
void delCharmManor(ACTION *ptAct)
{
	CHAREXTRA *ext;
	if (ptAct == NULL)
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	if (ext->ptmManorIcon) {
		DeathAction(ext->ptmManorIcon);
		ext->ptmManorIcon = NULL;
	}
}
#endif
#ifdef 人物称号
void setCharmTitle(ACTION *ptAct, int MindNo)
{
	//无称号图像时不显示
	//if (SpriteData[MindNo - SPRSTART].animSize == 0) {
		//return;
	//}
	CHAREXTRA *ext;
	if (ptAct == NULL)
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	ptAct->称号图档编号 = MindNo;
	if (ext->ptTitleIcon) {
		DeathAction(ext->ptTitleIcon);
		ext->ptTitleIcon = NULL;
	}
	ext->ptTitleIcon = createCommmonEffectAction(MindNo, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR);
}

void delCharmTitle(ACTION *ptAct)
{
	CHAREXTRA *ext;
	if (ptAct == NULL)
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	if (ext->ptTitleIcon) {
		DeathAction(ext->ptTitleIcon);
		ext->ptTitleIcon = NULL;
	}
}
#endif

int getCharType(ACTION *ptAct)
{
	extern int 人物屏蔽开关;
	if (ProcNo != PROC_GAME) return 0;
	if (!人物屏蔽开关) return 0;
	CHAREXTRA *ext;
	int no;
	if (ptAct == NULL)
		return 0;
	ext = (CHAREXTRA *)ptAct->pYobi;
	if (ext) {
		no = ext->charObjTblId;
		if (no >= 1500) return 0;
		if (strcmp(ptAct->name, charObj[no].name) == 0)
			if (charObj[no].charType == 256) {
				return 1;
			}
	}
	return 0;
}

#ifdef _NB_任务指引
void setNpcNotice( ACTION *ptAct, int MindNo){
	//无指引图像时不显示
	if (SpriteData[MindNo - SPRSTART].animSize == 0) {
		return;
	}
	CHAREXTRA *ext;
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	ptAct->noticeNo = MindNo;
	if(ext->ptNoticeIcon){
		DeathAction( ext->ptNoticeIcon);
		ext->ptNoticeIcon = NULL;
	}
	ext->ptNoticeIcon = createCommmonEffectAction( MindNo, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR);
}

void delNpcNotice( ACTION *ptAct){
	CHAREXTRA *ext;	
	if( ptAct == NULL )
		return;
	ext = (CHAREXTRA *)ptAct->pYobi;
	if(ext->ptNoticeIcon){
		DeathAction( ext->ptNoticeIcon);
		ext->ptNoticeIcon = NULL;
	}
}
#endif

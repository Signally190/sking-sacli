/************************/
/*	ohta.c				*/
/************************/
#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "ohta.h"
#include "../systeminc/loadsprbin.h"
#include "../systeminc/t_music.h"

ACTION *MakeJiki( void );
void Jiki( ACTION *pAct );

ACTION *MakeJikiTama( int x, int y, int dx, int dy, int bmpNo );
void JikiTama( ACTION *pAct );

ACTION *MakeEnemy( void );
void Enemy( ACTION *pAct );

ACTION *MakeEnemyTama( ACTION *pOya );
void EnemyTama( ACTION *pAct );

ACTION *MakeBg( void );
void Bg( ACTION *pAct );

ACTION *MakeKen( void );
void Ken( ACTION *pAct );

ACTION *MakeGameOver( void );
void GameOver( ACTION *pAct );

/* 爆発データ */
int bomData[] = { 21, 22, 23, 23, 22, 21 };

/* 自機のポインタ */
ACTION *pJiki;


/* 攻撃の種類 */
enum{
	TAMA_NORMAL,	/* ノーマル */
	TAMA_TWIN,		/* ツイン */
	TAMA_3WAY,		/* ３方向 */
	TAMA_DIR,		/* 全方向 */
	TAMA_END		/* 終わり */
};
	
/********************************************/
/*	自機									*/
/********************************************/

/* 自機予備構造体 */
typedef struct{
	int cnt,cnt2,cnt3;	/* 汎用カウンタ */
	int tamaDir;		/* 弾の方向 */
	int tamaDx;			/* 弾のＸ増分 */
	int attackKind;		/* 攻撃の種類 */
	STR_BUFFER strTestBuffer;	/* テスト文字列 */
}JIKI;

static int No = 36000;

/* 自機作成 *******************************************************************/
ACTION *MakeJiki( void )
{
	ACTION *pAct;
	JIKI *pYobi;
	
	/* アクションリストに登録 */
	pAct = GetAction( PRIO_JIKI, sizeof( JIKI ) );
	if( pAct == NULL ) return NULL;
	
	/* 予備構造体のアドレス */
	pYobi = ( JIKI *)pAct->pYobi;
	
	/* 実行関数 */
	pAct->func = Jiki;
	/* 表示優先度 */
	pAct->dispPrio = DISP_PRIO_JIKI;
	// 当たり判定 + ボックス表示
	pAct->atr |= ACT_ATR_HIT_BOX;
	/* スプライト番号 */
	//pAct->bmpNo = 31;
	//pAct->anim_chr_no = 20001;
	// anim_tbl.h の番号
	pAct->anim_chr_no = 31555; // イノシシ
	//pAct->anim_chr_no = 5200; // マウスカーソル
	//pAct->anim_chr_no = 9350; // SCORE
	// 動作番号
	//pAct->anim_no = ANIM_ATTACK;
	pAct->anim_no = ANIM_STAND;
	pAct->anim_no = ANIM_STAND + 1;
	// アニメーション向き( ０～７ )( 下が０で右回り )
	pAct->anim_ang = 0;
	/* ＨＰ */
	pAct->hp = 100;
	/* 初期位置 */
	pAct->x = 304;
	pAct->y = 400;

	/* 敵作成 */
	MakeEnemy();
	/* ＢＧ作成 */
	MakeBg();
	
	/* ケン作成 */
	MakeKen();
	
	/* 攻撃の種類 */
	//pYobi->attackKind = TAMA_DIR;
	pYobi->attackKind = TAMA_3WAY;
	/* 弾方向 */
	pYobi->tamaDir = 1;
	pYobi->tamaDx = 1;
	/* グローバルに保存 */
	pJiki = pAct;
	
	// 文字列の長さ
	pYobi->strTestBuffer.len = 10;
	// 文字列の色
	pYobi->strTestBuffer.color = 1;
	pYobi->strTestBuffer.x = 320;
	pYobi->strTestBuffer.y = 260;
	
	return pAct;
	
}

/* 自機処理 *******************************************************************/
void Jiki( ACTION *pAct )
{
	JIKI *pYobi = ( JIKI *)pAct->pYobi;
	//int i;
	
	/* 攻撃受けた時 */
	if( pAct->damage > 0 ){
		pAct->hp -= pAct->damage;
		if( pAct->hp <= 0 ){ 
			pAct->state = ACT_STATE_DEAD;
			pYobi->cnt  = 0;
			pYobi->cnt2 = 0;
			pYobi->cnt3 = 0;
		}
		pAct->damage = 0;
	}
	// 取出す座標
//	rcX = Rnd( 0,608 );
//	rcY = Rnd( 0,400 );
//	SetRect( &Rc[ pAct->bmpNo ], rcX, rcY, rcX + 32, rcY + 32 ); 
	
	// マウス移動
	//pAct->x = mouse.nowPoint.x;
	//pAct->y = mouse.nowPoint.y;

	// マウスで弾発射
	if( pYobi->cnt3 <= 0 ){
		// 弾発射
		//if( mouse.state == MOUSE_LEFT_CRICK ){
		if( mouse.onceState & MOUSE_LEFT_CRICK ){
			// 入力フォーカス取得
			//GetKeyInputFocus( &pYobi->strTestBuffer );
			/* 弾作成 */
			//MakeJikiTama( pAct->x , pAct->y -16, 0, -8, Rnd( 0, BMP_FILE - 1 ) );
			//MakeJikiTama( pAct->x , pAct->y -16, 0, -8, 1034 );
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, 25 );
			
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 0, BMP_FILE - 2 ) );
			
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 31554, 31557 ) ); // ＴＧＳ
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 31000, 31069 ) ); // モンスター
//			MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 30100, 30135 ) ); // ＰＣ
			//MakeJikiTama( pAct->x + 10, pAct->y -16, 0, -8, Rnd( 0, BMP_FILE - 1 ) );
			//MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 29 );
			//MakeJikiTama( pAct->x - 10, pAct->y -16, 0, -8, 29 );
			//MakeJikiTama( pAct->x + 10, pAct->y -16, 0, -8, 29 );
			pYobi->cnt3 = 5;
#if 0
			for( i = 0; i < (int)256; i++ ){
				// 明るくする
				Palette[i].peRed 	 *= 0.95;
				Palette[i].peGreen *= 0.95;
				Palette[i].peBlue  *= 0.95;
			}
			// パレットの中身を設定
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
#endif
		}
		// 真中ボタン
		if( GetAsyncKeyState( VK_MBUTTON ) ){
			//MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 29 );
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 0, BMP_FILE - 1 ) );
	//		MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, 32 );
			pYobi->cnt3 = 5;
			
		}
		// 弾発射
		//if( mouse.onceState & MOUSE_RIGHT_CRICK ){
		if( mouse.state & MOUSE_RIGHT_CRICK ){
			// 入力フォーカス取得
			//GetKeyInputFocus( &MyChatBuffer );
		//if( GetAsyncKeyState( VK_RBUTTON ) 	& 0x8000 ){
		//	MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 29 );
		// サーフェスがあったら
		//if( lpSurface[ 0 ] != NULL ){
		//	// リリース
		//	lpSurface[ 0 ]->Release( );
		//	lpSurface[ 0 ] = NULL;
		//}
		//	MakeBg();
#if 0		
			MakeJikiTama( pAct->x - 10, pAct->y -16, -6, -8, Rnd( 0, BMP_FILE - 2 ) );
			MakeJikiTama( pAct->x -  5, pAct->y -16, -3, -8, Rnd( 0, BMP_FILE - 2 ) );
			MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 0, BMP_FILE - 2 ) );
			MakeJikiTama( pAct->x +  5, pAct->y -16,  3, -8, Rnd( 0, BMP_FILE - 2 ) );
			MakeJikiTama( pAct->x + 10, pAct->y -16,  6, -8, Rnd( 0, BMP_FILE - 2 ) );
#else
	//		MakeJikiTama( pAct->x - 10, pAct->y -16, -6, -8, No );
	//		MakeJikiTama( pAct->x -  5, pAct->y -16, -3, -8, No );
	//		MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, No );
	//		MakeJikiTama( pAct->x +  5, pAct->y -16,  3, -8, No );
	//		MakeJikiTama( pAct->x + 10, pAct->y -16,  6, -8, No );
#endif		
			pYobi->cnt3 = 5;
#if 0
			for( i = 0; i < (int)256; i++ ){
				// 暗くする
				Palette[i].peRed  *= 1.05;
				Palette[i].peGreen*= 1.05;
				Palette[i].peBlue *= 1.05;
			}
			//パレットの中身を設定
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
#endif
		}
		
	}else pYobi->cnt3--;

	/* 状態で分岐 */
	switch( pAct->state ){
		/* 通常時 */
		case ACT_STATE_ACT:
		
			/* 敵弾との当たり判定 */
			//Hit( pAct, PRIO_ENEMY_TAMA );
			/* 敵との当たり判定 */
			//Hit( pAct, PRIO_ENEMY );

			/* 移動 */
			if( joy_con[ 0 ] & JOY_UP ){	/* 上 */
				pAct->y -= 2;
				if( pAct->y < 0 ) pAct->y = 0;
			}
			if( joy_con[ 0 ] & JOY_DOWN ){	/* 下 */
				pAct->y += 2;
				if( pAct->y > 448 ) pAct->y = 448;
			}
			if( joy_con[ 0 ] & JOY_LEFT ){	/* 左 */
				pAct->x -= 2;
				if( pAct->x < 0 ) pAct->x = 0;
			}
			if( joy_con[ 0 ] & JOY_RIGHT ){	/* 右 */
				pAct->x += 2;
				if( pAct->x > 608 ) pAct->x = 608;
			}
			
			// 敵作成
			//if( joy_con[ 0 ] & JOY_B )	MakeEnemy();
			
			/* 弾発射 */
			if( pYobi->cnt3 <= 0 ){ /* 弾ウェイト */
				if( joy_con[ 0 ] & JOY_A ){
					
					/*DeathAllAction();*/
					/* 弾の種類で分岐 */
					switch( pYobi->attackKind ){
						
						case TAMA_NORMAL:
						
							/* 弾作成 */
							MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 28 );
							
							break;
							
						case TAMA_TWIN:
						
							/* 弾作成 */
							MakeJikiTama( pAct->x - 10, pAct->y -16, 0, -8, 29 );
							MakeJikiTama( pAct->x + 10, pAct->y -16, 0, -8, 29 );
							
							break;
							
						case TAMA_3WAY:
						
							MakeJikiTama( pAct->x - 10, pAct->y -16, -4, -8, 29 );
							MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, 29 );
							MakeJikiTama( pAct->x + 10, pAct->y -16,  4, -8, 29 );
							
							break;
							
						case TAMA_DIR:
						
							MakeJikiTama( pAct->x, pAct->y -16,  pYobi->tamaDx, -8, 29 );
							MakeJikiTama( pAct->x, pAct->y -16, -pYobi->tamaDx, -8, 29 );
							/* 弾方向 */
							pYobi->tamaDx += pYobi->tamaDir;
							if(	pYobi->tamaDx <= -10 || 10 <= pYobi->tamaDx ){
								pYobi->tamaDir *= -1;
							}
							break;
					}
					pYobi->cnt3 = 2; /*  弾ウェイト */
					//pYobi->cnt3 = 0; /*  弾ウェイト */
				}
			}else{
				if( pYobi->cnt3 > 0 ) pYobi->cnt3--;
			}
			
			/* 点滅 */
/*			if( ( int )pAct->x % 2 ) pAct->atr |= ACT_ATR_HIDE;
			else pAct->atr &= ~ACT_ATR_HIDE;
*/		
			/* 敵作成 */
			pYobi->cnt++;
			if( pYobi->cnt >= 1 ){
				//MakeEnemy();
				pYobi->cnt2++;
				if( pYobi->cnt2 >= 5 ){
					pYobi->cnt2 = 0;
					/* 弾変更 */
					//pYobi->attackKind++;
					if( pYobi->attackKind >= TAMA_END ){
						pYobi->attackKind = 0;
					}
				}
				pYobi->cnt = 0;
			}
			
			break;
	
		/* 死亡時 */	
		case ACT_STATE_DEAD:
		
			if( pYobi->cnt == 0 ){ 
				pAct->bmpNo = bomData[ 0 ];
				pYobi->cnt = 1;
			}
			/* 爆発アニメーション */
			pYobi->cnt2++;
			if( pYobi->cnt2 >= 5 ){
				pAct->bmpNo = bomData[ pYobi->cnt ];
				pYobi->cnt++;
				pYobi->cnt2 = 0;
				/* 終わったら死ぬ */
				if( pYobi->cnt == 6 ){
					DeathAction( pAct );
					MakeGameOver( );
				}
			}
			
			break;
	}
	
	// 表示データをバッファに溜める
	//StockDispBuffer( pAct->x + 32, pAct->y, pAct->dispPrio, pAct->anim_chr_no, 0 );
	//StockDispBuffer( pAct->x - 32, pAct->y, pAct->dispPrio, pAct->anim_chr_no, 0 );
	{
		char szMoji[ 256 ];
		wsprintf( szMoji,"BmpNo = %d",No );
	//	StockFontBuffer( pAct->x, pAct->y, FONT_PRIO_FRONT, 2, szMoji, 0 );
	}
	
	// アニメーション
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
	// テスト文字列表示
	//StockFontBuffer( 320, 200, FONT_PRIO_FRONT, pYobi->strTestBuffer.color, pYobi->strTestBuffer.buffer, 0 );
	StockFontBuffer2( &pYobi->strTestBuffer );
}

/********************************************/
/*	自機弾									*/
/********************************************/

/* 自機弾作成 *********************************************************************/
ACTION *MakeJikiTama( int x, int y, int dx, int dy, int bmpNo )
{
	ACTION *pAct;
	
	/* アクションリストに登録 */
	pAct = GetAction( PRIO_JIKI_TAMA, 0 );
	if( pAct == NULL ) return NULL;
	
	/* 実行関数 */
	pAct->func = JikiTama;
	// anim_tbl.h の番号
	pAct->anim_chr_no = bmpNo; // イノシシ
	// 動作番号
	//pAct->anim_no = ANIM_ATTACK;
	pAct->anim_no = Rnd( ANIM_ATTACK, ANIM_LIST_);
	//pAct->anim_no = ANIM_WALK;
	// アニメーション向き( ０～７ )( 下が０で右回り )
	pAct->anim_ang = Rnd( 0, 7 );
	///* スプライト番号 */
	//pAct->bmpNo = bmpNo;
	/* 表示優先度 */
	pAct->dispPrio = 2;
	/* ＨＰ */
	pAct->hp = 10000;
	/* 初期位置 */
	pAct->x = x - dx;
	pAct->y = y - dy;
	/* 増分 */
	pAct->dx = dx / 2;
	pAct->dy = dy / 2;
	
	return pAct;
}

/* 自機弾処理 *********************************************************************/
void JikiTama( ACTION *pAct )
{

	/* 攻撃受けた時 */
	if( pAct->damage > 0 ){
		pAct->hp -= pAct->damage;
		if( pAct->hp <= 0 ){ 
			pAct->state = ACT_STATE_DEAD;
		}
		pAct->damage = 0;
	}

	/* 状態で分岐 */
	switch( pAct->state ){
		/* 通常時 */
		case ACT_STATE_ACT:

			/* 敵との当たり判定 */
			//Hit( pAct, PRIO_ENEMY );

			/* 移動 */
			pAct->x += pAct->dx;
			pAct->y += pAct->dy;
	
			/* リミット */
			if(	pAct->y <= -32 ){ 
				pAct->y = -32;
				MakeEnemyTama( pAct );
				DeathAction( pAct );
			}else
			if( 480 <= pAct->y ){ 
				pAct->y = 480;
				DeathAction( pAct );
			}
			
			if(	pAct->x <= -32 ){
				pAct->x = -32;
				DeathAction( pAct );
			}else
			if( 640 <= pAct->x ){ 
				pAct->x = 640;
				DeathAction( pAct );
			}

			break;
		
		/* 死亡時 */	
		case ACT_STATE_DEAD:
		
			DeathAction( pAct );
			
			break;
	}
	
	
	// アニメーション
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
}


/********************************************/
/*	敵										*/
/********************************************/

/* 敵予備構造体 */
typedef struct{
	int cnt,cnt2,cnt3,animCnt;	/* 汎用カウンタ */
}ENEMY;

/* 敵作成 *********************************************************************/
ACTION *MakeEnemy( void )
{
	ACTION *pAct;
	ENEMY *pYobi;
	short bmpTbl[] = { 1, 3, 5, 7, 9, 29 };
	
	/* アクションリストに登録 */
	pAct = GetAction( PRIO_ENEMY, sizeof( ENEMY ) );
	if( pAct == NULL ) return NULL;
	
	/* 予備構造体のアドレス */
	pYobi = ( ENEMY *)pAct->pYobi;
	
	/* 実行関数 */
	pAct->func = Enemy;
	/* スプライト番号 */
	pAct->bmpNo = bmpTbl[ ( int )Rnd( 0, 4 ) ];
	//pAct->bmpNo = Rnd( 1, 10 );
	/* 表示優先度 */
	pAct->dispPrio = 1;
	/* ＨＰ */
    pAct->hp = 10;//Rnd( 10, 50 );
	/* 初期位置ランダム */
//	pAct->x = Rnd( 0, 608 );
	pAct->x = Rnd( 0, 640 );
	ATR_CRS(pAct) = 16;
	ATR_SPD(pAct) = Rnd( 28, 36 );
	pAct->dx = 0;

	pAct->y = 0;
	/* 増分ランダム */
	pAct->dx = Rnd( -3, 3 );
	pAct->dy = Rnd( 1, 3 );
	
	return pAct;
}

/* 敵弾処理 *********************************************************************/
void enemy_missile( ACTION *a0 )
{
	gemini(a0);		//移動
	//画面外なら
	if(ATR_H_POS(a0)>=640 || ATR_H_POS(a0)<-32 || ATR_V_POS(a0)>480 || ATR_V_POS(a0)<-32)
		DeathAction( a0 );		//終了
}

/* 敵処理 *********************************************************************/
void Enemy( ACTION *pAct )
{
	ENEMY *pYobi = ( ENEMY *)pAct->pYobi;

	/* 攻撃受けた時 */
	if( pAct->damage > 0 ){
		pAct->hp -= pAct->damage;
		if( pAct->hp <= 0 ){ 
			pAct->state = ACT_STATE_DEAD;
			pYobi->cnt  = 0;
			pYobi->cnt2 = 0;
			pYobi->cnt3 = 0;
		}
		pAct->damage = 0;
	}

	/* 状態で分岐 */
	switch( pAct->state ){
		/* 通常時 */
		case ACT_STATE_ACT:
		
			/* 移動 */
//			pAct->x += pAct->dx;
//			pAct->y += pAct->dy;

			/* リミットチェック */
//			if( pAct->x < 0 ){ 
//				pAct->x = 0;
//				pAct->dx *= -1;
//			}
//			else
//			if( pAct->x > 608 ){ 
//				pAct->x = 608;
//				pAct->dx *= -1;
//			}
//			if( pAct->y > 480 ){ 
//				pAct->y = -32;
//			}

			pAct->dx++;		//２割り込みに１回スピード変更する
			pAct->dx&=1;	//
			gemini(pAct);		//移動
			if(ATR_CRS(pAct)==0){		//上移動中なら
				if( pAct->y < -32 ){			//上端なら
					ATR_CRS(pAct)=16;			//下移動開始
					ATR_SPD(pAct) = Rnd( 28, 36 );
					pAct->x = Rnd( 0, 640 );	//
				} else {
					if(pAct->dx)
						ATR_SPD(pAct)++;		//スピードアップ
				}
			} else {
				if(pAct->dx){
					ATR_SPD(pAct)--;		//スピードダウン
					if(!ATR_SPD(pAct)){		//停止したなら
						ATR_CRS(pAct)=0;		//上移動開始
						ACTION *a1;		//ミサイル発射
						int d1,d2;
						/* アクションリストに登録 */
						a1 = GetAction( PRIO_ENEMY_TAMA, 0 );
						if( a1 == NULL ) break;
						/* 実行関数 */
						a1->func = enemy_missile;
						/* スプライト番号 */
						a1->bmpNo = No++;
						/* 表示優先度 */
						a1->dispPrio = 3;
						/* 初期位置 */
						ATR_H_POS(a1) = ATR_H_POS(pAct)+16;
						ATR_V_POS(a1) = ATR_V_POS(pAct)+16;
						d1 = pJiki->x;			//自機の方向セット
						d2 = pJiki->y;			//
						radar(a1, &d1, &d2);	//
						ATR_CRS(a1)=d1;			//
						ATR_SPD(a1)=16;			//移動速度
						play_se( 2, ATR_H_POS(a1), ATR_V_POS(a1) );		//ミサイル発射音
					}
				}
			}
			// アニメーション
/*			pYobi->cnt3 += pAct->dy;
			if( pYobi->cnt3 >= 20 ){ 
				pYobi->cnt3 = 0;
				pYobi->animCnt = pYobi->animCnt * ( -2 ) + 1;
				pAct->bmpNo += pYobi->animCnt;
			}
*/
/*			
			pAct->cnt++;
			if( pAct->cnt >= 300 ){
				pAct->state = ACT_STATE_DEAD;
				pAct->cnt = 0;
			}
*/			
			//if( Rnd( 0, 300 ) == 0 ) MakeEnemyTama( pAct );
			//MakeEnemyTama( pAct );
			
			break;
		
		/* 死亡時 */	
		case ACT_STATE_DEAD:
		
			if( pYobi->cnt == 0 ){ 
				pAct->bmpNo = bomData[ 0 ];
				pYobi->cnt = 1;
			}
			/* 爆発アニメーション */
			pYobi->cnt2++;
			if( pYobi->cnt2 >= 5 ){
				pAct->bmpNo = bomData[ pYobi->cnt ];
				pYobi->cnt++;
				pYobi->cnt2 = 0;
				/* 終わったら死ぬ */
				if( pYobi->cnt == 6 ){
					DeathAction( pAct );
				}
			}
			
			break;
	}

}

/********************************************/
/*	敵弾									*/
/********************************************/

/* 敵弾作成 *********************************************************************/
ACTION *MakeEnemyTama( ACTION *pOya )
{
	ACTION *pAct;
	//float dir;
	
	/* アクションリストに登録 */
	pAct = GetAction( PRIO_ENEMY_TAMA, 0 );
	if( pAct == NULL ) return NULL;
	
	/* 実行関数 */
	pAct->func = EnemyTama;
	/* スプライト番号 */
	pAct->anim_chr_no = Rnd( 31554, 31556 );		//スライム
	// 当たり判定 + ボックス表示
	pAct->atr |= ACT_ATR_HIT_BOX;
	//pAct->anim_no = Rnd( ANIM_ATTACK, ANIM_SATTACK );
	//pAct->anim_no = ANIM_ATTACK;
	pAct->anim_no = ANIM_WALK;
	/* 表示優先度 */
	pAct->dispPrio = DISP_PRIO_JIKI;
	/* ＨＰ */
	pAct->hp = 0;
	/* 初期位置 */
	//pAct->x = pOya->x + 16;
	//pAct->y = pOya->y + 32 ;
	pAct->x = Rnd( 100, 500 );
	pAct->y = Rnd( 100, 400 );
	
	// 移動方向	
	radar2( pAct, pJiki->x, pJiki->y, 1 );
	//移動速度
	pAct->spd = 16;			
	
	// アニメーション
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
	return pAct;
}

/* 敵弾処理 *********************************************************************/
void EnemyTama( ACTION *pAct )
{

	/* 攻撃受けた時 */
	if( pAct->damage > 0 ){
		pAct->hp -= pAct->damage;
		if( pAct->hp <= 0 ){ 
			pAct->state = ACT_STATE_DEAD;
		}
		pAct->damage = 0;
	}

	/* 状態で分岐 */
	switch( pAct->state ){
		/* 通常時 */
		case ACT_STATE_ACT:
			
			// マウスカーソルがヒットしていたら
			if( pAct->hitDispNo == HitDispNo ){
				// 左クリックが押されている時
				if( mouse.state & MOUSE_LEFT_CRICK ){
					// ドラッグ移動
					pAct->x = mouse.nowPoint.x;
					pAct->y = mouse.nowPoint.y;
				}
				// 左クリックが押されている時
				if( mouse.state & MOUSE_RIGHT_CRICK ){
					// ドラッグ移動
					DeathAction( pAct );
				}
				
			}	
			/* 移動 */
			//pAct->x += pAct->dx;
			//pAct->y += pAct->dy;
			//gemini( pAct );		//移動
#if 0	
			/* リミット */
			if(	pAct->y <= 0 ){
				pAct->y = 0;
				DeathAction( pAct );
			}else
			if(	pAct->y >= 481 ){ 
				DeathAction( pAct );
			}
			if(	pAct->x <= -8 ){
				DeathAction( pAct );
			}else
			if( 640 <= pAct->x ){ 
				DeathAction( pAct );
			}
#endif			
			break;
		
		/* 死亡時 */	
		case ACT_STATE_DEAD:
		
			DeathAction( pAct );
			
			break;
	}
	// 移動方向	
	radar2( pAct, pJiki->x, pJiki->y, Rnd( 0, 10 ) );
	// アニメーション
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
}

/* ゲームオーバー作成 *******************************************************************/
ACTION *MakeGameOver( void )
{
	ACTION *pAct;
	
	/* アクションリストに登録 */
	pAct = GetAction( PRIO_GAME_OVER, 0 );
	if( pAct == NULL ) return NULL;
	
	/* 実行関数 */
	pAct->func = GameOver;
	
	// 非表示
	pAct->atr |= ACT_ATR_HIDE;
	
	/* スプライト番号 */
	pAct->bmpNo = 0;
//	pAct->bmpNo = 30;
	/* ＨＰ */
	pAct->hp = 100;
	/* 初期位置 */
	pAct->x = 250;
	pAct->y = 200;

	return pAct;
	
}

/* ゲームオーバー処理 *******************************************************************/
void GameOver( ACTION *pAct )
{
	//char szMoji[128];
	
	// GAME OVER 表示
	
}

/* ＢＧ作成 *********************************************************************/
ACTION *MakeBg( void )
{
	ACTION *pAct;
	
	/* アクションリストに登録 */
	pAct = GetAction( PRIO_BG, NULL );
	if( pAct == NULL ) return NULL;
	
	/* 実行関数 */
	pAct->func = Bg;
	
	/* スプライト番号 */
	//pAct->bmpNo = 30;
//	pAct->bmpNo = 44;
	pAct->anim_chr_no = 9376;
	/* 表示優先度 */
	pAct->dispPrio = 0;
	/* 初期位置 */
	pAct->x = 320;
	pAct->y = 240;

	// 取出す座標
	//pYobi->rcX = 0;
	//pYobi->rcY = 480;
	//SetRect( &Rc[ pAct->bmpNo ], pYobi->rcX, pYobi->rcY, pYobi->rcX + 640, pYobi->rcY + 480 ); 

	return pAct;
}
	
/* ＢＧ処理 *********************************************************************/
void Bg( ACTION *pAct )
{
	// 取出す座標
	// 取出す座標
	//pAct->y++;
	//if( pAct->y >= 240 ) pAct->y = -240; 
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );		//アニメーション処理
	//StockDispBuffer( pAct->x, pAct->y+480, 0, pAct->anim_chr_no, 0 );
	//StockDispBuffer( pAct->x, pAct->y, 0, pAct->anim_chr_no, 0 );
	//StockDispBuffer( pAct->x, pAct->y+480, 0, pAct->anim_chr_no, 0);
}


/* ケン作成 *********************************************************************/
ACTION *MakeKen( void )
{
	ACTION *pAct;
	
	/* アクションリストに登録 */
	pAct = GetAction( PRIO_BG, NULL );
	if( pAct == NULL ) return NULL;
	
	/* 実行関数 */
	pAct->func = Ken;
	
	/* スプライト番号 */
	pAct->bmpNo = 33;
	/* 表示優先度 */
	pAct->dispPrio = 100;
	/* 初期位置 */
	pAct->x = 100;
	pAct->y = 100;
	
	// 取出す座標
	//pYobi->rcX = 0;
	//pYobi->rcY = 480;
	//SetRect( &Rc[ pAct->bmpNo ], pYobi->rcX, pYobi->rcY, pYobi->rcX + 640, pYobi->rcY + 480 ); 

	return pAct;
}
	
/* ケン処理 *********************************************************************/
void Ken( ACTION *pAct )
{
	int anim[] = { 	0,0,0,0,
					1,1,1,1,
					2,2,2,2,
					3,3,3,3,
					2,2,2,2,
					1,1,1,1 
					};
}


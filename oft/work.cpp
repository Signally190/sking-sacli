﻿#include "../systeminc/version.h"
#include	"vg410.h"
#include	"work.h"

/*----------  ???  ----------*/
Uint32 joy_con[2]={0,0};
Uint32 joy_trg[2]={0,0};
Uint32 joy_pul[2]={0,0};
Uint32 joy_buf[2];
Uint32 joy_auto[2]={0,0};
Uint16 joy_timer[2];
Uint16 s_timer;
unsigned char di_key[256];
#ifndef DI_2
LPDIRECTINPUTDEVICE8 pDInputDevice;
LPDIRECTINPUTDEVICE8 pDInputDevice2;
#else
LPDIRECTINPUTDEVICE pDInputDevice;
LPDIRECTINPUTDEVICE2 pDInputDevice2;
#endif
char joy_flg = FALSE;
char keyboad_flg = FALSE;
int	slow_flg = 0;
int flash_vct_no = 0;
int quake_flg = 0;
int quake_vct_no = 0;
ACTION *p_party[20];
int action_inf;
// 
BOOL DInputActiveFlag = TRUE;	

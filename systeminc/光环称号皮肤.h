#pragma once


extern ACTION *pActMenu光环;
extern ACTION *pActMenu称号;
extern ACTION *pActMenu皮肤;



#define MAX_COUNT_光环 200
#define MAX_COUNT_称号 200
#define MAX_COUNT_皮肤 600


struct 光环STRUCT
{
	int bmpNO;
	char name[20];
	int time;	  //使用时限  -1:永久  0:不可使用  >0:限时
};

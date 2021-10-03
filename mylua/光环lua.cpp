#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "../systeminc/netmain.h"
#include "winlua.h"
#include "../systeminc/光环称号皮肤.h"

#include <locale.h>
#include <io.h>
#ifdef _WIN_LUAJIT_



extern struct 光环STRUCT data称号[];
extern int count_英雄称号;

static int SetCount(lua_State *L)
{
	const int num = luaL_checkint(L, 1);
	count_英雄称号 = num;
	return 1;
}

static int SetList(lua_State *L)
{
	size_t l;
	const int id = luaL_checkint(L, 1);
	const int bmp = luaL_checkint(L, 2);
	const char * name = luaL_checklstring(L, 3, &l);
	const int time = luaL_checkint(L, 4);


	data称号[id].bmpNO = bmp;
	strcpy(data称号[id].name, name);
	data称号[id].time = time;
	return 1;
}


static const luaL_Reg ChenghaoFunc[] = {
{ "SetCount",		SetCount },
{ "SetList",		SetList },
{ NULL,					NULL }
};

LUALIB_API int luaopen_ChenghaoList(lua_State *L) {
	luaL_register(L, "ChengHao", ChenghaoFunc);
	return 1;
}
#endif



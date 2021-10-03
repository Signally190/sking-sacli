#include "../systeminc/version.h"
#include "../systeminc/system.h"
#include "../systeminc/pc.h"
#include "winlua.h"

#include <locale.h>
#include <io.h>
#ifdef _WIN_LUAJIT_
extern PET pet[];
static int getPetData(lua_State *L) 
{
	const int id = luaL_checkint(L, 1);
	size_t l;
	const char *attribute=luaL_checklstring(L, 2, &l);
	
 	if(strcmp(attribute, "위치") == 0){     
		lua_pushnumber(L, pet[id].index);
 	}else if(strcmp(attribute, "이미지번호") == 0){     
		lua_pushnumber(L, pet[id].graNo);
	}else if(strcmp(attribute, "HP") == 0){     
		lua_pushnumber(L, pet[id].hp);
	}else if(strcmp(attribute, "MaxHP") == 0){     
		lua_pushnumber(L, pet[id].maxHp);
	}else if(strcmp(attribute, "MP") == 0){     
		lua_pushnumber(L, pet[id].graNo);
	}else if(strcmp(attribute, "MaxMP") == 0){     
		lua_pushnumber(L, pet[id].graNo);
	}else if(strcmp(attribute, "경험") == 0){     
		lua_pushnumber(L, pet[id].exp);
	}else if(strcmp(attribute, "최대경험") == 0){     
		lua_pushnumber(L, pet[id].maxExp);
	}else if(strcmp(attribute, "레벨") == 0){     
		lua_pushnumber(L, pet[id].level);
	}else if(strcmp(attribute, "공격력") == 0){     
		lua_pushnumber(L, pet[id].atk);
	}else if(strcmp(attribute, "방어력") == 0){     
		lua_pushnumber(L, pet[id].def);
	}else if(strcmp(attribute, "민첩력") == 0){     
		lua_pushnumber(L, pet[id].quick);
	}else if(strcmp(attribute, "충성") == 0){     
		lua_pushnumber(L, pet[id].ai);
	}else if(strcmp(attribute, "地") == 0){     
		lua_pushnumber(L, pet[id].earth);
	}else if(strcmp(attribute, "水") == 0){     
		lua_pushnumber(L, pet[id].water);
	}else if(strcmp(attribute, "火") == 0){     
		lua_pushnumber(L, pet[id].fire);
	}else if(strcmp(attribute, "风") == 0){     
		lua_pushnumber(L, pet[id].wind);
	}else if(strcmp(attribute, "최대기능수") == 0){     
		lua_pushnumber(L, pet[id].maxSkill);
	}else if(strcmp(attribute, "회전수") == 0){     
		lua_pushnumber(L, pet[id].trn);
	}else if(strcmp(attribute, "종별부호") == 0){     
		lua_pushnumber(L, pet[id].fusion);
	}else if(strcmp(attribute, "상태") == 0){     
		lua_pushnumber(L, pet[id].status);
	}else if(strcmp(attribute, "이름") == 0){     
		lua_pushstring(L, pet[id].name);
	}else if(strcmp(attribute, "애칭") == 0){     
		lua_pushstring(L, pet[id].freeName);
	}else if(strcmp(attribute, "플래그 사용") == 0){     
		lua_pushnumber(L, pet[id].useFlag);
	}else if(strcmp(attribute, "개명표식") == 0){     
		lua_pushnumber(L, pet[id].changeNameFlag);
	}     
	return 1;
}


static const luaL_Reg Datalib[] = {
	{"pet",					getPetData},
	{NULL,					NULL}
};

LUALIB_API int luaopen_Data (lua_State *L) {
	luaL_register(L, "chardata", Datalib);
	return 1;
}
#endif



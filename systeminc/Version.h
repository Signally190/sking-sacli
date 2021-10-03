#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef _WIN32
#include <crtdbg.h>
#endif




#define CC_HOST
/* 请将控制专案的 definition 放在这个档案
 * 然后藉由 remark/unmark 来开关功能
 */


#define ITEM_AURA_

#define _NB_管理模式						// (2018.10.17)

#define _NB_ShiQiMax


#define _NB_游戏任务日志文2 "游戏版本：" __DATE__	// (2019.01.05)

#ifdef _NB_ShiQiMax
#define _服务器标识 "ShiQiMax"
#define _LUA_KEY_ "vFaJakKuDkKxkogfgWjWDBBoKgilxHoD"  // shiqimax可用
#define _分离补丁密钥	"UqtLlUHgwfuWjdIfdsFCAfaHqouLOpUb"	// shiqimax可用
#define _NB_游戏任务日志文1 "官方网站：www.shiqimax.com"	// (2019.01.05)
#define _NB_红包系统						// (2019.01.28)
#define 无挂服务器
#endif

//石器版本控制
//#define __3_0					   // ( 3.0 开) Cary 石器3.0的控制
//#define _SA_VERSION_60		   // ( 6.0 开) WON ADD 石器时代6.0 的版本控制
#define _SA_VERSION_70			   // ( 7.0 开) WON ADD 石器时代7.0 的版本控制

//#define _DEFAULT_PKEY "Wonwonwon"		// (可开放)( 石器 6.0 )
//#define _RUNNING_KEY  "11111111"		// (可开放)( 石器 6.0 )

#ifndef CC_HOST
#define _DEFAULT_PKEY "cary"		// (可开放)( 石器 6.0 )
#define _RUNNING_KEY  "1234567"		// (可开放)( 石器 6.0 )
#else
#define _DEFAULT_PKEY "www.93sa.com"		// (可开放)( 石器 6.0 )
#define _RUNNING_KEY  "www.93sa.com"		// (可开放)( 石器 6.0 )
#define _SA_VERSION 'L'
//#define _DEFAULT_PKEY "1828431919"		// (可开放)( 石器 6.0 )
//#define _RUNNING_KEY  "53568238706"		// (可开放)( 石器 6.0 )
#endif


/* -------------------------------------------------------------------
 * 专案：族人职务与家族功能补充
 * 人员：小瑜、彦勋、志伟、小民
 * 说明：
 */
#define  _FMVER21                      // (可开放) 家族职掌功能
#define _PERSONAL_FAME                 // (可开放) Arminius 8.30 家族个人声望
#define _LEADERRIDE	 				   // (可开放) 四大家族骑暴功能 Robin 0921
 
// -------------------------------------------------------------------
// 专案：伊甸大陆新NPC交谈介面
// 人员：晓威
// 说明：__EDEN_EFFECT:赌场的银行NPC'委托交易的NPC
//			 __EDEN_AUCTION:委托NPC拍卖物品介面
#define __EDEN_EFFECT			// (可开放) 但测试时要打开
#define __EDEN_AUCTION		    // (可开放) 但测试时要打开

// -------------------------------------------------------------------
// 专案：攻击性咒术的制作
// 人员：笃志
// 说明：
#define __ATTACK_MAGIC

// -------------------------------------------------------------------
// 专案：吓年兽的鞭炮
// 人员：晓威
// 说明：
#define _ITEM_FIRECREAKER

/* -------------------------------------------------------------------
 * 新功能：PK星系
 * 人员：彦勋
 * 说明：
 */
//#define _PKSERVER // (不可开放)


// -------------------------------------------------------------------
// 专案：传送手机简讯的玩家界面实作
// 人员：昌磊  (LeiBoy)
// 说明：让玩家可以传简讯给特定的ASP网页处理
// 主要相关档 : PhoneMsg.cpp
// 完成日期 : 2002年01月28日
// 开放日期 : 2002年03月04日
// Status : Ready!
//#define __PHONEMESSAGE    
//#define _FIX_URLENCODE             // WON ADD (可 开 放) 北京修正简讯无法加空格的问题


// -------------------------------------------------------------------
// 专案：传送手机简讯的二期工程
// 人员：(原定)昌磊  (接班人未定)
// 说明：让玩家可以传简讯到名片中好友的手机中
// 开放日期 : 禁止开放!!
// Status : 建构中...
//#define __CARDMESSAGE   //(禁用)!!


//#define __NEW_CLIENT_ONLY_WRITE	//Cary(禁用,未测过)
//#define CHINA_VER			//Cary (禁用)
//#define __ALBUM_4
//#define __ALBUM_6			//Cary 加入新的宠物相簿(含年兽)
//#define __TEST_SERVER		//Cary 3.0的测试server
//#define __EMOTION			//Cary 特殊的表情，只做client
#define __HELP				//Cary 线上说明
#define __NEW_PROTOCOL		//Cary 新的底层
#define __NEW_CLIENT		//Cary 
#define __NEW_CLIENT_MEM	//Cary
#define __SKYISLAND			//Cary 加入天空岛的功能
#define __HI_COLOR			//Cary 加入高彩的功能(debug version专用)
#define __TALK_TO_NPC		//Cary 使用左键和npc说话
#define __NEW_BATTLE_EFFECT	//Cary 2002.2.21 新的战斗特效，如:替身娃娃
#define _AFTER_TRADE_PETWAIT_	   // (可 开 放) set pet status to wait after trade :vincent

/* -------------------------------------------------------------------
 * 新功能：伊甸4
 * 人员：志旺
 * 说明：
 */
#define _FIX_DEL_MAP			   // (可开放) WON ADD 修正玩家抽地图

#define _FIX_9_FMPOINT			   // (可开放) WON ADD 九大庄园
#define _FIX_10_FMPOINT			//(可开) Change 十大庄园 20050404
#ifdef _FIX_10_FMPOINT
#define fm_point_num	10		   // 庄园数
#else
#define fm_point_num	9		   // (可开放) WON ADD 庄园数
#endif

#define _ADD_POOL_ITEM			   // (可开放) WON ADD 增加寄放道具
//#define _ADD_FAMILY_TAX	       // (不可开) WON ADD 增加庄园税收

#define _LOST_FOREST_FOG           // (可开放) the lostforest's fog. code:shan 
#define _MIND_ICON                 // (可开放) show icon from char mind. code:shan
#define _MAGIC_WEAKEN              // (可开放) show magic(weaken). code:shan

#define __ALBUM_47				   // 虎鲸两只






// -------------------------------------------------------------------
// vincent define
#define _MAGIC_DEEPPOISION         // (可开放) show magic(deeppoison). code:vincent
#define _MAGIC_BARRIER             // (可开放) show magic(barrier). code:vincent
#define _MAGIC_NOCAST              // (可开放) show magic(nocast). code:vincent
#define _ITEM_CRACKER              // (可开放) 道具:拉炮 code:vincent
#define _SKILL_ROAR                // (可开放) vincent  宠技:大吼(克年兽)
#define _SKILL_SELFEXPLODE         // (可开放) vincent  宠技:自爆

#define _NEW_RIDEPETS			   // (可开放) 新骑宠

#define _ATTDOUBLE_ATTACK	       // (可开放) 属性强化攻击
#define __TOCALL_MAGIC			   // (可开放) kjl 02/06/24	
#define _VARY_WOLF                 // (可开放) pet skill : vary wolf. code:shan
#define _PIRATE_ANM                // (可开放) monster move(海盗王). code: shan
#define _TRADE_PETCOLOR            // (可开放) Syu ADD 改变交易时宠物转生颜色
#define _FIXSHOWMPERR			   // (可开放) Syu ADD 修正补气时动画显示错误
#define _LIZARDPOSITION			   // (可开放) Syu ADD 修正龙蜥显示位置过低


/* -------------------------------------------------------------------
 * 新功能：
 * 更新日期2002/12/18 14:00 更新
*/

#define _TEAM_KICKPARTY				// (可开放) ANDY 队长踢人
#define _MONEYINPUT					// (可开放) Syu ADD 手动输入金钱量

#define _PETS_SELECTCON				// (可开放) ANDY 宠物选择



/* -------------------------------------------------------------------
 * 新功能：石器6.0
 * 更新日期
*/
#define _OBJSEND_C				   // (可开放) ANDY 6.0

#define _ADD_SHOW_ITEMDAMAGE			   // (可开放) WON ADD 显示道具的耐久度

#define _WATERANIMATION			   // (可开放) Syu ADD 泪之海动画层
#define _NEWDRAWBATTLEMAP		   // (可开放) Syu ADD 水世界战斗地表与战斗特效 
#define _AniCharBubble		       // (可开放) Syu ADD 动画层人物吐出气泡
#define _AniCrossFrame		       // (可开放) Syu ADD 动画层游过画面生物
#define _AniImmobile			   // (可开放) Syu ADD 定点产生特定动画
#define _AniRandom				   // (可开放) Syu ADD 随机产生环境动画
//#define _DELBORNPLACE			   // (可开放) Syu ADD 6.0 统一出生于新手村
#define _NEWLOGO				   // (可开放) Syu ADD 6.0登入画面新logo 要关_v4_0 
#define _NEWMUSICFILE6_0		   // (可开放) Syu ADD 6.0 新地图音乐

#define _TRADESHOWNAME			   // (可开放) Syu ADD 交易显示对方名称
#define _TRADEINPUTMONEY		   // (可开放) Syu ADD 交易手动输入金钱
#define _TRADE_SHOWHP			   // (可开放) Syu ADD 交易显示血量
#define _CHANGETRADERULE		   // (可开放) Syu ADD 交易规则修订

#define _PETSKILL_CANNEDFOOD	   // (可开放) ANDY 宠物技能罐头
#define _SPECIALSPACEANIM		   // (可开放) Syu ADD 特殊场景动画配置
#define _SHOOTCHESTNUT			   // (可开放) Syu ADD 宠技：丢栗子



/* -------------------------------------------------------------------
 * 新功能：石器7.0
 * 更新日期
*/
//已开放
#define _SARS						// (可开放) WON ADD 毒煞蔓延
#define _FIX_MAXGOLD			    // (可开放) WON ADD 增加人物金钱上限

#define _ITEM_PILENUMS				// (可开放) ANDY 道具素材堆叠

#define _ITEM_EQUITSPACE			// (可开放) ANDY 新增装备栏位
#define _EQUIT_ARRANGE				// (可开放) ANDY 格挡
#define _EQUIT_NEWGLOVE				// (可开放) ANDY 手套栏位

#define _LOCKPARTYFLOOR				// (可开放) Syu ADD 增加不可组队的地图编号
#define _LOCKHELP_OK				// (可开放) Syu ADD 锁定不可加入战斗

#define _TRADESYSTEM2				// (可开放) Syu ADD 新交易系统

#define _TRADETALKWND				// (可开放) Syu ADD 交易新增对话框架
#define _TRADELOCKBTN				// (可开放) Syu ADD 增加锁定键

#define _WAVETRACK					// (可开放) Syu ADD 音波攻击轨道修正

#define _CHAR_NEWLOGOUT				// (可开放) ANDY 新登出机制
//end deleted by lsh

//deleted by lsh on 2011.08.03

//新功能：人物职业
#define _TABLEOFSKILL				// (可开放) Syu ADD 7.0 新增人物技能表单
//#define _NEWPANEL_71				// (可开放) ROG ADD 新login画面
//#define	_NEWPANEL_70				// (可开放) Syu ADD 石器7.0 LOGIN 画面
#define _CHAR_PROFESSION			// (可开放) WON ADD 人物职业
#define _SKILLSORT					// (可开放) Syu ADD 职业技能排列分类
#define _NEWREQUESTPROTOCOL			// (可开放) Syu ADD 新增Protocol要求能力细项
#define _OUTOFBATTLESKILL			// (可开放) Syu ADD 非战斗时技能Protocol
#define _NEWSOUNDEFFECT				// (可开放) Syu ADD 动画音效档
#define _FIXSTATUS					// (可开放) Syu ADD 新战斗状态显示方式
#define _ALLDOMAN					// (可开放) Syu ADD 排行榜NPC
#define _CHATROOMPROTOCOL			// (可开放) Syu ADD 聊天室频道
#define _FRIENDCHANNEL				// (可开放) ROG ADD 好友频道
#define	_TELLCHANNEL				// (可开放) ROG ADD 密语频道

#define _DROPPETWND					// (可开放) Syu ADD 丢弃宠物确认

#define _MAILSHOWPLANET				// (可开放) Syu ADD 显示名片星球
#define _MOON_FAIRYLAND				// (可开放) ROG ADD 月之仙境
#define _SPECIALSPACEANIM_FIX		// (可开放) ROG ADD 修正动画
#define _NEW_CLOUD					// (可开) Change 新的地图的云效果
//#define _TIMEBAR_FUNCTION			// (不可开) ROG ADD 时间bar函式

//10/23
#define _FM_MODIFY				// 家族功能修改
//end deleted by lsh

#define _HI_COLOR_16				// (可开) ANDY (8bit to 16bit) 含图档
#define _HI_COLOR_32
#define _FIREHUNTER_SKILL			// (可开放) ROG ADD 朱雀技能_火线猎杀
#define _PETSKILL_ACUPUNCTURE       // (可开放) Change 宠技:针刺外皮
#define _PETSKILL_DAMAGETOHP	    // (可开放) Change 宠技:暗月狂狼(嗜血技的变体) 
#define _PETSKILL_BECOMEFOX         // (可开放) Change 宠技:媚惑术
#define _PETSKILL_BECOMEPIG         // (可开) Change 宠技:乌力化


#define _CHANNEL_MODIFY		// 频道功能整合

// 新功能：连接新资料库(新付费机制)
#define _NEW_WGS_MSG				// (可开) WON ADD WGS的新视窗

#define _CHANGEGALAXY // Robin 星系移民
#define _ERROR301 // Robin AP回传301错误,产品尚未注册
//end deleted by lsh

#define _STANDBYPET // Robin 送待机宠列表到Server

#define	DIRECTINPUT_VERSION 0x0700	// Robin 增加DirectX SDK的相容性

//#define _CLOUD_EFFECT				// (不可开) ROG ADD	云层效果
//#define _NPC_WELFARE_2			// (不可开) WON ADD 职业NPC-2

//#define _NPC_SELLSTH				// (可开放) ANDY 拍卖
//#define _SYUTEST
//#define _SYUTEST2
//#define _SYUTEST3
//#define _AUCPROTOCOL				// (不可开) Syu ADD 拍卖频道开关Protocol

/* -------------------------------------------------------------------
 * 新功能：PK伺服器
 * 更新日期
*/
//测试中
//制作中

//#define _PKSERVERCHARSEL			// (不可开) Syu ADD PK伺服器选择星系人物

//#define _PK2007
//#define HITFLAG_FONT

//PK服务管理员功能
#define PK_SERVICE_MAN
// -------------------------------------------------------------------
// 以下不可开
/* -------------------------------------------------------------------
 * 新功能：real.bin分图
 * 更新日期
*/


// -------------------------------------------------------------------
//#define _OMIT_WGS					// (不可开) 略过帐号、密码至WGS. code:shan
// -------------------------------------------------------------------
//#define _CHECK_GAMESPEED		    // (不可开) ANDY 检测加速
/* -------------------------------------------------------------------
 * 新功能：石器5.0
 * 说明：
 */
//#define _CANT_ATK					// (不可开) can't attack(own team). code: shan
//#define _SETTICK_COUNT			// (不可开) ANDY 重复发话
//#define _GM_MARK					// (不可开) WON ADD GM 识别
//#define _GM_IDENTIFY		        // (不可开)Rog ADD GM识别的gm指令

//#define _HI16_REEDITIMAGE			// (不可开) ANDY (8bit to 16bit) 图档处理


/* -------------------------------------------------------------------
 * 新功能：石器 800 * 600 
 * 人员：旭诚,镇玮
 * 说明：
*/
#define _NEW_RESOMODE  //800 600模式
//#define _NEWDRAWBATTLEMAP_FIX		   // (不可开)ROG ADD 战斗地图修改
/* 重要：更新Client前要把两个值改为0 */
//#define DISPLACEMENT_X 384
//#define DISPLACEMENT_Y 288
//#define DISPLACEMENT_X 160
//#define DISPLACEMENT_Y 120
//#define DISPLACEMENT_X 0
//#define DISPLACEMENT_Y 0
extern int DISPLACEMENT_X;
extern int DISPLACEMENT_Y;

// Terry define start -------------------------------------------------------------------
// 侦测有没有memory leak ----------------------------------------------------------------
#ifdef _STONDEBUG_
#define MALLOC(s) _malloc_dbg(s,_CLIENT_BLOCK,__FILE__,__LINE__)
#define CALLOC(c,s) _calloc_dbg(c,s,_CLIENT_BLOCK,__FILE__,__LINE__)
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#define FREE(p) _free_dbg(p,_CLIENT_BLOCK)
#else
#define MALLOC(s) malloc(s)
#define CALLOC(c,s) calloc(c,s)
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#define FREE(p) free(p)
#endif
// --------------------------------------------------------------------------------------
//蔚赊酦珨羲宎腔唾昜温堤懂
//#ifdef _STONDEBUG_  // wxy 蛁庋
//#define PROFILE_TEST
//#define _PET_TRACE_MOUSE
//#define __EDEN_DICE  				// (不可开) 赌场骰子
//#endif  //wxy 蛁庋
//-----------------------
unsigned int TimeGetTime(void);
// Terry define end   -------------------------------------------------------------------

//复刻版控制
//#define _BACK_VERSION //复刻版


#ifdef _BACK_VERSION
	#define _REMAKE_20				// 石器复刻功能控制(防外挂)
	#define _BACK_LOGO
	#define _TIME_GET_TIME		// 用QueryPerformanceCounter取代timeGetTime
	#define _BACK_WINDOW //复刻版视窗化(debug版才可开,内部人员使用)
	#define _READ16BITBMP			// Terry 可读入及显示高彩图
#else
	#define _ATTACK_AI					//内挂攻击AI
	#define _AI_OTHER					// AI其它功能
	#define _AI_NEW_FIRST			// AI 首次可以用职业技能
	#define __AI				//Cary 加入自动战斗功能
	#define _BATTLESKILL				// (可开放) Syu ADD 战斗技能介面
	#define _NEWPANEL				    // (可开放) Syu ADD 7.0 新人物状态介面
#endif

//end modified by lsh

/* -------------------------------------------------------------------
 * 新功能：石器7.5
 * 更新日期
*/


#define _ANGEL_SUMMON			// 天使的招唤 Robin
#define _PROFESSION_ADDSKILL  // (可开) Change 追加职业技能
#define _TRANS_6				// (可开) Change 人物6转
#define _TRANS_7				//  人物7转
#define _PET_2TRANS			// (可开) Change 宠物2转
//end deleted by lsh

//#define _75_LOGIN					// 7.5版login画面 
#define _SURFACE_ANIM     // 动态地上物显示

#define _STREET_VENDOR		// 摆摊功能
#define _NEW_MANOR_LAW		// 新庄园规则

#define _PETSKILL_LER			// 雷尔技能
//#define _PK_SERVER            //百万PK活动
//#define _8_TEST					//8.0测试server

#define _STREET_VENDOR_CHANGE_ICON	// 修改摆摊图示

//#define _TALK_WINDOW				// 对话视窗
#define _SHOW_FUSION				// (可开) Change 融合宠物在CLIENT端只显示转生数,修改为融合宠显示为融合

#define _FONT_SIZE					// Robin (要开) 可改字型大小
#define _CONTRACT					// Robin (要开) 契约功能
#define _HALLOWEEN_EFFECT			// 万圣节特效
#define _ADD_STATUS_2				// 新增传送人物状态第二类
#define _PETSKILL_BATTLE_MODEL		// 宠物技能战斗模组
#define _RACEMAN					// cyg 猎宠大会新npc
#define _PETSKILL_ANTINTER			// (可开) Change 宠技:蚁之葬
#define _PETSKILL_EXPLODE			// (可开) Change 宠技:爆裂攻击
#define _ITEM_FIREWORK				// 烟火功能
#define _FIREWORK_SOUND				// cyg 新增音效(烟火用)
#define _GET_HOSTNAME				// (可开) Change 取得电脑名称
#define _SKILL_ADDBARRIER			// (可开) Change 宠技:为魔障增加异常抗性功能
#define _PETSKILL_PEEL				// (可开) Change 宠技:金蝉脱壳
#define _PETSKILL_RIDE

#define _MOUSE_DBL_CLICK			// Robin 滑鼠右键双击功能

//--------------------------------------------------------------------------------------------------------
//专案 8.0 失落的世界 新加功能定义区 人员：彦勋 奇铨 晓威 逸俊
//--------------------------------------------------------------------------------------------------------
#define _PET_ITEM				// Terry 宠物可装备道具
#define _NPC_PICTURE			// (可开) Change 新赌场功能(随时变身的NPC)
#define _JOBDAILY				// cyg 任务日志功能
#define _ITEM_PATH				// (可开) Change 可踏上不可走区域的道具
#define _ALCHEMIST				// Robin 精炼功能 
#define _ALCHEPLUS				// Robin 鉴定功能 
#define _EXTEND_AB				// cyg 扩充名片功能
#define _ITEM_INTENSIFY			// (可开) Change 宠技:强化道具
#define _TEACHER_SYSTEM			// Terry 导师系统
#define _80_LOGIN				// Terry 8.0 登入画面
#define _8_RIDEPETS				// (可开) Change 8.0骑宠
#define _8BATTLEMAP				// Change 8.0 战斗地表
#define _NEW_EARTH				// Terry 新地球,可看见内部伺服器
//#define _80_LOGIN_PLAY		// Terry 8.0 登入画面
#endif
#define _DRAGON_FUSION			// 人龙进化

#define _CHATWORLDWIDEPROTOCOL		// (可开放) CC 世界频道

#define _FONT_STYLE_

#define _WIN_LUAJIT_						// LUA
#ifndef _NB_管理模式
#define _LUA加密
#endif

#ifdef _LUA加密
#define _LUA_PATCH_ ".//salua"				// LUA路径
#else
#define _LUA_PATCH_ ".//lua"				// LUA路径
#endif

#define _FONTDATALEN_ 100					// 聊天信息长度 112

#define _PTTERN_SEPARATION_BIN				// 分离补丁加载

#ifndef _NB_管理模式
#define 分离补丁加密						// 分离补丁加密
#endif
#ifdef 分离补丁加密
#define _分离补丁目录 ".//sabin"
#else
#define _分离补丁目录 ".//sapath"
#endif

#define MAX_BATTLE_ROW_CHARS   5			//每行最大战斗角色数
#define _NEW_COLOR_							//补丁支持16位RGB565高彩图片
#define _NEW_ALPHA_							//新建半透明处理
//#define _LOGINKICK							// (不可开) Syu ADD 登入画面超过10秒踢回星系列表
#define _PETCOLOR_							//1级宠物变色

#define _NEW_SPEED_							//新游戏速度
#define	_SUNDAY_STR_SEARCH					//SUNDAY字符串搜索算法  比KMP更牛速度更快的算法
#define _SYSTEM_SLEEP_   14					//Game Speed
#define BATTLKPKPLYAERNUM 20				//最大参战角色数
#define _SKILL_85_LSFIX						//火球技能不能选敌人修复
#define 画面色盘锁定白天
#define _ATTACK_EFFECT


#define 对话框2文字拉长
#define 直接骑新骑宠
#define 聊天表情
#define 修复邮件服务器显示

#define NEW家族银行图档
#define NEW个人银行图档偏移  //新UI的OK CANCEL偏移

//#define 顶部称号按钮

//#define 皮肤称号光环按钮

#define 战斗5宠
#ifdef 战斗5宠
#define 等待宠数量 5
#else
#define 等待宠数量 3
#endif 


//#define 加载新字体   //有bug，句柄泄露

#define 新机暴红狗金飞

//#define _JL_FIX_战斗速度过快

//调试用
//#define _CHAR_MANOR_DEBUG   //调试光环    .光环 图档
//#define _人物称号_DEBUG   //调试光环    .光环 图档
#define _SHOW_bmpNo		//显示图档编号  已由外挂按钮来控制
//#define _F11_SHOW_ID		//按F11，玩家名字带objIndex
//#define _SHOW_ALL_ID		//所有玩家和npc显示objIndex
//#define _SHOW_mouse		//显示鼠标坐标 底部显示
//#define _SHOW_MOUSE_POS   //显示鼠标坐标 跟随鼠标
#define LABEL_STR_
#define _MOUSE_SHOW_INFO_FOR_HEAD			//  longzoro 鼠标显示信息
/*----------------------------- New Version -----------------------------*/
#ifdef _NB_管理模式
//#define _NB_FUNRECV_DEBUG					// (2018.11.02)接收封包调试
//#define _NB_CMD_MODE						// (2018.10.11)调试模式DOS反馈printf信息
#endif
#define _NB_GAMETIME						// (2018.10.11)线路选项下角显示编译时间

#define _NB_SetResoMode						// (2018.10.10)分辨率热切
#define _NB_DIY_SEND						// (2018.10.11)发送自定义封包，封包可提供3个数值型参数

#define _NB_CANCEL_FANTI					// (2018.10.10)ESC菜单 简繁切换
#define _NB_CANCEL_BGM						// (2018.10.10)ESC菜单 开关声音
#define _NB_SupetWalk						// (2018.10.11)快速走路
#define _NB_EXPSHOW							// (2018.10.11)战斗经验显示
#define _NB_CALLGCGC						// (2018.10.11)原地遇敌CALL gcgc
#define _NB_LABEL_DISP						// (2018.10.10)聊天频道图标化
#define _NB_CHANNEL_ALLSERV					// (2018.10.11)星球频道
#define _NB_DELCHARTWOPASS					// (2018.10.13)对应服务端删除角色需安全码
//#define _NB_SA_LIAOTIAN						// (2018.10.14)聊天删减按钮
#define _NB_WINDOWS_PetList					// (2018.10.14)加强宠物选择框详细信息
#define _NB_FAMILYBADGE						// (2018.10.15)家族徽章
#define _SHOWFAMILYBADGE_						// (2018.10.15)家族徽章
#define _NB_SHOW_MAPNAME					// (2018.10.16)显示地图名和坐标 底部显示
#define _NB_道具自动叠加					// (2018.10.17)
#define _NB_道具栏详情						// (2018.10.17)
#define _NB_顶部详情						// (2018.10.17)美化顶部功能栏
//#define _NB_宠物栏详情						// (2018.10.23)
#define _NB_宠物绑定值						// (2018.12.07)
#define _NB_宠物初始值						// (2018.10.23)
#define _NB_秀宠物							// (2018.10.24)
//#define _NB_人物栏详情						// (2018.10.26)
//#define _NB_状态明细详情					// (2018.10.27)
#define _NB_新道具栏界面					// (2018.10.30)
//#define _NB_新下任务栏						// (2018.10.30)

//#define _NB_文字称号						// (2018.11.02)不要和人物称号同时开
#define 战斗光环
#define 人物光环   
#define 人物称号							// 不要和文字称号同时开
#define 人物皮肤							
#define _NB_公告系统						// (2018.11.02)
#define _NB_任务指引						// (2018.11.03)
#define _NB_可骑乘任何宠物					// (2018.12.18)
#define _NB_Fix无动画补丁					// (2018.11.25)修正无动画补丁时显示为豆丁图号
#define _NB_登录错误信息提示				// (2018.12.30)
#define _Jason_修复外挂原登问题				// (2019.01.05)
#define _NB_获取机器信息包
#define _NB_宠技商店需声望要求				// (2019.01.08)
#define _NB_战斗地图优化					// (2019.03.05)
#ifdef 无挂服务器
#define _NB_客户端直接启动					// (2018.12.10)
#define _NB_断线自动重连					// (2019.01.16)
#define _ON_OFF_ITEM						// EXP ONOFF
#define _ON_OFF_NOENEMY						// NOENEMY ONOPFF
#define _CLICK_PARTYAUTO					// CLICK PARTYAUTO 
#define _CHAR_INFOSTORAGE
#endif

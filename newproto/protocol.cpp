//#ifdef __NEW_PROTOCOL

#include <windows.h>
#include <stdio.h>
#include "../systeminc/version.h"
#include "autil.h"
#include "protocol.h"
#include "../systeminc/pc.h"
#include "../Wgs/message.h"
#include "../systeminc/system.h"
// WON FIX
#include <time.h>
#include "../systeminc/field.h"

//JL
#include "../systeminc/JL_func.h"
#include "../systeminc/menu.h"
#include "../PetRanking.h"

extern int g_服务器;

void LogToRecvdata(char *data)
{
#ifdef _LOG_MSG
	extern char lssproto_readlogfilename[];
	if(lssproto_readlogfilename[0] != '\0'){
		FILE *rfp;
		rfp = fopen(lssproto_readlogfilename, "a+");
		if(rfp){
			fprintf(rfp, "%s: 收到：\t%s\n", pc.name, data);
			fclose(rfp);
		}
	}
#endif
}

void LogToSenddata(char *data)
{
#if _LOG_MSG
	extern char lssproto_readlogfilename[];
	if(lssproto_readlogfilename[0] != '\0'){
		FILE *rfp;
		rfp = fopen(lssproto_readlogfilename, "a+");
		if(rfp){
			fprintf(rfp, "%s: 送出：\t%s\n", pc.name, data);
			fclose(rfp);
		}
	}
#endif
}

int SaDispatchMessage(int fd, char *encoded)
{
	int		func,fieldcount;
	int		iChecksum = 0, iChecksumrecv;
	char	raw[16384];

	util_DecodeMessage(raw,encoded);
#ifdef _LOG_MSG
	FILE *fp = fopen( "recvtime.txt", "a+" );
	if( fp ) fprintf( fp, "%u RECV %d  %s\n", TimeGetTime(), strlen(raw), raw);
	if( fp ) fclose( fp );
#endif	

	util_SplitMessage(raw,SEPARATOR);
	if (util_GetFunctionFromSlice(&func, &fieldcount)){

#ifdef _LOG_MSG
		FILE *fp=fopen("recvtime.txt","a+");
		if( fp ) fprintf( fp, "%u DISPATCH  func: %d\n", TimeGetTime(), func );
		if( fp ) fclose( fp );
#endif		

		//JL 2016.8.23
		JL_resetRecv();

	if (func==LSSPROTO_XYD_RECV) {
		int x;
		int y;
		int dir;

		iChecksum += util_deint(2, &x);
		iChecksum += util_deint(3, &y);
		iChecksum += util_deint(4, &dir);
		util_deint(5, &iChecksumrecv);
		if (iChecksum != iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_XYD_recv(fd, x, y, dir);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|x:%d|y:%d|dir:%d",func,"LSSPROTO_XYD_RECV",x,y,dir);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_EV_RECV)
	{
		int seqno;
		int result;

		iChecksum += util_deint(2, &seqno);
		iChecksum += util_deint(3, &result);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_EV_recv(fd, seqno, result);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|seqno:%d|result:%d",func,"LSSPROTO_EV_RECV",seqno,result);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_EN_RECV)	//JL 7号包  战斗包
	{
		int result;
		int field;

		iChecksum += util_deint(2, &result);
		iChecksum += util_deint(3, &field);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_EN_recv(fd, result, field);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|result:%d|field:%d",func,"LSSPROTO_EN_RECV",result,field);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_RS_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_RS_recv(fd, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|data:%s",func,"LSSPROTO_RS_RECV",data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_RD_RECV) 
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_RD_recv(fd, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|data:%s",func,"LSSPROTO_RD_RECV",data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_B_RECV)
	{
		char command[16384];

		iChecksum += util_destring(2, command);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}

		LogToRecvdata( command );

		lssproto_B_recv(fd, command);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|command:%s",func,"LSSPROTO_B_RECV",command);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_I_RECV)   // 22 道具包
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_I_recv(fd, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|data:%s",func,"LSSPROTO_I_RECV",data);
#endif
		SliceCount=0;
		return func;
	}else if (func==LSSPROTO_SI_RECV){
		int fromindex;
		int toindex;

		iChecksum += util_deint(2, &fromindex);
		iChecksum += util_deint(3, &toindex);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_SI_recv(fd, fromindex, toindex);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|fromindex:%d|toindex:%d",func,"LSSPROTO_SI_RECV",fromindex,toindex);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_MSG_RECV)
	{
		int aindex;
		char text[16384];
		int color;

		iChecksum += util_deint(2, &aindex);
		iChecksum += util_destring(3, text);
		iChecksum += util_deint(4, &color);
		util_deint(5, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_MSG_recv(fd, aindex, text, color);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|aindex:%d|text:%s|color:%d",func,"LSSPROTO_MSG_RECV",aindex,text,color);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_PME_RECV)
	{
		int objindex;
		int graphicsno;
		int x;
		int y;
		int dir;
		int flg;
		int no;
		char cdata[16384];

		iChecksum += util_deint(2, &objindex);
		iChecksum += util_deint(3, &graphicsno);
		iChecksum += util_deint(4, &x);
		iChecksum += util_deint(5, &y);
		iChecksum += util_deint(6, &dir);
		iChecksum += util_deint(7, &flg);
		iChecksum += util_deint(8, &no);
		iChecksum += util_destring(9, cdata);
		util_deint(10, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_PME_recv(fd, objindex, graphicsno, x, y, dir, flg, no, cdata);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|objindex:%d|graphicsno:%d|x:%d|y:%d|dir:%d|flg:%d|no:%d|cdata:%s",func,"LSSPROTO_PME_RECV",objindex, graphicsno, x, y, dir, flg, no, cdata);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_AB_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_AB_recv(fd, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|data:%s",func,"LSSPROTO_AB_RECV",data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_ABI_RECV)
	{
		int num;
		char data[16384];

		iChecksum += util_deint(2, &num);
		iChecksum += util_destring(3, data);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_ABI_recv(fd, num, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|num:%d|data:%s",func,"LSSPROTO_ABI_RECV",num,data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_TK_RECV)
	{
		int index;
		char message[16384];
		int color;

		iChecksum += util_deint(2, &index);
		iChecksum += util_destring(3, message);
		iChecksum += util_deint(4, &color);
		util_deint(5, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_TK_recv(fd, index, message, color);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|index:%d|message:%s|color:%d",func,"LSSPROTO_TK_RECV",index,message,color);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_MC_RECV)
	{
		int fl;
		int x1;
		int y1;
		int x2;
		int y2;
		int tilesum;
		int objsum;
		int eventsum;
		char data[16384];

		iChecksum += util_deint(2, &fl);
		iChecksum += util_deint(3, &x1);
		iChecksum += util_deint(4, &y1);
		iChecksum += util_deint(5, &x2);
		iChecksum += util_deint(6, &y2);
		iChecksum += util_deint(7, &tilesum);
		iChecksum += util_deint(8, &objsum);
		iChecksum += util_deint(9, &eventsum);
		iChecksum += util_destring(10, data);
		util_deint(11, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
#ifdef __SKYISLAND
		extern void SkyIslandSetNo( int fl);
		SkyIslandSetNo( fl);
#endif
		
		LogToRecvdata(data);

		lssproto_MC_recv(fd, fl, x1, y1, x2, y2, tilesum, objsum, eventsum, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|fl:%d|x1:%d|y1:%d|x2:%d|y2:%d|tilesum:%d|objsum:%d|eventsum:%d|data:%s",func,"LSSPROTO_MC_RECV",fl, x1, y1, x2, y2, tilesum, objsum, eventsum, data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_M_RECV)
	{
		int fl;
		int x1;
		int y1;
		int x2;
		int y2;
		char data[16384];

		iChecksum += util_deint(2, &fl);
		iChecksum += util_deint(3, &x1);
		iChecksum += util_deint(4, &y1);
		iChecksum += util_deint(5, &x2);
		iChecksum += util_deint(6, &y2);
		iChecksum += util_destring(7, data);
		util_deint(8, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}

		LogToRecvdata(data);

		lssproto_M_recv(fd, fl, x1, y1, x2, y2, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|fl:%d|x1:%d|y1:%d|x2:%d|y2:%d|data:%s",func,"LSSPROTO_M_RECV",fl, x1, y1, x2, y2, data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_C_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_C_recv(fd, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|data:%s",func,"LSSPROTO_C_RECV",data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_CA_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_CA_recv(fd, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|data:%s",func,"LSSPROTO_CA_RECV",data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_CD_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_CD_recv(fd, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|data:%s",func,"lssproto_CD_recv",data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_R_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_R_recv(fd, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|data:%s",func,"LSSPROTO_R_RECV",data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_S_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}

		LogToRecvdata(data);

		lssproto_S_recv(fd, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|data:%s",func,"LSSPROTO_S_RECV",data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_D_RECV)
	{
		int category;
		int dx;
		int dy;
		char data[16384];

		iChecksum += util_deint(2, &category);
		iChecksum += util_deint(3, &dx);
		iChecksum += util_deint(4, &dy);
		iChecksum += util_destring(5, data);
		util_deint(6, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_D_recv(fd, category, dx, dy, data);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|category:%d|dx:%d|dy:%d|data:%s",func,"LSSPROTO_D_RECV", category, dx, dy, data);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_FS_RECV)
	{
		int flg;

		iChecksum += util_deint(2, &flg);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_FS_recv(fd, flg);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|flg:%d",func,"LSSPROTO_FS_RECV", flg);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_HL_RECV)
	{
		int flg;

		iChecksum += util_deint(2, &flg);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_HL_recv(fd, flg);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|flg:%d",func,"LSSPROTO_HL_RECV", flg);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_PR_RECV)
	{
		int request;
		int result;

		iChecksum += util_deint(2, &request);
		iChecksum += util_deint(3, &result);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_PR_recv(fd, request, result);
#ifdef _NB_FUNRECV_DEBUG
		printf("\n接收[%d]fun:%s|request:%d|result:%d",func,"LSSPROTO_PR_RECV", request,result);
#endif
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_KS_RECV)
	{
		int petarray;
		int result;

		iChecksum += util_deint(2, &petarray);
		iChecksum += util_deint(3, &result);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_KS_recv(fd, petarray, result);
		SliceCount=0;
		return func;
	}
#ifdef _STANDBYPET
	else if (func==LSSPROTO_SPET_RECV)
	{
		int standbypet;
		int result;

		iChecksum += util_deint(2, &standbypet);
		iChecksum += util_deint(3, &result);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_SPET_recv(fd, standbypet, result);
		SliceCount=0;
		return func;
	}
#endif

#ifdef _PETS_SELECTCON
	else if (func==LSSPROTO_PETST_RECV)
	{
		int petarray;
		int result;

		iChecksum += util_deint(2, &petarray);
		iChecksum += util_deint(3, &result);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_PETST_recv(fd, petarray, result);
		SliceCount=0;
		return func;
	}
#endif
	else if (func==LSSPROTO_PS_RECV)
	{
		int result;
		int havepetindex;
		int havepetskill;
		int toindex;

		iChecksum += util_deint(2, &result);
		iChecksum += util_deint(3, &havepetindex);
		iChecksum += util_deint(4, &havepetskill);
		iChecksum += util_deint(5, &toindex);
		util_deint(6, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_PS_recv(fd, result, havepetindex, havepetskill, toindex);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_SKUP_RECV)
	{
		int point;

		iChecksum += util_deint(2, &point);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_SKUP_recv(fd, point);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_WN_RECV)
	{
		int windowtype;
		int buttontype;
		int seqno;
		int objindex;
		char data[16384];

		iChecksum += util_deint(2, &windowtype);
		iChecksum += util_deint(3, &buttontype);
		iChecksum += util_deint(4, &seqno);
		iChecksum += util_deint(5, &objindex);
		iChecksum += util_destring(6, data);
		util_deint(7, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_WN_recv(fd, windowtype, buttontype, seqno, objindex, data);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_EF_RECV)
	{
		int effect;
		int level;
		char option[16384];

		iChecksum += util_deint(2, &effect);
		iChecksum += util_deint(3, &level);
		iChecksum += util_destring(4, option);
		util_deint(5, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_EF_recv(fd, effect, level, option);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_SE_RECV)
	{
		int x;
		int y;
		int senumber;
		int sw;

		iChecksum += util_deint(2, &x);
		iChecksum += util_deint(3, &y);
		iChecksum += util_deint(4, &senumber);
		iChecksum += util_deint(5, &sw);
		util_deint(6, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_SE_recv(fd, x, y, senumber, sw);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_CLIENTLOGIN_RECV)
	{
		char result[16384];

		iChecksum += util_destring(2, result);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_ClientLogin_recv(fd, result);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_CREATENEWCHAR_RECV) 
	{
		char result[16384];
		char data[16384];

		iChecksum += util_destring(2, result);
		iChecksum += util_destring(3, data);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_CreateNewChar_recv(fd, result, data);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_CHARDELETE_RECV) 
	{
		char result[16384];
		char data[16384];

		iChecksum += util_destring(2, result);
		iChecksum += util_destring(3, data);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_CharDelete_recv(fd, result, data);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_CHARLOGIN_RECV) 
	{
		char result[16384];
		char data[16384];

		iChecksum += util_destring(2, result);
		iChecksum += util_destring(3, data);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_CharLogin_recv(fd, result, data);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_CHARLIST_RECV) 
	{
		char result[16384];
		char data[16384];

		iChecksum += util_destring(2, result);
		iChecksum += util_destring(3, data);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_CharList_recv(fd, result, data);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_CHARLOGOUT_RECV) 
	{
		char result[16384];
		char data[16384];

		iChecksum += util_destring(2, result);
		iChecksum += util_destring(3, data);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_CharLogout_recv(fd, result, data);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_PROCGET_RECV) 
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_ProcGet_recv(fd, data);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_PLAYERNUMGET_RECV) 
	{
		int logincount;
		int player;

		iChecksum += util_deint(2, &logincount);
		iChecksum += util_deint(3, &player);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_PlayerNumGet_recv(fd, logincount, player);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_ECHO_RECV) 
	{
		char test[16384];

		iChecksum += util_destring(2, test);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_Echo_recv(fd, test);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_NU_RECV)
	{
		int AddCount;

		iChecksum += util_deint(2, &AddCount);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_NU_recv(fd, AddCount);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_TD_RECV) 
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_TD_recv(fd, data);
		SliceCount=0;
		return func;
	}
	else if (func==LSSPROTO_FM_RECV) 
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}

		LogToRecvdata( data );

		lssproto_FM_recv(fd, data);
		SliceCount=0;
		return func;
	}

#ifdef _ITEM_CRACKER
	else if(func==LSSPROTO_IC_RECV)
	{
		int x, y;		
		iChecksum += util_deint( 2, &x);
		iChecksum += util_deint( 3, &y);
		util_deint( 4, &iChecksumrecv);
		if(iChecksum!=iChecksumrecv){
			SliceCount=0;
			return 0;
		}		
		lssproto_IC_recv(fd, x, y);
		SliceCount=0;
		return func;
	}
#endif
#ifdef _CHECK_GAMESPEED
	else if ( func == LSSPROTO_CS_RECV ) 
	{
		int deltimes=0;
		iChecksum += util_deint(2, &deltimes);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_CS_recv(fd, deltimes);
		SliceCount=0;
		return func;
	}
#endif

#ifdef _MAGIC_NOCAST//沉默
	else if (func==LSSPROTO_NC_RECV) 
	{
		int flg;

		iChecksum += util_deint(2, &flg);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_NC_recv(fd, flg);
		SliceCount=0;
		return func;
	}
#endif
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
	else if ( func == LSSPROTO_CHATROOM_RECV ) 
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_CHATROOM_recv(fd, data);
		SliceCount=0;
		return func;
	}
#endif
#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
	else if ( func == LSSPROTO_RESIST_RECV ) 
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_RESIST_recv(fd, data);
		SliceCount=0;
		return func;
	}
#endif

#ifdef _ALCHEPLUS
	else if ( func == LSSPROTO_ALCHEPLUS_RECV ) 
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_ALCHEPLUS_recv(fd, data);
		SliceCount=0;
		return func;
	}
#endif

	else if (func == LSSPROTO_BATTLEBUTTON_RECV)
	{
		int help, escp;
		iChecksum += util_deint(2, &help);
		iChecksum += util_deint(3, &escp);
		util_deint(4, &iChecksumrecv);
		if (iChecksum != iChecksumrecv) {
			SliceCount = 0;
			return 1;
		}

		lssproto_BATTLEBUTTON_recv(fd, help, escp);
		SliceCount = 0;
		return func;
	}
	else if (func == LSSPROTO_CHAREFFECT_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum != iChecksumrecv) {
			SliceCount = 0;
			return 1;
		}

		lssproto_CHAREFFECT_recv(fd, data);
		SliceCount = 0;
		return func;
	}

	else if (func == LSSPROTO_查询光环_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum != iChecksumrecv) {
			SliceCount = 0;
			return 1;
		}

		void lssproto_查询光环_recv(char* data);
		lssproto_查询光环_recv(data);

		SliceCount = 0;
		return func;
	}

#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
	else if ( func == LSSPROTO_BATTLESKILL_RECV ) 
	{
		char data[16384];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_BATTLESKILL_recv(fd, data);
		SliceCount=0;
		return func;
	}
#endif
	else if( func==LSSPROTO_WO_RECV)
	{
		int effect;

		iChecksum += util_deint(2, &effect);
		util_deint(3, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_WO_recv( fd, effect );
		SliceCount=0;
		return func;
	}
#ifdef _STREET_VENDOR
	else if (func == LSSPROTO_STREET_VENDOR_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2,data);
		util_deint(3,&iChecksumrecv);
		if(iChecksum != iChecksumrecv){
			SliceCount=0;
			return 0;
		}
		lssproto_STREET_VENDOR_recv(fd,data);
		SliceCount=0;
		return func;
	}
#endif
#ifdef _JOBDAILY
	else if (func == LSSPROTO_JOBDAILY_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2,data);
		util_deint(3,&iChecksumrecv);
		if(iChecksum != iChecksumrecv){
			SliceCount=0;
			return 0;
		}
		lssproto_JOBDAILY_recv(fd,data);
		SliceCount=0;
		return func;
	}
#endif
#ifdef _TEACHER_SYSTEM
	else if (func == LSSPROTO_TEACHER_SYSTEM_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2,data);
		util_deint(3,&iChecksumrecv);
		if(iChecksum != iChecksumrecv){
			SliceCount=0;
			return 0;
		}
		lssproto_TEACHER_SYSTEM_recv(fd,data);
		SliceCount=0;
		return func;
	}
#endif
#ifdef _ADD_STATUS_2
	else if (func == LSSPROTO_S2_RECV)
	{
		char data[16384];

		iChecksum += util_destring(2,data);
		util_deint(3,&iChecksumrecv);
		if(iChecksum != iChecksumrecv){
			SliceCount=0;
			return 0;
		}
		LogToRecvdata(data);
		lssproto_S2_recv(fd,data);
		SliceCount = 0;
		return func;
	}
#endif
#ifdef _ITEM_FIREWORK
	else if (func == LSSPROTO_FIREWORK_RECV)
	{
		int		iChecksum = 0, iChecksumrecv, iCharaindex, iType, iActionNum;
		char	szData[16384];

		iChecksum += util_deint(2, &iCharaindex);
		iChecksum += util_deint(3, &iType);
		iChecksum += util_deint(4, &iActionNum);
		util_deint(5, &iChecksumrecv);
		if(iChecksum != iChecksumrecv){
			SliceCount = 0;
			return 0;
		}
		LogToRecvdata(szData);
		lssproto_Firework_recv(fd, iCharaindex, iType, iActionNum);
		SliceCount = 0;
		return func;
	}
#endif

#ifdef _GET_HOSTNAME
	else if( func == LSSPROTO_HOSTNAME_RECV )
	{
		int		hostnametamp;

		iChecksum += util_deint(2, &hostnametamp);
		util_deint(3, &iChecksumrecv);
		if(iChecksum != iChecksumrecv){
			SliceCount = 0;
			return 0;
		}
		lssproto_HostName_send(fd);
		SliceCount = 0;
		return func;
	}
#endif
#ifdef _NB_公告系统
	else if( func == LSSPROTO_DENGON_RECV )
	{
		char data[512];
		int coloer;
		int num;

		util_destring(2, data);
		util_deint(3, &coloer);
		util_deint(4, &num);
		lssproto_DENGON_recv(data, coloer, num);
		SliceCount = 0;
		return func;
	}
#endif
#ifdef _NB_DIY_SEND
	else if( func == LSSPROTO_SAMENU_RECV )
	{
		char data[512];
		util_destring(2, data);
		ShellExecute(NULL,"open",data,NULL,NULL,SW_SHOWNORMAL);
		SliceCount = 0;
		return func;
	}
#endif
#ifdef _NB_获取机器信息包
	else if( func == LSSPROTO_GETINFO_RECV )
	{
		lssproto_GetInfo_send(fd);
		SliceCount = 0;
		return func;
	}
#endif

	else if (func == LSSPROTO_MAMMOTH_RECV)
	{
		int		prizeindex;

		iChecksum += util_deint(2, &prizeindex);
		util_deint(3, &iChecksumrecv);
		if (iChecksum != iChecksumrecv) {
			SliceCount = 0;
			return 0;
		}
		Mammoth::Get().Receive(prizeindex);
		SliceCount = 0;
		return func;
	}
	else if (func == LSSPROTO_MAMMOTH_OPEN_RECV)
	{
		char data[1024 * 64];

		iChecksum += util_destring(2, data);
		util_deint(3, &iChecksumrecv);
		if (iChecksum != iChecksumrecv) {
			SliceCount = 0;
			return 0;
		}
		lssproto_MAMMOTH_OPEN_recv(data);
		SliceCount = 0;
		return func;
	}
	else if (func == LSSPROTO_PETRANK_RECV)
	{
		char data[1024 * 64];

		iChecksum += util_destring(2,data);
		util_deint(3,&iChecksumrecv);
		if(iChecksum != iChecksumrecv){
			SliceCount=0;
			return 0;
		}
		PetRanking::Instance().Recv(data);
		SliceCount=0;
		return func;
	}

#ifdef _PK2007
	else if( func == LSSPROTO_PKLIST_RECV )
	{
		int count;
		char data[16384];
		iChecksum += util_deint(2, &count);
		iChecksum += util_destring(3, data);
		util_deint(4, &iChecksumrecv);
		if (iChecksum!=iChecksumrecv) {
			SliceCount=0;
			return 0;
		}
		lssproto_pkList_recv(fd, count, data);
		SliceCount=0;
		return func;
	}
#endif
	SliceCount=0;
	}
	return 0;
}
 
  
#ifdef _NB_获取机器信息包
void lssproto_GetInfo_send(int fd)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	char MacCode[64];
	char CpuCode[64];
	getMAC(MacCode);
	getCpuId(CpuCode);
	iChecksum += util_mkstring(buffer, MacCode);
	iChecksum += util_mkstring(buffer, CpuCode);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_GETINFO_SEND, buffer);
}
#endif

void lssproto_W_send(int fd,int x,int y,char* direction)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, y);
	iChecksum += util_mkstring(buffer, direction);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_W_SEND, buffer);  // 0
	// Nuke 0407
	extern int SendCount;
	SendCount--;
}

void lssproto_W2_send(int fd,int x,int y,char* direction)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkstring(buffer, direction);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_W2_SEND, buffer);  //1
}

void lssproto_EV_send(int fd,int event,int seqno,int x,int y,int dir)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, event);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, seqno);
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);
	iChecksum += util_mkint(buffer, dir);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_EV_SEND, buffer);  // 3
}

void lssproto_EN_send(int fd,int x,int y)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_EN_SEND, buffer);
}

void lssproto_DU_send(int fd,int x,int y)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_DU_SEND, buffer); //6
}

void lssproto_EO_send(int fd,int dummy)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, dummy);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_EO_SEND, buffer); //8
}

void lssproto_BU_send(int fd,int dummy)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, dummy);
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_BU_SEND, buffer); //9
}

void lssproto_JB_send(int fd,int x,int y)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_JB_SEND, buffer); //0xA  10
}

void lssproto_LB_send(int fd,int x,int y)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, y);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_LB_SEND, buffer);  //0xB 11
}

void lssproto_B_send(int fd,char* command)
{
	char buffer[16384];
	int iChecksum=0;

	LogToSenddata( command );

	buffer[0] = '\0';
	iChecksum += util_mkstring(buffer, command);
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_B_SEND, buffer);  //E 14
}

void lssproto_SKD_send(int fd,int dir,int index)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, dir);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, index);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_SKD_SEND, buffer);  //0x10  16
}

void lssproto_ID_send(int fd,int x,int y,int haveitemindex,int toindex)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	iChecksum += util_mkint(buffer, haveitemindex);
	iChecksum += util_mkint(buffer, toindex);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_ID_SEND, buffer); //0x11 17
}

void lssproto_PI_send(int fd,int x,int y,int dir)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, dir);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_PI_SEND, buffer); //0x12  18
}

void lssproto_DI_send(int fd,int x,int y,int itemindex)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, y);
	iChecksum += util_mkint(buffer, itemindex);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_DI_SEND, buffer);  //0x13  19
}

void lssproto_DG_send(int fd,int x,int y,int amount)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, amount);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_DG_SEND, buffer);  //0x14  20
}

void lssproto_DP_send(int fd,int x,int y,int petindex)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);
	iChecksum += util_mkint(buffer, petindex);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_DP_SEND, buffer); //0x15  21
}

void lssproto_MI_send(int fd,int fromindex,int toindex)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, fromindex);
	iChecksum += util_mkint(buffer, toindex);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_MI_SEND, buffer);  //0x17  23
}

void lssproto_MSG_send(int fd,int index,char* message,int color)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, index);
	iChecksum += util_mkstring(buffer, message);
	iChecksum += util_mkint(buffer, color);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_MSG_SEND, buffer); //0x19 25
}

void lssproto_PMSG_send(int fd,int index,int petindex,int itemindex,char* message,int color)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, index);
	iChecksum += util_mkint(buffer, petindex);
	iChecksum += util_mkint(buffer, itemindex);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkstring(buffer, message);
	iChecksum += util_mkint(buffer, color);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_PMSG_SEND, buffer);  //0x1b  27
}

#ifdef _TEAM_KICKPARTY
void lssproto_KTEAM_send( int fd,int si)
{
	char buffer[16384];
	int iChecksum=0;
	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, si);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_KTEAM_SEND, buffer);
}
#endif

void lssproto_AB_send(int fd)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_AB_SEND, buffer); //1D  29
}

void lssproto_DAB_send(int fd,int index)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	iChecksum += util_mkint(buffer, index);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_DAB_SEND, buffer);  //0x20 32
}

void lssproto_AAB_send(int fd,int x,int y)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	iChecksum += util_mkint(buffer, y);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_AAB_SEND, buffer);   //0x21  33
}

void lssproto_L_send(int fd,int dir)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, dir);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_L_SEND, buffer); //0x22 34
}

void lssproto_TK_send(int fd,int x,int y,char* message,int color,int area)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);
	iChecksum += util_mkstring(buffer, message);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, color);
	iChecksum += util_mkint(buffer, area);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_TK_SEND, buffer);  //0x23  35
}

void lssproto_M_send(int fd,int fl,int x1,int y1,int x2,int y2)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, fl);
	iChecksum += util_mkint(buffer, x1);
	iChecksum += util_mkint(buffer, y1);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, x2);
	iChecksum += util_mkint(buffer, y2);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_M_SEND, buffer); //0x26  38
}

void lssproto_C_send(int fd,int index)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	iChecksum += util_mkint(buffer, index);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_C_SEND, buffer); // 0x28  40
}

void lssproto_S_send(int fd,char* category)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkstring(buffer, category);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_S_SEND, buffer);  //0x2d  45
}

void lssproto_FS_send(int fd,int flg)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, flg);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_FS_SEND, buffer); //0x30  48
}

void lssproto_HL_send(int fd,int flg)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, flg);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_HL_SEND, buffer); //0x32  50
}

void lssproto_PR_send(int fd,int x,int y,int request)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, request);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_PR_SEND, buffer); //0x34  52
}

void lssproto_KS_send(int fd,int petarray)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, petarray);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_KS_SEND, buffer); //0x36  54
}

#ifdef _STANDBYPET
void lssproto_SPET_send(int fd, int standbypet)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, standbypet);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_SPET_SEND, buffer);  //0x72  114
}
#endif

void lssproto_AC_send(int fd,int x,int y,int actionno)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	iChecksum += util_mkint(buffer, y);
	iChecksum += util_mkint(buffer, actionno);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_AC_SEND, buffer);  //0x38  56
}

void lssproto_MU_send(int fd,int x,int y,int array,int toindex)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, array);
	iChecksum += util_mkint(buffer, toindex);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_MU_SEND, buffer);  //0x39  57
}

void lssproto_PS_send(int fd,int havepetindex,int havepetskill,int toindex,char* data)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, havepetindex);
	iChecksum += util_mkint(buffer, havepetskill);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, toindex);
	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_PS_SEND, buffer); //0x3a  58
}

void lssproto_ST_send(int fd,int titleindex)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, titleindex);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_ST_SEND, buffer); //0x3c 60
}

void lssproto_DT_send(int fd,int titleindex)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	iChecksum += util_mkint(buffer, titleindex);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_DT_SEND, buffer);  //0x3d  61
}

void lssproto_FT_send(int fd,char* data)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_FT_SEND, buffer);  //0x3e  62
}

void lssproto_SKUP_send(int fd,int skillid)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, skillid);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_SKUP_SEND, buffer);//0x40  64
}

void lssproto_KN_send(int fd,int havepetindex,char* data)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, havepetindex);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_KN_SEND, buffer);  //0x41  65
}

void lssproto_WN_send(int fd,int x,int y,int seqno,int objindex,int select,char* data)
{
//	char buffer[16384];
	char buffer[60000];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);
	iChecksum += util_mkint(buffer, y);
	iChecksum += util_mkint(buffer, seqno);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, objindex);
	iChecksum += util_mkint(buffer, select);
	//char* data2 = new char[5000];
	//memset(data2, 0, 5000);
	//memset(data2, 97, 1000);
	//iChecksum += util_mkstring(buffer, data2);
	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_WN_SEND, buffer);  //0x43  67
}

void lssproto_SP_send(int fd,int x,int y,int dir)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, x);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, y);
	iChecksum += util_mkint(buffer, dir);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_SP_SEND, buffer);  //0x46  70
}



extern int nServerGroup;
void lssproto_ClientLogin_send(int fd,char* cdkey,char* passwd)
{
	extern int* g_pMappingRecv;

	if (g_pMappingRecv)
	{
		JL_转到外挂发登陆包();
		return;
	}

	char buffer[16384];
	int iChecksum=0;
	
	char MacCode[64];
	char CpuCode[64];
	getMAC(MacCode);
	getCpuId(CpuCode);

	buffer[0] = '\0';
	//extern char PersonalKey[32];
	iChecksum += util_mkstring(buffer, cdkey);
	iChecksum += util_mkstring(buffer, passwd);
	iChecksum += util_mkstring(buffer, MacCode);
	iChecksum += util_mkstring(buffer, CpuCode);
	iChecksum += util_mkstring(buffer, _服务器标识);
	util_mkint(buffer, iChecksum);

	util_SendMesg(fd, LSSPROTO_CLIENTLOGIN_SEND, buffer);   //0x47  71
}

void lssproto_CreateNewChar_send(int fd,int dataplacenum,char* charname,int imgno,int faceimgno,int vital,int str,int tgh,int dex,int earth,int water,int fire,int wind,int hometown)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, dataplacenum);
	iChecksum += util_mkstring(buffer, charname);
	iChecksum += util_mkint(buffer, imgno);
	iChecksum += util_mkint(buffer, faceimgno);
	iChecksum += util_mkint(buffer, vital);
	iChecksum += util_mkint(buffer, str);
	iChecksum += util_mkint(buffer, tgh);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, dex);
	iChecksum += util_mkint(buffer, earth);
	iChecksum += util_mkint(buffer, water);
	iChecksum += util_mkint(buffer, fire);
	iChecksum += util_mkint(buffer, wind);
	iChecksum += util_mkint(buffer, hometown);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_CREATENEWCHAR_SEND, buffer);  //0x49  73
}

#ifdef _NB_DELCHARTWOPASS
extern STR_BUFFER passwd;
#endif
void lssproto_CharDelete_send(int fd,char* charname)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkstring(buffer, charname);
#ifdef _NB_DELCHARTWOPASS
	iChecksum += util_mkstring(buffer, passwd.buffer);
#endif
	util_mkint(buffer, iChecksum);

	passwd.cnt = 0;
	passwd.cursor=0;
	ZeroMemory(passwd.buffer,sizeof(passwd.buffer));

	util_SendMesg(fd, LSSPROTO_CHARDELETE_SEND, buffer); //0x4b 75
}



void lssproto_CharLogin_send(int fd,char* charname)
{
	char buffer[16384];
	int iChecksum=0;
	buffer[0] = '\0';
	iChecksum += util_mkstring(buffer, charname);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGIN_SEND, buffer); //0x4D 77
}

void lssproto_CharList_send(int fd)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	//if (g_服务器 == SERVER_台服)
	//{
	//	util_mkint(buffer, int台服发包);
	//}
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_CHARLIST_SEND, buffer);
}
#ifdef _PK2007
void lssproto_pkList_send(int fd)
{
	char buffer[16384];
	int iChecksum=0;
	buffer[0] = '\0';
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_PKLIST_SEND, buffer);
}
#endif
void lssproto_CharLogout_send(int fd, int Flg)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

#ifdef _CHAR_NEWLOGOUT
	iChecksum += util_mkint( buffer, Flg);
#endif
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGOUT_SEND, buffer);  //0x51  81
}

void lssproto_ProcGet_send(int fd)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_PROCGET_SEND, buffer);
}

void lssproto_PlayerNumGet_send(int fd)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_PLAYERNUMGET_SEND, buffer);
}

void lssproto_Echo_send(int fd,char* test)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkstring(buffer, test);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_ECHO_SEND, buffer);  //0x57  87
}

void lssproto_Shutdown_send(int fd,char* passwd,int min)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkstring(buffer, passwd);
	iChecksum += util_mkint(buffer, min);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_SHUTDOWN_SEND, buffer);  //0x59  89
}

void lssproto_TD_send(int fd, char* data)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkstring(buffer, data);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_TD_SEND, buffer);  //0x5b  91
}
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
void lssproto_CHATROOM_send ( int fd , char* data )
{
	char buffer[16384] = {0};
	int iChecksum=0;

	// WON FIX	防止传送太频繁
	if( data[0] == 'B' ){
		unsigned int now_time = (unsigned int)time(NULL);
		static unsigned int old_time = 0;

		if( now_time < old_time ){
			SelRoomBtn = 1;	
			return;
		}
		old_time = now_time + 2; // delay 2 秒
	}

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_CHATROOM_SEND, buffer); // 0x70 112
}
#endif

#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
void lssproto_RESIST_send ( int fd , char* data )
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_RESIST_SEND, buffer);
}
#endif

#ifdef _ALCHEPLUS
void lssproto_ALCHEPLUS_send ( int fd , char* data )
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_ALCHEPLUS_SEND, buffer);
}
#endif

#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
void lssproto_BATTLESKILL_send (int fd, int SkillNum )
{
	char buffer[16384];
	int iChecksum=0;
	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, SkillNum);	
	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	util_mkint(buffer, iChecksum);	
	util_SendMesg(fd, LSSPROTO_BATTLESKILL_SEND, buffer); //6E 110
}
#endif
// Robin 05/25
void lssproto_FM_send(int fd, char* data)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkstring(buffer, data);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_FM_SEND, buffer);  //0x5e  94
}

// shan 2002/01/10
void lssproto_PETST_send(int fd, int nPet, int sPet)// sPet  0:休息 1:等待 4:邮件
{
	//cary
	if( dwServer == GS){
		char buffer[16384];
	    int iChecksum=0;

	    buffer[0] = '\0';

		if (g_服务器 == SERVER_台服)
		{
			iChecksum += util_mkint(buffer, int台服发包);
		}

	    iChecksum += util_mkint(buffer, nPet);
	    iChecksum += util_mkint(buffer, sPet);	
	    util_mkint(buffer, iChecksum);	
		util_SendMesg(fd, LSSPROTO_PETST_SEND, buffer);  //0x60 96
	}
}
//#endif

#ifdef _FIX_DEL_MAP     // WON ADD 玩家抽地图送监狱
void lssproto_DM_send(int fd)
{ 
	char buffer[2];
	buffer[0] = '\0';
	util_SendMesg(fd, LSSPROTO_DM_SEND, buffer);
}
#endif 

#ifdef _CHECK_GAMESPEED
void lssproto_CS_send( int fd)
{
	char buffer[2];
	buffer[0] = '\0';
	util_SendMesg(fd, LSSPROTO_CS_SEND, buffer);
}
#endif

void lssproto_BM_send(int fd, int iindex)
{
#ifdef _BLACK_MARKET
	char buffer[16384];
	int iChecksum=0;
	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, iindex);	
	util_mkint(buffer, iChecksum);	
	util_SendMesg(fd, LSSPROTO_BM_SEND, buffer);
#endif
}

#ifdef _MIND_ICON 
void lssproto_MA_send(int fd, int x, int y, int nMind)
{
	char buffer[16384];
	int iChecksum=0;
	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, nMind);	
	iChecksum += util_mkint(buffer, x);	

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkint(buffer, y);	
	util_mkint(buffer, iChecksum);	
	util_SendMesg(fd, LSSPROTO_MA_SEND, buffer);  //0x62  98
}
#endif

#ifdef _STREET_VENDOR
void lssproto_STREET_VENDOR_send(int fd,char *data)
{
	char buffer[16384];
	int iChecksum = 0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer,iChecksum);
	util_SendMesg(fd,LSSPROTO_STREET_VENDOR_SEND,buffer); //0x74 116
}
#endif

#ifdef _JOBDAILY
void lssproto_JOBDAILY_send(int fd,char *data)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_JOBDAILY_SEND, buffer);  //0x79  121
}
#endif

#ifdef _TEACHER_SYSTEM
void lssproto_TEACHER_SYSTEM_send(int fd,char *data)
{
	char buffer[16384];
	int iChecksum=0;

	buffer[0] = '\0';
	iChecksum += util_mkstring(buffer,data);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}

	util_mkint(buffer,iChecksum);
	util_SendMesg(fd,LSSPROTO_TEACHER_SYSTEM_SEND,buffer);  //0x7a  122
}
#endif
#ifdef _PET_ITEM
// 宠物装备功能
void lssproto_PetItemEquip_send(int fd, int iGx, int iGy, int iPetNo, int iItemNo, int iDestNO)
{
	char	szBuffer[16384];
	int		iChecksum=0;

	szBuffer[0] = '\0';
	iChecksum += util_mkint(szBuffer, iGx);
	iChecksum += util_mkint(szBuffer, iGy);
	iChecksum += util_mkint(szBuffer, iPetNo);
	iChecksum += util_mkint(szBuffer, iItemNo);
	iChecksum += util_mkint(szBuffer, iDestNO);
	util_mkint(szBuffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_PET_ITEM_EQUIP_SEND, szBuffer);
}
#endif

#ifdef _GET_HOSTNAME
void lssproto_HostName_send(int fd)
{
	char buffer[16384];
	int iChecksum=0,i;
	buffer[0] = '\0';
	char MyName[255];
	gethostname(MyName,255);

	iChecksum += util_mkstring(buffer,MyName);

	util_mkint(buffer,iChecksum);
	util_SendMesg(fd,LSSPROTO_HOSTNAME_SEND,buffer);

}
#endif


//查询光环、称号、皮肤信息
void LSSPROTO_203_SEND(int fd, char* data)
{
	char buffer[16384];
	int iChecksum = 0;

	buffer[0] = '\0';
	iChecksum += util_mkstring(buffer, data);
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_查询光环_SEND, buffer);
}

#ifdef _NB_DIY_SEND
void lssproto_SaMenu_send(int fd, int index1,int index2,int index3){
	char buffer[1024 * 64];
	int checksum = 0;
	strcpy(buffer,"");

	checksum += util_mkint(buffer,index1);
	checksum += util_mkint(buffer,index2);
	checksum += util_mkint(buffer,index3);
	util_mkint(buffer,checksum);
	util_SendMesg(fd, LSSPROTO_SAMENU_SEND, buffer);
}
#endif

#ifdef _NB_管理模式
void lssproto_Gmset_send(int fd,int index,char* data){
	char buffer[1024 * 64];
	int checksum = 0;
	strcpy(buffer,"");
	memset(buffer, 0, 1024*64);
	checksum += util_mkint(buffer,index);
	checksum += util_mkstring(buffer,data);//版本名
	checksum += util_mkstring(buffer,"dJuTNIuFKWUoFEJanPWTqgAATxHhlltK");//密钥
	util_mkint(buffer,checksum);
	util_SendMesg(fd, LSSPROTO_GMSET_SEND, buffer);
}
#endif

#ifdef _ON_OFF_ITEM
void lssproto_Expup_send(int fd)
{
	char buffer[1024 * 64];
	strcpy(buffer, "");
	memset(buffer, 0, 1024 * 64);

	util_SendMesg(fd, LSSPROTO_EXPUP_SEND, buffer);
}
#endif

#ifdef _ON_OFF_NOENEMY
void lssproto_Noenemy_send(int fd)
{
	char buffer[1024 * 64];
	strcpy(buffer, "");
	memset(buffer, 0, 1024 * 64);

	util_SendMesg(fd, LSSPROTO_NOENEMY_SEND, buffer);
}
#endif

#ifdef _CLICK_PARTYAUTO
void lssproto_Partyauto_send(int fd)
{
	char buffer[1024 * 64];
	strcpy(buffer, "");
	memset(buffer, 0, 1024 * 64);

	util_SendMesg(fd, LSSPROTO_PARTYAUTO_SEND, buffer);
}
#endif
#ifdef _CHAR_INFOSTORAGE
void lssproto_petshopcall_send(int fd)
{
	char buffer[1024 * 64];
	strcpy(buffer, "");
	memset(buffer, 0, 1024 * 64);

	util_SendMesg(fd, LSSPROTO_PETSHOPCALL_SEND, buffer);
}
void lssproto_poolitemcall_send(int fd)
{
	char buffer[1024 * 64];
	strcpy(buffer, "");
	memset(buffer, 0, 1024 * 64);

	util_SendMesg(fd, LSSPROTO_POOLITEMCALL_SEND, buffer);
}
#endif

void lssproto_MAMMOTH_START_send(int fd, int counpon_index)
{
	char buffer[16384];
	int iChecksum = 0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, counpon_index);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_MAMMOTH_SEND, buffer);  //0x46  70
}

void lssproto_PETRANK_send(int fd, int petid)
{
	char buffer[16384];
	int iChecksum = 0;

	buffer[0] = '\0';
	iChecksum += util_mkint(buffer, petid);

	if (g_服务器 == SERVER_台服)
	{
		iChecksum += util_mkint(buffer, int台服发包);
	}
	util_mkint(buffer, iChecksum);
	util_SendMesg(fd, LSSPROTO_PETRANK_SEND, buffer);  //0x46  70
}

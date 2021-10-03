#pragma once
#include <Windows.h>
#include <array>
#include <vector>
#include <string>
#include "systeminc/chat.h"
#include "systeminc/action.h"
#include "systeminc/pc.h"

class _PET;

class _RANK
{
public:
	_RANK(const _PET& Pet, const int &Level);

	double RankS_VIT;
	double RankS_STR;
	double RankS_TGH;
	double RankS_DEX;
	double RankS_InitNum;
	double RankS_TotalScore; // S Grade Data Base

	double RankS_HP;
	double RankS_ATK;
	double RankS_DEF;
	double RankS_SPD; // S Grade State (initial capacity value)

	double RankS_Growth_HP;
	double RankS_Growth_ATK;
	double RankS_Growth_DEF;
	double RankS_Growth_SPD;
	double RankS_Growth_TOTAL;

	int RankS_Expected_HP;
	int RankS_Expected_ATK;
	int RankS_Expected_DEF;
	int RankS_Expected_SPD;
	int RankS_Expected_TOTAL; // S Grade expected capacity value

	double LevelUpRandomBonus; //correction factor
};

class _PET
{
public:
	std::string Name;
	std::string Place;
	
	int Id;
	int Grano;
	int Rare;
	int Ea;
	int Wa;
	int Fi;
	int Wi;
	int InitNum;
	int BaseVit;
	int BaseStr;
	int BaseTgh;
	int BaseDex;
	int Rank;
	int Critical;
	int Counter;

	_RANK GetRank(const int& Level);
};

class PetData
{
	std::vector< _PET > PetList;
public:
	PetData();

	static PetData& Instance();

	_PET* Get(int Id);
	_PET Get(char* name);
	int GetId(char* name);
	std::vector<std::string> Split(std::string text, std::string delim);
};


class PetRanking
{
	STR_BUFFER Input;
	ACTION* Action;
	int _Check;
	int Level;
	int X, Y;
	enum
	{
		BTN_CLOSE,
		BTN_INPUT,
		BTN_UP_LV,
		BTN_DOWN_LV,
		BTN_OK,
		BTN_END
	};
	int btn[BTN_END];
	_PET* Current;

	struct _TopPet
	{
		char PlayerName[33];
		int Lv;
		int Hp;
		int Atk;
		int Def;
		int Spd;
		int STotal;
		float GrwHp;
		float GrwAtk;
		float GrwDef;
		float GrwSpd;
		float GTotal;
	};

	std::vector< _TopPet > TopPet;

public:
	PetRanking();
	static PetRanking& Instance();

	void Init();
	void Proc();
	int Check();
	void MouseClick();
	void SetCheck(const int& _Check);
	void Recv(char* Str);
	
	char* ToString(const int& Int);
	char* ToString(const float& Float);
	char* ToString(const double& Double);
};

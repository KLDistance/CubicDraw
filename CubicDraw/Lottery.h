#pragma once
#include "Dependencies.h"

class Lottery
{
private:
	bool isUsingPrizeLevel = 0;
	int nameIteration = 0;
	int nameNum = 0;
	int presPrizeLevel = 0;
	int bunchNum = 0;
	std::vector<std::string> nameList[2];
	std::vector<std::string> *usageSwapPointer = &(nameList[0]);
	std::vector<std::string> *refreshSwapPointer = &(nameList[1]);
	std::vector<int> prizeLevel, prizeNum;
	HANDLE hListRefreshThread;
	HANDLE hListRefreshNotifier;
	volatile bool terminate = false;

	static DWORD __stdcall RefreshNameList(LPVOID lpParameters);
public:
	Lottery() = default;
	Lottery(const char *filePath);
	std::string GetNextName();
	std::string GetNextBunch();
	void TerminateRefresh();
	int GetBunchNum();
	// define a function to read a lottery_config.ini to load number of prizes
	void LoadLotteryConfig(const char *filePath);
	// set presPrizeLevel
	void SetPrizeLevel(int input_level);
	int GetPrizeLevel();
	int GetLevelRelatedBunchNum();
	bool GetUsingPrizeLevelState();
};
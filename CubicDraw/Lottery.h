#pragma once
#include "Dependencies.h"

class Lottery
{
private:
	int nameIteration = 0;
	int nameNum = 0;
	std::vector<std::string> nameList[2];
	std::vector<std::string> *usageSwapPointer = &(nameList[0]);
	std::vector<std::string> *refreshSwapPointer = &(nameList[1]);
	HANDLE hListRefreshThread;
	HANDLE hListRefreshNotifier;
	volatile bool terminate = false;

	static DWORD __stdcall RefreshNameList(LPVOID lpParameters);
public:
	Lottery() = default;
	Lottery(const char *filePath);
	std::string GetNextName();
	void TerminateRefresh();
};
#include "Lottery.h"
#include "OpenXLSX\OpenXLSX.h"

Lottery::Lottery(const char *filePath)
{
	OpenXLSX::XLDocument xlsxDoc;
	xlsxDoc.OpenDocument(filePath);
	auto sheet = xlsxDoc.Workbook().Worksheet("Sheet1");
	size_t rowNum = sheet.RowCount();

	this->nameList[0].resize(rowNum);
	this->nameList[1].resize(rowNum);
	for (int i = 0; i < rowNum; i++)
	{
		std::ostringstream oss;
		oss << "A" << i + 1;
		this->nameList[0][i] = sheet.Cell(oss.str()).Value().Get<std::string>();
		this->nameList[1][i] = sheet.Cell(oss.str()).Value().Get<std::string>();
	}
	this->nameNum = rowNum;
	srand(time(nullptr));
	for (int i = rowNum - 1; i >= 0; i--)
	{
		std::swap(this->nameList[0][rand() % (i + 1)], this->nameList[0][i]);
		std::swap(this->nameList[1][rand() % (i + 1)], this->nameList[1][i]);
	}
	::CreateThread(0, 0, Lottery::RefreshNameList, this, 0, 0);
}

std::string Lottery::GetNextName()
{
	if (this->nameIteration > this->nameNum - 1)
	{
		std::swap(this->usageSwapPointer, this->refreshSwapPointer);
		::SetEvent(this->hListRefreshNotifier);
		this->nameIteration = 0;
	}
	return (*this->usageSwapPointer)[this->nameIteration++];
}

std::string Lottery::GetNextBunch()
{
	std::ostringstream oss;
	for (int i = 0; i < this->bunchNum - 1; i++)
	{
		oss << this->GetNextName() << ", ";
	}
	oss << this->GetNextName();
	return oss.str();
}

void Lottery::TerminateRefresh()
{
	this->terminate = true;
	::SetEvent(this->hListRefreshNotifier);
	::WaitForSingleObject(this->hListRefreshThread, INFINITE);
	::CloseHandle(this->hListRefreshThread);
	::CloseHandle(this->hListRefreshNotifier);
}

int Lottery::GetBunchNum()
{
	return this->bunchNum;
}

void Lottery::LoadLotteryConfig(const char *filePath)
{
	std::ifstream fpConfig;
	fpConfig.open(filePath, std::ifstream::in);
	int level, num;
	while (fpConfig >> level >> num)
	{
		this->prizeLevel.push_back(level);
		this->prizeNum.push_back(num);
	}
}

void Lottery::SetPrizeLevel(int input_level)
{
	this->isUsingPrizeLevel = 1;
	this->presPrizeLevel = input_level;
	this->bunchNum = this->prizeNum[this->presPrizeLevel - 1];
}

int Lottery::GetPrizeLevel()
{
	return this->presPrizeLevel;
}

int Lottery::GetLevelRelatedBunchNum()
{
	return this->prizeNum[this->presPrizeLevel - 1];
}

bool Lottery::GetUsingPrizeLevelState()
{
	return this->isUsingPrizeLevel;
}

DWORD __stdcall Lottery::RefreshNameList(LPVOID lpParameters)
{
	Lottery *pLottery = (Lottery*)lpParameters;
	while (true)
	{
		pLottery->hListRefreshNotifier = ::CreateEvent(0, 0, 0, 0);
		::WaitForSingleObject(pLottery->hListRefreshNotifier, INFINITE);
		if (pLottery->terminate) break;
		for (int i = pLottery->nameNum - 1; i >= 0; i--)
		{
			std::swap(
				(*pLottery->refreshSwapPointer)[rand() % (i + 1)], (*pLottery->refreshSwapPointer)[i]
			);
		}
	}
	return 0;
}

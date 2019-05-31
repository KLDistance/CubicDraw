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

void Lottery::TerminateRefresh()
{
	this->terminate = true;
	::SetEvent(this->hListRefreshNotifier);
	::WaitForSingleObject(this->hListRefreshThread, INFINITE);
	::CloseHandle(this->hListRefreshThread);
	::CloseHandle(this->hListRefreshNotifier);
}

DWORD __stdcall Lottery::RefreshNameList(LPVOID lpParameters)
{
	Lottery *pLottery = (Lottery*)lpParameters;
	while (true)
	{
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

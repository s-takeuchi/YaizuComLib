#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\com\stkdata\stkdata.h"

int Insert16383Records();
int Insert16383RecordsAtOneTime();
int SmallTable();
int TableVersion();
int BinaryTest();
int ManyRecords();
int AutoSaveTest();
int FloatTest();

//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main(int argc, char *argv[])
{
	printf("Test started.\r\n");
	if (ManyRecords() == -1) {
		return -1;
	}
	if (SmallTable() == -1) {
		return -1;
	}
	if (TableVersion() == -1) {
		return -1;
	}
	if (BinaryTest() == -1) {
		return -1;
	}
	if (Insert16383Records() == -1) {
		return -1;
	}
	if (Insert16383RecordsAtOneTime() == -1) {
		return -1;
	}
	if (FloatTest() == -1) {
		return -1;
	}
	if (AutoSaveTest() == -1) {
		return -1;
	}
	printf("Test completed.\r\n");
	return 0;
}

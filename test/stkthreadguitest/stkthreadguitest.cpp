#include <windows.h>
#include <stdio.h>
#include <cstring>
#include "..\..\src\stkthread\stkthread.h"
#include "..\..\src\stkthreadgui\stkthreadgui.h"

int ElemStkThreadInit(int Id)
{
	return 0;
}

int ElemStkThreadFinal(int Id)
{
	return 0;
}

int ElemStkThreadMain(int Id)
{
	return 0;
}

int ElemStkThreadStart(int Id)
{
	wchar_t Buf[50];
	wsprintf(Buf, L"Thread[%d]", Id);
	AddStkThreadLogWithThreadInfo(Buf, L"Started\r\n");
	return 0;
}

int ElemStkThreadStop(int Id)
{
	wchar_t Buf[50];
	wsprintf(Buf, L"Thread[%d]", Id);
	AddStkThreadLogWithThreadInfo(Buf, L"Stopped\r\n");
	return 0;
}

void ProcBefore()
{
	AddStkThreadLog(L"ProcBefore\r\n");
}

void ProcAfter()
{
	AddStkThreadLog(L"ProcAfter\r\n");
}

DWORD WINAPI TestThreadProc(LPVOID Param)
{
	Sleep(1000);
	for (int Loop = 0; Loop < 10; Loop++) {
		printf("全スレッド自動実行\r\n");
		StartAllStkThreads();
		Sleep(1000);

		wchar_t BufName[MAX_LENGTH_OF_STKTHREAD_NAME];
		wchar_t BufDesc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
		wsprintf(BufName, L"Name%03d", Loop);
		wsprintf(BufDesc, L"Description%03d", Loop);
		AddStkThreadForGui(Loop, BufName, BufDesc, NULL, NULL, NULL, ElemStkThreadStart, ElemStkThreadStop);
		Sleep(1000);

		printf("全スレッド自動停止\r\n");
		StopAllStkThreads();
		Sleep(1000);
	}
	for (int Loop = 0; Loop < 10; Loop++) {
		printf("スレッド削除\r\n");
		DeleteStkThreadForGui(Loop);
		Sleep(1000);
	}
	return 0;
}

int main(int Argc, char* Argv[])
{
	ChangeStkThreadLogSize(100);
	SetProcBeforeFirstStkThreadStarts(ProcBefore);
	SetProcAfterLastStkThreadStops(ProcAfter);
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		wchar_t BufName[MAX_LENGTH_OF_STKTHREAD_NAME];
		wchar_t BufDesc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
		wsprintf(BufName, L"Name%03d", Loop);
		wsprintf(BufDesc, L"Description%03d", Loop);
		AddStkThreadForGui(Loop, BufName, BufDesc, NULL, NULL, NULL, ElemStkThreadStart, ElemStkThreadStop);
	}
	printf("全スレッド開始→停止を押してください。ログ出力が100文字以内であることを確認しダイアログを閉じてください。\r\n");
	ShowStkThreadController(NULL, 0, L"Test");
	wchar_t LogBuf[20000];
	GetStkThreadLog(LogBuf, 10);
	if (lstrcmp(LogBuf, L"ProcAfter") != 0) {
		printf("Error : Log does not start with 'ProcAfter'.\r\n");
		exit(0);
	}

	ChangeStkThreadLogSize(1);
	printf("全スレッド開始→停止を押してください。ログ出力が前回と変化無いことを確認しダイアログを閉じてください。\r\n");
	ShowStkThreadController(NULL, 0, L"Test");
	GetStkThreadLog(LogBuf, 12000);
	if (wcsstr(LogBuf, L"ProcAfter") == 0) {
		printf("Error : Log does not start with 'ProcAfter'.\r\n");
		exit(0);
	}

	ChangeStkThreadLogSize(50000);
	printf("ログ出力量が拡張されていることを確認しダイアログを閉じてください。\r\n");
	ShowStkThreadController(NULL, 0, L"Test");

	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		DeleteStkThreadForGui(Loop);
	}
	printf("何もスレッドが存在しないことを確認しダイアログを閉じてください。\r\n");
	ShowStkThreadController(NULL, 0, L"Test");

	DWORD TmpId;
	CreateThread(NULL, 0, &TestThreadProc, NULL, 0, &TmpId);
	printf("StkThreadController自動制御--------------------\r\n");
	printf("スレッドが10個分追加されたあと，10個分削除されます。\r\n");
	ShowStkThreadController(NULL, 0, L"Test");

	printf("\r\nテストが完了しました。\r\n");

	return 0;
}

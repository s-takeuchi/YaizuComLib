#include <windows.h>
#include <tchar.h>
#include <stdio.h>
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
	TCHAR Buf[50];
	wsprintf(Buf, _T("Thread[%d]"), Id);
	AddStkThreadLogWithThreadInfo(Buf, _T("Started\r\n"));
	return 0;
}

int ElemStkThreadStop(int Id)
{
	TCHAR Buf[50];
	wsprintf(Buf, _T("Thread[%d]"), Id);
	AddStkThreadLogWithThreadInfo(Buf, _T("Stopped\r\n"));
	return 0;
}

void ProcBefore()
{
	AddStkThreadLog(_T("ProcBefore\r\n"));
}

void ProcAfter()
{
	AddStkThreadLog(_T("ProcAfter\r\n"));
}

DWORD WINAPI TestThreadProc(LPVOID Param)
{
	Sleep(1000);
	for (int Loop = 0; Loop < 10; Loop++) {
		printf("全スレッド自動実行\r\n");
		StartAllStkThreads();
		Sleep(1000);

		TCHAR BufName[MAX_LENGTH_OF_STKTHREAD_NAME];
		TCHAR BufDesc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
		wsprintf(BufName, _T("Name%03d"), Loop);
		wsprintf(BufDesc, _T("Description%03d"), Loop);
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
		TCHAR BufName[MAX_LENGTH_OF_STKTHREAD_NAME];
		TCHAR BufDesc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
		wsprintf(BufName, _T("Name%03d"), Loop);
		wsprintf(BufDesc, _T("Description%03d"), Loop);
		AddStkThreadForGui(Loop, BufName, BufDesc, NULL, NULL, NULL, ElemStkThreadStart, ElemStkThreadStop);
	}
	printf("全スレッド開始→停止を押してください。ログ出力が100文字以内であることを確認しダイアログを閉じてください。\r\n");
	ShowStkThreadController(NULL, 0, _T("Test"));

	ChangeStkThreadLogSize(1);
	printf("全スレッド開始→停止を押してください。ログ出力が前回と変化無いことを確認しダイアログを閉じてください。\r\n");
	ShowStkThreadController(NULL, 0, _T("Test"));

	ChangeStkThreadLogSize(50000);
	printf("ログ出力量が拡張されていることを確認しダイアログを閉じてください。\r\n");
	ShowStkThreadController(NULL, 0, _T("Test"));

	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		DeleteStkThreadForGui(Loop);
	}
	printf("何もスレッドが存在しないことを確認しダイアログを閉じてください。\r\n");
	ShowStkThreadController(NULL, 0, _T("Test"));

	DWORD TmpId;
	CreateThread(NULL, 0, &TestThreadProc, NULL, 0, &TmpId);
	printf("StkThreadController自動制御--------------------\r\n");
	printf("スレッドが10個分追加されたあと，10個分削除されます。\r\n");
	ShowStkThreadController(NULL, 0, _T("Test"));

	printf("\r\nテストが完了しました。\r\n");

	return 0;
}

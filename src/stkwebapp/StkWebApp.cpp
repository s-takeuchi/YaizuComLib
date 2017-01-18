#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include "..\stksocket\stksocket.h"
#include "..\..\src\stkthread\stkthread.h"
#include "..\commonfunc\stkobject.h"
#include "StkWebApp.h"
#include "StkWebAppExec.h"

#define DATA_LEN 1000000
#define MAX_THREAD_COUNT 64
#define MAX_REQHANDLER_COUNT 1024
#define MAX_IMPL_COUNT 8

int StkWebAppCount = 0;
StkWebApp* StkWebAppArray[MAX_IMPL_COUNT];

class StkWebApp::Impl
{
public:
	CRITICAL_SECTION ReqHandlerCs;

	int WebThreadCount;
	int WebThreadIds[MAX_THREAD_COUNT];

	int ReqHandlerCount;
	StkObject* Req[MAX_REQHANDLER_COUNT];
	StkWebAppExec* Handler[MAX_REQHANDLER_COUNT];

public:
	TCHAR* SkipHttpHeader(TCHAR*);

	TCHAR* Uft8ToWideChar(BYTE* Txt);
	BYTE* WideCharToUtf8(TCHAR*);

	StkObject* RecvRequest(int, int*);
	void SendResponse(StkObject*, int, int);

	static int ElemStkThreadMainRecv(int);

	int AddReqHandler(StkObject*, StkWebAppExec*);
	int DeleteReqHandler(StkObject*);
};

TCHAR* StkWebApp::Impl::SkipHttpHeader(TCHAR* Txt)
{
	TCHAR* Ptr = NULL;
	Ptr = StrStr(Txt, _T("\r\n\r\n"));
	if (Ptr != NULL) {
		Ptr += 4;
		return Ptr;
	}
	Ptr = StrStr(Txt, _T("\n\r\n\r"));
	if (Ptr != NULL) {
		Ptr += 4;
		return Ptr;
	}
	Ptr = StrStr(Txt, _T("\n\n"));
	if (Ptr != NULL) {
		Ptr += 2;
		return Ptr;
	}
	return Txt;
}

TCHAR* StkWebApp::Impl::Uft8ToWideChar(BYTE* Txt)
{
	int WcSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)Txt, -1, NULL, NULL);
	if (WcSize > 0) {
		TCHAR* WcTxt = new TCHAR[WcSize + 1];
		WcSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)Txt, -1, WcTxt, WcSize);
		WcTxt[WcSize] = '\0';
		return WcTxt;
	}
	return NULL;
}

BYTE* StkWebApp::Impl::WideCharToUtf8(TCHAR* Txt)
{
	int Utf8Size = WideCharToMultiByte(CP_UTF8, 0, Txt, -1, NULL, 0, NULL, NULL);
	if (Utf8Size > 0) {
		BYTE* Utf8Txt = new BYTE[Utf8Size + 1];
		Utf8Size = WideCharToMultiByte(CP_UTF8, 0, Txt, -1, (LPSTR)Utf8Txt, Utf8Size, NULL, NULL);
		Utf8Txt[Utf8Size] = '\0';
		return Utf8Txt;
	}
	return NULL;
}

StkObject* StkWebApp::Impl::RecvRequest(int TargetId, int* XmlJsonType)
{
	int Ret = StkSocket_Accept(TargetId);
	if (Ret == -1) {
		return NULL;
	}
	BYTE Dat[DATA_LEN];
	Ret = StkSocket_Receive(TargetId, TargetId, Dat, DATA_LEN, 500, NULL, -1, FALSE);
	if (Ret == -1 || Ret == -2) {
		StkSocket_CloseAccept(TargetId, TargetId, TRUE);
		return NULL;
	}
	TCHAR *DatWc = Uft8ToWideChar(Dat);
	if (DatWc == NULL) {
		StkSocket_CloseAccept(TargetId, TargetId, TRUE);
		return NULL;
	}
	TCHAR* Req = SkipHttpHeader(DatWc);
	*XmlJsonType = StkObject::Analyze(Req);
	if (*XmlJsonType == -1) {
		StkSocket_CloseAccept(TargetId, TargetId, TRUE);
		delete DatWc;
		return NULL;
	}
	int ErrorCode = 0;
	StkObject* ReqObj = NULL;
	if (*XmlJsonType == 1) {
		ReqObj = StkObject::CreateObjectFromXml(Req, &ErrorCode);
	} else if (*XmlJsonType == 2) {
		ReqObj = StkObject::CreateObjectFromJson(Req, &ErrorCode);
	}
	delete DatWc;
	return ReqObj;
};

void StkWebApp::Impl::SendResponse(StkObject* Obj, int TargetId, int XmlJsonType)
{
	if (XmlJsonType != 1 && XmlJsonType != 2) {
		return;
	}
	char ContType[64] = "";
	BYTE* Dat;
	TCHAR XmlOrJson[DATA_LEN] = _T("");
	if (XmlJsonType == 1) {
		Obj->ToXml(XmlOrJson, DATA_LEN);
		strcpy_s(ContType, 64, "Content-Type: application/xml\r\n\r\n");
	} else if (XmlJsonType == 2) {
		Obj->ToJson(XmlOrJson, DATA_LEN);
		strcpy_s(ContType, 64, "Content-Type: application/json\r\n\r\n");
	}
	Dat = WideCharToUtf8(XmlOrJson);
	int Ret1 = StkSocket_Send(TargetId, TargetId, (BYTE*)ContType, strlen(ContType));
	int Ret2 = StkSocket_Send(TargetId, TargetId, Dat, strlen((char*)Dat) + 1);
	delete Dat;
	StkSocket_CloseAccept(TargetId, TargetId, TRUE);
};

int StkWebApp::Impl::ElemStkThreadMainRecv(int Id)
{
	StkWebApp* Obj = StkWebApp::GetStkWebAppByThreadId(Id);
	if (Obj == NULL) {
		return 0;
	}
	return Obj->ThreadLoop(Id);
}

int StkWebApp::Impl::AddReqHandler(StkObject* ReqObj, StkWebAppExec* HandlerObj)
{
	EnterCriticalSection(&ReqHandlerCs);
	for (int Loop = 0; Loop < ReqHandlerCount; Loop++) {
		if (ReqObj->Equals(Req[Loop]) == TRUE) {
			LeaveCriticalSection(&ReqHandlerCs);
			return -1;
		}
	}
	Req[ReqHandlerCount] = ReqObj;
	Handler[ReqHandlerCount] = HandlerObj;
	ReqHandlerCount++;
	LeaveCriticalSection(&ReqHandlerCs);
	return ReqHandlerCount;
}

int StkWebApp::Impl::DeleteReqHandler(StkObject* ReqObj)
{
	EnterCriticalSection(&ReqHandlerCs);
	for (int Loop = 0; Loop < ReqHandlerCount; Loop++) {
		if (Req[Loop] != ReqObj) {
			continue;
		}
		delete Req[Loop];
		delete Handler[Loop];
		for (int Loop2 = Loop; Loop2 < ReqHandlerCount - 1; Loop2++) {
			Req[Loop2] = Req[Loop2 + 1];
			Handler[Loop2] = Handler[Loop2 + 1];
		}
		ReqHandlerCount--;
		LeaveCriticalSection(&ReqHandlerCs);
		return ReqHandlerCount;
	}
	LeaveCriticalSection(&ReqHandlerCs);
	return -1;
}

BOOL StkWebApp::Contains(int ThreadId)
{
	for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
		if (pImpl->WebThreadIds[Loop] == ThreadId) {
			return TRUE;
		}
	}
	return FALSE;
}

int StkWebApp::ThreadLoop(int ThreadId)
{
	int XmlJsonType;
	StkObject* StkObjReq = pImpl->RecvRequest(ThreadId, &XmlJsonType);
	if (StkObjReq == NULL) {
	} else {
		StkObject* StkObjRes = NULL;
		for (int Loop = 0; Loop < pImpl->ReqHandlerCount; Loop++) {
			if (StkObjReq->Contains(pImpl->Req[Loop]) != NULL) {
				StkObjRes = pImpl->Handler[Loop]->Execute(StkObjReq);
				break;
			}
		}
		if (StkObjRes == NULL) {
			StkObjRes = new StkObject(_T(""));
		}
		pImpl->SendResponse(StkObjRes, ThreadId, XmlJsonType);
		delete StkObjReq;
		delete StkObjRes;
	}
	return 0;
}

StkWebApp* StkWebApp::GetStkWebAppByThreadId(int ThreadId)
{
	for (int Loop = 0; Loop < StkWebAppCount; Loop++) {
		if (StkWebAppArray[Loop]->Contains(ThreadId) == TRUE) {
			return StkWebAppArray[Loop];
		}
	}
	return NULL;
}

StkWebApp::StkWebApp(int* TargetIds, int Count, TCHAR* HostName, int TargetPort)
{
	pImpl = new Impl;
	InitializeCriticalSection(&pImpl->ReqHandlerCs);
	pImpl->WebThreadCount = 0;
	pImpl->ReqHandlerCount = 0;

	// Update array of StkWebApp
	if (StkWebAppCount < MAX_IMPL_COUNT) {
		StkWebAppArray[StkWebAppCount] = this;
		StkWebAppCount++;
	}

	// Init thread ID and count
	if (Count > MAX_THREAD_COUNT) {
		pImpl->WebThreadCount = MAX_THREAD_COUNT;
	} else {
		pImpl->WebThreadCount = Count;
	}
	for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
		pImpl->WebThreadIds[Loop] = TargetIds[Loop];
	}

	// Add and open socket
	if (pImpl->WebThreadCount >= 1) {
		StkSocket_AddInfo(pImpl->WebThreadIds[0], SOCK_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, HostName, TargetPort);
		for (int Loop = 1; Loop < pImpl->WebThreadCount; Loop++) {
			StkSocket_CopyInfo(pImpl->WebThreadIds[Loop], pImpl->WebThreadIds[0]);
		}
		StkSocket_Open(pImpl->WebThreadIds[0]);
	}

	// Add threads
	TCHAR Name[32];
	TCHAR Desc[32];
	for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
		wsprintf(Name, _T("Recv-%d"), Loop);
		wsprintf(Desc, _T("Worker thread"), Loop);
		AddStkThread(pImpl->WebThreadIds[Loop], Name, Desc, NULL, NULL, Impl::ElemStkThreadMainRecv, NULL, NULL);
	}

	// Start threads
	StartSpecifiedStkThreads(pImpl->WebThreadIds, pImpl->WebThreadCount);
	BOOL ThreadsStarted = FALSE;
	while (ThreadsStarted == FALSE) {
		Sleep(100);
		ThreadsStarted = TRUE;
		for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
			if (GetStkThreadStatus(pImpl->WebThreadIds[Loop]) != STKTHREAD_STATUS_RUNNING) {
				ThreadsStarted = FALSE;
			}
		}
	}
}

StkWebApp::~StkWebApp()
{
	// Stop threads
	StopSpecifiedStkThreads(pImpl->WebThreadIds, pImpl->WebThreadCount);
	BOOL ThreadsStopped = FALSE;
	while (ThreadsStopped == FALSE) {
		Sleep(100);
		ThreadsStopped = TRUE;
		for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
			if (GetStkThreadStatus(pImpl->WebThreadIds[Loop]) != STKTHREAD_STATUS_READY) {
				ThreadsStopped = FALSE;
			}
		}
	}

	// Delete threads
	for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
		DeleteStkThread(pImpl->WebThreadIds[Loop]);
	}

	// Close and delete socket
	if (pImpl->WebThreadCount >= 1) {
		for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
			StkSocket_Close(pImpl->WebThreadIds[Loop], TRUE);
			StkSocket_DeleteInfo(pImpl->WebThreadIds[Loop]);
		}
	}

	// Update array of StkWebApp
	for (int Loop = 0; Loop < StkWebAppCount; Loop++) {
		if (StkWebAppArray[Loop] != this) {
			continue;
		}
		for (int Loop2 = Loop; Loop2 < StkWebAppCount - 1; Loop2++) {
			StkWebAppArray[Loop2] = StkWebAppArray[Loop2 + 1];
		}
		StkWebAppCount--;
		break;
	}

	delete pImpl;
};

int StkWebApp::AddReqHandler(StkObject* ReqObj, StkWebAppExec* HandlerObj)
{
	return pImpl->AddReqHandler(ReqObj, HandlerObj);
}

int StkWebApp::DeleteReqHandler(StkObject* ReqObj)
{
	return pImpl->DeleteReqHandler(ReqObj);
}

#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include "..\stksocket\stksocket.h"
#include "..\..\src\stkthread\stkthread.h"
#include "..\commonfunc\stkobject.h"
#include "StkObjectConverter.h"

#define DATA_LEN 1000000
#define MAX_THREAD_COUNT 64

class StkObjectConverter::Impl
{
public:
	int WebThreadIds[MAX_THREAD_COUNT];
	int WebThreadCount;

public:
	static TCHAR* SkipHttpHeader(TCHAR*);

	static TCHAR* Uft8ToWideChar(BYTE* Txt);
	static BYTE* WideCharToUtf8(TCHAR*);

	static StkObject* RecvRequest(int, int*);
	static void SendResponse(StkObject*, int, int);

	static int ElemStkThreadMainRecv(int);
};

TCHAR* StkObjectConverter::Impl::SkipHttpHeader(TCHAR* Txt)
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

TCHAR* StkObjectConverter::Impl::Uft8ToWideChar(BYTE* Txt)
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

BYTE* StkObjectConverter::Impl::WideCharToUtf8(TCHAR* Txt)
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

StkObject* StkObjectConverter::Impl::RecvRequest(int TargetId, int* XmlJsonType)
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

void StkObjectConverter::Impl::SendResponse(StkObject* Obj, int TargetId, int XmlJsonType)
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

int StkObjectConverter::Impl::ElemStkThreadMainRecv(int Id)
{
	int XmlJsonType;
	StkObject* StkObj = RecvRequest(Id, &XmlJsonType);
	if (StkObj == NULL) {
	} else {
		SendResponse(StkObj, Id, XmlJsonType);
		delete StkObj;
	}
	return 0;
}

StkObjectConverter::StkObjectConverter(int* TargetIds, int Count, TCHAR* HostName, int TargetPort)
{
	pImpl = new Impl;

	// Init thread ID and count
	if (Count > MAX_THREAD_COUNT) {
		pImpl->WebThreadCount = MAX_THREAD_COUNT;
	} else {
		pImpl->WebThreadCount = Count;
	}
	for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
		pImpl->WebThreadIds[Loop] = TargetIds[Loop];
	}

	// Open socket
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
}

StkObjectConverter::~StkObjectConverter()
{
	// Stop threads
	StopSpecifiedStkThreads(pImpl->WebThreadIds, pImpl->WebThreadCount);

	// Delete threads
	for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
		DeleteStkThread(pImpl->WebThreadIds[Loop]);
	}

	// Close socket
	if (pImpl->WebThreadCount >= 1) {
		for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
			StkSocket_Close(pImpl->WebThreadIds[Loop], TRUE);
		}
	}

	delete pImpl;
};

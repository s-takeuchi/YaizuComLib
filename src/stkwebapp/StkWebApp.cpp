#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <time.h>
#include "..\stksocket\stksocket.h"
#include "..\stkthread\stkthread.h"
#include "..\commonfunc\stkobject.h"
#include "..\commonfunc\StkStringParser.h"
#include "..\commonfunc\MsgProc.h"
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

	int HandlerCount;
	int HandlerMethod[MAX_REQHANDLER_COUNT];
	TCHAR HandlerUrlPath[MAX_REQHANDLER_COUNT][128];
	StkWebAppExec* Handler[MAX_REQHANDLER_COUNT];

	BOOL StopFlag;

public:
	TCHAR* SkipHttpHeader(TCHAR*);

	TCHAR* Uft8ToWideChar(BYTE* Txt);
	BYTE* WideCharToUtf8(TCHAR*);

	BYTE* MakeHttpHeader(int, int, int);
	StkObject* RecvRequest(int, int*, int*, TCHAR[128]);
	void SendResponse(StkObject*, int, int, int);
	StkObject* MakeErrorResponse(int ErrId);

	static int ElemStkThreadMainRecv(int);

	int AddReqHandler(int, TCHAR[128], StkWebAppExec*);
	int DeleteReqHandler(int, TCHAR[128]);
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

BYTE* StkWebApp::Impl::MakeHttpHeader(int ResultCode, int DataLength, int XmlJsonType)
{
	char* HeaderData = new char[1024];
	strcpy_s(HeaderData, 1024, "");

	char Status[32] = "";
	char RespLine[64] = "";
	char ContType[64] = "";
	char ContLen[64] = "";
	char Connection[64] = "";
	char CacheCont[64] = "";
	char Date[64] = "";

	/***** Make response status begin *****/
	switch (ResultCode) {
	case 200:
		strcpy_s(Status, 32, "OK");
		break;
	case 202:
		strcpy_s(Status, 32, "Accepted");
		break;
	case 400:
		strcpy_s(Status, 32, "Bad Request");
		break;
	case 404:
		strcpy_s(Status, 32, "Not Found");
		break;
	default:
		strcpy_s(Status, 32, "Internal Server Error");
		break;
	};
	sprintf_s(RespLine, "HTTP/1.1 %d %s\r\n", ResultCode, Status);
	/***** Make response status end *****/

	if (XmlJsonType == 1) {
		strcpy_s(ContType, 64, "Content-Type: application/xml\r\n");
	} else if (XmlJsonType == 2) {
		strcpy_s(ContType, 64, "Content-Type: application/json\r\n");
	} else if (XmlJsonType == 0) {
	}
	sprintf_s(ContLen, 64, "Content-Length: %d\r\n", DataLength);
	sprintf_s(Connection, 64, "Connection: close\r\n");
	sprintf_s(CacheCont, 64, "Cache-Control: no-cache\r\n");

	/***** Make time string begin *****/
	struct tm GmtTime;
	__int64 Ltime;
	_time64(&Ltime);
	_gmtime64_s(&GmtTime, &Ltime);
	char MonStr[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	char WdayStr[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	sprintf_s(Date, 64, "Date: %s, %02d %s %d %02d:%02d:%02d GMT\r\n",
		WdayStr[GmtTime.tm_wday],
		GmtTime.tm_mday,
		MonStr[GmtTime.tm_mon],
		GmtTime.tm_year + 1900,
		GmtTime.tm_hour,
		GmtTime.tm_min,
		GmtTime.tm_sec);
	/***** Make time string end *****/

	strcat_s(HeaderData, 1024, RespLine);
	strcat_s(HeaderData, 1024, ContType);
	strcat_s(HeaderData, 1024, ContLen);
	strcat_s(HeaderData, 1024, Connection);
	strcat_s(HeaderData, 1024, CacheCont);
	strcat_s(HeaderData, 1024, Date);

	strcat_s(HeaderData, 1024, "\r\n");

	return (BYTE*)HeaderData;
}

StkObject* StkWebApp::Impl::RecvRequest(int TargetId, int* XmlJsonType, int* Method, TCHAR UrlPath[128])
{
	*XmlJsonType = -1;
	*Method = STKWEBAPP_METHOD_UNDEFINED;
	lstrcpy(UrlPath, _T(""));

	int Ret = StkSocket_Accept(TargetId);
	if (Ret == -1) {
		return NULL;
	}
	BYTE Dat[DATA_LEN];
	Ret = StkSocket_Receive(TargetId, TargetId, Dat, DATA_LEN, 200030, NULL, -1, FALSE);
	if (Ret == -1 || Ret == -2) {
		StkSocket_CloseAccept(TargetId, TargetId, TRUE);
		return NULL;
	}
	if (Ret >= DATA_LEN) {
		Dat[DATA_LEN - 1] = '\0';
	} else {
		Dat[Ret] = '\0';
	}
	TCHAR *DatWc = Uft8ToWideChar(Dat);
	if (DatWc == NULL) {
		StkSocket_CloseAccept(TargetId, TargetId, TRUE);
		return NULL;
	}

	//// Acquire METHOD and URL path begin ////
	TCHAR MethodStr[16];
	StkStringParser::ParseInto3Params(DatWc, _T("# # HTTP#"), _T('#'), MethodStr, 16, UrlPath, 128, NULL, -1);
	if (lstrcmp(MethodStr, _T("GET")) == 0) {
		*Method = STKWEBAPP_METHOD_GET;
	} else if (lstrcmp(MethodStr, _T("HEAD")) == 0) {
		*Method = STKWEBAPP_METHOD_HEAD;
	} else if (lstrcmp(MethodStr, _T("POST")) == 0) {
		*Method = STKWEBAPP_METHOD_POST;
	} else if (lstrcmp(MethodStr, _T("PUT")) == 0) {
		*Method = STKWEBAPP_METHOD_PUT;
	} else if (lstrcmp(MethodStr, _T("DELETE")) == 0) {
		*Method = STKWEBAPP_METHOD_DELETE;
	} else {
		*Method = STKWEBAPP_METHOD_INVALID;
		delete DatWc;
		return NULL;
	}
	//// Acquire METHOD and URL path end ////

	TCHAR* Req = SkipHttpHeader(DatWc);
	*XmlJsonType = StkObject::Analyze(Req);
	if (*XmlJsonType == -1 || *XmlJsonType == 1) {
		delete DatWc;
		return NULL;
	}

	int ErrorCode = 0;
	StkObject* ReqObj = NULL;
	if (*XmlJsonType == 2) {
		ReqObj = StkObject::CreateObjectFromJson(Req, &ErrorCode);
	} else if (*XmlJsonType == 0) {
	}
	delete DatWc;
	return ReqObj;
}

void StkWebApp::Impl::SendResponse(StkObject* Obj, int TargetId, int XmlJsonType, int ResultCode)
{
	BYTE* Dat = NULL;
	int DatLength = 0;
	TCHAR XmlOrJson[DATA_LEN] = _T("");

	if (XmlJsonType != 0 && XmlJsonType != 1 && XmlJsonType != 2) {
		ResultCode = 500;
	} else if ((XmlJsonType == 1 && Obj == NULL) || (XmlJsonType == 2 && Obj == NULL)) {
		ResultCode = 500;
	} else {
		if (XmlJsonType == 1 && Obj != NULL) {
			ResultCode = 500;
		} else if ((XmlJsonType == 0 || XmlJsonType == 2) && Obj != NULL) {
			Obj->ToJson(XmlOrJson, DATA_LEN);
		}
		Dat = WideCharToUtf8(XmlOrJson);
		DatLength = strlen((char*)Dat);
	}

	BYTE* HeaderDat = MakeHttpHeader(ResultCode, DatLength, XmlJsonType);
	int HeaderDatLength = strlen((char*)HeaderDat);

	int RespDatLength = DatLength + HeaderDatLength;
	BYTE* RespDat = new BYTE[RespDatLength + 1];

	strcpy_s((char*)RespDat, RespDatLength + 1, "");
	strcat_s((char*)RespDat, RespDatLength + 1, (char*)HeaderDat);
	if (Dat != NULL) {
		strcat_s((char*)RespDat, RespDatLength + 1, (char*)Dat);
	}

	int RetD = StkSocket_Send(TargetId, TargetId, RespDat, RespDatLength);

	if (Dat != NULL) {
		delete Dat;
	}
	delete HeaderDat;
	delete RespDat;
	StkSocket_CloseAccept(TargetId, TargetId, TRUE);
}

StkObject* StkWebApp::Impl::MakeErrorResponse(int ErrId)
{
	StkObject* ErrResObj = new StkObject(_T("Res"));
	ErrResObj->AppendChildElement(new StkObject(_T("Code"), ErrId));
	ErrResObj->AppendChildElement(new StkObject(_T("MsgEng"), MessageProc::GetMsgEng(ErrId)));
	ErrResObj->AppendChildElement(new StkObject(_T("MsgJpn"), MessageProc::GetMsgJpn(ErrId)));
	return ErrResObj;
}

int StkWebApp::Impl::ElemStkThreadMainRecv(int Id)
{
	StkWebApp* Obj = StkWebApp::GetStkWebAppByThreadId(Id);
	if (Obj == NULL) {
		return 0;
	}
	return Obj->ThreadLoop(Id);
}

int StkWebApp::Impl::AddReqHandler(int Method, TCHAR UrlPath[128], StkWebAppExec* HandlerObj)
{
	EnterCriticalSection(&ReqHandlerCs);
	for (int Loop = 0; Loop < HandlerCount; Loop++) {
		if (Method == HandlerMethod[Loop] && lstrcmp(UrlPath, HandlerUrlPath[Loop]) == 0) {
			LeaveCriticalSection(&ReqHandlerCs);
			return -1;
		}
	}
	HandlerMethod[HandlerCount] = Method;
	lstrcpy(HandlerUrlPath[HandlerCount], UrlPath);
	Handler[HandlerCount] = HandlerObj;
	HandlerCount++;
	LeaveCriticalSection(&ReqHandlerCs);
	return HandlerCount;
}

int StkWebApp::Impl::DeleteReqHandler(int Method, TCHAR UrlPath[128])
{
	EnterCriticalSection(&ReqHandlerCs);
	for (int Loop = 0; Loop < HandlerCount; Loop++) {
		if (Method != HandlerMethod[Loop] || lstrcmp(UrlPath, HandlerUrlPath[Loop]) != 0) {
			continue;
		}
		HandlerMethod[Loop] = STKWEBAPP_METHOD_UNDEFINED;
		lstrcpy(HandlerUrlPath[Loop], _T(""));
		delete Handler[Loop];
		for (int Loop2 = Loop; Loop2 < HandlerCount - 1; Loop2++) {
			HandlerMethod[Loop2] = HandlerMethod[Loop2 + 1];
			lstrcpy(HandlerUrlPath[Loop2], HandlerUrlPath[Loop2 + 1]);
			Handler[Loop2] = Handler[Loop2 + 1];
		}
		HandlerCount--;
		LeaveCriticalSection(&ReqHandlerCs);
		return HandlerCount;
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

void StkWebApp::TheLoop()
{
	while (TRUE) {
		if (pImpl->StopFlag) {
			Sleep(100);
			break;
		}
		Sleep(100);
	}
}

int StkWebApp::ThreadLoop(int ThreadId)
{
	int XmlJsonType;
	int Method;
	TCHAR UrlPath[128];
	int ResultCode;

	StkObject* StkObjReq = pImpl->RecvRequest(ThreadId, &XmlJsonType, &Method, UrlPath);

	// If no request is received, return from this method.
	if (StkObjReq == NULL && Method == StkWebApp::STKWEBAPP_METHOD_UNDEFINED && lstrcmp(UrlPath, _T("")) == 0 && XmlJsonType == -1) {
		return 0;
	}

	// If a request is received...
	StkObject* StkObjRes = NULL;
	BOOL FndFlag = FALSE;
	if (XmlJsonType == 0 || XmlJsonType == 2) {
		// If valid request is received...
		for (int Loop = 0; Loop < pImpl->HandlerCount; Loop++) {
			TCHAR Param[4][128] = {_T(""), _T(""), _T(""), _T("")};
			if (Method & pImpl->HandlerMethod[Loop] &&
				StkStringParser::ParseInto4Params(UrlPath, pImpl->HandlerUrlPath[Loop], _T('$'), Param[0], Param[1], Param[2], Param[3]) == 1) {
				StkObjRes = pImpl->Handler[Loop]->Execute(StkObjReq, Method, UrlPath, &ResultCode);
				FndFlag = TRUE;
				break;
			}
		}
	} else {
		// If invalid request is received...
		if (Method == StkWebApp::STKWEBAPP_METHOD_INVALID) {
			ResultCode = 400;
			StkObjRes = pImpl->MakeErrorResponse(1005);
		} else {
			ResultCode = 400;
			StkObjRes = pImpl->MakeErrorResponse(1002);
		}
		FndFlag = TRUE;
	}

	// If service stop request is presented...
	if (FndFlag == FALSE && Method == STKWEBAPP_METHOD_POST && lstrcmp(UrlPath, _T("/service/")) == 0) {
		int ErrorCode;
		StkObject* TmpObj = StkObject::CreateObjectFromJson(_T("{ \"Operation\" : \"Stop\" }"), &ErrorCode);
		if (StkObjReq->Equals(TmpObj) == TRUE) {
			ResultCode = 202;
			FndFlag = TRUE;
			pImpl->StopFlag = TRUE;
		} else {
			ResultCode = 400;
			StkObjRes = pImpl->MakeErrorResponse(1004);
			FndFlag = TRUE;
		}
		delete TmpObj;
	}

	// Corresponding API is not found.
	if (FndFlag == FALSE) {
		ResultCode = 404;
		StkObjRes = pImpl->MakeErrorResponse(1001);
	}

	// Reset "XmlJsonType"
	if (StkObjRes == NULL && XmlJsonType == 2) {
		XmlJsonType = 0;
	}
	if (StkObjRes != NULL && (XmlJsonType == -1 || XmlJsonType == 0 || XmlJsonType == 1)) {
		XmlJsonType = 2;
	}

	pImpl->SendResponse(StkObjRes, ThreadId, XmlJsonType, ResultCode);
	delete StkObjRes;
	if (StkObjReq != NULL) {
		delete StkObjReq;
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
	pImpl->HandlerCount = 0;
	pImpl->StopFlag = FALSE;

	// Message definition
	MessageProc::AddJpn(1001, _T("クライアントからのリクエストに対応するAPIは定義されていません。"));
	MessageProc::AddEng(1001, _T("No API is defined for the request sent from client."));
	MessageProc::AddJpn(1002, _T("リクエストはJSONではないデータを含んでいます。"));
	MessageProc::AddEng(1002, _T("The request contains non-JSON data."));
	MessageProc::AddJpn(1003, _T("HTTPヘッダのContent-Typeと実際のデータの型が一致しません。"));
	MessageProc::AddEng(1003, _T("Actual data type does not correspond with Content-Type in HTTP header."));
	MessageProc::AddJpn(1004, _T("URL\"/service/\"にPOSTされたリクエストは不正です。"));
	MessageProc::AddEng(1004, _T("An invalid request is posted to URL\"/service/\"."));
	MessageProc::AddJpn(1005, _T("不正なリクエストを受信しました。リクエストが壊れているかHTTPヘッダに不正な値が指定されています。"));
	MessageProc::AddEng(1005, _T("An invalid request is received. The request might be broken or invalid value is configured in the HTTP header."));

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
		SetStkThreadInterval(pImpl->WebThreadIds[Loop], 1);
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
	// Close and delete socket
	if (pImpl->WebThreadCount >= 1) {
		for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
			StkSocket_Close(pImpl->WebThreadIds[Loop], TRUE);
			StkSocket_DeleteInfo(pImpl->WebThreadIds[Loop]);
		}
	}

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

	// Message deletion
	MessageProc::ClearAllMsg();

	delete pImpl;
};

int StkWebApp::AddReqHandler(int Method, TCHAR UrlPath[128], StkWebAppExec* HandlerObj)
{
	return pImpl->AddReqHandler(Method, UrlPath, HandlerObj);
}

int StkWebApp::DeleteReqHandler(int Method, TCHAR UrlPath[128])
{
	return pImpl->DeleteReqHandler(Method, UrlPath);
}

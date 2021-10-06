#include <mutex>
#include "../stkpl/StkPl.h"
#include "../stksocket/stksocket.h"
#include "../stkthread/stkthread.h"
#include "../commonfunc/StkObject.h"
#include "../commonfunc/StkStringParser.h"
#include "../commonfunc/msgproc.h"
#include "StkWebApp.h"
#include "StkWebAppExec.h"

#define MAX_THREAD_COUNT MAX_NUM_OF_STKTHREADS
#define MAX_REQHANDLER_COUNT 1024
#define MAX_IMPL_COUNT 8
#define MAX_HTTPHEADERSIZE 2048

#define XMLJSONTYPE_INVALID -1
#define XMLJSONTYPE_EMPTYSTR 0
#define XMLJSONTYPE_XML 1
#define XMLJSONTYPE_JSON 2

int StkWebAppCount = 0;
StkWebApp* StkWebAppArray[MAX_IMPL_COUNT];

class StkWebApp::Impl
{
public:
	std::mutex ReqHandlerCs;

	int WebThreadCount;
	int WebThreadIds[MAX_THREAD_COUNT];

	int HandlerCount;
	int HandlerMethod[MAX_REQHANDLER_COUNT];
	wchar_t HandlerUrlPath[MAX_REQHANDLER_COUNT][StkWebAppExec::URL_PATH_LENGTH];
	StkWebAppExec* Handler[MAX_REQHANDLER_COUNT];
	bool HandlerSimpleMode[MAX_REQHANDLER_COUNT];

	bool StopFlag;

	int SendBufSize;
	int RecvBufSize;

	int TimeoutInterval;

public:
	const wchar_t* SkipHttpHeader(wchar_t*);

	unsigned char* MakeHttpHeader(int, int, int);
	int RecvRequestHeader(int, int*, wchar_t[StkWebAppExec::URL_PATH_LENGTH], wchar_t[MAX_HTTPHEADERSIZE]);
	StkObject* RecvRequest(int, int*, int, wchar_t*);
	void SendResponse(StkObject*, int, int, int);
	StkObject* MakeErrorResponse(int ErrId);

	static int ElemStkThreadMainRecv(int);

	int AddReqHandler(int, const wchar_t[StkWebAppExec::URL_PATH_LENGTH], StkWebAppExec*, bool = true);
	int DeleteReqHandler(int, const wchar_t[StkWebAppExec::URL_PATH_LENGTH]);
};

const wchar_t* StkWebApp::Impl::SkipHttpHeader(wchar_t* Txt)
{
	const wchar_t* Ptr = NULL;
	Ptr = StkPlWcsStr(Txt, L"\r\n\r\n");
	if (Ptr != NULL) {
		Ptr += 4;
		return Ptr;
	}
	Ptr = StkPlWcsStr(Txt, L"\n\r\n\r");
	if (Ptr != NULL) {
		Ptr += 4;
		return Ptr;
	}
	Ptr = StkPlWcsStr(Txt, L"\n\n");
	if (Ptr != NULL) {
		Ptr += 2;
		return Ptr;
	}
	return Txt;
}

unsigned char* StkWebApp::Impl::MakeHttpHeader(int ResultCode, int DataLength, int XmlJsonType)
{
	char* HeaderData = new char[2048];
	StkPlStrCpy(HeaderData, 2048, "");

	char Status[32] = "";
	char RespLine[64] = "";
	char ContType[64] = "";
	char ContLen[64] = "";
	char Connection[64] = "";
	char CacheCont[64] = "";
	char Date[64] = "";
	char DateTmp[64] = "";

	/***** Make response status begin *****/
	switch (ResultCode) {
	case 100:	StkPlStrCpy(Status, 32, "Continue"); break;
	case 101:	StkPlStrCpy(Status, 32, "Switching Protocols"); break;
	case 102:	StkPlStrCpy(Status, 32, "Processing"); break;
	case 200:	StkPlStrCpy(Status, 32, "OK"); break;
	case 201:	StkPlStrCpy(Status, 32, "Created"); break;
	case 202:	StkPlStrCpy(Status, 32, "Accepted"); break;
	case 203:	StkPlStrCpy(Status, 32, "Non-Authoritative Information"); break;
	case 204:	StkPlStrCpy(Status, 32, "No Content"); break;
	case 205:	StkPlStrCpy(Status, 32, "Reset Content"); break;
	case 206:	StkPlStrCpy(Status, 32, "Partial Content"); break;
	case 207:	StkPlStrCpy(Status, 32, "Multi-Status"); break;
	case 208:	StkPlStrCpy(Status, 32, "Already Reported"); break;
	case 226:	StkPlStrCpy(Status, 32, "IM Used"); break;
	case 300:	StkPlStrCpy(Status, 32, "Multiple Choices"); break;
	case 301:	StkPlStrCpy(Status, 32, "Moved Permanently"); break;
	case 302:	StkPlStrCpy(Status, 32, "Found"); break;
	case 303:	StkPlStrCpy(Status, 32, "See Other"); break;
	case 304:	StkPlStrCpy(Status, 32, "Not Modified"); break;
	case 305:	StkPlStrCpy(Status, 32, "Use Proxy"); break;
	case 307:	StkPlStrCpy(Status, 32, "Temporary Redirect"); break;
	case 308:	StkPlStrCpy(Status, 32, "Permanent Redirect"); break;
	case 400:	StkPlStrCpy(Status, 32, "Bad Request"); break;
	case 401:	StkPlStrCpy(Status, 32, "Unauthorized"); break;
	case 402:	StkPlStrCpy(Status, 32, "Payment Required"); break;
	case 403:	StkPlStrCpy(Status, 32, "Forbidden"); break;
	case 404:	StkPlStrCpy(Status, 32, "Not Found"); break;
	case 405:	StkPlStrCpy(Status, 32, "Method Not Allowed"); break;
	case 406:	StkPlStrCpy(Status, 32, "Not Acceptable"); break;
	case 407:	StkPlStrCpy(Status, 32, "Proxy Authentication Required"); break;
	case 408:	StkPlStrCpy(Status, 32, "Request Timeout"); break;
	case 409:	StkPlStrCpy(Status, 32, "Conflict"); break;
	case 410:	StkPlStrCpy(Status, 32, "Gone"); break;
	case 411:	StkPlStrCpy(Status, 32, "Length Required"); break;
	case 412:	StkPlStrCpy(Status, 32, "Precondition Failed"); break;
	case 413:	StkPlStrCpy(Status, 32, "Payload Too Large"); break;
	case 414:	StkPlStrCpy(Status, 32, "URI Too Long"); break;
	case 415:	StkPlStrCpy(Status, 32, "Unsupported Media Type"); break;
	case 416:	StkPlStrCpy(Status, 32, "Range Not Satisfiable"); break;
	case 417:	StkPlStrCpy(Status, 32, "Expectation Failed"); break;
	case 418:	StkPlStrCpy(Status, 32, "I'm a teapot"); break;
	case 421:	StkPlStrCpy(Status, 32, "Misdirected Request"); break;
	case 422:	StkPlStrCpy(Status, 32, "Unprocessable Entity"); break;
	case 423:	StkPlStrCpy(Status, 32, "Locked"); break;
	case 424:	StkPlStrCpy(Status, 32, "Failed Dependency"); break;
	case 426:	StkPlStrCpy(Status, 32, "Upgrade Required"); break;
	case 451:	StkPlStrCpy(Status, 32, "Unavailable For Legal Reasons"); break;
	case 500:	StkPlStrCpy(Status, 32, "Internal Server Error"); break;
	case 501:	StkPlStrCpy(Status, 32, "Not Implemented"); break;
	case 502:	StkPlStrCpy(Status, 32, "Bad Gateway"); break;
	case 503:	StkPlStrCpy(Status, 32, "Service Unavailable"); break;
	case 504:	StkPlStrCpy(Status, 32, "Gateway Timeout"); break;
	case 505:	StkPlStrCpy(Status, 32, "HTTP Version Not Supported"); break;
	case 506:	StkPlStrCpy(Status, 32, "Variant Also Negotiates"); break;
	case 507:	StkPlStrCpy(Status, 32, "Insufficient Storage"); break;
	case 508:	StkPlStrCpy(Status, 32, "Loop Detected"); break;
	case 509:	StkPlStrCpy(Status, 32, "Bandwidth Limit Exceeded"); break;
	case 510:	StkPlStrCpy(Status, 32, "Not Extended"); break;
	default:
		StkPlStrCpy(Status, 32, "Internal Server Error");
		break;
	};
	StkPlSPrintf(RespLine, 64, "HTTP/1.1 %d %s\r\n", ResultCode, Status);
	/***** Make response status end *****/

	if (XmlJsonType == XMLJSONTYPE_XML) {
		StkPlStrCpy(ContType, 64, "Content-Type: application/xml\r\n");
	} else if (XmlJsonType == XMLJSONTYPE_JSON) {
		StkPlStrCpy(ContType, 64, "Content-Type: application/json\r\n");
	} else if (XmlJsonType == XMLJSONTYPE_EMPTYSTR) {
	}
	StkPlSPrintf(ContLen, 64, "Content-Length: %d\r\n", DataLength);
	StkPlSPrintf(Connection, 64, "Connection: close\r\n");
	StkPlSPrintf(CacheCont, 64, "Cache-Control: no-cache\r\n");

	StkPlGetTimeInRfc2822(DateTmp, false);
	StkPlSPrintf(Date, 64, "Date: %s\r\n", DateTmp);

	StkPlStrCat(HeaderData, 2048, RespLine);
	StkPlStrCat(HeaderData, 2048, ContType);
	StkPlStrCat(HeaderData, 2048, ContLen);
	StkPlStrCat(HeaderData, 2048, Connection);
	StkPlStrCat(HeaderData, 2048, CacheCont);
	StkPlStrCat(HeaderData, 2048, "Access-Control-Allow-Origin: *\r\n");
	StkPlStrCat(HeaderData, 2048, "Access-Control-Allow-Headers: Authorization, Origin, Accept, Content-Type\r\n");
	StkPlStrCat(HeaderData, 2048, "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n");
	StkPlStrCat(HeaderData, 2048, "Access-Control-Max-Age: 3600\r\n");
	StkPlStrCat(HeaderData, 2048, Date);

	StkPlStrCat(HeaderData, 2048, "\r\n");

	return (unsigned char*)HeaderData;
}

// Return = -1 : No request received
// Return = -2 : Header size exceeds the upper limit
// Return = -3 : Request method is invalid
// Return = -4 : Error / Timeout
int StkWebApp::Impl::RecvRequestHeader(int TargetId, int* Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], wchar_t HttpHeader[MAX_HTTPHEADERSIZE])
{
	size_t MaxHeaderSize = MAX_HTTPHEADERSIZE;

	*Method = StkWebAppExec::STKWEBAPP_METHOD_UNDEFINED;
	StkPlWcsCpy(UrlPath, StkWebAppExec::URL_PATH_LENGTH, L"");

	int Ret = StkSocket_Accept(TargetId);
	if (Ret == -1) {
		return -1;
	}
	int Loop = 0;
	for (; Loop < MaxHeaderSize - 1; Loop++) {
		unsigned char TmpDat[1];
		Ret = StkSocket_Receive(TargetId, TargetId, TmpDat, 1, 1, TimeoutInterval, NULL, -1);
		if ((Ret == -1 || Ret == -2) && Loop >= 1) {
			return -4;
		} else if (Ret == 0 || Ret == -1 || Ret == -2) {
			StkSocket_CloseAccept(TargetId, TargetId, true);
			return -1;
		}
		HttpHeader[Loop] = (wchar_t)TmpDat[0];
		if (Loop >= 1 && HttpHeader[Loop - 1] == L'\n' && HttpHeader[Loop] == L'\n') {
			break;
		} else if (Loop >= 3 && 
			((HttpHeader[Loop - 3] == L'\r' && HttpHeader[Loop - 2] == L'\n' && HttpHeader[Loop - 1] == L'\r' && HttpHeader[Loop] == L'\n') ||
			 (HttpHeader[Loop - 3] == L'\n' && HttpHeader[Loop - 2] == L'\r' && HttpHeader[Loop - 1] == L'\n' && HttpHeader[Loop] == L'\r'))) {
			break;
		}
	}
	if (Loop == MaxHeaderSize - 1) {
		return -2;
	}
	HttpHeader[Loop] = L'\0';

	// Acquire METHOD and URL path
	wchar_t MethodStr[16] = L"";
	StkStringParser::ParseInto3Params(HttpHeader, L"# # HTTP#", L'#', MethodStr, 16, UrlPath, StkWebAppExec::URL_PATH_LENGTH, NULL, -1);
	if (StkPlWcsCmp(MethodStr, L"GET") == 0) {
		*Method = StkWebAppExec::STKWEBAPP_METHOD_GET;
	} else if (StkPlWcsCmp(MethodStr, L"HEAD") == 0) {
		*Method = StkWebAppExec::STKWEBAPP_METHOD_HEAD;
	} else if (StkPlWcsCmp(MethodStr, L"POST") == 0) {
		*Method = StkWebAppExec::STKWEBAPP_METHOD_POST;
	} else if (StkPlWcsCmp(MethodStr, L"PUT") == 0) {
		*Method = StkWebAppExec::STKWEBAPP_METHOD_PUT;
	} else if (StkPlWcsCmp(MethodStr, L"DELETE") == 0) {
		*Method = StkWebAppExec::STKWEBAPP_METHOD_DELETE;
	} else if (StkPlWcsCmp(MethodStr, L"OPTIONS") == 0) {
		*Method = StkWebAppExec::STKWEBAPP_METHOD_OPTIONS;
	} else {
		*Method = StkWebAppExec::STKWEBAPP_METHOD_INVALID;
		return -3;
	}

	// Acquire "Content-Length:"
	wchar_t ContentLength[16] = L"";
	StkStringParser::ParseInto3Params(HttpHeader, L"#Content-Length:#\n#", L'#', NULL, -1, ContentLength, 16, NULL, -1);
	int ContLen = StkPlWcsToL(ContentLength);

	return ContLen;
}

StkObject* StkWebApp::Impl::RecvRequest(int TargetId, int* XmlJsonType, int ContLen, wchar_t* HttpHeader)
{
	int Ret = StkSocket_Accept(TargetId);
	if (Ret == -1) {
		return NULL;
	}
	int ActLen = 0;
	if (RecvBufSize < ContLen) {
		ActLen = RecvBufSize;
	} else {
		ActLen = ContLen;
	}
	unsigned char *Dat = new unsigned char[ActLen + 1];
	Ret = StkSocket_Receive(TargetId, TargetId, Dat, ActLen + 1, ActLen, TimeoutInterval, NULL, -1);
	if (Ret == 0 || Ret == -1 || Ret == -2) {
		StkSocket_CloseAccept(TargetId, TargetId, true);
		delete[] Dat;
		return NULL;
	}
	if (Ret >= ActLen) {
		Dat[ActLen] = '\0';
	} else {
		Dat[Ret] = '\0';
	}
	wchar_t *DatWc = StkPlCreateWideCharFromUtf8((char*)Dat);
	delete[] Dat;
	if (DatWc == NULL) {
		StkSocket_CloseAccept(TargetId, TargetId, true);
		return NULL;
	}

	// Check whether the presented body is consist of JSON
	*XmlJsonType = StkObject::Analyze(DatWc);
	if (*XmlJsonType == XMLJSONTYPE_INVALID || *XmlJsonType == XMLJSONTYPE_XML) {
		delete[] DatWc;
		return NULL;
	}
	{
		wchar_t Buf[1024];
		wchar_t ContentType[32];
		StkStringParser::ParseInto3Params(HttpHeader, L"#Content-Type: #\n#", L'#', Buf, 1024, ContentType, 32, NULL, -1);
		if (StkPlWcsStr(ContentType, L"application/json") == NULL && StkPlWcsCmp(ContentType, L"") != 0) {
			*XmlJsonType = XMLJSONTYPE_INVALID;
			delete[] DatWc;
			return NULL;
		}
	}

	// Create JSON object
	int ErrorCode = 0;
	StkObject* ReqObj = NULL;
	if (*XmlJsonType == XMLJSONTYPE_JSON) {
		ReqObj = StkObject::CreateObjectFromJson(DatWc, &ErrorCode);
	} else if (*XmlJsonType == XMLJSONTYPE_EMPTYSTR) {
	}
	delete DatWc;
	return ReqObj;
}

void StkWebApp::Impl::SendResponse(StkObject* Obj, int TargetId, int XmlJsonType, int ResultCode)
{
	unsigned char* Dat = NULL;
	int DatLength = 0;

	if (XmlJsonType != XMLJSONTYPE_EMPTYSTR && XmlJsonType != XMLJSONTYPE_XML && XmlJsonType != XMLJSONTYPE_JSON) {
		// There is no chance to handle this case without a bug.
		ResultCode = 500;
	} else if ((XmlJsonType == XMLJSONTYPE_XML && Obj == NULL) || (XmlJsonType == XMLJSONTYPE_JSON && Obj == NULL)) {
		// There is no chance to handle this case without a bug.
		ResultCode = 500;
	} else {
		if (XmlJsonType == 1 && Obj != NULL) {
			ResultCode = 500;
		} else if ((XmlJsonType == XMLJSONTYPE_EMPTYSTR || XmlJsonType == XMLJSONTYPE_JSON) && Obj != NULL) {
			wchar_t* XmlOrJson = new wchar_t[SendBufSize];
			StkPlWcsCpy(XmlOrJson, SendBufSize, L"");
			int Length = Obj->ToJson(XmlOrJson, SendBufSize);
			if (Length == SendBufSize - 1) {
				ResultCode = 500;
				StkObject* ErrObj = MakeErrorResponse(1006);
				wchar_t TmpBuf[1024] = L"";
				ErrObj->ToJson(TmpBuf, 1024);
				Dat = (unsigned char*)StkPlCreateUtf8FromWideChar(TmpBuf);
				DatLength = (int)StkPlStrLen((char*)Dat);
				delete [] XmlOrJson;
			} else {
				Dat = (unsigned char*)StkPlCreateUtf8FromWideChar(XmlOrJson);
				DatLength = (int)StkPlStrLen((char*)Dat);
				delete [] XmlOrJson;
			}
		}
	}

	unsigned char* HeaderDat = MakeHttpHeader(ResultCode, DatLength, XmlJsonType);
	int HeaderDatLength = (int)StkPlStrLen((char*)HeaderDat);
	int RetH = StkSocket_Send(TargetId, TargetId, HeaderDat, HeaderDatLength);
	delete[] HeaderDat;

	if (Dat != NULL) {
		int RetD = StkSocket_Send(TargetId, TargetId, Dat, DatLength);
		delete Dat;
	}

	StkSocket_CloseAccept(TargetId, TargetId, true);
}

StkObject* StkWebApp::Impl::MakeErrorResponse(int ErrId)
{
	StkObject* ErrResObj = new StkObject(L"");
	ErrResObj->AppendChildElement(new StkObject(L"Code", ErrId));
	ErrResObj->AppendChildElement(new StkObject(L"MsgEng", MessageProc::GetMsgEng(ErrId)));
	ErrResObj->AppendChildElement(new StkObject(L"MsgJpn", MessageProc::GetMsgJpn(ErrId)));
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

int StkWebApp::Impl::AddReqHandler(int Method, const wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], StkWebAppExec* HandlerObj, bool SimpleMode)
{
	ReqHandlerCs.lock();
	for (int Loop = 0; Loop < HandlerCount; Loop++) {
		if (Method == HandlerMethod[Loop] && StkPlWcsCmp(UrlPath, HandlerUrlPath[Loop]) == 0) {
			ReqHandlerCs.unlock();
			return -1;
		}
	}
	HandlerMethod[HandlerCount] = Method;
	StkPlWcsCpy(HandlerUrlPath[HandlerCount], StkWebAppExec::URL_PATH_LENGTH, UrlPath);
	Handler[HandlerCount] = HandlerObj;
	HandlerSimpleMode[HandlerCount] = SimpleMode;
	HandlerCount++;
	ReqHandlerCs.unlock();
	return HandlerCount;
}

int StkWebApp::Impl::DeleteReqHandler(int Method, const wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH])
{
	ReqHandlerCs.lock();
	for (int Loop = 0; Loop < HandlerCount; Loop++) {
		if (Method != HandlerMethod[Loop] || StkPlWcsCmp(UrlPath, HandlerUrlPath[Loop]) != 0) {
			continue;
		}
		HandlerMethod[Loop] = StkWebAppExec::STKWEBAPP_METHOD_UNDEFINED;
		StkPlWcsCpy(HandlerUrlPath[Loop], StkWebAppExec::URL_PATH_LENGTH, L"");
		delete Handler[Loop];
		for (int Loop2 = Loop; Loop2 < HandlerCount - 1; Loop2++) {
			HandlerMethod[Loop2] = HandlerMethod[Loop2 + 1];
			StkPlWcsCpy(HandlerUrlPath[Loop2], StkWebAppExec::URL_PATH_LENGTH, HandlerUrlPath[Loop2 + 1]);
			Handler[Loop2] = Handler[Loop2 + 1];
		}
		HandlerCount--;
		ReqHandlerCs.unlock();
		return HandlerCount;
	}
	ReqHandlerCs.unlock();
	return -1;
}

bool StkWebApp::Contains(int ThreadId)
{
	for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
		if (pImpl->WebThreadIds[Loop] == ThreadId) {
			return true;
		}
	}
	return false;
}

void StkWebApp::TheLoop()
{
	while (true) {
		if (pImpl->StopFlag) {
			StkPlSleepMs(100);
			break;
		}
		StkPlSleepMs(100);
	}
}

int StkWebApp::ThreadLoop(int ThreadId)
{
	int XmlJsonType = XMLJSONTYPE_EMPTYSTR;
	int Method;
	wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH];
	int ResultCode = 200;
	wchar_t HttpHeader[MAX_HTTPHEADERSIZE] = L"";

	// Acquire HTTP header
	int ContLen = pImpl->RecvRequestHeader(ThreadId, &Method, UrlPath, HttpHeader);
	if (ContLen == -1 && Method == StkWebAppExec::STKWEBAPP_METHOD_UNDEFINED && StkPlWcsCmp(UrlPath, L"") == 0) {
		return 0;
	} else if (ContLen == -2) {
		ResultCode = 400;
		StkObject* ErrorObj = pImpl->MakeErrorResponse(1003);
		pImpl->SendResponse(ErrorObj, ThreadId, XMLJSONTYPE_JSON, ResultCode);
		delete ErrorObj;
		return 0;
	} else if (ContLen == -3 || ContLen == -4) {
		ResultCode = 400;
		StkObject* ErrorObj = pImpl->MakeErrorResponse(1005);
		pImpl->SendResponse(ErrorObj, ThreadId, XMLJSONTYPE_JSON, ResultCode);
		delete ErrorObj;
		return 0;
	}

	StkObject* StkObjReq = NULL;
	if (ContLen > 0) {
		StkObjReq = pImpl->RecvRequest(ThreadId, &XmlJsonType, ContLen, HttpHeader);
	}

	// If a request is received...
	StkObject* StkObjRes = NULL;
	bool FndFlag = false;
	if (XmlJsonType == XMLJSONTYPE_EMPTYSTR || XmlJsonType == XMLJSONTYPE_JSON) {
		// If valid request is received...
		for (int Loop = 0; Loop < pImpl->HandlerCount; Loop++) {
			wchar_t Param[4][StkWebAppExec::URL_PATH_LENGTH] = {L"", L"", L"", L""};
			int MethodCalRes = (Method & pImpl->HandlerMethod[Loop]) | (Method & StkWebAppExec::STKWEBAPP_METHOD_OPTIONS);
			int FindRes = StkStringParser::ParseInto4Params(UrlPath, pImpl->HandlerUrlPath[Loop], L'$', Param[0], Param[1], Param[2], Param[3]);
			if (MethodCalRes && FindRes == 1) {
				if (Method == StkWebAppExec::STKWEBAPP_METHOD_OPTIONS) {
					StkObjRes = NULL;
					ResultCode = 200;
				} else {
					StkObjRes = pImpl->Handler[Loop]->Execute(StkObjReq, Method, UrlPath, &ResultCode, HttpHeader);
				}
				FndFlag = true;
				break;
			}
		}
	} else {
		// If invalid request is received...
		if (Method == StkWebAppExec::STKWEBAPP_METHOD_INVALID) {
			ResultCode = 400;
			StkObjRes = pImpl->MakeErrorResponse(1005);
		} else {
			ResultCode = 400;
			StkObjRes = pImpl->MakeErrorResponse(1002);
		}
		FndFlag = true;
	}

	// If service stop request is presented...
	if (FndFlag == false && Method == StkWebAppExec::STKWEBAPP_METHOD_POST && StkPlWcsCmp(UrlPath, L"/service/") == 0) {
		int ErrorCode;
		StkObject* TmpObj = StkObject::CreateObjectFromJson(L"{ \"Operation\" : \"Stop\" }", &ErrorCode);
		if (StkObjReq != NULL && StkObjReq->Equals(TmpObj) == true) {
			ResultCode = 202;
			FndFlag = true;
			pImpl->StopFlag = true;
		} else {
			ResultCode = 400;
			StkObjRes = pImpl->MakeErrorResponse(1004);
			FndFlag = true;
		}
		delete TmpObj;
	}

	// Corresponding API is not found.
	if (FndFlag == false) {
		ResultCode = 404;
		StkObjRes = pImpl->MakeErrorResponse(1001);
	}

	// Reset "XmlJsonType"
	if (StkObjRes == NULL && XmlJsonType == XMLJSONTYPE_JSON) {
		XmlJsonType = XMLJSONTYPE_EMPTYSTR;
	}
	if (StkObjRes != NULL && (XmlJsonType == XMLJSONTYPE_INVALID || XmlJsonType == XMLJSONTYPE_EMPTYSTR || XmlJsonType == XMLJSONTYPE_XML)) {
		XmlJsonType = XMLJSONTYPE_JSON;
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
		if (StkWebAppArray[Loop]->Contains(ThreadId) == true) {
			return StkWebAppArray[Loop];
		}
	}
	return NULL;
}

StkWebApp::StkWebApp(int* TargetIds, int Count, const wchar_t* HostName, int TargetPort, const char* PrivateKey, const char* Certificate)
{
	pImpl = new Impl;
	pImpl->WebThreadCount = 0;
	pImpl->HandlerCount = 0;
	pImpl->StopFlag = false;
	pImpl->SendBufSize = 1000000;
	pImpl->RecvBufSize = 1000000;
	pImpl->TimeoutInterval = 3000;

	// Message definition
	MessageProc::AddJpn(1001, L"クライアントからのリクエストに対応するAPIは定義されていません。");
	MessageProc::AddEng(1001, L"No API is defined for the request sent from client.");
	MessageProc::AddJpn(1002, L"リクエストがJSONではないデータを含んでいるかHTTPヘッダのContent-Typeがapplication/jsonではありません。");
	MessageProc::AddEng(1002, L"The request contains non-JSON data or Content-Type of HTTP header is not application/json.");
	MessageProc::AddJpn(1003, L"HTTPヘッダサイズが許容できる上限を超えました。");
	MessageProc::AddEng(1003, L"HTTP header size exceeds the upper limit.");
	MessageProc::AddJpn(1004, L"URL\"/service/\"にPOSTされたリクエストは不正です。");
	MessageProc::AddEng(1004, L"An invalid request is posted to URL\"/service/\".");
	MessageProc::AddJpn(1005, L"不正なリクエストを受信しました。リクエストが壊れているおそれがあります。");
	MessageProc::AddEng(1005, L"An invalid request is received. The request might be broken.");
	MessageProc::AddJpn(1006, L"データサイズが送信できるサイズの上限を超えました。");
	MessageProc::AddEng(1006, L"Data size has exceeded the limit for sending.");

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
		if (PrivateKey != NULL && Certificate != NULL) {
			StkSocket_InitSecureSetting();
		}
		StkSocket_AddInfo(pImpl->WebThreadIds[0], STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, HostName, TargetPort);
		if (PrivateKey != NULL && Certificate != NULL) {
			StkSocket_SecureForRecv(pImpl->WebThreadIds[0], PrivateKey, Certificate);
		}
		for (int Loop = 1; Loop < pImpl->WebThreadCount; Loop++) {
			StkSocket_CopyInfo(pImpl->WebThreadIds[Loop], pImpl->WebThreadIds[0]);
		}
		StkSocket_Open(pImpl->WebThreadIds[0]);
	}

	// Add threads
	wchar_t Name[32];
	wchar_t Desc[32];
	for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
		StkPlSwPrintf(Name, 32, L"Recv-%d", Loop);
		StkPlSwPrintf(Desc, 32, L"Worker thread", Loop);
		AddStkThread(pImpl->WebThreadIds[Loop], Name, Desc, NULL, NULL, Impl::ElemStkThreadMainRecv, NULL, NULL);
		SetStkThreadInterval(pImpl->WebThreadIds[Loop], 1);
	}

	// Start threads
	StartSpecifiedStkThreads(pImpl->WebThreadIds, pImpl->WebThreadCount);
	bool ThreadsStarted = false;
	while (ThreadsStarted == false) {
		StkPlSleepMs(100);
		ThreadsStarted = true;
		for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
			if (GetStkThreadStatus(pImpl->WebThreadIds[Loop]) != STKTHREAD_STATUS_RUNNING) {
				ThreadsStarted = false;
			}
		}
	}
}

StkWebApp::~StkWebApp()
{
	// Close and delete socket
	if (pImpl->WebThreadCount >= 1) {
		for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
			StkSocket_Close(pImpl->WebThreadIds[Loop], true);
			StkSocket_DeleteInfo(pImpl->WebThreadIds[Loop]);
		}
	}

	// Stop threads
	StopSpecifiedStkThreads(pImpl->WebThreadIds, pImpl->WebThreadCount);
	bool ThreadsStopped = false;
	while (ThreadsStopped == false) {
		StkPlSleepMs(100);
		ThreadsStopped = true;
		for (int Loop = 0; Loop < pImpl->WebThreadCount; Loop++) {
			if (GetStkThreadStatus(pImpl->WebThreadIds[Loop]) != STKTHREAD_STATUS_READY) {
				ThreadsStopped = false;
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

	delete pImpl;
};

int StkWebApp::AddReqHandler(int Method, const wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], StkWebAppExec* HandlerObj, bool SimpleMode)
{
	return pImpl->AddReqHandler(Method, UrlPath, HandlerObj, SimpleMode);
}

int StkWebApp::DeleteReqHandler(int Method, const wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH])
{
	return pImpl->DeleteReqHandler(Method, UrlPath);
}

int StkWebApp::GetSendBufSize()
{
	return pImpl->SendBufSize;
}

void StkWebApp::SetSendBufSize(int Size)
{
	pImpl->SendBufSize = Size;
}

int StkWebApp::GetRecvBufSize()
{
	return pImpl->RecvBufSize;
}

void StkWebApp::SetRecvBufSize(int Size)
{
	pImpl->RecvBufSize = Size;
}

int StkWebApp::GetTimeoutInterval()
{
	return pImpl->TimeoutInterval;
}

void StkWebApp::SetTimeoutInterval(int Interval)
{
	pImpl->TimeoutInterval = Interval;
}

#include "../stkpl/StkPl.h"
#include "../commonfunc/StkObject.h"
#include "../commonfunc/StkStringParser.h"
#include "../stksocket/stksocket.h"
#include "StkWebAppSend.h"

class StkWebAppSend::Impl
{
public:
	int SendBufSize;
	int RecvBufSize;

	int TimeoutInterval;

	int MyId;

	char* Auth;

public:
	const wchar_t* SkipHttpHeader(const wchar_t*);
	StkObject* RecvResponse(int, wchar_t[1024]);
	int SendRequest(int, int, const char*, StkObject*);
};

const wchar_t* StkWebAppSend::Impl::SkipHttpHeader(const wchar_t* Txt)
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

StkObject* StkWebAppSend::Impl::RecvResponse(int TargetId, wchar_t Header[1024])
{
	unsigned char *Dat = new unsigned char[RecvBufSize];
	int Ret = StkSocket_Receive(TargetId, TargetId, Dat, RecvBufSize, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, TimeoutInterval, NULL, -1);
	if (Ret == 0 || Ret == -1 || Ret == -2) {
		delete Dat;
		return NULL;
	}
	if (Ret >= RecvBufSize) {
		Dat[RecvBufSize - 1] = '\0';
	} else {
		Dat[Ret] = '\0';
	}
	wchar_t *DatWc = StkPlCreateWideCharFromUtf8((char*)Dat);
	delete Dat;
	if (DatWc == NULL) {
		return NULL;
	}

	// Acquire HTTP header
	const wchar_t* Req = SkipHttpHeader(DatWc);
	if (Req == DatWc || Req - DatWc >= 1024 - 1) {
		delete DatWc;
		return NULL;
	}
	StkPlWcsNCpy(Header, 1024, DatWc, Req - DatWc);
	int Tmp;
	StkObject* ReqObj = StkObject::CreateObjectFromJson(Req, &Tmp);

	delete DatWc;
	return ReqObj;
}

int StkWebAppSend::Impl::SendRequest(int TargetId, int Method, const char* Url, StkObject* ReqObj)
{
	// JSON conversion and UTF-8 conversion
	wchar_t* WDat = new wchar_t[SendBufSize];
	StkPlWcsCpy(WDat, SendBufSize, L"");
	int Length = 0;
	if (ReqObj != NULL) {
		ReqObj->ToJson(WDat, SendBufSize);
	}
	if (Length == SendBufSize - 1) {
		delete WDat;
		return -1;
	}
	char* Dat = StkPlCreateUtf8FromWideChar(WDat);
	size_t DatLength = StkPlStrLen(Dat);
	delete WDat;

	// Making HTTP header
	char HttpHeader[1024] = "";

	if (Method == STKWEBAPP_METHOD_GET) {
		StkPlStrCat(HttpHeader, 1024, "GET ");
	} else if (Method == STKWEBAPP_METHOD_POST) {
		StkPlStrCat(HttpHeader, 1024, "POST ");
	} else if (Method == STKWEBAPP_METHOD_PUT) {
		StkPlStrCat(HttpHeader, 1024, "PUT ");
	} else if (Method == STKWEBAPP_METHOD_DELETE) {
		StkPlStrCat(HttpHeader, 1024, "DELETE ");
	} else if (Method == STKWEBAPP_METHOD_HEAD) {
		StkPlStrCat(HttpHeader, 1024, "HEAD ");
	} else if (Method == STKWEBAPP_METHOD_OPTIONS) {
		StkPlStrCat(HttpHeader, 1024, "OPTIONS ");
	}
	StkPlStrCat(HttpHeader, 1024, Url);
	StkPlStrCat(HttpHeader, 1024, " HTTP/1.1\r\n");
	if (DatLength != 0) {
		char HttpHeaderContLen[64] = "";
		StkPlSPrintf(HttpHeaderContLen, 64, "Content-Length: %d\r\n", DatLength);
		StkPlStrCat(HttpHeader, 1024, HttpHeaderContLen);
	}
	if (Dat != NULL && *Dat != L'\0') {
		StkPlStrCat(HttpHeader, 1024, "Content-Type: application/json\r\n");
	}
	{
		char Date[64] = "";
		char DateTmp[64] = "";
		StkPlGetTimeInRfc2822(DateTmp, false);
		StkPlSPrintf(Date, 64, "Date: %s\r\n", DateTmp);
		StkPlStrCat(HttpHeader, 1024, Date);
	}
	{
		int SockType = 0;
		int ActionType = 0;
		wchar_t TargetAddr[256] = L"";
		int TargetPort = 0;
		bool CopiedFlag = true;
		if (StkSocket_GetInfo(TargetId, &SockType, &ActionType, TargetAddr, &TargetPort, &CopiedFlag) == 0) {
			char HostHeader[512] = "";
			char* TargetAddru8 = StkPlCreateUtf8FromWideChar(TargetAddr);
			StkPlSPrintf(HostHeader, 512, "Host: %s\r\n", TargetAddru8);
			StkPlStrCat(HttpHeader, 1024, HostHeader);
			delete TargetAddru8;
		}
	}
	{
		if (Auth != NULL) {
			StkPlStrCat(HttpHeader, 1024, "Authorization: ");
			StkPlStrCat(HttpHeader, 1024, Auth);
			StkPlStrCat(HttpHeader, 1024, "\r\n");
		}
	}
	StkPlStrCat(HttpHeader, 1024, "\r\n");
	int HeaderLength = (int)StkPlStrLen(HttpHeader);

	// Making request data and send
	int ReqDatLength = (int)(DatLength + HeaderLength);
	if (ReqDatLength >= SendBufSize) {
		delete Dat;
		return -1;
	}
	char* ReqDat = new char[ReqDatLength + 1];
	StkPlStrCpy(ReqDat, ReqDatLength + 1, "");
	StkPlStrCat(ReqDat, ReqDatLength + 1, HttpHeader);
	StkPlStrCat((char*)ReqDat, ReqDatLength + 1, Dat);
	int RetD = StkSocket_Send(TargetId, TargetId, (unsigned char*)ReqDat, ReqDatLength);

	// Delete array
	delete Dat;
	delete ReqDat;
	return RetD;
}

StkWebAppSend::StkWebAppSend(int TargetId, const wchar_t* HostNameOrIpAddr, int PortNum)
{
	pImpl = new Impl;
	pImpl->SendBufSize = 1000000;
	pImpl->RecvBufSize = 1000000;
	pImpl->TimeoutInterval = 3000;
	pImpl->MyId = TargetId;
	pImpl->Auth = NULL;
	StkSocket_AddInfo(pImpl->MyId, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, HostNameOrIpAddr, PortNum);
}

StkWebAppSend::~StkWebAppSend()
{
	StkSocket_DeleteInfo(pImpl->MyId);
	if (pImpl->Auth != NULL) {
		delete pImpl->Auth;
	}
	delete pImpl;
};

StkObject* StkWebAppSend::SendRequestRecvResponse(int Method, const char* Url, StkObject* ReqObj, int* ResultCode)
{
	int RetConn = StkSocket_Connect(pImpl->MyId);
	if (RetConn != 0) {
		*ResultCode = -1;
		return NULL;
	}

	StkObject* ResDat = NULL;
	wchar_t Header[1024] = L"";
	int RetSend = pImpl->SendRequest(pImpl->MyId, Method, Url, ReqObj);
	if (RetSend >= 0) {
		ResDat = pImpl->RecvResponse(pImpl->MyId, Header);
		*ResultCode = -1;
		wchar_t StrResultCode[16] = L"";
		StkStringParser::ParseInto3Params(Header, L"$ $ $", L'$', NULL, -1, StrResultCode, 16, NULL, -1);
		if (StrResultCode != NULL) {
			*ResultCode = StkPlWcsToL(StrResultCode);
		}
	} else {
		*ResultCode = -1;
	}

	StkSocket_Disconnect(pImpl->MyId, pImpl->MyId, true);

	return ResDat;
}

int StkWebAppSend::GetSendBufSize()
{
	return pImpl->SendBufSize;
}

void StkWebAppSend::SetSendBufSize(int Size)
{
	pImpl->SendBufSize = Size;
}

int StkWebAppSend::GetRecvBufSize()
{
	return pImpl->RecvBufSize;
}

void StkWebAppSend::SetRecvBufSize(int Size)
{
	pImpl->RecvBufSize = Size;
}

int StkWebAppSend::GetTimeoutInterval()
{
	return pImpl->TimeoutInterval;
}

void StkWebAppSend::SetTimeoutInterval(int Interval)
{
	pImpl->TimeoutInterval = Interval;
}

void StkWebAppSend::SetAutholization(const char* Auth)
{
	if (pImpl->Auth != NULL) {
		delete pImpl->Auth;
	}
	pImpl->Auth = new char[256];
	StkPlStrCpy(pImpl->Auth, 256, Auth);
}

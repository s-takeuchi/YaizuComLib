#include "../stkpl/StkPl.h"
#include "../commonfunc/StkObject.h"
#include "../stksocket/stksocket.h"
#include "StkWebAppSend.h"

class StkWebAppSend::Impl
{
public:
	int SendBufSize;
	int RecvBufSize;

	int TimeoutInterval;

public:
	const wchar_t* SkipHttpHeader(const wchar_t*);
	StkObject* RecvResponse(int, wchar_t[1024], int*);
	int SendRequest(int, int, char*, StkObject*);
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

StkObject* StkWebAppSend::Impl::RecvResponse(int TargetId, wchar_t Header[1024], int* ResultCode)
{
	unsigned char *Dat = new unsigned char[RecvBufSize];
	int Ret = StkSocket_Receive(TargetId, TargetId, Dat, RecvBufSize, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, TimeoutInterval, NULL, -1);
	if (Ret == 0 || Ret == -1 || Ret == -2) {
		delete Dat;
		*ResultCode = -1;
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
		*ResultCode = -1;
		delete DatWc;
		return NULL;
	}
	StkPlWcsNCpy(Header, 1024, DatWc, Req - DatWc);
	StkObject* ReqObj = StkObject::CreateObjectFromJson(Req, ResultCode);

	delete DatWc;
	return ReqObj;
}

int StkWebAppSend::Impl::SendRequest(int TargetId, int Method, char* Url, StkObject* ReqObj)
{
	// JSON conversion and UTF-8 conversion
	wchar_t* WDat = new wchar_t[SendBufSize];
	StkPlWcsCpy(WDat, SendBufSize, L"");
	int Length = ReqObj->ToJson(WDat, SendBufSize);
	if (Length == SendBufSize - 1) {
		delete WDat;
		return -1;
	}
	char* Dat = StkPlCreateUtf8FromWideChar(WDat);
	size_t DatLength = StkPlStrLen(Dat);
	delete WDat;
	if (Dat == NULL) {
		return -1;
	}

	// Making HTTP header
	char HttpHeader[1024] = "";
	char HttpHeaderContLen[64] = "";
	if (Method == STKWEBAPP_METHOD_GET) {
		StkPlStrCat(HttpHeader, 1024, "GET ");
	} else if (Method == STKWEBAPP_METHOD_POST) {
		StkPlStrCat(HttpHeader, 1024, "POST ");
	} else if (Method == STKWEBAPP_METHOD_PUT) {
		StkPlStrCat(HttpHeader, 1024, "PUT ");
	} else if (Method == STKWEBAPP_METHOD_DELETE) {
		StkPlStrCat(HttpHeader, 1024, "DELETE ");
	}
	StkPlStrCat(HttpHeader, 1024, Url);
	StkPlStrCat(HttpHeader, 1024, " HTTP/1.1\r\n");
	StkPlSPrintf(HttpHeaderContLen, 64, "Content-Length: %d\r\n", DatLength);
	StkPlStrCat(HttpHeader, 1024, HttpHeaderContLen);
	StkPlStrCat(HttpHeader, 1024, "\r\n");
	int HeaderLength = StkPlStrLen(HttpHeader);

	// Making request data and send
	int ReqDatLength = DatLength + HeaderLength;
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

StkWebAppSend::StkWebAppSend()
{
	pImpl = new Impl;
	pImpl->SendBufSize = 1000000;
	pImpl->RecvBufSize = 1000000;
	pImpl->TimeoutInterval = 3000;
}

StkWebAppSend::~StkWebAppSend()
{
	delete pImpl;
};

StkObject* StkWebAppSend::SendRequestRecvResponse(wchar_t* HostNameOrIpAddr, int PortNum, int Method, char* Url, StkObject* ReqObj)
{
	int TargetId = 0;

	// Calculate TargetId from existing IDs.
	for (int Loop = 0; Loop < StkSocket_GetNumOfStkInfos(); Loop++) {
		int CurrentId = -1;
		int SockType = -1;
		int ActionType = -1;
		wchar_t TargetAddr[256] = L"";
		int TargetPort = -1;
		bool CopiedFlag = false;
		StkSocket_GetInfo(Loop, &CurrentId, &SockType, &ActionType, TargetAddr, &TargetPort, &CopiedFlag);
		if (CurrentId >= TargetId) {
			TargetId = CurrentId + 1;
		}
	}

	StkSocket_AddInfo(TargetId, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, HostNameOrIpAddr, PortNum);
	StkSocket_Connect(TargetId);

	StkObject* ResDat = NULL;
	wchar_t Header[1024];
	int ResultCode;
	int RetSend = pImpl->SendRequest(TargetId, Method, Url, ReqObj);
	if (RetSend >= 0) {
		ResDat = pImpl->RecvResponse(TargetId, Header, &ResultCode);
	}

	StkSocket_Disconnect(TargetId, TargetId, true);
	StkSocket_DeleteInfo(TargetId);
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

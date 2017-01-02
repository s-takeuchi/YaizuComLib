#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include "..\stksocket\stksocket.h"
#include "..\commonfunc\stkobject.h"
#include "StkObjectConverter.h"

#define DATA_LEN 10000000

class StkObjectConverter::Impl
{
public:
	TCHAR* SkipHttpHeader(TCHAR*);

	TCHAR* Uft8ToWideChar(BYTE* Txt);
	BYTE* WideCharToUtf8(TCHAR*);
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

StkObjectConverter::StkObjectConverter(int* TargetIds, int Count, TCHAR* HostName, int TargetPort)
{
	pImpl = new Impl;

	if (Count >= 1) {
		StkSocket_AddInfo(TargetIds[0], SOCK_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, HostName, TargetPort);
		for (int Loop = 1; Loop < Count; Loop++) {
			StkSocket_CopyInfo(TargetIds[Loop], TargetIds[0]);
		}
		StkSocket_Open(TargetIds[0]);
	}
};

StkObjectConverter::~StkObjectConverter()
{
	delete pImpl;
};

void StkObjectConverter::AllClose(int* TargetIds, int Count)
{
	if (Count >= 1) {
		for (int Loop = 0; Loop < Count; Loop++) {
			StkSocket_Close(TargetIds[Loop], TRUE);
		}
	}
};

StkObject* StkObjectConverter::RecvRequest(int TargetId, int* XmlJsonType)
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
	TCHAR *DatWc = pImpl->Uft8ToWideChar(Dat);
	if (DatWc == NULL) {
		StkSocket_CloseAccept(TargetId, TargetId, TRUE);
		return NULL;
	}
	TCHAR* Req = pImpl->SkipHttpHeader(DatWc);
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

void StkObjectConverter::SendResponse(StkObject* Obj, int TargetId, int XmlJsonType)
{
	if (XmlJsonType != 1 && XmlJsonType != 2) {
		return;
	}
	char ContType[64] = "";
	BYTE* Dat;
	std::wstring XmlOrJson;
	if (XmlJsonType == 1) {
		Obj->ToXml(&XmlOrJson);
		strcpy_s(ContType, 64, "Content-Type: application/xml\r\n\r\n");
	} else if (XmlJsonType == 2) {
		Obj->ToJson(&XmlOrJson);
		strcpy_s(ContType, 64, "Content-Type: application/json\r\n\r\n");
	}
	Dat = pImpl->WideCharToUtf8((TCHAR*)XmlOrJson.c_str());
	int Ret1 = StkSocket_Send(TargetId, TargetId, (BYTE*)ContType, strlen(ContType));
	int Ret2 = StkSocket_Send(TargetId, TargetId, Dat, strlen((char*)Dat) + 1);
	delete Dat;
	StkSocket_CloseAccept(TargetId, TargetId, TRUE);
};

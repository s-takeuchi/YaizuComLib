#include <windows.h>
#include "..\stksocket\stksocket.h"
#include "..\commonfunc\stkobject.h"
#include "StkObjectConverter.h"

class StkObjectConverter::Impl
{
public:
	int DeleteHttpHeader(TCHAR*);
	int Utf8ToWideCharSize(BYTE*);
	void Utf8ToWideChar(BYTE*, TCHAR*, int);
};

// Delete the http header from the specified text.
// Txt [out] : Header deleted data
// Return : Result code [0: Normal, -1: No header/Abnormal
int StkObjectConverter::Impl::DeleteHttpHeader(TCHAR* Txt)
{
	// Search the header end
	return 0;
}

int StkObjectConverter::Impl::Utf8ToWideCharSize(BYTE* Txt)
{
	return MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)Txt, -1, NULL, NULL);
}

void StkObjectConverter::Impl::Utf8ToWideChar(BYTE* Txt, TCHAR* WcTxt, int WcSize)
{
	MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)Txt, -1, WcTxt, WcSize);
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
	BYTE Dat[10000000];
	Ret = StkSocket_Receive(TargetId, TargetId, Dat, 10000000, 9999999, NULL, -1, FALSE);
	if (Ret == -1) {
		return NULL;
	}
	int WcSize = pImpl->Utf8ToWideCharSize(Dat);
	if (WcSize <= 0) {
		TCHAR *DatWc = new TCHAR[WcSize + 1];
		pImpl->Utf8ToWideChar(Dat, DatWc, WcSize + 1);
	}
	//*XmlJsonType = StkObject::Analyze(
	return NULL;
};

void StkObjectConverter::SendResponse(StkObject* Obj, int TargetId, int* XmlJsonType)
{
};

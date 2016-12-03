#include "..\stksocket\stksocket.h"
#include "StkObjectConverter.h"

StkObjectConverter::StkObjectConverter(int* TargetIds, int Count, TCHAR* HostName, int TargetPort)
{
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
};

void StkObjectConverter::AllClose(int* TargetIds, int Count)
{
	if (Count >= 1) {
		for (int Loop = 0; Loop < Count; Loop++) {
			StkSocket_Close(TargetIds[Loop], TRUE);
		}
	}
};

StkObject* StkObjectConverter::RecvRequest(int TargetId)
{
	int Ret = StkSocket_Accept(TargetId);
	if (Ret == -1) {
		return NULL;
	}
	BYTE Dat[10000000];
	Ret = StkSocket_Receive(TargetId, TargetId, Dat, 10000000, 9999999, NULL, -1, FALSE);
	return NULL;
};

void StkObjectConverter::SendResponse(StkObject* Obj, int TargetId)
{
};

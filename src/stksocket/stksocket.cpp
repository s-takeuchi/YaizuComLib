#include "StkSocketMgr.h"

int StkSocket_AddInfo(int TargetId, int SockType, int ActionType, const wchar_t TargetAddr[256], int TargetPort)
{
	return StkSocketMgr::GetInstance()->AddSocketInfo(TargetId, SockType, ActionType, TargetAddr, TargetPort);
}

int StkSocket_DeleteInfo(int TargetId)
{
	return StkSocketMgr::GetInstance()->DeleteSocketInfo(TargetId);
}

int StkSocket_GetInfo(int Index, int* TargetId, int* SockType, int* ActionType, wchar_t TargetAddr[256], int* TargetPort, bool* CopiedFlag)
{
	return StkSocketMgr::GetInstance()->GetSocketInfo(Index, TargetId, SockType, ActionType, TargetAddr, TargetPort, CopiedFlag);
}

int StkSocket_GetInfo(int TargetId, int* SockType, int* ActionType, wchar_t TargetAddr[256], int* TargetPort, bool* CopiedFlag)
{
	return StkSocketMgr::GetInstance()->GetSocketInfo(TargetId, SockType, ActionType, TargetAddr, TargetPort, CopiedFlag);
}

int StkSocket_CopyInfo(int NewId, int ExistingId)
{
	return StkSocketMgr::GetInstance()->CopySocketInfo(NewId, ExistingId);
}

void StkSocket_InitSecureSetting()
{
	return StkSocketMgr::GetInstance()->InitSecureSetting();
}

int StkSocket_SecureForRecv(int TargetId, const char* PrivateKey, const char* Certificate)
{
	return StkSocketMgr::GetInstance()->SecureForRecv(TargetId, PrivateKey, Certificate);
}

int StkSocket_SecureForSend(int TargetId, const char* FileName, const char* Path)
{
	return StkSocketMgr::GetInstance()->SecureForSend(TargetId, FileName, Path);
}

int StkSocket_GetSecureStatus(int TargetId)
{
	return StkSocketMgr::GetInstance()->GetSecureStatus(TargetId);
}

int StkSocket_Unsecure(int TargetId)
{
	return StkSocketMgr::GetInstance()->Unsecure(TargetId);
}

int StkSocket_Open(int TargetId)
{
	return StkSocketMgr::GetInstance()->OpenSocket(TargetId);
}

int StkSocket_Close(int TargetId, bool WaitForPeerClose)
{
	return StkSocketMgr::GetInstance()->CloseSocket(TargetId, WaitForPeerClose);
}

int StkSocket_Connect(int Id)
{
	return StkSocketMgr::GetInstance()->ConnectSocket(Id);
}

int StkSocket_Disconnect(int Id, int LogId, bool WaitForPeerClose)
{
	return StkSocketMgr::GetInstance()->DisconnectSocket(Id, LogId, WaitForPeerClose);
}

int StkSocket_Accept(int Id)
{
	return StkSocketMgr::GetInstance()->Accept(Id);
}

int StkSocket_CloseAccept(int Id, int LogId, bool WaitForPeerClose)
{
	return StkSocketMgr::GetInstance()->CloseAccept(Id, LogId, WaitForPeerClose);
}

int StkSocket_Receive(int Id, int LogId, unsigned char* Buffer, int BufferSize, int FinishCondition, int FinishCondTimeout, unsigned char* VarDat, int VarDatSize)
{
	return StkSocketMgr::GetInstance()->Receive(Id, LogId, Buffer, BufferSize, FinishCondition, FinishCondTimeout, VarDat, VarDatSize);
}

int StkSocket_Send(int Id, int LogId, const unsigned char* Buffer, int BufferSize)
{
	return StkSocketMgr::GetInstance()->Send(Id, LogId, Buffer, BufferSize);
}

int StkSocket_ReceiveUdp(int Id, int LogId, unsigned char* Buffer, int BufferSize)
{
	return StkSocketMgr::GetInstance()->ReceiveUdp(Id, LogId, Buffer, BufferSize);
}

int StkSocket_SendUdp(int Id, int LogId, const unsigned char* Buffer, int BufferSize)
{
	return StkSocketMgr::GetInstance()->SendUdp(Id, LogId, Buffer, BufferSize);
}

int StkSocket_GetUdpMaxMessageSize(int Id)
{
	return StkSocketMgr::GetInstance()->GetUdpMaxMessageSize(Id);
}

int StkSocket_ForceStop(int Id)
{
	return StkSocketMgr::GetInstance()->ForceStop(Id);
}

int StkSocket_ClearForceStop(int Id) {
	return StkSocketMgr::GetInstance()->ClearForceStop(Id);
}


int StkSocket_GetNumOfStkInfos()
{
	return StkSocketMgr::GetInstance()->GetNumOfStkInfos();
}

void StkSocket_TakeLastLog(int* TmpLog, int* TmpLogId, wchar_t* TmpLogParamStr1, wchar_t* TmpLogParamStr2, int* TmpLogParamInt1, int* TmpLogParamInt2)
{
	return StkSocketMgr::GetInstance()->TakeLastLog(TmpLog, TmpLogId, TmpLogParamStr1, TmpLogParamStr2, TmpLogParamInt1, TmpLogParamInt2);
}

void StkSocket_TakeFirstLog(int* TmpLog, int* TmpLogId, wchar_t* TmpLogParamStr1, wchar_t* TmpLogParamStr2, int* TmpLogParamInt1, int* TmpLogParamInt2)
{
	return StkSocketMgr::GetInstance()->TakeFirstLog(TmpLog, TmpLogId, TmpLogParamStr1, TmpLogParamStr2, TmpLogParamInt1, TmpLogParamInt2);
}

int StkSocket_GetNumOfLogs()
{
	return StkSocketMgr::GetInstance()->GetNumOfLogs();
}

int StkSocket_GetStatus(int TargetId)
{
	return StkSocketMgr::GetInstance()->GetStatus(TargetId);
}

void StkSocket_ClearLog()
{
	return StkSocketMgr::GetInstance()->ClearLog();
}

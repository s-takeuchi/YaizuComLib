#include "StkSocketMgr.h"

int StkSocket_AddInfo(int TargetId, int SockType, int ActionType, TCHAR TargetAddr[256], int TargetPort)
{
	return StkSocketMgr::GetInstance()->AddSocketInfo(TargetId, SockType, ActionType, TargetAddr, TargetPort);
}

int StkSocket_DeleteInfo(int TargetId)
{
	return StkSocketMgr::GetInstance()->DeleteSocketInfo(TargetId);
}

int StkSocket_CopyInfo(int NewId, int ExistingId)
{
	return StkSocketMgr::GetInstance()->CopySocketInfo(NewId, ExistingId);
}

int StkSocket_Open(int TargetId)
{
	return StkSocketMgr::GetInstance()->OpenSocket(TargetId);
}

int StkSocket_Close(int TargetId, BOOL WaitForPeerClose)
{
	return StkSocketMgr::GetInstance()->CloseSocket(TargetId, WaitForPeerClose);
}

int StkSocket_Connect(int Id)
{
	return StkSocketMgr::GetInstance()->ConnectSocket(Id);
}

int StkSocket_Disconnect(int Id, int LogId, BOOL WaitForPeerClose)
{
	return StkSocketMgr::GetInstance()->DisconnectSocket(Id, LogId, WaitForPeerClose);
}

int StkSocket_Accept(int Id)
{
	return StkSocketMgr::GetInstance()->Accept(Id);
}

int StkSocket_CloseAccept(int Id, int LogId, BOOL WaitForPeerClose)
{
	return StkSocketMgr::GetInstance()->CloseAccept(Id, LogId, WaitForPeerClose);
}

int StkSocket_Receive(int Id, int LogId, BYTE* Buffer, int BufferSize, int FinishCondition, BYTE* VarDat, int VarDatSize, BOOL ForceStop)
{
	return StkSocketMgr::GetInstance()->Receive(Id, LogId, Buffer, BufferSize, FinishCondition, VarDat, VarDatSize, ForceStop);
}

int StkSocket_Send(int Id, int LogId, BYTE* Buffer, int BufferSize)
{
	return StkSocketMgr::GetInstance()->Send(Id, LogId, Buffer, BufferSize);
}

int StkSocket_ReceiveUdp(int Id, int LogId, BYTE* Buffer, int BufferSize)
{
	return StkSocketMgr::GetInstance()->ReceiveUdp(Id, LogId, Buffer, BufferSize);
}

int StkSocket_SendUdp(int Id, int LogId, BYTE* Buffer, int BufferSize)
{
	return StkSocketMgr::GetInstance()->SendUdp(Id, LogId, Buffer, BufferSize);
}

int StkSocket_GetUdpMaxMessageSize(int Id)
{
	return StkSocketMgr::GetInstance()->GetUdpMaxMessageSize(Id);
}

int StkSocket_GetNumOfStkInfos()
{
	return StkSocketMgr::GetInstance()->GetNumOfStkInfos();
}

void StkSocket_TakeLastLog(int* TmpLog, int* TmpLogId, TCHAR* TmpLogParamStr1, TCHAR* TmpLogParamStr2, int* TmpLogParamInt1, int* TmpLogParamInt2)
{
	return StkSocketMgr::GetInstance()->TakeLastLog(TmpLog, TmpLogId, TmpLogParamStr1, TmpLogParamStr2, TmpLogParamInt1, TmpLogParamInt2);
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

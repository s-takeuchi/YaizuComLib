#include <winsock2.h>
#include <Ws2tcpip.h.>
#include <windows.h>
#include <stdio.h>
#include "StkSocketMgr.h"

StkSocketMgr* StkSocketMgr::ThisInstance;

// Get this instance
StkSocketMgr* StkSocketMgr::GetInstance()
{
	static int Init = 1;
	if (Init == 1) {
		Init = 0;
		ThisInstance = new StkSocketMgr();
	}
	return ThisInstance;
}

// Constructor
StkSocketMgr::StkSocketMgr()
{
	NumOfSocketInfo = 0;
	NumOfLogs = 0;

	WSAStartup(MAKEWORD(2,0), &WsaDat);
	InitializeCriticalSection(&Cs4Log);
	ClearLog();
}

// Destructor
StkSocketMgr::~StkSocketMgr()
{
	WSACleanup();
}

void StkSocketMgr::PutLog(int TmpLog, int TmpLogId, TCHAR* TmpLogParamStr1, TCHAR* TmpLogParamStr2, int TmpLogParamInt1, int TmpLogParamInt2)
{
	EnterCriticalSection(&Cs4Log);
	if (NumOfLogs == MAX_NUM_OF_LOG) {
		for (int Loop = 0; Loop < MAX_NUM_OF_LOG - 1; Loop++) {
			Log[Loop] = Log[Loop + 1];
			LogId[Loop] = LogId[Loop + 1];
			lstrcpyn(LogParamStr1[Loop], LogParamStr1[Loop + 1], 256);
			lstrcpyn(LogParamStr2[Loop], LogParamStr2[Loop + 1], 256);
			LogParamInt1[Loop] = LogParamInt1[Loop + 1];
			LogParamInt2[Loop] = LogParamInt2[Loop + 1];
		}
		NumOfLogs--;
	}
	Log[NumOfLogs] = TmpLog;
	LogId[NumOfLogs] = TmpLogId;
	lstrcpyn(LogParamStr1[NumOfLogs], TmpLogParamStr1, 256);
	lstrcpyn(LogParamStr2[NumOfLogs], TmpLogParamStr2, 256);
	LogParamInt1[NumOfLogs] = TmpLogParamInt1;
	LogParamInt2[NumOfLogs] = TmpLogParamInt2;
	NumOfLogs++;
	LeaveCriticalSection(&Cs4Log);
}

void StkSocketMgr::TakeLastLog(int* TmpLog, int* TmpLogId, TCHAR* TmpLogParamStr1, TCHAR* TmpLogParamStr2, int* TmpLogParamInt1, int* TmpLogParamInt2)
{
	if (NumOfLogs <= 0) {
		*TmpLog = 0;
		*TmpLogId = 0;
		*TmpLogParamStr1 = NULL;
		*TmpLogParamStr2 = NULL;
		*TmpLogParamInt1 = 0;
		*TmpLogParamInt2 = 0;
		return;
	}
	EnterCriticalSection(&Cs4Log);
	NumOfLogs--;
	*TmpLog = Log[NumOfLogs];
	*TmpLogId = LogId[NumOfLogs];
	lstrcpyn(TmpLogParamStr1, LogParamStr1[NumOfLogs], 256);
	lstrcpyn(TmpLogParamStr2, LogParamStr2[NumOfLogs], 256);
	*TmpLogParamInt1 = LogParamInt1[NumOfLogs];
	*TmpLogParamInt2 = LogParamInt2[NumOfLogs];

	Log[NumOfLogs] = 0;
	LogId[NumOfLogs] = 0;
	lstrcpy(LogParamStr1[NumOfLogs], _T(""));
	lstrcpy(LogParamStr2[NumOfLogs], _T(""));
	LogParamInt1[NumOfLogs] = 0;
	LogParamInt2[NumOfLogs] = 0;
	LeaveCriticalSection(&Cs4Log);
}

void StkSocketMgr::TakeFirstLog(int* TmpLog, int* TmpLogId, TCHAR* TmpLogParamStr1, TCHAR* TmpLogParamStr2, int* TmpLogParamInt1, int* TmpLogParamInt2)
{
	if (NumOfLogs <= 0) {
		*TmpLog = 0;
		*TmpLogId = 0;
		*TmpLogParamStr1 = NULL;
		*TmpLogParamStr2 = NULL;
		*TmpLogParamInt1 = 0;
		*TmpLogParamInt2 = 0;
		return;
	}
	EnterCriticalSection(&Cs4Log);
	*TmpLog = Log[0];
	*TmpLogId = LogId[0];
	lstrcpyn(TmpLogParamStr1, LogParamStr1[0], 256);
	lstrcpyn(TmpLogParamStr2, LogParamStr2[0], 256);
	*TmpLogParamInt1 = LogParamInt1[0];
	*TmpLogParamInt2 = LogParamInt2[0];

	for (int Loop = 0; Loop < NumOfLogs - 1; Loop++) {
		Log[Loop] = Log[Loop + 1];
		LogId[Loop] = LogId[Loop + 1];
		lstrcpy(LogParamStr1[Loop], LogParamStr1[Loop + 1]);
		lstrcpy(LogParamStr2[Loop], LogParamStr2[Loop + 1]);
		LogParamInt1[Loop] = LogParamInt1[Loop + 1];
		LogParamInt2[Loop] = LogParamInt2[Loop + 1];
	}
	NumOfLogs--;
	Log[NumOfLogs] = 0;
	LogId[NumOfLogs] = 0;
	lstrcpy(LogParamStr1[NumOfLogs], _T(""));
	lstrcpy(LogParamStr2[NumOfLogs], _T(""));
	LogParamInt1[NumOfLogs] = 0;
	LogParamInt2[NumOfLogs] = 0;
	LeaveCriticalSection(&Cs4Log);
}

int StkSocketMgr::GetNumOfLogs()
{
	return NumOfLogs;
}

void StkSocketMgr::ClearLog()
{
	EnterCriticalSection(&Cs4Log);
	for (int Loop = 0; Loop < MAX_NUM_OF_LOG; Loop++) {
		Log[Loop] = 0;
		LogId[Loop] = 0;
		lstrcpy(LogParamStr1[Loop], _T(""));
		lstrcpy(LogParamStr2[Loop], _T(""));
		LogParamInt1[Loop] = 0;
		LogParamInt2[Loop] = 0;
	}
	NumOfLogs = 0;
	LeaveCriticalSection(&Cs4Log);
}

// Add socket information
// TargetId [in] : Target ID
// ActionType [in] : 0:Receiver or 1:Sender
// TargetAddr [in] : Host name or IP address
// TargetPort [in] : Port number
// Return : 0:Success, -1:Failure
int StkSocketMgr::AddSocketInfo(int TargetId, int SockType, int ActionType, TCHAR TargetAddr[256], int TargetPort)
{
	// Check for maximum number of StkSocketInfos
	if (NumOfSocketInfo >= MAX_SOCKET_NUMBER) {
		return -1;
	}
	// Check for duplicated ID
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (TargetId == SocketInfo[Loop].ElementId) {
			return -1;
		}
	}
	// Check whether the specified value is valid action type
	if (ActionType != StkSocketMgr::ACTIONTYPE_RECEIVER && ActionType != StkSocketMgr::ACTIONTYPE_SENDER) {
		return -1;
	}

	SocketInfo[NumOfSocketInfo].SocketType = SockType;
	SocketInfo[NumOfSocketInfo].ActionType = ActionType;
	SocketInfo[NumOfSocketInfo].ElementId = TargetId;
	SocketInfo[NumOfSocketInfo].Status = StkSocketInfo::STATUS_CLOSE;
	SocketInfo[NumOfSocketInfo].Port = TargetPort;
	SocketInfo[NumOfSocketInfo].Sock = NULL;
	SocketInfo[NumOfSocketInfo].AcceptedSock = NULL;
	SocketInfo[NumOfSocketInfo].CopiedSocketFlag = FALSE;
	SocketInfo[NumOfSocketInfo].CopySourceId = -1;
	lstrcpy(SocketInfo[NumOfSocketInfo].HostOrIpAddr, TargetAddr);
	NumOfSocketInfo++;

	return 0;
}

int StkSocketMgr::CopySocketInfo(int NewId, int ExistingId)
{
	// Check for maximum number of StkSocketInfos
	if (NumOfSocketInfo >= MAX_SOCKET_NUMBER) {
		return -1;
	}
	// Check for duplicated ID
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (NewId == SocketInfo[Loop].ElementId) {
			return -1;
		}
	}
	// Check whether the ExistingId exists or not
	int FndIndex = 0;
	for (; FndIndex < NumOfSocketInfo; FndIndex++) {
		if (SocketInfo[FndIndex].ElementId == ExistingId) {
			break;
		}
	}
	if (FndIndex == NumOfSocketInfo) {
		return -1;
	}
	// Check the original socket is not for TCP.
	if (SocketInfo[FndIndex].SocketType != StkSocketMgr::SOCKTYPE_STREAM) {
		return -1;
	}
	// Check the original socket is not a receiver.
	if (SocketInfo[FndIndex].ActionType != StkSocketMgr::ACTIONTYPE_RECEIVER) {
		return -1;
	}
	// Check the original socket is copied socket.
	if (SocketInfo[FndIndex].CopiedSocketFlag == TRUE) {
		return -1;
	}

	SocketInfo[NumOfSocketInfo].SocketType = SocketInfo[FndIndex].SocketType;
	SocketInfo[NumOfSocketInfo].ActionType = SocketInfo[FndIndex].ActionType;
	SocketInfo[NumOfSocketInfo].ElementId = NewId;
	if (SocketInfo[FndIndex].Status == StkSocketInfo::STATUS_CLOSE) {
		SocketInfo[NumOfSocketInfo].Status = StkSocketInfo::STATUS_CLOSE;
	} else {
		SocketInfo[NumOfSocketInfo].Status = StkSocketInfo::STATUS_OPEN;
	}
	SocketInfo[NumOfSocketInfo].Port = SocketInfo[FndIndex].Port;
	SocketInfo[NumOfSocketInfo].Sock = SocketInfo[FndIndex].Sock;
	SocketInfo[NumOfSocketInfo].AcceptedSock = NULL;
	SocketInfo[NumOfSocketInfo].CopiedSocketFlag = TRUE;
	SocketInfo[NumOfSocketInfo].CopySourceId = ExistingId;
	lstrcpy(SocketInfo[NumOfSocketInfo].HostOrIpAddr, SocketInfo[FndIndex].HostOrIpAddr);
	NumOfSocketInfo++;

	return 0;
}

// Delete socket information
// TargetId [in] : Delete target ID
// Return : If there is no socket info, -1 is returned.
//        : If the specified ID does not exist, -1 is also returned.
int StkSocketMgr::DeleteSocketInfo(int TargetId)
{

	// Check for maximum number of StkSocketInfos
	if (NumOfSocketInfo <= 0) {
		return -1;
	}
	// Check the specified ID is existing.
	BOOL FoundFlag = FALSE;
	int Loop;
	for (Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (TargetId == SocketInfo[Loop].ElementId) {
			FoundFlag = TRUE;
			break;
		}
	}
	if (FoundFlag == FALSE) {
		return -1;
	}

	CloseSocket(TargetId, TRUE);
	if (NumOfSocketInfo >= 1) {
		SocketInfo[Loop].SocketType = SocketInfo[NumOfSocketInfo - 1].SocketType;
		SocketInfo[Loop].ActionType = SocketInfo[NumOfSocketInfo - 1].ActionType;
		SocketInfo[Loop].ElementId = SocketInfo[NumOfSocketInfo - 1].ElementId;
		SocketInfo[Loop].Status = SocketInfo[NumOfSocketInfo - 1].Status;
		SocketInfo[Loop].Port = SocketInfo[NumOfSocketInfo - 1].Port;
		SocketInfo[Loop].Sock = SocketInfo[NumOfSocketInfo - 1].Sock;
		SocketInfo[Loop].AcceptedSock = SocketInfo[NumOfSocketInfo - 1].AcceptedSock;
		SocketInfo[Loop].CopiedSocketFlag = SocketInfo[NumOfSocketInfo - 1].CopiedSocketFlag;
		SocketInfo[Loop].CopySourceId = SocketInfo[NumOfSocketInfo - 1].CopySourceId;
		lstrcpy(SocketInfo[Loop].HostOrIpAddr, SocketInfo[NumOfSocketInfo - 1].HostOrIpAddr);
		memcpy(&SocketInfo[Loop].LastAccessedAddr, &SocketInfo[NumOfSocketInfo - 1].LastAccessedAddr, sizeof(sockaddr_in));
	}
	NumOfSocketInfo--;

	return 0;
}

int StkSocketMgr::GetSocketInfo(int Index, int* TargetId, int* SockType, int* ActionType, TCHAR TargetAddr[256], int* TargetPort, BOOL* CopiedFlag)
{
	if (Index < 0 || Index >= NumOfSocketInfo) {
		return -1;
	}
	*TargetId = SocketInfo[Index].ElementId;
	*SockType = SocketInfo[Index].SocketType;
	*ActionType = SocketInfo[Index].ActionType;
	lstrcpy(TargetAddr, SocketInfo[Index].HostOrIpAddr);
	*TargetPort = SocketInfo[Index].Port;
	*CopiedFlag = SocketInfo[Index].CopiedSocketFlag;
	return 0;
}

void StkSocketMgr::CloseSocketWaitForPeerClose(SOCKET Target)
{
	shutdown(Target, SD_SEND);
	int Timeo = 10000;
	setsockopt(Target, SOL_SOCKET, SO_RCVTIMEO, (const char *)&Timeo, sizeof(int));
	while (TRUE) {
		char Buf[10000];
		int Ret = recv(Target, Buf, 10000, 0);
		if (Ret == 0 || Ret == SOCKET_ERROR) {
			break;
		}
	}
	shutdown(Target, SD_BOTH);
	closesocket(Target);
}

int StkSocketMgr::ConnectSocket(int Id)
{
	addrinfo Hints;
	addrinfo* ResAddr = NULL;

	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id && SocketInfo[Loop].Status == StkSocketInfo::STATUS_CLOSE) {
			// If the type is not TCP...
			if (SocketInfo[Loop].SocketType != StkSocketMgr::SOCKTYPE_STREAM &&
				SocketInfo[Loop].SocketType != StkSocketMgr::SOCKTYPE_DGRAM) {
				return -1;
			}

			// Get address information
			memset(&Hints, 0, sizeof(Hints));
			Hints.ai_family = AF_UNSPEC;
			Hints.ai_socktype = SocketInfo[Loop].SocketType;
			Hints.ai_flags = AI_PASSIVE;
			char NodeName[256];
			char ServName[256];
			sprintf_s(NodeName, 256, "%S", SocketInfo[Loop].HostOrIpAddr);
			sprintf_s(ServName, 256, "%d", SocketInfo[Loop].Port);
			int Ret = getaddrinfo(NodeName, ServName, &Hints, &ResAddr);
			if (Ret != 0) {
				PutLog(LOG_NAMESOLVEERR, Id, _T(""), _T(""), 0, WSAGetLastError());
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
				return -1;
			}

			SocketInfo[Loop].Sock = socket(ResAddr->ai_family, ResAddr->ai_socktype, ResAddr->ai_protocol);
			// ソケットのオープンに失敗したら"Continue"
			if (SocketInfo[Loop].Sock == INVALID_SOCKET) {
				freeaddrinfo(ResAddr);
				return -1;
			}

			// For avoiding java connection reset exception
			linger Sol;
			Sol.l_onoff = 1;
			Sol.l_linger = 10;
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_LINGER, (const char *)&Sol, sizeof(Sol));
			// Timeout setting
			int Timeo = 10000;
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&Timeo, sizeof(int));
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&Timeo, sizeof(int));
			// Buffer setting
			int Buffr = 1000000;
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_RCVBUF, (const char *)&Buffr, sizeof(int));
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_SNDBUF, (const char *)&Buffr, sizeof(int));

			if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
				if (connect(SocketInfo[Loop].Sock, ResAddr->ai_addr, ResAddr->ai_addrlen) == SOCKET_ERROR) {
					PutLog(LOG_CONNERROR, Id, _T(""), _T(""), 0, WSAGetLastError());
					SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
					closesocket(SocketInfo[Loop].Sock);
					freeaddrinfo(ResAddr);
					return -1;
				}
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;
				PutLog(LOG_SUCCESSCSC, Id, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
				freeaddrinfo(ResAddr);
				break;
			} else {
				int Yes = 1;
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_BROADCAST, (char *)&Yes, sizeof(Yes));
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;
				PutLog(LOG_SUCCESSCS, Id, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
				freeaddrinfo(ResAddr);
				break;
			}
		}
	}
	return 0;
}

int StkSocketMgr::DisconnectSocket(int Id, int LogId, BOOL WaitForPeerClose)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id && SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN) {
			// If the type is not TCP...
			if (SocketInfo[Loop].SocketType != StkSocketMgr::SOCKTYPE_STREAM &&
				SocketInfo[Loop].SocketType != StkSocketMgr::SOCKTYPE_DGRAM) {
				return -1;
			}
			if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM && WaitForPeerClose) {
				CloseSocketWaitForPeerClose(SocketInfo[Loop].Sock);
			} else {
				closesocket(SocketInfo[Loop].Sock);
			}
			SocketInfo[Loop].Sock = NULL;
			SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
			if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_DGRAM) {
				PutLog(LOG_UDPSOCKCLOSE, LogId, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
			} else {
				PutLog(LOG_SOCKCLOSE, LogId, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
			}
			break;
		}
	}
	return 0;
}

// Socketのオープン
int StkSocketMgr::OpenSocket(int TargetId)
{
	addrinfo Hints;
	addrinfo* ResAddr = NULL;

	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].Status == StkSocketInfo::STATUS_CLOSE && SocketInfo[Loop].ElementId == TargetId) {
			// If the type is neither TCP and UDP...
			if (SocketInfo[Loop].SocketType != StkSocketMgr::SOCKTYPE_STREAM &&
				SocketInfo[Loop].SocketType != StkSocketMgr::SOCKTYPE_DGRAM) {
				return -1;
			}
			// If the SocketInfo is created by StkSocket_CopyInfo, -1 only status is changed.
			if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM &&
				SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_RECEIVER &&
				SocketInfo[Loop].CopiedSocketFlag == TRUE) {
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;
				return 0;
			}
			// Receiverの場合
			if (SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_RECEIVER) {

				// Get address information
				memset(&Hints, 0, sizeof(Hints));
				Hints.ai_family = AF_UNSPEC;
				Hints.ai_socktype = SocketInfo[Loop].SocketType;
				Hints.ai_flags = AI_PASSIVE;
				char NodeName[256];
				char ServName[256];
				sprintf_s(NodeName, 256, "%S", SocketInfo[Loop].HostOrIpAddr);
				sprintf_s(ServName, 256, "%d", SocketInfo[Loop].Port);
				int Ret = getaddrinfo(NodeName, ServName, &Hints, &ResAddr);
				if (Ret != 0) {
					PutLog(LOG_NAMESOLVEERR, TargetId, _T(""), _T(""), 0, WSAGetLastError());
					SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
					return -1;
				}

				SocketInfo[Loop].Sock = socket(ResAddr->ai_family, ResAddr->ai_socktype, ResAddr->ai_protocol);
				// ソケットのオープンに失敗したら呼び出し元に戻る
				if (SocketInfo[Loop].Sock == INVALID_SOCKET) {
					freeaddrinfo(ResAddr);
					return -1;
				}

				// BINDに失敗したらソケットをクローズする
				int RetBind = bind(SocketInfo[Loop].Sock, ResAddr->ai_addr, ResAddr->ai_addrlen);
				if (RetBind == SOCKET_ERROR) {
					if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
						PutLog(LOG_BINDLISTENERR, TargetId, _T(""), _T(""), 0, WSAGetLastError());
					} else {
						PutLog(LOG_BINDERR, TargetId, _T(""), _T(""), 0, WSAGetLastError());
					}
					SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
					closesocket(SocketInfo[Loop].Sock);
					freeaddrinfo(ResAddr);
					return -1;
				}
				if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
					// LISTENに失敗したらソケットをクローズする
					int RetListen = listen(SocketInfo[Loop].Sock, 5);
					if (RetListen == SOCKET_ERROR) {
						PutLog(LOG_BINDLISTENERR, TargetId, _T(""), _T(""), 0, WSAGetLastError());
						SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
						closesocket(SocketInfo[Loop].Sock);
						freeaddrinfo(ResAddr);
						return -1;
					}
				}
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;

				// Change the status of copied socket information
				if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
					for (int CpyLoop = 0; CpyLoop < NumOfSocketInfo; CpyLoop++) {
						if (SocketInfo[CpyLoop].CopiedSocketFlag == TRUE && SocketInfo[CpyLoop].CopySourceId == TargetId) {
							SocketInfo[CpyLoop].Status = StkSocketInfo::STATUS_OPEN;
							SocketInfo[CpyLoop].Sock = SocketInfo[Loop].Sock;
						}
					}
				}

				// For avoiding bind failuer "Address in use"
				BOOL Yes = 1;
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&Yes, sizeof(BOOL));
				// Timeout setting
				int Timeo = 10000;
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&Timeo, sizeof(int));
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&Timeo, sizeof(int));
				// Buffer setting
				int Buffr = 1000000;
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_RCVBUF, (const char *)&Buffr, sizeof(int));
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_SNDBUF, (const char *)&Buffr, sizeof(int));
				if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
					PutLog(LOG_SUCCESSCSBNLS, TargetId, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
				} else {
					int MgsLen = GetUdpMaxMessageSize(TargetId);
					PutLog(LOG_SUCCESSCSBN, TargetId, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, MgsLen);
				}
				freeaddrinfo(ResAddr);
				return 0;
			}
			// Senderの場合
			if (SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_SENDER) {
				// 特に何もしない
				return 0;
			}
		}
	}
	return 0;
}

// Socketのクローズ
int StkSocketMgr::CloseSocket(int TargetId, BOOL WaitForPeerClose)
{
	// Find socket descriptor
	int FndSock = -1;
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == TargetId && SocketInfo[Loop].CopiedSocketFlag == FALSE) {
			FndSock = SocketInfo[Loop].Sock;
		}
	}

	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		// If the type is neither TCP and UDP...
		if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
			BOOL AcceptSockFnd = FALSE;

			BOOL Cond1 = (SocketInfo[Loop].CopiedSocketFlag == TRUE &&
							SocketInfo[Loop].ElementId != TargetId &&
							SocketInfo[Loop].Sock == FndSock &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT);
			BOOL Cond2 = (SocketInfo[Loop].CopiedSocketFlag == TRUE &&
							SocketInfo[Loop].ElementId == TargetId &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT);
			BOOL Cond3 = (SocketInfo[Loop].CopiedSocketFlag == FALSE &&
							SocketInfo[Loop].ElementId == TargetId &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT);

			// Closure condition of ACCEPT socket. Cond1 || Cond2 || Cond3
			if (Cond1 || Cond2 || Cond3) {
				if (WaitForPeerClose) {
					CloseSocketWaitForPeerClose(SocketInfo[Loop].AcceptedSock);
				} else {
					closesocket(SocketInfo[Loop].AcceptedSock);
				}
				SocketInfo[Loop].AcceptedSock = NULL;
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;
				// If the target is 'copied' StkSocket or If the current element is 'copied' StkSocket associating to 'added' StkSocket of the target.
				if (Cond1 || Cond2) {
					PutLog(LOG_CLOSEACCEPTSOCK, SocketInfo[Loop].ElementId, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
				}
				//If the target is 'added' StkSocket
				if (Cond3) {
					AcceptSockFnd = TRUE;
				}
			}

			BOOL CondC1 = (SocketInfo[Loop].CopiedSocketFlag == TRUE &&
							SocketInfo[Loop].ElementId != TargetId &&
							SocketInfo[Loop].Sock == FndSock &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN);
			BOOL CondC2 = (SocketInfo[Loop].CopiedSocketFlag == TRUE &&
							SocketInfo[Loop].ElementId == TargetId &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN);
			BOOL CondC3 = (SocketInfo[Loop].CopiedSocketFlag == FALSE &&
							SocketInfo[Loop].ElementId == TargetId &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN);

			if (CondC1 || CondC2) {
				// Closing procedure for 'copied' StkSocket.
				SocketInfo[Loop].Sock = NULL;
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
			}
			if (CondC3) {
				// If the socket information is not copied, closesocke() is called.
				closesocket(SocketInfo[Loop].Sock);
				SocketInfo[Loop].Sock = NULL;
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
				if (AcceptSockFnd == FALSE) {
					PutLog(LOG_SOCKCLOSE, TargetId, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
				} else {
					PutLog(LOG_CLOSEACCLISNSOCK, TargetId, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
				}
			}
		} else if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_DGRAM) {
			if (SocketInfo[Loop].ElementId != TargetId) {
				continue;
			}
			if (SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN) {
				closesocket(SocketInfo[Loop].Sock);
				SocketInfo[Loop].Sock = NULL;
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
				PutLog(LOG_UDPSOCKCLOSE, TargetId, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
			}
		} else {
			return -1;
		}
	}
	return 0;
}

int StkSocketMgr::Accept(int Id)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id && SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN) {
			// Select用FDS作成
			timeval Timeout;
			Timeout.tv_sec = 0;
			Timeout.tv_usec = 0;
			fd_set AccFds;
			FD_ZERO(&AccFds);
			FD_SET(SocketInfo[Loop].Sock, &AccFds);
			// 一定時間待ったあとSockに接続があるか確認する
			select(0, &AccFds, NULL, NULL, &Timeout);
			if (!FD_ISSET(SocketInfo[Loop].Sock, &AccFds)) {
				return -1;
			}
			SocketInfo[Loop].AcceptedSock = accept(SocketInfo[Loop].Sock, NULL, NULL);
			if (SocketInfo[Loop].AcceptedSock == INVALID_SOCKET) {
				return -1;
			}
			SocketInfo[Loop].Status = StkSocketInfo::STATUS_ACCEPT;

			// For avoiding java connection reset exception
			linger Sol;
			Sol.l_onoff = 1;
			Sol.l_linger = 10;
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_LINGER, (const char *)&Sol, sizeof(Sol));
			// Timeout setting
			int Timeo = 10000;
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&Timeo, sizeof(int));
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&Timeo, sizeof(int));
			// Buffer setting
			int Buffr = 1000000;
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_RCVBUF, (const char *)&Buffr, sizeof(int));
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_SNDBUF, (const char *)&Buffr, sizeof(int));

			PutLog(LOG_CREATEACCEPTSOCK, Id, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
			break;
		}
	}
	return 0;
}

int StkSocketMgr::CloseAccept(int Id, int LogId, BOOL WaitForPeerClose)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id && SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT) {
			if (WaitForPeerClose) {
				CloseSocketWaitForPeerClose(SocketInfo[Loop].AcceptedSock);
			} else {
				closesocket(SocketInfo[Loop].AcceptedSock);
			}
			SocketInfo[Loop].AcceptedSock = NULL;
			SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;
			PutLog(LOG_CLOSEACCEPTSOCK, LogId, SocketInfo[Loop].HostOrIpAddr, _T(""), SocketInfo[Loop].Port, 0);
			return 0;
		}
	}
	return 0;
}

int StkSocketMgr::Receive(int Id, int LogId, BYTE* Buffer, int BufferSize, int FinishCondition, BYTE* VarDat, int VarDatSize, BOOL ForceStop)
{
	// Select用FDS作成
	DWORD CurrWaitTime = GetTickCount();
	timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 0;
	fd_set RecFds;

	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		SOCKET TmpSock = NULL;
		int RecvLog = 0;
		if (SocketInfo[Loop].ElementId == Id &&
			SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT &&
			SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM &&
			SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_RECEIVER) {
			TmpSock = SocketInfo[Loop].AcceptedSock;
			RecvLog = LOG_ACPTRECV;
		}
		if (SocketInfo[Loop].ElementId == Id &&
			SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN &&
			SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM &&
			SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_SENDER) {
			TmpSock = SocketInfo[Loop].Sock;
			RecvLog = LOG_CNCTRECV;
		}
		if (TmpSock == NULL || RecvLog == 0) {
			continue;
		}
		int Offset = 0;
		while (TRUE) {
			FD_ZERO(&RecFds);
			FD_SET(TmpSock, &RecFds);
			// 一定時間待ったあとAcceptedSockに接続があるか確認する
			select(0, &RecFds, NULL, NULL, &Timeout);
			if (!FD_ISSET(TmpSock, &RecFds)) {
				// Timeout occurrence and no data received
				if (ForceStop == TRUE) {
					// There was a stopping thread request.
					return 0;
				}
				if ((FinishCondition >= 0 && FinishCondition <= 180000) || (FinishCondition >= 200001 && FinishCondition <= 380000)) {
					int Expire = (FinishCondition >= 200001 && FinishCondition <= 380000)? FinishCondition - 200000 : FinishCondition;
					DWORD CurrTime = GetTickCount();
					if ((int)(CurrTime - CurrWaitTime) > Expire) {
						if (Offset == 0) {
							return -2;
						} else {
							PutLog(RecvLog, LogId, _T(""), _T(""), Offset, 0);
							return Offset;
						}
					}
				}
				continue;
			}
			int FetchSize;
			if (FinishCondition >= 200001 && FinishCondition <= 380000) {
				// if HTTP termination rule is selected...
				FetchSize = 1;
			} else {
				// Otherwise
				FetchSize = BufferSize - Offset;
			}
			int Ret = recv(TmpSock, (char*)Buffer + Offset, FetchSize, 0);
			CurrWaitTime = GetTickCount();
			if (Ret == SOCKET_ERROR) {
				PutLog(LOG_RECVERROR, LogId, _T(""), _T(""), 0, WSAGetLastError());
				return Ret;
			}
			if (Ret == 0 && Offset == 0) {
				// 接続先ソケットがクローズした場合Ret=0となる
				return Offset;
			}
			Offset += Ret;
			if (Ret == 0 && Offset > 0) {
				// 接続先ソケットがクローズした場合Ret=0となる
				PutLog(RecvLog, LogId, _T(""), _T(""), Offset, 0);
				return Offset;
			}
			if (FinishCondition < 0 && Offset >= VarDatSize) {
				if (memcmp(Buffer + Offset - VarDatSize, VarDat, VarDatSize) == 0) {
					PutLog(RecvLog, LogId, _T(""), _T(""), Offset, 0);
					return Offset;
				}
			}
			if (FinishCondition >= 10000001 && FinishCondition <= 19999999 && FinishCondition - 10000000 <= Offset) {
				// If the finish condition is set as checking data length and exceeded the configured length...
				PutLog(RecvLog, LogId, _T(""), _T(""), Offset, 0);
				return Offset;
			}
			if (Ret > 0 && FinishCondition == 0) {
				// If the finish condition is set as unconditionally...
				PutLog(RecvLog, LogId, _T(""), _T(""), Offset, 0);
				return Offset;
			}
			if (FinishCondition >= 200001 && FinishCondition <= 380000) {
				// if HTTP termination rule is selected...
				if ((Buffer[Offset - 2] == '\n' && Buffer[Offset - 1] == '\n') ||
					(Buffer[Offset - 4] == '\r' && Buffer[Offset - 3] == '\n' && Buffer[Offset - 2] == '\r' && Buffer[Offset - 1] == '\n') ||
					(Buffer[Offset - 4] == '\n' && Buffer[Offset - 3] == '\r' && Buffer[Offset - 2] == '\n' && Buffer[Offset - 1] == '\r')) {
					// If double new-line-code was detected...
					Buffer[Offset] = '\0';
					BYTE* ContLenPtr = (BYTE*)strstr((char*)Buffer, "Content-Length:");
					if (ContLenPtr == NULL) {
						// If Content-Length is not presented.
					} else {
						ContLenPtr += 15;
						BYTE* ContLenEndPtr;
						if ((ContLenEndPtr = (BYTE*)strstr((char*)ContLenPtr, "\r\n")) == NULL) {
							if ((ContLenEndPtr = (BYTE*)strstr((char*)ContLenPtr, "\n\r")) == NULL) {
								if ((ContLenEndPtr = (BYTE*)strstr((char*)ContLenPtr, "\n")) == NULL) {
									// If new-line-code after Content-Length is not found. (It may be an impossible situation.)
									continue;
								}
							}
						}
						if (ContLenEndPtr != NULL && ContLenEndPtr - ContLenPtr <= 10) {
							char TmpBuf[100];
							strncpy_s(TmpBuf, 100, (char*)ContLenPtr, (int)(ContLenEndPtr - ContLenPtr + 1));
							int ContLen = atoi(TmpBuf);
							if (ContLen == 0) {
								// If inappropriate value or zero is set for Content-Length.
								PutLog(RecvLog, LogId, _T(""), _T(""), Offset, 0);
								return Offset;
							} else {
								FinishCondition = 10000000 + ContLen + Offset;
							}
						}
					}
					continue;
				}
			}
			if (Ret > 0 && FinishCondition == 9999999) {
				// If the finish condition is set as socket close detection...
			}
		}
	}
	// ココでエラーログを出力してはいけない。他の送信要素のソケットからの返信待ちの場合必ずエラーが発生する
	return -1;
}

int StkSocketMgr::ReceiveUdp(int Id, int LogId, BYTE* Buffer, int BufferSize)
{
	// Select用FDS作成
	timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 0;
	fd_set RecFds;

	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		SOCKET TmpSock = NULL;
		if (SocketInfo[Loop].ElementId == Id &&
			SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN &&
			SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_DGRAM &&
			(SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_RECEIVER ||
			SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_SENDER)) {
			TmpSock = SocketInfo[Loop].Sock;
		}
		if (TmpSock == NULL) {
			continue;
		}
		FD_ZERO(&RecFds);
		FD_SET(TmpSock, &RecFds);
		// 一定時間待ったあとTmpSockに接続があるか確認する
		select(0, &RecFds, NULL, NULL, &Timeout);
		if (!FD_ISSET(TmpSock, &RecFds)) {
			// Timeout occurrence and no data received
			return -2;
		}

		sockaddr_storage SenderAddr;
		int SenderAddrLen = sizeof(SenderAddr);
		int Ret = recvfrom(TmpSock, (char*)Buffer, BufferSize, 0, (sockaddr*)&SenderAddr, &SenderAddrLen);
		if (Ret == SOCKET_ERROR) {
			PutLog(LOG_RECVERROR, LogId, _T(""), _T(""), 0, WSAGetLastError());
			return Ret;
		}
		memcpy(&SocketInfo[Loop].LastAccessedAddr, &SenderAddr, SenderAddrLen);
		if (Ret == 0) {
			// 接続先ソケットがクローズした場合Ret=0となる
			PutLog(LOG_UDPRECV, LogId, _T(""), _T(""), Ret, 0);
			return Ret;
		}
		if (Ret > 0) {
			PutLog(LOG_UDPRECV, LogId, _T(""), _T(""), Ret, 0);
			return Ret;
		}
		return -1;
	}
	// ココでエラーログを出力してはいけない。他の送信要素のソケットからの返信待ちの場合必ずエラーが発生する
	return -1;
}

// Acquire UDP max message size
// Id [in] : Target ID
// Return : max message size, -1:ID is not found/Not a datagram
int StkSocketMgr::GetUdpMaxMessageSize(int Id)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id && SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_DGRAM) {
			int OptLen = sizeof(int);
			int OptVal;
			getsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&OptVal, &OptLen);
			return OptVal;
		}
	}
	return -1;
}

int StkSocketMgr::GetStatus(int TargetId)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == TargetId) {
			return SocketInfo[Loop].Status;
		}
	}
	return -1;
}

int StkSocketMgr::Send(int Id, int LogId, BYTE* Buffer, int BufferSize)
{
	int ErrCode = 0;
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id &&
			SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT &&
			SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM &&
			SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_RECEIVER) {
			int Ret = send(SocketInfo[Loop].AcceptedSock, (char*)Buffer, BufferSize, 0);
			if (Ret == SOCKET_ERROR) {
				PutLog(LOG_SENDERROR, LogId, _T(""), _T(""), 0, WSAGetLastError());
				return Ret;
			}
			PutLog(LOG_ACPTSEND, LogId, _T(""), _T(""), BufferSize, 0);
			return Ret;
		} else if (SocketInfo[Loop].ElementId == Id &&
			SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN &&
			SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM &&
			SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_SENDER) {
			int Ret = send(SocketInfo[Loop].Sock, (char*)Buffer, BufferSize, 0);
			if (Ret == SOCKET_ERROR) {
				PutLog(LOG_SENDERROR, LogId, _T(""), _T(""), 0, WSAGetLastError());
				return Ret;
			}
			PutLog(LOG_CNCTSEND, LogId, _T(""), _T(""), BufferSize, 0);
			return Ret;
		}
	}
	// ココでエラーログを出力してはいけない。他の受信要素へ返信する場合必ずエラーが発生する
	return -1;
}

int StkSocketMgr::SendUdp(int Id, int LogId, BYTE* Buffer, int BufferSize)
{
	addrinfo Hints;
	addrinfo* ResAddr = NULL;

	int ErrCode = 0;
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id &&
			SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN &&
			SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_DGRAM &&
			(SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_RECEIVER ||
			SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_SENDER)) {

			// Address setting...
			sockaddr_storage Addr;
			int Ret;
			if (SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_SENDER) {

				// Get address information
				memset(&Hints, 0, sizeof(Hints));
				Hints.ai_family = AF_UNSPEC;
				Hints.ai_socktype = SocketInfo[Loop].SocketType;
				Hints.ai_flags = AI_PASSIVE;
				char NodeName[256];
				char ServName[256];
				sprintf_s(NodeName, 256, "%S", SocketInfo[Loop].HostOrIpAddr);
				sprintf_s(ServName, 256, "%d", SocketInfo[Loop].Port);
				int RetMethod = getaddrinfo(NodeName, ServName, &Hints, &ResAddr);
				if (RetMethod != 0) {
					PutLog(LOG_NAMESOLVEERR, Id, _T(""), _T(""), 0, WSAGetLastError());
					SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
					closesocket(SocketInfo[Loop].Sock);
					return -1;
				}
				Ret = sendto(SocketInfo[Loop].Sock, (char*)Buffer, BufferSize, 0, ResAddr->ai_addr, ResAddr->ai_addrlen);
				freeaddrinfo(ResAddr);
			} else {
				memcpy(&Addr, &SocketInfo[Loop].LastAccessedAddr, sizeof(sockaddr_storage));
				Ret = sendto(SocketInfo[Loop].Sock, (char*)Buffer, BufferSize, 0, (sockaddr*)&Addr, sizeof(Addr));
			}

			if (Ret == SOCKET_ERROR) {
				PutLog(LOG_SENDERROR, LogId, _T(""), _T(""), 0, WSAGetLastError());
				return Ret;
			}
			PutLog(LOG_UDPSEND, LogId, _T(""), _T(""), BufferSize, 0);
			return Ret;
		}
	}
	// ココでエラーログを出力してはいけない。他の受信要素へ返信する場合必ずエラーが発生する
	return -1;
}

int StkSocketMgr::GetNumOfStkInfos()
{
	return NumOfSocketInfo;
}

#include <mutex>
#include <cstring>

#ifdef WIN32

#include <winsock2.h>
#include <Ws2tcpip.h>
#define STKSOCKET_ERRORCODE WSAGetLastError()
#define STKSOCKET_ERROR SOCKET_ERROR

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#define STKSOCKET_ERRORCODE errno
#define STKSOCKET_ERROR -1

#endif

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

#include "../stkpl/StkPl.h"
#include "StkSocketMgr.h"
#include "StkSocketInfo.h"

#define OPT_TIMEOUT 0

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

#ifdef WIN32
	static WSADATA WsaDat;
	WSAStartup(MAKEWORD(2,0), &WsaDat);
#endif

	ClearLog();
}

// Destructor
StkSocketMgr::~StkSocketMgr()
{
#ifdef WIN32
	WSACleanup();
#endif
}

void StkSocketMgr::PutLog(int TmpLog, int TmpLogId, const wchar_t* TmpLogParamStr1, const wchar_t* TmpLogParamStr2, int TmpLogParamInt1, int TmpLogParamInt2)
{
	Cs4Log.lock();
	if (NumOfLogs == MAX_NUM_OF_LOG) {
		for (int Loop = 0; Loop < MAX_NUM_OF_LOG - 1; Loop++) {
			Log[Loop] = Log[Loop + 1];
			LogId[Loop] = LogId[Loop + 1];
			StkPlWcsNCpy(LogParamStr1[Loop], 256, LogParamStr1[Loop + 1], 256);
			LogParamStr1[Loop][256 - 1] = '\0';
			StkPlWcsNCpy(LogParamStr2[Loop], 256, LogParamStr2[Loop + 1], 256);
			LogParamStr2[Loop][256 - 1] = '\0';
			LogParamInt1[Loop] = LogParamInt1[Loop + 1];
			LogParamInt2[Loop] = LogParamInt2[Loop + 1];
		}
		NumOfLogs--;
	}
	Log[NumOfLogs] = TmpLog;
	LogId[NumOfLogs] = TmpLogId;
	StkPlWcsNCpy(LogParamStr1[NumOfLogs], 256, TmpLogParamStr1, 256);
	LogParamStr1[NumOfLogs][256 - 1] = '\0';
	StkPlWcsNCpy(LogParamStr2[NumOfLogs], 256, TmpLogParamStr2, 256);
	LogParamStr2[NumOfLogs][256 - 1] = '\0';
	LogParamInt1[NumOfLogs] = TmpLogParamInt1;
	LogParamInt2[NumOfLogs] = TmpLogParamInt2;
	NumOfLogs++;
	Cs4Log.unlock();
}

void StkSocketMgr::TakeLastLog(int* TmpLog, int* TmpLogId, wchar_t* TmpLogParamStr1, wchar_t* TmpLogParamStr2, int* TmpLogParamInt1, int* TmpLogParamInt2)
{
	Cs4Log.lock();
	if (NumOfLogs <= 0) {
		*TmpLog = 0;
		*TmpLogId = 0;
		*TmpLogParamStr1 = L'\0';
		*TmpLogParamStr2 = L'\0';
		*TmpLogParamInt1 = 0;
		*TmpLogParamInt2 = 0;
		Cs4Log.unlock();
		return;
	}
	NumOfLogs--;
	*TmpLog = Log[NumOfLogs];
	*TmpLogId = LogId[NumOfLogs];
	StkPlWcsNCpy(TmpLogParamStr1, 256, LogParamStr1[NumOfLogs], 256);
	TmpLogParamStr1[256 - 1] = '\0';
	StkPlWcsNCpy(TmpLogParamStr2, 256, LogParamStr2[NumOfLogs], 256);
	TmpLogParamStr2[256 - 1] = '\0';
	*TmpLogParamInt1 = LogParamInt1[NumOfLogs];
	*TmpLogParamInt2 = LogParamInt2[NumOfLogs];

	Log[NumOfLogs] = 0;
	LogId[NumOfLogs] = 0;
	StkPlWcsCpy(LogParamStr1[NumOfLogs], 256, L"");
	StkPlWcsCpy(LogParamStr2[NumOfLogs], 256, L"");
	LogParamInt1[NumOfLogs] = 0;
	LogParamInt2[NumOfLogs] = 0;
	Cs4Log.unlock();
}

void StkSocketMgr::TakeFirstLog(int* TmpLog, int* TmpLogId, wchar_t* TmpLogParamStr1, wchar_t* TmpLogParamStr2, int* TmpLogParamInt1, int* TmpLogParamInt2)
{
	Cs4Log.lock();
	if (NumOfLogs <= 0) {
		*TmpLog = 0;
		*TmpLogId = 0;
		*TmpLogParamStr1 = L'\0';
		*TmpLogParamStr2 = L'\0';
		*TmpLogParamInt1 = 0;
		*TmpLogParamInt2 = 0;
		Cs4Log.unlock();
		return;
	}
	*TmpLog = Log[0];
	*TmpLogId = LogId[0];
	StkPlWcsNCpy(TmpLogParamStr1, 256, LogParamStr1[0], 256);
	TmpLogParamStr1[256 - 1] = '\0';
	StkPlWcsNCpy(TmpLogParamStr2, 256, LogParamStr2[0], 256);
	TmpLogParamStr2[256 - 1] = '\0';
	*TmpLogParamInt1 = LogParamInt1[0];
	*TmpLogParamInt2 = LogParamInt2[0];

	for (int Loop = 0; Loop < NumOfLogs - 1; Loop++) {
		Log[Loop] = Log[Loop + 1];
		LogId[Loop] = LogId[Loop + 1];
		StkPlWcsCpy(LogParamStr1[Loop], 256, LogParamStr1[Loop + 1]);
		StkPlWcsCpy(LogParamStr2[Loop], 256, LogParamStr2[Loop + 1]);
		LogParamInt1[Loop] = LogParamInt1[Loop + 1];
		LogParamInt2[Loop] = LogParamInt2[Loop + 1];
	}
	NumOfLogs--;
	Log[NumOfLogs] = 0;
	LogId[NumOfLogs] = 0;
	StkPlWcsCpy(LogParamStr1[NumOfLogs], 256, L"");
	StkPlWcsCpy(LogParamStr2[NumOfLogs], 256, L"");
	LogParamInt1[NumOfLogs] = 0;
	LogParamInt2[NumOfLogs] = 0;
	Cs4Log.unlock();
}

int StkSocketMgr::GetNumOfLogs()
{
	return NumOfLogs;
}

void StkSocketMgr::ClearLog()
{
	Cs4Log.lock();
	for (int Loop = 0; Loop < MAX_NUM_OF_LOG; Loop++) {
		Log[Loop] = 0;
		LogId[Loop] = 0;
		StkPlWcsCpy(LogParamStr1[Loop], 256, L"");
		StkPlWcsCpy(LogParamStr2[Loop], 256, L"");
		LogParamInt1[Loop] = 0;
		LogParamInt2[Loop] = 0;
	}
	NumOfLogs = 0;
	Cs4Log.unlock();
}

// Add socket information
// TargetId [in] : Target ID
// ActionType [in] : 0:Receiver or 1:Sender
// TargetAddr [in] : Host name or IP address
// TargetPort [in] : Port number
// Return : 0:Success, -1:Failure
int StkSocketMgr::AddSocketInfo(int TargetId, int SockType, int ActionType, const wchar_t TargetAddr[256], int TargetPort)
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
	SocketInfo[NumOfSocketInfo].Sock = 0;
	SocketInfo[NumOfSocketInfo].AcceptedSock = 0;
	SocketInfo[NumOfSocketInfo].CopiedSocketFlag = false;
	SocketInfo[NumOfSocketInfo].CopySourceId = -1;
	SocketInfo[NumOfSocketInfo].ForceStop = false;
	SocketInfo[NumOfSocketInfo].SecureCtx = NULL;
	SocketInfo[NumOfSocketInfo].SecureSsl = NULL;
	StkPlWcsCpy(SocketInfo[NumOfSocketInfo].HostOrIpAddr, 256, TargetAddr);
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
	if (SocketInfo[FndIndex].CopiedSocketFlag == true) {
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
	SocketInfo[NumOfSocketInfo].AcceptedSock = 0;
	SocketInfo[NumOfSocketInfo].CopiedSocketFlag = true;
	SocketInfo[NumOfSocketInfo].CopySourceId = ExistingId;
	SocketInfo[NumOfSocketInfo].ForceStop = SocketInfo[FndIndex].ForceStop;
	SocketInfo[NumOfSocketInfo].SecureCtx = SocketInfo[FndIndex].SecureCtx;
	SocketInfo[NumOfSocketInfo].SecureSsl = NULL;
	StkPlWcsCpy(SocketInfo[NumOfSocketInfo].HostOrIpAddr, 256, SocketInfo[FndIndex].HostOrIpAddr);
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
	bool FoundFlag = false;
	int Loop;
	for (Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (TargetId == SocketInfo[Loop].ElementId) {
			FoundFlag = true;
			break;
		}
	}
	if (FoundFlag == false) {
		return -1;
	}

	CloseSocket(TargetId, true);

	if (SocketInfo[Loop].CopiedSocketFlag == false && SocketInfo[Loop].SecureCtx != NULL) {
		SSL_CTX_free(SocketInfo[Loop].SecureCtx);
		SocketInfo[Loop].SecureCtx = NULL;
	}

	if (Loop < NumOfSocketInfo - 1) {
		SocketInfo[Loop].SocketType = SocketInfo[NumOfSocketInfo - 1].SocketType;
		SocketInfo[Loop].ActionType = SocketInfo[NumOfSocketInfo - 1].ActionType;
		SocketInfo[Loop].ElementId = SocketInfo[NumOfSocketInfo - 1].ElementId;
		SocketInfo[Loop].Status = SocketInfo[NumOfSocketInfo - 1].Status;
		SocketInfo[Loop].Port = SocketInfo[NumOfSocketInfo - 1].Port;
		SocketInfo[Loop].Sock = SocketInfo[NumOfSocketInfo - 1].Sock;
		SocketInfo[Loop].AcceptedSock = SocketInfo[NumOfSocketInfo - 1].AcceptedSock;
		SocketInfo[Loop].CopiedSocketFlag = SocketInfo[NumOfSocketInfo - 1].CopiedSocketFlag;
		SocketInfo[Loop].CopySourceId = SocketInfo[NumOfSocketInfo - 1].CopySourceId;
		SocketInfo[Loop].ForceStop = SocketInfo[NumOfSocketInfo - 1].ForceStop;
		SocketInfo[Loop].SecureCtx = SocketInfo[NumOfSocketInfo - 1].SecureCtx;
		SocketInfo[Loop].SecureSsl = SocketInfo[NumOfSocketInfo - 1].SecureSsl;
		StkPlWcsCpy(SocketInfo[Loop].HostOrIpAddr, 256, SocketInfo[NumOfSocketInfo - 1].HostOrIpAddr);
		memcpy(&SocketInfo[Loop].LastAccessedAddr, &SocketInfo[NumOfSocketInfo - 1].LastAccessedAddr, sizeof(sockaddr_storage));
	}
	NumOfSocketInfo--;

	return 0;
}

void StkSocketMgr::InitSecureSetting()
{
	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_all_algorithms();
}

int StkSocketMgr::SecureForRecv(int TargetId, const char* PrivateKey, const char* Certificate)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == TargetId) {
			SocketInfo[Loop].SecureCtx = SSL_CTX_new(SSLv23_server_method());
			SSL_CTX_use_certificate_file(SocketInfo[Loop].SecureCtx, Certificate, SSL_FILETYPE_PEM);
			SSL_CTX_use_PrivateKey_file(SocketInfo[Loop].SecureCtx, PrivateKey, SSL_FILETYPE_PEM);
			return 0;
		}
	}
	return -1;
}

int StkSocketMgr::SecureForSend(int TargetId, const char* FileName, const char* Path)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == TargetId) {
			SocketInfo[Loop].SecureCtx = SSL_CTX_new(SSLv23_client_method());
			if (FileName != NULL || Path != NULL) {
				SSL_CTX_load_verify_locations(SocketInfo[Loop].SecureCtx, FileName, Path);
			}
			return 0;
		}
	}
	return -1;
}

int StkSocketMgr::Unsecure(int TargetId)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == TargetId) {
			if (SocketInfo[Loop].SecureSsl != NULL) {
				SSL_free(SocketInfo[Loop].SecureSsl);
				SocketInfo[Loop].SecureSsl = NULL;
			}
			if (SocketInfo[Loop].SecureCtx != NULL) {
				SSL_CTX_free(SocketInfo[Loop].SecureCtx);
				SocketInfo[Loop].SecureCtx = NULL;
			}
			return 0;
		}
	}
	return -1;
}

int StkSocketMgr::GetSocketInfo(int Index, int* TargetId, int* SockType, int* ActionType, wchar_t TargetAddr[256], int* TargetPort, bool* CopiedFlag)
{
	if (Index < 0 || Index >= NumOfSocketInfo) {
		return -1;
	}
	*TargetId = SocketInfo[Index].ElementId;
	*SockType = SocketInfo[Index].SocketType;
	*ActionType = SocketInfo[Index].ActionType;
	StkPlWcsCpy(TargetAddr, 256, SocketInfo[Index].HostOrIpAddr);
	*TargetPort = SocketInfo[Index].Port;
	*CopiedFlag = SocketInfo[Index].CopiedSocketFlag;
	return 0;
}

int StkSocketMgr::GetSocketInfo(int TargetId, int* SockType, int* ActionType, wchar_t TargetAddr[256], int* TargetPort, bool* CopiedFlag)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == TargetId) {
			*SockType = SocketInfo[Loop].SocketType;
			*ActionType = SocketInfo[Loop].ActionType;
			StkPlWcsCpy(TargetAddr, 256, SocketInfo[Loop].HostOrIpAddr);
			*TargetPort = SocketInfo[Loop].Port;
			*CopiedFlag = SocketInfo[Loop].CopiedSocketFlag;
			return 0;
		}
	}
	return -1;
}

void StkSocketMgr::CloseSocketWaitForPeerClose(STK_SOCKET Target, SSL* SSLTarget)
{
	if (SSLTarget != NULL) {
		int RetShutdown = SSL_shutdown(SSLTarget);
		if (!RetShutdown) {
			SSL_shutdown(SSLTarget);
		}
		SSL_free(SSLTarget);
#ifdef WIN32
		closesocket(Target);
#else
		close(Target);
#endif
		return;
	}


#ifdef WIN32
	shutdown(Target, SD_SEND);
#else
	shutdown(Target, SHUT_WR);
#endif
	while (true) {
		char Buf[10000];
		int Ret = recv(Target, Buf, 10000, 0);
		if (Ret == 0 || Ret == STKSOCKET_ERROR) {
			break;
		}
	}
#ifdef WIN32
	shutdown(Target, SD_BOTH);
#else
	shutdown(Target, SHUT_RDWR);
#endif

#ifdef WIN32
	closesocket(Target);
#else
	close(Target);
#endif
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
			StkPlSPrintf(NodeName, 256, "%ls", SocketInfo[Loop].HostOrIpAddr);
			StkPlSPrintf(ServName, 256, "%d", SocketInfo[Loop].Port);
			int Ret = getaddrinfo(NodeName, ServName, &Hints, &ResAddr);
			if (Ret != 0) {
				PutLog(LOG_NAMESOLVEERR, Id, L"", L"", 0, STKSOCKET_ERRORCODE);
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
				return -1;
			}

			SocketInfo[Loop].Sock = socket(ResAddr->ai_family, ResAddr->ai_socktype, ResAddr->ai_protocol);
			// ソケットのオープンに失敗したら"Continue"
#ifdef WIN32
			if (SocketInfo[Loop].Sock == INVALID_SOCKET) {
				freeaddrinfo(ResAddr);
				return -1;
			}
#else
			if (SocketInfo[Loop].Sock < 0) {
				freeaddrinfo(ResAddr);
				return -1;
			}
#endif

			// For avoiding java connection reset exception
			linger Sol;
			Sol.l_onoff = 1;
			Sol.l_linger = 10;
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_LINGER, (const char *)&Sol, sizeof(Sol));
			// Timeout setting
			struct timeval Tv;
			Tv.tv_sec = OPT_TIMEOUT;
			Tv.tv_usec = 0;
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&Tv, sizeof(Tv));
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&Tv, sizeof(Tv));
			// Buffer setting
			int Buffr = 1000000;
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_RCVBUF, (const char *)&Buffr, sizeof(int));
			setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_SNDBUF, (const char *)&Buffr, sizeof(int));

			if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
				if (connect(SocketInfo[Loop].Sock, ResAddr->ai_addr, (int)ResAddr->ai_addrlen) == STKSOCKET_ERROR) {
					PutLog(LOG_CONNERROR, Id, L"", L"", 0, STKSOCKET_ERRORCODE);
					SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
#ifdef WIN32
					closesocket(SocketInfo[Loop].Sock);
#else
					close(SocketInfo[Loop].Sock);
#endif
					freeaddrinfo(ResAddr);
					return -1;
				}
				if (SocketInfo[Loop].SecureCtx != NULL) {
					SocketInfo[Loop].SecureSsl = SSL_new(SocketInfo[Loop].SecureCtx);
					SSL_set_fd(SocketInfo[Loop].SecureSsl, (int)SocketInfo[Loop].Sock);
					if (SSL_connect(SocketInfo[Loop].SecureSsl) <= 0) {
						SSL_free(SocketInfo[Loop].SecureSsl);
						SocketInfo[Loop].SecureSsl = NULL;
						PutLog(LOG_CONNERROR, Id, L"", L"", 0, 0);
						SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
#ifdef WIN32
						closesocket(SocketInfo[Loop].Sock);
#else
						close(SocketInfo[Loop].Sock);
#endif
						freeaddrinfo(ResAddr);
						return -1;
					}
					X509* PeerCert = SSL_get_peer_certificate(SocketInfo[Loop].SecureSsl);
					if (PeerCert == NULL) {
						SSL_free(SocketInfo[Loop].SecureSsl);
						SocketInfo[Loop].SecureSsl = NULL;
						PutLog(LOG_CONNERROR, Id, L"", L"", 0, 0);
						SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
#ifdef WIN32
						closesocket(SocketInfo[Loop].Sock);
#else
						close(SocketInfo[Loop].Sock);
#endif
						freeaddrinfo(ResAddr);
						return -1;
					}
					X509_free(PeerCert);
					if (SSL_get_verify_result(SocketInfo[Loop].SecureSsl) != 0) {
						SSL_free(SocketInfo[Loop].SecureSsl);
						SocketInfo[Loop].SecureSsl = NULL;
						PutLog(LOG_CONNERROR, Id, L"", L"", 0, 0);
						SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
#ifdef WIN32
						closesocket(SocketInfo[Loop].Sock);
#else
						close(SocketInfo[Loop].Sock);
#endif
						freeaddrinfo(ResAddr);
						return -1;
					}
				}

				SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;
				PutLog(LOG_SUCCESSCSC, Id, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
				freeaddrinfo(ResAddr);
				break;
			} else {
				int Yes = 1;
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_BROADCAST, (char *)&Yes, sizeof(Yes));
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;
				PutLog(LOG_SUCCESSCS, Id, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
				freeaddrinfo(ResAddr);
				break;
			}
		}
	}
	return 0;
}

int StkSocketMgr::DisconnectSocket(int Id, int LogId, bool WaitForPeerClose)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id && SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN) {
			// If the type is not TCP...
			if (SocketInfo[Loop].SocketType != StkSocketMgr::SOCKTYPE_STREAM &&
				SocketInfo[Loop].SocketType != StkSocketMgr::SOCKTYPE_DGRAM) {
				return -1;
			}
			if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM && WaitForPeerClose) {
				CloseSocketWaitForPeerClose(SocketInfo[Loop].Sock, SocketInfo[Loop].SecureSsl);
			} else {
				if (SocketInfo[Loop].SecureCtx != NULL && SocketInfo[Loop].SecureSsl != NULL) {
					SSL_free(SocketInfo[Loop].SecureSsl);
				}
#ifdef WIN32
				closesocket(SocketInfo[Loop].Sock);
#else
				close(SocketInfo[Loop].Sock);
#endif
			}
			SocketInfo[Loop].SecureSsl = NULL;
			SocketInfo[Loop].Sock = 0;
			if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_DGRAM) {
				PutLog(LOG_UDPSOCKCLOSE, LogId, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
			} else {
				PutLog(LOG_SOCKCLOSE, LogId, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
			}
			SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
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
				SocketInfo[Loop].CopiedSocketFlag == true) {
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
				StkPlSPrintf(NodeName, 256, "%ls", SocketInfo[Loop].HostOrIpAddr);
				StkPlSPrintf(ServName, 256, "%d", SocketInfo[Loop].Port);
				int Ret = getaddrinfo(NodeName, ServName, &Hints, &ResAddr);
				if (Ret != 0) {
					PutLog(LOG_NAMESOLVEERR, TargetId, L"", L"", 0, STKSOCKET_ERRORCODE);
					SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
					return -1;
				}

				SocketInfo[Loop].Sock = socket(ResAddr->ai_family, ResAddr->ai_socktype, ResAddr->ai_protocol);
				// ソケットのオープンに失敗したら呼び出し元に戻る
#ifdef WIN32
				if (SocketInfo[Loop].Sock == INVALID_SOCKET) {
					freeaddrinfo(ResAddr);
					return -1;
				}
#else
				if (SocketInfo[Loop].Sock < 0) {
					freeaddrinfo(ResAddr);
					return -1;
				}
#endif

				// For avoiding bind failuer "Address in use"
				int Yes = 1;
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&Yes, sizeof(int));

				// BINDに失敗したらソケットをクローズする
				int RetBind = bind(SocketInfo[Loop].Sock, ResAddr->ai_addr, (int)ResAddr->ai_addrlen);
				if (RetBind == STKSOCKET_ERROR) {
					if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
						PutLog(LOG_BINDLISTENERR, TargetId, L"", L"", 0, STKSOCKET_ERRORCODE);
					} else {
						PutLog(LOG_BINDERR, TargetId, L"", L"", 0, STKSOCKET_ERRORCODE);
					}
#ifdef WIN32
					closesocket(SocketInfo[Loop].Sock);
#else
					close(SocketInfo[Loop].Sock);
#endif
					freeaddrinfo(ResAddr);
					SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
					return -1;
				}
				if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
					// LISTENに失敗したらソケットをクローズする
					int RetListen = listen(SocketInfo[Loop].Sock, 5);
					if (RetListen == STKSOCKET_ERROR) {
						PutLog(LOG_BINDLISTENERR, TargetId, L"", L"", 0, STKSOCKET_ERRORCODE);
#ifdef WIN32
						closesocket(SocketInfo[Loop].Sock);
#else
						close(SocketInfo[Loop].Sock);
#endif
						freeaddrinfo(ResAddr);
						SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
						return -1;
					}
				}
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;

				// Change the status of copied socket information
				if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
					for (int CpyLoop = 0; CpyLoop < NumOfSocketInfo; CpyLoop++) {
						if (SocketInfo[CpyLoop].CopiedSocketFlag == true && SocketInfo[CpyLoop].CopySourceId == TargetId) {
							SocketInfo[CpyLoop].Status = StkSocketInfo::STATUS_OPEN;
							SocketInfo[CpyLoop].Sock = SocketInfo[Loop].Sock;
						}
					}
				}

				// Timeout setting
				struct timeval Tv;
				Tv.tv_sec = OPT_TIMEOUT;
				Tv.tv_usec = 0;
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&Tv, sizeof(Tv));
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&Tv, sizeof(Tv));
				// Buffer setting
				int Buffr = 1000000;
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_RCVBUF, (const char *)&Buffr, sizeof(int));
				setsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_SNDBUF, (const char *)&Buffr, sizeof(int));
				if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
					PutLog(LOG_SUCCESSCSBNLS, TargetId, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
				} else {
					int MgsLen = GetUdpMaxMessageSize(TargetId);
					PutLog(LOG_SUCCESSCSBN, TargetId, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, MgsLen);
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
int StkSocketMgr::CloseSocket(int TargetId, bool WaitForPeerClose)
{
	// Find socket descriptor
	int FndSock = -1;
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == TargetId && SocketInfo[Loop].CopiedSocketFlag == false) {
			FndSock = (int)SocketInfo[Loop].Sock;
		}
	}

	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		// If the type is neither TCP and UDP...
		if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM) {
			bool AcceptSockFnd = false;

			bool Cond1 = (SocketInfo[Loop].CopiedSocketFlag == true &&
							SocketInfo[Loop].ElementId != TargetId &&
							SocketInfo[Loop].Sock == FndSock &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT);
			bool Cond2 = (SocketInfo[Loop].CopiedSocketFlag == true &&
							SocketInfo[Loop].ElementId == TargetId &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT);
			bool Cond3 = (SocketInfo[Loop].CopiedSocketFlag == false &&
							SocketInfo[Loop].ElementId == TargetId &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT);

			// Closure condition of ACCEPT socket. Cond1 || Cond2 || Cond3
			if (Cond1 || Cond2 || Cond3) {
				if (WaitForPeerClose) {
					CloseSocketWaitForPeerClose(SocketInfo[Loop].AcceptedSock, SocketInfo[Loop].SecureSsl);
				} else {
					if (SocketInfo[Loop].SecureCtx != NULL && SocketInfo[Loop].SecureSsl != NULL) {
						SSL_free(SocketInfo[Loop].SecureSsl);
					}
#ifdef WIN32
					closesocket(SocketInfo[Loop].AcceptedSock);
#else
					close(SocketInfo[Loop].AcceptedSock);
#endif
				}
				SocketInfo[Loop].SecureSsl = NULL;
				SocketInfo[Loop].AcceptedSock = 0;
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;
				// If the target is 'copied' StkSocket or If the current element is 'copied' StkSocket associating to 'added' StkSocket of the target.
				if (Cond1 || Cond2) {
					PutLog(LOG_CLOSEACCEPTSOCK, SocketInfo[Loop].ElementId, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
				}
				//If the target is 'added' StkSocket
				if (Cond3) {
					AcceptSockFnd = true;
				}
			}

			bool CondC1 = (SocketInfo[Loop].CopiedSocketFlag == true &&
							SocketInfo[Loop].ElementId != TargetId &&
							SocketInfo[Loop].Sock == FndSock &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN);
			bool CondC2 = (SocketInfo[Loop].CopiedSocketFlag == true &&
							SocketInfo[Loop].ElementId == TargetId &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN);
			bool CondC3 = (SocketInfo[Loop].CopiedSocketFlag == false &&
							SocketInfo[Loop].ElementId == TargetId &&
							SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN);

			if (CondC1 || CondC2) {
				// Closing procedure for 'copied' StkSocket.
				SocketInfo[Loop].Sock = 0;
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
			}
			if (CondC3) {
				// If the socket information is not copied, closesocket() is called.
				if (WaitForPeerClose) {
					CloseSocketWaitForPeerClose(SocketInfo[Loop].Sock, SocketInfo[Loop].SecureSsl);
				} else {
					if (SocketInfo[Loop].SecureCtx != NULL && SocketInfo[Loop].SecureSsl != NULL) {
						SSL_free(SocketInfo[Loop].SecureSsl);
					}
#ifdef WIN32
					closesocket(SocketInfo[Loop].Sock);
#else
					close(SocketInfo[Loop].Sock);
#endif
				}
				SocketInfo[Loop].SecureSsl = NULL;
				SocketInfo[Loop].Sock = 0;
				if (AcceptSockFnd == false) {
					PutLog(LOG_SOCKCLOSE, TargetId, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
				} else {
					PutLog(LOG_CLOSEACCLISNSOCK, TargetId, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
				}
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
			}
		} else if (SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_DGRAM) {
			if (SocketInfo[Loop].ElementId != TargetId) {
				continue;
			}
			if (SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN) {
#ifdef WIN32
				closesocket(SocketInfo[Loop].Sock);
#else
				close(SocketInfo[Loop].Sock);
#endif
				SocketInfo[Loop].Sock = 0;
				PutLog(LOG_UDPSOCKCLOSE, TargetId, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
				SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
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
			select((int)SocketInfo[Loop].Sock + 1, &AccFds, NULL, NULL, &Timeout);

			if (!FD_ISSET(SocketInfo[Loop].Sock, &AccFds)) {
				return -1;
			}
			SocketInfo[Loop].AcceptedSock = accept(SocketInfo[Loop].Sock, NULL, NULL);
#ifdef WIN32
			if (SocketInfo[Loop].AcceptedSock == INVALID_SOCKET) {
				return -1;
			}
#else
			if (SocketInfo[Loop].AcceptedSock < 0) {
				return -1;
			}
#endif
			if (SocketInfo[Loop].SecureCtx != NULL) {
				SocketInfo[Loop].SecureSsl = SSL_new(SocketInfo[Loop].SecureCtx);
				SSL_set_fd(SocketInfo[Loop].SecureSsl, (int)SocketInfo[Loop].AcceptedSock);
				if (SSL_accept(SocketInfo[Loop].SecureSsl) <= 0) {
					SSL_free(SocketInfo[Loop].SecureSsl);
					SocketInfo[Loop].SecureSsl = NULL;
#ifdef WIN32
					closesocket(SocketInfo[Loop].AcceptedSock);
#else
					close(SocketInfo[Loop].AcceptedSock);
#endif
					return -1;
				}
			}
			SocketInfo[Loop].Status = StkSocketInfo::STATUS_ACCEPT;

			// For avoiding java connection reset exception
			linger Sol;
			Sol.l_onoff = 1;
			Sol.l_linger = 10;
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_LINGER, (const char *)&Sol, sizeof(Sol));
			// Timeout setting
			struct timeval Tv;
			Tv.tv_sec = OPT_TIMEOUT;
			Tv.tv_usec = 0;
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&Tv, sizeof(Tv));
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&Tv, sizeof(Tv));
			// Buffer setting
			int Buffr = 1000000;
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_RCVBUF, (const char *)&Buffr, sizeof(int));
			setsockopt(SocketInfo[Loop].AcceptedSock, SOL_SOCKET, SO_SNDBUF, (const char *)&Buffr, sizeof(int));

			PutLog(LOG_CREATEACCEPTSOCK, Id, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
			break;
		}
	}
	return 0;
}

int StkSocketMgr::CloseAccept(int Id, int LogId, bool WaitForPeerClose)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id && SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT) {
			if (WaitForPeerClose) {
				CloseSocketWaitForPeerClose(SocketInfo[Loop].AcceptedSock, SocketInfo[Loop].SecureSsl);
			} else {
				if (SocketInfo[Loop].SecureCtx != NULL && SocketInfo[Loop].SecureSsl != NULL) {
					SSL_free(SocketInfo[Loop].SecureSsl);
				}
#ifdef WIN32
				closesocket(SocketInfo[Loop].AcceptedSock);
#else
				close(SocketInfo[Loop].AcceptedSock);
#endif
			}
			SocketInfo[Loop].SecureSsl = NULL;
			SocketInfo[Loop].AcceptedSock = 0;
			SocketInfo[Loop].Status = StkSocketInfo::STATUS_OPEN;
			PutLog(LOG_CLOSEACCEPTSOCK, LogId, SocketInfo[Loop].HostOrIpAddr, L"", SocketInfo[Loop].Port, 0);
			return 0;
		}
	}
	return 0;
}

// FinishCondition >  0 : Exit method after receiving data the size meets with the specified length
// FinishCondition =  0 : Exit method after receiving data
// FinishCondition = -1 : Exit method if the specified string appears in the received data
// FinishCondition = -2 : Exit method if closure by peer detected
// FinishCondition = -3 : Exit method according to "Content-Length" or "Transfer-Encoding: chunked" in HTTP header
// FinishCondition = -4 : Exit method if timeout is detected
int StkSocketMgr::Receive(int Id, int LogId, unsigned char* Buffer, int BufferSize, int FinishCondition, int FinishCondTimeout, unsigned char* Opt1, int Opt2)
{
	// Select用FDS作成
	long long CurrWaitTime = StkPlGetTickCount();

	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		STK_SOCKET TmpSock = 0;
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
		if (TmpSock == 0 || RecvLog == 0) {
			continue;
		}

		int ChunkSize = 0;
		bool GetChunkMode = false;
		bool ChunkEnd = false;
		unsigned char* TmpSizePtr = 0;

		int Offset = 0;
		while (true) {
			timeval Timeout;
			Timeout.tv_sec = 0;
			Timeout.tv_usec = 0;
			fd_set RecFds;
			FD_ZERO(&RecFds);
			FD_SET(TmpSock, &RecFds);
			// 一定時間待ったあとAcceptedSockに接続があるか確認する
			select((int)TmpSock + 1, &RecFds, NULL, NULL, &Timeout);
			int SslPend = 0;
			if (SocketInfo[Loop].SecureCtx != NULL && SocketInfo[Loop].SecureSsl != NULL) {
				SslPend = SSL_has_pending(SocketInfo[Loop].SecureSsl);
			}
			if (!FD_ISSET(TmpSock, &RecFds) && SslPend == 0) {
				// Timeout occurrence and no data received
				if (SocketInfo[Loop].ForceStop == true) {
					// There was a stopping thread request.
					return Offset;
				}
				if (FinishCondTimeout > 0) {
					long long CurrTime = StkPlGetTickCount();
					if ((int)(CurrTime - CurrWaitTime) > FinishCondTimeout) {
						if (Offset == 0) {
							return -2;
						} else {
							PutLog(RecvLog, LogId, L"", L"", Offset, 0);
							return Offset;
						}
					}
					if ((int)(CurrTime - CurrWaitTime) > 20000) {
						StkPlSleepMs(1000);
					} else if ((int)(CurrTime - CurrWaitTime) > 5000) {
						StkPlSleepMs(250);
					} else if ((int)(CurrTime - CurrWaitTime) > 2000) {
						StkPlSleepMs(100);
					} else if ((int)(CurrTime - CurrWaitTime) > 1000) {
						StkPlSleepMs(50);
					}
				}
				continue;
			}

			// Check whether the buffer is full.
			if (BufferSize <= Offset) {
				PutLog(RecvLog, LogId, L"", L"", Offset, 0);
				return Offset;
			}

			// Set fetch size
			int FetchSize = 0;
			if (FinishCondition == RECV_FINISHCOND_CONTENTLENGTH || FinishCondition == RECV_FINISHCOND_STRING) {
				// if HTTP termination rule is selected...
				FetchSize = 1;
			} else if (FinishCondition == RECV_FINISHCOND_CHUNK) {
				if (BufferSize >= Offset + ChunkSize) {
					FetchSize = ChunkSize;
				} else {
					FetchSize = BufferSize - Offset;
				}
			} else if (FinishCondition > 0) {
				if (BufferSize > FinishCondition) {
					FetchSize = FinishCondition - Offset;
				} else {
					FetchSize = BufferSize - Offset;
				}
			} else {
				// Otherwise
				FetchSize = BufferSize - Offset;
			}

			int Ret = 0;
			if (SocketInfo[Loop].SecureCtx != NULL && SocketInfo[Loop].SecureSsl != NULL) {
				Ret = SSL_read(SocketInfo[Loop].SecureSsl, (char*)Buffer + Offset, FetchSize);
				if (Ret < 0) {
					int Err = SSL_get_error(SocketInfo[Loop].SecureSsl, Ret);
					if (Err == SSL_ERROR_WANT_READ || Err == SSL_ERROR_SYSCALL) {
						continue;
					}
				}
			} else {
				Ret = recv(TmpSock, (char*)Buffer + Offset, FetchSize, 0);
			}

			CurrWaitTime = StkPlGetTickCount();
			if (Ret == STKSOCKET_ERROR) {
				PutLog(LOG_RECVERROR, LogId, L"", L"", 0, STKSOCKET_ERRORCODE);
				return Ret;
			}
			if (Ret == 0 && Offset == 0) {
				// 接続先ソケットがクローズした場合Ret=0となる
				return Offset;
			}
			Offset += Ret;
			if (Ret == 0 && Offset > 0) {
				// 接続先ソケットがクローズした場合Ret=0となる
				PutLog(RecvLog, LogId, L"", L"", Offset, 0);
				return Offset;
			}
			if (FinishCondition == RECV_FINISHCOND_STRING && Offset >= Opt2 && Opt1 != NULL) {
				if (memcmp(Buffer + Offset - Opt2, Opt1, Opt2) == 0) {
					PutLog(RecvLog, LogId, L"", L"", Offset, 0);
					return Offset;
				}
			}
			if (FinishCondition > 0 && FinishCondition <= Offset) {
				// If the finish condition is set as checking data length and exceeded the configured length...
				PutLog(RecvLog, LogId, L"", L"", Offset, 0);
				return Offset;
			}
			if (Ret > 0 && FinishCondition == RECV_FINISHCOND_UNCONDITIONAL) {
				// If the finish condition is set as unconditionally...
				PutLog(RecvLog, LogId, L"", L"", Offset, 0);
				return Offset;
			}
			if (FinishCondition == RECV_FINISHCOND_CHUNK) {
				if (Ret > 0 && Ret < FetchSize) {
					ChunkSize = FetchSize - Ret;
					continue;
				}
				if (GetChunkMode == false && Offset >= 2 && Buffer[Offset - 2] == '\r' && Buffer[Offset - 1] == '\n') {
					// After size acquisition
					int TmpSize = 0;
					if (TmpSizePtr != 0) {
						StkPlSScanf((char*)TmpSizePtr, "%x", &TmpSize);
						if (TmpSize == 0) {
							ChunkEnd = true;
						}
						ChunkSize = TmpSize + 2;
						if (!(Opt2 & 0b00000010)) {
							int Diff = (int)(&Buffer[Offset] - TmpSizePtr);
							Offset -= Diff;
						}
					}
					GetChunkMode = true;
					continue;
				} else if (GetChunkMode == true && Offset >= 2 && Buffer[Offset - 2] == '\r' && Buffer[Offset - 1] == '\n') {
					// After data acquisition
					ChunkSize = 1;
					GetChunkMode = false;
					if (!(Opt2 & 0b00000010)) {
						Offset -= 2;
					}
					TmpSizePtr = &Buffer[Offset];
					if (ChunkEnd == true || Opt2 & 0b00000001) {
						PutLog(RecvLog, LogId, L"", L"", Offset, 0);
						return Offset;
					}
					continue;
				}
			}
			if (FinishCondition == RECV_FINISHCOND_CONTENTLENGTH) {
				if (Offset >= 2 && Buffer[Offset - 2] == '\r' && Buffer[Offset - 1] == '\n' && (Opt2 & 0b00000001)) {
					StkPlSScanf((const char*)Buffer, "%x", &ChunkSize);
					if (ChunkSize != 0) {
						FinishCondition = RECV_FINISHCOND_CHUNK;
						GetChunkMode = true;
						ChunkSize += 2;
						if (!(Opt2 & 0b00000010)) {
							int Diff = (int)(&Buffer[Offset] - Buffer);
							Offset -= Diff;
						}
						continue;
					}
				}
			}
			if (FinishCondition == RECV_FINISHCOND_CONTENTLENGTH) {
				// if HTTP termination rule is selected...
				if ((Offset >= 2 && Buffer[Offset - 2] == '\n' && Buffer[Offset - 1] == '\n') ||
					(Offset >= 4 && Buffer[Offset - 4] == '\r' && Buffer[Offset - 3] == '\n' && Buffer[Offset - 2] == '\r' && Buffer[Offset - 1] == '\n') ||
					(Offset >= 4 && Buffer[Offset - 4] == '\n' && Buffer[Offset - 3] == '\r' && Buffer[Offset - 2] == '\n' && Buffer[Offset - 1] == '\r')) {
					// If double new-line-code was detected...
					Buffer[Offset] = '\0'; // '\0' will be overwritten by next fetch.
					unsigned char* ContLenPtr = (unsigned char*)strstr((char*)Buffer, "Content-Length:");
					if (ContLenPtr == NULL) {
						if (strstr((char*)Buffer, "Transfer-Encoding: chunked") != 0) {
							// If "Transfer-Encoding: chunked" is presented...
							FinishCondition = RECV_FINISHCOND_CHUNK;
							GetChunkMode = false;
							ChunkSize = 1;
							TmpSizePtr = &Buffer[Offset];
						} else {
							// If both "Content-Length:" and "Transfer-Encoding: chunked" are not presented...May be GET request
							PutLog(RecvLog, LogId, L"", L"", Offset, 0);
							return Offset;
						}
					} else {
						ContLenPtr += 15;
						unsigned char* ContLenEndPtr;
						if ((ContLenEndPtr = (unsigned char*)strstr((char*)ContLenPtr, "\r\n")) == NULL) {
							if ((ContLenEndPtr = (unsigned char*)strstr((char*)ContLenPtr, "\n\r")) == NULL) {
								if ((ContLenEndPtr = (unsigned char*)strstr((char*)ContLenPtr, "\n")) == NULL) {
									// If new-line-code after Content-Length is not found. (It may be an impossible situation.)
									continue;
								}
							}
						}
						if (ContLenEndPtr != NULL && ContLenEndPtr - ContLenPtr <= 10) {
							char TmpBuf[100];
							StkPlStrNCpy(TmpBuf, 100, (char*)ContLenPtr, (int)(ContLenEndPtr - ContLenPtr + 1));
							int ContLen = atoi(TmpBuf);
							if (ContLen == 0) {
								// If inappropriate value or zero is set for Content-Length.
								PutLog(RecvLog, LogId, L"", L"", Offset, 0);
								return Offset;
							} else {
								FinishCondition = ContLen + Offset;
							}
						}
					}
					continue;
				}
			}
			if (Ret > 0 && FinishCondition == RECV_FINISHCOND_PEERCLOSURE) {
				// If the finish condition is set as socket close detection...
			}
			if (FinishCondition == RECV_FINISHCOND_TIMEOUT) {
				// If the finish condition is set as detecting timeout...
			}
		}
	}
	// ココでエラーログを出力してはいけない。他の送信要素のソケットからの返信待ちの場合必ずエラーが発生する
	return -1;
}

int StkSocketMgr::ReceiveUdp(int Id, int LogId, unsigned char* Buffer, int BufferSize)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		STK_SOCKET TmpSock = 0;
		if (SocketInfo[Loop].ElementId == Id &&
			SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN &&
			SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_DGRAM &&
			(SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_RECEIVER ||
			SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_SENDER)) {
			TmpSock = SocketInfo[Loop].Sock;
		}
		if (TmpSock == 0) {
			continue;
		}
		// Select用FDS作成
		timeval Timeout;
		Timeout.tv_sec = 0;
		Timeout.tv_usec = 0;
		fd_set RecFds;
		FD_ZERO(&RecFds);
		FD_SET(TmpSock, &RecFds);
		// 一定時間待ったあとTmpSockに接続があるか確認する
		select((int)TmpSock + 1, &RecFds, NULL, NULL, &Timeout);
		if (!FD_ISSET(TmpSock, &RecFds)) {
			// Timeout occurrence and no data received
			return -2;
		}

		sockaddr_storage SenderAddr;
#ifdef WIN32
		int SenderAddrLen = sizeof(SenderAddr);
		int Ret = recvfrom(TmpSock, (char*)Buffer, BufferSize, 0, (sockaddr*)&SenderAddr, &SenderAddrLen);
#else
		socklen_t SenderAddrLen = sizeof(SenderAddr);
		int Ret = recvfrom(TmpSock, (char*)Buffer, BufferSize, 0, (sockaddr*)&SenderAddr, &SenderAddrLen);
#endif
		if (Ret == STKSOCKET_ERROR) {
			PutLog(LOG_RECVERROR, LogId, L"", L"", 0, STKSOCKET_ERRORCODE);
			return Ret;
		}
		memcpy(&SocketInfo[Loop].LastAccessedAddr, &SenderAddr, (size_t)SenderAddrLen);
		if (Ret == 0) {
			// 接続先ソケットがクローズした場合Ret=0となる
			PutLog(LOG_UDPRECV, LogId, L"", L"", Ret, 0);
			return Ret;
		}
		if (Ret > 0) {
			PutLog(LOG_UDPRECV, LogId, L"", L"", Ret, 0);
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
			int OptVal = 65507;
#ifdef WIN32
			getsockopt(SocketInfo[Loop].Sock, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&OptVal, &OptLen);
#endif
			return OptVal;
		}
	}
	return -1;
}

// Stop forcedly
// Id [in] : Target ID
// Return : 0:ID found, -1:ID not found
int StkSocketMgr::ForceStop(int Id)
{
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id) {
			SocketInfo[Loop].ForceStop = true;
			return 0;
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

int StkSocketMgr::Send(int Id, int LogId, const unsigned char* Buffer, int BufferSize)
{
	int ErrCode = 0;
	for (int Loop = 0; Loop < NumOfSocketInfo; Loop++) {
		if (SocketInfo[Loop].ElementId == Id &&
			SocketInfo[Loop].Status == StkSocketInfo::STATUS_ACCEPT &&
			SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM &&
			SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_RECEIVER) {
			int Ret = 0;
			if (SocketInfo[Loop].SecureCtx != NULL && SocketInfo[Loop].SecureSsl != NULL) {
				Ret = SSL_write(SocketInfo[Loop].SecureSsl, (char*)Buffer, BufferSize);
			} else {
				Ret = send(SocketInfo[Loop].AcceptedSock, (char*)Buffer, BufferSize, 0);
			}
			if (Ret == STKSOCKET_ERROR) {
				PutLog(LOG_SENDERROR, LogId, L"", L"", 0, STKSOCKET_ERRORCODE);
				return Ret;
			}
			PutLog(LOG_ACPTSEND, LogId, L"", L"", Ret, 0);
			return Ret;
		} else if (SocketInfo[Loop].ElementId == Id &&
			SocketInfo[Loop].Status == StkSocketInfo::STATUS_OPEN &&
			SocketInfo[Loop].SocketType == StkSocketMgr::SOCKTYPE_STREAM &&
			SocketInfo[Loop].ActionType == StkSocketMgr::ACTIONTYPE_SENDER) {
			int Ret = 0;
			if (SocketInfo[Loop].SecureCtx != NULL && SocketInfo[Loop].SecureSsl != NULL) {
				Ret = SSL_write(SocketInfo[Loop].SecureSsl, (char*)Buffer, BufferSize);
			} else {
				Ret = send(SocketInfo[Loop].Sock, (char*)Buffer, BufferSize, 0);
			}
			if (Ret == STKSOCKET_ERROR) {
				PutLog(LOG_SENDERROR, LogId, L"", L"", 0, STKSOCKET_ERRORCODE);
				return Ret;
			}
			PutLog(LOG_CNCTSEND, LogId, L"", L"", Ret, 0);
			return Ret;
		}
	}
	// ココでエラーログを出力してはいけない。他の受信要素へ返信する場合必ずエラーが発生する
	return -1;
}

int StkSocketMgr::SendUdp(int Id, int LogId, const unsigned char* Buffer, int BufferSize)
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
				StkPlSPrintf(NodeName, 256, "%ls", SocketInfo[Loop].HostOrIpAddr);
				StkPlSPrintf(ServName, 256, "%d", SocketInfo[Loop].Port);
				int RetMethod = getaddrinfo(NodeName, ServName, &Hints, &ResAddr);
				if (RetMethod != 0) {
					PutLog(LOG_NAMESOLVEERR, Id, L"", L"", 0, STKSOCKET_ERRORCODE);
#ifdef WIN32
					closesocket(SocketInfo[Loop].Sock);
#else
					close(SocketInfo[Loop].Sock);
#endif
					SocketInfo[Loop].Status = StkSocketInfo::STATUS_CLOSE;
					return -1;
				}
				Ret = sendto(SocketInfo[Loop].Sock, (char*)Buffer, BufferSize, 0, ResAddr->ai_addr, (int)ResAddr->ai_addrlen);
				freeaddrinfo(ResAddr);
			} else {
				memcpy(&Addr, &SocketInfo[Loop].LastAccessedAddr, sizeof(sockaddr_storage));
				Ret = sendto(SocketInfo[Loop].Sock, (char*)Buffer, BufferSize, 0, (sockaddr*)&Addr, sizeof(Addr));
			}

			if (Ret == STKSOCKET_ERROR) {
				PutLog(LOG_SENDERROR, LogId, L"", L"", 0, STKSOCKET_ERRORCODE);
				return Ret;
			}
			PutLog(LOG_UDPSEND, LogId, L"", L"", BufferSize, 0);
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

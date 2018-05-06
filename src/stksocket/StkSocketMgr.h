#pragma once
#include <winsock2.h>
#include <windows.h>
#include "StkSocketInfo.h"

class StkSocketMgr
{
public:
	// 最大ログの数
	static const int MAX_NUM_OF_LOG = 64;
	// ソケットの数（他のノードに接続可能な最大数）
	static const int MAX_SOCKET_NUMBER = 256;

private:
	StkSocketInfo SocketInfo[MAX_SOCKET_NUMBER];
	int NumOfSocketInfo;

	// WSADATA
	WSADATA WsaDat;

	// This instance
	static StkSocketMgr* ThisInstance;

	// Logging information
	int Log[MAX_NUM_OF_LOG];
	int LogId[MAX_NUM_OF_LOG];
	TCHAR LogParamStr1[MAX_NUM_OF_LOG][256];
	TCHAR LogParamStr2[MAX_NUM_OF_LOG][256];
	int LogParamInt1[MAX_NUM_OF_LOG];
	int LogParamInt2[MAX_NUM_OF_LOG];
	int NumOfLogs;

	// Critical section
	CRITICAL_SECTION Cs4Log;

private:
	// Constructor
	StkSocketMgr();
	// Destructor
	~StkSocketMgr();

	// Logging method
	void PutLog(int, int, TCHAR*, TCHAR*, int, int);

	// Safe socket close
	void CloseSocketWaitForPeerClose(SOCKET);

public:
	static const int ACTIONTYPE_RECEIVER = 0;
	static const int ACTIONTYPE_SENDER   = 1;

	static const int SOCKTYPE_STREAM = SOCK_STREAM;
	static const int SOCKTYPE_DGRAM  = SOCK_DGRAM;

	static const int LOG_NAMESOLVEERR     = 1000;
	static const int LOG_CONNERROR        = 1001;
	static const int LOG_SUCCESSCSC       = 1002; // Success Create-Socket and Connect
	static const int LOG_SOCKCLOSE        = 1003;
	static const int LOG_BINDLISTENERR    = 1004;
	static const int LOG_SUCCESSCSBNLS    = 1005; // Success Create-Socket, BiNd and LiSten
	static const int LOG_CLOSEACCEPTSOCK  = 1006;
	static const int LOG_CREATEACCEPTSOCK = 1007;
	static const int LOG_ACPTRECV         = 1008;
	static const int LOG_CNCTRECV         = 1009;
	static const int LOG_RECVERROR        = 1010;
	static const int LOG_SENDERROR        = 1011;
	static const int LOG_ACPTSEND         = 1012;
	static const int LOG_CNCTSEND         = 1013;
	static const int LOG_CLOSEACCLISNSOCK = 1014;
	static const int LOG_BINDERR          = 1015;
	static const int LOG_SUCCESSCSBN      = 1016; // Success Create-Socket and BiNd
	static const int LOG_UDPRECV          = 1017;
	static const int LOG_UDPSEND          = 1018;
	static const int LOG_SUCCESSCS        = 1019; // Success Create-Socket
	static const int LOG_UDPSOCKCLOSE     = 1020;

	static const int RECV_FINISHCOND_UNCONDITIONAL = 0;
	static const int RECV_FINISHCOND_STRING        = -1;
	static const int RECV_FINISHCOND_PEERCLOSURE   = -2;
	static const int RECV_FINISHCOND_CONTENTLENGTH = -3;

	// Get this instance
	static StkSocketMgr* GetInstance();

	// Logging method
	void TakeLastLog(int*, int*, TCHAR*, TCHAR*, int*, int*);
	void TakeFirstLog(int*, int*, TCHAR*, TCHAR*, int*, int*);
	int GetNumOfLogs();
	void ClearLog();

	// Add socket information
	int AddSocketInfo(int, int, int, TCHAR[256], int);
	// Copy the socket information for new ACCEPT
	int CopySocketInfo(int, int);
	// Delete socket information
	int DeleteSocketInfo(int);
	// Get socket information
	int GetSocketInfo(int, int*, int*, int*, TCHAR[256], int*, BOOL*);

	// Socket open/close
	int OpenSocket(int);
	int CloseSocket(int, BOOL);

	// 他ノードへの接続
	int ConnectSocket(int);
	// 他ノードへの接続切断
	int DisconnectSocket(int, int, BOOL);

	// acceptする
	int Accept(int);
	// acceptしたソケットをクローズする
	int CloseAccept(int, int, BOOL);

	// データ受信(TCP)
	int Receive(int, int, BYTE*, int, int, int, BYTE*, int, BOOL);
	// データ送信(TCP)
	int Send(int, int, BYTE*, int);
	// データ受信(UDP)
	int ReceiveUdp(int, int, BYTE*, int);
	// データ送信(UDP)
	int SendUdp(int, int, BYTE*, int);

	// Acquire UDP max message size
	int GetUdpMaxMessageSize(int);

	// Get number of stkthreads
	int GetNumOfStkInfos();

	// Get status
	int GetStatus(int);

};

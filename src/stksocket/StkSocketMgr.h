#pragma once
#include <mutex>

#ifdef WIN32

#include <winsock2.h>

#else

#include <sys/socket.h>

#endif

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

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

	// This instance
	static StkSocketMgr* ThisInstance;

	// Logging information
	int Log[MAX_NUM_OF_LOG];
	int LogId[MAX_NUM_OF_LOG];
	wchar_t LogParamStr1[MAX_NUM_OF_LOG][256];
	wchar_t LogParamStr2[MAX_NUM_OF_LOG][256];
	int LogParamInt1[MAX_NUM_OF_LOG];
	int LogParamInt2[MAX_NUM_OF_LOG];
	int NumOfLogs;

	// Critical section
	std::mutex Cs4Log;

private:
	// Constructor
	StkSocketMgr();
	// Destructor
	~StkSocketMgr();

	// Logging method
	void PutLog(int, int, const wchar_t*, const wchar_t*, int, int);

	// Safe socket close
	void CloseSocketWaitForPeerClose(STK_SOCKET, SSL*, bool*);

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
	static const int LOG_PRIVATEKEY       = 1021;
	static const int LOG_SERVERCERT       = 1022;
	static const int LOG_CACERT           = 1023;

	static const int RECV_FINISHCOND_UNCONDITIONAL =  0;
	static const int RECV_FINISHCOND_STRING        = -1;
	static const int RECV_FINISHCOND_PEERCLOSURE   = -2;
	static const int RECV_FINISHCOND_CONTENTLENGTH = -3;
	static const int RECV_FINISHCOND_TIMEOUT       = -4;
	static const int RECV_FINISHCOND_CHUNK         = -5;

	// Get this instance
	static StkSocketMgr* GetInstance();

	// Logging method
	void TakeLastLog(int*, int*, wchar_t*, wchar_t*, int*, int*);
	void TakeFirstLog(int*, int*, wchar_t*, wchar_t*, int*, int*);
	int GetNumOfLogs();
	void ClearLog();

	// Add socket information
	int AddSocketInfo(int, int, int, const wchar_t[256], int);
	// Copy the socket information for new ACCEPT
	int CopySocketInfo(int, int);
	// Delete socket information
	int DeleteSocketInfo(int);
	// Get socket information
	int GetSocketInfo(int, int*, int*, int*, wchar_t[256], int*, bool*);
	int GetSocketInfo(int, int*, int*, wchar_t[256], int*, bool*);
	// Secure setting
	void InitSecureSetting();
	int SecureForRecv(int, const char*, const char*);
	int SecureForSend(int, const char*, const char*);
	int GetSecureStatus(int);
	int Unsecure(int);

	// Socket open/close
	int OpenSocket(int);
	int CloseSocket(int, bool);

	// 他ノードへの接続
	int ConnectSocket(int);
	// 他ノードへの接続切断
	int DisconnectSocket(int, int, bool);

	// acceptする
	int Accept(int);
	// acceptしたソケットをクローズする
	int CloseAccept(int, int, bool);

	// データ受信(TCP)
	int Receive(int, int, unsigned char*, int, int, int, unsigned char*, int);
	// データ送信(TCP)
	int Send(int, int, const unsigned char*, int);
	// データ受信(UDP)
	int ReceiveUdp(int, int, unsigned char*, int);
	// データ送信(UDP)
	int SendUdp(int, int, const unsigned char*, int);

	// Acquire UDP max message size
	int GetUdpMaxMessageSize(int);

	int ForceStop(int);

	// Get number of stkthreads
	int GetNumOfStkInfos();

	// Get status
	int GetStatus(int);

};

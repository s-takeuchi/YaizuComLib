#pragma once

#ifdef WIN32

#include <winsock2.h>
#define STK_SOCKET SOCKET

#else

#include <sys/socket.h>
#define STK_SOCKET int

#endif

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

struct StkSocketInfo
{
	// Statusのの取り得る値
	static const int STATUS_OPEN = 1;
	static const int STATUS_ACCEPT = 2;
	static const int STATUS_CLOSE = 3;

	// Socket Type
	int SocketType;

	// ViewElementのID
	int ElementId;

	// ソケット
	STK_SOCKET Sock;
	// Acceptしたソケット
	STK_SOCKET AcceptedSock;

	// 状態
	int Status;

	// 動作種別 (Receiver=0またはSender=1)
	int ActionType;

	// ホスト名またはIPアドレス
	// Receiverの場合待ち受け，Senderの場合接続先
	wchar_t HostOrIpAddr[256];
	// In case of UDP receiver, last accessed IP address is stored.
	sockaddr_storage LastAccessedAddr;

	// 通信用ポート番号
	// Receiverの場合待ち受け，Senderの場合接続先
	int Port;

	// Flag which shows whether this socket is copied socket or not.
	bool CopiedSocketFlag;
	// If CopiedSocketFlag is true, Element ID of copy source is set. Otherwise -1 is set.
	int CopySourceId;

	// Force stop flag
	bool ForceStop;

	// Secure setting
	SSL_CTX* SecureCtx;
	SSL* SecureSsl;
};

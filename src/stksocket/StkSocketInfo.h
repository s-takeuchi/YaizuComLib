#pragma once
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>

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
	SOCKET Sock;
	// Acceptしたソケット
	SOCKET AcceptedSock;

	// 状態
	int Status;

	// 動作種別 (Receiver=0またはSender=1)
	int ActionType;

	// ホスト名またはIPアドレス
	// Receiverの場合待ち受け，Senderの場合接続先
	TCHAR HostOrIpAddr[256];
	// In case of UDP receiver, last accessed IP address is stored.
	sockaddr_in LastAccessedAddr;

	// 通信用ポート番号
	// Receiverの場合待ち受け，Senderの場合接続先
	int Port;

	// Flag which shows whether this socket is copied socket or not.
	BOOL CopiedSocketFlag;
	// If CopiedSocketFlag is true, Element ID of copy source is set. Otherwise -1 is set.
	int CopySourceId;

};

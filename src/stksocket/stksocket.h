#pragma once
#include <windows.h>

#define STKSOCKET_MAX_NUM_OF_LOG 64
#define STKSOCKET_MAX_SOCKET_NUMBER 256

#define STKSOCKET_ACTIONTYPE_RECEIVER 0
#define STKSOCKET_ACTIONTYPE_SENDER   1

#define STKSOCKET_TYPE_STREAM SOCK_STREAM
#define STKSOCKET_TYPE_DGRAM  SOCK_DGRAM

#define STKSOCKET_STATUS_OPEN 1
#define STKSOCKET_STATUS_ACCEPT 2
#define STKSOCKET_STATUS_CLOSE 3

#define STKSOCKET_LOG_NAMESOLVEERR      1000
#define STKSOCKET_LOG_CONNERROR         1001
#define STKSOCKET_LOG_SUCCESSCSC        1002 // Success Create-Socket and Connect
#define STKSOCKET_LOG_SOCKCLOSE         1003
#define STKSOCKET_LOG_BINDLISTENERR     1004
#define STKSOCKET_LOG_SUCCESSCSBNLS     1005 // Success Create-Socket, BiNd and LiSten
#define STKSOCKET_LOG_CLOSEACCEPTSOCK   1006
#define STKSOCKET_LOG_CREATEACCEPTSOCK  1007
#define STKSOCKET_LOG_ACPTRECV          1008
#define STKSOCKET_LOG_CNCTRECV          1009
#define STKSOCKET_LOG_RECVERROR         1010
#define STKSOCKET_LOG_SENDERROR         1011
#define STKSOCKET_LOG_ACPTSEND          1012
#define STKSOCKET_LOG_CNCTSEND          1013
#define STKSOCKET_LOG_CLOSEACCLISNSOCK  1014
#define STKSOCKET_LOG_BINDERR           1015
#define STKSOCKET_LOG_SUCCESSCSBN       1016 // Success Create-Socket and BiNd
#define STKSOCKET_LOG_UDPRECV           1017
#define STKSOCKET_LOG_UDPSEND           1018
#define STKSOCKET_LOG_SUCCESSCS         1019 // Success Create-Socket
#define STKSOCKET_LOG_UDPSOCKCLOSE      1020

int StkSocket_AddInfo(int, int, int, TCHAR[256], int);
int StkSocket_DeleteInfo(int);
int StkSocket_GetInfo(int, int*, int*, int*, TCHAR[256], int*, BOOL*);
int StkSocket_CopyInfo(int, int);
int StkSocket_GetNumOfStkInfos();
int StkSocket_GetStatus(int);

int StkSocket_Open(int);
int StkSocket_Close(int, BOOL);
int StkSocket_Connect(int);
int StkSocket_Disconnect(int, int, BOOL);
int StkSocket_Accept(int);
int StkSocket_CloseAccept(int, int, BOOL);

int StkSocket_Receive(int, int, BYTE*, int, int, BYTE*, int, BOOL);
int StkSocket_Receive(int, int, BYTE*, int, int, int, BYTE*, int, BOOL);
int StkSocket_Send(int, int, BYTE*, int);
int StkSocket_ReceiveUdp(int, int, BYTE*, int);
int StkSocket_SendUdp(int, int, BYTE*, int);
int StkSocket_GetUdpMaxMessageSize(int);

void StkSocket_TakeLastLog(int*, int*, TCHAR*, TCHAR*, int*, int*);
void StkSocket_TakeFirstLog(int*, int*, TCHAR*, TCHAR*, int*, int*);
int StkSocket_GetNumOfLogs();
void StkSocket_ClearLog();

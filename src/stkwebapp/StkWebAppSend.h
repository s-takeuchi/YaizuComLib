#pragma once
#include "../commonfunc/StkObject.h"

class StkWebAppSend
{
private:
	class Impl;
	Impl* pImpl;

public:
	static const unsigned char STKWEBAPP_METHOD_UNDEFINED = 0x00;
	static const unsigned char STKWEBAPP_METHOD_GET       = 0x01;
	static const unsigned char STKWEBAPP_METHOD_HEAD      = 0x02;
	static const unsigned char STKWEBAPP_METHOD_POST      = 0x04;
	static const unsigned char STKWEBAPP_METHOD_PUT       = 0x08;
	static const unsigned char STKWEBAPP_METHOD_DELETE    = 0x10;
	static const unsigned char STKWEBAPP_METHOD_OPTIONS   = 0x20;
	static const unsigned char STKWEBAPP_METHOD_INVALID   = 0x40;

	StkWebAppSend(int, const wchar_t*, int);
	virtual ~StkWebAppSend();

	int GetSendBufSize();
	void SetSendBufSize(int);
	int GetRecvBufSize();
	void SetRecvBufSize(int);

	int GetTimeoutInterval();
	void SetTimeoutInterval(int);

	void SetAutholization(const char*);

	StkObject* SendRequestRecvResponse(int, const char*, StkObject*, int*);
};

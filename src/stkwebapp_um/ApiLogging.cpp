﻿#include "ApiLogging.h"
#include "DataAccessUm.h"
#include "UserManagement.h"
#
StkObject* ApiLogging::ExecuteImpl(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3], wchar_t* Token)
{
	wchar_t LogMsgTime[UserManagement::MAXNUM_OF_LOGRECORDS][UserManagement::MAXLEN_OF_LOGTIME];
	wchar_t LogMsgEn[UserManagement::MAXNUM_OF_LOGRECORDS][UserManagement::MAXLEN_OF_LOGMSG];
	wchar_t LogMsgJa[UserManagement::MAXNUM_OF_LOGRECORDS][UserManagement::MAXLEN_OF_LOGMSG];

	DataAccessUm* Dac = DataAccessUm::GetInstance();
	int NumOfRec = Dac->GetLogs(LogMsgTime, LogMsgEn, LogMsgJa);

	StkObject* ResObj = new StkObject(L"");
	wchar_t YourName[UserManagement::MAXLEN_OF_USERNAME] = L"";
	if (!CheckCredentials(Token, YourName)) {
		AddCodeAndMsg(ResObj, UserManagement::UM_AUTH_ERROR, UserManagement::GetMsgEng(UserManagement::UM_AUTH_ERROR), UserManagement::GetMsgJpn(UserManagement::UM_AUTH_ERROR));
		*ResultCode = 401;
		return ResObj;
	}

	AddCodeAndMsg(ResObj, 0, L"", L"");
	StkObject* DatObj = new StkObject(L"Data");
	for (int Loop = 0; Loop < NumOfRec; Loop++) {
		StkObject* Log4Add = new StkObject(L"Log");
		Log4Add->AppendChildElement(new StkObject(L"Time", LogMsgTime[Loop]));
		Log4Add->AppendChildElement(new StkObject(L"MsgEn", LogMsgEn[Loop]));
		Log4Add->AppendChildElement(new StkObject(L"MsgJa", LogMsgJa[Loop]));
		DatObj->AppendChildElement(Log4Add);
	}
	ResObj->AppendChildElement(DatObj);

	*ResultCode = 200;

	return ResObj;
}
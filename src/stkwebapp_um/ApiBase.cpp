#include "../stkpl/StkPl.h"
#include "../commonfunc/StkStringParser.h"
#include "stkwebapp_um.h"
#include "DataAccessUm.h"
#include "ApiBase.h"

void ApiBase::AddCodeAndMsg(StkObject* StkObj, int Code, const wchar_t* MsgEng, const wchar_t* MsgJpn)
{
	StkObj->AppendChildElement(new StkObject(L"Code", Code));
	StkObj->AppendChildElement(new StkObject(L"MsgEng", MsgEng));
	StkObj->AppendChildElement(new StkObject(L"MsgJpn", MsgJpn));
}

void ApiBase::PrintRequest(unsigned int Identifier, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH])
{
	wchar_t StrMethod[32];
	switch (Method) {
	case STKWEBAPP_METHOD_UNDEFINED:
		StkPlWcsCpy(StrMethod, 32, L"Undifined"); break;
	case STKWEBAPP_METHOD_GET:
		StkPlWcsCpy(StrMethod, 32, L"Get"); break;
	case STKWEBAPP_METHOD_HEAD:
		StkPlWcsCpy(StrMethod, 32, L"Head"); break;
	case STKWEBAPP_METHOD_POST:
		StkPlWcsCpy(StrMethod, 32, L"Post"); break;
	case STKWEBAPP_METHOD_PUT:
		StkPlWcsCpy(StrMethod, 32, L"Put"); break;
	case STKWEBAPP_METHOD_DELETE:
		StkPlWcsCpy(StrMethod, 32, L"Delete"); break;
	default:
		StkPlWcsCpy(StrMethod, 32, L"Invalid"); break;
	}
	wchar_t LocalTimeStr[64];
	StkPlGetWTimeInOldFormat(LocalTimeStr, true);
	StkPlWPrintf(L"%08x| %ls   %ls %ls\r\n", Identifier, LocalTimeStr, StrMethod, UrlPath);
}

void ApiBase::PrintResponse(unsigned int Identifier, int ResultCode)
{
	wchar_t LocalTimeStr[64];
	StkPlGetWTimeInOldFormat(LocalTimeStr, true);
	StkPlWPrintf(L"%08x| %ls   %d\r\n", Identifier, LocalTimeStr, ResultCode);
}

void ApiBase::DebugObject(StkObject* StkObj)
{
	wchar_t DebugStr[65536];
	StkObj->ToJson(DebugStr, 65536);
	StkPlWPrintf(L"%ls\r\n", DebugStr);
}

bool ApiBase::CheckCredentials(wchar_t* Token, wchar_t* Name)
{
	if (Token == NULL || *Token == L'\0') {
		return false;
	}

	wchar_t TmpName[UserManagement::MAXLEN_OF_USERNAME] = L"";
	wchar_t TmpPassword[UserManagement::MAXLEN_OF_PASSWORD] = L"";
	StkStringParser::ParseInto2Params(Token, L"# #", L'#', TmpName, UserManagement::MAXLEN_OF_USERNAME, TmpPassword, UserManagement::MAXLEN_OF_PASSWORD);
	if (TmpName == NULL || TmpPassword == NULL || *TmpName == L'\0' || *TmpPassword == L'\0') {
		return false;
	}

	int Id = 0;
	wchar_t Password[UserManagement::MAXLEN_OF_PASSWORD];
	int Role = 0;
	bool Ret = DataAccessUm::GetInstance()->GetTargetUserByName(TmpName, &Id, Password, &Role);
	if (Ret == false) {
		return false;
	}

	if (StkPlWcsCmp(TmpPassword, Password) == 0) {
		if (Name != NULL) {
			StkPlWcsCpy(Name, UserManagement::MAXLEN_OF_USERNAME, TmpName);
		}
		return true;
	} else {
		return false;
	}
}

bool ApiBase::IsAdminUser(wchar_t* Token)
{
	int UserId = 0;
	wchar_t UserName[UserManagement::MAXLEN_OF_USERNAME] = L"";
	wchar_t UserPassword[UserManagement::MAXLEN_OF_PASSWORD] = L"";
	int Role = 0;
	if (!CheckCredentials(Token, UserName)) {
		return false;
	}
	DataAccessUm::GetInstance()->GetTargetUserByName(UserName, &UserId, UserPassword, &Role);
	if (Role != 0) {
		return false;
	}
	return true;
}

StkObject* ApiBase::Execute(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t* HttpHeader)
{
	static unsigned int Identifier = 0;
	Identifier++;
	unsigned int LocalId = Identifier;
	PrintRequest(LocalId, Method, UrlPath);

	wchar_t Locale[3] = L"";;
	StkStringParser::ParseInto2Params(HttpHeader, L"#Accept-Language: #", L'#', NULL, 0, Locale, 3);

	wchar_t Token[256] = L"";
	StkStringParser::ParseInto4Params(HttpHeader, L"#Authorization:#Bearer #\r\n#", L'#', NULL, 0, NULL, 0, Token, 256, NULL, 0);

	StkObject* RetObj = ExecuteImpl(ReqObj, Method, UrlPath, ResultCode, Locale, Token);

	PrintResponse(LocalId, *ResultCode);

	return RetObj;
}

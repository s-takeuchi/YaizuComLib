#include "../stkpl/StkPl.h"
#include "UserManagement.h"
#include "ApiDeleteUser.h"
#include "DataAccessUm.h"
#include "../commonfunc/StkStringParser.h"

StkObject* ApiDeleteUser::ExecuteImpl(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3], wchar_t* Token)
{
	StkObject* ResObj = new StkObject(L"");

	wchar_t YourName[UserManagement::MAXLEN_OF_USERNAME] = L"";
	if (!CheckCredentials(Token, YourName)) {
		AddCodeAndMsg(ResObj, UserManagement::UM_AUTH_ERROR, UserManagement::GetMsgEng(UserManagement::UM_AUTH_ERROR), UserManagement::GetMsgJpn(UserManagement::UM_AUTH_ERROR));
		*ResultCode = 401;
		return ResObj;
	}

	if (!IsAdminUser(Token)) {
		AddCodeAndMsg(ResObj, UserManagement::UM_ACCESS_RIGHT_ERROR, UserManagement::GetMsgEng(UserManagement::UM_ACCESS_RIGHT_ERROR), UserManagement::GetMsgJpn(UserManagement::UM_ACCESS_RIGHT_ERROR));
		*ResultCode = 403;
		return ResObj;
	}

	wchar_t Dummy[256] = L"";
	wchar_t UserIdStr[16] = L"";
	if (StkPlWcsStr(UrlPath, L"/")) {
		StkStringParser::ParseInto2Params(UrlPath, L"$user/$/", L'$', Dummy, 256, UserIdStr, 16);
	}
	if (UserIdStr == NULL || UserIdStr[0] == L'\0') {
		AddCodeAndMsg(ResObj, UserManagement::UM_REQ_NOT_SUFFICIENT, UserManagement::GetMsgEng(UserManagement::UM_REQ_NOT_SUFFICIENT), UserManagement::GetMsgJpn(UserManagement::UM_REQ_NOT_SUFFICIENT));
		*ResultCode = 400;
		return ResObj;
	}
	int UserId = StkPlWcsToL(UserIdStr);

	wchar_t TmpName[UserManagement::MAXLEN_OF_USERNAME] = L"";
	wchar_t TmpPassword[UserManagement::MAXLEN_OF_PASSWORD] = L"";
	int TmpRole = -1;
	if (DataAccessUm::GetInstance()->GetTargetUserById(UserId, TmpName, TmpPassword, &TmpRole) == false) {
		AddCodeAndMsg(ResObj, UserManagement::UM_USER_DOES_NOT_EXIST, UserManagement::GetMsgEng(UserManagement::UM_USER_DOES_NOT_EXIST), UserManagement::GetMsgJpn(UserManagement::UM_USER_DOES_NOT_EXIST));
		*ResultCode = 400;
		return ResObj;
	}
	if (StkPlWcsCmp(YourName, TmpName) == 0) {
		AddCodeAndMsg(ResObj, UserManagement::UM_CANNOT_MODIFY_YOUR_INFO, UserManagement::GetMsgEng(UserManagement::UM_CANNOT_MODIFY_YOUR_INFO), UserManagement::GetMsgJpn(UserManagement::UM_CANNOT_MODIFY_YOUR_INFO));
		*ResultCode = 400;
		return ResObj;
	}

	DataAccessUm::GetInstance()->DeleteUser(UserId);

	wchar_t LogBufEng[512] = L"";
	wchar_t LogBufJpn[512] = L"";
	StkPlSwPrintf(LogBufEng, 512, UserManagement::GetMsgEng(UserManagement::UM_USER_DELETE), TmpName);
	StkPlSwPrintf(LogBufJpn, 512, UserManagement::GetMsgJpn(UserManagement::UM_USER_DELETE), TmpName);
	DataAccessUm::GetInstance()->AddLogMsg(LogBufEng, LogBufJpn);

	AddCodeAndMsg(ResObj, 0, L"", L"");
	*ResultCode = 200;
	return ResObj;
}

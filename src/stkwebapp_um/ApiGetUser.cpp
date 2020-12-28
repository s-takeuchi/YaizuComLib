#include "../stkpl/StkPl.h"
#include "UserManagement.h"
#include "ApiGetUser.h"
#include "DataAccessUm.h"

StkObject* ApiGetUser::ExecuteImpl(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3], wchar_t* Token)
{
	int UserId = 0;
	wchar_t UserName[UserManagement::MAXLEN_OF_USERNAME] = L"";
	wchar_t UserPassword[UserManagement::MAXLEN_OF_PASSWORD] = L"";
	int Role = 0;
	StkObject* TmpObj = new StkObject(L"");
	if (!CheckCredentials(Token, UserName)) {
		AddCodeAndMsg(TmpObj, UserManagement::UM_AUTH_ERROR, UserManagement::GetMsgEng(UserManagement::UM_AUTH_ERROR), UserManagement::GetMsgJpn(UserManagement::UM_AUTH_ERROR));
		*ResultCode = 401;
		return TmpObj;
	}
	DataAccessUm::GetInstance()->GetTargetUserByName(UserName, &UserId, UserPassword, &Role);
	if (StkPlWcsStr(UrlPath, L"?target=all") != NULL) {
		if (Role != 0) {
			AddCodeAndMsg(TmpObj, UserManagement::UM_ACCESS_RIGHT_ERROR, UserManagement::GetMsgEng(UserManagement::UM_ACCESS_RIGHT_ERROR), UserManagement::GetMsgJpn(UserManagement::UM_ACCESS_RIGHT_ERROR));
			*ResultCode = 403;
			return TmpObj;
		}
		int AryUserId[UserManagement::MAXNUM_OF_USERRECORDS];
		wchar_t AryUserName[UserManagement::MAXNUM_OF_USERRECORDS][UserManagement::MAXLEN_OF_USERNAME];
		wchar_t AryUserPassword[UserManagement::MAXNUM_OF_USERRECORDS][UserManagement::MAXLEN_OF_PASSWORD];
		int AryRole[UserManagement::MAXNUM_OF_USERRECORDS];
		int Cnt = DataAccessUm::GetInstance()->GetTargetUsers(AryUserId, AryUserName, AryUserPassword, AryRole);
		StkObject* TmpObjD = new StkObject(L"Data");
		for (int Loop = 0; Loop < Cnt; Loop++) {
			StkObject* TmpObjC = new StkObject(L"User");
			TmpObjC->AppendChildElement(new StkObject(L"Id", AryUserId[Loop]));
			TmpObjC->AppendChildElement(new StkObject(L"Name", AryUserName[Loop]));
			TmpObjC->AppendChildElement(new StkObject(L"Role", AryRole[Loop]));
			TmpObjD->AppendChildElement(TmpObjC);
		}
		TmpObj->AppendChildElement(TmpObjD);
		AddCodeAndMsg(TmpObj, 0, L"", L"");
		*ResultCode = 200;
		return TmpObj;
	} else {
		StkObject* TmpObjC = new StkObject(L"User");
		TmpObjC->AppendChildElement(new StkObject(L"Id", UserId));
		TmpObjC->AppendChildElement(new StkObject(L"Name", UserName));
		TmpObjC->AppendChildElement(new StkObject(L"Role", Role));
		StkObject* TmpObjD = new StkObject(L"Data");
		TmpObjD->AppendChildElement(TmpObjC);
		TmpObj->AppendChildElement(TmpObjD);
		AddCodeAndMsg(TmpObj, 0, L"", L"");
		*ResultCode = 200;
		return TmpObj;
	}
	return TmpObj;
}

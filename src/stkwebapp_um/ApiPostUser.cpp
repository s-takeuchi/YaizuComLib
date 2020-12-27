#include "../stkpl/StkPl.h"
#include "UserManagement.h"
#include "ApiPostUser.h"
#include "DataAccessUm.h"
#include "stkwebapp_um.h"

StkObject* ApiPostUser::ExecuteImpl(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3], wchar_t* Token)
{
	StkObject* ResObj = new StkObject(L"");

	wchar_t YourName[MAXLEN_OF_USERNAME] = L"";
	if (!CheckCredentials(Token, YourName)) {
		AddCodeAndMsg(ResObj, UserManagement::UM_AUTH_ERROR, UserManagement::GetMsgEng(UserManagement::UM_AUTH_ERROR), UserManagement::GetMsgJpn(UserManagement::UM_AUTH_ERROR));
		*ResultCode = 401;
		return ResObj;
	}

	if (ReqObj == NULL) {
		AddCodeAndMsg(ResObj, UserManagement::UM_REQ_NOT_SUFFICIENT, UserManagement::GetMsgEng(UserManagement::UM_REQ_NOT_SUFFICIENT), UserManagement::GetMsgJpn(UserManagement::UM_REQ_NOT_SUFFICIENT));
		*ResultCode = 400;
		return ResObj;
	}
	int Id = -1;
	wchar_t Name[MAXLEN_OF_USERNAME] = L"";
	wchar_t Password[MAXLEN_OF_PASSWORD] = L"";
	int Role = -1;
	StkObject* CurObj = ReqObj->GetFirstChildElement();
	if (CurObj == NULL) {
		AddCodeAndMsg(ResObj, UserManagement::UM_REQ_NOT_SUFFICIENT, UserManagement::GetMsgEng(UserManagement::UM_REQ_NOT_SUFFICIENT), UserManagement::GetMsgJpn(UserManagement::UM_REQ_NOT_SUFFICIENT));
		*ResultCode = 400;
		return ResObj;
	}
	while (CurObj) {
		if (StkPlWcsCmp(CurObj->GetName(), L"Id") == 0) {
			Id = CurObj->GetIntValue();
		} else if (StkPlWcsCmp(CurObj->GetName(), L"Name") == 0) {
			if (StkPlWcsLen(CurObj->GetStringValue()) >= MAXLEN_OF_USERNAME) {
				AddCodeAndMsg(ResObj, UserManagement::UM_PARAM_LENGTH_TOO_LONG, UserManagement::GetMsgEng(UserManagement::UM_PARAM_LENGTH_TOO_LONG), UserManagement::GetMsgJpn(UserManagement::UM_PARAM_LENGTH_TOO_LONG));
				*ResultCode = 400;
				return ResObj;
			}
			StkPlWcsCpy(Name, MAXLEN_OF_USERNAME, CurObj->GetStringValue());

			if (StkPlWcsLen(Name) <= 3) {
				AddCodeAndMsg(ResObj, UserManagement::UM_INVALID_LEN_OF_USER_NAME, UserManagement::GetMsgEng(UserManagement::UM_INVALID_LEN_OF_USER_NAME), UserManagement::GetMsgJpn(UserManagement::UM_INVALID_LEN_OF_USER_NAME));
				*ResultCode = 400;
				return ResObj;
			}

			wchar_t* PtrName = Name;
			while (*PtrName) {
				if (!(*PtrName >= L'a' && *PtrName <= L'z') && !(*PtrName >= L'A' && *PtrName <= L'Z') && !(*PtrName >= L'0' && *PtrName <= L'9') && 
					*PtrName != L'@' && *PtrName != L'.' && *PtrName != L'-' && *PtrName != L'_' && *PtrName != L'/') {
					AddCodeAndMsg(ResObj, UserManagement::UM_INVALID_USER_NAME, UserManagement::GetMsgEng(UserManagement::UM_INVALID_USER_NAME), UserManagement::GetMsgJpn(UserManagement::UM_INVALID_USER_NAME));
					*ResultCode = 400;
					return ResObj;
				}
				PtrName++;
			}
		} else if (StkPlWcsCmp(CurObj->GetName(), L"Password") == 0) {
			if (StkPlWcsLen(CurObj->GetStringValue()) >= MAXLEN_OF_PASSWORD) {
				AddCodeAndMsg(ResObj, UserManagement::UM_PARAM_LENGTH_TOO_LONG, UserManagement::GetMsgEng(UserManagement::UM_PARAM_LENGTH_TOO_LONG), UserManagement::GetMsgJpn(UserManagement::UM_PARAM_LENGTH_TOO_LONG));
				*ResultCode = 400;
				return ResObj;
			}
			StkPlWcsCpy(Password, MAXLEN_OF_PASSWORD, CurObj->GetStringValue());

			if (StkPlWcsLen(Password) <= 3) {
				AddCodeAndMsg(ResObj, UserManagement::UM_INVALID_LEN_OF_PASSWORD, UserManagement::GetMsgEng(UserManagement::UM_INVALID_LEN_OF_PASSWORD), UserManagement::GetMsgJpn(UserManagement::UM_INVALID_LEN_OF_PASSWORD));
				*ResultCode = 400;
				return ResObj;
			}

			wchar_t* PtrPassword = Password;
			while (*PtrPassword) {
				if (!(*PtrPassword >= L'a' && *PtrPassword <= L'z') && !(*PtrPassword >= L'A' && *PtrPassword <= L'Z') && !(*PtrPassword >= L'0' && *PtrPassword <= L'9') &&
					*PtrPassword != L'!' && *PtrPassword != L'?' && *PtrPassword != L'.' && *PtrPassword != L'+' && *PtrPassword != L'-' && *PtrPassword != L'$' &&
					*PtrPassword != L'%' && *PtrPassword != L'#' && *PtrPassword != L'&' && *PtrPassword != L'*' && *PtrPassword != L'/' && *PtrPassword != L'=' && *PtrPassword != L'@') {
					AddCodeAndMsg(ResObj, UserManagement::UM_INVALID_PASSWORD, UserManagement::GetMsgEng(UserManagement::UM_INVALID_PASSWORD), UserManagement::GetMsgJpn(UserManagement::UM_INVALID_PASSWORD));
					*ResultCode = 400;
					return ResObj;
				}
				PtrPassword++;
			}
		} else if (StkPlWcsCmp(CurObj->GetName(), L"Role") == 0) {
			Role = CurObj->GetIntValue();
		}
		CurObj = CurObj->GetNext();
	}
	// In case both of name or role is not specified for addition scenario.
	if (Id == -1 && (StkPlWcsCmp(Name, L"") == 0 || Role == -1)) {
		AddCodeAndMsg(ResObj, UserManagement::UM_REQ_NOT_SUFFICIENT, UserManagement::GetMsgEng(UserManagement::UM_REQ_NOT_SUFFICIENT), UserManagement::GetMsgJpn(UserManagement::UM_REQ_NOT_SUFFICIENT));
		*ResultCode = 400;
		return ResObj;
	}

	int TmpId = -1;
	wchar_t TmpName[MAXLEN_OF_USERNAME] = L"";
	wchar_t TmpPassword[MAXLEN_OF_PASSWORD] = L"";
	int TmpRole = -1;
	// In case number of users exceeds the maximum.
	if (Id == -1 && DataAccessUm::GetInstance()->GetNumberOfUsers() + 1 > MAXNUM_OF_USERRECORDS) {
		AddCodeAndMsg(ResObj, UserManagement::UM_EXCEEDED_MAX_NUM_OF_USER, UserManagement::GetMsgEng(UserManagement::UM_EXCEEDED_MAX_NUM_OF_USER), UserManagement::GetMsgJpn(UserManagement::UM_EXCEEDED_MAX_NUM_OF_USER));
		*ResultCode = 400;
		return ResObj;
	}
	// In case user which has the specified ID does not exist.
	if (Id != -1 && DataAccessUm::GetInstance()->GetTargetUserById(Id, TmpName, TmpPassword, &TmpRole) == false) {
		AddCodeAndMsg(ResObj, UserManagement::UM_USER_DOES_NOT_EXIST, UserManagement::GetMsgEng(UserManagement::UM_USER_DOES_NOT_EXIST), UserManagement::GetMsgJpn(UserManagement::UM_USER_DOES_NOT_EXIST));
		*ResultCode = 400;
		return ResObj;
	}
	// In case (1) Update & (2) target is your information & (3) only id and password is presented
	if (Id != -1 && StkPlWcsCmp(YourName, TmpName) == 0 && StkPlWcsCmp(Name, L"") == 0 && Role == -1) {
	} else {
		if (!IsAdminUser(Token)) {
			AddCodeAndMsg(ResObj, UserManagement::UM_ACCESS_RIGHT_ERROR, UserManagement::GetMsgEng(UserManagement::UM_ACCESS_RIGHT_ERROR), UserManagement::GetMsgJpn(UserManagement::UM_ACCESS_RIGHT_ERROR));
			*ResultCode = 403;
			return ResObj;
		}
	}
	// In case user information yourself (regarding name and/or role) is changed.
	if (!(StkPlWcsCmp(Name, L"") == 0 || Role == -1) && StkPlWcsCmp(YourName, TmpName) == 0) {
		AddCodeAndMsg(ResObj, UserManagement::UM_CANNOT_MODIFY_YOUR_INFO, UserManagement::GetMsgEng(UserManagement::UM_CANNOT_MODIFY_YOUR_INFO), UserManagement::GetMsgJpn(UserManagement::UM_CANNOT_MODIFY_YOUR_INFO));
		*ResultCode = 400;
		return ResObj;
	}
	// User name is changed, and the name already exists.
	if (StkPlWcsCmp(TmpName, Name) != 0 && DataAccessUm::GetInstance()->GetTargetUserByName(Name, &TmpId, TmpPassword, &TmpRole) == true) {
		AddCodeAndMsg(ResObj, UserManagement::UM_SAME_USER_NAME_EXIST, UserManagement::GetMsgEng(UserManagement::UM_SAME_USER_NAME_EXIST), UserManagement::GetMsgJpn(UserManagement::UM_SAME_USER_NAME_EXIST));
		*ResultCode = 400;
		return ResObj;
	}
	// Password not specified for add user operation
	if (Id == -1 && StkPlWcsCmp(Password, L"") == 0) {
		AddCodeAndMsg(ResObj, UserManagement::UM_PASSWORD_REQUIRED_FOR_ADD, UserManagement::GetMsgEng(UserManagement::UM_PASSWORD_REQUIRED_FOR_ADD), UserManagement::GetMsgJpn(UserManagement::UM_PASSWORD_REQUIRED_FOR_ADD));
		*ResultCode = 400;
		return ResObj;
	}
	AddCodeAndMsg(ResObj, 0, L"", L"");
	*ResultCode = 200;
	if (Id == -1) {
		DataAccessUm::GetInstance()->AddUser(Name, Role, Password);
		wchar_t LogBufEng[512] = L"";
		wchar_t LogBufJpn[512] = L"";
		StkPlSwPrintf(LogBufEng, 512, UserManagement::GetMsgEng(UserManagement::UM_USER_ADD), Name);
		StkPlSwPrintf(LogBufJpn, 512, UserManagement::GetMsgJpn(UserManagement::UM_USER_ADD), Name);
		DataAccessUm::GetInstance()->AddLogMsg(LogBufEng, LogBufJpn);
	} else {
		DataAccessUm::GetInstance()->UpdateUser(Id, Name, Role, Password);
		wchar_t LogBufEng[512] = L"";
		wchar_t LogBufJpn[512] = L"";
		StkPlSwPrintf(LogBufEng, 512, UserManagement::GetMsgEng(UserManagement::UM_USER_EDIT), TmpName);
		StkPlSwPrintf(LogBufJpn, 512, UserManagement::GetMsgJpn(UserManagement::UM_USER_EDIT), TmpName);
		DataAccessUm::GetInstance()->AddLogMsg(LogBufEng, LogBufJpn);
	}

	{
		int RetId = -1;
		wchar_t RetPassword[MAXLEN_OF_PASSWORD] = L"";
		int RetRole = -1;

		DataAccessUm::GetInstance()->GetTargetUserByName(Name, &RetId, RetPassword, &RetRole);
		StkObject* RetData = new StkObject(L"Data");
		StkObject* RetUser = new StkObject(L"User");
		RetUser->AppendChildElement(new StkObject(L"Id", RetId));
		RetUser->AppendChildElement(new StkObject(L"Name", Name));
		RetUser->AppendChildElement(new StkObject(L"Role", RetRole));
		RetData->AppendChildElement(RetUser);
		ResObj->AppendChildElement(RetData);
	}

	return ResObj;
}

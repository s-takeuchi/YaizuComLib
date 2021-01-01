#include "../stkpl/StkPl.h"
#include "UserManagement.h"
#include "ApiGetLanguage.h"

StkObject* ApiGetLanguage::ExecuteImpl(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3], wchar_t* Token)
{
	StkObject* ResObj = new StkObject(L"");
	if (Locale == NULL || Locale[2] != '\0' || Locale[0] == '\0') {
		AddCodeAndMsg(ResObj, UserManagement::UM_NO_CLIENTLOCALE, UserManagement::GetMsgEng(UserManagement::UM_NO_CLIENTLOCALE), UserManagement::GetMsgJpn(UserManagement::UM_NO_CLIENTLOCALE));
		*ResultCode = 406;
	} else {
		AddCodeAndMsg(ResObj, 0, L"", L"");
		StkObject* DatObj = new StkObject(L"Data");
		DatObj->AppendChildElement(new StkObject(L"ClientLanguage", Locale));
		ResObj->AppendChildElement(DatObj);
		*ResultCode = 200;
	}
	return ResObj;
}

#include "../commonfunc/msgproc.h"
#include "../stkwebapp\StkWebApp.h"
#include "UserManagement.h"
#include "ApiLogging.h"
#include "ApiGetUser.h"
#include "ApiPostUser.h"
#include "ApiDeleteUser.h"
#include "ApiGetLanguage.h"

void UserManagement::AddMsg()
{
	//(★)
	MessageProc::AddJpn(UM_AUTH_ERROR, L"認証エラーが発生しました。");
	MessageProc::AddEng(UM_AUTH_ERROR, L"Authentication error occurred.");

	//(★)
	MessageProc::AddJpn(UM_ACCESS_RIGHT_ERROR, L"アクセス権限エラーが発生しました。");
	MessageProc::AddEng(UM_ACCESS_RIGHT_ERROR, L"Access right error occurred.");

	//(★)
	MessageProc::AddJpn(UM_USER_DOES_NOT_EXIST, L"指定したユーザーは存在しません。");
	MessageProc::AddEng(UM_USER_DOES_NOT_EXIST, L"The specified user does not exist.");

	//(★)
	MessageProc::AddJpn(UM_CANNOT_MODIFY_YOUR_INFO, L"自分自身のユーザー情報を変更することはできません。");
	MessageProc::AddEng(UM_CANNOT_MODIFY_YOUR_INFO, L"You cannot modify user information yourself.");

	//(★)
	MessageProc::AddJpn(UM_REQ_NOT_SUFFICIENT, L"必要なパラメータがリクエストに含まれていません。");
	MessageProc::AddEng(UM_REQ_NOT_SUFFICIENT, L"The necessary parameter(s) is/are not contained in the request.");

	//(★)
	MessageProc::AddJpn(UM_PARAM_LENGTH_TOO_LONG, L"指定されたパラメータは，指定可能な長さの最大を超えています。");
	MessageProc::AddEng(UM_PARAM_LENGTH_TOO_LONG, L"The specified parameter exceeds the maximum length.");

	//(★)
	MessageProc::AddJpn(UM_INVALID_LEN_OF_USER_NAME, L"ユーザー名は4文字以上である必要があります。");
	MessageProc::AddEng(UM_INVALID_LEN_OF_USER_NAME, L"The user name needs to contain at least 4 characters.");

	//(★)
	MessageProc::AddJpn(UM_INVALID_LEN_OF_PASSWORD, L"パスワードは4文字以上である必要があります。");
	MessageProc::AddEng(UM_INVALID_LEN_OF_PASSWORD, L"The password needs to contain at least 4 characters.");

	//(★)
	MessageProc::AddJpn(UM_INVALID_USER_NAME, L"指定されたユーザー名に使用禁止文字が含まれています。");
	MessageProc::AddEng(UM_INVALID_USER_NAME, L"The specified user name contains prohibited character(s).");

	//(★)
	MessageProc::AddJpn(UM_INVALID_PASSWORD, L"指定されたパスワードに使用禁止文字が含まれています。");
	MessageProc::AddEng(UM_INVALID_PASSWORD, L"The specified password contains prohibited character(s).");

	//(★)
	MessageProc::AddJpn(UM_EXCEEDED_MAX_NUM_OF_USER, L"登録可能なユーザーの最大数を超えました。");
	MessageProc::AddEng(UM_EXCEEDED_MAX_NUM_OF_USER, L"The number of users has been exceeded the maximum.");

	//(★)
	MessageProc::AddJpn(UM_SAME_USER_NAME_EXIST, L"すでに同名のユーザーが存在するため，ユーザー情報を追加または変更できません。");
	MessageProc::AddEng(UM_SAME_USER_NAME_EXIST, L"You cannot add / modify user information because the user who has the same name you specified exists.");

	//(★)
	MessageProc::AddJpn(UM_PASSWORD_REQUIRED_FOR_ADD, L"ユーザーを追加する場合，パスワードを指定する必要があります。");
	MessageProc::AddEng(UM_PASSWORD_REQUIRED_FOR_ADD, L"In case user addition, a password needs to be specified.");

	//(★)
	MessageProc::AddJpn(UM_NO_CLIENTLOCALE, L"クライアントのロケールを検知できません。");
	MessageProc::AddEng(UM_NO_CLIENTLOCALE, L"Client locale cannot be detected.");

	//(★)
	MessageProc::AddJpn(UM_USER_ADD, L"新規にユーザーを追加しました。 [%ls]");
	MessageProc::AddEng(UM_USER_ADD, L"New user has been added. [%ls]");

	//(★)
	MessageProc::AddJpn(UM_USER_DELETE, L"ユーザーを削除しました。 [%ls]");
	MessageProc::AddEng(UM_USER_DELETE, L"A user has been deleted. [%ls]");

	//(★)
	MessageProc::AddJpn(UM_USER_EDIT, L"ユーザー情報を変更しました。 [%ls]");
	MessageProc::AddEng(UM_USER_EDIT, L"A user information has been modified. [%ls]");

}

wchar_t* UserManagement::GetMsg(int Id)
{
	return MessageProc::GetMsg(Id);
}

char* UserManagement::GetMsgSjis(int Id)
{
	return MessageProc::GetMsgSjis(Id);
}

wchar_t* UserManagement::GetMsgJpn(int Id)
{
	return MessageProc::GetMsgJpn(Id);
}

wchar_t* UserManagement::GetMsgEng(int Id)
{
	return MessageProc::GetMsgEng(Id);
}

void UserManagement::RegisterApi(StkWebApp* Swa)
{
	ApiLogging* ApiLoggingObj = new ApiLogging();
	Swa->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/api/logs/", (StkWebAppExec*)ApiLoggingObj);

	ApiGetUser* ApiGetUserObj = new ApiGetUser();
	Swa->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/api/user$", (StkWebAppExec*)ApiGetUserObj);

	ApiPostUser* ApiPostUserObj = new ApiPostUser();
	Swa->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/api/user/", (StkWebAppExec*)ApiPostUserObj);

	ApiDeleteUser* ApiDeleteUserObj = new ApiDeleteUser();
	Swa->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_DELETE, L"/api/user$", (StkWebAppExec*)ApiDeleteUserObj);

	ApiGetLanguage* ApiGetLanguageObj = new ApiGetLanguage();
	Swa->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/api/language/", (StkWebAppExec*)ApiGetLanguageObj);
}

void UserManagement::UnregisterApi(StkWebApp* Swa)
{
	Swa->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/api/logs/");
	Swa->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/api/user$");
	Swa->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/api/user/");
	Swa->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_DELETE, L"/api/user$");
	Swa->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/api/language/");
}

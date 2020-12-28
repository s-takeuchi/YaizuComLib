#include "../commonfunc/msgproc.h"
#include "../stkwebapp\StkWebApp.h"
#include "UserManagement.h"
#include "ApiLogging.h"
#include "ApiGetUser.h"
#include "ApiPostUser.h"
#include "ApiDeleteUser.h"

void UserManagement::AddMsg()
{
	//(��)
	MessageProc::AddJpn(UM_AUTH_ERROR, L"�F�؃G���[���������܂����B");
	MessageProc::AddEng(UM_AUTH_ERROR, L"Authentication error occurred.");

	//(��)
	MessageProc::AddJpn(UM_ACCESS_RIGHT_ERROR, L"�A�N�Z�X�����G���[���������܂����B");
	MessageProc::AddEng(UM_ACCESS_RIGHT_ERROR, L"Access right error occurred.");

	//(��)
	MessageProc::AddJpn(UM_USER_DOES_NOT_EXIST, L"�w�肵�����[�U�[�͑��݂��܂���B");
	MessageProc::AddEng(UM_USER_DOES_NOT_EXIST, L"The specified user does not exist.");

	//(��)
	MessageProc::AddJpn(UM_CANNOT_MODIFY_YOUR_INFO, L"�������g�̃��[�U�[����ύX���邱�Ƃ͂ł��܂���B");
	MessageProc::AddEng(UM_CANNOT_MODIFY_YOUR_INFO, L"You cannot modify user information yourself.");

	//(��)
	MessageProc::AddJpn(UM_REQ_NOT_SUFFICIENT, L"�K�v�ȃp�����[�^�����N�G�X�g�Ɋ܂܂�Ă��܂���B");
	MessageProc::AddEng(UM_REQ_NOT_SUFFICIENT, L"The necessary parameter(s) is/are not contained in the request.");

	//(��)
	MessageProc::AddJpn(UM_PARAM_LENGTH_TOO_LONG, L"�w�肳�ꂽ�p�����[�^�́C�w��\�Ȓ����̍ő�𒴂��Ă��܂��B");
	MessageProc::AddEng(UM_PARAM_LENGTH_TOO_LONG, L"The specified parameter exceeds the maximum length.");

	//(��)
	MessageProc::AddJpn(UM_INVALID_LEN_OF_USER_NAME, L"���[�U�[����4�����ȏ�ł���K�v������܂��B");
	MessageProc::AddEng(UM_INVALID_LEN_OF_USER_NAME, L"The user name needs to contain at least 4 characters.");

	//(��)
	MessageProc::AddJpn(UM_INVALID_LEN_OF_PASSWORD, L"�p�X���[�h��4�����ȏ�ł���K�v������܂��B");
	MessageProc::AddEng(UM_INVALID_LEN_OF_PASSWORD, L"The password needs to contain at least 4 characters.");

	//(��)
	MessageProc::AddJpn(UM_INVALID_USER_NAME, L"�w�肳�ꂽ���[�U�[���Ɏg�p�֎~�������܂܂�Ă��܂��B");
	MessageProc::AddEng(UM_INVALID_USER_NAME, L"The specified user name contains prohibited character(s).");

	//(��)
	MessageProc::AddJpn(UM_INVALID_PASSWORD, L"�w�肳�ꂽ�p�X���[�h�Ɏg�p�֎~�������܂܂�Ă��܂��B");
	MessageProc::AddEng(UM_INVALID_PASSWORD, L"The specified password contains prohibited character(s).");

	//(��)
	MessageProc::AddJpn(UM_EXCEEDED_MAX_NUM_OF_USER, L"�o�^�\�ȃ��[�U�[�̍ő吔�𒴂��܂����B");
	MessageProc::AddEng(UM_EXCEEDED_MAX_NUM_OF_USER, L"The number of users has been exceeded the maximum.");

	//(��)
	MessageProc::AddJpn(UM_SAME_USER_NAME_EXIST, L"���łɓ����̃��[�U�[�����݂��邽�߁C���[�U�[����ǉ��܂��͕ύX�ł��܂���B");
	MessageProc::AddEng(UM_SAME_USER_NAME_EXIST, L"You cannot add / modify user information because the user who has the same name you specified exists.");

	//(��)
	MessageProc::AddJpn(UM_PASSWORD_REQUIRED_FOR_ADD, L"���[�U�[��ǉ�����ꍇ�C�p�X���[�h���w�肷��K�v������܂��B");
	MessageProc::AddEng(UM_PASSWORD_REQUIRED_FOR_ADD, L"In case user addition, a password needs to be specified.");

	//(��)
	MessageProc::AddJpn(UM_USER_ADD, L"�V�K�Ƀ��[�U�[��ǉ����܂����B [%ls]");
	MessageProc::AddEng(UM_USER_ADD, L"New user has been added. [%ls]");

	//(��)
	MessageProc::AddJpn(UM_USER_DELETE, L"���[�U�[���폜���܂����B [%ls]");
	MessageProc::AddEng(UM_USER_DELETE, L"A user has been deleted. [%ls]");

	//(��)
	MessageProc::AddJpn(UM_USER_EDIT, L"���[�U�[����ύX���܂����B [%ls]");
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
}

void UserManagement::UnregisterApi(StkWebApp* Swa)
{
	Swa->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/api/logs/");
	Swa->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/api/user$");
	Swa->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/api/user/");
	Swa->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_DELETE, L"/api/user$");
}

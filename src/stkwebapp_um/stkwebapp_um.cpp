#include "stkwebapp_um.h"
#include "UserManagement.h"
#include "DataAccessUm.h"

void StkWebAppUm_RegisterApi(StkWebApp* Swa)
{
	UserManagement::RegisterApi(Swa);
}

void StkWebAppUm_UnregisterApi(StkWebApp* Swa)
{
	UserManagement::UnregisterApi(Swa);
}

int StkWebAppUm_CreateTable()
{
	return DataAccessUm::GetInstance()->CreateUserTable();
}

void StkWebAppUm_Init()
{
	UserManagement::AddMsg();
}

int StkWebAppUm_AddLogMsg(wchar_t MsgEn[STKWEBAPPUM_MAXLEN_OF_LOGMSG], wchar_t MsgJa[STKWEBAPPUM_MAXLEN_OF_LOGMSG], int UserId)
{
	return DataAccessUm::GetInstance()->AddLogMsg(MsgEn, MsgJa, UserId);
}

void StkWebAppUm_SetPropertyValueInt(const wchar_t* Name, const int Value)
{
	DataAccessUm::GetInstance()->SetPropertyValueInt(Name, Value);
}

void StkWebAppUm_SetPropertyValueWStr(const wchar_t* Name, const wchar_t Value[STKWEBAPPUM_MAXLEN_OF_PROPERTY_VALUEWSTR])
{
	DataAccessUm::GetInstance()->SetPropertyValueWStr(Name, Value);
}

int StkWebAppUm_GetPropertyValueInt(const wchar_t* Name)
{
	return DataAccessUm::GetInstance()->GetPropertyValueInt(Name);
}

void StkWebAppUm_GetPropertyValueWStr(const wchar_t* Name, wchar_t Value[STKWEBAPPUM_MAXLEN_OF_PROPERTY_VALUEWSTR])
{
	DataAccessUm::GetInstance()->GetPropertyValueWStr(Name, Value);
}

int StkWebAppUm_GetAllPropertyData(wchar_t Name[STKWEBAPPUM_MAXNUM_OF_PROPERTY_RECORDS][STKWEBAPPUM_MAXLEN_OF_PROPERTY_NAME],
	int ValInt[STKWEBAPPUM_MAXNUM_OF_PROPERTY_RECORDS],
	wchar_t ValWStr[STKWEBAPPUM_MAXNUM_OF_PROPERTY_RECORDS][STKWEBAPPUM_MAXLEN_OF_PROPERTY_VALUEWSTR])
{
	return DataAccessUm::GetInstance()->GetAllPropertyData(Name, ValInt, ValWStr);
}

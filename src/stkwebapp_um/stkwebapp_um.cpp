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
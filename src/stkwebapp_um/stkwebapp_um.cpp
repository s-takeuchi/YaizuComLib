#include "stkwebapp_um.h"
#include "UserManagement.h"

void StkWebAppUm_RegisterApi(StkWebApp* Swa)
{
	UserManagement::RegisterApi(Swa);
}

void StkWebAppUm_UnregisterApi(StkWebApp* Swa)
{
	UserManagement::UnregisterApi(Swa);
}

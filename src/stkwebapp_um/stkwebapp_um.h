#pragma once

#include "../stkwebapp/StkWebApp.h"

void StkWebAppUm_RegisterApi(StkWebApp*);
void StkWebAppUm_UnregisterApi(StkWebApp*);
int StkWebAppUm_CreateTable();
void StkWebAppUm_Init();

int StkWebAppUm_AddLogMsg(wchar_t*, wchar_t*);

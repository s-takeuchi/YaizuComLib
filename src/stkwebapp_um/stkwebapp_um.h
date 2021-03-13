#pragma once

#include "../stkwebapp/StkWebApp.h"

#define STKWEBAPPUM_MAXLEN_OF_LOGMSG 100
#define STKWEBAPPUM_MAXLEN_OF_PROPERTY_VALUEWSTR 256

void StkWebAppUm_RegisterApi(StkWebApp*);
void StkWebAppUm_UnregisterApi(StkWebApp*);
int StkWebAppUm_CreateTable();
void StkWebAppUm_Init();

int StkWebAppUm_AddLogMsg(wchar_t[STKWEBAPPUM_MAXLEN_OF_LOGMSG], wchar_t[STKWEBAPPUM_MAXLEN_OF_LOGMSG], int UserId = -1);

void StkWebAppUm_SetPropertyValueInt(const wchar_t*, const int);
void StkWebAppUm_SetPropertyValueWStr(const wchar_t*, const wchar_t[STKWEBAPPUM_MAXLEN_OF_PROPERTY_VALUEWSTR]);
int StkWebAppUm_GetPropertyValueInt(const wchar_t*);
void StkWebAppUm_GetPropertyValueWStr(const wchar_t*, wchar_t[STKWEBAPPUM_MAXLEN_OF_PROPERTY_VALUEWSTR]);

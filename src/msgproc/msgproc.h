#pragma once
#include <windows.h>
#include <tchar.h>

class MessageProc
{
private:
	MessageProc();
	virtual ~MessageProc();
	static MessageProc* Instance;
	class Impl;
	Impl* pImpl;

public:
	static const int UNKNOWNERROR = 2901;
	static const int FILEACCESSERROR = 2902;
	static const int NEEDADMINRIGHTS = 2903;
	static const int DATAUPDATED = 2904;
	static const int INVALIDVALUE = 2905;
	static const int AN_ERROR_OCCURRED = 2906;
	static const int COMMON_ADD = 2931;
	static const int COMMON_DELETE = 2932;
	static const int COMMON_EDIT = 2933;
	static const int COMMON_REFRESH = 2934;
	static const int COMMON_TRUE = 2935;
	static const int COMMON_FALSE = 2936;
	static const int COMMON_UNKNOWN = 2937;
	static const int COMMON_OK = 2941;
	static const int COMMON_CANCEL = 2942;

	static const int CMDFRK_CMDFRKSRV = 3100;
	static const int CMDFRK_CANNOT_START = 3101;
	static const int CMDFRK_ERROR_UNEXPECTED = 3121;
	static const int CMDFRK_ERROR_SERVICEDOWN = 3122;
	static const int CMDFRK_ERROR_JAVASCRIPTERR = 3123;
	static const int CMDFRK_ODBCINFO = 3124;
	static const int CMDFRK_ERROR_INVALIDDBCONN = 3125;
	static const int CMDFRK_LOG_DBMSCHANGE = 3126;
	static const int CMDFRK_FILTER_DESC = 3127;
	static const int CMDFRK_FILTER_INVALID = 3128;
	static const int CMDFRK_FILTERINGCOND_UPDATE = 3129;
	static const int CMDFRK_FILTERING_NOTABLE = 3130;
	static const int CMDFRK_NOTABLEINFO = 3131;
	static const int CMDFRK_EMPTYCONNSTR = 3132;
	static const int CMDFRK_WELCOME = 3133;
	static const int CMDFRK_ERROR_TOOMUCHRECS = 3134;
	static const int CMDFRK_ERROR_TOOLONGCONNSTR = 3135;
	static const int CMDFRK_ERROR_TOOLONGFILTER = 3136;
	static const int CMDFRK_SVCFAILEDTOSTART = 3142;
	static const int CMDFRK_SVCFAILEDTOSTOP = 3143;
	static const int CMDFRK_SVCALREADYSTART = 3144;
	static const int CMDFRK_SVCALREADYSTOP = 3145;
	static const int CMDFRK_SVCSTART = 3146;
	static const int CMDFRK_SVCSTOP = 3147;
	static const int CMDFRK_LOG_DESC = 3148;
	static const int CMDFRK_TABLEINFO = 3149;
	static const int CMDFRK_TARGETTABLE = 3150;
	static const int CMDFRK_COMPOINFO2 = 3154;

	static const int CMDFRK_DBMS = 3200;
	static const int CMDFRK_CONNSTR = 3201;
	static const int CMDFRK_MANUAL = 3202;
	static const int CMDFRK_MANUALMSG = 3203;
	static const int CMDFRK_COMPOINFO = 3210;
	static const int CMDFRK_LOGGINGTIME = 3226;
	static const int CMDFRK_LOGGINGMSG = 3227;
	static const int CMDFRK_UNSPEC = 3233;
	static const int CMDFRK_FILTERING = 3234;
	static const int CMDFRK_ODBCCONNECTIONS = 3235;
	static const int CMDFRK_TABLES = 3236;
	static const int CMDFRK_CREATERECORD = 3237;
	static const int CMDFRK_EDITRECORD = 3238;
	static const int CMDFRK_DELETERECORD = 3239;
	static const int CMDFRK_INFORMATION = 3240;
	static const int CMDFRK_REFRESHTABLELIST = 3241;


	static const int LOCALE_MODE_WIN32 = 0;
	static const int LOCALE_MODE_WEB = 1;
	static const int LOCALE_MODE_ENGLISH = 10;
	static const int LOCALE_MODE_JAPANESE = 11;

	static const int MLANG_ENGLISH = 0;
	static const int MLANG_JAPANESE = 1;

	static void SetLocaleMode(int);
	static TCHAR* GetMsg(int);
	static TCHAR* GetMsgEng(int);
	static TCHAR* GetMsgJpn(int);
	static BYTE* GetMsgSjis(int);
	static BYTE* GetMsgSjisEng(int);
	static BYTE* GetMsgSjisJpn(int);
	static void StkErr(int, HWND);
	static void StkErr(int, TCHAR*, HWND);
	static void StkInf(int, HWND);
	static void StkInf(int, TCHAR*, HWND);
	static int StkYesNo(int, HWND);
	static int StkYesNo(int, TCHAR*, HWND);

	static void AddEng(int, TCHAR*);
	static void AddJpn(int, TCHAR*);

};

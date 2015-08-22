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
	static const int LBA_FIRSTINFO = 1101;
	static const int LBA_PROCCALLFAILED = 1102;
	static const int LBA_ALREADYINSTALLED = 1103;
	static const int LBA_NOTINSTALLED = 1104;
	static const int LBA_OPECOMPLETED = 1105;

	static const int VAR_SELECTEDNOTONE = 2201;
	static const int VAR_AREYOUSUREDELETE = 2202;
	static const int VAR_NOITEMSELECTED = 2203;
	static const int VAR_NOTTRANSUTF8 = 2204;
	static const int VAR_INVALIDNAME = 2205;
	static const int VAR_ALREADYDELETED = 2206;
	static const int VAR_INVALIDTYPE = 2207;
	static const int VAR_BUFOVERFLOW = 2208;
	static const int VAR_MAXVARSIZE = 2209;
	static const int VAR_FLAGVARNOT = 2210;
	static const int VAR_FOLDERSETERR = 2211;
	static const int VAR_EXPERR = 2212;
	static const int VAR_IMPERR = 2213;
	static const int VAR_NOIMPTARGET = 2214;
	static const int VAR_NOEXPTARGET = 2215;

	static const int PROP_NAME = 2300;
	static const int PROP_CHKDAT_START = 2301;
	static const int PROP_CHKDAT_NSTART = 2302;
	static const int PROP_CHKDAT_END = 2303;
	static const int PROP_CHKDAT_NEND = 2304;
	static const int PROP_CHKDAT_CONT = 2305;
	static const int PROP_CHKDAT_NCONT = 2306;
	static const int PROP_CHKDAT_EQUAL = 2307;
	static const int PROP_CHKDAT_NEQUAL = 2308;
	static const int PROP_CHGDAT_INSERT = 2311;
	static const int PROP_CHGDAT_JOIN = 2312;
	static const int PROP_CHGDAT_REPLACE = 2313;
	static const int PROP_CHGDAT_EXTRACT = 2314;
	static const int PROP_CHGDAT_DROP = 2315;
	static const int PROP_CHGDAT_RESET = 2316;
	static const int PROP_CLOSEPORT = 2320;
	static const int PROP_TIMER_PAST = 2321;
	static const int PROP_TIMER_WAIT = 2322;
	static const int PROP_TIMER_SEC = 2323;
	static const int PROP_MAP_SEARCH = 2324;
	static const int PROP_MAP_REPLACE = 2325;
	static const int PROP_MAP_START = 2326;
	static const int PROP_MAP_USEONCE = 2327;
	static const int PROP_CHECK_TRUE = 2331;
	static const int PROP_CHECK_FALSE = 2332;
	static const int PROP_CHANGE_TRUE = 2333;
	static const int PROP_CHANGE_FALSE = 2334;
	static const int PROP_LOAD_VAR = 2341;
	static const int PROP_LOAD_CONT = 2342;
	static const int PROP_STORE_VAR = 2343;
	static const int PROP_STORE_CONT = 2344;
	static const int PROP_DATA_COMM = 2345;
	static const int PROP_DATA_PREF = 2346;
	static const int PROP_DATA_NUM = 2347;
	static const int PROP_NET_RECV = 2351;
	static const int PROP_NET_RECVTGT = 2352;
	static const int PROP_NET_SEND = 2353;
	static const int PROP_NET_SENDTGT = 2354;
	static const int PROP_NET_CLOSE_AFTERSEND = 2355;
	static const int PROP_NET_CLOSE_AFTERRECV = 2356;
	static const int PROP_NET_TIMEOUT = 2357;
	static const int PROP_NET_SENDER = 2358;
	static const int PROP_NET_RECEIVER = 2359;
	static const int PROP_NET_ICON = 2360;
	static const int PROP_NET_IPADDR = 2361;
	static const int PROP_NET_PORT = 2362;
	static const int PROP_FILEACCESS_NAME = 2363;
	static const int PROP_FILEACCESSW_ADD = 2364;
	static const int PROP_FILEACCESSW_INSERT = 2365;
	static const int PROP_FILEACCESSW_OVERWRITE = 2366;
	static const int PROP_FILEACCESSR_ADD = 2367;
	static const int PROP_FILEACCESSR_INSERT = 2368;
	static const int PROP_FILEACCESSR_OVERWRITE = 2369;
	static const int PROP_EXECPROG_COMMAND = 2370;
	static const int PROP_EXECPROG_WAIT = 2371;
	static const int PROP_EXECPROG_CURRDIR = 2372;
	static const int PROP_EXECPROG_STDOUT = 2373;
	static const int PROP_EXECPROG_STDIN = 2374;
	static const int PROP_NET_UNCOND = 2375;
	static const int PROP_NET_EXCEEDSIZE = 2376;
	static const int PROP_NET_RECVSTR = 2377;
	static const int PROP_NET_TERMCOND = 2378;
	static const int PROP_NET_CLOSEDETECT = 2379;
	static const int PROP_NET_CLOSEFORCE = 2380;
	static const int PROP_NET_CLOSEUDPPORT = 2381;
	static const int PROP_NET_CLOSETCPPORT = 2382;
	static const int PROP_NET_PROCEEDEVENIFNODATARECV = 2383;
	static const int PROP_NET_RECVMULTI = 2384;

	static const int STKFW_LOG_TITLE = 2400;
	static const int STKFW_LOG_START = 2401;
	static const int STKFW_LOG_STOP = 2402;
	static const int STKFW_LOG_SUCCESSCSC = 2403;
	static const int STKFW_LOG_SUCCESSCSBNLS = 2404;
	static const int STKFW_LOG_SOCKCLOSE = 2405;
	static const int STKFW_LOG_ACPTRECV = 2406;
	static const int STKFW_LOG_CNCTRECV = 2407;
	static const int STKFW_LOG_ACPTSEND = 2408;
	static const int STKFW_LOG_CNCTSEND = 2409;
	static const int STKFW_LOG_SENDERROR = 2410;
	static const int STKFW_LOG_RECVERROR = 2411;
	static const int STKFW_LOG_NAMESOLVEERR = 2412;
	static const int STKFW_LOG_CONNERROR = 2413;
	static const int STKFW_LOG_BINDLISTENERR = 2414;
	static const int STKFW_LOG_CREATEACCEPTSOCK = 2415;
	static const int STKFW_LOG_CLOSEACCEPTSOCK = 2416;
	static const int STKFW_LOG_CLOSELISTENACCEPTSOCK = 2417;
	static const int STKFW_LOG_BINDERR = 2418;
	static const int STKFW_LOG_SUCCESSCSBN = 2419;
	static const int STKFW_LOG_UDPRECV = 2420;
	static const int STKFW_LOG_UDPSEND = 2421;
	static const int STKFW_LOG_SUCCESSCS = 2422;
	static const int STKFW_LOG_UDPSOCKCLOSE = 2423;

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

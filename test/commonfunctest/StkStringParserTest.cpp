#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\commonfunc\StkStringParser.h"

int StkStringParserTest()
{
	printf("StkStringParserTest started\r\n");

	{
		printf("[StkStringParser] normal: two params ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789");
		TCHAR Format[100] = _T("abcdefg#+-*/#6789");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("012345")) != 0 || lstrcmp(OutStr2, _T("xyz")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: four params ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!");
		TCHAR Format[100] = _T("abcdefg#+-*/#6789#@@@#!!!!");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("012345")) != 0 ||
			lstrcmp(OutStr2, _T("xyz")) != 0 ||
			lstrcmp(OutStr3, _T("QWERTYUIO")) != 0 ||
			lstrcmp(OutStr4, _T("****")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Appear the target in the 1st and last char ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!");
		TCHAR Format[100] = _T("%012345+-*/%6789QWERTYUIO@@@****%");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('%'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("abcdefg")) != 0 || lstrcmp(OutStr2, _T("xyz")) != 0 || lstrcmp(OutStr3, _T("!!!!")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Appear the target in the 1st character ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!");
		TCHAR Format[100] = _T("#012345+-*/xyz6789QWERTYUIO@@@****!!!!");
		TCHAR OutStr1[256];
		StkStringParser::ParseInto1Param(Target, Format, _T('#'), OutStr1);
		if (lstrcmp(OutStr1, _T("abcdefg")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Appear the target in the last character ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!");
		TCHAR Format[100] = _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****#");
		TCHAR OutStr1[256];
		StkStringParser::ParseInto1Param(Target, Format, _T('#'), OutStr1);
		if (lstrcmp(OutStr1, _T("!!!!")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Specify trimed string ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!");
		TCHAR Format[100] = _T("efg0123&ERTYUIO@@@**");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('&'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("45+-*/xyz6789QW")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Four 1byte chars fetching ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!");
		TCHAR Format[100] = _T("#bcdefg#12345+-*/xyz6789QWERTYUIO@@@#***!!!#");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("a")) != 0 || lstrcmp(OutStr2, _T("0")) != 0 ||
			lstrcmp(OutStr3, _T("*")) != 0 || lstrcmp(OutStr4, _T("!")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Short string (1) ...");
		TCHAR Target[100] = _T("ab");
		TCHAR Format[100] = _T("#b");
		TCHAR OutStr1[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, NULL, NULL, NULL);
		if (lstrcmp(OutStr1, _T("a")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Short string (2) ...");
		TCHAR Target[100] = _T("ab");
		TCHAR Format[100] = _T("a#");
		TCHAR OutStr1[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, NULL, NULL, NULL);
		if (lstrcmp(OutStr1, _T("b")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Short string (3) ...");
		TCHAR Target[100] = _T("a");
		TCHAR Format[100] = _T("#");
		TCHAR OutStr1[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, NULL, NULL, NULL);
		if (lstrcmp(OutStr1, _T("a")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: All Fetch ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!");
		TCHAR Format[100] = _T("#");
		TCHAR OutStr1[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, NULL, NULL, NULL);
		if (lstrcmp(OutStr1, _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: For multi bytes characters ...");
		TCHAR Target[100] = _T("źěkéłŁéąé˛ĂĄźVájÔÂPXęătÄH~");
		TCHAR Format[100] = _T("%éłŁéąé˛%Váj%PXęă%");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('%'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("źěk")) != 0 || lstrcmp(OutStr2, _T("ĂĄź")) != 0 ||
			lstrcmp(OutStr3, _T("ÔÂ")) != 0 || lstrcmp(OutStr4, _T("tÄH~")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Fetching empty ...");
		TCHAR Target[100] = _T("źěkéłŁéąé˛ĂĄźVájÔÂPXęătÄH~");
		TCHAR Format[100] = _T("źVá%jÔÂ%PX");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		StkStringParser::ParseInto2Params(Target, Format, _T('%'), OutStr1, OutStr2);
		if (lstrcmp(OutStr1, _T("")) != 0 || lstrcmp(OutStr2, _T("")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Specify all NULLs ...");
		TCHAR Target[100] = _T("źěkéłŁéąé˛ĂĄźVájÔÂPXęătÄH~");
		TCHAR Format[100] = _T("%éłŁéąé˛%Váj%PXęă%");
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('%'), NULL, NULL, NULL, NULL);
		if (Ret != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Sequential double targets are specified ...");
		TCHAR Target[100] = _T("źěkéłŁéąé˛ĂĄźVájÔÂPXęătÄH~");
		TCHAR Format[100] = _T("éłŁéąé˛%%Váj");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('%'), OutStr1, OutStr2, NULL, NULL);
		if (lstrcmp(OutStr1, _T("")) != 0 || lstrcmp(OutStr2, _T("")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Sequential tripple targets are specified ...");
		TCHAR Target[100] = _T("źěkéłŁéąé˛ĂĄźVájÔÂPXęătÄH~");
		TCHAR Format[100] = _T("éłŁéąé˛%%%Váj");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('%'), OutStr1, OutStr2, OutStr3, NULL);
		if (lstrcmp(OutStr1, _T("")) != 0 || lstrcmp(OutStr2, _T("")) != 00 || lstrcmp(OutStr3, _T("")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Sequential tripple targets are specified ...");
		TCHAR Target[100] = _T("źěkéłŁéąé˛ĂĄźVájÔÂPXęătÄH~");
		TCHAR Format[100] = _T("źěkéłŁéąé˛ĂĄźVájÔÂPXęătÄH~");
		TCHAR OutStr1[256];
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('%'), OutStr1, NULL, NULL, NULL);
		if (lstrcmp(OutStr1, _T("")) != 0 || Ret != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Size exceeded target ...");
		TCHAR Target[100] = _T("źěkéłŁéąé˛ĂĄźVájÔÂPXęătÄH~");
		TCHAR Format[100] = _T("źěkéłŁéąé˛ĂĄźVáj%ÔÂPXęătÄH~");
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('%'), NULL, NULL, NULL, NULL);
		if (Ret != -1) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: NULL string is presented ...");
		int Ret = StkStringParser::ParseInto4Params(NULL, NULL, _T('%'), NULL, NULL, NULL, NULL);
		if (Ret != -1) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}

	printf("StkStringParserTest completed\r\n\r\n");
	return 0;
}

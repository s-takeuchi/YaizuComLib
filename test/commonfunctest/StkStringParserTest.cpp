#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\commonfunc\StkStringParser.h"

int StkStringParserTest()
{
	printf("StkStringParserTest starts\r\n");
	{
		printf("[StkStringParser] normal: two params ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789");
		TCHAR Format[100] = _T("abcdefg#+-*/#6789");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("012345")) != 0 && lstrcmp(OutStr2, _T("xyz")) != 0) {
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
		if (lstrcmp(OutStr1, _T("012345")) != 0 &&
			lstrcmp(OutStr2, _T("xyz")) != 0 &&
			lstrcmp(OutStr3, _T("QWERTYUIO")) != 0 &&
			lstrcmp(OutStr4, _T("****")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Appear the target in the 1st and last characters ...");
		TCHAR Target[100] = _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!");
		TCHAR Format[100] = _T("%012345+-*/%6789QWERTYUIO@@@****%");
		TCHAR OutStr1[256];
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('%'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("abcdefg")) != 0 && lstrcmp(OutStr2, _T("xyz")) != 0 && lstrcmp(OutStr3, _T("!!!!")) != 0) {
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
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, OutStr2, OutStr3, OutStr4);
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
		TCHAR OutStr2[256];
		TCHAR OutStr3[256];
		TCHAR OutStr4[256];
		StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("!!!!")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	printf("StkStringParserTest ends\r\n\r\n");
	return 0;
}

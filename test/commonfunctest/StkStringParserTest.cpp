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
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("012345")) != 0 ||
			lstrcmp(OutStr2, _T("xyz")) != 0 ||
			lstrcmp(OutStr3, _T("QWERTYUIO")) != 0 ||
			lstrcmp(OutStr4, _T("****")) != 0 ||
			Ret != 1) {
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
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('&'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("")) != 0 || lstrcmp(OutStr2, _T("")) != 0 || Ret != 0) {
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
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, NULL, NULL, NULL);
		if (lstrcmp(OutStr1, _T("a")) != 0 || Ret != 1) {
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
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('#'), OutStr1, NULL, NULL, NULL);
		if (lstrcmp(OutStr1, _T("abcdefg012345+-*/xyz6789QWERTYUIO@@@****!!!!")) != 0 || Ret != 1) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: For multi bytes characters ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("%È≥ñ£È±È≤%òVé·íjèó%ÇPÇXàÍã„%");
		TCHAR OutStr1[32];
		TCHAR OutStr2[32];
		TCHAR OutStr3[32];
		TCHAR OutStr4[32];
		StkStringParser::ParseInto4Params(Target, Format, _T('%'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (lstrcmp(OutStr1, _T("ìåêºìÏñk")) != 0 || lstrcmp(OutStr2, _T("å√ç°ìåêº")) != 0 ||
			lstrcmp(OutStr3, _T("ê‘ê¬îíçï")) != 0 || lstrcmp(OutStr4, _T("ètâƒèHì~")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Same string without targets is presented ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('%'), NULL, NULL, NULL, NULL);
		if (Ret != 1) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Partial string without targets is presented ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("È≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„");
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('%'), NULL, NULL, NULL, NULL);
		if (Ret != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] normal: Large data processing ...");
		TCHAR Target[100000] = _T("");
		TCHAR Format[100000] = _T("");
		TCHAR TargetPart1[20000];
		TCHAR TargetPart2[20000];
		TCHAR TargetPart3[20000];
		TCHAR TargetPart4[20000];
		TCHAR TargetPart5[20000];
		TCHAR OutStr1[20001];
		TCHAR OutStr2[20001];
		TCHAR OutStr3[20001];
		TCHAR Abc[20] = {_T('a'), _T('b'), _T('c'), _T('d'), _T('e'), 
						_T('1'), _T('2'), _T('3'), _T('4'), _T('5'), 
						_T('ïó'), _T('ó—'), _T('âŒ'), _T('éR'), _T('x'), 
						_T('y'), _T('È≥'), _T('ñ£'), _T('È±'), _T('È≤')
		};

		for (int Loop = 0; Loop < 19999; Loop++) {
			TargetPart1[Loop] = Abc[rand() % 20];
			TargetPart2[Loop] = Abc[rand() % 20];
			TargetPart3[Loop] = Abc[rand() % 20];
			TargetPart4[Loop] = Abc[rand() % 20];
			TargetPart5[Loop] = Abc[rand() % 20];
		}
		TargetPart1[19999] = _T('\0');
		TargetPart2[19999] = _T('\0');
		TargetPart3[19999] = _T('\0');
		TargetPart4[19999] = _T('\0');
		TargetPart5[19999] = _T('\0');

		lstrcat(Target, TargetPart1);
		lstrcat(Target, TargetPart2);
		lstrcat(Target, TargetPart3);
		lstrcat(Target, TargetPart4);
		lstrcat(Target, TargetPart5);

		lstrcat(Format, _T("!"));
		lstrcat(Format, TargetPart2);
		lstrcat(Format, _T("!"));
		lstrcat(Format, TargetPart4);
		lstrcat(Format, _T("!"));

		int Ret = StkStringParser::ParseInto3Params(Target, Format, _T('!'), OutStr1, OutStr2, OutStr3);
		if (Ret != 1 || lstrcmp(OutStr1, TargetPart1) != 0 || lstrcmp(OutStr2, TargetPart3) != 0 || lstrcmp(OutStr3, TargetPart5) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Fetching empty ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("ìåêºòVé·%íjèóê‘ê¬%îíçïÇPÇX");
		TCHAR OutStr1[64];
		TCHAR OutStr2[64];
		StkStringParser::ParseInto2Params(Target, Format, _T('%'), OutStr1, OutStr2);
		if (lstrcmp(OutStr1, _T("")) != 0 || lstrcmp(OutStr2, _T("")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Unmatch scenario (1) ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("ìåêºìÏñk%å√ç°ìåêº%ê‘ê¬îíçï%èƒì˜íËêH");
		TCHAR OutStr1[16];
		TCHAR OutStr2[16];
		TCHAR OutStr3[16];
		int Ret = StkStringParser::ParseInto3Params(Target, Format, _T('%'), OutStr1, OutStr2, OutStr3);
		if (lstrcmp(OutStr1, _T("È≥ñ£È±È≤")) != 0 || lstrcmp(OutStr2, _T("òVé·íjèó")) != 0 || OutStr3[0] != '\0' || Ret != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Unmatch scenario (2) ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("abc$xyz");
		TCHAR OutStr1[16];
		int Ret = StkStringParser::ParseInto1Param(Target, Format, _T('$'), OutStr1);
		if (OutStr1[0] != '\0' || Ret != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Unmatch scenario (3) ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("Ç†Ç¢Ç§Ç¶Ç®Ç©Ç´Ç≠ÇØÇ±Ç≥ÇµÇ∑ÇπÇªÇΩÇøÇ¬ÇƒÇ∆");
		TCHAR OutStr1[16];
		TCHAR OutStr2[16];
		TCHAR OutStr3[16];
		TCHAR OutStr4[16];
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('$'), OutStr1, OutStr2, OutStr3, OutStr4);
		if (OutStr1[0] != '\0' || OutStr2[0] != '\0' || OutStr3[0] != '\0' || OutStr4[0] != '\0' || Ret != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Specify all NULLs ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("%È≥ñ£È±È≤%òVé·íjèó%ÇPÇXàÍã„%");
		int Ret = StkStringParser::ParseInto4Params(Target, Format, _T('%'), NULL, NULL, NULL, NULL);
		if (Ret != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Sequential double targets are specified ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("È≥ñ£È±È≤%%òVé·íjèó");
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
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("È≥ñ£È±È≤%%%òVé·íjèó");
		TCHAR OutStr1[64];
		TCHAR OutStr2[64];
		TCHAR OutStr3[64];
		StkStringParser::ParseInto4Params(Target, Format, _T('%'), OutStr1, OutStr2, OutStr3, NULL);
		if (lstrcmp(OutStr1, _T("")) != 0 || lstrcmp(OutStr2, _T("")) != 00 || lstrcmp(OutStr3, _T("")) != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
	{
		printf("[StkStringParser] abnormal: Size exceeded target ...");
		TCHAR Target[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèóê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
		TCHAR Format[100] = _T("ìåêºìÏñkÈ≥ñ£È±È≤å√ç°ìåêºòVé·íjèó%ê‘ê¬îíçïÇPÇXàÍã„ètâƒèHì~");
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

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\commonfunc\StkGeneric.h"

void StkPropertiesTest();
void MsgProcTest();
void StkObjectTest();

int main(int argc, char *argv[])
{
	StkObjectTest();
	StkPropertiesTest();
	MsgProcTest();

	////////////////////////////////////////////////////////////////////////
	printf("Test started.\n");

	StkGeneric* StkGen = StkGeneric::GetInstance();

	{
		TCHAR Html1[50] = _T("abcdefghijklmnopqrstuvwxyz");
		printf("HtmlEncodeSize#1[%S]...", Html1);
		if (StkGen->HtmlEncodeSize(Html1) == 26) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	{
		TCHAR Html2[50] = _T("abcde<>hijklmn&pqrsuvwxy\"z");
		printf("HtmlEncodeSize#2[%S]...", Html2);
		int Len2 = StkGen->HtmlEncodeSize(Html2);
		if (Len2 == 41) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		printf("HtmlEncode#1[%S]...", Html2);
		TCHAR* NewHtml2 = new TCHAR[Len2 + 1];
		StkGen->HtmlEncode(Html2, NewHtml2, Len2 + 1);
		if (lstrcmp(_T("abcde&lt;&gt;hijklmn&amp;pqrsuvwxy&quot;z"), NewHtml2) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml2;
	}

	{
		TCHAR Html3[50] = _T("<>&\"");
		printf("HtmlEncode#2[%S]...", Html3);
		int Len3 = StkGen->HtmlEncodeSize(Html3);
		TCHAR* NewHtml3 = new TCHAR[Len3 + 1];
		StkGen->HtmlEncode(Html3, NewHtml3, Len3 + 1);
		if (lstrcmp(_T("&lt;&gt;&amp;&quot;"), NewHtml3) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml3;
	}

	{
		TCHAR Html4[10] = _T("abcdefghi");
		printf("HtmlEncode#3[%S]...", Html4);
		TCHAR* NewHtml4 = new TCHAR[10];
		StkGen->HtmlEncode(Html4, NewHtml4, 10);
		if (lstrcmp(_T("abcdefghi"), NewHtml4) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml4;
	}

	{
		TCHAR Html5[10] = _T("<>&\"");
		printf("HtmlEncode#4[%S;size=14]...", Html5);
		TCHAR* NewHtml5 = new TCHAR[14];
		StkGen->HtmlEncode(Html5, NewHtml5, 14);
		if (lstrcmp(_T("&lt;&gt;&amp;"), NewHtml5) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml5;
	}

	{
		TCHAR Html[10] = _T("<>&\"");
		printf("HtmlEncode#5[%S;size=13]...", Html);
		TCHAR* NewHtml = new TCHAR[14];
		StkGen->HtmlEncode(Html, NewHtml, 13);
		if (lstrcmp(_T("&lt;&gt;"), NewHtml) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml;
	}

	{
		TCHAR Html[10] = _T("<>&\"");
		printf("HtmlEncode#6[%S;size=20]...", Html);
		TCHAR* NewHtml = new TCHAR[20];
		StkGen->HtmlEncode(Html, NewHtml, 20);
		if (lstrcmp(_T("&lt;&gt;&amp;&quot;"), NewHtml) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml;
	}

	////////////////////////////////////////////////////////////////////////
	{
		TCHAR OrgPath[MAX_PATH] = _T("aaa\\bbb\\ccc\\ddd\\eee\\fff\\ggg\\xxx.txt");
		TCHAR CmpPath[MAX_PATH] = _T("aaa\\bbb\\ccc\\ddd\\eee\\fff\\ggg");
		TCHAR AcqPath[MAX_PATH];
		wprintf(_T("GetFullPathWithoutFileName#1[%s]..."), OrgPath);
		StkGen->GetFullPathWithoutFileName(OrgPath, AcqPath);
		if (lstrcmp(AcqPath, CmpPath) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	{
		TCHAR OrgPath[MAX_PATH] = _T("aaa\\bbb\\ccc\\ddd\\eee\\fff\\ggg\\");
		TCHAR CmpPath[MAX_PATH] = _T("aaa\\bbb\\ccc\\ddd\\eee\\fff\\ggg");
		TCHAR AcqPath[MAX_PATH];
		wprintf(_T("GetFullPathWithoutFileName#2[%s]..."), OrgPath);
		StkGen->GetFullPathWithoutFileName(OrgPath, AcqPath);
		if (lstrcmp(AcqPath, CmpPath) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	{
		TCHAR OrgPath[MAX_PATH] = _T("aaabbbcccdddeeefffggg");
		TCHAR AcqPath[MAX_PATH];
		wprintf(_T("GetFullPathWithoutFileName#3[%s]..."), OrgPath);
		if (StkGen->GetFullPathWithoutFileName(OrgPath, AcqPath) == -1) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	{
		TCHAR AcqPath[MAX_PATH];
		wprintf(_T("GetFullPathWithoutFileName#4[NULL]..."));
		if (StkGen->GetFullPathWithoutFileName(NULL, AcqPath) == -1) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		wprintf(_T("GetFullPathWithoutFileName#4[EMPTY]..."));
		if (StkGen->GetFullPathWithoutFileName(_T(""), AcqPath) == -1) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	printf("Test completed.\n");

	return 0;
}

//#include <windows.h>
//#include <stdio.h>
//#include "..\..\src\commonfunc\StkGeneric.h"

void StkPropertiesTest();
void MsgProcTest();
void StkObjectTest();
int StkStringParserTest();

int main(int argc, char *argv[])
{
	StkStringParserTest();
	StkObjectTest();
	StkPropertiesTest();
	MsgProcTest();
/*
	////////////////////////////////////////////////////////////////////////
	printf("Test started.\n");

	StkGeneric* StkGen = StkGeneric::GetInstance();

	{
		wchar_t Html1[50] = L"abcdefghijklmnopqrstuvwxyz";
		printf("HtmlEncodeSize#1[%S]...", Html1);
		if (StkGen->HtmlEncodeSize(Html1) == 26) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	{
		wchar_t Html2[50] = L"abcde<>hijklmn&pqrsuvwxy\"z";
		printf("HtmlEncodeSize#2[%S]...", Html2);
		int Len2 = StkGen->HtmlEncodeSize(Html2);
		if (Len2 == 41) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		printf("HtmlEncode#1[%S]...", Html2);
		wchar_t* NewHtml2 = new wchar_t[Len2 + 1];
		StkGen->HtmlEncode(Html2, NewHtml2, Len2 + 1);
		if (lstrcmp(L"abcde&lt;&gt;hijklmn&amp;pqrsuvwxy&quot;z", NewHtml2) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml2;
	}

	{
		wchar_t Html3[50] = L"<>&\"";
		printf("HtmlEncode#2[%S]...", Html3);
		int Len3 = StkGen->HtmlEncodeSize(Html3);
		wchar_t* NewHtml3 = new wchar_t[Len3 + 1];
		StkGen->HtmlEncode(Html3, NewHtml3, Len3 + 1);
		if (lstrcmp(L"&lt;&gt;&amp;&quot;", NewHtml3) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml3;
	}

	{
		wchar_t Html4[10] = L"abcdefghi";
		printf("HtmlEncode#3[%S]...", Html4);
		wchar_t* NewHtml4 = new wchar_t[10];
		StkGen->HtmlEncode(Html4, NewHtml4, 10);
		if (lstrcmp(L"abcdefghi", NewHtml4) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml4;
	}

	{
		wchar_t Html5[10] = L"<>&\"";
		printf("HtmlEncode#4[%S;size=14]...", Html5);
		wchar_t* NewHtml5 = new wchar_t[14];
		StkGen->HtmlEncode(Html5, NewHtml5, 14);
		if (lstrcmp(L"&lt;&gt;&amp;", NewHtml5) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml5;
	}

	{
		wchar_t Html[10] = L"<>&\"";
		printf("HtmlEncode#5[%S;size=13]...", Html);
		wchar_t* NewHtml = new wchar_t[14];
		StkGen->HtmlEncode(Html, NewHtml, 13);
		if (lstrcmp(L"&lt;&gt;", NewHtml) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml;
	}

	{
		wchar_t Html[10] = L"<>&\"";
		printf("HtmlEncode#6[%S;size=20]...", Html);
		wchar_t* NewHtml = new wchar_t[20];
		StkGen->HtmlEncode(Html, NewHtml, 20);
		if (lstrcmp(L"&lt;&gt;&amp;&quot;", NewHtml) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		delete NewHtml;
	}

	////////////////////////////////////////////////////////////////////////
	{
		wchar_t OrgPath[MAX_PATH] = L"aaa\\bbb\\ccc\\ddd\\eee\\fff\\ggg\\xxx.txt";
		wchar_t CmpPath[MAX_PATH] = L"aaa\\bbb\\ccc\\ddd\\eee\\fff\\ggg";
		wchar_t AcqPath[MAX_PATH];
		wprintf(L"GetFullPathWithoutFileName#1[%s]...", OrgPath);
		StkGen->GetFullPathWithoutFileName(OrgPath, AcqPath);
		if (lstrcmp(AcqPath, CmpPath) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	{
		wchar_t OrgPath[MAX_PATH] = L"aaa\\bbb\\ccc\\ddd\\eee\\fff\\ggg\\";
		wchar_t CmpPath[MAX_PATH] = L"aaa\\bbb\\ccc\\ddd\\eee\\fff\\ggg";
		wchar_t AcqPath[MAX_PATH];
		wprintf(L"GetFullPathWithoutFileName#2[%s]...", OrgPath);
		StkGen->GetFullPathWithoutFileName(OrgPath, AcqPath);
		if (lstrcmp(AcqPath, CmpPath) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	{
		wchar_t OrgPath[MAX_PATH] = L"aaabbbcccdddeeefffggg";
		wchar_t AcqPath[MAX_PATH];
		wprintf(L"GetFullPathWithoutFileName#3[%s]...", OrgPath);
		if (StkGen->GetFullPathWithoutFileName(OrgPath, AcqPath) == -1) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	{
		wchar_t AcqPath[MAX_PATH];
		wprintf(L"GetFullPathWithoutFileName#4[NULL]...");
		if (StkGen->GetFullPathWithoutFileName(NULL, AcqPath) == -1) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
		wprintf(L"GetFullPathWithoutFileName#4[EMPTY]...");
		if (StkGen->GetFullPathWithoutFileName(L"", AcqPath) == -1) {
			printf("OK\n");
		} else {
			printf("NG\n");
			exit(0);
		}
	}

	printf("Test completed.\n");
*/
	return 0;
}

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\com\commonfunc\StkGeneric.h"

int main(int argc, char *argv[])
{
	StkGeneric* StkGen = StkGeneric::GetInstance();
	int Ret;

	printf("Test started.\n");

	printf("Program is loading property file \"propertytest1.prop\" (zero byte data) ...");
	Ret = StkGen->GetProperties(_T("propertytest1.prop"));
	if (Ret == 0) {
		printf("Success\n");
	} else {
		printf("Failure\n");
		exit(0);
	}

	printf("Program is loading property file \"propertytest2.prop\"...");
	Ret = StkGen->GetProperties(_T("propertytest2.prop"));
	if (Ret == 0) {
		printf("Success\n");
	} else {
		printf("Failure\n");
		exit(0);
	}

	printf("Program is loading property file \"propertytest3.prop\"...");
	Ret = StkGen->GetProperties(_T("propertytest3.prop"));
	if (Ret == 0) {
		printf("Success\n");
	} else {
		printf("Failure\n");
		exit(0);
	}

	printf("Program trys to load unexisting property file \"propertytest4.prop\"...");
	Ret = StkGen->GetProperties(_T("propertytest4.prop"));
	if (Ret == -1) {
		printf("Success\n");
	} else {
		printf("Failure\n");
		exit(0);
	}


	char ValueStr[256];
	int ValueInt;

	StkGen->GetPropertyInt("prop01", &ValueInt);
	printf("prop01=[123]?...");
	if (ValueInt == 123) {
		printf("OK\n");
	} else {
		printf("NG\n");
	}

	StkGen->GetPropertyStr("prop02", ValueStr);
	printf("prop02=[abc]?...");
	if (strcmp(ValueStr, "abc") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
	}

	StkGen->GetPropertyInt("prop03", &ValueInt);
	printf("prop03=[456]?...");
	if (ValueInt == 456) {
		printf("OK\n");
	} else {
		printf("NG\n");
	}

	StkGen->GetPropertyStr("prop04", ValueStr);
	printf("prop04=[def]?...");
	if (strcmp(ValueStr, "def") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
	}

	StkGen->GetPropertyInt("prop07", &ValueInt);
	printf("prop07=[-123]?...");
	if (ValueInt == -123) {
		printf("OK\n");
	} else {
		printf("NG\n");
	}

	StkGen->GetPropertyStr("myname", ValueStr);
	printf("myname=[Shinya Takeuchi]?...");
	if (strcmp(ValueStr, "Shinya Takeuchi") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
	}

	Ret = StkGen->GetPropertyStr("equal1", ValueStr);
	printf("equal1=[ppp=ppp]?...");
	if (Ret == 0 && strcmp(ValueStr, "ppp=ppp") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
	}

	Ret = StkGen->GetPropertyStr("equal2", ValueStr);
	printf("equal2=[ppp = ppp]?...");
	if (Ret == 0 && strcmp(ValueStr, "ppp = ppp") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
	}

	Ret = StkGen->GetPropertyStr("equal3", ValueStr);
	printf("equal3=unexisting?...");
	if (Ret == -1) {
		printf("OK\n");
	} else {
		printf("NG\n");
	}

	////////////////////////////////////////////////////////////////////////
	{
		TCHAR Html1[50] = _T("abcdefghijklmnopqrstuvwxyz");
		printf("HtmlEncodeSize#1[%S]...", Html1);
		if (StkGen->HtmlEncodeSize(Html1) == 26) {
			printf("OK\n");
		} else {
			printf("NG\n");
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
		}
		printf("HtmlEncode#1[%S]...", Html2);
		TCHAR* NewHtml2 = new TCHAR[Len2 + 1];
		StkGen->HtmlEncode(Html2, NewHtml2, Len2 + 1);
		if (lstrcmp(_T("abcde&lt;&gt;hijklmn&amp;pqrsuvwxy&quot;z"), NewHtml2) == 0) {
			printf("OK\n");
		} else {
			printf("NG\n");
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
		}
		delete NewHtml;
	}

	printf("Test completed.\n");

	return 0;
}

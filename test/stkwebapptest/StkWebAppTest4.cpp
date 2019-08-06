#include "../../src/stkpl/StkPl.h"
#include "../../src/commonfunc/StkStringParser.h"
#include "StkWebAppTest4.h"

StkObject* StkWebAppTest4::Execute(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t* HttpHeader)
{
	if (StkPlWcsStr(HttpHeader, L"GET") == 0 || StkPlWcsStr(HttpHeader, L"Content-Length:") == 0 || StkPlWcsStr(HttpHeader, L"Content-Type:") == 0) {
		*ResultCode = -1;
		return NULL;
	}
	wchar_t Buf[10];
	StkStringParser::ParseInto1Param(UrlPath, L"/aaa/$/", L'$', Buf, 10);
	*ResultCode = StkPlWcsToL(Buf);
	return NULL;
}

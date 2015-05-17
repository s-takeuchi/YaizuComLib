#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define BUFSIZE 500000

void WriteText(TCHAR* Txt, TCHAR* Type)
{
	int Charset = 0;
	if (lstrcmp(Type, _T("mysql")) == 0) {
		Charset = CP_OEMCP;
	} else if (lstrcmp(Type, _T("postgresql")) == 0) {
		Charset = CP_UTF8;
	} else {
		return;
	}
	int cSize = WideCharToMultiByte(Charset, 0, Txt, -1, NULL, 0, NULL, NULL);
	if (cSize > 0) {
		BYTE* cTxt = new BYTE[cSize + 1];
		cSize = WideCharToMultiByte(Charset, 0, Txt, -1, (LPSTR)cTxt, cSize, NULL, NULL);
		cTxt[cSize] = '\0';
		printf_s("%s", cTxt);
		delete cTxt;
	}
}

void GenerateProductName(TCHAR* Name)
{
	TCHAR Head1[8][80] = {
		_T("豪快で"), _T("美しく"), _T("愉快で"), _T("軽快で"), _T("優れて"), _T("高速で"), _T("優雅で"), _T("快適で")
	};
	TCHAR Head2[5][80] = {
		_T("きらびやかな"), _T("抜け目のない"), _T("とても変質的な"), _T("馬鹿げた"), _T("やるせない")
	};
	TCHAR Country[5][80] = {
		_T("大日本帝国製"), _T("アメリカ合衆国産"), _T("大英帝国オリジナル"), _T("ゲルマン帝国製"), _T("フランス美女手作り")
	};
	TCHAR Engine[5][80] = {
		_T("Ｖ型６気筒３０００ＣＣ―"), _T("直列４気筒１５００ＣＣ―"), _T("リチウムイオン電動―"), _T("直列４気筒１０００ＣＣハイブリッド―"), _T("水平対向２０００ＣＣターボ―")
	};
	TCHAR Type[5][80] = {
		_T("クーペ型"), _T("オープン型"), _T("セダン型"), _T("ボックス型"), _T("ＳＵＶ型")
	};
	TCHAR Person[5][80] = {
		_T("二人乗り"), _T("４人乗り"), _T("５人乗り"), _T("８人乗り"), _T("６人乗り")
	};
	lstrcat(Name, Head1[rand() % 8]);
	lstrcat(Name, Head2[rand() % 5]);
	lstrcat(Name, Country[rand() % 5]);
	lstrcat(Name, Engine[rand() % 5]);
	lstrcat(Name, Type[rand() % 5]);
	lstrcat(Name, Person[rand() % 5]);
	lstrcat(Name, _T("自動車"));
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("Usage: %s <record count> <type of target dbms>\r\n", argv[0]);
		printf("type = mysql | postgresql\r\n");
		return -1;
	}
	TCHAR Type[100];
	_snwprintf_s(Type, 100, _TRUNCATE, _T("%S"), argv[2]);
	int MaxCnt = atoi(argv[1]);
	for (int Lo = 1; Lo <= MaxCnt; Lo++) {
		for (int Loop = 0; Loop < 51; Loop++) {
			TCHAR Name[BUFSIZE] = _T("");
			TCHAR Buf[BUFSIZE] = _T("");
			GenerateProductName(Name);
			if (lstrcmp(Type, _T("mysql")) == 0) {
				_snwprintf_s(Buf, BUFSIZE, _TRUNCATE, _T("%s(Rec%05d)"), Name, Lo);
			} else {
				_snwprintf_s(Buf, BUFSIZE, _TRUNCATE, _T("\"%s(Rec%05d)\""), Name, Lo);
			}
			if (Loop == 50) {
				lstrcat(Buf, _T("\n"));
			} else {
				lstrcat(Buf, _T(","));
			}
			WriteText(Buf, Type);
		}
	}
	return 0;
}

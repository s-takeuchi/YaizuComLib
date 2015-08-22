#include "msgproc.h"

MessageProc* MessageProc::Instance = NULL;

class MessageProc::Impl
{
public:
	static const int ENG = 0;
	static const int JPN = 1;

	static const int MAX_MSG_COUNT = 10000;

	int Mode;

	TCHAR* StkMsg[MAX_MSG_COUNT][2];
	BYTE* StkMsgSjis[MAX_MSG_COUNT][2];

	void Eng(int, TCHAR*);
	void Jpn(int, TCHAR*);

	void AllClear();

	int GetLocale();
};

int MessageProc::Impl::GetLocale()
{
	if (Mode == MessageProc::LOCALE_MODE_WIN32) {
		TCHAR Buf[256];
		GetLocaleInfo(GetUserDefaultLCID(), LOCALE_SENGLANGUAGE, Buf, 256);
		if (lstrcmp(Buf, _T("Japanese")) == 0) { // "XXX" or "Japanese"
			return MessageProc::MLANG_JAPANESE;
		}
		return MessageProc::MLANG_ENGLISH;
	}
	if (Mode == MessageProc::LOCALE_MODE_WEB) {
		char* Locale;
		size_t LocaleSize;
		if (_dupenv_s(&Locale, &LocaleSize, "HTTP_ACCEPT_LANGUAGE") == 0) {
			if (Locale == 0 || LocaleSize ==0) {
				return 0;
			}
			if (strstr(Locale, (char*)"ja") == Locale) {
				free(Locale);
				return MessageProc::MLANG_JAPANESE;
			}
		}
		return MessageProc::MLANG_ENGLISH;
	}
	if (Mode == MessageProc::LOCALE_MODE_ENGLISH) {
		return MessageProc::MLANG_ENGLISH;
	}
	if (Mode == MessageProc::LOCALE_MODE_JAPANESE) {
		return MessageProc::MLANG_JAPANESE;
	}
	return MessageProc::MLANG_ENGLISH;
}

void MessageProc::Impl::Eng(int Id, TCHAR* Msg)
{
	if (StkMsg[Id][ENG] != NULL) {
		delete StkMsg[Id][ENG];
	}
	int WcBufLen = lstrlen(Msg);
	StkMsg[Id][ENG] = new TCHAR[WcBufLen + 1];
	lstrcpyn(StkMsg[Id][ENG], Msg, WcBufLen + 1);

	if (StkMsgSjis[Id][ENG] != NULL) {
		delete StkMsgSjis[Id][ENG];
	}
	int MltBufLen = WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, (LPSTR)NULL, 0, NULL, NULL);
	StkMsgSjis[Id][ENG] = (BYTE*)new CHAR[MltBufLen];
	if (WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, (LPSTR)StkMsgSjis[Id][ENG], MltBufLen, NULL, NULL) != 0) {
		StkMsgSjis[Id][ENG][MltBufLen - 1] = 0;
	}
}

void MessageProc::Impl::Jpn(int Id, TCHAR* Msg)
{
	if (StkMsg[Id][JPN] != NULL) {
		delete StkMsg[Id][JPN];
	}
	int WcBufLen = lstrlen(Msg);
	StkMsg[Id][JPN] = new TCHAR[WcBufLen + 1];
	lstrcpyn(StkMsg[Id][JPN], Msg, WcBufLen + 1);

	if (StkMsgSjis[Id][JPN] != NULL) {
		delete StkMsgSjis[Id][JPN];
	}
	int MltBufLen = WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, (LPSTR)NULL, 0, NULL, NULL);
	StkMsgSjis[Id][JPN] = (BYTE*)new CHAR[MltBufLen];
	if (WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, (LPSTR)StkMsgSjis[Id][JPN], MltBufLen, NULL, NULL) != 0) {
		StkMsgSjis[Id][JPN][MltBufLen - 1] = 0;
	}
}

// This function clears all of messages without memory release of existing allocated message
void MessageProc::Impl::AllClear()
{
	for (int Loop = 0; Loop < Impl::MAX_MSG_COUNT; Loop++) {
		StkMsg[Loop][ENG] = NULL;
		StkMsg[Loop][JPN] = NULL;
		StkMsgSjis[Loop][ENG] = NULL;
		StkMsgSjis[Loop][JPN] = NULL;
	}
}

MessageProc::MessageProc()
{
	pImpl = new Impl;
	pImpl->Mode = MessageProc::LOCALE_MODE_WIN32;
	pImpl->AllClear();

	// =====================================================================================================
	//2201 (NC2)
	pImpl->Jpn(VAR_SELECTEDNOTONE, _T("変数が指定されていないか，2つ以上の変数が指定されました。\r\n1つの変数のみ指定可能です。"));
	pImpl->Eng(VAR_SELECTEDNOTONE, _T("No variable or more than one variable has been specified.\r\nOnly one variable may be specified."));

	//2202 (NC2)
	pImpl->Jpn(VAR_AREYOUSUREDELETE, _T("指定した変数を削除しても良いですか。"));
	pImpl->Eng(VAR_AREYOUSUREDELETE, _T("Are you sure you want to delete the specified variable(s)?"));

	//2203 (NC2)
	pImpl->Jpn(VAR_NOITEMSELECTED, _T("変数が指定されていません。\r\n１つ以上の変数を指定する必要があります。"));
	pImpl->Eng(VAR_NOITEMSELECTED, _T("No variable has been specified.\r\nPlease specify one or more variables."));

	//2204 (NC2)
	pImpl->Jpn(VAR_NOTTRANSUTF8, _T("UTF-8に変換できないコードが存在するため，\r\nUFT-8表示画面に遷移できません。"));
	pImpl->Eng(VAR_NOTTRANSUTF8, _T("The screen cannot be changed to UTF-8 view \r\nbecause the program has found code that cannot be rendered in UTF-8."));

	//2205 (NC2)
	pImpl->Jpn(VAR_INVALIDNAME, _T("不正な変数名が指定されました。\r\n指定した変数名を確認して再度実行してください。"));
	pImpl->Eng(VAR_INVALIDNAME, _T("Invalid variable name specified.\r\nCheck the specified variable name and then try again."));

	//2206 (NC2)
	pImpl->Jpn(VAR_ALREADYDELETED, _T("編集対象の変数は既に削除されています。\r\n編集操作は実行できません。"));
	pImpl->Eng(VAR_ALREADYDELETED, _T("The edit operation can not be performed because \r\nthe edit target variable has been deleted."));

	//2207 (NC2)
	pImpl->Jpn(VAR_INVALIDTYPE, _T("編集対象の変数は既に異なる種別の変数として使用されています。\r\n編集操作は実行できません。"));
	pImpl->Eng(VAR_INVALIDTYPE, _T("The edit operation can not be performed because \r\nthe target variable has already been used as another type of variable."));

	//2208 (NC3)
	pImpl->Jpn(VAR_BUFOVERFLOW, _T("変数の長さが上限の10Mバイトを超えました。\r\n上限を超えたデータは切り取られます。"));
	pImpl->Eng(VAR_BUFOVERFLOW, _T("The length of the variable specified exceeds the upper limit of 10 MB.\r\nAny data over this limit will be omitted."));

	//2209 (NC2)
	pImpl->Jpn(VAR_MAXVARSIZE, _T("変数を格納するメモリ容量が上限を超えました。"));
	pImpl->Eng(VAR_MAXVARSIZE, _T("The memory size available for storing variable data has exceeded the upper limit."));

	//2210 (NC3)
	pImpl->Jpn(VAR_FLAGVARNOT, _T("フラグ用変数は，インポート／エクスポートを行うことはできません。"));
	pImpl->Eng(VAR_FLAGVARNOT, _T("Import/export operation of the flag variable cannot be performed."));

	//2211 (NC3)
	pImpl->Jpn(VAR_FOLDERSETERR, _T("指定されたインポート，エクスポートフォルダは不正です。\r\nフォルダを確認し，再度操作を実行してください。"));
	pImpl->Eng(VAR_FOLDERSETERR, _T("An invalid import/export folder has been specified.\r\nCheck the folder and try again."));

	//2212 (NC3)
	pImpl->Jpn(VAR_IMPERR, _T("インポート操作が失敗しました。\r\nフォルダのアクセス権限またはファイル名を確認してください。"));
	pImpl->Eng(VAR_IMPERR, _T("Import operation failed.\r\nCheck the folder permissions and file names."));

	//2213 (NC3)
	pImpl->Jpn(VAR_EXPERR, _T("エクスポート操作が失敗しました。\r\nフォルダのアクセス権限や空きディスク容量，選択した変数名を確認してください。"));
	pImpl->Eng(VAR_EXPERR, _T("Export operation failed.\r\nCheck the folder permissions, the amount of free disk space and the variable names specified."));

	//2214 (NC3)
	pImpl->Jpn(VAR_NOIMPTARGET, _T("指定したフォルダにはインポート対象ファイルが存在しません。\r\nインポート対象ファイルの拡張子は\".xxx\"である必要があります。"));
	pImpl->Eng(VAR_NOIMPTARGET, _T("No import target file exists in the folder specified.\r\nThe extension of import target files must be \".xxx\"."));

	//2215 (NC3)
	pImpl->Jpn(VAR_NOEXPTARGET, _T("変数が指定されていません。\r\n一つ以上の変数を指定する必要があります。"));
	pImpl->Eng(VAR_NOEXPTARGET, _T("No variables have been specified.\r\nMore than one variable must be specified."));

	// =====================================================================================================
	// 2300 (---)
	pImpl->Jpn(PROP_NAME, _T("名称"));
	pImpl->Eng(PROP_NAME, _T("Name"));

	// 2301 (NC2)
	pImpl->Jpn(PROP_CHKDAT_START, _T("入力データは...から開始する"));
	pImpl->Eng(PROP_CHKDAT_START, _T("Input data starts with..."));

	// 2302 (NC2)
	pImpl->Jpn(PROP_CHKDAT_NSTART, _T("入力データは...から開始しない"));
	pImpl->Eng(PROP_CHKDAT_NSTART, _T("Input data does not start with..."));

	// 2303 (NC2)
	pImpl->Jpn(PROP_CHKDAT_END, _T("入力データは...で終了する"));
	pImpl->Eng(PROP_CHKDAT_END, _T("Input data ends with..."));

	// 2304 (NC2)
	pImpl->Jpn(PROP_CHKDAT_NEND, _T("入力データは...で終了しない"));
	pImpl->Eng(PROP_CHKDAT_NEND, _T("Input data does not end with..."));

	// 2305 (NC2)
	pImpl->Jpn(PROP_CHKDAT_CONT, _T("入力データは...を含む"));
	pImpl->Eng(PROP_CHKDAT_CONT, _T("Input data contains..."));

	// 2306 (NC2)
	pImpl->Jpn(PROP_CHKDAT_NCONT, _T("入力データは...を含まない"));
	pImpl->Eng(PROP_CHKDAT_NCONT, _T("Input data does not contain..."));

	// 2307 (NC2)
	pImpl->Jpn(PROP_CHKDAT_EQUAL, _T("入力データは...と一致する"));
	pImpl->Eng(PROP_CHKDAT_EQUAL, _T("Input data is equal to..."));

	// 2308 (NC2)
	pImpl->Jpn(PROP_CHKDAT_NEQUAL, _T("入力データは...と一致しない"));
	pImpl->Eng(PROP_CHKDAT_NEQUAL, _T("Input data is not equal to..."));

	// 2311 (NC2)
	pImpl->Jpn(PROP_CHGDAT_INSERT, _T("入力データの開始バイトに(A)を挿入する"));
	pImpl->Eng(PROP_CHGDAT_INSERT, _T("Insert (A) at the start byte of the input data."));

	// 2312 (NC2)
	pImpl->Jpn(PROP_CHGDAT_JOIN, _T("入力データの終了バイトに(A)を連結する"));
	pImpl->Eng(PROP_CHGDAT_JOIN, _T("Join (A) to the end byte of the input data."));

	// 2313 (NC2)
	pImpl->Jpn(PROP_CHGDAT_REPLACE, _T("入力データから(A)を検索し(B)に置き換える"));
	pImpl->Eng(PROP_CHGDAT_REPLACE, _T("Search for (A) in the input data and replace with (B)."));

	// 2314 (NC2)
	pImpl->Jpn(PROP_CHGDAT_EXTRACT, _T("入力データから(A)で始まり(B)で終わる領域を抽出する"));
	pImpl->Eng(PROP_CHGDAT_EXTRACT, _T("Extract an area that starts with (A) and ends with (B) from the input data."));

	// 2315 (NC2)
	pImpl->Jpn(PROP_CHGDAT_DROP, _T("入力データを捨て空データを返す"));
	pImpl->Eng(PROP_CHGDAT_DROP, _T("Drop the input data and return empty data."));

	// 2316 (NC2)
	pImpl->Jpn(PROP_CHGDAT_RESET, _T("入力データを捨て(A)にリセットする"));
	pImpl->Eng(PROP_CHGDAT_RESET, _T("Drop the input data and reset to (A)."));

	// 2320 (NC2)
	pImpl->Jpn(PROP_CLOSEPORT, _T("クローズするソケット"));
	pImpl->Eng(PROP_CLOSEPORT, _T("Socket to be closed"));

	// 2321 (NC2)
	pImpl->Jpn(PROP_TIMER_PAST, _T("次に示す日付と時刻が経過したら次のステップに移る"));
	pImpl->Eng(PROP_TIMER_PAST, _T("Progress to the next step if the date and time shown below are in the past."));

	// 2322 (NC2)
	pImpl->Jpn(PROP_TIMER_WAIT, _T("一定時間待ったあと次のステップに移る"));
	pImpl->Eng(PROP_TIMER_WAIT, _T("Progress to the next step after sleeping."));

	// 2323 (---)
	pImpl->Jpn(PROP_TIMER_SEC, _T("(秒) 0-3600"));
	pImpl->Eng(PROP_TIMER_SEC, _T("(Sec) 0-3600"));

	// 2324 (NC2)
	pImpl->Jpn(PROP_MAP_SEARCH, _T("検索対象変数名のプレフィックス"));
	pImpl->Eng(PROP_MAP_SEARCH, _T("Prefix of search target variable name."));

	// 2325 (NC2)
	pImpl->Jpn(PROP_MAP_REPLACE, _T("置換え対象変数名のプレフィックス"));
	pImpl->Eng(PROP_MAP_REPLACE, _T("Prefix of replacement target variable name"));

	// 2326 (NC2)
	pImpl->Jpn(PROP_MAP_START, _T("次の値から自動採番される"));
	pImpl->Eng(PROP_MAP_START, _T("Auto numbering from the next value"));

	// 2327 (NC3)
	pImpl->Jpn(PROP_MAP_USEONCE, _T("各変数へのマッピングは一度だけ使用される"));
	pImpl->Eng(PROP_MAP_USEONCE, _T("The mapping to each variable is used only once."));

	// 2331 (NC2)
	pImpl->Jpn(PROP_CHECK_TRUE, _T("TRUEならば次のステップに移る"));
	pImpl->Eng(PROP_CHECK_TRUE, _T("If TRUE, progress to the next step."));

	// 2332 (NC2)
	pImpl->Jpn(PROP_CHECK_FALSE, _T("FALSEならば次のステップに移る"));
	pImpl->Eng(PROP_CHECK_FALSE, _T("If FALSE, progress to the next step."));

	// 2333 (NC2)
	pImpl->Jpn(PROP_CHANGE_TRUE, _T("フラグをTRUEにする"));
	pImpl->Eng(PROP_CHANGE_TRUE, _T("Change flag to TRUE."));

	// 2334 (NC2)
	pImpl->Jpn(PROP_CHANGE_FALSE, _T("フラグをFALSEにする"));
	pImpl->Eng(PROP_CHANGE_FALSE, _T("Change flag to FALSE."));

	// 2341 (NC2)
	pImpl->Jpn(PROP_LOAD_VAR, _T("次の変数からデータを読込む"));
	pImpl->Eng(PROP_LOAD_VAR, _T("Load data from the following variable."));

	// 2342 (NC2)
	pImpl->Jpn(PROP_LOAD_CONT, _T("次の変数から連続的にデータを読込む"));
	pImpl->Eng(PROP_LOAD_CONT, _T("Load data from the following variables continuously."));

	// 2343 (NC2)
	pImpl->Jpn(PROP_STORE_VAR, _T("次の変数にデータを書き込む"));
	pImpl->Eng(PROP_STORE_VAR, _T("Store data into the following variable."));

	// 2344 (NC2)
	pImpl->Jpn(PROP_STORE_CONT, _T("次の変数に連続的にデータを書き込む"));
	pImpl->Eng(PROP_STORE_CONT, _T("Store data into the following variables continuously."));

	// 2345 (---)
	pImpl->Jpn(PROP_DATA_COMM, _T("コミュニケーション用変数"));
	pImpl->Eng(PROP_DATA_COMM, _T("Communication variable"));

	// 2346 (---)
	pImpl->Jpn(PROP_DATA_PREF, _T("変数名のプレフィックス"));
	pImpl->Eng(PROP_DATA_PREF, _T("Prefix of variable name"));

	// 2347 (NC3)
	pImpl->Jpn(PROP_DATA_NUM, _T("次の値からの自動採番"));
	pImpl->Eng(PROP_DATA_NUM, _T("Auto numbering from the next value"));

	// 2351 (NC2)
	pImpl->Jpn(PROP_NET_RECV, _T("指定したIPアドレスとポートからデータを受信する"));
	pImpl->Eng(PROP_NET_RECV, _T("Receive data using the specified IP address and port."));

	// 2352 (NC5)
	pImpl->Jpn(PROP_NET_RECVTGT, _T("指定した送信要素の接続先からデータを受信する"));
	pImpl->Eng(PROP_NET_RECVTGT, _T("Receive data from the connection target of the specified sender element."));

	// 2353 (NC2)
	pImpl->Jpn(PROP_NET_SEND, _T("指定したIPアドレスとポートにデータを送信する"));
	pImpl->Eng(PROP_NET_SEND, _T("Send data to the specified IP address and port."));

	// 2354 (NC5)
	pImpl->Jpn(PROP_NET_SENDTGT, _T("指定した受信要素の接続先にデータを送信する"));
	pImpl->Eng(PROP_NET_SENDTGT, _T("Send data to the connection target of the specified receiver element."));

	// 2355 (NC2)
	pImpl->Jpn(PROP_NET_CLOSE_AFTERSEND, _T("データ送信後ソケットをクローズする"));
	pImpl->Eng(PROP_NET_CLOSE_AFTERSEND, _T("Close socket after sending data."));

	// 2356 (NC2)
	pImpl->Jpn(PROP_NET_CLOSE_AFTERRECV, _T("データ受信後ソケットをクローズする"));
	pImpl->Eng(PROP_NET_CLOSE_AFTERRECV, _T("Close socket after receiving data."));

	// 2357 (NC4)
	pImpl->Jpn(PROP_NET_TIMEOUT, _T("最終アクセス以降にタイムアウトを検知した [1-180,000 ミリ秒]"));
	pImpl->Eng(PROP_NET_TIMEOUT, _T("Time-out detected after last access.  [1 - 180,000 mSec]"));

	// 2358 (---)
	pImpl->Jpn(PROP_NET_SENDER, _T("送信要素名"));
	pImpl->Eng(PROP_NET_SENDER, _T("Sender name"));

	// 2359 (---)
	pImpl->Jpn(PROP_NET_RECEIVER, _T("受信要素名"));
	pImpl->Eng(PROP_NET_RECEIVER, _T("Receiver name"));

	// 2360 (---)
	pImpl->Jpn(PROP_NET_ICON, _T("アイコン"));
	pImpl->Eng(PROP_NET_ICON, _T("Icon"));

	// 2361 (---)
	pImpl->Jpn(PROP_NET_IPADDR, _T("IPアドレス/ホスト名"));
	pImpl->Eng(PROP_NET_IPADDR, _T("IP address/hostname"));

	// 2362 (---)
	pImpl->Jpn(PROP_NET_PORT, _T("ポート番号"));
	pImpl->Eng(PROP_NET_PORT, _T("Port"));

	// 2363 (NC4)
	pImpl->Jpn(PROP_FILEACCESS_NAME, _T("プログラムがアクセスするファイル名を指定\r\nカレントフォルダからの相対パスを指定する必要がある。(例:\"..\\data\\test.bin\")\r\nカレントフォルダは通常，対象の\".std\"ファイルが格納されているフォルダとなる。"));
	pImpl->Eng(PROP_FILEACCESS_NAME, _T("Specify the file name used by the program. \r\nThe relative path from the current folder should be given (ex: \"..\\data\\test.bin\"). \r\nThe current folder is normally set as the one containing the target \".std\" file."));

	// 2364 (NC4)
	pImpl->Jpn(PROP_FILEACCESSW_ADD, _T("既存のファイルの最後にデータを連結する"));
	pImpl->Eng(PROP_FILEACCESSW_ADD, _T("Add data to the end of the existing file."));

	// 2365 (NC4)
	pImpl->Jpn(PROP_FILEACCESSW_INSERT, _T("既存のファイルの先頭ににデータを挿入する"));
	pImpl->Eng(PROP_FILEACCESSW_INSERT, _T("Insert data at the start of the existing file."));

	// 2366 (NC4)
	pImpl->Jpn(PROP_FILEACCESSW_OVERWRITE, _T("既存のファイルを上書きする"));
	pImpl->Eng(PROP_FILEACCESSW_OVERWRITE, _T("Overwrite the existing file."));

	// 2367 (NC4)
	pImpl->Jpn(PROP_FILEACCESSR_ADD, _T("入力データの終了バイトに読み込んだデータを連結する"));
	pImpl->Eng(PROP_FILEACCESSR_ADD, _T("Add loaded data to the end byte of the input data."));

	// 2368 (NC4)
	pImpl->Jpn(PROP_FILEACCESSR_INSERT, _T("入力データの開始バイトに読み込んだデータを挿入する"));
	pImpl->Eng(PROP_FILEACCESSR_INSERT, _T("Insert loaded data at the start byte of the input data."));

	// 2369 (NC4)
	pImpl->Jpn(PROP_FILEACCESSR_OVERWRITE, _T("ファイルから読み込んだデータを出力する"));
	pImpl->Eng(PROP_FILEACCESSR_OVERWRITE, _T("Output data read from the file."));

	// 2370 (NC4)
	pImpl->Jpn(PROP_EXECPROG_COMMAND, _T("外部プログラムへのパスを指定"));
	pImpl->Eng(PROP_EXECPROG_COMMAND, _T("Specify the path to the external program."));

	// 2371 (NC4)
	pImpl->Jpn(PROP_EXECPROG_WAIT, _T("実行したプログラムが終了するまで待機する"));
	pImpl->Eng(PROP_EXECPROG_WAIT, _T("Wait until the program executed is finished."));

	// 2372 (NC4)
	pImpl->Jpn(PROP_EXECPROG_CURRDIR, _T("外部プログラム実行時のカレントフォルダを指定\r\nカレントフォルダを指定しない場合，対象の\".std\"ファイルが配置されたフォルダがカレントフォルダとなる。"));
	pImpl->Eng(PROP_EXECPROG_CURRDIR, _T("Specify the current folder during external program execution. \r\nIf the current folder is not specified, the one containing the target \".std\" file will be set."));

	// 2373 (NC4)
	pImpl->Jpn(PROP_EXECPROG_STDOUT, _T("外部プログラムがSTDOUTへ書き込んだデータを次の処理要素に渡す"));
	pImpl->Eng(PROP_EXECPROG_STDOUT, _T("Pass data written by the external program to STDOUT to the next element."));

	// 2374 (NC4)
	pImpl->Jpn(PROP_EXECPROG_STDIN, _T("本処理要素の入力データを外部プログラムのSTDINに渡す"));
	pImpl->Eng(PROP_EXECPROG_STDIN, _T("Pass the input data of this processing element to STDIN."));

	// 2375 (NC4)
	pImpl->Jpn(PROP_NET_UNCOND, _T("無条件で次の処理要素に移る"));
	pImpl->Eng(PROP_NET_UNCOND, _T("Proceed unconditionally to the next processing element."));

	// 2376 (NC4)
	pImpl->Jpn(PROP_NET_EXCEEDSIZE, _T("受信データサイズが指定した値を超えた [1-9,999,999 バイト]"));
	pImpl->Eng(PROP_NET_EXCEEDSIZE, _T("The size of the data received exceeds the preset value. [1-9,999,999 bytes]"));

	// 2377 (NC4)
	pImpl->Jpn(PROP_NET_RECVSTR, _T("指定したデータで完了するデータを受信した"));
	pImpl->Eng(PROP_NET_RECVSTR, _T("Data ending with the specified value received."));

	// 2378 (NC4)
	pImpl->Jpn(PROP_NET_TERMCOND, _T("終了条件"));
	pImpl->Eng(PROP_NET_TERMCOND, _T("Termination condition"));

	// 2379 (NC4)
	pImpl->Jpn(PROP_NET_CLOSEDETECT, _T("送信側ソケットがクローズされたときだけ次の処理要素に移る"));
	pImpl->Eng(PROP_NET_CLOSEDETECT, _T("Proceed to the next processing element only when the sender socket is closed."));

	// 2380 (---)
	pImpl->Jpn(PROP_NET_CLOSEFORCE, _T("強制的にクローズする"));
	pImpl->Eng(PROP_NET_CLOSEFORCE, _T("Force close"));

	// 2381 (---)
	pImpl->Jpn(PROP_NET_CLOSEUDPPORT, _T("UDPソケットをクローズする"));
	pImpl->Eng(PROP_NET_CLOSEUDPPORT, _T("Close UDP socket"));

	// 2382 (---)
	pImpl->Jpn(PROP_NET_CLOSETCPPORT, _T("TCPソケットをクローズする"));
	pImpl->Eng(PROP_NET_CLOSETCPPORT, _T("Close TCP socket"));

	// 2383 (NC6)
	pImpl->Jpn(PROP_NET_PROCEEDEVENIFNODATARECV, _T("データ未受信でも次の処理要素に遷移する"));
	pImpl->Eng(PROP_NET_PROCEEDEVENIFNODATARECV, _T("Proceed even if no data are received."));

	// 2384 (★)
	pImpl->Jpn(PROP_NET_RECVMULTI, _T("指定した受信要素からデータを受信する"));
	pImpl->Eng(PROP_NET_RECVMULTI, _T("Receive data from the specified receiver."));

	// =====================================================================================================
	// 2400 (---)
	pImpl->Jpn(STKFW_LOG_TITLE, _T("StkFw - スレッドコントローラ\r\n"));
	pImpl->Eng(STKFW_LOG_TITLE, _T("StkFw - Thread controller\r\n"));

	// 2401 (NC3)
	pImpl->Jpn(STKFW_LOG_START, _T("スレッドコントローラはログの出力を開始しました。\r\n"));
	pImpl->Eng(STKFW_LOG_START, _T("The thread controller has started logging.\r\n"));

	// 2402 (NC3)
	pImpl->Jpn(STKFW_LOG_STOP, _T("スレッドコントローラはログの出力を停止しました。\r\n"));
	pImpl->Eng(STKFW_LOG_STOP, _T("The thread controller has stopped logging.\r\n"));

	// 2403 (NC3)
	pImpl->Jpn(STKFW_LOG_SUCCESSCSC, _T("ソケットの生成およびCONNECTに成功しました。"));
	pImpl->Eng(STKFW_LOG_SUCCESSCSC, _T("Creation of socket and connection succeeded."));

	// 2404 (NC3)
	pImpl->Jpn(STKFW_LOG_SUCCESSCSBNLS, _T("ソケットの生成，BINDおよびLISTENに成功しました。"));
	pImpl->Eng(STKFW_LOG_SUCCESSCSBNLS, _T("Creation of socket, binding and listening succeeded."));

	// 2405 (NC3)
	pImpl->Jpn(STKFW_LOG_SOCKCLOSE, _T("ソケットのクローズに成功しました。"));
	pImpl->Eng(STKFW_LOG_SOCKCLOSE, _T("Closure of socket succeeded."));

	// 2406 (NC3)
	pImpl->Jpn(STKFW_LOG_ACPTRECV, _T("ACCEPT用ソケットでデータを受信しました。"));
	pImpl->Eng(STKFW_LOG_ACPTRECV, _T("Data received using the accepting socket."));

	// 2407 (NC3)
	pImpl->Jpn(STKFW_LOG_CNCTRECV, _T("接続中のソケットでデータが返信されました。"));
	pImpl->Eng(STKFW_LOG_CNCTRECV, _T("Data returned using the communicating socket."));

	// 2408 (NC3)
	pImpl->Jpn(STKFW_LOG_ACPTSEND, _T("接続中のソケットでデータを返信しました。"));
	pImpl->Eng(STKFW_LOG_ACPTSEND, _T("Data sent back using the communicating socket."));

	// 2409 (NC3)
	pImpl->Jpn(STKFW_LOG_CNCTSEND, _T("CONNECTしたソケットでデータを送信しました。"));
	pImpl->Eng(STKFW_LOG_CNCTSEND, _T("Data sent using the connected socket."));

	// 2410 (NC3)
	pImpl->Jpn(STKFW_LOG_SENDERROR, _T("データ送信時にエラーが発生しました。"));
	pImpl->Eng(STKFW_LOG_SENDERROR, _T("An error occurred during data transmission."));

	// 2411 (NC3)
	pImpl->Jpn(STKFW_LOG_RECVERROR, _T("データ受信時にエラーが発生しました。"));
	pImpl->Eng(STKFW_LOG_RECVERROR, _T("An error occurred during data receipt."));

	// 2412 (NC3)
	pImpl->Jpn(STKFW_LOG_NAMESOLVEERR, _T("指定されたホスト名の解決に失敗しました。"));
	pImpl->Eng(STKFW_LOG_NAMESOLVEERR, _T("Resolution of the specified host name failed."));

	// 2413 (NC3)
	pImpl->Jpn(STKFW_LOG_CONNERROR, _T("指定されたホストへの接続に失敗しました。"));
	pImpl->Eng(STKFW_LOG_CONNERROR, _T("Connection to the specified host failed."));

	// 2414 (NC3)
	pImpl->Jpn(STKFW_LOG_BINDLISTENERR, _T("BINDまたはLISTENに失敗しました。"));
	pImpl->Eng(STKFW_LOG_BINDLISTENERR, _T("Binding or listening failed."));

	// 2415 (NC3)
	pImpl->Jpn(STKFW_LOG_CREATEACCEPTSOCK, _T("ACCEPT用ソケットの生成に成功しました。"));
	pImpl->Eng(STKFW_LOG_CREATEACCEPTSOCK, _T("Creation of accepting socket succeeded."));

	// 2416 (NC3)
	pImpl->Jpn(STKFW_LOG_CLOSEACCEPTSOCK, _T("ACCEPT用ソケットのクローズに成功しました。"));
	pImpl->Eng(STKFW_LOG_CLOSEACCEPTSOCK, _T("Closure of accepting socket succeeded."));

	// 2417 (NC5)
	pImpl->Jpn(STKFW_LOG_CLOSELISTENACCEPTSOCK, _T("ACCEPT用ソケットおよびLISTEN用ソケットのクローズに成功しました。"));
	pImpl->Eng(STKFW_LOG_CLOSELISTENACCEPTSOCK, _T("Closure of accept/listen sockets succeeded."));

	// 2418 (NC5)
	pImpl->Jpn(STKFW_LOG_BINDERR, _T("BINDに失敗しました。"));
	pImpl->Eng(STKFW_LOG_BINDERR, _T("Binding to port failed."));

	// 2419 (NC5)
	pImpl->Jpn(STKFW_LOG_SUCCESSCSBN, _T("UDPソケットの生成およびBINDに成功しました。"));
	pImpl->Eng(STKFW_LOG_SUCCESSCSBN, _T("Creation of socket and binding succeeded."));

	// 2420 (NC5)
	pImpl->Jpn(STKFW_LOG_UDPRECV, _T("UDPソケットでデータを受信しました。"));
	pImpl->Eng(STKFW_LOG_UDPRECV, _T("Data received using UDP socket."));

	// 2421 (NC5)
	pImpl->Jpn(STKFW_LOG_UDPSEND, _T("UDPソケットでデータを送信しました。"));
	pImpl->Eng(STKFW_LOG_UDPSEND, _T("Data sent using UDP socket."));

	// 2422 (NC5)
	pImpl->Jpn(STKFW_LOG_SUCCESSCS, _T("UDPソケットの生成に成功しました。"));
	pImpl->Eng(STKFW_LOG_SUCCESSCS, _T("Creation of UDP socket succeeded."));

	// 2423 (NC5)
	pImpl->Jpn(STKFW_LOG_UDPSOCKCLOSE, _T("UDPソケットのクローズに成功しました。"));
	pImpl->Eng(STKFW_LOG_UDPSOCKCLOSE, _T("Closure of UDP socket succeeded."));

	// =====================================================================================================
	//2901 (NC2)
	pImpl->Jpn(UNKNOWNERROR, _T("不明なエラーが発生しました。"));
	pImpl->Eng(UNKNOWNERROR, _T("An unknown error has occurred."));

	//2902 (NC2)
	pImpl->Jpn(FILEACCESSERROR, _T("ファイルアクセスエラーが発生しました。"));
	pImpl->Eng(FILEACCESSERROR, _T("A file access error has occurred."));

	//2903 (NC1)
	pImpl->Jpn(NEEDADMINRIGHTS, _T("このプログラムを実行するためにはLocal Administratorの権限が必要です。\r\nプログラムは終了します。\r\n\r\n"));
	pImpl->Eng(NEEDADMINRIGHTS, _T("You need local administrator's rights to run this program.\r\nThe program will be terminated.\r\n\r\n"));

	// 2904 (NC3)
	pImpl->Jpn(DATAUPDATED, _T("プログラムが使用するデータは更新されています。\r\nデータが失われますが操作を続けて良いですか。"));
	pImpl->Eng(DATAUPDATED, _T("Program data updated.\r\nData will be lost. Continue operation?"));

	// 2905 (NC4)
	pImpl->Jpn(INVALIDVALUE, _T("指定した値が不正です。値の指定可能な範囲を確認してください。"));
	pImpl->Eng(INVALIDVALUE, _T("Invalid value specified. Check the range of values allowed."));

	//2906 (---)
	pImpl->Jpn(AN_ERROR_OCCURRED, _T("エラーが発生しました。"));
	pImpl->Eng(AN_ERROR_OCCURRED, _T("An error occurred."));

	//2931 (---)
	pImpl->Jpn(COMMON_ADD, _T("追加"));
	pImpl->Eng(COMMON_ADD, _T("Add"));

	//2932 (---)
	pImpl->Jpn(COMMON_DELETE, _T("削除"));
	pImpl->Eng(COMMON_DELETE, _T("Delete"));

	//2933 (---)
	pImpl->Jpn(COMMON_EDIT, _T("編集"));
	pImpl->Eng(COMMON_EDIT, _T("Edit"));

	//2934 (---)
	pImpl->Jpn(COMMON_REFRESH, _T("更新"));
	pImpl->Eng(COMMON_REFRESH, _T("Refresh"));

	//2935 (---)
	pImpl->Jpn(COMMON_TRUE, _T("True"));
	pImpl->Eng(COMMON_TRUE, _T("True"));

	//2936 (---)
	pImpl->Jpn(COMMON_FALSE, _T("False"));
	pImpl->Eng(COMMON_FALSE, _T("False"));

	//2937 (---)
	pImpl->Jpn(COMMON_UNKNOWN, _T("不明"));
	pImpl->Eng(COMMON_UNKNOWN, _T("Unknown"));

	//2941 (---)
	pImpl->Jpn(COMMON_OK, _T("OK"));
	pImpl->Eng(COMMON_OK, _T("OK"));

	//2942 (---)
	pImpl->Jpn(COMMON_CANCEL, _T("ｷｬﾝｾﾙ"));
	pImpl->Eng(COMMON_CANCEL, _T("Cancel"));

	// =====================================================================================================
	//3100 (---)
	pImpl->Jpn(CMDFRK_CMDFRKSRV, _T("CmdFreak service program\r\nCopyright (C) 2014 Shinya Takeuchi\r\nAll Rights Reserved.\r\n"));
	pImpl->Eng(CMDFRK_CMDFRKSRV, _T("CmdFreak service program\r\nCopyright (C) 2014 Shinya Takeuchi\r\nAll Rights Reserved.\r\n"));

	//3101 (NC6)
	pImpl->Jpn(CMDFRK_CANNOT_START, _T("\r\nこのプログラムはWindowsサービスプログラムのため起動することができません。\r\nこのプログラムはCmdFreakのインストール時にWindowsサービスとしてシステムに登録されます。\r\n\r\n"));
	pImpl->Eng(CMDFRK_CANNOT_START, _T("\r\nThis program cannot be started because it is configured as a Windows service program.\r\nThe program is registered as a Windows service in the system when CmdFreak is installed.\r\n\r\n"));

	//3121 (NC6)
	pImpl->Jpn(CMDFRK_ERROR_UNEXPECTED, _T("予期しないエラーが発生しました。このエラーはWebブラウザからのリクエストが解析できなかったときに発生する場合があります。<br>"));
	pImpl->Eng(CMDFRK_ERROR_UNEXPECTED, _T("An unexpected error occurred. This may be caused by inability to analyze request data from the web browser.<br>"));

	//3122 (NC6)
	pImpl->Jpn(CMDFRK_ERROR_SERVICEDOWN, _T("CmdFreakサービスとの通信が失敗しました。次の要因が考えられます。<br>(1) CmdFreakサービスが開始されていない。<br>(2) CmdFreakサービスがファイアウォールに例外登録されていない。<br>(3) 接続先ホスト名およびポート番号の定義ファイル [bbb.conf, cmdfrksrv.conf] が不正。<br>(4) CmdFreakの内部処理で異常が発生している。<br>"));
	pImpl->Eng(CMDFRK_ERROR_SERVICEDOWN, _T("Connection with CmdFreak service failed. This may be caused by one of the following issues:<br>(1) CmdFreak service cannot be started.<br>(2) CmdFreak service is not registered as a firewall exception.<br>(3) The definition file [bbb.conf and/or cmdfrksrv.conf] for the host name and port number in the network connectivity settings is invalid.<br>(4) A CmdFreak internal process is invalid.<br>"));

	//3123 (NC6)
	pImpl->Jpn(CMDFRK_ERROR_JAVASCRIPTERR, _T("JavaScriptが無効です。<br>ブラウザの設定を変更してJavaScriptを有効後，ページをリフレッシュしてください。<br>\n"));
	pImpl->Eng(CMDFRK_ERROR_JAVASCRIPTERR, _T("JavaScript is disabled in your browser.<br>Change the browser setting to enable it and refresh the page.<br>\n"));

	//3124 (NC6)
	pImpl->Jpn(CMDFRK_ODBCINFO, _T("ODBC接続の設定を行います。<br>接続対象のDBMSを選択し，ODBCの接続文字列を指定してください。<br>本ソフトウェアは32ビットアプリケーションのため，ODBC接続文字列には，32ビット版ODBCドライバを使用するように指定を行ってください。オペレーティングシステムには，あらかじめ32ビット版ODBCドライバがインストールされている必要があります。<br>"));
	pImpl->Eng(CMDFRK_ODBCINFO, _T("Configure ODBC connection.<br>Select the connection target DBMS and specify the ODBC connection string.<br>As this is 32-bit application software, this string should be specified for the use of a 32-bit ODBC driver. As a prerequisite, a 32-bit ODBC driver needs to be installed on the operating system.<br>"));

	//3125 (NC6)
	pImpl->Jpn(CMDFRK_ERROR_INVALIDDBCONN, _T("DBMSとの接続に失敗しました。<br>次の要因が考えられます。<br>- DBMSが適切に起動していない。<br>- ODBCドライバがインストールされていない。<br>- ODBCの接続文字列が不正。<br>- DBMSとの接続経路に何らかの問題がある。<br>"));
	pImpl->Eng(CMDFRK_ERROR_INVALIDDBCONN, _T("DBMS connection failed.<br>This may be caused by one of the following issues:<br>- The DBMS is not working properly.<br>- No ODBC driver is installed.<br>- The ODBC connection string is invalid.<br>- There are issues with the DBMS on the network.<br>"));

	//3126 (NC6)
	pImpl->Jpn(CMDFRK_LOG_DBMSCHANGE, _T("ODBC接続の設定を変更しました。"));
	pImpl->Eng(CMDFRK_LOG_DBMSCHANGE, _T("The ODBC connection configuration has been changed."));

	//3127 (NC6)
	pImpl->Jpn(CMDFRK_FILTER_DESC, _T("ここではフィルタリングの設定を行うことができます。<br>指定した条件でレコード情報がフィルタリングされます。<br>フィルタリング項目は最大5件を指定することができます。<br>各フィルタリング項目間は論理積[AND]で結合されます。"));
	pImpl->Eng(CMDFRK_FILTER_DESC, _T("Filter settings can be specified here.<br>Records are filtered according to the specified criteria.<br>Up to five filter items can be specified.<br>All items are treated as logical AND operators."));

	//3128 (NC6)
	pImpl->Jpn(CMDFRK_FILTER_INVALID, _T("設定されているフィルタリング条件は現在のテーブルのスキーマと一致していません。<br>フィルタリング条件をクリアしてよいですか？"));
	pImpl->Eng(CMDFRK_FILTER_INVALID, _T("The filter conditions set do not match the current table schema.<br>Do you want to clear the filter conditions?"));

	//3129 (NC6)
	pImpl->Jpn(CMDFRK_FILTERINGCOND_UPDATE, _T("フィルタリング条件を更新しました。"));
	pImpl->Eng(CMDFRK_FILTERINGCOND_UPDATE, _T("The filter conditions have been updated."));

	//3130 (NC6)
	pImpl->Jpn(CMDFRK_FILTERING_NOTABLE, _T("フィルタリングの対象となるテーブルが存在しないため操作を継続できません。"));
	pImpl->Eng(CMDFRK_FILTERING_NOTABLE, _T("The operation cannot be performed because the target table for filtering does not exist."));

	//3131 (NC6)
	pImpl->Jpn(CMDFRK_NOTABLEINFO, _T("表示対象のテーブルが存在しないため操作を継続できません。"));
	pImpl->Eng(CMDFRK_NOTABLEINFO, _T("The operation cannot be performed because the target table for display does not exist."));

	//3132 (NC6)
	pImpl->Jpn(CMDFRK_EMPTYCONNSTR, _T("本ソフトウェアを使用してデータベースのデータを参照するには，まずはじめにODBC接続の設定を行ってください。"));
	pImpl->Eng(CMDFRK_EMPTYCONNSTR, _T("To refer to data in a database using this software, the ODBC connection needs to be configured first."));

	//3133 (NC6)
	pImpl->Jpn(CMDFRK_WELCOME, _T("ようこそ，CmdFreakのページです！"));
	pImpl->Eng(CMDFRK_WELCOME, _T("Welcome to the CmdFreak page!"));

	//3134 (NC6)
	pImpl->Jpn(CMDFRK_ERROR_TOOMUCHRECS, _T("対象テーブルのレコード数がCmdFreakで操作可能な上限を超えました。"));
	pImpl->Eng(CMDFRK_ERROR_TOOMUCHRECS, _T("The number of records in the target table exceeds the limit that CmdFreak can handle."));

	//3135 (NC6)
	pImpl->Jpn(CMDFRK_ERROR_TOOLONGCONNSTR, _T("ODBCの接続文字列で指定可能な長さの上限を超えました。"));
	pImpl->Eng(CMDFRK_ERROR_TOOLONGCONNSTR, _T("The length of the ODBC connection string exceeds the allowable limit."));

	//3136 (NC6)
	pImpl->Jpn(CMDFRK_ERROR_TOOLONGFILTER, _T("フィルタリングの条件が指定可能な上限を超えました。"));
	pImpl->Eng(CMDFRK_ERROR_TOOLONGFILTER, _T("The length of the filtering condition string exceeds the allowable limit."));

	//3142 (NC6)
	pImpl->Jpn(CMDFRK_SVCFAILEDTOSTART, _T("CmdFreakサービスの開始に失敗しました。"));
	pImpl->Eng(CMDFRK_SVCFAILEDTOSTART, _T("CmdFreak service initiation failed."));

	//3143 (NC6)
	pImpl->Jpn(CMDFRK_SVCFAILEDTOSTOP, _T("CmdFreakサービスの停止に失敗しました。"));
	pImpl->Eng(CMDFRK_SVCFAILEDTOSTOP, _T("CmdFreak service termination failed."));

	//3144 (NC6)
	pImpl->Jpn(CMDFRK_SVCALREADYSTART, _T("CmdFreakサービスはすでに開始しています。"));
	pImpl->Eng(CMDFRK_SVCALREADYSTART, _T("CmdFreak service has already been started."));

	//3145 (NC6)
	pImpl->Jpn(CMDFRK_SVCALREADYSTOP, _T("CmdFreakサービスはすでに停止しています。"));
	pImpl->Eng(CMDFRK_SVCALREADYSTOP, _T("CmdFreak service has already been stopped."));

	//3146 (NC6)
	pImpl->Jpn(CMDFRK_SVCSTART, _T("CmdFreakサービスを開始しました。"));
	pImpl->Eng(CMDFRK_SVCSTART, _T("CmdFreak service has started."));

	//3147 (NC6)
	pImpl->Jpn(CMDFRK_SVCSTOP, _T("CmdFreakサービスを停止しました。"));
	pImpl->Eng(CMDFRK_SVCSTOP, _T("CmdFreak service has stopped."));

	//3148 (NC6)
	pImpl->Jpn(CMDFRK_LOG_DESC, _T("以下にトレースログが表示されます。<br>表示される時刻はWebサーバーが配置された地域の時刻となります。"));
	pImpl->Eng(CMDFRK_LOG_DESC, _T("The trace log is displayed below.<br>Times are as per the time zone of the web server\\'s location."));

	//3149 (NC6)
	pImpl->Jpn(CMDFRK_TABLEINFO, _T("以下に指定されたテーブルの詳細情報を表示します。"));
	pImpl->Eng(CMDFRK_TABLEINFO, _T("Detailed information on the specified table is shown below."));

	//3150 (---)
	pImpl->Jpn(CMDFRK_TARGETTABLE, _T("対象テーブル: "));
	pImpl->Eng(CMDFRK_TARGETTABLE, _T("The target table: "));

	//3154 (NC6)
	pImpl->Jpn(CMDFRK_COMPOINFO2, _T("このソフトウェアは次の技術を取り込んでいます。<br/>- Mongoose 3.7 (Released as MIT License)<br/>- JQuery 1.9.1 (Released as MIT License)<br/>- JQuery UI 1.10.3 (Released as MIT License)<br/>- ParamQuery Grid 1.1.2 (Released as MIT License)<br/>"));
	pImpl->Eng(CMDFRK_COMPOINFO2, _T("This software has the following technologies embedded:<br/>- Mongoose 3.7 (Released as MIT License)<br/>- JQuery 1.9.1 (Released as MIT License)<br/>- JQuery UI 1.10.3 (Released as MIT License)<br/>- ParamQuery Grid 1.1.2 (Released as MIT License)<br/>"));

	//3200 (---)
	pImpl->Jpn(CMDFRK_DBMS, _T("DBMSの選択 : "));
	pImpl->Eng(CMDFRK_DBMS, _T("Select DBMS : "));

	//3201 (---)
	pImpl->Jpn(CMDFRK_CONNSTR, _T("接続文字列の指定 : "));
	pImpl->Eng(CMDFRK_CONNSTR, _T("Specify connection string : "));

	//3202 (---)
	pImpl->Jpn(CMDFRK_MANUAL, _T("manual/jpn/index.htm"));
	pImpl->Eng(CMDFRK_MANUAL, _T("manual/eng/index.htm"));

	//3203 (---)
	pImpl->Jpn(CMDFRK_MANUALMSG, _T("オンラインマニュアルを開く"));
	pImpl->Eng(CMDFRK_MANUALMSG, _T("Open online manual"));

	//3210 (---)
	pImpl->Jpn(CMDFRK_COMPOINFO, _T("CmdFreak Version 1.0.0<br/>Copyright (C) 2014 Shinya Takeuchi<br/>All Rights Reserved."));
	pImpl->Eng(CMDFRK_COMPOINFO, _T("CmdFreak Version 1.0.0<br/>Copyright (C) 2014 Shinya Takeuchi<br/>All Rights Reserved."));

	//3226 (---)
	pImpl->Jpn(CMDFRK_LOGGINGTIME, _T("ログ出力時刻"));
	pImpl->Eng(CMDFRK_LOGGINGTIME, _T("Logging time"));

	//3227 (---)
	pImpl->Jpn(CMDFRK_LOGGINGMSG, _T("メッセージ"));
	pImpl->Eng(CMDFRK_LOGGINGMSG, _T("Message"));

	//3233 (---)
	pImpl->Jpn(CMDFRK_UNSPEC, _T("未指定"));
	pImpl->Eng(CMDFRK_UNSPEC, _T("Unspecified"));

	//3234 (---)
	pImpl->Jpn(CMDFRK_FILTERING, _T("フィルタリング"));
	pImpl->Eng(CMDFRK_FILTERING, _T("Filtering"));

	//3235 (---)
	pImpl->Jpn(CMDFRK_ODBCCONNECTIONS, _T("ODBC接続"));
	pImpl->Eng(CMDFRK_ODBCCONNECTIONS, _T("ODBC Connections"));

	//3236 (---)
	pImpl->Jpn(CMDFRK_TABLES, _T("テーブル情報"));
	pImpl->Eng(CMDFRK_TABLES, _T("Table Information"));

	//3237 (---)
	pImpl->Jpn(CMDFRK_CREATERECORD, _T("レコード生成"));
	pImpl->Eng(CMDFRK_CREATERECORD, _T("Create Record"));

	//3238 (---)
	pImpl->Jpn(CMDFRK_EDITRECORD, _T("レコード編集"));
	pImpl->Eng(CMDFRK_EDITRECORD, _T("Edit Record"));

	//3239 (---)
	pImpl->Jpn(CMDFRK_DELETERECORD, _T("レコード削除"));
	pImpl->Eng(CMDFRK_DELETERECORD, _T("Delete Record"));

	//3240 (---)
	pImpl->Jpn(CMDFRK_INFORMATION, _T("情報"));
	pImpl->Eng(CMDFRK_INFORMATION, _T("Information"));

	//3241 (---)
	pImpl->Jpn(CMDFRK_REFRESHTABLELIST, _T("テーブル一覧更新"));
	pImpl->Eng(CMDFRK_REFRESHTABLELIST, _T("Refresh Table List"));

}

MessageProc::~MessageProc()
{
	delete pImpl;
}

void MessageProc::SetLocaleMode(int SpecifiedMode)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	Instance->pImpl->Mode = SpecifiedMode;
}

TCHAR* MessageProc::GetMsg(int Id)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	return Instance->pImpl->StkMsg[Id][Instance->pImpl->GetLocale()];
}

TCHAR* MessageProc::GetMsgEng(int Id)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	return Instance->pImpl->StkMsg[Id][MessageProc::MLANG_ENGLISH];
}

TCHAR* MessageProc::GetMsgJpn(int Id)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	return Instance->pImpl->StkMsg[Id][MessageProc::MLANG_JAPANESE];
}

BYTE* MessageProc::GetMsgSjis(int Id)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	return Instance->pImpl->StkMsgSjis[Id][Instance->pImpl->GetLocale()];
}

BYTE* MessageProc::GetMsgSjisEng(int Id)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	return Instance->pImpl->StkMsgSjis[Id][MessageProc::MLANG_ENGLISH];
}

BYTE* MessageProc::GetMsgSjisJpn(int Id)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	return Instance->pImpl->StkMsgSjis[Id][MessageProc::MLANG_JAPANESE];
}

void MessageProc::StkErr(int Id, HWND WndHndl)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	TCHAR Buf[32];
	wsprintf(Buf, _T("Message ID : %d"), Id);
	MessageBox(WndHndl, GetMsg(Id), Buf, MB_OK | MB_ICONSTOP);
}

void MessageProc::StkErr(int Id, TCHAR* Str, HWND WndHndl)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	TCHAR Buf[32];
	wsprintf(Buf, _T("Message ID : %d"), Id);
	TCHAR MsgBuf[1024];
	wsprintf(MsgBuf, _T("%s\r\n[%s]"),  GetMsg(Id), Str);
	MessageBox(WndHndl, MsgBuf, Buf, MB_OK | MB_ICONSTOP);
}

void MessageProc::StkInf(int Id, HWND WndHndl)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	TCHAR Buf[32];
	wsprintf(Buf, _T("Message ID : %d"), Id);
	MessageBox(WndHndl, GetMsg(Id), Buf, MB_OK | MB_ICONINFORMATION);
}

void MessageProc::StkInf(int Id, TCHAR* Str, HWND WndHndl)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	TCHAR Buf[32];
	wsprintf(Buf, _T("Message ID : %d"), Id);
	TCHAR MsgBuf[1024];
	wsprintf(MsgBuf, _T("%s\r\n[%s]"),  GetMsg(Id), Str);
	MessageBox(WndHndl, MsgBuf, Buf, MB_OK | MB_ICONINFORMATION);
}

int MessageProc::StkYesNo(int Id, HWND WndHndl)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	TCHAR Buf[32];
	wsprintf(Buf, _T("Message ID : %d"), Id);
	int Ret = MessageBox(WndHndl, GetMsg(Id), Buf, MB_YESNO | MB_ICONQUESTION);
	return Ret;
}

int MessageProc::StkYesNo(int Id, TCHAR* Str, HWND WndHndl)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	TCHAR Buf[32];
	wsprintf(Buf, _T("Message ID : %d"), Id);
	TCHAR MsgBuf[1024];
	wsprintf(MsgBuf, _T("%s\r\n[%s]"),  GetMsg(Id), Str);
	int Ret = MessageBox(WndHndl, MsgBuf, Buf, MB_YESNO | MB_ICONQUESTION);
	return Ret;
}

void MessageProc::AddEng(int Id, TCHAR* Msg)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	Instance->pImpl->Eng(Id, Msg);
}

void MessageProc::AddJpn(int Id, TCHAR* Msg)
{
	if (Instance == NULL) {
		Instance = new MessageProc();
	}
	Instance->pImpl->Jpn(Id, Msg);
}

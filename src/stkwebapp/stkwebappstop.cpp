#include "../stkpl/StkPl.h"
#include "../commonfunc/StkProperties.h"
#include "../commonfunc/StkStringParser.h"
#include "../stksocket/stksocket.h"

void StopProcess(const wchar_t* ConfFileName)
{
	/***** Stop StkWebApp *****/
	char IpAddrTmp[256] = "";
	wchar_t IpAddr[256] = L"";
	int Port = 0;
	StkProperties *Prop = new StkProperties();
	if (Prop->GetProperties(ConfFileName) == 0) {
		Prop->GetPropertyStr("servicehost", IpAddrTmp);
		StkPlConvUtf8ToWideChar(IpAddr, 256, IpAddrTmp);
		Prop->GetPropertyInt("serviceport", &Port);
		StkPlPrintf("servicehost=%s, serviceport=%d\r\n", IpAddrTmp, Port);
	} else {
		StkPlPrintf("Configuration file is not found.\r\n");
		StkPlExit(-1);
	}
	delete Prop;

	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, IpAddr, Port);
	if (StkSocket_Connect(1) == 0) {
		char SendDat[1024];
		char Dat[256] = "{ \"Operation\" : \"Stop\" }";
		unsigned char RecvDat[1024];
		StkPlSPrintf(SendDat, 1024, "POST /service/ HTTP/1.1\nContent-Length: %d\nContent-Type: application/json\n\n%s", StkPlStrLen(Dat), Dat);
		StkSocket_Send(1, 1, (unsigned char*)SendDat, StkPlStrLen((char*)SendDat));
		StkPlPrintf("-----\r\n%s\r\n", SendDat);
		int RetR;
		for (int Loop = 0; Loop < 10; Loop++) {
			RetR = StkSocket_Receive(1, 1, RecvDat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 5000, NULL, -1);
			if (RetR > 0) {
				StkPlPrintf("-----\r\n%s\r\n", RecvDat);
				break;
			}
		}
		StkSocket_Disconnect(1, 1, true);
	} else {
		int Log = 0;
		int LogId = 0;
		wchar_t ParamStr1[256] = L"";
		wchar_t ParamStr2[256] = L"";
		int ParamInt1 = 0;
		int ParamInt2 = 0;
		StkSocket_TakeLastLog(&Log, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		StkPlPrintf("Connection failure %d, %d\r\n", Log, ParamInt2);
	}
	StkSocket_DeleteInfo(1);
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		StkPlPrintf("Usage %s <configuration file name>\r\n", argv[0]);
		return -1;
	}
	wchar_t* ConfFileName = StkPlCreateWideCharFromUtf8(argv[1]);
	StopProcess(ConfFileName);
	delete ConfFileName;

	return 0;
}

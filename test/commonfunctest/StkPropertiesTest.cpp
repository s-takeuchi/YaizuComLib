#include "../../src/StkPl.h"
#include "../../src/commonfunc/StkProperties.h"

void StkPropertiesTest()
{
	StkProperties StkProp;
	int Ret;

	StkPlPrintf("StkPropertiesTest started.\n");

	StkPlPrintf("Program is loading property file \"propertytest1.prop\" (zero byte data) ...");
	Ret = StkProp.GetProperties(L"propertytest1.prop");
	if (Ret == 0) {
		StkPlPrintf("Success\n");
	} else {
		StkPlPrintf("Failure\n");
		StkPlExit(0);
	}

	StkPlPrintf("Program is loading property file \"propertytest2.prop\"...");
	Ret = StkProp.GetProperties(L"propertytest2.prop");
	if (Ret == 0) {
		StkPlPrintf("Success\n");
	} else {
		StkPlPrintf("Failure\n");
		StkPlExit(0);
	}

	StkPlPrintf("Program is loading property file \"propertytest3.prop\"...");
	Ret = StkProp.GetProperties(L"propertytest3.prop");
	if (Ret == 0) {
		StkPlPrintf("Success\n");
	} else {
		StkPlPrintf("Failure\n");
		StkPlExit(0);
	}

	StkPlPrintf("Program trys to load unexisting property file \"propertytest4.prop\"...");
	Ret = StkProp.GetProperties(L"propertytest4.prop");
	if (Ret == -1) {
		StkPlPrintf("Success\n");
	} else {
		StkPlPrintf("Failure\n");
		StkPlExit(0);
	}

	char ValueStr[256];
	int ValueInt;

	StkProp.GetPropertyInt("prop01", &ValueInt);
	StkPlPrintf("prop01=[123]?...");
	if (ValueInt == 123) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		StkPlExit(0);
	}

	StkProp.GetPropertyStr("prop02", ValueStr);
	StkPlPrintf("prop02=[abc]?...");
	if (StkPlStrCmp(ValueStr, "abc") == 0) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		StkPlExit(0);
	}

	StkProp.GetPropertyInt("prop03", &ValueInt);
	StkPlPrintf("prop03=[456]?...");
	if (ValueInt == 456) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		StkPlExit(0);
	}

	StkProp.GetPropertyStr("prop04", ValueStr);
	StkPlPrintf("prop04=[def]?...");
	if (StkPlStrCmp(ValueStr, "def") == 0) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		StkPlExit(0);
	}

	StkProp.GetPropertyInt("prop07", &ValueInt);
	StkPlPrintf("prop07=[-123]?...");
	if (ValueInt == -123) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		StkPlExit(0);
	}

	StkProp.GetPropertyStr("myname", ValueStr);
	StkPlPrintf("myname=[Shinya Takeuchi]?...");
	if (StkPlStrCmp(ValueStr, "Shinya Takeuchi") == 0) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		StkPlExit(0);
	}

	Ret = StkProp.GetPropertyStr("equal1", ValueStr);
	StkPlPrintf("equal1=[ppp=ppp]?...");
	if (Ret == 0 && StkPlStrCmp(ValueStr, "ppp=ppp") == 0) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		StkPlExit(0);
	}

	Ret = StkProp.GetPropertyStr("equal2", ValueStr);
	StkPlPrintf("equal2=[ppp = ppp]?...");
	if (Ret == 0 && StkPlStrCmp(ValueStr, "ppp = ppp") == 0) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		StkPlExit(0);
	}

	Ret = StkProp.GetPropertyStr("equal3", ValueStr);
	StkPlPrintf("equal3=unexisting?...");
	if (Ret == -1) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		StkPlExit(0);
	}

	StkPlPrintf("StkPropertiesTest completed.\n\n\n");
}

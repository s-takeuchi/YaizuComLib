#include <windows.h>
#include <stdio.h>
#include "..\..\src\commonfunc\StkProperties.h"

void StkPropertiesTest()
{
	StkProperties StkProp;
	int Ret;

	printf("StkPropertiesTest started.\n");

	printf("Program is loading property file \"propertytest1.prop\" (zero byte data) ...");
	Ret = StkProp.GetProperties(L"propertytest1.prop");
	if (Ret == 0) {
		printf("Success\n");
	} else {
		printf("Failure\n");
		exit(0);
	}

	printf("Program is loading property file \"propertytest2.prop\"...");
	Ret = StkProp.GetProperties(L"propertytest2.prop");
	if (Ret == 0) {
		printf("Success\n");
	} else {
		printf("Failure\n");
		exit(0);
	}

	printf("Program is loading property file \"propertytest3.prop\"...");
	Ret = StkProp.GetProperties(L"propertytest3.prop");
	if (Ret == 0) {
		printf("Success\n");
	} else {
		printf("Failure\n");
		exit(0);
	}

	printf("Program trys to load unexisting property file \"propertytest4.prop\"...");
	Ret = StkProp.GetProperties(L"propertytest4.prop");
	if (Ret == -1) {
		printf("Success\n");
	} else {
		printf("Failure\n");
		exit(0);
	}

	char ValueStr[256];
	int ValueInt;

	StkProp.GetPropertyInt("prop01", &ValueInt);
	printf("prop01=[123]?...");
	if (ValueInt == 123) {
		printf("OK\n");
	} else {
		printf("NG\n");
		exit(0);
	}

	StkProp.GetPropertyStr("prop02", ValueStr);
	printf("prop02=[abc]?...");
	if (strcmp(ValueStr, "abc") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
		exit(0);
	}

	StkProp.GetPropertyInt("prop03", &ValueInt);
	printf("prop03=[456]?...");
	if (ValueInt == 456) {
		printf("OK\n");
	} else {
		printf("NG\n");
		exit(0);
	}

	StkProp.GetPropertyStr("prop04", ValueStr);
	printf("prop04=[def]?...");
	if (strcmp(ValueStr, "def") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
		exit(0);
	}

	StkProp.GetPropertyInt("prop07", &ValueInt);
	printf("prop07=[-123]?...");
	if (ValueInt == -123) {
		printf("OK\n");
	} else {
		printf("NG\n");
		exit(0);
	}

	StkProp.GetPropertyStr("myname", ValueStr);
	printf("myname=[Shinya Takeuchi]?...");
	if (strcmp(ValueStr, "Shinya Takeuchi") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
		exit(0);
	}

	Ret = StkProp.GetPropertyStr("equal1", ValueStr);
	printf("equal1=[ppp=ppp]?...");
	if (Ret == 0 && strcmp(ValueStr, "ppp=ppp") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
		exit(0);
	}

	Ret = StkProp.GetPropertyStr("equal2", ValueStr);
	printf("equal2=[ppp = ppp]?...");
	if (Ret == 0 && strcmp(ValueStr, "ppp = ppp") == 0) {
		printf("OK\n");
	} else {
		printf("NG\n");
		exit(0);
	}

	Ret = StkProp.GetPropertyStr("equal3", ValueStr);
	printf("equal3=unexisting?...");
	if (Ret == -1) {
		printf("OK\n");
	} else {
		printf("NG\n");
		exit(0);
	}

	printf("StkPropertiesTest completed.\n\n\n");
}

#include "../../src/stkpl/StkPl.h"

void StkPropertiesTest();
void MsgProcTest1();
void MsgProcTest2();
void StkObjectTest();
int StkStringParserTest();

int main(int argc, char *argv[])
{
	StkStringParserTest();
	StkObjectTest();
	StkPropertiesTest();

	StkPlPrintf("MsgProcTest started.\n");
	MsgProcTest1();
	MsgProcTest2();
	StkPlPrintf("MsgProcTest completed.\n\n\n");

	return 0;
}

#include <windows.h>
#pragma data_seg(".mydata")

	int OperationType = 0;
	int TargetProcess = 0;

#pragma data_seg()

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

__declspec(dllexport) void __stdcall DllShowProperty(int Id)
{
}

__declspec(dllexport) int __stdcall DllStkThreadInit(int Id)
{
	return -222;
}

__declspec(dllexport) int __stdcall DllStkThreadFinal(int Id)
{
	return -333;
}

__declspec(dllexport) int __stdcall DllStkThreadMain(int Id)
{
	return -111;
}

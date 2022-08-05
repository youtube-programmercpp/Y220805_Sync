#include <Windows.h>
#include <stdio.h>
#include "StopFlag.h"
int main()
{
	puts("始めます。");
	if (const auto hEvent = CreateEventW(nullptr, true, false, _CRT_WIDE(StopFlag))) {
		WaitForSingleObject(hEvent, INFINITE);
		CloseHandle(hEvent);
	}
	puts("終わります。");
}

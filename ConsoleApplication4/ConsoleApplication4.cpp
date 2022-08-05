#include <Windows.h>
#include <stdio.h>
int main()
{
	if (const auto hEvent = CreateEventW(nullptr, true, false, L"StopFlag")) {
		const auto e = GetLastError();//現スレッドにおいての直近の Win32 エラー
		if (e == ERROR_ALREADY_EXISTS) {
			//別のプロセスが待機状態にあると看做せる
			(void)SetEvent(hEvent);
		}
		else {
			puts("始めます。");
			(void)WaitForSingleObject(hEvent, INFINITE);
			puts("終わります。");
		}
		(void)CloseHandle(hEvent);
	}
}

#include <Windows.h>
#include <stdio.h>
#include <memory>
#define	StopFlag1	_CRT_STRINGIZE(StopFlag1)
#define	StopFlag2	_CRT_STRINGIZE(StopFlag2)
int main()
{
	const std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*> hEvent1
	{ CreateEventA(nullptr, true, false, StopFlag1)
	, CloseHandle
	};
	const std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*> hEvent2
	{ CreateEventA(nullptr, true, false, StopFlag1)
	, CloseHandle
	};
	if (hEvent1 && hEvent2) {
		const HANDLE rghEvents[] =
		{ hEvent1.get()
		, hEvent2.get()
		};
		puts("始めます。");
		WaitForMultipleObjects(_countof(rghEvents), rghEvents, false, INFINITE);
		puts("終わります。");
	}
}

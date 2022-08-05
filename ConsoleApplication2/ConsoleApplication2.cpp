#include <Windows.h>
#include <stdio.h>
#include <memory>
int main(int argc, char** argv)
{
	if (argc == 2) {
		if (const std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*> hEvent
		{ OpenEventA(MAXIMUM_ALLOWED, false, argv[1])
		, CloseHandle
		}) {
			if (SetEvent(hEvent.get())) {
				//成功
				return EXIT_SUCCESS;
			}
			else {
				//失敗
				fputs("SetEventが失敗しました。", stderr);
				return EXIT_FAILURE;
			}
		}
		else {
			fputs("イベントの生成に失敗しました。", stderr);
			return EXIT_FAILURE;
		}
	}
	else {
		fputs("イベント名をコマンドライン引数に指定してください。", stderr);
		return EXIT_FAILURE;
	}
}

// C++サンプルプログラム：通知を受けてプログラムを起動する Copyright(C) 2022 https://www.youtube.com/@ProgrammerCpp
// ～試し方～
// WindowsProject1/WindowsProject1.cpp 内で説明していますのでそちらの方をご覧ください。

#include <windows.h>
#include <sstream>
#include <array>
#include <memory>
#include <stdexcept>
#include "Notification.h"

//
//	例外オブジェクト(std::runtime_errorクラス)を作る
//
auto Win32エラーオブジェクトを作る_(DWORD dwLastError, const char* function, const char* file, int line)
{
	return std::runtime_error{ (std::ostringstream() << file << "(" << line << "): " << function << " 関数がエラーコード " << dwLastError << " で失敗しました。").str() };
}
#define	Win32エラーオブジェクトを作る(e, function) Win32エラーオブジェクトを作る_(e, function, __FILE__, __LINE__)

//
//	通知オブジェクトを作る
//
auto 通知を作る(_In_ LPCSTR 通知の名前)
{
	const auto hEvent = CreateEventA
	( /*_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes*/nullptr
	, /*_In_     BOOL                  bManualReset     */false
	, /*_In_     BOOL                  bInitialState    */false
	, /*_In_opt_ LPCSTR                lpName           */通知の名前
	);
	const auto e = GetLastError();
	if (hEvent) {
		if (e == ERROR_ALREADY_EXISTS) {
			CloseHandle(hEvent);
			throw Win32エラーオブジェクトを作る(e, "CreateEventA");
		}
		else {
			return std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*>
			{ hEvent
			, CloseHandle
			};
		}
	}
	else
		throw Win32エラーオブジェクトを作る(e, "CreateEventA");
}
#include <iostream>
int main()
{
	try {
		const auto メモ帳起動依頼通知       = 通知を作る(ConsoleApplication6_Notification_メモ帳起動依頼通知      );
		const auto 当プログラム終了依頼通知 = 通知を作る(ConsoleApplication6_Notification_ConsoleApplication6終了依頼通知);
		const HANDLE handles[] =
		{ /*[0]*/メモ帳起動依頼通知      .get()
		, /*[1]*/当プログラム終了依頼通知.get()
		};
		for (;;) {
			//通知の到来を待つ
			switch (const auto dwResult = WaitForMultipleObjects
			( /*_In_               DWORD          nCount        */_countof(handles)
			, /*_In_reads_(nCount) CONST HANDLE * lpHandles     */handles
			, /*_In_               BOOL           bWaitAll      */false
			, /*_In_               DWORD          dwMilliseconds*/INFINITE
			)) {
			case WAIT_OBJECT_0 + 0://handles[0] メモ帳起動依頼通知
				//メモ帳起動依頼通知が到来したのでメモ帳を起動する。
				PROCESS_INFORMATION procinfo;
				if (CreateProcessW
				( /*_In_opt_    LPCWSTR               lpApplicationName   */nullptr
				, /*_Inout_opt_ LPWSTR                lpCommandLine       */&std::wstring(L"notepad").front()
				, /*_In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes */nullptr
				, /*_In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes  */nullptr
				, /*_In_        BOOL                  bInheritHandles     */false
				, /*_In_        DWORD                 dwCreationFlags     */0
				, /*_In_opt_    LPVOID                lpEnvironment       */nullptr
				, /*_In_opt_    LPCWSTR               lpCurrentDirectory  */nullptr
				, /*_In_        LPSTARTUPINFOW        lpStartupInfo       */std::array<STARTUPINFOW, 1>{sizeof(STARTUPINFOW)}.data()
				, /*_Out_       LPPROCESS_INFORMATION lpProcessInformation*/& procinfo
				))  {
					(void)CloseHandle(procinfo.hThread );
					(void)CloseHandle(procinfo.hProcess);
					continue;
				}
				else
					throw Win32エラーオブジェクトを作る(GetLastError(), "CreateProcessW");
			case WAIT_OBJECT_0 + 1://handles[1] 当プログラム終了依頼通知
				//当プログラムに対する終了依頼通知が到来したので、これで終了
				return EXIT_SUCCESS;
			case WAIT_FAILED:
				throw Win32エラーオブジェクトを作る(GetLastError(), "WaitForMultipleObjects");
			default:
				throw std::runtime_error((std::ostringstream() << __FILE__ "(" _CRT_STRINGIZE(__LINE__) "): WaitForMultipleObjects 関数が予期しない値 " << dwResult << "を返しました。").str());
			}
		}
	}
	catch (const std::exception& e) {
#ifdef _DEBUG
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
#endif
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
/*
https://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q11270673286
1151375515さん

2022/11/7 17:59

1回答

C＋＋の質問です。
アプリケーションから外部アプリケーションを起動する際、受信側のアプリはどのような通知を待ってEXEファイルを起動できますか？
ソースを教えていただけると嬉しいです。

C言語関連・5閲覧
*/

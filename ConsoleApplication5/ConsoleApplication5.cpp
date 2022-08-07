#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include <process.h>
#include <memory>

class Thread
{
	HANDLE hNamedPipe;
	HANDLE hStopEvent;
	HWND   hDlg      ;

	unsigned ThreadProc()
	{
		if (const std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*> hEvent
		{ CreateEventW(nullptr, true, false, nullptr)
		, CloseHandle
		}) {
			OVERLAPPED ov
			{ /*ULONG_PTR Internal      ;*/0
			, /*ULONG_PTR InternalHigh  ;*/0
			, /*union {}  DUMMYUNIONNAME;*/{}
			, /*HANDLE    hEvent        ;*/hEvent.get()
			};

			//
			//	クライアントからの接続を受け付ける
			//


			if (/*WINBASEAPI BOOL WINAPI*/ConnectNamedPipe
			( /*_In_        HANDLE       hNamedPipe  */hNamedPipe
			, /*_Inout_opt_ LPOVERLAPPED lpOverlapped*/&ov
			)) {
				PostMessageW(hDlg, WM_COMMAND, IDOK, 0);
				return EXIT_SUCCESS;
			}
			else {
				const auto e = GetLastError();
				if (e == ERROR_IO_PENDING) {
					const HANDLE handles[] =
					{ /*[0]*/hEvent.get() //接続できたイベント
					, /*[1]*/hStopEvent   //キャンセルボタンが押された
					};
					switch (const auto dwResult = WaitForMultipleObjects(_countof(handles), handles, false, INFINITE)) {
					case WAIT_OBJECT_0 + 0: PostMessageW(hDlg, WM_COMMAND, IDOK   , 0); return EXIT_SUCCESS;
					case WAIT_OBJECT_0 + 1:                                             return EXIT_SUCCESS;
					case WAIT_FAILED      :
					default               :
						PostMessageW(hDlg, WM_COMMAND, IDABORT, 0);
						return EXIT_FAILURE;
					}
				}
				else
					return EXIT_FAILURE;
			}
		}
		else
			return EXIT_FAILURE;
	}
public:
	Thread
	( HANDLE hNamedPipe
	, HANDLE hStopEvent
	, HWND   hDlg      
	) noexcept
		: /*HANDLE*/hNamedPipe{hNamedPipe}
		, /*HANDLE*/hStopEvent{hStopEvent}
		, /*HWND  */hDlg      {hDlg      }
	{
	}
	std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*> Run()
	{
		return std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*>
		{ HANDLE(/*_Success_(return != 0) _ACRTIMP uintptr_t __cdecl*/_beginthreadex
			( /*_In_opt_  void                   * _Security    */nullptr
			, /*_In_      unsigned                 _StackSize   */0
			, /*_In_      _beginthreadex_proc_type _StartAddress*/[](void* pvThis)->unsigned
				{
					//
					//	この関数が終わると pvThis が指す Thread オブジェクトは delete される
					//
					return std::unique_ptr<Thread>(static_cast<Thread*>(pvThis))->ThreadProc();
				}
			, /*_In_opt_  void                   * _ArgList     */this
			, /*_In_      unsigned                 _InitFlag    */0
			, /*_Out_opt_ unsigned               * _ThrdAddr    */nullptr
			))
		, CloseHandle
		};
	}
};
class DlgBox {
	HANDLE hNamedPipe;
	std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*> hStopEvent;
	std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*> hThread   ;
	static DlgBox* FromHandle(HWND hDlg) noexcept
	{
		return reinterpret_cast<DlgBox*>(GetWindowLongPtrW(hDlg, DWLP_USER));
	}
	INT_PTR OnInitDialog(HWND hDlg)
	{
		auto background_thread = std::make_unique<Thread>
		( /*HANDLE hNamedPipe*/hNamedPipe
		, /*HANDLE hStopEvent*/hStopEvent.get()
		, /*HWND   hDlg      */hDlg      
		);
		if (auto hThread = background_thread->Run()) {
			this->hThread = std::move(hThread);
			(void)background_thread.release();

		}
		else
			EndDialog(hDlg, IDCANCEL);
		return true;
	}
public:
	DlgBox
	( HANDLE hNamedPipe
	, std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*>&& hStopEvent
	) noexcept
		: /*HANDLE                                                                */hNamedPipe {           hNamedPipe   }
		, /*std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*>*/hStopEvent { std::move(hStopEvent ) }
		, /*std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*>*/hThread    { nullptr, CloseHandle   }
	{
	}
	INT_PTR ConnectNamedPipe
	( _In_opt_ HWND      hWndParent    
	)
	{
		const auto result = /*WINUSERAPI INT_PTR WINAPI*/DialogBoxParamW
		( /*_In_opt_ HINSTANCE hInstance     */nullptr
		, /*_In_     LPCWSTR   lpTemplateName*/MAKEINTRESOURCEW(IDD_DIALOG_ConnectNamedPipe)
		, /*_In_opt_ HWND      hWndParent    */hWndParent
		, /*_In_opt_ DLGPROC   lpDialogFunc  */[](HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)->INT_PTR
			{
				switch (message) {
				case WM_INITDIALOG:
					SetWindowLongPtrW(hDlg, DWLP_USER, lParam);
					return reinterpret_cast<DlgBox*>(lParam)->OnInitDialog(hDlg);
				case WM_COMMAND   :
					switch (LOWORD(wParam)) {
					case IDCANCEL:
						(void)SetEvent(FromHandle(hDlg)->hStopEvent.get());
						EndDialog(hDlg, IDCANCEL);
						return true;
					case IDOK:
						EndDialog(hDlg, IDOK);
						return true;
					case IDABORT:
						EndDialog(hDlg, IDABORT);
						return true;
					default:
						return false;
					}
				default:
					return false;
				}
			}
		, /*_In_     LPARAM    dwInitParam   */LPARAM(this)
		);
		if (hThread)
			(void)WaitForSingleObject(hThread.get(), INFINITE);
		return result;
	}
};
int main()
{
	NULL   ;//C言語用、古いC++用(C++ では実は整数の 0 だった…)
	nullptr;//C++用


	const auto hNamedPipe = /*WINBASEAPI HANDLE WINAPI*/CreateNamedPipeW
	( /*_In_     LPCWSTR                lpName              */LR"(\\.\pipe\test)"
	, /*_In_     DWORD                  dwOpenMode          */PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED
	, /*_In_     DWORD                  dwPipeMode          */0
	, /*_In_     DWORD                  nMaxInstances       */1
	, /*_In_     DWORD                  nOutBufferSize      */0
	, /*_In_     DWORD                  nInBufferSize       */0
	, /*_In_     DWORD                  nDefaultTimeOut     */NMPWAIT_USE_DEFAULT_WAIT
	, /*_In_opt_ LPSECURITY_ATTRIBUTES  lpSecurityAttributes*/nullptr
	);
	if (hNamedPipe != INVALID_HANDLE_VALUE) {
		if (std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(CloseHandle)*> hStopEvent
		{ CreateEventW(nullptr, true, false, nullptr)
		, CloseHandle
		}) {
			const auto result = DlgBox(hNamedPipe, std::move(hStopEvent)).ConnectNamedPipe(nullptr);
			if (result == IDOK) {
				for (;;) {
					char buf[256];
					DWORD cbRead;
					if (ReadFile(hNamedPipe, buf, sizeof buf, &cbRead, nullptr)) {
						if (cbRead)
							fwrite(buf, cbRead, 1, stdout);
						else
							break;
					}
					else
						break;
				}
			}
		}
		CloseHandle(hNamedPipe);
	}
}

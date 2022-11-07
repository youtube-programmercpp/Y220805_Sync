// C++サンプルプログラム：通知イベント発行 Copyright(C) 2022 https://www.youtube.com/@ProgrammerCpp
// ～試し方～
// ConsoleApplication6.exeを先に起動しておきます。
// 当プログラム(WindowsProject1.exe)を起動します。
// 「メモ帳起動依頼通知を発行する」ボタンを押します。するとConsoleApplication6からメモ帳が起動されます。
// 「ConsoleApplication6 終了依頼通知を発行する」ボタンをおします。するとConsoleApplication6は終わります。

#include <Windows.h>
#include "Resource.h"
#include "../ConsoleApplication6/Notification.h"

void 通知を発行する(_In_ HWND hWnd, _In_ LPCSTR lpName)
{
	if (const auto hEvent = OpenEventA
	( /*_In_ DWORD   dwDesiredAccess*/MAXIMUM_ALLOWED
	, /*_In_ BOOL    bInheritHandle */false
	, /*_In_ LPCSTR  lpName         */lpName
	)) {
		const auto result = SetEvent(hEvent);
		(void)CloseHandle(hEvent);
		if (!result)
			MessageBoxA(hWnd, "通知イベントを発行することができませんでした。", lpName, MB_ICONSTOP);
	}
	else
		MessageBoxA(hWnd, "通知イベントを開くことができませんでした。", lpName, MB_ICONSTOP);
}
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_ExecNotepad                 :
			通知を発行する(hDlg, ConsoleApplication6_Notification_メモ帳起動依頼通知);
			return true;
		case IDC_BUTTON_TerminateConsoleApplication6:
			通知を発行する(hDlg, ConsoleApplication6_Notification_ConsoleApplication6終了依頼通知);
			return true;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return true;
		default:
			return false;
		}
	default:
		return false;
	}
}
int APIENTRY wWinMain
( _In_     HINSTANCE hInstance    
, _In_opt_ HINSTANCE hPrevInstance
, _In_     LPWSTR    lpCmdLine    
, _In_     int       nCmdShow     
)
{
	(void)DialogBoxW
	( /*hInstance   */hInstance
	, /*lpTemplate  */MAKEINTRESOURCEW(IDD_DIALOG1)
	, /*hWndParent  */nullptr
	, /*lpDialogFunc*/DialogProc
	);
}

/*
	about.c

	'about' dialog box
*/

#include "../dialog_boxes.h"


/* Message handler for the about dialog box. */
INT_PTR CALLBACK AboutProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
				default:
					EndDialog(hWnd, LOWORD(wParam));
					SetFocus(hWnd_focused);
					return TRUE;
			}
	}
	return FALSE;
}


bool about_dialog_box (HWND hWnd)
{
	return (bool)DialogBox (main_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutProc);
	// if main_hInst = NULL then the icon will not load
}

/*
	print.c

	Links:
	Print Dialog Box: http://msdn.microsoft.com/en-us/library/windows/desktop/ms646964%28v=vs.85%29.aspx
	PRINTDLG structure: http://msdn.microsoft.com/en-us/library/windows/desktop/ms646843%28v=vs.85%29.aspx
	PrintDlg function: http://msdn.microsoft.com/en-us/library/windows/desktop/ms646940%28v=vs.85%29.aspx

	TODO: CODE NOT COMPLETE !
*/

#include "../dialog_boxes.h"

static PRINTDLG printDlg;

bool print_dialog_box (HWND hWnd)
{
	memset (&printDlg, 0, sizeof(printDlg));

	printDlg.lStructSize    = sizeof(printDlg);
	printDlg.hwndOwner      = hWnd;
	printDlg.Flags          = PD_ALLPAGES | PD_NOSELECTION;
	printDlg.nCopies        = 1;

	PrintDlg(&printDlg);
	return FALSE;
}

/*
	font.c

	Links:
	Font Dialog Box: http://msdn.microsoft.com/en-us/library/windows/desktop/ms646958%28v=vs.85%29.aspx
	CHOOSEFONT structure: http://msdn.microsoft.com/en-us/library/windows/desktop/ms646832%28v=vs.85%29.aspx
	ChooseFont function: http://msdn.microsoft.com/en-us/library/windows/desktop/ms646914%28v=vs.85%29.aspx
	LOGFONT structure: http://msdn.microsoft.com/en-us/library/windows/desktop/dd145037%28v=vs.85%29.aspx
*/

#include "font.h"

static HFONT hFont;
static LOGFONT logFont;
static CHOOSEFONT chooseFont;


static void font_set (BOOL redraw)
{
	hFont = CreateFontIndirect (&logFont);

	SendMessage (hWnd_main_text     , WM_SETFONT, (WPARAM)hFont, redraw);

	SendMessage (hWnd_eval_button   , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_mesg_text     , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_lock_button   , WM_SETFONT, (WPARAM)hFont, redraw);

	SendMessage (hWnd_prev_button   , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_next_button   , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_delete_button , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_clear_button  , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_path_text     , WM_SETFONT, (WPARAM)hFont, redraw);

	SendMessage (hWnd_pause_button  , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_forward_button, WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_lower_button  , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_higher_button , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_time_text     , WM_SETFONT, (WPARAM)hFont, redraw);

	SendMessage (hWnd_calc_button   , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_calc_input    , WM_SETFONT, (WPARAM)hFont, redraw);
	SendMessage (hWnd_calc_result   , WM_SETFONT, (WPARAM)hFont, redraw);

	SendMessage (hWnd_status_bar    , WM_SETFONT, (WPARAM)hFont, redraw);
}


// set initial font on text fields
void font_initialise ()
{
	logFont.lfHeight         = 0xFFFFFFF0;
	logFont.lfWidth          = 0;
	logFont.lfEscapement     = 0;
	logFont.lfOrientation    = 0;
	logFont.lfWeight         = 0x190;
	logFont.lfItalic         = false;
	logFont.lfUnderline      = false;
	logFont.lfStrikeOut      = false;
	logFont.lfCharSet        = ANSI_CHARSET;
	logFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality        = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
	strcpy22(logFont.lfFaceName, L"Liberation Mono");
	font_set(false);
}


// delete objects created by font
void font_remove ()
{
	DeleteObject(hFont);
}


// show dialog box and change font
bool font_dialog_box (HWND hWnd)
{
	memset (&chooseFont, 0, sizeof(chooseFont));

	chooseFont.lStructSize = sizeof(chooseFont);
	chooseFont.hwndOwner   = hWnd;
	chooseFont.lpLogFont   = &logFont;
	chooseFont.Flags       = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST;

	if(ChooseFont(&chooseFont))
	{
		font_set (true);
		return true;
	}
	else return false;
}


static int start, stop;
static const wchar* buffer = NULL;

static bool get_text (HWND hWnd)
{
	SendMessage (hWnd_focused, EM_GETSEL, (WPARAM)&start, (LPARAM)&stop);
	if(start==stop) { MessageBox (hWnd, L"No text selected." , L"Error", MB_OK); return false; }
	buffer = hWnd_get_text(hWnd_focused);
	strcpy22S(WCHAR(buffer), buffer+start, stop-start);
	return true;
}

static bool replace_text (HWND hWnd, value (*CALL) (value v))
{
	if(!get_text(hWnd)) return false;
	uint32_t stack[100000];
	CALL(setStr22(stack, buffer));
	const wchar* text = getStr2(vGet(stack));
	SendMessage (hWnd_focused, EM_REPLACESEL, true, (LPARAM)text);
	SendMessage (hWnd_focused, EM_SETSEL, start, start+strlen2(text));
	return true;
}

bool font_pcn_to_chr (HWND hWnd) { return replace_text(hWnd, pcn_to_chr); }
bool font_chr_to_pcn (HWND hWnd) { return replace_text(hWnd, chr_to_pcn); }
bool font_chr_to_fcn (HWND hWnd) { return replace_text(hWnd, chr_to_fcn); }
bool font_set_pif_cn (HWND hWnd)
{
	if(!get_text(hWnd)) return false;
	uint32_t stack[100000];
	if(!VERROR(set_pif_cn(setStr22(stack, buffer)))) return true;
	MessageBox (hWnd, getMessage(vGet(stack)), L"Error", MB_OK);
	return false;
}

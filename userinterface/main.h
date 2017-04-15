#ifndef _USER_MAIN_GUI_H
#define _USER_MAIN_GUI_H
/*
    main.h
*/
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <_string.h>

#define MAX_FILE_NAME 300

extern HINSTANCE main_hInst;
extern HWND hWnd_main_window;
extern HWND hWnd_main_text;

extern HWND hWnd_eval_button;
extern HWND hWnd_mesg_text;
extern HWND hWnd_lock_button;

extern HWND hWnd_prev_button;
extern HWND hWnd_next_button;
extern HWND hWnd_delete_button;
extern HWND hWnd_clear_button;
extern HWND hWnd_path_text;

extern HWND hWnd_pause_button;
extern HWND hWnd_forward_button;
extern HWND hWnd_lower_button;
extern HWND hWnd_higher_button;
extern HWND hWnd_time_text;

extern HWND hWnd_calc_button;
extern HWND hWnd_calc_input;
extern HWND hWnd_calc_result;

extern HWND hWnd_status_bar;
extern HWND hWnd_focused;

bool SetWindowTitle (HWND hWnd, const TCHAR* fileName, bool fileExists);

void get_caret_position (HWND hWnd_text, int *line_ptr, int *coln_ptr);

bool keyboard_input_event (HWND dw, UINT message, WPARAM wParam, LPARAM lParam);

static inline int hWnd_get_text (mchar** buffer_ptr, HWND hWnd)
{
    int length;
    if(buffer_ptr==NULL) return 0;
    length = 1+(int)SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
    *buffer_ptr = mchar_alloc (*buffer_ptr, length);
    SendMessage(hWnd, WM_GETTEXT, length+1, (LPARAM)(*buffer_ptr));
    (*buffer_ptr)[length]=0;
    return length;
}

#endif

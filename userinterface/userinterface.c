/*******************************************
    userinterface.c
*******************************************/

#include "main.h"
#include <files.h>
#include <_stdio.h>
#include <userinterface.h>
#include <timer.h>


void wait_for_user_first (const wchar* title, const wchar* message)
{ MessageBox(hWnd_main_window, message, title, MB_OK); }

bool wait_for_confirmation (const wchar* title, const wchar* message)
{ return MessageBox(hWnd_main_window, message, title, MB_OKCANCEL) == IDOK; }


static HWND get_hWnd_text (enum UI_ITEM ui_item)
{
    switch (ui_item)
    {
        case UI_MAIN_TEXT: return hWnd_main_text;
        case UI_MESG_TEXT: return hWnd_mesg_text;
        case UI_PATH_TEXT: return hWnd_path_text;
        case UI_TIME_TEXT: return hWnd_time_text;
        case UI_CALC_INPUT: return hWnd_calc_input;
        case UI_CALC_RESULT: return hWnd_calc_result;
        case UI_PAUSE_BUTTON: return hWnd_pause_button;
        case UI_FORWARD_BUTTON: return hWnd_forward_button;
        default: return NULL;
    }
}


void userinterface_set_text (enum UI_ITEM ui_item, const wchar* text)
{
    static bool check=false;
    HWND hWnd_text = get_hWnd_text(ui_item);
    if(hWnd_text==NULL) return;

    if(ui_item==UI_MAIN_TEXT)
    {
        if(get_file_name())
        {
            // first check if changes are minimal, if so then keep file opened
            //static wchar* buf = NULL; hWnd_get_text(&buf, hWnd_text);
            if(0==strcmp22(text, userinterface_get_text(UI_MAIN_TEXT))) return;

            // call to new_file() will come back here
            if(check) return; else check=true;
            if(!new_file()) { check=false; return; }
        }
        check=false;
    }
    SendMessage(hWnd_text, WM_SETTEXT, 0, (LPARAM)text);
}


const wchar* userinterface_get_text (enum UI_ITEM ui_item)
{
    HWND hWnd_text = get_hWnd_text(ui_item);
    if(hWnd_text==NULL) return NULL;
    return hWnd_get_text(hWnd_text);
}

void userinterface_clean () { hWnd_get_text(NULL); }


const wchar* hWnd_get_text (HWND hWnd)
{
    static wchar* buffer = NULL;
    if(!hWnd) { buffer = wchar_free(buffer); return NULL; }
    int length = 1+(int)SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
    buffer = wchar_alloc (buffer, length);
    SendMessage(hWnd, WM_GETTEXT, length+1, (LPARAM)buffer);
    buffer[length]=0;
    return buffer;
}


#define IDT_TIMER 0

void timer_pause_do ()
{
    KillTimer(hWnd_main_window, IDT_TIMER);
}

static void timer_handler (HWND hWnd, UINT message, UINT_PTR idEvent, DWORD dwTime)
{
    if(message !=  WM_TIMER) return;
    if(idEvent != IDT_TIMER) return;
    timer_handler_do();
}

void timer_set_period_do (int period)
{
    SetTimer(hWnd_main_window,
             IDT_TIMER,   // timer identifier
             period,      // interval in milliseconds
             (TIMERPROC) timer_handler);
}

/*******************************************
    userinterface.c
*******************************************/

#include "main.h"
#include "files.h"
#include <_stdio.h>
#include <userinterface.h>


void wait_for_user_first (const mchar* title, const mchar* message)
{ MessageBox (hWnd_main_window, message, title, MB_OK); }

bool wait_for_confirmation (const mchar* title, const mchar* message)
{ return (MessageBox (hWnd_main_window, message, title, MB_OKCANCEL) == IDOK) ? true : false; }


static HWND get_hWnd_text (enum UI_ITEM ui_item)
{
    switch (ui_item)
    {
        case UI_main_text: return hWnd_main_text;
        case UI_mesg_text: return hWnd_mesg_text;
        case UI_path_text: return hWnd_path_text;
        case UI_time_text: return hWnd_time_text;
        case UI_calc_input: return hWnd_calc_input;
        case UI_calc_result: return hWnd_calc_result;
        case UI_pause_button: return hWnd_pause_button;
        case UI_forward_button: return hWnd_forward_button;
        default: return NULL;
    }
}

static mchar* buffer = NULL;

const mchar* userinterface_get_text (enum UI_ITEM ui_item)
{
    HWND hWnd_text = get_hWnd_text(ui_item);
    if(hWnd_text==NULL) return NULL;
    hWnd_get_text(&buffer, hWnd_text);
    return buffer;
}

void userinterface_set_text (enum UI_ITEM ui_item, const mchar* text)
{
    const mchar* mstr;     
    static bool check=false;
    HWND hWnd_text = get_hWnd_text(ui_item);
    if(hWnd_text == NULL) return;

    if(ui_item==UI_main_text)
    {
        //if(file_exists_get())
        {
            // first check if changes are minimal, if so then keep file opened
            mstr = userinterface_get_text(UI_main_text);
            if(0==strcmp22(text, mstr)) return;

            // call to new_file() will come back here
            if(check) return; else check=true;
            if(!new_file()) { check=false; return; }
        }
        check=false;
    }
    SendMessage (hWnd_text, WM_SETTEXT, 0, (LPARAM)text);
}

void userinterface_clean ()
{
    mchar_free(buffer); buffer=NULL;
}


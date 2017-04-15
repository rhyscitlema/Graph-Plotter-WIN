/*
    create.c
    Links:
    Using Dialog Boxes: http://msdn.microsoft.com/en-us/library/windows/desktop/ms644996%28v=vs.85%29.aspx
    Single Line Edit Control: http://msdn.microsoft.com/en-us/library/windows/desktop/hh298434%28v=vs.85%29.aspx
*/

#include "../dialog_boxes.h"
#include <_math.h>

#ifdef CREATE_DIALOG
static HWND hWnd_dialog_box;
#endif

static void (*user_entry_get) (mchar* entry);

static const mchar* (*user_entry_apply) (const mchar* entry);



/* Message handler for the dialog box. */
static INT_PTR CALLBACK DialogBoxProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    mchar entry[300];
    const mchar* error;
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        user_entry_get (entry);
        SetDlgItemText(hWnd, IDC_EDIT0, entry);
        #ifdef CREATE_DIALOG
        //SendMessage (hWnd, DM_SETDEFID, IDCANCEL, (LPARAM) 0);
        #endif
        return TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            entry[0]=0;
            GetDlgItemText (hWnd, IDC_EDIT0, entry, sizeof(entry));

            error = user_entry_apply(entry);
            if(error!=NULL)
            {   MessageBox (hWnd, error, L"Error", MB_OK);
                SetFocus (hWnd);
                return TRUE;
            }
            // no break

        case IDCANCEL:
            #ifndef CREATE_DIALOG
            EndDialog(hWnd, LOWORD(wParam));
            #else
            DestroyWindow(hWnd);
            hWnd_dialog_box = NULL;
            #endif
            SetFocus(hWnd_focused);
            return TRUE;

        case IDC_EDIT0:
            if(HIWORD(wParam) == EN_CHANGE)
                SendMessage (hWnd, DM_SETDEFID, IDOK, (LPARAM)0);
        }
    }
    return FALSE;
}



bool create_dialog_box (HWND hWnd, int IDD, void (*_user_entry_get) (mchar* entry), const mchar* (*_user_entry_apply) (const mchar* entry))
{
    user_entry_get = _user_entry_get;
    user_entry_apply = _user_entry_apply;

    #ifndef CREATE_DIALOG
    return (int)DialogBox (NULL, MAKEINTRESOURCE(IDD), hWnd, DialogBoxProc);
    #else
    if(!IsWindow(hWnd_dialog_box))
    {
        hWnd_dialog_box = CreateDialog (NULL, MAKEINTRESOURCE(IDD), hWnd, DialogBoxProc);
        if(hWnd_dialog_box == NULL) return FALSE;
        ShowWindow (hWnd_dialog_box, SW_SHOW);
    }
    else SetFocus (hWnd_dialog_box);
    return TRUE;
    #endif
}



static mchar* buffer=NULL;

static void user_entry_get_for_goto (mchar* entry)
{
    int line;
    get_caret_position(hWnd_main_text, &line, NULL);
    intToStr(entry, line);
}

static const mchar* user_entry_apply_for_goto (const mchar* entry)
{
    int i, j, k, n;
    HWND hWnd_active = hWnd_main_text;

    const value* vst = mfet_parse_and_evaluate(entry, L"goto", VST11);
    if(!vst || !integerFromVst(&n, vst, 1, errorMessage(), "goto")) return errorMessage();

    hWnd_get_text(&buffer, hWnd_active);

    if(n<1) n=1;
    j=1;
    k=0;
    if(buffer[0]==0);
    else if(buffer[1]==0);
    else
    {
        for(i=2; ; i++)
        {
            if(j==n) break;
            if(buffer[i-1]=='\n' && buffer[i-2]=='\r')
            { j++; k=i; }
            if(buffer[i]==0) break;
        }
    }

    SendMessage (hWnd_active, EM_SETSEL, k, k);
    SendMessage (hWnd_active, EM_SCROLLCARET, 0, 0);
    hWnd_focused = hWnd_active;
    return NULL;
}

bool goto_dialog_box (HWND hWnd)
{ return create_dialog_box(hWnd, IDD_GOTOBOX, user_entry_get_for_goto, user_entry_apply_for_goto); }



void get_caret_position (HWND hWnd_text, int *line_ptr, int *coln_ptr)
{
    int i, line, coln, post;

    // TODO: use EM_LINEINDEX with line=-1 instead
    SendMessage (hWnd_text, EM_GETSEL, (WPARAM)NULL, (LPARAM)&post);

    hWnd_get_text (&buffer, hWnd_text);

    line=1;
    if(post==0 || buffer[0]==0) coln=1;
    else if(post==1 || buffer[1]==0) coln=2;
    else
    {   coln=2;
        for(i=2; ; i++)
        {
            if(buffer[i-1]=='\n' && buffer[i-2]=='\r')
            { line++; coln=1; }
            else coln++;
            if(i==post) break;
            if(buffer[i]==0) break;
        }
    }
    if(line_ptr!=NULL) *line_ptr = line;
    if(coln_ptr!=NULL) *coln_ptr = coln;
}

/*
    find_repl.c

    'find' and 'repalce' dialog boxes
*/

#include "../dialog_boxes.h"



static FINDREPLACE findrepl;

static HWND hWnd_active;
HWND hWnd_find;
HWND hWnd_repl;

#define MAXSIZE 10000
static TCHAR find_text[6*MAXSIZE];
static TCHAR repl_text[6*MAXSIZE];
static TCHAR* buffer = NULL;



#define ID_FIND 0x480
#define ID_REPL 0x481

#define GET_FIND_TEXT(hWnd) { \
    *((LPWORD)find_text) = MAXSIZE; \
    fsize = (int)SendDlgItemMessage (hWnd, ID_FIND, EM_GETLINE, 0, (LPARAM)find_text); \
    find_text[fsize] = '\0'; } \

#define GET_REPL_TEXT(hWnd) { \
    *((LPWORD)repl_text) = MAXSIZE; \
    rsize = (int)SendDlgItemMessage (hWnd, ID_REPL, EM_GETLINE, 0, (LPARAM)repl_text); \
    repl_text[rsize] = '\0'; } \



// return true if match found, return false otherwise
static bool match_found (const TCHAR* ptr1, const int length,
                         const TCHAR* ptr2, const int size,
                         const int start)
{
    TCHAR c, d;
    int i, flags = findrepl.Flags;

    if(start+size > length) return FALSE;  // if out of range

    if(flags & FR_MATCHCASE)
    {
        for(i = 0; i < size; i++)
        {
            c = ptr1[start+i];
            d = ptr2[i];
            if(c != d) return FALSE;
        }
    }
    else
    {
        for(i = 0; i < size; i++)
        {
            c = ptr1[start+i]; if(c>='A' && c<='Z') c += 'a'-'A'; // change from upper
            d = ptr2[i];       if(d>='A' && d<='Z') d += 'a'-'A'; // to lower case
            if(c != d) return FALSE;
        }
    }

    if(flags & FR_WHOLEWORD)
    {
        if(start > 0)
        {
            c = ptr1[start-1];
            if((c>='a' && c<='z') || (c>='A' && c<='Z'))
                return FALSE;
        }
        if(start+size < length)
        {
            c = ptr1[start+size];
            if((c>='a' && c<='z') || (c>='A' && c<='Z'))
                return FALSE;
        }
    }
    return TRUE;
}



static bool find_next (HWND hWnd)
{
    bool found = false;
    int i, start, stop, length, fsize;
    wchar findtext[MAXSIZE];

    SendMessage (hWnd_active, EM_GETSEL, (WPARAM)&start, (LPARAM)&stop);
    length = hWnd_get_text (&buffer, hWnd_active);

    GET_FIND_TEXT(hWnd);
    strcpy22(findtext, pcn_to_chr_22(0,find_text)); fsize = strlen2(findtext);

    if(findrepl.Flags & FR_DOWN)     // if search downwards/forwards
    {
        for(i = stop; i <= length-fsize; i++)
            if(match_found (buffer, length, findtext, fsize, i))
                { found=true; break; }
        if(!found){
        for(i = 0; i <= stop && i <= length-fsize; i++)
            if(match_found (buffer, length, findtext, fsize, i))
                { found=true; break; }
        }
    }   
    else                                // else search upwards/backwards
    {
        for(i = start-fsize; i >= 0; i--)
            if(match_found (buffer, length, findtext, fsize, i))
                { found=true; break; }
        if(!found){
        for(i = length-fsize; i >= start-fsize; i--)
            if(match_found (buffer, length, findtext, fsize, i))
                { found=true; break; }
        }
    }
    if(found)
    {
        SetFocus (hWnd_active);
        SendMessage (hWnd_active, EM_SETSEL, (WPARAM)i, (LPARAM)(i+fsize));
        SendMessage (hWnd_active, EM_SCROLLCARET, 0, 0);
    }
    else
    {
        sprintf2 (buffer, L"\"%s\" not found.", findtext);
        MessageBox (hWnd, buffer, L"Text not found", MB_OK);
    }
    return found;
}



static bool repl_next (HWND hWnd)
{
    int start, stop, fsize, rsize;
    wchar findtext[MAXSIZE];
    wchar repltext[MAXSIZE];

    // get what is currently selected
    SendMessage (hWnd_active, EM_GETSEL, (WPARAM)&start, (LPARAM)&stop);
    hWnd_get_text (&buffer, hWnd_active);
    strcpy22S (buffer, buffer+start, stop-start);

    // get the text to be found and replaced
    GET_FIND_TEXT(hWnd);
    strcpy22(findtext, pcn_to_chr_22(0,find_text)); fsize = strlen2(findtext);

    // check if what is selected matches with the text to be replaced
    if(match_found(buffer, stop-start, findtext, fsize, 0))
    {
        // get the text to replace with
        GET_REPL_TEXT(hWnd);
    	strcpy22(repltext, pcn_to_chr_22(0,repl_text)); rsize = strlen2(repltext);
        // finally do the replacement
        SendMessage (hWnd_active, EM_REPLACESEL, TRUE, (LPARAM)repltext);
    }
    return find_next(hWnd);
}



static bool repl_all (HWND hWnd)
{
    int i, j, k;
    int length, fsize, rsize;
    wchar findtext[MAXSIZE];
    wchar repltext[MAXSIZE];
    wchar* buffer2 = NULL;

    if(!find_next(hWnd)) return FALSE;

    GET_REPL_TEXT(hWnd);
    strcpy22(repltext, pcn_to_chr_22(0,repl_text)); rsize = strlen2(repltext);
    strcpy22(findtext, pcn_to_chr_22(0,find_text)); fsize = strlen2(findtext);

    length = strlen2(buffer);
    buffer2 = wchar_alloc (buffer2, 2*length);

    j=0; i=0;
    while(i < length)
    {
        if(match_found (buffer, length, findtext, fsize, i))
        {
            for(k=0; k < rsize; k++)
                buffer2[j++] = repltext[k];
            i += fsize;
        }
        else buffer2[j++] = buffer[i++];
    }
    buffer2[j] = '\0';

    //SendMessage (hWnd_active, WM_SETTEXT, 0, (LPARAM)buffer2);
        SendMessage (hWnd_active, EM_SETSEL, 0, -1);
        SendMessage (hWnd_active, EM_REPLACESEL, TRUE, (LPARAM)buffer2);

    wchar_free(buffer2);
    return TRUE;
}



/* Message handler for the find or repl dialog box. */
static INT_PTR CALLBACK FRHookProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch(message)
    {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case 0x1:           // if find button
                    find_next(hWnd);
                    return TRUE;

                case 0x2:           // if cancel button
                    SetFocus(hWnd_active);
                    break;

                case 0x400:         // if repl button
                    repl_next(hWnd);
                    return TRUE;

                case 0x401:         // if repl_all button
                    repl_all(hWnd);
                    return TRUE;

                case 0x420:         // if 'up' selected
                    findrepl.Flags &= ~FR_DOWN;  // clear bit
                    return TRUE;

                case 0x421:         // if 'down' selected
                    findrepl.Flags |= FR_DOWN;   // set bit
                    return TRUE;

                case 0x410:         // if Match-Whole-Word selected
                    findrepl.Flags ^= FR_WHOLEWORD;  // toggle bit
                    return TRUE;

                case 0x411:         // if Match-Case selected
                    findrepl.Flags ^= FR_MATCHCASE;  // toggle bit
                    return TRUE;

                case ID_FIND:       // if FindWhat edit text field
                    hWnd_focused = GetDlgItem(hWnd, ID_FIND);
                    break;

                case ID_REPL:       // if ReplWith edit text field
                    hWnd_focused = GetDlgItem(hWnd, ID_REPL);
                    break;

                // Setting hWnd_focused above causes a problem
                // with two successive calls to Ctrl+F due to '\'.
            }
    }
    return FALSE;
}



static bool initialise_structure (HWND hWnd_dialog)
{
    int fsize, start, stop;
    hWnd_active = hWnd_main_text;

    SendMessage (hWnd_focused, EM_GETSEL, (WPARAM)&start, (LPARAM)&stop);
    if(start != stop)
    {
        if(stop-start > MAXSIZE)
        {
            sprintf1((char*)buffer, "Selection length cannot be more than %d.", MAXSIZE);
            MessageBox(hWnd_focused, CST21((char*)buffer), L"Error", MB_OK);
            return FALSE;
        }
        hWnd_get_text(&buffer, hWnd_focused);
        strcpy22S(buffer, buffer+start, stop-start);
        strcpy22(find_text, chr_to_pcn_22(0,buffer));
    }

    if(IsWindow(hWnd_find))
    {
        if(hWnd_dialog == hWnd_find)
        {
            if(start != stop) SetDlgItemText (hWnd_dialog, ID_FIND, find_text);
            SetFocus(hWnd_dialog);
            return FALSE;
        }
        if(start==stop) GET_FIND_TEXT(hWnd_find);
        DestroyWindow(hWnd_find);
    }
    else if(IsWindow(hWnd_repl))
    {
        if(hWnd_dialog == hWnd_repl)
        {
            if(start != stop) SetDlgItemText (hWnd_dialog, ID_FIND, find_text);
            SetFocus(hWnd_dialog);
            return FALSE;
        }
        if(start==stop) GET_FIND_TEXT(hWnd_repl);
        DestroyWindow(hWnd_repl);
    }

    memset (&findrepl, 0, sizeof(findrepl));
    findrepl.lStructSize      = sizeof(findrepl);
    findrepl.hwndOwner        = hWnd_active;
    findrepl.hInstance        = NULL;
    findrepl.Flags            = FR_DOWN | FR_ENABLEHOOK;
    findrepl.lpstrFindWhat    = find_text;
    findrepl.lpstrReplaceWith = repl_text;
    findrepl.wFindWhatLen     = SIZEOF(find_text);
    findrepl.wReplaceWithLen  = SIZEOF(repl_text);
    findrepl.lCustData        = 0;
    findrepl.lpfnHook         = (LPFRHOOKPROC) FRHookProc;
    findrepl.lpTemplateName   = NULL;
    return TRUE;
}



bool find_dialog_box ()
{
    if(initialise_structure (hWnd_find))
    {
        hWnd_find = FindText(&findrepl);
        if(hWnd_find == NULL) return FALSE;
    }
    return TRUE;
}

bool repl_dialog_box ()
{
    if(initialise_structure (hWnd_repl))
    {
        hWnd_repl = ReplaceText(&findrepl);
        if(hWnd_repl == NULL) return FALSE;
    }
    return TRUE;
}

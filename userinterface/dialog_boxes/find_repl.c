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
static const wchar* buffer = NULL;



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

    if(start+size > length) return false;  // if out of range

    if(flags & FR_MATCHCASE)
    {
        for(i = 0; i < size; i++)
        {
            c = ptr1[start+i];
            d = ptr2[i];
            if(c != d) return false;
        }
    }
    else
    {
        for(i = 0; i < size; i++)
        {
            c = ptr1[start+i]; if(c>='A' && c<='Z') c += 'a'-'A'; // change from upper
            d = ptr2[i];       if(d>='A' && d<='Z') d += 'a'-'A'; // to lower case
            if(c != d) return false;
        }
    }

    if(flags & FR_WHOLEWORD)
    {
        if(start > 0)
        {
            c = ptr1[start-1];
            if((c>='a' && c<='z') || (c>='A' && c<='Z'))
                return false;
        }
        if(start+size < length)
        {
            c = ptr1[start+size];
            if((c>='a' && c<='z') || (c>='A' && c<='Z'))
                return false;
        }
    }
    return true;
}



static bool find_next (HWND hWnd)
{
    bool found = false;
    int i, start, stop, length, fsize;
    const wchar* findtext;
    uint32_t stack[MAXSIZE*10];

    SendMessage (hWnd_active, EM_GETSEL, (WPARAM)&start, (LPARAM)&stop);
    buffer = hWnd_get_text(hWnd_active);
    length = strlen2(buffer);

    GET_FIND_TEXT(hWnd);
    pcn_to_chr(setStr22(stack, find_text));
    findtext = getStr2(vGet(stack));
    fsize = strlen2(findtext);

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
        wchar message[strlen2(findtext)+20];
        const wchar* argv[2] = { L"\"%s\" not found.", findtext };
        sprintf2(message, 2, argv);
        MessageBox(hWnd, message, L"Text not found", MB_OK);
    }
    return found;
}



static bool repl_next (HWND hWnd)
{
    int start, stop, fsize, rsize;
    uint32_t stack[MAXSIZE*10];
    const wchar* findtext;
    const wchar* repltext;

    // get what is currently selected
    SendMessage (hWnd_active, EM_GETSEL, (WPARAM)&start, (LPARAM)&stop);
    buffer = hWnd_get_text(hWnd_active);
    strcpy22S(WCHAR(buffer), buffer+start, stop-start);

    // get the text to be found and replaced
    GET_FIND_TEXT(hWnd);
    value v = pcn_to_chr(setStr22(stack, find_text));
    findtext = getStr2(vGet(stack));
    fsize = strlen2(findtext);

    // check if what is selected matches with the text to be replaced
    if(match_found(buffer, stop-start, findtext, fsize, 0))
    {
        // get the text to replace with
        GET_REPL_TEXT(hWnd);
        pcn_to_chr(setStr22(v, repl_text));
        repltext = getStr2(vGet(v));
        rsize = strlen2(repltext);
        // finally do the replacement
        SendMessage (hWnd_active, EM_REPLACESEL, true, (LPARAM)repltext);
    }
    return find_next(hWnd);
}



static bool repl_all (HWND hWnd)
{
    int i, j, k;
    int length, fsize, rsize;
    uint32_t stack[MAXSIZE*10];
    const wchar* findtext;
    const wchar* repltext;
    wchar* buffer2 = NULL;

    if(!find_next(hWnd)) return false;
    value v = pcn_to_chr(setStr22(stack, find_text));
    findtext = getStr2(vGet(stack));
    fsize = strlen2(findtext);

    GET_REPL_TEXT(hWnd);
    pcn_to_chr(setStr22(v, repl_text));
    repltext = getStr2(vGet(v));
    rsize = strlen2(repltext);

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
        SendMessage (hWnd_active, EM_REPLACESEL, true, (LPARAM)buffer2);

    wchar_free(buffer2);
    return true;
}



/* Message handler for the find or repl dialog box. */
static INT_PTR CALLBACK FRHookProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch(message)
    {
        case WM_INITDIALOG:
            return true;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case 0x1:           // if find button
                    find_next(hWnd);
                    return true;

                case 0x2:           // if cancel button
                    SetFocus(hWnd_active);
                    break;

                case 0x400:         // if repl button
                    repl_next(hWnd);
                    return true;

                case 0x401:         // if repl_all button
                    repl_all(hWnd);
                    return true;

                case 0x420:         // if 'up' selected
                    findrepl.Flags &= ~FR_DOWN;  // clear bit
                    return true;

                case 0x421:         // if 'down' selected
                    findrepl.Flags |= FR_DOWN;   // set bit
                    return true;

                case 0x410:         // if Match-Whole-Word selected
                    findrepl.Flags ^= FR_WHOLEWORD;  // toggle bit
                    return true;

                case 0x411:         // if Match-Case selected
                    findrepl.Flags ^= FR_MATCHCASE;  // toggle bit
                    return true;

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
    return false;
}



static bool initialise_structure (HWND hWnd_dialog)
{
    int fsize, start, stop;
    hWnd_active = hWnd_main_text;
    uint32_t stack[MAXSIZE*10];

    SendMessage (hWnd_focused, EM_GETSEL, (WPARAM)&start, (LPARAM)&stop);
    if(start != stop)
    {
        if(stop-start > MAXSIZE)
        {
            MessageBox(hWnd_focused, L"Selection length is too long.", L"Error", MB_OK);
            return false;
        }
        buffer = hWnd_get_text(hWnd_focused);
        strcpy22S(WCHAR(buffer), buffer+start, stop-start);
        chr_to_pcn(setStr22(stack, buffer));
        strcpy22(find_text, getStr2(vGet(stack)));
    }

    if(IsWindow(hWnd_find))
    {
        if(hWnd_dialog == hWnd_find)
        {
            if(start != stop) SetDlgItemText (hWnd_dialog, ID_FIND, find_text);
            SetFocus(hWnd_dialog);
            return false;
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
            return false;
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
    return true;
}



bool find_dialog_box ()
{
    if(initialise_structure (hWnd_find))
    {
        hWnd_find = FindText(&findrepl);
        if(hWnd_find == NULL) return false;
    }
    return true;
}

bool repl_dialog_box ()
{
    if(initialise_structure (hWnd_repl))
    {
        hWnd_repl = ReplaceText(&findrepl);
        if(hWnd_repl == NULL) return false;
    }
    return true;
}

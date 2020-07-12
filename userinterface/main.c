/*
    main.c

    Build the Graphical User Interface
*/

#include "main.h"
#include "font.h"
#include "files.h"
#include "resource.h"
#include "undo_redo.h"
#include "dialog_boxes.h"
#include <userinterface.h>
#include <surface.h>
#include <camera.h>
#include <tools.h>
#include <timer.h>
#include <_stdio.h>


static TCHAR szTitle[100];        // The title bar text
static TCHAR szWindowClass[100];  // Main window class name

static WINDOWPLACEMENT wPlacement;
int main_window_width  = 600;
int main_window_height = 500;

HINSTANCE main_hInst;
HWND hWnd_main_window;
HWND hWnd_main_text;

HWND hWnd_eval_button;
HWND hWnd_mesg_text;
HWND hWnd_lock_button;

HWND hWnd_prev_button;
HWND hWnd_next_button;
HWND hWnd_delete_button;
HWND hWnd_clear_button;
HWND hWnd_path_text;

HWND hWnd_pause_button;
HWND hWnd_forward_button;
HWND hWnd_lower_button;
HWND hWnd_higher_button;
HWND hWnd_time_text;

HWND hWnd_calc_button;
HWND hWnd_calc_input;
HWND hWnd_calc_result;

HWND hWnd_status_bar;
HWND hWnd_focused;

static BOOL createMainWindow (HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
static void load_launched_file();



/*** Software entry point: main() ***/
int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR     lpCmdLine,
                    int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Store instance handle in global variable
    main_hInst = hInstance;

    // Initialise global strings
    #ifdef LIBRODT
    strcpy21 (szTitle, "Rhyscitlema Graph Plotter 3D");
    strcpy21 (szWindowClass, "Rhyscitlema Graph Plotter 3D - Software");
    #else
    strcpy21 (szTitle, "Rhyscitlema RFET Calculator");
    strcpy21 (szWindowClass, "Rhyscitlema RFET Calculator - Software");
    #endif

    // Initialise accelerator table
    HACCEL hAccelTable = LoadAccelerators (hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

    // Initialise application main user interface
    if(!createMainWindow (hInstance, nCmdShow)) return false;

    // install or initialise timer device
    timer_install_do();

    // load launched file
    load_launched_file();

    MSG msg;
    // Main message loop
    while(GetMessage(&msg, NULL, 0, 0))
    {
        if( !TranslateAccelerator(hWnd_main_window, hAccelTable, &msg)
         && !(IsWindow(hWnd_find) && IsDialogMessage(hWnd_find, &msg))
         && !(IsWindow(hWnd_repl) && IsDialogMessage(hWnd_repl, &msg)) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}



/* Register the main window class, and,
   Create and display the main window.
*/
static BOOL createMainWindow (HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wcex;

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpszClassName  = szWindowClass;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_APP_MENU);
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

    RegisterClassEx (&wcex); // returned value not used

    hWnd =
    CreateWindowEx (WS_EX_ACCEPTFILES,
                    szWindowClass,
                    szTitle,
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    main_window_width,
                    main_window_height,
                    NULL, NULL, hInstance, NULL);

    if(!hWnd) return false;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    hWnd_main_window = hWnd;

    // Set length of Window Placement structure
    wPlacement.length = sizeof(wPlacement);
    return true;
}



bool SetWindowTitle (HWND hWnd, const TCHAR* fileName, bool fileExists)
{
    TCHAR title[MAX_FILE_NAME];
    sprintf2(title, L"%s - %s ", get_name_from_path_name(NULL, fileName), szTitle);
    return SetWindowText(hWnd, title);
}



static void SetStatusBar (HWND hWnd_text)
{
    static int Line, Coln;
    int line, coln;
    wchar str[100];

    get_caret_position (hWnd_text, &line, &coln);
    if(Line==line && Coln==coln) return;
    Line=line; Coln=coln;

    sprintf2(str, L" Line:Coln %s:%s ", TIS2(0,line), TIS2(1,coln));
    SendMessage (hWnd_status_bar, WM_SETTEXT, 0, (LPARAM)str);
}



static bool on_launch_or_drop_file (const wchar* fileName)
{
    const wchar* extension;
    if(!open_file(fileName)) { display_message(errorMessage()); return false; }
    get_path_from_path_name (default_file_path, fileName);
    extension = get_extension_from_name(NULL, fileName);
    if(0==strcmp21(extension, "rodt")
    || 0==strcmp21(extension, "rfet"))
        tools_do_eval(get_name_from_path_name(NULL,fileName));
    return true;
}

static void load_launched_file ()
{
    int i;
    wchar str[MAX_PATH_SIZE];
    wchar* argv=NULL;
    const wchar* CmdLine;
    CmdLine = GetCommandLine();
    CmdLine = sgets2(CmdLine, NULL);  // skip the program file name
    for(i=1; ; i++)
    {
        CmdLine = sgets2(CmdLine, &argv); // get the opened file name
        if(!argv || !argv[0]) break;
        on_launch_or_drop_file(argv);
    }
    wchar_free(argv);
    if(i<=1)
    {
        #ifdef LIBRODT
        strcpy21(str, "\r\n To get started:\r\n");
        strcat21(str, "\r\n Drag-and-drop to open an RFET or RODT File, or,\r\n");
        strcat21(str, "\r\n Launch the software from an RFET or RODT file, or,\r\n");
        strcat21(str, "\r\n Go to Menu -> File -> Open... then do Evaluate.\r\n");
        display_main_text(str);
        #endif
    }
    #ifdef LIBRODT
    calculator_evaluate_calc(true);
    #endif
    userinterface_update();
}



static inline void do_resize (HWND hWnd, int L[4])
{ MoveWindow (hWnd, L[0], L[1], L[2], L[3], true); }

bool main_window_resize ()
{
    int i=0, layout[20][4];
    if(!tools_uidt_eval(layout, NULL))
    { display_message(errorMessage()); return false; }

    do_resize (hWnd_main_text       , layout[i++]);

    do_resize (hWnd_eval_button     , layout[i++]);
    do_resize (hWnd_mesg_text       , layout[i++]);
    do_resize (hWnd_lock_button     , layout[i++]);

    do_resize (hWnd_prev_button     , layout[i++]);
    do_resize (hWnd_next_button     , layout[i++]);
    do_resize (hWnd_delete_button   , layout[i++]);
    do_resize (hWnd_clear_button    , layout[i++]);
    do_resize (hWnd_path_text       , layout[i++]);

    do_resize (hWnd_pause_button    , layout[i++]);
    do_resize (hWnd_forward_button  , layout[i++]);
    do_resize (hWnd_lower_button    , layout[i++]);
    do_resize (hWnd_higher_button   , layout[i++]);
    do_resize (hWnd_time_text       , layout[i++]);

    do_resize (hWnd_calc_button     , layout[i++]);
    do_resize (hWnd_calc_input      , layout[i++]);
    do_resize (hWnd_calc_result     , layout[i++]);

    do_resize (hWnd_status_bar      , layout[i++]);
    return true;
}

static HWND create_button (HWND hWnd, int ID, const char* name)
{
    return CreateWindowEx (0, L"BUTTON", (const TCHAR*)CST21(name),
                           WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                           0,0,0,0, hWnd, (HMENU)(UINT_PTR)ID, main_hInst, NULL);
}

static void main_window_create (HWND hWnd)
{
    hWnd_main_text =
        CreateWindowEx (0, L"EDIT", L"",
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL |
                        ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN,
                        0,0,0,0, hWnd, (HMENU)IDC_MAIN_TEXT, main_hInst, NULL);
    hWnd_mesg_text =
        CreateWindowEx (WS_EX_CLIENTEDGE, L"EDIT", L"",
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_VSCROLL |
                        ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
                        0,0,0,0, hWnd, (HMENU)IDC_MESG_TEXT, main_hInst, NULL);
    hWnd_path_text =
        CreateWindowEx (WS_EX_CLIENTEDGE, L"EDIT", L"",
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_WANTRETURN | ES_AUTOHSCROLL,
                        0,0,0,0, hWnd, (HMENU)IDC_PATH_TEXT, main_hInst, NULL);
    hWnd_time_text =
        CreateWindowEx (WS_EX_CLIENTEDGE, L"EDIT", L"",
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_WANTRETURN | ES_AUTOHSCROLL,
                        0,0,0,0, hWnd, (HMENU)IDC_TIME_TEXT, main_hInst, NULL);
    hWnd_calc_input =
        #ifdef LIBRODT
		CreateWindowEx (WS_EX_CLIENTEDGE, L"EDIT", L" LocalPointedPoint",
        #else
		CreateWindowEx (WS_EX_CLIENTEDGE, L"EDIT", L" 1+1 ",
        #endif
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD |
                        ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN,
                        0,0,0,0, hWnd, (HMENU)IDC_CALC_INPUT, main_hInst, NULL);
    hWnd_calc_result =
        CreateWindowEx (WS_EX_CLIENTEDGE, L"EDIT", L"",
                        WS_TABSTOP | WS_VISIBLE | WS_CHILD |
                        ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY ,
                        0,0,0,0, hWnd, (HMENU)IDC_CALC_RESULT, main_hInst, NULL);
    hWnd_status_bar =
        CreateWindowEx (0, L"STATIC", L"",
                        WS_VISIBLE | WS_CHILD,
                        0,0,0,0, hWnd, (HMENU)IDC_STATUS_BAR, main_hInst, NULL);

    hWnd_eval_button   = create_button (hWnd, IDC_EVAL_BUTTON       , TEXT_EVAL);
    hWnd_lock_button   = create_button (hWnd, IDC_LOCK_BUTTON       , NULL);

    hWnd_prev_button    = create_button (hWnd, IDC_PREV_BUTTON      , TEXT_PREV);
    hWnd_next_button    = create_button (hWnd, IDC_NEXT_BUTTON      , TEXT_NEXT);
    hWnd_delete_button  = create_button (hWnd, IDC_DELETE_BUTTON    , TEXT_DELE);
    hWnd_clear_button   = create_button (hWnd, IDC_CLEAR_BUTTON     , TEXT_CLEAR);

    hWnd_pause_button   = create_button (hWnd, IDC_PAUSE_BUTTON     , TEXT_RESUME);
    hWnd_forward_button = create_button (hWnd, IDC_FORWARD_BUTTON   , TEXT_BACKWARD);
    hWnd_lower_button   = create_button (hWnd, IDC_LOWER_BUTTON     , TEXT_LOWER);
    hWnd_higher_button  = create_button (hWnd, IDC_HIGHER_BUTTON    , TEXT_HIGHER);

    hWnd_calc_button    = create_button (hWnd, IDC_CALC_BUTTON      , TEXT_CALC);

    main_window_resize();
}

static void main_window_destroy (HWND hWnd)
{
    if(check_save_changes())
    {   // deal with any future call to check_save_changes()
        // especially since tools_clean() calls UI_MAIN_TEXT
        SendMessage (hWnd_main_text , WM_SETTEXT, 0, 0);
        set_undo (hWnd_main_text, 0);

        tools_clean();
        DestroyWindow(hWnd);
    }
}



/*
//  Process messages for the main window.
//
//  WM_COMMAND  - process menu items and buttons
//  WM_PAINT    - paint the main window
//  WM_DESTROY  - post a quit message and return
*/
static LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu, hMenu_main;
    TCHAR filename[MAX_FILE_NAME];
    int low, high;
    PAINTSTRUCT ps;

    switch (message)
    {

    case WM_INITMENUPOPUP:
        hMenu_main = GetMenu(hWnd);
        hMenu = (HMENU)wParam;
        if(hMenu == GetSubMenu(hMenu_main, 0)) // if Menu->File
        {
            EnableMenuItem(hMenu, IDM_RELOAD, (file_exists_get() ? MF_ENABLED : MF_GRAYED));
        }
        else if(hMenu == GetSubMenu(hMenu_main, 1)) // if Menu->Edit
        {
            if(get_undo(hWnd_focused)==0
            && SendMessage(hWnd_focused, EM_GETMODIFY, 0, 0))
                set_undo(hWnd_focused, 1);

            if(SendMessage (hWnd_focused, EM_GETMODIFY, 0, 0))
                 EnableMenuItem(hMenu, IDM_UNDO, MF_ENABLED);
            else EnableMenuItem(hMenu, IDM_UNDO, MF_GRAYED);

            SendMessage (hWnd_focused, EM_GETSEL, (WPARAM)&low, (LPARAM)&high);
            EnableMenuItem(hMenu, IDM_CUT   , (low==high ? MF_GRAYED : MF_ENABLED));
            EnableMenuItem(hMenu, IDM_COPY  , (low==high ? MF_GRAYED : MF_ENABLED));
            EnableMenuItem(hMenu, IDM_DELETE, (low==high ? MF_GRAYED : MF_ENABLED));

            if(low == 0 && high == SendMessage(hWnd_focused, WM_GETTEXTLENGTH, 0, 0))
                 EnableMenuItem(hMenu, IDM_SELECTALL, MF_GRAYED);
            else EnableMenuItem(hMenu, IDM_SELECTALL, MF_ENABLED);
        }
        else if(hMenu == GetSubMenu(hMenu_main, 2)) // if Menu->View
        {
            GetWindowPlacement (hWnd_main_window, &wPlacement);
            if(wPlacement.showCmd == SW_MAXIMIZE)
                 CheckMenuItem(hMenu, IDM_FULLSCREEN, MF_CHECKED);
            else CheckMenuItem(hMenu, IDM_FULLSCREEN, MF_UNCHECKED);
        }
        else if(hMenu == GetSubMenu(hMenu_main, 4)) // if Menu->Tools
        {
            #ifdef LIBRODT
            UINT choice = (camera_list->size==0 && surface_list->size==0) ? MF_GRAYED : MF_ENABLED;
            EnableMenuItem(hMenu, IDM_SAVEALLOBJECTS, choice);
            EnableMenuItem(hMenu, IDM_REMOVEALLOBJECTS, choice);
            #endif
        }
        break;

    case WM_DROPFILES:
        if(DragQueryFile((HDROP)wParam, 0, filename, SIZEOF(filename)))
            on_launch_or_drop_file(filename);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
         keyboard_input_event(hWnd, message, wParam, lParam);
         break;


    case WM_COMMAND:
        low  = LOWORD(wParam);
        high = HIWORD(wParam);

        switch(low)
        {
            // 1) For File Menu
            case IDM_NEW:           new_file(); break;
            case IDM_OPEN:          open_file(0); break;
            case IDM_SAVE:          save_file(); break;
            case IDM_SAVEAS:        save_file_as(0); break;
            case IDM_RELOAD:        reload_file(); break;
            case IDM_PRINT:         print_dialog_box(hWnd); break;
            case IDM_EXIT:          main_window_destroy(hWnd); break;
            // 2) For Edit Menu
            case IDM_UNDO:          SendMessage (hWnd_focused, WM_UNDO  , 0, 0); break;
            case IDM_REDO:          break;
            case IDM_CUT:           SendMessage (hWnd_focused, WM_CUT   , 0, 0); break;
            case IDM_COPY:          SendMessage (hWnd_focused, WM_COPY  , 0, 0); break;
            case IDM_PASTE:         SendMessage (hWnd_focused, WM_PASTE , 0, 0); break;
            case IDM_DELETE:        SendMessage (hWnd_focused, WM_CLEAR , 0, 0); break;
            case IDM_SELECTALL:     SendMessage (hWnd_focused, EM_SETSEL, 0,-1); break;
            case IDM_PCN_TO_CHR:    font_pcn_to_chr(hWnd); break;
            case IDM_CHR_TO_PCN:    font_chr_to_pcn(hWnd); break;
            case IDM_CHR_TO_FCN:    font_chr_to_fcn(hWnd); break;
            case IDM_SET_PIF_CN:    font_set_pif_cn(hWnd); break;
            case IDM_FIND:          find_dialog_box(); break;
            case IDM_REPLACE:       repl_dialog_box(); break;
            case IDM_GOTO:          goto_dialog_box(hWnd); break;

            // 3) For View Menu
            case IDM_FULLSCREEN:    if(wPlacement.showCmd == SW_MAXIMIZE)
                                        ShowWindow (hWnd, SW_RESTORE);
                                    else ShowWindow (hWnd, SW_MAXIMIZE);
                                    break;

            case IDM_FONT:          font_dialog_box(hWnd); break;

            // 4) For Tool Menu
            case IDM_TAKE_PICTURE:      take_camera_picture(); break;
            case IDM_SAVEALLOBJECTS:    save_all_objects(); break;
            case IDM_REMOVEALLOBJECTS:  tools_remove_all_objects(true); break;

            // 5) For Help Menu
            case IDM_HELP:          break;
            case IDM_ABOUT:         about_dialog_box(hWnd); break;

            // 6) For text fields
            case IDC_MAIN_TEXT:     hWnd_focused = hWnd_main_text;
                                    SetStatusBar(hWnd_focused); break;

            case IDC_MESG_TEXT:     hWnd_focused = hWnd_mesg_text;
                                    SetStatusBar(hWnd_focused); break;

            case IDC_PATH_TEXT:     hWnd_focused = hWnd_path_text;
                                    if(high==EN_KILLFOCUS) tools_do_select();
                                    SetStatusBar(hWnd_focused); break;

            case IDC_TIME_TEXT:     hWnd_focused = hWnd_time_text;
                                    if(high==EN_KILLFOCUS) tools_set_time(NULL);
                                    SetStatusBar(hWnd_focused); break;

            case IDC_CALC_INPUT:    hWnd_focused = hWnd_calc_input;
                                    SetStatusBar(hWnd_focused); break;

            case IDC_CALC_RESULT:   hWnd_focused = hWnd_calc_result;
                                    SetStatusBar(hWnd_focused); break;

            // 7) For buttons
            case IDC_EVAL_BUTTON:       tools_do_eval(NULL); break;
            case IDC_LOCK_BUTTON:       SetFocus(hWnd); break;

            case IDC_PREV_BUTTON:       tools_get_prev(); break;
            case IDC_NEXT_BUTTON:       tools_get_next(); break;
            case IDC_DELETE_BUTTON:     tools_do_delete(); break;
            case IDC_CLEAR_BUTTON:      tools_do_clear(); break;

            case IDC_PAUSE_BUTTON:      tools_do_pause(!timer_paused()); break;
            case IDC_FORWARD_BUTTON:    tools_go_forward(timer_get_period()<0); break;
            case IDC_LOWER_BUTTON:      tools_lower_period(); break;
            case IDC_HIGHER_BUTTON:     tools_higher_period(); break;

            case IDC_CALC_BUTTON: calculator_evaluate_calc(true); break;

            default: return DefWindowProc (hWnd, message, wParam, lParam);
        } // end of switch(low)
        break;


    case WM_CREATE:
        hMenu = GetSubMenu(GetSubMenu(GetMenu(hWnd), 2), 1); // get Menu->View->keypad
        CheckMenuRadioItem(hMenu, IDM_STANDARD, IDM_VIEW_NONE, IDM_VIEW_NONE, MF_BYCOMMAND);

        tools_init(10000000,NULL);

        main_window_create(hWnd);
        hWnd_main_window = hWnd;

        hWnd_focused = hWnd_main_text;
        SetFocus(hWnd_focused);

        font_initialise();
        new_file();
        break;


    case WM_SIZE:
        main_window_width  = LOWORD(lParam);
        main_window_height = HIWORD(lParam);
        main_window_resize();
        break;

    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_CLOSE:
         main_window_destroy(hWnd);
         break;

    case WM_DESTROY:
        tools_clean();
        font_remove();
        PostQuitMessage(0);
        break;

    case WM_SETCURSOR:
    case WM_CTLCOLOREDIT:
        SetStatusBar(hWnd_focused);

    default: return DefWindowProc (hWnd, message, wParam, lParam);
    } // end of switch (message)
    return 0;
}

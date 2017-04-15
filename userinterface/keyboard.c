/*
    keyboard.c
*/
#include "main.h"
#include <mouse.h>


static int WINAPI_KEY_TO_MFET_KEY (int KEY)
{
    if(('0'<=KEY && KEY<='9')
    || ('A'<=KEY && KEY<='Z')
    || ('a'<=KEY && KEY<='z'))
        return KEY;

    switch(KEY)
    {
    case VK_RETURN:     return Key_Enter;
    case VK_ESCAPE:     return Key_Escape;
    case VK_DELETE:     return Key_Delete;
    case VK_SPACE:      return Key_Space;
    case VK_BACK:       return Key_Backspace;

    case VK_PRINT:      return Key_Print;
    case VK_PAUSE:      return Key_Break; // Key_Pause
    case VK_INSERT:     return Key_Insert;
    case VK_HOME:       return Key_Home;
    case VK_PRIOR:      return Key_PageUp;
    case VK_NEXT:       return Key_PageDown;
    case VK_END:        return Key_End;
    case VK_TAB:        return Key_Tab;
    case VK_MENU:       return Key_Menu;

  //case GDK_KEY_Num_Lock:    return Key_NumLock;
  //case GDK_KEY_Scroll_Lock: return Key_ScrollLock;
    case VK_CAPITAL:          return Key_CapsLock;

    case VK_CONTROL:        return Key_Ctrl_Left;
  //case GDK_KEY_Control_R: return Key_Ctrl_Right;
  //case GDK_KEY_Alt_L:     return Key_Alt_Left;
  //case GDK_KEY_Alt_R:     return Key_Alt_Right;
    case VK_SHIFT:          return Key_Shift_Left;
  //case GDK_KEY_Shift_R:   return Key_Shift_Right;

    case VK_UP:        return Key_Up;
    case VK_DOWN:      return Key_Down;
    case VK_LEFT:      return Key_Left;
    case VK_RIGHT:     return Key_Right;

    case VK_F1:        return Key_F1;
    case VK_F2:        return Key_F2;
    case VK_F3:        return Key_F3;
    case VK_F4:        return Key_F4;
    case VK_F5:        return Key_F5;
    case VK_F6:        return Key_F6;
    case VK_F7:        return Key_F7;
    case VK_F8:        return Key_F8;
    case VK_F9:        return Key_F9;
    case VK_F10:       return Key_F10;
    case VK_F11:       return Key_F11;
    case VK_F12:       return Key_F12;

    default: return 0;
    }
}


bool keyboard_input_event (HWND dw, UINT message, WPARAM wParam, LPARAM lParam)
{
    int key = WINAPI_KEY_TO_MFET_KEY((int)wParam);
    bool pressed = message == WM_KEYDOWN;
    return on_key_event(key, pressed);
}


/*
	drawing_window.c
*/

#include "main.h"
#include <drawing_window.h>
#include <userinterface.h>
#include <mouse.h>
#include "resource.h"
#include <zmouse.h>



static bool mouse_input_event (HWND dw, UINT message, WPARAM wParam, LPARAM lParam, enum BUTTON_STATE state)
{
	int px=0, py=0, dz=0, button=0;

	if(wParam & MK_LBUTTON) button |= 0x1;
	if(wParam & MK_MBUTTON) button |= 0x2;
	if(wParam & MK_RBUTTON) button |= 0x4;

	if(message == WM_MOUSEWHEEL)
		dz = (signed short)(HIWORD(wParam)) / WHEEL_DELTA;
	else{
		px = LOWORD(lParam); // get X cursor position
		py = HIWORD(lParam); // get Y cursor position
	}
	return on_mouse_event(px, py, dz, button, state, dw);
}



static void draw_to_window (HDC hDC, const int* pixel_array, int width, int height)
{
	int y;
	BITMAPINFO bmi;

	if(!hDC || !pixel_array) return;

	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = width;
	//bmi.bmiHeader.biWidth     = height;
	bmi.bmiHeader.biHeight      = 1;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biBitCount    = 32;

	for(y=0; y < height; y++)
		SetDIBitsToDevice (
			hDC,
			0,y,
			width, 1,
			0,0,
			0,width,
			pixel_array + y*width,
			&bmi,
			DIB_RGB_COLORS );

	/* Version below has un-comprehensible issues...!
	** Why is it that the simplest things never work in GUI development?!

	SetDIBitsToDevice (
		hDC,
		0,0,
		width, height,
		0,0,
		0,width,
		pixel_array,
		&bmi,
		DIB_RGB_COLORS );*/
}



static LRESULT CALLBACK DrawingProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	Camera *cmr;
	enum BUTTON_STATE button_state = BUTTON_SAME;

	switch(message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		keyboard_input_event(hWnd, message, wParam, lParam);
		break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		button_state = BUTTON_PRESS;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		if(button_state==BUTTON_SAME)
			button_state = BUTTON_RELEASE;
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		mouse_input_event(hWnd, message, wParam, lParam, button_state);
		break;
	/*
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		mouse_event(hWnd, message, wParam, lParam);
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		if(GetCapture()==hWnd) ReleaseCapture();
	*/
	case WM_MOVE:
	case WM_SIZE:
		cmr = findCameraFromDW(hWnd);
		if(cmr==NULL) break;
		if(!cmr->notfirst) break;

		GetWindowRect(hWnd, &rect);
		cmr->dXPost = rect.left - cmr->XPost;
		cmr->dYPost = rect.top  - cmr->YPost;

		GetClientRect(hWnd, &rect);
		cmr->dXSize = rect.right  - cmr->XSize;
		cmr->dYSize = rect.bottom - cmr->YSize;

		drawing_window_resize_do(hWnd);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		cmr = findCameraFromDW (hWnd);
		if(cmr) draw_to_window (hdc, cmr->pixelColour, cmr->XSize, cmr->YSize);
		draw_request_count--;
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:
		drawing_window_close_do(hWnd);
		break;

	default: return DefWindowProc (hWnd, message, wParam, lParam);
	}
	return 0;
}



DrawingWindow drawing_window_new ()
{
	HWND hWnd;
	WNDCLASSEX wcex;
	const TCHAR* cameraWindowClass = L"Camera Window Class";

	memset(&wcex, 0, sizeof(WNDCLASSEX));
	wcex.cbSize         = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpszClassName  = cameraWindowClass;
	wcex.lpfnWndProc    = DrawingProc;
	wcex.hInstance      = main_hInst;
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon          = LoadIcon(main_hInst, MAKEINTRESOURCE(IDI_APP_ICON));
	wcex.hIconSm        = LoadIcon(main_hInst, MAKEINTRESOURCE(IDI_APP_ICON));

	RegisterClassEx (&wcex); // returned value not used

	hWnd = CreateWindow (
		cameraWindowClass,
		L"Camera",
		WS_TILEDWINDOW,
		0,0,0,0,
		NULL, NULL, main_hInst, NULL );

	if(hWnd == NULL)return NULL;
	ShowWindow (hWnd, SW_SHOW);
	return hWnd;
}



void drawing_window_name (DrawingWindow hWnd, const wchar* name)
{ SetWindowText ((HWND)hWnd, name); }

void drawing_window_move (DrawingWindow hWnd)
{
	RECT rect;
	Camera* cmr = findCameraFromDW(hWnd);
	if(cmr==NULL) return;
	MoveWindow((HWND)hWnd, cmr->XPost, cmr->YPost, cmr->XSize + cmr->XS, cmr->YSize + cmr->YS, true);

	GetClientRect((HWND)hWnd, &rect);
	if(cmr->XSize != rect.right)  cmr->XS = cmr->XSize - rect.right;
	if(cmr->YSize != rect.bottom) cmr->YS = cmr->YSize - rect.bottom;
	MoveWindow((HWND)hWnd, cmr->XPost, cmr->YPost, cmr->XSize + cmr->XS, cmr->YSize + cmr->YS, true);
	cmr->notfirst = true;
}

void drawing_window_draw (DrawingWindow hWnd)
{
	InvalidateRect((HWND)hWnd, NULL, false);            // option 1
	//RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);   // option 2
	//UpdateWindow(hWnd);                               // option 3

	// Option 3 is the simplest but at the same time the only one that
	// does NOT work. It does not generate any WM_PAINT message at all:
	// "...If the update region is empty, no message is sent." ?
}

void drawing_window_remove (DrawingWindow hWnd)
{ DestroyWindow((HWND)hWnd); }

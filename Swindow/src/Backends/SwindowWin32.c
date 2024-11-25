#include "SwindowWin32.h"

#include <GL/GL.h>
#pragma comment (lib, "opengl32.lib")

static LPCWSTR ConvertCharArrayToLPCWSTR(const char* charArray)
{
	const int stringLength = MultiByteToWideChar(CP_ACP, 0, charArray, -1, NULL, 0);
	wchar_t* wString = (wchar_t*)malloc(stringLength * sizeof(wchar_t));
	if (wString == NULL) return NULL;
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, stringLength);
	return wString;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	swWindow* window = (swWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	if (window)
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
			if (window->native_window->input->callbacks.key_callback)
			{
				int key = (int)wParam;
				int is_pressed = (uMsg == WM_KEYDOWN) ? 1 : 0;
				window->native_window->input->callbacks.key_callback(key, is_pressed);
			}
			break;
		case WM_LBUTTONDOWN:

		case WM_MOUSEWHEEL:
			/*if (window->input_callbacks->mouse_wheel_callback)
			{
				float value = (float)GET_WHEEL_DELTA_WPARAM(wParam);
				window->input_callbacks->mouse_wheel_callback(value);
			}*/
			break;
		case WM_RBUTTONDOWN:
			/* if (window->input_callbacks->mouse_callback) {
				 int button = (int)wParam;
				 int is_pressed = (uMsg == WM_MBUTTONDOWN) ? 1 : 0;
				 window->input_callbacks->mouse_callback(button);
			 }*/
			break;
		case WM_SIZE:
			window->width = LOWORD(lParam);
			window->height = HIWORD(lParam);
			if (window->callbacks.resize)
			{
				window->callbacks.resize(window->width, window->height);
			}
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			if (window)
			{
				window->native_window->is_running = false;
			}
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			if (window)
			{
				window->native_window->is_running = false;

				//if(window->callbacks.close)
				//	window->callbacks.close();
			}
			return 0;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void swWin32CreateWindow(swWindow* window, int width, int height, const char* title)
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"SwindowWindowClass";

	if (!RegisterClass(&wc)) return;

	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	int adjustedWidth = rect.right - rect.left;
	int adjustedHeight = rect.bottom - rect.top;

	const HWND hwnd = CreateWindowEx(0, wc.lpszClassName, ConvertCharArrayToLPCWSTR(title), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, adjustedWidth, adjustedHeight,
		NULL, NULL, wc.hInstance, NULL);
	if (!hwnd) return;

	const HDC hdc = GetDC(hwnd);
	const PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1,
								  PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
								  PFD_TYPE_RGBA, 32 };
	const int pf = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pf, &pfd);

	const HGLRC glContext = wglCreateContext(hdc);
	wglMakeCurrent(hdc, glContext);

	swNativeWindow* nativeWindow = malloc(sizeof(swNativeWindow));
	nativeWindow->hwnd = hwnd;
	nativeWindow->hdc = hdc;
	nativeWindow->gl_context = glContext;
	nativeWindow->is_running = true;
	nativeWindow->wp_prev.length = sizeof(WINDOWPLACEMENT);
	window->native_window = nativeWindow;

	ShowWindow(window->native_window->hwnd, SW_SHOW);
	UpdateWindow(window->native_window->hwnd);

	SetWindowLongPtr(window->native_window->hwnd, GWLP_USERDATA, (LONG_PTR)window);

	window->width = width;
	window->height = height;
}


void swWin32SetFullscreen(swWindow* window)
{
	if (!window->native_window) return;

	DWORD dwStyle = GetWindowLong(window->native_window->hwnd, GWL_STYLE);

	if (dwStyle & WS_OVERLAPPEDWINDOW) 
	{
		MONITORINFO mi = { sizeof(mi) };
		if (GetWindowPlacement(window->native_window->hwnd, &window->native_window->wp_prev) &&
			GetMonitorInfo(MonitorFromWindow(window->native_window->hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {

			SetWindowLong(window->native_window->hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

			SetWindowPos(window->native_window->hwnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER);

			ShowWindow(window->native_window->hwnd, SW_MAXIMIZE);
		}
	}
	else 
	{
		SetWindowLong(window->native_window->hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(window->native_window->hwnd, &window->native_window->wp_prev);
		SetWindowPos(window->native_window->hwnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		ShowWindow(window->native_window->hwnd, SW_RESTORE);
	}
}


void swWin32SwapBuffers(swWindow* window)
{
	SwapBuffers(GetDC(window->native_window->hwnd));
}

void swWin32PollEvents(swWindow* window)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			if(window->callbacks.close)
			{
				window->callbacks.close();
			}

			window->native_window->is_running = false;
		}
	}
}

void swWin32DestroyWindow(swNativeWindow* nw)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(nw->gl_context);
	ReleaseDC(nw->hwnd, nw->hdc);
	DestroyWindow(nw->hwnd);
	free(nw);
}
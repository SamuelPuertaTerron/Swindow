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
				const int key = (int)wParam;
				const int is_pressed = (uMsg == WM_KEYDOWN) ? 1 : 0;
				window->native_window->input->callbacks.key_callback(key, is_pressed);
			}
			break;
		case WM_LBUTTONDOWN:
			if (window->native_window->input->callbacks.mouse_callback) {
				const int button = (int)wParam;
				window->native_window->input->callbacks.mouse_callback(button);
			}
			break;
		case WM_MOUSEWHEEL:
			if (window->native_window->input->callbacks.mouse_wheel_callback)
			{
				const double value = GET_WHEEL_DELTA_WPARAM(wParam);
				window->native_window->input->callbacks.mouse_wheel_callback(value);
			}
			break;
		case WM_RBUTTONDOWN:
			if (window->native_window->input->callbacks.mouse_callback) {
				 const int button = (int)wParam;
				 window->native_window->input->callbacks.mouse_callback(button);
			 }
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
	const int adjustedWidth = rect.right - rect.left;
	const int adjustedHeight = rect.bottom - rect.top;

	const HWND hwnd = CreateWindowEx(0, wc.lpszClassName, ConvertCharArrayToLPCWSTR(title), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, adjustedWidth, adjustedHeight,
		NULL, NULL, wc.hInstance, NULL);
	if (!hwnd) return;

	const HDC hdc = GetDC(hwnd);

	swNativeWindow* nativeWindow = malloc(sizeof(swNativeWindow));
	if (!nativeWindow) return;

	nativeWindow->hwnd = hwnd;
	nativeWindow->hdc = hdc;
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

void swWin32CreateContext(swWindow* window)
{
	const PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR), // Size of the descriptor
	1,                             // Version number
	PFD_DRAW_TO_WINDOW |           // Support window drawing
	PFD_SUPPORT_OPENGL |           // Support OpenGL
	PFD_DOUBLEBUFFER,              // Enable double buffering
	PFD_TYPE_RGBA,                 // RGBA color mode
	32,                            // 32-bit color depth
	0, 0, 0, 0, 0, 0, // Ignore color bits
	0,                             // No alpha buffer
	0,                             // Ignore shift bit
	0,                             // No accumulation buffer
	0, 0, 0, 0, // Ignore accumulation bits
	24,                            // 24-bit z-buffer
	8,                             // 8-bit stencil buffer
	0,                             // No auxiliary buffer
	PFD_MAIN_PLANE,                // Main drawing layer
	0,                             // Reserved
	0, 0, 0 // Layer masks ignored
	};

	const int pf = ChoosePixelFormat(window->native_window->hdc, &pfd);
	SetPixelFormat(window->native_window->hdc, pf, &pfd);

	const HGLRC glContext = wglCreateContext(window->native_window->hdc);
	wglMakeCurrent(window->native_window->hdc, glContext);

	window->native_window->gl_context = glContext;
}

void* swWin32GetProcAddress(const char* name)
{
	void* proc = (void*)wglGetProcAddress(name);
	if (!proc) {
		return NULL;
	}
	return proc;
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
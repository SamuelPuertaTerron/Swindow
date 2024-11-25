#include "Swindow.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*struct InputCallbacks
{
	KeyCallback key_callback;
	MouseCallback mouse_callback;
	MouseWheelCallback mouse_wheel_callback;
};

struct WindowCallbacks
{
	ResizeCallback resize_callback;
	WindowCloseCallback close_callback;
};

#ifdef SW_PLATFORM_WINDOWS
#include <Windows.h>
#include <GL/GL.h>
#pragma comment (lib, "opengl32.lib")


LPCWSTR convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString;
	int stringLength = MultiByteToWideChar(CP_ACP, 0, charArray, -1, NULL, 0);
	wString = (wchar_t*)malloc(stringLength * sizeof(wchar_t));
	if (wString == NULL) return NULL;
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, stringLength);
	return wString;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SwindowWindow* window = (SwindowWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	if (window) {
		if ((window->input_callbacks->key_callback || window->input_callbacks->mouse_callback))
		{
			switch (uMsg)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
				if (window->input_callbacks->key_callback) 
				{
					int key = (int)wParam;
					int is_pressed = (uMsg == WM_KEYDOWN) ? 1 : 0;
					window->input_callbacks->key_callback(key, is_pressed);
				}
				break;
			case WM_LBUTTONDOWN:
				if (window->input_callbacks->mouse_callback)
				{
					int button = (int)wParam;
					window->input_callbacks->mouse_callback(button);
				}
				break;;
			case WM_MOUSEWHEEL:
				if(window->input_callbacks->mouse_wheel_callback)
				{
					float value = (float)GET_WHEEL_DELTA_WPARAM(wParam);
					window->input_callbacks->mouse_wheel_callback(value);
				}
				break;
			case WM_RBUTTONDOWN:
				if (window->input_callbacks->mouse_callback) {
					int button = (int)wParam;
					int is_pressed = (uMsg == WM_MBUTTONDOWN) ? 1 : 0;
					window->input_callbacks->mouse_callback(button);
				}
				break;
			case WM_SIZE:
				window->width = LOWORD(lParam);
				window->height = HIWORD(lParam);
				if(window->window_callbacks && window->window_callbacks->resize_callback)
				{
					window->window_callbacks->resize_callback(window->width, window->height);
				}
				break;
			}
		}
	}

	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		if (window)
		{
			window->is_window_running = false;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		if (window)
		{
			window->is_window_running = false;
		}
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

SwindowWindow* sw_window_create(int width, int height, const char* title)
{
	SwindowWindow* window = malloc(sizeof(SwindowWindow));
	if (!window) return NULL;

	window->width = width;
	window->height = height;
	window->is_window_running = true;
	window->input_callbacks = NULL;
	window->window_callbacks = NULL;

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, convertCharArrayToLPCWSTR(title), NULL };
	RegisterClassEx(&wc);

	sw_create_native_window(window);

	memset(&window->native_window->wp_prev, 0, sizeof(window->native_window->wp_prev));
	window->native_window->wp_prev.length = sizeof(window->native_window->wp_prev);

	window->native_window->hwnd = CreateWindowEx(0, wc.lpszClassName, convertCharArrayToLPCWSTR(title), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, wc.hInstance, NULL);

	ShowWindow(window->native_window->hwnd, SW_SHOWDEFAULT);
	UpdateWindow(window->native_window->hwnd);

	SetWindowLongPtr(window->native_window->hwnd, GWLP_USERDATA, (LONG_PTR)window);

	WindowCallbacks* window_callback = sw_window_callbacks_create();
	window->window_callbacks = window_callback;

	window->window_callbacks->resize_callback = sw_default_window_resize;
	window->window_callbacks->close_callback = sw_default_window_close;

	InputCallbacks* input = sw_input_create();
	window->input_callbacks = input;

	window->input_callbacks->key_callback = sw_default_key_callback;
	window->input_callbacks->mouse_callback = sw_default_mouse_callback;
	window->input_callbacks->mouse_wheel_callback = sw_default_mouse_wheel_callback;

	sw_toggle_fullscreen(window->native_window);

	return window;
}

WindowCallbacks* sw_window_callbacks_create()
{
	WindowCallbacks* callback = malloc(sizeof(WindowCallbacks));
	return callback;
}

void sw_window_callbacks_destroy(WindowCallbacks* callback)
{
	free(callback);
}

void sw_window_set_resize_callback(SwindowWindow* window, ResizeCallback resize)
{
	window->window_callbacks->resize_callback = resize;
}

void sw_window_set_close_callback(SwindowWindow* window, WindowCloseCallback close)
{
	window->window_callbacks->close_callback = close;
}

void sw_window_set_callbacks(SwindowWindow* window, WindowCallbacks* callback)
{
	window->window_callbacks = callback;
}

void sw_window_create_context(SwindowWindow* window)
{
	if (!window) return;

	HDC hdc = GetDC(window->native_window->hwnd);
	PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);
	HGLRC hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);
}

void* sw_get_proc_address(const char* proc)
{
	PROC procAddress = wglGetProcAddress(proc);

	// In some cases, wglGetProcAddress might return NULL for core functions
	// If that's the case, we fall back to GetProcAddress from opengl32.dll
	if (procAddress == NULL) {
		HMODULE module = LoadLibraryA("opengl32.dll");
		if (module != NULL) {
			procAddress = GetProcAddress(module, proc);
			FreeLibrary(module); 
		}
	}

	return (void*)procAddress;
}

void sw_toggle_fullscreen(SwindowNativeWindow* native_window)
{
	DWORD dwStyle = GetWindowLong(native_window->hwnd, GWL_STYLE);

	if (dwStyle & WS_OVERLAPPEDWINDOW) {
		// Save the current window placement and style
		MONITORINFO mi = { sizeof(mi) };
		if (GetWindowPlacement(native_window->hwnd, &native_window->wp_prev) &&
			GetMonitorInfo(MonitorFromWindow(native_window->hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
			// Remove window decorations
			SetWindowLong(native_window->hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			// Set window size to cover the entire monitor area
			SetWindowPos(native_window->hwnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else {
		// Restore the window's style and placement
		SetWindowLong(native_window->hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(native_window->hwnd, &native_window->wp_prev);
		SetWindowPos(native_window->hwnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

void sw_window_destroy(SwindowWindow* window)
{
	if (window) {
		sw_window_callbacks_destroy(window->window_callbacks);

		sw_input_destroy(window->input_callbacks);

		DestroyWindow(window->native_window->hwnd);
		sw_destroy_native_window(window);
		free(window);
	}

	printf("Window Destroyed");
}

void sw_window_poll_events(SwindowWindow* window)
{
	MSG msg;
	while (PeekMessage(&msg, window ? window->native_window->hwnd : NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) {
			if (window)
			{
				window->is_window_running = false;
			}
			break;
		}
	}
}

void sw_window_swap_buffers(SwindowWindow* window)
{
	if (window) SwapBuffers(GetDC(window->native_window->hwnd));
}

bool sw_window_is_running(SwindowWindow* window)
{
	return window ? window->is_window_running : false;
}

void sw_set_window_is_running(SwindowWindow* window, bool value)
{
	window->is_window_running = value;
}

void sw_window_shutdown(void)
{
	// Platform-specific shutdown code (Windows)
}

void sw_default_window_resize(int x, int y)
{
	
}

void sw_default_window_close(void)
{
	
}

void sw_create_native_window(SwindowWindow* window)
{
	window->native_window = malloc(sizeof(SwindowNativeWindow));
}

SwindowNativeWindow* sw_get_native_window(SwindowWindow* window)
{
	return window->native_window;
}

void sw_destroy_native_window(SwindowWindow* window)
{
	free(window->native_window);
}

InputCallbacks* sw_input_create()
{
	InputCallbacks* input = malloc(sizeof(InputCallbacks));
	return input;
}

void sw_input_destroy(InputCallbacks* callback)
{
	free(callback);
}

void sw_input_set_key_callback(SwindowWindow* window, KeyCallback key)
{
	window->input_callbacks->key_callback = key;
}

void sw_input_set_mouse_callback(SwindowWindow* window, MouseCallback mouse)
{
	window->input_callbacks->mouse_callback = mouse;
}

void sw_input_set_mouse_wheel_callback(SwindowWindow* window, MouseWheelCallback mouse_wheel)
{
	window->input_callbacks->mouse_wheel_callback = mouse_wheel;
}

void sw_default_key_callback(int key, int is_pressed)
{
}

void sw_default_mouse_callback(int key)
{
}

void sw_default_mouse_wheel_callback(float value)
{
}

void sw_gl_display_current_version(void)
{
	MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
}

void sw_gl_display_current_vendor(void)
{
	MessageBoxA(0, (char*)glGetString(GL_VENDOR), "OPENGL VENDOR", 0);
}

void sw_gl_display_current_renderer(void)
{
	MessageBoxA(0, (char*)glGetString(GL_RENDERER), "OPENGL RENDERER", 0);
}

void sw_gl_display_current_extension(void)
{
	MessageBoxA(0, (char*)glGetString(GL_EXTENSIONS), "OPENGL RENDERER", 0);
}

void sw_gl_set_clear_colour(float r, float g, float b, float a)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(r, g, b, a);
}

void sw_gl_set_viewport_size(int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif // SW_PLATFORM_WINDOWS

#ifdef SW_PLATFORM_LINUX
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <stdlib.h>

struct SwindowWindow {
	Display* display;
	Window win;
	GLXContext gl_context;
	int width;
	int height;
	int window_close;
};

SwindowWindow* window_create(int width, int height, const char* title) {
	SwindowWindow* window = malloc(sizeof(SwindowWindow));
	window->width = width;
	window->height = height;
	window->window_close = 0;

	window->display = XOpenDisplay(NULL);
	if (window->display == NULL) {
		return NULL;  // Failed to open display
	}

	int screen = DefaultScreen(window->display);
	Window root = RootWindow(window->display, screen);

	XSetWindowAttributes attr;
	attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;
	window->win = XCreateWindow(window->display, root, 0, 0, width, height, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &attr);

	XStoreName(window->display, window->win, title);
	XMapWindow(window->display, window->win);

	// OpenGL context creation
	GLint attribs[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };
	XVisualInfo* vi = glXChooseVisual(window->display, 0, attribs);
	window->gl_context = glXCreateContext(window->display, vi, NULL, GL_TRUE);
	glXMakeCurrent(window->display, window->win, window->gl_context);

	return window;
}

void create_context(SwindowWindow* window) {
	// OpenGL context is already created in window_create
}

void window_destroy(SwindowWindow* window) {
	if (window) {
		glXDestroyContext(window->display, window->gl_context);
		XDestroyWindow(window->display, window->win);
		XCloseDisplay(window->display);
		free(window);
	}
}

void window_poll_events(SwindowWindow* window) {
	XEvent event;
	while (XPending(window->display) > 0) {
		XNextEvent(window->display, &event);
		if (event.type == ClientMessage) {
			window->window_close = 1;
		}
	}
}

void window_main_loop(SwindowWindow* window) {
	while (!window_should_close(window)) {
		window_poll_events(window);
		glXSwapBuffers(window->display, window->win);
	}
}

int window_should_close(SwindowWindow* window) {
	return window->window_close;
}

void window_shutdown(void) {
	// Any platform-specific shutdown code (Linux)
}
#endif*/


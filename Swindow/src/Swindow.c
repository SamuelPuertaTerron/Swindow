#include "Swindow.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef SW_PLATFORM_WINDOWS
#include <Windows.h>
#include <GL/GL.h>
#pragma comment (lib, "opengl32.lib")

struct InputCallbacks
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

struct SwindowWindow
{
	HWND hwnd;
	int width;
	int height;
	int window_close;
	InputCallbacks* input_callbacks;
	WindowCallbacks* window_callbacks;
};

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
				if (window->input_callbacks->key_callback) {
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
				window->window_callbacks->resize_callback(window->width, window->height);
				break;
			}
		}
	}

	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		if (window)
		{
			window->window_close = 1;
			window->window_callbacks->close_callback(1);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		if (window)
		{
			window->window_close = 1;
			window->window_callbacks->close_callback(1);
		}
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

SwindowWindow* window_create(int width, int height, const char* title)
{
	SwindowWindow* window = malloc(sizeof(SwindowWindow));
	if (!window) return NULL;

	window->width = width;
	window->height = height;
	window->window_close = 0;
	window->input_callbacks = NULL;

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, convertCharArrayToLPCWSTR(title), NULL };
	RegisterClassEx(&wc);

	window->hwnd = CreateWindowEx(0, wc.lpszClassName, convertCharArrayToLPCWSTR(title), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, wc.hInstance, NULL);

	ShowWindow(window->hwnd, SW_SHOWDEFAULT);
	UpdateWindow(window->hwnd);

	SetWindowLongPtr(window->hwnd, GWLP_USERDATA, (LONG_PTR)window);

	WindowCallbacks* window_callback = window_callbacks_create();
	window->window_callbacks = window_callback;

	InputCallbacks* input = input_create();
	window->input_callbacks = input;

	return window;
}

WindowCallbacks* window_callbacks_create()
{
	WindowCallbacks* callback = malloc(sizeof(WindowCallbacks));
	return callback;
}

void window_callbacks_destroy(WindowCallbacks* callback)
{
	free(callback);
}

void window_set_resize_callback(SwindowWindow* window, ResizeCallback resize)
{
	window->window_callbacks->resize_callback = resize;
}

void window_set_close_callback(SwindowWindow* window, WindowCloseCallback close)
{
	window->window_callbacks->close_callback = close;
}

void window_set_callbacks(SwindowWindow* window, WindowCallbacks* callback)
{
	window->window_callbacks = callback;
}

void window_create_context(SwindowWindow* window)
{
	if (!window) return;

	HDC hdc = GetDC(window->hwnd);
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

void window_destroy(SwindowWindow* window)
{
	if (window) {
		window_callbacks_destroy(window->window_callbacks);

		input_destroy(window->input_callbacks);

		DestroyWindow(window->hwnd);
		free(window);
	}
}

void window_poll_events(SwindowWindow* window)
{
	MSG msg;
	while (PeekMessage(&msg, window ? window->hwnd : NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) {
			if (window) window->window_close = 1;
			break;
		}
	}
}

void window_swap_buffers(SwindowWindow* window)
{
	if (window) SwapBuffers(GetDC(window->hwnd));
}

int window_should_close(SwindowWindow* window)
{
	return window ? window->window_close : 1;
}

void window_shutdown(void)
{
	// Platform-specific shutdown code (Windows)
}

InputCallbacks* input_create()
{
	InputCallbacks* input = malloc(sizeof(InputCallbacks));
	return input;
}

void input_destroy(InputCallbacks* callback)
{
	free(callback);
}

void input_set_key_callback(SwindowWindow* window, KeyCallback key)
{
	window->input_callbacks->key_callback = key;
}

void input_set_mouse_callback(SwindowWindow* window, MouseCallback mouse)
{
	window->input_callbacks->mouse_callback = mouse;
}

void input_set_mouse_wheel_callback(SwindowWindow* window, MouseWheelCallback mouse_wheel)
{
	window->input_callbacks->mouse_wheel_callback = mouse_wheel;
}

void gl_display_current_version(void)
{
	MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
}

void gl_display_current_vendor(void)
{
	MessageBoxA(0, (char*)glGetString(GL_VENDOR), "OPENGL VENDOR", 0);
}

void gl_display_current_renderer(void)
{
	MessageBoxA(0, (char*)glGetString(GL_RENDERER), "OPENGL RENDERER", 0);
}

void gl_display_current_extension(void)
{
	MessageBoxA(0, (char*)glGetString(GL_EXTENSIONS), "OPENGL RENDERER", 0);
}

void gl_set_clear_colour(float r, float g, float b, float a)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(r, g, b, a);
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
#endif

#include "Swindow.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef SW_PLATFORM_WINDOWS
#include <Windows.h>
#include <GL/GL.h>
#pragma comment (lib, "opengl32.lib")

struct InputCallbacks
{
	KeyCallback keyCallback;
	MouseCallback mouseCallback;
};

struct WindowCallbacks
{
	ResizeCallback resize_callback;
};

struct Window
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
	Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	if (window) {
		if ((window->input_callbacks->keyCallback || window->input_callbacks->mouseCallback)) 
		{
			switch (uMsg)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
				if (window->input_callbacks->keyCallback) {
					int key = (int)wParam;
					int is_pressed = (uMsg == WM_KEYDOWN) ? 1 : 0;
					window->input_callbacks->keyCallback(key, is_pressed);
				}
				break;
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				if (window->input_callbacks->mouseCallback) {
					int key = (int)wParam;
					int is_pressed = (uMsg == WM_LBUTTONDOWN) ? 1 : 0;
					window->input_callbacks->mouseCallback(key, is_pressed);
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
		if (window) window->window_close = 1;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		if (window) window->window_close = 1;
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Window* window_create(int width, int height, const char* title)
{
	Window* window = malloc(sizeof(Window));
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

void window_callbacks_set_resize(WindowCallbacks* callback, ResizeCallback resize)
{
	callback->resize_callback = resize;
}

void window_set_callbacks(Window* window, WindowCallbacks* callback)
{
	window->window_callbacks = callback;
}

void window_create_context(Window* window)
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

void window_destroy(Window* window)
{
	if (window) {
		DestroyWindow(window->hwnd);
		free(window);
	}
}

void window_poll_events(Window* window)
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

void window_swap_buffers(Window* window)
{
	if (window) SwapBuffers(GetDC(window->hwnd));
}

int window_should_close(Window* window)
{
	return window ? window->window_close : 1;
}

void window_shutdown(void)
{
	// Platform-specific shutdown code (Windows)
}

void input_set_callbacks(Window* window, InputCallbacks* callbacks)
{
	window->input_callbacks = callbacks;
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

void input_set_window_callbacks(Window* window, InputCallbacks* callback)
{
	window->input_callbacks = callback;
}

void input_set_key_callback(InputCallbacks* callback, KeyCallback key)
{
	callback->keyCallback = key;
}

void input_set_mouse_callback(InputCallbacks* callback, MouseCallback mouse)
{
	callback->mouseCallback = mouse;
}

void gl_display_current__version(void) {
	MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
}

void gl_set_clear_colour(float r, float g, float b, float a) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(r, g, b, a);
}

#endif // SW_PLATFORM_WINDOWS

#ifdef SW_PLATFORM_LINUX
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <stdlib.h>

struct Window {
	Display* display;
	Window win;
	GLXContext gl_context;
	int width;
	int height;
	int window_close;
};

Window* window_create(int width, int height, const char* title) {
	Window* window = malloc(sizeof(Window));
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

void create_context(Window* window) {
	// OpenGL context is already created in window_create
}

void window_destroy(Window* window) {
	if (window) {
		glXDestroyContext(window->display, window->gl_context);
		XDestroyWindow(window->display, window->win);
		XCloseDisplay(window->display);
		free(window);
	}
}

void window_poll_events(Window* window) {
	XEvent event;
	while (XPending(window->display) > 0) {
		XNextEvent(window->display, &event);
		if (event.type == ClientMessage) {
			window->window_close = 1;
		}
	}
}

void window_main_loop(Window* window) {
	while (!window_should_close(window)) {
		window_poll_events(window);
		glXSwapBuffers(window->display, window->win);
	}
}

int window_should_close(Window* window) {
	return window->window_close;
}

void window_shutdown(void) {
	// Any platform-specific shutdown code (Linux)
}
#endif

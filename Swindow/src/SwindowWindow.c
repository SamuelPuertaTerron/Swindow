#include "SwindowWindow.h"

#ifdef _WIN32
#include "Backends/SwindowWin32.h"
#elif __LINUX__
#include "Backends/SwindowX11.h"
#else
#error "Unsported Platform"
#endif

swWindow* swCreateWindow(int width, int height, const char* title)
{
	swWindow* window = malloc(sizeof(swWindow));
	if (!window) return NULL;

	window->width = width;
	window->height = height;

#ifdef _WIN32
	swWin32CreateWindow(window, width, height, title);
#elif __LINUX__

#endif

	return window;
}

void swCreateContext(swWindow* window)
{
#ifdef _WIN32
	swWin32CreateContext(window);
#elif __LINUX__

#endif
}

void swGetProcAddress(const char* name)
{
#ifdef _WIN32
	swWin32GetProcAddress(name);
#elif __LINUX__

#endif
}

void swCreateCloseWindowCallback(swWindow* window, swOnCloseCallback close)
{
	window->callbacks.close = close;
}

void swCreateResizeWindowCallback(swWindow* window, swOnResizeCallback resize)
{
	window->callbacks.resize = resize;
}

void swSetFullscreen(swWindow* window)
{
#ifdef _WIN32
	swWin32SetFullscreen(window);
#elif __LINUX__

#endif
}

bool swGetIsWindowRunning(swWindow* window)
{
	return window->native_window->is_running;
}

void swSetIsWindowRunning(swWindow* window, bool value)
{
	window->native_window->is_running = value;
}

void swSwapBuffers(swWindow* window)
{
#ifdef _WIN32
	swWin32SwapBuffers(window);
#elif __LINUX__

#endif
}

void swPollEvents(swWindow* window)
{

#ifdef _WIN32
	swWin32PollEvents(window);
#elif __LINUX__

#endif
}

void swDestroyWindow(swWindow* window)
{

#ifdef _WIN32
	swWin32DestroyWindow(window->native_window);
#elif __LINUX__

#endif

	free(window);
}
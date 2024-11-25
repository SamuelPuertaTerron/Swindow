#include "SwindowInput.h"

#include <stdlib.h>
#ifdef _WIN32
#include "Backends/SwindowWin32.h"
#elif __LINUX__
#include "Backends/SwindowX11.h"
#else
#error "Unsported Platform"
#endif

int swCreateInput(swWindow* window)
{
	window->native_window->input = malloc(sizeof(swInput));

	return 0;
}

void swCreateKeyCallback(swWindow* window, swKeyCallback KeyCallback)
{
	window->native_window->input->callbacks.key_callback = KeyCallback;
}

void swCreateMouseCallback(swWindow* window, swMouseCallback mouseCallback)
{
	window->native_window->input->callbacks.mouse_callback = mouseCallback;
}

void swCreateMouseWheelCallback(swWindow* window, swMouseWheelCallback mouseCallback)
{
	window->native_window->input->callbacks.mouse_wheel_callback = mouseCallback;
}

void swInputDestroy(swWindow* window)
{
	free(window->native_window->input);
}

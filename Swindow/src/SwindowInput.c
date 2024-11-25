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

void swCreateKeyCallback(swWindow* Window, swKeyCallback KeyCallback)
{
	Window->native_window->input->callbacks.key_callback = KeyCallback;
}

void swInputDestroy(swWindow* window)
{
	free(window->native_window->input);
}

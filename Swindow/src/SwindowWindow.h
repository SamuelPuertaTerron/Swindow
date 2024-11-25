#ifndef SWINDOW_WINDOW_H
#define SWINDOW_WINDOW_H

#include <stdbool.h>
//#include "SwindowInput.h"

typedef struct SwindowNativeWindow swNativeWindow;

typedef void(*swOnResizeCallback)(int width, int height);
typedef void(*swOnCloseCallback)(void);

typedef struct SwindowWindowCallbacks
{
	swOnCloseCallback close;
	swOnResizeCallback resize;

} swWindowCallbacks;

typedef struct SwindowWindow
{
	swNativeWindow* native_window;
	int width;
	int height;
	swWindowCallbacks callbacks;
	//swInput* input;

} swWindow;

swWindow* swCreateWindow(int width, int height, const char* title);

void swCreateContext(swWindow* window);

void swCreateCloseWindowCallback(swWindow* window, swOnCloseCallback close);
void swCreateResizeWindowCallback(swWindow* window, swOnResizeCallback resize);

void swSetFullscreen(swWindow* window);

bool swGetIsWindowRunning(swWindow* window);
void swSetIsWindowRunning(swWindow* window, bool value);

void swSwapBuffers(swWindow* window);
void swPollEvents(swWindow* window);

void swDestroyWindow(swWindow* window);

#endif //SWINDOW_WINDOW_H
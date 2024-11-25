#ifndef SWINDOW_INPUT_H
#define SWINDOW_INPUT_H

#include "SwindowWindow.h"

typedef void (*swKeyCallback)(int key, int is_Pressed);
typedef void(*swMouseCallback)(int button);

typedef struct SwindowInputCallbacks
{
	swKeyCallback key_callback;
	swMouseCallback mouse_callback;
} swInputCallbacks;

typedef struct SwindowInput
{
	int code;
	swInputCallbacks callbacks;
} swInput;

int swCreateInput(swWindow* window);

void swCreateKeyCallback(swWindow* window, swKeyCallback KeyCallback);

void swInputDestroy(swWindow* window);

#endif // SWINDOW_INPUT_H
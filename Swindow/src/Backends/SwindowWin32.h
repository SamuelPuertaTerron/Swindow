/*
 * Windows 32 API implementation for creating a window
 *
 */

#ifndef SWINDOW_WIN32_H
#define SWINDOW_WIN32_H

#include <stdbool.h>
#include <Windows.h>
#include "../SwindowWindow.h"
#include "../SwindowInput.h"

//============ Windows =============================//

typedef struct SwindowNativeWindow
{
    HWND hwnd;
    HDC hdc;
    HGLRC gl_context;
    bool is_running;
    WINDOWPLACEMENT wp_prev;

    swInput* input; //TODO: Remove this from the native window

} swNativeWindow;

void swWin32CreateWindow(swWindow* window, int width, int height, const char* title);

void swWin32SetFullscreen(swWindow* window);

void swWin32SwapBuffers(swWindow* window);
void swWin32PollEvents(swWindow* window);

void swWin32DestroyWindow(swNativeWindow* nw);

//==================================================//

#endif //SWINDOW_WIN32_H
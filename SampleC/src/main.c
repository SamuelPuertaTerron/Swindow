/*
 *	This is an simple example of how to setup a window and input
 */

#include <stdio.h>
#include "Swindow.h"

swWindow* g_Window;
bool g_Fullscreen = false;

static void key_callback(int key, int is_pressed)
{
	if(is_pressed == 0)
	{
		if(key == 27) //Escape
		{
			swSetIsWindowRunning(g_Window, false);
		}

		if (key == 122) //F11
		{
			swSetFullscreen(g_Window);
		}
	}
}

static void mouse_callback(int button)
{
	printf("Key %d pressed \n", button);
}

static void mouse_wheel_callback(double value)
{
	printf("Mouse Wheel Moved %f \n", value);
}

static void resize_callback(int width, int height)
{
	printf("window resized [%d, %d] \n", width, height);
}

static void close(void)
{
	printf("Window Closed");
}

int main(void)
{
	g_Window = swCreateWindow(1270, 720, "Hello from Swindow");

	swCreateContext(g_Window);

	swCreateResizeWindowCallback(g_Window, resize_callback);
	swCreateCloseWindowCallback(g_Window, close);

	//Create Input system
	swCreateInput(g_Window);
	swCreateKeyCallback(g_Window, key_callback);
	swCreateMouseCallback(g_Window, mouse_callback);
	swCreateMouseWheelCallback(g_Window, mouse_wheel_callback);

	while(swGetIsWindowRunning(g_Window))
	{
		swSwapBuffers(g_Window);
		swPollEvents(g_Window);
	}

	swDestroyWindow(g_Window);
	return 0;
}

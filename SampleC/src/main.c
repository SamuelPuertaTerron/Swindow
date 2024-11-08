#include <stdio.h>
#include "Swindow.h"

void key_callback(int key, int is_pressed)
{
	if (is_pressed == 1) 
	{
		printf("Key %d pressed\n", key); 
	}
}

void mouse_callback(int button, int is_pressed)
{
	if(is_pressed == 1)
	{
		printf("Key %d pressed \n", button);
	}
}

void resize_callback(int width, int height)
{
	printf("Window changed size to {%d, %d} \n", width, height);
}

int main(void)
{
	Window* window = window_create(1270, 720, "Hello World");

	InputCallbacks* input = input_create();
	input_set_window_callbacks(window, input);
	input_set_mouse_callback(input, mouse_callback);
	input_set_key_callback(input, key_callback);

	WindowCallbacks* window_callback = window_callbacks_create();
	window_callbacks_set_resize(window_callback, resize_callback);
	window_set_callbacks(window, window_callback);

	window_create_context(window);

	gl_display_current__version();

	while (!window_should_close(window))
	{
		window_poll_events(window);

		gl_set_clear_colour(0.0f, 0.0f, 1.0f, 1.0f);

		window_swap_buffers(window);
	}

	window_callbacks_destroy(window_callback);

	input_destroy(input);

	window_destroy(window);

	return 0;
}

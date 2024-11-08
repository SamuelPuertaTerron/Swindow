#include <stdio.h>
#include "Swindow.h"

void key_callback(int key, int is_pressed)
{
	if (is_pressed == 1) 
	{
		printf("Key %d pressed\n", key); 
	}
}

void mouse_callback(int button)
{
	printf("Key %d pressed \n", button);
}

void mouse_wheel_callback(float value)
{
	printf("Mouse Wheel Moved %f \n", value);
}

void resize_callback(int width, int height)
{
	printf("Window changed size to {%d, %d} \n", width, height);
}

int main(void)
{
	SwindowWindow* window = window_create(1270, 720, "Hello World");

	input_set_mouse_callback(window, mouse_callback);
	input_set_key_callback(window, key_callback);
	input_set_mouse_wheel_callback(window, mouse_wheel_callback);

	window_set_resize_callback(window, resize_callback);

	window_create_context(window);

	gl_display_current_version();
	gl_display_current_vendor();
	gl_display_current_extension();
	gl_display_current_renderer();


	while (!window_should_close(window))
	{
		window_poll_events(window);

		gl_set_clear_colour(0.0f, 0.0f, 1.0f, 1.0f);

		window_swap_buffers(window);
	}

	window_destroy(window);

	return 0;
}

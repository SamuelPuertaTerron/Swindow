#ifndef SWINDOW_H
#define SWINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma region Window


    typedef void(*KeyCallback)(int key, int is_pressed);
    typedef void(*MouseCallback)(int key);
    typedef void(*MouseWheelCallback)(float value);

    typedef struct InputCallbacks InputCallbacks;

    typedef struct WindowCallbacks WindowCallbacks;

    typedef void(*ResizeCallback)(int x, int y);
    typedef void(*WindowCloseCallback)(int close);

    // Define a structure that holds platform-specific data
    typedef struct Window Window;

    // Create and initialize the window
    Window* window_create(int width, int height, const char* title);

    WindowCallbacks* window_callbacks_create();
    void window_callbacks_destroy(WindowCallbacks* callback);

    void window_set_resize_callback(Window* window, ResizeCallback resize);
    void window_set_close_callback(Window* window, WindowCloseCallback close);

    void window_set_callbacks(Window* window, WindowCallbacks* callback);

    void window_create_context(Window* window);

    // Destroy the window
    void window_destroy(Window* window);

    // Handle window events
    void window_poll_events(Window* window);

    // Main loop for the window
    void window_swap_buffers(Window* window);

    int window_should_close(Window* window);

    // Platform-specific function (for cleanup, etc.)
    void window_shutdown(void);

#pragma endregion

#pragma region Input

    InputCallbacks* input_create();
    void input_destroy(InputCallbacks* callback);

    //Callbacks
    void input_set_key_callback(Window* window, KeyCallback key);
    void input_set_mouse_callback(Window* window, MouseCallback mouse);
    void input_set_mouse_wheel_callback(Window* window, MouseWheelCallback mouse_wheel);

#pragma endregion

#pragma region OpenGL
    void gl_display_current_version(void);
    void gl_display_current_vendor(void);
    void gl_display_current_renderer(void);
    void gl_display_current_extension(void);

    void gl_set_clear_colour(float r, float g, float b, float a);
#pragma endregion

#ifdef __cplusplus
}
#endif

#endif // SWINDOW_H
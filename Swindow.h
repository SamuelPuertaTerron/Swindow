// Swindow - Simple Window
// 
// Swindow is a lightweight, cross-platform library for managing windows and handling user input.
// Licensed under the MIT License. See LICENSE.txt for details.

#pragma once

#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <typeindex>
#include <unordered_map>

#ifdef _WIN32
#include <Windows.h>
 //Include for OpenGL; including context creation.
#include <GL/GL.h>
#pragma comment (lib, "opengl32.lib")
#endif

namespace Swindow
{
	//Forward Declarations

	enum class KeyCode : uint8_t;
	enum class MouseButton;

	class Window;

	namespace Internal
	{
		class NativeWindow;
		class RenderContext;
	}

	//Types
	using WindowPtr = std::shared_ptr<Window>;

	//The render class (does not work in modern OpenGL)
	using Render = Internal::RenderContext;

	namespace Internal
	{
		using NativeWindowPtr = std::shared_ptr<Internal::NativeWindow>;
	}

	//Callbacks

	/**
	 * @brief Callback type for window close events.
	 *
	 * This callback is triggered when the user attempts to close the window.
	 *
	 * If a WindowCloseCallback is set, the application will invoke it.
	 * - If the callback returns `true`, the window will close, and the application will stop running.
	 * - If the callback returns `false`, the window will remain open, and the application will continue running.
	 *
	 * If no callback is set, the window will close by default.
	 *
	 * @return `true` to allow closing the window, `false` to keep it open.
	 */
	using WindowCloseCallback = std::function<bool()>;

	/**
	 * @brief Callback type for window resize events.
	 *
	 * This callback is triggered when the window size changes.
	 *
	 * @param width  The new width of the window.
	 * @param height The new height of the window.
	 */
	using WindowResizeCallback = std::function<void(int width, int height)>;

	/**
	 * @brief Callback type for handling key events.
	 *
	 * This callback is triggered when a key is pressed, released, or held.
	 *
	 * @param key    The key code of the pressed/released key.
	 * @param action The action type (press, release, repeat).
	 */
	using WindowKeyCallback = std::function<void(KeyCode key, bool action)>;

	/**
	 * @brief Callback type for mouse button events.
	 *
	 * This callback is triggered when a mouse button is pressed or released.
	 *
	 * @param button The mouse button being pressed/released.
	 * @param action The action type (press or release).
	 */
	using WindowMouseCallback = std::function<void(MouseButton button, bool action)>;

	/**
	 * @brief Callback type for mouse movement events.
	 *
	 * This callback is triggered when the mouse moves within the window.
	 *
	 * @param x The new x-coordinate of the mouse cursor.
	 * @param y The new y-coordinate of the mouse cursor.
	 */
	using WindowMouseMoveCallback = std::function<void(int x, int y)>;

	//Class Declarations

	//Public

	//Window

	struct WindowDescription
	{
		std::string Title;
		int Width;
		int Height;
	};

	struct WindowCallbacks
	{
		//Basic Window Callbacks
		WindowResizeCallback WindowResizeCallback;
		WindowCloseCallback WindowCloseCallback;

		//Input Callbacks
		WindowKeyCallback WindowKeyCallback;
		WindowMouseCallback WindowMouseCallback;
		WindowMouseMoveCallback WindowMouseMoveCallback;
	};

	class Window
	{
	public:
		/**
		 * @brief Default constructor. Should not be used directly.
		 *
		 * Use Window::Create instead to properly initialize a window.
		 */
		Window() = default;

		~Window() = default;

		/**
		 * @brief Creates the main application window.
		 *
		 * This function initializes and creates a new window with the specified settings.
		 *
		 * @param description The settings defining the properties of the window.
		 * @return A shared pointer to the newly created window.
		 */
		static WindowPtr Create(const WindowDescription& description);

		/**
		 * @brief Destroys the window.
		 *
		 * This function releases all resources associated with the window.
		 */
		void Destroy() const;

		/**
		 * @brief Checks if the window is still running.
		 *
		 * @return True if the window is running, false otherwise.
		 */
		bool GetIsRunning() const;

		/**
		 * @brief Sets the running state of the window.
		 *
		 * @param value True to keep the window running, false to stop it.
		 */
		void SetIsRunning(bool value);

		/**
		 * @brief Creates an OpenGL rendering context for the window.
		 *
		 * This function initializes an OpenGL context with the specified version.
		 * It supports both modern and legacy OpenGL versions.
		 *
		 * @param major The major OpenGL version (default is 4).
		 * @param minor The minor OpenGL version (default is 6).
		 * @param legacy Set to true to use legacy OpenGL (1.x or earlier) (default is false).
		 */
		void CreateContext(int major = 4, int minor = 6, bool legacy = false) const;

		/**
		 * @brief Swaps the front and back buffers.
		 *
		 * This function is typically called at the end of a render loop to present the next frame.
		 */
		void SwapBuffers() const;

		/**
		 * @brief Processes window events.
		 *
		 * This function handles user input and window events, such as resizing or closing.
		 * It should be called frequently within the main loop.
		 */
		void PollEvents() const;

		/**
		 * @brief Retrieves the address of an OpenGL function for the current context.
		 *
		 * @return A pointer to the requested function.
		 */
		char* GetProcAddress(const char* name) const;

		/**
		 * @brief Sets the callback function for window resizing.
		 *
		 * This callback is triggered when the window is resized.
		 *
		 * @param callback The function to be called when the window size changes.
		 */
		void SetWindowResizeCallback(WindowResizeCallback callback);

		/**
		* @brief Sets the callback function when the window is closing.
		*
		* This callback is triggered when the window is closing.
		*
		* @param callback The function to close the window.
		*/
		void SetWindowCloseCallback(WindowCloseCallback callback);

		/**
		 * @brief Sets the callback function for keyboard input.
		 *
		 * This callback is triggered when a key is pressed or released.
		 *
		 * @param callback The function to handle key events.
		 */
		void SetWindowKeyCallback(WindowKeyCallback callback);

		/**
		 * @brief Sets the callback function for mouse button events.
		 *
		 * This callback is triggered when a mouse button is pressed or released.
		 *
		 * @param callback The function to handle mouse button events.
		 */
		void SetWindowMouseCallback(WindowMouseCallback callback);

		/**
		 * @brief Sets the callback function for mouse movement.
		 *
		 * This callback is triggered when the mouse moves within the window.
		 *
		 * @param callback The function to handle mouse movement events.
		 */
		void SetWindowMouseMoveCallback(WindowMouseMoveCallback callback);

		/**
		 * @brief Sets the window size.
		 *
		 * This function updates the dimensions of the window.
		 *
		 * @param width The new width of the window.
		 * @param height The new height of the window.
		 */
		void SetWindowSize(int width, int height);

		/**
		 * @brief Retrieves the window description.
		 *
		 * @return The current window description.
		 */
		WindowDescription GetWindowDescription() const { return m_WindowDescription; }

		/**
		 * @brief Retrieves the window callbacks.
		 *
		 * @return The set of callback functions associated with the window.
		 */
		WindowCallbacks GetWindowCallbacks() const { return m_WindowCallbacks; }

	private:
		WindowDescription m_WindowDescription;
		WindowCallbacks m_WindowCallbacks;
		Internal::NativeWindowPtr m_NativeWindow;
		bool m_IsRunning;
	};

	//Input

	//Enum representing key codes
	enum class KeyCode : uint8_t
	{
		Unknown = 0,

		A, B, C, D, E, F, G, H, I, J, K, L, M,
		N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

		Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

		Escape, Enter, Space, Backspace, Tab, Shift, Ctrl, Alt,
		Left, Right, Up, Down,

		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

		Count // Helper to get number of keys
	};

	//Enum representing mouse buttons
	enum class MouseButton
	{
		Unknown = 0,
		LeftMouseButton,
		RightMouseButton,
	};

	//Utility

	struct Colour
	{
		float R, G, B, A;

		Colour(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
			: R(r), G(g), B(b), A(a) {}
	};


	//Private
	namespace Internal
	{
		class Logger
		{
		public:
			static void Log(const std::string& message);
		};

		class Error : public std::runtime_error
		{
		public:
			Error(const std::string& message) : runtime_error(message) {}
		};

		class RenderContext
		{
		public:
			/**
			* @brief Sets the viewport size for rendering.
			*
			* This function updates the OpenGL viewport to match the given width and height.
			* It ensures that all rendering operations are scaled appropriately within the window.
			*
			* @param width The width of the viewport in pixels.
			* @param height The height of the viewport in pixels.
			*/
			static void SetViewportSize(int width, int height);

			/**
			* @brief Clears the current framebuffer.
			*
			* This function clears the color and depth buffers, preparing the screen for a new frame.
			* It should be called at the beginning of each render loop.
			*/
			static void Clear();

			/**
			 * @brief Draws a colored quad at a specified position and scale.
			 *
			 * This function renders a simple quad (square) at the given position.
			 * It can be used for basic UI elements, sprites, or debug rendering.
			 *
			 * @param x The X coordinate of the quad's position.
			 * @param y The Y coordinate of the quad's position.
			 * @param scale The scale factor of the quad (default is 1.0f).
			 * @param colour The color of the quad (default is an empty/neutral color).
			 */
			static void DrawQuad(float x, float y, float scale = 1.0f, Colour colour = {});
		};

		class NativeWindow
		{
		public:
			static NativeWindowPtr Create(const WindowPtr& window);
			virtual void Destroy() {}

			virtual void RefreshScreen() {}
			virtual void PollEvents() {}

			virtual KeyCode ConvertNativeKeyCodes(int key) { return KeyCode::Unknown; }

			virtual void CreateContext(int major = 4, int minor = 6, bool legacy = false) {}
			virtual void* GetExternalAddress(const char* name) { return nullptr; }

			WindowPtr GetWindow() const { return m_Window; }

		protected:
			WindowPtr m_Window;
		};

#ifdef _WIN32

		class Win32NativeWindow : public NativeWindow
		{
		public:
			Win32NativeWindow(const WindowPtr& window);

			~Win32NativeWindow() = default;

			virtual void Destroy() override;

			virtual void PollEvents() override;
			virtual void RefreshScreen() override;

			virtual KeyCode ConvertNativeKeyCodes(int key) override;

			virtual void CreateContext(int major = 4, int minor = 6, bool legacy = false) override;

			virtual void* GetExternalAddress(const char* name) override;



		private:
			static void RegisterWindowClass(HINSTANCE hInstance);
		private:
			HWND m_WindowHandle;
			HDC m_DeviceContext;
			HINSTANCE m_Instance;
			HGLRC m_OpenGLContext;
		};

#endif

	}


	//Class Implementations

	//Public

#pragma region Window

	inline WindowPtr Window::Create(const WindowDescription& description)
	{
		auto window = std::make_shared<Window>();
		window->m_WindowDescription = description;

		window->m_NativeWindow = Internal::NativeWindow::Create(window);

		window->m_IsRunning = true;

		Internal::Logger::Log("Created Window");

		return window;
	}

	inline void Window::Destroy() const
	{
		m_NativeWindow->Destroy();
		Internal::Logger::Log("Destroyed Window");
	}

	inline bool Window::GetIsRunning() const
	{
		return m_IsRunning;
	}

	inline void Window::SetIsRunning(bool value)
	{
		m_IsRunning = value;
	}

	inline void Window::CreateContext(int major, int minor, bool legacy) const
	{
		m_NativeWindow->CreateContext(major, minor, legacy);
	}

	inline void Window::PollEvents() const
	{
		m_NativeWindow->PollEvents();
	}

	inline char* Window::GetProcAddress(const char* name) const
	{
		return static_cast<char*>(m_NativeWindow->GetExternalAddress(name));
	}

	inline void Window::SetWindowResizeCallback(WindowResizeCallback callback)
	{
		m_WindowCallbacks.WindowResizeCallback = std::move(callback);
	}

	inline void Window::SetWindowKeyCallback(WindowKeyCallback callback)
	{
		m_WindowCallbacks.WindowKeyCallback = std::move(callback);
	}

	inline void Window::SetWindowMouseCallback(WindowMouseCallback callback)
	{
		m_WindowCallbacks.WindowMouseCallback = std::move(callback);
	}

	inline void Window::SetWindowMouseMoveCallback(WindowMouseMoveCallback callback)
	{
		m_WindowCallbacks.WindowMouseMoveCallback = std::move(callback);
	}

	inline void Window::SetWindowCloseCallback(WindowCloseCallback callback)
	{
		m_WindowCallbacks.WindowCloseCallback = callback;
	}

	inline void Window::SetWindowSize(int width, int height)
	{
		m_WindowDescription.Width = width;
		m_WindowDescription.Height = height;
	}

	inline void Window::SwapBuffers() const
	{
		m_NativeWindow->RefreshScreen();
	}
#pragma endregion

	//Private
	namespace Internal
	{
		inline void Logger::Log(const std::string& message)
		{
			std::cout << message << "\n";
		}

		inline NativeWindowPtr NativeWindow::Create(const WindowPtr& window)
		{
#ifdef _WIN32
			return std::make_shared<Win32NativeWindow>(window);
#endif
		}

#pragma region Render

		inline void RenderContext::SetViewportSize(int width, int height)
		{
			glViewport(0, 0, width, height);
		}

		inline void RenderContext::Clear()
		{

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.0f, 0.0f, 0.0f, 1);
		}

		inline void RenderContext::DrawQuad(float x, float y, float scale, Colour colour)
		{
			// Enable blending for transparency
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Set up the translation and scaling transformations
			glPushMatrix();  // Save the current transformation matrix

			glTranslatef(x, y, 0.0f);  // Translate the quad to position (x, y)
			glScalef(scale, scale, 1.0f);  // Scale the quad by 'scale'

			// Set the color with an alpha value for transparency
			glColor4f(colour.R, colour.G, colour.B, colour.A);  // Red color with 25% opacity

			// Draw the quad
			glBegin(GL_QUADS);
			glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom left
			glVertex3f(1.0f, -1.0f, 0.0f);  // Bottom right
			glVertex3f(1.0f, 1.0f, 0.0f);   // Top right
			glVertex3f(-1.0f, 1.0f, 0.0f);  // Top left
			glEnd();

			// Restore the previous transformation matrix
			glPopMatrix();

			// Disable blending if it's not needed for other objects
			glDisable(GL_BLEND);
		}


#pragma endregion


#ifdef _WIN32

		//OpenGL

		typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);

		// These values come from the OpenGL and WGL extension specifications

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB   0x2093
#define WGL_CONTEXT_FLAGS_ARB         0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB  0x9126

// Values for WGL_CONTEXT_FLAGS_ARB
#define WGL_CONTEXT_DEBUG_BIT_ARB              0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

// Values for WGL_CONTEXT_PROFILE_MASK_ARB
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB		  0x0001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x0002


		inline Win32NativeWindow::Win32NativeWindow(const WindowPtr& window)
		{
			m_Window = window;

			m_Instance = GetModuleHandle(nullptr);

			RegisterWindowClass(m_Instance);

			//Defines the window size
			RECT rect = { 0, 0, m_Window->GetWindowDescription().Width, m_Window->GetWindowDescription().Height };
			AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

			//Create a temp title to avoid assertion
			const auto title = m_Window->GetWindowDescription().Title;
			const std::wstring wtitle(title.begin(), title.end());


			//Creates the main application window
			m_WindowHandle = CreateWindowEx(
				0, L"SwindowWindowClass",
				wtitle.c_str(),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
				nullptr, nullptr, m_Instance, m_Window.get()
			);

			if (!m_WindowHandle)
			{
				DWORD error = GetLastError();
				wchar_t errorMsg[256];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0, errorMsg, 256, nullptr);
				MessageBox(nullptr, errorMsg, L"CreateWindowEx Failed", MB_ICONERROR | MB_OK);

				return;
			}

			//Pass the window pointer to the WindowProc function
			SetWindowLongPtr(m_WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

			m_DeviceContext = GetDC(m_WindowHandle);

			ShowWindow(m_WindowHandle, SW_SHOW);
			UpdateWindow(m_WindowHandle);
		}


		inline void Win32NativeWindow::Destroy()
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(m_OpenGLContext);

			ReleaseDC(m_WindowHandle, m_DeviceContext);
			DestroyWindow(m_WindowHandle);

			UnregisterClass(L"SwindowWindowClass", GetModuleHandle(nullptr));

		}

		inline void Win32NativeWindow::PollEvents()
		{
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		inline void Win32NativeWindow::RefreshScreen()
		{
			SwapBuffers(m_DeviceContext);
		}

		inline KeyCode Win32NativeWindow::ConvertNativeKeyCodes(int key)
		{
			//Setup input map
			const std::unordered_map<int, KeyCode> keyMap = {
				{ 'A', KeyCode::A }, { 'B', KeyCode::B }, { 'C', KeyCode::C }, { 'D', KeyCode::D },
				{ 'E', KeyCode::E }, { 'F', KeyCode::F }, { 'G', KeyCode::G }, { 'H', KeyCode::H },
				{ 'I', KeyCode::I }, { 'J', KeyCode::J }, { 'K', KeyCode::K }, { 'L', KeyCode::L },
				{ 'M', KeyCode::M }, { 'N', KeyCode::N }, { 'O', KeyCode::O }, { 'P', KeyCode::P },
				{ 'Q', KeyCode::Q }, { 'R', KeyCode::R }, { 'S', KeyCode::S }, { 'T', KeyCode::T },
				{ 'U', KeyCode::U }, { 'V', KeyCode::V }, { 'W', KeyCode::W }, { 'X', KeyCode::X },
				{ 'Y', KeyCode::Y }, { 'Z', KeyCode::Z },

				{ VK_ESCAPE, KeyCode::Escape }, { VK_RETURN, KeyCode::Enter },
				{ VK_SPACE, KeyCode::Space },   { VK_BACK, KeyCode::Backspace },
				{ VK_TAB, KeyCode::Tab },		{ VK_SHIFT, KeyCode::Shift },
				{ VK_CONTROL, KeyCode::Ctrl },	{ VK_MENU, KeyCode::Alt },

				{ VK_LEFT, KeyCode::Left },		{ VK_RIGHT, KeyCode::Right },
				{ VK_UP, KeyCode::Up },			{ VK_DOWN, KeyCode::Down },

				{ VK_F1, KeyCode::F1 },			{ VK_F2, KeyCode::F2 },		{ VK_F3, KeyCode::F3 },
				{ VK_F4, KeyCode::F4 },			{ VK_F5, KeyCode::F5 },		{ VK_F6, KeyCode::F6 },
				{ VK_F7, KeyCode::F7 },			{ VK_F8, KeyCode::F8 },		{ VK_F9, KeyCode::F9 },
				{ VK_F10, KeyCode::F10 },		{ VK_F11, KeyCode::F11 },	{ VK_F12, KeyCode::F12 }
			};

			auto it = keyMap.find(key);
			return (it != keyMap.end()) ? it->second : KeyCode::Unknown;
		}

		inline void Win32NativeWindow::CreateContext(int major, int minor, bool legacy)
		{
			constexpr PIXELFORMATDESCRIPTOR formatDescriptor =
			{
				sizeof(PIXELFORMATDESCRIPTOR),													// Size of the descriptor
				1,																				// Version number
				PFD_DRAW_TO_WINDOW |															// Support window drawing
				PFD_SUPPORT_OPENGL |															// Support OpenGL
				PFD_DOUBLEBUFFER,																// Enable double buffering
				PFD_TYPE_RGBA,																	// RGBA color mode
				32,																				// 32-bit color depth
				0, 0, 0, 0, 0, 0,              // Ignore color bits
				0,																				// No alpha buffer
				0,																				// Ignore shift bit
				0,																				// No accumulation buffer
				0, 0, 0, 0,								// Ignore accumulation bits
				24,																				// 24-bit z-buffer
				8,																				// 8-bit stencil buffer
				0,																				// No auxiliary buffer
				PFD_MAIN_PLANE,																	// Main drawing layer
				0,																				// Reserved
		0, 0, 0															// Layer masks ignored
			};

			const int format = ChoosePixelFormat(m_DeviceContext, &formatDescriptor);
			SetPixelFormat(m_DeviceContext, format, &formatDescriptor);

			//Create a dummy context to load extensions
			HGLRC tempContext = wglCreateContext(m_DeviceContext);
			wglMakeCurrent(m_DeviceContext, tempContext);

			//Load wglCreateContextAttrribsARB
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
				(PFNWGLCREATECONTEXTATTRIBSARBPROC)GetExternalAddress("wglCreateContextAttribsARB");

			if (!wglCreateContextAttribsARB)
			{
				DWORD error = GetLastError();
				wchar_t errorMsg[256];
				int result = swprintf(errorMsg, sizeof(errorMsg), L"Error Code: %lu", error);
				MessageBox(nullptr, errorMsg, L"Context Creation Failed", MB_ICONERROR);
				return;
			}

			HGLRC glContext;

			if (legacy)
			{
				//Creates OpenGL version 1.x or earlier
				glContext = wglCreateContext(m_DeviceContext);
			}
			else
			{
				//Creates Modern OpenGL version

				//Specify the OpenGL specifications
				const int attribs[] =
				{
					WGL_CONTEXT_MAJOR_VERSION_ARB, major,
					WGL_CONTEXT_MINOR_VERSION_ARB, minor,
					WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
					WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
					0,
				};

				//Create the modern OpenGL context
				glContext = wglCreateContextAttribsARB(m_DeviceContext, nullptr, attribs);
			}


			if (!glContext)
			{
				MessageBox(nullptr, L"There was an error creating the OpenGL context", L"Error Creating Context", MB_ICONERROR);
				wglMakeCurrent(nullptr, nullptr);
				wglDeleteContext(tempContext);
				return;
			}

			//Delete the dummy context
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(tempContext);

			wglMakeCurrent(m_DeviceContext, glContext);
			m_OpenGLContext = glContext;
		}

		inline void* Win32NativeWindow::GetExternalAddress(const char* name)
		{
			void* proc = (void*)wglGetProcAddress(name);
			if (!proc)
			{
				//If fails we attempt to load the opengl32.dll
				HMODULE module = LoadLibraryA("opengl32.dll");
				if (module)
				{
					proc = (void*)GetProcAddress(module, name);
				}
			}
			return proc;
		}

		inline LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			Win32NativeWindow* windowPtr = (Win32NativeWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			//Return the Default if the window we get from the constructor is null. 
			if (!windowPtr)
			{
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}

			//Windows message loop
			switch (uMsg)
			{
			case WM_SIZE:
				//Window Resize Event
				if (windowPtr->GetWindow()->GetWindowCallbacks().WindowResizeCallback)
				{
					int width = LOWORD(lParam);
					int height = HIWORD(lParam);

					windowPtr->GetWindow()->SetWindowSize(width, height);

					windowPtr->GetWindow()->GetWindowCallbacks().WindowResizeCallback(width, height);
				}
				break;
			case WM_KEYDOWN:
			case WM_KEYUP:
				//Key Callback even
				if (windowPtr->GetWindow()->GetWindowCallbacks().WindowKeyCallback)
				{
					KeyCode key = windowPtr->ConvertNativeKeyCodes(static_cast<int>(wParam));
					const bool isPressed = (uMsg == WM_KEYDOWN);

					windowPtr->GetWindow()->GetWindowCallbacks().WindowKeyCallback(key, isPressed);
				}
				break;
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
				if (windowPtr->GetWindow()->GetWindowCallbacks().WindowMouseCallback)
				{
					MouseButton button = static_cast<MouseButton>(VK_LBUTTON);
					const bool isPressed = (uMsg == WM_LBUTTONDOWN);
					windowPtr->GetWindow()->GetWindowCallbacks().WindowMouseCallback(button, isPressed);
				}
				break;
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
				if (windowPtr->GetWindow()->GetWindowCallbacks().WindowMouseCallback)
				{
					MouseButton button = static_cast<MouseButton>(VK_RBUTTON);
					const bool isPressed = (uMsg == WM_LBUTTONDOWN);
					windowPtr->GetWindow()->GetWindowCallbacks().WindowMouseCallback(button, isPressed);
				}
				break;
			case WM_MOUSEMOVE:
				if (windowPtr->GetWindow()->GetWindowCallbacks().WindowMouseMoveCallback)
				{
					const int mouseX = LOWORD(lParam);
					const int mouseY = HIWORD(lParam);

					windowPtr->GetWindow()->GetWindowCallbacks().WindowMouseMoveCallback(mouseX, mouseY);
				}
				break;
			case WM_CLOSE:
				//If the user has the WindowCloseCallback: Then the application will check if that is true before returning.
				if (windowPtr->GetWindow()->GetWindowCallbacks().WindowCloseCallback)
				{
					if (windowPtr->GetWindow()->GetWindowCallbacks().WindowCloseCallback())
					{
						windowPtr->GetWindow()->SetIsRunning(false);
					}
					//Continue running the loop if returns false
				}
				else
				{
					windowPtr->GetWindow()->SetIsRunning(false);
				}
				return 0;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;

			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}

			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		inline void Win32NativeWindow::RegisterWindowClass(HINSTANCE hInstance)
		{
			static bool registered = false;
			if (registered) return;

			WNDCLASSEX wc = {};
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WindowProc;
			wc.hInstance = hInstance;
			wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wc.lpszClassName = L"SwindowWindowClass";
			wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

			if (!RegisterClassEx(&wc))
			{
				MessageBox(nullptr, L"Failed to register window class", L"Error", MB_ICONERROR | MB_OK);
			}
			registered = true;
		}

#endif


	}

}//Namespace Swindow

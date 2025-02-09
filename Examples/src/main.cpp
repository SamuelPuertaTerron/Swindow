/*
 * Example usage of Swindow for creating a window and handling input.
 * This example allows users to spawn red quads at the mouse position.
 */

#include "../../Swindow.h"

namespace SwindowExample
{
	using namespace Swindow;

	// Stores the mouse position in normalized world coordinates (-1 to 1)
	struct MousePosition
	{
		float x;
		float y;
	};

	// Represents a spawned object in world space
	struct Object
	{
		float x, y;

		Object(float x, float y)
			: x(x), y(y) {}
	};

	static WindowPtr g_Window;
	static MousePosition g_MousePosition;
	static std::vector<Object> g_Objects;

	static bool g_ShouldClose;

	// Adds an object at the specified world coordinates
	static void Spawn(float x, float y);

	// Adjusts the viewport size when the window is resized
	static void WindowResizeCallback(int width, int height)
	{
		Render::SetViewportSize(width, height);
	}

	// Handles key input (e.g., pressing Escape to exit)
	static void KeyCallback(KeyCode key, bool isPressed)
	{
		if (isPressed && key == KeyCode::Escape)
		{
			g_Window->SetIsRunning(false);
		}
	}

	// Handles mouse clicks; left-click spawns an object
	static void MouseCallback(MouseButton button, bool isPressed)
	{
		if (isPressed && button == MouseButton::LeftMouseButton)
		{
			Spawn(g_MousePosition.x, g_MousePosition.y);
		}
	}

	// Converts mouse screen coordinates to normalized world coordinates (-1 to 1)
	static void MousePositionCallback(int mouseX, int mouseY)
	{
		float screenWidth = static_cast<float>(g_Window->GetWindowDescription().Width);
		float screenHeight = static_cast<float>(g_Window->GetWindowDescription().Height);

		// Convert pixel position to OpenGL world coordinates (-1 to 1)
		g_MousePosition.x = (static_cast<float>(mouseX) / screenWidth) * 2.0f - 1.0f;
		g_MousePosition.y = 1.0f - (static_cast<float>(mouseY) / screenHeight) * 2.0f; // Flip Y-axis
	}

	// Initializes and runs the application
	static void Run()
	{
		// Configure window properties
		WindowDescription desc;
		desc.Title = "Hello from Swindow";
		desc.Width = 720;
		desc.Height = 500;

		// Create and initialize the window
		g_Window = Window::Create(desc);

		// Set event callbacks
		g_Window->SetWindowResizeCallback(WindowResizeCallback);

		g_Window->SetWindowKeyCallback(KeyCallback);
		g_Window->SetWindowMouseCallback(MouseCallback);
		g_Window->SetWindowMouseMoveCallback(MousePositionCallback);

		// OpenGL setup (legacy mode)
		g_Window->CreateContext(1, 1, true);

		// OpenGL setup (Modern Mode)
		//g_Window->CreateContext(4, 6);

		// Set viewport dimensions
		Render::SetViewportSize(desc.Width, desc.Height);

		// Main application loop
		while (g_Window->GetIsRunning())
		{
			g_Window->PollEvents(); // Process user input events
			Render::Clear();        // Clear the screen

			// Draw background quad at the center
			Render::DrawQuad(0, 0, 1.0f, { 0.35f, 0.55f, 0.75f, 1.0f });

			// Draw all spawned objects (red quads)
			for (const auto& obj : g_Objects)
			{
				Render::DrawQuad(obj.x, obj.y, 0.05f, { 1.0f, 0.0f, 0.0f, 1.0f });
			}

			g_Window->SwapBuffers(); // Display rendered frame
		}

		// Cleanup and close the window
		g_Window->Destroy();
	}

	// Spawns a new object at the given world coordinates
	void Spawn(float x, float y)
	{
		g_Objects.emplace_back(x, y);
	}
}//Namespace SwindowExample

// Application entry point
int main()
{
	SwindowExample::Run();
}

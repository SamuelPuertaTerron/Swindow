# Swindow

**Swindow** (Simple Window) is a lightweight, Windows library designed for managing windows and handling user input. It is written as a single header file in C++14, making it easy to integrate into your project with minimal setup.

## Features

- **Simple and Lightweight:** A single header file for easy integration.
- **User Input Handling:** Supports keyboard, mouse, and window events.
- **No Dependencies:** Completely self-contained, with no external libraries required.

## Getting Started

To get started, simply download the header file `Swindow.h` and include it in your C++ project.

## Simple Example

```cpp
#include "Swindow.h"

static Swindow::WindowPtr g_Window;

// Handles key input (e.g., pressing Escape to exit)
static void KeyCallback(Swindow::KeyCode key, bool isPressed)
{
	if (isPressed)
	{
		if (key == Swindow::KeyCode::Escape)
		{
			g_Window->SetIsRunning(false);
		}
	}
}

int main()
{
	using namespace Swindow;

	// Configure window properties
	WindowDescription desc;
	desc.Title = "Hello from Swindow";
	desc.Width = 720;
	desc.Height = 500;

	// Create and initialize the window
	g_Window = Window::Create(desc);

	// Set window callbacks
	g_Window->SetWindowKeyCallback(KeyCallback);

	// Main application loop
	while (g_Window->GetIsRunning())
	{
		// Process user input events
		g_Window->PollEvents();

		// Display rendered frame
		g_Window->SwapBuffers();
	}

	// Cleanup and close the window
	g_Window->Destroy();
}
```

## Building the source

Premake 5 is used to build the examples into a vs2022 solution. Run the `build.bat` file inside the `Build Scripts` directory.
Premake 5 is already built-in, but if for any reason it is not present, download it from here https://premake.github.io/ 

## Futre Features Planned

- [ ] Add Linux Support
- [ ] Add ImGui Support
- [ ] Add more window related callbacks, such as Input. 




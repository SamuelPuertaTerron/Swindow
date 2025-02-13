#include "ImGuiExample.h"

#define SW_IMGUI_IMPLEMENTATION
#include "../../Swindow.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"

namespace ImGuiExample
{
	static Swindow::WindowPtr g_Window;

	// Handles key input (e.g., pressing Escape to exit)
	static void KeyCallback(Swindow::KeyCode key, bool isPressed)
	{
		if (isPressed && key == Swindow::KeyCode::Escape)
		{
			g_Window->SetIsRunning(false);
		}
	}

	void Run()
	{
		using namespace Swindow;

		// Configure window properties
		WindowDescription desc;
		desc.Title = "ImGui Example with Swindow";
		desc.Width = 720;
		desc.Height = 500;

		// Create and initialize the window
		g_Window = Window::Create(desc);

		// Set event callbacks
		g_Window->SetWindowKeyCallback(KeyCallback);

		// OpenGL setup, leave blank for the modern OpenGL 4.6
		g_Window->CreateContext();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::StyleColorsDark();

		SwindowImGui::ImGui_ImplSwindow_InitForOpenGL(g_Window, true);
		ImGui_ImplOpenGL3_Init("#version 460");

		//Store the text data for the InputText field here
		std::string data;

		// Main application loop
		while (g_Window->GetIsRunning())
		{
			g_Window->PollEvents(); // Process user input events

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ImGui_ImplOpenGL3_NewFrame();
			SwindowImGui::ImGui_ImplSwindow_NewFrame();
			ImGui::NewFrame();

			if (ImGui::Begin("Hello World!"))
			{
				ImGui::Text("This is useful text");

				if (ImGui::Button("Click Me"))
				{
					std::cout << "Pressed the button \n";
				}

				// Make sure buffer size is large enough
				char buffer[256];
				strncpy_s(buffer, data.c_str(), sizeof(buffer));
				buffer[sizeof(buffer) - 1] = '\0'; // Ensure null termination

				if (ImGui::InputText("Text", buffer, sizeof(buffer)))
				{
					data = std::string(buffer); // Update the std::string
				}

				ImGui::End();
			}

			ImGui::Render();
			ImGui::EndFrame();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			g_Window->SwapBuffers(); // Display rendered frame
		}

		ImGui_ImplOpenGL3_Shutdown();
		SwindowImGui::ImGui_ImplSwindow_Shutdown();

		// Cleanup and close the window
		g_Window->Destroy();
	}

}//Namespace

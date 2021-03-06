#pragma once

#include "Window.h"
#include "Apex/Core/Layers/LayerStack.h"
#include "Apex/Core/Events/Event.h"
#include "Apex/Core/Events/ApplicationEvent.h"
#include "Apex/Core/Timer.h"

#include "Apex/ImGui/ImGuiLayer.h"

namespace Apex {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void Close();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		inline ImGuiLayer& GetImGuiLayer() { return *m_ImGuiLayer; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		float m_LastFrameTime;

		static Application* s_Instance;
	};


	//To be defined in client
	Application* CreateApplication();

}

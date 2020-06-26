#include "apex_pch.h"
#include "Application.h"

#include "Apex/Log/Log.h"
#include "Apex/Renderer/Renderer.h"
#include "Apex/Input/Input.h"

#include "Apex/Utils/Profiler.h"

#include <GLFW/glfw3.h>

namespace Apex {

#define BIND_CALLBACK_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		APEX_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		m_Window = Apex::Scope<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_CALLBACK_FN(OnEvent));
		m_Window->SetVSync(true);

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

	}


	Application::~Application()
	{
	}

	void Application::Run()
	{
		Instrumentor::Get().BeginSession(__FUNCTION__);

		while (m_Running) {

			Timer::UpdateTime();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			//Render ImGui
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}

		Instrumentor::Get().EndSession();
	}

	// Event Handlers

	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_CALLBACK_FN(OnWindowClose));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			if (e.IsHandled())
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer * layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer * overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent & e)
	{
		m_Running = false;
		return false;
	}

}
#include <Apex.h>
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Apex/Material/Material.h"

class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquareColor(0.78f, 0.23f, 0.47f, 1.0f)
	{
		/// Square Data ///
		m_SquareVA = Apex::VertexArray::Create();

		Apex::Ref<Apex::VertexBuffer> squareVB;
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		squareVB = Apex::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ Apex::ShaderDataType::Float3, "a_Position" },
			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		Apex::Ref<Apex::IndexBuffer> squareIB;
		uint32_t squareIndices[] = {
			0, 1, 2,
			0, 2, 3
		};
		squareIB = Apex::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_SquareVA->AddIndexBuffer(squareIB);

		m_SquareVA->Unbind();
		/// End Square Data ///

		/// Texture Shader ///
		
		auto textureShader = Apex::AssetManager::GetShaderLibrary().Load("assets/shaders/Texture.glsl");
		m_PusheenTexture = Apex::Texture2D::Create("assets/textures/pusheen-thug-life.png");
		m_CheckerTexture = Apex::Texture2D::Create("assets/textures/Checkerboard.png");
		textureShader->Bind();
		textureShader->SetUniInt("u_Texture", 0);

		/* ------------------TEST CODE------------------------- */
		//Apex::Material material;
		//material.SetShader(textureShader);
		//material.AddTexture("Texture", m_CheckerTexture);
		//material.Bind();
		/* ---------------------------------------------------- */

		/// Flat Shader ///
		std::string flatVertexSrc = R"(
			#version 450

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Model;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
			}
		)";

		std::string flatFragmentSrc = R"(
			#version 450

			layout(location = 0) out vec4 o_Color;
			uniform vec4 u_Color;
			
			in vec3 v_Position;

			void main()
			{
				o_Color = u_Color;
			}
		)";
		m_FlatShader = Apex::Shader::Create("FlatColorShader", flatVertexSrc, flatFragmentSrc);

		/// Frame Buffer ///
		m_ScreenVA = Apex::VertexArray::Create();
		m_FrameBuffer = Apex::FrameBuffer::Create();
		m_ScreenColorTexture = Apex::Texture2D::Create();
		m_FrameBuffer->AttachTexture(m_ScreenColorTexture);

		Apex::Ref<Apex::VertexBuffer> screenVB;
		float screenVertices[] = {
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
		
			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		screenVB = Apex::VertexBuffer::Create(screenVertices, sizeof(screenVertices));
		screenVB->SetLayout({
			{ Apex::ShaderDataType::Float2, "a_Position" },
			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_ScreenVA->AddVertexBuffer(screenVB);

		std::string screenVertexSrc = R"(
			#version 450

			layout(location = 0) in vec2 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = vec4(a_Position, 0.0, 1.0);
			}
		)";

		std::string screenFragmentSrc = R"(
			#version 450

			layout(location = 0) out vec4 o_Color;
			
			in vec2 v_TexCoord;

			uniform sampler2D u_ScreenTexture;

			void main()
			{
				o_Color = texture(u_ScreenTexture, v_TexCoord);
			}
		)";
		m_ScreenShader = Apex::Shader::Create("assets/shaders/GaussianBlur.glsl");
		m_ScreenShader->Bind();
		m_ScreenShader->SetUniInt("u_ScreenTexture", 0);
	}

	// Inherited via Layer
	void OnAttach() override {}

	void OnDetach() override {}

	void OnUpdate() override
	{
		auto[mouseX, mouseY] = Apex::Input::GetMousePos();
		if (!ImGui::IsAnyItemActive() && !ImGui::IsAnyWindowFocused()) {
			std::pair<float, float> mouseDiff = {
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseX - m_MousePos.first) : 0.0f,
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseY - m_MousePos.second) : 0.0f };
			m_CameraRotation += Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_RIGHT) ? ((mouseY - m_MousePos.second) * m_CameraRotateSpeed * Apex::Timer::GetSeconds()) : 0.0f;
			m_CameraPosition.x -= mouseDiff.first * m_CameraMoveSpeed;
			m_CameraPosition.y += mouseDiff.second * m_CameraMoveSpeed;
		}
		
		m_MousePos = { mouseX, mouseY };

		m_MsgList.clear();
		std::stringstream ss;
		ss << "mousePos : " << mouseX << " , " << mouseY;
		m_MsgList.emplace_back(ss.str());
		ss.str(std::string());
		ss << "Camera Position : " << m_CameraPosition.x << " , " << m_CameraPosition.y << " , " << m_CameraPosition.z;
		m_MsgList.emplace_back(ss.str());

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);


		//// First Pass : Render to custom framebuffer ////
		m_FrameBuffer->Bind();
		Apex::RenderCommands::SetDepthTest(true);
		Apex::RenderCommands::SetClearColor({ 0.12f, 0.1185f, 0.12f, 1.0f });
		Apex::RenderCommands::Clear();
		Apex::Renderer::BeginScene(m_Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.075f, 0.075f, 0.075f));

		m_FlatShader->Bind();
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
				m_FlatShader->SetUniFloat4("u_Color", m_SquareColor);
				Apex::Renderer::Submit(m_FlatShader, m_SquareVA,
					glm::translate(glm::mat4(1.0f), glm::vec3(i*0.085f - 9.5f * 0.085f, j*0.085f - 9.5f * 0.085f, -2.3f)) * scale);
			}
		}

		auto textureShader = Apex::AssetManager::GetShaderLibrary().GetShader("Texture");

		m_CheckerTexture->Bind();
		Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.7f)));
		m_PusheenTexture->Bind();
		Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

		Apex::Renderer::EndScene();

		//// Second Pass : Render to default framebuffer ////
		m_FrameBuffer->Unbind();
		Apex::RenderCommands::SetDepthTest(false);
		Apex::RenderCommands::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		Apex::RenderCommands::Clear();
		m_ScreenColorTexture->Bind();
		Apex::Renderer::SubmitPostProcess(m_ScreenShader, m_ScreenVA);

	}

	void OnImGuiRender() override
	{
		ImGui::ShowMetricsWindow();
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
		for (auto& msg : m_MsgList)
			ImGui::TextUnformatted(msg.c_str());
		ImGui::End();
	}

	void OnEvent(Apex::Event& event) override
	{
		Apex::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Apex::MouseScrolledEvent>(APEX_BIND_EVENT_FN(SandboxLayer::OnMouseScrolled));
	}

	bool OnMouseScrolled(Apex::MouseScrolledEvent& event)
	{
		m_CameraPosition.z -= event.GetOffsetY();
		
		return false;
	}

private:
	/// Main scene objects ///
	Apex::Ref<Apex::Shader> m_FlatShader;
	//Apex::Ref<Apex::Shader> m_TextureShader;
	Apex::Ref<Apex::VertexArray> m_SquareVA;
	Apex::Ref<Apex::Texture2D> m_PusheenTexture;
	Apex::Ref<Apex::Texture2D> m_CheckerTexture;

	/// Custom framebuffer objects ///
	Apex::Ref<Apex::Texture2D> m_ScreenColorTexture;
	Apex::Ref<Apex::VertexArray> m_ScreenVA;
	Apex::Ref<Apex::Shader> m_ScreenShader;
	Apex::Ref<Apex::FrameBuffer> m_FrameBuffer;
	
	/// Camera ///
	Apex::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed = 0.0025f;
	float m_CameraRotateSpeed = 30.0f;

	std::pair<float, float> m_MousePos = { 0.0f, 0.0f };
	
	glm::vec4 m_SquareColor;

	std::list<std::string> m_MsgList;
};

class Sandbox : public Apex::Application
{
public:
	Sandbox()
	{
		PushLayer(new SandboxLayer());
	}

	~Sandbox()
	{

	}

};

Apex::Application* Apex::CreateApplication()
{
	return new Sandbox();
}
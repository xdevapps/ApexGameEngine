#pragma once

class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquareColor(5.78f, 3.23f, 3.47f, 1.0f), m_BlurEffect()
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

		/// Instances Data ///
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.075f, 0.075f, 0.075f));

		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
					m_InstancedModelMatrices.push_back(translate(glm::mat4(1.0f), glm::vec3(i*0.125f - 9.5f * 0.125f, j*0.125f - 9.5f * 0.125f, -2.3f)) * scale);
			}
		}

		/*for (glm::mat4 mat : m_InstancedModelMatrices)
		{
			APEX_TRACE("{0}", Apex::MathParser::ParseMatrix(mat));
		}*/

		/// Texture Shader ///
		
		auto textureShader = Apex::AssetManager::GetShaderLibrary().Load("assets/shaders/Texture.glsl");
		m_PusheenTexture = Apex::Texture2D::Create("assets/textures/pusheen-thug-life.png");
		m_CheckerTexture = Apex::Texture2D::Create("assets/textures/Checkerboard.png");
		textureShader->Bind();
		textureShader->SetUniInt1("u_Texture", 0);

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
			uniform mat4 u_Model[100];

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Model[gl_InstanceID] * vec4(a_Position, 1.0);
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
		m_FlatShader->Bind();
		m_FlatShader->SetUniFloat4("u_Color", m_SquareColor);
		//m_FlatShader->SetUniMat4("u_ModelTemp", glm::translate(glm::mat4(1.0f), glm::vec3(-9.5f * 0.085f, -9.5f * 0.085f, -2.3f)) * scale);

		/// Frame Buffer ///
		m_FrameBuffer = Apex::FrameBuffer::Create(true);
		m_ScreenColorTexture = Apex::Texture2D_HDR::Create();
		m_FrameBuffer->AttachTexture(m_ScreenColorTexture);

		m_PostProcessFB = Apex::FrameBuffer::Create(false);
		m_BrightColorTexture = Apex::Texture2D_HDR::Create();
		m_PostProcessFB->AttachTexture(m_BrightColorTexture);

// 		m_ScreenVA = Apex::VertexArray::Create();
// 		Apex::Ref<Apex::VertexBuffer> screenVB;
// 		float screenVertices[] = {
// 			-1.0f,  1.0f,  0.0f, 1.0f,
// 			-1.0f, -1.0f,  0.0f, 0.0f,
// 			 1.0f, -1.0f,  1.0f, 0.0f,
// 		
// 			-1.0f,  1.0f,  0.0f, 1.0f,
// 			 1.0f, -1.0f,  1.0f, 0.0f,
// 			 1.0f,  1.0f,  1.0f, 1.0f
// 		};
// 		screenVB = Apex::VertexBuffer::Create(screenVertices, sizeof(screenVertices));
// 		screenVB->SetLayout({
// 			{ Apex::ShaderDataType::Float2, "a_Position" },
// 			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
// 			});
// 		m_ScreenVA->AddVertexBuffer(screenVB);

		m_BrightShader = Apex::Shader::Create("assets/shaders/BrightnessSplit.glsl");
		m_BrightShader->Bind();
		m_BrightShader->SetUniInt1("u_ScreenTexture", 0);
		
		m_BlendShader = Apex::Shader::Create("assets/shaders/Blend.glsl");
		m_BlendShader->Bind();
		m_BlendShader->SetUniInt1("u_SceneTexture", 0);
		m_BlendShader->SetUniInt1("u_BlurTexture", 1);

		auto screenHDRShader = Apex::AssetManager::GetShaderLibrary().Load("assets/shaders/ScreenHDR.glsl");
		screenHDRShader->Bind();
		screenHDRShader->SetUniInt1("u_ScreenTexture", 0);
		auto screenShader = Apex::AssetManager::GetShaderLibrary().Load("assets/shaders/Screen.glsl");
		screenShader->Bind();
		screenShader->SetUniInt1("u_ScreenTexture", 0);

		m_NoiseTexture = Apex::Texture2D_HDR::Create(256U, 256U, "noise");
		m_ComputeShader = Apex::ComputeShader::Create("assets/compute/noise.compute");
	}

	// Inherited via Layer
	void OnAttach() override
	{
		m_NoiseTexture->BindImage(0U, false, true);
		m_ComputeShader->Bind();
		m_ComputeShader->Dispatch(m_NoiseTexture->GetWidth(), m_NoiseTexture->GetHeight(), 1U);
	}

	void OnDetach() override {}

	void OnUpdate() override
	{
		auto[mouseX, mouseY] = Apex::Input::GetMousePos();
		if (!ImGui::IsAnyItemActive() && !ImGui::IsAnyWindowFocused()) {
			std::pair<float, float> mouseDiff = {
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseX - m_MousePos.first) : 0.0f,
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseY - m_MousePos.second) : 0.0f
			};
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

		/*static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.075f, 0.075f, 0.075f));
		m_FlatShader->Bind();
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
				m_FlatShader->SetUniFloat4("u_Color", glm::vec4{ i*0.085f - 9.5f * 0.085f, j*0.085f - 9.5f * 0.085f, -2.3f, 1.f});
				Apex::Renderer::Submit(m_FlatShader, m_SquareVA,
					glm::translate(glm::mat4(1.0f), glm::vec3(i*0.085f - 9.5f * 0.085f, j*0.085f - 9.5f * 0.085f, -2.3f))); //* scale);
			}
		}*/
		
		/*glm::mat4 matArray[4] = {
			glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, -2.3f)),
			glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -2.3f)),
			glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, -2.3f)),
			glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, -2.3f))
		};*/

		m_FlatShader->SetUniFloat4("u_Color", m_SquareColor);
		Apex::Renderer::SubmitArray(m_FlatShader, m_SquareVA, m_InstancedModelMatrices.data(), 100 /*m_InstancedModelMatrices.size()*/);
		

		auto textureShader = Apex::AssetManager::GetShaderLibrary().GetShader("Texture");

		//m_CheckerTexture->Bind();
		m_PusheenTexture->Bind();
		Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.1f, -1.7f)));
		m_NoiseTexture->Bind();
		Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

		Apex::Renderer::EndScene();

		//// Split Pass : Render to Bright Texture ////
		m_PostProcessFB->Bind();
		Apex::RenderCommands::SetDepthTest(false);
		Apex::RenderCommands::SetClearColor({ 0.12f, 0.1185f, 0.12f, 1.0f });
		Apex::RenderCommands::Clear();
		m_ScreenColorTexture->Bind(0);
		Apex::PostProcess::Submit(m_BrightShader);

		//// Blur Pass : Render to Blur framebuffer ////
		
		auto blurTex = m_BlurEffect.GaussianBlur(5, m_BrightColorTexture);

		//// Final Pass : Render to default framebuffer ////
		m_FrameBuffer->Unbind();
		Apex::RenderCommands::SetDepthTest(false);
		Apex::RenderCommands::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		Apex::RenderCommands::Clear();
		m_ScreenColorTexture->Bind(0);
		//m_BrightColorTexture->Bind(0);
		blurTex->Bind(1);
		//Apex::AssetManager::GetShaderLibrary().GetShader("Screen");
		//auto screenShader = Apex::AssetManager::GetShaderLibrary().GetShader("ScreenHDR");
		//screenShader->Bind();
		//screenShader->SetUniFloat1("exposure", m_Exposure);
		//screenShader->SetUniFloat1("contrast", m_Contrast);
		m_BlendShader->Bind();
		m_BlendShader->SetUniFloat1("exposure", m_Exposure);
		Apex::PostProcess::Submit(m_BlendShader);

	}

	void OnImGuiRender() override
	{
		ImGui::ShowMetricsWindow();
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::DragFloat("Exposure", &m_Exposure, 0.005f, 0.0f, 10.0f);
		ImGui::DragFloat("Contrast", &m_Contrast, 0.005f, 0.0f, 3.0f);
		for (auto& msg : m_MsgList)
			ImGui::TextUnformatted(msg.c_str());
		ImGui::End();

		ImGui::Begin("Textures");
		ImGui::Image((void*)(intptr_t)m_NoiseTexture->GetID(), { 256.f, 256.f });
		ImGui::End();
	}

	void OnEvent(Apex::Event& event) override
	{
		Apex::EventDispatcher dispatcher(event);
		//dispatcher.Dispatch<Apex::MouseScrolledEvent>(APEX_BIND_EVENT_FN(SandboxLayer::OnMouseScrolled));
		dispatcher.Dispatch<Apex::MouseScrolledEvent>([this] (auto& event) -> bool { this->m_CameraPosition.z -= event.GetOffsetY(); return false; });
	}

private:
	/// Main scene objects ///
	Apex::Ref<Apex::Shader> m_FlatShader;
	//Apex::Ref<Apex::Shader> m_TextureShader;
	Apex::Ref<Apex::VertexArray> m_SquareVA;
	Apex::Ref<Apex::Texture2D> m_PusheenTexture;
	Apex::Ref<Apex::Texture2D> m_CheckerTexture;
	Apex::Ref<Apex::Texture2D_HDR> m_NoiseTexture;
	Apex::Ref<Apex::ComputeShader> m_ComputeShader;

	/// Custom framebuffer objects ///
	//Apex::Ref<Apex::VertexArray> m_ScreenVA;
	Apex::Ref<Apex::Shader> m_BrightShader;
	Apex::Ref<Apex::Shader> m_BlendShader;
	Apex::Ref<Apex::Texture2D_HDR> m_ScreenColorTexture;
	Apex::Ref<Apex::Texture2D_HDR> m_BrightColorTexture;
	Apex::Ref<Apex::FrameBuffer> m_FrameBuffer;
	Apex::Ref<Apex::FrameBuffer> m_PostProcessFB;

	Apex::Blur m_BlurEffect;

	/// Camera ///
	Apex::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed = 0.0025f;
	float m_CameraRotateSpeed = 30.0f;

	std::pair<float, float> m_MousePos = { 0.0f, 0.0f };
	
	glm::vec4 m_SquareColor;

	std::list<std::string> m_MsgList;

	std::vector<glm::mat4> m_InstancedModelMatrices;

	float m_Exposure = 1.0f;
	float m_Contrast = 0.0f;
};


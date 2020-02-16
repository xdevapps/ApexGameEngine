#include "apex_pch.h"
#include "Blur.h"

#include "Apex/Renderer/Renderer.h"
#include "Apex/Renderer/RenderCommands.h"

namespace Apex {

	Blur::Blur()
	{
		m_BlurShader = Shader::Create("assets/shaders/GaussianBlur.glsl");
		m_VerBlurTex = Texture2D_HDR::Create();
		m_HorBlurTex = Texture2D_HDR::Create();
		m_VerBlurFBO = FrameBuffer::Create(false);
		m_HorBlurFBO = FrameBuffer::Create(false);
	}

	Ref<Texture> Blur::GaussianBlur(int amount, const Ref<Texture>& inputTexture, const Ref<VertexArray>& vao)
	{

		m_BlurShader->Bind();
		m_BlurShader->SetUniInt("u_ScreenTexture", 0);
		m_BlurShader->SetUniFloat2("u_Resolution", glm::vec2(1280.f, 720.f));

		//Ref<Shader> m_BlurShader = Shader::Create("assets/shaders/GaussianBlur.glsl");
		//Ref<Texture2D_HDR> m_VerBlurTex = Texture2D_HDR::Create();
		//Ref<Texture2D_HDR> m_HorBlurTex = Texture2D_HDR::Create();
		//Ref<FrameBuffer> m_VerBlurFBO = FrameBuffer::Create(false);
		//Ref<FrameBuffer> m_HorBlurFBO = FrameBuffer::Create(false);

		m_VerBlurFBO->AttachTexture(m_VerBlurTex);
		m_HorBlurFBO->AttachTexture(m_HorBlurTex);

		m_VerBlurFBO->Bind();
		Apex::RenderCommands::SetDepthTest(false);
		//Apex::RenderCommands::SetClearColor({ 0.0f, 0.0f, 1.0f, 1.0f });
		Apex::RenderCommands::Clear();
		inputTexture->Bind();
		m_BlurShader->Bind();
		m_BlurShader->SetUniInt("u_Horizontal", 0);
		Apex::Renderer::SubmitPostProcess(m_BlurShader, vao);

		for (int i = 0; i < amount; i++) {
			m_HorBlurFBO->Bind();
			Apex::RenderCommands::SetDepthTest(false);
			//Apex::RenderCommands::SetClearColor({ 0.0f, 1.0f, 0.0f, 1.0f });
			Apex::RenderCommands::Clear();
			m_VerBlurTex->Bind();
			m_BlurShader->Bind();
			m_BlurShader->SetUniInt("u_Horizontal", 1);
			Apex::Renderer::SubmitPostProcess(m_BlurShader, vao);

			m_VerBlurFBO->Bind();
			Apex::RenderCommands::SetDepthTest(false);
			Apex::RenderCommands::SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });
			Apex::RenderCommands::Clear();
			m_HorBlurTex->Bind();
			m_BlurShader->Bind();
			m_BlurShader->SetUniInt("u_Horizontal", 0);
			Apex::Renderer::SubmitPostProcess(m_BlurShader, vao);
		}

		m_HorBlurFBO->Bind();
		Apex::RenderCommands::SetDepthTest(false);
		//Apex::RenderCommands::SetClearColor({ 0.0f, 1.0f, 0.0f, 1.0f });
		Apex::RenderCommands::Clear();
		m_VerBlurTex->Bind();
		m_BlurShader->Bind();
		m_BlurShader->SetUniInt("u_Horizontal", 1);
		Apex::Renderer::SubmitPostProcess(m_BlurShader, vao);

		return m_HorBlurTex;
	}


}
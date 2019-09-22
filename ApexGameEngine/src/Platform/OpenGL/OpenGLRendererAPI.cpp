#include "apex_pch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Apex {
	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		APEX_CORE_TRACE("RendererAPI (OpenGL) initialized successfully!");
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4 & color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::Draw(const Ref<VertexArray>& vertexArray)
	{
		glDrawArrays(GL_TRIANGLES, 0, vertexArray->GetVertexBuffers().at(0)->GetCount());
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffers().at(0)->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::SetDepthTest(bool value)
	{
		if (value)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

}
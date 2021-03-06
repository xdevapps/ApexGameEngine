#include "apex_pch.h"
#include "VertexArray.h"

#include "Apex/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Apex {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexArray>();

			default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}

}

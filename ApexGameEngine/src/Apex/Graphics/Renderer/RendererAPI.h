#pragma once

#include <glm/glm.hpp>

namespace Apex {

	class VertexArray; // Forward Declaration
	
	/* OpenGL specific -> change later */
	enum class BlendingMode
	{
		ZERO = 0, ONE = 1,

		SRC_COLOR = 0x0300,
		ONE_MINUS_SRC_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA_SATURATE,

		CONSTANT_COLOR = 0x8001,
		ONE_MINUS_CONSTANT_COLOR,
		CONSTANT_ALPHA,
		ONE_MINUS_CONSTANT_ALPHA
	};
	
	enum class DepthStencilMode
	{
		NEVER = 0x0200, LESS, EQUAL, LEQUAL,
		GREATER, NOTEQUAL, GEQUAL, ALWAYS
	};
	
	enum class DepthStencilAction
	{
		ZERO = 0, ONE = 1,
		
		KEEP = 0x1E00,
		REPLACE,
		INCREMENT,
		DECREMENT,
		
		INCR_WRAP = 0x8507,
		DECR_WRAP,
		
		INVERT = 0x150A
	};
	
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};

	public:
		virtual void Init() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void Draw(const Ref<VertexArray>& vertexArray) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawInstanced(const Ref<VertexArray>& vertexArray, size_t count) = 0;

		virtual void SetDepthTest(bool value) = 0;
		virtual void SetDepthWriteMode(bool value) = 0;
		virtual void SetDepthTestFunction(DepthStencilMode criterion) = 0;
		
		virtual void SetStencilTest(bool value) = 0;
		virtual void SetStencilTestFunction(DepthStencilMode criterion, int8_t refValue, uint8_t mask = 0xff) = 0;
		virtual void SetStencilWriteMask(uint32_t mask = 0xff) = 0;
		virtual void SetStencilAction(DepthStencilAction onStencilFail = DepthStencilAction::KEEP, DepthStencilAction onDepthFail = DepthStencilAction::KEEP, DepthStencilAction onPass = DepthStencilAction::KEEP) = 0;
		
		virtual void SetCulling(bool value) = 0;
		virtual void SetBlendMode(BlendingMode sourceMode, BlendingMode destMode) = 0;
		
		virtual void SetViewport(uint32_t x, uint32_t y, size_t width, size_t height) = 0;
		virtual void ImageAccessBarrier() = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}

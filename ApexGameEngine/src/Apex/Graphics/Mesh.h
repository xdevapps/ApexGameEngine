#pragma once

#include "glm/glm.hpp"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

namespace Apex {

	class Mesh
	{
	public:
		Mesh(float* vertices, size_t vCount, uint32_t* indices, size_t iCount, const BufferLayout& layout);
		virtual ~Mesh() = default;

		virtual inline void Bind() const { m_VAO->Bind(); }
		virtual inline const Ref<VertexArray>& GetVAO() const { return m_VAO; }

		virtual inline bool& Show() { return m_Show; }
		virtual inline const bool& Show() const { return m_Show; }

		virtual inline void SetTextures(std::unordered_map<std::string, Ref<Texture>> textures) { m_Textures = textures; }
		virtual inline const std::unordered_map<std::string, Ref<Texture>>& GetTextures() const { return m_Textures; }
		
		virtual void AddTexture(std::pair<std::string, Ref<Texture>>& nameTexturePair);
		virtual void AddTexture(std::string name, Ref<Texture> texture);

		virtual inline bool HasNormals() const { return m_HasNormals; }
		virtual inline bool HasTangents() const { return m_HasTangents; }
		
	private:
		Ref<VertexArray> m_VAO;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;
		bool m_Show = true;
		
		bool m_HasNormals = true;
		bool m_HasTangents = false;
		
		friend class Model;
	};

}
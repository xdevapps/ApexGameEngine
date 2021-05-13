#include "SceneHeirarchyPanel.h"

#include <imgui.h>
#include "Apex/Core/ECS/Components.h"

namespace Apex {
	
	SceneHeirarchyPanel::SceneHeirarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}
		
	void SceneHeirarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}
		
	void SceneHeirarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene");
		
		m_Context->m_Registry.each([&](auto entityId)
		{
			Entity entity{ entityId, m_Context.Get() };
			DrawEntityNode(entity);
		});
		
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};
		
		ImGui::End();
	}
	
	void SceneHeirarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= (m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0;
		
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;
		}
		
		if (opened) {
			// Draw children
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)98141, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			
			ImGui::TreePop();
		}
	}
	
}
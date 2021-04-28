#pragma once
#include "Entity.h"

namespace Apex {
	
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;
		
		template<typename Component_t>
		inline decltype(auto) GetComponent()
		{
			return m_Entity.GetComponent<Component_t>();
		}
		
		template<typename... Component_t>
		inline decltype(auto) GetComponents()
		{
			return m_Entity.GetComponents<Component_t...>();
		}
		
	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {}
		
	private:
		Entity m_Entity;
		
		friend class Scene;
	};
	
}

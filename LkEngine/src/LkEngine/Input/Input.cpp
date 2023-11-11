#include "LKpch.h"
#include "LkEngine/Input/Input.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

    Input::Input(Application* app)
        : m_App(s_ptr<Application>(app))
    {
        s_Instance = this;
        LK_ASSERT(m_App);
        LK_ASSERT(s_Instance);
    }

    void Input::Init()
    {
        LOG_DEBUG("Initializing input");
    }
    
    void Input::HandleScene(Scene& scene)
    {
        auto mousePos = Mouse::GetMousePos();
        auto raycastResults = Physics2D::RaycastFromScreen(scene);
		int raycastHits = raycastResults.size();

		if (raycastHits == 1)
		{
		    Raycast2DResult raycast = raycastResults.at(0);
		    Entity entity = raycast.HitEntity;
		    EditorLayer::SelectedEntityID = raycast.HitEntity.GetUUID();
		}

		else if (raycastHits > 1)
		{
		    for (const auto& raycast : raycastResults)
		    {
		        Entity entity = raycast.HitEntity;
		        
		        uint64_t hitEntityID = entity.GetUUID();
		        if (Mouse::IsButtonPressed(MouseButton::ButtonLeft) && EditorLayer::SelectedEntityID == 0)
		        {
		            Entity currentEntity = EditorLayer::SelectedEntity;
		            EditorLayer::SelectedEntityID = hitEntityID;
		            EditorLayer::SelectedEntity = raycast.HitEntity;
		        }
		    }
		}

        if (Keyboard::IsKeyPressed(Key::Escape) && EditorLayer::SelectedEntityID != 0)
        {
            EditorLayer::SelectedEntity = { (entt::entity)0, &scene };
            EditorLayer::SelectedEntityID = 0;
        }

    }

}
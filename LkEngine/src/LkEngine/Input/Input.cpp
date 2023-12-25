#include "LKpch.h"
#include "LkEngine/Input/Input.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

    Input::Input(Application* app)
        : m_App(s_ptr<Application>(app))
    {
        m_Instance = this;
        LK_ASSERT(m_App);
    }

    s_ptr<Input> Input::Create(Application* app)
	{ 
		return std::make_shared<Input>(app);
	}

	void Input::OnUpdate()
	{
		if (m_Scene == nullptr)
			return;

        auto mousePos = Mouse::GetMousePos();
        auto raycastResults = Physics2D::RaycastFromScreen(*m_Scene);
		int raycastHits = raycastResults.size();

		if (raycastHits == 1)
		{
			LOG_DEBUG("Raycast hits: 1");
		    Raycast2DResult raycast = raycastResults.at(0);
		    Entity entity = raycast.HitEntity;
		    //if ((Mouse::IsButtonPressed(MouseButton::ButtonLeft) && EditorLayer::SelectedEntity != raycast.HitEntity))
		    if (Mouse::IsButtonPressed(MouseButton::ButtonLeft))
		    {
		        EditorLayer::SelectedEntity = raycast.HitEntity;
				EditorLayer::SelectedEntityID = raycast.HitEntity.GetUUID();
		    }
		}
		else if (raycastHits > 1)
		{
		    for (const auto& raycast : raycastResults)
		    {
				//LOG_CRITICAL("iterating entity hitcast results");
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
		else // NO HITS
		{
		#if 0
			if (Mouse::IsButtonPressed(MouseButton::ButtonLeft))
			{
				EditorLayer::SelectedEntity = { (entt::entity)0, &scene };
				EditorLayer::SelectedEntityID = 0;
			}
		#endif
		}

        if (Keyboard::IsKeyPressed(Key::Escape) && EditorLayer::SelectedEntityID != 0)
        {
            EditorLayer::SelectedEntity = { (entt::entity)0, m_Scene };
            EditorLayer::SelectedEntityID = 0;
        }
	}

}
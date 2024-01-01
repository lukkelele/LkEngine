#include "LKpch.h"
#include "LkEngine/Input/Input.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

    //s_ptr<Application> m_App = nullptr;
    Application* m_App = nullptr;

    Input::Input(Application* app)
        //: m_App(s_ptr<Application>(app))
    {
        m_Instance = this;
		//m_App = s_ptr<Application>(app);
		m_App = app;
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

		auto* editor = EditorLayer::Get();
		if (Mouse::IsButtonPressed(MouseButton::ButtonLeft) == false)
		{
			if (Keyboard::IsKeyPressed(Key::Escape))
			{
				//if (editor && editor->IsEnabled() && editor->GetSelectedEntityID() != 0)
				LOG_TRACE("De-selecting (clicked escape)");
				if (editor && editor->IsEnabled())
				{
					Entity entity = { (entt::entity)0, m_Scene };
					editor->SetSelectedEntity(entity);
				}
			}
			return;
		}

        auto mousePos = Mouse::GetMousePos();
        auto raycastResults = Physics2D::RaycastFromScreen(*m_Scene);
		int raycastHits = raycastResults.size();

		if (raycastHits == 1)
		{
			LOG_DEBUG("Hitcast result == 1");
		    Raycast2DResult raycast = raycastResults.at(0);
		    Entity entity = raycast.HitEntity;
		    //if ((Mouse::IsButtonPressed(MouseButton::ButtonLeft) && EditorLayer::SelectedEntity != raycast.HitEntity))
		    if (Mouse::IsButtonPressed(MouseButton::ButtonLeft))
		    {
				if (editor)
				{
					//EditorLayer::SelectedEntity = raycast.HitEntity;
					//EditorLayer::SelectedEntityID = raycast.HitEntity.GetUUID();
					editor->SetSelectedEntity(raycast.HitEntity);
				}
		    }
		}
		else if (raycastHits > 1)
		{
			LOG_WARN("Hitcast results > 1");
		    for (auto& raycast : raycastResults)
		    {
		        //Entity entity = raycast.HitEntity;
		        //uint64_t hitEntityID = entity.GetUUID();
				if (editor && editor->IsEnabled())
				{
					editor->SetSelectedEntity(raycast.HitEntity);
					break;
				}

#if 0
				if (Mouse::IsButtonPressed(MouseButton::ButtonLeft))
				{
					if (editor && editor->GetSelectedEntityID() == 0)
					{
						editor->SetSelectedEntity(raycast.HitEntity);
					}
				}
#endif
		    }
		}
		else // NO HITS
		{
			LOG_ERROR("Hitcast results == 0");
		#if 0
			if (Mouse::IsButtonPressed(MouseButton::ButtonLeft))
			{
				EditorLayer::SelectedEntity = { (entt::entity)0, &scene };
				EditorLayer::SelectedEntityID = 0;
			}
		#endif
		}

        if (Keyboard::IsKeyPressed(Key::Escape))
        {
			//if (editor && editor->IsEnabled() && editor->GetSelectedEntityID() != 0)
			LOG_TRACE("De-selecting (clicked escape)");
			if (editor && editor->IsEnabled())
			{
				Entity entity = { (entt::entity)0, m_Scene };
				editor->SetSelectedEntity(entity);
			}
        }
	}

}
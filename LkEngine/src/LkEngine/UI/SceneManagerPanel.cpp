#include "LKpch.h"
#include "SceneManagerPanel.h"

#include "Property.h"
#include "SelectionContext.h"

#include "LkEngine/Renderer/Framebuffer.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

	void DumpAttachedComponents(Entity entity)
	{
		if (entity.HasComponent<IDComponent>() && (uint32_t)entity.GetUUID() != 0)
		{
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "DrawEntityNode Failed for entity {}", entity.GetUUID());
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "IDComponent: {}", entity.HasComponent<IDComponent>());
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "TagComponent: {}", entity.HasComponent<TagComponent>());
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "TransformComponent: {}", entity.HasComponent<TransformComponent>());
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "SpriteComponent: {}", entity.HasComponent<SpriteComponent>());
		}
	}

	SceneManagerPanel::SceneManagerPanel()
		: m_Scene(nullptr)
	{
	}

	SceneManagerPanel::SceneManagerPanel(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	void SceneManagerPanel::Init()
	{
		m_ComponentCopyScene = Scene::CreateEmpty();
		m_ComponentCopyEntity = m_ComponentCopyScene->CreateEntity();
	}

	void SceneManagerPanel::OnRender()
	{
	}

	void SceneManagerPanel::OnImGuiRender()
	{
		ImRect windowRect = { ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax() };

		if (m_Scene)
		{
			ImGui::SeparatorText(fmt::format("Current Scene - {}", m_Scene->m_Name).c_str());
			ImGui::Text("Entities: %d", m_Scene->m_Registry.size());
			m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Scene.Raw() };
				DrawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				SELECTION::SelectedEntity = {};
			}
		}

		if (ImGui::BeginDragDropTargetCustom(windowRect, ImGui::GetCurrentWindow()->ID))
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_ENTITY_NODE", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

			if (payload)
			{
				size_t count = payload->DataSize / sizeof(UUID);

				for (size_t i = 0; i < count; i++)
				{
					UUID entityID = *(((UUID*)payload->Data) + i);
					Entity entity = m_Scene->GetEntityWithUUID(entityID);
					m_Scene->UnparentEntity(entity);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	template<typename T, typename UIFunction>
	void SceneManagerPanel::DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		static const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen 
			| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (!entity.HasComponent<T>())
			return;

		auto& component = entity.GetComponent<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

		if (ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			// FIXME: Crashes right now, need to set on_destruct callbacks
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;
			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
		{
			// Check if component can be removed
			auto& component = entity.GetComponent<T>();
			LK_CORE_DEBUG("Removing component from {}", entity.Name());
			entity.RemoveComponent<T>();
		}
	}

	template<typename T>
	void SceneManagerPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		if (!SELECTION::SelectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				SELECTION::SelectedEntity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	void SceneManagerPanel::DrawComponents(Entity entity)
	{
		if (!entity)
			return;
		LK_CORE_VERIFY(entity.m_Scene, "Entity doesn't have a scene assigned");

		UI::BeginSubwindow(UI_SELECTED_ENTITY_INFO);
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		//---------------------------------------------------------------------------
		// Transform Component
		//---------------------------------------------------------------------------
		DrawComponent<TransformComponent>("Transform", entity, [&entity](auto& transform)
		{
			ImGui::Text("Position");
			UI::Property::PositionXYZ(transform.Translation, 2.0);

			ImGui::Text("Scale");
			ImGui::SliderFloat3("##scale", &transform.Scale.x, 0.10f, 15.0f, "%.2f");

			ImGui::Text("Rotation");
			ImGui::SliderAngle("##2d-rotation", &transform.Rotation2D, -360.0f, 360.0f, "%1.f");
			auto rot = transform.GetRotation();
			//transform.Rotation = glm::angleAxis(transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f));
			transform.SetRotation(glm::angleAxis(rot.x, glm::vec3(0.0f, 0.0f, 1.0f)));
			//transform.SetRotation(rot);
		});

		//---------------------------------------------------------------------------
		// Sprite Component
		//---------------------------------------------------------------------------
		DrawComponent<SpriteComponent>("Sprite", entity, [&entity](auto& sprite)
		{
			ImGui::Text("Sprite Component");
			ImGui::Text("Size");
			ImGui::SameLine();
			ImGui::SliderFloat2("##Size", &sprite.Size.x, 0.0f, 800.0f, "%1.f");
			UI::Property::RGBAColor(sprite.Color);
			
		});

#if 0
		//---------------------------------------------------------------------------
		// Material Component
		//---------------------------------------------------------------------------
		DrawComponent<MaterialComponent>("Material", entity, [&entity](auto& mc)
		{
			auto texture = mc.GetTexture();
			if (!texture)
				return;

			auto textures2D = TextureLibrary::Get()->GetTextures2D();
			std::string textureName = texture->GetName();

			// Selectable texture
			if (ImGui::BeginTable("##MaterialTableProperties", 2, ImGuiTableFlags_SizingStretchProp))
			{
				ImGui::TableSetupColumn("##1", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_IndentDisable);
				ImGui::TableSetupColumn("##2", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_IndentDisable);

				// Texture
				ImGui::TableNextRow(ImGuiTableFlags_SizingStretchProp | ImGuiTableColumnFlags_IndentDisable);
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Texture");

					ImGui::TableSetColumnIndex(1);
					if (ImGui::BeginCombo("##DrawMaterialComponent", textureName.c_str(), ImGuiComboFlags_HeightLargest))
					{
						for (auto& tex : textures2D)
						{
							if (ImGui::Selectable(tex.first.c_str()))
								mc.SetTexture(tex.second);
						}
						ImGui::EndCombo();
					}
				}

				// Roughness
				ImGui::TableNextRow(ImGuiTableFlags_SizingStretchProp | ImGuiTableColumnFlags_IndentDisable);
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Roughness");

					ImGui::TableSetColumnIndex(1);
					auto material = mc.GetMaterial();
					float roughness = material->GetRoughness();
					ImGui::SliderFloat("##Roughness", &roughness, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
					material->SetRoughness(roughness);

				}
				ImGui::EndTable();
			}
		});
#endif

		UI::EndSubwindow();
	}

	std::pair<float, float> SceneManagerPanel::GetMouseViewportSpace(bool primaryViewport)
	{
		auto [mx, my] = ImGui::GetMousePos();
		const auto& viewportBounds = primaryViewport ? EditorLayer::Get()->m_ViewportBounds : EditorLayer::Get()->m_SecondViewportBounds;
		mx -= viewportBounds[0].x;
		my -= viewportBounds[0].y;
		auto viewportWidth = viewportBounds[1].x - viewportBounds[0].x;
		auto viewportHeight = viewportBounds[1].y - viewportBounds[0].y;

		return { (mx / viewportWidth) * 2.0f - 1.0f, ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f };
	}

    void SceneManagerPanel::UI_SelectedEntityProperties()
    {
		static float pos_step_size = 5.0f;
        auto& scene = *Scene::GetActiveScene();
        //Entity entity = scene.GetEntityWithUUID(SelectedEntityID);
		if (!SelectionContext::SelectedEntity)
			return;

		Entity& selectedEntity = SelectionContext::SelectedEntity;

		Window& window = *EditorLayer::Get()->m_Window;
		static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;

		UI::PushID("UI_SELECTED_ENTITY_PROPERTIES");
		// FIXME: The SelectedEntityMenuSize is awful
        ImGui::BeginChild(UI_SELECTED_ENTITY_INFO, EditorLayer::SelectedEntityMenuSize, true, windowFlags);
        if (SelectionContext::SelectedEntity.HasComponent<TransformComponent>())
        {
            ImGui::SeparatorText(SELECTION::SelectedEntity.Name().c_str());
            ImGui::Indent();

			TransformComponent& transform = SELECTION::SelectedEntity.GetComponent<TransformComponent>();
            ImGui::Text("Position");
			UI::Property::PositionXY(transform.Translation, pos_step_size);

            ImGui::Text("Scale");
            ImGui::SliderFloat3("##scale", &transform.Scale.x, 0.10f, 15.0f, "%.2f");

            ImGui::Text("Rotation");
            ImGui::SliderAngle("##2d-rotation", &transform.Rotation2D, -360.0f, 360.0f, "%1.f");
			transform.SetRotation(glm::angleAxis(transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f)));

            ImGui::Unindent();
		}

		if (SELECTION::SelectedEntity.HasComponent<SpriteComponent>())
		{
			ImGui::Text("Sprite Component");
			SpriteComponent& sc = SELECTION::SelectedEntity.GetComponent<SpriteComponent>();
			UI::Property::RGBAColor(sc.Color);
			ImGui::SliderFloat3("##SpriteColor", &sc.Size.x, 0.0f, 800.0f, "%1.f");
		}

        ImGui::EndChild();
		UI::PopID();
    }

	void SceneManagerPanel::DrawEntityNode(Entity entity)
	{
		if (!entity || !entity.HasComponent<IDComponent>() || !entity.HasComponent<TagComponent>())
		{
			DumpAttachedComponents(entity);
			return;
		}

		auto& tag = entity.GetComponent<TagComponent>().Tag;
		std::string tagWithEntityHandle = fmt::format("{}  ({})", tag, entity.m_EntityHandle);

		bool entity_selected = SELECTION::SelectedEntity && (SELECTION::SelectedEntity.GetUUID() == entity.GetUUID());
		ImGuiTreeNodeFlags flags = (entity_selected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		//bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagWithEntityHandle.c_str());

		if (ImGui::IsItemClicked())
		{
			SELECTION::SelectedEntity = entity;
			LK_CORE_DEBUG("Selecting entity {} ({})", entity.Name(), entity.GetUUID());
		}

		bool entityDeleted = false;
		bool entityReset = false;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete object"))
				entityDeleted= true;
			else if (ImGui::MenuItem("Reset object"))
				entityReset = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Scene->DestroyEntity(entity);
			if (SELECTION::SelectedEntity.GetUUID() == entity.GetUUID())
				SELECTION::SelectedEntity = {};
		}
	}

	void SceneManagerPanel::SetScene(const Ref<Scene>& scene)
	{
		m_Scene = scene;
	}

	void SceneManagerPanel::UI_CameraSettings()
	{
		static ImVec2 ButtonSize = ImVec2(92, 50);
		//UI::Begin("Editor Camera", ImGuiDockNodeFlags_DockSpace);
		ImGui::BeginGroup();
		{
			if (ImGui::Button("2D Mode", ButtonSize))
			{
				LK_CORE_DEBUG_TAG("SceneManagerPanel", "Button clicked -> 2D Mode");
				auto& editorCamera = *m_Scene->GetEditorCamera();
				editorCamera.ApplyViewMode(EditorCamera::ViewMode::Fake2D);
				editorCamera.SetPosition({ -10, 8, -10 });
			}

		}
		ImGui::EndGroup();
		//UI::End();
	}

}
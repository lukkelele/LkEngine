#include "LKpch.h"
#include "SceneManagerPanel.h"

#include "Property.h"
#include "LkEngine/Core/SelectionContext.h"

#include "LkEngine/Renderer/Framebuffer.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

	static void DumpAttachedComponents(LEntity entity)
	{
		if (entity.HasComponent<LIDComponent>() && (static_cast<uint32_t>(entity.GetUUID()) != 0))
		{
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "LIDComponent: {}", entity.HasComponent<LIDComponent>());
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "LTagComponent: {}", entity.HasComponent<LTagComponent>());
		}
		if (entity.HasComponent<LTransformComponent>())
		{
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "LTransformComponent: {}", entity.GetComponent<LTransformComponent>().ToString());
		}
	}

	LSceneManagerPanel::LSceneManagerPanel()
		: m_Scene(nullptr)
	{
	}

	LSceneManagerPanel::LSceneManagerPanel(const TObjectPtr<LScene>& InScene)
		: m_Scene(InScene)
	{
	}

	LSceneManagerPanel::~LSceneManagerPanel()
	{
	}

	void LSceneManagerPanel::Init()
	{
		m_ComponentCopyScene = LScene::CreateEmpty();
		m_ComponentCopyEntity = m_ComponentCopyScene->CreateEntity();
	}

	void LSceneManagerPanel::OnRender()
	{
	}

	void LSceneManagerPanel::OnImGuiRender()
	{
		const ImRect WindowContent = { ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax() };

		if (m_Scene)
		{
			ImGui::SeparatorText(fmt::format("Current Scene - {}", m_Scene->Name).c_str());
			ImGui::Text("Entities: %d", m_Scene->m_Registry.size());
			m_Scene->m_Registry.each([&](auto entityID)
			{
				LEntity entity{ entityID, m_Scene.Get() };
				DrawEntityNode(entity);
			});

			/// TODO: Make it so the input interface is used here instead of raw ImGui calls.
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				//SELECTION::SelectedEntity = {};
			}
		}

		if (ImGui::BeginDragDropTargetCustom(WindowContent, ImGui::GetCurrentWindow()->ID))
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_ENTITY_NODE", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

			if (payload)
			{
				size_t count = payload->DataSize / sizeof(UUID);

				for (size_t i = 0; i < count; i++)
				{
					UUID entityID = *(((UUID*)payload->Data) + i);
					LEntity entity = m_Scene->GetEntityWithUUID(entityID);
					m_Scene->UnparentEntity(entity);
				}
			}

			ImGui::EndDragDropTarget();
		}
	}

	template<typename T, typename UIFunction>
	void LSceneManagerPanel::DrawComponent(const std::string& name, LEntity entity, UIFunction uiFunction)
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
	void LSceneManagerPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
#if false
		if (!SELECTION::SelectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				SELECTION::SelectedEntity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
#endif
	}

	void LSceneManagerPanel::DrawComponents(LEntity entity)
	{
		if (!entity)
		{
			return;
		}
		LK_CORE_VERIFY(entity.m_Scene, "Entity doesn't have a scene assigned");

		UI::BeginSubwindow(UI_SELECTED_ENTITY_INFO);
		if (entity.HasComponent<LTagComponent>())
		{
			auto& tag = entity.GetComponent<LTagComponent>().Tag;

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
		DrawComponent<LTransformComponent>("Transform", entity, [&entity](auto& transform)
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
		DrawComponent<LSpriteComponent>("Sprite", entity, [&entity](auto& sprite)
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

	std::pair<float, float> LSceneManagerPanel::GetMouseViewportSpace(bool primaryViewport)
	{
		auto [MousePosX, MousePosY] = ImGui::GetMousePos();

		const auto& viewportBounds = primaryViewport ? LEditorLayer::Get()->m_ViewportBounds : LEditorLayer::Get()->m_SecondViewportBounds;
		MousePosX -= viewportBounds[0].x;
		MousePosY -= viewportBounds[0].y;

		const float viewportWidth = viewportBounds[1].x - viewportBounds[0].x;
		const float viewportHeight = viewportBounds[1].y - viewportBounds[0].y;

		return { 
			(MousePosX / viewportWidth) * 2.0f - 1.0f, 
			((MousePosY / viewportHeight) * 2.0f - 1.0f) * -1.0f 
		};
	}

    void LSceneManagerPanel::UI_SelectedEntityProperties()
    {
#if 0 
		static float pos_step_size = 5.0f;
        const LScene& Scene = *LScene::GetActiveScene();
        //Entity entity = scene.GetEntityWithUUID(SelectedEntityID);
		if (!SelectionContext::SelectedEntity)
		{
			return;
		}

		LEntity& selectedEntity = SelectionContext::SelectedEntity;

		LWindow& WindowRef = *EditorLayer::Get()->Window;
		static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;

		UI::PushID("UI_SELECTED_ENTITY_PROPERTIES");
		// FIXME: The SelectedEntityMenuSize is awful
        ImGui::BeginChild(UI_SELECTED_ENTITY_INFO, EditorLayer::SelectedEntityMenuSize, true, windowFlags);
        if (SelectionContext::SelectedEntity.HasComponent<LTransformComponent>())
        {
            ImGui::SeparatorText(SELECTION::SelectedEntity.Name().c_str());
            ImGui::Indent();

			LTransformComponent& transform = SELECTION::SelectedEntity.GetComponent<LTransformComponent>();
            ImGui::Text("Position");
			UI::Property::PositionXY(transform.Translation, pos_step_size);

            ImGui::Text("Scale");
            ImGui::SliderFloat3("##scale", &transform.Scale.x, 0.10f, 15.0f, "%.2f");

            ImGui::Text("Rotation");
            ImGui::SliderAngle("##2d-rotation", &transform.Rotation2D, -360.0f, 360.0f, "%1.f");
			transform.SetRotation(glm::angleAxis(transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f)));

            ImGui::Unindent();
		}

		if (SELECTION::SelectedEntity.HasComponent<LSpriteComponent>())
		{
			ImGui::Text("Sprite Component");
			LSpriteComponent& sc = SELECTION::SelectedEntity.GetComponent<LSpriteComponent>();
			UI::Property::RGBAColor(sc.Color);
			ImGui::SliderFloat3("##SpriteColor", &sc.Size.x, 0.0f, 800.0f, "%1.f");
		}

        ImGui::EndChild();
		UI::PopID();
#endif

    }

	void LSceneManagerPanel::DrawEntityNode(LEntity entity)
	{
		if (!entity || !entity.HasComponent<LIDComponent>() || !entity.HasComponent<LTagComponent>())
		{
			DumpAttachedComponents(entity);
			return;
		}

		const std::string& Tag = entity.GetComponent<LTagComponent>().Tag;
		std::string tagWithEntityHandle = fmt::format("{}  ({})", Tag, entity.m_EntityHandle);

		//bool bEntitySelected = SELECTION::SelectedEntity && (SELECTION::SelectedEntity.GetUUID() == entity.GetUUID());
		const bool bEntitySelected = false;
		ImGuiTreeNodeFlags flags = (bEntitySelected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		//bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagWithEntityHandle.c_str());

		if (ImGui::IsItemClicked())
		{
			//SELECTION::SelectedEntity = entity;
			//LK_CORE_DEBUG("Selecting entity {} ({})", entity.Name(), entity.GetUUID());
		}

		bool entityDeleted = false;
		bool entityReset = false;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete object"))
			{
				entityDeleted= true;
			}
			else if (ImGui::MenuItem("Reset object"))
			{
				entityReset = true;
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			const bool bOpened = ImGui::TreeNodeEx((void*)9817239, Flags, Tag.c_str());
			if (bOpened)
			{
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Scene->DestroyEntity(entity);
#if 0 
			if (SELECTION::SelectedEntity.GetUUID() == entity.GetUUID())
			{
				SELECTION::SelectedEntity = {};
			}
#endif
		}
	}

	void LSceneManagerPanel::SetScene(const TObjectPtr<LScene>& Scene)
	{
		m_Scene = Scene;
	}

	void LSceneManagerPanel::UI_CameraSettings()
	{
		static ImVec2 ButtonSize = ImVec2(92, 50);
		//UI::Begin("Editor Camera", ImGuiDockNodeFlags_DockSpace);
		ImGui::BeginGroup();
		{
			if (ImGui::Button("2D Mode", ButtonSize))
			{
				LK_CORE_DEBUG_TAG("SceneManagerPanel", "Button clicked -> 2D Mode");
				LEditorCamera& EditorCamera = *m_Scene->GetEditorCamera();
				EditorCamera.ApplyViewMode(LEditorCamera::ViewMode::Fake2D);
				EditorCamera.SetPosition({ -10, 8, -10 });
			}

		}
		ImGui::EndGroup();
		//UI::End();
	}

}
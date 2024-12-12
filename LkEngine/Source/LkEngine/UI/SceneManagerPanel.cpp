#include "LKpch.h"
#include "SceneManagerPanel.h"

#include "Property.h"
#include "LkEngine/Core/SelectionContext.h"

#include "LkEngine/Renderer/FrameBuffer.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/UI/UILayer.h"


namespace LkEngine {

	/// REFACTOR
	/* Log helper function. */
	static void DumpAttachedComponents(LEntity Entity)
	{
		if (Entity.HasComponent<LIDComponent>() && (static_cast<uint32_t>(Entity.GetUUID()) != 0))
		{
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "LIDComponent: {}", Entity.HasComponent<LIDComponent>());
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "LTagComponent: {}", Entity.HasComponent<LTagComponent>());
		}
		if (Entity.HasComponent<LTransformComponent>())
		{
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "LTransformComponent: {}", 
							  Entity.GetComponent<LTransformComponent>().ToString());
		}
	}

	LSceneManagerPanel::LSceneManagerPanel(TObjectPtr<LScene> InScene)
		: Scene(InScene)
	{
		LCLASS_REGISTER();
	}

	void LSceneManagerPanel::Initialize()
	{
		GOnSceneSetActive.Add(this, &LSceneManagerPanel::SetScene);
	}

	void LSceneManagerPanel::OnRender()
	{
	}

	void LSceneManagerPanel::OnRenderUI(bool& IsOpen)
	{
		const bool IsWindowDocked = UI::IsWindowDocked(LK_UI_SCENEMANAGER);
		if (bIsWindow)
		{
			if (ImGuiWindow* ThisWindow = ImGui::FindWindowByName(LK_UI_SCENEMANAGER))
			{
				ImGui::BringWindowToDisplayFront(ThisWindow);
			}

			ImGuiWindowFlags Flags = ImGuiWindowFlags_None;
			if (IsWindowDocked)
			{
				//Flags |= ImGuiWindowFlags_NoMove;
			}

			ImGui::Begin(LK_UI_SCENEMANAGER, &IsOpen, Flags);
		}

		const ImRect WindowContent = { ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax() };

		if (Scene)
		{
			ImGui::SeparatorText(LK_FORMAT_STRING("{}", Scene->Name).c_str());
			auto& EntityStorage = Scene->Registry.storage<entt::entity>();
			ImGui::Text("Content: %d", EntityStorage.size());

			for (auto EntityID : EntityStorage)
			{
				LEntity Entity { EntityID, Scene.Get() };
				DrawEntityNode(Entity);
			}
		}

		if (ImGui::BeginDragDropTargetCustom(WindowContent, ImGui::GetCurrentWindow()->ID))
		{
			const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("SCENE_ENTITY_NODE", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
			if (Payload)
			{
				const std::size_t Count = (Payload->DataSize / sizeof(UUID));
				for (std::size_t i = 0; i < Count; i++)
				{
					const UUID EntityID = *((static_cast<UUID*>(Payload->Data)) + i);
					LEntity Entity = Scene->GetEntityWithUUID(EntityID);
					Scene->UnparentEntity(Entity);
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (bIsWindow)
		{
			ImGui::End();
		}
	}

	template<typename T, typename TUIFunction>
	void LSceneManagerPanel::DrawComponent(const std::string& ComponentName, LEntity Entity, TUIFunction UIFunction)
	{
		static const ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen 
			| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (!Entity.HasComponent<T>())
		{
			return;
		}

		auto& Component = Entity.GetComponent<T>();
		const ImVec2 ContentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float LineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

		ImGui::Separator();
		bool bOpen = ImGui::TreeNodeEx(
			(void*)typeid(T).hash_code(), 
			TreeNodeFlags, 
			ComponentName.c_str()
		);
		ImGui::PopStyleVar();

		ImGui::SameLine(ContentRegionAvailable.x - LineHeight * 0.5f);
		if (ImGui::Button("+", ImVec2(LineHeight, LineHeight)))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool bRemoveComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			// FIXME: Crashes right now, need to set on_destruct callbacks
			if (ImGui::MenuItem("Remove Component"))
			{
				bRemoveComponent = true;
			}
			ImGui::EndPopup();
		}

		if (bOpen)
		{
			UIFunction(Component);
			ImGui::TreePop();
		}

		if (bRemoveComponent)
		{
			/* Check if the component can be removed. */
			auto& Component = Entity.GetComponent<T>();
			LK_CORE_DEBUG("Removing Component from {}", Entity.Name());
			Entity.RemoveComponent<T>();
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

	void LSceneManagerPanel::DrawComponents(LEntity Entity)
	{
		if (!Entity)
		{
			return;
		}

		LK_CORE_VERIFY(Entity.Scene, "Entity doesn't have a scene assigned");

		ImGui::PushID(UI::ID::SelectedEntityInfo);
		if (ImGui::Begin("SceneManagerPanel-FIXME", nullptr))
		{
			if (Entity.HasComponent<LTagComponent>())
			{
				auto& Tag = Entity.GetComponent<LTagComponent>().Tag;

				char Buffer[256];
				memset(Buffer, 0, sizeof(Buffer));
				std::strncpy(Buffer, Tag.c_str(), sizeof(Buffer));
				if (ImGui::InputText("##Tag", Buffer, sizeof(Buffer)))
				{
					Tag = std::string(Buffer);
				}
			}

			ImGui::SameLine();
			ImGui::PushItemWidth(-1);

			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("AddComponent");
			}
			if (ImGui::BeginPopup("AddComponent"))
			{
				ImGui::EndPopup();
			}
			ImGui::PopItemWidth();

			//---------------------------------------------------------------------------
			// Transform Component
			//---------------------------------------------------------------------------
			DrawComponent<LTransformComponent>("Transform", Entity, [&Entity](auto& Transform)
			{
				ImGui::Text("Position");
				UI::Property::PositionXYZ(Transform.Translation, 2.0);

				ImGui::Text("Scale");
				ImGui::SliderFloat3("##scale", &Transform.Scale.x, 0.10f, 15.0f, "%.2f");

				ImGui::Text("Rotation");
				ImGui::SliderAngle("##2d-rotation", &Transform.Rotation2D, -360.0f, 360.0f, "%1.f");
				auto rot = Transform.GetRotation();
				//Transform.Rotation = glm::angleAxis(Transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f));
				Transform.SetRotation(glm::angleAxis(rot.x, glm::vec3(0.0f, 0.0f, 1.0f)));
				//Transform.SetRotation(rot);
			});

			//---------------------------------------------------------------------------
			// Sprite Component
			//---------------------------------------------------------------------------
			DrawComponent<LSpriteComponent>("Sprite", Entity, [&Entity](auto& Sprite)
			{
				ImGui::Text("Sprite Component");
				ImGui::Text("Size");
				ImGui::SameLine();
				ImGui::SliderFloat2("##Size", &Sprite.Size.x, 0.0f, 800.0f, "%1.f");
				UI::Property::RGBAColor(Sprite.Color);
				
			});

	#if 0
			//---------------------------------------------------------------------------
			// Material Component
			//---------------------------------------------------------------------------
			DrawComponent<MaterialComponent>("Material", Entity, [&Entity](auto& mc)
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

		}

		ImGui::End();
	}

	std::pair<float, float> LSceneManagerPanel::GetMouseViewportSpace(const bool primaryViewport)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED("FIXME");
	#if 0
		auto [MousePosX, MousePosY] = ImGui::GetMousePos();

		const auto& ViewportBounds = primaryViewport 
			? LEditorLayer::Get()->ViewportBounds 
			: LEditorLayer::Get()->SecondViewportBounds;

		MousePosX -= ViewportBounds[0].X;
		MousePosY -= ViewportBounds[0].Y;

		const float ViewportWidth = ViewportBounds[1].X - ViewportBounds[0].X;
		const float ViewportHeight = ViewportBounds[1].Y - ViewportBounds[0].Y;

		return { 
			(MousePosX / ViewportWidth) * 2.0f - 1.0f, 
			((MousePosY / ViewportHeight) * 2.0f - 1.0f) * -1.0f 
		};
	#endif
		return {};
	}

    void LSceneManagerPanel::UI_SelectedEntityProperties()
    {
#if 0 
		static float pos_step_size = 5.0f;
        const LScene& Scene = *LScene::GetActiveScene();
        //Entity Entity = scene.GetEntityWithUUID(SelectedEntityID);
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

			LTransformComponent& Transform = SELECTION::SelectedEntity.GetComponent<LTransformComponent>();
            ImGui::Text("Position");
			UI::Property::PositionXY(Transform.Translation, pos_step_size);

            ImGui::Text("Scale");
            ImGui::SliderFloat3("##scale", &Transform.Scale.x, 0.10f, 15.0f, "%.2f");

            ImGui::Text("Rotation");
            ImGui::SliderAngle("##2d-rotation", &Transform.Rotation2D, -360.0f, 360.0f, "%1.f");
			Transform.SetRotation(glm::angleAxis(Transform.Rotation2D, glm::vec3(0.0f, 0.0f, 1.0f)));

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

	void LSceneManagerPanel::DrawEntityNode(LEntity Entity)
	{
		if (!Entity || !Entity.HasComponent<LIDComponent>() || !Entity.HasComponent<LTagComponent>())
		{
			DumpAttachedComponents(Entity);
			return;
		}

		const std::string& Tag = Entity.GetComponent<LTagComponent>().Tag;
		std::string TagWithEntityHandle = LK_FORMAT_STRING("{} ({})", Tag, Entity.Handle);
		if (Entity.HasComponent<LMeshComponent>())
		{
			TagWithEntityHandle = LK_FORMAT_STRING("{} (AssetID: {})", Tag, Entity.GetComponent<LMeshComponent>().Mesh);
		}
		else
		{
			TagWithEntityHandle = LK_FORMAT_STRING("{} ({})", Tag, Entity.Handle);
		}

		const bool bEntitySelected = false;
		ImGuiTreeNodeFlags flags = (bEntitySelected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool bOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)Entity, flags, TagWithEntityHandle.c_str());

		if (ImGui::IsItemClicked())
		{
			if (Entity.HasComponent<LMeshComponent>())
			{
				LK_UI_INFO("Clicked: {} (AssetID={})", Tag, Entity.GetComponent<LMeshComponent>().Mesh);
			}
			else
			{
				LK_UI_INFO("Clicked: {}", Tag);
			}

			LSelectionContext::Select(Entity);
		}

		bool EntityDeleted = false;
		bool EntityReset = false;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete object"))
			{
				EntityDeleted = true;
			}
			else if (ImGui::MenuItem("Reset object"))
			{
				EntityReset = true;
			}

			ImGui::EndPopup();
		}

		if (bOpened)
		{
			ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			const bool bOpened = ImGui::TreeNodeEx((void*)9817239, Flags, Tag.c_str());
			if (bOpened)
			{
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (EntityDeleted)
		{
			Scene->DestroyEntity(Entity);
		}
	}

	void LSceneManagerPanel::SetScene(const TObjectPtr<LScene>& InScene)
	{
		LK_CORE_VERIFY(InScene, "Invalid scene reference");
		Scene = InScene;
	}

	void LSceneManagerPanel::UI_CameraSettings()
	{
		static ImVec2 ButtonSize = ImVec2(92, 50);
		ImGui::BeginGroup();
		{
			if (ImGui::Button("Camera Mode", ButtonSize))
			{
			}
		}
		ImGui::EndGroup();
	}

}

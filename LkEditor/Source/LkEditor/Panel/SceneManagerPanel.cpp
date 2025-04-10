#include "SceneManagerPanel.h"

#include <LkEngine/Core/SelectionContext.h>

#include <LkEngine/Renderer/Framebuffer.h>
#include <LkEngine/Renderer/Color.h>

#include <LkEngine/Editor/EditorCore.h>

#include <LkEngine/Project/Project.h>

#include <LkEngine/Renderer/UI/Property.h>
#include <LkEngine/Renderer/UI/UILayer.h>

#include "LkEditor/EditorLayer.h"

namespace LkEngine {

	namespace 
	{
		FSceneRendererFlushData FlushData{};
		std::vector<FSceneSelectionData> SceneSelectionData{};
	}

	LSceneManagerPanel::LSceneManagerPanel(TObjectPtr<LScene> InScene)
		: Scene(InScene)
	{
		LOBJECT_REGISTER();
	}

	void LSceneManagerPanel::Initialize()
	{
		GOnSceneSetActive.Add(this, &LSceneManagerPanel::SetScene);
	}

	void LSceneManagerPanel::RenderUI(bool& IsOpen)
	{
		if (bIsWindow)
		{
			if (ImGuiWindow* ThisWindow = ImGui::FindWindowByName(PanelID::SceneManager))
			{
				if (ThisWindow->DockNode)
				{
					ThisWindow->DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
				}
			}

			static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoCollapse 
				| ImGuiWindowFlags_NoNavFocus;
			ImGui::Begin(PanelID::SceneManager, &IsOpen, WindowFlags);
			//ImGui::Text("Window Focused: %s", UI::IsWindowFocused(PanelID::SceneManager) ? "Yes" : "No");
		}

		if (Scene)
		{
			LK_CORE_ASSERT(!Scene->Name.empty());
			ImGui::SeparatorText(LK_FMT_LIB::format("{}", Scene->Name).c_str());

			/* Scene raycast info. */
			{
				UI_SceneRaycastData();
			}

			/* Render info. */
			{
				ImGui::Text("Drawlist Info");
				ImGui::Indent();
				{
					ImGui::Text("StaticMesh Size:  %d", FlushData.StaticMeshDrawListSize);
					ImGui::Text("Mesh Size:        %d", FlushData.MeshDrawListSize);
				}
				ImGui::Unindent();

				ImGui::Separator();
			}

			/* Scene content. */
			auto& EntityStorage = Scene->Registry.storage<entt::entity>();
			for (entt::entity EntityID : EntityStorage)
			{
				if (static_cast<uint64_t>(EntityID) == 0)
				{
					continue;
				}

				LEntity Entity { EntityID, Scene.Get() };
				DrawEntityNode(Entity);
			}
		}

	#if 0
		{
			UI::FScopedStyle WindowPadding(ImGuiStyleVar_WindowPadding, ImVec2(2.0, 4.0f));
			ImGui::Begin("##EntityProperties");
			DrawComponents({ LSelectionContext::SelectedEntityID });
			ImGui::End();
		}
	#endif

		if (bIsWindow)
		{
		#if 0
			UI::End();
		#else
			LK_UI_DEBUG_WINDOW_ON_HOVER();
			ImGui::End();
		#endif
		}
	}

	void LSceneManagerPanel::SerializeToYaml(YAML::Emitter& Out) const
	{
		/* Empty for now. */
	}

	void LSceneManagerPanel::DeserializeFromYaml(const YAML::Node& Data)
	{
		/* Empty for now. */
	}

	template<typename T, typename TUIFunction>
	void LSceneManagerPanel::DrawComponent(const std::string& ComponentName, 
										   LEntity Entity, 
										   const std::vector<LUUID>& Entities,
										   TUIFunction UIFunction)
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

		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		//float LineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 2, 2 });
		const float LineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 1.0f;

		/* TODO: Reduce the height. */
		ImGui::Separator();
		const bool bOpen = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), TreeNodeFlags, ComponentName.c_str());
		ImGui::PopStyleVar();

		ImGui::SameLine(ContentRegionAvailable.x - LineHeight * 0.5f);
		if (ImGui::Button("+", ImVec2(LineHeight, LineHeight)))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool bRemoveComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
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
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "Removing component from: {}", Entity.Name());
			Entity.RemoveComponent<T>();
		}
	}

	template<typename T>
	void LSceneManagerPanel::DisplayAddComponentEntry(const std::string& EntryName)
	{
		LK_UNUSED(EntryName);
	}

	void LSceneManagerPanel::DrawEntityNode(LEntity Entity)
	{
		const char* Name = "Unnamed Entity";
		if (Entity.HasComponent<LTagComponent>())
		{
			Name = Entity.GetComponent<LTagComponent>().Tag.c_str();
		}

		const bool IsEntitySelected = LSelectionContext::IsSelected(ESelectionContext::Scene, Entity);

		ImGuiTreeNodeFlags TreeNodeFlags = (IsEntitySelected ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None);
		TreeNodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		const ImGuiID EntityImGuiID = ImGui::GetID((void*)(uint64_t)(uint32_t)Entity);

		const bool WasNodeOpened = ImGui::TreeNodeBehaviorIsOpen(EntityImGuiID);
		const bool NodeOpened = ImGui::TreeNodeEx((void*)EntityImGuiID, TreeNodeFlags, Entity.Name().c_str());

		/// TODO: Remove the depenency for the selection vector in the call to DrawComponent.
		/* Get the selected entities. */
		const std::vector<LUUID>& Entities = LSelectionContext::GetSelected(ESelectionContext::Scene);

		if (NodeOpened)
		{
			const std::string UuidStr = LK_FMT_LIB::format("{}", (uint64_t)Entity.GetUUID());
			ImGui::Text("UUID: %s", UuidStr.c_str());

			/**
			 * Draw: MeshComponent
			 */
			if (Entity.HasComponent<LMeshComponent>())
			{
				TObjectPtr<IAssetManager> AssetManager = LProject::Current()->GetAssetManager();
				DrawComponent<LMeshComponent>("Mesh", Entity, Entities, [&Entity, &AssetManager](LMeshComponent& Mesh)
				{
					const FAssetMetadata& Metadata = AssetManager->GetMetadata(Mesh.Mesh);
					const std::string AssetHandleStr = LK_FMT_LIB::format("{}", (uint64_t)Mesh.Mesh);
					ImGui::Text("Asset: %s", AssetHandleStr.c_str());
					ImGui::Text("Loaded: %s", Metadata.bIsDataLoaded ? "Yes" : "No");
					ImGui::Text("Memory Asset: %s", Metadata.bIsMemoryAsset ? "Yes" : "No");
					ImGui::Text("Filepath: %s", Metadata.FilePath.string().c_str());
				});
			}

			/**
			 * Draw: StaticMeshComponent
			 */
			if (Entity.HasComponent<LStaticMeshComponent>())
			{
				TObjectPtr<LRuntimeAssetManager> AssetManager = LProject::Current()->GetRuntimeAssetManager();
				DrawComponent<LStaticMeshComponent>("StaticMesh", Entity, Entities, [&Entity, &AssetManager](LStaticMeshComponent& Mesh)
				{
					const FAssetMetadata& Metadata = AssetManager->GetMetadata(Mesh.StaticMesh);
					const std::string AssetHandleStr = LK_FMT_LIB::format("{}", (uint64_t)Mesh.StaticMesh);
					ImGui::Text("Asset: %s", AssetHandleStr.c_str());
					ImGui::Text("Loaded: %s", Metadata.bIsDataLoaded ? "Yes" : "No");
					ImGui::Text("Memory Asset: %s", Metadata.bIsMemoryAsset ? "Yes" : "No");
					ImGui::Text("Filepath: %s", Metadata.FilePath.string().c_str());
				});
			}

			/**
			 * Draw: TransformComponent
			 */
			if (Entity.HasComponent<LTransformComponent>())
			{
				DrawComponent<LTransformComponent>("Transform", Entity, Entities, [&](LTransformComponent& TransformComp)
				{
					ImGui::BeginTable("VectorControl", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip);
					ImGui::TableSetupColumn("LabelColumn", 0, 100.0f);
					ImGui::TableSetupColumn("ValueColumn", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip, ImGui::GetContentRegionAvail().x - 100.0f);

					auto GetTransform = [](const LTransformComponent& Other) { return Other.Translation; };
					auto GetRotationEuler = [](const LTransformComponent& Other) { return Other.GetRotationEuler(); };
					auto GetScale = [](const LTransformComponent& Other) { return Other.Scale; };
					uint32_t TranslationAxes = GetInconsistentVectorAxis<glm::vec3, LTransformComponent>(GetTransform);
					uint32_t RotationAxes = GetInconsistentVectorAxis<glm::vec3, LTransformComponent>(GetRotationEuler);
					uint32_t ScaleAxes = GetInconsistentVectorAxis<glm::vec3, LTransformComponent>(GetScale);

					/* Store current values. */
					glm::vec3 Translation = TransformComp.Translation;
					glm::vec3 Rotation = glm::degrees(TransformComp.GetRotationEuler());
					glm::vec3 Scale = TransformComp.Scale;
					const glm::vec3 OldTranslation = Translation;
					const glm::vec3 OldRotation = Rotation;
					const glm::vec3 OldScale = Scale;

					/* Flags to detect changes in the vector controls. */
					static bool TranslationModified = false;
					static bool RotationModified = false;
					static bool ScaleModified = false;

					bool Changed = false;

					auto& EditorSettings = FEditorSettings::Get();

					/* Transform. */
					ImGui::TableNextRow();
					Changed = UI::Draw::Vec3Control(
						"Transform", 
						Translation, 
						0.10f, /* Reset value. */
						EditorSettings.TranslationSnapValue, 
						UI::Slider::MIN_UNLIMITED,
						UI::Slider::MAX_UNLIMITED,
						100.0f,         /* Column width. */
						TranslationAxes /* Axes. */
					);

					/* Rotation. */
					ImGui::TableNextRow();
					Changed |= UI::Draw::Vec3Control(
						"Rotation", 
						Rotation, 
						0.0f, /* Reset value. */
						EditorSettings.RotationSnapValue, 
						UI::Slider::MIN_UNLIMITED,
						UI::Slider::MAX_UNLIMITED,
						100.0f,       /* Column width. */
						RotationAxes  /* Axes. */
					);

					/* Scale. */
					ImGui::TableNextRow();
					Changed |= UI::Draw::Vec3Control(
						"Scale", 
						Scale, 
						0.010f, /* Reset value. */
						EditorSettings.ScaleSnapValue, 
						UI::Slider::MIN_UNLIMITED,
						UI::Slider::MAX_UNLIMITED,
						100.0f,    /* Column width. */
						ScaleAxes  /* Axes. */
					);

					if (Changed)
					{
						/* Disabled until the manually edited flag works for the UI sliders. */
						if (TranslationModified || RotationModified || ScaleModified)
						{
					#if 0
							TranslationAxes = GetInconsistentVectorAxis(Translation, OldTranslation);
							RotationAxes = GetInconsistentVectorAxis(Rotation, OldRotation);
							ScaleAxes = GetInconsistentVectorAxis(Scale, OldScale);

							/* Translation. */
							if ((TranslationAxes & EVectorAxis::X) != 0)
							{
								TransformComp.Translation.x = Translation.x;
							}
							if ((TranslationAxes & EVectorAxis::Y) != 0)
							{
								TransformComp.Translation.y = Translation.y;
							}
							if ((TranslationAxes & EVectorAxis::Z) != 0)
							{
								TransformComp.Translation.z = Translation.z;
							}

							/* Rotation (Radians). */
							glm::vec3 ComponentRotation = TransformComp.GetRotationEuler();
							if ((RotationAxes & EVectorAxis::X) != 0)
							{
								ComponentRotation.x = glm::radians(Rotation.x);
							}
							if ((RotationAxes & EVectorAxis::Y) != 0)
							{
								ComponentRotation.y = glm::radians(Rotation.y);
							}
							if ((RotationAxes & EVectorAxis::Z) != 0)
							{
								ComponentRotation.z = glm::radians(Rotation.z);
							}
							TransformComp.SetRotationEuler(ComponentRotation);

							/* Scale. */
							if ((ScaleAxes & EVectorAxis::X) != 0)
							{
								TransformComp.Scale.x = Scale.x;
							}
							if ((ScaleAxes & EVectorAxis::Y) != 0)
							{
								TransformComp.Scale.y = Scale.y;
							}
							if ((ScaleAxes & EVectorAxis::Z) != 0)
							{
								TransformComp.Scale.z = Scale.z;
							}
						#endif
						}
						else
						{
							const glm::vec3 TranslationDiff = Translation - OldTranslation;
							const glm::vec3 RotationDiff = Rotation - OldRotation;
							const glm::vec3 ScaleDiff = Scale - OldScale;

							TransformComp.Translation += TranslationDiff;
							glm::vec3 ComponentRotation = TransformComp.GetRotationEuler();
							ComponentRotation += glm::radians(RotationDiff);
							TransformComp.SetRotationEuler(ComponentRotation);
							TransformComp.Scale += ScaleDiff;

						#if 0
							for (const auto& EntityID : Entities)
							{
								LEntity Entity = Scene->GetEntityWithUUID(EntityID);
								auto& Component = Entity.GetComponent<LTransformComponent>();

								Component.Translation += TranslationDiff;

								glm::vec3 ComponentRotation = Component.GetRotationEuler();
								ComponentRotation += glm::radians(RotationDiff);
								Component.SetRotationEuler(ComponentRotation);

								Component.Scale += ScaleDiff;
							}
						#endif
						}
					}

					ImGui::EndTable();

					UI::ShiftCursorY(-8.0f);
					UI::Draw::Underline();
					UI::ShiftCursorY(18.0f);
				});

				DrawEntityCreateMenu(Entity);

				if (ImGui::MenuItem("Delete"))
				{
					LK_CORE_DEBUG_TAG("SceneManager", "Deleting entity: {} ({})", Entity.Name(), Entity.GetUUID());
				}
			}

			/* Select the entity in the scene context if the node was opened this tick. */
			if (!IsEntitySelected && NodeOpened)
			{
				LK_CORE_INFO_TAG("SceneManager", "Selecting {} ({}) in the scene context", Entity.Name(), Entity.GetUUID());
				LSelectionContext::Select(ESelectionContext::Scene, Entity);
			}

			ImGui::TreePop();
		}
	}

	void LSceneManagerPanel::DrawComponents(const std::vector<LUUID>& Entities)
	{
		if (Entities.empty())
		{
			return;
		}

		{
			UI::FScopedColor TableBg(ImGuiCol_ChildBg, RGBA32::Text::Darker);
			ImGuiTableFlags TableFlags = ImGuiTableFlags_NoPadInnerX
				| ImGuiTableFlags_Resizable
				| ImGuiTableFlags_Reorderable
				| ImGuiTableFlags_ScrollY;

			static constexpr int NumColumns = 3;
			if (ImGui::BeginTable("##SceneManager-Table", NumColumns, TableFlags, ImVec2(ImGui::GetContentRegionAvail())))
			{
				ImGui::TableSetupColumn("Label");
				ImGui::TableSetupColumn("Type");
				ImGui::TableSetupColumn("Visibility");
				/* Headers. */
				{
					static constexpr float EdgeOffset = 4.0f;
					const ImU32 ColorActive = RGBA32::Titlebar::Green;
					UI::FScopedColorStack HeaderColors(ImGuiCol_HeaderHovered, ColorActive, ImGuiCol_HeaderActive, ColorActive);

					ImGui::TableSetupScrollFreeze(ImGui::TableGetColumnCount(), 1);

					ImGui::TableNextRow(ImGuiTableRowFlags_Headers, 22.0f);
					for (int ColumnIndex = 0; ColumnIndex < ImGui::TableGetColumnCount(); ColumnIndex++)
					{
						ImGui::TableSetColumnIndex(ColumnIndex);
						const char* ColumnName = ImGui::TableGetColumnName(ColumnIndex);
						UI::FScopedID ColumnID(ColumnIndex);

						UI::ShiftCursor(EdgeOffset * 3.0f, EdgeOffset * 2.0f);
						ImGui::TableHeader(ColumnName);
						UI::ShiftCursor(-EdgeOffset * 3.0f, -EdgeOffset * 2.0f);
					}
					ImGui::SetCursorPosX(ImGui::GetCurrentTable()->OuterRect.Min.x);
					UI::Draw::Underline(true, 0.0f, 5.0f);
				}

				ImGui::EndTable();
			}
		}
	}

	void LSceneManagerPanel::SetScene(const TObjectPtr<LScene>& InScene)
	{
		LK_CORE_VERIFY(InScene, "Invalid scene reference");
		Scene = InScene;
	}

	void LSceneManagerPanel::OnSceneRendererDrawListFlush(const FSceneRendererFlushData& Data)
	{
		FlushData = Data;
	}

	void LSceneManagerPanel::OnSceneSelectionUpdated(const std::vector<FSceneSelectionData>& InSelectionData) 
	{
		SceneSelectionData = InSelectionData;
	}

	void LSceneManagerPanel::OnProjectChanged(const TObjectPtr<LProject>& InProject)
	{
		LK_CORE_WARN_TAG("SceneManager", "OnProjectChanged");
	}

	void LSceneManagerPanel::DrawEntityCreateMenu(LEntity Parent)
	{
		if (!ImGui::BeginMenu("Create"))
		{
			return;
		}

		LEntity NewEntity;

		if (ImGui::MenuItem("Empty"))
		{
			NewEntity = Scene->CreateEntity("Empty Entity");
		}

		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::MenuItem("From View"))
			{
				NewEntity = Scene->CreateEntity("Camera");
				NewEntity.AddComponent<LCameraComponent>();

				for (auto& PluginFunc : EntityMenuPlugins)
				{
					PluginFunc(NewEntity);
				}
			}

			if (ImGui::MenuItem("At World Origin"))
			{
				NewEntity = Scene->CreateEntity("Camera");
				NewEntity.AddComponent<LCameraComponent>();
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Text"))
		{
		#if 0
			NewEntity = Scene->CreateEntity("Text");
			auto& TextComp = NewEntity.AddComponent<LTextComponent>();
			/* UI::Font or LFont? */
			TextComp.FontHandle = UI::Font::GetDefault()->Handle;
		#endif
		}

		if (ImGui::BeginMenu("3D"))
		{
			if (ImGui::MenuItem("Cube"))
			{
			}

			if (ImGui::MenuItem("Sphere"))
			{
			}

			if (ImGui::MenuItem("Plane"))
			{
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

	void LSceneManagerPanel::UI_SceneRaycastData()
	{
		ImGui::BeginGroup();
		{
			UI::Font::Push("Large");
			ImGui::Text("Selection Data");
			UI::Font::Pop();

			ImGui::Separator();
			ImGui::Indent();
			ImGui::Text("Selections: %d", (int)SceneSelectionData.size());
			for (const FSceneSelectionData& SceneSelectionData : SceneSelectionData)
			{
				ImGui::BulletText("%s  Dist: %.2f", SceneSelectionData.Entity.Name().c_str(), SceneSelectionData.Distance);
			}
			ImGui::Unindent();
			ImGui::Separator();
		}
		ImGui::EndGroup();
	}

}

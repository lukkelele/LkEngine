#include "LKpch.h"
#include "SceneManagerPanel.h"

#include "LkEngine/UI/Property.h"
#include "LkEngine/Core/SelectionContext.h"

#include "LkEngine/Renderer/FrameBuffer.h"
#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/Color.h"

#include "LkEngine/Editor/EditorCore.h"
#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/Project/Project.h"

#include "LkEngine/UI/UILayer.h"


namespace LkEngine {

	/// TODO: Move this
	/* Helper function. */
	void DumpAttachedComponents(const LEntity Entity);

	LSceneManagerPanel::LSceneManagerPanel(TObjectPtr<LScene> InScene)
		: Scene(InScene)
	{
		LOBJECT_REGISTER();
	}

	void LSceneManagerPanel::Initialize()
	{
		GOnSceneSetActive.Add(this, &LSceneManagerPanel::SetScene);
	}

	void LSceneManagerPanel::OnRenderUI(bool& IsOpen)
	{
		if (bIsWindow)
		{
			if (ImGuiWindow* ThisWindow = ImGui::FindWindowByName(LK_UI_SCENEMANAGER))
			{
				//ImGui::BringWindowToDisplayFront(ThisWindow);
				if (ThisWindow->DockNode)
				{
					ThisWindow->DockNode->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
				}
			}

			ImGui::Begin(LK_UI_SCENEMANAGER, &IsOpen, ImGuiWindowFlags_NoCollapse);
		}

		if (Scene)
		{
			ImGui::SeparatorText(std::format("{}", Scene->Name).c_str());
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
			LK_UI_DEBUG_ON_HOVER();
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
		float LineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 1.0f;

		/* TODO: Reduce the height. */
		ImGui::Separator();
		const bool bOpen = ImGui::TreeNodeEx((void*)typeid(T).hash_code(),TreeNodeFlags, ComponentName.c_str());
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
			/* TODO: Implement a static 'GetName' to the component structs. */
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

		const bool IsSelected = LSelectionContext::IsSelected(Entity);

		ImGuiTreeNodeFlags TreeNodeFlags = (IsSelected ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None);
		TreeNodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		const bool Opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)Entity, TreeNodeFlags, Entity.Name().c_str());

		/* Get the selected entities. */
		const std::vector<LUUID>& Entities = LSelectionContext::GetSelected();

		if (Opened)
		{
			ImGui::Text("UUID: %ulld", Entity.GetUUID());

			if (Entity.HasComponent<LMeshComponent>())
			{
				TObjectPtr<LRuntimeAssetManager> AssetManager = LProject::Current()->GetRuntimeAssetManager();
				DrawComponent<LMeshComponent>("Mesh", Entity, Entities, [&Entity, &AssetManager](LMeshComponent& Mesh)
				{
					auto& Metadata = AssetManager->GetMetadata(Mesh.Mesh);
					ImGui::Text("AssetID: %lld", Mesh.Mesh);
					ImGui::Text("Loaded: %s", Metadata.bIsDataLoaded ? "Yes" : "No");
					ImGui::Text("Memory Asset: %s", Metadata.bIsMemoryAsset ? "Yes" : "No");
					ImGui::Text("Filepath: %s", Metadata.FilePath.string().c_str());
				});
			}

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
					//Changed = UI::Draw::Vec3Control("Transform", Translation, TranslationModified, 0.10f, 100.0f, TranslationAxes);
					Changed = UI::Draw::Vec3Control(
						"Transform", 
						Translation, 
						TranslationModified, 
						0.10f, /* Reset Value. */
						EditorSettings.TranslationSnapValue, 
						UI::SLIDER_MIN_UNLIMITED_POS,
						UI::SLIDER_MAX_UNLIMITED,
						100.0f,         /* Column Width. */
						TranslationAxes /* Axes. */
					);

					/* Rotation. */
					ImGui::TableNextRow();
					Changed |= UI::Draw::Vec3Control(
						"Rotation", 
						Rotation, 
						RotationModified, 
						0.0f, /* Reset Value. */
						EditorSettings.RotationSnapValue, 
						UI::SLIDER_MIN_UNLIMITED_POS,
						UI::SLIDER_MAX_UNLIMITED,
						100.0f,       /* Column Width. */
						RotationAxes  /* Axes. */
					);

					/* Scale. */
					ImGui::TableNextRow();
					Changed |= UI::Draw::Vec3Control(
						"Scale", 
						Scale, 
						ScaleModified, 
						0.010f, 
						EditorSettings.ScaleSnapValue, 
						UI::SLIDER_MIN_UNLIMITED_POS,
						UI::SLIDER_MAX_UNLIMITED,
						100.0f,    /* Column Width. */
						ScaleAxes  /* Axes. */
					);

					if (Changed)
					{
						if (TranslationModified || RotationModified || ScaleModified)
						{
							TranslationAxes = GetInconsistentVectorAxis(Translation, OldTranslation);
							RotationAxes = GetInconsistentVectorAxis(Rotation, OldRotation);
							ScaleAxes = GetInconsistentVectorAxis(Scale, OldScale);

						#if 1
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
						#else
							/* FIXME: To be used when selection works fully. 
							 *        When raycasting works for selecting individual components in the scene.
							 */
							for (auto& EntityID : Entities)
							{
								LEntity Entity = Scene->GetEntityWithUUID(EntityID);
								auto& Component = Entity.GetComponent<LTransformComponent>();

								/* Translation. */
								if ((TranslationAxes & EVectorAxis::X) != 0)
								{
									Component.Translation.x = Translation.x;
								}
								if ((TranslationAxes & EVectorAxis::Y) != 0)
								{
									Component.Translation.y = Translation.y;
								}
								if ((TranslationAxes & EVectorAxis::Z) != 0)
								{
									Component.Translation.z = Translation.z;
								}

								/* Rotation. */
								glm::vec3 ComponentRotation = Component.GetRotationEuler();
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
								Component.SetRotationEuler(ComponentRotation);

								/* Scale. */
								if ((ScaleAxes & EVectorAxis::X) != 0)
								{
									Component.Scale.x = Scale.x;
								}
								if ((ScaleAxes & EVectorAxis::Y) != 0)
								{
									Component.Scale.y = Scale.y;
								}
								if ((ScaleAxes & EVectorAxis::Z) != 0)
								{
									Component.Scale.z = Scale.z;
								}
							}
						#endif
						}
						else
						{
							const glm::vec3 TranslationDiff = Translation - OldTranslation;
							const glm::vec3 RotationDiff = Rotation - OldRotation;
							const glm::vec3 ScaleDiff = Scale - OldScale;

						#if 1
							TransformComp.Translation += TranslationDiff;
							glm::vec3 ComponentRotation = TransformComp.GetRotationEuler();
							ComponentRotation += glm::radians(RotationDiff);
							TransformComp.SetRotationEuler(ComponentRotation);
							TransformComp.Scale += ScaleDiff;
						#else
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

	/*-------------------------------------------------------------------------------*/

	void DumpAttachedComponents(const LEntity Entity)
	{
		if (Entity.HasComponent<LIDComponent>() && (static_cast<uint32_t>(Entity.GetUUID()) != 0))
		{
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "LIDComponent: {}", Entity.HasComponent<LIDComponent>());
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "LTagComponent: {}", Entity.HasComponent<LTagComponent>());
		}
		if (Entity.HasComponent<LTransformComponent>())
		{
			LK_CORE_DEBUG_TAG("SceneManagerPanel", "LTransformComponent: {}", Entity.GetComponent<LTransformComponent>().ToString());
		}
	}

}

#include "LKpch.h"
#include "LkEngine/Editor/EditorLayer.h"
//#include "LkEngine/Physics/World.h"
//#include "LkEngine/Physics/Constraints.h"
#include "LkEngine/Math/Math.h"
#include "LkEngine/Scene/Components.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>


namespace LkEngine {

	Entity EditorLayer::SelectedEntity;

	EditorLayer::EditorLayer(s_ptr<Scene> scene)
		: m_Scene(scene)
	{
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui::Begin("Editor menu");

		if (m_Scene)
		{
			auto& registry = m_Scene->GetRegistry();
			registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Scene.get() };
				DrawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				SelectedEntity = {};
		}

		ImGui::Begin("Properties");
		if (SelectedEntity)
		{
			DrawComponents(SelectedEntity);
		}
		ImGui::End(); // Properties

		ImGuiTreeNodeFlags gizmoFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		ImVec2 buttonSize(76.0f, 30.0f);

		ImGui::End(); // Editor Menu

		if (SelectedEntity && m_GizmoType != -1)
		{
			auto window = ImGui::GetCurrentWindow();
			ImGui::SetNextWindowViewport(window->ID);
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			auto& camera = m_Scene->GetActiveCamera();
			glm::mat4 cameraView = camera->GetView();
			glm::mat4 cameraProj = camera->GetProjection();
			TransformComponent& tc = SelectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProj), 
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform));

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, scale;
				glm::quat rotation;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				tc.Translation = translation;
				tc.Rotation = rotation;
				tc.Scale = scale;
			}
		}
	}

	void EditorLayer::SelectEntity(Entity& entity)
	{
		if (SelectedEntity != entity)
			SelectedEntity = entity;
	}

	void EditorLayer::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			SelectedEntity = entity;
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
				if (SelectedEntity == entity)
					SelectedEntity = {};
		}

	}

	template<typename T, typename UIFunction>
	void EditorLayer::DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen 
			| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
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
				entity.RemoveComponent<T>();
		}
	}

	template<typename T>
	void EditorLayer::DisplayAddComponentEntry(const std::string& entryName)
	{
		if (!SelectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				SelectedEntity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	void EditorLayer::DrawComponents(Entity entity)
	{
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
			//DisplayAddComponentEntry<Mesh>("Mesh");
			// DisplayAddComponentEntry<Rigidbody>("Rigidbody");
			//DisplayAddComponentEntry<Material>("Material");
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [&entity](auto& component)
		{
			/* Translation / Position */
			glm::vec3 translation = component.Translation;
			//UI::Property::Vector3Control("Position", translation);
			component.Translation = translation;

			/* Scale */
			glm::vec3 scale = component.Scale;
			//UI::Property::Vector3Control("Scale", scale);
			component.Scale = scale;
		});


	}

}
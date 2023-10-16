#include "LKpch.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Editor/EditorCamera.h"
#include "LkEngine/UI/UI.h"
#include <imgui/imgui.h>


namespace LkEngine {

	Scene* Scene::ActiveScene;

	Scene::Scene()
	{
		ActiveScene = this;
		m_Camera = create_s_ptr<SceneCamera>();
		m_EditorCamera = create_s_ptr<EditorCamera>();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name);
		return entity;
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		TagComponent& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		LOG_DEBUG("Created new entity: {0} (UUID: {1}, tag: {2})", name, uuid, tag.Tag);
		m_EntityMap[uuid] = entity;
		return entity;
	}

	Entity Scene::GetEntityWithUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };
		return { };
	}

	Entity Scene::FindEntity(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
			{
				return Entity{ entity , this };
			}
		}
		return {};
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
		LOG_CRITICAL("Entity successfully deleted");
	}

	void Scene::OnUpdate(float ts)
	{
		m_Camera->OnUpdate(ts);
		m_EditorCamera->OnUpdate(ts);

		auto entities = m_Registry.view<TransformComponent>();
		for (auto& ent : entities)
		{	
			Entity entity = { ent, this };

			auto& transform = entity.GetComponent<TransformComponent>();
			if (entity.HasComponent<MeshComponent>())
			{
				auto& mesh = entity.GetComponent<MeshComponent>();
				// Submit render
				mesh.Shader->Bind();
				mesh.Shader->SetUniformMat4f("u_TransformMatrix", transform.GetTransform());
				Renderer::Draw(mesh);
				//Renderer::Draw(*mesh.VAO, *mesh.IBO, *mesh.Shader);
			}

		}
	}

	void Scene::Pause(bool paused)
	{
		m_Paused = paused;
		//m_World->Pause(paused);
	}

	void Scene::OnImGuiRender()
	{
		auto entities = m_Registry.view<TransformComponent>();
		ImGui::Begin(SIDEBAR_RIGHT);
		ImGui::SeparatorText("Rendered Entities");
		for (auto& ent : entities)
		{	
			Entity entity = { ent, this };
			//ImGui::Text("Entity: %s", entity.GetName().c_str());
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			uint32_t id = entity;
			if (ImGui::TreeNode("Entity: %s", entity.GetName().c_str()))
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				if (entity.HasComponent<MeshComponent>())
				{
					auto& mesh = entity.GetComponent<MeshComponent>();
					// Submit render
					std::string pos_id = "Pos##" + std::to_string(id);
					std::string scale_id = "Scale##" + std::to_string(id);
					std::string rot_id = "Rot##" + std::to_string(id);
					std::string color_id = "Color##" + std::to_string(id);
					ImGui::SliderFloat2(pos_id.c_str(), &transform.Translation.x, -2.0f, 2.0f, "%.3f");
					ImGui::SliderFloat2(scale_id.c_str(), &transform.Scale.x, -2.0f, 2.0f, "%.3f");
					ImGui::SliderFloat2(rot_id.c_str(), &transform.Rotation.x, -6.0f, 6.0f, "%.3f"); // TODO: make to rad
					ImGui::SliderFloat4(color_id.c_str(), &mesh.Color.x, 0.0f, 1.0f, "%.3f");
					mesh.Shader->Bind();
					mesh.Shader->SetUniformMat4f("u_TransformMatrix", transform.GetTransform());
					mesh.Shader->SetUniform4f("u_Color", mesh.Color.x, mesh.Color.y, mesh.Color.z, mesh.Color.w);
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void Scene::SwitchCamera()
	{
		//Camera* editorCamera = dynamic_cast<Camera*>(m_EditorCamera.get());
		//if (m_Camera != m_EditorCamera)
		//{
		//	LOG_DEBUG("Switching cameras");
		//}
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
	}

		template<>
		void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& rigidbody)
		{
			LOG_INFO("{0} : TransformComponent added!", entity.GetName());
		}

		template<>
		void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& mesh)
		{
			LOG_INFO("{0} : MeshComponent added!", entity.GetName());
		}


}
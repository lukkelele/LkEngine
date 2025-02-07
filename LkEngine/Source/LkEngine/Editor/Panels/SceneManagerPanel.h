#pragma once

#include "LkEngine/Renderer/UI/Panel.h"

#include "LkEngine/Core/SelectionContext.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneSelectionData.h"

#include "LkEngine/Renderer/SceneRenderer.h"


namespace LkEngine {

	class LSceneManagerPanel : public IPanel
	{
	public:
		LSceneManagerPanel(TObjectPtr<LScene> InScene = nullptr);
		~LSceneManagerPanel() = default;

		virtual void Initialize() override;

		virtual void Render() override {}
		virtual void RenderUI(bool& IsOpen) override;

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

		void DrawEntityNode(LEntity Entity);
		void DrawComponents(const std::vector<LUUID>& Entities);

		/**
		 * DrawComponent
		 */
		template<typename T, typename TUIFunction>
		static void DrawComponent(const std::string& ComponentName, 
								  LEntity Entity, 
								  const std::vector<LUUID>& Entities,
								  TUIFunction UIFunction);

		template<typename T>
		void DisplayAddComponentEntry(const std::string& InComponentEntryName);

		/**
		 * Set the scene context.
		 */
		void SetScene(const TObjectPtr<LScene>& scene);

		void OnSceneRendererDrawListFlush(const FSceneRendererFlushData& InData);
		void OnSceneSelectionUpdated(const std::vector<FSceneSelectionData>& InSelectionData);

		virtual void OnProjectChanged(const TObjectPtr<LProject>& InProject);

	private:
		void DrawEntityCreateMenu(LEntity Parent);

		void UI_SceneRaycastData();

		template<typename TVectorType, typename TComponent, typename GetOtherFunc>
		uint32_t GetInconsistentVectorAxis(GetOtherFunc OtherFunc)
		{
			const std::vector<LUUID>& Entities = LSelectionContext::GetSelected(ESelectionContext::Scene);
			if (Entities.size() < 2)
			{
				return 0;
			}

			uint32_t Axes = 0;
			LEntity FirstEntity = Scene->GetEntityWithUUID(Entities[0]);
			const TVectorType& First = OtherFunc(FirstEntity.GetComponent<TComponent>());

			static_assert(std::is_same_v<LK_Enum_Type_EVectorAxis, decltype(Axes)>, "Invalid axes integer type");
			for (std::size_t i = 1; i < Entities.size(); i++)
			{
				LEntity Entity = Scene->GetEntityWithUUID(Entities[i]);
				const TVectorType& OtherVector = OtherFunc(Entity.GetComponent<TComponent>());

				if (glm::epsilonNotEqual(OtherVector.x, First.x, FLT_EPSILON))
				{
					Axes |= (uint32_t)EVectorAxis::X;
				}

				if (glm::epsilonNotEqual(OtherVector.y, First.y, FLT_EPSILON))
				{
					Axes |= (uint32_t)EVectorAxis::Y;
				}

				if constexpr (std::is_same_v<TVectorType, glm::vec3> || std::is_same_v<TVectorType, glm::vec4>)
				{
					if (glm::epsilonNotEqual(OtherVector.z, First.z, FLT_EPSILON))
					{
						Axes |= (uint32_t)EVectorAxis::Z;
					}
				}

				if constexpr (std::is_same_v<TVectorType, glm::vec4>)
				{
					if (glm::epsilonNotEqual(OtherVector.w, First.w, FLT_EPSILON))
					{
						Axes |= (uint32_t)EVectorAxis::W;
					}
				}
			}

			return Axes;
		}

		template<typename TVectorType>
		uint32_t GetInconsistentVectorAxis(const TVectorType& First, const TVectorType& Other)
		{
			uint32_t Axes = 0;
			static_assert(std::is_same_v<LK_Enum_Type_EVectorAxis, decltype(Axes)>, "Invalid axes integer type");
			if (glm::epsilonNotEqual(Other.x, First.x, FLT_EPSILON))
			{
				Axes |= (uint32_t)EVectorAxis::X;
			}

			if (glm::epsilonNotEqual(Other.y, First.y, FLT_EPSILON))
			{
				Axes |= (uint32_t)EVectorAxis::Y;
			}

			if constexpr (std::is_same_v<TVectorType, glm::vec3> || std::is_same_v<TVectorType, glm::vec4>)
			{
				if (glm::epsilonNotEqual(Other.z, First.z, FLT_EPSILON))
				{
					Axes |= (uint32_t)EVectorAxis::Z;
				}
			}

			if constexpr (std::is_same_v<TVectorType, glm::vec4>)
			{
				if (glm::epsilonNotEqual(Other.w, First.w, FLT_EPSILON))
				{
					Axes |= (uint32_t)EVectorAxis::W;
				}
			}

			return Axes;
		}

	private:
		bool bIsWindow = true;
		TObjectPtr<LScene> Scene{};

		TObjectPtr<LScene> ComponentCopyScene{};
		LEntity ComponentCopyEntity;

		std::vector<std::function<void(LEntity)>> EntityMenuPlugins;

		LCLASS(LSceneManagerPanel);
	};

}

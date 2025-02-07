#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Renderer/UI/Panel.h"


namespace LkEngine {

	class LProject;

	enum class EPanelInitState : uint8_t
	{
		Closed = 0,
		Open,
	};

	struct FPanelData
	{
		const char* ID = "";
		const char* Name = "";
		TObjectPtr<IPanel> Panel{};
		bool bIsOpen = false;
	};

	enum class EPanelCategory
	{
		Edit, 
		View, 
		COUNT
	};

	class LPanelManager : public LObject
	{
	public:
		LPanelManager();
		~LPanelManager();

		void Initialize();

		void RenderUI();

		void RemovePanel(const char* PanelID);

		FORCEINLINE FPanelData* GetPanelData(const uint32_t PanelID)
		{
			for (auto& PanelMap : Panels)
			{
				if (PanelMap.find(PanelID) != PanelMap.end())
				{
					return &PanelMap.at(PanelID);
				}
			}

			return nullptr;
		}

		FORCEINLINE const FPanelData* GetPanelData(const uint32_t PanelID) const
		{
			return GetPanelData(PanelID);
		}

		FORCEINLINE FPanelData* GetPanelData(const char* PanelName)
		{
			const uint32_t PanelID = LHash::GenerateFNVHash(PanelName);
			for (auto& PanelMap : Panels)
			{
				if (PanelMap.find(PanelID) != PanelMap.end())
				{
					return &PanelMap.at(PanelID);
				}
			}

			return nullptr;
		}

		std::unordered_map<uint32_t, FPanelData>& GetPanels(const EPanelCategory Category) 
		{ 
			return Panels[static_cast<std::size_t>(Category)]; 
		}

		const std::unordered_map<uint32_t, FPanelData>& GetPanels(const EPanelCategory Category) const 
		{ 
			return Panels[static_cast<std::size_t>(Category)]; 
		}

		template<typename TPanel>
		TObjectPtr<TPanel> AddPanel(const EPanelCategory Category, const FPanelData& PanelData)
		{
			static_assert(std::is_base_of_v<IPanel, TPanel>, "Panel needs to implement the IPanel interface");

			const uint32_t ID = LHash::GenerateFNVHash(PanelData.ID);
			auto& PanelMap = Panels[(std::size_t)Category];
			if (PanelMap.find(ID) != PanelMap.end())
			{
				LK_CORE_ERROR_TAG("PanelManager", "A panel with ID '{}' already exists", PanelData.ID);
				return nullptr;
			}

			PanelMap[ID] = PanelData;
			LK_CORE_TRACE_TAG("PanelManager", "Add panel {} ({})", PanelData.Name, PanelData.ID);

			return PanelData.Panel.As<TPanel>();
		}

		template<typename TPanel, typename... TArgs>
		TObjectPtr<TPanel> AddPanel(const EPanelCategory Category, const char* StringID, EPanelInitState InitState, TArgs&&... Args)
		{
			const bool bIsOpenByDefault = (InitState == EPanelInitState::Open);
			return AddPanel<TPanel>(Category, FPanelData{ StringID, StringID, TObjectPtr<TPanel>::Create(std::forward<TArgs>(Args)...), bIsOpenByDefault });
		}

		template<typename TPanel, typename... TArgs>
		TObjectPtr<TPanel> AddPanel(const EPanelCategory Category, const char* StringID, const char* DisplayName, EPanelInitState InitState, TArgs&&... Args)
		{
			const bool bIsOpenByDefault = (InitState == EPanelInitState::Open);
			return AddPanel<TPanel>(Category, FPanelData{ StringID, DisplayName, TObjectPtr<TPanel>::Create(std::forward<TArgs>(Args)...), bIsOpenByDefault });
		}

		template<typename TPanel>
		TObjectPtr<TPanel> GetPanel(const char* StringID)
		{
			static_assert(std::is_base_of_v<IPanel, TPanel>, "Panel needs to implement the IPanel interface");
			const uint32_t ID = LHash::GenerateFNVHash(StringID);
			for (const auto& PanelMap : Panels)
			{
				if (PanelMap.find(ID) != PanelMap.end())
				{
					return PanelMap.at(ID).Panel.As<TPanel>();
				}
			}

			LK_CORE_ERROR_TAG("PanelManager", "Cannot find panel with id '{}'", StringID);
			return nullptr;
		}

		void Serialize();
		void Deserialize();

		void OnProjectChanged(const TObjectPtr<LProject> InProject);

	private:
		std::array<std::unordered_map<uint32_t, FPanelData>, (std::size_t)EPanelCategory::COUNT> Panels;

		LCLASS(LPanelManager);
	};

}

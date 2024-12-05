#pragma once

#include "EditorTab.h"


namespace LkEngine {

    using TabEntry = std::pair<uint8_t, std::shared_ptr<LTab>>;

    class LEditorTabManager
    {
    public:
        LEditorTabManager() = default;
        ~LEditorTabManager() = default;

        static LEditorTabManager& Get();

        static void Init(const bool InSwitchToNewTabsOnCreation = false);
        static void Destroy();

        static void End();

        static std::shared_ptr<LTab> NewTab(std::string_view TabName, 
                                       const ETabType TabType, 
                                       const bool SetAsActive = false);

        FORCEINLINE static void SetActiveTab(std::shared_ptr<LTab> Tab)
        {
			ActiveTab = Tab;
			if (Tab && (Tab->GetTabType() == ETabType::NodeEditor))
			{
				std::shared_ptr<LNodeEditorTab> NodeTab = std::static_pointer_cast<LNodeEditorTab>(Tab);
				NodeTab->NodeEditor->ActivateContext();
			}
        }

        FORCEINLINE static void SetActiveTab(std::string_view TabName)
        {
            if (std::shared_ptr<LTab> Tab = GetTab(TabName))
            {
				ActiveTab = Tab;
				if (Tab && (Tab->GetTabType() == ETabType::NodeEditor))
				{
					std::shared_ptr<LNodeEditorTab> NodeTab = std::static_pointer_cast<LNodeEditorTab>(Tab);
					NodeTab->NodeEditor->ActivateContext();
				}
            }
        }

        FORCEINLINE static int GetTabCount()
        { 
            return static_cast<int>(Tabs.size());
        }

        FORCEINLINE static std::shared_ptr<LTab> GetActiveTab() 
        { 
            return ActiveTab; 
        }

        FORCEINLINE static void SetSwitchToNewTabsOnCreation(const bool Enabled) 
        { 
            bSwitchToNewTabsOnCreation = Enabled;
        }

        FORCEINLINE static std::string GetActiveTabName()
        {
            return (ActiveTab ? ActiveTab->Name : "");
        }

        template<typename T>
        FORCEINLINE static std::shared_ptr<LTab> GetTab(T Arg)
        {
            static_assert(std::disjunction_v<
                            std::is_same<T, std::uint8_t>,
                            std::is_same<T, std::string_view>,
                            std::is_same<T, std::string>
                          >, "Invalid type");
            return nullptr;
        }

        template<>
        FORCEINLINE static std::shared_ptr<LTab> GetTab(const uint8_t TabIndex);

        template<>
        FORCEINLINE static std::shared_ptr<LTab> GetTab(std::string_view TabName);

        static void PopTab();

        FORCEINLINE static void CloseTab(std::shared_ptr<LTab> Tab)
        {
			Tab->Closed = true;
			TabsToClose.push_back(Tab);
        }

    private:
        static std::shared_ptr<LTab> ActiveTab;
        static std::unordered_map<uint8_t, std::shared_ptr<LTab>> Tabs;

        inline static std::vector<std::shared_ptr<LTab>> TabsToClose{};
        inline static bool bSwitchToNewTabsOnCreation = false;

        friend class LEditorLayer;
    };


	template<>
	inline static std::shared_ptr<LTab> LEditorTabManager::GetTab(const uint8_t TabIndex)
    {
        if (Tabs.contains(TabIndex))
        {
            return Tabs.find(TabIndex)->second;
        }

        return nullptr;
    }

	template<>
	inline static std::shared_ptr<LTab> LEditorTabManager::GetTab(std::string_view TabName)
	{
		auto FindTabInCollection = [TabName](const TabEntry& Entry)
		{
			return (Entry.second && (Entry.second->Name == TabName));
		};
		auto Iter = std::find_if(Tabs.begin(), Tabs.end(), FindTabInCollection);

		if (Iter == Tabs.end())
		{
			return nullptr;
		}

		return (*Iter).second;
	}

}

#pragma once

#include "EditorTab.h"


namespace LkEngine {

    using TabEntry = std::pair<uint8_t, TSharedPtr<LTab>>;

    class LEditorTabManager
    {
    public:
        LEditorTabManager() = default;
        ~LEditorTabManager() = default;

        static LEditorTabManager& Get();

        static void Init(const bool InSwitchToNewTabsOnCreation = false);
        static void Destroy();

        static void End();

        static TSharedPtr<LTab> NewTab(std::string tabName, const EditorTabType tabType, bool setAsActive = false);

        FORCEINLINE static void SetActiveTab(TSharedPtr<LTab> Tab)
        {
			ActiveTab = Tab;
			if (Tab && (Tab->GetTabType() == EditorTabType::NodeEditor))
			{
				TSharedPtr<NodeEditorTab> NodeTab = std::static_pointer_cast<NodeEditorTab>(Tab);
				NodeTab->NodeEditor->ActivateContext();
			}
        }

        FORCEINLINE static void SetActiveTab(std::string_view TabName)
        {
            if (TSharedPtr<LTab> Tab = GetTab(TabName))
            {
				ActiveTab = Tab;
				if (Tab && (Tab->GetTabType() == EditorTabType::NodeEditor))
				{
					TSharedPtr<NodeEditorTab> NodeTab = std::static_pointer_cast<NodeEditorTab>(Tab);
					NodeTab->NodeEditor->ActivateContext();
				}
            }
        }

        FORCEINLINE static int GetTabCount()
        { 
            return static_cast<int>(Tabs.size());
        }

        FORCEINLINE static TSharedPtr<LTab> GetActiveTab() 
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
        FORCEINLINE static TSharedPtr<LTab> GetTab(T TArg)
        {
            static_assert(std::disjunction_v<
                            std::is_same<T, std::uint8_t>,
                            std::is_same<T, std::string_view>,
                            std::is_same<T, std::string>
                          >, "Invalid type");
            return nullptr;
        }

        template<>
        FORCEINLINE static TSharedPtr<LTab> GetTab(const uint8_t TabIndex);

        template<>
        FORCEINLINE static TSharedPtr<LTab> GetTab(std::string_view TabName);

        static void PopTab();

        FORCEINLINE static void CloseTab(TSharedPtr<LTab> tab)
        {
			tab->Closed = true;
			TabsToClose.push_back(tab);
        }

    private:
        static TSharedPtr<LTab> ActiveTab;
        static std::unordered_map<uint8_t, TSharedPtr<LTab>> Tabs;

        inline static std::vector<TSharedPtr<LTab>> TabsToClose{};
        inline static bool bSwitchToNewTabsOnCreation = false;

        friend class LEditorLayer;
    };


	template<>
	inline static TSharedPtr<LTab> LEditorTabManager::GetTab(const uint8_t TabIndex)
    {
        if (Tabs.contains(TabIndex))
        {
            return Tabs.find(TabIndex)->second;
        }

        return nullptr;
    }

	template<>
	inline static TSharedPtr<LTab> LEditorTabManager::GetTab(std::string_view TabName)
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
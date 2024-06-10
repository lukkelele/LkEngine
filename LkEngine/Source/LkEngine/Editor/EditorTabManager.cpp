#include "LKpch.h"
#include "EditorTabManager.h"

#include "LkEngine/Renderer/Framebuffer.h"


namespace LkEngine {

    TSharedPtr<LTab> LEditorTabManager::ActiveTab = nullptr;
    std::unordered_map<uint8_t, TSharedPtr<LTab>> LEditorTabManager::Tabs{};

    LEditorTabManager& LEditorTabManager::Get()
    {
        static LEditorTabManager EditorTabManager;
        return EditorTabManager;
    }

    void LEditorTabManager::Init(bool InSwitchToNewTabsOnCreation)
    {
        bSwitchToNewTabsOnCreation = InSwitchToNewTabsOnCreation;
    }

    TSharedPtr<LTab> LEditorTabManager::NewTab(std::string tabName, const EditorTabType tabType, bool setAsActive)
    {
        auto searchedTab = GetTab(tabName);
        if (searchedTab != nullptr)
        {
            unsigned int tabCopies = 1;
            std::string originalTabName = tabName;
            while (searchedTab != nullptr)
            {
                LK_CORE_DEBUG("Tab already exists, adding copy marker -> ({})", tabCopies);
                tabName = originalTabName + "(" + std::to_string(tabCopies++) + ")";
                searchedTab = GetTab(tabName);
            }
        }
        
        TSharedPtr<LTab> tab = nullptr;
        if (tabType == EditorTabType::Viewport)
        {
            tab = std::make_shared<ViewportTab>(tabName, tabType);
        }
        else if (tabType == EditorTabType::NodeEditor)
        {
            tab = std::make_shared<NodeEditorTab>(tabName);
        }
        else if (tabType == EditorTabType::MaterialEditor)
        {
            tab = std::make_shared<MaterialEditorTab>(tabName, tabType);
        }

        UUID tabIndex = UUID();
        tab->Index = tabIndex;
        Tabs.emplace(tabIndex, tab);

        LK_CORE_ASSERT(tab, "New tab could not be created");
        if (tab == nullptr)
        {
            LK_CORE_ASSERT(false, "New tab could not be created");
        }
        if (bSwitchToNewTabsOnCreation || setAsActive)
        {
            SetActiveTab(tab);
        }

        LK_CORE_DEBUG("New Tab: {}", tabName);
        return tab;
    }

    void LEditorTabManager::PopTab()
    {
        LK_CORE_DEBUG("EditorTabManager::PopTab() -> Tabs.size() == {}", Tabs.size());
        Tabs.erase(Tabs.size());
        if (uint8_t TabCount = Tabs.size(); TabCount > 0)
        {
            LK_CORE_DEBUG("Erase tab: {}", Tabs.find(TabCount)->second->Name); // at uses index, erase uses key
        }

        if (Tabs.size() == 0)
        {
            ActiveTab = nullptr;
        }
    }

    /// REMOVE
    void LEditorTabManager::End()
    {
        for (auto Iter = TabsToClose.begin(); Iter != TabsToClose.end();)
        {
            if (const auto& Entry = Tabs.find((*Iter)->Index); Entry->second)
            {
				//TSharedPtr<LTab> FoundTab = Tabs.find((*Iter)->Index);
                TSharedPtr<LTab>& Tab = Entry->second;
				LK_CORE_DEBUG("Removing tab \"{}\"", Tab->Name);
				Tabs.erase(Tab->Index);
				Iter = TabsToClose.erase(std::find(TabsToClose.begin(), TabsToClose.end(), Tab));
            }
            else
            {
                Iter++;
            }
        }
    }

}
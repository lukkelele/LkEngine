#include "LKpch.h"
#include "EditorTabManager.h"

#include "LkEngine/Renderer/Framebuffer.h"


namespace LkEngine {

    std::shared_ptr<LTab> LEditorTabManager::ActiveTab = nullptr;
    std::unordered_map<uint8_t, std::shared_ptr<LTab>> LEditorTabManager::Tabs{};

    LEditorTabManager& LEditorTabManager::Get()
    {
        static LEditorTabManager EditorTabManager;
        return EditorTabManager;
    }

    void LEditorTabManager::Init(const bool InSwitchToNewTabsOnCreation)
    {
        bSwitchToNewTabsOnCreation = InSwitchToNewTabsOnCreation;
    }

    void LEditorTabManager::Destroy()
    {
    }

    /// REFACTOR
    std::shared_ptr<LTab> LEditorTabManager::NewTab(std::string_view TabName, const ETabType TabType, const bool bSetAsActive)
    {
        LK_CORE_INFO_TAG("Editor", "New tab: {}", TabName);
        std::shared_ptr<LTab> SearchedTab = GetTab(TabName);
        /// REFACTOR: what the .... is going on here
        if (SearchedTab)
        {
            int TabCopies = 1;
            std::string OriginalTabName = TabName.data();

            while (SearchedTab != nullptr)
            {
                LK_CORE_DEBUG("Tab already exists, adding copy marker -> ({})", TabCopies);
                TabName = OriginalTabName + "(" + std::to_string(TabCopies++) + ")";
                SearchedTab = GetTab(TabName);
            }
        }
        
        /// FIXME
        std::shared_ptr<LTab> Tab = nullptr;
        if (TabType == ETabType::Viewport)
        {
            Tab = std::make_shared<ViewportTab>(TabName);
        }
        else if (TabType == ETabType::NodeEditor)
        {
            Tab = std::make_shared<LNodeEditorTab>(TabName);
        }
        else if (TabType == ETabType::MaterialEditor)
        {
            Tab = std::make_shared<LMaterialEditorTab>(TabName);
        }

        UUID TabIndex = UUID();
        Tab->Index = TabIndex;
        Tabs.emplace(TabIndex, Tab);

        LK_CORE_ASSERT(Tab, "New Tab could not be created");
        if (bSwitchToNewTabsOnCreation || bSetAsActive)
        {
            SetActiveTab(Tab);
        }

        LK_CORE_DEBUG("New Tab: {}", TabName);
        return Tab;
    }

    void LEditorTabManager::PopTab()
    {
        LK_CORE_DEBUG("EditorTabManager::PopTab() -> Tabs.size() == {}", Tabs.size());
        Tabs.erase(Tabs.size());
        if (uint8_t TabCount = Tabs.size(); TabCount > 0)
        {
            LK_CORE_DEBUG("Erase Tab: {}", Tabs.find(TabCount)->second->Name); // at uses index, erase uses key
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
                std::shared_ptr<LTab>& Tab = Entry->second;
				LK_CORE_DEBUG("Removing Tab \"{}\"", Tab->Name);

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

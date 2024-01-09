#include "LKpch.h"
#include "EditorTabManager.h"


namespace LkEngine {

    EditorTabManager::EditorTabManager()
    {
    }

    void EditorTabManager::Init(bool switchToNewTabsOnCreation)
    {
        m_SwitchToNewTabsOnCreation = switchToNewTabsOnCreation;
    }

    s_ptr<Tab> EditorTabManager::NewTab(std::string tabName, const EditorTabType tabType, bool setAsActive)
    {
        auto searchedTab = GetTab(tabName);
        if (searchedTab != nullptr)
        {
            unsigned int tabCopies = 1;
            std::string originalTabName = tabName;
            while (searchedTab != nullptr)
            {
                LOG_DEBUG("Tab already exists, adding copy marker -> ({})", tabCopies);
                tabName = originalTabName + "(" + std::to_string(tabCopies++) + ")";
                searchedTab = GetTab(tabName);
            }
        }
        
        s_ptr<Tab> tab = nullptr;
        if (tabType == EditorTabType::Viewport)
            tab = std::make_shared<ViewportTab>(tabName, tabType);
        else if (tabType == EditorTabType::NodeEditor)
            tab = std::make_shared<NodeEditorTab>(tabName);
        else if (tabType == EditorTabType::MaterialEditor)
            tab = std::make_shared<MaterialEditorTab>(tabName, tabType);

        UUID tabIndex = UUID();
        tab->Index = tabIndex;
        m_Tabs.emplace(tabIndex, tab);

        if (tab == nullptr)
            throw std::runtime_error("New tab could not be created successfully, make sure the EditorTabType implementation exists for the specific type");
        if (m_SwitchToNewTabsOnCreation || setAsActive)
            SetActiveTab(tab);

        LOG_DEBUG("New Tab: {}", tabName);
        return tab;
    }

    void EditorTabManager::SetActiveTab(s_ptr<Tab> tab)
    {
        m_ActiveTab = tab;
        if (m_ActiveTab->GetTabType() == EditorTabType::NodeEditor)
        {
            auto* nodeTab = static_cast<NodeEditorTab*>(m_ActiveTab.get());
            nodeTab->NodeEditorRef->ActivateContext();
        }
    }

    void EditorTabManager::SetActiveTab(std::string_view tabName)
    {
        s_ptr<Tab> tab = GetTab(tabName);
        m_ActiveTab = tab;
        if (tab->GetTabType() == EditorTabType::NodeEditor)
        {
            auto* nodeTab = static_cast<NodeEditorTab*>(m_ActiveTab.get());
            //nodeTab->NodeEditorRef->GetEditorContext()->SetAsCurrentEditorContext();
            nodeTab->NodeEditorRef->ActivateContext();
        }
    }

    std::string EditorTabManager::GetActiveTabName() const
    {
        if (m_ActiveTab != nullptr)
            return m_ActiveTab->Name;
        return "";
    }

    s_ptr<Tab> EditorTabManager::GetTab(std::string_view tabName)
    {
        for (std::pair<uint8_t, s_ptr<Tab>> tabEntry : m_Tabs)
        {
            s_ptr<Tab> tab = tabEntry.second;
            if (tab->Name == tabName)
                return tab;
        }
        return nullptr;
    }

    s_ptr<Tab> EditorTabManager::GetTab(uint8_t idx)
    {
        return m_Tabs.find(idx)->second;
    }

    void EditorTabManager::PopTab()
    {
        LOG_DEBUG("EditorTabManager::PopTab() -> m_Tabs.size() == {}", m_Tabs.size());
        m_Tabs.erase(m_Tabs.size());
        if (uint8_t tabCount = m_Tabs.size() > 0)
            LOG_DEBUG("Erase tab: {}", m_Tabs.find(tabCount)->second->Name); // at uses index, erase uses key

        if (m_Tabs.size() == 0)
        {
            m_ActiveTab = nullptr;
        }
    }

    void EditorTabManager::CloseTab(s_ptr<Tab> tab)
    {
        tab->Closed = true;
        m_TabsToClose.push_back(tab);
    }

    void EditorTabManager::End()
    {
        for (s_ptr<Tab> tab : m_TabsToClose)
        {
            //s_ptr<Tab> foundTab = m_Tabs.find(tab->Index);
            auto foundTab = m_Tabs.find(tab->Index);
            if (foundTab != m_Tabs.end())
            {
                LOG_DEBUG("Removing tab \"{}\"", tab->Name);
                m_Tabs.erase(tab->Index);
                m_TabsToClose.erase(std::find(m_TabsToClose.begin(), m_TabsToClose.end(), tab));
            }
        }
    }

}
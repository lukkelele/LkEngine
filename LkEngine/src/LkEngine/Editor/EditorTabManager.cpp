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

    s_ptr<Tab> EditorTabManager::NewTab(std::string_view tabName, const EditorTabType tabType)
    {
        if (tabType == EditorTabType::NodeEditor)
        {
            auto tab = std::make_shared<NodeEditorTab>(tabName);
            m_Tabs.emplace(m_Tabs.size() + 1, tab);
        }
        else
        {
            auto tab = std::make_shared<Tab>(tabName, tabType);
            m_Tabs.emplace(m_Tabs.size() + 1, tab);
        }

        s_ptr<Tab> tab = GetTab(tabName);
        if (m_SwitchToNewTabsOnCreation)
            SetActiveTab(tab);
        return tab;
    }

    void EditorTabManager::SetActiveTab(s_ptr<Tab> tab)
    {
        m_ActiveTab = tab;
    }

    void EditorTabManager::SetActiveTab(std::string_view tabName)
    {
        s_ptr<Tab> tab = GetTab(tabName);
        m_ActiveTab = tab;
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

}
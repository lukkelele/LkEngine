#pragma once

#include "EditorTab.h"


namespace LkEngine {

    using TabEntry = std::pair<uint8_t, s_ptr<Tab>>;
    using Tabs = std::unordered_map<uint8_t, s_ptr<Tab>>;

    class EditorTabManager
    {
    public:
        EditorTabManager();
        ~EditorTabManager() = default;

        void Init(bool switchToNewTabsOnCreation = false);
        s_ptr<Tab> NewTab(std::string_view tabName, const EditorTabType tabType, bool setAsActive = false);
        void SetActiveTab(s_ptr<Tab> tab);
        void SetActiveTab(std::string_view tabName);
        uint8_t GetTabCount() const { return m_Tabs.size(); }
        s_ptr<Tab> GetActiveTab() { return m_ActiveTab; }
        std::string GetActiveTabName() const;
        s_ptr<Tab> GetTab(std::string_view tabName);
        s_ptr<Tab> GetTab(uint8_t idx);
        void PopTab();
        void SetSwitchToNewTabsOnCreation(bool enabled) { m_SwitchToNewTabsOnCreation = enabled; }

    private:
        s_ptr<Tab> m_ActiveTab = nullptr;
        Tabs m_Tabs;
        bool m_SwitchToNewTabsOnCreation = false;

        friend class Editor;
    };

}
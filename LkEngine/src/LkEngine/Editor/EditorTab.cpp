#include "LKpch.h"
#include "EditorTab.h"
#include "Editor.h"


namespace LkEngine {

    //-----------------------------------------------------------
    // NodeEditorTab
    //-----------------------------------------------------------
    NodeEditorTab::NodeEditorTab(std::string_view name) 
    {
        Name = std::string(name);
        Type = EditorTabType::NodeEditor;
        NodeEditorRef = new NodeEditor(name);
    }

    NodeEditorTab::NodeEditorTab(std::string_view name, const EditorTabType tabType) 
        : NodeEditorTab(name) 
    {
    }

    NodeEditorTab::~NodeEditorTab()
    {
       NodeEditorRef->Destroy();
    }

    void NodeEditorTab::OnRender() 
    {
    }

    void NodeEditorTab::OnImGuiRender()
    {
        NodeEditorRef->OnImGuiRender();
    }



    //-----------------------------------------------------------
    // MaterialEditorTab
    //-----------------------------------------------------------
    MaterialEditorTab::MaterialEditorTab(std::string_view name) 
    {
        Name = std::string(name);
        Type = EditorTabType::MaterialEditor;
        NodeEditorRef = new NodeEditor(name);
    }

    MaterialEditorTab::MaterialEditorTab(std::string_view name, const EditorTabType tabType) 
        : MaterialEditorTab(name) 
    {
    }

    MaterialEditorTab::~MaterialEditorTab()
    {
        NodeEditorRef->Destroy();
    }

    void MaterialEditorTab::OnRender()
    {
    }

    void MaterialEditorTab::OnImGuiRender()
    {
        auto* editor = Editor::Get();
        static float div = 0.20f;

		ImGui::SetNextWindowPos({ editor->m_SecondViewportBounds[0].x, (editor->MenuBarSize.y + editor->TabBarSize.y) }, ImGuiCond_Always);
		ImGui::SetNextWindowSize({ editor->EditorWindowSize.x * (1 - div), editor->EditorWindowSize.y }, ImGuiCond_Always);
        NodeEditorRef->OnImGuiRender();

		ImGui::SetNextWindowPos({ editor->m_SecondViewportBounds[1].x * (1 - div), (editor->MenuBarSize.y + editor->TabBarSize.y) }, ImGuiCond_Always);
		//ImGui::SetNextWindowSize({ editor->EditorWindowSize.x * div, editor->EditorWindowSize.y }, ImGuiCond_Always);
		//ImGui::SetNextWindowSize({ editor->m_SecondViewportBounds[1].x - editor->EditorWindowSize.x * div, editor->EditorWindowSize.y }, ImGuiCond_Always);
		ImGui::SetNextWindowSize({ (editor->m_SecondViewportBounds[1].x - editor->m_SecondViewportBounds[0].x) * div, editor->EditorWindowSize.y}, ImGuiCond_Always);
        ImGui::Begin(MaterialRef->GetName().c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking);
        ImGui::SliderFloat("Roughness", &MaterialRef->GetRoughness(), 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);
        ImGui::End();
    }




}
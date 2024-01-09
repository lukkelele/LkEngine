#include "LKpch.h"
#include "EditorTab.h"
#include "Editor.h"

#include "LkEngine/Renderer/TextureLibrary.h"


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

    // TODO: Need to add a check for size changes if I want to be able to allow resizing without creating issues when pos is set
    //       Basically just replicate what's done in Editor, but I have no energy to do that now lol
    void MaterialEditorTab::OnImGuiRender()
    {
        auto* editor = Editor::Get();
        static float div = 0.30f;

        static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

        ImVec2 nodeWindowSize = ImVec2(editor->EditorWindowSize.x * (1 - div), editor->EditorWindowSize.y);
		ImGui::SetNextWindowPos({ editor->m_SecondViewportBounds[0].x, (editor->MenuBarSize.y + editor->TabBarSize.y) }, ImGuiCond_Always);
		ImGui::SetNextWindowSize(nodeWindowSize, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(0, nodeWindowSize.y), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        UI::Begin(windowFlags);
        NodeEditorRef->OnImGuiRender(nodeWindowSize);
        UI::End();
        ImGui::PopStyleVar(1);


        ImGui::SetNextWindowBgAlpha(0.2f);
		ImGui::SetNextWindowPos({ editor->LeftSidebarSize.x + (editor->EditorWindowSize.x * (1 - div)), (editor->MenuBarSize.y + editor->TabBarSize.y)}, ImGuiCond_Always);
		ImGui::SetNextWindowSize({ editor->EditorWindowSize.x * div, editor->EditorWindowSize.y }, ImGuiCond_Always);
        UI::Begin(MaterialRef->GetName().c_str(), windowFlags);
        {
            // Roughness
            ImGui::Text("Roughness"); 
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::SliderFloat("##Roughness", &MaterialRef->GetRoughness(), 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);

            // Texture
            auto& textures2D = TextureLibrary::Get()->GetTextures2D();
            static std::string selected_texture = textures2D[1].first;
            ImGui::SeparatorText("Texture"); 
            ImGui::Text("Current:");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##textures", selected_texture.c_str()))
            {
                for (auto& entry : textures2D)
                {
                    auto& texture = entry.second;
                    const bool is_selected = selected_texture == entry.second->GetName();

                    if (ImGui::Selectable(texture->GetName().c_str(), (selected_texture == texture->GetName()), ImGuiSelectableFlags_AllowDoubleClick))
                    {

                        selected_texture = texture->GetName();
                    }
                }
                ImGui::EndCombo();
            }
        }
        UI::End();
    }




}
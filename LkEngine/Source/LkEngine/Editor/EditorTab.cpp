#include "LKpch.h"
#include "EditorTab.h"

#include "EditorLayer.h"

#include "LkEngine/Renderer/TextureLibrary.h"


namespace LkEngine {

    LNodeEditorTab::LNodeEditorTab(std::string_view InName) 
    {
        Name = InName;
        Type = ETabType::NodeEditor;

        NodeEditor = TObjectPtr<LNodeEditor>::Create(InName);
    }

    LNodeEditorTab::~LNodeEditorTab()
    {
       NodeEditor->Destroy();
    }

    void LNodeEditorTab::OnImGuiRender()
    {
        NodeEditor->OnImGuiRender();
    }



    LMaterialEditorTab::LMaterialEditorTab(std::string_view InName) 
    {
        Name = InName;
        Type = ETabType::MaterialEditor;

        NodeEditor = TObjectPtr<LNodeEditor>::Create(InName);
    }

    LMaterialEditorTab::~LMaterialEditorTab()
    {
        NodeEditor->Destroy();
    }

    void LMaterialEditorTab::OnRender()
    {
    }

    // TODO: Need to add a check for size changes if I want to be able 
    // to allow resizing without creating issues when pos is set
    // Basically just replicate what's done in Editor, but I have no energy to do that now lol
    void LMaterialEditorTab::OnImGuiRender()
    {
        LEditorLayer* Editor = LEditorLayer::Get();
        static float div = 0.30f;

        static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking 
            | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse 
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    #if 0
        ImVec2 NodeWindowSize = ImVec2(Editor->EditorWindowSize.x * (1 - div), Editor->EditorWindowSize.y);

		ImGui::SetNextWindowPos(
            { Editor->SecondViewportBounds[0].X, (Editor->GetMenuBarSize().y + Editor->GetTabBarSize().y) }, 
            ImGuiCond_Always
        );
		ImGui::SetNextWindowSize(NodeWindowSize, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(0, NodeWindowSize.y), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        {
            NodeEditor->OnImGuiRender(NodeWindowSize);
        }
        ImGui::PopStyleVar(1);
    #endif

#if 0
        ImGui::SetNextWindowBgAlpha(0.2f);
		ImGui::SetNextWindowPos({ Editor->GetLeftSidebarSize().x + (Editor->EditorWindowSize.x * (1 - div)), (Editor->GetMenuBarSize().y + Editor->GetTabBarSize().y)}, ImGuiCond_Always);
		ImGui::SetNextWindowSize({ Editor->EditorWindowSize.x * div, Editor->EditorWindowSize.y }, ImGuiCond_Always);
        UI::Begin(MaterialRef->GetName().c_str(), windowFlags);
        {
            // Roughness
            ImGui::Text("Roughness"); 
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            //ImGui::SliderFloat("##Roughness", &MaterialRef->GetRoughness(), 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);
            float roughness = MaterialRef->GetRoughness();
            ImGui::SliderFloat("##Roughness", &roughness, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);
            MaterialRef->SetRoughness(roughness);

            // Texture
            auto textures2D = TextureLibrary::Get()->GetTextures2D();
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

                    if (ImGui::Selectable(texture->GetName().c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick))
                    {

                        selected_texture = texture->GetName();
                    }
                }
                ImGui::EndCombo();
            }
        }
        UI::End();
#endif
    }




}
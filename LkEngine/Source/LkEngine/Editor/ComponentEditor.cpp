#include "LKpch.h"
#include "ComponentEditor.h"

#include "EditorLayer.h"

#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/MaterialLibrary.h"
#include "LkEngine/Renderer/Renderer.h"

#include "LkEngine/ImGui/ImGuiUtilities.h"


namespace LkEngine {

    void LComponentEditor::Initialize()
    {
        // TODO
    }

    void LComponentEditor::OnRenderUI()
    {
        static ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
        if (ImGui::TreeNodeEx("Materials", treeNodeFlags))
        {
            static std::string selected_material = "";

            auto& Materials = LMaterialLibrary::Get().GetMaterials();
            for (std::pair<std::string, TObjectPtr<LMaterial>> MaterialEntry : Materials)
            {
                const std::string& MaterialName = MaterialEntry.first;
			    const bool bIsSelected = (MaterialName == selected_material);

                if (ImGui::Selectable(MaterialName.c_str(), bIsSelected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns))
                {
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        selected_material = MaterialName;
                        // Create new edit tab for the material
                        TSharedPtr<LTab> NewTab = LEditorTabManager::NewTab(
                            fmt::format("Edit - {}", MaterialName.c_str()), 
                            EditorTabType::MaterialEditor
                        );

                        MaterialEditorTab& MaterialEditorTabRef = static_cast<MaterialEditorTab&>(*NewTab.get());
                        MaterialEditorTabRef.MaterialRef = MaterialEntry.second;
                    }
                }

                if (bIsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Textures", treeNodeFlags))
        {
            for (auto& Texture2DEntry : LTextureLibrary::Get().GetTextures2D())
            {
                auto& Texture2D = Texture2DEntry.second;
                if (ImGui::TreeNodeEx(Texture2DEntry.first.c_str(), treeNodeFlags))
                {
                    RenderTextureEditor(Texture2D);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::Separator();
    }

    void LComponentEditor::RenderMaterialEditor(LMaterial& Material)
    {
        //UI::PushID();
        //float roughness = material.GetRoughness();
        //ImGui::SliderFloat("Path: %s", &roughness, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        //material.SetRoughness(roughness);
        //UI::PopID();
    }

    void LComponentEditor::RenderTextureEditor(const TObjectPtr<LTexture>& Texture)
    {
        UI::PushID();
        ImGui::Text("Path: %s", Texture->GetPath().string().c_str());
        ImGui::Text("Size: (%1.f, %1.f)", Texture->GetWidth(), Texture->GetHeight());
        UI::PopID();
    }

    void LComponentEditor::RenderTextureEditor(const TObjectPtr<LTexture2D>& Texture)
    {
        UI::PushID();
        ImGui::Text("Path: %s", Texture->GetPath().string().c_str());
        ImGui::Text("Size: (%1.f, %1.f)", Texture->GetWidth(), Texture->GetHeight());
        UI::PopID();
    }

}
#include "LKpch.h"
#include "ComponentEditor.h"

#include "EditorLayer.h"
#include "LkEngine/Editor/EditorTab.h"
#include "LkEngine/Editor/EditorTabManager.h"
#include "LkEngine/Editor/NodeEditor/NodeEditor.h"

#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/MaterialLibrary.h"
#include "LkEngine/Renderer/Renderer.h"

#include "LkEngine/UI/ImGui/ImGuiUtilities.h"


namespace LkEngine {

    LComponentEditor::LComponentEditor()
    {
        LCLASS_REGISTER();
    }

    void LComponentEditor::Initialize()
    {
        LObject::Initialize();
        // TODO
    }

    void LComponentEditor::OnRenderUI()
    {
        static std::unordered_map<std::string, TObjectPtr<LMaterial>> MaterialMap{};

        static ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
        if (ImGui::TreeNodeEx("Materials", treeNodeFlags))
        {
            static std::string SelectedMaterial = "";

            const int RetrievedMaterials = LMaterialLibrary::Get().GetMaterials(MaterialMap);
            for (auto& [ MaterialName, Material ] : MaterialMap)
            {
                //const std::string& MaterialName = MaterialEntry.first;
			    const bool bIsSelected = (MaterialName == SelectedMaterial);

                if (ImGui::Selectable(MaterialName.c_str(), bIsSelected, 
                        ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns))
                {
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        SelectedMaterial = MaterialName;

                        /* Create new edit tab for the material. */
                        TSharedPtr<LTab> NewTab = LEditorTabManager::NewTab(
                            LK_FORMAT_STRING("Edit->{}", MaterialName).c_str(),
                            ETabType::MaterialEditor
                        );

                        if (TSharedPtr<LMaterialEditorTab> MaterialEditorTabRef = std::static_pointer_cast<LMaterialEditorTab>(NewTab))
                        {
							MaterialEditorTabRef->MaterialRef = Material;
                        }
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
        #if 0
            for (auto& Texture2DEntry : LTextureLibrary::Get().GetTextures2D())
            {
                auto& Texture2D = Texture2DEntry.second;
                if (ImGui::TreeNodeEx(Texture2DEntry.first.c_str(), treeNodeFlags))
                {
                    RenderTextureEditor(Texture2D);
                    ImGui::TreePop();
                }
            }
        #endif
			ImGui::TreePop(); /* Textures. */
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
#include "LKpch.h"
#include "ComponentEditor.h"

#include "EditorLayer.h"

#include "LkEngine/Renderer/Renderer.h"

#include "LkEngine/Asset/EditorAssetManager.h"


namespace LkEngine {

    LComponentEditor::LComponentEditor()
    {
        LOBJECT_REGISTER();
    }

    void LComponentEditor::Initialize()
    {
        LObject::Initialize();
    }

    void LComponentEditor::RenderUI(bool& IsOpen)
    {
	#if 0
        static std::unordered_map<LUUID, TObjectPtr<LMaterialAsset>> MaterialMap{};
        static ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
        if (ImGui::TreeNodeEx("Materials", TreeNodeFlags))
        {
            static std::string SelectedMaterial = "";

            const int RetrievedMaterials = LEditorAssetManager::Get().GetMaterials(MaterialMap);
            for (auto& [ MatAssetHandle, MaterialAsset ] : MaterialMap)
            {
				//const std::string MaterialName = MaterialAsset->GetMaterial()->GetName();
				const TObjectPtr<LMaterial>& Material = MaterialAsset->GetMaterial();
				const std::string MaterialName = Material->GetName();

                //const std::string& MaterialName = MaterialEntry.first;
			    const bool bIsSelected = (MaterialName == SelectedMaterial);

                if (ImGui::Selectable(MaterialName.c_str(), bIsSelected, 
                        ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns))
                {
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        SelectedMaterial = MaterialName;

                        /* Create new edit tab for the material. */
                        std::shared_ptr<LTab> NewTab = LEditorTabManager::NewTab(
                            std::format("Edit->{}", MaterialName).c_str(),
                            ETabType::MaterialEditor
                        );

                        if (std::shared_ptr<LMaterialEditorTab> MaterialEditorTabRef = std::static_pointer_cast<LMaterialEditorTab>(NewTab))
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
        if (ImGui::TreeNodeEx("Textures", TreeNodeFlags))
        {
        #if 0
            for (auto& Texture2DEntry : LTextureLibrary::Get().GetTextures2D())
            {
                auto& Texture2D = Texture2DEntry.second;
                if (ImGui::TreeNodeEx(Texture2DEntry.first.c_str(), TreeNodeFlags))
                {
                    RenderTextureEditor(Texture2D);
                    ImGui::TreePop();
                }
            }
        #endif
			ImGui::TreePop(); /* Textures. */
        }

        ImGui::Separator();
	#endif
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

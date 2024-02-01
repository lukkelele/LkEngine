#include "LKpch.h"
#include "ComponentEditor.h"

#include "EditorLayer.h"

#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Renderer/MaterialLibrary.h"

#include "LkEngine/ImGui/ImGuiUtilities.h"


namespace LkEngine {

    ComponentEditor::ComponentEditor()
    {
    }

    ComponentEditor::~ComponentEditor()
    {
    }

    void ComponentEditor::Init()
    {
    }

    void ComponentEditor::OnRender()
    {
    }

    void ComponentEditor::OnImGuiRender()
    {
        static ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
        if (ImGui::TreeNodeEx("Materials", treeNodeFlags))
        {
            static std::string selected_material = "";
            auto& materials = MaterialLibrary::Get()->GetMaterials();
            for (std::pair<std::string, Ref<Material>> materialEntry: materials)
            {
                std::string materialName = materialEntry.first;
			    const bool is_selected = (materialName == selected_material);
                if (ImGui::Selectable(materialName.c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns))
                {
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        selected_material = materialName;
                        // Create new edit tab for the material
                        auto* tabManager = EditorLayer::Get()->GetTabManager();
                        auto newTab = tabManager->NewTab(fmt::format("Edit - {}", materialName.c_str()), EditorTabType::MaterialEditor);
                        MaterialEditorTab& materialEditorTab = static_cast<MaterialEditorTab&>(*newTab.get());
                        materialEditorTab.MaterialRef = materialEntry.second;
                    }
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Textures", treeNodeFlags))
        {
            for (auto& texture2DEntry : TextureLibrary::Get()->GetTextures2D())
            {
                auto& texture2D = texture2DEntry.second;
                if (ImGui::TreeNodeEx(texture2DEntry.first.c_str(), treeNodeFlags))
                {
                    RenderTextureEditor(texture2D);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::Separator();
    }

    void ComponentEditor::RenderMaterialEditor(Material& material)
    {
        UI::PushID();
        float roughness = material.GetRoughness();
        ImGui::SliderFloat("Path: %s", &roughness, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        material.SetRoughness(roughness);
        UI::PopID();
    }

    void ComponentEditor::RenderTextureEditor(const Ref<Texture> texture)
    {
        UI::PushID();
        ImGui::Text("Path: %s", texture->GetPath().string().c_str());
        ImGui::Text("Size: (%1.f, %1.f)", texture->GetWidth(), texture->GetHeight());
        UI::PopID();
    }

    void ComponentEditor::RenderTextureEditor(const Ref<Texture2D> texture)
    {
        UI::PushID();
        ImGui::Text("Path: %s", texture->GetPath().string().c_str());
        ImGui::Text("Size: (%1.f, %1.f)", texture->GetWidth(), texture->GetHeight());
        UI::PopID();
    }

}
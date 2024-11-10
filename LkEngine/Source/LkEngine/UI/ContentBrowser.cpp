#include "LKpch.h"
#include "ContentBrowser.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Renderer/Image.h"
#include "LkEngine/Renderer/Texture.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

    static TObjectPtr<LImage> FileIconImage{};

    static int Mode = 0;

    static const char* TestNames[9] =
    {
        "Bobby", "Beatrice", "Betty", "Brianna", 
        "Barry", "Bernard", "Bibi", "Blaine", "Bryn"
    };

    LContentBrowser::LContentBrowser()
    {
        LCLASS_REGISTER();
    }

    void LContentBrowser::Initialize()
    {
        /* Create icons. */
        FTextureSpecification TextureSpec{};

		TextureSpec.Name = "FileIcon";
		TextureSpec.DebugName = "FileIcon";
		TextureSpec.Width = 200;
		TextureSpec.Height = 200;
		TextureSpec.Format = EImageFormat::RGBA32F;
		TextureSpec.Path = "Assets/Icons/file-icon-48x48.png";
		FileIconTexture = LTexture2D::Create(TextureSpec);

		TextureSpec.Name = "FolderIcon";
		TextureSpec.DebugName = "FolderIcon";
		TextureSpec.Path = "Assets/Icons/folder-icon-64x64.png";
        FolderIconTexture = LTexture2D::Create(TextureSpec);
    }

	void LContentBrowser::OnRender()
	{
	}

	void LContentBrowser::OnRenderUI()
	{
        /// FIXME
		if (ImGui::ImageButton("FILE_ICON", (ImTextureID)(LContentBrowser::FileIconTexture->GetRendererID()), ImVec2(48, 48), ImVec2(0, 0)))
		{
		    LK_CORE_DEBUG_TAG("ContentBrowser", "Clicked FileIcon");
		}

        // File Icon
        static std::string file_data = "FILE";
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload("FILE_DATA_TYPE", &file_data, sizeof(char) * 4);
            ImGui::Text("%s", LContentBrowser::FileIconTexture->GetName().data());

            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CB_PAYLOAD", ImGuiDragDropFlags_None);
            if (payload)
            {

                LK_CORE_DEBUG_TAG("ContentBrowser", "File - AcceptDragDropPayload  Size={}", (int)payload->SourceId);
            }

            ImGui::EndDragDropTarget();
        }
        // ~File Icon

        ImGui::SameLine();

        /// FIXME
        // Folder Icon
        static std::string folder_data = "FOLDER";
		if (ImGui::ImageButton("FOLDER_ICON", (ImTextureID)FolderIconTexture->GetRendererID(), ImVec2(48, 48), ImVec2(0, 0)))
		{
		    LK_CORE_DEBUG_TAG("ContentBrowser", "Clicked FolderIcon!");
		}
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            std::string payloadData = "CUBE";
            ImGui::SetDragDropPayload("CB_PAYLOAD", payloadData.c_str(), payloadData.size() + 1);
            //ImGui::Text("%s", LContentBrowser::FolderIconTexture->GetName().c_str());
            ImGui::Text("3D CUBE wowww");

            ImGui::EndDragDropSource();
        }
#if 0
        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CB_PAYLOAD", ImGuiDragDropFlags_None);
            if (payload)
            {
                //LK_CORE_DEBUG_TAG("ContentBrowser", "Folder - AcceptDragDropPayload  Size={}", (int)payload->SourceId);
            }
            ImGui::EndDragDropTarget();
        }
#endif

	}

}
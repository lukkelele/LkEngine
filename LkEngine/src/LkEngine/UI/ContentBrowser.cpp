#include "LKpch.h"
#include "ContentBrowser.h"

#include "LkEngine/Renderer/Image.h"
#include "LkEngine/Renderer/Texture.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Editor/Editor.h"


namespace LkEngine {

	static Ref<Image> FileIconImage;

    enum Mode
    {
        Mode_Copy,
        Mode_Move,
        Mode_Swap
    };
    static int mode = 0;

    static const char* names[9] =
    {
        "Bobby", "Beatrice", "Betty",
        "Brianna", "Barry", "Bernard",
        "Bibi", "Blaine", "Bryn"
    };


	ContentBrowser::ContentBrowser()
	{
		TextureSpecification textureSpec;
		textureSpec.Name = "FileIcon";
		textureSpec.DebugName = "FileIcon";
		textureSpec.Width = 200;
		textureSpec.Height = 200;
		textureSpec.Format = ImageFormat::RGBA32F;
		textureSpec.Path = "assets/icons/file-icon-48x48.png";
		FileIconTexture = Texture2D::Create(textureSpec);

		textureSpec.Name = "FolderIcon";
		textureSpec.DebugName = "FolderIcon";
		textureSpec.Path = "assets/icons/folder-icon-64x64.png";
        FolderIconTexture = Texture2D::Create(textureSpec);
	}

	void ContentBrowser::OnRender()
	{
	}

	void ContentBrowser::OnImGuiRender()
	{

		if (ImGui::ImageButton((void*)ContentBrowser::FileIconTexture->GetRendererID(), ImVec2(48, 48), ImVec2(0, 0)))
		{
		    LK_CORE_DEBUG_TAG("ContentBrowser", "Clicked FileIcon!");
		}

        static std::string file_data = "FILE";
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload("FILE_DATA_TYPE", &file_data, sizeof(char) * 4);
            ImGui::Text("%s", ContentBrowser::FileIconTexture->GetName().c_str());

            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FOLDER_DATA_TYPE", ImGuiDragDropFlags_None);
            if (payload)
            {

                LK_CORE_DEBUG_TAG("ContentBrowser", "File - AcceptDragDropPayload  Size={}", (int)payload->SourceId);
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();

        static std::string folder_data = "FOLDER";
		if (ImGui::ImageButton((void*)FolderIconTexture->GetRendererID(), ImVec2(48, 48), ImVec2(0, 0)))
		{
		    LK_CORE_DEBUG_TAG("ContentBrowser", "Clicked FolderIcon!");
		}
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            //ImGui::SetDragDropPayload(std::to_string(FolderIconTexture->GetRendererID()).c_str(), &FolderIconTexture->GetRendererID(), sizeof(RendererID));
            ImGui::SetDragDropPayload("FOLDER_DATA_TYPE", &folder_data, sizeof(char) * 7);
            ImGui::Text("%s", ContentBrowser::FolderIconTexture->GetName().c_str());

            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DATA_TYPE", ImGuiDragDropFlags_None);
            if (payload)
            {
                LK_CORE_DEBUG_TAG("ContentBrowser", "Folder - AcceptDragDropPayload  Size={}", (int)payload->SourceId);
            }
            ImGui::EndDragDropTarget();
        }


        if (ImGui::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } ImGui::SameLine();
        if (ImGui::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } ImGui::SameLine();
        if (ImGui::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; }

        if (ImGui::TreeNode("Drag and drop to copy/swap items"))
        {
            if (ImGui::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } ImGui::SameLine();
            if (ImGui::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } ImGui::SameLine();
            if (ImGui::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; }

            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                ImGui::PushID(n);
                if ((n % 3) != 0)
                    ImGui::SameLine();
                ImGui::Button(names[n], ImVec2(60, 60));

                // Our buttons are both drag sources and drag targets here!
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    // Set payload to carry the index of our item (could be anything)
                    ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

                    // Display preview (could be anything, e.g. when dragging an image we could decide to display
                    // the filename and a small preview of the image, etc.)
                    if (mode == Mode_Copy) { ImGui::Text("Copy %s", names[n]); }
                    if (mode == Mode_Move) { ImGui::Text("Move %s", names[n]); }
                    if (mode == Mode_Swap) { ImGui::Text("Swap %s", names[n]); }

                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        if (mode == Mode_Copy)
                        {
                            names[n] = names[payload_n];
                        }
                        if (mode == Mode_Move)
                        {
                            names[n] = names[payload_n];
                            names[payload_n] = "";
                        }
                        if (mode == Mode_Swap)
                        {
                            const char* tmp = names[n];
                            names[n] = names[payload_n];
                            names[payload_n] = tmp;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        if (ImGui::BeginDragDropTarget())
        {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
			{
                //memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
				LK_CORE_DEBUG_TAG("ContentBrowser", "Payload 4F");
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
			{
                //memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
				LK_CORE_DEBUG_TAG("ContentBrowser", "Payload 4F");
			}
            ImGui::EndDragDropTarget();
        }


	}

}
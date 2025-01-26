#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/IO/FileSystem.h"

#include "LkEngine/Editor/EditorGlobals.h"

#include "LkEngine/Renderer/Texture.h"

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>


namespace LkEngine {

	/** 
     * EGizmo 
	 * 
	 *  TODO: Rework this.
     */
	enum class EGizmo
	{ 
		Translate = ImGuizmo::TRANSLATE,
		Rotate	  = ImGuizmo::ROTATE,
		Scale	  = ImGuizmo::SCALE
	};

	/* TODO: Move to other file. */
	namespace Debug 
	{
		namespace UI 
		{
			extern bool bDisplayWindowSizeOnHover;
			extern bool bBoundingBoxesOnHover;

			namespace ContentBrowser 
			{
				extern bool bDrawOutlinerBorders;
				extern ImVec4 OutlinerBorderColor;
			}
		}
	}

	/**
	 * FEditorResources
	 * 
	 *  Assets and resources used by the editor in a global setting.
	 */
	struct FEditorResources
	{
		inline static TObjectPtr<LTexture2D> FolderIcon{};
		inline static TObjectPtr<LTexture2D> FileIcon{};
		inline static TObjectPtr<LTexture2D> ForwardIcon{};
		inline static TObjectPtr<LTexture2D> BackwardIcon{};
		inline static TObjectPtr<LTexture2D> RefreshIcon{};
		inline static TObjectPtr<LTexture2D> SearchIcon{};
		inline static TObjectPtr<LTexture2D> GearIcon{};
		inline static TObjectPtr<LTexture2D> PlusIcon{};
		inline static TObjectPtr<LTexture2D> PencilIcon{};
		inline static TObjectPtr<LTexture2D> PointerIcon{};
		inline static TObjectPtr<LTexture2D> SaveIcon{};
		inline static TObjectPtr<LTexture2D> ClearIcon{};

		inline static TObjectPtr<LTexture2D> MeshFileIcon{};
		inline static TObjectPtr<LTexture2D> StaticMeshFileIcon{};

		inline static TObjectPtr<LTexture2D> JPGFileIcon{};
		inline static TObjectPtr<LTexture2D> PNGFileIcon{};

		static void Initialize()
		{
			LK_CORE_DEBUG_TAG("EditorResources", "Loading icons");
			FTextureSpecification Spec;
			Spec.SamplerWrap = ETextureWrap::Clamp;
			Spec.Format = EImageFormat::RGBA32F;
			Spec.Width = 200;
			Spec.Height = 200;

			FolderIcon = LoadTexture("Icon/Folder.png", "FolderIcon", Spec);
			FileIcon = LoadTexture("Icon/File.png", "FileIcon", Spec);
			ForwardIcon = LoadTexture("Icon/Forward.png", "ForwardIcon", Spec);
			BackwardIcon = LoadTexture("Icon/Backward.png", "BackwardIcon", Spec);
			PointerIcon = LoadTexture("Icon/Pointer.png", "PointerIcon", Spec);
			PencilIcon = LoadTexture("Icon/Pencil.png", "PencilIcon", Spec);
			SearchIcon = LoadTexture("Icon/Search.png", "SearchIcon", Spec);
			ClearIcon = LoadTexture("Icon/Clear.png", "ClearIcon", Spec);
			SaveIcon = LoadTexture("Icon/Save.png", "SaveIcon", Spec);
			RefreshIcon = LoadTexture("Icon/Refresh.png", "RefreshIcon", Spec);
			GearIcon = LoadTexture("Icon/Gear.png", "GearIcon", Spec);
			PlusIcon = LoadTexture("Icon/Plus.png", "PlusIcon", Spec);
			LK_CORE_ASSERT(FolderIcon && FileIcon);
			LK_CORE_ASSERT(ForwardIcon && BackwardIcon && PointerIcon);
			LK_CORE_ASSERT(GearIcon && PlusIcon);
		}

		static void Destroy()
		{
			LK_CORE_DEBUG_TAG("EditorResources", "Releasing resources");
			FolderIcon.Reset();
			FileIcon.Reset();
			ForwardIcon.Reset();
			BackwardIcon.Reset();
			PointerIcon.Reset();
			PencilIcon.Reset();
			SearchIcon.Reset();
			ClearIcon.Reset();
			SaveIcon.Reset();
			RefreshIcon.Reset();
			GearIcon.Reset();
			PlusIcon.Reset();
		}

	private:
		static TObjectPtr<LTexture2D> LoadTexture(const std::filesystem::path& RelativePath, 
												  FTextureSpecification Specification = FTextureSpecification())
		{
			return LoadTexture(RelativePath, "", Specification);
		}

		static TObjectPtr<LTexture2D> LoadTexture(const std::filesystem::path& RelativePath, 
												  const std::string& Name, 
												  FTextureSpecification Specification)
		{
			Specification.DebugName = Name;
			std::filesystem::path Path = std::filesystem::path("Assets") / "Editor" / RelativePath;
			Specification.Path = Path.string();
			
			if (!LFileSystem::Exists(Path))
			{
				LK_CORE_VERIFY(false, "Failed to load texture '{}', the file does not exist", Path.string());
				return nullptr;
			}

			return LTexture2D::Create(Specification);
		}
	};

}

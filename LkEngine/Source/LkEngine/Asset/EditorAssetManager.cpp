#include "LKpch.h"
#include "EditorAssetManager.h"

#include "AssetManager.h"
#include "AssetImporter.h"
#include "MeshSerializer.h"

#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Core/Utility/StringUtils.h"
#include "LkEngine/Project/Project.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Texture.h"


namespace LkEngine {

	namespace fs = std::filesystem;

	LEditorAssetManager::LEditorAssetManager()
	{
		LOBJECT_REGISTER();
	}

	LEditorAssetManager::~LEditorAssetManager()
	{
		if (ObjectFlags & EObjectFlag::BeginDestroy)
		{
			Destroy();
		}
	}

	void LEditorAssetManager::Initialize()
	{
		LK_CORE_TRACE_TAG("EditorAssetManager", "Initializing");
		LObject::Initialize();

		LAssetImporter::Initialize();

		const fs::path AssetsDir = LFileSystem::GetAssetsDir();
		static const fs::path DebugTexturesDir = AssetsDir / "Textures/Debug";

		/* Declare the lambda first so it can be used recursively. */
		std::function<void(const fs::path&)> AddTexturesInDirectory;
		AddTexturesInDirectory = [&](const fs::path& Directory) -> void
		{
			if (!LFileSystem::IsDirectory(Directory))
			{
				return;
			}

			FTextureSpecification TextureSpec{};
			for (const fs::directory_entry& Entry : fs::directory_iterator(Directory))
			{
				const fs::path& Path = Entry.path();
				if (Entry.is_regular_file())
				{
					TextureSpec.Format = EImageFormat::RGBA32F;
					TextureSpec.Width = 2048;
					TextureSpec.Height = 2048;
					TextureSpec.Path = LFileSystem::ConvertToUnixPath(Path);
					TextureSpec.Name = LFileSystem::RemoveFileExtension(Path.filename());
					TextureSpec.DebugName = TextureSpec.Name;
					TextureSpec.SamplerWrap = ETextureWrap::Clamp;
					TextureSpec.SamplerFilter = ETextureFilter::Linear;
					TextureSpec.bGenerateMips = true;
					
					LK_CORE_TRACE_TAG("EditorAssetManager", "Loading texture: {} ({})", 
									  TextureSpec.Name, std::filesystem::relative(TextureSpec.Path, LFileSystem::GetAssetsDir()));
					TObjectPtr<LTexture2D> Texture = LTexture2D::Create(TextureSpec);
					Texture2DMap.insert({ TextureSpec.Name, Texture });
				}
				else if (Entry.is_directory())
				{
					LK_CORE_TRACE_TAG("EditorAssetManager", "Checking directory '{}' recursively", Path.string());
					AddTexturesInDirectory(Path);
				}
			}
		};

		LK_CORE_TRACE_TAG("EditorAssetManager", "Loading debug textures");
		AddTexturesInDirectory(DebugTexturesDir);

		LoadBaseMaterials();
		bAssetRegistryValid = LoadAssetRegistry();

		LoadPrimitiveShapes();
	}

	void LEditorAssetManager::Destroy()
	{
		WriteRegistryToDisk();
	}

	FAssetHandle LEditorAssetManager::ImportAsset(const std::filesystem::path& Filepath)
	{
		const std::filesystem::path Path = GetRelativePath(Filepath);
		if (const FAssetMetadata& Metadata = GetMetadata(Path); Metadata.IsValid())
		{
			return Metadata.Handle;
		}

		const EAssetType AssetType = GetAssetTypeFromPath(Path);
		if (AssetType == EAssetType::None)
		{
			LK_CORE_ERROR_TAG("EditorAssetManager", "Import of asset '{}' failed, asset type is: {}", 
							  Filepath.string(), Enum::ToString(AssetType));
			return 0;
		}

		LK_CORE_INFO_TAG("EditorAssetManager", "Storing asset '{}' in registry", Filepath.filename().string());
		FAssetMetadata Metadata;
		Metadata.Handle = FAssetHandle();
		Metadata.FilePath = Path;
		Metadata.Type = AssetType;
		AssetRegistry[Metadata.Handle] = Metadata;

		return Metadata.Handle;
	}

	void LEditorAssetManager::RemoveAsset(const FAssetHandle Handle)
	{
		if (LoadedAssets.contains(Handle))
		{
			LoadedAssets.erase(Handle);
		}

		if (MemoryAssets.contains(Handle))
		{
			MemoryAssets.erase(Handle);
		}

		if (AssetRegistry.Contains(Handle))
		{
			AssetRegistry.Remove(Handle);
		}
	}

	template<typename T>
	TObjectPtr<T> LEditorAssetManager::ImportAsset(const std::filesystem::path& InFilePath)
	{
		LK_UNUSED(InFilePath);
	}
	
	template<>
    TObjectPtr<LMeshSource> LEditorAssetManager::ImportAsset<LMeshSource>(const std::filesystem::path& InFilePath)
    {
		for (const auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.FilePath == InFilePath)
			{
				return LoadedAssets[Metadata.Handle];
			}
		}

        LAssimpMeshImporter MeshImporter(InFilePath);
		if (!MeshImporter.IsValid())
		{
			LK_CORE_ERROR_TAG("EditorAssetManager", "Mesh importer is not valid for: '{}'", InFilePath.string());
			return nullptr;
		}

        TObjectPtr<LMeshSource> MeshSource = MeshImporter.ImportToMeshSource();
		LoadedAssets[MeshSource->Handle] = MeshSource;

		MeshSource->FilePath = InFilePath;

		FAssetMetadata Metadata;
		Metadata.Handle = MeshSource->Handle;
		Metadata.Type = EAssetType::MeshSource;
		Metadata.FilePath = InFilePath;
		Metadata.bIsMemoryAsset = false;
		Metadata.bIsDataLoaded = true;
		AssetRegistry[MeshSource->Handle] = Metadata;
		LK_CORE_TRACE_TAG("EditorAssetManager", "Loaded MeshSource: '{}', {} vertices and {} indices", 
						  MeshSource->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());

		return LoadedAssets[MeshSource->Handle];
    }

	template<>
    TObjectPtr<LMesh> LEditorAssetManager::ImportAsset<LMesh>(const std::filesystem::path& InFilePath)
    {
		/* 
		 * TODO: Validate that the imported asset is of the exact same type that the template is using.
		 *       LStaticMesh -> .lsmesh, LMesh -> .lmesh
		 */
		for (const auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.FilePath == InFilePath)
			{
				if (!Metadata.bIsDataLoaded)
				{
					if (!ReloadData(Handle))
					{
						LK_CORE_ERROR_TAG("EditorAssetManager", "Failed to reload data for: {} ({})", InFilePath.string(), Handle);
						return nullptr;
					}
				}

				LK_CORE_VERIFY(LoadedAssets.contains(Metadata.Handle));
				return LoadedAssets[Metadata.Handle];
			}
		}

		std::ifstream InputStream(LProject::GetAssetDirectory() / InFilePath);
		LK_CORE_VERIFY(InputStream.good(), "Invalid input stream for: '{}'", InFilePath.string());
		std::stringstream StringStream;
		StringStream << InputStream.rdbuf();

		YAML::Node Data = YAML::Load(StringStream.str());
		YAML::Node MeshNode = Data["Mesh"];

		const FAssetHandle MeshSourceHandle = static_cast<FAssetHandle>(MeshNode["MeshSource"].as<uint64_t>());
		TObjectPtr<LMeshSource> MeshSource = GetAsset(MeshSourceHandle).As<LMeshSource>();
		LK_CORE_ASSERT(MeshSource, "MeshSource is nullptr, file: '{}' (MeshSource={})", InFilePath.string(), ObjectHandle);
		TObjectPtr<LMesh> Mesh = TObjectPtr<LMesh>::Create(MeshSource);
		Mesh->SetName(InFilePath.filename().string());

		LoadedAssets[Mesh->Handle] = Mesh;

		FAssetMetadata Metadata;
		Metadata.Handle = Mesh->Handle;
		Metadata.Type = EAssetType::Mesh;
		Metadata.FilePath = InFilePath;
		Metadata.bIsMemoryAsset = false;
		Metadata.bIsDataLoaded = true;
		AssetRegistry[Mesh->Handle] = Metadata;
		LK_CORE_VERIFY(Mesh->GetMaterialTable(), "Mesh '{}' ({}) is missing its material table", Mesh->Handle, Metadata.FilePath.string());
		LK_CORE_TRACE_TAG("EditorAssetManager", "Loaded mesh '{}', {} vertices and {} indices", 
						  Mesh->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());

		return LoadedAssets[Mesh->Handle];
    }

	template<>
    TObjectPtr<LStaticMesh> LEditorAssetManager::ImportAsset<LStaticMesh>(const std::filesystem::path& InFilePath)
    {
		for (const auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.FilePath == InFilePath)
			{
				if (!Metadata.bIsDataLoaded)
				{
					if (!ReloadData(Handle))
					{
						LK_CORE_ERROR_TAG("EditorAssetManager", "Failed to reload data for: {} ({})", InFilePath.string(), Handle);
						return nullptr;
					}
				}

				LK_CORE_TRACE_TAG("EditorAssetManager", "Found static mesh loaded for: {}", InFilePath.filename().string());
				LK_CORE_VERIFY(LoadedAssets.contains(Metadata.Handle));
				return LoadedAssets[Metadata.Handle];
			}
		}

		std::ifstream InputStream(LProject::GetAssetDirectory() / InFilePath);
		LK_CORE_VERIFY(InputStream.good(), "Invalid input stream for: '{}'", InFilePath.string());
		std::stringstream StringStream;
		StringStream << InputStream.rdbuf();

		YAML::Node Data = YAML::Load(StringStream.str());
		YAML::Node MeshNode = Data["Mesh"];

		const FAssetHandle MeshSourceHandle = static_cast<FAssetHandle>(MeshNode["MeshSource"].as<uint64_t>());
		TObjectPtr<LMeshSource> MeshSource = GetAsset(MeshSourceHandle).As<LMeshSource>();
		LK_CORE_ASSERT(MeshSource, "MeshSource is nullptr, file: '{}' (MeshSource={})", InFilePath.string(), ObjectHandle);

		TObjectPtr<LStaticMesh> Mesh = TObjectPtr<LStaticMesh>::Create(MeshSource);
		Mesh->SetName(InFilePath.filename().string());

		LoadedAssets[Mesh->Handle] = Mesh;

		FAssetMetadata Metadata;
		Metadata.Handle = Mesh->Handle;
		Metadata.Type = EAssetType::Mesh;
		Metadata.FilePath = InFilePath;
		Metadata.bIsMemoryAsset = false;
		Metadata.bIsDataLoaded = true;
		AssetRegistry[Mesh->Handle] = Metadata;
		LK_CORE_VERIFY(Mesh->GetMaterialTable(), "Mesh '{}' ({}) is missing its MaterialTable", Mesh->Handle, Metadata.FilePath.string());
		LK_CORE_TRACE_TAG("EditorAssetManager", "Loaded static mesh '{}', {} vertices and {} indices", 
						  Mesh->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());

		return LoadedAssets[Mesh->Handle];
    }

	TObjectPtr<LAsset> LEditorAssetManager::GetAsset(const FAssetHandle Handle)
    {
		if (IsMemoryAsset(Handle))
		{
			LK_CORE_ASSERT(MemoryAssets.find(Handle) != MemoryAssets.end(), "Memory asset '{}' is not in map", Handle);
			return MemoryAssets[Handle];
		}

		FAssetMetadata& Metadata = GetMetadataInternal(Handle);
		if (!Metadata.IsValid())
		{
			LK_CORE_ERROR_TAG("EditorAssetManager", "GetAsset failed for {}, metadata is invalid for: {}", Handle, Metadata.ToString());
			return nullptr;
		}

		TObjectPtr<LAsset> Asset = nullptr;
		if (!Metadata.bIsDataLoaded)
		{
			Metadata.bIsDataLoaded = LAssetImporter::TryLoadData(Metadata, Asset);
			if (!Metadata.bIsDataLoaded && (Metadata.Type != EAssetType::None))
			{
				LK_CORE_ERROR_TAG("EditorAssetManager", "Failed to load asset: {}\n{}", Handle, Metadata.ToString());
				return nullptr;
			}

			LK_CORE_TRACE_TAG("EditorAssetManager", "Successfully loaded asset '{}' ({}) in to memory", Handle, Enum::ToString(Metadata.Type));
			LoadedAssets[Handle] = Asset;
		}
		else
		{
			Asset = LoadedAssets[Handle];
		}
		LK_CORE_VERIFY(Asset, "Invalid asset: {}", Handle);

		return Asset;
    }

	EAssetType LEditorAssetManager::GetAssetType(const FAssetHandle Handle) 
	{
		if (TObjectPtr<LAsset> Asset = GetAsset(Handle))
		{
			return Asset->GetAssetType();
		}

		return EAssetType::None;
	}

	void LEditorAssetManager::AddMemoryOnlyAsset(TObjectPtr<LAsset> Asset)
	{
		MemoryAssets[Asset->Handle] = Asset;
	}

	bool LEditorAssetManager::ReloadData(const FAssetHandle Handle)
	{
		FAssetMetadata& Metadata = GetMetadataInternal(Handle);
		if (!Metadata.IsValid())
		{
			LK_CORE_ERROR_TAG("EditorAssetManager", "Cannot reload invalid asset '{}'\n{}", Handle, Metadata.ToString());
			return false;
		}

		TObjectPtr<LAsset> Asset;
		Metadata.bIsDataLoaded = LAssetImporter::TryLoadData(Metadata, Asset);
		if (Metadata.bIsDataLoaded)
		{
			LoadedAssets[Handle] = Asset;
		}
		else
		{
			LK_CORE_ERROR_TAG("EditorAssetManager", "Could not load data for: {}", Handle);
		}

		return Metadata.bIsDataLoaded;
	}

	bool LEditorAssetManager::IsAssetHandleValid(const FAssetHandle Handle) const
	{
		if (Handle <= 0)
		{
			return false;
		}

		return IsMemoryAsset(Handle) || GetMetadata(Handle).IsValid();
	}

	bool LEditorAssetManager::IsAssetLoaded(const FAssetHandle Handle) const
	{
		return LoadedAssets.contains(Handle);
	}

	std::filesystem::path LEditorAssetManager::GetFileSystemPath(const FAssetMetadata& Metadata) const
	{
		return std::filesystem::absolute(LProject::GetAssetDirectory() / Metadata.FilePath);
	}

	std::filesystem::path LEditorAssetManager::GetFileSystemPath(const FAssetHandle Handle) const
	{
		return GetFileSystemPath(GetMetadata(Handle));
	}

	bool LEditorAssetManager::LoadAssetRegistry()
	{
		const std::filesystem::path& AssetRegistryPath = LProject::GetAssetRegistryPath();
		LK_CORE_INFO_TAG("EditorAssetManager", "Loading asset registry ({})", AssetRegistryPath.filename().string());
		if (!LFileSystem::Exists(AssetRegistryPath))
		{
			LK_CORE_ERROR_TAG("EditorAssetManager", "Asset registry file does not exist at: '{}'", AssetRegistryPath.string());
			return false;
		}

		std::ifstream InputStream(AssetRegistryPath);
		LK_CORE_VERIFY(InputStream.is_open(), "Inputstream is not OK");
		std::stringstream StringStream;
		StringStream << InputStream.rdbuf();

		YAML::Node Data = YAML::Load(StringStream.str());
		YAML::Node AssetHandles = Data["Assets"];
		if (!AssetHandles)
		{
			LK_CORE_VERIFY(false, "Asset registry failed to load: {}", AssetRegistryPath.string());
			return false;
		}

		for (auto Entry : AssetHandles)
		{
			const std::string Filepath = Entry["FilePath"].as<std::string>();

			FAssetMetadata Metadata{};
			Metadata.Handle = Entry["Handle"].as<FAssetHandle>();
			Metadata.FilePath = Filepath;
			Metadata.Type = Enum::FromString(Entry["Type"].as<std::string>());

		#if LK_DEBUG_ASSET_LOG
			if ((Metadata.Type == EAssetType::MeshSource) 
				|| (Metadata.Type == EAssetType::StaticMesh)
				|| (Metadata.Type == EAssetType::Mesh))
			{
				LK_CORE_INFO("Loading: {}  {}", Filepath, Metadata.ToString());
			}
		#endif

			if (Metadata.Type == EAssetType::None)
			{
				LK_CORE_WARN_TAG("EditorAssetManager", "Invalid asset type for: {}", Metadata.ToString());
				continue;
			}

			if (const EAssetType AssetType = GetAssetTypeFromPath(Filepath); Metadata.Type != AssetType)
			{
				LK_CORE_WARN_TAG("EditorAssetManager", "Mismatch between stored type and extension type for '{}' ({})\n{}", 
								 Metadata.Handle, Metadata.FilePath.string(), Metadata.ToString());
				Metadata.Type = AssetType;
				LK_CORE_VERIFY(AssetType != EAssetType::None);
			}

			if (Metadata.Handle == 0)
			{
				LK_CORE_WARN_TAG("EditorAssetManager", "AssetHandle is 0 for: {}\n{}", Metadata.FilePath, Metadata.ToString(false));
				continue;
			}

			LK_CORE_TRACE_TAG("EditorAssetManager", "Handle='{}'  Type='{}'  FilePath='{}'", 
							  Metadata.Handle, Enum::ToString(Metadata.Type), Metadata.FilePath);
			AssetRegistry[Metadata.Handle] = Metadata;
		}

		return true;
	}

	void LEditorAssetManager::WriteRegistryToDisk()
	{
		LK_CORE_INFO_TAG("EditorAssetManager", "Saving asset registry to disk");

		/* Sort Assets by their UUID. */
		struct AssetRegistryEntry
		{
			std::string FilePath{};
			EAssetType Type = EAssetType::None;
		};

		std::map<FAssetHandle, AssetRegistryEntry> SortedMap;
		for (auto& [Filepath, Metadata] : AssetRegistry)
		{
			if (Metadata.bIsMemoryAsset)
			{
				LK_CORE_TRACE_TAG("EditorAssetManager", "Asset '{}' is memory-only, skipping it", Metadata.Handle);
				continue;
			}

			if (Metadata.Type == EAssetType::None)
			{
				LK_CORE_WARN_TAG("EditorAssetManager", "Type for '{}' is , skipping it", Metadata.Handle, Enum::ToString(Metadata.Type));
				continue;
			}

			std::string PathToSerialize = Metadata.FilePath.string();
			std::replace(PathToSerialize.begin(), PathToSerialize.end(), '\\', '/');
			SortedMap[Metadata.Handle] = { PathToSerialize, Metadata.Type };
		}

		LK_CORE_TRACE_TAG("EditorAssetManager", "Serializing asset registry with {} entries", SortedMap.size());
		YAML::Emitter Out;
		Out << YAML::BeginMap;
		Out << YAML::Key << "Assets" << YAML::BeginSeq;
		for (auto& [Handle, Entry] : SortedMap)
		{
			Out << YAML::BeginMap;
			Out << YAML::Key << "Handle" << YAML::Value << Handle;
			Out << YAML::Key << "Type" << YAML::Value << Enum::ToString(Entry.Type);
			Out << YAML::Key << "FilePath" << YAML::Value << Entry.FilePath;
			Out << YAML::EndMap;

		#if LK_DEBUG_ASSET_LOG
			if ((Entry.Type == EAssetType::MeshSource) 
				|| (Entry.Type == EAssetType::StaticMesh) 
				|| (Entry.Type == EAssetType::Mesh))
			{
				LK_CORE_INFO("Handle={} Type={} ({})", Handle, Enum::ToString(Entry.Type), Entry.FilePath);
			}
		#endif
		}
		Out << YAML::EndSeq;
		Out << YAML::EndMap;

		const std::string& AssetRegistryPath = LProject::GetAssetRegistryPath().string();
		std::ofstream FileOut(AssetRegistryPath);
		FileOut << Out.c_str();
	}

	const FAssetMetadata& LEditorAssetManager::GetMetadata(const FAssetHandle Handle) const
	{
		if (AssetRegistry.Contains(Handle))
		{
			return AssetRegistry.Get(Handle);
		}

		return NullMetadata;
	}

    FAssetMetadata& LEditorAssetManager::GetMetadataInternal(const FAssetHandle Handle)
	{
		if (AssetRegistry.Contains(Handle))
		{
	    	return AssetRegistry[Handle];
		}

	    return NullMetadata; 
	}

    const FAssetMetadata& LEditorAssetManager::GetMetadata(const TObjectPtr<LAsset>& Asset) const
	{
		return GetMetadata(Asset->Handle);
	}

	const FAssetMetadata& LEditorAssetManager::GetMetadata(const std::filesystem::path& FilePath) const
	{
		LK_CORE_ASSERT(AssetRegistry.Count() > 0, "AssetRegistry is empty");
		const std::filesystem::path RelativePath = GetRelativePath(FilePath);
		for (auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.FilePath == RelativePath)
			{
				return Metadata;
			}
		}

		return NullMetadata;
	}

	std::filesystem::path LEditorAssetManager::GetRelativePath(const std::filesystem::path& FilePath) const
	{
		std::filesystem::path RelativePath = FilePath.lexically_normal();
		const std::string TempPath = FilePath.string();
		if (TempPath.find(LProject::GetAssetDirectory().string()) != std::string::npos)
		{
			RelativePath = std::filesystem::relative(FilePath, LProject::GetAssetDirectory());
			if (RelativePath.empty())
			{
				RelativePath = FilePath.lexically_normal();
			}
		}

		return RelativePath;
	}

    FAssetHandle LEditorAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& FilePath) const
	{
		return GetMetadata(FilePath).Handle;
	}

	EAssetType LEditorAssetManager::GetAssetTypeFromExtension(const std::string& InExtension) const
	{
		const std::string Extension = StringUtils::ToLower(InExtension);
		if (AssetExtensionMap.find(Extension) == AssetExtensionMap.end())
		{
			return EAssetType::None;
		}

		return AssetExtensionMap.at(Extension.c_str());
	}

	EAssetType LEditorAssetManager::GetAssetTypeFromPath(const std::filesystem::path& Path) const
	{
		return GetAssetTypeFromExtension(Path.extension().string());
	}

	std::unordered_set<FAssetHandle> LEditorAssetManager::GetAllAssetsWithType(const EAssetType AssetType)
	{
		std::unordered_set<FAssetHandle> AssetsOfType;
		for (const auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.Type == AssetType)
			{
				AssetsOfType.insert(Handle);
			}
		}

		return AssetsOfType;
	}

	std::size_t LEditorAssetManager::GetAllAssetsWithType(const EAssetType AssetType, std::unordered_set<FAssetHandle>& AssetsOfType)
	{
		AssetsOfType.clear();
		for (const auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.Type == AssetType)
			{
				AssetsOfType.insert(Handle);
			}
		}

		return AssetsOfType.size();
	}

	int LEditorAssetManager::GetTextures2D(std::vector<TObjectPtr<LTexture2D>>& TextureContainer)
	{
		TextureContainer.clear();
		TextureContainer.reserve(Texture2DMap.size());

        /* Copy the texture instances to the vector. */
        std::ranges::transform(
			Texture2DMap, 
			std::back_inserter(TextureContainer),
			[](const auto& PairEntry) { return PairEntry.second; }
		);

		return static_cast<int>(TextureContainer.size());
	}

	int LEditorAssetManager::GetTextures2D(std::vector<std::pair<std::string, TObjectPtr<LTexture2D>>>& TextureContainer)
	{
		TextureContainer.clear();
		TextureContainer.reserve(Texture2DMap.size());

        /* Copy all entries. */
        std::ranges::copy(
			Texture2DMap, 
			std::back_inserter(TextureContainer)
		);

		return static_cast<int>(TextureContainer.size());
	}

	const std::vector<std::pair<std::string, TObjectPtr<LTexture2D>>>& LEditorAssetManager::GetTextures2D() const
	{
        static std::vector<std::pair<std::string, TObjectPtr<LTexture2D>>> TexturesArray;
		TexturesArray.clear();
		TexturesArray.reserve(Texture2DMap.size());

        /* Copy all entries. */
        std::ranges::copy(
			Texture2DMap, 
			std::back_inserter(TexturesArray)
		);

		return TexturesArray;
	}

	void LEditorAssetManager::LoadBaseMaterials()
	{
		/* Base Material 1 */
		{
			TObjectPtr<LMaterial> BaseMat = LMaterial::Create(
				LRenderer::GetShaderLibrary()->Get("Renderer_Model"),
				BASE_MATERIAL
			);

			/* Set the texture. */
			static const std::string TextureName = "WoodContainer";
			LK_CORE_ASSERT(Texture2DMap.contains(TextureName));
			TObjectPtr<LTexture2D> Texture = Texture2DMap.at(TextureName);
			LK_CORE_VERIFY(Texture);
			BaseMat->SetTexture(Texture.As<LTexture>());

			BaseMaterial = TObjectPtr<LMaterialAsset>::Create(BaseMat);
			LK_CORE_VERIFY(BaseMaterial);
			Material_WoodContainer = BaseMaterial;

			FAssetMetadata MatAssetMetadata{};
			MatAssetMetadata.Handle = BaseMaterial->Handle;
			MatAssetMetadata.Type = EAssetType::Material;
			MatAssetMetadata.bIsDataLoaded = true;
			MatAssetMetadata.bIsMemoryAsset = true;
			AssetRegistry[BaseMaterial->Handle] = MatAssetMetadata;
			MemoryAssets[BaseMaterial->Handle] = BaseMaterial;
		}

		/* Base Material 2 */
		{
			TObjectPtr<LMaterial> BaseMat2 = LMaterial::Create(
				LRenderer::GetShaderLibrary()->Get("Renderer_Model"), 
				BASE_MATERIAL2
			);

			/* Set the texture. */
			static const std::string TextureName = "Metal";
			LK_CORE_ASSERT(Texture2DMap.contains(TextureName));
			TObjectPtr<LTexture2D> Texture = Texture2DMap.at(TextureName);
			LK_CORE_VERIFY(Texture);
			BaseMat2->SetTexture(Texture.As<LTexture>());

			BaseMaterial2 = TObjectPtr<LMaterialAsset>::Create(BaseMat2);
			LK_CORE_VERIFY(BaseMaterial2);
			Material_Metal = BaseMaterial2;

			FAssetMetadata MatAssetMetadata{};
			MatAssetMetadata.Handle = BaseMaterial2->Handle;
			MatAssetMetadata.Type = EAssetType::Material;
			MatAssetMetadata.bIsDataLoaded = true;
			MatAssetMetadata.bIsMemoryAsset = true;
			AssetRegistry[BaseMaterial2->Handle] = MatAssetMetadata;
			MemoryAssets[BaseMaterial2->Handle] = BaseMaterial2;
		}

		LK_CORE_TRACE_TAG("EditorAssetManager", "BaseMaterial:  {} ({})", BaseMaterial->Handle, BaseMaterial->GetMaterial()->GetTexture()->GetName());
		LK_CORE_TRACE_TAG("EditorAssetManager", "BaseMaterial2: {} ({})", BaseMaterial2->Handle, BaseMaterial2->GetMaterial()->GetTexture()->GetName());
	}

	void LEditorAssetManager::LoadPrimitiveShapes()
    {
		LK_CORE_ASSERT(BaseMaterial && BaseMaterial2);
		LK_CORE_ASSERT(Material_WoodContainer && Material_Metal);
		LK_CORE_TRACE_TAG("EditorAssetManager", "Loading shapes");

		/* Mesh: Cube. */
		{
			TObjectPtr<LMesh> CubeMesh = ImportAsset<LMesh>("Meshes/Default/Cube.lmesh");
			LK_CORE_VERIFY(CubeMesh && CubeMesh->GetMaterialTable() && CubeMesh->GetMaterialTable()->HasMaterial(0), "Import of Cube.lmesh failed");

			LK_CORE_TRACE_TAG("EditorAssetManager", "Getting cube mesh asset: {}", CubeMesh->Handle);
			TObjectPtr<LAsset> CubeMeshAsset = GetAsset(CubeMesh->Handle);
			LK_CORE_VERIFY(CubeMeshAsset, "CubeMeshAsset not valid");
			//CubeMesh->GetMaterialTable()->SetMaterial(0, BaseMaterial->Handle);

			/* TODO: The application of the shader layout should be automized somehow. */
			/* Setup the vertexbuffer layout. */
			TObjectPtr<LMeshSource> CubeSource = CubeMesh->GetMeshSource();
			CubeSource->VertexBuffer->SetLayout({
				{ "a_Position",   EShaderDataType::Float3 },
				{ "a_Normal",     EShaderDataType::Float3 },
				{ "a_Binormal",   EShaderDataType::Float3 },
				{ "a_Tangent",    EShaderDataType::Float3 },
				{ "a_TexCoord",   EShaderDataType::Float2 },
			});
			CubeSource->VertexBuffer->SetIndexBuffer(CubeSource->IndexBuffer);

		#if LK_DEBUG_ASSET_LOG
			LK_CORE_INFO("Cube Mesh: {}", CubeMesh->Handle);
			LK_CORE_INFO("Cube Mesh Source: {} ({})", CubeSource->Handle, CubeSource->GetFilePath().string());
		#endif
		}

		/* StaticMesh: Cube. */
		{
			TObjectPtr<LStaticMesh> StaticCubeMesh = ImportAsset<LStaticMesh>("Meshes/Default/Cube.lsmesh");
			LK_CORE_VERIFY(StaticCubeMesh && StaticCubeMesh->GetMaterialTable(), "Import of Cube.lsmesh failed");
			StaticCubeMesh->SetName("StaticCubeMesh");

			LK_CORE_TRACE_TAG("EditorAssetManager", "Getting static cube mesh asset: {}", StaticCubeMesh->Handle);
			TObjectPtr<LAsset> CubeMeshAsset = GetAsset(StaticCubeMesh->Handle);
			LK_CORE_VERIFY(CubeMeshAsset, "CubeMeshAsset not valid");

			/* TODO: The application of the shader layout should be automized somehow. */
			/* Setup the vertexbuffer layout. */
			TObjectPtr<LMeshSource> CubeSource = StaticCubeMesh->GetMeshSource();
			CubeSource->VertexBuffer->SetLayout({
				{ "a_Position",   EShaderDataType::Float3 },
				{ "a_Normal",     EShaderDataType::Float3 },
				{ "a_Binormal",   EShaderDataType::Float3 },
				{ "a_Tangent",    EShaderDataType::Float3 },
				{ "a_TexCoord",   EShaderDataType::Float2 },
			});
			CubeSource->VertexBuffer->SetIndexBuffer(CubeSource->IndexBuffer);

		#if LK_DEBUG_ASSET_LOG
			LK_CORE_INFO("Static Cube Mesh: {}", StaticCubeMesh->Handle);
			LK_CORE_INFO("Static Cube Mesh Source: {} ({})", CubeSource->Handle, CubeSource->GetFilePath().string());
		#endif
		}

		/* StaticMesh: Plane. */
		{
			TObjectPtr<LStaticMesh> StaticPlaneMesh = ImportAsset<LStaticMesh>("Meshes/Default/Plane.lsmesh");
			LK_CORE_VERIFY(StaticPlaneMesh && StaticPlaneMesh->GetMaterialTable(), "Import of Plane.lsmesh failed");
			StaticPlaneMesh->SetName("StaticPlaneMesh");

		#if 0
			TObjectPtr<LMaterialTable> MaterialTable = StaticPlaneMesh->GetMaterialTable();
			if (!MaterialTable->HasMaterial(0))
			{
				MaterialTable->SetMaterial(0, Material_Metal->Handle);
			}
			else
			{
				MaterialTable->SetMaterial(0, Material_Metal->Handle);
			}
		#endif

			LK_CORE_TRACE_TAG("EditorAssetManager", "Getting static plane mesh asset: {}", StaticPlaneMesh->Handle);
			TObjectPtr<LAsset> PlaneMeshAsset = GetAsset(StaticPlaneMesh->Handle);
			LK_CORE_VERIFY(PlaneMeshAsset, "PlaneMeshAsset not valid");

			/* TODO: The application of the shader layout should be automized somehow. */
			/* Setup the vertexbuffer layout. */
			TObjectPtr<LMeshSource> PlaneSource = StaticPlaneMesh->GetMeshSource();
			PlaneSource->VertexBuffer->SetLayout({
				{ "a_Position",   EShaderDataType::Float3 },
				{ "a_Normal",     EShaderDataType::Float3 },
				{ "a_Binormal",   EShaderDataType::Float3 },
				{ "a_Tangent",    EShaderDataType::Float3 },
				{ "a_TexCoord",   EShaderDataType::Float2 },
			});
			PlaneSource->VertexBuffer->SetIndexBuffer(PlaneSource->IndexBuffer);

		#if LK_DEBUG_ASSET_LOG
			LK_CORE_INFO("Static Plane Mesh: {}", StaticPlaneMesh->Handle);
			LK_CORE_INFO("Static Plane Mesh Source: {} ({})", PlaneSource->Handle, PlaneSource->GetFilePath().string());
			//PlaneSource->DumpVertexBuffer();
		#endif
		}

		/* StaticMesh: Ramp. */
		{
			TObjectPtr<LStaticMesh> StaticRampMesh = ImportAsset<LStaticMesh>("Meshes/Default/Ramp.lsmesh");
			LK_CORE_VERIFY(StaticRampMesh, "Import of Ramp.lsmesh failed");
			StaticRampMesh->SetName("StaticRampMesh");
			//LK_CORE_VERIFY(StaticRampMesh && StaticRampMesh->GetMaterialTable(), "Import of Ramp.lsmesh failed");

			TObjectPtr<LMaterialTable> MaterialTable = StaticRampMesh->GetMaterialTable();
			LK_CORE_ASSERT(MaterialTable);
		#if 0
			if (!MaterialTable->HasMaterial(0))
			{
				MaterialTable->SetMaterial(0, BaseMaterial->Handle);
			}
			else
			{
				MaterialTable->SetMaterial(0, BaseMaterial->Handle);
			}
		#endif

			LK_CORE_TRACE_TAG("EditorAssetManager", "Getting static ramp mesh asset: {}", StaticRampMesh->Handle);
			TObjectPtr<LAsset> RampMeshAsset = GetAsset(StaticRampMesh->Handle);
			LK_CORE_VERIFY(RampMeshAsset);

			/* TODO: The application of the shader layout should be automized somehow. */
			/* Setup the vertexbuffer layout. */
			TObjectPtr<LMeshSource> RampSource = StaticRampMesh->GetMeshSource();
			RampSource->VertexBuffer->SetLayout({
				{ "a_Position",   EShaderDataType::Float3 },
				{ "a_Normal",     EShaderDataType::Float3 },
				{ "a_Binormal",   EShaderDataType::Float3 },
				{ "a_Tangent",    EShaderDataType::Float3 },
				{ "a_TexCoord",   EShaderDataType::Float2 },
			});
			RampSource->VertexBuffer->SetIndexBuffer(RampSource->IndexBuffer);

		#if LK_DEBUG_ASSET_LOG
			LK_CORE_INFO("Static Ramp Mesh: {}", StaticRampMesh->Handle);
			LK_CORE_INFO("Static Ramp Mesh Source: {} ({})", RampSource->Handle, RampSource->GetFilePath().string());
		#endif
		}

    }

}

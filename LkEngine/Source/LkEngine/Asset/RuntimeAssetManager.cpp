#include "LKpch.h"
#include "RuntimeAssetManager.h"

#include "AssetManager.h"
#include "AssetImporter.h"
#include "MeshSerializer.h"

#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Core/Utility/StringUtils.h"
#include "LkEngine/Project/Project.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	namespace fs = std::filesystem;

	LRuntimeAssetManager::LRuntimeAssetManager()
	{
		LOBJECT_REGISTER();
	}

	LRuntimeAssetManager::~LRuntimeAssetManager()
	{
		if (ObjectFlags & EObjectFlag::BeginDestroy)
		{
			Destroy();
		}
	}

	void LRuntimeAssetManager::Initialize()
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		LObject::Initialize();

		LAssetImporter::Initialize();
		bAssetRegistryValid = LoadAssetRegistry();

		/* TODO: */
		/* Load textures into cache. */

		LoadPrimitiveShapes();
	}

	void LRuntimeAssetManager::Destroy()
	{
		WriteRegistryToDisk();
	}

	FAssetHandle LRuntimeAssetManager::ImportAsset(const std::filesystem::path& Filepath)
	{
		fs::path Path = GetRelativePath(Filepath);

		if (const FAssetMetadata& Metadata = GetMetadata(Path); Metadata.IsValid())
		{
			return Metadata.Handle;
		}

		const EAssetType AssetType = GetAssetTypeFromPath(Path);
		if (AssetType == EAssetType::None)
		{
			LK_CORE_ERROR_TAG("RuntimeAssetManager", "Import of asset '{}' failed, asset type is: {}", 
							  Filepath.string(), Enum::ToString(AssetType));
			return 0;
		}

		FAssetMetadata Metadata;
		Metadata.Handle = FAssetHandle();
		Metadata.FilePath = Path;
		Metadata.Type = AssetType;
		AssetRegistry[Metadata.Handle] = Metadata;

		return Metadata.Handle;
	}

	void LRuntimeAssetManager::RemoveAsset(const FAssetHandle Handle)
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
	TObjectPtr<T> LRuntimeAssetManager::ImportAsset(const std::filesystem::path& InFilePath)
	{
		LK_UNUSED(InFilePath);
	}
	
	template<>
    TObjectPtr<LMeshSource> LRuntimeAssetManager::ImportAsset<LMeshSource>(const std::filesystem::path& InFilePath)
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
			LK_CORE_ERROR("Mesh importer is not valid for: '{}'", InFilePath);
			return nullptr;
		}

        TObjectPtr<LMeshSource> MeshSource = MeshImporter.ImportToMeshSource();
		LoadedAssets[MeshSource->Handle] = MeshSource;

		FAssetMetadata Metadata;
		Metadata.Handle = MeshSource->Handle;
		Metadata.Type = EAssetType::MeshSource;
		Metadata.FilePath = InFilePath;
		Metadata.bIsMemoryAsset = false;
		Metadata.bIsDataLoaded = true;
		AssetRegistry[MeshSource->Handle] = Metadata;
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loaded MeshSource: '{}', {} vertices and {} indices", 
						  MeshSource->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());

		return LoadedAssets[MeshSource->Handle];
    }

	template<>
    TObjectPtr<LMesh> LRuntimeAssetManager::ImportAsset<LMesh>(const std::filesystem::path& InFilePath)
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
						LK_CORE_WARN_TAG("AssetManager", "Failed to reload data for '{}' ({})", InFilePath.string(), Handle);
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
		LK_CORE_VERIFY(Mesh->GetMaterialTable(), "Mesh '{}' ({}) is missing its MaterialTable", Mesh->Handle, Metadata.FilePath.string());
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loaded mesh '{}', {} vertices and {} indices", 
						  Mesh->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());

		return LoadedAssets[Mesh->Handle];
    }

	template<>
    TObjectPtr<LStaticMesh> LRuntimeAssetManager::ImportAsset<LStaticMesh>(const std::filesystem::path& InFilePath)
    {
		for (const auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.FilePath == InFilePath)
			{
				if (!Metadata.bIsDataLoaded)
				{
					if (!ReloadData(Handle))
					{
						LK_CORE_WARN_TAG("AssetManager", "Failed to reload data for '{}' ({})", InFilePath.string(), Handle);
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
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loaded static mesh '{}', {} vertices and {} indices", 
						  Mesh->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());

		return LoadedAssets[Mesh->Handle];
    }


	void LRuntimeAssetManager::AddMemoryOnlyAsset(TObjectPtr<LAsset> Asset)
	{
		MemoryAssets[Asset->Handle] = Asset;
	}

	bool LRuntimeAssetManager::ReloadData(const FAssetHandle Handle)
	{
		FAssetMetadata& Metadata = GetMetadataInternal(Handle);
		if (!Metadata.IsValid())
		{
			LK_CORE_ERROR_TAG("RuntimeAssetManager", "Cannot reload invalid asset '{}'\n{}", Handle, Metadata.ToString());
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
			LK_CORE_ERROR_TAG("RuntimeAssetManager", "Could not load data for: {}", Handle);
		}

		return Metadata.bIsDataLoaded;
	}

	bool LRuntimeAssetManager::IsAssetHandleValid(const FAssetHandle Handle) const
	{
		if (Handle <= 0)
		{
			return false;
		}

		return IsMemoryAsset(Handle) || GetMetadata(Handle).IsValid();
	}

	bool LRuntimeAssetManager::IsAssetLoaded(const FAssetHandle Handle) const
	{
		return LoadedAssets.contains(Handle);
	}

	EAssetType LRuntimeAssetManager::GetAssetType(FAssetHandle Handle)
	{
		if (TObjectPtr<LAsset> Asset = GetAsset(Handle))
		{
			return Asset->GetAssetType();
		}

		return EAssetType::None;
	}

	TObjectPtr<LAsset> LRuntimeAssetManager::GetAsset(const FAssetHandle Handle)
    {
		if (IsMemoryAsset(Handle))
		{
			LK_CORE_ASSERT(MemoryAssets.find(Handle) != MemoryAssets.end(), "Memory asset '{}' is not in map", Handle);
			return MemoryAssets[Handle];
		}

		FAssetMetadata& Metadata = GetMetadataInternal(Handle);
		if (!Metadata.IsValid())
		{
			LK_CORE_ERROR_TAG("RuntimeAssetManager", "GetAsset failed for {}, metadata is invalid for: {}", Handle, Metadata.ToString());
			return nullptr;
		}

		TObjectPtr<LAsset> Asset = nullptr;
		if (!Metadata.bIsDataLoaded)
		{
			Metadata.bIsDataLoaded = LAssetImporter::TryLoadData(Metadata, Asset);
			if (!Metadata.bIsDataLoaded && (Metadata.Type != EAssetType::None))
			{
				LK_CORE_ERROR_TAG("RuntimeAssetManager", "Failed to load asset: {}\n{}", Handle, Metadata.ToString());
				return nullptr;
			}

			LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Asset '{}' ({}) was loaded", Handle, Enum::ToString(Metadata.Type));
			LoadedAssets[Handle] = Asset;
		}
		else
		{
			Asset = LoadedAssets[Handle];
		}

		LK_CORE_VERIFY(Asset, "Asset not valid: {}", Handle);

		return Asset;
    }

	bool LRuntimeAssetManager::LoadAssetRegistry()
	{
		const std::filesystem::path& AssetRegistryPath = LProject::GetAssetRegistryPath();
		LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Loading asset registry ({})", AssetRegistryPath.filename().string());
		if (!LFileSystem::Exists(AssetRegistryPath))
		{
			LK_CORE_ERROR_TAG("RuntimeAssetManager", "Asset registry file does not exist at: '{}'", AssetRegistryPath.string());
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
				LK_CORE_WARN_TAG("RuntimeAssetManager", "Invalid asset type for: {}", Metadata.ToString());
				continue;
			}

			if (const EAssetType AssetType = GetAssetTypeFromPath(Filepath); Metadata.Type != AssetType)
			{
				LK_CORE_WARN_TAG("RuntimeAssetManager", "Mismatch between stored type and extension type for '{}' ({})\n{}", 
								 Metadata.Handle, Metadata.FilePath.string(), Metadata.ToString());
				Metadata.Type = AssetType;
				LK_CORE_VERIFY(AssetType != EAssetType::None);
			}

			if (Metadata.Handle == 0)
			{
				LK_CORE_WARN_TAG("RuntimeAssetManager", "AssetHandle is 0 for: {}\n{}", Metadata.FilePath, Metadata.ToString(false));
				continue;
			}

			LK_CORE_TRACE_TAG("RuntimeAssetManager", "Handle='{}'  Type='{}'  FilePath='{}'", 
							  Metadata.Handle, Enum::ToString(Metadata.Type), Metadata.FilePath);
			AssetRegistry[Metadata.Handle] = Metadata;
		}

		return true;
	}

	void LRuntimeAssetManager::WriteRegistryToDisk()
	{
		LK_CORE_INFO_TAG("RuntimeAssetManager", "Writing asset registry to disk");
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
				LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Asset '{}' is memory-only, skipping it", Metadata.Handle);
				continue;
			}

			if (Metadata.Type == EAssetType::None)
			{
				LK_CORE_WARN_TAG("RuntimeAssetManager", "Type for '{}' is , skipping it", Metadata.Handle, Enum::ToString(Metadata.Type));
				continue;
			}

			std::string PathToSerialize = Metadata.FilePath.string();
			std::replace(PathToSerialize.begin(), PathToSerialize.end(), '\\', '/');
			SortedMap[Metadata.Handle] = { PathToSerialize, Metadata.Type };
		}

		LK_CORE_DEBUG_TAG("AssetManager", "Serializing asset registry with {} entries", SortedMap.size());
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

	const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const FAssetHandle Handle) const
	{
		if (AssetRegistry.Contains(Handle))
		{
			return AssetRegistry.Get(Handle);
		}

		return NullMetadata;
	}

    FAssetMetadata& LRuntimeAssetManager::GetMetadataInternal(const FAssetHandle Handle)
	{
		if (AssetRegistry.Contains(Handle))
		{
	    	return AssetRegistry[Handle];
		}

	    return NullMetadata; 
	}

    LUUID LRuntimeAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& FilePath) const
	{
		return GetMetadata(FilePath).Handle;
	}

    const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const TObjectPtr<LAsset>& Asset) const
	{
		return GetMetadata(Asset->Handle);
	}

	const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const std::filesystem::path& FilePath) const
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

	std::filesystem::path LRuntimeAssetManager::GetFileSystemPath(const FAssetMetadata& Metadata) const
	{
		return std::filesystem::absolute(LProject::GetAssetDirectory() / Metadata.FilePath);
	}

	std::filesystem::path LRuntimeAssetManager::GetFileSystemPath(const FAssetHandle Handle) const
	{
		return GetFileSystemPath(GetMetadata(Handle));
	}

	std::filesystem::path LRuntimeAssetManager::GetRelativePath(const std::filesystem::path& FilePath) const
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

	EAssetType LRuntimeAssetManager::GetAssetTypeFromExtension(const std::string& InExtension) const
	{
		const std::string Extension = StringUtils::ToLower(InExtension);
		if (AssetExtensionMap.find(Extension) == AssetExtensionMap.end())
		{
			return EAssetType::None;
		}

		return AssetExtensionMap.at(Extension.c_str());
	}

	EAssetType LRuntimeAssetManager::GetAssetTypeFromPath(const std::filesystem::path& Path) const
	{
		return GetAssetTypeFromExtension(Path.extension().string());
	}

	std::unordered_set<FAssetHandle> LRuntimeAssetManager::GetAllAssetsWithType(const EAssetType AssetType)
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

	std::size_t LRuntimeAssetManager::GetAllAssetsWithType(const EAssetType AssetType, std::unordered_set<FAssetHandle>& AssetsOfType)
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

	int LRuntimeAssetManager::GetTextures2D(std::vector<TObjectPtr<LTexture2D>>& TextureContainer)
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

	int LRuntimeAssetManager::GetTextures2D(std::vector<std::pair<std::string, TObjectPtr<LTexture2D>>>& TextureContainer)
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

	const std::vector<std::pair<std::string, TObjectPtr<LTexture2D>>>& LRuntimeAssetManager::GetTextures2D() const
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

	void LRuntimeAssetManager::LoadBaseMaterials()
	{
		/* TODO: Determine if the static lifetime of the asset is required here or not. */
		/* Base Material 2 */
		{
			static const std::string TextureName = "WoodContainer";

			static TObjectPtr<LMaterialAsset> BaseMaterialAsset = LEditorAssetManager::BaseMaterial;
			LK_CORE_VERIFY(BaseMaterialAsset);
			TObjectPtr<LMaterial> BaseMaterial = BaseMaterialAsset->GetMaterial();
			LK_CORE_VERIFY(BaseMaterial, "BaseMaterial is not valid");

			/* TODO: Fix the retrieval of the texture here, should not be a raw string. */
			//TObjectPtr<LTexture2D> Texture = TextureLibrary.GetTexture(TextureName);
			LK_CORE_ASSERT(Texture2DMap.contains(TextureName));
			TObjectPtr<LTexture2D> Texture = Texture2DMap.at(TextureName);
			LK_CORE_VERIFY(Texture);
			BaseMaterial->SetTexture(Texture.As<LTexture>());

			/* TODO: Keep the base material in memory only, don't serialize it. */
			FAssetMetadata BaseMatAssetMetadata;
			BaseMatAssetMetadata.Handle = BaseMaterialAsset->Handle;
			BaseMatAssetMetadata.Type = EAssetType::Material;
			BaseMatAssetMetadata.bIsDataLoaded = true;
			BaseMatAssetMetadata.bIsMemoryAsset = true;
			AssetRegistry[BaseMaterialAsset->Handle] = BaseMatAssetMetadata;
			MemoryAssets[BaseMaterialAsset->Handle] = BaseMaterialAsset;
		}

		/* Base Material 2 */
		{
			static const std::string TextureName = "Metal";

			/* TODO: Determine if the static lifetime of the asset is required here or not. */

			static TObjectPtr<LMaterialAsset> BaseMaterialAsset2 = LEditorAssetManager::BaseMaterial2;

			TObjectPtr<LMaterial> BaseMaterial2 = BaseMaterialAsset2->GetMaterial();
			LK_CORE_VERIFY(BaseMaterial2, "BaseMaterial2 is not valid");

			/* TODO: Fix the retrieval of the texture here, should not be a raw string. */
			LK_CORE_ASSERT(Texture2DMap.contains(TextureName));
			TObjectPtr<LTexture2D> Texture = Texture2DMap.at(TextureName);
			LK_CORE_VERIFY(Texture);
			BaseMaterial2->SetTexture(Texture.As<LTexture>());

			/* TODO: Keep the base material in memory only, don't serialize it. */
			FAssetMetadata MatAssetMetadata;
			MatAssetMetadata.Handle = BaseMaterialAsset2->Handle;
			MatAssetMetadata.Type = EAssetType::Material;
			MatAssetMetadata.bIsDataLoaded = true;
			MatAssetMetadata.bIsMemoryAsset = true;
			AssetRegistry[BaseMaterialAsset2->Handle] = MatAssetMetadata;
			MemoryAssets[BaseMaterialAsset2->Handle] = BaseMaterialAsset2;
		}
	}

	void LRuntimeAssetManager::LoadPrimitiveShapes()
    {
	#if defined(LK_ENGINE_EDITOR)
		LK_CORE_ASSERT(false, "RuntimeAssetManager is running alongside EditorAssetManager");
	#endif
		LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Loading primitive shapes");
		LK_CORE_ASSERT(LScene::GetActiveScene(), "No active scene");

		LoadBaseMaterials();

		if (TObjectPtr<LScene> CurrentScene = LScene::GetActiveScene())
		{
			/* Mesh: Cube. */
			{
				TObjectPtr<LMesh> CubeMesh = ImportAsset<LMesh>("Meshes/Default/Cube.lmesh");
				LK_CORE_VERIFY(CubeMesh && CubeMesh->GetMaterialTable() && CubeMesh->GetMaterialTable()->HasMaterial(0), "Import of Cube.lmesh failed");

				LK_CORE_INFO_TAG("RuntimeAssetManager", "Getting cube mesh asset: {}", CubeMesh->Handle);
				TObjectPtr<LAsset> CubeMeshAsset = GetAsset(CubeMesh->Handle);
				LK_CORE_VERIFY(CubeMeshAsset, "CubeMeshAsset not valid");

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
				LK_CORE_INFO("Cube Vertices: {}", CubeSource->GetVertices().size());
				LK_CORE_INFO("Cube Indices:  {}", CubeSource->IndexBuffer->GetCount());
			#endif
			}

			/* StaticMesh: Cube. */
			{
				TObjectPtr<LStaticMesh> StaticCubeMesh = ImportAsset<LStaticMesh>("Meshes/Default/Cube.lsmesh");
				LK_CORE_VERIFY(StaticCubeMesh && StaticCubeMesh->GetMaterialTable(), "Import of Cube.lsmesh failed");

				LK_CORE_INFO_TAG("RuntimeAssetManager", "Getting static cube mesh asset: {}", StaticCubeMesh->Handle);
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
				LK_CORE_INFO("Static Cube Vertices: {}", CubeSource->GetVertices().size());
				LK_CORE_INFO("Static Cube Indices:  {}", CubeSource->IndexBuffer->GetCount());
			#endif
			}

			/* StaticMesh: Plane. */
			{
				TObjectPtr<LStaticMesh> StaticPlaneMesh = ImportAsset<LStaticMesh>("Meshes/Default/Plane.lsmesh");
				LK_CORE_VERIFY(StaticPlaneMesh && StaticPlaneMesh->GetMaterialTable(), "Import of Plane.lsmesh failed");

				if (!StaticPlaneMesh->GetMaterialTable()->HasMaterial(0))
				{
					StaticPlaneMesh->GetMaterialTable()->SetMaterial(0, LEditorAssetManager::BaseMaterial2->Handle);
				}

				LK_CORE_INFO_TAG("RuntimeAssetManager", "Getting static plane mesh asset: {}", StaticPlaneMesh->Handle);
				TObjectPtr<LAsset> PlaneMeshAsset = GetAsset(StaticPlaneMesh->Handle);
				LK_CORE_VERIFY(PlaneMeshAsset, "PlaneMeshAsset not valid");

				/* TODO: The application of the shader layout should be automized somehow. */
				/* Setup the vertexbuffer layout. */
				TObjectPtr<LMeshSource> PlaneSource = StaticPlaneMesh->GetMeshSource();
				PlaneSource->VertexBuffer->SetLayout({
					{ "a_Position",   EShaderDataType::Float3 },
					{ "a_Normal",     EShaderDataType::Float3 },
				#if 0
					{ "a_Binormal",   EShaderDataType::Float3 },
					{ "a_Tangent",    EShaderDataType::Float3 },
				#endif
					{ "a_TexCoord",   EShaderDataType::Float2 },
				});
				PlaneSource->VertexBuffer->SetIndexBuffer(PlaneSource->IndexBuffer);

			#if LK_DEBUG_ASSET_LOG
				LK_CORE_INFO("Static Plane Mesh: {}", StaticPlaneMesh->Handle);
				LK_CORE_INFO("Static Plane Mesh Source: {} ({})", PlaneSource->Handle, PlaneSource->GetFilePath().string());
				LK_CORE_INFO("Static Plane Vertices:    {}", PlaneSource->GetVertices().size());
				LK_CORE_INFO("Static Plane IndexBuffer: {}", PlaneSource->IndexBuffer->GetCount());
				PlaneSource->DumpVertexBuffer();
			#endif
			}

		}
    }

}

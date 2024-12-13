#include "LKpch.h"
#include "RuntimeAssetManager.h"

#include "AssetManager.h"
#include "MeshSerializer.h"

#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Core/Utility/StringUtils.h"
#include "LkEngine/Project/Project.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Renderer/Renderer.h"

namespace LkEngine {

	static FAssetMetadata NullMetadata{};

	/** 
	 * FRuntimeCache 
	 *
	 *  Cached and loaded assets.
	 */
	struct FRuntimeCache
	{
		std::vector<TTexture2DPair> Textures2D{};
	};

	static FRuntimeCache RuntimeCache{}; /* TODO: REMOVE */

	LRuntimeAssetManager::LRuntimeAssetManager()
		: TextureLibrary(LTextureLibrary::Get())
	{
		LOBJECT_REGISTER();
	}

	LRuntimeAssetManager::~LRuntimeAssetManager()
	{
		if (Flags & EObjectFlag::BeginDestroy)
		{
			Destroy();
		}
	}

	void LRuntimeAssetManager::Initialize()
	{
		LObject::Initialize();

		LoadAssetRegistry();

		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loading materials");
		LoadMaterials();

		/* Load all primitive shapes. */
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loading primitive shapes");
		LoadPrimitiveShapes();

		/* Load textures into cache. */
		TextureLibrary.GetTextures2D(RuntimeCache.Textures2D);
	}

	void LRuntimeAssetManager::Destroy()
	{
		WriteRegistryToDisk();
	}

	template<typename T>
	TObjectPtr<T> LRuntimeAssetManager::ImportAsset(const std::filesystem::path& InFilePath)
	{
		LK_UNUSED(InFilePath);
	}
	
	template<>
    TObjectPtr<LMesh> LRuntimeAssetManager::ImportAsset<LMesh>(const std::filesystem::path& InFilePath)
    {
		for (const auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.FilePath == InFilePath)
			{
				LK_CORE_INFO("Found {} asset with filepath '{}', returning it", 
							 Enum::ToString(Metadata.Type), InFilePath.string());
				//LK_CORE_VERIFY(LoadedAssets[Metadata.Handle], "{} asset '{}' is not loaded to memory", Enum::ToString(Metadata.Type), Metadata.Handle);
				/* Load the asset if not loaded in memory. */
				if (!LoadedAssets[Metadata.Handle])
				{
				}
				return LoadedAssets[Metadata.Handle];
			}
		}

		LK_CORE_INFO_TAG("RuntimeAssetManager", "Importing asset from: {}", InFilePath.string());
        LAssimpMeshImporter MeshImporter(InFilePath);
		if (!MeshImporter.IsValid())
		{
			LK_CORE_ERROR("Mesh importer is not valid for: '{}'", InFilePath);
			return nullptr;
		}

        TObjectPtr<LMeshSource> MeshSource = MeshImporter.ImportToMeshSource();
		TObjectPtr<LMesh> Mesh = TObjectPtr<LMesh>::Create(MeshSource);

		LoadedAssets[Mesh->Handle] = Mesh;

		FAssetMetadata Metadata;
		Metadata.Handle = Mesh->Handle;
		Metadata.Type = EAssetType::MeshSource;
		Metadata.FilePath = InFilePath;
		Metadata.bIsMemoryAsset = false;
		AssetRegistry[Mesh->Handle] = Metadata;
		LK_CORE_INFO_TAG("RuntimeAssetManager", "Loaded mesh source '{}', {} vertices and {} indices", 
						 Mesh->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());

		return LoadedAssets[Mesh->Handle];
    }


	void LRuntimeAssetManager::AddMemoryOnlyAsset(TObjectPtr<LAsset> asset)
	{
		MemoryAssets[asset->Handle] = asset;
	}

	bool LRuntimeAssetManager::ReloadData(FAssetHandle AssetHandle)
	{
		LK_CORE_ASSERT(false, "Not implemented");
		return true;
	}

	bool LRuntimeAssetManager::IsAssetHandleValid(FAssetHandle AssetHandle) const
	{
		if (AssetHandle <= 0)
		{
			return false;
		}

		return IsMemoryAsset(AssetHandle);
	}

	std::filesystem::path LRuntimeAssetManager::GetFileSystemPath(const FAssetMetadata& Metadata)
	{
		//return LProject::GetAssetDirectory() / Metadata.FilePath;
		return std::filesystem::absolute(LProject::GetAssetDirectory() / Metadata.FilePath);
	}

	std::filesystem::path LRuntimeAssetManager::GetFileSystemPath(const FAssetHandle Handle)
	{
		return GetFileSystemPath(GetMetadata(Handle));
	}

	EAssetType LRuntimeAssetManager::GetAssetType(FAssetHandle AssetHandle)
	{
		if (TObjectPtr<LAsset> Asset = GetAsset(AssetHandle))
		{
			return Asset->GetAssetType();
		}

		return EAssetType::None;
	}

	TObjectPtr<LAsset> LRuntimeAssetManager::GetAsset(const FAssetHandle AssetHandle)
    {
		if (IsMemoryAsset(AssetHandle))
		{
			return MemoryAssets[AssetHandle];
		}

		return LoadedAssets[AssetHandle];
	#if 0
		FAssetMetadata& Metadata = GetMetadataInternal(AssetHandle);
		if (!Metadata.IsValid())
		{
			LK_CORE_WARN_TAG("RuntimeAssetManager", "Metadata is invalid for {}", AssetHandle);
			return nullptr;
		}

		TObjectPtr<LAsset> Asset = nullptr;
		if (!Metadata.IsDataLoaded)
		{
			if (!Metadata.IsDataLoaded)
			{
				return nullptr;
			}

			LoadedAssets[AssetHandle] = Asset;
		}
		else
		{
			Asset = LoadedAssets[AssetHandle];
		}

		return Asset;
	#endif
    }

	void LRuntimeAssetManager::LoadAssetRegistry()
	{
		LK_CORE_WARN("Loading asset registry");

		const std::filesystem::path& AssetRegistryPath = LProject::GetAssetRegistryPath();
		if (!LFileSystem::Exists(AssetRegistryPath))
		{
			LK_CORE_ERROR_TAG("RuntimeAssetManager", "Asset registry file does not exist at: '{}'", AssetRegistryPath.string());
			return;
		}

		std::ifstream InputStream(AssetRegistryPath);
		LK_CORE_VERIFY(InputStream.is_open(), "Inputstream is not OK");
		std::stringstream StringStream;
		StringStream << InputStream.rdbuf();

		YAML::Node Data = YAML::Load(StringStream.str());
		YAML::Node AssetHandles = Data["Assets"];
		if (!AssetHandles)
		{
			LK_CORE_VERIFY(false, "Asset registry failed to load");
			return;
		}

		for (auto Entry : AssetHandles)
		{
			const std::string Filepath = Entry["FilePath"].as<std::string>();

			FAssetMetadata Metadata;
			Metadata.Handle = Entry["Handle"].as<uint64_t>();
			Metadata.FilePath = Filepath;
			Metadata.Type = static_cast<EAssetType>(Enum::FromString(Entry["Type"].as<std::string>()));

			if (Metadata.Type == EAssetType::None)
			{
				continue;
			}

			if (Metadata.Type != GetAssetTypeFromPath(Filepath))
			{
				LK_CORE_WARN_TAG("AssetManager", "Mismatch between stored EAssetType and extension type");
				Metadata.Type = GetAssetTypeFromPath(Filepath);
			}

			if (Metadata.Handle == 0)
			{
				LK_CORE_WARN_TAG("AssetManager", "AssetHandle for {} is 0", Metadata.FilePath);
				continue;
			}

			LK_CORE_DEBUG("Handle: {}   Type: {}   FilePath: {}", Metadata.Handle, Enum::ToString(Metadata.Type), Metadata.FilePath);
			AssetRegistry[Metadata.Handle] = Metadata;
		}
	}

	void LRuntimeAssetManager::WriteRegistryToDisk()
	{
		/* Sort assets by their UUID. */
		struct AssetRegistryEntry
		{
			std::string FilePath;
			EAssetType Type;
		};

		/// TODO: Need to make sure the entries are not overwritten in the registry file.
		std::map<UUID, AssetRegistryEntry> SortedMap;
		for (auto& [Filepath, Metadata] : AssetRegistry)
		{
			if (Metadata.bIsMemoryAsset)
			{
				continue;
			}

			if (Metadata.Type == EAssetType::None)
			{
				LK_CORE_WARN_TAG("RuntimeAssetManager", "Type for '{}' is None, skipping it", Metadata.Handle);
				continue;
			}

			std::string PathToSerialize = Metadata.FilePath.string();
			std::replace(PathToSerialize.begin(), PathToSerialize.end(), '\\', '/');
			SortedMap[Metadata.Handle] = { PathToSerialize, Metadata.Type };
		}

		LK_CORE_INFO_TAG("AssetManager", "Serializing asset registry with {} entries", SortedMap.size());

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
			LK_CORE_DEBUG("Handle: {}   Type: {}   FilePath: {}", Handle, Enum::ToString(Entry.Type), Entry.FilePath);
		}
		Out << YAML::EndSeq;
		Out << YAML::EndMap;

		const std::string& AssetRegistryPath = LProject::GetAssetRegistryPath().string();
		LK_CORE_DEBUG("Saving to asset registry -> '{}'", AssetRegistryPath);
		std::ofstream FileOut(AssetRegistryPath);
		FileOut << Out.c_str();
	}

	const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const FAssetHandle Handle)
	{
		if (AssetRegistry.Contains(Handle))
		{
			return AssetRegistry[Handle];
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

    FAssetHandle LRuntimeAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& FilePath)
	{
		return GetMetadata(FilePath).Handle;
	}

    const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const TObjectPtr<LAsset>& Asset)
	{
		return GetMetadata(Asset->Handle);
	}

	const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const std::filesystem::path& FilePath)
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

	std::filesystem::path LRuntimeAssetManager::GetRelativePath(const std::filesystem::path& FilePath)
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

		/* Return rvalue so we trigger move semantics for performance. */
		//return FilePath.lexically_normal();
	}

	EAssetType LRuntimeAssetManager::GetAssetTypeFromExtension(const std::string& InExtension)
	{
		/* FIXME: Write test for string utils functions - verify that the result string is converted correctly. */
		const std::string Extension = StringUtils::ToLower(InExtension);
		if (AssetExtensionMap.find(Extension) == AssetExtensionMap.end())
		{
			return EAssetType::None;
		}

		return AssetExtensionMap.at(Extension.c_str());
	}

	EAssetType LRuntimeAssetManager::GetAssetTypeFromPath(const std::filesystem::path& Path)
	{
		return GetAssetTypeFromExtension(Path.extension().string());
	}

	const std::vector<TTexture2DPair>& LRuntimeAssetManager::GetTextures2D() const
	{
		return RuntimeCache.Textures2D;
	}

	int LRuntimeAssetManager::GetTextures2D(std::vector<TTexture2DPair>& TextureContainer)
	{
		return TextureLibrary.GetTextures2D(TextureContainer);
	}

	void LRuntimeAssetManager::LoadMaterials()
	{
		LMaterialLibrary& MaterialLibrary = LMaterialLibrary::Get();

		// Asphalt
		{
		#if 0 
			TObjectPtr<LMaterial> AsphaltMaterial = MaterialLibrary.GetMaterial("Basic_Asphalt");
			AsphaltMaterial->SetTexture(TextureLibrary.GetWhiteTexture());
			TObjectPtr<LMaterialAsset> AsphaltMaterialAsset = TObjectPtr<LMaterialAsset>::Create(AsphaltMaterial);
			MemoryAssets[AsphaltMaterialAsset->Handle] = AsphaltMaterialAsset;

			FAssetMetadata Metadata;
			Metadata.Handle = AsphaltMaterialAsset->Handle;
			Metadata.Type = EAssetType::Material;
			AssetRegistry[AsphaltMaterialAsset->Handle] = Metadata;
		#endif
		}

		// Rubber
		{
		#if 0 
			TObjectPtr<LMaterial> RubberMaterial = MaterialLibrary.GetMaterial("Basic_Rubber");
			RubberMaterial->SetTexture(TextureLibrary.GetWhiteTexture());

			TObjectPtr<LMaterialAsset> RubberMaterialAsset = TObjectPtr<LMaterialAsset>::Create(RubberMaterial);
			MemoryAssets[RubberMaterialAsset->Handle] = RubberMaterialAsset;

			FAssetMetadata Metadata;
			Metadata.Handle = RubberMaterialAsset->Handle;
			Metadata.Type = EAssetType::Material;
			AssetRegistry[RubberMaterialAsset->Handle] = Metadata;
		#endif
		}
	}

    void LRuntimeAssetManager::LoadPrimitiveShapes()
    {
		LK_CORE_INFO_TAG("RuntimeAssetManager", "Creating primitive shapes");

		if (TObjectPtr<LScene> CurrentScene = LScene::GetActiveScene())
		{
			LK_CORE_INFO("Loading Cube.gltf");
			ImportAsset<LMesh>("Assets/Meshes/Cube.gltf");
			LK_CORE_INFO("Loading Cone.gltf");
			ImportAsset<LMesh>("Assets/Meshes/Cone.gltf");
		#if 0
			/* Create cube. */
			std::filesystem::path FilePath("Assets/Meshes/Cube.gltf");
			LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Importing cube from: '{}'", FilePath.string());
			LEntity CubeEntity = CurrentScene->CreateEntity("DebugCube");
			//TObjectPtr<LMesh> Cube = ImportAsset<LMesh>(FilePath.string());
			TObjectPtr<LMesh> Cube = ImportAsset<LMesh>(FilePath.string());
			LMeshComponent& MeshComponent = CubeEntity.AddComponent<LMeshComponent>(Cube->Handle);

			/* Setup the vertexbuffer layout. */
			TObjectPtr<LMeshSource> CubeSource = Cube->GetMeshSource();
			CubeSource->VertexBuffer->SetLayout({
			    { "a_Position",   EShaderDataType::Float3 },
			    { "a_Normal",     EShaderDataType::Float3 },
			    { "a_Binormal",   EShaderDataType::Float3 },
			    { "a_Tangent",    EShaderDataType::Float3 },
			    { "a_TexCoord",   EShaderDataType::Float2 },
			});
			CubeSource->VertexBuffer->SetIndexBuffer(CubeSource->IndexBuffer);

			/* Cube Metadata in the asset registry. */
			FAssetMetadata CubeMetadata;
			CubeMetadata.Handle = Cube->Handle;
			CubeMetadata.Type = EAssetType::Mesh;
			CubeMetadata.FilePath = FilePath;
			CubeMetadata.bIsDataLoaded = true;
			AssetRegistry[Cube->Handle] = CubeMetadata;

			/* Assign the Cube with the 'BaseMaterial' */
			LTextureLibrary& TextureLibrary = LTextureLibrary::Get();

			TObjectPtr<LMaterialAsset> BaseMaterialAsset = LMaterialLibrary::Get().GetMaterial(BASE_MATERIAL);
			TObjectPtr<LMaterial> BaseMaterial = BaseMaterialAsset->GetMaterial();
			LK_CORE_VERIFY(BaseMaterial, "BaseMaterial is not valid");

			/* TODO: Fix the retrieval of the texture here, should not be a raw string. */
            TObjectPtr<LTexture2D> WoodTexture = TextureLibrary.GetTexture("wood-container_512x512");
			LK_CORE_VERIFY(WoodTexture);
			BaseMaterial->SetTexture(WoodTexture.As<LTexture>());

			//TObjectPtr<LMaterialAsset> BaseMaterialAsset = TObjectPtr<LMaterialAsset>::Create(BaseMaterial);

			Cube->Materials->SetMaterial(0, BaseMaterialAsset->Handle);
			LK_CORE_VERIFY(BaseMaterial->GetTexture(""), "BaseMaterial texture is not valid");

			/* Base material Metadata in the asset registry. */
			FAssetMetadata BaseMaterialAssetMetadata;
			BaseMaterialAssetMetadata.Handle = BaseMaterialAsset->Handle;
			BaseMaterialAssetMetadata.Type = EAssetType::Material;
			BaseMaterialAssetMetadata.bIsDataLoaded = true;
			AssetRegistry[BaseMaterialAsset->Handle] = BaseMaterialAssetMetadata;

			MemoryAssets[Cube->Handle] = Cube;
			MemoryAssets[BaseMaterialAsset->Handle] = BaseMaterialAsset;
			LK_FATAL_TAG("RuntimeAssetManager", "LOAD PRIMITIVE TYPES   AssetRegistry: {}", AssetRegistry.Count());

			DebugCube = Cube;

			LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Cube created with Handle {}", Cube->Handle);
		#endif
		}
		else
		{
			LK_CORE_WARN_TAG("RuntimeAssetManager", "No active scene");
			LK_CORE_ASSERT(false);
		}
			
    }

}

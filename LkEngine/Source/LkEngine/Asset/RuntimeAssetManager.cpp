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

	static FAssetMetadata NullMetadata{};

	/** 
	 * FRuntimeCache 
	 *
	 *  Cached and loaded Assets.
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

		LAssetImporter::Initialize();
		LoadAssetRegistry();

		LoadPrimitiveShapes();

		/* Load textures into cache. */
		TextureLibrary.GetTextures2D(RuntimeCache.Textures2D); /// FIXME:
	}

	void LRuntimeAssetManager::Destroy()
	{
		WriteRegistryToDisk();
	}

	LUUID LRuntimeAssetManager::ImportAsset(const std::filesystem::path& Filepath)
	{
		fs::path Path = GetRelativePath(Filepath);

		if (const FAssetMetadata& Metadata = GetMetadata(Path); Metadata.IsValid())
		{
			return Metadata.Handle;
		}

		const EAssetType AssetType = GetAssetTypeFromPath(Path);
		if (AssetType == EAssetType::None)
		{
			return 0;
		}

		FAssetMetadata Metadata;
		Metadata.Handle = LUUID();
		Metadata.FilePath = Path;
		Metadata.Type = AssetType;
		AssetRegistry[Metadata.Handle] = Metadata;

		return Metadata.Handle;
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
		TObjectPtr<LMesh> Mesh = TObjectPtr<LMesh>::Create(MeshSource);

		LoadedAssets[Mesh->Handle] = Mesh;

		FAssetMetadata Metadata;
		Metadata.Handle = Mesh->Handle;
		Metadata.Type = EAssetType::MeshSource;
		Metadata.FilePath = InFilePath;
		Metadata.bIsMemoryAsset = false;
		Metadata.bIsDataLoaded = true;
		AssetRegistry[Mesh->Handle] = Metadata;
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loaded mesh source '{}', {} vertices and {} indices", Mesh->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());

		return LoadedAssets[Mesh->Handle];
    }

	template<>
    TObjectPtr<LMesh> LRuntimeAssetManager::ImportAsset<LMesh>(const std::filesystem::path& InFilePath)
    {
		for (const auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.FilePath == InFilePath)
			{
				if (!Metadata.bIsDataLoaded)
				{
					if (!ReloadData(Handle))
					{
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

		const LUUID MeshSourceHandle = static_cast<LUUID>(MeshNode["MeshSource"].as<uint64_t>());
		TObjectPtr<LMeshSource> MeshSource = GetAsset(MeshSourceHandle).As<LMeshSource>();
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
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loaded mesh '{}', {} vertices and {} indices", Mesh->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());
		LK_CORE_VERIFY(Mesh->GetMaterialTable(), "Mesh '{}' ({}) is missing its MaterialTable", Mesh->Handle, Metadata.FilePath.string());

		return LoadedAssets[Mesh->Handle];
    }

	void LRuntimeAssetManager::AddMemoryOnlyAsset(TObjectPtr<LAsset> Asset)
	{
		MemoryAssets[Asset->Handle] = Asset;
	}

	bool LRuntimeAssetManager::ReloadData(const LUUID AssetHandle)
	{
		FAssetMetadata& Metadata = GetMetadataInternal(AssetHandle);
		if (!Metadata.IsValid())
		{
			LK_CORE_ERROR("Cannot reload invalid Asset");
			return false;
		}

		TObjectPtr<LAsset> Asset;
		Metadata.bIsDataLoaded = LAssetImporter::TryLoadData(Metadata, Asset);
		if (Metadata.bIsDataLoaded)
		{
			LoadedAssets[AssetHandle] = Asset;
		}

		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Data loaded for '{}': {}", AssetHandle, (Metadata.bIsDataLoaded ? "Yes" : "No"));

		return Metadata.bIsDataLoaded;
	}

	bool LRuntimeAssetManager::IsAssetHandleValid(const LUUID AssetHandle) const
	{
		if (AssetHandle <= 0)
		{
			return false;
		}

		return IsMemoryAsset(AssetHandle);
	}

	bool LRuntimeAssetManager::IsAssetLoaded(const LUUID Handle) const
	{
		return LoadedAssets.contains(Handle);
	}

	std::filesystem::path LRuntimeAssetManager::GetFileSystemPath(const FAssetMetadata& Metadata)
	{
		return std::filesystem::absolute(LProject::GetAssetDirectory() / Metadata.FilePath);
	}

	std::filesystem::path LRuntimeAssetManager::GetFileSystemPath(const LUUID Handle)
	{
		return GetFileSystemPath(GetMetadata(Handle));
	}

	EAssetType LRuntimeAssetManager::GetAssetType(LUUID AssetHandle)
	{
		if (TObjectPtr<LAsset> Asset = GetAsset(AssetHandle))
		{
			return Asset->GetAssetType();
		}

		return EAssetType::None;
	}

	TObjectPtr<LAsset> LRuntimeAssetManager::GetAsset(const LUUID AssetHandle)
    {
		if (IsMemoryAsset(AssetHandle))
		{
			return MemoryAssets[AssetHandle];
		}

		FAssetMetadata& Metadata = GetMetadataInternal(AssetHandle);
		if (!Metadata.IsValid())
		{
			LK_CORE_WARN_TAG("RuntimeAssetManager", "Metadata is invalid for '{}'\n{}", AssetHandle, Metadata.ToString());
			return nullptr;
		}

		TObjectPtr<LAsset> Asset = nullptr;
		if (!Metadata.bIsDataLoaded)
		{
			Metadata.bIsDataLoaded = LAssetImporter::TryLoadData(Metadata, Asset);
			if (!Metadata.bIsDataLoaded)
			{
				return nullptr;
			}

			LoadedAssets[AssetHandle] = Asset;
		}
		else
		{
			Asset = LoadedAssets[AssetHandle];
			LK_CORE_VERIFY(Asset, "Asset '{}' is nullptr", AssetHandle);
		}

		return Asset;
    }

	void LRuntimeAssetManager::LoadAssetRegistry()
	{
		const std::filesystem::path& AssetRegistryPath = LProject::GetAssetRegistryPath();
		LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Loading asset registry ({})", AssetRegistryPath.string());
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
				LK_CORE_ERROR_TAG("RuntimeAssetManager", "Invalid asset type for '{}'", Metadata.FilePath);
				LK_CORE_WARN("Asset type is set to none\n{}", Metadata.ToString());
				continue;
			}

			if (Metadata.Type != GetAssetTypeFromPath(Filepath))
			{
				LK_CORE_WARN_TAG("AssetManager", "Mismatch between stored type and extension type for '{}' ({})\n{}", 
								 Metadata.Handle, Metadata.FilePath.string(), Metadata.ToString());
				Metadata.Type = GetAssetTypeFromPath(Filepath);
			}

			if (Metadata.Handle == 0)
			{
				LK_CORE_WARN_TAG("AssetManager", "AssetHandle for {} is 0", Metadata.FilePath);
				continue;
			}

			LK_CORE_TRACE_TAG("RuntimeAssetManager", "Handle='{}'  Type='{}'  FilePath='{}'", Metadata.Handle, Enum::ToString(Metadata.Type), Metadata.FilePath);
			AssetRegistry[Metadata.Handle] = Metadata;
		}
	}

	void LRuntimeAssetManager::WriteRegistryToDisk()
	{
		/* Sort Assets by their UUID. */
		struct AssetRegistryEntry
		{
			std::string FilePath;
			EAssetType Type;
		};

		std::map<LUUID, AssetRegistryEntry> SortedMap;
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

		LK_CORE_TRACE_TAG("AssetManager", "Serializing asset registry with {} entries", SortedMap.size());
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
			LK_CORE_TRACE("Handle: {}   Type: {}   FilePath: {}", Handle, Enum::ToString(Entry.Type), Entry.FilePath);
		}
		Out << YAML::EndSeq;
		Out << YAML::EndMap;

		const std::string& AssetRegistryPath = LProject::GetAssetRegistryPath().string();
		std::ofstream FileOut(AssetRegistryPath);
		FileOut << Out.c_str();
	}

	const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const LUUID Handle)
	{
		if (AssetRegistry.Contains(Handle))
		{
			return AssetRegistry[Handle];
		}

		return NullMetadata;
	}

    FAssetMetadata& LRuntimeAssetManager::GetMetadataInternal(const LUUID Handle)
	{
		if (AssetRegistry.Contains(Handle))
		{
	    	return AssetRegistry[Handle];
		}

	    return NullMetadata; 
	}

    LUUID LRuntimeAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& FilePath)
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
	}

	EAssetType LRuntimeAssetManager::GetAssetTypeFromExtension(const std::string& InExtension)
	{
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

    void LRuntimeAssetManager::LoadPrimitiveShapes()
    {
		LK_CORE_ASSERT(LScene::GetActiveScene(), "No active scene");
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Creating primitive shapes");
		if (TObjectPtr<LScene> CurrentScene = LScene::GetActiveScene())
		{
			/* Load the base material. */
			{
				/// FIXME
				static TObjectPtr<LMaterialAsset> BaseMaterialAsset = LMaterialLibrary::Get().GetMaterial(BASE_MATERIAL);
				TObjectPtr<LMaterial> BaseMaterial = BaseMaterialAsset->GetMaterial();
				LK_CORE_VERIFY(BaseMaterial, "BaseMaterial is not valid");

				/* TODO: Fix the retrieval of the texture here, should not be a raw string. */
				TObjectPtr<LTexture2D> WoodTexture = TextureLibrary.GetTexture("wood-container_512x512");
				LK_CORE_VERIFY(WoodTexture);
				BaseMaterial->SetTexture(WoodTexture.As<LTexture>());

				/* TODO: Keep the base material in memory only, don't serialize it. */
				FAssetMetadata BaseMatAssetMetadata;
				BaseMatAssetMetadata.Handle = BaseMaterialAsset->Handle;
				BaseMatAssetMetadata.Type = EAssetType::Material;
				BaseMatAssetMetadata.bIsDataLoaded = true;
				BaseMatAssetMetadata.bIsMemoryAsset = true;
				AssetRegistry[BaseMaterialAsset->Handle] = BaseMatAssetMetadata;
				MemoryAssets[BaseMaterialAsset->Handle] = BaseMaterialAsset;
			}

			/* Cube mesh. */
			{
				TObjectPtr<LMesh> CubeMesh = ImportAsset<LMesh>("Meshes/Default/Cube.lmesh");
				LK_CORE_VERIFY(CubeMesh && CubeMesh->GetMaterialTable() && CubeMesh->GetMaterialTable()->HasMaterial(0), "Import of Cube.lmesh failed");

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
			}

			/* Plane mesh. */
			{
				TObjectPtr<LMesh> PlaneMesh = ImportAsset<LMesh>("Meshes/Default/Plane.lmesh");
				LK_CORE_VERIFY(PlaneMesh && PlaneMesh->GetMaterialTable() && PlaneMesh->GetMaterialTable()->HasMaterial(0), "Import of Plane.lmesh failed");
			}
		}
    }

}

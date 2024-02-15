#include "LKpch.h"
#include "RuntimeAssetManager.h"

#include "AssetManager.h"

#include "LkEngine/Project/Project.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Renderer/Renderer.h"

namespace LkEngine {

    static AssetMetadata NullMetadata;

    RuntimeAssetManager::RuntimeAssetManager()
    {
    }

    RuntimeAssetManager::~RuntimeAssetManager()
    {
    }
	
	void RuntimeAssetManager::Init()
	{
		LoadMaterials();
		LoadPrimitiveShapes(); // Default shapes, cube etc
	}

	void RuntimeAssetManager::AddMemoryOnlyAsset(Ref<Asset> asset)
	{
		m_MemoryAssets[asset->Handle] = asset;
	}

	bool RuntimeAssetManager::ReloadData(AssetHandle assetHandle)
	{
		LK_CORE_ASSERT(false, "Not implemented");
		return true;
	}

	bool RuntimeAssetManager::IsAssetHandleValid(AssetHandle assetHandle)
	{
		if (assetHandle == 0)
			return false;

		return IsMemoryAsset(assetHandle);
	}

	AssetType RuntimeAssetManager::GetAssetType(AssetHandle assetHandle)
	{
		Ref<Asset> asset = GetAsset(assetHandle);
		if (!asset)
			return AssetType::None;

		return asset->GetAssetType();
	}

	template<typename T>
	Ref<T> RuntimeAssetManager::ImportAsset(std::filesystem::path filepath)
	{
	}
	
	template<>
    Ref<Mesh> RuntimeAssetManager::ImportAsset<Mesh>(std::filesystem::path filepath)
    {
        AssimpMeshImporter meshImporter = AssimpMeshImporter(filepath);
        Ref<MeshSource> source = meshImporter.ImportToMeshSource();
		Ref<Mesh> mesh = Ref<Mesh>::Create(source);

		m_LoadedAssets[mesh->Handle] = mesh;
		LK_CORE_INFO_TAG("RuntimeAssetManager", "Loaded mesh with {} vertices, {} indices and a handle of '{}'", source->GetVertices().size(), source->GetIndices().size(), mesh->Handle);

		return m_LoadedAssets[mesh->Handle];
    }

    Ref<Asset> RuntimeAssetManager::GetAsset(AssetHandle assetHandle)
    {
		if (IsMemoryAsset(assetHandle))
			return m_MemoryAssets[assetHandle];

		auto& metadata = GetMetadataInternal(assetHandle);
		if (!metadata.IsValid())
		{
			LK_CORE_WARN_TAG("RuntimeAssetManager", "Metadata is invalid for {}", assetHandle);
			return nullptr;
		}

		Ref<Asset> asset = nullptr;
		if (!metadata.IsDataLoaded)
		{
			if (!metadata.IsDataLoaded)
				return nullptr;

			m_LoadedAssets[assetHandle] = asset;
		}
		else
		{
			asset = m_LoadedAssets[assetHandle];
		}

		return asset;
    }

    const AssetMetadata& RuntimeAssetManager::GetMetadata(AssetHandle handle)
	{
		if (m_AssetRegistry.Contains(handle))
			return m_AssetRegistry[handle];

		return NullMetadata;
	}

    AssetMetadata& RuntimeAssetManager::GetMetadataInternal(AssetHandle handle)
	{
		if (m_AssetRegistry.Contains(handle))
	    	return m_AssetRegistry[handle];

	    return NullMetadata; 
	}

    AssetHandle RuntimeAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& filepath)
	{
		return GetMetadata(filepath).Handle;
	}

    const AssetMetadata& RuntimeAssetManager::GetMetadata(const Ref<Asset>& asset)
	{
		return GetMetadata(asset->Handle);
	}

	const AssetMetadata& RuntimeAssetManager::GetMetadata(const std::filesystem::path& filepath)
	{
		LK_CORE_ASSERT(m_AssetRegistry.Count() > 0, "AssetRegistry is empty!");
		const auto relativePath = GetRelativePath(filepath);
		for (auto& [handle, metadata] : m_AssetRegistry)
		{
			if (metadata.FilePath == relativePath)
				return metadata;
		}
		return NullMetadata;
	}

	std::filesystem::path RuntimeAssetManager::GetRelativePath(const std::filesystem::path& filepath)
	{
		std::filesystem::path relativePath = filepath.lexically_normal();
		std::string temp = filepath.string();
		LK_CORE_INFO("Returning relative filepath -> {}", relativePath.string());
		return relativePath;
	}

	void RuntimeAssetManager::WriteRegistryToFile()
	{
	}

	AssetType RuntimeAssetManager::GetAssetTypeFromExtension(const std::string& extension)
	{
		LK_CORE_ASSERT(false, "Not implemented yet!");
		return AssetType::None;
	}

	AssetType RuntimeAssetManager::GetAssetTypeFromPath(const std::filesystem::path& path)
	{
		LK_CORE_ASSERT(false, "Not implemented yet!");
		return GetAssetTypeFromExtension(path.extension().string());
	}

	void RuntimeAssetManager::LoadMaterials()
	{
		Ref<MaterialLibrary> materialLib = Renderer::GetMaterialLibrary();

		// Asphalt
		{
			Ref<Material> asphaltMat = materialLib->GetMaterial("Basic_Asphalt");
			asphaltMat->SetTexture(TextureLibrary::Get()->GetWhiteTexture());
			Ref<MaterialAsset> asphaltMatAsset = Ref<MaterialAsset>::Create(asphaltMat);
			m_MemoryAssets[asphaltMatAsset->Handle] = asphaltMatAsset;

			AssetMetadata metadata;
			metadata.Handle = asphaltMatAsset->Handle;
			metadata.Type = AssetType::Material;
			m_AssetRegistry[asphaltMatAsset->Handle] = metadata;
		}

		// Rubber
		{
			Ref<Material> rubberMat = materialLib->GetMaterial("Basic_Rubber");
			rubberMat->SetTexture(TextureLibrary::Get()->GetWhiteTexture());
			Ref<MaterialAsset> rubberMatAsset = Ref<MaterialAsset>::Create(rubberMat);
			m_MemoryAssets[rubberMatAsset->Handle] = rubberMatAsset;

			AssetMetadata metadata;
			metadata.Handle = rubberMatAsset->Handle;
			metadata.Type = AssetType::Material;
			m_AssetRegistry[rubberMatAsset->Handle] = metadata;
		}
	}

    void RuntimeAssetManager::LoadPrimitiveShapes()
    {
		LK_CORE_INFO_TAG("RuntimeAssetManager", "Creating primitive shapes");

		Ref<Project> project = Project::Current();
		Ref<TextureLibrary> TextureLib = Renderer::GetTextureLibrary();
		Ref<MaterialLibrary> MaterialLib = Renderer::GetMaterialLibrary();

		LK_CORE_WARN_TAG("RuntimeAssetManager", "Loading primitive shapes for project {}", project->GetName());
		// Cube
		{
			std::filesystem::path filepath("Assets/Meshes/Cube.gltf");
			Entity cubeEntity = Project::Current()->GetScene()->CreateEntity("Cube");
			Ref<Mesh> cube = ImportAsset<Mesh>(filepath.string());
			MeshComponent& meshComponent = cubeEntity.AddComponent<MeshComponent>(cube->Handle);

			// Assign the cube with the 'BaseMaterial'
			Ref<Material> baseMaterial = MaterialLib->GetBaseMaterial();
            baseMaterial->SetTexture(TextureLib->GetTexture("wood-container_512x512"));
            //baseMaterial->SetTexture(TextureLib->GetTexture("wood-container"));
            //baseMaterial->SetTexture(TextureLib->GetTexture("bricks"));
			Ref<MaterialAsset> baseMatAsset = Ref<MaterialAsset>::Create(baseMaterial);

			cube->m_Materials->SetMaterial(0, baseMatAsset->Handle);
			LK_CORE_ASSERT(TextureLib->GetTexture("wood-container"), "Texture wood-container is nullptr!");
			LK_CORE_ASSERT(baseMaterial->GetTexture(""), "BaseMaterial texture is nullptr");

			AssetMetadata cubeMetadata;
			cubeMetadata.Handle = cube->Handle;
			cubeMetadata.Type = AssetType::Mesh;
			cubeMetadata.FilePath = filepath;
			cubeMetadata.IsDataLoaded = true;
			m_AssetRegistry[cube->Handle] = cubeMetadata;

			AssetMetadata baseMaterialAssetMetadata;
			baseMaterialAssetMetadata.Handle = baseMatAsset->Handle;
			baseMaterialAssetMetadata.Type = AssetType::Material;
			baseMaterialAssetMetadata.IsDataLoaded = true;
			m_AssetRegistry[baseMatAsset->Handle] = baseMaterialAssetMetadata;

			Ref<MeshSource> cubeSource = cube->GetMeshSource();
			cubeSource->m_VertexBuffer->SetLayout({
			    { "a_Position",      ShaderDataType::Float3  },
			    { "a_Normal",        ShaderDataType::Float3  },
			    { "a_Binormal",      ShaderDataType::Float3  },
			    { "a_Tangent",       ShaderDataType::Float3  },
			    { "a_Texcoord",      ShaderDataType::Float2  },
			});
			cubeSource->m_VertexBuffer->SetIndexBuffer(cubeSource->m_IndexBuffer);

			m_MemoryAssets[cube->Handle] = cube;
			m_MemoryAssets[baseMatAsset->Handle] = baseMatAsset;

			// Test out the retrieval of the texture before continuing
			{
				AssetHandle cubeMaterialHandle = cube->GetMaterials()->GetMaterialHandle(0);
				Ref<Material> cubeMaterial = AssetManager::GetAsset<MaterialAsset>(cube->GetMaterials()->GetMaterialHandle(0))->GetMaterial();
				LK_CORE_VERIFY(cubeMaterial, "Retrieved material for CUBE is nullptr!");
			}

			LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Cube created with handle {}", cube->Handle);
		}
    }


}
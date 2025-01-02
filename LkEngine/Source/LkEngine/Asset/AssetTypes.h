#pragma once 

#include "LkEngine/Core/Core.h"


namespace LkEngine {

	/**
	 * AssetFlag enum.
	 * Allows bitwise logic without any casting.
	 */
	namespace AssetFlag
	{
		enum class Type : uint16_t
		{
			None       = 0 << 0,
			Missing    = 1 << 1,
			Invalid    = 1 << 2
		};
	}
	using EAssetFlag = AssetFlag::Type;

	/**
	 * EAssetType
	 * 
	 *  Type of asset.
	 */
	enum class EAssetType : uint16_t
	{
		None = 0,
		Scene,
		Prefab,
		Mesh,
		StaticMesh,
		MeshSource,
		Material,
		Texture,
		Font,
		Skeleton,
		Animation,
		EnvMap,
		Audio
	};


	namespace Enum 
	{
		FORCEINLINE static EAssetType FromString(std::string_view AssetType)
		{
			if (AssetType == "None")        return EAssetType::None;
			if (AssetType == "Scene")       return EAssetType::Scene;
			if (AssetType == "Prefab")      return EAssetType::Prefab;
			if (AssetType == "Mesh")        return EAssetType::Mesh;
			if (AssetType == "StaticMesh")  return EAssetType::StaticMesh;
			if (AssetType == "MeshSource")  return EAssetType::MeshSource;
			if (AssetType == "Material")    return EAssetType::Material;
			if (AssetType == "Texture")     return EAssetType::Texture;
			if (AssetType == "Font")        return EAssetType::Font;
			if (AssetType == "Skeleton")    return EAssetType::Skeleton;
			if (AssetType == "Animation")   return EAssetType::Animation;
			LK_CORE_ASSERT(false, "Enum::FromString failed for 'AssetType'");
			return EAssetType::None;
		}

		FORCEINLINE static constexpr const char* ToString(const EAssetType AssetType)
		{
			if (AssetType == EAssetType::None)        return "None";
			if (AssetType == EAssetType::Scene)       return "Scene";
			if (AssetType == EAssetType::Prefab)      return "Prefab";
			if (AssetType == EAssetType::Mesh)        return "Mesh";
			if (AssetType == EAssetType::StaticMesh)  return "StaticMesh";
			if (AssetType == EAssetType::MeshSource)  return "MeshSource";
			if (AssetType == EAssetType::Material)    return "Material";
			if (AssetType == EAssetType::Texture)     return "Texture";
			if (AssetType == EAssetType::Font)        return "Font";
			if (AssetType == EAssetType::Skeleton)    return "Skeleton";
			if (AssetType == EAssetType::Animation)   return "Animation";
			LK_CORE_ASSERT(false, "Enum::ToString failed for 'AssetType'");
			return "NULL";
		}

	}

	inline static std::unordered_map<std::string, EAssetType> AssetExtensionMap = 
	{
		/* LkEngine Filetypes */
		{ ".lscene",     EAssetType::Scene },
		{ ".lmesh",      EAssetType::Mesh },
		{ ".lsmesh",     EAssetType::StaticMesh },
		{ ".lmaterial",  EAssetType::Material },
		{ ".lskel",      EAssetType::Skeleton },
		{ ".lanim",      EAssetType::Animation },
		{ ".lprefab",    EAssetType::Prefab },

		/* Mesh / Animation */
		{ ".fbx",  EAssetType::MeshSource },
		{ ".gltf", EAssetType::MeshSource },
		{ ".glb",  EAssetType::MeshSource },
		{ ".obj",  EAssetType::MeshSource },

		/* Textures */
		{ ".png",  EAssetType::Texture },
		{ ".jpg",  EAssetType::Texture },
		{ ".jpeg", EAssetType::Texture },
		{ ".hdr",  EAssetType::EnvMap },

		/* Audio */
		{ ".wav", EAssetType::Audio },
		{ ".ogg", EAssetType::Audio },

		/* Fonts */
		{ ".ttf", EAssetType::Font },
		{ ".ttc", EAssetType::Font },
		{ ".otf", EAssetType::Font },
		
	};

}

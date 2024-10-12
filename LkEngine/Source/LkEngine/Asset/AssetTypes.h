#pragma once 

#include "LkEngine/Core/Core.h"


namespace LkEngine {

	/**
	 * AssetFlag enum.
	 * Allows bitwise logic without any casting.
	 */
	namespace AssetFlag
	{
		enum Type : uint16_t
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
		inline EAssetType AssetTypeFromString(std::string_view AssetType)
		{
			if (AssetType == "None")           return EAssetType::None;
			if (AssetType == "Scene")          return EAssetType::Scene;
			if (AssetType == "Prefab")         return EAssetType::Prefab;
			if (AssetType == "Mesh")           return EAssetType::Mesh;
			if (AssetType == "StaticMesh")     return EAssetType::StaticMesh;
			if (AssetType == "MeshSource")     return EAssetType::MeshSource;
			if (AssetType == "Material")       return EAssetType::Material;
			if (AssetType == "Texture")        return EAssetType::Texture;
			if (AssetType == "Font")           return EAssetType::Font;
			if (AssetType == "Skeleton")       return EAssetType::Skeleton;
			if (AssetType == "Animation")      return EAssetType::Animation;

			return EAssetType::None;
		}

		inline static constexpr const char* AssetTypeToString(const EAssetType AssetType)
		{
			if (AssetType == EAssetType::None)          return "None";
			if (AssetType == EAssetType::Scene)         return "Scene";
			if (AssetType == EAssetType::Prefab)        return "Prefab";
			if (AssetType == EAssetType::Mesh)          return "Mesh";
			if (AssetType == EAssetType::StaticMesh)    return "StaticMesh";
			if (AssetType == EAssetType::MeshSource)    return "MeshSource";
			if (AssetType == EAssetType::Material)      return "Material";
			if (AssetType == EAssetType::Texture)       return "Texture";
			if (AssetType == EAssetType::Font)          return "Font";
			if (AssetType == EAssetType::Skeleton)      return "Skeleton";
			if (AssetType == EAssetType::Animation)     return "Animation";

			return "EAssetType::None";
		}

	}

}
#pragma once 

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"


namespace LkEngine {

	enum class AssetFlag : uint16_t
	{
		None    = 0 << 0,
		Missing = 1 << 0,
		Invalid = 1 << 1
	};

	enum class AssetType : uint16_t
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
	};

	namespace Utils {

		inline AssetType AssetTypeFromString(std::string_view assetType)
		{
			if (assetType == "None")                return AssetType::None;
			if (assetType == "Scene")               return AssetType::Scene;
			if (assetType == "Prefab")              return AssetType::Prefab;
			if (assetType == "Mesh")                return AssetType::Mesh;
			if (assetType == "StaticMesh")          return AssetType::StaticMesh;
			if (assetType == "MeshSource")          return AssetType::MeshSource;
			if (assetType == "Material")            return AssetType::Material;
			if (assetType == "Texture")             return AssetType::Texture;
			if (assetType == "Font")                return AssetType::Font;
			if (assetType == "Skeleton")            return AssetType::Skeleton;
			if (assetType == "Animation")           return AssetType::Animation;

			return AssetType::None;
		}

		inline std::string AssetTypeToString(const AssetType& assetType)
		{
			if (assetType == AssetType::None)          return "None";
			if (assetType == AssetType::Scene)         return "Scene";
			if (assetType == AssetType::Prefab)        return "Prefab";
			if (assetType == AssetType::Mesh)          return "Mesh";
			if (assetType == AssetType::StaticMesh)    return "StaticMesh";
			if (assetType == AssetType::MeshSource)    return "MeshSource";
			if (assetType == AssetType::Material)      return "Material";
			if (assetType == AssetType::Texture)       return "Texture";
			if (assetType == AssetType::Font)          return "Font";
			if (assetType == AssetType::Skeleton)      return "Skeleton";
			if (assetType == AssetType::Animation)     return "Animation";

			return "NullAssetType";
		}

	}

}
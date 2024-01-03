#pragma once 

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"


namespace LkEngine {

	//enum class AssetFlag
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

	}

}
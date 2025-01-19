#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "AssetSerializer.h"


namespace LkEngine {

	class LMesh;
	class LStaticMesh;

	/**
	 * LMeshSourceSerializer
	 */
	class LMeshSourceSerializer : public IAssetSerializer, public LObject
	{
	public:
		LMeshSourceSerializer()
		{
			LOBJECT_REGISTER();
		}

		virtual void Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset) const override;
		virtual bool TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset) const override;

	private:
		LCLASS(LMeshSourceSerializer);
	};

	/**
	 * LMeshSerializer
	 */
	class LMeshSerializer : public IAssetSerializer, public LObject
	{
	public:
		using MeshType = LMesh;

		LMeshSerializer()
		{
			LOBJECT_REGISTER();
		}

		virtual void Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset) const override;
		virtual bool TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset) const override;
	private:
		std::string SerializeToYaml(TObjectPtr<LMesh> Mesh) const;
		bool DeserializeFromYaml(const std::string& YamlString, TObjectPtr<LMesh>& TargetMesh) const;

	private:
		LCLASS(LMeshSourceSerializer);
	};

	/**
	 * LStaticMeshSerializer
	 */
	class LStaticMeshSerializer : public IAssetSerializer, public LObject
	{
	public:
		using MeshType = LStaticMesh;

		LStaticMeshSerializer()
		{
			LOBJECT_REGISTER();
		}

		virtual void Serialize(const FAssetMetadata& Metadata, const TObjectPtr<LAsset>& Asset) const override;
		virtual bool TryLoadData(const FAssetMetadata& Metadata, TObjectPtr<LAsset>& Asset) const override;
	private:
		std::string SerializeToYaml(TObjectPtr<LStaticMesh> Mesh) const;
		bool DeserializeFromYaml(const std::string& YamlString, TObjectPtr<LStaticMesh>& TargetMesh) const;

	private:
		LCLASS(LStaticMeshSerializer);
	};

}


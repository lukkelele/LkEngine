#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/String.h"

/// FIXME: Cannot include LObject here.
///        Need to forward declare better.

namespace LkEngine {

	struct FClassMetadata
	{
		std::vector<std::string> Properties{};

		FClassMetadata() = default;
	};

	class LMetadataRegistry
	{
	public:
		FORCEINLINE static LMetadataRegistry& Get()
		{
			static LMetadataRegistry MetadataRegistry;
			return MetadataRegistry;
		}

		LMetadataRegistry(const LMetadataRegistry& Other) = delete;
		LMetadataRegistry(LMetadataRegistry&& Other) = delete;
		LMetadataRegistry& operator=(const LMetadataRegistry& Other) = delete;

		template<typename T>
		void Register(std::string_view InClassName, const T* ObjectPtr)
		{
			if (ObjectPtr)
			{
				std::printf("[MetadataRegistry] Registering class: \"%s\", TypeID=\"%s\"  Derived=\"%s\"\n", 
							InClassName.data(), typeid(ObjectPtr).name(), typeid(*ObjectPtr).name());
				if (StaticClassRegistry.find(InClassName.data()) == StaticClassRegistry.end())
				{
					StaticClassRegistry[InClassName.data()] = {};
				}
			}
			else
			{
				std::printf("[MetadataRegistry] [ERROR] Failed to register: \"%s\"\n", InClassName.data());
			}
		}

		template<typename T>
		void RegisterObject(std::string_view ObjectVariableName, const T& ObjectToRegister);

		void LogMetadata(std::string_view VariableName)
		{
			if (LiveCollection.find(VariableName.data()) != LiveCollection.end())
			{
				LK_CORE_INFO_TAG("MetadataRegistry", "Metadata for '{}'\n"
								 " - Type:     {}\n"
								 " - Created : {}\n",
								 LiveCollection[VariableName.data()].first,
								 std::ctime(&LiveCollection[VariableName.data()].second));
			}
			else
			{
				LK_CORE_WARN_TAG("MetadataRegistry", "No LiveCollection found for variable '{}'", VariableName);
			}
		}

		FORCEINLINE int GetClassRegistrySize() const 
		{ 
			return static_cast<int>(StaticClassRegistry.size());
		}

		FORCEINLINE const std::unordered_map<std::string, FClassMetadata>& GetStaticClassRegistry() const
		{
			return StaticClassRegistry;
		}

		FORCEINLINE const std::unordered_map<std::string, std::pair<std::string, std::time_t>>& GetLiveCollection() const
		{
			return LiveCollection;
		}

	private:
		LMetadataRegistry() = default;

	private:
		std::unordered_map<std::string, FClassMetadata> StaticClassRegistry{};
		std::unordered_map<std::string, std::pair<std::string, std::time_t>> LiveCollection{};
	};

}

#include "MetadataRegistry.tpp"
#pragma once

#include "LkEngine/Core/LObject/Object.h"

#include "SerializationMacros.h"

namespace LkEngine {

	enum class ESerializeFormat : uint8_t
	{
		None = 0,
		Yaml,
	};

	namespace Enum 
	{
		FORCEINLINE static constexpr const char* ToString(const ESerializeFormat Format)
		{
			switch (Format)
			{
				case ESerializeFormat::None: return "None";
				case ESerializeFormat::Yaml: return "Yaml";
			}

			LK_CORE_VERIFY(false);
			return nullptr;
		}

	}


	/**
	 * ISerializable
	 */
	class ISerializable
	{
	public:
		virtual ~ISerializable() = default;

		virtual void SerializeToYaml(YAML::Emitter& Out) const = 0;
		virtual void DeserializeFromYaml(const YAML::Node& Data) = 0;
	};

}

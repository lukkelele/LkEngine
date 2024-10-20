#pragma once

#include "LkEngine/Core/LObject/Object.h"


namespace LkEngine {

#if 0
	enum class ESerializeFormat
	{
		Unknown = 0,
		Yaml,
	};
#endif
	namespace SerializeFormat
	{
		enum class Type : uint8_t
		{
			None = 0,
			Yaml,
		};
	}
	using ESerializeFormat = SerializeFormat::Type;
	
	/**
	 * ISerializer
	 *
	 *  Interface for serializers.
	 */
	class ISerializer
	{
	public:
        virtual void Serialize() = 0;
        virtual void Serialize(const std::filesystem::path& InConfigFile) = 0;

        virtual bool Deserialize() = 0;
        virtual bool Deserialize(const std::filesystem::path& InConfigFile) = 0;
	};


	namespace Enum {

		FORCEINLINE static constexpr const char* ToString(const ESerializeFormat Format)
		{
			switch (Format)
			{
				case ESerializeFormat::None:  return "None";
				case ESerializeFormat::Yaml:  return "Yaml";
			}

			LK_ASSERT(false);
			return nullptr;
		}

	}

}

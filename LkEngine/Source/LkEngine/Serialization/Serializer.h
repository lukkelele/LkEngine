#pragma once

#include "LkEngine/Core/LObject/Object.h"

#include "YAMLSerialization.h"

/**
 * TODO: LOG IN CONSOLE
 */

/**
 * Helper for being able to pass C-strings and implicitly convert them
 * to std::string when using the serialization macros.
 * Required for decltype to evaluate correctly.
 */
template <typename T>
static constexpr auto ToStringIfNeeded(T Value)
{
    if constexpr (std::is_same_v<T, const char*> || std::is_array_v<T>)
    {
		/* Convert to std::string if it is const char* or const char[]. */
        return std::string(Value);
    }
    else
    {
        return Value;
    }
}

/**
 * LK_SERIALIZE_PROPERTY
 * 
 *  Serialize a property to a YAML node.
 *  The property name should NOT be passed with quotes.
 */
#define LK_SERIALIZE_PROPERTY(PropertyName, PropertyValue, OutputNode) \
	OutputNode << YAML::Key << #PropertyName << YAML::Value << PropertyValue

#define LK_SERIALIZE_PROPERTY_ASSET(PropertyName, PropertyValue, OutputData) \
	OutputData << YAML::Key << #PropertyName << YAML::Value << (PropertyValue ? static_cast<uint64_t>(PropertyValue->Handle) : 0);

/**
 * LK_DESERIALIZE_PROPERTY
 * 
 *  Deserialize a property from a YAML node.
 *  The property name should NOT be passed with quotes.
 */
#define LK_DESERIALIZE_PROPERTY(PropertyName, Destination, Node, DefaultValue) \
	if (Node.IsMap())                                                          \
	{                                                                          \
		if (auto FoundNode = Node[#PropertyName])                              \
		{                                                                      \
			try                                                                \
			{                                                                  \
				Destination = FoundNode.as<decltype(ToStringIfNeeded(DefaultValue))>(); \
			}                                                                  \
			catch (const std::exception& Exception)                            \
			{                                                                  \
				LK_CORE_ERROR("LK_DESERIALIZE_PROPERTY: {}", Exception.what());\
				Destination = DefaultValue;                                    \
			}                                                                  \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			Destination = DefaultValue;                                        \
		}                                                                      \
	}                                                                          \
	else                                                                       \
	{                                                                          \
		Destination = DefaultValue;                                            \
	}


namespace LkEngine {

	enum class ESerializeFormat : uint8_t
	{
		None = 0,
		Yaml,
	};

	namespace Enum {

		FORCEINLINE static constexpr const char* ToString(const ESerializeFormat Format)
		{
			switch (Format)
			{
				case ESerializeFormat::None: return "None";
				case ESerializeFormat::Yaml: return "Yaml";
			}

			LK_ASSERT(false);
			return nullptr;
		}

	}

}

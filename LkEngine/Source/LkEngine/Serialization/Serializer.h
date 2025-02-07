#pragma once

#include "LkEngine/Core/LObject/Object.h"

#include "YAMLSerialization.h"

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
				LK_CORE_FATAL("LK_DESERIALIZE_PROPERTY: {}", Exception.what());\
				Destination = DefaultValue;                                    \
			}                                                                  \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			LK_CORE_WARN_TAG("LK_DESERIALIZE_PROPERTY", "Failed to find property: '{}'", #PropertyName); \
			Destination = DefaultValue;                                        \
		}                                                                      \
	}                                                                          \
	else                                                                       \
	{                                                                          \
		LK_CORE_ERROR_TAG("LK_DESERIALIZE_PROPERTY", "Root node used with property '{}' is not a YAML::Map", #PropertyName); \
		Destination = DefaultValue;                                            \
	}

#define LK_SERIALIZE_BEGIN_GROUP(Name)    Out << YAML::Key << #Name << YAML::Value << YAML::BeginMap;
#define LK_SERIALIZE_END_GROUP()          Out << YAML::EndMap;
#define LK_SERIALIZE_VALUE(Name, InValue) Out << YAML::Key << #Name << YAML::Value << InValue;


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

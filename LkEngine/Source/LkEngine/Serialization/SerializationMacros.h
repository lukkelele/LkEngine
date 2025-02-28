#pragma once

#include <stdarg.h>
#include <string>

#include "YamlSerialization.h"


namespace LkEngine
{
	namespace Serialization::Internal 
	{
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
	}
}

/* TODO: Possible to statically evaluate the 'PropertyName' argument and if it's an Lvalue or Rvalue? */

/**
 * LK_SERIALIZE_PROPERTY
 * 
 *  Serialize a property to a YAML node.
 *  The property name should NOT be passed with quotes.
 */
#define LK_SERIALIZE_PROPERTY(PropertyName, PropertyValue, OutputNode) \
	OutputNode << YAML::Key << #PropertyName << YAML::Value << PropertyValue

/**
 * LK_SERIALIZE_PROPERTY_VALUE
 * 
 *  Serialize a property to a YAML node.
 *  The property argument is a value itself (e.g a string from Enum::ToString).
 */
#define LK_SERIALIZE_PROPERTY_VALUE(Property, PropertyValue, OutputNode) \
	OutputNode << YAML::Key << Property << YAML::Value << PropertyValue

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
				Destination = FoundNode.as<decltype(::LkEngine::Serialization::Internal::ToStringIfNeeded(DefaultValue))>(); \
			}                                                                  \
			catch (const std::exception& Exception)                            \
			{                                                                  \
				LK_CORE_FATAL("LK_DESERIALIZE_PROPERTY: {}", Exception.what());\
				Destination = DefaultValue;                                    \
			}                                                                  \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			LK_CORE_ERROR_TAG("LK_DESERIALIZE_PROPERTY", "Failed to find property: '{}'", #PropertyName); \
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


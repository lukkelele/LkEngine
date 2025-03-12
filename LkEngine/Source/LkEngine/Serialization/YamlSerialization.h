#pragma once

#include "LkEngine/Asset/Asset.h"

#include <yaml-cpp/yaml.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>


/**
 * Specializations for encode, decode and convert from yaml-cpp.
 */
namespace YAML 
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& Rhs)
		{
			Node YamlNode;
			YamlNode.push_back(Rhs.x);
			YamlNode.push_back(Rhs.y);
			return YamlNode;
		}

		static bool decode(const Node& YamlNode, glm::vec2& Rhs)
		{
			if (!YamlNode.IsSequence() || YamlNode.size() != 2)
			{
				return false;
			}

			Rhs.x = YamlNode[0].as<float>();
			Rhs.y = YamlNode[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& Rhs)
		{
			Node YamlNode;
			YamlNode.push_back(Rhs.x);
			YamlNode.push_back(Rhs.y);
			YamlNode.push_back(Rhs.z);
			return YamlNode;
		}

		static bool decode(const Node& YamlNode, glm::vec3& Rhs)
		{
			if (!YamlNode.IsSequence() || YamlNode.size() != 3)
			{
				return false;
			}

			Rhs.x = YamlNode[0].as<float>();
			Rhs.y = YamlNode[1].as<float>();
			Rhs.z = YamlNode[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& Rhs)
		{
			Node YamlNode;
			YamlNode.push_back(Rhs.x);
			YamlNode.push_back(Rhs.y);
			YamlNode.push_back(Rhs.z);
			YamlNode.push_back(Rhs.w);
			return YamlNode;
		}

		static bool decode(const Node& YamlNode, glm::vec4& Rhs)
		{
			if (!YamlNode.IsSequence() || YamlNode.size() != 4)
			{
				return false;
			}

			Rhs.x = YamlNode[0].as<float>();
			Rhs.y = YamlNode[1].as<float>();
			Rhs.z = YamlNode[2].as<float>();
			Rhs.w = YamlNode[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::quat>
	{
		static Node encode(const glm::quat& Rhs)
		{
			Node YamlNode;
			YamlNode.push_back(Rhs.w);
			YamlNode.push_back(Rhs.x);
			YamlNode.push_back(Rhs.y);
			YamlNode.push_back(Rhs.z);
			return YamlNode;
		}

		static bool decode(const Node& YamlNode, glm::quat& Rhs)
		{
			if (!YamlNode.IsSequence() || YamlNode.size() != 4)
			{
				return false;
			}

			Rhs.w = YamlNode[0].as<float>();
			Rhs.x = YamlNode[1].as<float>();
			Rhs.y = YamlNode[2].as<float>();
			Rhs.z = YamlNode[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<LkEngine::LUUID>
	{
		static Node encode(const LkEngine::LUUID& Rhs)
		{
			Node YamlNode;
			YamlNode.push_back(static_cast<uint64_t>(Rhs));
			return YamlNode;
		}

		static bool decode(const Node& YamlNode, LkEngine::LUUID& Rhs)
		{
			Rhs = YamlNode.as<uint64_t>();
			return true;
		}
	};

	/* LMesh / LMeshSource / LStaticMesh */
	template<>
	struct convert<std::vector<uint32_t>>
	{
		static Node encode(const std::vector<uint32_t>& Value)
		{
			Node YamlNode;
			for (uint32_t Element : Value)
			{
				YamlNode.push_back(Element);
			}

			return YamlNode;
		}

		static bool decode(const Node& NodeRef, std::vector<uint32_t>& Result)
		{
			if (!NodeRef.IsSequence())
			{
				return false;
			}

			Result.resize(NodeRef.size());
			for (std::size_t i = 0; i < NodeRef.size(); i++)
			{
				Result[i] = NodeRef[i].as<uint32_t>();
			}

			return true;
		}
	};

	template<>
	struct convert<std::chrono::seconds>
	{
		static Node encode(const std::chrono::seconds& Rhs)
		{
			Node YamlNode;
			YamlNode.push_back(Rhs.count());
			return YamlNode;
		}

		static bool decode(const Node& InNode, std::chrono::seconds& Rhs)
		{
			if (!InNode.IsScalar())
			{
				return false;
			}

			Rhs = std::chrono::seconds(InNode.as<int>());
			return true;
		}
	};


}

namespace LkEngine {

	inline YAML::Emitter& operator<<(YAML::Emitter& Out, const glm::vec2& InVec2)
	{
		Out << YAML::Flow;
		Out << YAML::BeginSeq << InVec2.x << InVec2.y << YAML::EndSeq;
		return Out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& Out, const glm::vec3& InVec3)
	{
		Out << YAML::Flow;
		Out << YAML::BeginSeq << InVec3.x << InVec3.y << InVec3.z << YAML::EndSeq;
		return Out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& Out, const glm::vec4& InVec4)
	{
		Out << YAML::Flow;
		Out << YAML::BeginSeq << InVec4.x << InVec4.y << InVec4.z << InVec4.w << YAML::EndSeq;
		return Out;
	}

	/* LMesh / LMeshSource / LStaticMesh */
	inline YAML::Emitter& operator<<(YAML::Emitter& Out, const std::vector<uint32_t>& Value)
	{
		Out << YAML::Flow;
		Out << YAML::BeginSeq;
		for (const uint32_t Element : Value)
		{
			Out << Element;
		}
		Out << YAML::EndSeq;

		return Out;
	}

}

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
		static Node encode(const glm::vec2& RHS)
		{
			Node node;
			node.push_back(RHS.x);
			node.push_back(RHS.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& RHS)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			RHS.x = node[0].as<float>();
			RHS.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& RHS)
		{
			Node node;
			node.push_back(RHS.x);
			node.push_back(RHS.y);
			node.push_back(RHS.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& RHS)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			RHS.x = node[0].as<float>();
			RHS.y = node[1].as<float>();
			RHS.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& RHS)
		{
			Node node;
			node.push_back(RHS.x);
			node.push_back(RHS.y);
			node.push_back(RHS.z);
			node.push_back(RHS.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& RHS)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			RHS.x = node[0].as<float>();
			RHS.y = node[1].as<float>();
			RHS.z = node[2].as<float>();
			RHS.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::quat>
	{
		static Node encode(const glm::quat& RHS)
		{
			Node node;
			node.push_back(RHS.w);
			node.push_back(RHS.x);
			node.push_back(RHS.y);
			node.push_back(RHS.z);
			return node;
		}

		static bool decode(const Node& node, glm::quat& RHS)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			RHS.w = node[0].as<float>();
			RHS.x = node[1].as<float>();
			RHS.y = node[2].as<float>();
			RHS.z = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<LkEngine::FAssetHandle>
	{
		static Node encode(const LkEngine::FAssetHandle& RHS)
		{
			Node node;
			//node.push_back((uint64_t)RHS);
			node.push_back(static_cast<uint64_t>(RHS));
			return node;
		}

		static bool decode(const Node& node, LkEngine::FAssetHandle& RHS)
		{
			RHS = node.as<uint64_t>();
			return true;
		}
	};

}

namespace LkEngine {

	/* Overloaded bitwise operators. */
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

}

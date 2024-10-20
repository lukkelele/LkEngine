#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Serialization/FileStream.h"


namespace LkEngine {

	struct ShaderProgramSource
	{
		std::string VertexSource{};
		std::string FragmentSource{};
	};

	class ShaderResourceDeclaration
	{
	public:
		ShaderResourceDeclaration() = default;
		ShaderResourceDeclaration(const std::string& InName, 
								  uint32_t InSet, 
								  uint32_t InResourceRegister, 
								  uint32_t InCount)
			: Name(InName)
			, Set(InSet)
			, Register(InResourceRegister)
			, Count(InCount) 
		{
		}

		virtual const std::string& GetName() const { return Name; }
		virtual uint32_t GetSet() const { return Set; }
		virtual uint32_t GetRegister() const { return Register; }
		virtual uint32_t GetCount() const { return Count; }

		static void Serialize(StreamWriter* Serializer, const ShaderResourceDeclaration& Instance)
		{
			Serializer->WriteString(Instance.Name);
			Serializer->WriteRaw(Instance.Set);
			Serializer->WriteRaw(Instance.Register);
			Serializer->WriteRaw(Instance.Count);
		}

		static void Deserialize(StreamReader* DeSerializer, ShaderResourceDeclaration& Instance)
		{
			DeSerializer->ReadString(Instance.Name);
			DeSerializer->ReadRaw(Instance.Set);
			DeSerializer->ReadRaw(Instance.Register);
			DeSerializer->ReadRaw(Instance.Count);
		}

	private:
		std::string Name;
		uint32_t Set = 0;
		uint32_t Register = 0;
		uint32_t Count = 0;
	};

	using ShaderResourceList = std::vector<ShaderResourceDeclaration*>;

}
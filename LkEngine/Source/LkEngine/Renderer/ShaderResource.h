#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Serialization/FileStream.h"


namespace LkEngine {

	struct FShaderProgramSource
	{
		std::string Vertex{};
		std::string Fragment{};

		bool IsValid() const
		{
			return ((Vertex.size() > 0) && (Fragment.size() > 0));
		}
	};

	class LShaderResourceDeclaration
	{
	public:
		LShaderResourceDeclaration() = default;
		LShaderResourceDeclaration(const std::string& InName, 
								  uint32_t InSet, 
								  uint32_t InResourceRegister, 
								  uint32_t InCount)
			: Name(InName)
			, Set(InSet)
			, Register(InResourceRegister)
			, Count(InCount) 
		{
		}

		FORCEINLINE virtual const std::string& GetName() const { return Name; }
		FORCEINLINE virtual uint32_t GetSet() const { return Set; }
		FORCEINLINE virtual uint32_t GetRegister() const { return Register; }
		FORCEINLINE virtual uint32_t GetCount() const { return Count; }

		static void Serialize(StreamWriter* Serializer, const LShaderResourceDeclaration& Instance)
		{
			Serializer->WriteString(Instance.Name);
			Serializer->WriteRaw(Instance.Set);
			Serializer->WriteRaw(Instance.Register);
			Serializer->WriteRaw(Instance.Count);
		}

		static void Deserialize(StreamReader* DeSerializer, LShaderResourceDeclaration& Instance)
		{
			DeSerializer->ReadString(Instance.Name);
			DeSerializer->ReadRaw(Instance.Set);
			DeSerializer->ReadRaw(Instance.Register);
			DeSerializer->ReadRaw(Instance.Count);
		}

	private:
		std::string Name{};
		uint32_t Set = 0;
		uint32_t Register = 0;
		uint32_t Count = 0;
	};

	using ShaderResourceList = std::vector<LShaderResourceDeclaration*>;
}
#pragma once

#include "LkEngine/Core/Core.h"


namespace LkEngine {

	/**
	 * EShaderDataType
	 */
	enum class EShaderDataType : uint8_t
	{
		Null = 0,
		Float, 
		Float2, 
		Float3, 
		Float4,
		Int, 
		Int2, 
		Int3, 
		Int4,
		Mat3, 
		Mat4,
		Bool,
	};

	namespace Enum
	{
		inline static constexpr const char* ToString(const EShaderDataType ShaderDataType)
		{
			switch (ShaderDataType)
			{
				case EShaderDataType::Null:		return "Null";
				case EShaderDataType::Bool:		return "Bool";
				case EShaderDataType::Int:		return "Int";
				case EShaderDataType::Int2:		return "Int2";
				case EShaderDataType::Int3:		return "Int3";
				case EShaderDataType::Int4:		return "Int4";
				case EShaderDataType::Float:	return "Float";
				case EShaderDataType::Float2:	return "Float2";
				case EShaderDataType::Float3:	return "Float3";
				case EShaderDataType::Float4:	return "Float4";
				case EShaderDataType::Mat3:		return "Mat3";
				case EShaderDataType::Mat4:		return "Mat4";
			}

			LK_CORE_ASSERT(false);
			return nullptr;
		}
	}

	/**
	 * EShaderDataMember
	 */
	enum class EShaderDataMember
	{
		Null = 0,
		Position,
		Color
	};

	inline static constexpr uint32_t GetShaderDataTypeSize(const EShaderDataType DataType)
	{
		switch (DataType)
		{
			case EShaderDataType::Bool:     return 1;
			case EShaderDataType::Int:      return 4;
			case EShaderDataType::Int2:     return 4 * 2;
			case EShaderDataType::Int3:     return 4 * 3;
			case EShaderDataType::Int4:     return 4 * 4;
			case EShaderDataType::Float:    return 4;
			case EShaderDataType::Float2:   return 4 * 2;
			case EShaderDataType::Float3:   return 4 * 3;
			case EShaderDataType::Float4:   return 4 * 4;
			case EShaderDataType::Mat3:     return 4 * 3 * 3;
			case EShaderDataType::Mat4:     return 4 * 4 * 4;
		}

		LK_CORE_ASSERT(false, "Invalid EShaderDataType value: {}", Enum::ToString(DataType));
		return 0;
	}

	struct FVertexBufferElement
	{
		std::string Name{};
		EShaderDataType Type;
		uint32_t Size = 0;
		size_t Offset = 0;
		unsigned char Normalized{};
		unsigned int Count = 0;

		FVertexBufferElement() = default;
		FVertexBufferElement(const std::string& InName, const EShaderDataType InShaderDataType, const bool InNormalized = false)
			: Type(InShaderDataType)
			, Name(InName)
			, Size(GetShaderDataTypeSize(InShaderDataType))
			, Offset(0)
			, Normalized(InNormalized)
			, Count(0)
		{
		}

		FORCEINLINE uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case EShaderDataType::Float:   return 1;
				case EShaderDataType::Float2:  return 2;
				case EShaderDataType::Float3:  return 3;
				case EShaderDataType::Float4:  return 4;
				case EShaderDataType::Mat3:    return 3 * 3;
				case EShaderDataType::Mat4:    return 4 * 4;
				case EShaderDataType::Int:     return 1;
				case EShaderDataType::Int2:    return 2;
				case EShaderDataType::Int3:    return 3;
				case EShaderDataType::Int4:    return 4;
				case EShaderDataType::Bool:    return 1;
			}

			LK_CORE_ASSERT(false, "GetComponentCount failed");
			return 0;
		}
	};

	struct FVertexBufferLayout
	{
	public:
		FVertexBufferLayout() = default;
		FVertexBufferLayout(const std::initializer_list<FVertexBufferElement>& InElements)
			: Elements(InElements)
		{
			CalculateOffsetsAndStride();
		}

		FORCEINLINE uint32_t GetStride() const { return Stride; }

		FORCEINLINE const std::vector<FVertexBufferElement>& GetElements() const 
		{ 
			return Elements; 
		}

		FORCEINLINE uint32_t GetElementCount() const 
		{ 
			return static_cast<uint32_t>(Elements.size()); 
		}

		[[nodiscard]] std::vector<FVertexBufferElement>::iterator begin() { return Elements.begin(); }
		[[nodiscard]] std::vector<FVertexBufferElement>::iterator end() { return Elements.end(); }
		[[nodiscard]] std::vector<FVertexBufferElement>::const_iterator begin() const { return Elements.begin(); }
		[[nodiscard]] std::vector<FVertexBufferElement>::const_iterator end() const { return Elements.end(); }

		FORCEINLINE std::string ToString() const
		{
			std::string String{};
			for (const FVertexBufferElement& Element : Elements)
			{
				String += LK_FORMAT_STRING("{}: {}\n", Element.Name, Enum::ToString(Element.Type));
			}

			return String;
		}

	private:
		FORCEINLINE void CalculateOffsetsAndStride()
		{
			Stride = 0;
			uint32_t Offset = 0;
			for (FVertexBufferElement& Element : Elements)
			{
				Element.Offset = Offset;
				Offset += Element.Size;
				Stride += Element.Size;
			}
		}

		std::vector<FVertexBufferElement> Elements{};
		uint32_t Stride = 0;
	};


}

#pragma once

#include "LkEngine/Core/Core.h"


namespace LkEngine {

	/**
	 * EShaderDataType
	 */
	enum class EShaderDataType
	{
		Null = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3, Mat4,
		Bool,
	};

	/**
	 * EShaderDataMember
	 */
	enum class EShaderDataMember
	{
		Null = 0,
		Position,
		Color
	};

	inline static constexpr uint32_t ShaderDataTypeSize(EShaderDataType DataType)
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

		LK_CORE_ASSERT(false, "Invalid EShaderDataType value: {}", (int)DataType);
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
		FVertexBufferElement(const std::string& name, EShaderDataType type, bool normalized = false)
			: Type(type)
			, Name(name)
			, Size(ShaderDataTypeSize(type))
			, Offset(0)
			, Normalized(normalized)
			, Count(0)
		{
		}

		FORCEINLINE std::string_view GetName() const { return Name; }

		uint32_t GetComponentCount() const
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

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() {}

		VertexBufferLayout(const std::initializer_list<FVertexBufferElement>& InElements)
			: m_Elements(InElements)
		{
			CalculateOffsetsAndStride();
		}

		FORCEINLINE uint32_t GetStride() const 
		{ 
			return m_Stride; 
		}

		FORCEINLINE const std::vector<FVertexBufferElement>& GetElements() const 
		{ 
			return m_Elements; 
		}

		FORCEINLINE uint32_t GetElementCount() const 
		{ 
			return (uint32_t)m_Elements.size(); 
		}

		[[nodiscard]] std::vector<FVertexBufferElement>::iterator begin() { return m_Elements.begin(); }
		[[nodiscard]] std::vector<FVertexBufferElement>::iterator end() { return m_Elements.end(); }
		[[nodiscard]] std::vector<FVertexBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		[[nodiscard]] std::vector<FVertexBufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:
		std::vector<FVertexBufferElement> m_Elements{};
		uint32_t m_Stride = 0;
	};


}

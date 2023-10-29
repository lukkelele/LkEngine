#pragma once

#include "LkEngine/Core/Base.h"
#include <glad/glad.h>


namespace LkEngine {

	enum class ShaderDataType
	{
		Null = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3, Mat4,
		Bool,
	};

	static unsigned int ShaderDataTypeSize(ShaderDataType dataType)
	{
		switch (dataType)
		{
			case ShaderDataType::Bool:     return 1;
			case ShaderDataType::Int:      return 4;
			case ShaderDataType::Int2:     return 4 * 2;
			case ShaderDataType::Int3:     return 4 * 3;
			case ShaderDataType::Int4:     return 4 * 4;
			case ShaderDataType::Float:    return 4;
			case ShaderDataType::Float2:   return 4 * 2;
			case ShaderDataType::Float3:   return 4 * 3;
			case ShaderDataType::Float4:   return 4 * 4;
			case ShaderDataType::Mat3:     return 4 * 3 * 3;
			case ShaderDataType::Mat4:     return 4 * 4 * 4;
		}
		LK_ASSERT(false);
		return 0;
	}

	struct VertexBufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		size_t Offset;
		unsigned char Normalized;
		unsigned int Count;

		VertexBufferElement() = default;

		VertexBufferElement(const std::string& name, ShaderDataType type, bool normalized = false)
			: Type(type)
			, Name(name)
			, Size(ShaderDataTypeSize(type))
			, Offset(0)
			, Normalized(normalized)
			, Count(0)
		{
		}

		std::string_view GetName() { return Name; }

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:   return 1;
				case ShaderDataType::Float2:  return 2;
				case ShaderDataType::Float3:  return 3;
				case ShaderDataType::Float4:  return 4;
				case ShaderDataType::Int:     return 1;
				case ShaderDataType::Int2:    return 2;
				case ShaderDataType::Int3:    return 3;
				case ShaderDataType::Int4:    return 4;
				case ShaderDataType::Mat3:    return 3; 
				case ShaderDataType::Mat4:    return 4; 
				case ShaderDataType::Bool:    return 1;
			}
			return 0;
		}
	};

	class VertexBufferLayout
	{
	protected:
		std::vector<VertexBufferElement> m_Elements;
		unsigned int m_Stride;

	public:
		VertexBufferLayout()
			: m_Stride(0) {}

		VertexBufferLayout(std::initializer_list<VertexBufferElement> elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		std::vector<VertexBufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<VertexBufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<VertexBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<VertexBufferElement>::const_iterator end() const { return m_Elements.end(); }

		inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }
		inline unsigned int GetStride() const { return m_Stride; }

		VertexBufferElement GetElement(const std::string& name)
		{
			for (auto it = begin(); it != end(); it++)
			{
				VertexBufferElement& element = *it;
				std::string_view elementName = element.GetName();
				if (elementName == name)
					return element;
			}
			LOG_ERROR("Could not retrieve element by the name '{}'", name);
			return VertexBufferElement();
		}

		void CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
			//LOG_DEBUG("VertexBufferLayout: {} elements, stride: {}", m_Elements.size(), m_Stride);
		}

	};

}

#pragma once

#include "LkEngine/Core/Base.h"
#include <glad/glad.h>


namespace LkEngine {

	enum class ShaderDataType
	{
		Null = 0, 
		Bool,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4, 
		Mat3, Mat4, 
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

		static unsigned int GetSizeOfType(unsigned int type)
		{
			switch (type)
			{
				case GL_FLOAT:			return 4;
				case GL_UNSIGNED_INT:   return 4;
				case GL_UNSIGNED_BYTE:  return 1;
			}
			return 0;
		}

		VertexBufferElement(const std::string& name, ShaderDataType type, bool normalized = false)
			: Name(name)
			, Type(type)
			, Size(ShaderDataTypeSize(type))
			, Offset(0)
			, Normalized(normalized)
			, Count(0)
		{
		}
	};

	class VertexBufferLayout
	{
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
		inline unsigned int GetStride() const { return m_Stride;  }

		void CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
				LOG_DEBUG("Element offset: {}", offset);
			}
			LOG_DEBUG("VertexBufferLayout: {} elements, stride: {}", m_Elements.size(), m_Stride);
		}

	private:
		std::vector<VertexBufferElement> m_Elements;
		unsigned int m_Stride;

	public:
		// TODO: Define outside
		template<typename T>
		void Push(unsigned int count)
		{
		}

		template<>
		void Push<float>(unsigned int count)
		{
			//m_Elements.push_back({"NULL", ShaderDataType::Float, ShaderDataTypeSize(ShaderDataType::Float), 0, GL_FALSE, count});
			m_Elements.push_back({ "NULL", ShaderDataType::Float, GL_FALSE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
		}

		template<>
		void Push<unsigned int>(unsigned int count)
		{
			//m_Elements.push_back({"NULL", GL_UNSIGNED_INT, count, GL_FALSE, static_cast<uint32_t>(count * sizeof(GL_UNSIGNED_INT)), 0});
			//m_Elements.push_back({"NULL", ShaderDataType::Int, ShaderDataTypeSize(ShaderDataType::Int), 0, GL_FALSE, count});
			m_Elements.push_back({ "NULL", ShaderDataType::Int, GL_FALSE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
		}


	};

}

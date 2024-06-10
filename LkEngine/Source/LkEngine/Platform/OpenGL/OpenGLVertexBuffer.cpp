#include "LKpch.h"
#include "OpenGLVertexBuffer.h"

#include "OpenGLIndexBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return GL_FLOAT;
			case ShaderDataType::Float2:   return GL_FLOAT;
			case ShaderDataType::Float3:   return GL_FLOAT;
			case ShaderDataType::Float4:   return GL_FLOAT;
			case ShaderDataType::Mat3:     return GL_FLOAT;
			case ShaderDataType::Mat4:     return GL_FLOAT;
			case ShaderDataType::Int:      return GL_INT;
			case ShaderDataType::Int2:     return GL_INT;
			case ShaderDataType::Int3:     return GL_INT;
			case ShaderDataType::Int4:     return GL_INT;
			case ShaderDataType::Bool:     return GL_BOOL;
		}
		return 0;
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* buffer, uint64_t size, VertexBufferUsage usage)
		: m_Size(size)
		, m_Usage(usage)
	{
		m_LocalData = Buffer(buffer, size);

		glGenVertexArrays(1, &m_VertexArrayID);
		glBindVertexArray(m_VertexArrayID);

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint64_t size, VertexBufferUsage usage)
		: m_Size(size)
		, m_Usage(usage)
	{
		m_LocalData.Allocate(size);

		glGenVertexArrays(1, &m_VertexArrayID);
		glBindVertexArray(m_VertexArrayID);

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		//LK_CORE_DEBUG_TAG("VertexBuffer", "New vertexbuffer, id: {}, size: {}", m_VertexArrayID, m_LocalData.GetSize());
	}
 
	OpenGLVertexBuffer::~OpenGLVertexBuffer() 
	{
		GL_CALL(glDeleteVertexArrays(1, &m_VertexArrayID));
		GL_CALL(glDeleteBuffers(1, &m_RendererID));
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindVertexArray(m_VertexArrayID);
	}

	void OpenGLVertexBuffer::SetData(void* buffer, uint64_t size, uint64_t offset)
	{
		m_LocalData.Data = buffer;
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, buffer);
	}

	void OpenGLVertexBuffer::RT_SetData(void* buffer, uint64_t size, uint64_t offset)
	{
		m_LocalData.Data = buffer;
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, buffer);
	}

	void OpenGLVertexBuffer::SetLayout(const VertexBufferLayout& layout)
	{
		m_BufferLayout = layout;
		AddVertexBufferToVertexArray();
	}

	void OpenGLVertexBuffer::AddVertexBufferToVertexArray()
	{
		GL_CALL(glBindVertexArray(m_VertexArrayID));
		VertexBufferLayout& layout = GetLayout();

		for (const auto& element : layout)
		{
			switch (element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
										  element.GetComponentCount(),
										  ShaderDataTypeToOpenGLBaseType(element.Type),
										  element.Normalized ? GL_TRUE : GL_FALSE,
										  layout.GetStride(),
										  (const void*)element.Offset);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribIPointer(m_VertexBufferIndex,
										   element.GetComponentCount(),
										   ShaderDataTypeToOpenGLBaseType(element.Type),
										   layout.GetStride(),
										   (const void*)element.Offset);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(m_VertexBufferIndex);
						glVertexAttribPointer(m_VertexBufferIndex,
											  count,
											  ShaderDataTypeToOpenGLBaseType(element.Type),
											  element.Normalized ? GL_TRUE : GL_FALSE,
											  layout.GetStride(),
											  (const void*)(element.Offset + sizeof(float) * count * i));
						glVertexAttribDivisor(m_VertexBufferIndex, 1);
						m_VertexBufferIndex++;
					}
					break;
				}
				default: LK_ASSERT(false);
			}
		}

	}

	void OpenGLVertexBuffer::AddVertexBuffer(VertexBuffer& vb)
	{
		glBindVertexArray(m_VertexArrayID);
		vb.Bind();
		VertexBufferLayout& layout = vb.GetLayout();

		for (const auto& element : layout)
		{
			switch (element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
										  element.GetComponentCount(),
										  ShaderDataTypeToOpenGLBaseType(element.Type),
										  element.Normalized ? GL_TRUE : GL_FALSE,
										  layout.GetStride(),
										  (const void*)element.Offset);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribIPointer(m_VertexBufferIndex,
										   element.GetComponentCount(),
										   ShaderDataTypeToOpenGLBaseType(element.Type),
										   layout.GetStride(),
										   (const void*)element.Offset);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(m_VertexBufferIndex);
						glVertexAttribPointer(m_VertexBufferIndex,
											  count,
											  ShaderDataTypeToOpenGLBaseType(element.Type),
											  element.Normalized ? GL_TRUE : GL_FALSE,
											  layout.GetStride(),
											  (const void*)(element.Offset + sizeof(float) * count * i));
						glVertexAttribDivisor(m_VertexBufferIndex, 1);
						m_VertexBufferIndex++;
					}
					break;
				}
				default: LK_ASSERT(false);
			}
		}
	}

#if 0
	void OpenGLVertexBuffer::BindVertexArray()
	{
		GL_CALL(glBindVertexArray(m_VertexArrayID));
	}

	void OpenGLVertexBuffer::UnbindVertexArray()
	{
		GL_CALL(glBindVertexArray(0));
	}
#endif

	void OpenGLVertexBuffer::SetIndexBuffer(const Ref<IndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_VertexArrayID);
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}

}
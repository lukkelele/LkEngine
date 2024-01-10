#include "LKpch.h"
#include "OpenGLVertexBuffer.h"

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

	OpenGLVertexBuffer::OpenGLVertexBuffer(unsigned int bufsize)
	{
		GL_CALL(glGenVertexArrays(1, &m_VertexArrayID));
		GL_CALL(glBindVertexArray(m_VertexArrayID));

		GL_CALL(glGenBuffers(1, &m_RendererID));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, bufsize, nullptr, GL_STATIC_DRAW));
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, unsigned int arrsize)
	{
		GL_CALL(glGenVertexArrays(1, &m_VertexArrayID));
		GL_CALL(glBindVertexArray(m_VertexArrayID));
		LOG_TRACE("New VertexArray, id: {}", m_VertexArrayID);

		GL_CALL(glGenBuffers(1, &m_RendererID));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, arrsize * sizeof(float), data, GL_STATIC_DRAW));
		LOG_TRACE("New OpenGLVertexBuffer, id: {}", m_RendererID);
	}
	 
	OpenGLVertexBuffer::~OpenGLVertexBuffer() 
	{
		GL_CALL(glDeleteVertexArrays(1, &m_VertexArrayID));
		GL_CALL(glDeleteBuffers(1, &m_RendererID));
	}

	void OpenGLVertexBuffer::Bind() const
	{
		GL_CALL(glBindVertexArray(m_VertexArrayID));
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		GL_CALL(glBindVertexArray(0));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void OpenGLVertexBuffer::SetData(const void* data, unsigned int size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGLVertexBuffer::SetLayout(const VertexBufferLayout& layout)
	{
		m_BufferLayout = layout;
		AddVertexBufferToVertexArray();
	}

	void OpenGLVertexBuffer::AddVertexBufferToVertexArray()
	{
		BindVertexArray();
		Bind();
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
		//glBindVertexArray(m_VertexArrayID);
		BindVertexArray();
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

	void OpenGLVertexBuffer::BindVertexArray()
	{
		GL_CALL(glBindVertexArray(m_VertexArrayID));
	}

	void OpenGLVertexBuffer::UnbindVertexArray()
	{
		GL_CALL(glBindVertexArray(0));
	}

	void OpenGLVertexBuffer::SetIndexBuffer(const s_ptr<IndexBuffer> indexBuffer)
	{
		BindVertexArray();
		Bind();
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}

}
#include "LKpch.h"
#include "OpenGLVertexBuffer.h"

#include "OpenGLIndexBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	/// MOVE
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


	OpenGLVertexBuffer::OpenGLVertexBuffer(void* InBuffer, 
										   const uint64_t InSize, 
										   const EVertexBufferUsage InBufferUsage)
		: m_Size(InSize)
		, m_Usage(InBufferUsage)
	{
		m_LocalData = FBuffer(InBuffer, InSize);

		glGenVertexArrays(1, &m_VertexArrayID);
		glBindVertexArray(m_VertexArrayID);

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, InSize, InBuffer, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const uint64_t InSize, 
										   const EVertexBufferUsage InBufferUsage)
		: m_Size(InSize)
		, m_Usage(InBufferUsage)
	{
		m_LocalData.Allocate(InSize);

		LK_OpenGL(glGenVertexArrays(1, &m_VertexArrayID));
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));

		LK_OpenGL(glGenBuffers(1, &m_RendererID));
		LK_OpenGL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glBufferData(GL_ARRAY_BUFFER, InSize, nullptr, GL_DYNAMIC_DRAW));
	}
 
	OpenGLVertexBuffer::~OpenGLVertexBuffer() 
	{
		LK_OpenGL(glDeleteVertexArrays(1, &m_VertexArrayID));
		LK_OpenGL(glDeleteBuffers(1, &m_RendererID));
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindVertexArray(m_VertexArrayID);
	}

	void OpenGLVertexBuffer::SetData(void* InData, uint64_t InSize, uint64_t InOffset)
	{
		m_LocalData.Data = InData;
		LK_OpenGL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glBufferSubData(GL_ARRAY_BUFFER, InOffset, InSize, InData));
	}

	void OpenGLVertexBuffer::RT_SetData(void* InData, uint64_t InSize, uint64_t offset)
	{
		m_LocalData.Data = InData;
		LK_OpenGL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glBufferSubData(GL_ARRAY_BUFFER, offset, InSize, InData));
	}

	void OpenGLVertexBuffer::SetLayout(const VertexBufferLayout& layout)
	{
		m_BufferLayout = layout;
		AddVertexBufferToVertexArray();
	}

	void OpenGLVertexBuffer::AddVertexBufferToVertexArray()
	{
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));
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

	void OpenGLVertexBuffer::AddVertexBuffer(LVertexBuffer& VertexBuffer)
	{
		glBindVertexArray(m_VertexArrayID);
		VertexBuffer.Bind();
		VertexBufferLayout& layout = VertexBuffer.GetLayout();

		for (const VertexBufferElement& element : layout)
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
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));
	}

	void OpenGLVertexBuffer::UnbindVertexArray()
	{
		LK_OpenGL(glBindVertexArray(0));
	}
#endif

	void OpenGLVertexBuffer::SetIndexBuffer(const TObjectPtr<LIndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_VertexArrayID);
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}

}
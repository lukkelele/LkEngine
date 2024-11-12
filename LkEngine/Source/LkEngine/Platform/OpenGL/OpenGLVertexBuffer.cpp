#include "LKpch.h"
#include "OpenGLVertexBuffer.h"

#include "OpenGLIndexBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* InBuffer, const uint64_t InSize, 
										   const EVertexBufferUsage InBufferUsage)
		: m_Size(InSize)
		, m_Usage(InBufferUsage)
	{
		m_LocalData = FBuffer(InBuffer, InSize);

		LK_OpenGL(glGenVertexArrays(1, &m_VertexArrayID));
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));

		LK_OpenGL(glGenBuffers(1, &m_RendererID));
		LK_OpenGL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glBufferData(GL_ARRAY_BUFFER, InSize, InBuffer, GL_DYNAMIC_DRAW));
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const uint64_t InSize, const EVertexBufferUsage InBufferUsage)
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
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));
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

	void OpenGLVertexBuffer::SetLayout(const VertexBufferLayout& Layout)
	{
		m_BufferLayout = Layout;
		AddVertexBufferToVertexArray();
	}

	void OpenGLVertexBuffer::AddVertexBufferToVertexArray()
	{
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));

		for (const FVertexBufferElement& Element : m_BufferLayout)
		{
			switch (Element.Type)
			{
				case EShaderDataType::Float:
				case EShaderDataType::Float2:
				case EShaderDataType::Float3:
				case EShaderDataType::Float4:
				{
					LK_OpenGL(glEnableVertexAttribArray(m_VertexBufferIndex));
					LK_OpenGL(glVertexAttribPointer(m_VertexBufferIndex,
													Element.GetComponentCount(),
													LOpenGL::ShaderDataTypeToOpenGLBaseType(Element.Type),
													(Element.Normalized ? GL_TRUE : GL_FALSE),
													m_BufferLayout.GetStride(),
													(const void*)Element.Offset));
					m_VertexBufferIndex++;
					break;
				}
				case EShaderDataType::Int:
				case EShaderDataType::Int2:
				case EShaderDataType::Int3:
				case EShaderDataType::Int4:
				case EShaderDataType::Bool:
				{
					LK_OpenGL(glEnableVertexAttribArray(m_VertexBufferIndex));
					LK_OpenGL(glVertexAttribIPointer(m_VertexBufferIndex,
													 Element.GetComponentCount(),
													 LOpenGL::ShaderDataTypeToOpenGLBaseType(Element.Type),
													 m_BufferLayout.GetStride(),
													 (const void*)Element.Offset));
					m_VertexBufferIndex++;
					break;
				}
				case EShaderDataType::Mat3:
				case EShaderDataType::Mat4:
				{
					uint8_t count = Element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						LK_OpenGL(glEnableVertexAttribArray(m_VertexBufferIndex));
						LK_OpenGL(glVertexAttribPointer(m_VertexBufferIndex,
														count,
														LOpenGL::ShaderDataTypeToOpenGLBaseType(Element.Type),
														(Element.Normalized ? GL_TRUE : GL_FALSE),
														m_BufferLayout.GetStride(),
														(const void*)(Element.Offset + sizeof(float) * count * i)));
						LK_OpenGL(glVertexAttribDivisor(m_VertexBufferIndex, 1));
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
		VertexBufferLayout& Layout = VertexBuffer.GetLayout();

		for (const FVertexBufferElement& Element : Layout)
		{
			switch (Element.Type)
			{
				case EShaderDataType::Float:
				case EShaderDataType::Float2:
				case EShaderDataType::Float3:
				case EShaderDataType::Float4:
				{
					LK_OpenGL(glEnableVertexAttribArray(m_VertexBufferIndex));
					LK_OpenGL(glVertexAttribPointer(m_VertexBufferIndex,
													Element.GetComponentCount(),
													LOpenGL::ShaderDataTypeToOpenGLBaseType(Element.Type),
													Element.Normalized ? GL_TRUE : GL_FALSE,
													Layout.GetStride(),
													(const void*)Element.Offset));
					m_VertexBufferIndex++;
					break;
				}
				case EShaderDataType::Int:
				case EShaderDataType::Int2:
				case EShaderDataType::Int3:
				case EShaderDataType::Int4:
				case EShaderDataType::Bool:
				{
					LK_OpenGL(glEnableVertexAttribArray(m_VertexBufferIndex));
					LK_OpenGL(glVertexAttribIPointer(m_VertexBufferIndex, 
													 Element.GetComponentCount(),
													 LOpenGL::ShaderDataTypeToOpenGLBaseType(Element.Type),
													 Layout.GetStride(),
													 (const void*)Element.Offset));
					m_VertexBufferIndex++;
					break;
				}
				case EShaderDataType::Mat3:
				case EShaderDataType::Mat4:
				{
					uint8_t count = Element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						LK_OpenGL(glEnableVertexAttribArray(m_VertexBufferIndex));
						LK_OpenGL(glVertexAttribPointer(m_VertexBufferIndex,
														count,
														LOpenGL::ShaderDataTypeToOpenGLBaseType(Element.Type),
														Element.Normalized ? GL_TRUE : GL_FALSE,
														Layout.GetStride(),
														(const void*)(Element.Offset + sizeof(float) * count * i)));
						LK_OpenGL(glVertexAttribDivisor(m_VertexBufferIndex, 1));
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
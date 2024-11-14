#include "LKpch.h"
#include "OpenGLVertexBuffer.h"

#include "OpenGLIndexBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	LOpenGLVertexBuffer::LOpenGLVertexBuffer(void* InBuffer, const uint64_t InSize, 
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

	LOpenGLVertexBuffer::LOpenGLVertexBuffer(const uint64_t InSize, const EVertexBufferUsage InBufferUsage)
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
 
	LOpenGLVertexBuffer::~LOpenGLVertexBuffer() 
	{
		LK_OpenGL(glDeleteVertexArrays(1, &m_VertexArrayID));
		LK_OpenGL(glDeleteBuffers(1, &m_RendererID));
	}

	void LOpenGLVertexBuffer::Bind() const
	{
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));
	}

	void LOpenGLVertexBuffer::SetData(void* InData, uint64_t InSize, uint64_t InOffset)
	{
		m_LocalData.Data = InData;
		LK_OpenGL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glBufferSubData(GL_ARRAY_BUFFER, InOffset, InSize, InData));
	}

	void LOpenGLVertexBuffer::RT_SetData(void* InData, uint64_t InSize, uint64_t offset)
	{
		m_LocalData.Data = InData;
		LK_OpenGL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
		LK_OpenGL(glBufferSubData(GL_ARRAY_BUFFER, offset, InSize, InData));
	}

	void LOpenGLVertexBuffer::SetLayout(const FVertexBufferLayout& InLayout)
	{
		Layout = InLayout;
		AddVertexBufferToVertexArray();
	}

	void LOpenGLVertexBuffer::AddVertexBufferToVertexArray()
	{
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));
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
													(Element.Normalized ? GL_TRUE : GL_FALSE),
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
														(Element.Normalized ? GL_TRUE : GL_FALSE),
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

	void LOpenGLVertexBuffer::AddVertexBuffer(LVertexBuffer& VertexBuffer)
	{
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));
		VertexBuffer.Bind();
		FVertexBufferLayout& VertexBufferLayout = VertexBuffer.GetLayout();

		for (const FVertexBufferElement& Element : VertexBufferLayout)
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
													VertexBufferLayout.GetStride(),
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
													 VertexBufferLayout.GetStride(),
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
														VertexBufferLayout.GetStride(),
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
	void LOpenGLVertexBuffer::BindVertexArray()
	{
		LK_OpenGL(glBindVertexArray(m_VertexArrayID));
	}

	void LOpenGLVertexBuffer::UnbindVertexArray()
	{
		LK_OpenGL(glBindVertexArray(0));
	}
#endif

	void LOpenGLVertexBuffer::SetIndexBuffer(const TObjectPtr<LIndexBuffer> InIndexBuffer)
	{
		glBindVertexArray(m_VertexArrayID);
		InIndexBuffer->Bind();
		m_IndexBuffer = InIndexBuffer;
	}

}
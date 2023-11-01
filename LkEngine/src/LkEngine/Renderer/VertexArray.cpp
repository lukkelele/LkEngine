#include "LKpch.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/VertexArray.h"
#include "LkEngine/Renderer/VertexBufferLayout.h"


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

	VertexArray::VertexArray()
	{
		GL_CALL(glGenVertexArrays(1, &m_RendererID));
		GL_CALL(glBindVertexArray(m_RendererID));
		LOG_TRACE("New VertexArray, id: {}", m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		GL_CALL(glDeleteVertexArrays(1, &m_RendererID));
	}

	// Adds buffer and configures appropriate layout 
	void VertexArray::AddVertexBuffer(VertexBuffer& vb)
	{ 
		glBindVertexArray(m_RendererID);
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
					LOG_DEBUG("ShaderDataType::Float1/2/3/4");
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
		//m_VertexBuffers.push_back(std::make_shared<VertexBuffer>(&vb));
		m_VertexBuffers.push_back(std::shared_ptr<VertexBuffer>(&vb));
	}

	void VertexArray::Bind() const
	{
		GL_CALL(glBindVertexArray(m_RendererID));
	}

	void VertexArray::Unbind() const
	{
		GL_CALL(glBindVertexArray(0));
	}

	void VertexArray::SetIndexBuffer(const s_ptr<IndexBuffer> ib)
	{
		glBindVertexArray(m_RendererID);
		ib->Bind();
		m_IndexBuffer = ib;
	}

}

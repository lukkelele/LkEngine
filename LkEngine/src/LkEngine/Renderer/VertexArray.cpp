#include "LKpch.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/VertexArray.h"
#include "LkEngine/Renderer/VertexBufferLayout.h"


namespace LkEngine {

	VertexArray::VertexArray()
	{
		GL_CALL(glGenVertexArrays(1, &m_RendererID));
		GL_CALL(glBindVertexArray(m_RendererID));
	}

	VertexArray::~VertexArray()
	{
		GL_CALL(glDeleteVertexArrays(1, &m_RendererID));
	}

	// Adds buffer and configures appropriate layout 
	void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
	{ 
		Bind();
		vb.Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (unsigned int i = 0 ; i < elements.size(); i++) {
			const auto& element = elements[i];
			GL_CALL(glEnableVertexAttribArray(i));
			GL_CALL(glVertexAttribPointer(i, element.count, element.type, element.normalized, 
											layout.GetStride(), (const void*) offset));
			offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		}
	}

	void VertexArray::Bind() const
	{
		GL_CALL(glBindVertexArray(m_RendererID));
	}

	void VertexArray::Unbind() const
	{
		GL_CALL(glBindVertexArray(0));
	}

}

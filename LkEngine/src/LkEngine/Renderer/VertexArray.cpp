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

	void VertexArray::AddVertexBuffer(VertexBuffer& vb)
	{
		Bind();
		vb.Bind();
		VertexBufferLayout layout = vb.GetLayout();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (unsigned int i = 0; i < elements.size(); i++) 
		{
			const auto& element = elements[i];
			GL_CALL(glEnableVertexAttribArray(i));
			GL_CALL(glVertexAttribPointer(i, element.Count, ShaderDataTypeSize(element.Type), element.Normalized, layout.GetStride(), (const void*)offset));
			//offset += element.Count * VertexBufferElement::GetSizeOfType(element.Type);
			//offset += element.Count * VertexBufferElement::GetSizeOfType(ShaderDataTypeSize(element.Type));
			//offset += ShaderDataTypeSize(element.Type) * 4;
			offset += element.Offset;
			LOG_TRACE("Offset: {}", offset);
		}
	}

	void VertexArray::AddVertexBuffer(s_ptr<VertexBuffer> vb)
	{
		Bind();
		vb->Bind();
		VertexBufferLayout layout = vb->GetLayout();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (unsigned int i = 0 ; i < elements.size(); i++) 
		{
			const auto& element = elements[i];
			GL_CALL(glEnableVertexAttribArray(i));
			GL_CALL(glVertexAttribPointer(i, element.Count, ShaderDataTypeSize(element.Type), element.Normalized, 
											layout.GetStride(), (const void*) offset));
			//offset += element.Count * VertexBufferElement::GetSizeOfType(element.Type);
			//offset += ShaderDataTypeSize(element.Type);
			offset += element.Offset;
			LOG_TRACE("Offset: {}", offset);
		}
	}

	void VertexArray::AddVertexBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
	{ 
		Bind();
		vb.Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (unsigned int i = 0 ; i < elements.size(); i++) {
			const auto& element = elements[i];
			GL_CALL(glEnableVertexAttribArray(i));
			GL_CALL(glVertexAttribPointer(i, element.Count, ShaderDataTypeSize(element.Type), element.Normalized, 
											layout.GetStride(), (const void*) offset));
			//offset += element.Count * VertexBufferElement::GetSizeOfType(element.Type);
			//offset += ShaderDataTypeSize(element.Type);
			offset += element.Offset;
			LOG_TRACE("Offset: {}", offset);
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

	void VertexArray::SetIndexBuffer(const s_ptr<IndexBuffer> ib)
	{
		glBindVertexArray(m_RendererID);
		ib->Bind();
		m_IndexBuffer = ib;
	}

}

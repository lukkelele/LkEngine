#include "LKpch.h"
#include "OpenGLUniformBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	LOpenGLUniformBuffer::LOpenGLUniformBuffer(const uint64_t size)
	{
		LK_OpenGL(glCreateBuffers(1, &m_ID));

		LK_OpenGL(glNamedBufferData(m_ID, size, nullptr, GL_DYNAMIC_DRAW)); 
		LK_OpenGL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_ID));
	}

	LOpenGLUniformBuffer::~LOpenGLUniformBuffer()
	{
		LK_OpenGL(glDeleteBuffers(1, &m_ID));
	}

	void LOpenGLUniformBuffer::SetData(const void* data, uint64_t size, uint64_t offset)
	{
		LK_OpenGL(glBindBuffer(GL_UNIFORM_BUFFER, m_ID));
		LK_OpenGL(glNamedBufferSubData(m_ID, offset, size, data));
		LK_OpenGL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	void LOpenGLUniformBuffer::RT_SetData(const void* data, uint64_t size, uint64_t offset)
	{
		LK_OpenGL(glBindBuffer(GL_UNIFORM_BUFFER, m_ID));
		LK_OpenGL(glNamedBufferSubData(m_ID, offset, size, data));
		LK_OpenGL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	void LOpenGLUniformBuffer::Bind()
	{
		LK_OpenGL(glBindBuffer(GL_UNIFORM_BUFFER, m_ID));
	}

	void LOpenGLUniformBuffer::Unbind()
	{
		LK_OpenGL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	void LOpenGLUniformBuffer::SetBinding(const TObjectPtr<LShader> Shader, std::string_view ubName, const uint32_t BlockIndex)
	{
		//uint32_t UBIndex = glGetUniformBlockIndex(Shader->GetRendererID(), std::string(ubName).c_str());
		uint32_t UBIndex;
		LK_OpenGL(UBIndex = glGetUniformBlockIndex(Shader->GetRendererID(), std::string(ubName).c_str()));
		if (UBIndex == BlockIndex)
		{
			LK_CORE_DEBUG("Uniformbuffer is already bound to {} for block \"{}\"", BlockIndex, ubName);
			return;
		}

		LK_OpenGL(glUniformBlockBinding(Shader->GetRendererID(), UBIndex, BlockIndex));
		LK_CORE_DEBUG_TAG("OpenGLUniformBuffer", "Updated buffer binding, block index is now set to {} for block {}", BlockIndex, ubName);
		//glUniformBlockBinding(Shader->GetRendererID(), m_ID, BlockIndex);
	}


}
#include "LKpch.h"
#include "OpenGLUniformBuffer.h"

#include "LkOpenGL.h"


namespace LkEngine {

	LOpenGLUniformBuffer::LOpenGLUniformBuffer(const uint64_t size)
	{
		glCreateBuffers(1, &m_ID);

		glNamedBufferData(m_ID, size, nullptr, GL_DYNAMIC_DRAW); 
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_ID);
	}

	LOpenGLUniformBuffer::~LOpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void LOpenGLUniformBuffer::SetData(const void* data, uint64_t size, uint64_t offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
		glNamedBufferSubData(m_ID, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void LOpenGLUniformBuffer::RT_SetData(const void* data, uint64_t size, uint64_t offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
		glNamedBufferSubData(m_ID, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void LOpenGLUniformBuffer::Bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	}

	void LOpenGLUniformBuffer::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void LOpenGLUniformBuffer::SetBinding(const TObjectPtr<LShader>& targetShader, 
										  std::string_view ubName, 
										  const uint32_t blockIndex)
	{
		uint32_t ubIndex = glGetUniformBlockIndex(targetShader->GetRendererID(), std::string(ubName).c_str());
		if (ubIndex == blockIndex)
		{
			LK_CORE_DEBUG("Uniformbuffer is already bound to {} for block \"{}\"", blockIndex, ubName);
			return;
		}

		glUniformBlockBinding(targetShader->GetRendererID(), ubIndex, blockIndex);
		LK_CORE_DEBUG_TAG("OpenGLUniformBuffer", "Updated buffer binding, block index is now set to {} for block {}", blockIndex, ubName);
		//glUniformBlockBinding(targetShader->GetRendererID(), m_ID, blockIndex);
	}


}
#pragma once

#include "LkEngine/Renderer/VertexBuffer.h"


namespace LkEngine {

    class OpenGLVertexBuffer : public VertexBuffer
    {
	public:
		OpenGLVertexBuffer(const void* data, unsigned int size);
		OpenGLVertexBuffer(unsigned int size);
		~OpenGLVertexBuffer();

		void Bind() const;
		void Unbind() const;
		void SetData(const void* data, unsigned int size);

    };

}
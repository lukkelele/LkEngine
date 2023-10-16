#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Renderer/VertexArray.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/IndexBuffer.h"
#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Scene/Components.h"
#include <glad/glad.h>

#define LK_DRAW_TRIANGLES  GL_TRIANGLES
#define LK_DRAW_LINES	   GL_LINES


namespace LkEngine {

	class Entity;

	class Renderer
	{
	public:
		static void Clear();

		static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		static void Draw(const MeshComponent& mesh);
		static void DrawTriangles(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		static void DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		static void DrawIndexed(const s_ptr<VertexArray>& va);
		static void SetDrawMode(int mode);


	public:
		static int DrawMode;
		static glm::vec4 BackgroundColor;
	};

}

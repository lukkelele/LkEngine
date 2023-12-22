#include "LKpch.h"
#include "LkEngine/Renderer/Shader.h"
#include <glad/glad.h>

#ifdef LK_RENDERER_API_VULKAN
	#include "LkEngine/Renderer/Vulkan/VulkanShader.h"
#elif defined(LK_RENDERER_API_OPENGL)
	#include "LkEngine/Renderer/OpenGL/OpenGLShader.h"
#endif

namespace LkEngine {

	s_ptr<Shader> Shader::Create(const std::string& filePath)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanShader>(filePath);
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLShader>(filePath);
	#else
		LK_ASSERT(false);
		return nullptr;
	#endif
	}

	s_ptr<Shader> Shader::Create(const std::string& vertexPath, const std::string& fragmentPath)
	{
	#ifdef LK_RENDERER_API_VULKAN
		return std::make_shared<VulkanShader>(vertexPath, fragmentPath);
	#elif defined(LK_RENDERER_API_OPENGL)
		return std::make_shared<OpenGLShader>(vertexPath, fragmentPath);
	#else
		LK_ASSERT(false);
		return nullptr;
	#endif
	}

	ShaderProgramSource Shader::ParseShader(const std::string& filepath)
	{
		enum class ShaderType
		{
			NONE = -1,
			VERTEX = 0,
			FRAGMENT = 1
		};

		std::ifstream stream(filepath);
		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::NONE;

		while (getline(stream, line))
		{
	#ifdef LK_ENGINE_PRINT_SHADER
			printf("%s\n", line.c_str());
	#endif
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
			} 
			else
			{	// Use ShaderType to append lines appropriately 
				if (type != ShaderType::NONE)
					ss[(int)type] << line << '\n';
			}
		}

		std::string vertex_str = ss[0].str();
		std::string frag_str = ss[1].str();
		if (vertex_str.empty())
			LOG_ERROR("Parsed vertex shader is empty!");
		if (frag_str.empty())
			LOG_ERROR("Parsed fragment shader is empty!");

		return { vertex_str, frag_str };
	}

	ShaderProgramSource Shader::ParseShaders(const std::string& vertexPath, const std::string& fragmentPath)
	{
		enum class ShaderType
		{
			NONE = -1,
			VERTEX = 0,
			FRAGMENT = 1
		};

		std::ifstream streamVertex(vertexPath);
		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::VERTEX;

		while (getline(streamVertex, line))
		{
	#ifdef LK_ENGINE_PRINT_SHADER
			printf("%s\n", line.c_str());
	#endif
			ss[(int)type] << line << '\n';
		}

		std::ifstream streamFrag(fragmentPath);
		type = ShaderType::FRAGMENT;
		while (getline(streamFrag, line))
		{
	#ifdef LK_ENGINE_PRINT_SHADER
			printf("%s\n", line.c_str());
	#endif
			ss[(int)type] << line << '\n';
		}

		std::string vertex_str = ss[0].str();
		std::string frag_str = ss[1].str();
		if (vertex_str.empty())
			LOG_ERROR("Parsed vertex shader is empty!");
		if (frag_str.empty())
			LOG_ERROR("Parsed fragment shader is empty!");

		return { vertex_str, frag_str };
	}


	//============================================================================
	// ShaderLibrary
	//
	//============================================================================

    ShaderLibrary::ShaderLibrary()
    {
    }

    ShaderLibrary::~ShaderLibrary()
    {
    }

    void ShaderLibrary::Add(const s_ptr<Shader>& shader)
    {
    }

    void ShaderLibrary::Load(std::string_view name, const std::string& path)
    {
		//m_Shaders[std::string(name)] = Shader::Create(path);
		m_Shaders[name] = Shader::Create(path);
    }

    s_ptr<Shader>& ShaderLibrary::Get(std::string_view shaderName)
    {
		LK_ASSERT(m_Shaders.find(shaderName) != m_Shaders.end());
		return m_Shaders.at(shaderName);
    }


}

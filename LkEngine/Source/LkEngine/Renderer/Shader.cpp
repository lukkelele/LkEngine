#include "LKpch.h"
#include "Shader.h"
//#include <glad/glad.h>

#include "LkEngine/Serialization/FileStream.h"
#include "LkEngine/Serialization/StreamReader.h"
#include "LkEngine/Serialization/StreamWriter.h"

#include "LkEngine/Platform/OpenGL/OpenGLShader.h"
//#include "LkEngine/Platform/Vulkan/VulkanShader.h"


#include "RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LShader> LShader::Create(const std::string& InFilepath)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL:
			{
				return TObjectPtr<OpenGLShader>::Create(InFilepath);
			}

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LShader> LShader::Create(const std::string& vertexPath, const std::string& fragmentPath)
	{
		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<OpenGLShader>::Create(vertexPath, fragmentPath);

			case ERendererAPI::None: break;
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	ShaderProgramSource LShader::ParseShader(const std::string& filepath)
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
				{
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos)
				{
					type = ShaderType::FRAGMENT;
				}
			} 
			else
			{	// Use ShaderType to append lines appropriately 
				if (type != ShaderType::NONE)
				{
					ss[(int)type] << line << '\n';
				}
			}
		}

		std::string vertex_str = ss[0].str();
		std::string frag_str = ss[1].str();

		/* Check for errors */
		if (vertex_str.empty())
		{
			LK_CORE_ERROR("Parsed vertex shader is empty!");
		}
		if (frag_str.empty())
		{
			LK_CORE_ERROR("Parsed fragment shader is empty!");
		}

		return { vertex_str, frag_str };
	}

	ShaderProgramSource LShader::ParseShaders(const std::string& vertexPath, const std::string& fragmentPath)
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
			ss[(int)type] << line << '\n';
		}

		std::ifstream streamFrag(fragmentPath);
		type = ShaderType::FRAGMENT;
		while (getline(streamFrag, line))
		{
			ss[(int)type] << line << '\n';
		}

		std::string vertex_str = ss[0].str();
		std::string frag_str = ss[1].str();
		LK_CORE_ASSERT(vertex_str.empty(), "Parsed vertex shader is empty!");
		LK_CORE_ASSERT(frag_str.empty(), "Parsed fragment shader is empty!");

		return { vertex_str, frag_str };
	}



	//============================================================================
	// ShaderLibrary
	//============================================================================
    void LShaderLibrary::Add(const TObjectPtr<LShader>& shader)
    {
    }

    void LShaderLibrary::Load(std::string_view name, const std::string& path)
    {
		Shaders[name] = LShader::Create(path);
    }

    TObjectPtr<LShader>& LShaderLibrary::Get(std::string_view ShaderName)
    {
		LK_ASSERT((Shaders.find(ShaderName) != Shaders.end()));
		return Shaders.at(ShaderName);
    }

}

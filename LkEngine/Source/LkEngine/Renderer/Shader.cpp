#include "LKpch.h"
#include "Shader.h"

#include "LkEngine/Serialization/FileStream.h"
#include "LkEngine/Serialization/StreamReader.h"
#include "LkEngine/Serialization/StreamWriter.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLShader.h"


#include "RendererAPI.h"


namespace LkEngine {

	TObjectPtr<LShader> LShader::Create(const std::string& InFilePath)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL:
			{
				return TObjectPtr<LOpenGLShader>::Create(InFilePath);
			}

			case ERendererAPI::None:
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	TObjectPtr<LShader> LShader::Create(const FShaderProgramSource& ShaderProgramSource)
	{
		switch (LRendererAPI::Get())
		{
			case ERendererAPI::OpenGL: return TObjectPtr<LOpenGLShader>::Create(ShaderProgramSource);

			case ERendererAPI::None:
			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid Render API");
		return nullptr;
	}

	bool LShader::ParseShader(FShaderProgramSource& ShaderProgramSource, const std::filesystem::path& InFilePath)
	{
		std::ifstream InputStream(InFilePath);

		std::stringstream ss[2];
		std::string Line;

		EShaderType ShaderType = EShaderType::None;
		while (getline(InputStream, Line))
		{
		#ifdef LK_ENGINE_PRINT_SHADER
			printf("%s\n", Line.c_str());
		#endif
			if (Line.find("#shader") != std::string::npos)
			{
				if (Line.find("vertex") != std::string::npos)
				{
					ShaderType = EShaderType::Vertex;
				}
				else if (Line.find("fragment") != std::string::npos)
				{
					ShaderType = EShaderType::Fragment;
				}
			} 
			else
			{	
				if (ShaderType != EShaderType::None)
				{
					ss[static_cast<int>(ShaderType)] << Line << '\n';
				}
			}
		}

		ShaderProgramSource.Vertex = ss[0].str();
		ShaderProgramSource.Fragment = ss[1].str();

		return ShaderProgramSource.IsValid();
	}

	bool LShader::ParseShaders(FShaderProgramSource& ShaderProgramSource, 
							   const std::filesystem::path& InVertexPath, 
							   const std::filesystem::path& InFragmentPath)
	{
		if (!std::filesystem::exists(InVertexPath) || !std::filesystem::exists(InFragmentPath))
		{
			LK_CORE_WARN_TAG("Shader", "Paths to vertex and/or fragment shader are invalid\n Vertex: {}\n Fragment: {}", 
							 InVertexPath.string(), InFragmentPath.string());
			return false;
		}

		std::ifstream InputStreamVertex(InVertexPath);
		std::string Line{};

		std::stringstream ss[2];

		/* Vertex Shader. */
		EShaderType Type = EShaderType::Vertex;
		while (getline(InputStreamVertex, Line))
		{
			ss[static_cast<int>(Type)] << Line << '\n';
		}

		/* Fragment Shader. */
		std::ifstream InputStreamFrag(InFragmentPath);
		Type = EShaderType::Fragment;
		while (getline(InputStreamFrag, Line))
		{
			ss[static_cast<int>(Type)] << Line << '\n';
		}

		ShaderProgramSource.Vertex = ss[0].str();
		ShaderProgramSource.Fragment = ss[1].str();

		return ShaderProgramSource.IsValid();
	}


	/***********************************************************************/

	LShaderLibrary::LShaderLibrary()
	{
		LCLASS_REGISTER();
	}

    void LShaderLibrary::Add(const TObjectPtr<LShader>& Shader)
    {
		LK_UNUSED(Shader);
		LK_MARK_FUNC_NOT_IMPLEMENTED();
    }

    void LShaderLibrary::Load(std::string_view Name, const std::string& InFilePath)
    {
		Shaders[Name] = LShader::Create(InFilePath);
    }

    TObjectPtr<LShader>& LShaderLibrary::Get(std::string_view ShaderName)
    {
		LK_ASSERT((Shaders.find(ShaderName) != Shaders.end()));
		return Shaders.at(ShaderName);
    }

}

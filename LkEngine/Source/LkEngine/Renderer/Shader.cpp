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
		std::string Line;

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

	std::pair<EUniformVarType, std::string> LShader::GetUniformVariable(const std::string& InString)
	{
		/* Ignore lines that do not 'uniform' in them or if they are commented. */
		const std::size_t StartPos = InString.find("uniform");
		if ((StartPos == std::string::npos) || (StartPos > 0))
		{
			return { EUniformVarType::None, "" };
		}

		static constexpr TStringLiteral Bool = Core::TStringLiteral("bool");
		static constexpr TStringLiteral Float = Core::TStringLiteral("float");
		static constexpr TStringLiteral Vec3 = Core::TStringLiteral("vec3");
		static constexpr TStringLiteral Sampler2d = Core::TStringLiteral("sampler2D");
		static constexpr TStringLiteral Sampler2dArray = Core::TStringLiteral("sampler2DArray");

		/* Uniform: Bool */
		if (const std::size_t Pos = InString.find(Bool.View()); Pos != std::string::npos)
		{
			std::string UniformName = InString.substr(Pos + Bool.Size());
			std::erase_if(UniformName, [](const char Character) { return Character == ';'; });
			return { EUniformVarType::Bool, UniformName };
		}

		/* Uniform: Float */
		if (const std::size_t Pos = InString.find(Float.View()); Pos != std::string::npos)
		{
			std::string UniformName = InString.substr(Pos + Float.Size());
			std::erase_if(UniformName, [](const char Character) { return Character == ';'; });
			return { EUniformVarType::Float, UniformName };
		}

		/* Uniform: Vec3 */
		if (const std::size_t Pos = InString.find(Vec3.View()); Pos != std::string::npos)
		{
			std::string UniformName = InString.substr(Pos + Vec3.Size());
			std::erase_if(UniformName, [](const char Character) { return Character == ';'; });
			return { EUniformVarType::Vec3, UniformName };
		}

		/* Uniform: Sampler2DArray */
		if (const std::size_t Pos = InString.find(Sampler2dArray.View()); Pos != std::string::npos)
		{
			std::string UniformName = InString.substr(Pos + Sampler2dArray.Size());
			std::erase_if(UniformName, [](const char Character) { return Character == ';'; });
			return { EUniformVarType::Sampler2DArray, UniformName };
		}

		/* Uniform: Sampler2D */
		if (const std::size_t Pos = InString.find(Sampler2d.View()); Pos != std::string::npos)
		{
			std::string UniformName = InString.substr(Pos + Sampler2d.Size());
			std::erase_if(UniformName, [](const char Character) { return Character == ';'; });
			return { EUniformVarType::Sampler2D, UniformName };
		}

		return { EUniformVarType::None, "" };
	}

	/***********************************************************************/

	LShaderLibrary::LShaderLibrary()
	{
		LOBJECT_REGISTER();
	}

	TObjectPtr<LShader>& LShaderLibrary::Get(const std::string& ShaderName)
	{
		LK_CORE_ASSERT(ShaderMap.contains(ShaderName), "Shader '{}' is not loaded", ShaderName);
		return ShaderMap.at(ShaderName);
	}

	void LShaderLibrary::Add(const TObjectPtr<LShader>& Shader)
    {
		LK_MARK_FUNC_NOT_IMPLEMENTED();
		LK_UNUSED(Shader);
    }

    void LShaderLibrary::Load(const std::string& ShaderName, const std::filesystem::path& InFilePath)
    {
		LK_CORE_ASSERT(!ShaderMap.contains(ShaderName), "Shader '{}' is already loaded", ShaderName);
		LK_CORE_DEBUG("Loading shader: {}", ShaderName);
		ShaderMap[ShaderName] = LShader::Create(InFilePath.string());
    }

}

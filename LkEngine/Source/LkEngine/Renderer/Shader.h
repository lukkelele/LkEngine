#pragma once

#include "LkEngine/Core/Hash.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "ShaderUniform.h"
#include "ShaderResource.h"

#include "Texture.h"


namespace LkEngine {

	/**
	 * @enum EShaderType
	 *
	 *  Type of shader, used on parsing of files during shader setup.
	 */
	enum class EShaderType
	{
		None = -1,
		Vertex,
		Fragment
	};

	/**
	 * EUniformVarType
	 * 
	 *  The type of an uniform variable in a glsl shader.
	 */
	enum class EUniformVarType
	{
		None = 0,
		Bool,
		Float,
		Vec3,
		Sampler2D,
		Sampler2DArray,
	};

	class LShader : public LObject
	{
	public:
		virtual ~LShader() = default;

		static TObjectPtr<LShader> Create(const std::string& InFilepath); 
		static TObjectPtr<LShader> Create(const FShaderProgramSource& ShaderProgramSource);

		virtual LRendererID GetRendererID() const = 0;
		virtual LRendererID& GetRendererID() = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Get(std::string_view Uniform, int& Value) = 0;
		virtual void Get(std::string_view Uniform, bool& Value) = 0;
		virtual void Get(std::string_view Uniform, float& Value) = 0;
		virtual void Get(std::string_view Uniform, uint32_t& Value) = 0;
		virtual void Get(std::string_view Uniform, glm::vec3& Value) = 0;
		virtual void Get(std::string_view Uniform, glm::vec4& Value) = 0;

		virtual void Set(std::string_view Uniform, int Value) = 0;
		virtual void Set(std::string_view Uniform, bool Value) = 0;
		virtual void Set(std::string_view Uniform, float Value) = 0;
		virtual void Set(std::string_view Uniform, uint32_t Value) = 0;
		virtual void Set(std::string_view Uniform, const glm::vec2& Value) = 0;
		virtual void Set(std::string_view Uniform, const glm::vec3& Value) = 0;
		virtual void Set(std::string_view Uniform, const glm::vec4& Value) = 0;
		virtual void Set(std::string_view Uniform, const glm::ivec2& Value) = 0;
		virtual void Set(std::string_view Uniform, const glm::ivec3& Value) = 0;
		virtual void Set(std::string_view Uniform, const glm::ivec4& Value) = 0;
		virtual void Set(std::string_view Uniform, const glm::mat4& Value) = 0;

		virtual uint32_t CompileShader(const uint32_t ShaderType, const std::string& Source) = 0;
		virtual uint32_t CreateShader(const FShaderProgramSource& ShaderProgramSource) = 0;

		bool ParseShader(FShaderProgramSource& ShaderProgramSource, const std::filesystem::path& InFilePath);
		bool ParseShaders(FShaderProgramSource& ShaderProgramSource, 
						  const std::filesystem::path& InVertexPath, 
						  const std::filesystem::path& InFragmentPath);

		virtual std::size_t GetHash() const = 0;
		virtual std::string GetName() const = 0;

		virtual const FShaderProgramSource& GetSource() const = 0;

		static const char* ConvertUniformType(const ETextureUniformType Type)
		{
			switch (Type)
			{
				case ETextureUniformType::Diffuse:          return "u_Diffuse";
				case ETextureUniformType::Specular:         return "u_Specular";
				case ETextureUniformType::Normal:           return "u_Normal";
				case ETextureUniformType::Height:           return "u_Height";
				case ETextureUniformType::DiffuseRoughness: return "u_DiffuseRoughness";
				case ETextureUniformType::Emissive:         return "u_Emissive";
			}

			LK_CORE_ASSERT(false, "Could not convert the TextureUniformType {}", static_cast<int>(Type));
			return nullptr;
		}

		/**
		 * Attempt to get a uniform variable from a string.
		 * Is used for parsing shader sources to retrieve the uniforms
		 * used for a specific shader.
		 */
		static std::pair<EUniformVarType, std::string> GetUniformVariable(const std::string& InString);

	private:
		LCLASS(LShader);
	};


    /**
     * LShaderLibrary
	 * 
	 *  Library for shaders.
     */
    class LShaderLibrary : public LObject
    {
    public:
		LShaderLibrary();
		~LShaderLibrary() = default;

        TObjectPtr<LShader>& Get(const std::string& ShaderName);

        void Add(const TObjectPtr<LShader>& InShader);
		void Load(const std::string& ShaderName, const std::filesystem::path& InFilepath);

		uint32_t GetShadersLoaded() const 
		{ 
			return static_cast<uint32_t>(ShaderMap.size()); 
		}

    private:
        std::unordered_map<std::string, TObjectPtr<LShader>> ShaderMap;

		friend class LMaterialEditor;

		LCLASS(LShaderLibrary);
    };


}

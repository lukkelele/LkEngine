#pragma once

#include "LkEngine/Core/Base.h"

#include <glm/glm.hpp>

#include "ShaderUniform.h"

#include "LkEngine/Serialization/FileStream.h"


namespace LkEngine {

	struct ShaderProgramSource
	{
		std::string VertexSource;
		std::string FragmentSource;
	};

	class ShaderResourceDeclaration
	{
	public:
		ShaderResourceDeclaration() = default;
		ShaderResourceDeclaration(const std::string& name, uint32_t set, uint32_t resourceRegister, uint32_t count)
			: m_Name(name)
			, m_Set(set)
			, m_Register(resourceRegister)
			, m_Count(count) { }

		virtual const std::string& GetName() const { return m_Name; }
		virtual uint32_t GetSet() const { return m_Set; }
		virtual uint32_t GetRegister() const { return m_Register; }
		virtual uint32_t GetCount() const { return m_Count; }

		static void Serialize(StreamWriter* serializer, const ShaderResourceDeclaration& instance)
		{
			serializer->WriteString(instance.m_Name);
			serializer->WriteRaw(instance.m_Set);
			serializer->WriteRaw(instance.m_Register);
			serializer->WriteRaw(instance.m_Count);
		}

		static void Deserialize(StreamReader* deserializer, ShaderResourceDeclaration& instance)
		{
			deserializer->ReadString(instance.m_Name);
			deserializer->ReadRaw(instance.m_Set);
			deserializer->ReadRaw(instance.m_Register);
			deserializer->ReadRaw(instance.m_Count);
		}

	private:
		std::string m_Name;
		uint32_t m_Set = 0;
		uint32_t m_Register = 0;
		uint32_t m_Count = 0;
	};

	using ShaderResourceList = std::vector<ShaderResourceDeclaration*>;


	class Shader : public RefCounted
	{
	public:
		virtual ~Shader() = default;

		static Ref<Shader> Create(const std::string& filepath); 
		static Ref<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath); 

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID& GetRendererID() = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Set(const std::string& name, int value) = 0;
		virtual void Set(const std::string& name, bool value) = 0;
		virtual void Set(const std::string& name, float value) = 0;
		virtual void Set(const std::string& name, uint32_t value) = 0;
		virtual void Set(const std::string& name, const glm::vec2& value) = 0;
		virtual void Set(const std::string& name, const glm::vec3& value) = 0;
		virtual void Set(const std::string& name, const glm::vec4& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec2& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec3& value) = 0;
		virtual void Set(const std::string& name, const glm::ivec4& value) = 0;
		virtual void Set(const std::string& name, const glm::mat4& value) = 0;

		virtual unsigned int CompileShader(unsigned int type, const std::string& source) = 0;
		virtual unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) = 0;

		ShaderProgramSource ParseShader(const std::string& filepath);
		ShaderProgramSource ParseShaders(const std::string& vertexPath, const std::string& fragmentPath);
	};


    class ShaderLibrary : public RefCounted
    {
    public:
        ShaderLibrary();
        ~ShaderLibrary();

		static Ref<ShaderLibrary> GetLibrary() { return m_Instance; }

        Ref<Shader>& Get(std::string_view shaderName);

        void Add(const Ref<Shader>& shader);
		void Load(std::string_view name, const std::string& path);

    private:
        std::unordered_map<std::string_view, Ref<Shader>> m_Shaders;

		inline static Ref<ShaderLibrary> m_Instance = nullptr;
    };


}

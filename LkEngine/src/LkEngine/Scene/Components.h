#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Math/Math.h"

#include "LkEngine/Renderer/Material.h"


namespace LkEngine{

	// Forward declaration
	class SceneCamera;

	struct IDComponent
	{
		UUID ID;
		bool Removable = true;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;
		bool Removable = true;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& Tag) : Tag(Tag) {}
	};

    struct TransformComponent
    {
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
		glm::quat Rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
		// TODO: Patch out the use of Rotation2D and just use the Rotation quaternion
		float Rotation2D = 0.0f;
		bool Removable = true;
		bool Static = false;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& Translation) : Translation(Translation) {}

		glm::vec3 GetTranslation() const { return Translation; }
		glm::vec3 GetScale() const { return Scale; }
		glm::quat GetRotation() const { return Rotation; }
		float GetRotation2D() const { return Rotation2D; }

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(Rotation)
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 GetInvTransform() const
		{
			glm::mat4 inv_translation = glm::translate(glm::mat4(1.0f), -Translation);
			glm::quat inv_rotation = glm::conjugate(Rotation);
			glm::mat4 inv_scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / Scale.x, 1.0f / Scale.y, 1.0f / Scale.z));
			return inv_scale * glm::toMat4(inv_rotation) * inv_translation;
		}

		void SetRotation(float degrees)
		{
			float angleRad = glm::radians(degrees);
			float qw = cos(angleRad / 2.0f);
			float qz = sin(angleRad / 2.0f);
			Rotation = glm::quat(qw, 0.0f, 0.0f, qz);
			//Rotation2D = angleRad; // FIXME
		}

		bool IsStatic() const { return Static; }
	};

    struct SpriteComponent
    {
        std::string FilePath;
        glm::vec2 Size;
        glm::vec4 Color; 
		bool Passthrough = false;
		bool Removable = true;

        SpriteComponent(const std::string& filepath, 
                        const glm::vec2& size = { 100.0f, 100.0f }, 
                        const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
            : FilePath(filepath)
            , Size(size)
            , Color(color) 
		{}

        SpriteComponent(const glm::vec2& size = { 0.0f, 0.0f },
                        const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
            : FilePath("")
            , Size(size)
            , Color(color) 
        {}

        float GetWidth() const { return Size.x; }
        float GetHeight() const { return Size.y; }
        glm::vec2 GetSize() const { return Size; }
		void SetSize(const glm::vec2& size) { Size = size; }
		void SetSize(float x, float y) { Size = { x, y }; }
		void SetPassthrough(bool passthrough) { Passthrough = passthrough; }
		bool IsPassthrough() const { return Passthrough; }
    };

	struct CameraComponent
	{
		enum class Type { Null = -1, Ortographic = 0 , Perspective = 1 };
		Type CameraType;

		SceneCamera* CameraRef = nullptr;
		bool Removable = true;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;

		operator SceneCamera& () { return *CameraRef; }
		operator const SceneCamera& () const { return *CameraRef; }
	};

	struct RigidBody2DComponent
	{
		enum class Type 
		{ 
			None = -1, 
			Static, 
			Dynamic, 
			Kinematic 
		};

		Type BodyType;
		bool FixedRotation = false;
		float Mass = 1.0f;
		float LinearDrag = 0.01f;
		float AngularDrag = 0.05f;
		float GravityScale = 1.0f;
		bool IsBullet = false;
		// Storage for runtime
		void* RuntimeBody = nullptr;
		bool Removable = true;

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent& other) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f,0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 1.0f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;
		bool Removable = true;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent& other) = default;
	};

	struct IOComponent
	{
		bool Draggable = true;
		bool Dragged = false;

		IOComponent() = default;
	};

	struct MaterialComponent
	{
		s_ptr<Material> m_Material = nullptr;

		s_ptr<Texture> GetTexture() { return m_Material->GetTexture(); }
		std::string GetTextureName() const 
		{
			if (m_Material->GetTexture() == nullptr)
				throw std::runtime_error("MaterialComponent: Cannot get texture name of a nullptr texture");
			return m_Material->GetTexture()->GetName();
		}
		void SetTexture(s_ptr<Texture> texture) { m_Material->SetTexture(texture); }

			//m_Material = std::make_shared<Material>();
		MaterialComponent() : m_Material(std::make_shared<Material>()) {}
		MaterialComponent(s_ptr<Material> material) : m_Material(material) {}
		MaterialComponent(s_ptr<Material> material, s_ptr<Texture> texture) 
			: m_Material(material)
		{
			m_Material->SetTexture(texture);
		}
		MaterialComponent(s_ptr<Texture> texture)
		{
			m_Material = std::make_shared<Material>();
			m_Material->SetTexture(texture);
		}
	};


	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<
		IDComponent, 
		TagComponent, 
		TransformComponent,
		SpriteComponent,
		CameraComponent
	>;

}

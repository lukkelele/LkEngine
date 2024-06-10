#pragma once

#include "LkEngine/Core/Base.h"

#include "LkEngine/Asset/Asset.h"
#include "LkEngine/Asset/MaterialAsset.h"

#include "LkEngine/Core/Math/Math.h"

#include "LkEngine/Renderer/Material.h"

#include "LkEngine/Physics2D/ContactListener2D.h"

#include "SceneCamera.h"


namespace LkEngine{

	// Component such as IDComponent and TagComponent are not in need of deriving BaseComponent as 
	// they cannot and should not be removed at all
	struct BaseComponent
	{
		bool Removable = true;
	};

	struct IDComponent 
	{
		UUID ID;

		operator UUID() { return ID; }
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct RelationshipComponent
	{
		UUID ParentHandle = 0;
		std::vector<UUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(UUID parent) : ParentHandle(parent) {}
	};

	// TODO: Patch out the use of Rotation2D and just use the Rotation quaternion
    struct TransformComponent 
    {
	public:
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
	private:
		glm::vec3 RotationEuler = { 0.0f, 0.0f, 0.0f };
		glm::quat Rotation = { 1.0f, 0.0f, 0.0f, 0.0f };

	public:
		float Rotation2D = 0.0f;
		bool Static = false;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& Translation) : Translation(Translation) {}

		glm::vec3 GetTranslation() const { return Translation; }
		glm::vec3 GetScale() const { return Scale; }
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

		glm::vec3 GetRotationEuler() const
		{
			return RotationEuler;
		}

		void SetRotationEuler(const glm::vec3& euler)
		{
			RotationEuler = euler;
			Rotation = glm::quat(RotationEuler);
		}

		glm::quat GetRotation() const
		{
			return Rotation;
		}

		void SetRotation(const glm::quat& quat)
		{
			auto originalEuler = RotationEuler;
			Rotation = quat;
			RotationEuler = glm::eulerAngles(Rotation);

			/* Attempt to avoid 180deg flips in the Euler angles when using SetRotation(quat). */
			if ((fabs(RotationEuler.x - originalEuler.x) == glm::pi<float>()) 
				&& (fabs(RotationEuler.z - originalEuler.z) == glm::pi<float>()))
			{
				RotationEuler.x = originalEuler.x;
				RotationEuler.y = glm::pi<float>() - RotationEuler.y;
				RotationEuler.z = originalEuler.z;
			}
		}

		bool IsStatic() const { return Static; }
	};


    struct SpriteComponent 
    {
        std::string FilePath;
        glm::vec2 Size;
        glm::vec4 Color; 
		bool Passthrough = false;

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

        const glm::vec2& GetSize() const { return Size; }
		const glm::vec4& GetColor() const { return Color; }
        float GetWidth() const { return Size.x; }
        float GetHeight() const { return Size.y; }
		void SetSize(const glm::vec2& size) { Size = size; }
		void SetSize(float x, float y) { Size = { x, y }; }
		void SetPassthrough(bool passthrough) { Passthrough = passthrough; }
		bool IsPassthrough() const { return Passthrough; }
    };


	struct CameraComponent
	{
		enum class Type { None = -1, Perspective, Orthographic };
		Type ProjectionType = Type::None;

		Ref<SceneCamera> Camera;
		bool Primary = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;
		
		operator SceneCamera& () { return *Camera; }
		operator const SceneCamera& () const { return *Camera; }
	};

	struct RigidBody2DComponent 
	{
		enum class Type 
		{ 
			None = -1, 
			Static,   // 0
			Dynamic,  // 1
			Kinematic // 2
		};

		Type BodyType;
		bool FixedRotation = false;
		float Mass = 1.0f;
		float LinearDrag = 0.01f;
		float AngularDrag = 0.05f;
		float GravityScale = 1.0f;
		bool IsBullet = false;
		void* RuntimeBody = nullptr; // Assigned at component creation

		RigidBody2DComponent() : BodyType(Type::Dynamic) {}
		RigidBody2DComponent(Type type) : BodyType(type) {}
		RigidBody2DComponent(const RigidBody2DComponent& other) = default;
	};


	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.50f, 0.50f };

		float Density = 1.0f;
		float Friction = 1.0f;
		bool IsSensor = false;
		void* RuntimeBody = nullptr; // Assigned at component creation

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent& other) = default;
	};

	struct MeshComponent
	{
		AssetHandle Mesh;
		uint32_t SubmeshIndex = 0;
		Ref<LkEngine::MaterialTable> MaterialTable = Ref<LkEngine::MaterialTable>::Create();
		std::vector<UUID> BoneEntityIds; // If mesh is rigged, these are the entities whose transforms will used to "skin" the rig
		bool Visible = true;

		MeshComponent() = default;
		MeshComponent(const MeshComponent& other)
			: Mesh(other.Mesh), SubmeshIndex(other.SubmeshIndex)
			, MaterialTable(Ref<LkEngine::MaterialTable>::Create(other.MaterialTable))
			, BoneEntityIds(other.BoneEntityIds) {}
		MeshComponent(AssetHandle mesh, uint32_t submeshIndex = 0)
			: Mesh(mesh), SubmeshIndex(submeshIndex) {}
	};

	struct StaticMeshComponent
	{
		AssetHandle StaticMesh;
		Ref<LkEngine::MaterialTable> MaterialTable = Ref<LkEngine::MaterialTable>::Create();
		bool Visible = true;

		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent& other)
			: StaticMesh(other.StaticMesh)
			, MaterialTable(Ref<LkEngine::MaterialTable>::Create(other.MaterialTable))
			, Visible(other.Visible) {}
		StaticMeshComponent(AssetHandle staticMesh)
			: StaticMesh(staticMesh) {}
	};

	struct Box2DWorldComponent 
	{
		std::unique_ptr<b2World> World;
		ContactListener2D ContactListener;
		bool DebugDrawerAttached = false;

		const glm::vec2 GetGravity() const
		{
			auto g = World->GetGravity();
			return glm::vec2(g.x, g.y);
		}
		int GetBodyCount() const { return World->GetBodyCount(); }
		int GetContactCount() const { return World->GetContactCount(); }
		bool IsPaused() const { return World->IsLocked(); }
		bool HasDebugDrawerAttached() const { return DebugDrawerAttached; }
	};

	struct SceneComponent
	{
		UUID SceneID;
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
		CameraComponent,
		RigidBody2DComponent,
		BoxCollider2DComponent,
		MeshComponent,
		StaticMeshComponent,
		SceneComponent	
	>;

}


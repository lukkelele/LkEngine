#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Asset/Asset.h"
#include "LkEngine/Asset/MaterialAsset.h"

#include "LkEngine/Core/Math/Math.h"

#include "LkEngine/Renderer/CameraBase.h"
#include "LkEngine/Renderer/Material.h"

#include "LkEngine/Physics2D/ContactListener2D.h"

#include "SceneCamera.h"


namespace LkEngine {

	/**
	 * IComponent
	 *
	 *  Component interface.
	 */
	struct IComponent
	{
		virtual ~IComponent() = default;

		virtual std::string ToString() const = 0;
	};

	struct LIDComponent
	{
		LUUID ID;

		operator LUUID() { return ID; }
	};

	struct LTagComponent
	{
		std::string Tag;

		LTagComponent() = default;
		LTagComponent(const LTagComponent& Other) = default;
		LTagComponent(const std::string& tag)
			: Tag(tag)
		{
		}

		operator std::string&() { return Tag; }
		operator const std::string&() const { return Tag; }
	};

	struct LRelationshipComponent
	{
		LUUID ParentHandle = 0;
		std::vector<LUUID> Children;

		LRelationshipComponent() = default;
		LRelationshipComponent(const LUUID InParentHandle)
			: ParentHandle(InParentHandle)
		{
		}
		LRelationshipComponent(const LRelationshipComponent& Other) = default;
	};

	// TODO: Patch out the use of Rotation2D and just use the Rotation quaternion
	struct LTransformComponent : public IComponent
	{
	public:
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	private:
		glm::vec3 RotationEuler = { 0.0f, 0.0f, 0.0f };
		glm::quat Rotation = { 1.0f, 0.0f, 0.0f, 0.0f };

	public:
		float Rotation2D = 0.0f;
		bool bIsStatic = false;

		LTransformComponent() = default;
		LTransformComponent(const LTransformComponent& Other) = default;
		LTransformComponent(const glm::vec3& Translation)
			: Translation(Translation)
		{
		}

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

		FORCEINLINE bool IsStatic() const { return bIsStatic; }

		virtual std::string ToString() const override
		{
			return std::string("LTransformComponent::ToString() | TODO");
		}
	};

	struct LSpriteComponent
	{
		std::string FilePath;
		glm::vec2 Size;
		glm::vec4 Color;
		bool Passthrough = false;

		LSpriteComponent(const std::string& filepath,
						 const glm::vec2& size = { 100.0f, 100.0f },
						 const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
			: FilePath(filepath)
			, Size(size)
			, Color(color)
		{
		}

		LSpriteComponent(const glm::vec2& size = { 0.0f, 0.0f },
						 const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
			: FilePath("")
			, Size(size)
			, Color(color)
		{
		}

		const glm::vec2& GetSize() const { return Size; }
		const glm::vec4& GetColor() const { return Color; }
		float GetWidth() const { return Size.x; }
		float GetHeight() const { return Size.y; }

		void SetSize(const glm::vec2& size)
		{
			Size = size;
		}

		void SetSize(float x, float y) { Size = { x, y }; }
		void SetPassthrough(bool passthrough) { Passthrough = passthrough; }
		bool IsPassthrough() const { return Passthrough; }
	};

	struct LCameraComponent
	{
		TObjectPtr<LSceneCamera> Camera{};
		ECameraProjection ProjectionType = ECameraProjection::None;

		bool Primary = false; /// REMOVE, should be handled by the CameraManager

		LCameraComponent() = default;
		LCameraComponent(const LCameraComponent& Other)
			: Camera(Other.Camera)
			, ProjectionType(Other.ProjectionType)
		{
		}

		operator LSceneCamera&() { return *Camera; }
		operator const LSceneCamera&() const { return *Camera; }
	};

	/** ERigidBodyType */
	enum class ERigidBodyType
	{
		None = -1,
		Static,	  // 0
		Dynamic,  // 1
		Kinematic // 2
	};

	struct LRigidBody2DComponent
	{
		enum class Type
		{
			None = -1,
			Static,	  // 0
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

		LRigidBody2DComponent()
			: BodyType(Type::Dynamic)
		{
		}
		LRigidBody2DComponent(Type type)
			: BodyType(type)
		{
		}
		LRigidBody2DComponent(const LRigidBody2DComponent& Other) = default;
	};

	struct LBoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.50f, 0.50f };

		float Density = 1.0f;
		float Friction = 1.0f;
		bool IsSensor = false;
		void* RuntimeBody = nullptr; // Assigned at component creation

		LBoxCollider2DComponent() = default;
		LBoxCollider2DComponent(const LBoxCollider2DComponent& Other) = default;
	};

	struct LMeshComponent
	{
		FAssetHandle Mesh;
		uint32_t SubmeshIndex = 0;
		TObjectPtr<LMaterialTable> MaterialTable = TObjectPtr<LMaterialTable>::Create();

		/** The entities whose transforms will used to "skin" the rig if mesh is rigged. */
		std::vector<FAssetHandle> BoneEntityIDs{};

		bool bVisible = true;

		LMeshComponent() = default;
		LMeshComponent(const LMeshComponent& Other)
			: Mesh(Other.Mesh)
			, SubmeshIndex(Other.SubmeshIndex)
			, MaterialTable(TObjectPtr<LkEngine::LMaterialTable>::Create(Other.MaterialTable))
			, BoneEntityIDs(Other.BoneEntityIDs)
		{
		}
		LMeshComponent(const FAssetHandle InMesh, const uint32_t InSubmeshIndex = 0)
			: Mesh(InMesh)
			, SubmeshIndex(InSubmeshIndex)
		{
		}
	};

	struct LStaticMeshComponent
	{
		FAssetHandle StaticMesh;
		TObjectPtr<LMaterialTable> MaterialTable = TObjectPtr<LMaterialTable>::Create();
		bool bVisible = true;

		LStaticMeshComponent() = default;
		LStaticMeshComponent(const LStaticMeshComponent& Other)
			: StaticMesh(Other.StaticMesh)
			, MaterialTable(TObjectPtr<LMaterialTable>::Create(Other.MaterialTable))
			, bVisible(Other.bVisible)
		{
		}
		LStaticMeshComponent(const FAssetHandle InStaticMesh)
			: StaticMesh(InStaticMesh)
		{
		}
	};

	struct Box2DWorldComponent
	{
#if 0 /// FIXME: DISABLED FOR NOW
		std::unique_ptr<b2World> World;
		ContactListener2D ContactListener;
		bool DebugDrawerAttached = false;

		const glm::vec2 GetGravity() const
		{
			auto g = World->GetGravity();
			return glm::vec2(g.x, g.y);
		}

		int GetBodyCount() const 
		{ 
			LK_VERIFY(World);
			return World->GetBodyCount(); }

		int GetContactCount() const 
		{ 
			LK_VERIFY(World);
			return World->GetContactCount(); 
		}

		FORCEINLINE bool IsPaused() const 
		{ 
			LK_VERIFY(World);
			return World->IsLocked(); 
		}

		FORCEINLINE bool HasDebugDrawerAttached() const 
		{ 
			return DebugDrawerAttached; 
		}
#endif
	};

	struct LSceneComponent
	{
		LUUID SceneID{};
	};

	template<typename... TComponent>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<
		LIDComponent,
		LTagComponent,
		LTransformComponent,
		LSpriteComponent,
		LCameraComponent,
		LRigidBody2DComponent,
		LBoxCollider2DComponent,
		LMeshComponent,
		LStaticMeshComponent,
		LSceneComponent
	>;

}


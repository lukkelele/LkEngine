#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Scene/SceneCamera.h"


namespace LkEngine {

	struct CameraComponent
	{
		enum class Type { Null = -1, FirstPerson, ThirdPerson, Ortographic };
		Type projectionType;
		SceneCamera camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;

		operator SceneCamera& () { return camera; }
		operator const SceneCamera& () const { return camera; }
	};

}

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Scene/SceneCamera.h"


namespace LkEngine {

	enum class CameraType 
	{ 
		Null = -1, 
		Ortographic,  // 2D
		Perspective,  // First Person
	};

	struct CameraComponent
	{

		CameraType Type;
		SceneCamera* CameraRef;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;

		operator SceneCamera& () { return *CameraRef; }
		operator const SceneCamera& () const { return *CameraRef; }
	};

}

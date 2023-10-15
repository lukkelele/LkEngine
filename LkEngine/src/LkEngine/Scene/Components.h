#pragma once

#include "LkEngine/Scene/Components/IDComponent.h"
#include "LkEngine/Scene/Components/TagComponent.h"
#include "LkEngine/Scene/Components/TransformComponent.h"
#include "LkEngine/Scene/Components/CameraComponent.h"


namespace LkEngine{

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<IDComponent, TagComponent>;

}

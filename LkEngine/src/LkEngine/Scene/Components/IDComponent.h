#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"


namespace LkEngine {

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

}

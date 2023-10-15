#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"


namespace LkEngine {

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& Tag)
			: Tag(Tag) {}
	};

}

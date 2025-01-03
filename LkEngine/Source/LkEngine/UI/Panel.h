#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/UI/UICore.h"
#include "LkEngine/UI/ImGui/LImGui.h"

#include "LkEngine/Serialization/YAMLSerialization.h"


namespace LkEngine {

	class IPanel : public LObject
	{
	public:
		virtual ~IPanel() = default;

		virtual void Initialize() {};
		
		virtual void OnRender() = 0;
		virtual void OnRenderUI(bool& IsOpen) = 0;

		virtual void SerializeToYaml(YAML::Emitter& Out) const = 0;
		virtual void DeserializeFromYaml(const YAML::Node& Data) = 0;
	};

}

#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Renderer/UI/UICore.h"
#include "LkEngine/Renderer/UI/ImGui/LImGui.h"

#include "LkEngine/Serialization/YamlSerialization.h"


namespace LkEngine {

	class LProject;

	class IPanel : public LObject
	{
	public:
		virtual ~IPanel() = default;

		virtual void Initialize() {};
		
		virtual void Render() = 0;
		virtual void RenderUI(bool& IsOpen) = 0;

		virtual void OnProjectChanged(const TObjectPtr<LProject>& InProject) {};

		virtual void SerializeToYaml(YAML::Emitter& Out) const = 0;
		virtual void DeserializeFromYaml(const YAML::Node& Data) = 0;

	protected:
		/**
		 * FWindow
		 * 
		 *  Data container for serialization for subwindows.
		 */
		struct FWindow
		{
			bool bOpen = false;
		};

	};

}

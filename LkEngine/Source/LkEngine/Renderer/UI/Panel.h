#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Renderer/UI/UICore.h"
#include "LkEngine/Renderer/UI/ImGui/LImGui.h"

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

	protected:
		/**
		 * FWindow
		 * 
		 *  Data container for serializing data for subwindows 
		 *  to restore on startup.
		 */
		struct FWindow
		{
			bool bOpen = false;
		};

	};

}

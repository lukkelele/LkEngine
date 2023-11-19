#pragma once

#include "LkEngine/Event/Event.h"


namespace LkEngine {

	class AssetEvent : public Event
	{
	public:
		//AssetEvent() {}
		virtual ~AssetEvent() {}

		//EventType GetEventType() const = 0
		//const char* GetName() const = 0;
		//virtual std::string ToString() const { return GetName(); }
		//virtual bool HandleEvent() = 0;
	};


	class TextureCreatedEvent : public AssetEvent
	{
	public:
		TextureCreatedEvent(uint64_t textureID) 
			: TextureID(textureID) {}

		bool HandleEvent() override
		{
			//Application::Get();
			//Renderer::RegisterTexture(TextureID);
			printf("TextureCreatedEvent: HandleEvent\n");
			return true;
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "TextureCreatedEvent: New texture created with ID '" << TextureID << "'";
			return ss.str();
		}

		const char* GetName() const { return "TextureCreatedEvent"; }
		EventType GetEventType() const { return EventType::TextureCreated;  }

	private:
		uint64_t TextureID;
	};

	class TextureDeletedEvent : public AssetEvent
	{
	public:
		TextureDeletedEvent(uint64_t textureID) 
			: TextureID(textureID) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "TextureDeletedEvent: Texture " << TextureID << " deleted";
			return ss.str();
		}

	private:
		uint64_t TextureID;
	};

}

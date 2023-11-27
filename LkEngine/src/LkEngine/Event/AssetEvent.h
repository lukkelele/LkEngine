#pragma once

#include "LkEngine/Event/Event.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/TextureLibrary.h"


namespace LkEngine {

	class AssetEvent : public Event
	{
	public:
		virtual ~AssetEvent() {}
	};


	class TextureCreatedEvent : public AssetEvent
	{
	public:
		TextureCreatedEvent(Texture& texture) 
			: m_Texture(&texture)
			, m_TextureID(texture.GetRendererID()) {}

		bool HandleEvent() override
		{
			printf("TextureCreatedEvent: HandleEvent\n");
			auto textureLibrary = TextureLibrary::Get();
			//textureLibrary->
			return true;
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "TextureCreatedEvent: New texture created with ID '" << m_TextureID << "'";
			return ss.str();
		}

		const char* GetName() const { return "TextureCreatedEvent"; }
		EventType GetEventType() const { return EventType::TextureCreated;  }

	private:
		uint32_t m_TextureID;
		Texture* m_Texture = nullptr;
	};

	class TextureDeletedEvent : public AssetEvent
	{
	public:
		TextureDeletedEvent(Texture& texture) 
			: m_Texture(&texture)
			, m_TextureID(texture.GetRendererID()) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "TextureDeletedEvent: Texture " << m_TextureID << " deleted";
			return ss.str();
		}

	private:
		uint64_t m_TextureID;
		Texture* m_Texture = nullptr;
	};

}

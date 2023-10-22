#include "LKpch.h"
#include "LkEngine/Renderer/Image.h"


namespace LkEngine {

	Image::Image(std::string_view filepath)
	{
	}
	
	std::shared_ptr<Image> Image::Create(std::string_view filepath)
	{
		return nullptr;
	}
	
	uint32_t Image::BytesPerPixel(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGBA:    return 4;
			case ImageFormat::RGBA32F: return 16;
			case ImageFormat::None:	   return 0;
		}
		return 0;
	}
	
	int64_t Image::GetImageFormat(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGBA:    return 4;
			case ImageFormat::RGBA32F: return 16;
			case ImageFormat::None:	   return 0;
		}
		return 0;
	}

}

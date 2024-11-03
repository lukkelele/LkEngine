#pragma once

#include <stb_image/stb_image.h>
#include <stb_image/stb_image_resize2.h>

namespace LkEngine {

	template <typename ElementType>
	FORCEINLINE void DestructItem(ElementType* Element)
	{
		if constexpr (!std::is_trivially_destructible_v<ElementType>)
		{
			using DestructItemElementType = ElementType;
			Element->DestructItemElementType::~DestructItemElementType();
		}
	}

	/**
	 * @brief Destructs a range of items in memory.
	 */
	template <typename ElementType, typename SizeType>
	FORCEINLINE void DestructItems(ElementType* Element, SizeType Count)
	{
		if constexpr (!std::is_trivially_destructible_v<ElementType>)
		{
			while (Count)
			{
				using DestructItemsElementType = ElementType;
				Element->DestructItemsElementType::~DestructItemsElementType();

				Element++;
				Count--;
			}
		}
	}

}


namespace LkEngine::MemoryUtils {

	static unsigned char* ResizeImageData(const void* Data, 
										  const uint64_t MemorySizeInBytes, 
										  const int OldWidth, 
										  const int OldHeight, 
										  const int NewWidth, 
										  const int NewHeight, 
										  const stbir_pixel_layout PixelLayout = STBIR_RGBA)
	{
		stbi_uc* ResizedMemoryAlloc = (stbi_uc*)malloc(MemorySizeInBytes);
		stbir_resize_uint8_linear((stbi_uc*)Data, 
								  OldWidth, 
								  OldHeight, 
								  0, 
								  ResizedMemoryAlloc, 
								  NewWidth, 
								  NewHeight, 
								  0, 
								  PixelLayout);

		return ResizedMemoryAlloc;
	}

}
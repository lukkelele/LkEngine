#pragma once

#include <stb/stb_image.h>
#include <stb/stb_image_resize2.h>

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

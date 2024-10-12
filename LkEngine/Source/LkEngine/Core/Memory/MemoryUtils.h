#pragma once

#include <stb_image/stb_image.h>
#include <stb_image/stb_image_resize2.h>


namespace LkEngine::MemoryUtils {

	static unsigned char* ResizeImageData(void* data, uint64_t memorySizeInBytes, size_t oldWidth, size_t oldHeight, size_t newWidth, size_t newHeight, stbir_pixel_layout pixelLayout = STBIR_RGBA)
	{
		stbi_uc* resizedMemoryAlloc = (stbi_uc*)malloc(memorySizeInBytes);
		stbir_resize_uint8_linear((stbi_uc*)data, oldWidth, oldHeight, 0, resizedMemoryAlloc, newWidth, newHeight, 0, pixelLayout);
		return resizedMemoryAlloc;
	}

}
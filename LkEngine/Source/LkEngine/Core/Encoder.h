/******************************************************************
 * Encoder
 *
 *
 *******************************************************************/
#pragma once

#include "LkEngine/Core/Core.h"

#include "EncoderType.h"


namespace LkEngine {

	class LEncoder
	{
	public:
		template<EncodingFormat::Type FormatType>
		inline static std::string Encode(const unsigned char* Data, const size_t Length)
		{
			return "Encode_NULL";
		}

		template<>
		inline static std::string Encode<EncodingFormat::Base64>(const unsigned char* Data, const size_t Length)
		{
			LK_CORE_ASSERT(Data && (Length > 0));

			static const char* Base64Characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			std::string EncodedString;
			EncodedString.reserve(((Length + 2) / 3) * 4);

			unsigned int Value = 0;
			int ValueB = -6;

			for (size_t i = 0; i < Length; ++i)
			{
				Value = (Value << 8) + Data[i];
				ValueB += 8;

				while (ValueB >= 0)
				{
					EncodedString.push_back(Base64Characters[(Value >> ValueB) & 0x3F]);
					ValueB -= 6;
				}
			}

			if (ValueB > -6)
			{
				EncodedString.push_back(Base64Characters[((Value << 8) >> (ValueB + 8)) & 0x3F]);
			}

			while (EncodedString.size() % 4)
			{
				EncodedString.push_back('=');
			}

			return EncodedString;
		}

		template<>
		inline static std::string Encode<EncodingFormat::Hex>(const unsigned char* Data, const size_t Length)
		{
			return "Hex_Encoded";
		}

		template<EncodingFormat::Type FormatType>
		inline static std::string Encode(const char* Data, const size_t Length)
		{
			return "Encode_NULL";
		}

		template<>
		inline static std::string Encode<EncodingFormat::Base64>(const char* Data, const size_t Length)
		{
			assert(Data != nullptr);
			assert(Length > 0);

			static const char* Base64Characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			std::string EncodedString;
			EncodedString.reserve(((Length + 2) / 3) * 4);

			unsigned int Value = 0;
			int ValueB = -6;

			for (size_t i = 0; i < Length; ++i)
			{
				Value = (Value << 8) + Data[i];
				ValueB += 8;

				while (ValueB >= 0)
				{
					EncodedString.push_back(Base64Characters[(Value >> ValueB) & 0x3F]);
					ValueB -= 6;
				}
			}

			if (ValueB > -6)
			{
				EncodedString.push_back(Base64Characters[((Value << 8) >> (ValueB + 8)) & 0x3F]);
			}

			while (EncodedString.size() % 4)
			{
				EncodedString.push_back('=');
			}

			return EncodedString;
		}

		template<>
		inline static std::string Encode<EncodingFormat::Hex>(const char* Data, const size_t Length)
		{
			return "Hex_Encoded";
		}

	};

}

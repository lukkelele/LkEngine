#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <string_view>
#include <locale>
#include <codecvt>

#include "LkEngine/Core/Assert.h"


namespace LkEngine {

    /// TEMPORARY
    using LStringView = std::string_view;

    /** Returns Char value of Nibble */
	[[nodiscard]] inline char NibbleToChar(uint8_t Num)
	{
		if (Num > 9)
		{
			return (char)('A' + (Num - 10));
		}
		return (char)('0' + Num);
		//return (char)('0' + Num);
	}

    class LString
    {
    public:
        LString()
            : m_Data(nullptr)
            , m_Length(0) 
        {
        }

        LString(const char* String)
        {
            LK_CORE_VERIFY(String != nullptr, "Passed NULL to LString constructor");
            Copy(String);
        }

        LString(const std::string& String)
            : LString(String.c_str())
        {
        }

        LString(std::string_view StringView)
            : LString(StringView.data())
        {
        }

        LString(const LString& Other)
        {
            if (Other.m_Data)
            {
                Copy(Other.m_Data);
            }
            else
            {
                m_Data = nullptr;
                m_Length = 0;
            }
        }

        LString(LString&& Other) noexcept
            : m_Data(Other.m_Data)
            , m_Length(Other.m_Length)
        {
            Other.m_Data = nullptr;
            Other.m_Length = 0;
        }

        ~LString()
        {
            if (m_Data)
            {
                delete[] m_Data;
                m_Data = nullptr;
            }

            m_Length = 0;
        }

        LString& operator=(const LString& Other) 
        {
            if (this != &Other) 
			{
				delete[] m_Data;
				if (Other.m_Data)
				{
					Copy(Other.m_Data);
				}
				else
				{
					m_Data = nullptr;
					m_Length = 0;
				}
			}

            return *this;
        }

        LString& operator=(LString&& Other) noexcept 
        {
            if (this != &Other) 
            {
                delete[] m_Data;
                m_Data = Other.m_Data;
                m_Length = Other.m_Length;
                Other.m_Data = nullptr;
                Other.m_Length = 0;
            }

            return *this;
        }

        LString& operator+=(const char Character)
        {
            // Create a new buffer with extra space for the new character and copy existing data to it.
			char* NewData = new char[m_Length + 2]; // +1 for the added character, +1 for null terminator
			if (m_Data)
			{
				std::strcpy(NewData, m_Data);
			}

			// Append the new character and null-terminate the string.
			NewData[m_Length] = Character;
			NewData[m_Length + 1] = '\0';

			// Delete old data and update the member variables.
			delete[] m_Data;
			m_Data = NewData;
			++m_Length;

			return *this;
        }

		LString& operator+=(const char* String)
		{
			size_t StringLength = std::strlen(String);

			// Create a new buffer with extra space for the new string and copy existing data to it.
			char* NewData = new char[m_Length + StringLength + 1]; // +1 for null terminator
			if (m_Data)
			{
				std::strcpy(NewData, m_Data);
			}

			// Append the new string.
			std::strcpy(NewData + m_Length, String);

			// Delete old data and update the member variables.
			delete[] m_Data;
			m_Data = NewData;
			m_Length += StringLength;

			return *this;
		}

		LString& operator+=(const LString& other)
		{
			return (*this += other.m_Data);
		}

		std::string operator*() const
		{
            return std::string(m_Data);
		}

        operator std::string() const 
        { 
            return std::string(m_Data ? m_Data : ""); 
        }

        //operator std::basic_string<char>() const 
        //{ 
        //    return m_Data ? m_Data : "";
        //}
        
        /* Get string as c string, same as c_str. */
        const char* CStr() const { return m_Data; }

		FORCEINLINE bool IsEmpty() const 
        { 
            return m_Length == 0 && !m_Data;
        }

		FORCEINLINE const size_t& Length() const { return m_Length; }
		FORCEINLINE const size_t& Size() const { return m_Length; }

		FORCEINLINE const char& At(int idx) const 
		{
			assert(idx >= 0 && idx < static_cast<int>(m_Length)); 
			return m_Data[idx];
		}

        LString Slice(const int Start, const int Stop) const 
        {
            LK_ASSERT((Start > Stop || Start < 0 || Stop > static_cast<int>(m_Length)), 
                "LString::Slice, invalid slice range ({}, {}) with length={}", Start, Stop, m_Length)
        
            size_t NewLength = static_cast<size_t>(Stop) - Start;
            char* SliceData = new char[NewLength + 1];

            for (size_t Index = 0; Index < NewLength; Index++) 
            {
                SliceData[Index] = m_Data[Start + Index];
            }

            SliceData[NewLength] = '\0';
        
            LString Result(SliceData);
            delete[] SliceData; 

            return Result;
        }

        FORCEINLINE int ToInteger() const 
		{
            char* End;
			long Result = std::strtol(m_Data, &End, 10);
			assert(m_Data != End);
			assert((Result >= std::numeric_limits<int>::min()) && (Result <= std::numeric_limits<int>::max()));
			return static_cast<int>(Result);
		}

        template<typename ...TArgs>
        static LString Format(const LString& String, TArgs&&... Args)
        {
            return fmt::format(String.m_Data, std::forward<TArgs>(Args)...);
        }

    private:
        void Copy(const char* String)
        {
			m_Length = std::strlen(String);
			m_Data = new char[m_Length + 1];
			std::strcpy(m_Data, String);
		}

    private:
        char* m_Data{};
        size_t m_Length = 0;
    };

    /** Convert byte to hex. */
    FORCEINLINE static void ByteToHex(uint8_t In, LString& Result)
	{
		Result += NibbleToChar(In >> 4);
		Result += NibbleToChar(In & 15);
	}

}
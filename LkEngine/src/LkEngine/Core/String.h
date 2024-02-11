#pragma once

#include <string>

#include "Assert.h"


namespace LkEngine {

    class String
    {
    public:
        String()
            : m_Data(nullptr)
            , m_Length(0) {}

        String(const wchar_t* str)
        {
            Copy(str);
        }

        String(const char* str)
        {
            LK_CORE_VERIFY(str != nullptr, "Passed NULL to String constructor");
            size_t length = mbstowcs(nullptr, str, 0);
            LK_CORE_ASSERT(length != -1, "String length is -1, make sure that you pass a valid const char* argument to the constructor");

            m_Data = new wchar_t[length + 1];
            size_t convertedLength = mbstowcs(m_Data, str, length + 1);
            LK_CORE_ASSERT(convertedLength != (size_t)-1, "Converted string length is equal to -1, const char* to wchar_t* conversion failed!");

            m_Length = length;
        }

        String(const std::string& string)
            : String(string.c_str())
        {
        }

        String(std::string_view view)
            : String(std::string(view).c_str())
        {
        }

        String(const String& other)
        {
            Copy(other.m_Data);
        }

        String(String&& other) noexcept
            : m_Data(other.m_Data)
            , m_Length(other.m_Length)
        {
            other.m_Data = nullptr;
            other.m_Length = 0;
        }

        ~String()
        {
            if (m_Data)
            {
                delete[] m_Data;
                m_Data = nullptr;
            }
            m_Length = 0;
        }

        String& operator=(const String& other) 
        {
              if (this != &other) 
              {
                  delete[] m_Data;
                  Copy(other.m_Data);
              }
              return *this;
        }

        String& operator=(String&& other) noexcept 
        {
            if (this != &other) 
            {
                delete[] m_Data;
                m_Data = other.m_Data;
                m_Length = other.m_Length;
                other.m_Data = nullptr;
                other.m_Length = 0;
            }
            return *this;
        }

        bool IsEmpty() const { return m_Length == 0 && !m_Data; }

        const size_t& Length() const { return m_Length; }

        const wchar_t& At(int idx) const 
        {
            LK_CORE_ASSERT(idx < 0 || idx >= static_cast<int>(m_Length)) 
            return m_Data[idx];
        }

        String Slice(int start, int stop) const 
        {
            if (start < 0) 
                start = static_cast<int>(m_Length) + start;
            if (stop < 0) 
                stop = static_cast<int>(m_Length) + stop;

            LK_ASSERT((start > stop || start < 0 || stop > static_cast<int>(m_Length)), "String::Slice, invalid slice range ({}, {}) with length={}", start, stop, m_Length)
        
            size_t newLength = static_cast<size_t>(stop) - start;
            wchar_t* sliceData = new wchar_t[newLength + 1];

            for (size_t i = 0; i < newLength; ++i) 
            {
                sliceData[i] = m_Data[start + i];
            }

            sliceData[newLength] = L'\0';
        
            String result(sliceData);
            delete[] sliceData; 
            return result;
        }

        inline int ToInteger() const 
        {
            wchar_t* end;
            long result = std::wcstol(m_Data, &end, 10);
            LK_CORE_ASSERT(m_Data != end, "String::ToInteger, no digits were found");
            LK_CORE_ASSERT((result < std::numeric_limits<int>::min() || result > std::numeric_limits<int>::max()), "Conversion result {} is out of int range", result);
            return static_cast<int>(result);
        }

    private:

        template<typename TString>
        void Copy(const TString str)
        {
        }

        template<>
        void Copy(const wchar_t* str)
        {
            m_Length = wcslen(str);
            m_Data = new wchar_t[m_Length + 1];
            wcscpy(m_Data, str);
        }

        template<>
        void Copy(const char* str)
        {
            m_Length = mbstowcs(nullptr, str, 0);
            LK_CORE_ASSERT(m_Length != -1, "String length is -1, make sure that you pass a valid const char* argument (String::Copy)");

            m_Data = new wchar_t[m_Length + 1];
            size_t convertedLength = mbstowcs(m_Data, str, m_Length + 1);
            LK_CORE_ASSERT(convertedLength != -1, "String::Copy failed, converted resulting length is -1 for argument \"{}\", length={}", str, m_Length);
        }

    private:
        wchar_t* m_Data;
        size_t m_Length;
    };

}
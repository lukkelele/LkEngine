#pragma once

#include "LkEngine/Core/Memory/Buffer.h"


namespace LkEngine {

    class StreamWriter
    {
	public:
		virtual ~StreamWriter() = default;

		virtual bool IsStreamGood() const = 0;
		virtual uint64_t GetStreamPosition() = 0;
		virtual void SetStreamPosition(uint64_t position) = 0;
		virtual bool WriteData(const char* data, const size_t size) = 0;
		
		operator bool() const { return IsStreamGood(); }

		void WriteBuffer(FBuffer buffer, bool bWriteSize = true);
		void WriteZero(uint64_t size);
		void WriteString(const std::string& string);

		template<typename T>
		void WriteRaw(const T& type)
		{
			const bool success = WriteData((char*)&type, sizeof(T));
		}

		template<typename T>
		void WriteObject(const T& obj)
		{
			T::Serialize(this, obj);
		}

		template<typename Key, typename Value>
		void WriteMap(const std::map<Key, Value>& map, const bool bWriteSize = true)
		{
			if (bWriteSize)
			{
				WriteRaw<uint32_t>((uint32_t)map.size());
			}

			for (const auto& [key, value] : map)
			{
				if constexpr (std::is_trivial<Key>())
				{
					WriteRaw<Key>(key);
				}
				else
				{
					WriteObject<Key>(key);
				}

				if constexpr (std::is_trivial<Value>())
				{
					WriteRaw<Value>(value);
				}
				else
				{
					WriteObject<Value>(value);
				}
			}
		}

		template<typename Key, typename Value>
		void WriteMap(const std::unordered_map<Key, Value>& map, const bool bWriteSize = true)
		{
			if (bWriteSize)
			{
				WriteRaw<uint32_t>((uint32_t)map.size());
			}

			for (const auto& [key, value] : map)
			{
				if constexpr (std::is_trivial<Key>())
				{
					WriteRaw<Key>(key);
				}
				else
				{
					WriteObject<Key>(key);
				}

				if constexpr (std::is_trivial<Value>())
				{
					WriteRaw<Value>(value);
				}
				else
				{
					WriteObject<Value>(value);
				}
			}
		}

		template<typename Value>
		void WriteMap(const std::unordered_map<std::string, Value>& map, bool bWriteSize = true)
		{
			if (bWriteSize)
				WriteRaw<uint32_t>((uint32_t)map.size());

			for (const auto& [key, value] : map)
			{
				WriteString(key);

				if constexpr (std::is_trivial<Value>())
					WriteRaw<Value>(value);
				else
					WriteObject<Value>(value);
			}
		}

		template<typename T>
		void WriteArray(const std::vector<T>& array, bool bWriteSize = true)
		{
			if (bWriteSize)
				WriteRaw<uint32_t>((uint32_t)array.size());

			for (const auto& element : array)
			{
				if constexpr (std::is_trivial<T>())
					WriteRaw<T>(element);
				else
					WriteObject<T>(element);
			}
		}

	};

	template<>
	inline void StreamWriter::WriteArray(const std::vector<std::string>& array, bool bWriteSize)
	{
		if (bWriteSize)
			WriteRaw<uint32_t>((uint32_t)array.size());

		for (const auto& element : array)
			WriteString(element);
	}

};
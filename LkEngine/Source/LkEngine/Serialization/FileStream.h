#pragma once

#include <filesystem>
#include <fstream>

#include "StreamWriter.h"
#include "StreamReader.h"

#include "YAMLSerialization.h"


namespace LkEngine {

	/**
	 * ESerializationOp
	 * 
	 *  Type of serialization operation.
	 */
	namespace SerializationOp
	{
		enum class Type : uint8_t
		{
			Serialize,
			Deserialize
		};
	}
	using ESerializationOp = SerializationOp::Type;

	class FileStreamWriter : public StreamWriter
	{
	public:
		FileStreamWriter(const std::filesystem::path& path);
		FileStreamWriter(const FileStreamWriter&) = delete;
		virtual ~FileStreamWriter();

		FORCEINLINE virtual bool IsStreamGood() const final override
		{ 
			return Stream.good(); 
		}

		FORCEINLINE virtual uint64_t GetStreamPosition() final override
		{ 
			return Stream.tellp(); 
		}

		FORCEINLINE virtual void SetStreamPosition(uint64_t Position) final override
		{ 
			Stream.seekp(Position); 
		}

		virtual bool WriteData(const char* Data, const std::size_t Size) final override;

	private:
		std::filesystem::path Path{};
		std::ofstream Stream;
	};


	class FileStreamReader : public StreamReader
	{
	public:
		FileStreamReader(const std::filesystem::path& path);
		FileStreamReader(const FileStreamReader&) = delete;
		~FileStreamReader();

		FORCEINLINE virtual bool IsStreamGood() const final override 
		{ 
			return Stream.good(); 
		}

		FORCEINLINE virtual uint64_t GetStreamPosition() override 
		{ 
			return Stream.tellg(); 
		}

		FORCEINLINE virtual void SetStreamPosition(const uint64_t Position) override 
		{ 
			Stream.seekg(Position); 
		}

		virtual bool ReadData(char* Destination, const std::size_t Size) override;

	private:
		std::filesystem::path Path;
		std::ifstream Stream;
	};

} 

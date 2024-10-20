#include "LKpch.h"
#include "FileStream.h"


namespace LkEngine {

	FileStreamWriter::FileStreamWriter(const std::filesystem::path& InPath)
		: Path(InPath)
	{
		Stream = std::ofstream(InPath, std::ifstream::out | std::ifstream::binary);
	}

	FileStreamWriter::~FileStreamWriter()
	{
		Stream.close();
	}

	bool FileStreamWriter::WriteData(const char* Data, const size_t Size)
	{
		Stream.write(Data, Size);

		//return true;
		return Stream.good();
	}


	FileStreamReader::FileStreamReader(const std::filesystem::path& InPath)
		: Path(InPath)
	{
		Stream = std::ifstream(InPath, std::ifstream::in | std::ifstream::binary);
	}

	FileStreamReader::~FileStreamReader()
	{
		Stream.close();
	}

	bool FileStreamReader::ReadData(char* Destination, const std::size_t Size)
	{
		Stream.read(Destination, Size);
		return true;
	}

}
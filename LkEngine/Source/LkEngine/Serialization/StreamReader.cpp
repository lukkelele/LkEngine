#include "LKpch.h"
#include "StreamReader.h"


namespace LkEngine {

	void StreamReader::ReadBuffer(FBuffer& Buffer, uint32_t size)
	{
		Buffer.Size = size;
		if (size == 0)
		{
			ReadData((char*)&Buffer.Size, sizeof(uint32_t));
		}

		Buffer.Allocate(Buffer.Size);
		ReadData((char*)Buffer.Data, Buffer.Size);
	}

	void StreamReader::ReadString(std::string& string)
	{
		size_t size;
		ReadData((char*)&size, sizeof(size_t));

		string.resize(size);
		ReadData((char*)string.data(), sizeof(char) * size);
	}

}

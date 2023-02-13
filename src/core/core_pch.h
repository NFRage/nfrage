#pragma once

#include "core.h"
#include "fs.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace nfr::fs
{

class Stream : public api::IStream
{
private:
	std::atomic_long refCount;

	bool bReadOnly = false;
	std::fstream stream;
	std::uint64_t fileSize = 0;
	std::uint32_t flags = 0;

public:
	Stream() = default;
	Stream(Stream&&) = default;
	Stream(std::fstream&& inStream, std::uint64_t inFileSize, bool inReadOnly);

	~Stream() override = default;

	bool getLine(std::string& line) override;

	std::uint64_t read(void* outData, std::uint64_t dataSize) override;
	std::uint64_t write(void* inData, std::uint64_t dataSize) override;
	std::uint64_t seek(api::EStreamMode mode, std::uint64_t position) override;

	std::uint64_t getPosition() override;
	std::uint64_t getSize() const override;

	bool isEndOfFile() const override;
	bool isOpen() const override;
	bool isReal() const override;	// means "real" file on disk or just memory representation
	bool isReadOnly() const override;

public:
	long addRef() override;
	long release() override;
};

}

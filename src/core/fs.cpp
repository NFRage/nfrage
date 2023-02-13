/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2022-2023. All rights reserved.
* nfrage - engine code for NFRage project
**********************************************************************
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free 
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
* Boston, MA 02110-1301 USA
*****************************************************************/
#include "core_pch.h"

namespace nfr::fs
{
	Stream::Stream(std::fstream&& inStream, std::uint64_t inFileSize, bool inReadOnly)
		: stream(std::move(inStream)), fileSize(inFileSize), bReadOnly(inReadOnly)
	{
	}

	bool Stream::getLine(std::string& line)
	{
		std::getline(stream, line);
		return !stream.eof();
	}

	std::uint64_t Stream::read(void* outData, std::uint64_t dataSize)
	{
		stream.read(reinterpret_cast<char*>(outData), dataSize);
		return dataSize;
	}

	std::uint64_t Stream::write(void* inData, std::uint64_t dataSize)
	{
		stream.write(reinterpret_cast<char*>(inData), dataSize);
		return dataSize;
	}

	std::uint64_t Stream::seek(api::EStreamMode mode, std::uint64_t position)
	{
		stream.seekg(position, (mode == api::EStreamMode::Set ? std::ios::beg : (mode == api::EStreamMode::Cur ? std::ios::cur : std::ios::end)));
        
		if (!isReadOnly()) {
			stream.seekp(position, (mode == api::EStreamMode::Set ? std::ios::beg : (mode == api::EStreamMode::Cur ? std::ios::cur : std::ios::end)));
		}

		return getPosition();
	}

	std::uint64_t Stream::getPosition()
	{
		return stream.tellg();
	}

	std::uint64_t Stream::getSize() const
	{
		return fileSize;
	}

	bool Stream::isEndOfFile() const
	{
		return stream.eof();
	}

	bool Stream::isOpen() const
	{
		return stream.is_open();
	}

	bool Stream::isReal() const
	{
		return true;
	}

	bool Stream::isReadOnly() const
	{
		return bReadOnly;
	}

	long Stream::addRef()
	{
		return refCount.fetch_add(1);
	}

	long Stream::release()
	{
		long returnRefCount = refCount.fetch_sub(1);
		if (returnRefCount == 0) {
			delete this;
		}

		return returnRefCount;
	}
}

namespace nfr::fs
{

api::path WorkingDirectory;
api::path MainDirectory;
api::path ResourcesDirectory;
api::path GameDirectory;
api::path TempDirectory;

bool Initialize()
{
	dbg::Log("Initializing \"Filesystem\" module...");

	WorkingDirectory = std::filesystem::current_path();
	MainDirectory = GetWorkingDirectory();
	MainDirectory.append("nfrage");

	if (!std::filesystem::exists(MainDirectory)) {
		std::error_code err;
		std::filesystem::create_directory(MainDirectory, err);
		if (err) {
			dbg::Warning("Can't create main directory. Ignoring and writing to system temp directory ({})", err.message());
			MainDirectory = std::filesystem::temp_directory_path();
			MainDirectory.append("nfrage");
		}
	}

	GameDirectory = GetMainDirectory();
	GameDirectory.append("game");
	if (!std::filesystem::exists(GameDirectory)) {
		std::error_code err;
		std::filesystem::create_directory(GameDirectory, err);
		if (err) {
			dbg::Error("Can't create game directory. Please, verify your working directory or folder permissions ({})", err.message());
		}
	}

	ResourcesDirectory = GetMainDirectory();
	ResourcesDirectory.append("resources");
	if (!std::filesystem::exists(ResourcesDirectory)) {
		std::error_code err;
		std::filesystem::create_directory(ResourcesDirectory, err);
		if (err) {
			dbg::Error("Can't create resources directory. Please, verify your working directory or folder permissions ({})", err.message());
		}
	}

	TempDirectory = GetMainDirectory();
	TempDirectory.append("temp");
	if (!std::filesystem::exists(TempDirectory)) {
		std::error_code err;
		std::filesystem::create_directory(TempDirectory, err);
		if (err) {
			dbg::Error("Can't create temp directory. Please, verify your working directory or folder permissions ({})", err.message());
		}
	}

	return true;
}

void Destroy()
{
	dbg::Log("Destroying \"Filesystem\" module...");
}

bool Exists(const char* filePath)
{
	return std::filesystem::exists(filePath);
}

bool Exists(const api::path& filePath)
{
	return std::filesystem::exists(filePath);
}

api::IStream* OpenFile(std::uint32_t flags, const char* filePath)
{
	return OpenFile(flags, api::path(filePath));
}

api::IStream* OpenFile(std::uint32_t flags, const api::path& filePath)
{
	bool bReadOnly = true;
	int streamFlags = std::ios::binary;

	if (flags & api::EStreamFlags::ReadFlag) {
		streamFlags |= std::ios::in;
	}

	if (flags & api::EStreamFlags::WriteFlag) {
		streamFlags |= std::ios::out;
		bReadOnly = false;
	}	
	
	if (flags & api::EStreamFlags::AppendFlag) {
		streamFlags |= std::ios::app;
		bReadOnly = false;
	}

	std::fstream stream(filePath, streamFlags);
	if (!stream.is_open()) {
		return new Stream(std::move(stream), 0, false);
		
	}

	std::uint64_t fileSize = std::filesystem::file_size(filePath);
	return new Stream(std::move(stream), fileSize, bReadOnly);
}

const api::path& GetWorkingDirectory()
{
	return WorkingDirectory;
}

const api::path& GetMainDirectory()
{
	return MainDirectory;
}

const api::path& GetResourcesDirectory()
{
	return ResourcesDirectory;
}

const api::path& GetGameDirectory()
{	
	return GameDirectory;
}

const api::path& GetTempDirectory()
{
	return TempDirectory;
}

}

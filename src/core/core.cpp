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

std::shared_ptr<spdlog::logger> CoreLogger;
spdlog::sink_ptr LogFileSink;
spdlog::sink_ptr ConsoleSink;

namespace nfr::core
{

bool Initialize()
{
	ConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

	CoreLogger = std::make_shared<spdlog::logger>("NFRage"); 
	CoreLogger->sinks().push_back(ConsoleSink);
	dbg::Log("Logger initalized...");

	if (!fs::Initialize()) {
		return false;
	}

	api::path logPath = fs::GetTempDirectory();
	logPath.append("log.txt");
	LogFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath.generic_string(), true);
	CoreLogger->sinks().push_back(LogFileSink);
	//CoreLogger->set_level(spdlog::level::trace);

	return true;
}

void Destroy()
{
	dbg::Log("Destroying \"Core\" module...");
}

spdlog::logger* GetLogger()
{
	return CoreLogger.get();
}

}


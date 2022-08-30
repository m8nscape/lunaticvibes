#include "replay_chart.h"
#include "common/utils.h"
#include "config/config_mgr.h"

#include <cereal/archives/portable_binary.hpp>
#include <sstream>

bool ReplayChart::loadFile(const Path& path)
{
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.good())
	{
		try
		{
			cereal::PortableBinaryInputArchive ia(ifs);
			ia(*this);
		}
		catch (...)
		{
			return false;
		}
		return validate();
	}
	return false;
}

bool ReplayChart::saveFile(const Path& path)
{
	fs::create_directories(path.parent_path());
	std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
	if (ofs.good())
	{
		generateChecksum();
		try
		{
			cereal::PortableBinaryOutputArchive oa(ofs);
			oa(*this);
		}
		catch (...)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool ReplayChart::validate()
{
	uint32_t checksumReal = checksum;
	generateChecksum();
	uint32_t checksumTmp = checksum;
	bool valid = (checksum == checksumReal);
	checksum = checksumReal;
	return valid;
}

void ReplayChart::generateChecksum()
{
	checksum = 0;

	std::stringstream ss;
	try
	{
		cereal::PortableBinaryOutputArchive oa(ss);
		oa(*this);
	}
	catch (...)
	{
		return;
	}

	auto hash = md5(ss.str()).hexdigest().substr(0, 4);
	checksum |= base16(hash[0]) << 0;
	checksum |= base16(hash[1]) << 8;
	checksum |= base16(hash[2]) << 16;
	checksum |= base16(hash[3]) << 24;
}


Path ReplayChart::getReplayPath(const HashMD5& chartMD5)
{
	return ConfigMgr::Profile()->getPath() / "replay" / "chart" / chartMD5.hexdigest();
}

Path ReplayChart::getReplayPath()
{
	return getReplayPath(chartHash);
}
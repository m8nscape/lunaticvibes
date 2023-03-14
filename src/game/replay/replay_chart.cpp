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



const std::map<Input::Pad, ReplayChart::Commands::Type> REPLAY_INPUT_DOWN_CMD_MAP =
{
    { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
    { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
    { Input::Pad::K11,      ReplayChart::Commands::Type::K11_DOWN },
    { Input::Pad::K12,      ReplayChart::Commands::Type::K12_DOWN },
    { Input::Pad::K13,      ReplayChart::Commands::Type::K13_DOWN },
    { Input::Pad::K14,      ReplayChart::Commands::Type::K14_DOWN },
    { Input::Pad::K15,      ReplayChart::Commands::Type::K15_DOWN },
    { Input::Pad::K16,      ReplayChart::Commands::Type::K16_DOWN },
    { Input::Pad::K17,      ReplayChart::Commands::Type::K17_DOWN },
    { Input::Pad::K18,      ReplayChart::Commands::Type::K18_DOWN },
    { Input::Pad::K19,      ReplayChart::Commands::Type::K19_DOWN },
    { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
    { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
    { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
    { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
    { Input::Pad::K21,      ReplayChart::Commands::Type::K21_DOWN },
    { Input::Pad::K22,      ReplayChart::Commands::Type::K22_DOWN },
    { Input::Pad::K23,      ReplayChart::Commands::Type::K23_DOWN },
    { Input::Pad::K24,      ReplayChart::Commands::Type::K24_DOWN },
    { Input::Pad::K25,      ReplayChart::Commands::Type::K25_DOWN },
    { Input::Pad::K26,      ReplayChart::Commands::Type::K26_DOWN },
    { Input::Pad::K27,      ReplayChart::Commands::Type::K27_DOWN },
    { Input::Pad::K28,      ReplayChart::Commands::Type::K28_DOWN },
    { Input::Pad::K29,      ReplayChart::Commands::Type::K29_DOWN },
    { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
    { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
};

const std::map<Input::Pad, ReplayChart::Commands::Type> REPLAY_INPUT_UP_CMD_MAP =
{
    { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
    { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
    { Input::Pad::K11,      ReplayChart::Commands::Type::K11_UP },
    { Input::Pad::K12,      ReplayChart::Commands::Type::K12_UP },
    { Input::Pad::K13,      ReplayChart::Commands::Type::K13_UP },
    { Input::Pad::K14,      ReplayChart::Commands::Type::K14_UP },
    { Input::Pad::K15,      ReplayChart::Commands::Type::K15_UP },
    { Input::Pad::K16,      ReplayChart::Commands::Type::K16_UP },
    { Input::Pad::K17,      ReplayChart::Commands::Type::K17_UP },
    { Input::Pad::K18,      ReplayChart::Commands::Type::K18_UP },
    { Input::Pad::K19,      ReplayChart::Commands::Type::K19_UP },
    { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
    { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
    { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
    { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
    { Input::Pad::K21,      ReplayChart::Commands::Type::K21_UP },
    { Input::Pad::K22,      ReplayChart::Commands::Type::K22_UP },
    { Input::Pad::K23,      ReplayChart::Commands::Type::K23_UP },
    { Input::Pad::K24,      ReplayChart::Commands::Type::K24_UP },
    { Input::Pad::K25,      ReplayChart::Commands::Type::K25_UP },
    { Input::Pad::K26,      ReplayChart::Commands::Type::K26_UP },
    { Input::Pad::K27,      ReplayChart::Commands::Type::K27_UP },
    { Input::Pad::K28,      ReplayChart::Commands::Type::K28_UP },
    { Input::Pad::K29,      ReplayChart::Commands::Type::K29_UP },
    { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
    { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
};

const std::map<Input::Pad, ReplayChart::Commands::Type> REPLAY_INPUT_DOWN_CMD_MAP_5K[4] =
{
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
        { Input::Pad::K11,      ReplayChart::Commands::Type::K11_DOWN },
        { Input::Pad::K12,      ReplayChart::Commands::Type::K12_DOWN },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K13_DOWN },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K14_DOWN },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K15_DOWN },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
        { Input::Pad::K21,      ReplayChart::Commands::Type::K21_DOWN },
        { Input::Pad::K22,      ReplayChart::Commands::Type::K22_DOWN },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K23_DOWN },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K24_DOWN },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K25_DOWN },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
        { Input::Pad::K11,      ReplayChart::Commands::Type::K11_DOWN },
        { Input::Pad::K12,      ReplayChart::Commands::Type::K12_DOWN },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K13_DOWN },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K14_DOWN },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K15_DOWN },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K21_DOWN },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K22_DOWN },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K23_DOWN },
        { Input::Pad::K26,      ReplayChart::Commands::Type::K24_DOWN },
        { Input::Pad::K27,      ReplayChart::Commands::Type::K25_DOWN },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K11_DOWN },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K12_DOWN },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K13_DOWN },
        { Input::Pad::K16,      ReplayChart::Commands::Type::K14_DOWN },
        { Input::Pad::K17,      ReplayChart::Commands::Type::K15_DOWN },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
        { Input::Pad::K21,      ReplayChart::Commands::Type::K21_DOWN },
        { Input::Pad::K22,      ReplayChart::Commands::Type::K22_DOWN },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K23_DOWN },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K24_DOWN },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K25_DOWN },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_DOWN },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_DOWN },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K11_DOWN },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K12_DOWN },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K13_DOWN },
        { Input::Pad::K16,      ReplayChart::Commands::Type::K14_DOWN },
        { Input::Pad::K17,      ReplayChart::Commands::Type::K15_DOWN },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_DOWN },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_DOWN },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_DOWN },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_DOWN },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K21_DOWN },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K22_DOWN },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K23_DOWN },
        { Input::Pad::K26,      ReplayChart::Commands::Type::K24_DOWN },
        { Input::Pad::K27,      ReplayChart::Commands::Type::K25_DOWN },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_DOWN },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_DOWN },
    },
};

const std::map<Input::Pad, ReplayChart::Commands::Type> REPLAY_INPUT_UP_CMD_MAP_5K[4] =
{
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
        { Input::Pad::K11,      ReplayChart::Commands::Type::K11_UP },
        { Input::Pad::K12,      ReplayChart::Commands::Type::K12_UP },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K13_UP },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K14_UP },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K15_UP },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
        { Input::Pad::K21,      ReplayChart::Commands::Type::K21_UP },
        { Input::Pad::K22,      ReplayChart::Commands::Type::K22_UP },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K23_UP },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K24_UP },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K25_UP },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
        { Input::Pad::K11,      ReplayChart::Commands::Type::K11_UP },
        { Input::Pad::K12,      ReplayChart::Commands::Type::K12_UP },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K13_UP },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K14_UP },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K15_UP },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K21_UP },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K22_UP },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K23_UP },
        { Input::Pad::K26,      ReplayChart::Commands::Type::K24_UP },
        { Input::Pad::K27,      ReplayChart::Commands::Type::K25_UP },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K11_UP },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K12_UP },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K13_UP },
        { Input::Pad::K16,      ReplayChart::Commands::Type::K14_UP },
        { Input::Pad::K17,      ReplayChart::Commands::Type::K15_UP },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
        { Input::Pad::K21,      ReplayChart::Commands::Type::K21_UP },
        { Input::Pad::K22,      ReplayChart::Commands::Type::K22_UP },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K23_UP },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K24_UP },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K25_UP },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
    },
    {
        { Input::Pad::S1L,      ReplayChart::Commands::Type::S1L_UP },
        { Input::Pad::S1R,      ReplayChart::Commands::Type::S1R_UP },
        { Input::Pad::K13,      ReplayChart::Commands::Type::K11_UP },
        { Input::Pad::K14,      ReplayChart::Commands::Type::K12_UP },
        { Input::Pad::K15,      ReplayChart::Commands::Type::K13_UP },
        { Input::Pad::K16,      ReplayChart::Commands::Type::K14_UP },
        { Input::Pad::K17,      ReplayChart::Commands::Type::K15_UP },
        { Input::Pad::K1START,  ReplayChart::Commands::Type::K1START_UP },
        { Input::Pad::K1SELECT, ReplayChart::Commands::Type::K1SELECT_UP },
        { Input::Pad::S2L,      ReplayChart::Commands::Type::S2L_UP },
        { Input::Pad::S2R,      ReplayChart::Commands::Type::S2R_UP },
        { Input::Pad::K23,      ReplayChart::Commands::Type::K21_UP },
        { Input::Pad::K24,      ReplayChart::Commands::Type::K22_UP },
        { Input::Pad::K25,      ReplayChart::Commands::Type::K23_UP },
        { Input::Pad::K26,      ReplayChart::Commands::Type::K24_UP },
        { Input::Pad::K27,      ReplayChart::Commands::Type::K25_UP },
        { Input::Pad::K2START,  ReplayChart::Commands::Type::K2START_UP },
        { Input::Pad::K2SELECT, ReplayChart::Commands::Type::K2SELECT_UP },
    },
};

const std::map<ReplayChart::Commands::Type, Input::Pad> REPLAY_CMD_INPUT_DOWN_MAP =
{
    { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
    { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
    { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K11 },
    { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K12 },
    { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K13 },
    { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K14 },
    { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K15 },
    { ReplayChart::Commands::Type::K16_DOWN, Input::Pad::K16 },
    { ReplayChart::Commands::Type::K17_DOWN, Input::Pad::K17 },
    { ReplayChart::Commands::Type::K18_DOWN, Input::Pad::K18 },
    { ReplayChart::Commands::Type::K19_DOWN, Input::Pad::K19 },
    { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
    { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
    { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
    { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
    { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K21 },
    { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K22 },
    { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K23 },
    { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K24 },
    { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K25 },
    { ReplayChart::Commands::Type::K26_DOWN, Input::Pad::K26 },
    { ReplayChart::Commands::Type::K27_DOWN, Input::Pad::K27 },
    { ReplayChart::Commands::Type::K28_DOWN, Input::Pad::K28 },
    { ReplayChart::Commands::Type::K29_DOWN, Input::Pad::K29 },
    { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
    { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
};

const std::map<ReplayChart::Commands::Type, Input::Pad> REPLAY_CMD_INPUT_UP_MAP =
{
    { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
    { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
    { ReplayChart::Commands::Type::K11_UP, Input::Pad::K11 },
    { ReplayChart::Commands::Type::K12_UP, Input::Pad::K12 },
    { ReplayChart::Commands::Type::K13_UP, Input::Pad::K13 },
    { ReplayChart::Commands::Type::K14_UP, Input::Pad::K14 },
    { ReplayChart::Commands::Type::K15_UP, Input::Pad::K15 },
    { ReplayChart::Commands::Type::K16_UP, Input::Pad::K16 },
    { ReplayChart::Commands::Type::K17_UP, Input::Pad::K17 },
    { ReplayChart::Commands::Type::K18_UP, Input::Pad::K18 },
    { ReplayChart::Commands::Type::K19_UP, Input::Pad::K19 },
    { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
    { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
    { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
    { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
    { ReplayChart::Commands::Type::K21_UP, Input::Pad::K21 },
    { ReplayChart::Commands::Type::K22_UP, Input::Pad::K22 },
    { ReplayChart::Commands::Type::K23_UP, Input::Pad::K23 },
    { ReplayChart::Commands::Type::K24_UP, Input::Pad::K24 },
    { ReplayChart::Commands::Type::K25_UP, Input::Pad::K25 },
    { ReplayChart::Commands::Type::K26_UP, Input::Pad::K26 },
    { ReplayChart::Commands::Type::K27_UP, Input::Pad::K27 },
    { ReplayChart::Commands::Type::K28_UP, Input::Pad::K28 },
    { ReplayChart::Commands::Type::K29_UP, Input::Pad::K29 },
    { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
    { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
};

const std::map<ReplayChart::Commands::Type, Input::Pad> REPLAY_CMD_INPUT_DOWN_MAP_5K[4] =
{
    {
        { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K11 },
        { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K12 },
        { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K21 },
        { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K22 },
        { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K11 },
        { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K12 },
        { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K26 },
        { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K27 },
        { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K16 },
        { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K17 },
        { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K21 },
        { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K22 },
        { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_DOWN, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_DOWN, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_DOWN, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K12_DOWN, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K13_DOWN, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K14_DOWN, Input::Pad::K16 },
        { ReplayChart::Commands::Type::K15_DOWN, Input::Pad::K17 },
        { ReplayChart::Commands::Type::K1START_DOWN, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_DOWN, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_DOWN, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_DOWN, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_DOWN, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K22_DOWN, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K23_DOWN, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K24_DOWN, Input::Pad::K26 },
        { ReplayChart::Commands::Type::K25_DOWN, Input::Pad::K27 },
        { ReplayChart::Commands::Type::K2START_DOWN, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_DOWN, Input::Pad::K2SELECT },
    },
};

const std::map<ReplayChart::Commands::Type, Input::Pad> REPLAY_CMD_INPUT_UP_MAP_5K[4] =
{
    {
        { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_UP, Input::Pad::K11 },
        { ReplayChart::Commands::Type::K12_UP, Input::Pad::K12 },
        { ReplayChart::Commands::Type::K13_UP, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K14_UP, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K15_UP, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_UP, Input::Pad::K21 },
        { ReplayChart::Commands::Type::K22_UP, Input::Pad::K22 },
        { ReplayChart::Commands::Type::K23_UP, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K24_UP, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K25_UP, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_UP, Input::Pad::K11 },
        { ReplayChart::Commands::Type::K12_UP, Input::Pad::K12 },
        { ReplayChart::Commands::Type::K13_UP, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K14_UP, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K15_UP, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_UP, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K22_UP, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K23_UP, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K24_UP, Input::Pad::K26 },
        { ReplayChart::Commands::Type::K25_UP, Input::Pad::K27 },
        { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_UP, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K12_UP, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K13_UP, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K14_UP, Input::Pad::K16 },
        { ReplayChart::Commands::Type::K15_UP, Input::Pad::K17 },
        { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_UP, Input::Pad::K21 },
        { ReplayChart::Commands::Type::K22_UP, Input::Pad::K22 },
        { ReplayChart::Commands::Type::K23_UP, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K24_UP, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K25_UP, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
    },
    {
        { ReplayChart::Commands::Type::S1L_UP, Input::Pad::S1L },
        { ReplayChart::Commands::Type::S1R_UP, Input::Pad::S1R },
        { ReplayChart::Commands::Type::K11_UP, Input::Pad::K13 },
        { ReplayChart::Commands::Type::K12_UP, Input::Pad::K14 },
        { ReplayChart::Commands::Type::K13_UP, Input::Pad::K15 },
        { ReplayChart::Commands::Type::K14_UP, Input::Pad::K16 },
        { ReplayChart::Commands::Type::K15_UP, Input::Pad::K17 },
        { ReplayChart::Commands::Type::K1START_UP, Input::Pad::K1START },
        { ReplayChart::Commands::Type::K1SELECT_UP, Input::Pad::K1SELECT },
        { ReplayChart::Commands::Type::S2L_UP, Input::Pad::S2L },
        { ReplayChart::Commands::Type::S2R_UP, Input::Pad::S2R },
        { ReplayChart::Commands::Type::K21_UP, Input::Pad::K23 },
        { ReplayChart::Commands::Type::K22_UP, Input::Pad::K24 },
        { ReplayChart::Commands::Type::K23_UP, Input::Pad::K25 },
        { ReplayChart::Commands::Type::K24_UP, Input::Pad::K26 },
        { ReplayChart::Commands::Type::K25_UP, Input::Pad::K27 },
        { ReplayChart::Commands::Type::K2START_UP, Input::Pad::K2START },
        { ReplayChart::Commands::Type::K2SELECT_UP, Input::Pad::K2SELECT },
    },
};


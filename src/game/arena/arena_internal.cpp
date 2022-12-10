#include "arena_internal.h"

#include <sstream>

#include "cereal/archives/portable_binary.hpp"

using namespace Arena;

struct NetworkPayload
{
	int messageIndex = 0;	// keep messages handled in order
	uint8_t type = 0;		// ArenaMessageType

};

std::shared_ptr<std::vector<unsigned char>> ArenaMessage::pack()
{
	std::stringstream ss;
	auto ret = std::make_shared<std::vector<unsigned char>>();
	try
	{
		//LOG_DEBUG << "[Arena] Pack message type: " << (int)type;

		cereal::PortableBinaryOutputArchive ar(ss);
		switch (type)
		{
		case RESPONSE:                ar(*static_cast<ArenaMessageResponse*>(this)); break;
		case HEARTBEAT:               ar(*static_cast<ArenaMessageHeartbeat*>(this)); break;
		case NOTICE:                  ar(*static_cast<ArenaMessageNotice*>(this)); break;
		case SEEK_LOBBY:              ar(*static_cast<ArenaMessageSeekLobby*>(this)); break;
		case DISBAND_LOBBY:           ar(*static_cast<ArenaMessageDisbandLobby*>(this)); break;
		case JOIN_LOBBY:              ar(*static_cast<ArenaMessageJoinLobby*>(this)); break;
		case LEAVE_LOBBY:             ar(*static_cast<ArenaMessageLeaveLobby*>(this)); break;
		case PLAYER_JOINED_LOBBY:     ar(*static_cast<ArenaMessagePlayerJoinedLobby*>(this)); break;
		case PLAYER_LEFT_LOBBY:       ar(*static_cast<ArenaMessagePlayerLeftLobby*>(this)); break;
		case REQUEST_CHART:           ar(*static_cast<ArenaMessageRequestChart*>(this)); break;
		case CHECK_CHART_EXIST:       ar(*static_cast<ArenaMessageCheckChartExist*>(this)); break;
		case HOST_REQUEST_CHART:      ar(*static_cast<ArenaMessageHostRequestChart*>(this)); break;
		case HOST_READY_STAT:         ar(*static_cast<ArenaMessageHostReadyStat*>(this)); break;
		case HOST_START_PLAYING:      ar(*static_cast<ArenaMessageHostStartPlaying*>(this)); break;
		case CLIENT_PLAY_INIT:        ar(*static_cast<ArenaMessageClientPlayInit*>(this)); break;
		case HOST_PLAY_INIT:          ar(*static_cast<ArenaMessageHostPlayInit*>(this)); break;
		case CLIENT_FINISHED_LOADING: ar(*static_cast<ArenaMessageClientFinishedLoading*>(this)); break;
		case HOST_FINISHED_LOADING:   ar(*static_cast<ArenaMessageHostFinishedLoading*>(this)); break;
		case CLIENT_PLAYDATA:         ar(*static_cast<ArenaMessageClientPlayData*>(this)); break;
		case HOST_PLAYDATA:           ar(*static_cast<ArenaMessageHostPlayData*>(this)); break;
		case CLIENT_FINISHED_PLAYING: ar(*static_cast<ArenaMessageClientFinishedPlaying*>(this)); break;
		case HOST_FINISHED_PLAYING:   ar(*static_cast<ArenaMessageHostFinishedPlaying*>(this)); break;
		case CLIENT_FINISHED_RESULT:  ar(*static_cast<ArenaMessageClientFinishedResult*>(this)); break;
		case HOST_FINISHED_RESULT:    ar(*static_cast<ArenaMessageHostFinishedResult*>(this)); break;
		}
	}
	catch (...)
	{
	}
	if (ss.str().empty())
	{
		LOG_WARNING << "[Arena] Message pack failed. Type: " << (int)type;
		return ret;
	}

	size_t length = ss.tellp();
	ret->resize(length + 1);
	(*ret)[0] = type;
	ss.read((char*)&(*ret)[1], length);

	return ret;
}

std::shared_ptr<ArenaMessage> ArenaMessage::unpack(const unsigned char* data, size_t len)
{
	if (len == 0)
	{
		LOG_WARNING << "[Arena] Empty message received";
		return nullptr;
	}
	if (data[0] == UNDEF || data[0] >= MESSAGE_TYPE_COUNT)
	{
		LOG_WARNING << "[Arena] Invalid message type: " << (int)data[0];
		return nullptr;
	}

	std::stringstream ss;
	ss.write((char*)&data[1], len - 1);
	try
	{
		//LOG_DEBUG << "[Arena] Parse message type: " << (int)data[0];
		
		switch (data[0])
		{
		case RESPONSE:                { auto m = std::make_shared<ArenaMessageResponse>();              { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case HEARTBEAT:               { auto m = std::make_shared<ArenaMessageHeartbeat>();             { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case NOTICE:                  { auto m = std::make_shared<ArenaMessageNotice>();                { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case SEEK_LOBBY:              { auto m = std::make_shared<ArenaMessageSeekLobby>();             { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case DISBAND_LOBBY:           { auto m = std::make_shared<ArenaMessageDisbandLobby>();          { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case JOIN_LOBBY:              { auto m = std::make_shared<ArenaMessageJoinLobby>();             { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case LEAVE_LOBBY:             { auto m = std::make_shared<ArenaMessageLeaveLobby>();            { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case PLAYER_JOINED_LOBBY:     { auto m = std::make_shared<ArenaMessagePlayerJoinedLobby>();     { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case PLAYER_LEFT_LOBBY:       { auto m = std::make_shared<ArenaMessagePlayerLeftLobby>();       { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case REQUEST_CHART:           { auto m = std::make_shared<ArenaMessageRequestChart>();          { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case CHECK_CHART_EXIST:       { auto m = std::make_shared<ArenaMessageCheckChartExist>();       { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case HOST_REQUEST_CHART:	  { auto m = std::make_shared<ArenaMessageHostRequestChart>();      { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case HOST_READY_STAT:         { auto m = std::make_shared<ArenaMessageHostReadyStat>();         { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case HOST_START_PLAYING:      { auto m = std::make_shared<ArenaMessageHostStartPlaying>();      { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case CLIENT_PLAY_INIT:        { auto m = std::make_shared<ArenaMessageClientPlayInit>();        { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case HOST_PLAY_INIT:          { auto m = std::make_shared<ArenaMessageHostPlayInit>();          { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case CLIENT_FINISHED_LOADING: { auto m = std::make_shared<ArenaMessageClientFinishedLoading>(); { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case HOST_FINISHED_LOADING:   { auto m = std::make_shared<ArenaMessageHostFinishedLoading>();   { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case CLIENT_PLAYDATA:         { auto m = std::make_shared<ArenaMessageClientPlayData>();        { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case HOST_PLAYDATA:           { auto m = std::make_shared<ArenaMessageHostPlayData>();          { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case CLIENT_FINISHED_PLAYING: { auto m = std::make_shared<ArenaMessageClientFinishedPlaying>(); { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case HOST_FINISHED_PLAYING:   { auto m = std::make_shared<ArenaMessageHostFinishedPlaying>();   { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case CLIENT_FINISHED_RESULT:  { auto m = std::make_shared<ArenaMessageClientFinishedResult>();  { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		case HOST_FINISHED_RESULT:    { auto m = std::make_shared<ArenaMessageHostFinishedResult>();    { cereal::PortableBinaryInputArchive ar(ss); ar(*m); } return m; }
		}
	}
	catch (...)
	{
	}
	LOG_WARNING << "[Arena] Message parsing failed. Type: " << (int)data[0];
	return nullptr;
}

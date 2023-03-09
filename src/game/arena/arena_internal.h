#pragma once
#include <vector>
#include <memory>
#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/map.hpp"

namespace Arena
{
enum ArenaMessageType : uint8_t
{
	UNDEF,

	RESPONSE,

	HEARTBEAT,					// host->client every 9s; client disconnect if no updates in 30s

	NOTICE,

	SEEK_LOBBY,

	DISBAND_LOBBY,				// no reply

	JOIN_LOBBY,
	LEAVE_LOBBY,
	PLAYER_JOINED_LOBBY,
	PLAYER_LEFT_LOBBY,

	REQUEST_CHART,				// also function as READY
	CHECK_CHART_EXIST,
	HOST_REQUEST_CHART,

	HOST_READY_STAT,
	HOST_START_PLAYING,			// sent when all players accepted chart. also includes random seed

	CLIENT_PLAY_INIT,
	HOST_PLAY_INIT,

	CLIENT_FINISHED_LOADING,	// includes #PLAYSTART
	HOST_FINISHED_LOADING,		// sent when all players loading finished. also includes max #PLAYSTART of all players

	CLIENT_PLAYDATA,			// no reply
	HOST_PLAYDATA,				// no reply

	CLIENT_FINISHED_PLAYING,
	HOST_FINISHED_PLAYING,		// sent when all players finished playing

	CLIENT_FINISHED_RESULT,
	HOST_FINISHED_RESULT,		// sent when all players requested skip result

	MESSAGE_TYPE_COUNT,
};
}

// Implement over UDP
class ArenaMessage
{
public:
	int32_t messageIndex = 0;	// keep messages handled in order
	uint8_t type = 0;		// ArenaMessageType

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ar(messageIndex);
		ar(type);
	}

	std::shared_ptr<std::vector<unsigned char>> pack();
	static std::shared_ptr<ArenaMessage> unpack(const unsigned char* data, size_t len);

};

class ArenaMessageResponse : public ArenaMessage
{
public:
	ArenaMessageResponse() { type = Arena::RESPONSE; }
	ArenaMessageResponse(const ArenaMessage& reqMsg) : ArenaMessageResponse()
	{
		messageIndex = reqMsg.messageIndex;
		reqType = reqMsg.type;
	}

public:
	uint8_t reqType = 0;
	uint8_t errorCode = 0;
	std::vector<unsigned char> payload;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(reqType);
		ar(errorCode);
		ar(payload);
	}
};

class ArenaMessageHeartbeat: public ArenaMessage
{
public:
	ArenaMessageHeartbeat() { type = Arena::HEARTBEAT; }
};

class ArenaMessageNotice : public ArenaMessage
{
public:
	ArenaMessageNotice() { type = Arena::NOTICE; }

public:
	enum Format
	{
		PLAIN,
		S1,
		S2,
		D1,
		D2,
	};

	uint32_t i18nIndex = 0;
	uint32_t format = PLAIN;
	std::string s1;
	std::string s2;
	int32_t d1 = 0;
	int32_t d2 = 0;
	double lf1 = 0.;
	double lf2 = 0.;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(i18nIndex);
		ar(format);
		ar(s1);
		ar(s2);
		ar(d1);
		ar(d2);
		ar(lf1);
		ar(lf2);
	}

};

class ArenaMessageSeekLobby : public ArenaMessage
{
public:
	ArenaMessageSeekLobby() { type = Arena::SEEK_LOBBY; }
};

class ArenaMessageDisbandLobby : public ArenaMessage
{
public:
	ArenaMessageDisbandLobby() { type = Arena::DISBAND_LOBBY; }
};

class ArenaMessageJoinLobby : public ArenaMessage
{
public:
	ArenaMessageJoinLobby() { type = Arena::JOIN_LOBBY; }

	std::string version;
	std::string playerName;
	std::string reserved;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(version);
		ar(playerName);
		ar(reserved);
	}
};

class ArenaMessageLeaveLobby : public ArenaMessage
{
public:
	ArenaMessageLeaveLobby() { type = Arena::LEAVE_LOBBY; }
};

class ArenaMessagePlayerJoinedLobby : public ArenaMessage
{
public:
	ArenaMessagePlayerJoinedLobby() { type = Arena::PLAYER_JOINED_LOBBY; }

	int32_t playerID = 0;
	std::string playerName;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(playerID);
		ar(playerName);
	}
};

class ArenaMessagePlayerLeftLobby : public ArenaMessage
{
public:
	ArenaMessagePlayerLeftLobby() { type = Arena::PLAYER_LEFT_LOBBY; }

	int32_t playerID = 0;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(playerID);
	}
};

class ArenaMessageRequestChart : public ArenaMessage
{
public:
	ArenaMessageRequestChart() { type = Arena::REQUEST_CHART; }

	std::string chartHashMD5String;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(chartHashMD5String);
	}
};

class ArenaMessageCheckChartExist : public ArenaMessage
{
public:
	ArenaMessageCheckChartExist() { type = Arena::CHECK_CHART_EXIST; }

	std::string chartHashMD5String;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(chartHashMD5String);
	}
};

class ArenaMessageHostRequestChart : public ArenaMessage
{
public:
	ArenaMessageHostRequestChart() { type = Arena::HOST_REQUEST_CHART; }

	std::string requestPlayerName;
	std::string chartHashMD5String;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(requestPlayerName);
		ar(chartHashMD5String);
	}
};

class ArenaMessageHostReadyStat : public ArenaMessage
{
public:
	ArenaMessageHostReadyStat() { type = Arena::HOST_READY_STAT; }

	std::map<int32_t, uint8_t> ready;	// using uint8_t instead of bool

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(ready);
	}
};

class ArenaMessageHostStartPlaying : public ArenaMessage
{
public:
	ArenaMessageHostStartPlaying() { type = Arena::HOST_START_PLAYING; }

	uint32_t rulesetType = 0;	// RulesetType
	std::string chartHashMD5String;
	uint64_t randomSeed = 0;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(rulesetType);
		ar(chartHashMD5String);
		ar(randomSeed);
	}
};

class ArenaMessageClientPlayInit : public ArenaMessage
{
public:
	ArenaMessageClientPlayInit() { type = Arena::CLIENT_PLAY_INIT; }

public:
	std::vector<unsigned char> payload;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(payload);
	}
};

class ArenaMessageHostPlayInit : public ArenaMessage
{
public:
	ArenaMessageHostPlayInit() { type = Arena::HOST_PLAY_INIT; }

public:
	int32_t playerID = 0;
	std::vector<unsigned char> rulesetPayload;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(playerID);
		ar(rulesetPayload);
	}
};

class ArenaMessageClientFinishedLoading : public ArenaMessage
{
public:
	ArenaMessageClientFinishedLoading() { type = Arena::CLIENT_FINISHED_LOADING; }

public:
	int32_t playStartTimeMs = 0;	// play skin #PLAYSTART

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(playStartTimeMs);
	}
};

class ArenaMessageHostFinishedLoading : public ArenaMessage
{
public:
	ArenaMessageHostFinishedLoading() { type = Arena::HOST_FINISHED_LOADING; }

public:
	int32_t playStartTimeMs = 0;	// adjust each player timing by ping

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(playStartTimeMs);
	}
};

class ArenaMessageClientPlayData : public ArenaMessage
{
public:
	ArenaMessageClientPlayData() { type = Arena::CLIENT_PLAYDATA; }

public:
	std::vector<unsigned char> payload;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(payload);
	}
};

class ArenaMessageHostPlayData : public ArenaMessage
{
public:
	ArenaMessageHostPlayData() { type = Arena::HOST_PLAYDATA; }

public:
	std::map<int, std::vector<unsigned char>> payload;

public:
	template<class Archive>
	void serialize(Archive& ar)
	{
		ArenaMessage::serialize(ar);
		ar(payload);
	}
};

class ArenaMessageClientFinishedPlaying : public ArenaMessage
{
public:
	ArenaMessageClientFinishedPlaying() { type = Arena::CLIENT_FINISHED_PLAYING; }
};

class ArenaMessageHostFinishedPlaying : public ArenaMessage
{
public:
	ArenaMessageHostFinishedPlaying() { type = Arena::HOST_FINISHED_PLAYING; }
};

class ArenaMessageClientFinishedResult : public ArenaMessage
{
public:
	ArenaMessageClientFinishedResult() { type = Arena::CLIENT_FINISHED_RESULT; }
};

class ArenaMessageHostFinishedResult : public ArenaMessage
{
public:
	ArenaMessageHostFinishedResult() { type = Arena::HOST_FINISHED_RESULT; }
};

/////////////////////////////////////////////////////////////////

struct ArenaLobbyResp
{
	std::string name;
	std::string address;
	uint16_t port;
	uint16_t playerCount;

	template<class Archive>
	void serialize(Archive& ar)
	{
		ar(name);
		ar(address);
		ar(port);
		ar(playerCount);
	}
};

struct ArenaJoinLobbyResp
{
	int32_t playerID;
	std::map<int32_t, std::string> playerName;

	template<class Archive>
	void serialize(Archive& ar)
	{
		ar(playerID);
		ar(playerName);
	}
};

struct ArenaCheckChartExistResp
{
	bool exist = false;

	template<class Archive>
	void serialize(Archive& ar)
	{
		ar(exist);
	}
};
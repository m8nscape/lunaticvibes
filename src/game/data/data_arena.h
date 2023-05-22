#pragma once
#include "game/ruleset/ruleset.h"

namespace lunaticvibes
{

class ArenaHost;
class ArenaClient;
class vRulesetNetwork;

constexpr size_t MAX_ARENA_PLAYERS = 8;			// not including yourself
constexpr uint16_t ARENA_HOST_PORT = 50201;		// 100201 - 50000

extern std::shared_ptr<ArenaHost> g_pArenaHost;
extern std::shared_ptr<ArenaClient> g_pArenaClient;

struct ArenaPlayData
{
    std::string name;
    bool ready = false;
    std::shared_ptr<vRulesetNetwork> ruleset;
};

inline class Class_ArenaData
{
public:
	friend class ArenaHost;
	friend class ArenaClient;

public:
    std::unordered_map<std::string, long long> timers;

    HashMD5 remoteRequestedChart;       // only valid when remote is requesting a new chart; reset after list change
    std::string remoteRequestedPlayer;  // only valid when remote is requesting a new chart; reset after list change

    bool isArenaReady = false;
    bool isInArenaRequest = false;
    bool isArenaCancellingRequest = false;

    int myRanking = 0;
    int playerRanking[MAX_ARENA_PLAYERS] = { 0 };


private:
	bool online = false;
	bool expired = false;

	bool playing = false;
	int playStartTimeMs = 0;
	bool playingFinished = false;
	uint64_t randomSeed = 0;

	bool ready = false;
	std::map<int, ArenaPlayData> data;	// host:0 client:1+
	std::vector<int> playerIDs;

public:
	void reset();
	constexpr bool isOnline() const { return online; }
	constexpr bool isExpired() const { return expired; }
	bool isServer() const { return g_pArenaHost != nullptr; }
	bool isClient() const { return g_pArenaClient != nullptr; }

	void initPlaying(RulesetType rulesetType);
	void startPlaying();
	void stopPlaying();

	bool isPlaying() const { return playing; }
	int getPlayStartTimeMs() const { return playStartTimeMs; }
	bool isPlayingFinished() const { return playingFinished; }
	uint64_t getRandomSeed() const { return randomSeed; }

	size_t getPlayerCount();
	const std::string& getPlayerName(size_t index);
	std::shared_ptr<vRulesetNetwork> getPlayerRuleset(size_t index);
	int getPlayerID(size_t index);

	bool isSelfReady();
	bool isPlayerReady(size_t index);

	void updateGlobals();

} ArenaData;

}
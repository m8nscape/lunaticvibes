#pragma once

#include <deque>

#include "game/ruleset/ruleset_network.h"

constexpr size_t MAX_ARENA_PLAYERS = 8;			// not including yourself
constexpr uint16_t ARENA_HOST_PORT = 50201;		// 100201 - 50000

struct ArenaPlayData
{
	std::string name;
	std::shared_ptr<vRulesetNetwork> ruleset;
};

class ArenaHost;
class ArenaClient;
extern std::shared_ptr<ArenaHost> g_pArenaHost;
extern std::shared_ptr<ArenaClient> g_pArenaClient;

class ArenaData
{
public:
	ArenaData() = default;
	friend class ArenaHost;
	friend class ArenaClient;

private:
	bool online = false;
	bool expired = false;

	bool playing = false;
	bool playReady = false;
	int playStartTimeMs = 0;
	bool playingFinished = false;

	std::map<int, ArenaPlayData> data;	// host:0 client:1+
	std::vector<int> playerIDs;

public:
	void reset();
	constexpr bool isOnline() const { return online; }
	constexpr bool isExpired() const { return expired; }
	bool isServer() const { return g_pArenaHost != nullptr; }
	bool isClient() const { return g_pArenaClient != nullptr; }

	void initPlaying(eRuleset rulesetType);
	void startPlaying();
	void stopPlaying();

	bool isPlaying() const { return playing; }
	int getPlayStartTimeMs() const { return playStartTimeMs; }

	bool isPlayingFinished() const { return playingFinished; }

	size_t getPlayerCount();
	const std::string& getPlayerName(size_t index);
	std::shared_ptr<vRulesetNetwork> getPlayerRuleset(size_t index);
	int getPlayerID(size_t index);

	void updateTexts();
	void updateGlobals();
};

extern ArenaData gArenaData;

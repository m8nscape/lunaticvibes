#include "common/pch.h"
#include "data_arena.h"
#include "data_play.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms_network.h"

namespace lunaticvibes
{

void Class_ArenaData::reset()
{
	online = false;
	expired = false;
	playing = false;
	data.clear();
	playerIDs.clear();
	playStartTimeMs = 0;
	playingFinished = false;
	ready = false;

	if (g_pArenaClient)
	{
		g_pArenaClient->loopEnd();
		g_pArenaClient.reset();
	}
	if (g_pArenaHost)
	{
		g_pArenaHost->loopEnd();
		g_pArenaHost.reset();
	}
}

size_t Class_ArenaData::getPlayerCount()
{
	return playerIDs.size();
}

const std::string& Class_ArenaData::getPlayerName(size_t index)
{
	static const std::string empty = "";
	return index < getPlayerCount() ? data[playerIDs[index]].name : empty;
}

std::shared_ptr<vRulesetNetwork> Class_ArenaData::getPlayerRuleset(size_t index)
{
	return index < getPlayerCount() && playing ? data[playerIDs[index]].ruleset : nullptr;
}

int Class_ArenaData::getPlayerID(size_t index)
{
	return index < getPlayerCount() ? playerIDs[index] : -1;
}

bool Class_ArenaData::isSelfReady()
{
	return ready;
}

bool Class_ArenaData::isPlayerReady(size_t index)
{
	return index < getPlayerCount() ? data[playerIDs[index]].ready : false;
}

void Class_ArenaData::initPlaying(RulesetType rulesetType)
{
	unsigned keys = 7;
	switch (PlayData.mode)
	{
	case SkinType::PLAY5:
	case SkinType::PLAY5_2: keys = 5; break;
	case SkinType::PLAY7:
	case SkinType::PLAY7_2: keys = 7; break;
	case SkinType::PLAY9:
	case SkinType::PLAY9_2: keys = 9; break;
	case SkinType::PLAY10: keys = 10; break;
	case SkinType::PLAY14: keys = 14; break;
	default: break;
	}

	unsigned idx = 0;

	for (auto& [id, d] : data)
	{
		switch (rulesetType)
		{
		case RulesetType::BMS: d.ruleset = std::make_shared<RulesetBMSNetwork>(keys, idx++); break;
		}

	}
}

void Class_ArenaData::startPlaying()
{
	playing = true;
}

void Class_ArenaData::stopPlaying()
{
	ready = false;
	playStartTimeMs = 0;
	playingFinished = false;
	playing = false;

	for (auto& [id, d] : data)
	{
		d.ready = false;
		d.ruleset.reset();
	}
}

void Class_ArenaData::updateGlobals()
{
	Time t;
	std::vector<std::pair<unsigned, int*>> ranking;
	for (size_t i = 0; i < getPlayerCount(); ++i)
	{
		auto& d = data[playerIDs[i]];
		if (d.ruleset)
		{
			d.ruleset->update(t);
			if (auto p = std::dynamic_pointer_cast<RulesetBMS>(d.ruleset); p)
			{
				ranking.push_back({ p->getExScore(), &ArenaData.playerRanking[i] });
			}
		}
	}

	auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
	if (r)
	{
		ranking.push_back({ r->getExScore(), &ArenaData.myRanking });
	}

	std::sort(ranking.begin(), ranking.end());

	int rank = 1;
	int step = 0;
	unsigned exscorePrev = 0;
	for (auto it = ranking.rbegin(); it != ranking.rend(); ++it)
	{
		auto& [exscore, ranking] = *it;
		if (exscore == exscorePrev)
		{
			step++;
		}
		else
		{
			rank += step;
			exscorePrev = exscore;
			step = 1;
		}
		*ranking = rank;
	}
}


}
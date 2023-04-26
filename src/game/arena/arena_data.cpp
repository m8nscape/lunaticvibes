#include "arena_data.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"
#include "game/ruleset/ruleset_bms_network.h"
#include "game/scene/scene_context.h"

ArenaDataInternal gArenaData;

void ArenaDataInternal::reset()
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

size_t ArenaDataInternal::getPlayerCount()
{ 
	return playerIDs.size(); 
}

const std::string& ArenaDataInternal::getPlayerName(size_t index)
{
	static const std::string empty = "";
	return index < getPlayerCount() ? data[playerIDs[index]].name : empty;
}

std::shared_ptr<vRulesetNetwork> ArenaDataInternal::getPlayerRuleset(size_t index)
{
	return index < getPlayerCount() && playing ? data[playerIDs[index]].ruleset : nullptr;
}

int ArenaDataInternal::getPlayerID(size_t index)
{
	return index < getPlayerCount() ? playerIDs[index] : -1;
}

bool ArenaDataInternal::isSelfReady()
{
	return ready;
}

bool ArenaDataInternal::isPlayerReady(size_t index)
{
	return index < getPlayerCount() ? data[playerIDs[index]].ready: false;
}

void ArenaDataInternal::initPlaying(RulesetType rulesetType)
{
	unsigned keys = 7;
	switch (gPlayContext.mode)
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

void ArenaDataInternal::startPlaying()
{
	playing = true;
}

void ArenaDataInternal::stopPlaying()
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

void ArenaDataInternal::updateTexts()
{
	State::set(IndexText::ARENA_PLAYER_NAME_1, gArenaData.getPlayerName(0));
	State::set(IndexText::ARENA_PLAYER_NAME_2, gArenaData.getPlayerName(1));
	State::set(IndexText::ARENA_PLAYER_NAME_3, gArenaData.getPlayerName(2));
	State::set(IndexText::ARENA_PLAYER_NAME_4, gArenaData.getPlayerName(3));
	State::set(IndexText::ARENA_PLAYER_NAME_5, gArenaData.getPlayerName(4));
	State::set(IndexText::ARENA_PLAYER_NAME_6, gArenaData.getPlayerName(5));
	State::set(IndexText::ARENA_PLAYER_NAME_7, gArenaData.getPlayerName(6));
	State::set(IndexText::ARENA_PLAYER_NAME_8, gArenaData.getPlayerName(7));

	for (size_t i = 0; i < MAX_ARENA_PLAYERS; ++i)
	{
		if (auto prb = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(i)); prb)
		{
			State::set(IndexText(size_t(IndexText::ARENA_MODIFIER_1) + i), prb->getModifierText());
			State::set(IndexText(size_t(IndexText::ARENA_MODIFIER_SHORT_1) + i), prb->getModifierTextShort());
		}
	}
}

void ArenaDataInternal::updateGlobals()
{
	Time t;
	std::vector<std::pair<unsigned, IndexOption>> ranking;
	for (size_t i = 0; i < getPlayerCount(); ++i)
	{
		auto& d = data[playerIDs[i]];
		if (d.ruleset)
		{
			d.ruleset->update(t);
			if (auto p = std::dynamic_pointer_cast<RulesetBMS>(d.ruleset); p)
			{
				size_t offset = (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * i;
				ranking.push_back({ p->getExScore(), IndexOption(int(IndexOption::ARENA_PLAYDATA_RANKING) + offset) });
			}
		}
	}
	ranking.push_back({ State::get(IndexNumber::PLAY_1P_EXSCORE), IndexOption::RESULT_ARENA_PLAYER_RANKING });

	std::sort(ranking.begin(), ranking.end());
	int rank = 1;
	int step = 0;
	unsigned exscorePrev = 0;
	for (auto it = ranking.rbegin(); it != ranking.rend(); ++it)
	{
		auto& [exscore, op] = *it;
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
		State::set(op, rank);
	}
}
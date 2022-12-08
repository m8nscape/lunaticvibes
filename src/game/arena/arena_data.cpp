#include "arena_data.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"
#include "game/ruleset/ruleset_bms_network.h"
#include "game/scene/scene_context.h"

ArenaData gArenaData;

void ArenaData::reset()
{
	online = false;
	expired = false;
	playing = false;
	data.clear();
	playerIDs.clear();
	playReady = false;
	playStartTimeMs = 0;
	playingFinished = false;

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

size_t ArenaData::getPlayerCount()
{ 
	return playerIDs.size(); 
}

const std::string& ArenaData::getPlayerName(size_t index)
{
	static const std::string empty = "";
	return index < getPlayerCount() ? data[playerIDs[index]].name : empty;
}

std::shared_ptr<vRulesetNetwork> ArenaData::getPlayerRuleset(size_t index)
{
	return index < getPlayerCount() && playing ? data[playerIDs[index]].ruleset : nullptr;
}


void ArenaData::initPlaying(eRuleset rulesetType)
{
	unsigned keys = 7;
	switch (gPlayContext.mode)
	{
	case eMode::PLAY5:
	case eMode::PLAY5_2: keys = 5; break;
	case eMode::PLAY7:
	case eMode::PLAY7_2: keys = 7; break;
	case eMode::PLAY9:
	case eMode::PLAY9_2: keys = 9; break;
	case eMode::PLAY10: keys = 10; break;
	case eMode::PLAY14: keys = 14; break;
	default: break;
	}

	unsigned idx = 0;

	for (auto& [id, d] : data)
	{
		switch (rulesetType)
		{
		case eRuleset::BMS: d.ruleset = std::make_shared<RulesetBMSNetwork>(keys, idx++); break;
		}
		
	}
}

void ArenaData::startPlaying()
{
	playing = true;
}

void ArenaData::stopPlaying()
{
	playing = false;

	for (auto& [id, d] : data)
	{
		d.ruleset.reset();
	}
}

void ArenaData::updateTexts()
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

void ArenaData::updateGlobals()
{
	Time t;
	for (auto& [id, d] : data)
	{
		if (d.ruleset)
		{
			d.ruleset->update(t);
		}
	}
}
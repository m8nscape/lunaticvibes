#include "common/pch.h"
#include "ruleset_bms_network.h"

#include "game/data/data_play.h"

namespace lunaticvibes
{

RulesetBMSNetwork::RulesetBMSNetwork(GameModeKeys keys, unsigned playerIndex) :
    RulesetBase(nullptr, nullptr), RulesetBMS(nullptr, nullptr, PlayModifierGaugeType::NORMAL, keys, JudgeDifficulty::NORMAL, 1.0, PlaySide::NETWORK), playerIndex(playerIndex)
{
    showJudge = false;
    _judgeScratch = false;
}

void RulesetBMSNetwork::update(const Time& t)
{
    if (!_hasStartTime)
        setStartTime(t);
}

std::vector<unsigned char> RulesetBMSNetwork::packInit(std::shared_ptr<RulesetBMS> local)
{
    PayloadInit p;
    p.randomLeft = (int8_t)PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft;
    p.randomRight = (int8_t)PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight;
    p.gauge = (int8_t)PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge;
    p.assist_mask = PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask;
    p.dpflip = PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip;
    p.health = local->getData().health;
    p.maxMoneyScore = local->getMaxMoneyScore();
    p.modifierText = local->getModifierText();
    p.modifierTextShort = local->getModifierTextShort();

    std::stringstream ss;
    std::vector<unsigned char> ret;
    try
    {
        ::cereal::PortableBinaryOutputArchive ar(ss);
        ar(p);
    }
    catch (...)
    {
        return ret;
    }

    size_t length = ss.tellp();
    ret.resize(length);
    ss.read((char*)&ret[0], length);

    return ret;
}

bool RulesetBMSNetwork::unpackInit(const std::vector<unsigned char>& payload)
{
    std::stringstream ss;
    ss.write((char*)&payload[0], payload.size());

    PayloadInit p;
    try
    {
        ::cereal::PortableBinaryInputArchive ar(ss);
        ar(p);
    }
    catch (...)
    {
        return false;
    }

    randomLeft = (PlayModifierRandomType)p.randomLeft;
    randomRight = (PlayModifierRandomType)p.randomRight;
    initGaugeParams((PlayModifierGaugeType)p.gauge);
    assist_mask = p.assist_mask;
    dpflip = p.dpflip;
    _basic.health = p.health;
    maxMoneyScore = p.maxMoneyScore;
    modifierText = p.modifierText;
    modifierTextShort = p.modifierTextShort;
    isInitialized = true;

    return true;
}


std::vector<unsigned char> RulesetBMSNetwork::packFrame(std::shared_ptr<RulesetBMS> local)
{
    PayloadFrame p;
    const auto d = local->getData();
    p.health = d.health;
    p.acc = d.acc;
    p.total_acc = d.total_acc;
    p.combo = d.combo;
    p.maxCombo = d.maxCombo;
    memcpy(p.judge, d.judge, sizeof(p.judge));
    p.isFinished = local->isFinished();
    p.isCleared = local->isCleared();
    p.isFailed = local->isFailed();
    p.moneyScore = local->getScore();
    p.exScore = local->getExScore();

    std::stringstream ss;
    std::vector<unsigned char> ret;
    try
    {
        ::cereal::PortableBinaryOutputArchive ar(ss);
        ar(p);
    }
    catch (...)
    {
        return ret;
    }

    size_t length = ss.tellp();
    ret.resize(length);
    ss.read((char*)&ret[0], length);

    return ret;
}

bool RulesetBMSNetwork::unpackFrame(std::vector<unsigned char>& payload)
{
    std::stringstream ss;
    ss.write((char*)&payload[0], payload.size());

    PayloadFrame p;
    try
    {
        ::cereal::PortableBinaryInputArchive ar(ss);
        ar(p);
    }
    catch (...)
    {
        return false;
    }

    _basic.health = p.health;
    _basic.acc = p.acc;
    _basic.total_acc = p.total_acc;
    _basic.combo = p.combo;
    _basic.maxCombo = p.maxCombo;
    memcpy(_basic.judge, p.judge, sizeof(_basic.judge));
    moneyScore = p.moneyScore;
    exScore = p.exScore;
    _isFinished = p.isFinished;
    _isCleared = p.isCleared;
    _isFailed = p.isFailed;

    return true;
}

}

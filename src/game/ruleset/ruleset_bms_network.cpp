#include "ruleset_bms_network.h"

#include <iostream>
#include <sstream>

#include "game/scene/scene_context.h"

#include "cereal/archives/portable_binary.hpp"

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

    auto B = [this](IndexBargraph base) -> IndexBargraph
    {
        return IndexBargraph((int)base + ((int)IndexBargraph::ARENA_PLAYDATA_MAX - (int)IndexBargraph::ARENA_PLAYDATA_BASE + 1) * playerIndex);
    };
    auto N = [this](IndexNumber base) -> IndexNumber
    {
        return IndexNumber((int)base + ((int)IndexNumber::ARENA_PLAYDATA_MAX - (int)IndexNumber::ARENA_PLAYDATA_BASE + 1) * playerIndex);
    };
    auto O = [this](IndexOption base) -> IndexOption
    {
        return IndexOption((int)base + ((int)IndexOption::ARENA_PLAYDATA_MAX - (int)IndexOption::ARENA_PLAYDATA_BASE + 1) * playerIndex);
    };

    State::set(B(IndexBargraph::ARENA_PLAYDATA_EXSCORE), _basic.total_acc / 100.0);
    State::set(B(IndexBargraph::ARENA_PLAYDATA_EXSCORE_PREDICT), _basic.acc / 100.0);

    State::set(N(IndexNumber::ARENA_PLAYDATA_SCORE), int(std::round(moneyScore)));
    State::set(N(IndexNumber::ARENA_PLAYDATA_EXSCORE), exScore);
    State::set(N(IndexNumber::ARENA_PLAYDATA_NOWCOMBO), _basic.combo + _basic.comboDisplay);
    State::set(N(IndexNumber::ARENA_PLAYDATA_MAXCOMBO), _basic.maxComboDisplay);
    State::set(N(IndexNumber::ARENA_PLAYDATA_RATE), int(std::floor(_basic.acc)));
    State::set(N(IndexNumber::ARENA_PLAYDATA_RATEDECIMAL), int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
    State::set(N(IndexNumber::ARENA_PLAYDATA_TOTALNOTES), getNoteCount());
    State::set(N(IndexNumber::ARENA_PLAYDATA_TOTAL_RATE), int(std::floor(_basic.total_acc)));
    State::set(N(IndexNumber::ARENA_PLAYDATA_TOTAL_RATEDECIMAL), int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
    State::set(N(IndexNumber::ARENA_PLAYDATA_PERFECT), _basic.judge[JUDGE_PERFECT]);
    State::set(N(IndexNumber::ARENA_PLAYDATA_GREAT), _basic.judge[JUDGE_GREAT]);
    State::set(N(IndexNumber::ARENA_PLAYDATA_GOOD), _basic.judge[JUDGE_GOOD]);
    State::set(N(IndexNumber::ARENA_PLAYDATA_BAD), _basic.judge[JUDGE_BAD]);
    State::set(N(IndexNumber::ARENA_PLAYDATA_POOR), _basic.judge[JUDGE_POOR]);
    State::set(N(IndexNumber::ARENA_PLAYDATA_GROOVEGAUGE), int(_basic.health * 100));
    State::set(N(IndexNumber::ARENA_PLAYDATA_MISS), _basic.judge[JUDGE_MISS]);
    State::set(N(IndexNumber::ARENA_PLAYDATA_COMBOBREAK), _basic.judge[JUDGE_CB]);
    State::set(N(IndexNumber::ARENA_PLAYDATA_BPOOR), _basic.judge[JUDGE_KPOOR]);
    State::set(N(IndexNumber::ARENA_PLAYDATA_BP), _basic.judge[JUDGE_BP]);

    State::set(O(IndexOption::ARENA_PLAYDATA_RANK_ESTIMATED), Option::getRankType(_basic.acc));
    State::set(O(IndexOption::ARENA_PLAYDATA_RANK), Option::getRankType(_basic.total_acc));
    State::set(O(IndexOption::ARENA_PLAYDATA_HEALTH), Option::getHealthType(_basic.health));

    int maxScore = getMaxScore();
    //if      (dp.total_acc >= 94.44) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * 1.000 - dp.score2));    // MAX-
    if (_basic.total_acc >= 100.0 * 8.0 / 9) State::set(N(IndexNumber::ARENA_PLAYDATA_NEXT_RANK_EX_DIFF), exScore - maxScore);    // MAX-
    else if (_basic.total_acc >= 100.0 * 7.0 / 9) State::set(N(IndexNumber::ARENA_PLAYDATA_NEXT_RANK_EX_DIFF), int(exScore - maxScore * 8.0 / 9));    // AAA-
    else if (_basic.total_acc >= 100.0 * 6.0 / 9) State::set(N(IndexNumber::ARENA_PLAYDATA_NEXT_RANK_EX_DIFF), int(exScore - maxScore * 7.0 / 9));    // AA-
    else if (_basic.total_acc >= 100.0 * 5.0 / 9) State::set(N(IndexNumber::ARENA_PLAYDATA_NEXT_RANK_EX_DIFF), int(exScore - maxScore * 6.0 / 9));    // A-
    else if (_basic.total_acc >= 100.0 * 4.0 / 9) State::set(N(IndexNumber::ARENA_PLAYDATA_NEXT_RANK_EX_DIFF), int(exScore - maxScore * 5.0 / 9));    // B-
    else if (_basic.total_acc >= 100.0 * 3.0 / 9) State::set(N(IndexNumber::ARENA_PLAYDATA_NEXT_RANK_EX_DIFF), int(exScore - maxScore * 4.0 / 9));    // C-
    else if (_basic.total_acc >= 100.0 * 2.0 / 9) State::set(N(IndexNumber::ARENA_PLAYDATA_NEXT_RANK_EX_DIFF), int(exScore - maxScore * 3.0 / 9));    // D-
    else                                          State::set(N(IndexNumber::ARENA_PLAYDATA_NEXT_RANK_EX_DIFF), int(exScore - maxScore * 2.0 / 9));    // E-

    if (auto player = std::dynamic_pointer_cast<RulesetBMS>(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]); player)
        State::set(N(IndexNumber::ARENA_PLAYDATA_PLAYER_EX_DIFF), player->getExScore() - exScore);

    Option::e_lamp_type lamp = Option::LAMP_NOPLAY;
    if (!isNoScore() && isFinished())
    {
        if (_basic.judge[JUDGE_CB] == 0)
        {
            if (_basic.acc >= 100.0)
                lamp = Option::LAMP_MAX;
            else if (_basic.judge[JUDGE_GOOD] == 0)
                lamp = Option::LAMP_PERFECT;
            else if (isCleared())
                lamp = Option::LAMP_FULLCOMBO;
        }
        else if (isFailed())
        {
            lamp = Option::LAMP_FAILED;
        }
        else
        {
            switch (_gauge)
            {
            case GaugeType::HARD:       lamp = Option::LAMP_HARD; break;
            case GaugeType::EXHARD:     lamp = Option::LAMP_EXHARD; break;
            case GaugeType::DEATH:      lamp = Option::LAMP_FULLCOMBO; break;
                //case GaugeType::P_ATK:      lamp = Option::LAMP_FULLCOMBO; break;
                //case GaugeType::G_ATK:      lamp = Option::LAMP_FULLCOMBO; break;
            case GaugeType::GROOVE:     lamp = Option::LAMP_NORMAL; break;
            case GaugeType::EASY:       lamp = Option::LAMP_EASY; break;
            case GaugeType::ASSIST:     lamp = Option::LAMP_ASSIST; break;
            case GaugeType::GRADE:      lamp = Option::LAMP_NOPLAY; break;
            case GaugeType::EXGRADE:    lamp = Option::LAMP_NOPLAY; break;
            default:
                break;
            }
        }
    }
    State::set(O(IndexOption::ARENA_PLAYDATA_CLEAR_TYPE), std::min(lamp, saveLampMax));
}

std::vector<unsigned char> RulesetBMSNetwork::packInit(std::shared_ptr<RulesetBMS> local)
{
    PayloadInit p;
    p.randomLeft = (int8_t)gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft;
    p.randomRight = (int8_t)gPlayContext.mods[PLAYER_SLOT_PLAYER].randomRight;
    p.gauge = (int8_t)gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge;
    p.assist_mask = gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask;
    p.dpflip = gPlayContext.mods[PLAYER_SLOT_PLAYER].DPFlip;
    p.health = local->getData().health;
    p.maxMoneyScore = local->getMaxMoneyScore();
    p.modifierText = local->getModifierText();
    p.modifierTextShort = local->getModifierTextShort();

    std::stringstream ss;
    std::vector<unsigned char> ret;
    try
    {
        cereal::PortableBinaryOutputArchive ar(ss);
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
        cereal::PortableBinaryInputArchive ar(ss);
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
        cereal::PortableBinaryOutputArchive ar(ss);
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
        cereal::PortableBinaryInputArchive ar(ss);
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
#pragma once

namespace lv
{

constexpr unsigned MAX_PLAYERS = 3;
constexpr unsigned PLAYER_SLOT_PLAYER = 0;
constexpr unsigned PLAYER_SLOT_TARGET = 1;
constexpr unsigned PLAYER_SLOT_MYBEST = 2;

class ::ChartObjectBase;
class ::RulesetBase;
class ::ReplayChart;
struct PlayerPlayData
{
    std::shared_ptr<ChartObjectBase> chartObj;
    std::shared_ptr<RulesetBase> ruleset;
    double initialHealth = 1.0;

    std::vector<int> graphGauge;
    std::vector<double> graphRate;
    PlayModifiers mods;

    double hispeed = 2.0;
    Time   hispeedGradientStart;
    double hispeedGradientFrom = 2.0;
    double hispeedGradientNow = 2.0;

    int lanecoverTop = 0;       // 0-1000
    int lanecoverBottom = 0;    // 0-1000

    int offsetVisual = 0;

    unsigned courseRunningCombo = 0;
    unsigned courseMaxCombo = 0;
};

inline struct Struct_PlayData
{
    std::unordered_map<std::string, long long> timer;

    SkinType mode = SkinType::PLAY7;

    uint64_t randomSeed;

    bool isAuto = false;
    bool isReplay = false;
    bool haveReplay = false;
    bool isBattle = false;  // Note: DB is NOT Battle

    std::array<PlayerPlayData, MAX_PLAYERS> player;

    int ghostType = 0;

    std::shared_ptr<ReplayChart> replay;
    std::shared_ptr<ReplayChart> replayMybest;
    std::shared_ptr<ReplayChart> replayNew;

    bool isCourse = false;
    int courseStage = 0;
    HashMD5 courseHash;
    std::vector<HashMD5> courseCharts;
    std::vector<std::shared_ptr<RulesetBase>> courseStageRulesetCopy[2];
    std::vector<Path> courseStageReplayPath;
    std::vector<Path> courseStageReplayPathNew;

    bool shift1PNotes5KFor7KSkin = false;
    bool shift2PNotes5KFor7KSkin = false;

    double loadProgressWav = 0.0;
    double loadProgressBga = 0.0;

    std::unordered_map<Input::Pad, bool> keyDown;

} PlayData;

}
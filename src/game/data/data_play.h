#pragma once
#include "common/keymap.h"
#include "game/graphics/texture_extra.h"

namespace lunaticvibes
{

class ChartObjectBase;
class RulesetBase;
class ReplayChart;

constexpr unsigned MAX_PLAYERS = 3;
constexpr unsigned PLAYER_SLOT_PLAYER = 0;
constexpr unsigned PLAYER_SLOT_TARGET = 1;
constexpr unsigned PLAYER_SLOT_MYBEST = 2;

enum class TargetType
{
    Zero,
    MyBest,
    RankAAA,
    RankAA,
    RankA,
    UseTargetRate,
};

enum class RankType
{
    MAX,
    AAA,
    AA,
    A,
    B,
    C,
    D,
    E,
    F,
    _
};
inline RankType getRankType(double rate)
{
    if (rate >= 100.0)              return RankType::MAX;
    else if (rate >= 100.0 * 8 / 9) return RankType::AAA;
    else if (rate >= 100.0 * 7 / 9) return RankType::AA;
    else if (rate >= 100.0 * 6 / 9) return RankType::A;
    else if (rate >= 100.0 * 5 / 9) return RankType::B;
    else if (rate >= 100.0 * 4 / 9) return RankType::C;
    else if (rate >= 100.0 * 3 / 9) return RankType::D;
    else if (rate >= 100.0 * 2 / 9) return RankType::E;
    else if (rate > 0.0)            return RankType::F;
    else                            return RankType::_;
}

constexpr int32_t PANEL_STYLE_POS_MASK = 3 << 0;
constexpr int32_t PANEL_STYLE_POS_LEFT = 0 << 0;
constexpr int32_t PANEL_STYLE_POS_MIDDLE = 1 << 0;
constexpr int32_t PANEL_STYLE_POS_RIGHT = 2 << 0;
constexpr int32_t PANEL_STYLE_GRAPH_MASK = 3 << 2;
constexpr int32_t PANEL_STYLE_GRAPH_OFF = 0 << 2;
constexpr int32_t PANEL_STYLE_GRAPH_FAR = 1 << 2;
constexpr int32_t PANEL_STYLE_GRAPH_NEAR = 2 << 2;
constexpr int32_t PANEL_STYLE_GRAPH_NEAR2 = 3 << 2;
constexpr int32_t PANEL_STYLE_BGA_MASK = 3 << 4;
constexpr int32_t PANEL_STYLE_BGA_OFF = 0 << 4;
constexpr int32_t PANEL_STYLE_BGA_SIDE = 1 << 4;
constexpr int32_t PANEL_STYLE_BGA_SIDE_EXPAND = 2 << 4;
constexpr int32_t PANEL_STYLE_BGA_FULLSCREEN = 3 << 4;

enum class GhostScorePosition
{
    Off,
    AboveJudge,
    NearJudge,
    NearJudgeLower,
};

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

    bool lockSpeed = false;
    double greenNumber = 300.0;
    double greenNumberMinBPM = 300.0;
    double greenNumberMaxBPM = 300.0;
    double speedValueInternal = 1.0;

    int lanecoverTop = 0;       // 0-1000
    int lanecoverBottom = 0;    // 0-1000

    int offsetVisual = 0;

    unsigned courseRunningCombo = 0;
    unsigned courseMaxCombo = 0;

    bool showingPoorBga = false;
    bool adjustingLanecover = false;
    bool adjustingHispeed = false;
};

inline struct Struct_PlayData
{
    std::unordered_map<std::string, long long> timers;

    SkinType mode = SkinType::PLAY7;
    int32_t panelStyle = 0;

    uint64_t randomSeed;

    bool isAuto = false;
    bool isReplay = false;

    PlayModifierBattleType battleType = PlayModifierBattleType::Off;

    std::shared_ptr<TextureBmsBga> bgaTexture = std::make_shared<TextureBmsBga>();

    std::array<PlayerPlayData, MAX_PLAYERS> player;

    GhostScorePosition ghostType = GhostScorePosition::Off;

    std::shared_ptr<ReplayChart> replay;
    std::shared_ptr<ReplayChart> replayMybest;
    std::shared_ptr<ReplayChart> replayNew;

    int courseStage = -1;    // -1 not course / 0~8 (0: stage1, etc)
    HashMD5 courseHash;
    struct CourseStageData
    {
        HashMD5 hash;
        bool fileExists = false;
        StringContent title;
        StringContent subTitle;
        int level = 0;
        int difficulty = 0;
        std::shared_ptr<RulesetBase> rulesetCopy[2];
        Path replayPath;
        Path replayPathNew;
    };
    std::vector<CourseStageData> courseStageData;

    bool canRetry = false;

    bool shift1PNotes5KFor7KSkin = false;
    bool shift2PNotes5KFor7KSkin = false;

    double loadProgressWav = 0.0;
    double loadProgressBga = 0.0;

    bool playStarted = false;

    std::unordered_map<Input::Pad, bool> keyDown;
    int displayCombo[2] = { 0 };

    double scratchAxis[2] = {0.0, 0.0};

    int targetRate = 50;
    TargetType targetType = TargetType::UseTargetRate;

    bool loadHasFinished = false;

    // Skin update and Chart update are called in different threads. Add these vars to prevent data race
    unsigned chartCurrentBar = 0;
    double chartCurrentMetre = 0.0;

    void clearContextPlayForRetry();
    void clearContextPlay();
    void pushGraphPoints();

} PlayData;

}
#pragma once

class ChartObjectBase;
class RulesetBase;
class ReplayChart;
namespace lunaticvibes::data
{

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

enum class LampType
{
    NOPLAY,
    FAILED,
    ASSIST,
    EASY,
    NORMAL,
    HARD,
    EXHARD,
    FULLCOMBO,
    PERFECT,
    MAX,
};

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
    bool haveReplay = false;

    bool isBattle = false;  // Note: DB is NOT Battle
    PlayModifierBattleType battleType = PlayModifierBattleType::Off;

    std::array<PlayerPlayData, MAX_PLAYERS> player;

    GhostScorePosition ghostType = GhostScorePosition::Off;

    std::shared_ptr<ReplayChart> replay;
    std::shared_ptr<ReplayChart> replayMybest;
    std::shared_ptr<ReplayChart> replayNew;

    bool isCourse = false;
    int courseStage = 0;    // 0~8 (0: stage1, etc)
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

    bool canSaveScore = false;
    LampType saveLampType = LampType::NOPLAY;

    bool loadHasFinished = false;

} PlayData;

}
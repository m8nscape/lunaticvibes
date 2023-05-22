#pragma once
#include "skin.h"
#include "game/graphics/sprite_lane.h"
#include "game/graphics/sprite_video.h"
#include "game/graphics/sprite_bar_entry.h"
#include "game/graphics/sprite_imagetext.h"
#include "game/graphics/sprite_graph.h"
#include "game/input/input_mgr.h"

namespace lunaticvibes
{

namespace LR2SkinDef
{
    enum gr_
    {
        NOTFOUND = -1,
        STAGEFILE = 101,
        BACKBMP,
        BANNER,
        SKIN_THUMBNAIL = 106,
        BLACK_DOT = 111,
        WHITE_DOT
    };

    enum blend
    {
        TRANSCOLOR = 0,

        ALPHA = 1,
        ADD,
        SUBTRACT,
        MULTIPLY,

        XOR = 6,

        MULTIPLY_INVERTED_BACKGROUND = 9,
        ANTI_COLOR = 10,
        MULTIPLY_WITH_ALPHA = 11

    };

    enum class type
    {
        GENERAL,
        NUMBER,
        SLIDER,
        BARGRAPH,
        BUTTON,
        ONMOUSE,
        TEXT
    };

    const int DST_TRUE = 0;
    const int DST_FALSE = 999;

    const size_t GLOBAL_SPRITE_IDX_1PJUDGE = 0;
    const size_t GLOBAL_SPRITE_IDX_1PJUDGENUM = 6;
    const size_t GLOBAL_SPRITE_IDX_2PJUDGE = 12;
    const size_t GLOBAL_SPRITE_IDX_2PJUDGENUM = 18;
    const size_t GLOBAL_SPRITE_IDX_1PGAUGE = 24;
    const size_t GLOBAL_SPRITE_IDX_2PGAUGE = 25;
    const size_t GLOBAL_SPRITE_IDX_1PJUDGELINE = 26;
    const size_t GLOBAL_SPRITE_IDX_2PJUDGELINE = 27;

    // SPRITE_GLOBAL_MAX = 32

    enum class ParseRet
    {
        OK,
        PARAM_NOT_ENOUGH,
        PARAM_INVALID,
        DIV_NOT_ENOUGH,

        SRC_DEF_INVALID,
        SRC_DEF_WRONG_TYPE,
    };
    
}

using namespace LR2SkinDef;

struct setDst { int dst; bool set; };

class SkinLR2: public SkinBase
{
public:
    virtual int setExtendedProperty(std::string&& key, void* value);

protected:
    static bool customizeDst[100];  // temporary workaround

private:
    static std::map<std::string, Path> LR2SkinFontPathCache;
    std::map<std::string, std::shared_ptr<Texture>> prevSkinTextureNameMap;

    struct Customize
    {
        enum class _Type { OPT, FILE } type;

        // shared
        StringContent title;
        size_t value;

        // opt
        unsigned dst_op;
        std::vector<StringContent> label;

        // file
        StringContent filepath;
        std::vector<Path> pathList;
        size_t defIdx;
    };
    std::vector<Customize> customize;
    std::map<size_t, size_t> customizeRandom;

    struct LR2Font
    {
        int S = 1;
        int M = 0;
        std::map<int, size_t> T_id;
        std::vector<std::shared_ptr<Texture>> T_texture;
        CharMappingList R;
    };
    static std::map<Path, std::shared_ptr<LR2Font>> LR2FontCache;
    static std::map<std::string, std::shared_ptr<LR2Font>> prevSkinLR2FontNameMap;
    static std::map<std::string, std::shared_ptr<LR2Font>> LR2FontNameMap;

    Path filePath;
    int loadMode = 0;   // 0: FULL / 1: No Text / 2: Header Only

protected:
    size_t imageCount = 0;
    int timeStartInputTimeRank = 0;            // Result / Course Result Only
    int timeStartInputTimeUpdate = 0;        // Result / Course Result Only
    int timeFadeoutLength = 0;
    bool reloadBanner = false;                  // unused
    bool flipSide = false;                      // flip 1P/2P defs: note indices, timers (42-139, 143, 144)
    bool disableFlipResult = false;

public:
    int adjustPlaySkinX = 0;
    int adjustPlaySkinY = 0;
    int adjustPlaySkinW = 0;
    int adjustPlaySkinH = 0;
    bool adjustPlayJudgePositionLift = true;
    int adjustPlayJudgePosition1PX = 0;
    int adjustPlayJudgePosition1PY = 0;
    int adjustPlayJudgePosition2PX = 0;
    int adjustPlayJudgePosition2PY = 0;
    int adjustPlayNote1PX = 0;
    int adjustPlayNote1PY = 0;
    int adjustPlayNote1PW = 0;
    int adjustPlayNote1PH = 0;
    int adjustPlayNote2PX = 0;
    int adjustPlayNote2PY = 0;
    int adjustPlayNote2PW = 0;
    int adjustPlayNote2PH = 0;

public:
    SkinLR2() = delete;
    SkinLR2(Path p, int loadMode = 0);
    virtual ~SkinLR2();

protected:
    bool loadCSV(Path p);
    void postLoad();
    void findAndExtractDXA(const Path& path);

protected:
    static constexpr size_t BAR_ENTRY_SPRITE_COUNT = 32;
    std::bitset<BAR_ENTRY_SPRITE_COUNT> barSpriteAvailable{ false };
    std::array<std::shared_ptr<SpriteBarEntry>, BAR_ENTRY_SPRITE_COUNT> barSprites;
    unsigned barCenter = 0;
    unsigned barClickableFrom = 0;
    unsigned barClickableTo = 0;
    bool hasBarMotionInterpOrigin = false;
    std::array<Rect, BAR_ENTRY_SPRITE_COUNT> barMotionInterpOrigin;

protected:
    std::vector<std::pair<std::shared_ptr<SpriteLaneVertical>, std::shared_ptr<SpriteLaneVertical>>> laneSprites;  // { normal, auto }

protected:
    std::list<std::shared_ptr<SpriteBase>> spritesMoveWithLift1P;
    std::list<std::shared_ptr<SpriteBase>> spritesMoveWithLift2P;
    Rect judgeLineRect1P;
    Rect judgeLineRect2P;

protected:
    typedef std::shared_ptr<SpriteLine> psLine;

private:
    unsigned csvLineNumber = 0;          // line parsing index

    // #XXX_XXXXX with #
    StringContent parseKeyBuf;

    // SRC 0:index? 1:gr 2-5:xywh 6:divx 7:divy 8:cycle 9:timer
    // SRC_IMAGE 10:op1 11:op2 12:op3
    // SRC_BUTTON 10:type 11:click 12:panel 13:plusonly
    // SRC_SLIDER 10:muki 11:type 12:range 13:disable
    // SRC_ONMOUSE 10:panel 11-14:x2y2w2h2
    // SRC_NOWJUDGE 10:noshift
    // SRC_BARGRAPH 10:type 11:muki
    // SRC_NUMBER(NOWCOMBO) 10:num 11:align 12:keta
    // DST 0:index? 1:time 2-5:xywh 6:acc 7-10:argb 11:blend 12:filter 13:angle 14:center
    // DST 15:loop 16:timer 17:op1 18:op2 19:op3
    Tokens parseParamBuf;

    std::shared_ptr<Texture> textureBuf;
    std::shared_ptr<sVideo>   videoBuf;
    bool     useVideo = false;

    int parseHeader(const Tokens& raw);
    int parseBody(const Tokens& raw);

    enum class DefType
    {
        UNDEF,

        IMAGE = 1,
        NUMBER,
        SLIDER,
        BARGRAPH,
        BUTTON,
        ONMOUSE,
        TEXT,
        JUDGELINE,
        GROOVEGAUGE,
        NOWJUDGE_1P,
        NOWCOMBO_1P,
        NOWJUDGE_2P,
        NOWCOMBO_2P,
        BGA,
        MOUSECURSOR,
        GAUGECHART_1P,
        GAUGECHART_2P,
        SCORECHART,

        BAR_BODY,       // SRC only
        BAR_BODY_OFF,   // DST only
        BAR_BODY_ON,    // DST only
        BAR_FLASH,    
        BAR_LEVEL,    
        BAR_LAMP,     
        BAR_TITLE,    
        BAR_RANK,     
        BAR_RIVAL,    
        BAR_MY_LAMP,  
        BAR_RIVAL_LAMP,

        LINE,
        NOTE,
        MINE,
        LN_END,
        LN_BODY,
        LN_START,
        AUTO_NOTE,
        AUTO_MINE,
        AUTO_LN_END,
        AUTO_LN_BODY,
        AUTO_LN_START,
    };

    static inline const std::vector<std::pair<StringContent, DefType>> defTypeName =
    {
        {"IMAGE",          DefType::IMAGE},
        {"NUMBER",         DefType::NUMBER},
        {"SLIDER",         DefType::SLIDER},
        {"BARGRAPH",       DefType::BARGRAPH},
        {"BUTTON",         DefType::BUTTON},
        {"ONMOUSE",        DefType::ONMOUSE},
        {"TEXT",           DefType::TEXT},
        {"JUDGELINE",      DefType::JUDGELINE},
        {"GROOVEGAUGE",    DefType::GROOVEGAUGE},
        {"NOWJUDGE_1P",    DefType::NOWJUDGE_1P},
        {"NOWCOMBO_1P",    DefType::NOWCOMBO_1P},
        {"NOWJUDGE_2P",    DefType::NOWJUDGE_2P},
        {"NOWCOMBO_2P",    DefType::NOWCOMBO_2P},
        {"BGA",            DefType::BGA},
        {"MOUSECURSOR",    DefType::MOUSECURSOR},
        {"GAUGECHART_1P",  DefType::GAUGECHART_1P},
        {"GAUGECHART_2P",  DefType::GAUGECHART_2P},
        {"SCORECHART",     DefType::SCORECHART},
        {"BAR_BODY_OFF",   DefType::BAR_BODY_OFF},
        {"BAR_BODY_ON",    DefType::BAR_BODY_ON},
        {"BAR_BODY",       DefType::BAR_BODY},
        {"BAR_FLASH",      DefType::BAR_FLASH},
        {"BAR_LEVEL",      DefType::BAR_LEVEL},
        {"BAR_LAMP",       DefType::BAR_LAMP},
        {"BAR_TITLE",      DefType::BAR_TITLE},
        {"BAR_RANK",       DefType::BAR_RANK},
        {"BAR_RIVAL",      DefType::BAR_RIVAL},
        {"BAR_MY_LAMP",    DefType::BAR_MY_LAMP},
        {"BAR_RIVAL_LAMP", DefType::BAR_RIVAL_LAMP},
        {"LINE",           DefType::LINE},
        {"NOTE",           DefType::NOTE},
        {"MINE",           DefType::MINE},
        {"LN_END",         DefType::LN_END},
        {"LN_BODY",        DefType::LN_BODY},
        {"LN_START",       DefType::LN_START},
        {"AUTO_NOTE",      DefType::AUTO_NOTE},
        {"AUTO_MINE",      DefType::AUTO_MINE},
        {"AUTO_LN_END",    DefType::AUTO_LN_END},
        {"AUTO_LN_BODY",   DefType::AUTO_LN_BODY},
        {"AUTO_LN_START",  DefType::AUTO_LN_START},
    };

    Path getCustomizePath(StringContentView input);

    Tokens csvLineTokenize(const std::string& raw);

    int IMAGE();
    int INCLUDE();
    int LR2FONT();
    int SYSTEMFONT();
    int TIMEOPTION();
    int others();

    bool SRC();
    ParseRet SRC_IMAGE();
    ParseRet SRC_NUMBER();
    ParseRet SRC_SLIDER();
    ParseRet SRC_BARGRAPH();
    ParseRet SRC_BUTTON();
    ParseRet SRC_ONMOUSE();
    ParseRet SRC_MOUSECURSOR();
    ParseRet SRC_TEXT();

    ParseRet SRC_JUDGELINE();
    ParseRet SRC_GROOVEGAUGE();
    ParseRet SRC_NOTE(DefType type);
    ParseRet SRC_NOWJUDGE(size_t idx);
    ParseRet SRC_NOWCOMBO(size_t idx);
    ParseRet SRC_NOWJUDGE1();
    ParseRet SRC_NOWJUDGE2();
    ParseRet SRC_NOWCOMBO1();
    ParseRet SRC_NOWCOMBO2();
    ParseRet SRC_BGA();

    ParseRet SRC_GAUGECHART(int player);
    ParseRet SRC_SCORECHART();

    ParseRet SRC_BAR_BODY();
    ParseRet SRC_BAR_FLASH();
    ParseRet SRC_BAR_LEVEL();
    ParseRet SRC_BAR_LAMP();
    ParseRet SRC_BAR_TITLE();
    ParseRet SRC_BAR_RANK();
    ParseRet SRC_BAR_RIVAL();
    ParseRet SRC_BAR_RIVAL_MYLAMP();
    ParseRet SRC_BAR_RIVAL_RIVALLAMP();

    bool DST();
    ParseRet DST_NOTE();
    ParseRet DST_LINE();

    ParseRet DST_BAR_BODY();
    ParseRet DST_BAR_FLASH();
    ParseRet DST_BAR_LEVEL();
    ParseRet DST_BAR_LAMP();
    ParseRet DST_BAR_TITLE();
    ParseRet DST_BAR_RANK();
    ParseRet DST_BAR_RIVAL();
    ParseRet DST_BAR_RIVAL_MYLAMP();
    ParseRet DST_BAR_RIVAL_RIVALLAMP();

protected:
    int  bufJudge1PSlot;
    int  bufJudge2PSlot;
    std::array<bool, 6> noshiftJudge1P{ false };
    std::array<bool, 6> noshiftJudge2P{ false };
    std::array<int, 6> alignNowCombo1P{ 0 };
    std::array<int, 6> alignNowCombo2P{ 0 };

    void IF(const Tokens& t, std::istream&, eFileEncoding enc, bool ifUnsatisfied = false, bool skipOnly = false);

    //std::vector<SkinLR2> _csvIncluded;


private:
    // generated sprite references
    std::shared_ptr<SpriteBase> spriteLanecoverTop1P = nullptr;
    std::shared_ptr<SpriteBase> spriteLanecoverTop2P = nullptr;

////////////////////////////////////////////////////////////////////////////////

protected:
    struct element
    {
        std::shared_ptr<SpriteBase> ps;
        int op1;
        int op2;
        int op3;
        int op4;
        std::vector<int> opEx;
    };
    std::vector<element> drawQueue;
public:
    virtual void update();
    virtual void reset_bar_animation();
    virtual void start_bar_animation() override;
    virtual void draw() const;

    virtual size_t getCustomizeOptionCount() const;
    virtual CustomizeOption getCustomizeOptionInfo(size_t idx) const;

    virtual StringContent getName() const;
    virtual StringContent getMaker() const;
    virtual StringPath getFilePath() const;
};


// adapt helper
void updateDstOpt();
void setCustomDstOpt(unsigned base, size_t offset, bool val);
void clearCustomDstOpt();
bool getDstOpt(int d);
StringPath getCustomizeFileName(StringPathView skinPath);

}
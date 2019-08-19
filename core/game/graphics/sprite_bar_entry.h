#pragma once
#include <memory>
#include <array>
#include "sprite.h"

enum class BarLevelType
{
    UNDEF,      // WHITE
    BEGINNER,   // GREEN
    NORMAL,     // BLUE
    HYPER,      // YELLOW
    ANOTHER,    // RED
    INSANE,     // PURPLE
    IRRANK,     // GREY
    LEVEL_TYPE_COUNT
};

enum class BarLampType
{
    NOPLAY,
    FAILED,
    EASY,
    NORMAL,
    HARD,
    PATK_OR_GATK,
    FULLCOMBO,
    PERFECT,
    ASSIST_EASY,
    LAMP_TYPE_COUNT
};

enum class BarRankType
{
    NONE,
    F,
    E,
    D,
    C,
    B,
    A,
    AA,
    AAA,
    MAX,
    RANK_TYPE_COUNT
};

enum class BarRivalType
{
    WIN,
    LOSE,
    DRAW,
    NOPLAY,
    RIVAL_TYPE_COUNT
};

// Bar Entry sprite:
// select screen song bar. Have many sub-parts
class SpriteBarEntry : public vSprite
{
    typedef std::shared_ptr<SpriteAnimated> psAnimated;
    typedef std::shared_ptr<SpriteNumber>   psNumber;
    typedef std::shared_ptr<SpriteOption>   psOption;
    typedef std::shared_ptr<SpriteText>     psText;

protected:
    unsigned    index = 0;
    psAnimated  sBody{ nullptr };
    psText      sTitle{ nullptr };
    std::array<psNumber, static_cast<size_t>(BarLevelType::LEVEL_TYPE_COUNT)> sLevel{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT)> sLamp{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarRankType::RANK_TYPE_COUNT)> sRank{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarRivalType::RIVAL_TYPE_COUNT)> sRivalWinLose{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT)> sRivalLampSelf{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT)> sRivalLampRival{ nullptr };

public:
    SpriteBarEntry(unsigned idx) : vSprite(nullptr, SpriteTypes::BAR_ENTRY), index(idx) {}
    virtual ~SpriteBarEntry() = default;
    int setBody(pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
        eTimer timer = eTimer::SCENE_START, bool texVertSplit = false);
    int setLevel(BarLevelType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime, unsigned digits, NumberAlign align,
        eTimer timer = eTimer::SCENE_START, bool texVertSplit = false);
    int setLamp(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
        eTimer timer = eTimer::SCENE_START, bool texVertSplit = false);
    int setTitle(pFont f,
        TextAlign align = TEXT_ALIGN_LEFT, unsigned ptsize = 72, Color c = 0xffffffff);
    int setRank(BarRankType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
        eTimer timer = eTimer::SCENE_START, bool texVertSplit = false);
    int setRivalWinLose(BarRivalType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
        eTimer timer = eTimer::SCENE_START, bool texVertSplit = false);
    int setRivalLampSelf(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
        eTimer timer = eTimer::SCENE_START, bool texVertSplit = false);
    int setRivalLampRival(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
        eTimer timer = eTimer::SCENE_START, bool texVertSplit = false);

public:
    virtual bool update(timestamp time);
    virtual void setLoopTime(int t);
    virtual void setTimer(eTimer t);
    virtual void appendKeyFrame(RenderKeyFrame f);
    virtual void draw() const;

public:
    void appendKeyFrameBody(RenderKeyFrame f);
    void appendKeyFrameTitle(RenderKeyFrame f);
    void appendKeyFrameLevel(RenderKeyFrame f)
    {
        for (auto& p : sLevel) if (p) p->appendKeyFrame(f);
    }
    void appendKeyFrameLevel(BarLevelType type, RenderKeyFrame f);
    void appendKeyFrameLamp(BarLampType type, RenderKeyFrame f);
    void appendKeyFrameRank(BarRankType type, RenderKeyFrame f);
    void appendKeyFrameRivalWinLose(BarRivalType type, RenderKeyFrame f);
    void appendKeyFrameRivalLampSelf(BarLampType type, RenderKeyFrame f);
    void appendKeyFrameRivalLampRival(BarLampType type, RenderKeyFrame f);
};

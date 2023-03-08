#pragma once
#include <memory>
#include <array>
#include <list>
#include <utility>
#include <map>
#include "sprite.h"
#include "sprite_imagetext.h"

enum class BarType
{
    SONG,
    NEW_SONG,
    FOLDER,
    CUSTOM_FOLDER,
    NEW_SONG_FOLDER,
    RIVAL,
    SONG_RIVAL,
    COURSE_FOLDER,
    NEW_COURSE,
    COURSE,
    RANDOM_COURSE,
    TYPE_COUNT
};

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
    //PATK_OR_GATK, // LR2Skin actually uses 5 as Fullcombo index
    FULLCOMBO,
    PERFECT,
    EXHARD,
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

enum class BarPartsType
{
    BODY_OFF,
    BODY_ON,
    TITLE,
    FLASH,
    LEVEL,
    LAMP,
    RANK,
    RIVAL,
    MYLAMP,
    RIVALLAMP,
    PARTS_TYPE_COUNT
};

enum class BarTitleType
{
    NORMAL,
    NEW_SONG,
    TITLE_TYPE_COUNT
};

class SkinLR2;

// Bar Entry sprite:
// select screen song bar. Have many sub-parts
// The parent-child chain is a bit odd, it looks like this:
//      BODY -> [sprite] -> TITLE/LEVEL/LAMP/RANK/etc.
class SpriteBarEntry : public vSprite, public iSpriteMouse
{
    typedef std::shared_ptr<SpriteAnimated> psAnimated;
    typedef std::shared_ptr<SpriteNumber>   psNumber;
    typedef std::shared_ptr<SpriteOption>   psOption;
    typedef std::shared_ptr<SpriteText>     psText;

    friend class SkinLR2;

protected:
    size_t      index = 0;
    bool        available = false;
    std::array<psAnimated, static_cast<size_t>(BarType::TYPE_COUNT)> sBodyOff{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarType::TYPE_COUNT)> sBodyOn{ nullptr };
    std::array<psText, static_cast<size_t>(BarTitleType::TITLE_TYPE_COUNT)> sTitle{ nullptr };
    psAnimated  sFlash{ nullptr };
    std::array<psNumber, static_cast<size_t>(BarLevelType::LEVEL_TYPE_COUNT)> sLevel{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT)> sLamp{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarRankType::RANK_TYPE_COUNT)> sRank{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarRivalType::RIVAL_TYPE_COUNT)> sRivalWinLose{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT)> sRivalLampSelf{ nullptr };
    std::array<psAnimated, static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT)> sRivalLampRival{ nullptr };
    bool drawBodyOn = false;
    bool drawTitle = false;
    bool drawFlash = false;
    bool drawLevel = false;
    bool drawLamp = false;
    bool drawRank = false;
    bool drawRival = false;
    bool drawRivalLampSelf = false;
    bool drawRivalLampRival = false;
    size_t drawBodyType = 0;
    size_t drawTitleType = 0;
    size_t drawLevelType = 0;
    size_t drawLampType = 0;
    size_t drawRankType = 0;
    size_t drawRivalType = 0;
    size_t drawRivalLampSelfType = 0;
    size_t drawRivalLampRivalType = 0;

    std::list<BarPartsType> partsOrder;
    std::list<std::pair<BarPartsType, unsigned>> drawQueue;

public:
    SpriteBarEntry(size_t idx) : vSprite(SpriteTypes::BAR_ENTRY, -1), index(idx) {}
    virtual ~SpriteBarEntry() = default;
    int setBody(BarType type, const SpriteAnimated::SpriteAnimatedBuilder& builder);
    int setFlash(const SpriteAnimated::SpriteAnimatedBuilder& builder);
	int setLevel(BarLevelType type, const SpriteNumber::SpriteNumberBuilder& builder);
    int setLamp(BarLampType type, const SpriteAnimated::SpriteAnimatedBuilder& builder);
    int setTitle(BarTitleType type, const SpriteText::SpriteTextBuilder& builder);
    int setTitle(BarTitleType type, const SpriteImageText::SpriteImageTextBuilder& builder);
    int setRank(BarRankType type, const SpriteAnimated::SpriteAnimatedBuilder& builder);
    int setRivalWinLose(BarRivalType type, const SpriteAnimated::SpriteAnimatedBuilder& builder);
    int setRivalLampSelf(BarLampType type, const SpriteAnimated::SpriteAnimatedBuilder& builder);
    int setRivalLampRival(BarLampType type, const SpriteAnimated::SpriteAnimatedBuilder& builder);

    void pushPartsOrder(BarPartsType type);

public:
    virtual bool update(Time time);
    virtual void setLoopTime(int t);
    virtual void setTrigTimer(IndexTimer t);
    virtual void appendKeyFrame(const RenderKeyFrame& f) override;
    virtual void draw() const;

public:
    auto getSpriteBodyOff(BarType type) { return sBodyOff[static_cast<size_t>(type)]; }
    auto getSpriteBodyOn(BarType type) { return sBodyOn[static_cast<size_t>(type)]; }
    auto getSpriteFlash() { return sFlash; }
    auto getSpriteTitle(BarTitleType type) { return sTitle[static_cast<size_t>(type)]; }
    auto getSpriteLevel(BarLevelType type) { return sLevel[static_cast<size_t>(type)]; }
    auto getSpriteLamp(BarLampType type) { return sLamp[static_cast<size_t>(type)]; }
    auto getSpriteRank(BarRankType type) { return sRank[static_cast<size_t>(type)]; }
    auto getSpriteRivalWinLose(BarRivalType type) { return sRivalWinLose[static_cast<size_t>(type)]; }
    auto getSpriteRivalLampSelf(BarLampType type) { return sRivalLampSelf[static_cast<size_t>(type)]; }
    auto getSpriteRivalLampRival(BarLampType type) { return sRivalLampRival[static_cast<size_t>(type)]; }

    void setRectOffsetAnim(float x, float y);
    void setRectOffsetBarIndex(float x, float y);

public:
    void setAvailable(bool c) { available = c; }
    virtual void OnMouseMove(int x, int y) {}
    virtual bool OnClick(int x, int y);
    virtual bool OnDrag(int x, int y) { return false; }
};

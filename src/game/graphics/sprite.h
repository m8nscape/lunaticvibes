#pragma once
#include "beat.h"
#include "graphics.h"
#include "game/data/number.h"
#include "game/data/timer.h"
#include "game/data/text.h"

#include <vector>
#include <memory>

enum class SpriteTypes
{
    VIRTUAL,
    STATIC,
    SPLIT,
    ANIMATED,
    TEXT,

    NUMBER,

    NOTE,
};

struct RenderParams
{
    Rect rect;
    enum accTy {
        CONSTANT = 0,
        ACCEL,
        DECEL,
        DISCONTINOUS
    } accel;
    Color color;
    double angle;           // rotate angle / degree, 360 for one lap
};

struct RenderKeyFrame
{
    rTime time;
    RenderParams param;
};

typedef std::shared_ptr<Texture> pTexture;

////////////////////////////////////////////////////////////////////////////////
// Render interface
class vSprite
{
protected:
    SpriteTypes _type;
public:
    constexpr SpriteTypes type() { return _type; }
protected:
    pTexture _pTexture;
    BlendMode _blend = BlendMode::ALPHA;
    eTimer _timerInd;
    int _loopTo = -1;
protected:
    RenderParams _current;
    std::vector<RenderKeyFrame> _keyFrames;
public:
    vSprite(pTexture pTexture, SpriteTypes type = SpriteTypes::VIRTUAL, eTimer timer = eTimer::SCENE_START);
    virtual ~vSprite() = default;
public:
    RenderParams getCurrentRenderParams();
    bool update(rTime time);
    virtual void setBlendMode(BlendMode b);
    virtual void setLoopTime(int t);
    virtual void appendKeyFrame(RenderKeyFrame f);
    virtual void draw() const = 0;
    bool isKeyFrameEmpty() { return _keyFrames.empty(); }
};


////////////////////////////////////////////////////////////////////////////////
// Static sprite:
// No texture splits. Used for Frames / Backgrounds.
class SpriteStatic : public vSprite
{
protected:
    Rect _texRect;
public:
    SpriteStatic() = delete;
    SpriteStatic(pTexture texture);
    SpriteStatic(pTexture texture, const Rect& rect);
    virtual ~SpriteStatic() = default;
public:
    virtual void draw() const;
};

////////////////////////////////////////////////////////////////////////////////
// Texture-split sprite:
// Split the source texture for selectable sprites.

typedef std::size_t frameIdx;

class SpriteSplit : public vSprite
{
protected:
    std::vector<Rect> _texRect;
    unsigned _srows = 0, _scols = 0, _segments = 0;
    frameIdx _segmentIdx = 0;        
public:
    SpriteSplit() = delete;

    SpriteSplit(pTexture texture, eTimer timer = eTimer::SCENE_START, 
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);  // Copy texture, full area

    SpriteSplit(pTexture texture, const Rect& rect, eTimer timer = eTimer::SCENE_START, 
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);  // Copy texture, specified area

    virtual ~SpriteSplit() = default;
public:
    virtual void updateSplit(frameIdx i);
    virtual void draw() const;
};

////////////////////////////////////////////////////////////////////////////////
// Animated sprite:
// Split resolution for animation frames.
// Animation frame rect calculation is based on split rect
class SpriteAnimated : public SpriteSplit
{
    friend class SpriteLaneVertical;
protected:
    bool _aVert = false;
    Rect _aRect;
    unsigned _arows, _acols;
    unsigned _aframes;
    unsigned _period = -1;   // time for each frame lasts
    Rect _drawRect;
public:
    SpriteAnimated() = delete;

    SpriteAnimated(pTexture texture,
        unsigned subRows, unsigned subCols, unsigned frameTime, eTimer timer = eTimer::SCENE_START, bool subVerticalIndexing = false,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);

    SpriteAnimated(pTexture texture, const Rect& rect,
        unsigned subRows, unsigned subCols, unsigned frameTime, eTimer timer = eTimer::SCENE_START, bool subVerticalIndexing = false,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);

    virtual ~SpriteAnimated() = default;
public:
    void updateByTimer(rTime t);
    virtual void updateAnimation(rTime t);
    void updateAnimationByTimer(rTime t);
    //void updateSplitByTimer(rTime t);
    virtual void draw() const;
};

////////////////////////////////////////////////////////////////////////////////
// Text sprite:
// TTFFont contains Texture object
class SpriteText: public SpriteSplit
{
private:
    std::shared_ptr<TTFFont> _pFont;
    eText _textInd;
    std::string _currText;
    Color _color;
    bool _haveRect = false;

public:
    SpriteText() = delete;
    SpriteText(const char* ttf, eText textInd = eText::INVALID, unsigned ptsize = 72, Color c = 0xffffffff);
    SpriteText(const char* ttf, Rect rect, eText textInd = eText::INVALID, unsigned ptsize = 72, Color c = 0xffffffff);
    virtual ~SpriteText() = default;
public:
    void updateText();
    void setText(const char* text, const Color& c);
};

////////////////////////////////////////////////////////////////////////////////
// Number sprite:
// A number sprite instance contains [digit] SpriteAnimated objects, 
// in which Digit is decided by SpriteSplit part.
// texture split count should be 10, 11, 22 or 24. Each represents:
//  10: 0123456789
//  11: 0123456789O (empty 0)
//  22: 01234567890123456789+- (0~9: positive 10~19: negative)
//  24: 0123456789O0123456789O+- (0~10: positive 11~21: negative)

enum NumberSplits
{
    NUM_NORMAL = 10,
    NUM_BLANKZERO = 11,
    NUM_SYMBOL = 22,
    NUM_FULL = 24
};
const size_t NUM_BZERO          = 10;
const size_t NUM_SYMBOL_PLUS    = 20;
const size_t NUM_SYMBOL_MINUS   = 21;
const size_t NUM_FULL_BZERO_POS = 10;
const size_t NUM_FULL_BZERO_NEG = 21;
const size_t NUM_FULL_PLUS      = 22;
const size_t NUM_FULL_MINUS     = 23;

class SpriteNumber : public vSprite
{
private:
    eNumber _numInd;
    NumberSplits _numType;
    //std::vector<Rect> _drawRectDigit, _outRectDigit; // idx from low digit to high, e.g. [0] represents 1 digit, [1] represents 10 digit, etc.
    std::vector<SpriteAnimated> _sDigit;
    std::vector<unsigned>       _digit;

public:
    SpriteNumber() = delete;

    SpriteNumber(pTexture texture, unsigned maxDigits,
        unsigned numRows, unsigned numCols, unsigned frameTime, eNumber num = eNumber::ZERO, eTimer timer = eTimer::SCENE_START,
        bool numVerticalIndexing = false, unsigned arows = 1, unsigned acols = 1, bool animVerticalIndexing = false);

    SpriteNumber(pTexture texture, const Rect& rect, unsigned maxDigits,
        unsigned numRows, unsigned numCols, unsigned frameTime, eNumber num = eNumber::ZERO, eTimer timer = eTimer::SCENE_START,
        bool numVerticalIndexing = false, unsigned arows = 1, unsigned acols = 1, bool animVerticalIndexing = false);

    virtual ~SpriteNumber() = default;

public:
    void updateByTimer(rTime t);
    void updateNumber(int n);           // invoke updateSplit to change number
    void updateNumberByInd();
    void updateAnimationByTimer(rTime t);   // invoke updateAnimation to change animation frames
    virtual void setBlendMode(BlendMode b);
    virtual void setLoopTime(int t);
    virtual void appendKeyFrame(RenderKeyFrame f);
    virtual void draw() const;
};
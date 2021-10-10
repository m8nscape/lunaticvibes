#pragma once
#include "common/beat.h"
#include "graphics.h"
#include "game/data/data.h"
#include <vector>
#include <memory>
#include <functional>

enum class SpriteTypes
{
    VIRTUAL,
    GLOBAL,

    STATIC,
    SPLIT,
    ANIMATED,
    TEXT,
    IMAGE_TEXT,

    NUMBER,
	SLIDER,
	BARGRAPH,
	OPTION,
    BUTTON,
    ONMOUSE,

	LINE,
    NOTE_VERT,

	VIDEO,
	BMS_BGA,

    BAR_ENTRY,

    MOUSE_CURSOR,
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
	BlendMode blend;
	bool filter;
    double angle;           // rotate angle / degree, 360 for one lap
	Point center;			// rotate center point, in doubles
};

struct RenderKeyFrame
{
    long long time;
    RenderParams param;
};

typedef std::shared_ptr<Texture> pTexture;

////////////////////////////////////////////////////////////////////////////////
// Render interface
class vSprite: public std::enable_shared_from_this<vSprite>
{
	friend class SkinLR2;
protected:
    SpriteTypes _type;
    std::weak_ptr<vSprite> _parent;
    bool _haveParent = false;
public:
    constexpr SpriteTypes type() { return _type; }
protected:
	bool _draw = false;
    pTexture _pTexture;
    eTimer _triggerTimer = eTimer::SCENE_START;
    int _loopTo = -1;
    int _srcLine = -1;
protected:
    RenderParams _current;
    std::vector<RenderKeyFrame> _keyFrames;
public:
    vSprite(pTexture pTexture, SpriteTypes type = SpriteTypes::VIRTUAL);
	virtual ~vSprite() = default;
public:
    void setSrcLine(int i) { _srcLine = i; }
    void setParent(std::weak_ptr<vSprite> p) { _parent = p; _haveParent = true; }
    RenderParams getCurrentRenderParams();
    RenderParams& getCurrentRenderParamsRef();
    bool updateByKeyframes(const Time& time);
	virtual bool update(const Time& time);
    virtual void setLoopTime(int t);
	virtual void setTrigTimer(eTimer t);
    virtual void appendKeyFrame(RenderKeyFrame f);
    void appendInvisibleLeadingFrame(int x, int y);
    virtual void draw() const = 0;
    bool isKeyFrameEmpty() { return _keyFrames.empty(); }
    void clearKeyFrames() { _keyFrames.clear(); }
};


////////////////////////////////////////////////////////////////////////////////
// Sprite placeholder
inline const size_t SPRITE_GLOBAL_MAX = 32;
class SpriteGlobal: public vSprite
{
protected:
    size_t idx;
    std::shared_ptr<vSprite> pS{ nullptr };

public:
    SpriteGlobal(size_t idx) :vSprite(nullptr, SpriteTypes::GLOBAL), idx(idx) {}
    virtual ~SpriteGlobal() = default;

    size_t getIdx() {
        return idx;
    }

    void set(std::shared_ptr<vSprite> p) {
        pS = p;
    }

    virtual bool update(const Time& time) {
        vSprite::update(time);
        if (pS) return pS->update(time);
        return false;
    }
    virtual void setLoopTime(int t) {
        vSprite::setLoopTime(t);
        if (pS) pS->setLoopTime(t);
    }
    virtual void setTrigTimer(eTimer t) {
        vSprite::setTrigTimer(t);
        if (pS) pS->setTrigTimer(t);
    }
    virtual void appendKeyFrame(RenderKeyFrame f) {
        vSprite::appendKeyFrame(f);
        if (pS) pS->appendKeyFrame(f);
    }
    virtual void draw() const {
        if (pS) pS->draw();
    }
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

class SpriteSelection : public vSprite
{
protected:
    std::vector<Rect> _texRect;
    unsigned _srows = 0, _scols = 0, _segments = 0;
    frameIdx _selectionIdx = 0;        
public:
    SpriteSelection() = delete;

    SpriteSelection(pTexture texture,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);  // Copy texture, full area

    SpriteSelection(pTexture texture, const Rect& rect,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);  // Copy texture, specified area

    virtual ~SpriteSelection() = default;
public:
	virtual bool update(const Time& t);
    virtual void updateSelection(frameIdx i);
    virtual void draw() const;
};

////////////////////////////////////////////////////////////////////////////////
// Animated sprite:
// Split resolution for animation frames.
// Animation frame rect is now strictly pointed to Selection rect
class SpriteAnimated : public SpriteSelection
{
    friend class SpriteLaneVertical;
    friend class SpriteLaneVerticalLN;
    friend class SpriteSlider;
protected:
    //bool _aVert = false;
    //Rect _aRect;
    //unsigned _arows, _acols;
    unsigned _animFrames = 0;
	unsigned _selections = 0;
	eTimer _resetAnimTimer;
    unsigned _period = -1;   // time for each ANIM LOOP lasts
    frameIdx _currAnimFrame = 0;
    //Rect _drawRect;
public:
    SpriteAnimated() = delete;

    SpriteAnimated(pTexture texture,
        unsigned animFrames, unsigned frameTime, eTimer timer = eTimer::SCENE_START, 
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    SpriteAnimated(pTexture texture, const Rect& rect,
        unsigned animFrames, unsigned frameTime, eTimer timer = eTimer::SCENE_START, 
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    virtual ~SpriteAnimated() = default;
public:
    void updateByTimer(const Time& t);
    virtual void updateAnimation(const Time& t);
    void updateAnimationByTimer(const Time& t);
    //void updateSplitByTimer(timestamp t);
	virtual bool update(const Time& t);
    virtual void draw() const;
};

////////////////////////////////////////////////////////////////////////////////

typedef std::shared_ptr<TTFFont> pFont;

// Text sprite:
// TTFFont contains Texture object
class SpriteText: public SpriteStatic
{
private:
	pFont _pFont;
    Color _color;
    inline static std::mutex _updateMutex;

protected:
    eText _textInd;
    std::string _currText;
	TextAlign _align;
    //bool _haveRect = false;
	//Rect _frameRect;

public:
    SpriteText() = delete;
    SpriteText(pFont f, eText textInd = eText::INVALID, TextAlign align = TEXT_ALIGN_LEFT, unsigned ptsize = 72, Color c = 0xffffffff);
    //SpriteText(pFont f, Rect rect, eText textInd = eText::INVALID, TextAlign align = TEXT_ALIGN_LEFT, unsigned ptsize = 72, Color c = 0xffffffff);
    virtual ~SpriteText() = default;
public:
    virtual void updateTextRect();
	virtual bool update(const Time& t);
    virtual void draw() const;
    TextAlign getAlignType() const { return _align; }
private:
    void setText(std::string&& text, const Color& c);
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

enum NumberType
{
    NUM_TYPE_NORMAL = 10,
    NUM_TYPE_BLANKZERO = 11,
    //NUM_SYMBOL = 22,
    NUM_TYPE_FULL = 24
};
enum NumberAlign
{
	NUM_ALIGN_RIGHT,
	NUM_ALIGN_LEFT,
	NUM_ALIGN_CENTER
};

const unsigned NUM_BZERO          = 10;
const unsigned NUM_SYMBOL_PLUS    = 20;
const unsigned NUM_SYMBOL_MINUS   = 21;
const unsigned NUM_FULL_BZERO_POS = 10;
const unsigned NUM_FULL_BZERO_NEG = 22;
const unsigned NUM_FULL_PLUS      = 11;
const unsigned NUM_FULL_MINUS     = 23;

class SpriteNumber : public SpriteAnimated
{
	friend class SkinLR2;
protected:
    eNumber _numInd;
    unsigned _maxDigits = 0;
	unsigned _numDigits = 0;
    NumberType _numType = NUM_TYPE_NORMAL;
	NumberAlign _alignType = NUM_ALIGN_RIGHT;
    //std::vector<Rect> _drawRectDigit, _outRectDigit; // idx from low digit to high, e.g. [1] represents 1 digit, [2] represents 10 digit, etc.
    std::vector<Rect>           _rects;
    std::vector<int>       _digit;
    bool _inhibitZero = false;

public:
    SpriteNumber() = delete;

	SpriteNumber(pTexture texture, NumberAlign align, unsigned maxDigits,
        unsigned numRows, unsigned numCols, unsigned frameTime, eNumber num = eNumber::ZERO, eTimer animtimer = eTimer::SCENE_START,
		unsigned animFrames = 1, bool numVerticalIndexing = false);

	SpriteNumber(pTexture texture, const Rect& rect, NumberAlign align, unsigned maxDigits,
        unsigned numRows, unsigned numCols, unsigned frameTime, eNumber num = eNumber::ZERO, eTimer animtimer = eTimer::SCENE_START,
		unsigned animFrames = 1, bool numVerticalIndexing = false);

    virtual ~SpriteNumber() = default;

public:
    void updateNumber(int n);           // invoke updateSplit to change number
    void updateNumberByInd();
    void setInhibitZero(bool b) { _inhibitZero = b; }
    void updateNumberRect();
	virtual bool update(const Time& t);
    virtual void appendKeyFrame(RenderKeyFrame f);
    virtual void draw() const;
};

////////////////////////////////////////////////////////////////////////////////
// Slider sprite:
// Determine pos by inner value
enum class SliderDirection
{
    UP,
    RIGHT,
    DOWN,
    LEFT,
};
class SpriteSlider : public SpriteAnimated
{
private:
    eSlider _ind;
    percent _value = 100;
    SliderDirection _dir;
	int _range;

public:
    SpriteSlider() = delete;

    SpriteSlider(pTexture texture, SliderDirection dir, int range,
        unsigned animFrames, unsigned frameTime, eSlider s = eSlider::ZERO, eTimer timer = eTimer::SCENE_START,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);

    SpriteSlider(pTexture texture, const Rect& rect, SliderDirection dir, int range,
        unsigned animFrames, unsigned frameTime, eSlider s = eSlider::ZERO, eTimer timer = eTimer::SCENE_START,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);

    virtual ~SpriteSlider() = default;

public:
    void updateVal(percent v);
    void updateValByInd();
    void updatePos();
	virtual bool update(const Time& t);
};

////////////////////////////////////////////////////////////////////////////////
// Bargraph sprite:
// Determine pos by inner value
enum class BargraphDirection
{
    RIGHT,
    DOWN,
    LEFT,
    UP,
};

class SpriteBargraph : public SpriteAnimated
{
private:
    eBargraph _ind;
    dpercent _value = 1.0;
    BargraphDirection _dir;

public:
    SpriteBargraph() = delete;

    SpriteBargraph(pTexture texture, BargraphDirection dir,
        unsigned animFrames, unsigned frameTime, eBargraph s = eBargraph::ZERO, eTimer timer = eTimer::SCENE_START,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);

    SpriteBargraph(pTexture texture, const Rect& rect, BargraphDirection dir,
        unsigned animFrames, unsigned frameTime, eBargraph s = eBargraph::ZERO, eTimer timer = eTimer::SCENE_START,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false);

    virtual ~SpriteBargraph() = default;

public:
    void updateVal(dpercent v);
    void updateValByInd();
    void updateSize();
	virtual bool update(const Time& t);
};

////////////////////////////////////////////////////////////////////////////////
// Option sprite:

class SpriteOption : public SpriteAnimated
{
public:
	enum class opType
	{
		UNDEF, SWITCH, OPTION,
	};
protected:
	union {
		eOption op;
		eSwitch sw;
	} _ind;
	opType _opType = opType::UNDEF;
    unsigned _value = 0;

public:
    SpriteOption() = delete;

    SpriteOption(pTexture texture,
        unsigned animFrames, unsigned frameTime, eTimer timer = eTimer::SCENE_START,
		 unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    SpriteOption(pTexture texture, const Rect& rect,
        unsigned animFrames, unsigned frameTime, eTimer timer = eTimer::SCENE_START,
		 unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    virtual ~SpriteOption() = default;

public:
	bool setInd(opType type, unsigned ind);
    void updateVal(unsigned v);     // invoke SpriteSplit::updateSplit(v)
    void updateValByInd();
	virtual bool update(const Time& t);
};

////////////////////////////////////////////////////////////////////////////////
// Button sprite (clickable):

class SpriteButton : public SpriteOption
{
protected:
    std::function<void(int)> _callback;
    int _panel;
    int _plusonly_value;

public:
    SpriteButton() = delete;

    SpriteButton(pTexture texture,
        unsigned animFrames, unsigned frameTime, std::function<void(int)> cb, int panel, int plusonlyValue, eTimer timer = eTimer::SCENE_START,
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    SpriteButton(pTexture texture, const Rect& rect,
        unsigned animFrames, unsigned frameTime, std::function<void(int)> cb, int panel, int plusonlyValue, eTimer timer = eTimer::SCENE_START,
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    virtual ~SpriteButton() = default;

public:
    bool doIfInRange(int x, int y);
};


////////////////////////////////////////////////////////////////////////////////
// Gauge sprite:
// 50 grids, each represent 2%

class SpriteGaugeGrid : public SpriteAnimated
{
public:
	enum class FlashType
	{
		NONE,
		CLASSIC,	// LR2
		// TBD
	};

	enum class GaugeType
	{
		GROOVE,
		SURVIVAL,
		EX_SURVIVAL
	};

	enum TextureSelection
	{
		CLEAR_LIGHT,
		NORMAL_LIGHT,
		CLEAR_DARK,
		NORMAL_DARK,
		EXHARD_LIGHT,
		EXHARD_DARK,
	};

private:
	int _delta_x, _delta_y;
	unsigned _min, _max;
	eNumber _numInd;
    unsigned short _grids = 50;
    unsigned short _req = 40;
	unsigned short _val = _grids / 2;
	TextureSelection _texIdxLightFail, _texIdxDarkFail;
    TextureSelection _texIdxLightClear, _texIdxDarkClear;
    unsigned _lightRectFailIdxOffset, _lightRectClearIdxOffset, _darkRectFailIdxOffset, _darkRectClearIdxOffset;
    std::vector<bool> _lighting;
	FlashType _flashType = FlashType::CLASSIC;
	GaugeType _gaugeType = GaugeType::GROOVE;

public:
    SpriteGaugeGrid() = delete;

    SpriteGaugeGrid(pTexture texture,
        unsigned animFrames, unsigned frameTime, int dx, int dy, 
        unsigned min = 0, unsigned max = 100, unsigned grids = 50,
		eTimer timer = eTimer::SCENE_START, eNumber num = eNumber::PLAY_1P_GROOVEGAUGE,
		 unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    SpriteGaugeGrid(pTexture texture, const Rect& rect,
        unsigned animFrames, unsigned frameTime, int dx, int dy, 
        unsigned min = 0, unsigned max = 100, unsigned grids = 50,
		eTimer timer = eTimer::SCENE_START, eNumber num = eNumber::PLAY_1P_GROOVEGAUGE,
		 unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    virtual ~SpriteGaugeGrid() = default;

public:
	void setFlashType(FlashType t);
	void setGaugeType(GaugeType t);

public:
    void updateVal(unsigned v);
    void updateValByInd();
	virtual bool update(const Time& t);
	virtual void draw() const;
};

////////////////////////////////////////////////////////////////////////////////
// OnMouse
class SpriteOnMouse : public SpriteAnimated
{
protected:
    int panelIdx;
    Rect area;

public:
    SpriteOnMouse() = delete;

    SpriteOnMouse(pTexture texture, 
        unsigned animFrames, unsigned frameTime, int panel, const Rect& mouseArea, eTimer timer = eTimer::SCENE_START,
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    SpriteOnMouse(pTexture texture, const Rect& rect, 
        unsigned animFrames, unsigned frameTime, int panel, const Rect& mouseArea, eTimer timer = eTimer::SCENE_START,
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    virtual ~SpriteOnMouse() = default;

public:
    virtual bool update(const Time& t);
    void checkMouseArea(int x, int y);
};


////////////////////////////////////////////////////////////////////////////////
// Cursor
class SpriteCursor : public SpriteAnimated
{
protected:
    int panelIdx;
    Rect area;

public:
    SpriteCursor() = delete;

    SpriteCursor(pTexture texture, 
        unsigned animFrames, unsigned frameTime, eTimer timer = eTimer::SCENE_START,
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    SpriteCursor(pTexture texture, const Rect& rect,
        unsigned animFrames, unsigned frameTime, eTimer timer = eTimer::SCENE_START,
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false);

    virtual ~SpriteCursor() = default;

public:
    virtual bool update(const Time& t);
    void moveToPos(int x, int y);
};

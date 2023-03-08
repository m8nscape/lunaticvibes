#pragma once
#include "common/beat.h"
#include "graphics.h"
#include "game/runtime/state.h"
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

    GAUGE,
};

struct KeyFrameParams;
struct RenderParams
{
    RectF rect;
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

    RenderParams& operator=(const KeyFrameParams& rhs);
};

struct KeyFrameParams
{
    Rect rect;
    RenderParams::accTy accel;
    Color color;
    BlendMode blend;
    bool filter;
    double angle;           // rotate angle / degree, 360 for one lap
    Point center;			// rotate center point, in doubles
};

struct RenderKeyFrame
{
    long long time;
    KeyFrameParams param;
};

typedef std::shared_ptr<Texture> pTexture;

////////////////////////////////////////////////////////////////////////////////
// Render interface

class SpriteGlobal;
class SpriteBarEntry;
class vSprite: public std::enable_shared_from_this<vSprite>
{
    friend class vSkin;
	friend class SkinLR2;
    friend class SpriteGlobal;
    friend class SpriteBarEntry;
protected:
    SpriteTypes _type;
public:
    constexpr SpriteTypes type() { return _type; }
protected:
    bool _draw = false;     // modified by self
    bool _drawn = false; // modified by self::update()
    bool _hideInternal = false;     // modified internally
    bool _hideExternal = false;     // modified externally
    pTexture _pTexture;
    IndexTimer _triggerTimer = IndexTimer::SCENE_START;
    int _loopTo = -1;
    int _srcLine = -1;
protected:
    RenderParams _current;
    std::vector<RenderKeyFrame> _keyFrames;
public:
    struct SpriteBuilder
    {
        int srcLine = -1;

        pTexture texture = nullptr;
    };
public:
    vSprite(const SpriteBuilder& builder);
	virtual ~vSprite() = default;
protected:
    vSprite(const SpriteTypes type, int srcLine) : _type(type), _srcLine(srcLine) {}
public:
    void setSrcLine(int i) { _srcLine = i; }
    bool updateByKeyframes(const Time& time);
	virtual bool update(const Time& time);
    virtual void setLoopTime(int t);
	virtual void setTrigTimer(IndexTimer t);
    virtual void appendKeyFrame(const RenderKeyFrame& f);
    bool isDraw() const { return _draw; }
    virtual void draw() const = 0;
    void setHideInternal(bool hide) { _hideInternal = hide; }
    void setHideExternal(bool hide) { _hideExternal = hide; }
    bool isHidden() const { return _hideInternal || _hideExternal; }
    bool isKeyFrameEmpty() { return _keyFrames.empty(); }
    void clearKeyFrames() { _keyFrames.clear(); }
    virtual void adjustAfterUpdate(int x, int y, int w = 0, int h = 0);
};

class iSpriteMouse
{
public:
    virtual void OnMouseMove(int x, int y) = 0;
    virtual bool OnClick(int x, int y) = 0;
    virtual bool OnDrag(int x, int y) = 0;
};


////////////////////////////////////////////////////////////////////////////////
// Sprite placeholder
inline const size_t SPRITE_GLOBAL_MAX = 32;
inline std::array<std::shared_ptr<vSprite>, SPRITE_GLOBAL_MAX> gSprites{ nullptr };
class SpriteGlobal: public vSprite
{
protected:
    size_t idx;
    std::shared_ptr<vSprite> pSpriteRef{ nullptr };

public:
    SpriteGlobal(size_t idx, int srcLine = -1) : vSprite(SpriteTypes::GLOBAL, srcLine), idx(idx) {}
    virtual ~SpriteGlobal() = default;

    size_t getIdx() {
        return idx;
    }

    void setSpriteReference(std::shared_ptr<vSprite> p) {
        pSpriteRef = p;
        _srcLine = p->_srcLine;
    }

    virtual bool update(const Time& time) {
        vSprite::update(time);
        if (getIdx()) 
            setSpriteReference(gSprites[getIdx()]);
        if (pSpriteRef) 
            return pSpriteRef->update(time);
        return false;
    }
    virtual void setLoopTime(int t) {
        vSprite::setLoopTime(t);
        if (pSpriteRef) 
            pSpriteRef->setLoopTime(t);
    }
    virtual void setTrigTimer(IndexTimer t) {
        vSprite::setTrigTimer(t);
        if (pSpriteRef) 
            pSpriteRef->setTrigTimer(t);
    }
    virtual void appendKeyFrame(const RenderKeyFrame& f) override {
        vSprite::appendKeyFrame(f);
        if (pSpriteRef) 
            pSpriteRef->appendKeyFrame(f);
    }
    virtual void draw() const {
        if (pSpriteRef) 
            pSpriteRef->draw();
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
    struct SpriteStaticBuilder : SpriteBuilder
    {
        Rect textureRect = RECT_FULL;

        std::shared_ptr<SpriteStatic> build() const { return std::make_shared<SpriteStatic>(*this); }
    };
public:
    SpriteStatic() = delete;
    SpriteStatic(const SpriteStaticBuilder& builder);
    virtual ~SpriteStatic() = default;
protected:
    SpriteStatic(pTexture texture, const Rect& texRect, int srcLine = -1);
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
    struct SpriteSelectionBuilder: SpriteBuilder
    {
        Rect textureRect;
        unsigned textureSheetRows = 0;
        unsigned textureSheetCols = 0;
        bool textureSheetVerticalIndexing = false;

        std::shared_ptr<SpriteSelection> build() const { return std::make_shared<SpriteSelection>(*this); }
    };
public:
    SpriteSelection() = delete;
    SpriteSelection(const SpriteSelectionBuilder& builder);
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
    friend class SkinLR2;
protected:
    //bool _aVert = false;
    //Rect _aRect;
    //unsigned _arows, _acols;
    unsigned _animFrames = 0;
	unsigned _selections = 0;
	IndexTimer _resetAnimTimer;
    unsigned _period = -1;   // time for each ANIM LOOP lasts
    frameIdx _currAnimFrame = 0;
    //Rect _drawRect;
public:
    struct SpriteAnimatedBuilder: SpriteSelectionBuilder
    {
        unsigned animationFrameCount = 0;
        unsigned animationLengthPerLoop = 0;
        IndexTimer animationTimer = IndexTimer::SCENE_START;

        std::shared_ptr<SpriteAnimated> build() const { return std::make_shared<SpriteAnimated>(*this); }
    };
public:
    SpriteAnimated() = delete;
    SpriteAnimated(const SpriteAnimatedBuilder& builder);
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
class SpriteText: public vSprite, public iSpriteMouse
{
private:
	pFont _pFont;
    unsigned _height;
    Color _color;
    inline static std::mutex _updateMutex;

protected:
    IndexText _textInd;
    std::string _currText;
	TextAlign _align;
    //bool _haveRect = false;
	//Rect _frameRect;
    bool _editable = false;
    bool _editing = false;
    std::string _textBeforeEdit;
    std::string _textAfterEdit;

private:
    pTexture _pTexture;
    Rect _texRect;

public:
    struct SpriteTextBuilder : SpriteBuilder
    {
        pFont font = nullptr;
        IndexText textInd = IndexText::INVALID;
        TextAlign align = TEXT_ALIGN_LEFT;
        unsigned ptsize = 72;
        Color color = 0xffffffff;
        bool editable = false;

        std::shared_ptr<SpriteText> build() const { return std::make_shared<SpriteText>(*this); }
    };
public:
    SpriteText() = delete;
    SpriteText(const SpriteTextBuilder& builder);
    virtual ~SpriteText() = default;
public:
    virtual void updateText();
    virtual void updateTextRect();
	virtual bool update(const Time& t);
    virtual void draw() const;
    TextAlign getAlignType() const { return _align; }
private:
    void setInputBindingText(std::string&& text, const Color& c);

public:
    virtual void OnMouseMove(int x, int y) {}
    virtual bool OnClick(int x, int y);
    virtual bool OnDrag(int x, int y) { return false; }

    bool isEditing() const { return _editing; }
    void startEditing(bool clear);
    void stopEditing(bool modify);
    IndexText getInd() const { return _textInd; }
    virtual void EditUpdateText(const std::string& text);
    void setOutline(int width, const Color& c);
};

////////////////////////////////////////////////////////////////////////////////
// 
//  sprite:
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
    IndexNumber _numInd;
    unsigned _maxDigits = 0;
	unsigned _numDigits = 0;
    NumberType _numType = NUM_TYPE_NORMAL;
	NumberAlign _alignType = NUM_ALIGN_RIGHT;
    //std::vector<Rect> _drawRectDigit, _outRectDigit; // idx from low digit to high, e.g. [1] represents 1 digit, [2] represents 10 digit, etc.
    std::vector<RectF>           _rects;
    std::vector<int>       _digit;
    bool _inhibitZero = false;

public:
    struct SpriteNumberBuilder : SpriteAnimatedBuilder
    {
        NumberAlign align = NUM_ALIGN_RIGHT;
        unsigned maxDigits = 0;
        IndexNumber numInd = IndexNumber::ZERO;
        bool hideLeadingZeros = false;

        std::shared_ptr<SpriteNumber> build() const { return std::make_shared<SpriteNumber>(*this); }
    };
public:
    SpriteNumber() = delete;
    SpriteNumber(const SpriteNumberBuilder& builder);
    virtual ~SpriteNumber() = default;

public:
    void updateNumber(int n);           // invoke updateSplit to change number
    void updateNumberByInd();
    void setInhibitZero(bool b) { _inhibitZero = b; }
    void updateNumberRect();
	virtual bool update(const Time& t);
    virtual void appendKeyFrame(const RenderKeyFrame& f) override;
    virtual void draw() const;
    virtual void adjustAfterUpdate(int x, int y, int w = 0, int h = 0) override;
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
class SpriteSlider : public SpriteAnimated, public iSpriteMouse
{
private:
    IndexSlider _ind;
    double _value = 1.00;
    SliderDirection _dir;
	int _range = 0;
    int _posMin = 0;

    std::function<void(double)> _callback;

public:
    struct SpriteSliderBuilder : SpriteAnimatedBuilder
    {
        SliderDirection sliderDirection = SliderDirection::UP;
        IndexSlider sliderInd = IndexSlider::ZERO;
        int sliderRange = 0;
        std::function<void(double)> callOnChanged;

        std::shared_ptr<SpriteSlider> build() const { return std::make_shared<SpriteSlider>(*this); }
    };
public:
    SpriteSlider() = delete;
    SpriteSlider(const SpriteSliderBuilder& builder);
    virtual ~SpriteSlider() = default;

public:
    void updateVal(double v);
    void updateValByInd();
    void updatePos();
	virtual bool update(const Time& t);

    virtual void OnMouseMove(int x, int y) {}
    virtual bool OnClick(int x, int y);
    virtual bool OnDrag(int x, int y);
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
    IndexBargraph _ind;
    Ratio _value = 1.0;
    BargraphDirection _dir;

public:
    struct SpriteBargraphBuilder : SpriteAnimatedBuilder
    {
        BargraphDirection barDirection = BargraphDirection::RIGHT;
        IndexBargraph barInd = IndexBargraph::ZERO;

        std::shared_ptr<SpriteBargraph> build() const { return std::make_shared<SpriteBargraph>(*this); }
    };
public:
    SpriteBargraph() = delete;
    SpriteBargraph(const SpriteBargraphBuilder& builder);
    virtual ~SpriteBargraph() = default;

public:
    void updateVal(Ratio v);
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
		UNDEF, SWITCH, OPTION, FIXED,
	};
protected:
	union {
		IndexOption op;
		IndexSwitch sw;
        unsigned fix;
	} _ind;
	opType _opType = opType::UNDEF;
    unsigned _value = 0;

public:
    struct SpriteOptionBuilder : SpriteAnimatedBuilder
    {
        opType optionType;
        unsigned optionInd;

        std::shared_ptr<SpriteOption> build() const { return std::make_shared<SpriteOption>(*this); }
    };
public:
    SpriteOption(const SpriteOptionBuilder& builder);
    SpriteOption() = delete;
    virtual ~SpriteOption() = default;

public:
	bool setInd(opType type, unsigned ind);
    void updateVal(unsigned v);     // invoke SpriteSplit::updateSplit(v)
    void updateValByInd();
	virtual bool update(const Time& t);
};

////////////////////////////////////////////////////////////////////////////////
// Button sprite (clickable):

class SpriteButton : public SpriteOption, public iSpriteMouse
{
protected:
    std::function<void(int)> _callback;
    int _panel;
    int _plusonly_value;

public:
    struct SpriteButtonBuilder : SpriteOptionBuilder
    {
        int clickableOnPanel = -1;
        int plusonlyDelta = 0;
        std::function<void(int)> callOnClick;

        std::shared_ptr<SpriteButton> build() const { return std::make_shared<SpriteButton>(*this); }
    };
public:
    SpriteButton() = delete;
    SpriteButton(const SpriteButtonBuilder& builder);
    virtual ~SpriteButton() = default;

public:
    virtual void OnMouseMove(int x, int y) {}
    virtual bool OnClick(int x, int y);
    virtual bool OnDrag(int x, int y) { return false; }
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
		EX_SURVIVAL,
        ASSIST_EASY,
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
	int _diff_x, _diff_y;
	unsigned _min, _max;
	IndexNumber _numInd;
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
    struct SpriteGaugeGridBuilder : SpriteAnimatedBuilder
    {
        int dx = 0;
        int dy = 0;
        int gaugeMin = 0;
        int gaugeMax = 100;
        int gridCount = 50;
        IndexNumber numInd = IndexNumber::PLAY_1P_GROOVEGAUGE;

        std::shared_ptr<SpriteGaugeGrid> build() const { return std::make_shared<SpriteGaugeGrid>(*this); }
    };
public:
    SpriteGaugeGrid() = delete;
    SpriteGaugeGrid(const SpriteGaugeGridBuilder& builder);
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
class SpriteOnMouse : public SpriteAnimated, public iSpriteMouse
{
protected:
    int panelIdx;
    Rect area;

public:
    struct SpriteOnMouseBuilder : SpriteAnimatedBuilder
    {
        int visibleOnPanel = -1;
        Rect mouseArea;

        std::shared_ptr<SpriteOnMouse> build() const { return std::make_shared<SpriteOnMouse>(*this); }
    };
public:
    SpriteOnMouse() = delete;
    SpriteOnMouse(const SpriteOnMouseBuilder& builder);
    virtual ~SpriteOnMouse() = default;

public:
    virtual bool update(const Time& t);

    virtual void OnMouseMove(int x, int y);
    virtual bool OnClick(int x, int y) { return false; }
    virtual bool OnDrag(int x, int y) { return false; }
};


////////////////////////////////////////////////////////////////////////////////
// Cursor
class SpriteCursor : public SpriteAnimated, public iSpriteMouse
{
public:
    struct SpriteCursorBuilder : SpriteAnimatedBuilder
    {
        std::shared_ptr<SpriteCursor> build() const { return std::make_shared<SpriteCursor>(*this); }
    };
public:
    SpriteCursor() = delete;
    SpriteCursor(const SpriteCursorBuilder& builder);
    virtual ~SpriteCursor() = default;

public:
    virtual bool update(const Time& t);

    virtual void OnMouseMove(int x, int y);
    virtual bool OnClick(int x, int y) { return false; }
    virtual bool OnDrag(int x, int y) { return false; }
};

#pragma once
#include "common/beat.h"
#include "graphics.h"
#include "game/runtime/state.h"

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

struct MotionKeyFrameParams
{
    Rect rect;
    enum accelType {
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

struct RenderParams
{
    using accelType = MotionKeyFrameParams::accelType;

    RectF rect;             // use float for rendering
    accelType accel;
    Color color;
    BlendMode blend;
    bool filter;
    double angle;           // rotate angle / degree, 360 for one lap
    Point center;			// rotate center point, in doubles

    RenderParams& operator=(const MotionKeyFrameParams& rhs);
};

struct MotionKeyFrame
{
    long long time;
    MotionKeyFrameParams param;
};

////////////////////////////////////////////////////////////////////////////////
// Render interface

class SpriteGlobal;
class SpriteBarEntry;
class SpriteBase: public std::enable_shared_from_this<SpriteBase>
{
    friend class SkinBase;
	friend class SkinLR2;
    friend class SpriteGlobal;
    friend class SpriteBarEntry;
protected:
    SpriteTypes _type;
public:
    constexpr SpriteTypes type() { return _type; }
protected:
    std::shared_ptr<Texture> pTexture;
    int srcLine = -1;
    bool _draw = false;     // modified by self
    bool drawn = false; // modified by self::update()
    bool hideInternal = false;     // modified internally
    bool hideExternal = false;     // modified externally
protected:
    RenderParams _current;
    std::vector<MotionKeyFrame> motionKeyFrames;
    int motionLoopTo = -1;
    IndexTimer motionStartTimer = IndexTimer::SCENE_START;

public:
    struct SpriteBuilder
    {
        int srcLine = -1;

        std::shared_ptr<Texture> texture = nullptr;
    };
public:
    SpriteBase(const SpriteBuilder& builder);
	virtual ~SpriteBase() = default;
protected:
    SpriteBase(const SpriteTypes type, int srcLine) : _type(type), srcLine(srcLine) {}

public:
    void setSrcLine(int i) { srcLine = i; }

    virtual void appendMotionKeyFrame(const MotionKeyFrame& f);
    virtual void setMotionLoopTo(int time);
	virtual void setMotionStartTimer(IndexTimer t);
    bool isMotionKeyFramesEmpty() const { return motionKeyFrames.empty(); }
    void clearMotionKeyFrames() { motionKeyFrames.clear(); }

    bool updateMotion(const Time& time);
    virtual bool update(const Time& time);
    virtual void adjustAfterUpdate(int x, int y, int w = 0, int h = 0);

protected:
    void setHideInternal(bool hide) { hideInternal = hide; }
public:
    void setHideExternal(bool hide) { hideExternal = hide; }

public:
    bool isHidden() const { return hideInternal || hideExternal; }
    bool isDraw() const { return _draw; }
    virtual void draw() const = 0;
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
inline std::array<std::shared_ptr<SpriteBase>, SPRITE_GLOBAL_MAX> gSprites{ nullptr };
class SpriteGlobal: public SpriteBase
{
protected:
    size_t globalSpriteIndex;
    std::shared_ptr<SpriteBase> pSpriteRef{ nullptr };

public:
    SpriteGlobal(size_t globalSpriteIndex, int srcLine = -1) : SpriteBase(SpriteTypes::GLOBAL, srcLine), globalSpriteIndex(globalSpriteIndex) {}
    virtual ~SpriteGlobal() = default;

    size_t getMyGlobalSpriteIndex() {
        return globalSpriteIndex;
    }

    void setSpriteReference(std::shared_ptr<SpriteBase> p) {
        pSpriteRef = p;
        srcLine = p->srcLine;
    }

    virtual bool update(const Time& time) {
        SpriteBase::update(time);
        if (getMyGlobalSpriteIndex()) 
            setSpriteReference(gSprites[getMyGlobalSpriteIndex()]);
        if (pSpriteRef) 
            return pSpriteRef->update(time);
        return false;
    }
    virtual void setMotionLoopTo(int t) {
        SpriteBase::setMotionLoopTo(t);
        if (pSpriteRef) 
            pSpriteRef->setMotionLoopTo(t);
    }
    virtual void setMotionStartTimer(IndexTimer t) {
        SpriteBase::setMotionStartTimer(t);
        if (pSpriteRef) 
            pSpriteRef->setMotionStartTimer(t);
    }
    virtual void appendMotionKeyFrame(const MotionKeyFrame& f) override {
        SpriteBase::appendMotionKeyFrame(f);
        if (pSpriteRef) 
            pSpriteRef->appendMotionKeyFrame(f);
    }
    virtual void draw() const {
        if (pSpriteRef) 
            pSpriteRef->draw();
    }
};


////////////////////////////////////////////////////////////////////////////////
// Static sprite:
// No texture splits. Used for Frames / Backgrounds.
class SpriteStatic : public SpriteBase
{
protected:
    Rect textureRect;
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
    SpriteStatic(std::shared_ptr<Texture> texture, const Rect& texRect, int srcLine = -1);
public:
    virtual void draw() const;

};

////////////////////////////////////////////////////////////////////////////////
// Texture-split sprite:
// Split the source texture for selectable sprites.

class SpriteSelection : public SpriteBase
{
protected:
    std::vector<Rect> textureRects;
    unsigned textureSheetRows = 0;
    unsigned textureSheetCols = 0;
    size_t selectionIndex = 0;
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
    virtual void updateSelection(size_t i);
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
    unsigned animationFrames = 0;
	unsigned selections = 0;
    size_t animationFrameIndex = 0;
	IndexTimer animationStartTimer;
    unsigned animationDurationPerLoop = -1;
public:
    struct SpriteAnimatedBuilder: SpriteSelectionBuilder
    {
        unsigned animationFrameCount = 0;
        unsigned animationDurationPerLoop = 0;
        IndexTimer animationTimer = IndexTimer::SCENE_START;

        std::shared_ptr<SpriteAnimated> build() const { return std::make_shared<SpriteAnimated>(*this); }
    };
public:
    SpriteAnimated() = delete;
    SpriteAnimated(const SpriteAnimatedBuilder& builder);
    virtual ~SpriteAnimated() = default;
public:
	virtual bool update(const Time& t);
    virtual void updateAnimation(const Time& t);
    virtual void draw() const;
};

////////////////////////////////////////////////////////////////////////////////

// Text sprite:
// TTFFont contains Texture object
class SpriteText: public SpriteBase, public iSpriteMouse
{
private:
    std::shared_ptr<TTFFont> pFont;
    unsigned textHeight;
    Color textColor;

protected:
    IndexText textInd;
	TextAlign align;
    bool editable = false;

protected:
    std::string text;
private:
    Rect textureRect;

protected:
    bool editing = false;
    std::string textBeforeEdit;
    std::string textAfterEdit;

public:
    struct SpriteTextBuilder : SpriteBuilder
    {
        std::shared_ptr<TTFFont> font = nullptr;
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
private:
    void updateTextTexture(std::string&& text, const Color& c);

public:
	virtual bool update(const Time& t);
    virtual void draw() const;

public:
    virtual void OnMouseMove(int x, int y) {}
    virtual bool OnClick(int x, int y);
    virtual bool OnDrag(int x, int y) { return false; }

    bool isEditing() const { return editing; }
    void startEditing(bool clear);
    void stopEditing(bool modify);
    IndexText getInd() const { return textInd; }
    virtual void updateTextWhileEditing(const std::string& text);
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
    IndexNumber numInd;
    unsigned maxDigits = 0;
    NumberType numberType = NUM_TYPE_NORMAL;
	NumberAlign alignType = NUM_ALIGN_RIGHT;
    bool hideLeadingZeros = false;

	unsigned digitCount = 0;
    std::vector<int> digitNumber;
    std::vector<RectF> digitOutRect;

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
    void updateNumberRect();
	virtual bool update(const Time& t);
    virtual void appendMotionKeyFrame(const MotionKeyFrame& f) override;
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
    IndexSlider sliderInd;
    double value = 1.00;
    SliderDirection dir;
	int valueRange = 0;
    int minValuePos = 0;

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
    IndexBargraph barInd;
    Ratio value = 1.0;
    BargraphDirection dir;

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
	} ind;
	opType indType = opType::UNDEF;
    unsigned value = 0;

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
    std::function<void(int)> callOnClick;
    int clickableOnPanel;
    int plusonlyDelta;

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

	enum GridType
	{
		CLEAR_LIGHT,
		NORMAL_LIGHT,
		CLEAR_DARK,
		NORMAL_DARK,
		EXHARD_LIGHT,
		EXHARD_DARK,
	};

private:
	int gridSizeW, gridSizeH;
	unsigned minValue, maxValue;
	IndexNumber numInd;
    unsigned short totalGrids = 50;
    unsigned short failGrids = 40;
	unsigned short value = totalGrids / 2;
	GridType lightFailGridType, darkFailGridType;
    GridType lightClearGridType, darkClearGridType;
    unsigned lightFailRectIdxOffset, lightClearRectIdxOffset, darkFailRectIdxOffset, darkClearRectIdxOffset;
    std::vector<bool> flashing;
	FlashType flashType = FlashType::CLASSIC;
	GaugeType gaugeType = GaugeType::GROOVE;

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
    int visibleOnPanel;
    Rect mouseArea;

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

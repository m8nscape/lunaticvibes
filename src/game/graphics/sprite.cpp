#include "common/pch.h"
#include "sprite.h"
#include "game/data/data_types.h"

namespace lunaticvibes
{

constexpr double grad(int dst, int src, double t)
{
    return (src == dst) ? src : (dst * t + src * (1.0 - t));
}

RenderParams& RenderParams::operator=(const MotionKeyFrameParams& rhs)
{
    rect.x = (float)rhs.rect.x;
    rect.y = (float)rhs.rect.y;
    rect.w = (float)rhs.rect.w;
    rect.h = (float)rhs.rect.h;

    accel = rhs.accel;
    color = rhs.color;
    blend = rhs.blend;
    filter = rhs.filter;
    angle = rhs.angle;
    center = rhs.center;

    return *this;
}

////////////////////////////////////////////////////////////////////////////////
// virtual base class functions
SpriteBase::SpriteBase(const SpriteBuilder& builder) :
    srcLine(builder.srcLine), 
    pTexture(builder.texture),
    _type(SpriteTypes::VIRTUAL), 
    _current({0, MotionKeyFrameParams::CONSTANT, 0x00000000, BlendMode::NONE, false, 0}),
    lr2SpriteData(builder.lr2SpriteData) {}

bool SpriteBase::updateMotion(const Time& rawTime)
{
    // Check if object is valid
	// Note that nullptr texture shall pass
    if (pTexture != nullptr && !pTexture->isLoaded())
        return false;

    // Check if frames are valid
    size_t frameCount = motionKeyFrames.size();
    if (frameCount < 1)
        return false;

	Time time;

	// Check if timer is 140
    if (motionStartTimer == "play.beat")
    {
        time = getTimerValue(motionStartTimer);
    }
    else 
    {
        // Check if timer is valid
        long long t = getTimerValue(motionStartTimer);
        if (t == TIMER_NEVER)
            return false;
        time = rawTime - Time(t, false);
        if (time < 0)
            return false;
    }

    // Check if the sprite is not visible yet
    if (!drawn && motionKeyFrames[0].time > 0 && time.norm() < motionKeyFrames[0].time)
        return false;

    // Check if import time is valid
    if (time.norm() < 0)
        return false;

    // Check if loop target is valid
    Time endTime = Time(motionKeyFrames[frameCount - 1].time, false);
    if (motionLoopTo < 0 && time > endTime)
        return false;
    if (motionLoopTo > motionKeyFrames[frameCount - 1].time)
        time = motionKeyFrames[frameCount - 1].time;


    // crop time into valid section
    if (time > endTime)
    {
		if (endTime != motionLoopTo)
			time = Time((time - motionLoopTo).norm() % (endTime - motionLoopTo).norm() + motionLoopTo, false);
        else
            time = motionLoopTo;
    }

    // Check if specific time
    if (time == motionKeyFrames[frameCount - 1].time)
    {
        // exactly last frame
        _current = motionKeyFrames[frameCount - 1].param;
    }
    else if (frameCount == 1 || time.norm() <= motionKeyFrames[0].time)
    {
        // exactly first frame
        _current = motionKeyFrames[0].param;
    }
    else
    {
        // get keyFrame section (iterators)
        decltype(motionKeyFrames.begin()) keyFrameCurr, keyFrameNext;
        for (auto it = motionKeyFrames.begin(); it != motionKeyFrames.end(); ++it)
        {
            if (it->time <= time.norm()) 
                keyFrameCurr = it;
            else 
                break;
        }
        keyFrameNext = keyFrameCurr;
        if (keyFrameCurr + 1 != motionKeyFrames.end()) ++keyFrameNext;

        // Check if section period is 0
        auto keyFrameLength = keyFrameNext->time - keyFrameCurr->time;
        if (keyFrameLength == 0)
        {
            _current = keyFrameCurr->param;
        }
        else
        {
            // normalize time
            double prog = 1.0 * (time.norm() - keyFrameCurr->time) / keyFrameLength;
            switch (keyFrameCurr->param.accel)
            {
            case MotionKeyFrameParams::CONSTANT:
                break;
            case MotionKeyFrameParams::ACCEL:
                //prog = -std::cos(prog * 1.57079632679) + 1.0;
                prog = prog * prog * prog;
                break;
            case MotionKeyFrameParams::DECEL:
                //prog = std::sin(prog * 1.57079632679);
                prog = 1.0 - ((1.0 - prog) * (1.0 - prog) * (1.0 - prog));
                break;
            case MotionKeyFrameParams::DISCONTINOUS:
                prog = 0.0;
            }

            // calculate parameters
            _current.rect.x = (float)grad(keyFrameNext->param.rect.x, keyFrameCurr->param.rect.x, prog);
            _current.rect.y = (float)grad(keyFrameNext->param.rect.y, keyFrameCurr->param.rect.y, prog);
            _current.rect.w = (float)grad(keyFrameNext->param.rect.w, keyFrameCurr->param.rect.w, prog);
            _current.rect.h = (float)grad(keyFrameNext->param.rect.h, keyFrameCurr->param.rect.h, prog);
            //_current.rcGrid  = keyFrameNext->param.rcGrid  * prog + keyFrameCurr->param.rcGrid  * (1.0 - prog);
            _current.color.r = (Uint8)grad(keyFrameNext->param.color.r, keyFrameCurr->param.color.r, prog);
            _current.color.g = (Uint8)grad(keyFrameNext->param.color.g, keyFrameCurr->param.color.g, prog);
            _current.color.b = (Uint8)grad(keyFrameNext->param.color.b, keyFrameCurr->param.color.b, prog);
            _current.color.a = (Uint8)grad(keyFrameNext->param.color.a, keyFrameCurr->param.color.a, prog);
            //_current.color = keyFrameNext->param.color * prog + keyFrameNext->param.color * (1.0 - prog);
            _current.angle = grad(static_cast<int>(std::round(keyFrameNext->param.angle)), static_cast<int>(std::round(keyFrameCurr->param.angle)), prog);
            _current.center = keyFrameCurr->param.center;
            //LOG_DEBUG << "[Skin] Time: " << time << 
            //    " @ " << _current.rcGrid.x << "," << _current.rcGrid.y << " " << _current.rcGrid.w << "x" << _current.rcGrid.h;
            //LOG_DEBUG<<"[Skin] keyFrameCurr: " << keyFrameCurr->param.rcGrid.x << "," << keyFrameCurr->param.rcGrid.y << " " << keyFrameCurr->param.rcGrid.w << "x" << keyFrameCurr->param.rcGrid.h;
            //LOG_DEBUG<<"[Skin] keyFrameNext: " << keyFrameNext->param.rcGrid.x << "," << keyFrameNext->param.rcGrid.y << " " << keyFrameNext->param.rcGrid.w << "x" << keyFrameNext->param.rcGrid.h;
            _current.blend = keyFrameCurr->param.blend;
            _current.filter = keyFrameCurr->param.filter;
        }
    }

    return true;
}

bool SpriteBase::update(const Time& t)
{
    updateCallback();

    _draw = updateMotion(t);

    if (_draw) drawn = true;
    return _draw;
}

void SpriteBase::setMotionStartTimer(const std::string& t)
{
	motionStartTimer = t;
}

void SpriteBase::appendMotionKeyFrame(const MotionKeyFrame& f)
{
    motionKeyFrames.push_back(f);
}

void SpriteBase::setMotionLoopTo(int time)
{
    motionLoopTo = time;
}

void SpriteBase::adjustAfterUpdate(int x, int y, int w, int h)
{
    _current.rect.x += x - w;
    _current.rect.y += y - h;
    _current.rect.w += w * 2;
    _current.rect.h += h * 2;
}


////////////////////////////////////////////////////////////////////////////////
// Static

SpriteStatic::SpriteStatic(const SpriteStaticBuilder& builder): SpriteBase(builder)
{
    _type = SpriteTypes::STATIC;

    if (pTexture && builder.textureRect == RECT_FULL)
        textureRect = pTexture->getRect();
    else
        textureRect = builder.textureRect;
}

SpriteStatic::SpriteStatic(std::shared_ptr<Texture> texture, const Rect& texRect, int srcLine) : SpriteBase(SpriteTypes::STATIC, srcLine)
{
    pTexture = texture;
    if (pTexture && texRect == RECT_FULL)
        textureRect = pTexture->getRect();
    else
        textureRect = texRect;
}

void SpriteStatic::draw() const
{
    if (isHidden()) return;

    if (_draw && pTexture->isLoaded())
        pTexture->draw(textureRect, _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
}

////////////////////////////////////////////////////////////////////////////////
// Split

SpriteSelection::SpriteSelection(const SpriteSelectionBuilder& builder) : SpriteBase(builder)
{
    _type = SpriteTypes::SPLIT;
    textureSheetRows = builder.textureSheetRows;
    textureSheetCols = builder.textureSheetCols;

    if (textureSheetRows == 0 || textureSheetCols == 0)
    {
        textureRects.resize(0);
        return;
    }

    Rect rcGrid;
    if (pTexture && builder.textureRect == RECT_FULL)
        rcGrid = pTexture->getRect();
    else
        rcGrid = builder.textureRect;
    rcGrid.w /= textureSheetCols;
    rcGrid.h /= textureSheetRows;

    if (!builder.textureSheetVerticalIndexing)
    {
        // Horizontal first
        for (unsigned r = 0; r < textureSheetRows; ++r)
            for (unsigned c = 0; c < textureSheetCols; ++c)
            {
                textureRects.emplace_back(
                    rcGrid.x + rcGrid.w * c,
                    rcGrid.y + rcGrid.h * r,
                    rcGrid.w,
                    rcGrid.h
                );
            }
    }
    else
    {
        // Vertical first
        for (unsigned c = 0; c < textureSheetCols; ++c)
            for (unsigned r = 0; r < textureSheetRows; ++r)
            {
                textureRects.emplace_back(
                    rcGrid.x + rcGrid.w * c,
                    rcGrid.y + rcGrid.h * r,
                    rcGrid.w,
                    rcGrid.h
                );
            }
    }
}

void SpriteSelection::draw() const
{
    if (isHidden()) return;

    if (_draw && pTexture->isLoaded())
        pTexture->draw(textureRects[selectionIndex], _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
}

void SpriteSelection::updateSelection(size_t frame)
{
    selectionIndex = frame < textureRects.size() ? frame : textureRects.size() - 1;
}

bool SpriteSelection::update(const Time& t)
{
	return SpriteBase::update(t);
}

////////////////////////////////////////////////////////////////////////////////
// Animated

SpriteAnimated::SpriteAnimated(const SpriteAnimatedBuilder& builder) : SpriteSelection(builder)
{
    _type = SpriteTypes::ANIMATED;
    animationFrames = builder.animationFrameCount;
    animationStartTimer = builder.animationTimer;

    if (textureRects.empty() || animationFrames == 0)
        return;

    selections = textureSheetRows * textureSheetCols / animationFrames;
    animationDurationPerLoop = builder.animationDurationPerLoop;
}

bool SpriteAnimated::update(const Time& t)
{
	if (SpriteSelection::update(t))
	{
        Time time;
        if (animationStartTimer == "play.beat")
        {
            time = getTimerValue(animationStartTimer);
            updateAnimation(time);
        }
        else
        {
            // Check if timer is valid
            long long t1 = getTimerValue(animationStartTimer);
            if (t1 > 0 && t1 != TIMER_NEVER)
            {
                time = t - Time(t1, false);
                updateAnimation(time);
            }
        }

		return true;
	}
	return false;
}

void SpriteAnimated::updateAnimation(const Time& time)
{
    if (textureRects.empty()) return;
    if (animationDurationPerLoop == -1) return;

    if (double timeEachFrame = double(animationDurationPerLoop) / animationFrames; timeEachFrame >= 1.0)
    {
        auto animFrameTime = (time.norm() >= 0) ? (time.norm() % animationDurationPerLoop) : 0;
        animationFrameIndex = static_cast<size_t>(std::floor(animFrameTime / timeEachFrame));
    }
}

void SpriteAnimated::draw() const
{
    if (isHidden()) return;

    if (_draw && animationFrameIndex < textureRects.size() && pTexture != nullptr && pTexture->isLoaded())
    {
        pTexture->draw(textureRects[selectionIndex * animationFrames + animationFrameIndex], _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Text

SpriteText::SpriteText(const SpriteTextBuilder& builder) : SpriteBase(builder)
{
    _type = SpriteTypes::TEXT;
    pFont = builder.font;
    align = builder.align;
    textHeight = builder.ptsize * 3 / 2;
    textColor = builder.color;
    editable = builder.editable;
    afterEditCallback = builder.afterEditCallback;

    updateCallback = [=]() { text = builder.textCallback(); };
}

void SpriteText::updateText()
{
    assert(IsMainThread());
    if (!_draw) return;

    updateTextTexture(_current.color);
    updateTextRect();

}

void SpriteText::updateTextTexture(const Color& c)
{
    if (!pFont || !pFont->isLoaded())
        return;

    if (pTexture != nullptr && textDisplaying == text && textColor == c)
        return;

    if (text.empty() || c.a == 0)
    {
        pTexture = nullptr;
        _draw = false;
        return;
    }

    textDisplaying = text;
    textColor = c;

    pTexture = pFont->TextUTF8(text.c_str(), c);
    if (pTexture)
    {
        textureRect = pTexture->getRect();
        _draw = true;
    }
    else
    {
        _draw = false;
    }
}

void SpriteText::updateTextRect()
{
	// fitting
	Rect textRect = textureRect;
	double sizeFactor = (double)_current.rect.h / textRect.h;
	int text_w = static_cast<int>(std::round(textRect.w * sizeFactor));
	switch (align)
	{
    case TextAlignType::Left:
		break;
    case TextAlignType::Center:
		_current.rect.x -= text_w / 2;
		break;
    case TextAlignType::Right:
		_current.rect.x -= text_w;
		break;
	}
	_current.rect.w = text_w;
}

bool SpriteText::OnClick(int x, int y)
{
    if (!editable) return false;
    if (_current.rect.x <= x && x < _current.rect.x + _current.rect.w &&
        _current.rect.y <= y && y < _current.rect.y + _current.rect.h)
    {
        return true;
    }
    return false;
}

void SpriteText::startEditing(bool clear)
{
    using namespace std::placeholders;
    if (!isEditing())
    {
        editing = true;
        textBeforeEdit = text;
        textAfterEdit = (clear ? "" : text);
        startTextInput(_current.rect, textAfterEdit, std::bind(&SpriteText::updateTextWhileEditing, this, _1));
    }
}

void SpriteText::stopEditing(bool modify)
{
    if (isEditing())
    {
        stopTextInput();
        editing = false;
        text = textAfterEdit;
        afterEditCallback(text);
        textDisplaying = text;
        updateText();
    }
}

void SpriteText::updateTextWhileEditing(const std::string& text)
{
    textAfterEdit = text;
    textDisplaying = text + "|";
    updateText();
}

void SpriteText::draw() const
{
    if (isHidden()) return;

    if (_draw && pTexture && pTexture->isLoaded())
    {
        pTexture->draw(textureRect, _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
    }
}

void SpriteText::setOutline(int width, const Color& c)
{
    pushMainThreadTask([&] { pFont->setOutline(width, c); });
}

////////////////////////////////////////////////////////////////////////////////
// Number

SpriteNumber::SpriteNumber(const SpriteNumberBuilder& builder): SpriteAnimated(builder)
{
    _type = SpriteTypes::NUMBER;
    alignType = builder.align;
    maxDigits = builder.maxDigits;
    hideLeadingZeros = builder.hideLeadingZeros;

    // invalid num type guard
    //numberType = NumberType(numRows * numCols);
    if (animationFrames != 0)
        numberType = NumberType(textureSheetRows * textureSheetCols / animationFrames);
    else
        numberType = NumberType(0);

    digitNumber.resize(maxDigits);
    digitOutRect.resize(maxDigits);

    switch (numberType)
    {
    case NUM_TYPE_NORMAL:
    case NUM_TYPE_BLANKZERO:
        break;
        //case NUM_SYMBOL:
    case NUM_TYPE_FULL:
        break;
    default: return;
    }

    updateCallback = [=]() { updateNumber(builder.numberCallback()); };
}

bool SpriteNumber::update(const Time& t)
{
    if (maxDigits == 0) return false;
    if (numberType == 0) return false;

	if (SpriteAnimated::update(t))
	{
        updateNumberRect();
		return true;
	}
	return false;
}

void SpriteNumber::updateNumber(int n)
{
    if (n == INT_MIN) n = 0;

    bool positive = n >= 0;
	int zeroIdx = -1;
    unsigned digits = static_cast<unsigned>(digitNumber.size());
	switch (numberType)
	{
	case NUM_TYPE_NORMAL:    zeroIdx = -1; break;
	case NUM_TYPE_BLANKZERO: zeroIdx = NUM_BZERO; break;
    case NUM_TYPE_FULL:      zeroIdx = positive ? NUM_FULL_BZERO_POS : NUM_FULL_BZERO_NEG; digits--; break;
	}

    // reset by zeroIdx to prevent unexpected glitches
    for (auto& d : digitNumber) d = zeroIdx;

	if (n == 0)
	{
        digitNumber[0] = 0;
		digitCount = 1;
	}
	else
	{
		digitCount = 0;
		int abs_n = std::abs(n);
		for (unsigned i = 0; abs_n && i < digits; ++i)
		{
			++digitCount;
			unsigned digit = abs_n % 10;
			abs_n /= 10;
            if (numberType == NUM_TYPE_FULL && !positive) digit += 12;
			digitNumber[i] = digit;
		}
	}

    // symbol
    switch (numberType)
    {
    /*
    case NUM_TYPE_NORMAL:
        for (unsigned i = digitCount; i < maxDigits; ++i)
        {
            _digit[i] = 0;
        }
        digitCount = maxDigits;
        break;
    */
	/*
    case NUM_SYMBOL:
    {
        _digit[_sDigit.size() - 1] = positive ? NUM_SYMBOL_PLUS : NUM_SYMBOL_MINUS;
        break;
    }
	*/
    case NUM_TYPE_FULL:
    {
        switch (alignType)
        {

        case NUM_ALIGN_RIGHT:
            if (!hideLeadingZeros || digitCount == maxDigits)
                digitCount = maxDigits - 1;
            digitNumber[digitCount++] = positive ? NUM_FULL_PLUS : NUM_FULL_MINUS;
            break;

        case NUM_ALIGN_LEFT:
        case NUM_ALIGN_CENTER: 
            if (digitCount == maxDigits)
                --digitCount;
            digitNumber[digitCount++] = positive ? NUM_FULL_PLUS : NUM_FULL_MINUS;
            break;
        }
        break;
    }
    }
}

void SpriteNumber::updateNumberRect()
{
    switch (alignType)
    {
    case NUM_ALIGN_RIGHT:
    {
        RectF offset{ _current.rect.w * (maxDigits - 1),0,0,0 };
        for (size_t i = 0; i < maxDigits; ++i)
        {
            digitOutRect[i] = _current.rect + offset;
            offset.x -= _current.rect.w;
        }
        break;
    }

    case NUM_ALIGN_LEFT:
    {
        RectF offset{ _current.rect.w * (digitCount - 1),0,0,0 };
        for (size_t i = 0; i < digitCount; ++i)
        {
            digitOutRect[i] = _current.rect + offset;
            offset.x -= _current.rect.w;
        }
        break;
    }

    case NUM_ALIGN_CENTER:
    {
        RectF offset{ 0,0,0,0 };
        if (hideLeadingZeros)
            offset.x = int(std::floor(_current.rect.w * 0.5 * (digitCount - 1)));
        else
            offset.x = int(std::floor(_current.rect.w * (0.5 * (maxDigits + digitCount) - 1)));
        for (size_t i = 0; i < digitCount; ++i)
        {
            digitOutRect[i] = _current.rect + offset;
            offset.x -= _current.rect.w;
        }
        break;
    }
    }
}

void SpriteNumber::appendMotionKeyFrame(const MotionKeyFrame& f)
{
    motionKeyFrames.push_back(f);
}

void SpriteNumber::draw() const
{
    if (isHidden()) return;

    if (pTexture->isLoaded() && _draw)
    {
        //for (size_t i = 0; i < _outRectDigit.size(); ++i)
        //    pTexture->draw(_drawRectDigit[i], _outRectDigit[i], _current.angle);

        size_t max = 0;
        switch (alignType)
        {
        case NUM_ALIGN_RIGHT:
            max = hideLeadingZeros ? digitCount : maxDigits;
            break;
        case NUM_ALIGN_LEFT:
        case NUM_ALIGN_CENTER:
            max = digitCount;
            break;
        default:
            break;
        }
        for (size_t i = 0; i < max; ++i)
        {
            if (digitNumber[i] != -1)
                pTexture->draw(textureRects[animationFrameIndex * selections + digitNumber[i]], digitOutRect[i],
                    _current.color, _current.blend, _current.filter, _current.angle);
        }
    }
}

void SpriteNumber::adjustAfterUpdate(int x, int y, int w, int h)
{
    SpriteBase::adjustAfterUpdate(x, y, w, h);
    for (auto& d : digitOutRect)
    {
        d.x += x - w;
        d.y += y - h;
        d.w += w * 2;
        d.h += h * 2;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Slider

SpriteSlider::SpriteSlider(const SpriteSliderBuilder& builder) : SpriteAnimated(builder)
{
    _type = SpriteTypes::SLIDER;
    dir = builder.sliderDirection;
    valueRange = builder.sliderRange;
    dragCallback = builder.dragCallback;

    updateCallback = [=]() { updateVal(builder.sliderCallback()); };
}

void SpriteSlider::updateVal(double v)
{
	value = v;
}

void SpriteSlider::updatePos()
{
	int pos_diff = static_cast<int>(std::floor((valueRange-1) * value));
	switch (dir)
	{
	case SliderDirection::DOWN:
        minValuePos = _current.rect.y + _current.rect.h / 2;
		_current.rect.y += pos_diff;
		break;
	case SliderDirection::UP:
        minValuePos = _current.rect.y + _current.rect.h / 2;
		_current.rect.y -= pos_diff;
		break;
	case SliderDirection::RIGHT:
        minValuePos = _current.rect.x + _current.rect.w / 2;
		_current.rect.x += pos_diff;
		break;
	case SliderDirection::LEFT:
        minValuePos = _current.rect.x + _current.rect.w / 2;
		_current.rect.x -= pos_diff;
		break;
	}
}

bool SpriteSlider::update(const Time& t)
{
	if (SpriteAnimated::update(t))
	{
		updatePos();
		return true;
	}
	return false;
}

bool SpriteSlider::OnClick(int x, int y)
{
    if (!_draw) return false;
    if (valueRange == 0) return false;

    bool inRange = false;
    double val = 0.0;
    switch (dir)
    {
    case SliderDirection::UP:
        if (_current.rect.x <= x && x < _current.rect.x + _current.rect.w &&
            minValuePos - valueRange <= y && y <= minValuePos)
        {
            inRange = true;
        }
        break;
    case SliderDirection::DOWN:
        if (_current.rect.x <= x && x < _current.rect.x + _current.rect.w &&
            minValuePos <= y && y <= minValuePos + valueRange)
        {
            inRange = true;
        }
        break;
    case SliderDirection::LEFT:
        if (_current.rect.y <= y && y < _current.rect.y + _current.rect.h &&
            minValuePos - valueRange <= x && x <= minValuePos)
        {
            inRange = true;
        }
        break;
    case SliderDirection::RIGHT:
        if (_current.rect.y <= y && y < _current.rect.y + _current.rect.h &&
            minValuePos <= x && x <= minValuePos + valueRange)
        {
            inRange = true;
        }
        break;
    }
    if (inRange)
    {
        OnDrag(x, y);
        return true;
    }
    return false;
}

bool SpriteSlider::OnDrag(int x, int y)
{
    if (!_draw) return false;
    if (valueRange == 0) return false;

    double val = 0.0;
    switch (dir)
    {
    case SliderDirection::UP:
        val = double(minValuePos - y) / valueRange;
        break;
    case SliderDirection::DOWN:
        val = double(y - minValuePos) / valueRange;
        break;
    case SliderDirection::LEFT:
        val = double(minValuePos - x) / valueRange;
        break;
    case SliderDirection::RIGHT:
        val = double(x - minValuePos) / valueRange;
        break;
    }
    val = std::clamp(val, 0.0, 1.0);
    if (std::abs(value - val) > 0.000001)  // this should be enough
    {
        value = val;
        dragCallback(value);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Bargraph

SpriteBargraph::SpriteBargraph(const SpriteBargraphBuilder& builder) : SpriteAnimated(builder)
{
    _type = SpriteTypes::BARGRAPH;
    dir = builder.barDirection;

    updateCallback = [=] { updateVal(builder.bargraphCallback()); };
}

void SpriteBargraph::updateVal(Ratio v)
{
	value = v;
}

#pragma warning(push)
#pragma warning(disable: 4244)
void SpriteBargraph::updateSize()
{
	int tmp;
	switch (dir)
	{
	case BargraphDirection::DOWN:
		_current.rect.h *= value;
		break;
	case BargraphDirection::UP:
		tmp = _current.rect.h;
		_current.rect.h *= value;
		_current.rect.y += tmp - _current.rect.h;
		break;
	case BargraphDirection::RIGHT:
		_current.rect.w *= value;
		break;
	case BargraphDirection::LEFT:
		tmp = _current.rect.w;
		_current.rect.w *= value;
		_current.rect.x += tmp - _current.rect.w;
		break;
	}
}
#pragma warning(pop)

bool SpriteBargraph::update(const Time& t)
{
	if (SpriteAnimated::update(t))
	{
		updateSize();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Option

SpriteOption::SpriteOption(const SpriteOptionBuilder& builder): SpriteAnimated(builder)
{
    _type = SpriteTypes::OPTION;

    updateCallback = [=]() { updateVal(builder.optionCallback()); };
}

void SpriteOption::updateVal(unsigned v)
{
	value = v;
	updateSelection(v);
}

bool SpriteOption::update(const Time& t)
{
	if (SpriteSelection::update(t))
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Button

SpriteButton::SpriteButton(const SpriteButtonBuilder& builder) : SpriteOption(builder)
{
    _type = SpriteTypes::BUTTON;
    clickableOnPanel = builder.clickableOnPanel;
    plusonlyDelta = builder.plusonlyDelta;
    callOnClick = builder.callOnClick;
}

bool SpriteButton::OnClick(int x, int y)
{
    if (!_draw) return false;

    if (clickableOnPanel < -1 || clickableOnPanel > 9) return false;

    switch (clickableOnPanel)
    {
    case 1: if (!SelectData.panel[0]) return false; break;
    case 2: if (!SelectData.panel[1]) return false; break;
    case 3: if (!SelectData.panel[2]) return false; break;
    case 4: if (!SelectData.panel[3]) return false; break;
    case 5: if (!SelectData.panel[4]) return false; break;
    case 6: if (!SelectData.panel[5]) return false; break;
    case 7: if (!SelectData.panel[6]) return false; break;
    case 8: if (!SelectData.panel[7]) return false; break;
    case 9: if (!SelectData.panel[8]) return false; break;
    }

    if (plusonlyDelta == 0)
    {
        int w_opt = _current.rect.w / 2;
        if (y >= _current.rect.y && y < _current.rect.y + _current.rect.h)
        {
            if (x >= _current.rect.x && x < _current.rect.x + w_opt)
            {
                // minus
                callOnClick(-1);
                return true;
            }
            else if (x >= _current.rect.x + w_opt && x < _current.rect.x + _current.rect.w)
            {
                // plus
                callOnClick(1);
                return true;
            }
        }
    }
    else
    {
        if (x >= _current.rect.x &&
            x < _current.rect.x + _current.rect.w &&
            y >= _current.rect.y &&
            y < _current.rect.y + _current.rect.h)
        {
            // plusonly
            callOnClick(plusonlyDelta);
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Gauge grids

SpriteGaugeGrid::SpriteGaugeGrid(const SpriteGaugeGridBuilder& builder) : SpriteAnimated(builder)
{
    _type = SpriteTypes::GAUGE;
    gridSizeW = builder.dx;
    gridSizeH = builder.dy;
    totalGrids = builder.gridCount;
    minValue = builder.gaugeMin;
    maxValue = builder.gaugeMax;

    flashing.resize(totalGrids, false);
    setGaugeType(GaugeType::GROOVE);

    updateCallback = [=]() { updateVal((unsigned)builder.numberCallback()); };
}

void SpriteGaugeGrid::setFlashType(SpriteGaugeGrid::FlashType t)
{
	flashType = t;
}

void SpriteGaugeGrid::setGaugeType(SpriteGaugeGrid::GaugeType ty)
{
	gaugeType = ty;
	switch (gaugeType)
	{
    case GaugeType::ASSIST_EASY:
        lightFailGridType = NORMAL_LIGHT; darkFailGridType = NORMAL_DARK;
        lightClearGridType = CLEAR_LIGHT; darkClearGridType = CLEAR_DARK;
        failGrids = (unsigned short)std::floor(0.6 * totalGrids); break;

    case GaugeType::GROOVE: 
        lightFailGridType = NORMAL_LIGHT; darkFailGridType = NORMAL_DARK; 
        lightClearGridType = CLEAR_LIGHT; darkClearGridType = CLEAR_DARK;
        failGrids = (unsigned short)std::floor(0.8 * totalGrids); break;

    case GaugeType::SURVIVAL:  
        lightFailGridType = CLEAR_LIGHT; darkFailGridType = CLEAR_DARK;
        lightClearGridType = CLEAR_LIGHT; darkClearGridType = CLEAR_DARK;
        failGrids = 1; break;

    case GaugeType::EX_SURVIVAL: 
        if (textureRects.size() > EXHARD_LIGHT)
        {
            lightFailGridType = EXHARD_LIGHT; darkFailGridType = EXHARD_DARK;
            lightClearGridType = EXHARD_LIGHT; darkClearGridType = EXHARD_DARK;
        }
        else
        {
            lightFailGridType = CLEAR_LIGHT; darkFailGridType = CLEAR_DARK;
            lightClearGridType = CLEAR_LIGHT; darkClearGridType = CLEAR_DARK;
        }
        failGrids = 1; break;
	default: break;
	}

    Time t(1);

    // set FailRect
    updateSelection(lightFailGridType);
    SpriteAnimated::update(t);
    lightFailRectIdxOffset = unsigned(selectionIndex * animationFrames);
    updateSelection(darkFailGridType);
    SpriteAnimated::update(t);
    darkFailRectIdxOffset = unsigned(selectionIndex * animationFrames);

    // set ClearRect
    updateSelection(lightClearGridType);
    SpriteAnimated::update(t);
    lightClearRectIdxOffset = unsigned(selectionIndex * animationFrames);
    updateSelection(darkClearGridType);
    SpriteAnimated::update(t);
    darkClearRectIdxOffset = unsigned(selectionIndex * animationFrames);
}

void SpriteGaugeGrid::updateVal(unsigned v)
{
	value = totalGrids * (v - minValue) / (maxValue - minValue);
}

bool SpriteGaugeGrid::update(const Time& t)
{
	if (SpriteAnimated::update(t))
	{
		switch (flashType)
		{
		case FlashType::NONE:
			for (unsigned i = 0; i < value; ++i)
				flashing[i] = true;
			for (unsigned i = value; i < totalGrids; ++i)
				flashing[i] = false;
			break;

		case FlashType::CLASSIC:
			for (unsigned i = 0; i < value; ++i)
				flashing[i] = true;
			if (value - 3 >= 0 && value - 3 < totalGrids && t.norm() / 17 % 2) flashing[value - 3] = false; // -3 grid: 17ms, per 2 units (1 0 1 0)
			if (value - 2 >= 0 && value - 2 < totalGrids && t.norm() / 17 % 4) flashing[value - 2] = false; // -2 grid: 17ms, per 4 units (1 0 0 0)
			for (unsigned i = value; i < totalGrids; ++i)
				flashing[i] = false;
			break;
			
		default: break;
		}
		return true;
	}
	return false;
}

void SpriteGaugeGrid::draw() const
{
    if (isHidden()) return;

    if (_draw && pTexture != nullptr && pTexture->isLoaded())
    {
		RectF r = _current.rect;
        unsigned grid_val = unsigned(failGrids - 1);
        for (unsigned i = 0; i < grid_val; ++i)
        {
            flashing[i] ?
                pTexture->draw(textureRects[lightFailRectIdxOffset + animationFrameIndex], r, _current.color, _current.blend, _current.filter, _current.angle) :
                pTexture->draw(textureRects[darkFailRectIdxOffset + animationFrameIndex], r, _current.color, _current.blend, _current.filter, _current.angle);
            r.x += gridSizeW;
            r.y += gridSizeH;
        }
        for (unsigned i = grid_val; i < totalGrids; ++i)
        {
            flashing[i] ?
                pTexture->draw(textureRects[lightClearRectIdxOffset + animationFrameIndex], r, _current.color, _current.blend, _current.filter, _current.angle) :
                pTexture->draw(textureRects[darkClearRectIdxOffset + animationFrameIndex], r, _current.color, _current.blend, _current.filter, _current.angle);
            r.x += gridSizeW;
            r.y += gridSizeH;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// OnMouse

SpriteOnMouse::SpriteOnMouse(const SpriteOnMouseBuilder& builder) : SpriteAnimated(builder)
{
    _type = SpriteTypes::ONMOUSE;
    visibleOnPanel = builder.visibleOnPanel;
    mouseArea = builder.mouseArea;
}

bool SpriteOnMouse::update(const Time& t)
{
    switch (visibleOnPanel)
    {
    case 1: if (!SelectData.panel[0]) { return false; } break;
    case 2: if (!SelectData.panel[1]) { return false; } break;
    case 3: if (!SelectData.panel[2]) { return false; } break;
    case 4: if (!SelectData.panel[3]) { return false; } break;
    case 5: if (!SelectData.panel[4]) { return false; } break;
    case 6: if (!SelectData.panel[5]) { return false; } break;
    case 7: if (!SelectData.panel[6]) { return false; } break;
    case 8: if (!SelectData.panel[7]) { return false; } break;
    case 9: if (!SelectData.panel[8]) { return false; } break;
    }

    if (SpriteSelection::update(t))
    {
        return true;
    }
    return false;
}

void SpriteOnMouse::OnMouseMove(int x, int y)
{
    if (_draw)
    {
        int bx = _current.rect.x + mouseArea.x;
        int by = _current.rect.y + mouseArea.y;
        if (x < bx || x > bx + mouseArea.w) _draw = false;
        if (y < by || y > by + mouseArea.h) _draw = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Cursor

SpriteCursor::SpriteCursor(const SpriteCursorBuilder& builder) : SpriteAnimated(builder)
{
    _type = SpriteTypes::MOUSE_CURSOR;
}

bool SpriteCursor::update(const Time& t)
{
    if (SpriteSelection::update(t))
    {
        return true;
    }
    return false;
}

void SpriteCursor::OnMouseMove(int x, int y)
{
    if (_draw)
    {
        _current.rect.x += x;
        _current.rect.y += y;
    }
}

}
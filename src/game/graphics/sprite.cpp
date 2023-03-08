#include "sprite.h"
#include <algorithm>


constexpr double grad(int dst, int src, double t)
{
    return (src == dst) ? src : (dst * t + src * (1.0 - t));
}

bool checkPanel(int panelIdx)
{
    switch (panelIdx)
    {
    case -1:
    {
        bool panel =
            State::get(IndexSwitch::SELECT_PANEL1) ||
            State::get(IndexSwitch::SELECT_PANEL2) ||
            State::get(IndexSwitch::SELECT_PANEL3) ||
            State::get(IndexSwitch::SELECT_PANEL4) ||
            State::get(IndexSwitch::SELECT_PANEL5) ||
            State::get(IndexSwitch::SELECT_PANEL6) ||
            State::get(IndexSwitch::SELECT_PANEL7) ||
            State::get(IndexSwitch::SELECT_PANEL8) ||
            State::get(IndexSwitch::SELECT_PANEL9);
        return !panel;
    }
    case 0: return true;
    case 1: return State::get(IndexSwitch::SELECT_PANEL1);
    case 2: return State::get(IndexSwitch::SELECT_PANEL2);
    case 3: return State::get(IndexSwitch::SELECT_PANEL3);
    case 4: return State::get(IndexSwitch::SELECT_PANEL4);
    case 5: return State::get(IndexSwitch::SELECT_PANEL5);
    case 6: return State::get(IndexSwitch::SELECT_PANEL6);
    case 7: return State::get(IndexSwitch::SELECT_PANEL7);
    case 8: return State::get(IndexSwitch::SELECT_PANEL8);
    case 9: return State::get(IndexSwitch::SELECT_PANEL9);
    default: return false;
    }
}

RenderParams& RenderParams::operator=(const KeyFrameParams& rhs)
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
vSprite::vSprite(const SpriteBuilder& builder) :
    _srcLine(builder.srcLine), _pTexture(builder.texture), _type(SpriteTypes::VIRTUAL), _current({0, RenderParams::CONSTANT, 0x00000000, BlendMode::NONE, false, 0}) {}

bool vSprite::updateByKeyframes(const Time& rawTime)
{
    // Check if object is valid
	// Note that nullptr texture shall pass
    if (_pTexture != nullptr && !_pTexture->_loaded)
        return false;

    // Check if frames are valid
    size_t frameCount = _keyFrames.size();
    if (frameCount < 1)
        return false;

	Time time;

    // Check if timer is valid
    if (State::get(_triggerTimer) < 0 || State::get(_triggerTimer) == TIMER_NEVER)
        return false;

	// Check if timer is 140
    if (_triggerTimer == IndexTimer::MUSIC_BEAT)
    {
        time = State::get(IndexTimer::MUSIC_BEAT);
    }
    else
    {
        time = rawTime - Time(State::get(_triggerTimer), false);
    }

    // Check if the sprite is not visible yet
    if (!_drawn && _keyFrames[0].time > 0 && time.norm() < _keyFrames[0].time)
        return false;

    // Check if import time is valid
    if (time.norm() < 0)
        return false;

    // Check if loop target is valid
    Time endTime = Time(_keyFrames[frameCount - 1].time, false);
    if (_loopTo < 0 && time > endTime)
        return false;
    if (_loopTo > _keyFrames[frameCount - 1].time)
        time = _keyFrames[frameCount - 1].time;


    // crop time into valid section
    if (time > endTime)
    {
		if (endTime != _loopTo)
			time = Time((time - _loopTo).norm() % (endTime - _loopTo).norm() + _loopTo, false);
        else
            time = _loopTo;
    }

    // Check if specific time
    if (time == _keyFrames[frameCount - 1].time)
    {
        // exactly last frame
        _current = _keyFrames[frameCount - 1].param;
    }
    else if (frameCount == 1 || time.norm() <= _keyFrames[0].time)
    {
        // exactly first frame
        _current = _keyFrames[0].param;
    }
    else
    {
        // get keyFrame section (iterators)
        decltype(_keyFrames.begin()) keyFrameCurr, keyFrameNext;
        for (auto it = _keyFrames.begin(); it != _keyFrames.end(); ++it)
        {
            if (it->time <= time.norm()) 
                keyFrameCurr = it;
            else 
                break;
        }
        keyFrameNext = keyFrameCurr;
        if (keyFrameCurr + 1 != _keyFrames.end()) ++keyFrameNext;

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
            case RenderParams::CONSTANT:
                break;
            case RenderParams::ACCEL:
                //prog = -std::cos(prog * 1.57079632679) + 1.0;
                prog = prog * prog * prog;
                break;
            case RenderParams::DECEL:
                //prog = std::sin(prog * 1.57079632679);
                prog = 1.0 - ((1.0 - prog) * (1.0 - prog) * (1.0 - prog));
                break;
            case RenderParams::DISCONTINOUS:
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

bool vSprite::update(const Time& t)
{
    _draw = updateByKeyframes(t);

    if (_draw) _drawn = true;
    return _draw;
}

void vSprite::setLoopTime(int t)
{
    _loopTo = t;
}

void vSprite::setTrigTimer(IndexTimer t)
{
	_triggerTimer = t;
}

void vSprite::appendKeyFrame(const RenderKeyFrame& f)
{
    _keyFrames.push_back(f);
}

void vSprite::adjustAfterUpdate(int x, int y, int w, int h)
{
    _current.rect.x += x - w;
    _current.rect.y += y - h;
    _current.rect.w += w * 2;
    _current.rect.h += h * 2;
}


////////////////////////////////////////////////////////////////////////////////
// Static

SpriteStatic::SpriteStatic(const SpriteStaticBuilder& builder): vSprite(builder)
{
    _type = SpriteTypes::STATIC;

    if (_pTexture && builder.textureRect == RECT_FULL)
        _texRect = _pTexture->getRect();
    else
        _texRect = builder.textureRect;
}

SpriteStatic::SpriteStatic(pTexture texture, const Rect& texRect, int srcLine) : vSprite(SpriteTypes::STATIC, srcLine)
{
    _pTexture = texture;
    if (_pTexture && texRect == RECT_FULL)
        _texRect = _pTexture->getRect();
    else
        _texRect = texRect;
}

void SpriteStatic::draw() const
{
    if (isHidden()) return;

    if (_draw && _pTexture->_loaded)
        _pTexture->draw(_texRect, _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
}

////////////////////////////////////////////////////////////////////////////////
// Split

SpriteSelection::SpriteSelection(const SpriteSelectionBuilder& builder) : vSprite(builder)
{
    _type = SpriteTypes::SPLIT;
    _srows = builder.textureSheetRows;
    _scols = builder.textureSheetCols;

    if (_srows == 0 || _scols == 0)
    {
        _texRect.resize(0);
        return;
    }
    _segments = _srows * _scols;

    Rect rcGrid;
    if (_pTexture && builder.textureRect == RECT_FULL)
        rcGrid = _pTexture->getRect();
    else
        rcGrid = builder.textureRect;
    rcGrid.w /= _scols;
    rcGrid.h /= _srows;

    if (!builder.textureSheetVerticalIndexing)
    {
        // Horizontal first
        for (unsigned r = 0; r < _srows; ++r)
            for (unsigned c = 0; c < _scols; ++c)
            {
                _texRect.emplace_back(
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
        for (unsigned c = 0; c < _scols; ++c)
            for (unsigned r = 0; r < _srows; ++r)
            {
                _texRect.emplace_back(
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

    if (_draw && _pTexture->_loaded)
        _pTexture->draw(_texRect[_selectionIdx], _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
}

void SpriteSelection::updateSelection(frameIdx frame)
{
    _selectionIdx = frame < _segments ? frame : _segments - 1;
}

bool SpriteSelection::update(const Time& t)
{
    if (_texRect.size() < _segments) return false;

	return vSprite::update(t);
}

////////////////////////////////////////////////////////////////////////////////
// Animated

SpriteAnimated::SpriteAnimated(const SpriteAnimatedBuilder& builder) : SpriteSelection(builder)
{
    _type = SpriteTypes::ANIMATED;
    _animFrames = builder.animationFrameCount;
    _resetAnimTimer = builder.animationTimer;

    if (_segments == 0 || _animFrames == 0)
        return;

    _selections = _srows * _scols / _animFrames;
    _period = builder.animationLengthPerLoop;
}

bool SpriteAnimated::update(const Time& t)
{
	if (SpriteSelection::update(t))
	{
		updateByTimer(t);
		//updateSplitByTimer(t);
		updateAnimationByTimer(t);
		return true;
	}
	return false;
}

void SpriteAnimated::updateByTimer(const Time& time)
{
	if (State::get(_triggerTimer))
		updateByKeyframes(time);
}

void SpriteAnimated::updateAnimation(const Time& time)
{
    if (_segments == 0) return;
    if (_period == -1) return;

    if (double timeEachFrame = double(_period) / _animFrames; timeEachFrame >= 1.0)
    {
        auto animFrameTime = (time.norm() >= 0) ? (time.norm() % _period) : 0;
        _currAnimFrame = static_cast<frameIdx>(std::floor(animFrameTime / timeEachFrame));
    }
	/*
    _drawRect = _texRect[_selectionIdx];
    _drawRect.w = _aRect.w;
    _drawRect.h = _aRect.h;
    if (!_aVert)
    {
        // Horizontal first
        _drawRect.x += _aRect.w * (f % _acols);
        _drawRect.y += _aRect.h * (f / _acols);
    }
    else
    {
        // Vertical first
        _drawRect.x += _aRect.w * (f / _arows);
        _drawRect.y += _aRect.h * (f % _arows);
    }
	*/
}

void SpriteAnimated::updateAnimationByTimer(const Time& time)
{
	if (State::get(_resetAnimTimer))
		updateAnimation(time - Time(State::get(_resetAnimTimer)));
}

// Commented for backup purpose. I don't think I can understand this...
// Animation should not affect Split rcGrid, which is decided by user.
/*
void SpriteAnimated::updateSplitByTimer(rTime time)
{
    // total frame:    _aframes
    // time one cycle: _period
    // time per frame: _period / _aframes
    // current time:   t
    // current frame:  t / (_period / _aframes)
    if (_period / _aframes > 0 && State::get(_triggerTimer))
        updateSplit((frameIdx)((time - State::get(_triggerTimer)) / (_period / _aframes)));
}
*/
void SpriteAnimated::draw() const
{
    if (isHidden()) return;

    if (_draw && _currAnimFrame < _texRect.size() && _pTexture != nullptr && _pTexture->_loaded)
    {
        _pTexture->draw(_texRect[_selectionIdx * _animFrames + _currAnimFrame], _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Text

SpriteText::SpriteText(const SpriteTextBuilder& builder) : vSprite(builder)
{
    _type = SpriteTypes::TEXT;
    _pFont = builder.font;
    _textInd = builder.textInd;
    _align = builder.align;
    _height = builder.ptsize * 3 / 2;
    _color = builder.color;
    _editable = builder.editable;
}

bool SpriteText::update(const Time& t)
{   
    return _draw = updateByKeyframes(t);
}

void SpriteText::updateText()
{
    if (!_draw) return;

    setInputBindingText(State::get(_textInd), _current.color);
    updateTextRect();

}
void SpriteText::updateTextRect()
{
	// fitting
	Rect textRect = _texRect;
	double sizeFactor = (double)_current.rect.h / textRect.h;
	int text_w = static_cast<int>(std::round(textRect.w * sizeFactor));
	switch (_align)
	{
	case TEXT_ALIGN_LEFT:
		break;
	case TEXT_ALIGN_CENTER:
		_current.rect.x -= text_w / 2;
		break;
	case TEXT_ALIGN_RIGHT:
		_current.rect.x -= text_w;
		break;
	}
	_current.rect.w = text_w;

    /*
    if (_haveParent && !_parent.expired())
    {
        auto parent = _parent.lock();
        auto r = parent->getCurrentRenderParams().rcGrid;
        if (r.w == -1 && r.h == -1)
        {
            _current.rcGrid.x = 0;
            _current.rcGrid.y = 0;
        }
        else
        {
            _current.rcGrid.x += parent->getCurrentRenderParams().rcGrid.x;
            _current.rcGrid.y += parent->getCurrentRenderParams().rcGrid.y;
        }
    }
    */

}

void SpriteText::setInputBindingText(std::string&& text, const Color& c)
{
    if (!_pFont || !_pFont->_loaded)
        return;

    if (_pTexture != nullptr && _currText == text && _color == c)
        return;

    if (text.empty() || c.a == 0)
    {
        _pTexture = nullptr;
        _draw = false;
        return;
    }

    _currText = text;
    _color = c;

    _pTexture = _pFont->TextUTF8(_currText.c_str(), c);
    if (_pTexture)
    {
        _texRect = _pTexture->getRect();
        _draw = true;
    }
    else
    {
        _draw = false;
    }
}

bool SpriteText::OnClick(int x, int y)
{
    if (!_editable) return false;
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
    if (!_editing)
    {
        _editing = true;
        _textBeforeEdit = State::get(_textInd);
        _textAfterEdit = (clear ? "" : _currText);
        startTextInput(_current.rect, _textAfterEdit, std::bind(&SpriteText::EditUpdateText, this, _1));
    }
}

void SpriteText::stopEditing(bool modify)
{
    if (_editing)
    {
        stopTextInput();
        _editing = false;
        State::set(_textInd, (modify ? _textAfterEdit : _textBeforeEdit));
        updateText();
    }
}

void SpriteText::EditUpdateText(const std::string& text)
{
    _textAfterEdit = text;
    State::set(_textInd, text + "|");
    updateText();
}

void SpriteText::draw() const
{
    if (isHidden()) return;

    if (_draw && _pTexture && _pTexture->_loaded)
    {
        _pTexture->draw(_texRect, _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
    }
}

void SpriteText::setOutline(int width, const Color& c)
{
    pushMainThreadTask([&] { _pFont->setOutline(width, c); });
}

////////////////////////////////////////////////////////////////////////////////
// Number

SpriteNumber::SpriteNumber(const SpriteNumberBuilder& builder): SpriteAnimated(builder)
{
    _type = SpriteTypes::NUMBER;
    _alignType = builder.align;
    _numInd = builder.numInd;
    _maxDigits = builder.maxDigits;
    _inhibitZero = builder.hideLeadingZeros;

    // invalid num type guard
    //_numType = NumberType(numRows * numCols);
    if (_animFrames != 0)
        _numType = NumberType(_srows * _scols / _animFrames);
    else
        _numType = NumberType(0);

    _digit.resize(_maxDigits);
    _rects.resize(_maxDigits);

    switch (_numType)
    {
    case NUM_TYPE_NORMAL:
    case NUM_TYPE_BLANKZERO:
        break;
        //case NUM_SYMBOL:
    case NUM_TYPE_FULL:
        break;
    default: return;
    }
}

bool SpriteNumber::update(const Time& t)
{
    if (_maxDigits == 0) return false;
    if (_numType == 0) return false;

	if (SpriteAnimated::update(t))
	{
        updateNumberByInd();
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
    unsigned maxDigits = static_cast<unsigned>(_digit.size());
	switch (_numType)
	{
	case NUM_TYPE_NORMAL:    zeroIdx = -1; break;
	case NUM_TYPE_BLANKZERO: zeroIdx = NUM_BZERO; break;
    case NUM_TYPE_FULL:      zeroIdx = positive ? NUM_FULL_BZERO_POS : NUM_FULL_BZERO_NEG; maxDigits--; break;
	}

    // reset by zeroIdx to prevent unexpected glitches
    for (auto& d : _digit) d = zeroIdx;

	if (n == 0)
	{
        _digit[0] = 0;
		_numDigits = 1;
	}
	else
	{
		_numDigits = 0;
		int abs_n = std::abs(n);
		for (unsigned i = 0; abs_n && i < maxDigits; ++i)
		{
			++_numDigits;
			unsigned digit = abs_n % 10;
			abs_n /= 10;
            if (_numType == NUM_TYPE_FULL && !positive) digit += 12;
			_digit[i] = digit;
		}
	}

    // symbol
    switch (_numType)
    {
    /*
    case NUM_TYPE_NORMAL:
        for (unsigned i = _numDigits; i < maxDigits; ++i)
        {
            _digit[i] = 0;
        }
        _numDigits = maxDigits;
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
        switch (_alignType)
        {

        case NUM_ALIGN_RIGHT:
            if (!_inhibitZero || _numDigits == _maxDigits)
                _numDigits = _maxDigits - 1;
            _digit[_numDigits++] = positive ? NUM_FULL_PLUS : NUM_FULL_MINUS;
            break;

        case NUM_ALIGN_LEFT:
        case NUM_ALIGN_CENTER: 
            if (_numDigits == _maxDigits)
                --_numDigits;
            _digit[_numDigits++] = positive ? NUM_FULL_PLUS : NUM_FULL_MINUS;
            break;
        }
        break;
    }
    }
}

void SpriteNumber::updateNumberByInd()
{
    int n;
    switch (_numInd)
    {
    case IndexNumber::RANDOM:
        n = std::rand();
        break;
    case IndexNumber::ZERO:
        n = 0;
        break;
	case (IndexNumber)10220:
		n = int(Time().norm() & 0xFFFFFFFF);
		break;
    default:
#ifdef _DEBUG
		n = (int)_numInd >= 10000 ? (int)State::get((IndexTimer)((int)_numInd - 10000)) : State::get(_numInd);
#else
        n = State::get(_numInd);
#endif
        break;
    }
    updateNumber(n);
}

void SpriteNumber::updateNumberRect()
{
    switch (_alignType)
    {
    case NUM_ALIGN_RIGHT:
    {
        RectF offset{ _current.rect.w * (_maxDigits - 1),0,0,0 };
        for (size_t i = 0; i < _maxDigits; ++i)
        {
            _rects[i] = _current.rect + offset;
            offset.x -= _current.rect.w;
        }
        break;
    }

    case NUM_ALIGN_LEFT:
    {
        RectF offset{ _current.rect.w * (_numDigits - 1),0,0,0 };
        for (size_t i = 0; i < _numDigits; ++i)
        {
            _rects[i] = _current.rect + offset;
            offset.x -= _current.rect.w;
        }
        break;
    }

    case NUM_ALIGN_CENTER:
    {
        RectF offset{ 0,0,0,0 };
        if (_inhibitZero)
            offset.x = int(std::floor(_current.rect.w * 0.5 * (_numDigits - 1)));
        else
            offset.x = int(std::floor(_current.rect.w * (0.5 * (_maxDigits + _numDigits) - 1)));
        for (size_t i = 0; i < _numDigits; ++i)
        {
            _rects[i] = _current.rect + offset;
            offset.x -= _current.rect.w;
        }
        break;
    }
    }
}

void SpriteNumber::appendKeyFrame(const RenderKeyFrame& f)
{
    _keyFrames.push_back(f);
}

void SpriteNumber::draw() const
{
    if (isHidden()) return;

    if (_pTexture->_loaded && _draw)
    {
        //for (size_t i = 0; i < _outRectDigit.size(); ++i)
        //    _pTexture->draw(_drawRectDigit[i], _outRectDigit[i], _current.angle);

        size_t max = 0;
        switch (_alignType)
        {
        case NUM_ALIGN_RIGHT:
            max = _inhibitZero ? _numDigits : _maxDigits;
            break;
        case NUM_ALIGN_LEFT:
        case NUM_ALIGN_CENTER:
            max = _numDigits;
            break;
        default:
            break;
        }
        for (size_t i = 0; i < max; ++i)
        {
            if (_digit[i] != -1)
                _pTexture->draw(_texRect[_currAnimFrame * _selections + _digit[i]], _rects[i],
                    _current.color, _current.blend, _current.filter, _current.angle);
        }
    }
}

void SpriteNumber::adjustAfterUpdate(int x, int y, int w, int h)
{
    vSprite::adjustAfterUpdate(x, y, w, h);
    for (auto& d : _rects)
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
    _dir = builder.sliderDirection;
    _ind = builder.sliderInd;
    _range = builder.sliderRange;
    _callback = builder.callOnChanged;
}

void SpriteSlider::updateVal(double v)
{
	_value = v;
}

void SpriteSlider::updateValByInd()
{
	updateVal(State::get(_ind));
}

void SpriteSlider::updatePos()
{
	int pos_diff = static_cast<int>(std::floor((_range-1) * _value));
	switch (_dir)
	{
	case SliderDirection::DOWN:
        _posMin = _current.rect.y + _current.rect.h / 2;
		_current.rect.y += pos_diff;
		break;
	case SliderDirection::UP:
        _posMin = _current.rect.y + _current.rect.h / 2;
		_current.rect.y -= pos_diff;
		break;
	case SliderDirection::RIGHT:
        _posMin = _current.rect.x + _current.rect.w / 2;
		_current.rect.x += pos_diff;
		break;
	case SliderDirection::LEFT:
        _posMin = _current.rect.x + _current.rect.w / 2;
		_current.rect.x -= pos_diff;
		break;
	}
}

bool SpriteSlider::update(const Time& t)
{
	if (SpriteAnimated::update(t))
	{
		updateValByInd();
		updatePos();
		return true;
	}
	return false;
}

bool SpriteSlider::OnClick(int x, int y)
{
    if (!_draw) return false;
    if (_range == 0) return false;

    bool inRange = false;
    double val = 0.0;
    switch (_dir)
    {
    case SliderDirection::UP:
        if (_current.rect.x <= x && x < _current.rect.x + _current.rect.w &&
            _posMin - _range <= y && y <= _posMin)
        {
            inRange = true;
        }
        break;
    case SliderDirection::DOWN:
        if (_current.rect.x <= x && x < _current.rect.x + _current.rect.w &&
            _posMin <= y && y <= _posMin + _range)
        {
            inRange = true;
        }
        break;
    case SliderDirection::LEFT:
        if (_current.rect.y <= y && y < _current.rect.y + _current.rect.h &&
            _posMin - _range <= x && x <= _posMin)
        {
            inRange = true;
        }
        break;
    case SliderDirection::RIGHT:
        if (_current.rect.y <= y && y < _current.rect.y + _current.rect.h &&
            _posMin <= x && x <= _posMin + _range)
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
    if (_range == 0) return false;

    double val = 0.0;
    switch (_dir)
    {
    case SliderDirection::UP:
        val = double(_posMin - y) / _range;
        break;
    case SliderDirection::DOWN:
        val = double(y - _posMin) / _range;
        break;
    case SliderDirection::LEFT:
        val = double(_posMin - x) / _range;
        break;
    case SliderDirection::RIGHT:
        val = double(x - _posMin) / _range;
        break;
    }
    val = std::clamp(val, 0.0, 1.0);
    if (std::abs(_value - val) > 0.000001)  // this should be enough
    {
        _value = val;
        _callback(_value);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Bargraph

SpriteBargraph::SpriteBargraph(const SpriteBargraphBuilder& builder) : SpriteAnimated(builder)
{
    _type = SpriteTypes::BARGRAPH;
    _dir = builder.barDirection;
    _ind = builder.barInd;
}

void SpriteBargraph::updateVal(Ratio v)
{
	_value = v;
}

void SpriteBargraph::updateValByInd()
{
	updateVal(State::get(_ind));
}

#pragma warning(push)
#pragma warning(disable: 4244)
void SpriteBargraph::updateSize()
{
	int tmp;
	switch (_dir)
	{
	case BargraphDirection::DOWN:
		_current.rect.h *= _value;
		break;
	case BargraphDirection::UP:
		tmp = _current.rect.h;
		_current.rect.h *= _value;
		_current.rect.y += tmp - _current.rect.h;
		break;
	case BargraphDirection::RIGHT:
		_current.rect.w *= _value;
		break;
	case BargraphDirection::LEFT:
		tmp = _current.rect.w;
		_current.rect.w *= _value;
		_current.rect.x += tmp - _current.rect.w;
		break;
	}
}
#pragma warning(pop)

bool SpriteBargraph::update(const Time& t)
{
	if (SpriteAnimated::update(t))
	{
		updateValByInd();
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

    switch (builder.optionType)
    {
    case opType::OPTION:
        _opType = opType::OPTION;
        _ind.op = (IndexOption)builder.optionInd;
        break;

    case opType::SWITCH:
        _opType = opType::SWITCH;
        _ind.sw = (IndexSwitch)builder.optionInd;
        break;

    case opType::FIXED:
        _opType = opType::FIXED;
        _ind.fix = builder.optionInd;
        break;
    }
}

bool SpriteOption::setInd(opType type, unsigned ind)
{
	if (_opType != opType::UNDEF) return false;
	switch (type)
	{
	case opType::UNDEF:
		return false;

	case opType::OPTION:
		_opType = opType::OPTION;
		_ind.op = (IndexOption)ind;
		return true;

	case opType::SWITCH:
		_opType = opType::SWITCH;
		_ind.sw = (IndexSwitch)ind;
		return true;

    case opType::FIXED:
        _opType = opType::FIXED;
        _ind.fix = ind;
        return true;
	}
	return false;
}

void SpriteOption::updateVal(unsigned v)
{
	_value = v;
	updateSelection(v);
}

void SpriteOption::updateValByInd()
{
	switch (_opType)
	{
	case opType::UNDEF:
		break;

	case opType::OPTION:
		updateVal(State::get(_ind.op));
		break;

	case opType::SWITCH:
		updateVal(State::get(_ind.sw));
		break;

    case opType::FIXED:
        updateVal(_ind.fix);
        break;
	}
}

bool SpriteOption::update(const Time& t)
{
	if (SpriteSelection::update(t))
	{
		updateValByInd();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Button

SpriteButton::SpriteButton(const SpriteButtonBuilder& builder) : SpriteOption(builder)
{
    _type = SpriteTypes::BUTTON;
    _panel = builder.clickableOnPanel;
    _plusonly_value = builder.plusonlyDelta;
    _callback = builder.callOnClick;
}

bool SpriteButton::OnClick(int x, int y)
{
    if (!_draw) return false;

    if (_panel < -1 || _panel > 9) return false;
    if (!checkPanel(_panel)) return false;

    if (_plusonly_value == 0)
    {
        int w_opt = _current.rect.w / 2;
        if (y >= _current.rect.y && y < _current.rect.y + _current.rect.h)
        {
            if (x >= _current.rect.x && x < _current.rect.x + w_opt)
            {
                // minus
                _callback(-1);
                return true;
            }
            else if (x >= _current.rect.x + w_opt && x < _current.rect.x + _current.rect.w)
            {
                // plus
                _callback(1);
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
            _callback(_plusonly_value);
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
    _diff_x = builder.dx;
    _diff_y = builder.dy;
    _grids = builder.gridCount;
    _min = builder.gaugeMin;
    _max = builder.gaugeMax;
    _numInd = builder.numInd;

    _lighting.resize(_grids, false);
    setGaugeType(GaugeType::GROOVE);
}

void SpriteGaugeGrid::setFlashType(SpriteGaugeGrid::FlashType t)
{
	_flashType = t;
}

void SpriteGaugeGrid::setGaugeType(SpriteGaugeGrid::GaugeType ty)
{
	_gaugeType = ty;
	switch (_gaugeType)
	{
    case GaugeType::ASSIST_EASY:
        _texIdxLightFail = NORMAL_LIGHT; _texIdxDarkFail = NORMAL_DARK;
        _texIdxLightClear = CLEAR_LIGHT; _texIdxDarkClear = CLEAR_DARK;
        _req = (unsigned short)std::floor(0.6 * _grids); break;

    case GaugeType::GROOVE: 
        _texIdxLightFail = NORMAL_LIGHT; _texIdxDarkFail = NORMAL_DARK; 
        _texIdxLightClear = CLEAR_LIGHT; _texIdxDarkClear = CLEAR_DARK;
        _req = (unsigned short)std::floor(0.8 * _grids); break;

    case GaugeType::SURVIVAL:  
        _texIdxLightFail = CLEAR_LIGHT; _texIdxDarkFail = CLEAR_DARK;
        _texIdxLightClear = CLEAR_LIGHT; _texIdxDarkClear = CLEAR_DARK;
        _req = 1; break;

    case GaugeType::EX_SURVIVAL: 
        if (_segments > EXHARD_LIGHT)
        {
            _texIdxLightFail = EXHARD_LIGHT; _texIdxDarkFail = EXHARD_DARK;
            _texIdxLightClear = EXHARD_LIGHT; _texIdxDarkClear = EXHARD_DARK;
        }
        else
        {
            _texIdxLightFail = CLEAR_LIGHT; _texIdxDarkFail = CLEAR_DARK;
            _texIdxLightClear = CLEAR_LIGHT; _texIdxDarkClear = CLEAR_DARK;
        }
        _req = 1; break;
	default: break;
	}

    Time t(1);

    // set FailRect
    updateSelection(_texIdxLightFail);
    SpriteAnimated::update(t);
    _lightRectFailIdxOffset = unsigned(_selectionIdx * _animFrames);
    updateSelection(_texIdxDarkFail);
    SpriteAnimated::update(t);
    _darkRectFailIdxOffset = unsigned(_selectionIdx * _animFrames);

    // set ClearRect
    updateSelection(_texIdxLightClear);
    SpriteAnimated::update(t);
    _lightRectClearIdxOffset = unsigned(_selectionIdx * _animFrames);
    updateSelection(_texIdxDarkClear);
    SpriteAnimated::update(t);
    _darkRectClearIdxOffset = unsigned(_selectionIdx * _animFrames);
}

void SpriteGaugeGrid::updateVal(unsigned v)
{
	_val = _grids * (v - _min) / (_max - _min);
}

void SpriteGaugeGrid::updateValByInd()
{
	updateVal(State::get(_numInd));
}

bool SpriteGaugeGrid::update(const Time& t)
{
	if (SpriteAnimated::update(t))
	{
        updateValByInd();
		switch (_flashType)
		{
		case FlashType::NONE:
			for (unsigned i = 0; i < _val; ++i)
				_lighting[i] = true;
			for (unsigned i = _val; i < _grids; ++i)
				_lighting[i] = false;
			break;

		case FlashType::CLASSIC:
			for (unsigned i = 0; i < _val; ++i)
				_lighting[i] = true;
			if (_val - 3 >= 0 && _val - 3 < _grids && t.norm() / 17 % 2) _lighting[_val - 3] = false; // -3 grid: 17ms, per 2 units (1 0 1 0)
			if (_val - 2 >= 0 && _val - 2 < _grids && t.norm() / 17 % 4) _lighting[_val - 2] = false; // -2 grid: 17ms, per 4 units (1 0 0 0)
			for (unsigned i = _val; i < _grids; ++i)
				_lighting[i] = false;
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

    if (_draw && _pTexture != nullptr && _pTexture->isLoaded())
    {
		RectF r = _current.rect;
        unsigned grid_val = unsigned(_req - 1);
        for (unsigned i = 0; i < grid_val; ++i)
        {
            _lighting[i] ?
                _pTexture->draw(_texRect[_lightRectFailIdxOffset + _currAnimFrame], r, _current.color, _current.blend, _current.filter, _current.angle) :
                _pTexture->draw(_texRect[_darkRectFailIdxOffset + _currAnimFrame], r, _current.color, _current.blend, _current.filter, _current.angle);
            r.x += _diff_x;
            r.y += _diff_y;
        }
        for (unsigned i = grid_val; i < _grids; ++i)
        {
            _lighting[i] ?
                _pTexture->draw(_texRect[_lightRectClearIdxOffset + _currAnimFrame], r, _current.color, _current.blend, _current.filter, _current.angle) :
                _pTexture->draw(_texRect[_darkRectClearIdxOffset + _currAnimFrame], r, _current.color, _current.blend, _current.filter, _current.angle);
            r.x += _diff_x;
            r.y += _diff_y;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// OnMouse

SpriteOnMouse::SpriteOnMouse(const SpriteOnMouseBuilder& builder) : SpriteAnimated(builder)
{
    _type = SpriteTypes::ONMOUSE;
    panelIdx = builder.visibleOnPanel;
    area = builder.mouseArea;
}

bool SpriteOnMouse::update(const Time& t)
{
    if (!checkPanel(panelIdx)) return false;
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
        int bx = _current.rect.x + area.x;
        int by = _current.rect.y + area.y;
        if (x < bx || x > bx + area.w) _draw = false;
        if (y < by || y > by + area.h) _draw = false;
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

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

////////////////////////////////////////////////////////////////////////////////
// virtual base class functions
vSprite::vSprite(pTexture tex, SpriteTypes type) :
    _pTexture(tex), _type(type), _current({ 0, RenderParams::CONSTANT, 0x00000000, BlendMode::NONE, false, 0 }) {}

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
    if (frameCount == 1 || time.norm() <= _keyFrames[0].time)
    {
        // exactly first frame
        _current = _keyFrames[0].param;
    }
    else if (time == _keyFrames[frameCount - 1].time)       
    {
        // exactly last frame
        _current = _keyFrames[frameCount - 1].param;
    }
    else
    {
        // get keyFrame section (iterators)
        decltype(_keyFrames.begin()) keyFrameCurr, keyFrameNext;
        for (auto it = _keyFrames.begin(); it != _keyFrames.end(); ++it)
        {
            if (it->time <= time.norm()) keyFrameCurr = it;
            else break;
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
            _current.rect.x = (int)grad(keyFrameNext->param.rect.x, keyFrameCurr->param.rect.x, prog);
            _current.rect.y = (int)grad(keyFrameNext->param.rect.y, keyFrameCurr->param.rect.y, prog);
            _current.rect.w = (int)grad(keyFrameNext->param.rect.w, keyFrameCurr->param.rect.w, prog);
            _current.rect.h = (int)grad(keyFrameNext->param.rect.h, keyFrameCurr->param.rect.h, prog);
            //_current.rect  = keyFrameNext->param.rect  * prog + keyFrameCurr->param.rect  * (1.0 - prog);
            _current.color.r = (Uint8)grad(keyFrameNext->param.color.r, keyFrameCurr->param.color.r, prog);
            _current.color.g = (Uint8)grad(keyFrameNext->param.color.g, keyFrameCurr->param.color.g, prog);
            _current.color.b = (Uint8)grad(keyFrameNext->param.color.b, keyFrameCurr->param.color.b, prog);
            _current.color.a = (Uint8)grad(keyFrameNext->param.color.a, keyFrameCurr->param.color.a, prog);
            //_current.color = keyFrameNext->param.color * prog + keyFrameNext->param.color * (1.0 - prog);
            _current.angle = grad(static_cast<int>(std::round(keyFrameNext->param.angle)), static_cast<int>(std::round(keyFrameCurr->param.angle)), prog);
            _current.center = keyFrameCurr->param.center;
            //LOG_DEBUG << "[Skin] Time: " << time << 
            //    " @ " << _current.rect.x << "," << _current.rect.y << " " << _current.rect.w << "x" << _current.rect.h;
            //LOG_DEBUG<<"[Skin] keyFrameCurr: " << keyFrameCurr->param.rect.x << "," << keyFrameCurr->param.rect.y << " " << keyFrameCurr->param.rect.w << "x" << keyFrameCurr->param.rect.h;
            //LOG_DEBUG<<"[Skin] keyFrameNext: " << keyFrameNext->param.rect.x << "," << keyFrameNext->param.rect.y << " " << keyFrameNext->param.rect.w << "x" << keyFrameNext->param.rect.h;
            _current.blend = keyFrameCurr->param.blend;
            _current.filter = keyFrameCurr->param.filter;
        }
    }

    if (hasParent())
    {
        auto parent = _parent.lock();
        const auto& parentRect = parent->getCurrentRenderParams().rect;
        _current.rect.x += parentRect.x;
        _current.rect.y += parentRect.y;
        _current.color.r = (Uint8)(parent->getCurrentRenderParams().color.r / 255.0 * _current.color.r);
        _current.color.g = (Uint8)(parent->getCurrentRenderParams().color.g / 255.0 * _current.color.g);
        _current.color.b = (Uint8)(parent->getCurrentRenderParams().color.b / 255.0 * _current.color.b);
        _current.color.a = (Uint8)(parent->getCurrentRenderParams().color.a / 255.0 * _current.color.a);
        //_current.angle += parent->getCurrentRenderParams().angle;
		//_current.center = parent->getCurrentRenderParams().center;
    }
    
    return true;
}

bool vSprite::update(const Time& t)
{
    _draw = updateByKeyframes(t);

    auto updateChildLambda = [&t](std::weak_ptr<vSprite> p) { if (!p.expired()) p.lock()->update(t); };

    // children under the same level can be shuffled
#ifdef _DEBUG
    std::for_each(std::execution::par_unseq, _children.begin(), _children.end(), updateChildLambda);
#else
    std::for_each(std::execution::par_unseq, _children.begin(), _children.end(), updateChildLambda);
#endif

    if (_draw) _drawn = true;
    return _draw;
}

RenderParams vSprite::getCurrentRenderParams()
{
    return _current;
}

RenderParams& vSprite::getCurrentRenderParamsRef()
{
    return _current;
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

void vSprite::moveAfterUpdate(int x, int y)
{
    _current.rect.x += x;
    _current.rect.y += y;

    auto updateChildLambda = [&](std::weak_ptr<vSprite> p) { if (!p.expired()) p.lock()->moveAfterUpdate(x, y); };

    // children under the same level can be shuffled
#ifdef _DEBUG
    std::for_each(std::execution::par_unseq, _children.begin(), _children.end(), updateChildLambda);
#else
    std::for_each(std::execution::par_unseq, _children.begin(), _children.end(), updateChildLambda);
#endif

}

void vSprite::replaceTexture(pTexture newTexture)
{
    if (_pTexture && _pTexture->isLoaded())
    {
        assert(newTexture->_loaded);
        assert(_pTexture->getRect() == newTexture->getRect());
    }
    _pTexture = newTexture;
}


const std::string& vSprite::getProperty(const std::string& key) const
{
    static std::string emptyVal;
    return properties.find(key) == properties.end() ? emptyVal : properties.at(key);
}
void vSprite::setProperty(const std::string& key, const std::string& value)
{
    properties[key] = value;
}


////////////////////////////////////////////////////////////////////////////////
// Static

SpriteStatic::SpriteStatic(pTexture texture) :
	SpriteStatic(texture, texture ? texture->getRect(): Rect()) {}
SpriteStatic::SpriteStatic(pTexture texture, const Rect& rect):
    vSprite(texture, SpriteTypes::STATIC), _texRect(rect) {}

void SpriteStatic::draw() const
{
    if (!_hide && _draw && _pTexture->_loaded)
        _pTexture->draw(_texRect, _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
}

////////////////////////////////////////////////////////////////////////////////
// Split

SpriteSelection::SpriteSelection(pTexture texture, unsigned rows, unsigned cols, bool v): 
    SpriteSelection(texture, texture ? texture->getRect() : Rect(), rows, cols, v)
{
}

SpriteSelection::SpriteSelection(pTexture texture, const Rect& r, unsigned rows, unsigned cols, bool v):
    vSprite(texture, SpriteTypes::SPLIT)
{
    if (rows == 0 || cols == 0)
    {
        _srows = _scols = 0;
        _texRect.resize(0);
        return;
    }

    _srows = rows;
    _scols = cols;
    _segments = rows * cols;
    auto rect = r;
    rect.w /= cols;
    rect.h /= rows;
    if (!v)
    {
        // Horizontal first
        for (unsigned r = 0; r < rows; ++r)
            for (unsigned c = 0; c < cols; ++c)
            {
                _texRect.emplace_back(
                    rect.x + rect.w * c,
                    rect.y + rect.h * r,
                    rect.w,
                    rect.h
                );
            }
    }
    else
    {
        // Vertical first
        for (unsigned c = 0; c < cols; ++c)
            for (unsigned r = 0; r < rows; ++r)
            {
                _texRect.emplace_back(
                    rect.x + rect.w * c,
                    rect.y + rect.h * r,
                    rect.w,
                    rect.h
                );
            }
    }
}

void SpriteSelection::draw() const
{
    if (!_hide && _draw && _pTexture->_loaded)
        _pTexture->draw(_texRect[_selectionIdx], _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
}

void SpriteSelection::updateSelection(frameIdx frame)
{
    _selectionIdx = frame < _segments ? frame : _segments - 1;
}

bool SpriteSelection::update(const Time& t)
{
	return vSprite::update(t);
}

////////////////////////////////////////////////////////////////////////////////
// Animated

SpriteAnimated::SpriteAnimated(pTexture texture, 
    unsigned animFrames, unsigned frameTime, IndexTimer t, 
    unsigned selRows, unsigned selCols, bool selVert):
    SpriteAnimated(texture, texture ? texture->getRect() : Rect(), animFrames, frameTime, t,
		selRows, selCols, selVert)
{
}

SpriteAnimated::SpriteAnimated(pTexture texture, const Rect& r, 
    unsigned animFrames, unsigned frameTime, IndexTimer t, 
    unsigned selRows, unsigned selCols, bool selVert):
    SpriteSelection(texture, r, selRows, selCols, selVert), _animFrames(animFrames), _resetAnimTimer(t)
{
    _type = SpriteTypes::ANIMATED;

    if (animFrames == 0 || selRows == 0 || selCols == 0) return;

	if (_animFrames != 0) _selections = selRows * selCols / _animFrames;
	//_aframes = animFrames;
    //_aRect.w = _texRect[0].w / animCols;
    //_aRect.h = _texRect[0].h / animRows;
    //_arows = animRows;
    //_acols = animCols;
    //_aframes = animRows * animCols;
    _period = frameTime;
    //_aVert = animVert;
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
// Animation should not affect Split rect, which is decided by user.
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
    if (!_hide && _draw && _currAnimFrame < _texRect.size() && _pTexture != nullptr && _pTexture->_loaded)
    {
        _pTexture->draw(_texRect[_selectionIdx * _animFrames + _currAnimFrame], _current.rect, _current.color, _current.blend, _current.filter, _current.angle, _current.center);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Text

SpriteText::SpriteText(pFont f, IndexText e, TextAlign a, unsigned ptsize, Color c):
   SpriteStatic(nullptr), _pFont(f), _textInd(e), _align(a), _height(ptsize * 3 / 2), _color(c)
{
    _type = SpriteTypes::TEXT;
}

/*
SpriteText::SpriteText(pFont f, Rect rect, IndexText e, TextAlign a, unsigned ptsize, Color c):
   SpriteStatic(nullptr), _pFont(f), _frameRect(rect), _textInd(e), _align(a), _color(c)
{
    _opType = SpriteTypes::TEXT;
    _haveRect = true;
	_texRect = rect;
}
*/

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
        auto r = parent->getCurrentRenderParams().rect;
        if (r.w == -1 && r.h == -1)
        {
            _current.rect.x = 0;
            _current.rect.y = 0;
        }
        else
        {
            _current.rect.x += parent->getCurrentRenderParams().rect.x;
            _current.rect.y += parent->getCurrentRenderParams().rect.y;
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
    if (!_hide && _draw && _pTexture)
    {
        SpriteStatic::draw();
    }
}

void SpriteText::setOutline(int width, const Color& c)
{
    pushMainThreadTask([&] { _pFont->setOutline(width, c); });
}


SpriteNumber::SpriteNumber(pTexture texture, NumberAlign align, unsigned maxDigits,
    unsigned numRows, unsigned numCols, unsigned frameTime, IndexNumber n, IndexTimer t,
    unsigned animFrames, bool numVert):
    SpriteNumber(texture, texture ? texture->getRect() : Rect(), align, maxDigits,
		numRows, numCols, frameTime, n, t,
		animFrames, numVert)
{
}

SpriteNumber::SpriteNumber(pTexture texture, const Rect& rect, NumberAlign align, unsigned maxDigits,
    unsigned numRows, unsigned numCols, unsigned frameTime, IndexNumber n, IndexTimer t,
    unsigned animFrames, bool numVert):
    SpriteAnimated(texture, rect, animFrames, frameTime, t, numRows, numCols, numVert),
    _alignType(align), _numInd(n), _maxDigits(maxDigits)
{
    _type = SpriteTypes::NUMBER;

    // invalid num type guard
    //_numType = NumberType(numRows * numCols);
    if (animFrames != 0)
        _numType = NumberType(numRows * numCols / animFrames);
    else
        _numType = NumberType(0);

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

    _digit.resize(maxDigits);
    _rects.resize(maxDigits);
}

bool SpriteNumber::update(const Time& t)
{
    if (_maxDigits == 0) return false;
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
        Rect offset{ int(_current.rect.w * (_maxDigits - 1)),0,0,0 };
        for (size_t i = 0; i < _maxDigits; ++i)
        {
            _rects[i] = _current.rect + offset;
            offset.x -= _current.rect.w;
        }
        break;
    }

    case NUM_ALIGN_LEFT:
    {
        Rect offset{ int(_current.rect.w * (_numDigits - 1)),0,0,0 };
        for (size_t i = 0; i < _numDigits; ++i)
        {
            _rects[i] = _current.rect + offset;
            offset.x -= _current.rect.w;
        }
        break;
    }

    case NUM_ALIGN_CENTER:
    {
        Rect offset{ 0,0,0,0 };
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
    if (!_hide && _pTexture->_loaded && _draw)
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

void SpriteNumber::moveAfterUpdate(int x, int y)
{
    vSprite::moveAfterUpdate(x, y);
    for (auto& d : _rects)
    {
        d.x += x;
        d.y += y;
    }
}

SpriteSlider::SpriteSlider(pTexture texture, SliderDirection d, int range, std::function<void(double)> cb,
	unsigned animFrames, unsigned frameTime, IndexSlider ind, IndexTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteSlider(texture, texture ? texture->getRect() : Rect(), d, range, cb,
		animFrames, frameTime, ind, timer,
		selRows, selCols, selVerticalIndexing) {}

SpriteSlider::SpriteSlider(pTexture texture, const Rect& rect, SliderDirection d, int range, std::function<void(double)> cb,
	unsigned animFrames, unsigned frameTime, IndexSlider ind, IndexTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteAnimated(texture, rect, animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing), _callback(cb), _ind(ind), _dir(d), _range(range)
{
	_type = SpriteTypes::SLIDER;
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

SpriteBargraph::SpriteBargraph(pTexture texture, BargraphDirection d,
	unsigned animFrames, unsigned frameTime, IndexBargraph ind, IndexTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteBargraph(texture, texture ? texture->getRect() : Rect(), d,
		animFrames, frameTime, ind, timer,
		selRows, selCols, selVerticalIndexing) {}

SpriteBargraph::SpriteBargraph(pTexture texture, const Rect& rect, BargraphDirection d,
	unsigned animFrames, unsigned frameTime, IndexBargraph ind, IndexTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteAnimated(texture, rect, animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing), _dir(d), _ind(ind)
{
	_type = SpriteTypes::BARGRAPH;
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

SpriteOption::SpriteOption(pTexture texture,
	unsigned animFrames, unsigned frameTime, IndexTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteOption(texture, texture ? texture->getRect() : Rect(),
		animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing) {}

SpriteOption::SpriteOption(pTexture texture, const Rect& rect,
	unsigned animFrames, unsigned frameTime, IndexTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteAnimated(texture, rect, animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing)
{
	_type = SpriteTypes::OPTION;
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

SpriteButton::SpriteButton(pTexture texture,
    unsigned animFrames, unsigned frameTime, std::function<void(int)> cb, int panel, int plusonlyValue, IndexTimer timer,
    unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
    SpriteButton(texture, texture ? texture->getRect() : Rect(),
        animFrames, frameTime, cb, panel, plusonlyValue, timer,
        selRows, selCols, selVerticalIndexing) {}

SpriteButton::SpriteButton(pTexture texture, const Rect& rect,
    unsigned animFrames, unsigned frameTime, std::function<void(int)> cb, int panel, int plusonlyValue, IndexTimer timer,
    unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
    SpriteOption(texture, rect, animFrames, frameTime, timer,
        selRows, selCols, selVerticalIndexing), _callback(cb), _panel(panel), _plusonly_value(plusonlyValue)
{
    _type = SpriteTypes::BUTTON;
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

SpriteGaugeGrid::SpriteGaugeGrid(pTexture texture,
	unsigned animFrames, unsigned frameTime, int dx, int dy, unsigned min, unsigned max, unsigned grids,
	IndexTimer timer, IndexNumber num, unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteGaugeGrid(texture, texture ? texture->getRect() : Rect(), animFrames, frameTime, 
        dx, dy, grids, min, max, timer, num, selRows, selCols, selVerticalIndexing) {}

SpriteGaugeGrid::SpriteGaugeGrid(pTexture texture, const Rect& rect,
	unsigned animFrames, unsigned frameTime,  int dx, int dy, unsigned min, unsigned max, unsigned grids,
	IndexTimer timer, IndexNumber num, unsigned selRows, unsigned selCols, bool selVerticalIndexing): 
	SpriteAnimated(texture, rect, animFrames, frameTime, timer, selRows, selCols, selVerticalIndexing),
	_diff_x(dx), _diff_y(dy), _grids(grids), _min(min), _max(max), _numInd(num)
{
    _type = SpriteTypes::GAUGE;
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
    if (!_hide && _draw && _pTexture != nullptr && _pTexture->isLoaded())
    {
		Rect r = _current.rect;
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

SpriteOnMouse::SpriteOnMouse(pTexture texture,
    unsigned animFrames, unsigned frameTime, int panel, const Rect& mouseArea, IndexTimer timer,
    unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
    SpriteOnMouse(texture, texture ? texture->getRect() : Rect(), 
        animFrames, frameTime, panel, mouseArea, timer,
        selRows, selCols, selVerticalIndexing) {}

SpriteOnMouse::SpriteOnMouse(pTexture texture, const Rect& rect, 
    unsigned animFrames, unsigned frameTime, int panel, const Rect& mouseArea, IndexTimer timer,
    unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
    SpriteAnimated(texture, rect, animFrames, frameTime, timer,
        selRows, selCols, selVerticalIndexing), panelIdx(panel), area(mouseArea)
{
    _type = SpriteTypes::ONMOUSE;
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

SpriteCursor::SpriteCursor(pTexture texture, 
    unsigned animFrames, unsigned frameTime, IndexTimer timer,
    unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
    SpriteCursor(texture, texture ? texture->getRect() : Rect(),
        animFrames, frameTime, timer,
        selRows, selCols, selVerticalIndexing) {}

SpriteCursor::SpriteCursor(pTexture texture, const Rect& rect,
    unsigned animFrames, unsigned frameTime, IndexTimer timer,
    unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
    SpriteAnimated(texture, rect, animFrames, frameTime, timer,
        selRows, selCols, selVerticalIndexing)
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

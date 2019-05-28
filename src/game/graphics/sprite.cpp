#include "sprite.h"
#include <plog/Log.h>

static inline double grad(double dst, double src, double t) { return dst * t + src * (1.0 - t); }

////////////////////////////////////////////////////////////////////////////////
// virtual base class functions
vSprite::vSprite(pTexture tex, SpriteTypes type) :
    _pTexture(tex), _type(type), _current({ 0, RenderParams::CONSTANT, 0x00000000, BlendMode::NONE, false, 0 }) {}

bool vSprite::updateByKeyframes(timestamp rawTime)
{
    // Check if object is valid
	// Note that nullptr texture shall pass
    if (_pTexture != nullptr && !_pTexture->_loaded)
        return false;

    // Check if frames is valid
    size_t frameCount = _keyFrames.size();
    if (frameCount < 1)
        return false;

	timestamp time;

	// Check if timer is 140
	if (_timerInd == eTimer::MUSIC_BEAT)
		time = gTimers.get(eTimer::MUSIC_BEAT);
	else
		time = rawTime - gTimers.get(_timerInd);
    
    // Check if import time is valid
	timestamp endTime = timestamp(_keyFrames[frameCount - 1].time);
    if (time.norm() < 0 || _loopTo < 0 && time > endTime)
        return false;

    // Check if loop target is valid
    if (_loopTo < 0 && time > endTime)
        return false;
    if (_loopTo > _keyFrames[frameCount - 1].time)
        time = _keyFrames[frameCount - 1].time;


    // crop time into valid section
    if (time > endTime)
    {
		if (endTime != _loopTo)
			time = timestamp((time - _loopTo).norm() % (endTime - _loopTo).norm() + _loopTo);
        else
            time = _loopTo;
    }

    // Check if specific time
    if (frameCount == 1 || time == _keyFrames[0].time)      
    {
        // exactly first frame
        _current = _keyFrames[0].param;
        return true;
    }
    else if (time == _keyFrames[frameCount - 1].time)       
    {
        // exactly last frame
        _current = _keyFrames[frameCount - 1].param;
        return true;
    }

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
        return true;
    }

    // normalize time
    double t = 1.0 * (time.norm() - keyFrameCurr->time) / keyFrameLength;
    switch (keyFrameCurr->param.accel)
    {
    case RenderParams::CONSTANT:
        break;
    case RenderParams::ACCEL:
        t = std::pow(t, 2.0);
        break;
    case RenderParams::DECEL:
        t = std::pow(t, 0.5);
        break;
    case RenderParams::DISCONTINOUS:
        t = 0.0;
    }

    // calculate parameters
	_current.rect.x = (int)grad((double)keyFrameNext->param.rect.x, (double)keyFrameCurr->param.rect.x, t);
	_current.rect.y = (int)grad((double)keyFrameNext->param.rect.y, (double)keyFrameCurr->param.rect.y, t);
	_current.rect.w = (int)grad((double)keyFrameNext->param.rect.w, (double)keyFrameCurr->param.rect.w, t);
	_current.rect.h = (int)grad((double)keyFrameNext->param.rect.h, (double)keyFrameCurr->param.rect.h, t);
    //_current.rect  = keyFrameNext->param.rect  * t + keyFrameCurr->param.rect  * (1.0 - t);
	_current.color.r = (Uint8)grad((double)keyFrameNext->param.color.r, (double)keyFrameCurr->param.color.r, t);
	_current.color.g = (Uint8)grad((double)keyFrameNext->param.color.g, (double)keyFrameCurr->param.color.g, t);
	_current.color.b = (Uint8)grad((double)keyFrameNext->param.color.b, (double)keyFrameCurr->param.color.b, t);
	_current.color.a = (Uint8)grad((double)keyFrameNext->param.color.a, (double)keyFrameCurr->param.color.a, t);
    //_current.color = keyFrameNext->param.color * t + keyFrameNext->param.color * (1.0 - t);
	_current.angle = grad(keyFrameNext->param.angle, keyFrameNext->param.angle, t);
    //LOG_DEBUG << "[Skin] Time: " << time << 
    //    " @ " << _current.rect.x << "," << _current.rect.y << " " << _current.rect.w << "x" << _current.rect.h;
    //LOG_DEBUG<<"[Skin] keyFrameCurr: " << keyFrameCurr->param.rect.x << "," << keyFrameCurr->param.rect.y << " " << keyFrameCurr->param.rect.w << "x" << keyFrameCurr->param.rect.h;
    //LOG_DEBUG<<"[Skin] keyFrameNext: " << keyFrameNext->param.rect.x << "," << keyFrameNext->param.rect.y << " " << keyFrameNext->param.rect.w << "x" << keyFrameNext->param.rect.h;
	_current.blend = keyFrameCurr->param.blend;
	_current.filter = keyFrameCurr->param.filter;
    
    return true;
}

bool vSprite::update(timestamp t)
{
	return _draw = updateByKeyframes(t);
}

RenderParams vSprite::getCurrentRenderParams()
{
    return _current;
}

void vSprite::setLoopTime(int t)
{
    _loopTo = t;
}

void vSprite::setTimer(eTimer t)
{
	_timerInd = t;
}

void vSprite::appendKeyFrame(RenderKeyFrame f)
{
    _keyFrames.push_back(f);
}

////////////////////////////////////////////////////////////////////////////////
// Static

SpriteStatic::SpriteStatic(pTexture texture) :
	SpriteStatic(texture, texture ? texture->getRect(): Rect()) {}
SpriteStatic::SpriteStatic(pTexture texture, const Rect& rect):
    vSprite(texture, SpriteTypes::STATIC), _texRect(rect) {}

bool SpriteStatic::update(timestamp t)
{
	return vSprite::update(t);
}

void SpriteStatic::draw() const
{
    if (_draw && _pTexture->_loaded)
        _pTexture->draw(_texRect, _current.rect, _current.color, _current.blend, _current.filter, _current.angle);
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
    if (_draw && _pTexture->_loaded)
        _pTexture->draw(_texRect[_segmentIdx], _current.rect, _current.color, _current.blend, _current.filter, _current.angle);
}

void SpriteSelection::updateSelection(frameIdx frame)
{
    _segmentIdx = frame < _segments ? frame : _segments - 1;
}

bool SpriteSelection::update(timestamp t)
{
	return vSprite::update(t);
}

////////////////////////////////////////////////////////////////////////////////
// Animated

SpriteAnimated::SpriteAnimated(pTexture texture, 
    unsigned animFrames, unsigned frameTime, eTimer t, 
    unsigned selRows, unsigned selCols, bool selVert):
    SpriteAnimated(texture, texture ? texture->getRect() : Rect(), animFrames, frameTime, t,
		selRows, selCols, selVert)
{
}

SpriteAnimated::SpriteAnimated(pTexture texture, const Rect& r, 
    unsigned animFrames, unsigned frameTime, eTimer t, 
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

bool SpriteAnimated::update(timestamp t)
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

void SpriteAnimated::updateByTimer(timestamp time)
{
	if (gTimers.get(_timerInd))
		updateByKeyframes(time - timestamp(gTimers.get(_timerInd)));
}

void SpriteAnimated::updateAnimation(timestamp time)
{
    if (_segments == 0) return;
    if (_period == -1) return;

    frameIdx f = 0;
    if (double timeEachFrame = double(_period) / _aframes; timeEachFrame >= 1.0)
    {
        auto animFrameTime = time.norm() % _period;
        f = std::round(animFrameTime / timeEachFrame);
    }
	/*
    _drawRect = _texRect[_segmentIdx];
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
	updateSelection(_segmentIdx + f * _selections);
}

void SpriteAnimated::updateAnimationByTimer(timestamp time)
{
	if (gTimers.get(_resetAnimTimer))
		updateAnimation(time - timestamp(gTimers.get(_resetAnimTimer)));
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
    if (_period / _aframes > 0 && gTimers.get(_timerInd))
        updateSplit((frameIdx)((time - gTimers.get(_timerInd)) / (_period / _aframes)));
}
*/

void SpriteAnimated::draw() const
{
    if (_draw && _pTexture != nullptr && _pTexture->_loaded)
    {
        _pTexture->draw(_drawRect, _current.rect, _current.color, _current.blend, _current.filter, _current.angle);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Text

SpriteText::SpriteText(pFont f, eText e, TextAlign a, unsigned ptsize, Color c):
   SpriteStatic(nullptr), _pFont(f), _textInd(e), _align(a), _color(c)
{
    _type = SpriteTypes::TEXT;
}

/*
SpriteText::SpriteText(pFont f, Rect rect, eText e, TextAlign a, unsigned ptsize, Color c):
   SpriteStatic(nullptr), _pFont(f), _frameRect(rect), _textInd(e), _align(a), _color(c)
{
    _opType = SpriteTypes::TEXT;
    _haveRect = true;
	_texRect = rect;
}
*/

bool SpriteText::update(timestamp t)
{
	if (updateByKeyframes(t))
	{
		setText(gTexts.get(_textInd).c_str(), _current.color);
		updateTextRect();
		return true;
	}
	return false;
}

void SpriteText::updateTextRect()
{
	// fitting
	Rect textRect = _texRect;
	double sizeFactor = (double)_current.rect.h / textRect.h;
	int text_w = textRect.w * sizeFactor;
	double widthFactor = (double)_current.rect.w / text_w;
	if (widthFactor > 1.0)
	{
		switch (_align)
		{
		case TEXT_ALIGN_LEFT:
			break;
		case TEXT_ALIGN_CENTER:
			_current.rect.x += (_current.rect.w - text_w) / 2;
			break;
		case TEXT_ALIGN_RIGHT:
			_current.rect.x += (_current.rect.w - text_w);
			break;
		}
		_current.rect.w = text_w;
	}
}

void SpriteText::setText(const char* text, const Color& c)
{
    if (!_pFont->_loaded) return;
    if (!strcmp(_currText.c_str(), text) && _color == c) return;
    _currText = std::string(text);
    _color = c;
    _pTexture = _pFont->TextUTF8(text, c);
	_texRect = _pTexture->getRect();
}

void SpriteText::draw() const
{
	if (_pTexture)
		SpriteStatic::draw();
}


SpriteNumber::SpriteNumber(pTexture texture, NumberAlign align, unsigned maxDigits,
    unsigned numRows, unsigned numCols, unsigned frameTime, eNumber n, eTimer t,
    unsigned animFrames, bool numVert):
    SpriteNumber(texture, texture ? texture->getRect() : Rect(), align, maxDigits,
		numRows, numCols, frameTime, n, t,
		animFrames, numVert)
{
}

SpriteNumber::SpriteNumber(pTexture texture, const Rect& rect, NumberAlign align, unsigned maxDigits,
    unsigned numRows, unsigned numCols, unsigned frameTime, eNumber n, eTimer t,
    unsigned animFrames, bool numVert):
    vSprite(texture, SpriteTypes::NUMBER), _alignType(align), _numInd(n)
{
    _type = SpriteTypes::NUMBER;

    //_numType = NumberType(numRows * numCols);
	if (animFrames != 0) _numType = NumberType(numRows * numCols / animFrames);
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
	Rect r = rect;
	int subw = r.w / numCols;
    int subh = r.h / numRows;
	for (size_t i = 0; i < maxDigits; ++i)
	{
		_sDigit.emplace_back(texture, r, animFrames, frameTime, t, numRows, numCols, numVert);
	}
	//r.x = r.y = 0;
    // placeholder, to make idx 1 to last digit
	//_sDigit.emplace_back(nullptr, r, animRows, animCols, frameTime, t, animVert, numRows, numCols, numVert);    
}

bool SpriteNumber::update(timestamp t)
{
	if (!_sDigit.empty() && _sDigit[0].update(t))
	{
        updateNumberByInd();
        updateByTimer(t);
        //updateSplitByTimer(t);
        updateAnimationByTimer(t);
        //updateRectsByTimer(t);

		for (size_t i = 0; i < _sDigit.size(); ++i)
			_sDigit[i].update(t);

		return true;
	}
	return false;
}

void SpriteNumber::updateByTimer(timestamp time)
{
	if (gTimers.get(_timerInd))
		for (auto& d : _sDigit)
			d.updateByKeyframes(time - timestamp(gTimers.get(_timerInd)));
}

void SpriteNumber::updateNumber(int n)
{
    bool positive = n >= 0;
	size_t zeroIdx = 0;
    unsigned maxDigits = _digit.size();
	switch (_numType)
	{
	case NUM_TYPE_NORMAL:    zeroIdx = 0; break;
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
                _numDigits = _digit.size();
                _digit[_numDigits] = positive ? NUM_FULL_PLUS : NUM_FULL_MINUS;
                break;

            case NUM_ALIGN_LEFT:
                _digit[_numDigits] = positive ? NUM_FULL_PLUS : NUM_FULL_MINUS;
                _numDigits++;
                break;

            case NUM_ALIGN_CENTER:
                // tbd
                break;
            }
            break;
        }
    }

    // sprites
	size_t blanks = _digit.size() - _numDigits;
    size_t symbolIdx = _numDigits - 1;
    switch (_alignType)
    {
    case NUM_ALIGN_RIGHT:
        for (size_t i = 0; i < blanks; ++i)
            _sDigit[i].updateSelection(zeroIdx);
        for (size_t i = blanks; i < _digit.size(); ++i)
            _sDigit[i].updateSelection(_digit[_digit.size() - 1 - i]);
        break;
    case NUM_ALIGN_LEFT:
        for (size_t i = 0; i < _numDigits; ++i)
            _sDigit[i].updateSelection(_digit[_numDigits - 1 - i]);
        break;
    case NUM_ALIGN_CENTER:
        // WIP
        break;
    }
}

void SpriteNumber::updateNumberByInd()
{
    int n;
    switch (_numInd)
    {
    case eNumber::RANDOM:
        n = std::rand();
        break;
    case eNumber::ZERO:
        n = 0;
        break;
	case (eNumber)10220:
		n = timestamp().norm();
		break;
    default:
#ifdef _DEBUG
		n = (int)_numInd >= 10000 ? (int)gTimers.get((eTimer)((int)_numInd - 10000)) : gNumbers.get(_numInd);
#else
        n = gNumbers.get(_numInd);
#endif
        break;
    }
    updateNumber(n);
}

void SpriteNumber::updateAnimationByTimer(timestamp t)
{
    //for (auto& d : _sDigit)
    //    d.updateAnimation(t);

    /*
    for (size_t i = 0; i < _sDigit.size(); ++i)
    {
        auto& fRect = _current.rect;
        auto& dRect = _outRectDigit[i];
        dRect = _drawRect;
        dRect.x = fRect.x + dRect.w * (_outRectDigit.size() - i - 1);
        dRect.y = fRect.y;
        dRect.w = (int)(dRect.w * ((double)fRect.w / ((long long)_texRect[0].w * _outRectDigit.size())));
        dRect.h = (int)(dRect.h * ((double)fRect.h / (long long)_texRect[0].h));
    }
    */

    for (size_t idx = 0; idx < _numDigits; ++idx)
        _sDigit[idx].updateAnimationByTimer(t);
}

//void SpriteNumber::updateDigitsRenderParams()
//{
//    // calculate sub rect from _current.Rect
//    for (size_t i = 0; i < _drawRectDigit.size(); ++i)
//    {
//        // get sub rect from right to left.
//        // TODO align type support
//
//        const auto& fRect = _current.rect;
//        auto& dRect = _sDigit[i]._current.rect;
//        dRect.x = fRect.x + dRect.w * (_drawRectDigit.size() - i - 1);
//        dRect.y = fRect.y;
//        dRect.w = dRect.w * ((double)fRect.w / (_texRect[0].w * _drawRectDigit.size()));
//        dRect.h = dRect.h * ((double)fRect.h / _texRect[0].h);
//
//        _sDigit[i]._current.angle = _current.angle;
//        _sDigit[i]._current.accel = _current.accel;
//        _sDigit[i]._current.color = _current.color;
//    }
//}
void SpriteNumber::setLoopTime(int t)
{
    for (auto& d : _sDigit)
        d.setLoopTime(t);
}

void SpriteNumber::appendKeyFrame(RenderKeyFrame f)
{
    _keyFrames.push_back(f);
	for (auto& d : _sDigit)
	{
		d.appendKeyFrame(f);
		f.param.rect.x += f.param.rect.w;
	}
}

void SpriteNumber::draw() const
{
    if (_pTexture->_loaded)
    {
        //for (size_t i = 0; i < _outRectDigit.size(); ++i)
        //    _pTexture->draw(_drawRectDigit[i], _outRectDigit[i], _current.angle);

        switch (_alignType)
        {
        case NUM_ALIGN_RIGHT:
            for (const auto& d : _sDigit)
                d.draw();
            break;

        case NUM_ALIGN_LEFT:
            for (size_t idx = 0; idx < _numDigits; ++idx)
                _sDigit[idx].draw();
            break;

        case NUM_ALIGN_CENTER:
            //tbd
            break;
        }
    }
}

SpriteSlider::SpriteSlider(pTexture texture, SliderDirection d, int range,
	unsigned animFrames, unsigned frameTime, eSlider ind, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteSlider(texture, texture ? texture->getRect() : Rect(), d, range,
		animFrames, frameTime, ind, timer,
		selRows, selCols, selVerticalIndexing) {}

SpriteSlider::SpriteSlider(pTexture texture, const Rect& rect, SliderDirection d, int range,
	unsigned animFrames, unsigned frameTime, eSlider ind, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteAnimated(texture, rect, animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing), _ind(ind), _dir(d), _range(range)
{
	_type = SpriteTypes::SLIDER;
}

void SpriteSlider::updateVal(percent v)
{
	_value = v;
}

void SpriteSlider::updateValByInd()
{
	updateVal(gSliders.get(_ind));
}

void SpriteSlider::updatePos()
{
	int pos_delta = (_range-1) * _value;
	switch (_dir)
	{
	case SliderDirection::DOWN:
		_current.rect.y += pos_delta;
		break;
	case SliderDirection::UP:
		_current.rect.y -= pos_delta;
		break;
	case SliderDirection::RIGHT:
		_current.rect.x += pos_delta;
		break;
	case SliderDirection::LEFT:
		_current.rect.x -= pos_delta;
		break;
	}
}

bool SpriteSlider::update(timestamp t)
{
	if (SpriteAnimated::update(t))
	{
		updateValByInd();
		updatePos();
		return true;
	}
	return false;
}

SpriteBargraph::SpriteBargraph(pTexture texture, BargraphDirection d,
	unsigned animFrames, unsigned frameTime, eBargraph ind, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteBargraph(texture, texture ? texture->getRect() : Rect(), d,
		animFrames, frameTime, ind, timer,
		selRows, selCols, selVerticalIndexing) {}

SpriteBargraph::SpriteBargraph(pTexture texture, const Rect& rect, BargraphDirection d,
	unsigned animFrames, unsigned frameTime, eBargraph ind, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteAnimated(texture, rect, animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing), _dir(d), _ind(ind)
{
	_type = SpriteTypes::BARGRAPH;
}

void SpriteBargraph::updateVal(dpercent v)
{
	_value = v;
}

void SpriteBargraph::updateValByInd()
{
	updateVal(gBargraphs.get(_ind));
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

bool SpriteBargraph::update(timestamp t)
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
	unsigned animFrames, unsigned frameTime, eTimer timer,
	unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteOption(texture, texture ? texture->getRect() : Rect(),
		animFrames, frameTime, timer,
		selRows, selCols, selVerticalIndexing) {}

SpriteOption::SpriteOption(pTexture texture, const Rect& rect,
	unsigned animFrames, unsigned frameTime, eTimer timer,
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
		_ind.op = (eOption)ind;
		return true;

	case opType::SWITCH:
		_opType = opType::SWITCH;
		_ind.sw = (eSwitch)ind;
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
		updateVal(gOptions.get(_ind.op));
		break;

	case opType::SWITCH:
		updateVal(gSwitches.get(_ind.sw));
		break;
	}
}

bool SpriteOption::update(timestamp t)
{
	if (SpriteSelection::update(t))
	{
		updateValByInd();
		return true;
	}
	return false;
}


SpriteGaugeGrid::SpriteGaugeGrid(pTexture texture,
	unsigned animRows, unsigned animCols, unsigned frameTime, int dx, int dy, unsigned min, unsigned max,
	eTimer timer, eNumber num,
	bool animVerticalIndexing, unsigned selRows, unsigned selCols, bool selVerticalIndexing) :
	SpriteGaugeGrid(texture, texture ? texture->getRect() : Rect(), animRows, animCols, frameTime, dx, dy, min, max, timer, num, animVerticalIndexing, selRows, selCols, selVerticalIndexing) {}

SpriteGaugeGrid::SpriteGaugeGrid(pTexture texture, const Rect& rect,
	unsigned animRows, unsigned animCols, unsigned frameTime, int dx, int dy, unsigned min, unsigned max, 
	eTimer timer, eNumber num, 
	bool animVerticalIndexing, unsigned selRows, unsigned selCols, bool selVerticalIndexing): 
	SpriteAnimated(texture, rect, animRows, animCols, frameTime, timer, animVerticalIndexing, selRows, selCols, selVerticalIndexing),
	_delta_x(dx), _delta_y(dy), _min(min), _max(max), _numInd(num)
{
	memset(_color, 1, sizeof(_color));	// 0xFFFFFFFF x 50
}

void SpriteGaugeGrid::setFlashType(SpriteGaugeGrid::FlashType t)
{
	_flashType = t;
}

void SpriteGaugeGrid::setGaugeType(SpriteGaugeGrid::GaugeType t)
{
	_gaugeType = t;
	switch (_gaugeType)
	{
	case GaugeType::NORMAL: _texIdxLight = NORMAL_LIGHT; _texIdxDark = NORMAL_DARK; break;
	case GaugeType::HARD:   _texIdxLight = HARD_LIGHT;   _texIdxDark = HARD_DARK;   break;
	case GaugeType::EXHARD: _texIdxLight = EXHARD_LIGHT; _texIdxDark = EXHARD_DARK; break;
	default: break;
	}
}

void SpriteGaugeGrid::updateVal(unsigned v)
{
	_val = 50 * (v - _min) / (_max - _min);
}

void SpriteGaugeGrid::updateValByInd()
{
	updateVal(gNumbers.get(_numInd));
}

bool SpriteGaugeGrid::update(timestamp t)
{
	updateSelection(_texIdxLight);
	if (SpriteAnimated::update(t))
	{
		updateValByInd();
		_lightRect = _drawRect;

		// set darkRect
		updateSelection(_texIdxDark);
		SpriteAnimated::update(t);
		_darkRect = _drawRect;

		switch (_flashType)
		{
		case FlashType::NONE:
			for (unsigned i = 0; i < _val; ++i)
				_color[i] = _current.color;
			for (unsigned i = _val; i < 50; ++i)
				_color[i] = 0x00000000;
			break;

		case FlashType::CLASSIC:
			for (unsigned i = 0; i < _val; ++i)
				_color[i] = _current.color;
			if (_val - 3 >= 0 && _val - 3 < 50 && !!t.norm() / 17 % 2) _color[_val - 3].a = 0; // 16.67ms, per 2f
			if (_val - 2 >= 0 && _val - 2 < 50 && !!t.norm() / 17 % 4) _color[_val - 2].a = 0; // 16.67ms, per 4f
			for (unsigned i = _val; i < 50; ++i)
				_color[i] = 0x00000000;
			break;
			
		default: break;
		}
		return true;
	}
	return false;
}

void SpriteGaugeGrid::draw() const
{
    if (_draw && _pTexture != nullptr && _pTexture->isLoaded())
    {
		Rect r = _current.rect;
		for (unsigned i = 0; i = _val; ++i)
		{
			_pTexture->draw(_drawRect, r, _color[i], _current.blend, _current.filter, _current.angle);
			r.x += _delta_x;
			r.y += _delta_y;
		}
    }
}

SpriteLine::SpriteLine(int width, Color color) : SpriteStatic(nullptr), _width(width), _color(color)
{
	_type = SpriteTypes::LINE;
}

void SpriteLine::appendPoint(const ColorPoint& c) { _points.push_back(c); }

void SpriteLine::draw() const
{
	size_t m = (size_t)std::floor((_points.size() - 1) * _progress);
	for (size_t i = 0; i < m; ++i)
	{
		LineMeta::draw(_points[i].p, _points[i+1].p, _width, _points[i].c);
	}
}

void SpriteLine::updateProgress(timestamp t)
{
	_progress = (double)(gTimers.get(_timerInd) - t.norm() - _timerStartOffset) / _duration;
	std::clamp(_progress, 0.0, 1.0);
}

bool SpriteLine::update(timestamp t)
{
	if (SpriteStatic::update(t))
	{
		updateProgress(t);
		return true;
	}
	return false;
}


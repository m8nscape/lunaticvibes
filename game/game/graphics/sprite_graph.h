#pragma once

#include "sprite.h"


////////////////////////////////////////////////////////////////////////////////
// Line sprite
struct ColorPoint
{
    double xf, yf;
    Color c;
};

enum class LineType
{
    GAUGE_F,
    GAUGE_C,
    SCORE,
    SCORE_TARGET
};

class SpriteLine : public SpriteStatic
{
private:
    int _player;
    LineType _ltype;
    GraphLine _line;
    Color _color;
    int _field_w, _field_h;
    int _start, _end;
    std::vector<ColorPoint> _points;
    std::vector<std::pair<Point, Point>> _rects;
    int _timerStartOffset = 0, _duration = 0;
    double _progress = 1.0;	// 0 ~ 1

public:
    SpriteLine() = delete;
    SpriteLine(int player, LineType ltype, int field_w, int field_h, int start, int end, int width = 1, Color color = 0xffffffff);
    virtual ~SpriteLine() = default;

public:
    void appendPoint(const ColorPoint&);

public:
    void updateProgress(Time t);
    void updateRects();
    virtual bool update(Time t);
    virtual void draw() const;
};


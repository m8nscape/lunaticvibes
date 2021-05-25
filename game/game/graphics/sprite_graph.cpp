#include "sprite_graph.h"
#include "game/scene/scene_context.h"

SpriteLine::SpriteLine(int player, LineType ltype, int field_w, int field_h, int start, int end, int width) :
    SpriteStatic(nullptr), _player(player), _ltype(ltype), _field_w(field_w), _field_h(field_h), _start(start), _end(end), _line(width)
{
    _type = SpriteTypes::LINE;
}

void SpriteLine::appendPoint(const ColorPoint& c) { _points.push_back(c); }

void SpriteLine::draw() const
{
    for (auto& [p1, p2]: _rects)
    {
        _line.draw(p1, p2, _current.color);
    }
}

void SpriteLine::updateProgress(Time t)
{
    //_progress = (double)(gTimers.get(_triggerTimer) - t.norm() - _timerStartOffset) / _duration;
    //_progress = std::clamp(_progress, 0.0, 1.0);
    _progress = 1.0;
}

void SpriteLine::updateRects()
{
    /*
    for (size_t i = 0; i < _rects.size(); ++i)
    {
        const auto& r = _current.rect;
        _rects[i].x = r.x + r.w * _points[i].xf;
        _rects[i].y = r.y - r.h * _points[i].yf;
    }
    */
    _rects.clear();

    const auto& r = _current.rect;
    if (!gPlayContext.ruleset[_player]) return;
    int h = gPlayContext.ruleset[_player]->getClearHealth() * 100;
    switch (_ltype)
    {
    case LineType::GAUGE_F:
    {
        auto& p = gPlayContext.graphGauge[_player];
        size_t s = p.size();
        size_t m = static_cast<size_t>(std::floor(s * _progress)) - 1;
        for (size_t i = 0; i < m; ++i)
        {
            if (p[i] <= h && p[i + 1] <= h)
            {
                _rects.push_back({
                    {
                        r.x + _field_w * (double(i) / (s - 1)),
                        r.y - _field_h * (p[i] / 100.0)
                    },
                    {
                        r.x + _field_w * (double(i + 1) / (s - 1)),
                        r.y - _field_h * (p[i + 1] / 100.0)
                    }
                    });
            }
        }
        break;
    }

    case LineType::GAUGE_C:
    {
        auto& p = gPlayContext.graphGauge[_player];
        size_t s = p.size();
        size_t m = static_cast<size_t>(std::floor(s * _progress)) - 1;
        for (size_t i = 0; i < m; ++i)
        {
            if (p[i] >= h && p[i + 1] >= h)
            {
                _rects.push_back({
                    {
                        r.x + _field_w * (double(i) / (s - 1)),
                        r.y - _field_h * (p[i] / 100.0)
                    },
                    {
                        r.x + _field_w * (double(i + 1) / (s - 1)),
                        r.y - _field_h * (p[i + 1] / 100.0)
                    }
                    });
            }
        }
        break;
    }

    case LineType::SCORE:
    {
        auto& p = gPlayContext.graphScore[_player];
        size_t s = p.size();
        size_t m = static_cast<size_t>(std::floor(s * _progress)) - 1;
        for (size_t i = 0; i < m; ++i)
        {
            _rects.push_back({
                {
                    r.x + _field_w * (double(i) / (s - 1)),
                    r.y - _field_h * (p[i] / 2000.0)
                },
                {
                    r.x + _field_w * (double(i + 1) / (s - 1)),
                    r.y - _field_h * (p[i + 1] / 2000.0)
                }
                });
        }
        break;
    }

    default:
        break;
    }
}

bool SpriteLine::update(Time t)
{
    if (SpriteStatic::update(t))
    {
        updateProgress(t);
        updateRects();
        return true;
    }
    return false;
}

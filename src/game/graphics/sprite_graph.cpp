#include "common/pch.h"
#include "sprite_graph.h"
#include "game/data/data_play.h"
#include "game/data/data_shared.h"
#include "game/ruleset/ruleset.h"

namespace lunaticvibes
{

const size_t RT_GRAPH_THRESHOLD = 50;

SpriteLine::SpriteLine(const SpriteLineBuilder& builder) : SpriteStatic(builder)
{
    _type = SpriteTypes::LINE;
    _player = builder.player;
    _ltype = builder.lineType;
    _field_w = builder.canvasW;
    _field_h = builder.canvasH;
    _start = builder.start;
    _end = builder.end;
    _line = builder.lineWeight;
    textColor = builder.color;
}

void SpriteLine::appendPoint(const ColorPoint& c) { _points.push_back(c); }

void SpriteLine::draw() const
{
    if (isHidden()) return;

    for (auto& [p1, p2] : _rects)
    {
        _line.draw(p1, p2, textColor * _current.color);
    }
}

void SpriteLine::updateProgress(const Time& t)
{
    int duration = _end - _start;
    if (duration > 0)
    {
        long long rt = t.norm() - getTimerValue(motionStartTimer);
        if (rt >= _start)
        {
            _progress = (double)(rt - _start) / duration;
            _progress = std::clamp(_progress, 0.0, 1.0);
        }
        else
        {
            _progress = 0.0;
        }
    }
    else
    {
        _progress = 1.0;
    }
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

    if (PlayData.player[_player].ruleset == nullptr)
        return;

    auto pushRects = [this](int size, const std::vector<int>& points, unsigned maxh, std::function<bool(int val1, int val2)> cond = [](int, int) { return true; })
    {
        std::vector<std::pair<Point, Point>> tmp;
        const auto& r = _current.rect;
        size_t region = static_cast<size_t>(std::floor(size * _progress));
        if (region == 0) return;
        region--;

        if (size > RT_GRAPH_THRESHOLD)
        {
            for (size_t i = 0; i < region; ++i)
            {
                if (cond(points[i], points[i + 1]))
                {
                    tmp.push_back({
                        {
                            r.x + _field_w * (double(i) / (size - 1)),
                            r.y - _field_h * (double(points[i]) / maxh)
                        },
                        {
                            r.x + _field_w * (double(i + 1) / (size - 1)),
                            r.y - _field_h * (double(points[i + 1]) / maxh)
                        }
                        });
                }
            }
        }
        else
        {
            for (size_t i = 0; i < region; ++i)
            {
                if (cond(points[i], points[i + 1]))
                {
                    tmp.push_back({
                        {
                            r.x + _field_w * (double(i) / (size - 1)),
                            r.y - _field_h * (double(points[i]) / maxh)
                        },
                        {
                            r.x + _field_w * (double(i) / (size - 1)),
                            r.y - _field_h * (double(points[i + 1]) / maxh)
                        }
                        });
                    tmp.push_back({
                        {
                            r.x + _field_w * (double(i) / (size - 1)),
                            r.y - _field_h * (double(points[i + 1]) / maxh)
                        },
                        {
                            r.x + _field_w * (double(i + 1) / (size - 1)),
                            r.y - _field_h * (double(points[i + 1]) / maxh)
                        }
                        });
                }
            }
        }

        _rects.clear();
        for (auto& [p1, p2] : tmp)
        {
            if (int(p1.x) != int(p2.x) || int(p1.y) != int(p2.y))
                _rects.push_back({ p1, p2 });
        }
    };

    auto pushRectsF = [this](int size, const std::vector<double>& points, double maxh, std::function<bool(int val1, int val2)> cond = [](int, int) { return true; })
    {
        std::vector<std::pair<Point, Point>> tmp;
        const auto& r = _current.rect;
        size_t region = static_cast<size_t>(std::floor(size * _progress));
        if (region == 0) return;
        region--;

        if (size > RT_GRAPH_THRESHOLD)
        {
            for (size_t i = 0; i < region; ++i)
            {
                if (cond(points[i], points[i + 1]))
                {
                    tmp.push_back({
                        {
                            r.x + _field_w * (double(i) / (size - 1)),
                            r.y - _field_h * points[i] / maxh
                        },
                        {
                            r.x + _field_w * (double(i + 1) / (size - 1)),
                            r.y - _field_h * points[i + 1] / maxh
                        }
                        });
                }
            }
        }
        else
        {
            for (size_t i = 0; i < region; ++i)
            {
                if (cond(points[i], points[i + 1]))
                {
                    tmp.push_back({
                        {
                            r.x + _field_w * (double(i) / (size - 1)),
                            r.y - _field_h * points[i] / maxh
                        },
                        {
                            r.x + _field_w * (double(i) / (size - 1)),
                            r.y - _field_h * points[i + 1] / maxh
                        }
                        });
                    tmp.push_back({
                        {
                            r.x + _field_w * (double(i) / (size - 1)),
                            r.y - _field_h * points[i + 1] / maxh
                        },
                        {
                            r.x + _field_w * (double(i + 1) / (size - 1)),
                            r.y - _field_h * points[i + 1] / maxh
                        }
                        });
                }
            }
        }

        _rects.clear();
        for (auto& [p1, p2] : tmp)
        {
            if (int(p1.x) != int(p2.x) || int(p1.y) != int(p2.y))
                _rects.push_back({ p1, p2 });
        }
    };

    int h = PlayData.player[_player].ruleset->getClearHealth() * 100;
    switch (_ltype)
    {
    case LineType::GAUGE_F:
    {
        auto p = PlayData.player[_player].graphGauge;
        size_t s = p.size();
        pushRects(s, p, 100.0, [h](int val1, int val2) {return (val1 <= h && val2 <= h); });
        break;
    }

    case LineType::GAUGE_C:
    {
        auto p = PlayData.player[_player].graphGauge;
        size_t s = p.size();
        pushRects(s, p, 100.0, [h](int val1, int val2) {return (val1 >= h && val2 >= h); });
        break;
    }

    case LineType::SCORE:
    {
        auto p = PlayData.player[_player].graphRate;
        size_t s = p.size();
        pushRectsF(s, p, 100.0);
        break;
    }

    case LineType::SCORE_MYBEST:
    {
        if (PlayData.player[PLAYER_SLOT_MYBEST].ruleset)
        {
            auto p = PlayData.player[PLAYER_SLOT_MYBEST].graphRate;
            size_t s = p.size();
            pushRectsF(s, p, 100.0);
        }
        break;
    }

    case LineType::SCORE_TARGET:
    {
        auto pt = PlayData.player[PLAYER_SLOT_TARGET].graphRate;
        size_t s = pt.size();
        pushRectsF(s, pt, 100.0);
        break;
    }

    default:
        break;
    }
}

bool SpriteLine::update(const Time& t)
{
    if (SpriteStatic::update(t))
    {
        if (_current.blend == BlendMode::NONE)
            _current.color.a = 255;

        switch (_ltype)
        {
        case LineType::GAUGE_F:
            _current.color.r = 0;
            _current.color.b = 0;
            break;

        case LineType::GAUGE_C:
            switch (PlayData.player[_player].mods.gauge)
            {
            case PlayModifierGaugeType::EXHARD:
            case PlayModifierGaugeType::DEATH:
            case PlayModifierGaugeType::CLASS_HARD:
            case PlayModifierGaugeType::CLASS_DEATH:
                _current.color.g = _current.color.r;
                break;
            default:
                _current.color.g = 0;
                break;
            }
            _current.color.b = 0;
            break;

        case LineType::SCORE:
            _current.color.r = 0;
            _current.color.g = 0;
            break;

        case LineType::SCORE_MYBEST:
            _current.color.r = 0;
            _current.color.b = 0;
            break;

        case LineType::SCORE_TARGET:
            _current.color.g = 0;
            _current.color.b = 0;
            break;
        }

        _line._width = _current.rect.w;

        updateProgress(t);
        updateRects();
        return true;
    }
    return false;
}

}

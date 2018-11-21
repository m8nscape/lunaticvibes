#include "chart.h"

StringContent vChart::getTitle() const
{
    return _title;
}

StringContent vChart::getSubTitle() const
{
    return _title2;
}

StringContent vChart::getArtist() const
{
    return _artist;
}

StringContent vChart::getSubArtist() const
{
    return _artist2;
}

StringContent vChart::getGenre() const
{
    return _genre;
}

BPM vChart::getMinBPM() const
{
    return _minBPM;
}

BPM vChart::getMaxBPM() const
{
    return _maxBPM;
}

BPM vChart::getInitialBPM() const
{
    return !_BPMs.empty() ? std::get<BPM>(_BPMs.front().value) : 130;
}

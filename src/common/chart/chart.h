#pragma once
#include <vector>
#include "types.h"
#include "beat.h"

// Subset of chart formats.
// Currently including BMS

enum class eChartType
{
    UNKNOWN,

    BMS,
    BMSON,
};

class vChart
{
    friend class ScenePlay;

protected:
    eChartType _type = eChartType::UNKNOWN;
public:
    constexpr eChartType type() { return _type; }

protected:

    Path _filePath;
    //_HashType_ _fileHash;

    StringContent _title;
    StringContent _title2;
    StringContent _artist;
    StringContent _artist2;
    StringContent _genre;
    StringContent _version;     // mostly known as difficulty name

    StringPath _BG;
    StringPath _banner;

    StringPath _text1;
    StringPath _text2;
    StringPath _text3;

    std::vector<StringPath> _wavFiles;
    std::vector<StringPath> _bgaFiles;

    std::vector<Note> _BPMs;
    std::vector<Beat> _measureLength;
    std::vector<Note> _notes;
    //std::vector<_Inherit_SpriteStatic_with_playbegin_timer_> _BGAsprites;

    double _minBPM = INFINITY;
    double _maxBPM = 0.0;

public:
    StringContent getTitle() const;
    StringContent getSubTitle() const;
    StringContent getArtist() const;
    StringContent getSubArtist() const;
    StringContent getGenre() const;
    BPM getInitialBPM() const;
    BPM getMinBPM() const;
    BPM getMaxBPM() const;
    BPM getAvgBPM() const;
    BPM getNormBPM() const;
};

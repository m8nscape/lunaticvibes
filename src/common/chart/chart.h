#pragma once
#include <vector>
#include <numeric>
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
    bool _loaded = false;
    constexpr bool isLoaded() { return _loaded; }

public:

    Path _filePath;
    HashMD5 _fileHash;

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

    BPM _minBPM = INFINITY;
    BPM _maxBPM = 0.0;
    BPM _itlBPM = 130.0;

public:
    BPM getAvgBPM() const;
    BPM getNormBPM() const;
    Path getDirectory() const;
};

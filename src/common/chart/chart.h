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

protected:

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

public:
    inline StringContent getTitle() const { return _title; }
    inline StringContent getSubTitle() const { return _title2; }
    inline StringContent getArtist() const { return _artist; }
    inline StringContent getSubArtist() const { return _artist2; }
    inline StringContent getGenre() const { return _genre; }
    constexpr BPM getInitialBPM() const { return !_BPMs.empty() ? std::get<BPM>(_BPMs.front().value) : 130; }
    constexpr BPM getMinBPM() const { return _minBPM; }
    constexpr BPM getMaxBPM() const { return _maxBPM; }
    BPM getAvgBPM() const;
    BPM getNormBPM() const;
};

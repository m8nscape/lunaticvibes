#pragma once
#include <regex>
#include <vector>
#include <numeric>
#include <memory>
#include "types.h"
#include "beat.h"

// Subset of chart formats.
// Currently including BMS

enum class eChartFormat
{
    UNKNOWN,

    BMS,
    BMSON,
};
eChartFormat matchChartType(const Path& p);

class vChartFormat: public std::enable_shared_from_this<vChartFormat>
{
protected:
    eChartFormat _type = eChartFormat::UNKNOWN;
public:
    constexpr eChartFormat type() { return _type; }

public:
    vChartFormat() = default;
    virtual ~vChartFormat() = default;
    static std::shared_ptr<vChartFormat> getFromFile(const Path& path);

protected:
    Path _path;
    bool _loaded = false;
public:
    constexpr bool isLoaded() { return _loaded; }

// following fields are generic info, which are stored in db
public:
    Path filePath;
    Path absolutePath;
    HashMD5 fileHash;
    HashMD5 folderHash;

    StringContent title;
    StringContent title2;
    StringContent artist;
    StringContent artist2;
    StringContent genre;
    StringContent version;     // mostly known as difficulty name
    double levelEstimated = 0.0;

    int totalLength = 0;// in seconds
    int totalNotes = 0;

    StringContent stagefile;
    StringContent banner;

    StringContent text1;
    StringContent text2;
    StringContent text3;

    BPM minBPM = INFINITY;
    BPM maxBPM = 0.0;
    BPM startBPM = 130.0;

// following fields are filled during loading
public:
    std::vector<StringContent> wavFiles;
    std::vector<StringContent> bgaFiles;

    std::vector<Note> BPMNotes;
    std::vector<Beat> barLength;
    std::vector<Note> notes;
    //std::vector<_Inherit_SpriteStatic_with_playbegin_timer_> _BGAsprites;

public:
    BPM getAvgBPM() const;
    BPM getNormBPM() const;
    Path getDirectory() const;

    virtual int getExtendedProperty(const std::string& key, void* ret) { return -1; }
};

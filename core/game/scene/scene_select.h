#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <memory>
#include "scene.h"

enum class eSelectState
{
    PREPARE,
    SELECT,
    SEARCH,
    PANEL1,     // play option
    PANEL2,     // system option
    PANEL3,
    PANEL4,
    PANEL5,
    PANEL6,
    PANEL7,
    PANEL8,
    PANEL9,
    FADEOUT,
};

class vChart;
class SceneSelect : public vScene
{
protected:
    enum class EntryType
    {
        FOLDER,
        CUSTOM_FOLDER,
        SONG,
        COURSE,
        SP_NEW_COURSE,
    };
    struct NextChart
    {
        bool have_one;
        bool have_more;
        std::shared_ptr<vChart> next;
    };
    struct SongEntry
    {
        EntryType type;
        std::shared_ptr<vChart> chart;
        unsigned level_type = 0;
        unsigned lamp = 0;
        unsigned rank = 0;

        // extend info
        unsigned rival = 3; // win / lose / draw / noplay
        unsigned rival_lamp_self = 0;
        unsigned rival_lamp_rival = 0;
        std::array<NextChart, size_t(eLevel::LEVEL_COUNT)> next_chart;
    };
    struct SongList
    {
        std::string name;       // folder path, search query+result, etc.
        std::vector<SongEntry> entries;
    };

private:
    std::mutex _mutex;
    eSelectState _state;
    InputMask _inputAvailable;

private:
    std::list<SongList> _songListHistory;
    SongList _filteredSongList;
    size_t _currentSongIdx = 0;

public:
    SceneSelect();
    virtual ~SceneSelect() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    void updatePrepare();
    void updateSelect();
    void updateSearch();
    void updatePanel(unsigned idx);
    void updateFadeout();

protected:
    // Inner-state updates
    void playBGMSamples();
    void changeKeySampleMapping(timestamp t);

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, timestamp);
    void inputGameHold(InputMask&, timestamp);
    void inputGameRelease(InputMask&, timestamp);

private:
    void _navigateEnter();
    void _navigateBack();
    void _decide();
};

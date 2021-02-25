#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <memory>
#include "scene.h"
#include "scene_context.h"

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

class vChartFormat;
class SceneSelect : public vScene
{
private:
    std::mutex _mutex;
    eSelectState _state;
    InputMask _inputAvailable;

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
    void procCommonNotes();
    void playBGMSamples();
    void changeKeySampleMapping(Time t);

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, Time);
    void inputGameHold(InputMask&, Time);
    void inputGameRelease(InputMask&, Time);

private:
    void loadSongList();
    void _navigateUpBy1();
    void _navigateDownBy1();
    void _navigateEnter();
    void _navigateBack();
    void _decide();
};

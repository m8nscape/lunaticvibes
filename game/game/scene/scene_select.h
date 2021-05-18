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
    eSelectState _state;
    InputMask _inputAvailable;

public:
    SceneSelect();
    virtual ~SceneSelect();

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

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, Time);
    void inputGameHold(InputMask&, Time);
    void inputGameRelease(InputMask&, Time);

private:
    void loadSongList();
    void _navigateUpBy1(Time t);
    void _navigateDownBy1(Time t);
    void _navigateEnter(Time t);
    void _navigateBack(Time t);
    void _decide();
};
